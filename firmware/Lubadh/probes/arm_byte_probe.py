#!/usr/bin/env python3
"""Offline original-byte ARM probes; never boots the appliance or runs its OS.

Unicorn executes instructions. Only explicitly listed libc memory services are
host hooks; DSP and transport routines are not replaced. Unsupported imports,
syscalls, unmapped accesses, missing returns, and exhausted budgets fail closed.
"""
from pathlib import Path
import hashlib
import json
import struct
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
import unicorn
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_ARM, UC_HOOK_CODE, UC_HOOK_INTR
from unicorn.arm_const import (
    UC_CPU_ARM_CORTEX_A15, UC_ARM_REG_R0, UC_ARM_REG_SP, UC_ARM_REG_LR,
    UC_ARM_REG_PC, UC_ARM_REG_CPSR, UC_ARM_REG_FPEXC, UC_ARM_REG_C1_C0_2,
    UC_ARM_REG_S0,
)

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from inspect_firmware import ELF32

RETURN = 0x3F0000
STACK = 0x3E0000
LIMIT = 0x400000
EXPECTED_SHA256 = '2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4'
if hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest() != EXPECTED_SHA256:
    raise RuntimeError('Byte probe addresses require the archived Lúbadh 2.1.0 ELF')
MEMORY_IMPORTS = {0x159E4: 'memmove', 0x16044: 'memcpy', 0x15D74: 'memset',
                  0x1590C: '_Znwj', 0x15E40: '_ZdlPv'}
_PLT_MAP = json.loads((ROOT / 'evidence/plt_map.json').read_text())
if any(_PLT_MAP.get(hex(address)) != name for address, name in MEMORY_IMPORTS.items()):
    raise RuntimeError('Memory-service import mapping does not match the archived evidence')
_ELF = ELF32(ROOT / 'extracted/bin/lubadh_main')
_FUNCTION_ADDRESSES = frozenset(address
    for symbol in _ELF.symbols if symbol['type'] == 2 and symbol['section'] and symbol['size']
    for address in range(symbol['address'], symbol['address'] + symbol['size'], 4))


