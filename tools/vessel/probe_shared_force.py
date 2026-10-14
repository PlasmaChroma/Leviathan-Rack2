#!/usr/bin/env python3
"""Screen shared contact excitation in isolated source copies; requires NumPy.

No production sources are rewritten. Fixed Reference quality, 48 kHz host,
192 kHz internal, constant descriptors/controls, and independent force-port
energy accounting. Outputs are pickup velocities, not Rack volts. This is a
quality/performance screen, not a calibrated physical or listening certification.
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

ROOT = Path(__file__).resolve().parents[2]
RATE = 48000


def replace_once(path, needle, replacement):
    text = path.read_text()
    if text.count(needle) != 1:
        raise RuntimeError(f'Experiment hook changed in {path.name}; review explicitly')
    path.write_text(text.replace(needle, replacement))


def peaks(signal, pitch):
    """Local maxima within the fundamental region; not a modal tracker."""
    signal = signal - np.mean(signal)
    magnitude = np.abs(np.fft.rfft(signal * np.hanning(len(signal))))
    hz = np.fft.rfftfreq(len(signal), 1 / RATE)
    indices = np.flatnonzero((magnitude[1:-1] > magnitude[:-2]) &
                             (magnitude[1:-1] >= magnitude[2:])) + 1
    indices = indices[(hz[indices] > .6 * pitch) & (hz[indices] < 1.4 * pitch)]
    indices = sorted(indices, key=lambda i: magnitude[i], reverse=True)[:5]
    result = []
    for i in indices:
        a, b, c = np.log(np.maximum(magnitude[i-1:i+2], 1e-300))
        offset = .5 * (a-c) / (a-2*b+c) if a-2*b+c else 0
        result.append({'hz': float((i+offset)*RATE/len(signal)),
                       'relative_db': float(20*np.log10(magnitude[i]/max(magnitude[indices[0]], 1e-300)))})
    return result


def stats(audio, pitch):
    rms = np.sqrt(np.mean(audio*audio, axis=0))
    return {'rms': rms.tolist(),
            'right_minus_left_db': float(20*np.log10(max(rms[1],1e-300)/max(rms[0],1e-300))),
            'peaks': [peaks(audio[:, channel], pitch) for channel in (0, 1)]}


def wav(path, samples):
    with wave.open(str(path), 'wb') as file:
        file.setnchannels(2)
        file.setsampwidth(2)
        file.setframerate(RATE)
        file.writeframes(np.rint(np.clip(samples,-1,1)*32767).astype('<i2').tobytes())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT/'build/vessel-shared-force')
    parser.add_argument('--seconds', type=float, default=60)
    parser.add_argument('--repeats', type=int, default=3)
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--verify-only', action='store_true',
                        help='check audited/fused equality with strikes, reversal and high pressure; no long capture')
    args = parser.parse_args()
    if not 20 <= args.seconds <= 120 or not 1 <= args.repeats <= 10:
        parser.error('seconds must be 20..120; repeats 1..10')
    output = args.output.resolve()
    output.mkdir(parents=True,exist_ok=True)
    source = ROOT/'src/vessel'
    digest = hashlib.sha256()
    for path in sorted(source.iterdir()):
        if path.suffix in ('.cpp','.hpp'):
            digest.update(path.name.encode()); digest.update(path.read_bytes())
    flags = ['-std=c++11','-O3','-march=nehalem','-Wall','-Wextra',
             '-fno-fast-math','-fno-unsafe-math-optimizations']
    with tempfile.TemporaryDirectory(prefix='vessel-shared-force-') as temp:
        tree = Path(temp)
        shutil.copytree(source,tree/'vessel')
        replace_once(tree/'vessel/VesselEngine.hpp', '    EngineFrame step() noexcept;',
                     '    EngineFrame step() noexcept;\n'
                     '    bool probeCaptureForce = false;\n    ModalVector probeForce {};')
        replace_once(tree/'vessel/VesselEngine.cpp', '    EngineFrame frame;',
                     '    EngineFrame frame;\n    if (probeCaptureForce) probeForce = {};')
        replace_once(tree/'vessel/VesselEngine.cpp',
                     '    const auto modalAudit = bank_.commit(free, force, audit_);',
                     '    if (probeCaptureForce) probeForce = force;\n'
                     '    const auto modalAudit = bank_.commit(free, force, audit_);')
        replace_once(tree/'vessel/ModalBank.hpp', '    ModalVector freeMidpoint() const noexcept;',
                     '''    bool probeAdvanceForced(const ModalVector& force, const ModalVector& observer,
                            double& velocity) noexcept {
        velocity = 0; bool valid = true;
        for (std::size_t j=0; j<count_; ++j) {
            auto& s=states_[j];
            const double free=(s.y-hotA_[j]*s.x)*hotInverseD_[j];
            const double mid=free+hotWeight_[j]*force[j];
            s.x += h_*hotOmega_[j]*mid;
            s.y = 2.0*mid-s.y;
            valid = valid && std::isfinite(s.x) && std::isfinite(s.y);
            velocity += observer[j]*s.y;
        }
        return valid && std::isfinite(velocity);
    }
    ModalVector freeMidpoint() const noexcept;''')
        replace_once(tree/'vessel/ModalBank.hpp', '#include "Types.hpp"',
                     '#include "Types.hpp"\n#include <cmath>')
        binary = tree/'probe'
        subprocess.run([args.compiler,*flags,'-I'+str(tree),
                        str(ROOT/'tools/vessel/probe_shared_force.cpp'),
                        *map(str,sorted((tree/'vessel').glob('*.cpp'))),'-o',str(binary)],check=True)
        with (output/'verification.txt').open('w') as report:
            subprocess.run([str(binary),'--verify'],stdout=report,check=True)
        (output/'verification_environment.json').write_text(json.dumps({
            'source_sha256':digest.hexdigest(),'flags':flags,
            'harness_sha256':hashlib.sha256((ROOT/'tools/vessel/probe_shared_force.cpp').read_bytes()).hexdigest(),
            'runner_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'compiler':subprocess.check_output([args.compiler,'--version'],text=True).splitlines()[0],
            'scope':'4096 host frames per fixture; forced strikes, reversal and 15 N pressure; audited/fused exact output/energy equality.'
        },indent=2)+'\n')
        if args.verify_only:
            print(f'Audited/fused checks passed; report: {output / "verification.txt"}')
            return
        with (output/'metrics.csv').open('w') as report:
            subprocess.run([str(binary),str(output),str(args.seconds),str(args.repeats)],
                           stdout=report,check=True)
    with (output/'metrics.csv').open() as report:
        metrics = list(csv.DictReader(report))
    comparisons=[]
    for row in metrics:
        case=row['case']; pitch=float(row['pitch']); impact=bool(int(row['impact']))
        ref=np.fromfile(output/f'{case}_reference.f64',dtype=np.float64).reshape(-1,2)
        shared=np.fromfile(output/f'{case}_shared.f64',dtype=np.float64).reshape(-1,2)
        if ref.shape!=shared.shape or not np.isfinite(ref).all() or not np.isfinite(shared).all():
            raise RuntimeError('invalid capture')
        windows={'early':(.1, min(4,args.seconds-10)),
                 'late':(args.seconds-15,args.seconds-5),
                 'tail':(args.seconds-4,args.seconds)}
        if impact:
            windows['attack']=(.04,.3)
        compare={'case':case,'pitch':pitch,'delta':float(row['delta']),
                 'impact':impact,
                 'cpu_saving_percent':100*(1-float(row['shared_us'])/float(row['reference_us'])),
                 'windows':{}}
        for name,(start,end) in windows.items():
            a=ref[int(start*RATE):int(end*RATE)]
            b=shared[int(start*RATE):int(end*RATE)]
            sa,sb=stats(a,pitch),stats(b,pitch)
            compare['windows'][name]={'reference':sa,'shared':sb,
                'right_level_change_db':float(20*np.log10(max(sb['rms'][1],1e-300)/max(sa['rms'][1],1e-300)))}
        # Preserve relative level in raw pairs; only one common safety gain.
        start,end=(0,8) if impact else windows['late']
        a=ref[int(start*RATE):int(end*RATE)]
        b=shared[int(start*RATE):int(end*RATE)]
        gain=.9/max(float(np.max(np.abs(a))),float(np.max(np.abs(b))),1e-30)
        wav(output/f'{case}_reference_raw.wav',a*gain)
        wav(output/f'{case}_shared_raw.wav',b*gain)
        match=float(np.sqrt(np.mean(a*a))/max(np.sqrt(np.mean(b*b)),1e-30))
        safety=.9/max(float(np.max(np.abs(a))),float(np.max(np.abs(b*match))),1e-30)
        wav(output/f'{case}_reference_matched.wav',a*safety)
        wav(output/f'{case}_shared_matched.wav',b*match*safety)
        compare['audition']={'start_s':start,'end_s':end,'raw_common_gain':gain,
                             'matched_shared_gain':match,'matched_common_gain':safety,
                             'matching':'Whole-stereo RMS; channel imbalance preserved. PCM16 preview only; full f64 captures retained.'}
        comparisons.append(compare)
    (output/'comparison.json').write_text(json.dumps(comparisons,indent=2)+'\n')
    metadata={'source_sha256':digest.hexdigest(),'compiler':subprocess.check_output([args.compiler,'--version'],text=True).splitlines()[0],
              'platform':platform.platform(),'flags':flags,'seconds':args.seconds,'repeats':args.repeats,
              'harness_sha256':hashlib.sha256((ROOT/'tools/vessel/probe_shared_force.cpp').read_bytes()).hexdigest(),
              'runner_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'host_hz':RATE,'internal_hz':192000,'speed_rps':.4,'pressure_N':2.5,'velocity':.5,
              'timing':'3 s settled-state copies, auditing/I/O disabled, alternating order, unpinned. Impact cases measure late tails, not contact cost. Instrumentation branch remains in both cores.',
              'scope':'Fixed tuning/geometry only. No fade/wake, retuning, reversals, high-pressure sweep, or aliasing certification.'}
    (output/'environment.json').write_text(json.dumps(metadata,indent=2)+'\n')
    print(f'Reports and auditions written to {output}')
    for row in comparisons:
        late=row['windows']['late']
        print(f"{row['case']}: CPU {row['cpu_saving_percent']:.1f}% saving; late right level {late['right_level_change_db']:+.1f} dB")


if __name__=='__main__':
    main()
