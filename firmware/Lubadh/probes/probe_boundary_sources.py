#!/usr/bin/env python3
"""Connect inline region producers to original callback boundary loads.

Selected constructor/link pointer stores are executed, not complete hardware
initialization or link-event handling. No callback movement/render is run.
"""
import hashlib
import itertools
import json

from arm_byte_probe import ARMBytes, ROOT, STACK
from probe_loop_regions import control_samples

CHANNEL, PRESET, LENGTH = 0x100000, 0x180000, 0x190000
WORK = CHANNEL + 172032
LOCAL_FIELDS = {'start': 0x0C, 'end': 0x1C, 'reverse_start': 0x28,
                'reverse_end': 0x14, 'period': 0x38,
                'physical_reverse': 0x40, 'physical_destination': 0x48,
                'reduced_reverse_start': 0x24}


def boundary_load(cpu, channel):
    cpu.reg(4, channel)
    cpu.putu(STACK + 0x2C, 128)
    cpu.call(0x47C3C, stop_before=0x47CAC)
    return {name: cpu.gets(STACK + offset) for name, offset in LOCAL_FIELDS.items()}


def main():
    cases, coverage, comparisons = [], set(), 0
    for period, start_control, length_control, fade_control in itertools.product(
            (49170, 16777219, 29500000), (0, 2048, 4095), (0, 2048, 4095), (0, 4095)):
        cpu = ARMBytes()
        # r5 was advanced by 36 at 0x3cd90 before LinkData construction.
        cpu.reg(4, CHANNEL)
        cpu.reg(5, CHANNEL + 36)
        cpu.call(0x3CDA0, stop_before=0x3CDA8)
        assert cpu.getu(CHANNEL + 0xE8) == CHANNEL + 36
        cpu.putu(CHANNEL + 0x58, LENGTH)
        cpu.reg(0, CHANNEL)
        cpu.call(0x386A4)
        reset = {hex(offset): cpu.gets(CHANNEL + offset) for offset in
                 (0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 0x50, 0x80, 0x84, 0x88)}
        assert list(reset.values()) == [1, 1, 29501997, 29501997, 2459,
                                       29501998, 29501998, 1, 12292, 1]
        assert cpu.getu(LENGTH) == 0
        # Recorded-loop bookkeeping supplies the logical extent and tail.
        cpu.putu(CHANNEL + 0x4C, period)
        cpu.putu(CHANNEL + 0x50, period + 2458)
        cpu.putu(LENGTH, period + 12292)
        cpu.putu(CHANNEL + 0xD4, PRESET)
        cpu.puts(PRESET + 0x30, 1280)
        cpu.puts(CHANNEL + 36 + 0x80, 8)
        for offset, value in ((0x4A4, start_control), (0x4A0, length_control),
                              (0x4B4, fade_control)):
            cpu.puts(WORK + offset, value)
        history = []
        old_length, old_fade = 1, 1
        for step in range(4):
            cpu.reg(0, CHANNEL)
            cpu.reg(1, 0)
            cpu.reg(2, int(step == 0))
            cpu.call(0x376AC)
            start = max(1, min(period - 2, control_samples(start_control, period)))
            length = max(1280, min(period - 1, control_samples(length_control, period - 1)))
            fade = max(128, control_samples(fade_control, min(old_length // 2, 12292)))
            end = start + old_length
            if end >= period + 2458:
                end %= period
            ra = start + old_fade
            expected = dict(start=start, end=end, reverse_start=ra,
                            reverse_end=end + fade, period=period, physical_reverse=2459,
                            physical_destination=period + 2458,
                            reduced_reverse_start=ra % period if ra >= period + 2458 else ra)
            observed = boundary_load(cpu, CHANNEL)
            assert observed == expected, (period, step, expected, observed)
            assert cpu.gets(CHANNEL + 0x80) == length
            assert cpu.gets(CHANNEL + 0x88) == fade
            history.append(observed)
            comparisons += len(expected) + 2
            old_length, old_fade = length, fade
        assert history[2] == history[3]
        cases.append(dict(period=period, start_control=start_control,
                          length_control=length_control, fade_control=fade_control,
                          callback_loads=history))
        coverage.update(cpu.coverage)
    # Execute exact pointer-switch stores with explicit Application placement.
    cpu = ARMBytes()
    app = 0x100000
    left, right = app + 72, app + 173440
    cpu.putu(left + 0x58, LENGTH)
    cpu.putu(right + 0x58, LENGTH)
    for channel, origin in ((left, 100), (right, 200)):
        for offset in (0x38, 0x3C, 0x40, 0x44, 0x48):
            cpu.puts(channel + offset, origin + offset)
        cpu.puts(channel + 0x4C, 49170)
        cpu.puts(channel + 0x50, 51628)
    cpu.reg(4, app)
    cpu.call(0x27E10, stop_before=0x27E24)
    assert cpu.getu(right + 0xE8) == right + 36
    own = boundary_load(cpu, right)
    cpu.reg(8, app + 172032)
    cpu.reg(3, left + 36)
    cpu.call(0x27DAC, stop_before=0x27DB0)
    assert cpu.getu(right + 0xE8) == left + 36
    linked = boundary_load(cpu, right)
    for name in ('start', 'end', 'reverse_start', 'reverse_end', 'physical_reverse'):
        assert own[name] - linked[name] == 100
    coverage.update(cpu.coverage)
    result = dict(status='PASS', cases=len(cases), region_callback_comparisons=comparisons,
                  reset_assertions_per_case=12, link_pointer_assertions=7,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  inline_offset=36, reset=reset, fixtures=cases,
                  link_fixture=dict(own=own, linked=linked),
                  distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Constructor pointer-store slice only; no complete construction.',
                               'Link pointer stores only; hardware eligibility and state synchronization remain open.',
                               'Recorded-loop extent fields are supplied fixtures, not an executed recording session.',
                               'Callback boundary-load slice only; no movement, head iteration or rendering.',
                               'No scheduling/concurrency proof between IO and audio callbacks.'])
    (ROOT / 'probes/boundary_source_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'cases', 'region_callback_comparisons',
                                            'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
