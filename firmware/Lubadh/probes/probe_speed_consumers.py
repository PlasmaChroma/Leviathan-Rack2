#!/usr/bin/env python3
"""Original main-ELF speed quantization, slew, snap and tap-speed consumers."""
import json
import math
import struct
from unicorn.arm_const import UC_ARM_REG_SP
from arm_byte_probe import ARMBytes, ROOT, STACK, EXPECTED_SHA256
from arm_leaf_probe import f32
from speed_table_model import normalized_markers

CHANNEL, LINK, PRESET, MARKERS = 0x100000, 0x140000, 0x150000, 0x160000


def fixture(mode=0, time=25.):
    c = ARMBytes()
    c.putu(CHANNEL + 232, LINK)
    c.putu(LINK + 176, PRESET)
    c.putu(PRESET + 64, mode)
    c.putf(LINK + 172, time)
    return c


def slew_count(time):
    return math.trunc(f32(f32(f32(math.trunc(f32(time))) / f32(2.7)) + 1.))


def main():
    coverage, counts, drift = set(), {}, []
    comparisons = 0
    for markers in ([0., .5, 1., 2., 4.], [0., .25, .5, 1., 2., 4.], [.5]):
        values = normalized_markers(markers)
        c = fixture()
        c.vector(LINK + 180, MARKERS, values)
        candidates = [-6., 6.] + values
        for a, b in zip(values, values[1:]):
            mid = f32(f32(a + b) * .5)
            candidates.extend([f32(mid - .000001), mid, f32(mid + .000001)])
        for x in candidates:
            c.reg(0, CHANNEL)
            c.fp(0, x)
            c.call(0x3748C)
            expected = values[0] if x < values[0] else values[-1]
            for a, b in zip(values, values[1:]):
                if a <= x <= b:
                    expected = b if x >= f32(f32(a + b) * .5) else a
                    break
            assert c.fp(0) == expected, (values, x, c.fp(0), expected)
            comparisons += 1
        coverage.update(c.coverage)
    counts['quantizer'] = comparisons
    before = comparisons
    for time in (0., 2.9, 5., 25., 100.):
        for start, target in [(-2., 4.), (1., -1.), (.1, .7), (0., 0.)]:
            for frames in (1, 16, 128):
                c = fixture(time=time)
                start, target = f32(start), f32(target)
                c.putf(CHANNEL + 728, start)
                c.reg(0, CHANNEL)
                c.fp(0, target)
                c.call(0x372D4)
                n = slew_count(time)
                increment = f32(f32(target - start) / f32(n))
                assert c.getf(CHANNEL + 44) == target
                assert c.gets(CHANNEL + 736) == n
                assert c.getf(CHANNEL + 732) == increment
                comparisons += 3
                current = start
                for i in range(n + 2):
                    c.putf(LINK, f32(.33))
                    c.reg(0, CHANNEL)
                    c.reg(1, frames)
                    c.cpu.reg_write(UC_ARM_REG_SP, STACK)
                    c.call(0x37408, stop_before=0x37444)
                    if i < n:
                        current = f32(current + increment)
                    assert c.getf(CHANNEL + 728) == current
                    assert c.gets(CHANNEL + 736) == max(0, n - i - 1)
                    assert c.getf(CHANNEL + 40) == f32(.33)
                    comparisons += 3
                drift.append(dict(time=time, start=start, target=target, frames=frames,
                                  calls=n, final=current, drift=current - target))
                coverage.update(c.coverage)
    counts['slew'] = comparisons - before
    before = comparisons
    for mode in range(4):
        for value in (-3., 0., 2.):
            c = fixture(mode=mode)
            c.putu(CHANNEL + 236, 7)
            c.putf(CHANNEL + 732, 99.)
            c.puts(CHANNEL + 736, 99)
            c.reg(0, CHANNEL)
            c.fp(0, value)
            c.call(0x3731C)
            assert all(c.getf(CHANNEL + a) == value for a in (44, 728, 36))
            assert c.getf(CHANNEL + 732) == 0.
            assert c.gets(CHANNEL + 736) == 0
            assert c.getu(CHANNEL + 236) == (int(value < 0) if mode == 3 else 7)
            comparisons += 6
            coverage.update(c.coverage)
    counts['snap'] = comparisons - before
    before = comparisons
    for mode in range(4):
        for raw in (0, 2047, 2048, 4095):
            for reverse in (0, 1):
                for x in (-1., 0., .01, .5, 4., 8.):
                    c = fixture(mode=mode)
                    c.putu(CHANNEL + 172032 + 0x498, raw)
                    c.putu(CHANNEL + 236, reverse)
                    c.putf(CHANNEL + 728, .75)
                    c.reg(0, CHANNEL)
                    c.fp(0, x)
                    c.call(0x37364)
                    value = max(f32(.01), min(4., f32(x)))
                    negative = bool(reverse) if mode == 3 else raw < 2048
                    if negative:
                        value = -value
                    n = slew_count(25.)
                    assert c.getf(CHANNEL + 44) == value
                    assert c.gets(CHANNEL + 736) == n
                    assert c.getf(CHANNEL + 732) == f32(f32(value - .75) / n)
                    comparisons += 3
                    coverage.update(c.coverage)
    counts['tap_speed'] = comparisons - before
    result = dict(status='PASS', comparisons=comparisons, comparisons_by_contract=counts,
                  main_sha256=EXPECTED_SHA256, distinct_instruction_addresses=len(coverage),
                  slew_fixtures=drift, limitations=[
                      'processSpeed stops before flutter and capacitive-touch processing.',
                      'Complete caller event sequences, ADC/CV pot handling and linked forwarding are not checked.',
                      'Finite valid marker lists and nonnegative slew times only.'])
    (ROOT / 'probes/speed_consumer_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k != 'slew_fixtures'}, indent=2))


if __name__ == '__main__':
    main()
