#!/usr/bin/env python3
"""Boundary probes for window, Freeze protection, and transition latching."""
from pathlib import Path
import json,struct
from probe_engine import Engine,POOL

def fill(e):
 one=struct.pack('<f',1.)
 e.u.mem_write(POOL,one*32768);e.u.mem_write(POOL+32768*4,one*32768)

def window_case(N,w):
 e=Engine(N=N,window=w)
 e.wf(0x4c,48000.);e.wf(0x50,48000./N);e.wf(0x54,48000./N)
 fill(e);e.wb(0x11a,1);e.wb(0x11b,1)
 out=e.process(n=N,kind='constant',value=0.)
 indices=sorted(set([0,1,22,23,24,238,239,240,518,519,520,758,759,760,975,976,998,N-1]))
 return dict(N=N,window=w,samples={str(i):out[2*i] for i in indices if i<N},maximum=max(out),minimum=min(out),final=e.snapshot(0))

def freeze_case():
 e=Engine(N=1000,window=0.)
 fill(e);e.process(n=100,kind='constant',value=.2)
 before=e.snapshot(0);e.wb(0x11b,1)
 e.process(n=100,kind='constant',value=.4)
 request_only=e.snapshot(0)
 e.wb(0x234,1);e.wb(0x235,1)
 for _ in range(4):e.process(n=256,kind='constant',value=.6)
 after_transition=e.snapshot(0)
 protected_before=bytes(e.u.mem_read(POOL,8000));history_before=bytes(e.u.mem_read(POOL+8000,32768*4-8000))
 e.process(n=256,kind='constant',value=.8)
 return dict(before=before,request_only=request_only,after_transition=after_transition,final=e.snapshot(0),protected_unchanged=protected_before==bytes(e.u.mem_read(POOL,8000)),history_unchanged=history_before==bytes(e.u.mem_read(POOL+8000,32768*4-8000)),events=e.events)

cases=[window_case(1000,w) for w in [0,.015,.015001,.02,1.]]
cases += [window_case(N,.02) for N in [100,300,480]]
r=dict(window_cases=cases,freeze=freeze_case())
Path(__file__).with_name('window_freeze_probes.json').write_text(json.dumps(r,indent=2)+'\n')
print(json.dumps(dict(windows=[dict(N=x['N'],window=x['window'],minimum=x['minimum'],maximum=x['maximum'],samples=x['samples']) for x in cases],freeze={k:v for k,v in r['freeze'].items() if k!='events'}),indent=2))
