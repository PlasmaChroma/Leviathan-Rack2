#!/usr/bin/env python3
"""Pair preserved benchmark_active_module binaries; require exact fingerprints."""
import argparse
import csv
import hashlib
import json
import os
from pathlib import Path
import statistics
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('baseline', type=Path)
p.add_argument('candidate', type=Path)
p.add_argument('output', type=Path)
p.add_argument('--pairs', type=int, default=4)
p.add_argument('--cpu', type=int)
a = p.parse_args()
if a.pairs < 3:
    p.error('at least three pairs required')
if a.cpu is not None:
    os.sched_setaffinity(0, {a.cpu})
a.output.mkdir(parents=True, exist_ok=True)
binaries = {'baseline': a.baseline.resolve(), 'candidate': a.candidate.resolve()}
report = {'pairs': a.pairs, 'cpu': a.cpu,
          'binary_sha256': {k: hashlib.sha256(v.read_bytes()).hexdigest() for k, v in binaries.items()}}
for mode, options in [('ordinary', []), ('expander', ['--expander'])]:
    data = {name: [] for name in binaries}
    for repeat in range(a.pairs):
        for name in (list(binaries) if repeat%2 == 0 else list(reversed(binaries))):
            raw = subprocess.check_output([str(binaries[name]), *options], text=True)
            (a.output/f'{mode}-{name}-{repeat}.csv').write_text(raw)
            data[name].append(list(csv.DictReader(raw.splitlines())))
            print(mode, repeat, name, flush=True)
    rows = []
    for index in range(16):
        savings = []
        for repeat in range(a.pairs):
            x, y = data['baseline'][repeat][index], data['candidate'][repeat][index]
            assert {k:v for k,v in x.items() if k != 'mean_us'} == {
                k:v for k,v in y.items() if k != 'mean_us'}, 'fixture/fingerprint mismatch'
            savings.append(100*(1-float(y['mean_us'])/float(x['mean_us'])))
        rows.append({'fixture': {k:v for k,v in x.items() if k not in ['mean_us', 'fingerprint']},
                     'median_saving_percent': statistics.median(savings), 'paired_savings_percent': savings})
    medians = [r['median_saving_percent'] for r in rows]
    report[mode] = {'median_saving_percent': statistics.median(medians),
                    'min_saving_percent': min(medians), 'max_saving_percent': max(medians), 'rows': rows}
    (a.output/'summary.json').write_text(json.dumps(report, indent=2, allow_nan=False)+'\n')
    print(mode, report[mode]['median_saving_percent'], flush=True)
