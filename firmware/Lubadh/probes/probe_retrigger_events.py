#!/usr/bin/env python3
"""Original retrigger debounce, event countdown, delay queue and V/oct reverse."""
import itertools
import json
import struct
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR, UC_ARM_REG_SP
from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32
from probe_speed_consumers import CHANNEL, LINK, PRESET, slew_count

STATE, QUEUE = 0x160000, 0x170000
PLT = json.loads((ROOT / 'evidence/plt_map.json').read_text())
assert PLT['0x16074'] == '_ZNSt6chrono3_V212steady_clock3nowEv'


class RetrigBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.now = 100000000
        self.requests, self.activations, self.kills = [], [], []
        self.observers[0x380A0] = lambda c: self.requests.append(c.reg(0))
        self.observers[0x4EEA8] = lambda c: self.activations.append([c.reg(i) for i in range(4)])
        self.observers[0x4ED80] = lambda c: self.kills.append([c.reg(i) for i in range(3)])

    def _code(self, cpu, address, size, user):
        if address == 0x16074:
            self.coverage.add(address)
            self.write(self.reg(0), struct.pack('<q', self.now))
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, user)


def fixture(state=2, speed=1., held=0, retrig=0, record=0):
    c = RetrigBytes()
    c.putu(CHANNEL + 232, LINK)
    c.putu(LINK + 176, PRESET)
    c.putu(CHANNEL + 632, STATE)
    c.putu(STATE + 4, state)
    c.putf(LINK, speed)
    c.putf(LINK + 8, .33)
    c.putu(LINK + 20, 1000)
    c.putu(LINK + 32, 9000)
    c.putu(LINK + 100, 128)
    c.putu(LINK + 144, retrig)
    c.putu(LINK + 148, held)
    c.putu(LINK + 160, record)
    for manager in (CHANNEL + 872, CHANNEL + 3688):
        c.reg(0, manager)
        c.call(0x4CA68)
    c.vector(CHANNEL + 716, QUEUE, [], capacity=64, integers=True)
    c.coverage.clear()
    return c


