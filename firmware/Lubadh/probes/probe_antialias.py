#!/usr/bin/env python3
"""Audit AntiAlias and its cutoff dispatch using the supplied instruction stream.

No appliance process is run. Objects are initialized explicitly. The dispatch
probe starts after processSpeed and stops before unrelated coloration processing.
"""
import csv
import hashlib
import json
import math
import random

from arm_leaf_probe import ARMLeaf, ROOT, f32, libm

SETTER = 0x4B1C0
PROCESS = 0x4B1F0
DISPATCH_START = 0x47CC4
DISPATCH_END = 0x47D44
OBJ, VEC, DATA = 0x100000, 0x110000, 0x120000


class DispatchProbe(ARMLeaf):
    def step(self):
        if self.pc == DISPATCH_END:
            self.npc = 0xFFFFFFF0
            return
        super().step()


def vector(cpu, values):
    for i, value in enumerate(values):
        cpu.putf(DATA + 4 * i, value)
    for i, pointer in enumerate((DATA, DATA + 4 * len(values), DATA + 4 * len(values))):
        cpu.putu(VEC + 4 * i, pointer)
    cpu.r[0], cpu.r[1] = OBJ, VEC
    cpu.call(PROCESS)
    return [cpu.getf(DATA + 4 * i) for i in range(len(values))]


def coefficient(cutoff):
    return f32(1.0 / (math.pi * f32(cutoff)))


def reference(values, coeff, state):
    # Independent recurrence with explicit float32 rounding. Very small c places
    # poles near -1; an unrounded recurrence drifts from float32 on alternating
    # inputs. Match arithmetic precision rather than expanding the tolerance.
    output = list(values)
    for stage in range(2):
        previous_input, previous_low = state[stage]
        for i, value in enumerate(output):
            numerator = libm.fmaf(-previous_low, f32(1.0 - coeff), f32(value + previous_input))
            low = f32(numerator / f32(1.0 + coeff))
            previous_input, previous_low = value, low
            output[i] = low
        state[stage] = [previous_input, previous_low]
    return output


def cutoff_for_speed(speed):
    speed = abs(f32(speed))
    if speed >= 1.0:
        return 20000.0
    if speed > f32(0.001):
        return f32(20000.0 * speed)
    return 20.0


