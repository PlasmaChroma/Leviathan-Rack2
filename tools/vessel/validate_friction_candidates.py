#!/usr/bin/env python3
"""Build and run existing suites against preserved isolated friction candidates."""
import argparse
from pathlib import Path
import subprocess,concurrent.futures
root=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('experiments',type=Path)
p.add_argument('--rack-dir',type=Path,default=root.parent/'Rack-SDK')
a=p.parse_args();out=a.experiments.resolve();sdk=a.rack_dir.resolve()
jobs=[]
for name,folder,variant in [('reciprocals','reciprocals','reciprocals'),('reciprocal_tanh','reciprocal-tanh','reciprocal_tanh')]:
 tree=out/folder/'sources'/variant
 for test in ['engine','host_rate','dual_bowl','passive_tail','module']:
  binary=out/(name+'-'+test+'-spec')
  cmd=['g++','-std=c++17' if test=='module' else '-std=c++11','-O2','-Wall','-Wextra','-Wno-unused-parameter','-fno-fast-math','-fno-unsafe-math-optimizations','-I'+str(tree),'-I'+str(root/'src')]
  sources=[str(root/'tests'/('vessel_'+test+'_spec.cpp'))]
  if test=='module':
   cmd+=['-D_USE_MATH_DEFINES','-I'+str(sdk/'include'),'-I'+str(sdk/'dep/include')]
   sources+=[str(root/'src'/n) for n in ['Vessel.cpp','VTune.cpp','VTuneBodyMapModule.cpp']]
  cmd+=sources+list(map(str,sorted((tree/'vessel').glob('*.cpp'))))
  if test=='module':cmd+=['-L'+str(sdk),'-lRack','-Wl,-rpath,'+str(sdk)]
  cmd+=['-o',str(binary)]
  jobs.append((cmd,binary))
def build(job):
 cmd,binary=job
 with Path(str(binary)+'.build.log').open('w') as log:subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT,check=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:list(pool.map(build,jobs))
for cmd,binary in jobs:
 print(binary.name,flush=True)
 with Path(str(binary)+'.log').open('w') as log:subprocess.run([str(binary)],stdout=log,stderr=subprocess.STDOUT,check=True)
