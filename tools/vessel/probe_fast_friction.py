#!/usr/bin/env python3
"""Measure the offline cubic friction prototype against the analytic core.

Production source is never rewritten. The shared trajectory harness measures
controls/retuning as well as processing; it excludes audits and disk I/O from
its timed runs. NumPy is required for waveform comparisons.
"""
import argparse
import csv
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import numpy as np

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,default=ROOT/'build/vessel-friction-probe')
parser.add_argument('--seconds',type=float,default=8)
parser.add_argument('--repeats',type=int,default=2)
parser.add_argument('--compiler',default='g++')
group = parser.add_mutually_exclusive_group()
group.add_argument('--hybrid',action='store_true',help='approximate only tanh; preserve analytic Gaussian')
group.add_argument('--gaussian-only',action='store_true')
group.add_argument('--reciprocals',action='store_true',help='analytic functions with per-solve reciprocals')
group.add_argument('--reciprocal-tanh',action='store_true',help='combine per-solve reciprocals with cubic tanh')
parser.add_argument('--cpu',type=int,default=None,help='Linux CPU affinity for measured runs')
args = parser.parse_args()
candidate_name = 'reciprocal_tanh' if args.reciprocal_tanh else 'hybrid' if args.hybrid else 'gaussian' if args.gaussian_only else 'reciprocals' if args.reciprocals else 'cubic'
variants = ['analytic',candidate_name]
output = args.output.resolve()
output.mkdir(parents=True,exist_ok=True)
flags = ['-std=c++11','-O3','-march=nehalem','-Wall','-Wextra','-fno-fast-math','-fno-unsafe-math-optimizations']
with tempfile.TemporaryDirectory(prefix='vessel-friction-probe-') as temp:
    temp = Path(temp)
    binaries = {}
    for variant in variants:
        tree = temp/variant
        shutil.copytree(ROOT/'src/vessel',tree/'vessel')
        # Reconstruct the analytic control from the current production solver.
        # All edits remain inside the isolated copy; fail on source drift.
        p = tree/'vessel/FrictionContact.cpp'
        code = p.read_text()
        if '#include "PreparedFrictionLaw.hpp"' in code:
            replacements = {
                '#include "PreparedFrictionLaw.hpp"\n':'',
                '    const PreparedFrictionLaw prepared(m);\n':'',
                'prepared.evaluate(result.slip, load)':'frictionValue(result.slip, load, m)',
                'return preparedFrictionNegativeSlopeBound(load,m);':
                    'return load*0.8577638849607068*(m.muS-m.muK)/m.weakeningVelocity;'}
            for before,after in replacements.items():
                if code.count(before)!=1: raise RuntimeError('production preparation changed; update control reconstruction')
                code=code.replace(before,after)
            p.write_text(code)
        if variant!='analytic':
            p = tree/'vessel/FrictionContact.cpp'
            code = p.read_text()
            (tree/'experiments').mkdir()
            for name in ['FrictionApproximation.hpp','FrictionTables.hpp']:
                shutil.copy2(ROOT/'tools/vessel/experiments'/name,tree/'experiments'/name)
            header = '../experiments/FrictionApproximation.hpp'
            code = code.replace('#include "StrikeContact.hpp"',f'#include "StrikeContact.hpp"\n#include "{header}"')
            if args.reciprocals or args.reciprocal_tanh:
                needle = "    double lo = -load*m.muS, hi = load*m.muS;"
                assert code.count(needle)==1
                code = code.replace(needle,"    const FrictionReciprocals prepared(m);\n"+needle)
                needle = "frictionValue(result.slip, load, m)"
                assert code.count(needle)==1
                code = code.replace(needle,"prepared.evaluate<true>(result.slip, load)" if args.reciprocal_tanh else "prepared.evaluate(result.slip, load)")
                if args.reciprocal_tanh:
                    needle = 'return load*0.8577638849607068*(m.muS-m.muK)/m.weakeningVelocity;'
                    assert code.count(needle)==1
                    code = code.replace(needle,'return frictionApproximateNegativeSlopeBound(load,m);')
            else:
                begin = code.index('FrictionValue frictionValue(')
                end = code.index('double frictionNegativeSlopeBound',begin)
                evaluator = 'frictionValueTanhApproximate' if args.hybrid else 'frictionValueGaussianApproximate' if args.gaussian_only else 'frictionValueApproximate'
                code = code[:begin]+'''FrictionValue frictionValue(double slip, double load, const MalletDescriptor& m) noexcept {
    return frictionValueApproximate(slip,load,m);
}
'''.replace('frictionValueApproximate',evaluator)+code[end:]
                needle = 'return load*0.8577638849607068*(m.muS-m.muK)/m.weakeningVelocity;'
                if code.count(needle)!=1:
                    raise RuntimeError('slope bound source changed; update experiment explicitly')
                code = code.replace(needle,'return frictionApproximateNegativeSlopeBound(load,m);')
            p.write_text(code)
        binary = tree/'probe'
        subprocess.run([args.compiler,*flags,'-I'+str(tree),str(ROOT/'tools/vessel/probe_solver_tolerance.cpp'),
                        *map(str,sorted((tree/'vessel').glob('*.cpp'))),'-o',str(binary)],check=True)
        binaries[variant] = binary
    # Preserve exact experiment sources/binaries for follow-up callback builds.
    for variant in variants:
        shutil.copytree(temp/variant,output/'sources'/variant,dirs_exist_ok=True)
    if args.cpu is not None:
        os.sched_setaffinity(0,{args.cpu})
    # Repeat in opposite order to expose sequential timing bias.
    for variant,repeat in [('analytic',0),(candidate_name,0),(candidate_name,1),('analytic',1)]:
        folder = output/variant/str(repeat)
        folder.mkdir(parents=True,exist_ok=True)
        with (folder/'metrics.csv').open('w') as report:
            subprocess.run([str(binaries[variant]),str(folder),str(args.seconds),str(args.repeats)],stdout=report,check=True)

