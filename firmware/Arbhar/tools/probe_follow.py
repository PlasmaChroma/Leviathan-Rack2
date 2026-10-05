#!/usr/bin/env python3
"""Restricted instruction-text execution of calculateFollowSpeed ONLY.
Not an ARM emulator, Pd host, firmware boot, or hardware test. Finite inputs only.
Supports only instructions actually present; fails closed on unsupported text.
"""
from pathlib import Path
import re,struct,csv,json,math,hashlib,sys
from extract_evidence import ELF32
R=Path(__file__).resolve().parents[1]
f32=lambda x:struct.unpack('<f',struct.pack('<f',x))[0]
class LeafProbe:
 def __init__(self):
  self.e=ELF32(R/'extracted/arbhar_gpio~.pd_linux');self.code={}
  p=R/'evidence/elf/arbhar_gpio~.pd_linux/functions/calculateFollowSpeed.asm'
  for l in p.read_text().splitlines():
   m=re.match(r'\s*([0-9a-f]+):\s+[0-9a-f]{8}\s+(\S+)\s+(.*)',l)
   if m:self.code[int(m[1],16)]=(m[2],m[3].split('@')[0].strip())
 def run(self,raw,mode):
  s={'s0':f32(raw)};r={'r0':int(mode)};pc=0x3f10;flags={'N':False,'Z':False,'C':False,'V':False};fp=flags.copy();steps=0
  def cond(c):
   n,z,k,v=(flags[x] for x in ('N','Z','C','V'))
   return {'':True,'eq':z,'ne':not z,'pl':not n,'mi':n,'ge':n==v,'lt':n!=v,'gt':not z and n==v,'le':z or n!=v,'hi':k and not z,'ls':not k or z}.get(c,False)
  def v(a):return float(a[1:]) if a.startswith('#') else s[a]
  while steps<200:
   steps+=1;op,args=self.code[pc];nxt=pc+4
   if op=='bx':return s['s0'],steps
   if op=='cmp':
    a,b=[x.strip() for x in args.split(',')];a=r[a];b=int(b[1:]);t=a-b;flags=dict(N=t<0,Z=t==0,C=a>=b,V=False)
   elif op=='vmrs':flags=fp.copy()
   elif op.startswith('b'):
    if cond(op[1:]):nxt=int(args.split()[0],16)
   elif op=='vldr':
    m=re.match(r'(s\d+), \[pc, #(-?\d+)\]',args)
    if not m:raise ValueError((op,args))
    s[m[1]]=struct.unpack('<f',self.e.readva(pc+8+int(m[2]),4))[0]
   elif op.startswith('vcmp'):
    a,b=[x.strip() for x in args.split(',')];a=v(a);b=v(b)
    if not math.isfinite(a+b):raise ValueError('finite inputs only')
    fp=dict(N=a<b,Z=a==b,C=a>=b,V=False)
   else:
    stem=op.split('.')[0];m=re.fullmatch(r'(vmov|vadd|vsub|vmul|vmla|vmls|vnmla|vnmls)(eq|ne|gt|ge|lt|le|hi|ls|mi|pl)?',stem)
    if not m:raise ValueError((hex(pc),op,args))
    base,c=m.groups()
    if cond(c or ''):
     a=[x.strip() for x in args.split(',')];d=a[0];x=v(a[1]);y=v(a[2]) if len(a)>2 else 0
     if base=='vmov':out=x
     elif base=='vadd':out=x+y
     elif base=='vsub':out=x-y
     elif base=='vmul':out=x*y
     else:
      product=f32(x*y)
      out={'vmla':lambda:s[d]+product,'vmls':lambda:s[d]-product,'vnmla':lambda:-s[d]-product,'vnmls':lambda:-s[d]+product}[base]()
     s[d]=f32(out)
   pc=nxt
  raise RuntimeError('instruction budget exceeded')
def formula(raw,mode):
 # Algebraic form, distinct from the register-by-register interpreter.
 u=raw if mode==2 else 4095-raw
 if mode==1:u*=2
 if u<1100:return 1+19*(1-u/1100)**2
 if u<1300:return 1.
 if u<4095:return 1-(u-1300)/2795
 if u<=6890:return -1+(6890-u)/2795
 if u<=7090:return -1.
 return -(1+19*(1-(8190-u)/1100)**2)
if __name__=='__main__':
 p=LeafProbe();rows=[];err=0;maxsteps=0
 for mode in range(3):
  for raw in range(4096):
   actual,n=p.run(raw,mode);expected=formula(raw,mode);d=abs(actual-expected);err=max(err,d);maxsteps=max(maxsteps,n)
   if d>1e-5:raise AssertionError((mode,raw,actual,expected,d))
   rows.append((mode,raw,actual,expected))
 with (R/'tables/follow_probe.csv').open('w',newline='') as f:
  w=csv.writer(f);w.writerow(['mode','input_12bit','instruction_text_result','algebraic_result']);w.writerows(rows)
 result={'routine':'calculateFollowSpeed','binary':'arbhar_gpio~.pd_linux','VA':'0x3f10','binary_sha256':hashlib.sha256(p.e.data).hexdigest(),'comparisons':len(rows),'failures':0,'max_absolute_error':err,'tolerance':1e-5,'max_instructions_per_call':maxsteps,'scope':'Restricted finite-input ARM/VFP instruction-text interpreter; NOT real ARM execution or firmware/hardware equivalence.'}
 (R/'tables/follow_probe_results.json').write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2))
