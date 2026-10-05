#!/usr/bin/env python3
"""Original-byte setLoopingParameters probe with independently expressed rules."""
import csv
import hashlib
import itertools
import json
import math
import random

import unicorn
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32

CHANNEL, LINK, PRESET = 0x100000, 0x140000, 0x150000
WORK = CHANNEL + 172032
LENGTH_ADC, START_ADC = CHANNEL + 173320, CHANNEL + 173336
FIELDS = {'start': 0x38, 'start_plus_old_fade': 0x3C, 'end': 0x40,
          'end_plus_new_fade': 0x44, 'length': 0x80, 'fade': 0x88,
          'cursor': 0x78, 'cursor_enabled': 0x7C}


def control_samples(raw, span):
    return math.trunc(f32(f32(f32(raw) / f32(4095.)) * f32(span)))


def model(config):
    c = config
    start_changed = abs(c['start_adc'] - c['start_previous']) > c['threshold']
    length_changed = abs(c['length_adc'] - c['length_previous']) > c['threshold']
    grid = (c['period'] - 1) // c['divisions']
    assert grid > 0, 'This probe tests valid nonzero grids; invalid preset behavior is separate.'
    start, length = c['old_start'], c['old_length']
    update_start = start_changed or c['force']
    update_length = length_changed or c['force']
    if update_start:
        start = max(1, min(c['span'] - 2, control_samples(c['start_control'], c['span'])))
        if c['start_quantized'] and not c['modifier']:
            start = (start // grid) * grid
    if update_length:
        length = control_samples(c['length_control'], c['period'] - 1)
        # The modifier suppresses length quantization only in the branch reached
        # through an updated, quantized start. The other length branch explicitly
        # checks modifier and Channel+0x90.
        quantize = c['length_quantized'] and not c['modifier']
        if update_start and c['start_quantized'] and c['modifier']:
            quantize = False
        if quantize:
            length = (length // grid + 1) * grid
        length = max(max(1, c['minimum']), min(c['period'] - 1, length))
    fade_span = min(math.trunc(c['old_length'] / 2), c['maximum_fade'])
    fade = max(128, math.trunc(f32(f32(fade_span) *
                                 f32(f32(c['fade_control']) / f32(4095.)))))
    end = start + c['old_length']
    if end >= c['end_marker']:
        end -= math.trunc(end / c['period']) * c['period']
    cursor, enabled = c['cursor'], c['cursor_enabled']
    if enabled:
        a, b = c['link_start'], c['link_end']
        inside = (cursor > a and cursor <= b) if a < b else (cursor > a or cursor <= b)
        if not inside:
            cursor = c['link_reverse_end'] if c['speed'] < 0 else b
    return dict(start=start, length=length, fade=fade,
                start_plus_old_fade=start + c['old_fade'], end=end,
                end_plus_new_fade=end + fade, cursor=cursor, cursor_enabled=enabled,
                start_previous=c['start_adc'], length_previous=c['length_adc'])


def read_state(cpu):
    observed = {name: (cpu.read(CHANNEL + offset, 1)[0] if name == 'cursor_enabled'
                       else cpu.gets(CHANNEL + offset)) for name, offset in FIELDS.items()}
    observed.update(start_previous=cpu.gets(START_ADC + 8), length_previous=cpu.gets(LENGTH_ADC + 8))
    return observed


def execute(c):
    cpu = ARMBytes()
    cpu.putu(CHANNEL + 0xE8, LINK)
    cpu.putu(CHANNEL + 0xD4, PRESET)
    for offset, value in ((0x4C, c['period']), (0x50, c['end_marker']),
                          (0x38, c['old_start']), (0x80, c['old_length']),
                          (0x84, c['maximum_fade']), (0x88, c['old_fade']),
                          (0x78, c['cursor'])):
        cpu.puts(CHANNEL + offset, value)
    cpu.write(CHANNEL + 0x90, bytes([c['length_quantized']]))
    cpu.write(CHANNEL + 0x7C, bytes([c['cursor_enabled']]))
    for offset, value in ((0x28, c['span']), (0x80, c['divisions']),
                          (0x14, c['link_start']), (0x18, c['link_reverse_end']),
                          (0x1C, c['link_end'])):
        cpu.puts(LINK + offset, value)
    cpu.putf(LINK, c['speed'])
    cpu.write(LINK + 0x6C, bytes([c['start_quantized']]))
    cpu.puts(PRESET + 0x30, c['minimum'])
    cpu.puts(WORK + 0x4A4, c['start_control'])
    cpu.puts(WORK + 0x4A0, c['length_control'])
    cpu.puts(WORK + 0x4B4, c['fade_control'])
    for address, current, previous in ((START_ADC, c['start_adc'], c['start_previous']),
                                       (LENGTH_ADC, c['length_adc'], c['length_previous'])):
        cpu.puts(address + 4, current)
        cpu.puts(address + 8, previous)
        cpu.puts(address + 12, c['threshold'])
    cpu.reg(0, CHANNEL)
    cpu.reg(1, c['modifier'])
    cpu.reg(2, c['force'])
    cpu.call(0x376AC)
    return read_state(cpu), cpu


def main():
    rng, rows, coverage, examples = random.Random(210), [], set(), []
    base = dict(period=49170, span=49170, end_marker=51628, divisions=8,
                old_start=1, old_length=20000, old_fade=512, maximum_fade=12292,
                minimum=1280, fade_control=2048, start_control=2048, length_control=1024,
                start_adc=100, start_previous=90, length_adc=100, length_previous=90,
                threshold=4, modifier=False, force=False, start_quantized=False,
                length_quantized=False, cursor=200, cursor_enabled=False,
                link_start=100, link_end=500, link_reverse_end=400, speed=1.)
    configs = []
    for modifier, force, sq, lq, sc, lc, control in itertools.product(
            (False, True), (False, True), (False, True), (False, True),
            (False, True), (False, True), (0, 1, 2047, 2048, 4094, 4095)):
        c = dict(base, modifier=modifier, force=force, start_quantized=sq, length_quantized=lq,
                 start_previous=90 if sc else 100, length_previous=90 if lc else 100,
                 start_control=control, length_control=control)
        configs.append(c)
    for _ in range(1500):
        period = rng.choice((128, 1024, 12293, 49170, 16777219, 29500000))
        c = dict(base, period=period, span=period + rng.choice((0, 12292)),
                 end_marker=period + rng.choice((0, 2458)), divisions=rng.choice((1, 2, 3, 8, 16)),
                 minimum=rng.choice((-1, 1, 128, 1280)), old_length=rng.randint(1, period-1),
                 old_start=rng.randint(0, period-1), old_fade=rng.randint(128, 12292),
                 maximum_fade=rng.choice((127, 512, 12292)),
                 fade_control=rng.choice((0, 1, 1024, 2048, 4095)),
                 start_control=rng.randint(0, 4095), length_control=rng.randint(0, 4095),
                 start_previous=rng.randint(94, 106), length_previous=rng.randint(94, 106),
                 threshold=rng.choice((0, 4, 6)),
                 modifier=bool(rng.getrandbits(1)), force=bool(rng.getrandbits(1)),
                 start_quantized=bool(rng.getrandbits(1)), length_quantized=bool(rng.getrandbits(1)),
                 cursor_enabled=bool(rng.getrandbits(1)), cursor=rng.choice((99, 100, 101, 200, 499, 500, 501)),
                 link_start=rng.choice((100, 500)), link_end=rng.choice((100, 500)),
                 speed=rng.choice((-1., 0., 1.)))
        configs.append(c)
    for number, config in enumerate(configs):
        expected = model(config)
        observed, cpu = execute(config)
        assert observed == expected, (number, config, expected, observed)
        coverage.update(cpu.coverage)
        rows.extend((number, field, value, observed[field]) for field, value in expected.items())
        if number in (0, 192, 384, 768) or len(examples) < 12 and number >= 384 and config['force']:
            examples.append(dict(case=number, config=config, output=observed))
    # Consecutive calls on the SAME emulated object expose endpoint settling,
    # while hasChanged consumes the ADC value on the first call.
    config = dict(base, force=True)
    settling = []
    observed, cpu = execute(config)
    for step in range(4):
        if step:
            config = dict(config, force=False, old_start=observed['start'],
                          old_length=observed['length'], old_fade=observed['fade'],
                          start_previous=observed['start_previous'], length_previous=observed['length_previous'])
            cpu.reg(0, CHANNEL)
            cpu.reg(1, config['modifier'])
            cpu.reg(2, config['force'])
            cpu.call(0x376AC)
            observed = read_state(cpu)
        expected = model(config)
        assert observed == expected, ('settling', step, expected, observed)
        rows.extend((len(configs) + step, field, value, observed[field]) for field, value in expected.items())
        settling.append(dict(call=step, output=observed))
    assert settling[2]['output'] == settling[3]['output'], settling
    coverage.update(cpu.coverage)
    with (ROOT / 'probes/loop_region_probe_vectors.csv').open('w', newline='') as handle:
        writer = csv.writer(handle)
        writer.writerow(['case', 'field', 'model', 'original_byte_probe'])
        writer.writerows(rows)
    result = dict(status='PASS', seed=210, unicorn_version=unicorn.__version__,
                  method='Original setLoopingParameters bytes with explicit Channel/ADC/LinkData fixtures',
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  cases=len(configs), settling_calls=settling, comparisons=len(rows), max_abs_error=0,
                  examples=examples, distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(address) for address in sorted(coverage)],
                  limitations=[
                      'No full callback/event/link execution; fixture fields can be independently varied.',
                      'Only positive periods/spans and nonzero integer grids; invalid preset cases remain separate.',
                      'Modifier and force are routine arguments, not yet a complete hardware-button mapping.',
                      'Cached old-length/fade behavior is established per call, not a host-time scheduling claim.',
                  ])
    (ROOT / 'probes/loop_region_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in (
        'status', 'cases', 'comparisons', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