def main():
    coverage, comparisons, cases = set(), 0, {}
    # Full doRetrig and nested original allocation/fade routines, fresh pools.
    for state, speed, held, retrig, record, elapsed in itertools.product(
            range(4), (-1., -.1, -.099, 0., .099, .1, 1.), range(2), range(2), range(2),
            (-1, 19999998, 19999999, 20000000, 20000001)):
        speed = f32(speed)
        c = fixture(state, speed, held, retrig, record)
        prior = 100000000
        c.write(CHANNEL + 568, struct.pack('<q', prior))
        c.now = prior + elapsed
        c.reg(0, CHANNEL)
        c.call(0x380A0)
        eligible_time = elapsed > 19999999
        timestamp = c.now if eligible_time else prior
        assert struct.unpack('<q', c.read(CHANNEL + 568, 8))[0] == timestamp
        eligible = eligible_time and not (held == 1 and abs(speed) < f32(.1)) and state in (2, 3)
        direction = int(speed >= 0.)
        position = 1000 if direction else 9000
        expected = [[CHANNEL + 872, position, 128, direction]] if eligible else []
        kills = [[CHANNEL + 872, 128, direction]] if eligible and retrig == 0 else []
        if eligible and state == 2 and record == 0 and retrig == 0:
            expected.append([CHANNEL + 3688, position, 128, direction])
            kills.append([CHANNEL + 3688, 128, direction])
        assert c.activations == expected, (state, speed, held, retrig, record, elapsed, c.activations, expected)
        assert c.kills == kills
        comparisons += 3
        coverage.update(c.coverage)
    cases['full_retrigger'] = 1120
    # Complete event 2: suppression count, active-menu gate and queue insertion.
    for pending, menu, delay in itertools.product((0, 1, 2, 5), range(2), (0, 1, 3, 10)):
        c = fixture(state=0)
        c.putu(CHANNEL + 272, pending)
        c.write(CHANNEL + 172032 + 0x328, bytes([menu]))
        c.putu(PRESET + 56, delay)
        expected_pending, queue, requests = pending, [], 0
        for tick in range(8):
            c.reg(0, CHANNEL)
            c.reg(1, 2)
            c.call(0x3F1C4)
            if not menu:
                if expected_pending:
                    expected_pending -= 1
                elif delay:
                    queue.append(delay)
                else:
                    requests += 1
            assert c.getu(CHANNEL + 272) == expected_pending
            assert c.getu(CHANNEL + 720) == QUEUE + 4 * len(queue)
            assert [c.gets(QUEUE + 4 * i) for i in range(len(queue))] == queue
            assert len(c.requests) == requests
            comparisons += 4
        coverage.update(c.coverage)
    cases['event_countdown_sequences'] = 32
    # Complete delayed queue maintenance for both decks; inert state avoids activation.
    stride = 173368
    for initial in ([], [1], [2, 1, 3], [1, 1, 1, 1, 1], [0, 2, 0, 1], [-1, 1, 4]):
        c = fixture(state=0)
        c.vector(CHANNEL + 716, QUEUE, initial, capacity=64, integers=True)
        other = CHANNEL + stride
        c.vector(other + 716, QUEUE + 4096, [], capacity=64, integers=True)
        queue, requests = list(initial), 0
        for tick in range(12):
            c.reg(6, CHANNEL)
            c.reg(7, CHANNEL + 2 * stride)
            c.cpu.reg_write(UC_ARM_REG_SP, STACK)
            c.call(0x493D0, stop_before=0x493F0)
            decremented = [n - 1 for n in queue]
            requests += decremented.count(0)
            queue = [n for n in decremented if n != 0]
            assert len(c.requests) == requests
            assert c.getu(CHANNEL + 720) == QUEUE + 4 * len(queue)
            assert [c.gets(QUEUE + 4 * i) for i in range(len(queue))] == queue
            comparisons += 3
        coverage.update(c.coverage)
    cases['delayed_queue_sequences'] = 6
    # Full V/oct direction event 8, including target inversion and slew setup.
    for mode, reverse, start, target in itertools.product(range(4), range(3), (-1., .5), (-2., 0., 3.)):
        c = fixture(state=0)
        c.putu(PRESET + 64, mode)
        c.putu(CHANNEL + 236, reverse)
        c.putf(LINK + 172, 25.)
        c.putf(CHANNEL + 728, start)
        c.putf(CHANNEL + 44, target)
        c.reg(0, CHANNEL)
        c.reg(1, 8)
        c.call(0x3F1C4)
        if mode == 3:
            expected_reverse = 1 - reverse if reverse in (0, 1) else reverse
            expected_target = -target
            n = slew_count(25.)
            assert c.gets(CHANNEL + 736) == n
            assert c.getf(CHANNEL + 732) == f32(f32(expected_target - start) / n)
            assert c.read(CHANNEL + 656, 8) == struct.pack('<II', 185, 30)
            comparisons += 3
        else:
            expected_reverse, expected_target = reverse, target
        assert c.getu(CHANNEL + 236) == expected_reverse
        assert c.getf(CHANNEL + 44) == expected_target
        assert c.getf(CHANNEL + 728) == start
        comparisons += 3
        coverage.update(c.coverage)
    cases['direction_events'] = 72
    result = dict(status='PASS', cases=cases, comparisons=comparisons,
                  distinct_instruction_addresses=len(coverage),
                  limitations=['Synthetic nanosecond steady_clock; no wall-clock or appliance execution.',
                               'Fresh tap pools in full-retrigger matrix; pool exhaustion belongs to separate tests.',
                               'Event numbers and state-object labels supplied; upstream gestures not executed.',
                               'Delayed queue slice executes both deck traversals, with second queue empty and inert state.',
                               'Full delayed-event audio render and physical timing not checked.'])
    (ROOT / 'probes/retrigger_event_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
