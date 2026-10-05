#!/usr/bin/env python3
"""Original callback fade move/dispatch/expiry with explicit inert recordInput boundary."""
import itertools
import json
import math
from arm_byte_probe import ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_channel_set_state import StateBytes, fixture, FADE
from probe_speed_consumers import CHANNEL, PRESET
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR


class ExpiryBytes(StateBytes):
    def __init__(self):
        super().__init__()
        self.records = []

    def _code(self, cpu, address, size, data):
        if address == 0x47818:
            self.records.append(dict(tap=self.reg(2), fade_position=self.gets(FADE + 4),
                                     fade_fraction=self.getf(FADE + 8), fade_active=self.read(FADE, 1)[0]))
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, data)


def main():
    cases, checks, coverage, lower, upper, record_calls = 0, 0, set(), 0, 0, 0
    for hold, position, fraction, step, frames, factor, head in itertools.product(
            range(2), (-1, 0, 1, 253, 254, 255), (0., .75), (-2., -.125, .125, 2.),
            (1, 32, 128), (0., .25, 1., 4.), range(2)):
        c = fixture(0, 0, 1., 0, 0, 0, 1, 128, cpu_class=ExpiryBytes)
        c.putu(PRESET + 4, hold)
        c.puts(FADE + 4, position)
        c.putf(FADE + 8, fraction)
        c.putf(FADE + 20, step)
        if head:
            for i, value in enumerate((CHANNEL + 3688, 700, 128, 1)):
                c.reg(i, value)
            c.call(0x4EEA8)
        c.records.clear()
        amount = f32(frames) if hold else f32(f32(frames) * f32(factor))
        part = libm.fmaf(f32(step), amount, f32(fraction))
        whole = math.floor(part)
        expected_position = position + whole
        expected_fraction = f32(part - whole)
        expired_low, expired_high = expected_position < 0, expected_position > 254
        def stop(machine):
            machine.returned = True
            machine.cpu.emu_stop()
        c.observers[0x48544] = stop
        c.reg(4, CHANNEL)
        c.reg(6, CHANNEL + 4096)
        c.fp(16, frames)
        c.fp(21, factor)
        c.putu(STACK + 0x30, CHANNEL + 3688)
        c.putu(STACK + 0x3C, 0)
        c.call(0x48C58)
        assert c.records == ([dict(tap=CHANNEL + 3688, fade_position=expected_position,
                                  fade_fraction=expected_fraction, fade_active=1)] if head else [])
        assert c.read(FADE, 1)[0] == int(not (expired_low or expired_high))
        assert c.gets(FADE + 4) == (0 if expired_low or expired_high else expected_position)
        assert c.getf(FADE + 8) == (0. if expired_low or expired_high else expected_fraction)
        assert c.getf(FADE + 20) == (1. if expired_low or expired_high else f32(step))
        assert c.read(CHANNEL + 3688, 1)[0] == int(head and not expired_low)
        cases += 1
        checks += 6
        lower += int(expired_low)
        upper += int(expired_high)
        record_calls += len(c.records)
        coverage.update(c.coverage)
    result = dict(status='PASS', callback_cases=cases, comparisons=checks, lower_expiries=lower,
                  upper_expiries=upper, observed_record_calls=record_calls,
                  distinct_instruction_addresses=len(coverage),
                  limitations=['Original fade movement, real active-list dispatch, expiry and record-manager reset; stops before manager update.',
                               'recordInput is an explicit inert observer: no tape-write/envelope-boundary arithmetic checked.',
                               'Supplied active fade, empty/fresh one-head record pool, looping-mode and callback frame/motion factors.'])
    (ROOT / 'probes/write_fade_expiry_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
