#!/usr/bin/env python3
"""Execute original transport/write bytes with small, inspectable object fixtures.

The full Channel/AudioEngine callback and appliance environment are not run.
Original Tap, InputBuffer, fade, write-blend and clipping routines execute in
Unicorn. Comparisons use independent coordinate/resampling equations.
"""
import csv
import hashlib
import json
import math
import random
import struct

import unicorn
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32

CHANNEL, LINK, TAP, INPUT = 0x100000, 0x140000, 0x150000, 0x160000
INPUT_DATA, TAPE, CONTRIBUTION, INDICES, FEEDBACK = 0x170000, 0x180000, 0x190000, 0x1A0000, 0x1B0000
AUDIO_DATA = CHANNEL + 171900
WRITE_WORK = CHANNEL + 172032
TAPE_LENGTH = 2048
WORK_COUNT = 1024


def cubic(a, b, c, d, t):
    delta = c - b
    return b + t * (delta - (1 - t) * f32(0.16666670143604279) *
                    (d + 2 * a - 3 * b + t * (d - a - 3 * delta)))


def input_sample(values, position):
    index = max(0, min(len(values) - 4, math.trunc(position)))
    return cubic(*values[index:index + 4], position - index)


def setup():
    cpu = ARMBytes()
    cpu.putu(CHANNEL + 0xE8, LINK)
    cpu.putu(AUDIO_DATA, CHANNEL)
    cpu.putu(AUDIO_DATA + 0x48, TAPE)
    cpu.vector(WRITE_WORK + 0x24, CONTRIBUTION, [0.] * WORK_COUNT)
    cpu.vector(WRITE_WORK + 0x30, INDICES, [0] * WORK_COUNT, integers=True)
    cpu.vector(WRITE_WORK + 0x3C, FEEDBACK, [0.] * WORK_COUNT)
    # Original constructor sets position, held-speed flag, and four fade states.
    cpu.reg(0, TAP)
    cpu.call(0x4BB88)
    # Deliberately transparent finite-range write clipping for initial transport
    # isolation: original setter with knee=1 and compensation=0.
    cpu.reg(0, CHANNEL + 25160)
    cpu.fp(0, 1.)
    cpu.fp(1, 0.)
    cpu.call(0x4FFE0)
    fade_table = (ROOT / 'tables/reconstructed_xfade_table.f32le').read_bytes()
    assert len(fade_table) == 1024
    cpu.write(0x93F98, fade_table)
    return cpu


def tap_state(cpu):
    return dict(position=cpu.gets(TAP + 4), fraction=cpu.getf(TAP + 8),
                previous_position=cpu.gets(TAP + 12), previous_fraction=cpu.getf(TAP + 16))


def move(cpu, speed, factor=1., frames=1., held=None):
    cpu.putf(LINK, speed)
    cpu.write(TAP + 0x14, bytes([held is not None]))
    cpu.putf(TAP + 24, speed if held is None else held)
    cpu.reg(0, TAP)
    cpu.fp(0, speed)
    cpu.fp(1, factor)
    cpu.fp(2, frames)
    cpu.call(0x4BDE0)
    return tap_state(cpu)


def record(cpu, speed, input_values, feedback=0., factor=1., phase=None, held=None):
    cpu.vector(INPUT + 4, INPUT_DATA, input_values)
    if phase is not None:
        cpu.putf(INPUT + 16, phase)
    cpu.putf(LINK, speed)
    cpu.write(TAP + 0x14, bytes([held is not None]))
    cpu.putf(TAP + 24, speed if held is None else held)
    cpu.reg(0, 0)
    cpu.reg(1, CHANNEL)
    cpu.reg(2, TAP)
    cpu.reg(3, INPUT)
    cpu.fp(0, factor)
    cpu.fp(1, feedback)
    cpu.call(0x47818)
    count = abs(cpu.gets(TAP + 4) - cpu.gets(TAP + 12))
    return dict(count=count, input_phase=cpu.getf(INPUT + 16),
                indices=[cpu.gets(INDICES + i * 4) for i in range(count)],
                contribution=cpu.floats(CONTRIBUTION, count),
                feedback=cpu.floats(FEEDBACK, count), tape=cpu.floats(TAPE, TAPE_LENGTH))


