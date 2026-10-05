#!/usr/bin/env python3
"""Persistent original render -> output AntiAlias -> engine-gain/mix execution."""
import hashlib
import itertools
import json
import math
import struct

from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_antialias import coefficient, cutoff_for_speed, reference
from probe_engine_gain import BASE
from probe_render_iteration import chosen, head_gain, split_advance
from probe_splice_render import (CHANNEL, MANAGER, WORK, RECORD_MANAGER, TAPE,
                                 OUTPUT, GAINS, TABLE)
from probe_tap_allocation import call

AA, GAIN = CHANNEL + 6568, CHANNEL + 6528
MIX, MIX_HEADER = 0x1E0000, 0x1F0000


def active(cpu, manager):
    address = call(cpu, 0x4D984, manager)
    return [cpu.getu(address + i * 4) for i in range(20) if cpu.getu(address + i * 4)]


def rounded_cubic(a, b, c, d, t):
    """Recovered cubic equation, preserving renderer float32/FMA evaluation."""
    delta = f32(c - b)
    curvature = libm.fmaf(-3., delta, f32(d - a))
    bend = libm.fmaf(2., a, d)
    bend = libm.fmaf(-3., b, bend)
    bend = libm.fmaf(t, curvature, bend)
    weight = f32(-f32(0.16666670143604279) * f32(1. - t))
    slope = libm.fmaf(weight, bend, delta)
    return libm.fmaf(t, slope, b)


def raw_model(cpu, heads, records, c, tape):
    result = [0.] * c['frames']
    for head in heads:
        gains = head_gain(cpu, head, records, c)
        position, fraction = cpu.gets(head + 12), cpu.getf(head + 16)
        position += 2 if chosen(cpu, head, c['speed']) >= 0 else -3
        current = f32(f32(cpu.gets(head + 4)) + cpu.getf(head + 8))
        previous = f32(f32(cpu.gets(head + 12)) + cpu.getf(head + 16))
        step = f32(f32(current - previous) / c['frames'])
        for i in range(c['frames']):
            index = max(1, min(29501997, position)) - 1
            assert 0 <= index <= len(tape) - 4
            result[i] = libm.fmaf(gains[i], rounded_cubic(*tape[index:index + 4], fraction), result[i])
            position, fraction = split_advance(position, fraction, step)
    return result


