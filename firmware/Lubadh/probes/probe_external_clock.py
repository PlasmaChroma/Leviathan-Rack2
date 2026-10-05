#!/usr/bin/env python3
"""Complete original tempoClock, preset deque resize and independent estimator."""
import itertools
import json
import math
import struct
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR
from arm_byte_probe import ARMBytes, ROOT
from arm_leaf_probe import f32
from probe_speed_consumers import CHANNEL, LINK, PRESET, slew_count

PLT = json.loads((ROOT / 'evidence/plt_map.json').read_text())
assert PLT['0x15918'] == '_ZNSt6chrono3_V212system_clock3nowEv'
assert PLT['0x15eac'] == '__aeabi_l2f'
MAP, BLOCK = 0x180000, 0x190000


class ClockBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.now = 0

    def _code(self, cpu, address, size, user):
        if address == 0x15918:
            self.coverage.add(address)
            self.write(self.reg(0), struct.pack('<q', self.now))
        elif address == 0x15EAC:
            self.coverage.add(address)
            value = struct.unpack('<q', struct.pack('<II', self.reg(0), self.reg(1)))[0]
            self.reg(0, struct.unpack('<I', struct.pack('<f', f32(value)))[0])
        else:
            super()._code(cpu, address, size, user)
            return
        cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))


def contents(c):
    current, node, end = c.getu(CHANNEL + 532), c.getu(CHANNEL + 544), c.getu(CHANNEL + 548)
    last = c.getu(CHANNEL + 540)
    values = []
    while current != end:
        values.append(c.getf(current))
        current += 4
        if current == last:
            node += 4
            current = c.getu(node)
            last = current + 512
        assert len(values) <= 128
    return values


def main():
    coverage, comparisons, pulses, sequences, traces = set(), 0, 0, 0, []
    for average, resolution, division, timeout, mode, reverse in itertools.product(
            (1, 4, 16), (0, 4, 64), (0, 3), (0, 1, 3), (0, 3), (0, 1)):
        c = ClockBytes()
        c.putu(CHANNEL + 232, LINK)
        c.putu(LINK + 176, PRESET)
        c.putu(PRESET + 28, average)
        c.putu(PRESET + 32, resolution)
        c.putu(PRESET + 36, timeout)
        c.putu(PRESET + 64, mode)
        c.write(CHANNEL + 472, bytes([int(timeout != 0)]))
        c.putu(CHANNEL + 236, reverse)
        c.puts(CHANNEL + 172032 + 0x498, 2047 if reverse else 2048)
        c.putu(CHANNEL + 172032 + 0x4D8, division)
        c.putu(LINK + 40, 100000)
        c.putf(LINK + 172, 25.)
        c.putf(CHANNEL + 728, .75)
        c.putf(CHANNEL + 44, 1.25)
        c.now = 1000000000
        c.reg(4, CHANNEL)
        c.reg(5, CHANNEL + 172032)
        c.call(0x3D844, stop_before=0x3D864)
        assert struct.unpack('<q', c.read(CHANNEL + 464, 8))[0] == c.now
        comparisons += 1
        # A valid empty libstdc++ deque header; original preset resize fills it.
        c.putu(CHANNEL + 524, MAP)
        c.putu(CHANNEL + 528, 8)
        c.putu(MAP + 12, BLOCK)
        for header in (CHANNEL + 532, CHANNEL + 548):
            for i, pointer in enumerate((BLOCK, BLOCK, BLOCK + 512, MAP + 12)):
                c.putu(header + 4 * i, pointer)
        c.reg(6, CHANNEL)
        c.call(0x3E428, stop_before=0x3E450)
        window = [0.] * (average if resolution > 0 else 1)
        assert contents(c) == window
        comparisons += 1
        previous, target, increment, remaining = 1000000000, 1.25, 0., 0
        trace = []
        for tick in range(300):
            interval_ns = (500000000, 400000000, 600000000, 1000000000,
                           1000000128, 2000000000, 10000000, 0, -10000000,
                           3000000000, 3000000256, 5000000000)[tick % 12]
            c.now = previous + interval_ns
            c.reg(0, CHANNEL)
            c.call(0x3EED8)
            interval = f32(f32(interval_ns) / f32(1e9))
            previous = c.now  # Also updated on rejected intervals.
            rejected = bool(timeout and interval > f32(timeout))
            if not rejected:
                window = window[1:] + [interval]
                total = 0.
                for value in window:
                    total = f32(total + value)
                denominator = f32(f32(total / f32(average)) * f32(49170.25390625))
                selected = resolution if resolution else division
                numerator = f32(100000. / selected) if selected else 100000.
                value = math.copysign(math.inf, numerator * denominator) if denominator == 0. else f32(numerator / denominator)
                value = max(f32(.01), min(4., value))
                target = -value if reverse else value
                remaining = slew_count(25.)
                increment = f32(f32(target - .75) / remaining)
            assert struct.unpack('<q', c.read(CHANNEL + 464, 8))[0] == previous
            assert contents(c) == window, (tick, contents(c), window)
            assert c.getf(CHANNEL + 44) == target, (average, resolution, division, timeout, tick, c.getf(CHANNEL + 44), target)
            assert c.getf(CHANNEL + 732) == increment
            assert c.gets(CHANNEL + 736) == remaining
            assert c.getf(CHANNEL + 728) == .75
            comparisons += 6
            pulses += 1
            if tick < 18:
                trace.append(dict(tick=tick, interval_ns=interval_ns, rejected=rejected, window=window,
                                  target=target, increment=increment, remaining=remaining))
        coverage.update(c.coverage)
        sequences += 1
        if average == 4 and division == 3 and mode == 0 and reverse == 0:
            traces.append(dict(average=average, resolution=resolution, division=division, timeout=timeout, steps=trace))
    result = dict(status='PASS', sequences=sequences, pulses=pulses, comparisons=comparisons,
                  distinct_instruction_addresses=len(coverage), traces=traces,
                  limitations=['Synthetic system-clock nanoseconds and explicit host int64-to-float service; no OS or appliance execution.',
                               'Deque header fixture; original preset resize and complete tempoClock execute, including node allocation/rotation.',
                               'Original constructor timestamp slice executes; clock value synthetic and physical first-clock event not executed.',
                               'Clock jack/gesture producers, asynchronous timeout handling, linked events and audio scheduling remain open.',
                               'Positive nonzero averages; no invalid preset or NaN fixture.'])
    (ROOT / 'probes/external_clock_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'traces'}, indent=2))


if __name__ == '__main__':
    main()
