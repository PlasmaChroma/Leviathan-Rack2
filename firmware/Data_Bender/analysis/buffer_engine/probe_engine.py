#!/usr/bin/env python3
"""Execute original Buffer::Init/Process Thumb code on small emulated buffers.
Scalars and tables stay original. Isolated libc rand/pow/memset replacements
are explicit; this does not emulate codec, interrupts, analog I/O or timing.
"""
from pathlib import Path
from firmware_input import firmware_path
import struct,json,math
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[2];BASE=0x08000000
OBJ=0x20000000;IN=0x20010000;OUT=0x20030000;STOP=0x200e0000;POOL=0x30000000

def bits(x):return struct.unpack('<I',struct.pack('<f',x))[0]
def unbits(x):return struct.unpack('<f',struct.pack('<I',x))[0]

class Engine:
 def __init__(self,N=1000,repeats=0,silence=0.,window=.02,speed=1.,bend=0.,brk=0.,traverse=0.,randoms=(128,)):
  self.u=u=Uc(UC_ARCH_ARM,UC_MODE_THUMB);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
  b=firmware_path().read_bytes();u.mem_map(BASE,(len(b)+4095)&~4095);u.mem_write(BASE,b)
  for addr,n in [(0x20000000,0x100000),(0x24000000,0x100000),(0xe0000000,0x100000),(POOL,0x100000)]:u.mem_map(addr,n)
  u.mem_write(0xe000ed88,struct.pack('<I',0x00f00000));u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
  self.rng=list(randoms);self.rngidx=0;self.calls=[];self.events=[];self.frame=[0,0];self.last=[None,None];self.track=False
  def hook(u,a,size,unused):
   if a==STOP:u.emu_stop();return
   if a==0x08018958:
    v=self.rng[self.rngidx%len(self.rng)];self.rngidx+=1;self.calls.append(dict(ch=u.reg_read(UC_ARM_REG_R8),frame=list(self.frame),rand=v))
    u.reg_write(UC_ARM_REG_R0,v);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
   if a==0x08017e88:
    x=unbits(u.reg_read(UC_ARM_REG_S0));y=unbits(u.reg_read(UC_ARM_REG_S1));u.reg_write(UC_ARM_REG_S0,bits(x**y));u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
   if a==0x08018948:
    p=u.reg_read(UC_ARM_REG_R0);v=u.reg_read(UC_ARM_REG_R1);n=u.reg_read(UC_ARM_REG_R2);u.mem_write(p,bytes([v&255])*n);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
   if a==0x080051be and self.track:
    ch=u.reg_read(UC_ARM_REG_R8);self.frame[ch]+=1;s=self.snapshot(ch)
    keys=['N','segment','start','end','audible','bankW','bankR','repeats','frozen','freezeRequested','reroll','timeResize']
    sig=tuple(s[k] for k in keys)
    if sig!=self.last[ch]:self.events.append(dict(ch=ch,frame=self.frame[ch],**s));self.last[ch]=sig
  for a in [STOP,0x08018958,0x08017e88,0x08018948,0x080051be]:u.hook_add(UC_HOOK_CODE,hook,begin=a,end=a)
  self.wf(0xb4,1.);self.wf(0xec,1.)
  u.reg_write(UC_ARM_REG_R0,OBJ);u.reg_write(UC_ARM_REG_R1,POOL);u.reg_write(UC_ARM_REG_R2,65536);u.reg_write(UC_ARM_REG_S0,bits(96028.))
  self.execute(0x08004a6c)
  for o,x in [(0x50,96028./N),(0x54,96028./N),(0x6c,repeats),(0x88,silence),(0x10c,window),(0xa4,speed),(0x104,bend),(0x108,brk),(0xc0,traverse)]:self.wf(o,x)
  for ch in [0,1]:
   for o,x in [(0x154,N),(0x14c,N),(0x15c,0),(0x164,N),(0x16c,N),(0x18c,N),(0x1b4,1)]:self.wi(o+4*ch,x)
  self.track=True
 def wf(self,o,v):self.u.mem_write(OBJ+o,struct.pack('<f',v))
 def wi(self,o,v):self.u.mem_write(OBJ+o,struct.pack('<I',v&0xffffffff))
 def wb(self,o,v):self.u.mem_write(OBJ+o,bytes([v]))
 def rf(self,o):return struct.unpack('<f',self.u.mem_read(OBJ+o,4))[0]
 def ri(self,o):return struct.unpack('<I',self.u.mem_read(OBJ+o,4))[0]
 def rb(self,o):return self.u.mem_read(OBJ+o,1)[0]
 def execute(self,addr):
  self.u.reg_write(UC_ARM_REG_SP,0x200f0000);self.u.reg_write(UC_ARM_REG_LR,STOP|1)
  self.u.emu_start(addr|1,STOP,count=20000000)
  if self.u.reg_read(UC_ARM_REG_PC)!=STOP:raise RuntimeError('Instruction limit at '+hex(self.u.reg_read(UC_ARM_REG_PC)))
 def snapshot(self,ch):
  r={name:self.ri(o+4*ch) for name,o in [('N',0x154),('segment',0x14c),('start',0x15c),('end',0x164),('audible',0x18c),('candidate',0x184),('bankW',0x194),('bankR',0x19c),('repeats',0x1b4)]}
  r.update({name:self.rf(o+4*ch) for name,o in [('rpos',0x13c),('wpos',0x144),('rate',0xf8),('bend',0xa8),('randomExponent',0x70),('randomSilence',0x8c),('randomTraverse',0xc4)]})
  r.update(frozen=self.rb(0x11a),freezeRequested=self.rb(0x11b),reroll=self.rb(0x238+ch),timeResize=self.rb(0x236+ch))
  return r
 def process(self,n=256,kind='sine',value=1.):
  t=self.frame[0]
  samples=[]
  for i in range(n):
   x=math.sin((t+i)*.2) if kind=='sine' else (0. if kind=='zero' else value)
   samples.extend([x,x])
  u=self.u;u.mem_write(IN,struct.pack('<%df'%len(samples),*samples));u.reg_write(UC_ARM_REG_R0,OBJ);u.reg_write(UC_ARM_REG_R1,IN);u.reg_write(UC_ARM_REG_R2,OUT);u.reg_write(UC_ARM_REG_R3,len(samples));self.execute(0x08004d58)
  return struct.unpack('<%df'%len(samples),u.mem_read(OUT,4*len(samples)))

if __name__=='__main__':
 records=[]
 for repeats,silence,traverse in [(0,0,0),(1,0,0),(2,0,0),(8,0,0),(2,.5,0),(2,0,.5),(2,0,1)]:
  e=Engine(repeats=repeats,silence=silence,traverse=traverse)
  try:
   for _ in range(8):e.process()
   records.append(dict(case=dict(exponent=repeats,silence=silence,traverse=traverse),events=e.events,final=e.snapshot(0),rng_calls=len(e.calls)))
  except Exception as ex:records.append(dict(error=str(ex),pc=hex(e.u.reg_read(UC_ARM_REG_PC))))
 Path(__file__).with_name('buffer_engine_probes.json').write_text(json.dumps(records,indent=2)+'\n')
 print(json.dumps([dict(case=x.get('case'),final=x.get('final'),events=len(x.get('events',[])),error=x.get('error')) for x in records],indent=2))
