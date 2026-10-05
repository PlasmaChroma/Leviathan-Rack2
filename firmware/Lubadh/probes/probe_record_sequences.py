#!/usr/bin/env python3
"""Original-byte block-history, direction-change and physical-write probes.

Explicit calls establish routine contracts, not the complete Channel scheduler.
"""
import hashlib
import json
import math
import struct

import unicorn
from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32
from probe_transport import (setup, move, record, input_sample, CHANNEL, TAP,
                             INPUT, INPUT_DATA, TAPE, TAPE_LENGTH, AUDIO_DATA,
                             WRITE_WORK, CONTRIBUTION, INDICES, FEEDBACK)

BLOCK_HEADER, BLOCK_DATA = 0x1C0000, 0x1D0000


def main():
    comparisons, max_error, coverage, sequences = 0, 0., set(), []

    def check(expected, observed, label, tolerance=3e-7):
        nonlocal comparisons, max_error
        error = abs(expected - observed)
        assert math.isfinite(observed) and error <= tolerance, (label, expected, observed, error)
        comparisons += 1
        max_error = max(max_error, error)

    # Original input() carries four samples into the next fixed-size block.
    # Hold phase across calls; vary speed, including stall and sign changes.
    schedules = [
        [1.] * 6, [-1.] * 6, [.25] * 8, [-.25] * 8,
        [0., .25, .5, 1., 2., 4., 0., -4., -2., -1., -.5, -.25, 0., 1.],
        [.3, -.3, .7, -.7, 1.3, -1.3, 0., .3],
    ]
    for frames in (7, 32, 128):
        for schedule_number, schedule in enumerate(schedules):
            cpu = setup()
            cpu.puts(TAP + 4, 1024)
            cpu.putf(TAP + 8, .25)
            cpu.putf(INPUT + 16, .125)
            history = [f32(-.03 + i * .01) for i in range(4)]
            buffer = history + [0.] * frames
            cpu.vector(INPUT + 4, INPUT_DATA, buffer)
            tape, coordinate, fraction, phase = [0.] * TAPE_LENGTH, 1024, .25, .125
            for block, speed in enumerate(schedule):
                samples = [f32(.15 * math.sin((block * frames + i) * .071)) for i in range(frames)]
                buffer = buffer[-4:] + samples
                cpu.vector(BLOCK_HEADER, BLOCK_DATA, samples)
                cpu.reg(0, INPUT)
                cpu.reg(1, BLOCK_HEADER)
                cpu.call(0x38A78)
                observed_buffer = cpu.floats(INPUT_DATA, frames + 4)
                for i, sample in enumerate(buffer):
                    check(sample, observed_buffer[i], f'history:{frames}:{schedule_number}:{block}:{i}', 0.)
                before = coordinate
                total = f32(fraction + f32(f32(speed) * frames))
                whole = math.floor(total)
                coordinate += whole
                fraction = total - whole
                moved = move(cpu, speed, frames=float(frames))
                check(coordinate, moved['position'], 'sequence position', 0.)
                check(fraction, moved['fraction'], 'sequence fraction', 0.)
                observed = record(cpu, speed, observed_buffer, feedback=.5)
                count = abs(coordinate - before)
                inverse = f32(1. / abs(f32(speed))) if abs(speed) >= f32(1e-6) else 0.
                direction = 1 if speed > 0 else -1 if speed < 0 else 0
                for i in range(count):
                    index = before + direction * i
                    assert 0 <= index < TAPE_LENGTH, ('sequence fixture exceeds storage', index)
                    tape[index] = f32(input_sample(buffer, phase) + .5 * tape[index])
                    phase = f32(phase + inverse)
                if count == 0:
                    phase = f32(phase + inverse)
                phase -= math.trunc(phase)
                check(phase, observed['input_phase'], 'sequence phase', 0.)
                for index, value in enumerate(tape):
                    check(value, observed['tape'][index], f'sequence tape:{index}')
            sequences.append(dict(frames=frames, speeds=schedule, position=coordinate,
                                  fraction=fraction, input_phase=phase))
            coverage.update(cpu.coverage)

    # AudioData::add gathers ALL old samples, clips, then scatters. Repeated
    # clamped indices therefore read the same original cell and last write wins.
    capacity = 29502000
    for indices in ([-3, -1, 0, 1, 0],
                    [capacity - 2, capacity - 1, capacity, capacity + 10, capacity - 1]):
        cpu = setup()
        if max(indices) >= capacity:
            cpu.map_fixture(TAPE + (capacity - 4) * 4, 8 * 4)
        clamped = [max(0, min(capacity - 1, index)) for index in indices]
        for index in set(clamped):
            cpu.putf(TAPE + index * 4, .2)
        values = [f32(.01 * (i + 1)) for i in range(len(indices))]
        cpu.vector(WRITE_WORK + 0x24, CONTRIBUTION, values)
        cpu.vector(WRITE_WORK + 0x30, INDICES, indices, integers=True)
        cpu.vector(WRITE_WORK + 0x3C, FEEDBACK, [.5] * len(indices))
        cpu.reg(0, AUDIO_DATA)
        cpu.reg(1, len(indices))
        cpu.reg(2, WRITE_WORK + 0x24)
        cpu.reg(3, WRITE_WORK + 0x30)
        cpu.putu(STACK, WRITE_WORK + 0x3C)
        cpu.call(0x387FC)
        expected_tape = {}
        for i, index in enumerate(clamped):
            value = f32(values[i] + .5 * f32(.2))
            check(value, cpu.getf(CONTRIBUTION + i * 4), 'gathered blend', 2e-8)
            expected_tape[index] = value
        for index, value in expected_tape.items():
            check(value, cpu.getf(TAPE + index * 4), 'last scatter wins', 2e-8)
        coverage.update(cpu.coverage)

    result = dict(status='PASS', method='Original ARM bytes in Unicorn with explicit call sequences',
                  unicorn_version=unicorn.__version__,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  comparisons=comparisons, max_abs_error=max_error, sequences=sequences,
                  distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(address) for address in sorted(coverage)],
                  limitations=[
                      'Direct routine sequencing, not proof of Channel callback scheduling or UI reachability.',
                      'No logical loop wraps, tail write scheduling, moving write fade or allocator exhaustion.',
                      'Capacity scatter probes establish memory primitive behavior, not full-buffer state transitions.',
                  ])
    (ROOT / 'probes/record_sequence_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in (
        'status', 'comparisons', 'max_abs_error', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
