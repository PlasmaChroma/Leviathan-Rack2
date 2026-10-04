"""Bounded execution of original SP67 Thumb instructions (not a board emulator).

Optional dependency: unicorn==2.1.4. RAM and CPU inputs must be supplied by
the caller. No peripherals, interrupts, boot, codec or physical I/O are modeled.
"""
import hashlib
import struct
from pathlib import Path

from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS, UC_HOOK_CODE
from unicorn import arm_const as arm

ROOT = Path(__file__).resolve().parents[1]
SHA256 = 'b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9'


class SliceMachine:
    def __init__(self, double_precision=False):
        firmware = (ROOT / 'firmware/sp67.bin').read_bytes()
        if hashlib.sha256(firmware).hexdigest() != SHA256:
            raise ValueError('Unexpected firmware hash')
        # Unicorn's M7 model rejects this image's f64 instructions. Its MAX
        # ARM profile can check those instructions, without pretending to be
        # the target MCU or validating exception/peripheral behavior.
        self.uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB | (0 if double_precision else UC_MODE_MCLASS))
        self.uc.ctl_set_cpu_model(arm.UC_CPU_ARM_MAX if double_precision else arm.UC_CPU_ARM_CORTEX_M7)
        for base, size in ((0x08020000, 0x30000), (0x20000000, 0x20000),
                           (0x30000000, 0x20000), (0x38000000, 0x10000),
                           (0xe0000000, 0x100000)):
            self.uc.mem_map(base, size)
        self.uc.mem_write(0x08020000, firmware)
        self.uc.reg_write(arm.UC_ARM_REG_C1_C0_2, 0xf00000)
        self.uc.reg_write(arm.UC_ARM_REG_FPEXC, 0x40000000)
        self.uc.reg_write(arm.UC_ARM_REG_FPSCR, 1 << 24)  # flush-to-zero
        self.sp = 0x2001f000
        self.uc.reg_write(arm.UC_ARM_REG_SP, self.sp)
        self.r(11, 0x0803a674)
        self.s(29, 8192)
        self.last_trace = []

    def r(self, n, value=None):
        register = getattr(arm, f'UC_ARM_REG_R{n}')
        if value is not None:
            self.uc.reg_write(register, value)
        return self.uc.reg_read(register)

    def s(self, n, value=None):
        register = getattr(arm, f'UC_ARM_REG_S{n}')
        if value is not None:
            self.uc.reg_write(register, struct.unpack('<I', struct.pack('<f', value))[0])
        return struct.unpack('<f', struct.pack('<I', self.uc.reg_read(register)))[0]

    def word(self, address, value=None):
        if value is not None:
            self.uc.mem_write(address, struct.pack('<I', value & 0xffffffff))
        return struct.unpack('<I', self.uc.mem_read(address, 4))[0]

    def floats(self, address, values=None, count=1):
        if values is not None:
            self.uc.mem_write(address, struct.pack(f'<{len(values)}f', *values))
            count = len(values)
        return list(struct.unpack(f'<{count}f', self.uc.mem_read(address, 4 * count)))

    def run(self, start, stop, limit=10000):
        """Execute until one explicit boundary (or any address in a collection).

        Boundary instructions are not executed or counted in last_trace. This
        permits transparent external-I/O fixtures without attributing substituted
        functions to original-instruction coverage.
        """
        stops = {stop} if isinstance(stop, int) else set(stop)
        self.last_trace = []
        reached = False

        def hook(uc, address, size, user):
            nonlocal reached
            if address in stops:
                reached = True
                uc.emu_stop()
            else:
                self.last_trace.append(address)

        handle = self.uc.hook_add(UC_HOOK_CODE, hook)
        try:
            self.uc.emu_start(start | 1, 0, count=limit)
        finally:
            self.uc.hook_del(handle)
        if not reached:
            targets=', '.join(f'{address:#x}' for address in sorted(stops))
            raise RuntimeError(f'Slice did not reach {targets}; PC={self.uc.reg_read(arm.UC_ARM_REG_PC):#x}')
        return len(self.last_trace)
