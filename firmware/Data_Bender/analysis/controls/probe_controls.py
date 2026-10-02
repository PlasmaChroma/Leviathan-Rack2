#!/usr/bin/env python3
"""Isolated execution of original control-mapping instructions from uploaded firmware.
Stops before buffer/audio processing. Library math may be replaced by explicit
Python equivalents; clamp instructions are original ARMv8 FP extension opcodes.
"""
from pathlib import Path
import struct,json,math
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[2]
FW=ROOT/'upload/Data_Bender_v1_4_7.bin'
P=0x24000d68;BASE=0x08000000

def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def unbits(v):return struct.unpack('<f',struct.pack('<I',v))[0]

def run(bend=.5,cv=0.,mode=1,brk=0.,mix=.5,reverse=False,silence=False,window=.02):
 u=Uc(UC_ARCH_ARM,UC_MODE_THUMB);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
 b=FW.read_bytes();u.mem_map(BASE,(len(b)+4095)&~4095);u.mem_write(BASE,b)
 u.mem_map(0x24000000,0x100000);u.mem_map(0x20000000,0x100000);u.mem_map(0xe0000000,0x100000)
 u.mem_write(0xe000ed88,struct.pack('<I',0x00f00000));u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
 def wf(o,x):u.mem_write(P+o,struct.pack('<f',x))
 def wi(o,x):u.mem_write(P+o,struct.pack('<I',x))
 def rb(o):return u.mem_read(P+o,1)[0]
 def rf(o):return struct.unpack('<f',u.mem_read(P+o,4))[0]
 wi(4,mode);wi(0x684,1)
 for o,x in [(0,96028.),(0x30,bend),(0x34,brk),(0x50,mix),(0x54,cv),(0x6c8,0.),(0x6cc,1.),(0x3c,1.),(0x40,1.),(0x44,1.),(0x1a4,window)]:wf(o,x)
 u.mem_write(P+0xc+(mode==1),bytes([reverse]));u.mem_write(P+0xe if mode==1 else P+0xf,bytes([silence]))
 u.reg_write(UC_ARM_REG_R0,P);u.reg_write(UC_ARM_REG_R1,0x20010000);u.reg_write(UC_ARM_REG_R2,0x20020000);u.reg_write(UC_ARM_REG_R3,0)
 u.reg_write(UC_ARM_REG_SP,0x200f0000);u.reg_write(UC_ARM_REG_LR,0x200e0001)
 trace=[]
 def hook(u,a,size,unused):
  trace.append(hex(a))
  if len(trace)>8:trace.pop(0)
  if a==0x080020bc:u.emu_stop();return
  if a==0x08017da4:
   x=unbits(u.reg_read(UC_ARM_REG_S0));u.reg_write(UC_ARM_REG_S0,bits(math.exp(x)));u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
  if a==0x08017e88:
   x=unbits(u.reg_read(UC_ARM_REG_S0));y=unbits(u.reg_read(UC_ARM_REG_S1));u.reg_write(UC_ARM_REG_S0,bits(x**y));u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR));return
 u.hook_add(UC_HOOK_CODE,hook)
 r=dict(bend=bend,cv=cv,mode=mode,break_amount=brk,mix=mix,reverse=reverse,silence=silence)
 try:
  u.emu_start(0x08001e4d,0x200e0000,count=20000)
  r['mapped']={name:rf(o) for name,o in [('bend_effective',0x18),('break_effective',0x1c),('corrupt_effective',0x20),('repeats',0x28),('mix',0x2c),('speed',0x13c),('micro_silence',0x120),('micro_traverse',0x158),('macro_bend',0x19c),('macro_break',0x1a0)]}
  r['octave_led']=rb(0x75);r['pc']=hex(u.reg_read(UC_ARM_REG_PC))
 except Exception as e:r.update(error=str(e),trace=trace,pc=hex(u.reg_read(UC_ARM_REG_PC)))
 return r
if __name__=='__main__':
 cases=[run(bend=x) for x in [0,.01,.02,1/6,.25,1/3,.49,.5,.51,2/3,5/6,.99,1]]
 cases += [run(bend=.5,cv=x) for x in [-1,-.2,0,.2,1]]
 cases += [run(bend=.5,reverse=True),run(brk=.5,silence=True),run(brk=.5,silence=False)]
 cases += [run(mix=x) for x in [0,.019,.02,.029,.03,.5,.98,.981,1]]
 p=Path(__file__).with_name('controls_probe.json');p.write_text(json.dumps(cases,indent=2)+'\n')
 print(json.dumps(cases[:3],indent=2))
