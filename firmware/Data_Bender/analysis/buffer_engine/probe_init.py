#!/usr/bin/env python3
"""Execute original Init with exact main() allocation arguments.
Large zeroing is skipped; this validates allocation metadata, not SDRAM hardware.
"""
from pathlib import Path
from firmware_input import firmware_path
import struct,json
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_THUMB,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[2];BASE=0x08000000;P=0x24000d68;B=P+0x98;STOP=0x200e0000
u=Uc(UC_ARCH_ARM,UC_MODE_THUMB);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
blob=firmware_path().read_bytes();u.mem_map(BASE,(len(blob)+4095)&~4095);u.mem_write(BASE,blob)
for addr,n in [(0x20000000,0x100000),(0x24000000,0x100000),(0xe0000000,0x100000)]:u.mem_map(addr,n)
u.mem_write(0xe000ed88,struct.pack('<I',0x00f00000));u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
u.reg_write(UC_ARM_REG_R0,B);u.reg_write(UC_ARM_REG_R1,0xc0000000);u.reg_write(UC_ARM_REG_R2,7202100);u.reg_write(UC_ARM_REG_S0,struct.unpack('<I',struct.pack('<f',96028.))[0]);u.reg_write(UC_ARM_REG_SP,0x200f0000);u.reg_write(UC_ARM_REG_LR,STOP|1)
zeroes=[]
def hook(u,a,size,unused):
 if a==STOP:u.emu_stop();return
 if a==0x08018948:
  zeroes.append(dict(address=hex(u.reg_read(UC_ARM_REG_R0)),bytes=u.reg_read(UC_ARM_REG_R2)));u.reg_write(UC_ARM_REG_PC,u.reg_read(UC_ARM_REG_LR))
for a in [STOP,0x08018948]:u.hook_add(UC_HOOK_CODE,hook,begin=a,end=a)
u.emu_start(0x08004a6d,STOP,count=20000)
ri=lambda o:struct.unpack('<I',u.mem_read(B+o,4))[0]
result=dict(args=dict(sample_rate_constant=96028.,base='0xc0000000',float_count=7202100),allocation=dict(total_floats=ri(4),floats_per_channel=ri(8),left_pointer=hex(ri(0xc)),left_capacity=ri(0x10),right_pointer=hex(ri(0x2c)),right_capacity=ri(0x30),history_left_pointer=hex(ri(0x1bc)),history_left_capacity=ri(0x1c0),history_right_pointer=hex(ri(0x1d0)),history_right_capacity=ri(0x1d4),maximum_capture_window_frames=ri(8)>>1),memset_calls=zeroes,pc=hex(u.reg_read(UC_ARM_REG_PC)))
Path(__file__).with_name('buffer_init_probe.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
