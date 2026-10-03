#!/usr/bin/env python3
"""Compact original-instruction buffer fixtures. No firmware/emulator required."""
import argparse
from pathlib import Path
from generate_reference_fixtures import load, bits, ROOT

DEST = ROOT / 'tests/fixtures/tiamat/buffer_v1.txt'
FIELDS = ('N', 'segment', 'start', 'end', 'audible', 'candidate', 'bankW', 'bankR', 'repeats',
          'rpos', 'wpos', 'rate', 'frozen', 'freezeRequested', 'timeResize')


def state(s):
    return ' '.join(str(s[k]) for k in FIELDS)


def generate():
    source = load('integration/reader_rng_probes.json')
    rows = ['# Tiamat buffer fixtures v1', '# firmware_sha256 ' + source['firmware_sha256'],
            '# READ: original reader, address ramp, 160 frames, frozen, unit slew; no substitutions.',
            '# WINDOW: frozen all-one planes; coefficient rate 48000, one N-frame block, zero input.',
            '# ENGINE: original Buffer; 32768-frame planes, 96028 coefficients, N=1000, 8x256 frames, sin(frame*.2) input.',
            '# ENGINE rand=128/powf/memset hooks; no outer clock, Corrupt or output; all recorded state transitions retained.',
            '# HISTORY: frozen N=1200, rate=.125, memory=.2, input=.8, cursor=32766, one 96-frame block.',
            '# TIME: original Buffer with modeled outer smoother/clock; reduced 32768 planes, 800x96 frames.',
            '# TIME uses probe_time_memory.py exact prepared driver; not full callback emulation.',
            '# STATE fields: ' + ' '.join(FIELDS)]
    for case in source['reader_cases']:
        rows.append('READ ' + ' '.join(map(str, [case['length'], case['rate'], len(case['wrap_frames']),
            *case['wrap_frames'], *[bits(x) for x in case['first_samples']]])))
    for case in load('buffer_engine/window_freeze_probes.json')['window_cases']:
        rows.append(f"WINDOW {case['N']} {case['window']} {len(case['samples'])}")
        for frame, sample in case['samples'].items():
            rows.append(f'W {frame} {bits(sample)}')
    for i, case in enumerate(load('buffer_engine/buffer_engine_probes.json')):
        c = case['case']
        rows.append(f"ENGINE {i} {c['exponent']} {c['silence']} {c['traverse']} {case['rng_calls']} {len(case['events'])}")
        for e in case['events']:
            rows.append(f"E {e['ch']} {e['frame']} " + state(e))
        rows.append('FINAL ' + state(case['final']))
    for case in load('character/history_boundary_probes.json')['cases']:
        rows.append(f"HISTORY {case['read_bank']} {case['changed_count']} {case['history_cursor']}")
        rows.append('H ' + ' '.join(str(bits(x)) for pair in case['first_12_output_frames'] for x in pair))
        rows.append('FINAL ' + state(case['final']))
    for case in load('character/time_memory_probes.json')['cases']:
        rows.append(f"TIME {case['speed']} {int(case['frozen'])} {case['initial_read_bank']} {int(case['clock_requests'])}")
        for stage in case['stages']:
            rows.append(f"STAGE {stage['name']} {stage['initial_region_changed_count']} {stage['guard_blocks']} {len(stage['states'])}")
            for checkpoint in stage['states']:
                rows.append(f"CHECK {checkpoint['frame']} {int(checkpoint['guard'])}")
                for ch in checkpoint['channels']:
                    rows.append('FINAL ' + state(ch))
    return '\n'.join(rows) + '\n'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    expected = generate()
    if args.check:
        if not DEST.exists() or DEST.read_text(encoding='utf-8') != expected:
            raise SystemExit('Tiamat buffer fixtures are stale; regenerate them.')
        print('Tiamat buffer fixtures are current.')
    else:
        DEST.parent.mkdir(parents=True, exist_ok=True)
        DEST.write_text(expected, encoding='utf-8', newline='\n')
        print(DEST)
