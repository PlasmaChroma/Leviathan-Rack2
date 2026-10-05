#!/usr/bin/env python3
"""Execute the original calibrated V/oct arithmetic, bypassing file I/O."""
import ctypes
import json
import math
import struct

from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR
from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from preset_byte_probe import PresetBytes, EXPECTED, PLT

libm.powf.argtypes = [ctypes.c_float, ctypes.c_float]
libm.powf.restype = ctypes.c_float
assert PLT[0x1392C] == 'powf'
TABLE = 0x120000


class CurveBytes(PresetBytes):
    def _code(self, cpu, address, size, user):
        if address == 0x1392C:
            self.coverage.add(address)
            self.fp(0, libm.powf(self.fp(0), self.fp(1)))
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, user)


def curve(zero, three):
    spacing = f32(f32(f32(three - zero) * 819.) / 2457.)
    output = []
    for i in range(4096):
        if i < math.trunc(spacing):
            value = f32(f32(f32(i) * .25) / spacing)
        else:
            exponent = f32(f32(f32(i) / spacing) - 1.)
            value = max(0., min(4., f32(libm.powf(2., exponent) * .25)))
        output.append(value)
    return spacing, output


def main():
    fixtures, coverage, comparisons = [], set(), 0
    # Explicit calibration fixtures, not appliance calibration measurements.
    for zero, three in [(0, 2457), (17, 2450), (64, 2500), (128, 2700), (0, 3000)]:
        cpu = CurveBytes()
        cpu.reg(9, zero)
        cpu.reg(4, three)
        cpu.reg(5, TABLE)
        cpu.call(0x162B0, stop_before=0x1636C)
        actual = cpu.floats(TABLE, 4096)
        spacing, expected = curve(zero, three)
        assert actual == expected
        comparisons += len(actual)
        coverage.update(cpu.coverage)
        fixtures.append(dict(zeroV=zero, threeV=three, counts_per_octave=spacing,
                             table=actual, endpoints=[actual[0], actual[-1]],
                             linear_to_exponential_index=math.trunc(spacing),
                             first_saturated_index=next((i for i, x in enumerate(actual) if x == 4.), None)))
    result = dict(status='PASS', cases=len(fixtures), comparisons=comparisons,
                  max_abs_error=0., preset_load_sha256=EXPECTED,
                  slice=['0x162b0', '0x1636c exclusive'],
                  distinct_instruction_addresses=len(coverage), fixtures=fixtures,
                  limitations=['Calibration Hjson parser and exception paths bypassed.',
                               'Explicit host glibc powf service, not archived ARM libm.',
                               'Calibration pairs are supplied fixtures; actual appliance calibration file is absent.'])
    (ROOT / 'probes/v_oct_curve_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k not in ('fixtures', 'limitations')}, indent=2))


if __name__ == '__main__':
    main()
