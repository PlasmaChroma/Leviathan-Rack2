#!/usr/bin/env python3
"""Isolated emulation of recovered Corrupt firmware behavior; no hardware I/O."""
from pathlib import Path
import struct, json, math
import unicorn
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *
FW=Path(__file__).resolve().parents[2]/'upload/Data_Bender_v1_4_7.bin'
BASE=0x08000000; RAM=0x20000000; OBJ=RAM; IN=RAM+0x10000; OUT=RAM+0x20000; STOP=RAM+0xe0000

def fbytes(v):return struct.pack('<f',v)
def tofloat(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def fbits(v):return struct.unpack('<I',fbytes(v))[0]

def run(mode=2,amount=.5,gate=True,randoms=(0,),frames=4,hook_math=False):
 uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB)
 uc.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
 b=FW.read_bytes();uc.mem_map(BASE,(len(b)+4095)&~4095);uc.mem_write(BASE,b)
 uc.mem_map(RAM,0x100000);uc.mem_map(0x24000000,0x100000);uc.mem_map(0xe0000000,0x100000)
 uc.mem_write(0xe000ed88,struct.pack('<I',0x00f00000))
 uc.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000)
 uc.reg_write(UC_ARM_REG_FPEXC,0x40000000)
 uc.mem_write(OBJ,struct.pack('<I',mode));uc.mem_write(OBJ+0x10,fbytes(amount));uc.mem_write(OBJ+0x18,bytes([gate]));
 uc.mem_write(OBJ+0x28,bytes([16]));uc.mem_write(OBJ+0x50,bytes([16]));
 samples=[(-1 if i%4 else 1)*((i+1)/16) for i in range(frames*2)]
 uc.mem_write(IN,struct.pack('<%df'%len(samples),*samples))
 uc.reg_write(UC_ARM_REG_R0,OBJ);uc.reg_write(UC_ARM_REG_R1,IN);uc.reg_write(UC_ARM_REG_R2,OUT);uc.reg_write(UC_ARM_REG_R3,len(samples))
 uc.reg_write(UC_ARM_REG_SP,RAM+0xf0000);uc.reg_write(UC_ARM_REG_LR,STOP|1)
 uc.mem_write(STOP,b'\x00\xbe')
 rng=iter(randoms);trace=[];calls=[]
 def hook(uc,addr,size,user):
  trace.append(hex(addr))
  if len(trace)>30:trace.pop(0)
  if addr==STOP:uc.emu_stop();return
  if addr==0x08018958:
   v=next(rng,0);calls.append(['rand',v]);uc.reg_write(UC_ARM_REG_R0,v);uc.reg_write(UC_ARM_REG_PC,uc.reg_read(UC_ARM_REG_LR));return
  unary={0x08017da4:('expf',math.exp),0x080181b0:('tanhf',math.tanh),0x08018268:('sinf',math.sin)}
  if hook_math and addr in unary:
   name,fn=unary[addr];x=tofloat(uc.reg_read(UC_ARM_REG_S0));y=fn(x);uc.reg_write(UC_ARM_REG_S0,fbits(y));uc.reg_write(UC_ARM_REG_PC,uc.reg_read(UC_ARM_REG_LR));return
  if hook_math and addr==0x08018474:
   x=tofloat(uc.reg_read(UC_ARM_REG_S0));y=tofloat(uc.reg_read(UC_ARM_REG_S1));uc.reg_write(UC_ARM_REG_S0,fbits(math.fmod(x,y)));uc.reg_write(UC_ARM_REG_PC,uc.reg_read(UC_ARM_REG_LR));return
 uc.hook_add(UC_HOOK_CODE,hook)
 result={'mode':mode,'amount':amount,'initial_gate':gate,'rand':list(randoms),'inputs':samples}
 try:
  uc.emu_start(0x08001869,STOP,count=100000)
  result.update(outputs=list(struct.unpack('<%df'%len(samples),uc.mem_read(OUT,4*len(samples)))),final_gate=bool(uc.mem_read(OBJ+0x18,1)[0]),calls=calls,pc=hex(uc.reg_read(UC_ARM_REG_PC)))
 except Exception as e:result.update(error=str(e),trace=trace,pc=hex(uc.reg_read(UC_ARM_REG_PC)))
 return result
if __name__=='__main__':
 cases=[run(amount=.0,gate=False),run(amount=.03,gate=False),run(amount=.031,gate=True,randoms=(0,)),run(amount=.5,gate=True,randoms=(0,)),run(amount=.5,gate=False,randoms=(1000,)),run(amount=1.,gate=True,randoms=(0,)),run(amount=1.,gate=True,randoms=(1000,))]
 print(json.dumps(cases,indent=2))
 Path(__file__).with_name('dropout_emulation.json').write_text(json.dumps(cases,indent=2)+'\n')
