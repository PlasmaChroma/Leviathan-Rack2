#!/usr/bin/env python3
"""Complete original setState with real tap managers and inert log/semaphore services."""
import itertools
import json
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32
from probe_speed_consumers import CHANNEL, LINK, PRESET
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR

STATUS, ERASE, LENGTH = 0x190000, 0x191000, 0x192000
FADE = CHANNEL + 6504
STRINGS = {0x2EBC8, 0x2E8EC, 0x36FA8, 0x36EF8}
_SERVICE_SYMBOLS = {symbol['address']: symbol['name'] for symbol in ARMBytes().elf.symbols
                    if symbol['type'] == 2}
assert all('basic_string' in _SERVICE_SYMBOLS[address] or '__to_xstring' in _SERVICE_SYMBOLS[address]
           for address in STRINGS)
assert 'MessageLogger' in _SERVICE_SYMBOLS[0x6FF20]
assert 'Semaphore' in _SERVICE_SYMBOLS[0x70820]
assert '_M_append' in json.loads((ROOT / 'evidence/plt_map.json').read_text())['0x15e88']
_PLT = json.loads((ROOT / 'evidence/plt_map.json').read_text())
assert 'basic_string' in _PLT['0x15c00']
assert 'basic_string' in _PLT['0x1644c']


class StateBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.activations, self.posts = [], []
        self.observers[0x4EEA8] = lambda c: self.activations.append([c.reg(i) for i in range(4)])

    def _code(self, cpu, address, size, data):
        if address in STRINGS or address == 0x15C00:
            # Only diagnostic strings use these sites in this graph. Empty valid
            # SSO objects let original destructor/control instructions execute.
            out = self.reg(0)
            self.putu(out, out + 8)
            self.putu(out + 4, 0)
            self.write(out + 8, b'\0')
        elif address in (0x15E88, 0x1644C):  # diagnostic append keeps empty object
            pass
        elif address == 0x6FF20:
            pass
        elif address == 0x70820:
            self.posts.append(self.reg(0))
        else:
            super()._code(cpu, address, size, data)
            return
        cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))


def fixture(mode, armed, speed, fixed, playback, held, active, duration, cpu_class=StateBytes):
    c = cpu_class()
    c.putu(CHANNEL + 232, LINK)
    c.putu(LINK + 176, PRESET)
    c.putf(LINK, speed)
    c.putu(LINK + 132, mode)
    c.putu(LINK + 160, fixed)
    c.putu(LINK + 152, held)
    c.putu(LINK + 20, 1000)
    c.putu(LINK + 32, 9000)
    c.putu(LINK + 100, duration)
    c.putf(LINK + 8, .33)
    c.write(CHANNEL + 636, bytes([armed]))
    c.write(CHANNEL + 637, b'\x07')
    c.write(CHANNEL + 124, b'\0')
    c.puts(CHANNEL + 120, 77)
    c.putu(CHANNEL + 171868, STATUS)
    c.write(STATUS, b'\x07')
    c.putu(CHANNEL + 88, LENGTH)
    c.putu(LENGTH, 123)
    audio = CHANNEL + 171900
    c.putu(audio, CHANNEL)
    c.putu(audio + 8, ERASE)
    c.write(ERASE, b'\0')
    for manager in (CHANNEL + 872, CHANNEL + 3688):
        c.reg(0, manager)
        c.call(0x4CA68)
    if playback:
        for i, value in enumerate((CHANNEL + 872, 3456, duration, int(speed >= 0))):
            c.reg(i, value)
        c.call(0x4EEA8)
    c.write(FADE, bytes([active]))
    c.puts(FADE + 4, 31)
    c.putf(FADE + 8, .125)
    c.puts(FADE + 12, 17)
    c.putf(FADE + 16, .5)
    c.putf(FADE + 20, -.33)
    c.activations.clear()
    c.coverage.clear()
    return c


