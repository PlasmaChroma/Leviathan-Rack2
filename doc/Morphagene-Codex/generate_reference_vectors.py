#!/usr/bin/env python3
"""Generate mathematical reference vectors for the proposed DSP profile.

Standard library only. This validates its own anchors, not a Rack implementation.
Run: python generate_reference_vectors.py --check reference_vectors.json
Run: python generate_reference_vectors.py --output reference_vectors.json
"""
from __future__ import annotations

import argparse
import json
import math
import struct
from pathlib import Path
from typing import Any

FS = 48_000
EPS = 0.0001
DEFAULT_SEED = 0x6D2B79F5


def clamp(value: float, lo: float, hi: float) -> float:
    return min(hi, max(lo, value))


def positive_round(value: float) -> int:
    """C++ std::round convention for the nonnegative values in this profile."""
    return math.floor(value + 0.5)


def classic_rate(knob: float, attenuverter: float = 0.0, volts: float = 0.0) -> float:
    coordinate = clamp(2.0 * knob - 1.0 + attenuverter * volts / 4.0, -1.0, 1.0)
    if abs(coordinate) <= EPS:
        return 0.0
    q = (abs(coordinate) - EPS) / (1.0 - EPS)
    unity_q = (2.0 / 3.0 - EPS) / (1.0 - EPS)
    gamma = math.log(0.5) / math.log(unity_q)
    return math.copysign(2.0 * q**gamma, coordinate)


def forward_base(knob: float) -> float:
    if knob <= EPS:
        return 0.0
    q = (knob - EPS) / (1.0 - EPS)
    unity_q = (0.75 - EPS) / (1.0 - EPS)
    return 2.0 * q ** (math.log(0.5) / math.log(unity_q))


def finite_gene_frames(length: int, control: float) -> int:
    if length < 1:
        raise ValueError('A finite Gene requires a nonempty Splice')
    f32 = lambda x: struct.unpack('f', struct.pack('f', x))[0]
    code = positive_round(f32(f32(clamp(control, 0.0, 1.0)) * 4095.0))
    if code <= 199:
        return length  # Whole-splice traversal bypasses duration folding.
    base = f32(length)
    while base > 576000.0:
        base *= 0.5
    lut_value = f32(2.0 ** f32(f32((1073 - (code >> 2)) / 341.0) - 3.0))
    result = f32(f32(f32(base * lut_value) * lut_value) * lut_value)
    return positive_round(max(8.0, result))


def density(morph: float) -> float:
    anchors = [(0.0, 0.9), (1 / 6, 1.0), (0.5, 2.0), (5 / 6, 3.0), (1.0, 4.0)]
    morph = clamp(morph, 0.0, 1.0)
    for (a, da), (b, db) in zip(anchors, anchors[1:]):
        if morph <= b:
            return da + (db - da) * (morph - a) / (b - a)
    return 4.0


def cubic(a: float, b: float, c: float, d: float, t: float) -> float:
    return b + 0.5 * t * (c - a + t * (2 * a - 5 * b + 4 * c - d + t * (3 * (b - c) + d - a)))


