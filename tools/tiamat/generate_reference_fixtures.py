#!/usr/bin/env python3
"""Extract compact Phase 1 fixtures; stdlib only, never executes firmware.

Run from any directory. --check verifies the checked-in fixture byte for byte.
The C++ tests load this text directly; Python is only an offline build tool.
"""
import argparse
import json
import math
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[2]
ANALYSIS = ROOT / 'firmware/Data_Bender/analysis'
DEST = ROOT / 'tests/fixtures/tiamat/reference_v1.txt'


def load(name):
    return json.loads((ANALYSIS / name).read_text(encoding='utf-8'))


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def generate():
    rng = load('integration/reader_rng_probes.json')
    rows = [
        '# Tiamat Phase 1 fixtures v1',
        '# firmware_sha256 ' + rng['firmware_sha256'],
        '# RNG: original instructions; four seeds, 256 draws each; first 12 outputs and final state retained.',
        '# Tables: static IEEE754 words from static/lookup_tables.json; no substitutions.',
        '# MICRO: controls/controls_probe.json; prepared dispatcher state, zero audio frames;',
        '# expf/powf replaced by Python math in original probe; CV normalized there, volts here.',
        '# TIME: controls/clock_probe.json; isolated clock map, formula values; no full callback emulation.',
        '# TONE: spec section 8 + 0x08017acc; float32 offline equation oracle, not executed-firmware capture.',
        '# TONE uses float32 two-pi, multiply/divide, cos, fused b*b-1, sqrt; no Nyquist clamp.',
    ]
    for c in rng['rng']['cases']:
        rows.append('RNG ' + ' '.join(map(str, [c['seed'], c['draws_checked'], int(c['final_state'], 16), *c['first_12']])))
    tables = load('static/lookup_tables.json')
    for name in ('micro_octave_targets', 'external_clock_ratios', 'macro_bend_rate_table',
                 'macro_break_silence_table', 'decimate_bitcrush_table', 'decimate_downsample_table'):
        rows.append('TABLE ' + name + ' ' + ' '.join(str(int(w, 16)) for w in tables[name]['raw_words']))
    for c in load('controls/controls_probe.json'):
        m = c['mapped']
        rows.append('MICRO ' + ' '.join(map(str, [c['bend'], c['cv'] * 5, int(c['reverse']), m['speed'], c['octave_led']])))
    for c in load('controls/clock_probe.json')['time']:
        if not c['external']:
            rows.append('TIME ' + ' '.join(map(str, [c['x'], c['frequency_hz_formula'], c['ratio']])))
    angle = f32(f32(f32(2 * math.pi) * 38000) / 96028)
    b = f32(2 - f32(math.cos(angle)))
    c = f32(b - f32(math.sqrt(f32(float(b) * b - 1))))
    rows.append(f'TONE {bits(f32(1-c))} {bits(c)}')
    return '\n'.join(rows) + '\n'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    expected = generate()
    if args.check:
        if not DEST.exists() or DEST.read_text(encoding='utf-8') != expected:
            raise SystemExit('Tiamat reference fixtures are stale; regenerate them.')
        print('Tiamat reference fixtures are current.')
    else:
        DEST.parent.mkdir(parents=True, exist_ok=True)
        DEST.write_text(expected, encoding='utf-8', newline='\n')
        print(DEST)
