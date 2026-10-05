"""Original constructor with explicit host-libm and deterministic entropy boundary."""
import ctypes
import json
import math
import struct
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32, libm
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR

SERVICES = {0x162E4:'_ZNSt13random_device7_M_initERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE',
            0x160F8:'_ZNSt13random_device9_M_getvalEv',0x159D8:'tanf',0x16224:'sinf',0x16368:'expf'}
_plt = json.loads((ROOT/'evidence/plt_map.json').read_text())
assert all(_plt[hex(address)]==name for address,name in SERVICES.items())
for name in ('tanf','sinf','expf'):
    getattr(libm,name).argtypes = [ctypes.c_float]
    getattr(libm,name).restype = ctypes.c_float


class FactoryBytes(ARMBytes):
    def __init__(self, seed):
        super().__init__()
        self.seed = seed
        self.services = []

    def _code(self,cpu,address,size,data):
        if address not in SERVICES:
            super()._code(cpu,address,size,data)
            return
        self.services.append(dict(address=hex(address),input=self.fp(0)))
        if address == 0x160F8:
            self.reg(0,self.seed)
        elif address != 0x162E4:
            self.fp(0,getattr(libm,SERVICES[address])(self.fp(0)))
        cpu.reg_write(UC_ARM_REG_PC,cpu.reg_read(UC_ARM_REG_LR))


def factory_table():
    return [0.]+[libm.sinf(f32(f32(i/256.)*f32(2*math.pi))) for i in range(1,256)]


def factory_coefficients():
    low_frequency = struct.unpack('<f',struct.pack('<I',0x38AA9A72))[0]
    high_frequency = struct.unpack('<f',struct.pack('<I',0x382A9A72))[0]
    low = libm.expf(f32(low_frequency*(-2*math.pi)))
    high = libm.expf(f32((.5-high_frequency)*(-2*math.pi)))
    return (([1.,-low,0.],[f32(1.-low),0.,0.]),
            ([1.,high,0.],[f32(1.-high),0.,0.]))
