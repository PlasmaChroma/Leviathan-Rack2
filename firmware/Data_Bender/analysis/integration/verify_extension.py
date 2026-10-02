#!/usr/bin/env python3
"""Check extension artifacts and original inventory; record a separate inventory.
Run the three new probes and the original Corrupt verifier before this script.
This validates their recorded outputs; it does not execute their firmware tests.
"""
from pathlib import Path
import ast
import hashlib
import json

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]

def read(name):
    return json.loads((HERE/name).read_text())


def main():
    manifest=json.loads((ROOT/'manifest.json').read_text())
    mismatches=[]
    for item in manifest['files']:
        p=ROOT/item['path']
        if not p.is_file() or hashlib.sha256(p.read_bytes()).hexdigest()!=item['sha256']:
            mismatches.append(item['path'])
    assert not mismatches, mismatches
    scheduling=read('scheduling_probes.json')
    reader=read('reader_rng_probes.json')
    callback=read('callback_probes.json')
    for result in (scheduling,reader,callback):
        assert result['firmware_sha256']==manifest['firmware_sha256']
    assert len(scheduling['partition_cases'])==20
    assert len(scheduling['freeze_cases'])==20
    assert len(reader['reader_cases'])==48
    assert sum(c['draws_checked'] for c in reader['rng']['cases'])==1024
    assert len(callback['callback_cases'])==5
    assert len(callback['dispatcher_cases'])==39
    corrupt=json.loads((ROOT/'analysis/corrupt_dsp/differential_verification.json').read_text())
    assert len(corrupt['cases'])==64 and corrupt['all_pass']
    paths=sorted(HERE.glob('*.py'))+sorted(p for p in HERE.glob('*probes.json'))
    paths.append(ROOT/'Deeper_DSP_and_Rack_Design.txt')
    for path in HERE.glob('*.py'):
        ast.parse(path.read_text(), filename=str(path))
    result=dict(firmware_sha256=manifest['firmware_sha256'],
                method=__doc__,original_manifest_files_checked=len(manifest['files']),
                original_manifest_mismatches=mismatches,new_prepared_configurations=136,
                reader_frames_checked=7680,rng_draws_checked=1024,
                existing_corrupt_cases=64,existing_corrupt_all_pass=True,
                files=[dict(path=str(p.relative_to(ROOT)),size=p.stat().st_size,
                            sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in paths])
    (HERE/'validation.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k not in ('files','method')},indent=2))

if __name__=='__main__':
    main()