class ARMBytes:
    def __init__(self):
        self.elf = _ELF
        self.cpu = Uc(UC_ARCH_ARM, UC_MODE_ARM)
        self.cpu.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A15)
        self.cpu.mem_map(0, LIMIT)
        self.memory_regions = [(0, LIMIT)]
        for section in self.elf.sections:
            if section['flags'] & 2 and section['type'] != 8:
                self.cpu.mem_write(section['addr'], self.elf.section_data(section))
        self.cpu.reg_write(UC_ARM_REG_CPSR, 0x13)
        self.cpu.reg_write(UC_ARM_REG_C1_C0_2, 0xF << 20)
        self.cpu.reg_write(UC_ARM_REG_FPEXC, 1 << 30)
        self.cpu.reg_write(UC_ARM_REG_SP, STACK)
        self.coverage = set()
        self.visits = {}
        self.observers = {}
        self.hook_calls = []
        self.heap = 0x280000
        self.returned = False
        self.stops = set()
        self.decoder = Cs(CS_ARCH_ARM, CS_MODE_ARM)
        self.cpu.hook_add(UC_HOOK_CODE, self._code)
        self.cpu.hook_add(UC_HOOK_INTR, self._interrupt)

    def _interrupt(self, cpu, number, _):
        raise RuntimeError(f'Firmware syscall/interrupt rejected: {number}')

    def _code(self, cpu, address, size, _):
        if address == RETURN or address in self.stops:
            self.returned = True
            cpu.emu_stop()
            return
        self.coverage.add(address)
        self.visits[address] = self.visits.get(address, 0) + 1
        if address in self.observers:
            self.observers[address](self)
        if address in MEMORY_IMPORTS:
            self._memory_service(address)
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        if address not in _FUNCTION_ADDRESSES:
            instruction = next(self.decoder.disasm(self.read(address, size), address), None)
            raise RuntimeError(f'Unmodelled import or non-function execution at {address:#x}: {instruction}')

    def _memory_service(self, address):
        a, b, n = [self.reg(i) for i in range(3)]
        self.hook_calls.append(dict(address=hex(address), arguments=[a, b, n]))
        if address in (0x159E4, 0x16044):
            self.write(a, self.read(b, n))
        elif address == 0x15D74:
            self.write(a, bytes([b & 255]) * n)
        elif address == 0x1590C:
            pointer = self.heap
            self.heap += (a + 15) & ~15
            if self.heap >= STACK - 0x10000:
                raise RuntimeError('Probe heap exhausted')
            self.reg(0, pointer)
        # delete is a lifetime-only no-op; fixtures do not recycle pointers.

    def read(self, address, count):
        if address < 0 or count < 0 or not any(begin <= address and address + count <= end
                                               for begin, end in self.memory_regions):
            raise RuntimeError(f'Out of probe memory: {address:#x} + {count}')
        return bytes(self.cpu.mem_read(address, count)) if count else b''

    def write(self, address, data):
        if address < 0 or not any(begin <= address and address + len(data) <= end
                                 for begin, end in self.memory_regions):
            raise RuntimeError(f'Out of probe memory: {address:#x} + {len(data)}')
        if data:
            self.cpu.mem_write(address, bytes(data))

    def map_fixture(self, address, count):
        begin = address & ~4095
        end = (address + count + 4095) & ~4095
        if any(begin < old_end and end > old_begin for old_begin, old_end in self.memory_regions):
            raise ValueError('Sparse fixture overlaps an existing map')
        self.cpu.mem_map(begin, end - begin)
        self.memory_regions.append((begin, end))

    def reg(self, index, value=None):
        register = UC_ARM_REG_R0 + index
        if value is None:
            return self.cpu.reg_read(register)
        self.cpu.reg_write(register, value & 0xFFFFFFFF)

    def fp(self, index, value=None):
        if value is None:
            return struct.unpack('<f', struct.pack('<I', self.cpu.reg_read(UC_ARM_REG_S0 + index)))[0]
        self.cpu.reg_write(UC_ARM_REG_S0 + index, struct.unpack('<I', struct.pack('<f', value))[0])

    def putu(self, address, value):
        self.write(address, struct.pack('<I', value & 0xFFFFFFFF))

    def getu(self, address):
        return struct.unpack('<I', self.read(address, 4))[0]

    def puts(self, address, value):
        self.write(address, struct.pack('<i', value))

    def gets(self, address):
        return struct.unpack('<i', self.read(address, 4))[0]

    def putf(self, address, value):
        self.write(address, struct.pack('<f', value))

    def getf(self, address):
        return struct.unpack('<f', self.read(address, 4))[0]

    def floats(self, address, count):
        return list(struct.unpack('<' + 'f' * count, self.read(address, count * 4)))

    def vector(self, header, data, values, capacity=None, integers=False):
        if values:
            self.write(data, struct.pack('<' + ('i' if integers else 'f') * len(values), *values))
        for i, pointer in enumerate((data, data + 4 * len(values), data + 4 * (capacity or len(values)))):
            self.putu(header + 4 * i, pointer)

    def call(self, address, budget=2000000, stop_before=None):
        self.returned = False
        self.stops = {stop_before} if stop_before is not None else set()
        self.cpu.reg_write(UC_ARM_REG_LR, RETURN)
        try:
            self.cpu.emu_start(address, RETURN + 4, count=budget)
        except Exception as exc:
            pc = self.cpu.reg_read(UC_ARM_REG_PC)
            instruction = next(self.decoder.disasm(self.read(pc, 4), pc), None)
            raise RuntimeError(f'ARM byte probe at {pc:#x}: {instruction}: {exc}') from exc
        if not self.returned:
            raise RuntimeError(f'No return within {budget} instructions at {address:#x}')


if __name__ == '__main__':
    cpu = ARMBytes()
    cpu.putf(0x100000, 0.5)
    cpu.reg(0, 0x100000)
    cpu.reg(1, 4095)
    cpu.call(0x6C104)
    assert cpu.reg(0) == 2047
    print(f'PASS: original-byte ADC smoke probe; Unicorn {unicorn.__version__}')
