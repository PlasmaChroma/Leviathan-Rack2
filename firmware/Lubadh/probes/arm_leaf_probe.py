#!/usr/bin/env python3
"""Restricted ARM/VFP instruction-stream interpreter for firmware leaf probes.

NOT a complete ARM emulator. Loads actual ELF bytes and reads instruction text
from LLVM's disassembly. Unsupported instructions fail, rather than being skipped.
Uses host libm fma/fmaf for fused operations. No firmware launcher, OS or hardware
is run. Passing probes therefore validates selected arithmetic paths, not the
module, analogue circuitry, scheduler, or a complete floating-point environment.
"""
from pathlib import Path
import re,struct,math,ctypes,sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from inspect_firmware import ELF32
U32=0xffffffff
libm=ctypes.CDLL('libm.so.6')
libm.fmaf.argtypes=[ctypes.c_float]*3;libm.fmaf.restype=ctypes.c_float
libm.fma.argtypes=[ctypes.c_double]*3;libm.fma.restype=ctypes.c_double

def f32(x):return ctypes.c_float(x).value

def signed(x):return x if x<0x80000000 else x-0x100000000

def split_args(s):
 return re.split(r',\s*(?![^\[\]{}]*[\]}])',s)

class ARMLeaf:
 def __init__(self):
  elf=ELF32(ROOT/'extracted/bin/lubadh_main')
  self.mem=bytearray(0x400000)
  for s in elf.sections:
   if s['flags']&2 and s['type']!=8:
    self.mem[s['addr']:s['addr']+s['size']]=elf.section_data(s)
  self.code={}
  for line in (ROOT/'evidence/main_disassembly.txt').read_text().splitlines():
   m=re.match(r'\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(\S+)\s*(.*)',line)
   if m:
    a,w,op,arg=m.groups();self.code[int(a,16)]=(int(w,16),op,arg.split('@')[0].split('<')[0].strip())
  self.r=[0]*16;self.fp=bytearray(256);self.flags=(0,0,0,0);self.vflags=self.flags
  self.r[13]=0x300000;self.steps=0;self.coverage=set()
 def load(self,addr,data):self.mem[addr:addr+len(data)]=data
 def u(self,addr,n=4):return int.from_bytes(self.mem[addr:addr+n],'little')
 def putu(self,addr,x,n=4):self.load(addr,(int(x)&((1<<(8*n))-1)).to_bytes(n,'little'))
 def putf(self,addr,x):self.load(addr,struct.pack('<f',x))
 def getf(self,addr):return struct.unpack_from('<f',self.mem,addr)[0]
 def rid(self,r):return {'sp':13,'lr':14,'pc':15}.get(r,int(r[1:]) if r.startswith('r') else -1)
 def reg(self,r):
  if r=='pc':return self.pc+8
  return self.r[self.rid(r)]
 def setreg(self,r,x):
  self.r[self.rid(r)]=x&U32
  if r=='pc':self.npc=x&U32
 def fpbits(self,x):
  return self.fp[(int(x[1:])*(4 if x[0]=='s' else 8)):][:4 if x[0]=='s' else 8]
 def fpvalue(self,x):return struct.unpack('<f' if x[0]=='s' else '<d',self.fpbits(x))[0]
 def putfpbits(self,x,b):
  off=int(x[1:])*(4 if x[0]=='s' else 8);self.fp[off:off+len(b)]=b
 def putfp(self,x,v):self.putfpbits(x,struct.pack('<f' if x[0]=='s' else '<d',f32(v) if x[0]=='s' else v))
 def val(self,x):return int(x[1:],0) if x.startswith('#') else self.reg(x)
 def fval(self,x):return float(x[1:]) if x.startswith('#') else self.fpvalue(x)
 def address(self,s):
  wb=s.endswith('!');s=s.rstrip('!');inside=s.strip('[]');a=split_args(inside);base=self.reg(a[0]);off=0
  if len(a)>1:off=self.val(a[1])
  addr=(base+off)&U32
  if wb:self.setreg(a[0],addr)
  return addr
 def ok(self,c):
  n,z,carry,v=self.flags
  return [z,not z,carry,not carry,n,not n,v,not v,carry and not z,not carry or z,n==v,n!=v,not z and n==v,z or n!=v,True,True][c]
 def compare(self,a,b,floating=False):
  if floating:
   self.vflags=(0,0,1,1) if math.isnan(a) or math.isnan(b) else ((1,0,0,0) if a<b else ((0,1,1,0) if a==b else (0,0,1,0)))
  else:
   a&=U32;b&=U32;r=(a-b)&U32;self.flags=(bool(r&0x80000000),r==0,a>=b,bool(((a^b)&(a^r))&0x80000000))
 def call(self,addr,max_steps=1000000):
  self.pc=addr;self.r[14]=0xfffffff0
  while self.pc!=0xfffffff0:
   self.steps+=1
   if self.steps>max_steps:raise RuntimeError('Step budget exceeded')
   self.coverage.add(self.pc)
   try:self.step()
   except Exception as exc:raise RuntimeError(f'{self.pc:#x}: {self.code.get(self.pc)}: {exc}') from exc
   self.pc=self.npc
 def step(self):
  word,mn,arg=self.code[self.pc];self.npc=self.pc+4;cond=word>>28
  if not self.ok(cond):return
  parts=mn.split('.');op=parts[0]
  if cond not in (14,15):op=op[:-2]
  a=split_args(arg) if arg else []
  if op in ('b','bl'):
   if op=='bl':self.r[14]=self.pc+4
   self.npc=int(a[0],16);return
  if op=='bx':self.npc=self.reg(a[0]);return
  if op=='nop':return
  if op=='cmp':self.compare(self.reg(a[0]),self.val(a[1]));return
  if op in ('mov','movw','movt'):
   x=self.val(a[1]);self.setreg(a[0],((self.reg(a[0])&65535)|(x<<16)) if op=='movt' else x);return
  if op in ('add','sub','rsb','bic','orr','eor','and'):
   x,y=self.reg(a[1]),self.val(a[2])
   if len(a)>3:
    shift,bits=a[3].split();bits=self.val(bits)
    y=(y<<bits)&U32 if shift=='lsl' else y>>bits if shift=='lsr' else (signed(y)>>bits)&U32
   z={'add':lambda:x+y,'sub':lambda:x-y,'rsb':lambda:y-x,'bic':lambda:x&~y,'orr':lambda:x|y,'eor':lambda:x^y,'and':lambda:x&y}[op]()
   self.setreg(a[0],z);return
  if op in ('asr','lsr','lsl','asrs','lsrs','lsls'):
   x,bits=self.reg(a[1]),self.val(a[2]);base=op[:3]
   z=((signed(x)>>bits) if base=='asr' else x>>bits if base=='lsr' else x<<bits)&U32
   self.setreg(a[0],z)
   if op.endswith('s'):
    n,zero,c,v=self.flags
    if bits:
     c=((x>>(bits-1))&1) if base!='lsl' and bits<=32 else ((x>>(32-bits))&1) if base=='lsl' and bits<=32 else int(base=='asr' and bool(x&0x80000000))
    self.flags=(bool(z&0x80000000),z==0,c,v)
   return
  if op in ('ldm','ldmib','stm','stmib'):
   base=a[0].rstrip('!');addr=self.reg(base)+(4 if op.endswith('ib') else 0)
   for r in a[1].strip('{}').split(', '):
    if op.startswith('ldm'):self.setreg(r,self.u(addr))
    else:self.putu(addr,self.reg(r))
    addr+=4
   if a[0].endswith('!'):self.setreg(base,addr)
   return
  if op in ('push','pop','vpush','vpop'):
   regs=a[0].strip('{}').split(', ');sz=8 if op.startswith('v') else 4
   pushing=op.endswith('push');addr=self.r[13]-sz*len(regs) if pushing else self.r[13]
   start=addr
   for r in regs:
    if op.startswith('v'):
     if pushing:self.load(addr,self.fpbits(r))
     else:self.putfpbits(r,self.mem[addr:addr+sz])
    else:
     if pushing:self.putu(addr,self.reg(r))
     else:self.setreg(r,self.u(addr))
    addr+=sz
   self.r[13]=start if pushing else addr
   return
  if op in ('sdiv','udiv','mul','mla','mls'):
   x,y=self.reg(a[1]),self.reg(a[2])
   if op=='sdiv':z=math.trunc(signed(x)/signed(y)) if y else 0
   elif op=='udiv':z=x//y if y else 0
   elif op=='mul':z=x*y
   elif op=='mla':z=x*y+self.reg(a[3])
   else:z=self.reg(a[3])-x*y
   self.setreg(a[0],z);return
  if op in ('ldrd','strd'):
   addr=self.address(a[2])
   for i in range(2):
    if op=='ldrd':self.setreg(a[i],self.u(addr+4*i))
    else:self.putu(addr+4*i,self.reg(a[i]))
   return
  if op in ('vld1','vst1'):
   addr=self.address(a[1]);regs=a[0].strip('{}').split(', ')
   for v in regs:
    if op=='vld1':self.putfpbits(v,self.mem[addr:addr+8])
    else:self.load(addr,self.fpbits(v))
    addr+=8
   if a[1].endswith('!'):self.setreg(a[1].strip('[]!'),addr)
   return
  if op in ('ldr','ldrb','str','strb'):
   addr=self.address(a[1]);n=1 if op.endswith('b') else 4
   if op.startswith('ldr'):self.setreg(a[0],self.u(addr,n))
   else:self.putu(addr,self.reg(a[0]),n)
   if len(a)>2:
    base=a[1][1:].split(',')[0].rstrip(']');self.setreg(base,self.reg(base)+self.val(a[2]))
   return
  if op in ('vldr','vstr'):
   addr=self.address(a[1]);n=4 if a[0].startswith('s') else 8
   if op=='vldr':self.putfpbits(a[0],self.mem[addr:addr+n])
   else:self.load(addr,self.fpbits(a[0]))
   return
  if op in ('vldmia','vstmia'):
   base=a[0].rstrip('!');addr=self.reg(base)
   for v in a[1].strip('{}').split(', '):
    n=4 if v.startswith('s') else 8
    if op=='vldmia':self.putfpbits(v,self.mem[addr:addr+n])
    else:self.load(addr,self.fpbits(v))
    addr+=n
   if a[0].endswith('!'):self.setreg(base,addr)
   return
  if op=='vmrs':self.flags=self.vflags;return
  if op in ('vcmp','vcmpe'):self.compare(self.fval(a[0]),self.fval(a[1]),True);return
  if op=='vmov':
   if len(parts)>1:self.putfp(a[0],self.fval(a[1]));return
   if a[0].startswith(('r','lr','sp')):self.setreg(a[0],int.from_bytes(self.fpbits(a[1]),'little'))
   else:self.putfpbits(a[0],self.reg(a[1]).to_bytes(4,'little'))
   return
  if op=='vcvt':
   to,fr=parts[1:]
   if to in ('f32','f64'):
    v=self.fpvalue(a[1]) if fr.startswith('f') else int.from_bytes(self.fpbits(a[1]),'little',signed=fr=='s32')
    self.putfp(a[0],float(v))
   else:
    v=math.trunc(self.fpvalue(a[1]));self.putfpbits(a[0],(v&U32).to_bytes(4,'little'))
   return
  if op in ('vabs','vneg','vsqrt'):
   v=self.fpvalue(a[1]);self.putfp(a[0],abs(v) if op=='vabs' else -v if op=='vneg' else math.sqrt(v));return
  if op in ('vadd','vsub','vmul','vnmul','vdiv','vfma','vfms','vfnms','vfnma'):
   x,y=self.fpvalue(a[1]),self.fpvalue(a[2]);acc=self.fpvalue(a[0]);fma=libm.fmaf if a[0].startswith('s') else libm.fma
   if op=='vadd':z=x+y
   elif op=='vsub':z=x-y
   elif op=='vmul':z=x*y
   elif op=='vnmul':z=-x*y
   elif op=='vdiv':z=x/y
   elif op=='vfma':z=fma(x,y,acc)
   elif op=='vfms':z=fma(-x,y,acc)
   elif op=='vfnms':z=fma(x,y,-acc)
   else:z=fma(-x,y,-acc)
   self.putfp(a[0],z);return
  raise NotImplementedError(op)

def run_vector(addr,x,fields):
 cpu=ARMLeaf();obj=0x100000;vec=0x110000;data=0x120000
 for offset,value in fields.items():cpu.putf(obj+offset,value)
 for i,v in enumerate(x):cpu.putf(data+4*i,v)
 for j,v in enumerate((data,data+4*len(x),data+4*len(x))):cpu.putu(vec+4*j,v)
 cpu.r[0]=obj;cpu.r[1]=vec;cpu.call(addr)
 return [cpu.getf(data+4*i) for i in range(len(x))],cpu

if __name__=='__main__':
 y,cpu=run_vector(0x4fc60,[-1,-.5,0,.5,1],{0:.5})
 print(y,cpu.steps)
