#!/usr/bin/env python3
"""Validate repeat captures and aggregate isolated-engine/callback experiments."""
import csv
import json
from pathlib import Path
import statistics
import sys
import numpy as np
root=Path(sys.argv[1]);result={}
variants={'gaussian':'gaussian','tanh':'hybrid','reciprocals':'reciprocals','both':'cubic','reciprocal-tanh':'reciprocal_tanh'}
for folder,variant in variants.items():
    base=root/folder
    rows=json.loads((base/'comparison.json').read_text())
    for row in rows:
        key=(row['case'],str(row['quality'])); stem=f'{key[0]}_{key[1]}.f64'
        signals=[]
        for name in ['analytic',variant]:
            p,q=base/name/'0'/stem,base/name/'1'/stem
            assert p.read_bytes()==q.read_bytes(),'nondeterministic capture'
            data=np.fromfile(p,dtype=np.float64)
            assert data.size==8*48000*2 and np.isfinite(data).all()
            signals.append(data)
            for repeat in ['0','1']:
                metrics=list(csv.DictReader((base/name/repeat/'metrics.csv').open()))
                metric=next(r for r in metrics if (r['case'],r['quality'])==key)
                assert int(metric['faults'])==0,'fixture fault'
                if name=='analytic' and repeat=='0':reference=metric
                assert all(metric[k]==reference[k] for k in ['start_rate','end_rate','rate_changes'])
        a,b=signals; diff=b-a
        row['rms_error_percent']=float(100*np.linalg.norm(diff)/np.linalg.norm(a))
        row['peak_error_percent']=float(100*np.max(np.abs(diff))/np.max(np.abs(a)))
    result[folder]={'engine_median_saving_percent':statistics.median(r['saving_percent'] for r in rows),
                    'engine_min_saving_percent':min(r['saving_percent'] for r in rows),
                    'engine_max_saving_percent':max(r['saving_percent'] for r in rows),
                    'max_rms_error_percent':max(r['rms_error_percent'] for r in rows),
                    'max_peak_error_percent':max(r['peak_error_percent'] for r in rows),
                    'max_ledger_residual':max(r['max_ledger_residual'] for r in rows),
                    'rows':rows}
result['callbacks']=json.loads((root/'callbacks/summary.json').read_text())
(root/'summary.json').write_text(json.dumps(result,indent=2,allow_nan=False)+'\n')
for name,row in result.items():
    if name!='callbacks':print(name,{k:v for k,v in row.items() if k!='rows'})
