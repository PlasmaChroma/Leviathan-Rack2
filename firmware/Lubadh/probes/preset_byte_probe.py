"""Offline preset_load ELF execution; memory imports only, no appliance launch."""
import hashlib
import json
import struct

from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR
from arm_byte_probe import ARMBytes, ROOT, RETURN
from inspect_firmware import ELF32

PRESET_ELF = ELF32(ROOT / 'extracted/bin/preset_load')
EXPECTED = '3ab7566b2a0d5a641454cb3928acfb658cb3e000c5b0505d2569cc2c78086e1b'
assert hashlib.sha256(PRESET_ELF.data).hexdigest() == EXPECTED
SECTIONS = {s['name']: s for s in PRESET_ELF.sections}
DYN = SECTIONS['.dynsym']
STRINGS = PRESET_ELF.section_data(PRESET_ELF.sections[DYN['link']])
PLT = {}
for i, offset in enumerate(range(0, SECTIONS['.rel.plt']['size'], 8)):
    _, info = struct.unpack_from('<II', PRESET_ELF.section_data(SECTIONS['.rel.plt']), offset)
    name = struct.unpack_from('<I', PRESET_ELF.section_data(DYN), (info >> 8) * DYN['entsize'])[0]
    PLT[SECTIONS['.plt']['addr'] + 20 + i * 12] = PRESET_ELF.cstr(STRINGS, name)
ALLOWED = {0x13AAC: '_Znwj', 0x13824: '_ZdlPv', 0x139D4: 'memmove',
           0x137D0: 'memcpy', 0x13B0C: 'memset'}
assert all(PLT.get(a) == n for a, n in ALLOWED.items())
FUNCTIONS = frozenset(a for s in PRESET_ELF.symbols if s['type'] == 2 and s['section'] and s['size']
                      for a in range(s['address'], s['address'] + s['size'], 4))


class PresetBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.cpu.mem_write(0, bytes(0x400000))
        self.elf = PRESET_ELF
        for section in self.elf.sections:
            if section['flags'] & 2 and section['type'] != 8:
                self.cpu.mem_write(section['addr'], self.elf.section_data(section))

    def _code(self, cpu, address, size, user):
        if address == RETURN or address in self.stops:
            self.returned = True
            cpu.emu_stop()
            return
        self.coverage.add(address)
        if address in self.observers:
            self.observers[address](self)
        if address in ALLOWED:
            a, b, n = (self.reg(i) for i in range(3))
            name = ALLOWED[address]
            self.hook_calls.append(dict(address=hex(address), service=name, arguments=[a, b, n]))
            if name in ('memmove', 'memcpy'):
                self.write(a, self.read(b, n))
            elif name == 'memset':
                self.write(a, bytes([b & 255]) * n)
            elif name == '_Znwj':
                pointer = self.heap
                self.heap += (a + 15) & ~15
                assert self.heap < 0x3D0000
                self.reg(0, pointer)
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        if address not in FUNCTIONS:
            raise RuntimeError(f'Unmodelled preset import or instruction at {address:#x}: {PLT.get(address)}')
