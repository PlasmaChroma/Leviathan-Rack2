#!/usr/bin/env python3
"""Complete gate wrappers and three original state consumers; setState recorder."""
import itertools
import json
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32, libm
from probe_speed_consumers import CHANNEL, LINK, PRESET
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR

STATE, VTABLE = 0x160000, 0x161000
METHODS = {0: 0x3A470, 2: 0x3A4EC, 3: 0x3A5A0}
FADE = CHANNEL + 6504


class GateBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.requests = []
        self.state_events = []
        for address in METHODS.values():
            self.observers[address] = lambda c: self.state_events.append(c.reg(1))

    def _code(self, cpu, address, size, data):
        if address == 0x3A018:
            assert self.reg(0) == CHANNEL
            self.requests.append(self.reg(1))
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, data)


def main():
    coverage, cases, comparisons, moves = set(), 0, 0, 0
    for state, event, mode, armed, active, duration in itertools.product(
            METHODS, (4, 5), range(3), range(2), range(2), (128, 1000, 4096)):
        c = GateBytes()
        c.putu(CHANNEL + 232, LINK)
        c.putu(LINK + 176, PRESET)
        c.putu(CHANNEL + 632, STATE)
        c.putu(STATE, VTABLE)
        c.putu(STATE + 4, state)
        c.putu(STATE + 8, CHANNEL)
        c.putu(VTABLE, METHODS[state])
        c.putu(LINK + 132, mode)
        c.putu(LINK + 100, duration)
        c.write(CHANNEL + 636, bytes([armed]))
        c.write(CHANNEL + 637, b'\x07')
        c.write(CHANNEL + 651, bytes([int(event == 5)]))
        c.write(CHANNEL + 645, b'\0')
        c.write(CHANNEL + 124, b'\1')
        c.write(FADE, bytes([active]))
        for field, value in ((4, 31), (12, 17)):
            c.puts(FADE + field, value)
        for field, value in ((8, .125), (16, .5), (20, .33)):
            c.putf(FADE + field, value)
        expected_fade = [active, 31, .125, 17, .5, f32(.33)]
        expected_requests = [1 if state == 0 else 2] if event == 4 and state in (0, 3) else []
        expected_record_flag = 1
        if state == 2 and event == 5:
            expected_requests = [3]
            expected_record_flag = 0
            if not active:
                expected_fade = [1, 254, 0., 254, 0., f32(.33)]
            expected_fade[5] = -f32(256. / f32(duration))
        c.reg(0, CHANNEL)
        c.reg(1, event)
        c.call(0x3F1C4)
        observed_fade = [c.read(FADE, 1)[0], c.gets(FADE + 4), c.getf(FADE + 8),
                         c.gets(FADE + 12), c.getf(FADE + 16), c.getf(FADE + 20)]
        assert observed_fade == expected_fade, (state, event, active, duration, observed_fade, expected_fade)
        assert c.requests == expected_requests
        assert c.state_events == [6 if event == 4 else 7]
        assert c.read(CHANNEL + 651, 1)[0] == int(event == 4)
        assert c.read(CHANNEL + 645, 1)[0] == 1
        assert c.read(CHANNEL + 124, 1)[0] == expected_record_flag
        assert c.read(CHANNEL + 636, 2) == bytes([armed, 7])
        comparisons += 12
        cases += 1
        if state == 2 and event == 5:
            # Carry independent event-produced fade state through original move,
            # including crossing below zero; callback expiry is a separate path.
            for frames in (1, 7, 32, 128) * 16:
                expected_fade[3:5] = expected_fade[1:3]
                fraction = libm.fmaf(expected_fade[5], f32(frames), expected_fade[2])
                while fraction >= 1.:
                    fraction = f32(fraction - 1.)
                    expected_fade[1] += 1
                while fraction < 0.:
                    fraction = f32(fraction + 1.)
                    expected_fade[1] -= 1
                expected_fade[2] = fraction
                c.reg(0, FADE)
                c.fp(0, frames)
                c.call(0x4BAFC)
                observed = [c.read(FADE, 1)[0], c.gets(FADE + 4), c.getf(FADE + 8),
                            c.gets(FADE + 12), c.getf(FADE + 16), c.getf(FADE + 20)]
                assert observed == expected_fade
                comparisons += 6
                moves += 1
        coverage.update(c.coverage)
    result = dict(status='PASS', gate_event_cases=cases, comparisons=comparisons,
                  event_produced_fade_moves=moves,
                  distinct_instruction_addresses=len(coverage),
                  limitations=['Complete Channel events 4/5 and real Empty/Overdub/Playback state methods.',
                               'setState requests recorded; full state transition graph not executed.',
                               'FirstRec state, linked dispatcher, callback fade expiry and tape writes outside this suite.'])
    (ROOT / 'probes/record_gate_event_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
