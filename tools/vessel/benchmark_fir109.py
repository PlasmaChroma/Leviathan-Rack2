#!/usr/bin/env python3
"""Serial alternating baseline/candidate timing; run after builds/tests finish."""
import argparse
import csv
import json
import os
from pathlib import Path
import statistics
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--pairs', type=int, default=6)
    parser.add_argument('--cpu', type=int, default=None)
    args = parser.parse_args()
    if args.pairs < 3:
        parser.error('at least three pairs required')
    args.output.mkdir(parents=True, exist_ok=True)
    # Affinity is inherited by the benchmark children on Linux; optional elsewhere.
    if args.cpu is not None:
        os.sched_setaffinity(0, {args.cpu})
    results = {}
    for mode in ('fir', 'standalone', 'expander'):
        paired = []
        for repeat in range(args.pairs):
            rows = {}
            for name in (('reference','candidate') if repeat%2 == 0 else ('candidate','reference')):
                if mode == 'fir':
                    command = ['build/tools/vessel_fir_capture'+('_109' if name=='candidate' else ''), '--benchmark']
                else:
                    command = ['build/tools/vessel_benchmark_active_module'+('_109' if name=='candidate' else '')]
                    if mode == 'expander':
                        command += ['--expander']
                print(mode, repeat, name, flush=True)
                path = args.output/f'{mode}-{repeat}-{name}.csv'
                with path.open('w') as out:
                    subprocess.run(command, stdout=out, check=True)
                rows[name] = list(csv.DictReader(path.open()))
            if mode == 'fir':
                # Each process reports seven batches per factor; compare medians.
                pair = {}
                for factor in ('1','2','4','8'):
                    a,b = [statistics.median(float(r['ns_per_stereo_frame']) for r in rows[name] if r['factor']==factor)
                           for name in ('reference','candidate')]
                    pair[factor] = {'reference':a, 'candidate':b, 'saving_percent':100*(1-b/a)}
            else:
                assert len(rows['reference']) == len(rows['candidate']) == 16
                pair = {}
                for a,b in zip(rows['reference'], rows['candidate']):
                    keys = ('bowl','quality','separation','coupled')
                    assert all(a[k] == b[k] for k in keys)
                    x,y = float(a['mean_us']),float(b['mean_us'])
                    pair[','.join(a[k] for k in keys)] = {'reference':x, 'candidate':y, 'saving_percent':100*(1-y/x)}
            paired.append(pair)
        fixtures = {}
        for key in paired[0]:
            values = [p[key]['saving_percent'] for p in paired]
            fixtures[key] = dict(median_saving_percent=statistics.median(values), min_saving_percent=min(values),
                                 max_saving_percent=max(values), faster_pairs=sum(v>0 for v in values),
                                 reference_median=statistics.median(p[key]['reference'] for p in paired),
                                 candidate_median=statistics.median(p[key]['candidate'] for p in paired))
        medians = [v['median_saving_percent'] for v in fixtures.values()]
        results[mode] = dict(fixtures=fixtures, median_fixture_saving_percent=statistics.median(medians),
                             min_fixture_saving_percent=min(medians), max_fixture_saving_percent=max(medians))
        (args.output/'summary.json').write_text(json.dumps(results, indent=2)+'\n')
    print(json.dumps(results, indent=2))


if __name__ == '__main__':
    main()
