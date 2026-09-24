#!/usr/bin/env python3
"""Compare the Rack-independent C++ profile with the authored JSON anchors."""
from __future__ import annotations

import argparse
import json
import math
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
VECTORS = ROOT / 'doc/Morphagene-Codex/reference_vectors.json'
IDS = ROOT / 'tests/chimera_ids_v1.json'
TYPES = ROOT / 'src/ChimeraTypes.hpp'


def check_ids() -> None:
    fixture = json.loads(IDS.read_text(encoding='utf-8'))
    header = TYPES.read_text(encoding='utf-8')
    for category, enumeration, sentinel in (
        ('parameters', 'ParamId', 'NUM_PARAMS'),
        ('inputs', 'InputId', 'NUM_INPUTS'),
        ('outputs', 'OutputId', 'NUM_OUTPUTS'),
        ('lights', 'LightId', 'NUM_LIGHTS'),
    ):
        match = re.search(r'enum\s+' + enumeration + r'\s*\{([^}]*)\}', header, re.S)
        assert match, f'missing {enumeration}'
        names = re.findall(r'\b[A-Z][A-Z0-9_]+\b', match.group(1))
        assert names == fixture[category] + [sentinel], f'{enumeration} differs from frozen IDs'


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('executable', type=Path)
    args = parser.parse_args()
    data = json.loads(VECTORS.read_text(encoding='utf-8'))
    check_ids()
    cases: list[tuple[str, list[float]]] = []

    def add(line: str, *expected: float) -> None:
        cases.append((line, list(expected)))

    for v in data['classicRate']:
        add(f"classic {v['knob']} {v['attenuverter']} {v['cvVolts']}", v['rate'])
    for v in data['pitchRate']:
        add(f"pitch {v['mode']} {v['knob']} {v['attenuverter']} {v['cvVolts']}", v['rate'])
    for v in data['sos']:
        add(f"sos {v['knob']} {int(v['patched'])} {v['cvVolts']}", v['mix'])
    for v in data['finiteGene']:
        add(f"gene {v['spliceFrames']} {v['normalizedControl']}", v['frames'])
    for v in data['morphDensity']:
        add(f"density {v['morph']}", v['density'], v['hopFor480FrameGene'])
    for v in data['cubic']:
        add('cubic ' + ' '.join(map(str, [*v['taps'], v['fraction']])), v['value'])
    for v in data['windows']:
        for sample in v['samples']:
            add(f"window {v['frames']} {int(v['smooth'])} {sample['age']}",
                v['edgeFrames'], sample['weight'])
    for v in data['unityEnvelope']:
        add(f"unity {v['density']} {v['baseWindow']} {int(v['smooth'])}",
            v['unityBlend'], v['effectiveWindow'])
    state = data['prng']['seed']
    for v in data['prng']['draws']:
        add(f'prng {state}', v['uint32'], v['uniform'])
        state = v['uint32']
    for v in data['stereoBalance']:
        add(f"pan {v['pan']}", v['gainL'], v['gainR'])
    for v in data['onePole']:
        add(f"pole {v['tauSeconds']}", v['alphaAt48k'])

    input_text = '\n'.join(case[0] for case in cases) + '\n'
    result = subprocess.run([str(args.executable.resolve())], input=input_text,
                            text=True, capture_output=True, check=True)
    actual_lines = result.stdout.splitlines()
    assert len(actual_lines) == len(cases), f'expected {len(cases)} responses, got {len(actual_lines)}'
    for i, ((line, expected), response) in enumerate(zip(cases, actual_lines)):
        actual = [float(value) for value in response.split()]
        assert len(actual) == len(expected), f'case {i} {line}: wrong arity'
        for observed, wanted in zip(actual, expected):
            assert math.isfinite(observed) and abs(observed - wanted) <= 1e-6, (
                f'case {i} {line}: got {observed}, expected {wanted}')
    print(f'PASS: Chimera frozen IDs and {len(cases)} profile-1 JSON vector evaluations')


if __name__ == '__main__':
    main()
