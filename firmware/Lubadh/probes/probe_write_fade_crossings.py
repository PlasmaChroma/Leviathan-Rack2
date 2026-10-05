#!/usr/bin/env python3
"""Original recordInput crossing suffixes against independent finite-prefix model."""
import itertools
import json
import math
import struct
from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from probe_transport import setup, move, record, CHANNEL, TAP, TAPE, TAPE_LENGTH
from probe_splice_render import TABLE
from probe_playback_pipeline import rounded_cubic


def main():
    cases, checks, coverage, error_max, examples = 0, 0, set(), 0., []
    endpoints = [(previous, current) for previous, current in itertools.product((2., 4., 16.), (-.25, -.75, -1., -2.))]
    endpoints += [(previous, current) for previous, current in itertools.product((252., 253., 254.), (255., 255.25, 256., 258.))]
    for (previous, requested), speed, frames, feedback in itertools.product(
            endpoints, (-2., -1., -.5, .5, 1., 2.), (32, 128), (0., .5, 1.)):
        c = setup()
        c.puts(TAP + 4, 700)
        c.putf(TAP + 8, .375)
        move(c, speed, frames=frames)
        count = int(abs(speed) * frames)
        fade = CHANNEL + 6504
        step = f32((requested - previous) / frames)
        c.write(fade, b'\1')
        c.puts(fade + 4, math.floor(previous))
        c.putf(fade + 8, previous - math.floor(previous))
        c.putf(fade + 20, step)
        c.reg(0, fade)
        c.fp(0, frames)
        c.call(0x4BAFC)
        total = libm.fmaf(step, f32(frames), f32(previous - math.floor(previous)))
        current_integer = math.floor(previous) + math.floor(total)
        current_fraction = f32(total - math.floor(total))
        current = f32(current_integer + current_fraction)
        assert c.gets(fade + 4) == current_integer and c.getf(fade + 8) == current_fraction
        delta = f32(current - f32(previous))
        envelope_step = f32(delta / f32(count))
        magnitude_step = f32(abs(delta) / f32(count))
        outside = f32(-current) if current_integer < 0 else f32(current - 254.)
        removed = math.trunc(f32(outside / magnitude_step)) + 1
        removed = min(removed, count)
        prefix = count - removed if removed >= 0 else count
        assert 0 <= prefix <= count
        tape = [f32(.1 * math.sin(i * .17)) for i in range(TAPE_LENGTH)]
        c.write(TAPE, struct.pack('<2048f', *tape))
        samples = [f32(.07 * math.cos(i * .071)) for i in range(frames + 4)]
        expected = list(tape)
        position, fraction, phase = math.floor(previous), f32(previous - math.floor(previous)), .125
        inverse_speed = f32(1. / abs(speed))
        direction = 1 if speed > 0 else -1
        for i in range(count):
            if i < prefix:
                assert 0 <= position <= 254, 'Fixture prefix exceeds supported table interpolation'
                gain = libm.fmaf(fraction, f32(TABLE[position+1] - TABLE[position]), TABLE[position])
                rev = libm.fmaf(f32(1. - fraction), f32(TABLE[254-position] - TABLE[255-position]), TABLE[255-position])
                retention = libm.fmaf(f32(1. - feedback), rev, feedback)
                fraction = f32(fraction + envelope_step)
                whole = math.floor(fraction)
                position += whole
                fraction = f32(fraction - whole)
            elif current_integer < 0:
                gain, retention = 0., 1.
            else:
                gain, retention = 1., feedback
            source = max(0, min(len(samples)-4, math.trunc(phase)))
            value = rounded_cubic(*samples[source:source+4], f32(phase-source))
            index = 700 + direction * i
            expected[index] = max(-1., min(1., libm.fmaf(tape[index], retention, f32(value * gain))))
            phase = f32(phase + inverse_speed)
        phase = f32(phase - math.trunc(phase))
        observed = record(c, speed, samples, feedback=feedback, phase=.125)
        assert observed['count'] == count and observed['input_phase'] == phase
        for a, b in zip(expected, observed['tape']):
            error = abs(a-b)
            assert math.isfinite(b) and error <= 3e-7, (previous, requested, speed, frames, feedback, prefix, a, b)
            error_max = max(error_max, error)
            checks += 1
        cases += 1
        checks += 3
        coverage.update(c.coverage)
        if speed == 1. and frames == 128 and feedback == .5 and previous in (2., 254.):
            examples.append(dict(previous=previous, current=current, writes=count, interpolated_prefix=prefix, suffix=count-prefix))
    result = dict(status='PASS', cases=cases, comparisons=checks, max_abs_error=error_max,
                  distinct_instruction_addresses=len(coverage), examples=examples,
                  limitations=['Original Fade.move and complete recordInput; explicit sequencing rather than full callback expiry.',
                               'Initialized Tap, transparent clipper, reconstructed fade table, supplied input and tape; prefixes kept within table bounds.',
                               'Stationary out-of-range, arbitrarily large prefix overshoots, event production and manager lifecycle outside this suite.'])
    (ROOT / 'probes/write_fade_crossing_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key:value for key,value in result.items() if key != 'examples'}, indent=2))


if __name__ == '__main__':
    main()
