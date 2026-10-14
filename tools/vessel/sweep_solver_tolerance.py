#!/usr/bin/env python3
"""Compile/run friction-tolerance experiments without changing the live core.

Requires a C++ compiler and NumPy. Only the friction residual threshold changes;
strike/coupled outer tolerances, iteration limits and recovery stay unchanged.
"""
import argparse
import csv
import hashlib
import json
import platform
from pathlib import Path
import shutil
import subprocess
import tempfile
import numpy as np

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output', type=Path, default=ROOT/'build/vessel-solver-sweep')
parser.add_argument('--seconds', type=float, default=8)
parser.add_argument('--repeats', type=int, default=3)
parser.add_argument('--compiler', default='g++')
parser.add_argument('--reverse', action='store_true', help='run candidates before reference for order checks')
args = parser.parse_args()
output = args.output.resolve()
output.mkdir(parents=True, exist_ok=True)
scales = [1, 10, 100, 1000]
flags = ['-std=c++11', '-O3', '-march=nehalem', '-Wall', '-Wextra', '-fno-fast-math', '-fno-unsafe-math-optimizations']
source = ROOT/'src/vessel'
source_hash = hashlib.sha256()
for p in sorted(source.iterdir()):
    if p.suffix in ('.hpp', '.cpp'):
        source_hash.update(p.name.encode())
        source_hash.update(p.read_bytes())
with tempfile.TemporaryDirectory(prefix='vessel-solver-sweep-') as temp:
    temp = Path(temp)
    binaries = {}
    for scale in scales:
        tree = temp/str(scale)
        shutil.copytree(source, tree/'vessel')
        friction = tree/'vessel/FrictionContact.cpp'
        code = friction.read_text()
        needle = 'const double tolerance = 1e-12+1e-11*std::max(1.0, hi);'
        if code.count(needle) != 1:
            raise RuntimeError('friction tolerance source changed; update experiment explicitly')
        friction.write_text(code.replace(needle, f'const double tolerance = {scale}.0*(1e-12+1e-11*std::max(1.0, hi));'))
        binary = tree/'probe'
        subprocess.run([args.compiler, *flags, '-I'+str(tree), str(ROOT/'tools/vessel/probe_solver_tolerance.cpp'),
                        *map(str, sorted((tree/'vessel').glob('*.cpp'))), '-o', str(binary)], check=True)
        binaries[scale] = binary
    for scale in (list(reversed(scales)) if args.reverse else scales):
        run = output/str(scale)
        run.mkdir(exist_ok=True)
        with (run/'metrics.csv').open('w') as report:
            subprocess.run([str(binaries[scale]), str(run), str(args.seconds), str(args.repeats)], stdout=report, check=True)

metrics = {}
for scale in scales:
    with (output/str(scale)/'metrics.csv').open() as report:
        metrics[scale] = {(r['case'], r['quality']): r for r in csv.DictReader(report)}
comparisons = []
for key, baseline in metrics[1].items():
    name = f'{key[0]}_{key[1]}.f64'
    ref = np.fromfile(output/'1'/name, dtype=np.float64).reshape(-1, 2)
    for scale in scales[1:]:
        candidate = metrics[scale][key]
        audio = np.fromfile(output/str(scale)/name, dtype=np.float64).reshape(-1, 2)
        if audio.shape != ref.shape:
            raise RuntimeError('frame count mismatch')
        rms = float(np.sqrt(np.mean(ref*ref)))
        error = float(np.sqrt(np.mean((audio-ref)**2)))
        row = {'case': key[0], 'quality': int(key[1]), 'scale': scale,
               'speedup_percent': 100*(1-float(candidate['us_per_frame'])/float(baseline['us_per_frame'])),
               'reference_rms': rms, 'candidate_rms': float(candidate['rms']), 'error_rms': error,
               'relative_error_db': float(20*np.log10(error/rms)) if error and rms else None,
               'peak_error': float(np.max(np.abs(audio-ref))), 'faults': int(candidate['faults']),
               'max_ledger_residual': float(candidate['max_ledger_residual']),
               'reference_iterations': float(baseline['mean_friction_iterations']),
               'candidate_iterations': float(candidate['mean_friction_iterations']),
               'rate_changes': int(candidate['rate_changes'])}
        comparisons.append(row)
(output/'comparison.json').write_text(json.dumps(comparisons, indent=2)+'\n')
(output/'environment.json').write_text(json.dumps({'source_sha256': source_hash.hexdigest(), 'flags': flags,
    'compiler': subprocess.check_output([args.compiler, '--version'], text=True).splitlines()[0],
    'platform': platform.platform(), 'machine': platform.machine(),
    'harness_sha256': hashlib.sha256((ROOT/'tools/vessel/probe_solver_tolerance.cpp').read_bytes()).hexdigest(),
    'seconds': args.seconds, 'repeats': args.repeats, 'scales': scales, 'reverse': args.reverse,
    'timing': 'Unpinned offline elapsed time; includes controls/retuning, excludes auditing and audio file writes. No warmup outside each trajectory; not a Rack callback measurement.'}, indent=2)+'\n')
print(f'Reports written to {output}')
