#!/usr/bin/env python3
"""Validate paired plain/profile CSVs and summarize work, not CPU time."""
import csv
import json
import sys
from pathlib import Path

plain, profile, output = map(Path, sys.argv[1:])
def read(path):
    with path.open() as stream:
        reader = csv.DictReader(stream)
        assert len(reader.fieldnames) == len(set(reader.fieldnames)), 'duplicate CSV fields'
        return list(reader)
a, b = [read(p) for p in (plain, profile)]
assert len(a) == len(b) == 480
identity = ['bowl','mallet','quality','separation','speed','phase','frames','internal_rate','fingerprint','max_energy_residual']
for x, y in zip(a,b):
    assert all(x[k] == y[k] for k in identity), 'trajectory/fixture mismatch'
    for kind in ['rub','coupled']:
        n = lambda k: int(y[kind+'_'+k])
        assert n('solves') == n('converged')
        assert n('laws') == n('newton')+n('fallback')+n('converged')-n('zero_load')
    assert int(y['outer_trials']) == int(y['coupled_solves'])
summary = {'fixtures':96, 'phase_rows':len(b), 'trajectory_fingerprints_match':True,
           'scope':'Operation counts, not percentages of CPU time. 48 kHz host, 0.002/0.2/1.5 rev/s, pressure 2.5, default pitch, audit enabled.', 'phases':{}}
def aggregate(rows):
    sums = {k:sum(int(r[k]) for r in rows) for k in b[0] if k not in identity and not k.startswith('laws_per_callback')}
    frames = sum(int(r['frames']) for r in rows)
    result={'host_callbacks':frames,'totals':sums}
    for kind in ['rub','coupled']:
        laws=sums[kind+'_laws']; solves=sums[kind+'_solves']
        result[kind]={'laws_per_solve':laws/max(1,solves),
                      'fallback_percent_of_steps':100*sums[kind+'_fallback']/max(1,sums[kind+'_newton']+sums[kind+'_fallback']),
                      'exp_percent_of_laws':100*sums[kind+'_exp']/max(1,laws),
                      'tanh_percent_of_laws':100*sums[kind+'_tanh']/max(1,laws),
                      'tanh_lookup_percent_of_laws':100*sums.get(kind+'_tanh_lookups',0)/max(1,laws),
                      'gaussian_core_percent':100*sums[kind+'_gaussian_core']/max(1,laws),
                      'gaussian_tail_percent':100*sums[kind+'_gaussian_tail']/max(1,laws)}
    result['laws_per_host_callback']=(sums['rub_laws']+sums['coupled_laws'])/frames
    result['coupled_inner_solves_per_outer_solve']=sums['coupled_solves']/max(1,sums['outer_solves'])
    result['coupled_laws_per_outer_solve']=sums['coupled_laws']/max(1,sums['outer_solves'])
    worst=max(rows,key=lambda r:int(r['laws_per_callback_max']))
    result['worst_callback']={k:worst[k] for k in ['bowl','mallet','quality','separation','speed','laws_per_callback_max']}
    return result
for phase in ['startup','sustained','overlap','release','restart']:
    summary['phases'][phase]=aggregate([r for r in b if r['phase']==phase])
summary['by_speed']={}
for speed in sorted(set(r['speed'] for r in b),key=float):
    summary['by_speed'][speed]={phase:aggregate([r for r in b if r['speed']==speed and r['phase']==phase])
                              for phase in summary['phases']}
output.write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps(summary,indent=2))