def main():
    tape = [f32(.3 * math.sin(i * .137) + .1 * math.cos(i * .071)) for i in range(8192)]
    rows, coverage, comparisons = [], set(), 0
    errors = dict(raw=0., filtered=0., mix=0., filter_state=0.)
    for record_profile, factor_profile in itertools.product(('none', 'two'), ('stable', 'changing')):
        cpu = ARMBytes()
        call(cpu, 0x4CA68, MANAGER)
        call(cpu, 0x4CA68, RECORD_MANAGER)
        cpu.reg(4, CHANNEL)
        cpu.call(0x3CF0C, stop_before=0x3CF34)
        call(cpu, 0x4B09C, AA)
        cpu.putu(CHANNEL + 171972, TAPE)
        cpu.write(TAPE, struct.pack('<8192f', *tape))
        cpu.write(0x93F98, struct.pack('<256f', *TABLE))
        if record_profile == 'two':
            for i, speed in enumerate((-1., .5)):
                head = call(cpu, 0x4EEA8, RECORD_MANAGER, 1000 + i * 100, 32, True)
                cpu.write(head + 20, b'\x01')
                cpu.putf(head + 24, speed)
                cpu.putf(head + 8, .375)
        state = [[0., 0.], [0., 0.]]
        gain, increment, remaining, cached = 0., 0., 0, 0
        previous_speed, previous_factor = .125, .75
        engines, history = 0, []
        for block in range(24):
            frames = (1, 7, 32, 128)[block % 4]
            speed = (.125, .5, 1., -.125, -.5, .25)[block % 6]
            factor = .75 if factor_profile == 'stable' else (.5, 1., .75)[block % 3]
            c = dict(frames=frames, speed=speed, previous_speed=previous_speed,
                     factor=factor, previous_factor=previous_factor)
            if block in (0, 3, 6, 9):
                head = call(cpu, 0x4EEA8, MANAGER, 1000 + engines * 100, 32, speed >= 0)
                cpu.putf(head + 8, .375)
                if engines % 2:
                    cpu.write(head + 20, b'\x01')
                    cpu.putf(head + 24, -.5)
                engines += 1
            if block == 15:
                for engine in range(4):
                    head = call(cpu, 0x4E460, MANAGER, 1050 + engine * 100, engine)
                    assert head
                    cpu.putf(head + 8, .375)
            heads, records = active(cpu, MANAGER), active(cpu, RECORD_MANAGER)
            for head in heads + records:
                cpu.reg(0, head)
                cpu.fp(0, speed)
                cpu.fp(1, factor)
                cpu.fp(2, frames)
                cpu.call(0x4BDE0)
            expected_raw = raw_model(cpu, heads, records, c, tape)
            cutoff = cutoff_for_speed(speed)
            cpu.reg(0, AA)
            cpu.fp(0, cutoff)
            cpu.call(0x4B1C0)
            coeff = coefficient(cutoff)
            assert cpu.getf(AA) == coeff and cpu.getf(AA + 12) == coeff
            expected_filtered = reference(expected_raw, coeff, state)
            target = 1.
            for _ in range(engines - 1):
                target = f32(target * BASE)
            if engines != cached:
                increment = f32(f32(target - gain) * .125)
                remaining, cached = 8, engines
            if remaining:
                gain, remaining = f32(gain + increment), remaining - 1
            prior = [f32(.1 * math.cos(i * .071 - block)) for i in range(frames)]
            expected_mix = [libm.fmaf(a, gain, b) for a, b in zip(expected_filtered, prior)]
            cpu.vector(WORK + 12, OUTPUT, [0.] * frames)
            cpu.vector(WORK + 24, GAINS, [1.] * frames)
            cpu.vector(MIX_HEADER, MIX, prior)
            for offset, value in ((0x20, MANAGER), (0x30, RECORD_MANAGER),
                                  (0x54, AA), (0x5C, WORK + 12), (0x2C, frames)):
                cpu.putu(STACK + offset, value)
            cpu.reg(4, CHANNEL)
            cpu.reg(5, WORK)
            cpu.reg(10, MIX_HEADER)
            for register, value in ((16, frames), (17, factor), (18, previous_factor),
                                     (19, speed), (20, previous_speed),
                                     (21, abs(speed)), (22, abs(previous_speed))):
                cpu.fp(register, value)
            observed = {}
            cpu.observers[0x483A4] = lambda machine: observed.update(raw=machine.floats(OUTPUT, frames))
            cpu.observers[0x483B0] = lambda machine: observed.update(filtered=machine.floats(OUTPUT, frames))
            cpu.call(0x47FE4, stop_before=0x48454)
            observed['mix'] = cpu.floats(MIX, frames)
            observed['filter_state'] = [cpu.getf(AA + o) for o in (4, 8, 16, 20)]
            for field, expected in (('raw', expected_raw), ('filtered', expected_filtered),
                                    ('mix', expected_mix), ('filter_state', state[0] + state[1])):
                for want, actual in zip(expected, observed[field]):
                    error = abs(want - actual)
                    assert math.isfinite(actual) and error <= 8e-7, (record_profile, factor_profile, block, field, want, actual, error)
                    errors[field] = max(errors[field], error)
                    comparisons += 1
            assert [cpu.getf(GAIN), cpu.getf(GAIN + 4), cpu.gets(GAIN + 8), cpu.gets(GAIN + 12)] == [gain, increment, remaining, cached]
            comparisons += 4
            history.append(dict(block=block, config=c, engines=engines, active_heads=len(heads),
                                cutoff=cutoff, gain=gain, observed=observed,
                                model_mix=expected_mix))
            previous_speed, previous_factor = speed, factor
        rows.append(dict(record_profile=record_profile, factor_profile=factor_profile, blocks=history))
        coverage.update(cpu.coverage)
    result = dict(status='PASS', sequences=len(rows), blocks=sum(len(r['blocks']) for r in rows),
                  comparisons=comparisons, max_abs_errors=errors,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  fixtures=rows, distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Persistent playback subset, not a complete callback or hardware comparison.',
                               'Movement and allocation are explicit setup; no boundary or natural expiry/update.',
                               'Record heads move and attenuate playback but do not write tape.',
                               'Cutoff argument is selected by recovered speed rule; dispatch/producer not executed.',
                               'Fade model reads original initialized states; table uses host-cosine reconstruction.',
                               'Stops before TapeFilter/compander/allpass/plate/preview/clipper output chain.',
                               'Nonzero destination contribution is supplied, not recovered monitoring.'])
    (ROOT / 'probes/playback_pipeline_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'sequences', 'blocks', 'comparisons',
                                            'max_abs_errors', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
