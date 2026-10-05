#!/usr/bin/env python3
"""Original reverb-control publication and independent modulation tables."""
import hashlib
import itertools
import json
import math

from arm_byte_probe import ROOT
from arm_leaf_probe import f32, libm
from probe_plate_process import initialized
from probe_plate_initialization import CHANNEL, PLATE
from plate_model import DRY, WET, DECAY, MODULATORS, modulation_table

LINK, PRESET, MIRROR = 0x140000, 0x150000, 0x210000


def main():
    cpu = initialized()
    reconstructed = modulation_table()
    # Read the actual constructor-generated tables, not the reconstructed
    # parameters supplied to the independent process model.
    tables = {o: cpu.floats(PLATE + o + 36, 128) for o in MODULATORS}
    assert all(t == reconstructed for t in tables.values())
    assertions = 256
    cpu.putu(CHANNEL + 0xE8, LINK)
    cpu.putu(LINK + 0xB0, PRESET)
    cpu.putu(CHANNEL + 171868, MIRROR)
    rows = []
    for amount, preset in itertools.product((-.25, 0., .125, .25, .449, .45, .5, .75, 1., 1.25),
                                           (-.5, 0., .125, .5, 1., 2.)):
        cpu.putf(LINK + 168, amount)
        cpu.putf(PRESET + 16384 + 664, preset)
        cpu.reg(4, CHANNEL)
        cpu.call(0x37CE8, stop_before=0x37D54)
        u = max(0., min(1., f32(f32(amount) * f32(preset))))
        complement = f32(1. - u)
        norm = f32(math.sqrt(libm.fmaf(u, u, f32(complement * complement))))
        expected = {DRY: f32(complement / norm), WET: f32(u / norm), DECAY: min(f32(2. * u), f32(.9))}
        actual = {o: cpu.getf(PLATE + o) for o in expected}
        assert expected == actual, (amount, preset, u, expected, actual)
        assert cpu.getf(MIRROR + 40) == f32(amount)
        assertions += 4
        rows.append(dict(amount=amount, preset=preset, clamped=u, dry=actual[DRY], wet=actual[WET], decay=actual[DECAY]))
    result = dict(status='PASS', cases=len(rows), assertions=assertions,
                  modulation_table=reconstructed, fixtures=rows,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(cpu.coverage), coverage_addresses=[hex(a) for a in sorted(cpu.coverage)],
                  limitations=['Original setTime reverb sub-slice with supplied Time amount and preset fields.',
                               'No complete Time gesture/ADC/CV event or linked publication comparison.',
                               'Host sqrt of nonnegative finite float32 control expression; no hardware libm equivalence claim.'])
    (ROOT / 'probes/plate_control_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'cases', 'assertions', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
