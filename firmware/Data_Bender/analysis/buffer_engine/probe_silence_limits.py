#!/usr/bin/env python3
"""Compare .9 and1 Micro Silence endpoints on the original Buffer processor.
Fixed four-way subdivision, constant-one protected buffer and zero liveinput.
The default Window companion case resolves the zero-length-gate boundary.
"""
from pathlib import Path
import json,struct
from unicorn.arm_const import UC_ARM_REG_PC
from probe_engine import Engine,POOL
records=[]
for window,silence,traverse in [(0.,.9,0.),(0.,1.,0.),(.02,.9,0.),(.02,1.,0.),(0.,1.,.5)]:
  e=Engine(N=1000,repeats=2,silence=silence,window=window,traverse=traverse)
  e.wf(0x4c,48000.);e.wf(0x50,48.);e.wf(0x54,48.)
  e.wb(0x11a,1);e.wb(0x11b,1)
  e.u.mem_write(POOL,struct.pack('<f',1.)*65536)
  for i in range(4):e.process(256,kind='constant',value=0.)
  out=e.process(1000,kind='constant',value=0.)[::2]
  records.append(dict(silence=silence,window=window,traverse=traverse,synthetic_nonzero_slice=bool(traverse),snapshot=e.snapshot(0),audibleEnd=e.ri(0x16c),minimum=min(out),maximum=max(out),zero_samples=sum(x==0 for x in out),nonzero_samples=sum(x!=0 for x in out),first_40=list(out[:40]),final_pc=hex(e.u.reg_read(UC_ARM_REG_PC))))
Path(__file__).with_name('micro_silence_limits_probe.json').write_text(json.dumps(records,indent=2)+'\n')
print(json.dumps([{k:v for k,v in r.items() if k not in ['snapshot','first_40']} for r in records],indent=2))
