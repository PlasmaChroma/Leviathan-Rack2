#!/usr/bin/env python3
"""Original multihead splice rendering against fade/interpolation equations."""
import csv
import hashlib
import itertools
import json
import math
import struct

import unicorn
from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_boundary_transitions import execute, CHANNEL, MANAGER, SLOT_SIZE
from probe_tap_allocation import call
from probe_transport import cubic

WORK, RECORD_MANAGER = CHANNEL + 172032, CHANNEL + 3688
TAPE, OUTPUT, GAINS = 0x180000, 0x1D0000, 0x1C0000
TABLE = list(struct.unpack('<256f', (ROOT/'tables/reconstructed_xfade_table.f32le').read_bytes()))


def fade_curve(cpu, pointer, frames):
    gain = [1.] * frames
    for kind in range(4):
        b = pointer + 28 + kind * 24
        if not cpu.read(b, 1)[0]:
            continue
        ci, pi = cpu.gets(b + 4), cpu.gets(b + 12)
        cf, pf = cpu.getf(b + 8), cpu.getf(b + 16)
        current, previous = f32(f32(ci)+cf), f32(f32(pi)+pf)
        difference = f32(current-previous)
        curve = [1.] * frames
        if abs(difference) < f32(.1):
            assert 0 <= ci <= 255, ('stationary out-of-table fixture', ci)
            next_value = TABLE[ci+1] if ci < 255 else 0.
            value = libm.fmaf(cf, f32(next_value-TABLE[ci]), TABLE[ci])
            curve = [value] * frames
        else:
            step, magnitude = f32(difference/frames), f32(abs(difference)/frames)
            begin, end, phase = 0, frames, previous
            if pi < 0 or pi > 254:
                distance = f32(-previous if pi < 0 else previous-254.)
                begin = max(0, min(frames, math.trunc(f32(distance/magnitude))+1))
                phase = libm.fmaf(magnitude if pi < 0 else -magnitude, f32(begin), previous)
                if pi < 0:
                    curve[:begin] = [0.] * begin
            if ci < 0 or ci > 254:
                distance = f32(-current if ci < 0 else current-254.)
                remaining = max(0, min(frames, math.trunc(f32(distance/magnitude))+1))
                end = frames-remaining
                if ci < 0:
                    curve[end:] = [0.] * (frames-end)
            if pi < 0 or pi > 254:
                index = math.trunc(phase)
                fraction = f32(phase-f32(index))
            else:
                index, fraction = pi, pf
            for i in range(begin, end):
                assert 0 <= index <= 254, ('moving fade index', index, phase, begin, end)
                curve[i] = libm.fmaf(fraction, f32(TABLE[index+1]-TABLE[index]), TABLE[index])
                fraction = f32(fraction+step)
                whole = math.floor(fraction)
                index += whole
                fraction = f32(fraction-whole)
        gain = [f32(a*b) for a, b in zip(gain, curve)]
    return gain


def render(cpu, pointer, c, tape, accumulated):
    frames = int(c['frames'])
    gains = fade_curve(cpu, pointer, frames)
    chosen = cpu.getf(pointer+24) if cpu.read(pointer+20, 1)[0] else c['speed']
    speed_gain = min(1., max(0., 4*abs(chosen)))
    gains = [f32(g*speed_gain) for g in gains]
    previous = cpu.gets(pointer+12)
    fraction = cpu.getf(pointer+16)
    current_coordinate = f32(f32(cpu.gets(pointer+4))+cpu.getf(pointer+8))
    previous_coordinate = f32(f32(previous)+fraction)
    increment = f32(f32(current_coordinate-previous_coordinate)/frames)
    anchor = previous+(2 if chosen >= 0 else -3)
    expected = list(accumulated)
    for i in range(frames):
        index = max(1, min(29501997, anchor))-1
        assert 0 <= index <= len(tape)-4, ('render fixture exceeds tape', index)
        value = cubic(*tape[index:index+4], fraction)
        expected[i] += gains[i]*value
        fraction = f32(fraction+increment)
        whole = math.floor(fraction)
        anchor += whole
        fraction = f32(fraction-whole)
    cpu.reg(4, CHANNEL)
    cpu.reg(5, WORK)
    cpu.reg(6, pointer)
    cpu.reg(7, 29501997)
    for register, value in ((16, frames), (17, c['factor']), (18, 0.),
                             (19, c['speed']), (20, c['speed']), (21, abs(c['speed'])),
                             (22, abs(c['speed'])), (24, 4096.), (25, 0.)):
        cpu.fp(register, value)
    cpu.call(0x48048, stop_before=0x48398)
    return expected, cpu.floats(OUTPUT, frames), gains, cpu.floats(GAINS, frames)


