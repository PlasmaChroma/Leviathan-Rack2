#!/usr/bin/env python3
"""Execute clock mapper and edge-period filter from original firmware in isolation.
Hardware sampling, downstream audio and timestamp reads are replaced explicitly.
This checks arithmetic/branching; does not claim measured wall-clock timing.
"""
from pathlib import Path
import struct,json,math
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[2];FW=ROOT/'upload/Data_Bender_v1_4_7.bin'
BASE=0x08000000;C=0x24000cf8;P=0x24000d68;STOP=0x200e0000

def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def unbits(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def setup():
 u=Uc(UC_ARCH_ARM,UC_MODE_THUMB);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
 b=FW.read_bytes();u.mem_map(BASE,(len(b)+4095)&~4095);u.mem_write(BASE,b)
 u.mem_map(0x24000000,0x100000);u.mem_map(0x20000000,0x100000);u.mem_map(0xe0000000,0x100000)
 u.mem_write(0xe000ed88,struct.pack('<I',0x00f00000));u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
 u.reg_write(UC_ARM_REG_SP,0x200f0000);u.reg_write(UC_ARM_REG_LR,STOP|1)
 return u

def map_time(x,external=False):
 u=setup()
 def wi(o,x):u.mem_write(C+o,struct.pack('<I',x))
 def wf(o,x):u.mem_write(C+o,struct.pack('<f',x))
 def rf(o):return struct.unpack('<f',u.mem_read(C+o,4))[0]
 def ri(o):return struct.unpack('<I',u.mem_read(C+o,4))[0]
 wi(0,int(external));wi(0x30,500000);wi(0x4c,500000)
 for o,v in [(0x10,96028.),(0x38,x),(0x3c,0),(0x40,-1),(0x44,-1),(0x68,500)]:wf(o,v)
 u.reg_write(UC_ARM_REG_R0,0x20010000);u.reg_write(UC_ARM_REG_R1,0x20020000);u.reg_write(UC_ARM_REG_R2,2)
 def hook(u,a,size,data):
  if a==STOP:u.emu_stop();return
  if a in (0x080070e4,0x080023a0,0x08001e4c):
   u.reg_write(UC_ARM_REG_R0,0);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
  if a in (0x0800950c,0x08009510):
   u.reg_write(UC_ARM_REG_R0,200000000 if a==0x08009510 else 1000);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
  if a==0x08017da4:
   v=unbits(u.reg_read(UC_ARM_REG_S0));u.reg_write(UC_ARM_REG_S0,bits(math.exp(v)));u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
 u.hook_add(UC_HOOK_CODE,hook)
 r=dict(x=x,external=external)
 try:
  u.emu_start(0x080032fd,STOP,count=20000)
  r.update(effective=rf(0x40),accepted=rf(0x44),frequency_hz_formula=rf(0x34),ratio=rf(0x48),period_us=ri(0x30))
 except Exception as e:r.update(error=str(e),pc=hex(u.reg_read(UC_ARM_REG_PC)))
 return r

def period_median(intervals):
 u=setup();u.mem_write(C,struct.pack('<I',1));u.mem_write(C+0x50,struct.pack('<III',500000,500000,500000))
 ticks=[0];results=[]
 def hook(u,a,size,data):
  if a==STOP:u.emu_stop();return
  if a in (0x0800950c,0x08009510):
   u.reg_write(UC_ARM_REG_R0,ticks[0] if a==0x08009510 else ticks[0]//200000);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
 u.hook_add(UC_HOOK_CODE,hook)
 for v in intervals:
  ticks[0]+=round(v*200)
  u.reg_write(UC_ARM_REG_SP,0x200f0000);u.reg_write(UC_ARM_REG_LR,STOP|1)
  u.emu_start(0x08003265,STOP,count=10000)
  vals=struct.unpack('<IIII',u.mem_read(C+0x4c,16))
  results.append(dict(interval_us=v,median_us=vals[0],history=list(vals[1:])))
 return results
if __name__=='__main__':
 xs=[0,.0625,.1211,.1213,.25,.3635,.3637,.4848,.485,.5,.606,.6061,.7272,.7274,.8484,.8486,.9696,.9698,1]
 r=dict(time=[map_time(x,e) for e in [False,True] for x in xs],period_filter=period_median([500000,1000000,501000,502000,503000]))
 Path(__file__).with_name('clock_probe.json').write_text(json.dumps(r,indent=2)+'\n')
 print(json.dumps(r,indent=2))
