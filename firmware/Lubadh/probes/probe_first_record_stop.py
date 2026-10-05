#!/usr/bin/env python3
"""Complete first-record event consumer, real state/region/head consumers."""
import itertools
import json
from arm_byte_probe import ROOT
from probe_channel_set_state import fixture, LENGTH, STATUS
from probe_speed_consumers import CHANNEL, LINK, PRESET
from probe_loop_regions import model, read_state, LENGTH_ADC, START_ADC, WORK


def main():
    cases, checks, coverage, completions, rejected = 0, 0, set(), 0, 0
    for minimum, offset, event, mode, armed, speed, multitap in itertools.product(
            (128, 1280), (-1, 0, 1, 20000), (-1, 0, 1, 6, 7), range(3), range(2), (-1., 1.), range(2)):
        n = minimum * 4 + offset
        c = fixture(mode, armed, speed, 0, 0, 0, 0, 128)
        state = CHANNEL + 592
        c.putu(state + 8, CHANNEL)
        c.write(state + 12, b'\0')
        c.putu(CHANNEL + 632, state)
        c.putu(CHANNEL + 212, PRESET)
        c.putu(PRESET + 48, minimum)
        c.putu(PRESET + 64, 3)
        c.putu(CHANNEL + 236, 1)
        c.putu(LINK + 140, multitap)
        c.putu(LINK + 40, 49170)
        c.putu(LINK + 128, 4)
        c.putu(LINK + 24, 9000)
        c.putu(LINK + 28, 5000)
        c.putu(CHANNEL + 744, n)
        c.putu(CHANNEL + 76, 30000)
        c.putu(CHANNEL + 80, 32458)
        c.putu(CHANNEL + 128, 1000)
        c.putu(CHANNEL + 136, 128)
        c.putu(CHANNEL + 56, 1)
        c.putu(CHANNEL + 132, 512)
        c.puts(WORK + 0x4A4, 2048)
        c.puts(WORK + 0x4A0, 1024)
        c.puts(WORK + 0x4B4, 2048)
        for adc in (LENGTH_ADC, START_ADC):
            c.putu(adc + 4, 0)
            c.putu(adc + 8, 0)
            c.putu(adc + 12, 2)
        c.reg(0, state)
        c.reg(1, event)
        c.call(0x3A6AC)
        eligible = event in (0, 1, 7)
        enough = n >= minimum * 4
        assert c.read(state + 12, 1)[0] == int(eligible and not enough)
        complete = eligible and enough
        assert c.getu(CHANNEL + 632) == (CHANNEL + 620 if complete else state)
        if complete:
            expected = model(dict(period=n, span=49170, end_marker=n + 2458, divisions=4,
                                  old_start=1, old_length=1000, old_fade=128, maximum_fade=min(n-1, 12292),
                                  minimum=minimum, fade_control=2048, start_control=2048, length_control=1024,
                                  start_adc=0, start_previous=0, length_adc=0, length_previous=0,
                                  threshold=2, modifier=False, force=True, start_quantized=False,
                                  length_quantized=False, cursor=77, cursor_enabled=False,
                                  link_start=1000, link_end=5000, link_reverse_end=9000, speed=speed))
            assert read_state(c) == expected, (n, event, read_state(c), expected)
            assert c.getu(CHANNEL + 76) == n
            assert c.getu(CHANNEL + 80) == n + 2458
            assert c.getu(CHANNEL + 132) == min(n - 1, 12292)
            assert c.getu(LENGTH) == n + 12292
            assert c.getu(STATUS + 4) == n + 12292
            assert c.getf(CHANNEL + 44) == 1.
            assert c.getf(CHANNEL + 728) == 1.
            assert c.getf(CHANNEL + 732) == 0.
            assert c.getu(CHANNEL + 736) == 0
            assert c.getu(CHANNEL + 236) == 0
            assert c.activations == ([] if multitap else [[CHANNEL + 872, 9000 if speed < 0 else 1000, 128, 1]])
            completions += 1
            checks += 12
        else:
            assert c.getu(CHANNEL + 76) == 30000
            assert c.getu(LENGTH) == 123
            assert c.activations == []
            rejected += int(eligible)
            checks += 3
        checks += 2
        cases += 1
        coverage.update(c.coverage)
    result = dict(status='PASS', event_cases=cases, completions=completions, too_short_rejections=rejected,
                  comparisons=checks, distinct_instruction_addresses=len(coverage),
                  limitations=['Complete FirstRec event method and original setState/setLoopingParameters/head consumers.',
                               'Inherited inert diagnostics and recorded semaphore service; supplied first-record counter and control fields.',
                               'Separate supplied Link pointer; no whole GPIO/button producer, callback counter growth, trailing audio writes or capacity limit.'])
    (ROOT / 'probes/first_record_stop_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