def main():
    rows, coverage, cases = [], set(), []
    tape = [f32(.3*math.sin(i*.137)+.1*math.cos(i*.071)) for i in range(4096)]
    for speed, held, frames, fraction, direction, family in itertools.product(
            (0., .125, .5, 1.), (None, .125), (7, 32), (0., .375), (-1, 1), ('ordinary', 'physical')):
        speed *= direction
        held = held*direction if held is not None else None
        effective = speed if held is None else held
        delta = f32(effective*.75*frames)
        target = (501 if direction > 0 else 131) if family == 'ordinary' else (1025 if direction > 0 else 31)
        c = dict(start=100 if family == 'ordinary' else 700, end=500 if family == 'ordinary' else 300,
                 reverse_start=132 if family == 'ordinary' else 732,
                 reverse_end=532 if family == 'ordinary' else 332,
                 period=1024, physical_reverse=32, physical_reverse_destination=1282,
                 fade=32, speed=speed, held=held, effective_speed=effective, factor=.75, frames=frames,
                 initial_fraction=fraction, previous=target-math.floor(f32(fraction+delta)),
                 replacement=True, saturated=False, already_fading=False)
        boundary, cpu = execute(c, return_cpu=True)
        call(cpu, 0x4CA68, RECORD_MANAGER)
        cpu.vector(WORK+12, OUTPUT, [0.]*frames)
        cpu.vector(WORK+24, GAINS, [1.]*frames)
        cpu.putu(CHANNEL+171972, TAPE)
        cpu.write(TAPE, struct.pack('<4096f', *tape))
        cpu.write(0x93F98, struct.pack('<256f', *TABLE))
        cpu.putu(STACK+0x30, RECORD_MANAGER)
        cpu.putu(STACK+12, WORK+24)
        accumulated = [0.]*frames
        for slot in boundary['active_slots']:
            pointer = MANAGER+slot*SLOT_SIZE
            expected, observed, model_gain, original_gain = render(cpu, pointer, c, tape, accumulated)
            name = f'{family}:{speed}:{held}:{frames}:{fraction}:{slot}'
            for i in range(frames):
                for field, want, actual in (('mix', expected[i], observed[i]), ('gain', model_gain[i], original_gain[i])):
                    error = abs(want-actual)
                    assert math.isfinite(actual) and error <= 8e-7, (name, field, i, want, actual, error)
                    rows.append([name, field, i, want, actual, error])
            accumulated = expected
        coverage.update(cpu.coverage)
        cases.append(dict(config=c, active_slots=boundary['active_slots'], output=observed, model_output=accumulated))
    with (ROOT/'probes/splice_render_probe_vectors.csv').open('w', newline='') as handle:
        writer = csv.writer(handle)
        writer.writerow(['case','field','sample','model','original_byte_probe','abs_error'])
        writer.writerows(rows)
    result = dict(status='PASS', unicorn_version=unicorn.__version__, cases=len(cases), comparisons=len(rows),
                  max_abs_error=max(r[-1] for r in rows), examples=cases,
                  method='Original boundary slice followed by original one-head render slices; independent fade/cubic/mix model',
                  main_sha256=hashlib.sha256((ROOT/'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(coverage),coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=[
                      'No complete callback/OS/hardware comparison; upstream locals are fixtures.',
                      'Fade table is host-cosine reconstruction; model uses host fmaf for fused operations.',
                      'Renderer model reads resulting original fade state; independent trigger-phase equations remain open.',
                      'No active record heads, overlap suppression, coloration, plate or final engine-count normalization.',
                      'Following-speed ramp starts and ends at the same magnitude; nonstationary ramp remains open.',
                      'Render active slots directly; callback active-list ordering/iteration remains to be checked.',
                  ])
    (ROOT/'probes/splice_render_probe_results.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({key:result[key] for key in ('status','cases','comparisons','max_abs_error','distinct_instruction_addresses')},indent=2))


if __name__ == '__main__':
    main()
