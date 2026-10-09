#!/usr/bin/env python3
"""Compile preserved probe variants and time full Vessel callbacks serially.
Run probe_fast_friction.py's tanh, reciprocals and reciprocal-tanh experiments
first, under subdirectories named tanh, reciprocals, reciprocal-tanh.
"""
import argparse
import csv
import hashlib
import json
import os
from pathlib import Path
import statistics
import subprocess

ROOT=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('experiments',type=Path)
p.add_argument('--rack-dir',type=Path,default=ROOT.parent/'Rack-SDK')
p.add_argument('--pairs',type=int,default=4)
p.add_argument('--cpu',type=int,default=None)
a=p.parse_args()
if a.pairs<3: p.error('at least three pairs required')
out=a.experiments.resolve()/'callbacks';out.mkdir(parents=True,exist_ok=True)
variants={'analytic':('reciprocal-tanh','analytic'),'tanh':('tanh','hybrid'),
          'reciprocals':('reciprocals','reciprocals'),'reciprocal_tanh':('reciprocal-tanh','reciprocal_tanh')}
binaries={};builds={}
for name,(folder,variant) in variants.items():
    tree=a.experiments.resolve()/folder/'sources'/variant
    binary=out/name
    sdk=a.rack_dir.resolve()
    command=['g++','-std=c++17','-D_USE_MATH_DEFINES','-O3','-march=nehalem','-Wall','-Wextra','-Wno-unused-parameter',
             '-fno-fast-math','-fno-unsafe-math-optimizations','-I'+str(tree),'-I'+str(ROOT/'src'),
             '-I'+str(sdk/'include'),'-I'+str(sdk/'dep/include'),str(ROOT/'tools/vessel/benchmark_active_module.cpp'),
             str(ROOT/'src/Vessel.cpp'),*map(str,sorted((tree/'vessel').glob('*.cpp'))),
             '-L'+str(sdk),'-lRack','-Wl,-rpath,'+str(sdk),'-o',str(binary)]
    subprocess.run(command,check=True)
    binaries[name]=binary
    builds[name]={'command':command,'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest()}
(out/'environment.json').write_text(json.dumps({'builds':builds,'cpu_affinity':a.cpu,'pairs':a.pairs},indent=2)+'\n')
if a.cpu is not None: os.sched_setaffinity(0,{a.cpu})
summary={}
for mode,options in [('ordinary',[]),('slow_felt',['--speed','.0001','--pressure','15','--mallet','3'])]:
    measurements={name:[] for name in variants}
    for repeat in range(a.pairs):
        order=list(variants) if repeat%2==0 else list(reversed(variants))
        for name in order:
            path=out/f'{mode}-{name}-{repeat}.csv'
            print(mode,repeat,name,flush=True)
            with path.open('w') as stream: subprocess.run([str(binaries[name]),*options],stdout=stream,check=True)
            with path.open() as stream: measurements[name].append(list(csv.DictReader(stream)))
    report={}
    for name in variants:
        if name=='analytic':continue
        fixtures=[]
        for index in range(16):
            savings=[]
            for i in range(a.pairs):
                x,y=measurements['analytic'][i][index],measurements[name][i][index]
                keys=['bowl','quality','separation','coupled','mallet','speed','pressure']
                assert all(x[k]==y[k] for k in keys)
                savings.append(100*(1-float(y['mean_us'])/float(x['mean_us'])))
            fixtures.append({**{k:y[k] for k in keys},'median_saving_percent':statistics.median(savings),
                             'min_saving_percent':min(savings),'max_saving_percent':max(savings),
                             'faster_pairs':sum(v>0 for v in savings)})
        medians=[r['median_saving_percent'] for r in fixtures]
        report[name]={'fixtures':fixtures,'median_fixture_saving_percent':statistics.median(medians),
                      'min_fixture_saving_percent':min(medians),'max_fixture_saving_percent':max(medians)}
    summary[mode]=report
    (out/'summary.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps(summary,indent=2))