def main():
    cases, checks, coverage = 0, 0, set()
    for mode, armed, speed, fixed, playback, held, active, duration in itertools.product(
            range(3), range(2), (-1., 0., 1.), range(2), range(2), range(2), range(2), (128, 1000, 4096)):
        c = fixture(mode, armed, speed, fixed, playback, held, active, duration)
        c.reg(0, CHANNEL)
        c.reg(1, 2)
        c.call(0x3A018)
        position = 3456 if playback else (9000 if speed < 0 and not fixed else 1000)
        assert c.getu(CHANNEL + 632) == CHANNEL + 608
        assert c.activations == [[CHANNEL + 3688, position, duration, int(speed >= 0)]]
        assert c.read(CHANNEL + 636, 2) == (bytes([1, armed ^ 1]) if mode == 2 else bytes([armed, 7]))
        assert c.read(STATUS, 1) == b'\0'
        assert c.read(FADE, 1) == b'\1'
        assert (c.gets(FADE + 4), c.getf(FADE + 8), c.gets(FADE + 12), c.getf(FADE + 16)) == (
            (31, .125, 17, .5) if active else (0, 0., 0, 0.))
        assert c.getf(FADE + 20) == f32(256. / f32(duration))
        assert c.getf(CHANNEL + 3688 + 24) == 1.
        assert c.read(CHANNEL + 124, 1)[0] == held
        expected_anchor = position + (1 if speed < 0 and not fixed else -1) if held else 77
        assert c.gets(CHANNEL + 120) == expected_anchor
        checks += 10
        cases += 1
        coverage.update(c.coverage)
    simple = 0
    for target, mode, armed in itertools.product((0, 1, 3), range(3), range(2)):
        c = fixture(mode, armed, 1., 0, 1, 0, 0, 128)
        c.reg(0, CHANNEL)
        c.reg(1, target)
        c.call(0x3A018)
        assert c.getu(CHANNEL + 632) == CHANNEL + {0: 580, 1: 592, 3: 620}[target]
        assert c.read(CHANNEL + 636, 2) == (bytes([1, armed ^ 1]) if mode == 2 else bytes([armed, 7]))
        assert c.read(STATUS, 1)[0] == int(target != 3)
        assert c.activations == []
        if target == 0:
            assert c.posts == [CHANNEL + 171940]
            assert c.read(ERASE, 1) == b'\1'
            assert c.getu(LENGTH) == 0
            for manager in (CHANNEL + 872, CHANNEL + 3688):
                assert all(c.read(manager + engine * 680 + slot * 132, 1) == b'\0'
                           for engine in range(4) for slot in range(5))
        elif target == 1:
            assert c.read(CHANNEL + 740, 1) == b'\1'
            assert c.gets(CHANNEL + 744) == 0
            assert c.getf(CHANNEL + 764) == 1.
        else:
            assert c.read(CHANNEL + 872, 1) == b'\1'
        checks += 4
        cases += 1
        simple += 1
        coverage.update(c.coverage)
    connected = 0
    for mode, armed, active, duration in itertools.product(range(3), range(2), range(2), (128, 1000, 4096)):
        c = fixture(mode, armed, 1., 0, 1, 0, active, duration)
        for offset, method, label in ((608, 0x3A4EC, 2), (620, 0x3A5A0, 3)):
            table = 0x1A0000 + label * 16
            c.putu(CHANNEL + offset, table)
            c.putu(CHANNEL + offset + 4, label)
            c.putu(CHANNEL + offset + 8, CHANNEL)
            c.putu(table, method)
        c.putu(CHANNEL + 632, CHANNEL + 608)
        c.write(CHANNEL + 651, b'\1')
        c.reg(0, CHANNEL)
        c.reg(1, 5)
        c.call(0x3F1C4)
        coords = (31, .125, 17, .5) if active else (254, 0., 254, 0.)
        assert c.getu(CHANNEL + 632) == CHANNEL + 620
        assert c.read(CHANNEL + 651, 1) == b'\0'
        assert c.getf(FADE + 20) == -f32(256. / f32(duration))
        assert (c.gets(FADE + 4), c.getf(FADE + 8), c.gets(FADE + 12), c.getf(FADE + 16)) == coords
        c.reg(0, CHANNEL)
        c.reg(1, 4)
        c.call(0x3F1C4)
        assert c.getu(CHANNEL + 632) == CHANNEL + 608
        assert c.read(CHANNEL + 651, 1) == b'\1'
        assert c.getf(FADE + 20) == f32(256. / f32(duration))
        assert (c.gets(FADE + 4), c.getf(FADE + 8), c.gets(FADE + 12), c.getf(FADE + 16)) == coords
        assert c.activations == [[CHANNEL + 3688, 3456, duration, 1]]
        connected += 1
        checks += 9
        coverage.update(c.coverage)
    result = dict(status='PASS', state_cases=cases, simple_state_cases=simple,
                  connected_gate_round_trips=connected,
                  comparisons=checks, distinct_instruction_addresses=len(coverage),
                  limitations=['Complete setState and nested original managers/fades/AudioData erase publication.',
                               'Diagnostic string services supply empty SSO objects; logging inert; semaphore post recorded without worker execution.',
                               'Fresh record pool; one existing playback head or none; allocation exhaustion and arbitrary prior-state histories not covered.',
                               'Connected gate round trips cover Playback/Overdub only; FirstRec/Empty gate histories and asynchronous erase completion/tape writes not executed.'])
    (ROOT / 'probes/channel_set_state_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
