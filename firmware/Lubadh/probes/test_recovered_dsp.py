#!/usr/bin/env python3
"""Compare seven selected firmware routines in six groups (instruction-stream probes) with
independent mathematical models. Finite, normal-range arithmetic only.
The interpreter is supplied for audit. These are NOT hardware golden vectors.
"""
from arm_leaf_probe import ARMLeaf,run_vector,f32,signed,ROOT
import math,random,json,csv
A=f32(28.274333953857422);B=f32(9.42477798461914)

def rational(x):return max(-1.,min(1.,x*(A+x*x)/(A+B*x*x)))
def softclip(x,k,c):
 k,c,x=map(f32,(k,c,x))
 if k<=.01:return rational((1+c)*x)
 x=max(-1.,min(1.,x));a=max(0.,abs(x)-k)
 y=x if a==0 else math.copysign(k+a/(1+(a/(1-k))**2),x)
 return y*(1+c*(2/(1+k)-1))
def cubic(a,b,c,d,t):return b+t*((c-b)-(1-t)/6*(d+2*a-3*b+t*(d-a-3*(c-b))))

def setup(x):
 cpu=ARMLeaf();obj=0x100000;vec=0x110000;data=0x120000
 for i,v in enumerate(x):cpu.putf(data+4*i,v)
 for j,v in enumerate((data,data+4*len(x),data+4*len(x))):cpu.putu(vec+4*j,v)
 return cpu,obj,vec,data

def main():
 rng=random.Random(210);rows=[];result=[];allcov=set()
 def check(name,expected,actual,cpu,meta,tol=2e-6):
  err=abs(expected-actual);assert math.isfinite(actual) and err<=tol,(name,expected,actual,meta,err)
  rows.append(dict(test=name,parameters=json.dumps(meta),expected=expected,firmware_probe=actual,abs_error=err))
  allcov.update(cpu.coverage)
 x=[f32(v) for v in [-4,-1,-.99,-.5,-.1,0,.1,.5,.99,1,4]+[rng.uniform(-3,3) for _ in range(129)]]
 for k in [0,.0099,.01,.0101,.1,.3,.9,1]:
  for c in [0,.2,1]:
   cpu,obj,vec,data=setup(x);cpu.r[0]=obj;cpu.putfp('s0',k);cpu.putfp('s1',c);cpu.call(0x4ffe0)
   flag=cpu.u(obj+8,1);assert flag==int(f32(k)<=.01)
   cpu.r[0]=obj;cpu.r[1]=vec;cpu.call(0x4fea8)
   for i,v in enumerate(x):check('SoftClipper',softclip(v,k,c),cpu.getf(data+4*i),cpu,{'x':v,'knee':k,'compensation':c})
 for w in [0,.125,.5,1]:
  out,cpu=run_vector(0x4fc60,x,{0:w})
  for v,y in zip(x,out):check('TapeCompander',(1-w)*v+w*v*abs(v),y,cpu,{'x':v,'wear':w})
 # LP and HP share the same lowpass state and differ only in returned signal.
 impulse=[1.]+[0.]*95
 for coeff in [.25,1,5,100]:
  for addr,name in [(0x4b0b0,'OnePoleLP'),(0x4b104,'OnePoleHP')]:
   out,cpu=run_vector(addr,impulse,{0:coeff,4:0,8:0});prevx=prevy=0.
   for i,(v,y) in enumerate(zip(impulse,out)):
    lp=(v+prevx-(1-coeff)*prevy)/(1+coeff)
    check(name,lp if name=='OnePoleLP' else v-lp,y,cpu,{'coefficient':coeff,'sample':i})
    prevx,prevy=v,lp
 # InputBuffer object carries its vector at +4.
 datax=[f32(rng.uniform(-1,1)) for _ in range(64)]
 for t in [0.,.125,.5,.875,1.25,7.333,17.9,33.2,60.]:
  cpu,obj,vec,data=setup(datax)
  for j,v in enumerate((data,data+4*len(datax),data+4*len(datax))):cpu.putu(obj+4+4*j,v)
  cpu.r[0]=obj;cpu.putfp('s0',t);cpu.call(0x38ab4);q=f32(t);i=max(0,min(len(datax)-4,math.trunc(q)))
  check('InputBufferCubic',cubic(*datax[i:i+4],q-i),cpu.fpvalue('s0'),cpu,{'position':t})
 for coeff in [0,.1,.5,1]:
  cpu=ARMLeaf();obj=0x100000;cpu.putf(obj,coeff);prev=0
  for v in [0,4095,0,2048,4095,4095,1]:
   cpu.r[0]=obj;cpu.r[1]=v;cpu.call(0x6c104);actual=signed(cpu.r[0])
   # Independently emulate the documented intermediate float32 multiply and fused add.
   c=f32(coeff);expected=math.trunc(f32(v*c+f32(prev*f32(1-c))))
   check('ADCIntegerSmoothing',expected,actual,cpu,{'coefficient':coeff,'input':v,'previous':prev},tol=0)
   prev=actual
 with (ROOT/'probes/leaf_probe_vectors.csv').open('w',newline='') as f:
  w=csv.DictWriter(f,fieldnames=list(rows[0]));w.writeheader();w.writerows(rows)
 names=sorted(set(r['test'] for r in rows))
 for n in names:
  rr=[r for r in rows if r['test']==n]
  result.append({'test':n,'checks':len(rr),'max_abs_error':max(r['abs_error'] for r in rr),'status':'PASS'})
 out={'method':'restricted LLVM-disassembly-driven ARM/VFP interpreter; not hardware or complete firmware execution','seed':210,'comparisons':len(rows),'distinct_instruction_addresses_exercised':len(allcov),'results':result,'coverage_addresses':[hex(x) for x in sorted(allcov)],'limitations':['No operating system or hardware execution','Interpreter is independently written and not a certified ARM emulator','No NaN/Inf, FPSCR trap, denormal flush, or analogue behavior claims','No transport, scheduler, reverb or complete module verification in this probe suite']}
 (ROOT/'probes/probe_results.json').write_text(json.dumps(out,indent=2)+'\n')
 print(json.dumps({k:v for k,v in out.items() if k!='coverage_addresses'},indent=2))
if __name__=='__main__':main()
