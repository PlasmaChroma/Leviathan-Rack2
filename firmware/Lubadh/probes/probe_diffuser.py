#!/usr/bin/env python3
"""Probe original TapeAllpass routine against a signed feedforward-delay model.
Fields are initialized explicitly; no complete Channel constructor is executed.
"""
from arm_leaf_probe import ARMLeaf,ROOT,f32
import csv,json,math
cpu=ARMLeaf();obj=0x100000;vec=0x110000;data=0x120000
lengths=[68,159,251,375];n=1200;amount=.7
cpu.putf(obj,amount)
for i,length in enumerate(lengths):cpu.putu(obj+0x1794+4*i,length)
for i,v in enumerate((data,data+4*n,data+4*n)):cpu.putu(vec+4*i,v)
cpu.putf(data,1.)
cpu.r[0]=obj;cpu.r[1]=vec;cpu.call(0x4fce8)
bufs=[[0.]*length for length in lengths];ptr=[0]*4;rows=[]
for i in range(n):
 x=1. if i==0 else 0.;d=[b[p] for b,p in zip(bufs,ptr)]
 expected=(1-amount)*x+amount*.175*(x+sum(d))
 writes=[x,x-d[0],x+d[0]-d[1],x+d[0]+d[1]-d[2]]
 for j in range(4):bufs[j][ptr[j]]=writes[j];ptr[j]=(ptr[j]+1)%lengths[j]
 actual=cpu.getf(data+4*i);error=abs(expected-actual)
 assert error<2e-7,(i,expected,actual,error)
 rows.append([i,expected,actual,error])
with (ROOT/'probes/diffuser_impulse_vectors.csv').open('w',newline='') as f:
 w=csv.writer(f);w.writerow(['sample','model','firmware_instruction_probe','absolute_error']);w.writerows(rows)
result=dict(method='Restricted ARM/VFP instruction-stream probe; explicitly initialized object and original function 0x4fce8',checks=n,status='PASS',max_abs_error=max(x[-1] for x in rows),lengths=lengths,amount=amount,nonzero_samples=[x[0] for x in rows if x[2]!=0],distinct_instruction_addresses=len(cpu.coverage),limitations='Not complete firmware or hardware execution. Confirms this routine for this test input and amount, not all initialization/configuration paths.')
(ROOT/'probes/diffuser_probe_results.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
