#!/usr/bin/env python3
"""Reusable isolated Cortex-M7 firmware caller for Data Bender 1.4.7.

This runs uploaded machine code, not a high-level model. Hardware boot is not
emulated. Callers explicitly prepare object state and may hook peripheral or
random-number functions. Code hooks can replace a function by setting its return
register(s) and setting PC to LR. The default runs the original libm routines.
"""
from pathlib import Path
import struct
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import *

FIRMWARE = Path(__file__).resolve().parents[2] / 'upload/Data_Bender_v1_4_7.bin'
FLASH = 0x08000000
RETURN_SENTINEL = 0x200E0000
STACK_TOP = 0x200F0000

def f32bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]

def bitsf32(value):
    return struct.unpack('<f', struct.pack('<I', value & 0xffffffff))[0]

class FirmwareMachine:
    def __init__(self, firmware=FIRMWARE):
        self.uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
        self.uc.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
        binary = Path(firmware).read_bytes()
        self.uc.mem_map(FLASH, (len(binary) + 4095) & ~4095)
        self.uc.mem_write(FLASH, binary)
        for base,size in [(0x20000000,0x100000), (0x24000000,0x100000), (0xe0000000,0x100000)]:
            self.uc.mem_map(base,size)
        self.u32(0xe000ed88,0x00f00000)
        self.uc.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000)
        self.uc.reg_write(UC_ARM_REG_FPEXC,0x40000000)
        self.uc.mem_write(RETURN_SENTINEL,b'\x00\xbe')
        self.hooks = {}
        self.trace = []
        self.uc.hook_add(UC_HOOK_CODE,self._code)
    def _code(self,uc,address,size,user):
        self.trace.append(address)
        if len(self.trace)>40:
            del self.trace[0]
        if address == RETURN_SENTINEL:
            uc.emu_stop()
        elif address in self.hooks:
            self.hooks[address](self)
    def return_u32(self,value):
        self.uc.reg_write(UC_ARM_REG_R0,value & 0xffffffff)
        self.uc.reg_write(UC_ARM_REG_PC,self.uc.reg_read(UC_ARM_REG_LR))
    def return_f32(self,value):
        self.uc.reg_write(UC_ARM_REG_S0,f32bits(value))
        self.uc.reg_write(UC_ARM_REG_PC,self.uc.reg_read(UC_ARM_REG_LR))
    def call(self,address,args=(),floats=(),max_instructions=1000000):
        regs=[UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3]
        for reg,value in zip(regs,args):self.uc.reg_write(reg,value & 0xffffffff)
        for i,value in enumerate(floats):self.uc.reg_write(UC_ARM_REG_S0+i,f32bits(value))
        self.uc.reg_write(UC_ARM_REG_SP,STACK_TOP)
        self.uc.reg_write(UC_ARM_REG_LR,RETURN_SENTINEL|1)
        self.trace=[]
        self.uc.emu_start(address|1,RETURN_SENTINEL,count=max_instructions)
        if self.uc.reg_read(UC_ARM_REG_PC)!=RETURN_SENTINEL:
            raise RuntimeError('Instruction budget exhausted; PC='+hex(self.uc.reg_read(UC_ARM_REG_PC)))
        return self.uc.reg_read(UC_ARM_REG_R0),bitsf32(self.uc.reg_read(UC_ARM_REG_S0))
    def u32(self,address,value=None):
        if value is None:return struct.unpack('<I',self.uc.mem_read(address,4))[0]
        self.uc.mem_write(address,struct.pack('<I',value & 0xffffffff))
    def f32(self,address,value=None):
        if value is None:return struct.unpack('<f',self.uc.mem_read(address,4))[0]
        self.uc.mem_write(address,struct.pack('<f',value))
    def u8(self,address,value=None):
        if value is None:return self.uc.mem_read(address,1)[0]
        self.uc.mem_write(address,bytes([value & 255]))
    def floats(self,address,values_or_count):
        if isinstance(values_or_count,int):
            return list(struct.unpack('<%df'%values_or_count,self.uc.mem_read(address,values_or_count*4)))
        self.uc.mem_write(address,struct.pack('<%df'%len(values_or_count),*values_or_count))

if __name__=='__main__':
    machine=FirmwareMachine()
    _,value=machine.call(0x08017da4,floats=(1.0,))
    print('Original firmware expf(1.0) =',value)