def main():
    rng = random.Random(210)
    rows, dispatch_rows, coverage = [], [], set()
    setter_checks = 0
    cutoffs = [20.0, 40.0, 200.0, 5000.0, 10000.0, 20000.0]
    for cutoff in cutoffs:
        cpu = ARMLeaf()
        for offset, value in ((4, 0.25), (8, -0.5), (16, 0.75), (20, -0.125), (24, 123.0)):
            cpu.putf(OBJ + offset, value)
        before = bytes(cpu.mem[OBJ:OBJ + 28])
        cpu.r[0] = OBJ
        cpu.putfp('s0', cutoff)
        cpu.call(SETTER)
        assert cpu.getf(OBJ) == coefficient(cutoff)
        assert cpu.getf(OBJ + 12) == coefficient(cutoff)
        assert bytes(cpu.mem[OBJ + 4:OBJ + 12]) == before[4:12]
        assert bytes(cpu.mem[OBJ + 16:OBJ + 28]) == before[16:28]
        setter_checks += 4
        coverage.update(cpu.coverage)

    # The extra coefficients deliberately test the topology beyond only its tiny
    # production coefficients. Guard bytes catch a third stage or adjacent write.
    stimuli = {
        'impulse': [1.0] + [0.0] * 255,
        'dc': [0.25] * 256,
        'alternating': [0.75 if i % 2 == 0 else -0.75 for i in range(256)],
        'random': [f32(rng.uniform(-1.0, 1.0)) for _ in range(256)],
    }
    coefficients = [coefficient(c) for c in cutoffs] + [0.25, 1.0, 5.0, 100.0]
    for coeff in coefficients:
        for name, values in stimuli.items():
            cpu = ARMLeaf()
            cpu.putf(OBJ, coeff)
            cpu.putf(OBJ + 12, coeff)
            cpu.load(OBJ + 24, b'guard!!!')
            expected = reference(values, coeff, [[0.0, 0.0], [0.0, 0.0]])
            actual = vector(cpu, values)
            assert bytes(cpu.mem[OBJ + 24:OBJ + 32]) == b'guard!!!'
            coverage.update(cpu.coverage)
            for i, (model, observed) in enumerate(zip(expected, actual)):
                error = abs(model - observed)
                assert math.isfinite(observed) and error <= 2e-7, (coeff, name, i, error)
                rows.append([name, coeff, i, model, observed, error])

    # Retain history across irregular vector sizes, empty blocks, and live cutoff
    # changes. This checks more than a single zero-state impulse.
    cpu = ARMLeaf()
    state = [[0.0, 0.0], [0.0, 0.0]]
    for block, (cutoff, count) in enumerate(zip(cutoffs * 2, [1, 0, 7, 128, 3, 64] * 2)):
        coeff = coefficient(cutoff)
        cpu.r[0] = OBJ
        cpu.putfp('s0', cutoff)
        cpu.call(SETTER)
        values = [f32(rng.uniform(-0.5, 0.5)) for _ in range(count)]
        before = bytes(cpu.mem[OBJ:OBJ + 24])
        expected = reference(values, coeff, state)
        actual = vector(cpu, values)
        if not values:
            assert bytes(cpu.mem[OBJ:OBJ + 24]) == before
        coverage.update(cpu.coverage)
        for i, (model, observed) in enumerate(zip(expected, actual)):
            error = abs(model - observed)
            assert math.isfinite(observed) and error <= 2e-7, (block, i, error)
            rows.append(['cutoff_transition', coeff, i, model, observed, error])

    # Execute the original dispatch slice and both original setters. The caller's
    # LinkData and post-processSpeed registers are fixtures, not a full Channel.
    threshold = f32(0.001)
    speeds = [-4.0, -1.0, -0.5, -0.001, 0.0, threshold * 0.999,
              threshold, threshold * 1.001, 0.25, 0.5, 0.999, 1.0, 4.0]
    for record_mode in (0, 1):
        for speed in speeds:
            cpu = DispatchProbe()
            channel, link = OBJ, VEC
            cpu.r[4], cpu.r[3] = channel, link
            for offset, value in ((0, speed), (4, -2.0), (12, 0.4), (16, 1.2)):
                cpu.putf(link + offset, value)
            cpu.putu(link + 0xA0, record_mode)
            cpu.call(DISPATCH_START)
            playback_cutoff = cutoff_for_speed(speed)
            input_cutoff = playback_cutoff if record_mode == 0 else 20000.0
            input_coeff = cpu.getf(channel + 6544)
            playback_coeff = cpu.getf(channel + 6568)
            assert input_coeff == coefficient(input_cutoff)
            assert playback_coeff == coefficient(playback_cutoff)
            assert cpu.getf(channel + 6556) == input_coeff
            assert cpu.getf(channel + 6580) == playback_coeff
            coverage.update(cpu.coverage - {DISPATCH_END})
            dispatch_rows.append(dict(speed=f32(speed), record_mode=record_mode,
                                      input_cutoff=input_cutoff, playback_cutoff=playback_cutoff,
                                      input_coefficient=input_coeff,
                                      playback_coefficient=playback_coeff))

    # Ideal linear transfer, derived from the recovered recurrence. This is not a
    # sampled device response or a float32 long-run frequency sweep.
    frequency_response = []
    for cutoff in cutoffs:
        c = coefficient(cutoff)
        for hz in (100.0, 1000.0, 10000.0, 20000.0, 23900.0):
            z = complex(math.cos(-2.0 * math.pi * hz / 48000.0),
                        math.sin(-2.0 * math.pi * hz / 48000.0))
            stage = (1.0 + z) / ((1.0 + c) + (1.0 - c) * z)
            db = 20.0 * math.log10(abs(stage * stage))
            frequency_response.append(dict(cutoff_argument=cutoff, frequency_hz=hz,
                                           assumed_rate_hz=48000.0, ideal_gain_db=db))

    with (ROOT / 'probes/antialias_probe_vectors.csv').open('w', newline='') as handle:
        writer = csv.writer(handle)
        writer.writerow(['stimulus', 'coefficient', 'sample', 'model', 'instruction_probe', 'abs_error'])
        writer.writerows(rows)
    result = dict(
        status='PASS', seed=210,
        method='Restricted original ARM/VFP instructions with explicit state fixtures; no hardware execution',
        main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
        routines=[hex(SETTER), hex(PROCESS)],
        dispatch_slice=[hex(DISPATCH_START), hex(DISPATCH_END)],
        output_comparisons=len(rows), setter_assertions=setter_checks,
        dispatch_cases=len(dispatch_rows),
        max_abs_error=max(row[-1] for row in rows), tolerance=2e-7,
        distinct_instruction_addresses=len(coverage),
        coverage_addresses=[hex(address) for address in sorted(coverage)],
        coefficient_equation='float32(1 / (pi_double * float32(cutoff_argument)))',
        stages=2, state_bytes=24,
        dispatch=dispatch_rows, ideal_frequency_response=frequency_response,
        limitations=[
            'Restricted interpreter is not an independent certified ARM emulator.',
            'Interpreter and rounded reference share host libm fmaf; zero discrepancy is not hardware bit identity.',
            'Dispatch starts after processSpeed; upstream speed/flutter/touch setup is not exercised.',
            'No complete Channel constructor, full callback, transport, analog circuit or codec execution.',
            'Frequency response is an ideal recurrence calculation at an explicitly assumed 48000 Hz.',
            'No claim about NaN/Inf, denormal flushing, all inputs, or end-to-end alias rejection.',
        ],
    )
    (ROOT / 'probes/antialias_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in (
        'status', 'output_comparisons', 'setter_assertions', 'dispatch_cases',
        'max_abs_error', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
