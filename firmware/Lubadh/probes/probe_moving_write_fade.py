#!/usr/bin/env python3
"""Original channel write-fade motion and record envelope, in-table fixtures."""
import hashlib
import itertools
import json
import math
import struct

from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from probe_playback_pipeline import rounded_cubic
from probe_transport import setup, move, record, CHANNEL, TAP, TAPE, TAPE_LENGTH
from probe_splice_render import TABLE


def main():
    rows, coverage, comparisons, error_max = [], set(), 0, 0.
    for previous, delta, speed, frames, feedback in itertools.product(
            (.25, 64.75, 128.5, 200.25), (-64., -8., -.05, 0., .05, 8., 32., 64.),
            (-2., -.3, .3, 2.), (7, 32, 128), (0., .5, 1.)):
        if not 0 <= previous + delta < 254:
            continue
        cpu = setup()
        cpu.puts(TAP + 4, 700)
        cpu.putf(TAP + 8, .375)
        move(cpu, speed, frames=frames)
        count = abs(cpu.gets(TAP + 4) - 700)
        fade = CHANNEL + 6504
        cpu.write(fade, b'\x01')
        cpu.puts(fade + 4, math.floor(previous))
        cpu.putf(fade + 8, previous - math.floor(previous))
        cpu.putf(fade + 20, f32(delta / frames))
        cpu.reg(0, fade)
        cpu.fp(0, frames)
        cpu.call(0x4BAFC)
        current = f32(cpu.gets(fade + 4) + cpu.getf(fade + 8))
        old = f32(cpu.gets(fade + 12) + cpu.getf(fade + 16))
        difference = f32(current - old)
        stationary = abs(difference) < f32(.1)
        position = cpu.gets(fade + (4 if stationary else 12))
        fraction = cpu.getf(fade + (8 if stationary else 16))
        step = f32(difference / count)
        tape = [f32(.2 * math.sin(i * .19)) for i in range(TAPE_LENGTH)]
        cpu.write(TAPE, struct.pack('<2048f', *tape))
        values = [f32(.15 * math.cos(i * .071)) for i in range(frames + 4)]
        phase = .125
        inverse = f32(1. / abs(f32(speed)))
        direction = 1 if speed > 0 else -1
        expected = list(tape)
        for i in range(count):
            incoming = libm.fmaf(fraction, f32(TABLE[position + 1] - TABLE[position]), TABLE[position])
            reverse = libm.fmaf(f32(1. - fraction), f32(TABLE[254 - position] - TABLE[255 - position]), TABLE[255 - position])
            retention = libm.fmaf(f32(1. - feedback), reverse, feedback)
            source = max(0, min(len(values) - 4, math.trunc(phase)))
            sample = rounded_cubic(*values[source:source + 4], f32(phase - source))
            index = 700 + direction * i
            expected[index] = max(-1., min(1., libm.fmaf(tape[index], retention, f32(sample * incoming))))
            phase = f32(phase + inverse)
            if not stationary:
                fraction = f32(fraction + step)
                whole = math.floor(fraction)
                position, fraction = position + whole, f32(fraction - whole)
        phase = f32(phase - math.trunc(phase))
        actual = record(cpu, speed, values, feedback=feedback, phase=.125)
        assert actual['count'] == count
        assert actual['input_phase'] == phase
        for want, observed in zip(expected, actual['tape']):
            error = abs(want - observed)
            assert math.isfinite(observed) and error <= 3e-7, (previous, delta, speed, frames, feedback, want, observed)
            error_max = max(error_max, error)
            comparisons += 1
        rows.append(dict(previous=old, current=current, requested_delta=delta,
                         stationary=stationary, speed=speed, frames=frames, feedback=feedback,
                         writes=count, final_phase=phase))
        coverage.update(cpu.coverage)
    result = dict(status='PASS', cases=len(rows), comparisons=comparisons,
                  max_abs_error=error_max, stationary_cases=sum(r['stationary'] for r in rows),
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(coverage), coverage_addresses=[hex(a) for a in sorted(coverage)],
                  fixtures=rows, limitations=[
                      'Explicit Fade::move and recordInput calls, not full callback or event-driven write-fade activation.',
                      'Both channel write-fade endpoints are inside [0,254); out-of-table prefix/suffix behavior remains open.',
                      'No additional tap fade, boundary splice, tail, control producer or output coloration.',
                      'Transparent knee=1 compensation=0 write clipper; reconstructed shared fade table.'])
    (ROOT / 'probes/moving_write_fade_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'cases', 'comparisons', 'max_abs_error', 'stationary_cases', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
