#!/usr/bin/env python3
"""Compact offline Corrupt vectors. Uses the independently verified Python model.

No firmware execution required. Long-run packed audio SHA256 must match recorded
original-instruction evidence before any fixture is written. Runtime tests read
only the compact text; neither the models nor firmware ship with Tiamat.
"""
import argparse
import hashlib
import importlib.util
import math
import random
import struct
from generate_reference_fixtures import ROOT, ANALYSIS, load, bits

DEST = ROOT / 'tests/fixtures/tiamat/corrupt_v1.txt'
spec = importlib.util.spec_from_file_location('corrupt_reference', ANALYSIS / 'corrupt_dsp/reference_models.py')
model = importlib.util.module_from_spec(spec)
spec.loader.exec_module(model)


def words(values):
    return ' '.join(str(bits(v)) for v in values)


def hash_audio(values):
    h = 14695981039346656037
    for b in struct.pack('<%df' % len(values), *values):
        h = ((h ^ b) * 1099511628211) & ((1 << 64) - 1)
    return h


def generate():
    evidence = load('corrupt_dsp/differential_verification.json')
    long = load('character/vinyl_long_probes.json')
    rows = ['# Tiamat Corrupt fixtures v1', '# firmware_sha256 ' + long['firmware_sha256'],
            '# SHORT: 64 cases, 128 stereo frames; seed147 Python random input, seed1 Vinyl, coefficientRate96028.',
            '# Expected samples: independent float32 model, verified against original instructions including original libm.',
            '# Existing 12 original short vectors cross-checked below. DJ only permits 2e-5 amplitude difference.',
            '# LONG: three 288096-frame silence runs, 96-frame blocks; all packed audio SHA256 checked against original.',
            '# Each block stores FNV1a64 of little-endian audio words plus exact stochastic state (model).',
            '# RARE: five original 31-frame forced dust events, input=(.2,-.1); injected LCG and counters.',
            '# DROPOUT: seven original cases with injected rand; MIX: original dispatcher, Buffer/Corrupt/Tone stubbed.',
            '# No integrated callback or board emulation; runtime has no Python/firmware dependency.']
    rng = random.Random(147)
    samples = [model.F(rng.uniform(-.8, .8)) for _ in range(256)]
    rows.append('INPUT ' + words(samples))
    for case in evidence['cases']:
        mode, u = case['mode'], model.F(case['amount'])
        if mode == 1:
            refs = [model.Decimator(), model.Decimator()]
            for ref in refs: ref.configure(u)
            outputs = [refs[i % 2].process(x) for i, x in enumerate(samples)]
        else:
            ref = {3: model.Destroy, 4: lambda: model.DjFilter(96028), 5: lambda: model.Vinyl(96028)}[mode]()
            ref.configure(u)
            outputs = [y for i in range(0, 256, 2) for y in ref.process(*samples[i:i+2])]
        assert case['pass_tolerance'] and (case['bit_exact'] or mode == 4)
        for v in evidence['vectors']:
            if v['mode'] == mode and v['amount'] == case['amount']:
                assert words(samples[:32]) == words(v['inputs'])
                if mode != 4: assert words(outputs[:32]) == words(v['outputs'])
                else: assert max(abs(a-b) for a,b in zip(outputs, v['outputs'])) < 2e-5
        rows.append(f'SHORT {mode} {bits(u)} ' + words(outputs))
    for case in long['cases']:
        ref = model.Vinyl(96028)
        sha = hashlib.sha256()
        rows.append(f"LONG {bits(case['amount'])} {int(case['pause'])} {case['frames'] // 96}")
        for start in range(0, case['frames'], 96):
            u = .05 if case['pause'] and 47904 <= start < 48096 else case['amount']
            ref.configure(model.F(u))
            outputs = [y for _ in range(96) for y in ref.process(0., 0.)]
            sha.update(struct.pack('<192f', *outputs))
            rows.append('BLOCK ' + ' '.join(map(str, [hash_audio(outputs), ref.lcg,
                ref.slow_seed & 0xffffffff, ref.fast_seed & 0xffffffff, ref.slow_counter, ref.mod_counter,
                bits(ref.modulation), bits(ref.slow_value), *[d.counter for d in ref.dust], *[bits(d.value) for d in ref.dust]])))
        assert sha.hexdigest() == case['audio_float32_sha256'], 'Long oracle no longer matches original audio'
    for c in load('character/vinyl_rare_probes.json')['cases']:
        rows.append(f"RARE {c['layer']} {c['injected_lcg_state']} {len(c['samples'])}")
        for s in c['samples']: rows.append('SAMPLE ' + words(s['output'] + s['dust']))
    for c in load('corrupt_dsp/dropout_emulation.json'):
        rows.append('DROPOUT ' + ' '.join(map(str, [bits(c['amount']), int(c['initial_gate']), c['rand'][0],
            int(c['final_gate']), len(c['calls']), len(c['inputs'])//2])) + ' ' + words(c['inputs'] + c['outputs']))
    for c in load('output_probe.json')['mix_width']:
        rows.append('MIX ' + words([c['mix'], c['width_crossfeed'], c['smoothed_mix'], *c['actual']]))
    return '\n'.join(rows) + '\n'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    expected = generate()
    if args.check:
        if not DEST.exists() or DEST.read_text(encoding='utf-8') != expected:
            raise SystemExit('Tiamat Corrupt fixtures stale; regenerate.')
        print('Tiamat Corrupt fixtures current; all three original long audio hashes match.')
    else:
        DEST.write_text(expected, encoding='utf-8', newline='\n')
        print(DEST)
