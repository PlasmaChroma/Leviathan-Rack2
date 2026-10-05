#!/usr/bin/env python3
"""Already-active trigger_fade with explicit inert diagnostic string services."""
import itertools
import json
import struct
from arm_byte_probe import ROOT, STACK
from probe_channel_set_state import StateBytes, _SERVICE_SYMBOLS, _PLT
from probe_tap_fade_state import triggered
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR

TAP = 0x150000
assert '__to_xstring' in _SERVICE_SYMBOLS[0x4A288]
assert 'basic_string' in _PLT['0x15af8']


class DiagnosticBytes(StateBytes):
    def __init__(self):
        super().__init__()
        self.logs = 0

    def _code(self, cpu, address, size, data):
        if address == 0x6FF20:
            self.logs += 1
        if address in (0x4A288, 0x15AF8):
            out = self.reg(0)
            self.putu(out, out+8)
            self.putu(out+4, 0)
            self.write(out+8, b'\0')
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, data)


def main():
    cases, coverage = 0, set()
    for kind, phase, step, duration, flags in itertools.product(
            range(4), (-1, 0, 31, 254, 255), (-2., 0., 2.), (32, 128, 1000), range(4)):
        c = DiagnosticBytes()
        c.reg(0, TAP)
        c.call(0x4BB88)
        c.reg(0, TAP)
        c.reg(1, 1000)
        c.call(0x4DF18)
        b = TAP+28+kind*24
        c.write(b, b'\1')
        c.puts(b+4, phase)
        c.putf(b+8, .375)
        c.puts(b+12, phase-7)
        c.putf(b+16, .75)
        c.putf(b+20, step)
        before = c.read(TAP, 132)
        expected = bytearray(before)
        state = triggered(1000, 0., 1000, 0., 2000, duration, flags & 1, flags >> 1)
        base = 28+kind*24
        expected[base] = state[0]
        for offset, value, fmt in zip((4,8,12,16,20), state[1:], ('i','f','i','f','f')):
            expected[base+offset:base+offset+4] = struct.pack('<'+fmt, value)
        observed = []
        c.observers[0x4E7C0] = lambda m: observed.append('active branch')
        c.reg(0, TAP)
        c.reg(1, kind)
        c.reg(2, 2000)
        c.reg(3, duration)
        c.putu(STACK, flags & 1)
        c.putu(STACK+4, flags >> 1)
        c.call(0x4E46C)
        after = c.read(TAP, 132)
        assert after == expected, (kind,phase,step,duration,flags, [(i,a,b) for i,(a,b) in enumerate(zip(expected,after)) if a != b])
        assert observed == ['active branch'] and c.logs == 1, (observed, c.logs)
        coverage.update(c.coverage)
        cases += 1
    result = dict(status='PASS', cases=cases, tap_bytes_checked=cases*132,
                  distinct_instruction_addresses=len(coverage),
                  limitations=['Complete already-active trigger branch, valid empty diagnostic strings and inert logger; no appliance services.',
                               'Existing selected fade is reinitialized after diagnostics, not ignored. Supplied active histories and noncrossing target; no allocation/event/audio scheduling or zero duration.'])
    (ROOT/'probes/active_fade_retrigger_probe_results.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
