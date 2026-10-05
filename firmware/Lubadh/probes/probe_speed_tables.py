#!/usr/bin/env python3
"""Execute the callable preset_load speed generator and export complete tables."""
import hashlib
import json
import struct

from arm_byte_probe import ROOT
from arm_leaf_probe import f32
from preset_byte_probe import PresetBytes, EXPECTED, PLT
from speed_table_model import speed_table
from probe_v_oct_curve import curve

PRESET, SOURCE = 0x100000, 0x120000


def main():
    presets = json.loads((ROOT / 'tables/factory_presets.json').read_text())
    presets.update({
        'fixture-smooth': {'SpeedControl': 2, 'SpeedMarkers': [0., .5, 1., 2., 4.]},
        'fixture-overlapping-notches': {'SpeedControl': 0, 'SpeedMarkers': [0., .01, .025, .04, .1, .5, 1., 4.]},
        'fixture-duplicates-unsorted': {'SpeedControl': 0, 'SpeedMarkers': [2., .5, 0., 1., .5, 4., 2.]},
    })
    fixtures, coverage, comparisons, error_max = [], set(), 0, 0.
    for name, preset in presets.items():
        cpu = PresetBytes()
        cpu.putu(PRESET + 64, preset['SpeedControl'])
        markers = list(preset['SpeedMarkers']) + [0.] * (120 - len(preset['SpeedMarkers']))
        cpu.write(PRESET + 68, struct.pack('<120f', *markers))
        _, source = curve(0, 2457)
        cpu.write(SOURCE, struct.pack('<4096f', *source))
        cpu.reg(0, PRESET)
        cpu.reg(1, SOURCE)
        notches = []
        def capture(machine):
            begin, end = machine.reg(5), machine.reg(4)
            notches.extend(struct.unpack('<fiii', machine.read(a, 16)) for a in range(begin, end, 16))
        cpu.observers[0x15BCC] = capture
        cpu.call(0x157AC)
        table = cpu.floats(PRESET + 548, 4096)
        expected = speed_table(preset['SpeedMarkers'], preset['SpeedControl'], source)
        for i, (want, actual) in enumerate(zip(expected, table)):
            error = abs(want - actual)
            assert error <= 5e-7, (name, i, want, actual, error)
            error_max = max(error_max, error)
            comparisons += 1
        normalized_markers = cpu.floats(PRESET + 68, 120)
        coverage.update(cpu.coverage)
        fixtures.append(dict(preset=name, mode=preset['SpeedControl'], input_markers=preset['SpeedMarkers'],
                             normalized_markers=normalized_markers, table=table,
                             table_sha256=hashlib.sha256(struct.pack('<4096f', *table)).hexdigest(),
                             range=[min(table), max(table)], endpoints=[table[0], table[-1]],
                             center=table[2046:2050]))
        fixtures[-1]['notches_before_sort'] = notches
    result = dict(status='PASS', cases=len(fixtures), comparisons=comparisons, max_abs_error=error_max, preset_load_sha256=EXPECTED,
                  import_map={hex(a): n for a, n in PLT.items()},
                  distinct_instruction_addresses=len(coverage), coverage_addresses=[hex(a) for a in sorted(coverage)],
                  fixtures=fixtures, limitations=['Original complete fillSpeedTable only; no preset loader process or appliance launched.',
                                                'V/oct curve uses recovered law with supplied zeroV=0, threeV=2457 calibration, not a measured appliance calibration.',
                                                'Independent table construction compared; linked speed/control events remain unrecovered.'])
    (ROOT / 'probes/speed_table_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(status=result['status'], cases=len(fixtures), comparisons=comparisons, max_abs_error=error_max, coverage=len(coverage),
                         profiles=[{k: r[k] for k in ('preset', 'mode', 'range', 'endpoints', 'center')} for r in fixtures]), indent=2))


if __name__ == '__main__':
    main()