def edge_frames(n: int, smooth: bool) -> int:
    short = min((n - 1) // 2, max(1, positive_round(0.002 * FS)))
    return min((n - 1) // 2, max(short, positive_round(0.2 * n))) if smooth else short


def window(n: int, age: int, smooth: bool) -> float:
    edge = edge_frames(n, smooth)
    if n <= 2 or edge == 0:
        return 1.0
    phase = clamp(min(age / edge, (n - 1 - age) / edge), 0.0, 1.0)
    return 0.5 - 0.5 * math.cos(math.pi * phase)


def envelope_anchors() -> dict[str, Any]:
    """Offline recurrence oracle, bypassing audio DC blockers and SRC."""
    attack = 1 - math.exp(-1 / (0.005 * FS))
    release = 1 - math.exp(-1 / (0.08 * FS))
    state = 0.0
    tail = []
    for frame in range(2 * FS):
        value = math.sin(2 * math.pi * (frame % 48) / 48)
        energy = value * value  # identical or opposite-polarity stereo
        state += (attack if energy > state else release) * (energy - state)
        if frame >= FS:
            tail.append(8 * math.sqrt(state))
    mean = sum(tail) / len(tail)
    if not mean > 8 / math.sqrt(2) + 0.5:
        raise ValueError('Asymmetric follower must not be mistaken for a true RMS meter')
    return {
        'conditioningBypassed': True,
        'initialEnergy': 0.0,
        'attackSeconds': 0.005,
        'releaseSeconds': 0.08,
        'sine': {'frequencyHz': 1000, 'peakPerChannel': 1.0,
                 'renderFrames': 2 * FS, 'summaryStartFrame': FS,
                 'meanVolts': mean, 'minVolts': min(tail), 'maxVolts': max(tail)},
        'unitEnergyStep': [
            {'updates': n, 'risingFromZeroVolts': 8 * math.sqrt(1 - math.exp(-n / (0.005 * FS))),
             'fallingFromUnityVolts': 8 * math.sqrt(math.exp(-n / (0.08 * FS)))}
            for n in [1, 240, 3840, 48000]
        ],
    }


def build_vectors() -> dict[str, Any]:
    vectors: dict[str, Any] = {
        'schema': 'leviathan.chimera.reference-vectors',
        'schemaVersion': 1,
        'dspProfile': 1,
        'purpose': 'Mathematical anchors for the proposed software design; not hardware captures or implementation test results.',
        'coreRateHz': FS,
        'rounding': 'Nonnegative round = floor(x + 0.5); integer uint32 operations wrap mod 2^32.',
        'tolerance': {'referenceAbsolute': 1e-6, 'permittedOptimizedTableAbsolute': 1e-5},
        'capacities': {
            'maxFrames': 8_352_000,
            'maxSeconds': 174,
            'channels': 2,
            'bytesPerFrame': 8,
            'audioPayloadBytes': 66_816_000,
            'pageFrames': 256,
            'pageBytes': 2048,
            'fullReelPages': 32_625,
            'activePlusReserveBytes': 133_632_000,
            'maxRegions': 300,
            'maxReelSlots': 32,
        },
    }
    vectors['classicRate'] = [
        {'knob': k, 'attenuverter': a, 'cvVolts': v, 'rate': classic_rate(k, a, v)}
        for k, a, v in [
            (0, 0, 0), (1/6, 0, 0), (0.25, 0, 0), (0.49, 0, 0),
            (0.5, 0, 0), (0.51, 0, 0), (0.75, 0, 0), (5/6, 0, 0), (1, 0, 0),
            (0.5, 1, 4), (0.5, 1, -4), (0.5, -1, 4), (0.75, 1, -2),
        ]
    ]
    pitch_rows = []
    for mode, k in [(1, 5/6), (1, 1/6), (1, 0.5), (2, 0.0), (2, 0.75), (2, 1.0)]:
        for volts in [-2.0, -1.0, 0.0, 1.0, 2.0, 8.0]:
            base = classic_rate(k) if mode == 1 else forward_base(k)
            pitch_rows.append({'mode': mode, 'knob': k, 'attenuverter': 1.0, 'cvVolts': volts,
                               'rate': clamp(base * 2**volts, -32.0, 32.0)})
    vectors['pitchRate'] = pitch_rows
    vectors['sos'] = [
        {'knob': k, 'patched': patched, 'cvVolts': v,
         'mix': clamp(k * (v/8 if patched else 1), 0, 1)}
        for k, patched, v in [(0.75, False, 0), (0.75, True, 0), (0.75, True, 4),
                              (0.75, True, 8), (0.75, True, 16), (0.75, True, -4),
                              (0, False, 8), (1, True, 8)]
    ]
    vectors['finiteGene'] = [
        {'spliceFrames': length, 'normalizedControl': u, 'frames': finite_gene_frames(length, u)}
        for length in [1, 2, 3, 16, 257, 4800, 48_000, 8_352_000]
        for u in [0.0002, 0.25, 0.5, 0.75, 1.0]
    ]
    vectors['morphDensity'] = [{'morph': m, 'density': density(m), 'hopFor480FrameGene': 480 / density(m)}
                               for m in [0, 1/12, 1/6, 1/3, 0.5, 2/3, 5/6, 11/12, 1]]
    vectors['cubic'] = [{'taps': list(taps), 'fraction': t, 'value': cubic(*taps, t)}
                        for taps in [(0, 1, 2, 3), (0, 0, 1, 0), (1, -1, 0.5, 2)]
                        for t in [0, 0.25, 0.5, 0.75, 1]]
    vectors['windows'] = [
        {'frames': n, 'smooth': smooth, 'edgeFrames': edge_frames(n, smooth),
         'samples': [{'age': j, 'weight': window(n, j, smooth)}
                     for j in sorted({0, min(1, n-1), (n-1)//2, max(0, n-2), n-1,
                                      min(edge_frames(n, smooth), n-1)})]}
        for n in [1, 2, 3, 16, 480, 4800] for smooth in [False, True]
    ]
    vectors['unityEnvelope'] = [
        {'density': d, 'baseWindow': w, 'smooth': smooth,
         'unityBlend': (0.0 if smooth else clamp(1-abs(d-1)/0.025, 0, 1)),
         'effectiveWindow': w if smooth else
            (1-clamp(1-abs(d-1)/0.025, 0, 1))*w + clamp(1-abs(d-1)/0.025, 0, 1)}
        for d in [0.9, 0.99, 1.0, 1.01, 2.0] for w in [0.0, 0.5, 1.0] for smooth in [False, True]
    ]
    prng = []
    state = DEFAULT_SEED
    for draw in range(16):
        state ^= (state << 13) & 0xffffffff
        state ^= state >> 17
        state ^= (state << 5) & 0xffffffff
        state &= 0xffffffff
        prng.append({'draw': draw, 'uint32': state, 'uniform': (state >> 8) / 16_777_216.0})
    vectors['prng'] = {'seed': DEFAULT_SEED, 'seedHex': '0x6D2B79F5', 'draws': prng}
    vectors['stereoBalance'] = [
        {'pan': p, 'gainL': math.sqrt(2)*math.cos(math.pi*(p+1)/4),
         'gainR': math.sqrt(2)*math.sin(math.pi*(p+1)/4)}
        for p in [-1, -0.5, 0, 0.5, 1]
    ]
    vectors['inputGain'] = [{'decibels': db, 'linearGain': 10**(db/20)} for db in [-3, 0, 6, 12]]
    vectors['onePole'] = [{'tauSeconds': tau, 'alphaAt48k': 1-math.exp(-1/(tau*FS))}
                          for tau in [0.00025, 0.001, 0.002, 0.005, 0.01, 0.08]]
    vectors['envelopeFollower'] = envelope_anchors()
    vectors['slotFilenames'] = [f'mg{i}.wav' for i in range(1, 10)] + [f'mg{chr(c)}.wav' for c in range(ord('a'), ord('w')+1)]
    vectors['timingAnchors'] = {
        'finiteGeneBorn100Length480': {'firstFrame': 100, 'lastFrame': 579, 'completionFrame': 580},
        'recordStart100Stop110': {'firstFrame': 100, 'lastFrame': 109, 'frameCount': 10},
        'stretch480FramesPer2400TickClock': {'sourceVelocityForward': 0.2, 'sourceVelocityReverse': -0.2},
        'fullSplice4800Frames': [{'rate': 0.5, 'coreFramesPerCycle': 9600},
                               {'rate': 1, 'coreFramesPerCycle': 4800},
                               {'rate': 2, 'coreFramesPerCycle': 2400}],
        'fullReelReferenceCapture': {'pagesPerTick': 8, 'ticks': math.ceil(32625/8),
                                    'seconds': math.ceil(32625/8)/FS},
    }
    assert len(vectors['slotFilenames']) == 32
    assert vectors['slotFilenames'][-1] == 'mgw.wav'
    assert math.isclose(classic_rate(5/6), 1.0, abs_tol=1e-12)
    assert math.isclose(classic_rate(1/6), -1.0, abs_tol=1e-12)
    assert math.isclose(forward_base(0.75), 1.0, abs_tol=1e-12)
    assert 8_352_000 // 256 == 32_625
    assert finite_gene_frames(4800, 1) == 13
    assert math.isclose(density(0.5), 2.0)
    return vectors


def check_equivalent(expected: Any, actual: Any, path: str = '$') -> None:
    if isinstance(expected, dict):
        if not isinstance(actual, dict) or expected.keys() != actual.keys():
            raise ValueError(f'{path}: dictionary keys differ')
        for key in expected:
            check_equivalent(expected[key], actual[key], f'{path}.{key}')
    elif isinstance(expected, list):
        if not isinstance(actual, list) or len(expected) != len(actual):
            raise ValueError(f'{path}: list shape differs')
        for i, (a, b) in enumerate(zip(expected, actual)):
            check_equivalent(a, b, f'{path}[{i}]')
    elif isinstance(expected, bool):
        if type(actual) is not bool or expected != actual:
            raise ValueError(f'{path}: expected boolean {expected!r}, got {actual!r}')
    elif isinstance(expected, int):
        if type(actual) is not int or expected != actual:
            raise ValueError(f'{path}: expected integer {expected!r}, got {actual!r}')
    elif isinstance(expected, float):
        if type(actual) not in (int, float) or not math.isfinite(actual) or not math.isclose(expected, actual, rel_tol=1e-12, abs_tol=1e-12):
            raise ValueError(f'{path}: {expected!r} != {actual!r}')
    elif expected != actual:
        raise ValueError(f'{path}: {expected!r} != {actual!r}')


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--output', type=Path, default=None)
    mode.add_argument('--check', type=Path, default=None)
    args = parser.parse_args()
    vectors = build_vectors()
    if args.check is not None:
        actual = json.loads(args.check.read_text(encoding='utf-8'))
        check_equivalent(vectors, actual)
        print('PASS: checked mathematical vectors and generator anchors; no module implementation was tested.')
    else:
        destination = args.output or Path(__file__).with_name('reference_vectors.json')
        destination.write_text(json.dumps(vectors, indent=2, allow_nan=False) + '\n', encoding='utf-8')
        print(f'Wrote {destination}')


if __name__ == '__main__':
    main()
