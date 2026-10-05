#!/usr/bin/env python3
"""Cross-check the restricted text interpreter against actual-byte emulation.

This is independent instruction decoding/execution, not an additional hardware
measurement. Both paths still use explicit object fixtures and finite inputs.
"""
import json
import math
import random

import unicorn
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import ARMLeaf, f32

OBJ, VEC, DATA = 0x100000, 0x110000, 0x120000


def main():
    rng = random.Random(210)
    groups, coverage = {}, set()

    def compare(name, original, restricted):
        assert len(original) == len(restricted)
        group = groups.setdefault(name, dict(comparisons=0, max_abs_error=0., status='PASS'))
        for expected, observed in zip(original, restricted):
            error = abs(expected - observed)
            assert math.isfinite(observed) and error <= 2e-7, (name, expected, observed, error)
            group['comparisons'] += 1
            group['max_abs_error'] = max(group['max_abs_error'], error)

    def pair(values):
        byte, text = ARMBytes(), ARMLeaf()
        byte.vector(VEC, DATA, values)
        for i, value in enumerate(values):
            text.putf(DATA + 4 * i, value)
        for i, value in enumerate((DATA, DATA + 4 * len(values), DATA + 4 * len(values))):
            text.putu(VEC + 4 * i, value)
        return byte, text

    def process(byte, text, address, values, name):
        byte.reg(0, OBJ)
        byte.reg(1, VEC)
        byte.call(address)
        text.r[0], text.r[1] = OBJ, VEC
        text.call(address)
        compare(name, byte.floats(DATA, len(values)),
                [text.getf(DATA + 4 * i) for i in range(len(values))])
        coverage.update(byte.coverage)

    values = [f32(value) for value in [-4, -1, -.99, -.5, -.1, 0, .1, .5, .99, 1, 4]
              + [rng.uniform(-3, 3) for _ in range(129)]]
    for knee in (0., .0099, .01, .0101, .1, .3, .9, 1.):
        for compensation in (0., .2, 1.):
            byte, text = pair(values)
            byte.reg(0, OBJ)
            byte.fp(0, knee)
            byte.fp(1, compensation)
            byte.call(0x4FFE0)
            text.r[0] = OBJ
            text.putfp('s0', knee)
            text.putfp('s1', compensation)
            text.call(0x4FFE0)
            assert byte.read(OBJ, 12) == bytes(text.mem[OBJ:OBJ + 12])
            process(byte, text, 0x4FEA8, values, 'SoftClipper')

    for amount in (0., .125, .5, 1.):
        byte, text = pair(values)
        byte.putf(OBJ, amount)
        text.putf(OBJ, amount)
        process(byte, text, 0x4FC60, values, 'TapeCompander')

    impulse = [1.] + [0.] * 95
    for coefficient in (.25, 1., 5., 100.):
        for address, name in ((0x4B0B0, 'OnePoleLP'), (0x4B104, 'OnePoleHP')):
            byte, text = pair(impulse)
            byte.putf(OBJ, coefficient)
            text.putf(OBJ, coefficient)
            process(byte, text, address, impulse, name)

    for cutoff in (20., 40., 200., 5000., 10000., 20000.):
        byte, text = pair(impulse)
        byte.reg(0, OBJ)
        byte.fp(0, cutoff)
        byte.call(0x4B1C0)
        text.r[0] = OBJ
        text.putfp('s0', cutoff)
        text.call(0x4B1C0)
        assert byte.read(OBJ, 24) == bytes(text.mem[OBJ:OBJ + 24])
        process(byte, text, 0x4B1F0, impulse, 'AntiAlias')

    for coefficient in (0., .1, .5, 1.):
        byte, text = pair([])
        byte.putf(OBJ, coefficient)
        text.putf(OBJ, coefficient)
        for raw in (0, 4095, 0, 2048, 4095, 4095, 1):
            byte.reg(0, OBJ)
            byte.reg(1, raw)
            byte.call(0x6C104)
            text.r[0], text.r[1] = OBJ, raw
            text.call(0x6C104)
            compare('ADC', [byte.reg(0)], [text.r[0]])
        coverage.update(byte.coverage)

    samples = [f32(rng.uniform(-1, 1)) for _ in range(64)]
    for position in (-1., -.125, 0., .125, .5, .875, 1.25, 7.333, 17.9, 33.2, 60.):
        byte, text = pair(samples)
        byte.vector(OBJ + 4, DATA, samples)
        for i, pointer in enumerate((DATA, DATA + 4 * len(samples), DATA + 4 * len(samples))):
            text.putu(OBJ + 4 + 4 * i, pointer)
        byte.reg(0, OBJ)
        byte.fp(0, position)
        byte.call(0x38AB4)
        text.r[0] = OBJ
        text.putfp('s0', position)
        text.call(0x38AB4)
        compare('InputBufferCubic', [byte.fp(0)], [text.fpvalue('s0')])
        coverage.update(byte.coverage)

    byte, text = pair([1.] + [0.] * 1199)
    for cpu in (byte, text):
        cpu.putf(OBJ, .7)
        for i, length in enumerate((68, 159, 251, 375)):
            cpu.putu(OBJ + 0x1794 + 4 * i, length)
    process(byte, text, 0x4FCE8, [1.] + [0.] * 1199, 'TapeDiffuser')
    result = dict(status='PASS', unicorn_version=unicorn.__version__,
                  method='Restricted LLVM text interpreter versus Unicorn execution of actual ELF instruction bytes',
                  comparisons=sum(group['comparisons'] for group in groups.values()), groups=groups,
                  distinct_byte_instruction_addresses=len(coverage),
                  limitations=['Not a hardware comparison or full firmware validation.',
                               'Finite fixtures; no complete FPSCR/denormal/NaN/Inf coverage.'])
    (ROOT / 'probes/interpreter_crosscheck_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
