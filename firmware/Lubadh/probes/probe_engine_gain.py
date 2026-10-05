#!/usr/bin/env python3
"""Original engine-count gain state machine and additive output mixing."""
import hashlib
import itertools
import json
import math
import struct

from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32, libm
from probe_tap_allocation import call

CHANNEL = 0x100000
MANAGER, WORK = CHANNEL + 872, CHANNEL + 172032
RAW, OUTPUT, HEADER = 0x180000, 0x190000, 0x1A0000
GAIN = CHANNEL + 6528
BASE = struct.unpack('<f', struct.pack('<I', 0x3F51EB85))[0]


def main():
    steady = [0, 0] + sum(([n] * 10 for n in (1, 2, 4, 3, 0, 4)), [])
    interrupted = [1, 2, 4, 3, 2, 0, 1, 4, 0, 3] + [3] * 10
    rows, coverage, comparisons, max_error = [], set(), 0, 0.
    for frames, slots, sequence in itertools.product((0, 1, 7, 32, 128), (1, 5), (steady, interrupted)):
        cpu = ARMBytes()
        cpu.reg(4, CHANNEL)
        cpu.call(0x3CF0C, stop_before=0x3CF34)
        assert cpu.read(GAIN, 16) == bytes(16)
        gain, increment, remaining, cached = 0., 0., 0, 0
        history = []
        for block, engines in enumerate(sequence):
            call(cpu, 0x4CA68, MANAGER)
            for engine in range(engines):
                assert call(cpu, 0x4EEA8, MANAGER, 1000, 32, True) == MANAGER + engine * 680
                for slot in range(1, slots):
                    assert call(cpu, 0x4E460, MANAGER, 1000, engine) == MANAGER + engine * 680 + slot * 132
            target = 1.
            for _ in range(max(0, engines - 1)):
                target = f32(target * BASE)
            if engines != cached:
                increment = f32(f32(target - gain) * .125)
                remaining, cached = 8, engines
            if remaining > 0:
                gain = f32(gain + increment)
                remaining -= 1
            raw = [f32(.3 * math.sin(i * .137 + block)) for i in range(frames)]
            prior = [f32(.1 * math.cos(i * .071 - block)) for i in range(frames)]
            expected = [libm.fmaf(a, gain, b) for a, b in zip(raw, prior)]
            cpu.vector(WORK + 12, RAW, raw)
            cpu.vector(HEADER, OUTPUT, prior)
            cpu.putu(STACK + 0x20, MANAGER)
            cpu.puts(STACK + 0x2C, frames)
            cpu.reg(4, CHANNEL)
            cpu.reg(5, WORK)
            cpu.reg(10, HEADER)
            cpu.call(0x483B0, stop_before=0x48454)
            state = dict(gain=cpu.getf(GAIN), increment=cpu.getf(GAIN + 4),
                         remaining=cpu.gets(GAIN + 8), cached=cpu.gets(GAIN + 12))
            assert state == dict(gain=gain, increment=increment, remaining=remaining, cached=cached)
            comparisons += 4
            output = cpu.floats(OUTPUT, frames)
            for a, b in zip(expected, output):
                error = abs(a - b)
                assert error <= 1e-7, (frames, engines, block, a, b)
                max_error = max(max_error, error)
                comparisons += 1
            history.append(dict(block=block, engines=engines, target=target, state=state, output=output))
        rows.append(dict(frames=frames, slots_per_engine=slots,
                         sequence='steady' if sequence is steady else 'interrupted', blocks=history))
        coverage.update(cpu.coverage)
    trajectories = {(r['frames'], r['slots_per_engine'], r['sequence']):
                    [b['state'] for b in r['blocks']] for r in rows}
    trajectory_assertions = 0
    for frames, name in itertools.product((0, 1, 7, 32, 128), ('steady', 'interrupted')):
        assert trajectories[frames, 1, name] == trajectories[frames, 5, name]
        trajectory_assertions += 1
        if frames:
            for slots in (1, 5):
                assert trajectories[frames, slots, name] == trajectories[0, slots, name]
                trajectory_assertions += 1
    result = dict(status='PASS', sequences=len(rows), blocks=sum(len(r['blocks']) for r in rows),
                  trajectory_assertions=trajectory_assertions,
                  comparisons=comparisons, max_abs_error=max_error, base=BASE,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  fixtures=rows, distinct_instruction_addresses=len(coverage),
                  coverage_addresses=[hex(a) for a in sorted(coverage)],
                  limitations=['Selected gain/mix slice only; AntiAlias and coloration are outside this execution.',
                               'Manager is rebuilt to realize count fixtures; no natural expiry scheduling.',
                               'Zero-frame blocks are explicit slice inputs, not established hardware callbacks.',
                               'Output starts with supplied nonzero values; upstream monitoring origin remains open.'])
    (ROOT / 'probes/engine_gain_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'sequences', 'blocks', 'comparisons',
                                            'max_abs_error', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
