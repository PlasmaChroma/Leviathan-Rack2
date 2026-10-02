#!/usr/bin/env python3
"""Validate recorded character probes and inventory their artifacts.
Run the numerical probe scripts first. This checks results, not a fresh emulation.
"""
from pathlib import Path
import ast,hashlib,json,wave
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]

def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def read(name):return json.loads((HERE/name).read_text())

def main():
    original=json.loads((ROOT/'manifest.json').read_text())
    prior=json.loads((ROOT/'analysis/integration/validation.json').read_text())
    for manifest in (original,prior):
        for item in manifest['files']:
            assert digest(ROOT/item['path'])==item['sha256'],item['path']
    time=read('time_memory_probes.json')['cases']
    assert len(time)==10
    assert all(sum(s['frames'] for s in c['stages'])==76800 for c in time)
    for c in time:
        for s in c['stages']:
            assert all(sum(reads.values())==s['frames'] for reads in s['reads'])
            assert all(state['rate']==c['speed'] for event in s['states'] for state in event['channels'])
            if c['frozen']:
                assert all(state['frozen']==1 for event in s['states'] for state in event['channels'])
    anchor1=next(c for c in time if c['speed']==1. and c['frozen'] and c['initial_read_bank']==1)
    assert anchor1['stages'][1]['reads'][0]['fresh_tail']==11340
    assert anchor1['stages'][3]['reads'][0]['fresh_bank1']==1807
    slow=next(c for c in time if c['speed']==.125 and c['frozen'] and c['initial_read_bank']==1 and c['clock_requests'])
    assert not slow['stages'][1]['reads'][0].get('fresh_tail',0)
    assert time[-1]['stages'][1]['reads'][0]['fresh_tail']==8448
    boundary=read('history_boundary_probes.json')['cases']
    assert len(boundary)==2 and all(c['changed_count']==94 for c in boundary)
    long=read('vinyl_long_probes.json')['cases']
    assert len(long)==3 and all(c['frames']==288096 for c in long)
    assert all(c['max_abs_error']==0 and c['unequal_float32_samples']==0 for c in long)
    assert [c['rollovers'][0]['frame'] for c in long]==[48013,48013,48205]
    rare=read('vinyl_rare_probes.json')['cases']
    assert len(rare)==5 and all(c['max_error']==0 for c in rare)
    for c in rare:
        assert len(c['samples'])==31
        assert c['samples'][0]['dust'][c['layer']]<0
    with wave.open(str(HERE/'vinyl_silence_48k.wav'),'rb') as w:
        assert (w.getnchannels(),w.getsampwidth(),w.getframerate(),w.getnframes())==(2,2,48000,288096)
    paths=sorted(HERE.glob('*.py'))+sorted(HERE.glob('*probes.json'))+[HERE/'vinyl_silence_48k.wav',ROOT/'Time_Freeze_and_Vinyl_Character.txt']
    for p in HERE.glob('*.py'):ast.parse(p.read_text(),filename=str(p))
    for p in HERE.glob('*probes.json'):
        r=json.loads(p.read_text())
        if 'firmware_sha256' in r:assert r['firmware_sha256']==original['firmware_sha256']
    result=dict(method=__doc__,firmware_sha256=original['firmware_sha256'],
        original_files_unchanged=len(original['files']),prior_extension_files_unchanged=len(prior['files']),
        time_trajectory_cases=10,time_stereo_frames=768000,history_boundary_cases=2,
        vinyl_natural_cases=3,vinyl_natural_stereo_frames=864288,
        vinyl_natural_bit_exact=True,vinyl_forced_layer_cases=5,vinyl_forced_stereo_frames=155,
        all_recorded_checks_pass=True,files=[dict(path=str(p.relative_to(ROOT)),size=p.stat().st_size,sha256=digest(p)) for p in paths])
    (HERE/'validation.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k not in ('files','method')},indent=2))

if __name__=='__main__':main()
