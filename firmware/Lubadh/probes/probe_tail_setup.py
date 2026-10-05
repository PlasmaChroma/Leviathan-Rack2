#!/usr/bin/env python3
"""Probe first-record completion bookkeeping, stopping before region setup.

Runs actual bytes 0x3a8a4..0x3a8e0. It does not emulate button eligibility,
setState, setLoopingParameters, or the callback that records the trailing audio.
"""
import hashlib
import json

from arm_byte_probe import ARMBytes, ROOT


def main():
    rows, coverage = [], set()
    channel, mode, length = 0x100000, 0x140000, 0x150000
    for counter in (128, 1024, 2458, 12292, 12293, 48000, 49170, 29501900):
        cpu = ARMBytes()
        cpu.putu(mode + 8, channel)
        cpu.putu(channel + 0x58, length)
        cpu.putu(channel + 0x2E8, counter)
        cpu.reg(4, mode)
        cpu.call(0x3A8A4, stop_before=0x3A8E4)
        result = dict(first_record_counter=counter, logical_length=cpu.getu(channel + 0x4C),
                      end_marker=cpu.getu(channel + 0x50),
                      maximum_fade_span=cpu.getu(channel + 0x84),
                      stored_length=cpu.getu(length))
        assert result['logical_length'] == counter
        assert result['end_marker'] == counter + 2458
        assert result['maximum_fade_span'] == min(counter - 1, 12292)
        assert result['stored_length'] == counter + 12292
        rows.append(result)
        coverage.update(cpu.coverage)
    result = dict(status='PASS', method='Original-byte first-record completion slice with explicit fixtures',
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  slice_start='0x3a8a4', stop_before='0x3a8e4', cases=rows, assertions=len(rows) * 4,
                  distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(address) for address in sorted(coverage)],
                  literals=dict(added_storage_samples=12292, added_end_marker_samples=2458),
                  duration_at_rates={str(rate): dict(tail_seconds=12292 / rate,
                                                     end_marker_seconds=2458 / rate)
                                     for rate in (48000., 49148., 49170.25390625)},
                  limitations=[
                      'Completion eligibility and actual trailing write scheduling are not exercised.',
                      'The semantic purpose of the separate end-marker offset remains to be traced.',
                      'Test counters are explicit fixtures, not evidence that all are UI-reachable.',
                      'Does not establish imported-file tail policies or capacity exhaustion behavior.',
                  ])
    (ROOT / 'probes/tail_setup_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
