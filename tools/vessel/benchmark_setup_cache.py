#!/usr/bin/env python3
"""Compare preserved benchmark_pitch_module executables, with alternating order."""
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
data = {name: [] for name in binaries}
for repeat in range(a.pairs):
    for name in (list(binaries) if repeat%2 == 0 else list(reversed(binaries))):
        raw = subprocess.check_output([str(binaries[name])], text=True)
        (a.output/f'{name}-{repeat}.csv').write_text(raw)
        data[name].append(list(csv.DictReader(raw.splitlines())))
        print(repeat, name, flush=True)
keys = ['bowl', 'quality', 'separation', 'rub']
timings = ['mean_us', 'median_us', 'p99_us', 'max_us', 'stream_mean_us']
groups = {}
for repeat in range(a.pairs):
    assert len(data['baseline'][repeat]) == len(data['candidate'][repeat]) == 192
    for x, y in zip(data['baseline'][repeat], data['candidate'][repeat]):
        assert {k:v for k,v in x.items() if k not in timings} == {
            k:v for k,v in y.items() if k not in timings}, 'fixture/output mismatch'
        key = tuple(x[k] for k in keys)
        if key not in groups:
            groups[key] = {m: [[] for _ in range(a.pairs)] for m in ['mean_us', 'stream_mean_us']}
        for metric in groups[key]:
            groups[key][metric][repeat].append((float(x[metric]), float(y[metric])))
rows = []
for key, metrics in groups.items():
    row = dict(zip(keys, key))
    for metric, pairs in metrics.items():
        savings = [100*(1-statistics.median(y for x,y in pair)/statistics.median(x for x,y in pair)) for pair in pairs]
        row[metric] = {'paired_savings_percent': savings, 'median_saving_percent': statistics.median(savings)}
    rows.append(row)
report = {'pairs': a.pairs, 'cpu': a.cpu,
          'binary_sha256': {k: hashlib.sha256(v.read_bytes()).hexdigest() for k,v in binaries.items()}, 'rows': rows}
for metric in ['mean_us', 'stream_mean_us']:
    values = [r[metric]['median_saving_percent'] for r in rows]
    report[metric] = {'median_saving_percent': statistics.median(values),
                      'min_saving_percent': min(values), 'max_saving_percent': max(values)}
(a.output/'summary.json').write_text(json.dumps(report, indent=2, allow_nan=False)+'\n')
print(json.dumps({k:report[k] for k in ['mean_us', 'stream_mean_us']}, indent=2))
