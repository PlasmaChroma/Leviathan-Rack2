#!/usr/bin/env python3
"""Original render traversal, speed ramps and record-head proximity attenuation."""
import hashlib
import itertools
import json
import math
import struct

from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_splice_render import (CHANNEL, MANAGER, WORK, RECORD_MANAGER, TAPE,
                                 OUTPUT, GAINS, TABLE, fade_curve)
from probe_tap_allocation import call
from probe_transport import cubic


def split_advance(integer, fraction, step):
    fraction = f32(fraction + step)
    whole = math.floor(fraction)
    return integer + whole, f32(fraction - whole)


def chosen(cpu, head, following):
    return cpu.getf(head + 24) if cpu.read(head + 20, 1)[0] else following


def head_gain(cpu, head, records, c):
    frames = c['frames']
    gain = fade_curve(cpu, head, frames)
    if cpu.read(head + 20, 1)[0]:
        gain = [f32(g * min(1., max(0., 4 * abs(cpu.getf(head + 24))))) for g in gain]
    else:
        phase = abs(c['previous_speed'])
        step = f32(f32(abs(c['speed']) - phase) / frames)
        for i in range(frames):
            gain[i] = f32(gain[i] * min(1., max(0., f32(4 * phase))))
            phase = f32(phase + step)
    for record in records:
        ps = chosen(cpu, head, c['speed'])
        rs = chosen(cpu, record, c['speed'])
        pv = f32(c['previous_factor'] * chosen(cpu, head, c['previous_speed']))
        rv = f32(c['previous_factor'] * chosen(cpu, record, c['previous_speed']))
        pstep = f32(libm.fmaf(c['factor'], ps, -pv) / frames)
        rstep = f32(libm.fmaf(c['factor'], rs, -rv) / frames)
        pi, pf = cpu.gets(head + 12), cpu.getf(head + 16)
        ri, rf = cpu.gets(record + 12), cpu.getf(record + 16)
        for i in range(frames):
            difference = abs(f32(pv - rv))
            if difference > f32(1e-6):
                distance = abs(f32(f32(f32(pi) + pf) - f32(f32(ri) + rf)))
                attenuation = min(1., max(0., f32(distance / f32(difference * 4096.))))
                gain[i] = f32(gain[i] * attenuation)
            pi, pf = split_advance(pi, pf, pv)
            ri, rf = split_advance(ri, rf, rv)
            pv, rv = f32(pv + pstep), f32(rv + rstep)
    return gain