def main():
    rows, cases, coverage = [], [], set()
    rng = random.Random(210)

    def check(case, field, expected, observed, tolerance=2e-6):
        error = abs(expected - observed)
        assert math.isfinite(observed) and error <= tolerance, (case, field, expected, observed, error)
        rows.append([case, field, expected, observed, error])

    # Split integer/fraction transport at long coordinates and both signs.
    for origin in (0, 100, 16777217, 29501990):
        for initial_fraction in (0., .125, .75):
            for speed in (-4., -2., -1., -.5, -.25, 0., .25, .5, 1., 2., 4.):
                for held in (None, -.5, 1.):
                    cpu = setup()
                    cpu.puts(TAP + 4, origin)
                    cpu.putf(TAP + 8, initial_fraction)
                    effective = speed if held is None else held
                    total = f32(initial_fraction + f32(f32(effective * .75) * 7.))
                    whole = math.floor(total)
                    result = move(cpu, speed, factor=.75, frames=7., held=held)
                    name = f'move:{origin}:{initial_fraction}:{speed}:{held}'
                    check(name, 'position', origin + whole, result['position'], 0.)
                    check(name, 'fraction', total - whole, result['fraction'], 0.)
                    check(name, 'previous_position', origin, result['previous_position'], 0.)
                    check(name, 'previous_fraction', initial_fraction, result['previous_fraction'], 0.)
                    coverage.update(cpu.coverage)

    # Whole recordInput routine: the previous integer coordinate is included and
    # the newly reached integer coordinate is excluded, in either direction.
    values = [f32(.1 + .002 * i) for i in range(132)]
    speeds = [-4., -2., -1., -.5, -.25, 0., .25, .5, 1., 2., 4.]
    for speed in speeds:
        for frames in (1, 7, 32, 128):
            for initial_phase in (0., .25, .75):
                cpu = setup()
                origin, initial_fraction = 700, .25
                cpu.puts(TAP + 4, origin)
                cpu.putf(TAP + 8, initial_fraction)
                move(cpu, speed, frames=float(frames))
                result = record(cpu, speed, values, phase=initial_phase)
                count = abs(math.floor(initial_fraction + speed * frames))
                direction = 1 if speed > 0 else -1 if speed < 0 else 0
                expected_indices = [origin + direction * i for i in range(count)]
                name = f'record:{speed}:{frames}:{initial_phase}'
                check(name, 'count', count, result['count'], 0.)
                assert result['indices'] == expected_indices, (name, result['indices'], expected_indices)
                inverse = 1. / abs(speed) if abs(speed) >= f32(1e-6) else 0.
                phase = initial_phase
                for i, index in enumerate(expected_indices):
                    expected = input_sample(values, phase)
                    check(name, f'tape[{index}]', expected, result['tape'][index])
                    phase = f32(phase + inverse)
                if not count:
                    phase = f32(phase + inverse)
                phase -= math.trunc(phase)
                check(name, 'input_phase', phase, result['input_phase'])
                unchanged = set(range(TAPE_LENGTH)) - set(expected_indices)
                assert all(result['tape'][i] == 0. for i in unchanged)
                cases.append(dict(case=name, speed=speed, frames=frames, initial_phase=initial_phase,
                                  write_count=count, first_index=expected_indices[0] if count else None,
                                  last_index=expected_indices[-1] if count else None,
                                  final_phase=result['input_phase']))
                coverage.update(cpu.coverage)

    # Overdub and held-record speed: follow only the selected effective speed,
    # resample input, blend the old sample, and clip before writing storage.
    for speed, held, factor in ((2., None, .75), (-1., None, .5),
                                (4., .25, 1.), (-4., 1., 1.), (1., -2., .5)):
        for feedback in (0., .5, 1.):
            for knee, compensation in ((1., 0.), (.3, 0.), (0., .2)):
                cpu = setup()
                old = [f32(.15 * math.sin(i * .17)) for i in range(TAPE_LENGTH)]
                cpu.write(TAPE, struct.pack('<' + 'f' * len(old), *old))
                cpu.reg(0, CHANNEL + 25160)
                cpu.fp(0, knee)
                cpu.fp(1, compensation)
                cpu.call(0x4FFE0)
                origin = 700
                cpu.puts(TAP + 4, origin)
                cpu.putf(TAP + 8, .25)
                effective = f32((speed if held is None else held) * factor)
                move(cpu, speed, factor=factor, frames=128., held=held)
                result = record(cpu, speed, values, feedback=feedback, factor=factor, phase=.125, held=held)
                count = abs(math.floor(.25 + effective * 128.))
                direction = 1 if effective > 0 else -1
                phase = .125
                name = f'overdub:{speed}:{held}:{factor}:{feedback}:{knee}:{compensation}'
                for i in range(count):
                    index = origin + direction * i
                    value = input_sample(values, phase) + feedback * old[index]
                    if knee == 0.:
                        value *= 1. + compensation
                        squared = value * value
                        expected = max(-1., min(1., value * (28.274333953857422 + squared) /
                                               (28.274333953857422 + 9.42477798461914 * squared)))
                    else:
                        value = max(-1., min(1., value))
                        delta = max(0., abs(value) - knee)
                        expected = value if delta == 0. else math.copysign(
                            knee + delta / (1. + (delta / (1. - knee)) ** 2), value)
                    check(name, f'tape[{index}]', expected, result['tape'][index], 4e-7)
                    phase = f32(phase + f32(1. / abs(effective)))
                coverage.update(cpu.coverage)

    # Active playback-tap fades multiply, rather than replacing one another.
    # Use in-table coordinates here; overflow/expiry scheduling remains separate.
    table = list(struct.unpack('<256f', (ROOT / 'tables/reconstructed_xfade_table.f32le').read_bytes()))

    def fade_at(coordinate):
        index = math.floor(coordinate)
        fraction = coordinate - index
        return table[index] + fraction * (table[index + 1] - table[index])

    for fade_count in (1, 2, 4):
        cpu = setup()
        cpu.puts(TAP + 4, 700)
        cpu.putf(TAP + 8, .25)
        move(cpu, 1., frames=32.)
        for fade in range(fade_count):
            base = TAP + 28 + fade * 24
            cpu.write(base, b'\x01')
            cpu.puts(base + 4, 68 + fade * 16)
            cpu.putf(base + 8, .75)
            cpu.puts(base + 12, 64 + fade * 16)
            cpu.putf(base + 16, .25)
        result = record(cpu, 1., values, phase=.125)
        for i in range(32):
            gain = math.prod(fade_at(64.25 + fade * 16 + 4.5 * i / 32) for fade in range(fade_count))
            expected = input_sample(values, .125 + i) * gain
            check(f'tap_fades:{fade_count}', f'tape[{700+i}]', expected, result['tape'][700 + i], 3e-7)
        coverage.update(cpu.coverage)

    # The additional write fade controls incoming contribution and old-buffer
    # retention independently. Its reversed table interpolation is recovered as
    # written, including the 254-index convention in this stationary branch.
    for position, fraction in ((0, 0.), (64, .25), (128, .5), (253, .75)):
        for feedback in (0., .5, 1.):
            cpu = setup()
            cpu.write(TAPE, struct.pack('<' + 'f' * TAPE_LENGTH, *([.2] * TAPE_LENGTH)))
            cpu.puts(TAP + 4, 700)
            move(cpu, 1., frames=16.)
            fade = CHANNEL + 6504
            cpu.write(fade, b'\x01')
            for offset, value in ((4, position), (12, position)):
                cpu.puts(fade + offset, value)
            for offset, value in ((8, fraction), (16, fraction)):
                cpu.putf(fade + offset, value)
            result = record(cpu, 1., values, feedback=feedback, phase=.125)
            incoming_gain = fade_at(position + fraction)
            retention = feedback + (1. - feedback) * fade_at(254 - position + fraction)
            for i in range(16):
                expected = input_sample(values, .125 + i) * incoming_gain + .2 * retention
                name = f'write_fade:{position}:{fraction}:{feedback}'
                check(name, f'tape[{700+i}]', expected, result['tape'][700 + i], 3e-7)
                check(name, f'feedback[{i}]', retention, result['feedback'][i], 2e-7)
            coverage.update(cpu.coverage)

    # Signed read addressing is directional and clamps to storage capacity rather
    # than this fixture's logical loop. Staying near the low boundary is sufficient
    # to expose the direction-dependent neighborhoods safely in a small buffer.
    values = [f32(rng.uniform(-.8, .8)) for _ in range(TAPE_LENGTH)]
    for speed in (-1., 0., 1.):
        for held in (None, -1., 1.):
            for position in (-4, -1, 0, 1, 2, 3, 5, 20):
                for fraction in (0., .125, .5, .875):
                    cpu = setup()
                    cpu.write(TAPE, struct.pack('<' + 'f' * len(values), *values))
                    cpu.puts(TAP + 4, position)
                    cpu.putf(TAP + 8, fraction)
                    cpu.putf(LINK, speed)
                    cpu.write(TAP + 0x14, bytes([held is not None]))
                    cpu.putf(TAP + 24, speed if held is None else held)
                    effective = speed if held is None else held
                    anchor = max(1, position + (2 if effective >= 0. else -3)) - 1
                    expected = cubic(*values[anchor:anchor + 4], fraction)
                    cpu.reg(0, AUDIO_DATA)
                    cpu.reg(1, TAP)
                    cpu.call(0x38748)
                    name = f'read:{speed}:{held}:{position}:{fraction}'
                    check(name, 'sample', expected, cpu.fp(0), 3e-7)
                    coverage.update(cpu.coverage)

    # Physical-capacity high clamp with sparse pages, independent of small-loop
    # scheduling. No 118 MB buffer allocation is needed for this boundary probe.
    capacity = 29502000
    for speed in (-1., 0., 1.):
        for position in (capacity - 5, capacity - 2, capacity - 1, capacity, capacity + 10):
            for fraction in (0., .5, .875):
                cpu = setup()
                first = capacity - 12
                cpu.map_fixture(TAPE + first * 4, 16 * 4)
                high_values = [f32(rng.uniform(-.8, .8)) for _ in range(16)]
                cpu.write(TAPE + first * 4, struct.pack('<16f', *high_values))
                cpu.putf(LINK, speed)
                cpu.puts(TAP + 4, position)
                cpu.putf(TAP + 8, fraction)
                anchor = max(1, min(capacity - 3, position + (2 if speed >= 0 else -3))) - 1
                expected = cubic(*high_values[anchor - first:anchor - first + 4], fraction)
                cpu.reg(0, AUDIO_DATA)
                cpu.reg(1, TAP)
                cpu.call(0x38748)
                check(f'high_read:{speed}:{position}:{fraction}', 'sample', expected, cpu.fp(0), 3e-7)
                coverage.update(cpu.coverage)

    with (ROOT / 'probes/transport_probe_vectors.csv').open('w', newline='') as handle:
        writer = csv.writer(handle)
        writer.writerow(['case', 'field', 'model', 'original_byte_probe', 'abs_error'])
        writer.writerows(rows)
    result = dict(
        status='PASS', seed=210, unicorn_version=unicorn.__version__,
        method='Original ARM bytes in Unicorn Cortex-A15 with initialized objects; no full callback or OS',
        main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
        comparisons=len(rows), record_cases=cases,
        max_abs_error=max(row[-1] for row in rows),
        distinct_instruction_addresses=len(coverage),
        coverage_addresses=[hex(address) for address in sorted(coverage)],
        limitations=[
            'No complete Channel construction, main callback, hardware IO or OS execution.',
            'Active tap fades and stationary write fades are covered; complete expiry/transition scheduling is not.',
            'InputBuffer contents and starting phase are explicit fixtures.',
            'No loop wrap scheduling, recording tail, clock/link or transition allocator claim yet.',
            'Read capacity clamps are exercised; signed wrap scheduling and write capacity exhaustion remain open.',
        ],
    )
    (ROOT / 'probes/transport_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in (
        'status', 'comparisons', 'max_abs_error', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
