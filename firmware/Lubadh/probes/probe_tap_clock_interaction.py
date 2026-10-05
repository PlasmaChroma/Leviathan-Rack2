#!/usr/bin/env python3
"""Complete tempoTap, tap reset event and interleaved tempoClock consumers."""
import itertools
import json
import math
import struct
from arm_byte_probe import ROOT
from arm_leaf_probe import f32
from unicorn.arm_const import UC_ARM_REG_S0
from probe_external_clock import ClockBytes, contents
from probe_speed_consumers import CHANNEL, LINK, PRESET, slew_count


def deque(c, offset, mapping, block, values):
    c.putu(CHANNEL + offset, mapping)
    c.putu(CHANNEL + offset + 4, 8)
    c.putu(mapping + 12, block)
    c.write(block, struct.pack('<' + 'i' * len(values), *values))
    for header, current in ((CHANNEL + offset + 8, block), (CHANNEL + offset + 24, block + 4 * len(values))):
        for i, pointer in enumerate((current, block, block + 512, mapping + 12)):
            c.putu(header + i * 4, pointer)


def magnitude(numerator, denominator):
    ratio = math.copysign(math.inf, denominator) if denominator == 0. else f32(numerator / denominator)
    return max(f32(.01), min(4., ratio))


def main():
    sequence = [('tap', 500000000), ('tap', 400000000), ('clock', 600000000),
                ('tap', 1000000000), ('reset', 0), ('tap', 10000000),
                ('clock', 500000000), ('tap', 500000000), ('clock', 2000000000),
                ('tap', 500000000), ('tap', 0), ('tap', -10000000),
                ('reset', 0), ('tap', 500000000), ('tap', 15000000000)]
    comparisons, calls, sequences, coverage, traces = 0, 0, 0, set(), []
    span_cases = 0
    for region, division, speed in itertools.product((128, 1000, 49170, 100000, 16777219),
                                                    (0, 1, 3, 64), (0., -.125, .125, -1., 1., 4.)):
        c = ClockBytes()
        c.putu(CHANNEL + 232, LINK)
        c.putf(LINK, speed)
        c.putu(CHANNEL + 172032 + 0x4D8, division)
        c.reg(4, CHANNEL)
        c.reg(5, CHANNEL + 172032)
        c.cpu.reg_write(UC_ARM_REG_S0 + 24, region)
        c.call(0x48C80, stop_before=0x48CB0)
        span = math.trunc(f32(f32(region) / f32(division))) if division else math.trunc(f32(region))
        numerator = f32(f32(span / 2.) * 1000.)
        denominator = abs(f32(speed)) * 2.7 * 49170.25390625
        ratio = numerator / denominator if denominator else math.inf
        limit = 5 if ratio >= 5 else (math.trunc(ratio) if ratio > 2 else 2)
        assert c.gets(CHANNEL + 172032 + 0x4F4) == span
        assert c.gets(CHANNEL + 172032 + 0x4D4) == limit
        comparisons += 2
        span_cases += 1
        coverage.update(c.coverage)
    for mode, reverse, first, span, time in itertools.product(range(4), range(3), range(2),
                                                           (1, 1000, 100000), (0., 2.9, 25.)):
        initial_first = first
        c = ClockBytes()
        c.putu(CHANNEL + 232, LINK)
        c.putu(LINK + 176, PRESET)
        c.putu(PRESET + 64, mode)
        c.putu(PRESET + 28, 4)
        c.putu(PRESET + 32, 64)
        c.putu(PRESET + 36, 1)
        c.write(CHANNEL + 472, b'\1')
        c.write(CHANNEL + 480, bytes([first]))
        c.putu(CHANNEL + 236, reverse)
        c.puts(CHANNEL + 172032 + 0x498, 2048 if reverse == 0 else 2047)
        c.putu(CHANNEL + 172032 + 0x4F4, span)
        c.putu(LINK + 40, 100000)
        c.putf(LINK + 172, time)
        c.putf(CHANNEL + 728, .75)
        c.putf(CHANNEL + 44, 1.25)
        deque(c, 484, 0x180000, 0x190000, [11, 22, 33, 44])
        deque(c, 524, 0x181000, 0x191000, [0, 0, 0, 0])
        previous = c.now = 1000000000
        c.write(CHANNEL + 464, struct.pack('<q', previous))
        target, increment, remaining, window = 1.25, 0., 0, [0.] * 4
        trace = []
        for event, elapsed in sequence:
            c.now += elapsed
            interval = f32(f32(c.now - previous) / f32(1e9))
            c.reg(0, CHANNEL)
            if event == 'reset':
                c.reg(1, 9)
                c.call(0x3F1C4)
                first = 1
                assert c.getu(CHANNEL + 508) == c.getu(CHANNEL + 492)
                comparisons += 1
            else:
                c.call(0x37120 if event == 'tap' else 0x3EED8)
                if event == 'tap' and first:
                    first = 0
                elif event == 'tap' or interval <= 1.:
                    if event == 'tap':
                        numerator = f32(span)
                        denominator = f32(interval * f32(49170.25390625))
                    else:
                        window = window[1:] + [interval]
                        total = 0.
                        for value in window:
                            total = f32(total + value)
                        numerator = f32(100000. / 64.)
                        denominator = f32(f32(total / 4.) * f32(49170.25390625))
                    target = magnitude(numerator, denominator)
                    if reverse != 0:
                        target = -target
                    remaining = slew_count(time)
                    increment = f32(f32(target - .75) / remaining)
                previous = c.now
            assert c.read(CHANNEL + 480, 1)[0] == first
            assert struct.unpack('<q', c.read(CHANNEL + 464, 8))[0] == previous
            assert c.getf(CHANNEL + 44) == target, (mode, reverse, event, span, time, target, c.getf(CHANNEL + 44))
            assert c.getf(CHANNEL + 732) == increment
            assert c.gets(CHANNEL + 736) == remaining
            assert c.getf(CHANNEL + 728) == .75
            assert contents(c) == window
            comparisons += 7
            calls += 1
            trace.append(dict(event=event, elapsed_ns=elapsed, first_tap=first, timestamp=previous,
                              target=target, remaining=remaining, window=window))
        coverage.update(c.coverage)
        sequences += 1
        if mode == 0 and reverse == 0 and span == 100000 and time == 25.:
            traces.append(dict(initial_first_tap=initial_first, steps=trace))
    result = dict(status='PASS', sequences=sequences, consumer_calls=calls, comparisons=comparisons,
                  span_publication_cases=span_cases,
                  distinct_instruction_addresses=len(coverage), traces=traces,
                  limitations=['Synthetic system-clock boundary; complete consumer calls, not upstream physical button/clock producers.',
                               'Initialized deque fixtures, one-node integer reset and four-entry clock window.',
                               'Tap span supplied in interleaved suite; callback span/limit slice separately checked with supplied region/division/live speed.',
                               'No linked deck event forwarding or audible transport scheduling.'])
    (ROOT / 'probes/tap_clock_interaction_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'traces'}, indent=2))


if __name__ == '__main__':
    main()
