#!/usr/bin/env python3
"""Extract Macro/scheduling original-instruction observations; stdlib only."""
import argparse
from generate_reference_fixtures import load, ROOT
from generate_buffer_fixtures import state

DEST = ROOT / 'tests/fixtures/tiamat/events_v1.txt'


def extended(s):
    return state(s) + ' ' + ' '.join(str(s[k]) for k in ('bend', 'randomExponent', 'randomSilence', 'randomTraverse'))


def generate():
    data = load('integration/scheduling_probes.json')
    rows = ['# Tiamat events fixtures v1', '# firmware_sha256 ' + data['firmware_sha256'],
            '# MACRO: 68 isolated original helpers; injected rand sequence; initial left Break state=(7,.5,.8).',
            '# PARTITION: 1920 frames, N=1000, prepared sin(address*.071) memory, frozen, input=0.',
            '# time target=96028/2000 without outer guard; macro amounts=.9/.8, initial writer phase=980.',
            '# rand sequence=(5,210,400,99,254,33,180,601,75,0,160); powf/memset hooks.',
            '# FREEZE: N=64, pending request, constant memory/input, supplied previous raw value, 1920 frames.',
            '# STATE: buffer_v1 STATE fields followed by bend randomExponent randomSilence randomTraverse.',
            '# No hardware I/O, full callback or Corrupt/output processing.']
    for c in load('buffer_engine/macro_helper_probes.json'):
        rows.append('MACRO ' + ' '.join(map(str, [c['which'], c['amount'], c['channel'], int(c['shared']),
            c['initial_slew'], len(c['randoms']), *c['randoms'], len(c['rng_consumed']),
            *[c['outputs'][k] for k in ('bend_ratio', 'slew', 'repeat_exponent_add', 'silence_fraction', 'slice_random')]])))
    for c in data['partition_cases']:
        rows.append('PARTITION ' + ' '.join(map(str, [c['block_frames'], c['scenario'], c['shared'], c['rng_calls'],
            c['first_stereo_difference'] if c['first_stereo_difference'] is not None else -1, c['stereo_max_difference']])))
        for ch in c['states']:
            rows.append('STATE ' + extended(ch))
    for c in data['freeze_cases']:
        rows.append('FREEZE ' + ' '.join(map(str, [c['block_frames'], c['guard_initial'], c['previous'], c['buffer_value'], len(c['accepted'])])))
        for e in c['accepted']:
            rows.append(f"ACCEPT {e['channel']} {e['frame']}")
        for ch in c['final']:
            rows.append('STATE ' + extended(ch))
    return '\n'.join(rows) + '\n'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    expected = generate()
    if args.check:
        if not DEST.exists() or DEST.read_text(encoding='utf-8') != expected:
            raise SystemExit('Tiamat event fixtures are stale; regenerate them.')
        print('Tiamat event fixtures are current.')
    else:
        DEST.write_text(expected, encoding='utf-8', newline='\n')
        print(DEST)
