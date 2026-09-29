#!/usr/bin/env python3
"""Verify this handoff, not a future Rack implementation or physical TEMPI.

Only Python's standard library is needed. Source models and original fixtures
are intentionally retained unchanged. New policy examples are labeled as such.
"""
from __future__ import annotations

import csv
from dataclasses import asdict
from fractions import Fraction
import hashlib
import json
import math
from pathlib import Path
import re
import sys
from typing import Any

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'reference'))
import models  # noqa: E402
import timing_models  # noqa: E402


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def rows(name: str) -> list[dict[str, str]]:
    with (ROOT / 'fixtures' / name).open(newline='', encoding='utf-8') as stream:
        return list(csv.DictReader(stream))


def load(name: str) -> Any:
    return json.loads((ROOT / 'fixtures' / name).read_text(encoding='utf-8'))


def frequency(r: int) -> Fraction:
    return Fraction(r + 4, 4) if r >= 0 else Fraction(4, 4 - r)


def check() -> dict[str, Any]:
    counts: dict[str, int] = {}
    manifest = ROOT / 'PACKAGE_SHA256SUMS.txt'
    require(manifest.is_file(), 'Missing PACKAGE_SHA256SUMS.txt')
    for line in manifest.read_text(encoding='utf-8').splitlines():
        expected, relative = line.split('  ', 1)
        path = ROOT / relative
        require(path.is_file(), f'Missing packaged file: {relative}')
        require(hashlib.sha256(path.read_bytes()).hexdigest() == expected,
                f'Checksum mismatch: {relative}')
    counts['packaged_files_hashed'] = len(manifest.read_text().splitlines())

    for row in rows('ratio_test_vectors.csv'):
        r, h = int(row['code']), int(row['master_halfperiod_ticks'])
        actual = models.ratio_halfperiod(r, h)
        require(actual == int(row['computed_halfperiod_ticks']), f'Ratio mismatch: {row}')
    counts['ratio_fixture_rows'] = len(rows('ratio_test_vectors.csv'))

    for row in rows('phase_test_vectors.csv'):
        actual = models.phase_offset(int(row['code']), int(row['phase_byte']),
                                     int(row['master_halfperiod_ticks']))
        require(actual == int(row['offset_ticks']), f'Phase mismatch: {row}')
    counts['phase_fixture_rows'] = len(rows('phase_test_vectors.csv'))

    for row in rows('ratio_alignment_factors.csv'):
        require(frequency(int(row['ratio_code'])).denominator == int(row['master_cycles_to_alignment']),
                f'Alignment mismatch: {row}')
    counts['alignment_fixture_rows'] = len(rows('ratio_alignment_factors.csv'))

    for row in rows('random_test_vectors.csv'):
        state, byte = models.next_random(int(row['seed'], 16))
        require(state == int(row['next_state'], 16), f'PRNG state mismatch: {row}')
        require(byte == int(row['returned_u16']) and 0 <= byte <= 255,
                f'PRNG byte mismatch: {row}')
    counts['prng_fixture_rows'] = len(rows('random_test_vectors.csv'))

    factory = load('factory_states.json')
    table = rows('factory_states.csv')
    require(len(factory) == len(table) == 64, 'Factory table length')
    for i, (value, row) in enumerate(zip(factory, table)):
        require(value['state'] == i and value['bank'] == i // 16 and value['slot'] == i % 16,
                f'Factory index mismatch: {i}')
        require(value['ratio_codes'] == [int(row[f'ratio_ch{c}']) for c in range(1, 7)],
                f'Factory ratio mismatch: {i}')
        require(value['phase_codes'] == [int(row[f'phase_ch{c}']) for c in range(1, 7)],
                f'Factory phase mismatch: {i}')
        require(value['enable_mask'] == int(row['enable_mask']) == 63,
                f'Factory enable mask mismatch: {i}')
        require(value['mod_mask'] == int(row['mod_mask']) == 0,
                f'Factory MOD mask mismatch: {i}')
    counts['factory_states_checked'] = 64

    # Original address fixture can be checked without shipping EEPROM bytes.
    ee_rows = rows('state_eeprom_address_tests.csv')
    eeprom = bytearray(1024)
    for row in ee_rows:
        eeprom[int(row['eeprom_address'], 16)] = int(row['value'])
    by_state: dict[int, dict[int, int]] = {}
    for row in ee_rows:
        by_state.setdefault(int(row['state']), {})[int(row['eeprom_address'], 16)] = int(row['value'])
    for s, addresses in by_state.items():
        decoded = models.decode_state(eeprom, s)
        require(len(addresses) == 14, f'State {s} needs fourteen byte addresses')
        for c in range(6):
            raw = addresses[64*c+s]
            require(decoded['ratio_codes'][c] == (raw if raw < 128 else raw-256),
                    f'EEPROM ratio addressing {s}/{c}')
            require(decoded['phase_codes'][c] == addresses[0x180+64*c+s],
                    f'EEPROM phase addressing {s}/{c}')
        require(decoded['enable_mask'] == addresses[0x300+s], f'EEPROM enable {s}')
        require(decoded['mod_mask'] == addresses[0x340+s], f'EEPROM MOD {s}')
    counts['eeprom_address_cases'] = len(by_state)

    require(tuple(int(row['integer_value']) for row in rows('tempo_control_table.csv')) ==
            timing_models.TEMPO_TABLE, 'Tempo table mismatch')
    timing = load('timing_validation.json')
    for ex in timing['leading_examples']:
        actual = asdict(timing_models.leading_capture_service(timing_models.LeadingState(**ex['input'])))
        require(actual == ex['output'], f'Leading example mismatch: {ex}')
    counts['bundled_leading_examples_rechecked'] = len(timing['leading_examples'])
    for ex in timing['tempo_control_examples']:
        actual = timing_models.tempo_control(ex['adc'], ex['previous'], 8000, ex['thresholds'])
        require(actual == (ex['period'], ex['retained_adc'], ex['moved']),
                f'Tempo example mismatch: {ex}')
    counts['bundled_tempo_examples_rechecked'] = len(timing['tempo_control_examples'])

    thresholds = [32+64*j for j in range(16)]
    for previous in range(16):
        for adc in range(1024):
            q = models.adc_state(adc, previous, thresholds)
            require(isinstance(q, int) and 0 <= q <= 15, 'ADC output outside slot range')
        require(models.adc_state(65535, previous, thresholds) == previous,
                'ADC sentinel did not retain State')
    # This is a bounds/sentinel check, not an independent full-function oracle.
    counts['adc_bounds_cases'] = 16*1024
    counts['adc_sentinel_cases'] = 16

    data = load('default_patch_v1.json')
    require(data['schemaVersion'] == 1 and data['policyVersion'] == 'TempiPoliciesV1',
            'Default payload schema/policy')
    require(data['virtualTickHz'] == 32000, 'Default payload tick rate')
    for layer in ('savedStates', 'workingStates'):
        require(len(data[layer]) == 64, f'Default {layer} length')
        for i, program in enumerate(data[layer]):
            original = factory[i]
            require(program == dict(ratio=original['ratio_codes'], phase=original['phase_codes'],
                                    enableMask=original['enable_mask'], modMask=original['mod_mask']),
                    f'Default payload differs from factory: {layer}/{i}')
    require(data['savedGlobals'] == data['workingGlobals'], 'Default global layers differ')
    require(data['savedLeadingH'] == data['session']['currentLeadingH'] == 8000, 'Default H')
    require(len(data['session']['rngStateHex']) == len(data['session']['meshHex']) == 16,
            'Default hex width')
    counts['default_payload_programs_checked'] = 128

    policy = load('spec_policy_vectors.json')
    edge = policy['rationalHalfEdges']
    rate = frequency(edge['ratioCode'])
    actual_edges = [math.ceil(Fraction(j*edge['masterH']*rate.denominator, rate.numerator))
                    for j in range(7)]
    require(actual_edges == edge['ticks'], 'Rational policy example')
    ring = [0,1,2,5,4,3]
    mapping = list(range(6))
    shifted = mapping.copy()
    for pos, dest in enumerate(ring):
        shifted[dest] = mapping[ring[(pos-1) % len(ring)]]
    require([x+1 for x in shifted] == policy['clockwiseAllEligible']['sourceOneBased'],
            'Clockwise policy example')
    phase = policy['phaseFineEarlier']
    w = 4-phase['ratioCode'] if phase['ratioCode'] < 0 else 4
    require((phase['phaseByteBefore']-1) % w == phase['phaseByteAfter'], 'Fine phase example')
    mutation = policy['mutationFromUnity']
    seed = int(mutation['seedHex'], 16)
    for c, expected in enumerate(mutation['channels']):
        seed, a = models.next_random(seed)
        seed, b = models.next_random(seed)
        r = max(-124, min(124, (a % 7)-3))
        w = 4-r if r < 0 else 4
        p = ((b % 5)-2) % w
        require(expected == dict(channel=c,ratioDraw=a,phaseDraw=b,ratio=r,phase=p),
                f'Mutation policy example {c}')
    require(seed == int(mutation['finalSeedHex'], 16) and mutation['drawCount'] == 12,
            'Mutation final state/draw count')
    combo = policy['stateCombo']
    require(math.floor(combo['volts']/5*combo['knob']*1023+0.5) == combo['adc'],
            'Combo attenuation example')
    counts['software_policy_example_groups_checked'] = 5

    spec = (ROOT/'TEMPI_VCV_RACK_CODEX_SPEC.md').read_text(encoding='utf-8')
    fence_count = sum(1 for line in spec.splitlines() if line.startswith('```'))
    require(fence_count % 2 == 0, 'Unbalanced Markdown code fences')
    ids = re.findall(r'^\| ([A-Z]+-\d+) \|', spec, flags=re.M)
    require(len(ids) == len(set(ids)), 'Duplicate acceptance-test IDs')
    require(len(ids) >= 130, 'Unexpectedly incomplete acceptance-test inventory')
    require('Rack patch save is not TEMPI Store' in spec, 'Missing core memory invariant')
    counts['unique_acceptance_test_ids'] = len(ids)
    counts['markdown_code_fences'] = fence_count

    # Check the main spec's literal factory table independently of its prose.
    factory_section = spec.split('### 10.3 Factory table [F]', 1)[1].split('### 10.4', 1)[0]
    parsed = re.findall(r'^\| (\d+) \| `([^`]+)` \| `([^`]+)` \|$',factory_section, flags=re.M)
    require(len(parsed) == 16, 'Main specification factory-table rows')
    for number, ratios, phases in parsed:
        index = int(number)-1
        require([int(x.strip()) for x in ratios.split(',')] == factory[index]['ratio_codes'],
                f'Spec factory ratios differ: {number}')
        require([int(x.strip()) for x in phases.split(',')] == factory[index]['phase_codes'],
                f'Spec factory phases differ: {number}')
    counts['spec_factory_rows_checked'] = 16

    return dict(status='PASS', scope='Handoff integrity and reference consistency only', counts=counts,
                not_tested=['Future C++ implementation','Rack plugin build or UI',
                            'Full fresh original timing-harness run','Physical hardware'])


if __name__ == '__main__':
    try:
        print(json.dumps(check(), indent=2))
    except (OSError, ValueError, KeyError, TypeError, IndexError) as exc:
        print(f'Handoff check failed: {exc}', file=sys.stderr)
        raise SystemExit(1)