def main():
    tape = [f32(.3 * math.sin(i * .137) + .1 * math.cos(i * .071)) for i in range(8192)]
    coverage, rows, comparisons, max_error = set(), [], 0, 0.
    for frames, speed_pair, factor_pair, record_profile, head_count in itertools.product(
            (7, 32), ((.125, .125), (.0, .5), (-.5, -.125), (.125, -.125)),
            ((.75, .75), (.5, 1.)),
            ('none', 'same', 'opposed', 'two', 'far', 'near_equal', 'above_equal', 'held_zero'), (1, 4)):
        previous_speed, speed = speed_pair
        previous_factor, factor = factor_pair
        c = dict(frames=frames, speed=speed, previous_speed=previous_speed,
                 factor=factor, previous_factor=previous_factor)
        cpu = ARMBytes()
        call(cpu, 0x4CA68, MANAGER)
        call(cpu, 0x4CA68, RECORD_MANAGER)
        for i in range(head_count):
            head = call(cpu, 0x4EEA8, MANAGER, 1000 + i * 50, 32, speed >= 0)
            cpu.putf(head + 8, .375)
            if i % 2:
                cpu.write(head + 20, b'\x01')
                cpu.putf(head + 24, -.5)
            cpu.reg(0, head)
            cpu.fp(0, speed)
            cpu.fp(1, factor)
            cpu.fp(2, frames)
            cpu.call(0x4BDE0)
        records = []
        if record_profile != 'none':
            for i in range(2 if record_profile == 'two' else 1):
                record = call(cpu, 0x4EEA8, RECORD_MANAGER,
                              (6000 if record_profile == 'far' else 1000) + i * 50, 32, True)
                cpu.putf(record + 16, .375 if i == 0 else .875)
                if record_profile in ('opposed', 'two'):
                    cpu.write(record + 20, b'\x01')
                    cpu.putf(record + 24, -1. if i == 0 else .5)
                if record_profile in ('near_equal', 'above_equal', 'held_zero'):
                    cpu.write(record + 20, b'\x01')
                    cpu.putf(record + 24, {'near_equal': .125 + .5e-6,
                                          'above_equal': .125 + 2e-6,
                                          'held_zero': 0.}[record_profile])
                records.append(record)
        play_list = call(cpu, 0x4D984, MANAGER)
        heads = [cpu.getu(play_list + i * 4) for i in range(20) if cpu.getu(play_list + i * 4)]
        record_list = call(cpu, 0x4D984, RECORD_MANAGER)
        records = [cpu.getu(record_list + i * 4) for i in range(20) if cpu.getu(record_list + i * 4)]
        cpu.vector(WORK + 12, OUTPUT, [0.] * frames)
        cpu.vector(WORK + 24, GAINS, [1.] * frames)
        cpu.putu(CHANNEL + 171972, TAPE)
        cpu.write(TAPE, struct.pack('<8192f', *tape))
        cpu.write(0x93F98, struct.pack('<256f', *TABLE))
        cpu.putu(STACK + 0x20, MANAGER)
        cpu.putu(STACK + 0x30, RECORD_MANAGER)
        cpu.reg(4, CHANNEL)
        cpu.reg(5, WORK)
        for register, value in ((16, frames), (17, factor), (18, previous_factor),
                                 (19, speed), (20, previous_speed),
                                 (21, abs(speed)), (22, abs(previous_speed))):
            cpu.fp(register, value)
        expected, checked, gains_observed = [0.] * frames, [], []
        model_gains = []
        for head in heads:
            gains = head_gain(cpu, head, records, c)
            model_gains.append(gains)
            position, fraction = cpu.gets(head + 12), cpu.getf(head + 16)
            position += 2 if chosen(cpu, head, speed) >= 0 else -3
            current = f32(f32(cpu.gets(head + 4)) + cpu.getf(head + 8))
            previous = f32(f32(cpu.gets(head + 12)) + cpu.getf(head + 16))
            step = f32(f32(current - previous) / frames)
            for i in range(frames):
                index = max(1, min(29501997, position)) - 1
                value = cubic(*tape[index:index + 4], fraction)
                expected[i] += gains[i] * value
                position, fraction = split_advance(position, fraction, step)
        cpu.observers[0x48048] = lambda machine: checked.append(machine.reg(6))
        cpu.observers[0x4829C] = lambda machine: gains_observed.append(machine.floats(GAINS, frames))
        cpu.call(0x47FE4, stop_before=0x483A4)
        assert checked == heads
        assert len(gains_observed) == len(heads)
        actual = cpu.floats(OUTPUT, frames)
        for want, observed in [(expected, actual)] + list(zip(model_gains, gains_observed)):
            for a, b in zip(want, observed):
                error = abs(a - b)
                assert math.isfinite(b) and error <= 8e-7, (c, record_profile, head_count, a, b, error)
                max_error = max(max_error, error)
                comparisons += 1
        rows.append(dict(config=c, record_profile=record_profile, heads=head_count,
                         checked=[p - MANAGER for p in checked], output=actual,
                         model_output=expected, gains=gains_observed))
        coverage.update(cpu.coverage)
    result = dict(status='PASS', cases=len(rows), comparisons=comparisons, max_abs_error=max_error,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  fixtures=rows, distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Render phase only; positions, velocity locals and record states are explicit fixtures.',
                               'No tape writes or complete simultaneous record/playback callback.',
                               'No boundary generation in this probe; original allocation/movement is setup.',
                               'Fade model reads original fade states and uses reconstructed cosine table.',
                               'No anti-alias output, engine-count gain, coloration or hardware comparison.'])
    (ROOT / 'probes/render_iteration_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'cases', 'comparisons', 'max_abs_error',
                                            'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
