#!/usr/bin/env python3
"""Isolated original-instruction probes for Data Bender buffer Macro helpers.
Original firmware executes in Unicorn Cortex M7. Only libc rand is substituted
with a caller-selected sequence, permitting exact boundary/path verification.
"""
from pathlib import Path
from firmware_input import firmware_path
import struct,json
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[2]
BASE=0x08000000; OBJ=0x20000000; STOP=0x200e0000

def fb(x):return struct.pack('<f',x)
def run(which,amount,randoms=(0,),channel=0,shared=True,initial_slew=.1):
 u=Uc(UC_ARCH_ARM,UC_MODE_THUMB);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
 b=firmware_path().read_bytes()
 u.mem_map(BASE,(len(b)+4095)&~4095);u.mem_write(BASE,b)
 u.mem_map(0x20000000,0x100000);u.mem_map(0x24000000,0x100000);u.mem_map(0xe0000000,0x100000)
 u.mem_write(0xe000ed88,struct.pack('<I',0x00f00000));u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
 u.mem_write(OBJ+0x104,fb(amount));u.mem_write(OBJ+0x108,fb(amount));u.mem_write(OBJ+0x68,struct.pack('<I',shared))
 u.mem_write(OBJ+0xe0+4*channel,fb(initial_slew))
 for o,v in [(0x70,7.),(0x8c,.5),(0xc4,.8)]:u.mem_write(OBJ+o,fb(v))
 u.reg_write(UC_ARM_REG_R0,OBJ);u.reg_write(UC_ARM_REG_R1,channel)
 u.reg_write(UC_ARM_REG_SP,0x200f0000);u.reg_write(UC_ARM_REG_LR,STOP|1)
 rng=iter(randoms);calls=[]
 def hook(u,a,size,unused):
  if a==STOP:u.emu_stop();return
  if a==0x08018958:
   v=next(rng,0);calls.append(v);u.reg_write(UC_ARM_REG_R0,v);u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR))
 u.hook_add(UC_HOOK_CODE,hook)
 r=dict(which=which,amount=amount,randoms=list(randoms),channel=channel,shared=shared,initial_slew=initial_slew)
 try:
  u.emu_start((0x08004678 if which=='bend' else 0x08004bf8)|1,STOP,count=20000)
  r.update(outputs={k:struct.unpack('<f',u.mem_read(OBJ+o+channel*4,4))[0] for k,o in [('bend_ratio',0xa8),('slew',0xe0),('repeat_exponent_add',0x70),('silence_fraction',0x8c),('slice_random',0xc4)]},rng_consumed=calls,pc=hex(u.reg_read(UC_ARM_REG_PC)))
 except Exception as e:r.update(error=str(e),rng_consumed=calls,pc=hex(u.reg_read(UC_ARM_REG_PC)))
 return r
if __name__=='__main__':
 cases=[]
 for b in [.03,.031,.25,.5,.667,.668,1.]:
  for r in [0,128,254]:cases.append(run('bend',b,(r,128,300)))
 for r2 in [63,64]:
  for r3 in [127,128,255,256,638,639]:cases.append(run('bend',1.,(128,r2,r3)))
 for b in [.03,.031,.33,.331,.5,.5624,.5625,.6875,.8125,.9375,1.]:
  for r in [0,128,254]:cases.append(run('break',b,(r,r,r)))
 cases += [run('break',1.,(0,),channel=1,shared=True),run('break',1.,(254,254,254),channel=1,shared=False)]
 out=Path(__file__).with_name('macro_helper_probes.json');out.write_text(json.dumps(cases,indent=2)+'\n')
 print(json.dumps({'cases':len(cases),'errors':[x for x in cases if 'error' in x],'first':cases[0],'last':cases[-1]},indent=2))
