#!/usr/bin/env python3
"""Compile and screen exact composed tails in an isolated Vessel core copy.

No production DSP edits. Grants private access only to the offline prototype.
Timing is unpinned core timing, not a Rack callback certification.
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
import wave

import numpy as np

ROOT=Path(__file__).resolve().parents[2]


def replace(path,needle,replacement):
    text=path.read_text()
    if text.count(needle)!=1:
        raise RuntimeError(f'Prototype access hook changed: {path.name}; review explicitly')
    path.write_text(text.replace(needle,replacement))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'build/vessel-composed-tail')
    parser.add_argument('--compiler',default='g++')
    parser.add_argument('--quick',action='store_true')
    parser.add_argument('--sanitize',action='store_true')
    args=parser.parse_args()
    output=args.output.resolve();output.mkdir(parents=True,exist_ok=True)
    source=ROOT/'src/vessel';digest=hashlib.sha256()
    for path in sorted(source.iterdir()):
        if path.suffix in ('.hpp','.cpp'):
            digest.update(path.name.encode());digest.update(path.read_bytes())
    flags=['-std=c++11','-O3','-march=nehalem','-Wall','-Wextra','-fno-fast-math','-fno-unsafe-math-optimizations']
    if args.sanitize:
        flags+=['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer']
    metadata={'source_sha256':digest.hexdigest(),'flags':flags,'quick':args.quick,'sanitize':args.sanitize,
        'platform':platform.platform(),'compiler':subprocess.check_output([args.compiler,'--version'],text=True).splitlines()[0],
        'harness_sha256':hashlib.sha256((ROOT/'tools/vessel/probe_composed_tail.cpp').read_bytes()).hexdigest(),
        'prototype_sha256':hashlib.sha256((ROOT/'tools/vessel/experiments/ComposedTailHost.hpp').read_bytes()).hexdigest(),
        'timing':'Five alternating-order copies, two seconds each, auditing off. Synchronous configuration reported separately; streaming handoff onset/setup/continuation and simultaneous 1/8/32-instance re-entry reported in reentry_timing.csv. No whole-module or Windows claim.'}
    (output/'environment.json').write_text(json.dumps(metadata,indent=2)+'\n')
    with tempfile.TemporaryDirectory(prefix='vessel-composed-tail-') as temp:
        tree=Path(temp);shutil.copytree(source,tree/'vessel')
        for file,name in [('HostRateAdapter.hpp','StereoDecimator'),('HostRateAdapter.hpp','HostRateAdapter'),
                          ('ModalBank.hpp','ModalBank'),('VesselEngine.hpp','VesselEngine')]:
            replace(tree/'vessel'/file,f'class {name} {{',f'class {name} {{\n    friend class ComposedTailHost;')
        replace(tree/'vessel/HostRateAdapter.hpp','    const VesselEngine& engine() const noexcept',
            '    VesselEngine& probeEngine() noexcept { return engine_; }\n    const VesselEngine& engine() const noexcept')
        replace(tree/'vessel/VesselEngine.hpp','    const ModalBank& bowl() const noexcept',
            '    ModalBank& probeBank() noexcept { return bank_; }\n    const ModalBank& bowl() const noexcept')
        binary=tree/'probe'
        subprocess.run([args.compiler,*flags,'-I'+str(tree),'-I'+str(ROOT/'tools/vessel/experiments'),
            str(ROOT/'tools/vessel/probe_composed_tail.cpp'),*map(str,sorted((tree/'vessel').glob('*.cpp'))),
            '-o',str(binary)],check=True)
        with (output/'metrics.csv').open('w') as report:
            subprocess.run([str(binary),'--output',str(output),*(['--quick'] if args.quick else [])],stdout=report,check=True)
    with (output/'metrics.csv').open() as report: rows=list(csv.DictReader(report))
    summary={'cases':len(rows),'max_relative_error':max(float(r['relative_error']) for r in rows),
        'max_peak_error':max(float(r['peak_error']) for r in rows),
        'max_energy_error':max(float(r['energy_error']) for r in rows),
        'max_state_error':max(float(r['state_error']) for r in rows),
        'max_contact_exit_us':max(float(r['max_contact_exit_us']) for r in rows),'timing':[]}
    for row in rows:
        if row['group'] in ('timing','paired'):
            summary['timing'].append({'group':row['group'],'bowl':int(row['bowl']),'factor':int(row['factor']),
                'reference_us':float(row['reference_us']),'candidate_us':float(row['candidate_us']),
                'saving_percent':100*(1-float(row['candidate_us'])/float(row['reference_us']))})
    summary['auditions']=[]
    for path in sorted(output.glob('*_reference.f64')):
        name=path.name.removesuffix('_reference.f64')
        a=np.fromfile(path,dtype=np.float64).reshape(-1,2)
        b=np.fromfile(output/f'{name}_candidate.f64',dtype=np.float64).reshape(-1,2)
        require_finite=np.isfinite(a).all() and np.isfinite(b).all()
        if not require_finite or a.shape!=b.shape:raise RuntimeError('invalid audition capture')
        (b-a).astype('<f8').tofile(output/f'{name}_difference.f64')
        gain=.9/max(np.max(np.abs(a)),np.max(np.abs(b)),1e-30)
        for suffix,samples in [('reference',a),('candidate',b)]:
            with wave.open(str(output/f'{name}_{suffix}.wav'),'wb') as file:
                file.setnchannels(2);file.setsampwidth(2);file.setframerate(48000)
                file.writeframes(np.rint(np.clip(samples*gain,-1,1)*32767).astype('<i2').tobytes())
        summary['auditions'].append({'name':name,'common_gain':float(gain),
            'difference_rms':float(np.sqrt(np.mean((b-a)**2))),'difference_peak':float(np.max(np.abs(b-a)))})
    (output/'summary.json').write_text(json.dumps(summary,indent=2)+'\n')
    print(json.dumps(summary,indent=2))


if __name__=='__main__':main()