metrics = {}
for variant in variants:
    metrics[variant] = []
    for repeat in range(2):
        with (output/variant/str(repeat)/'metrics.csv').open() as report:
            metrics[variant].append({(r['case'],r['quality']):r for r in csv.DictReader(report)})
rows = []
for key,ref_metrics in metrics['analytic'][0].items():
    ref = np.fromfile(output/'analytic/0'/f'{key[0]}_{key[1]}.f64',dtype=np.float64)
    candidate = np.fromfile(output/candidate_name/'0'/f'{key[0]}_{key[1]}.f64',dtype=np.float64)
    if ref.shape!=candidate.shape or not np.isfinite(ref).all() or not np.isfinite(candidate).all():
        raise RuntimeError('frame count/nonfinite audio mismatch')
    for variant in variants:
        first = output/variant/'0'/f'{key[0]}_{key[1]}.f64'
        second = output/variant/'1'/f'{key[0]}_{key[1]}.f64'
        if first.read_bytes()!=second.read_bytes():
            raise RuntimeError('nondeterministic repeated capture')
        for run in metrics[variant]:
            if int(run[key]['faults']):
                raise RuntimeError('fixture solver fault')
            if any(run[key][field]!=ref_metrics[field] for field in ['start_rate','end_rate','rate_changes']):
                raise RuntimeError('candidate rate selection changed')
    rms = float(np.sqrt(np.mean(ref*ref)))
    error = float(np.sqrt(np.mean((candidate-ref)**2)))
    before,after = [sum(float(run[key]['us_per_frame']) for run in metrics[variant])/2 for variant in variants]
    row = dict(case=key[0],quality=int(key[1]),reference_us_per_frame=before,candidate_us_per_frame=after,variant=candidate_name,
        saving_percent=100*(1-after/before),rms_error_percent=100*error/rms if rms else None,
        peak_error_percent=100*float(np.max(np.abs(candidate-ref)))/float(np.max(np.abs(ref))) if rms else None,relative_error_db=float(20*np.log10(error/rms)) if error and rms else None,
        level_delta_db=float(20*np.log10(float(metrics[candidate_name][0][key]['rms'])/rms)) if rms else None,
        faults=sum(int(run[key]['faults']) for run in metrics[candidate_name]),
        max_ledger_residual=max(float(run[key]['max_ledger_residual']) for run in metrics[candidate_name]),
        max_friction_iterations=max(int(run[key]['max_friction_iterations']) for run in metrics[candidate_name]))
    rows.append(row)
(output/'comparison.json').write_text(json.dumps(rows,indent=2)+'\n')
source_hash = hashlib.sha256()
for path in sorted((ROOT/'src/vessel').iterdir()):
    if path.suffix in ['.cpp','.hpp']:
        source_hash.update(path.name.encode())
        source_hash.update(path.read_bytes())
(output/'environment.json').write_text(json.dumps(dict(source_sha256=source_hash.hexdigest(),flags=flags,
    seconds=args.seconds,repeats=args.repeats,order=['analytic',candidate_name,candidate_name,'analytic'],
    compiler=subprocess.check_output([args.compiler,'--version'],text=True).splitlines()[0],
    prototype_sha256=hashlib.sha256((ROOT/'tools/vessel/experiments/FrictionApproximation.hpp').read_bytes()).hexdigest(),
    tables_sha256=hashlib.sha256((ROOT/'tools/vessel/experiments/FrictionTables.hpp').read_bytes()).hexdigest(),
    cpu_affinity=args.cpu, timing='Linux elapsed time; includes controls/retunes, excludes auditing/file I/O. No Rack callback or UI measurement.'),indent=2)+'\n')
print(f'Reports written to {output}')
