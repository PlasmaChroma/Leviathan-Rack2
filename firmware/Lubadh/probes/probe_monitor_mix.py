#!/usr/bin/env python3
"""Original post-color input-monitor copy/fade branch versus state recurrence."""
import hashlib
import itertools
import json
import struct

from arm_byte_probe import ARMBytes, ROOT, STACK
from arm_leaf_probe import f32

CHANNEL, SOURCE, DEST, SOURCE_DATA, DEST_DATA = 0x100000, 0x110000, 0x120000, 0x130000, 0x140000
STEP = struct.unpack('<f', struct.pack('<I', 0x3B8548AA))[0]


def main():
    cpu = ARMBytes()
    rows, comparisons, assertions = [], 0, 0
    for frames, monitoring, fading, initial in itertools.product((0, 7, 32, 128, 257), (0, 1), (0, 1), (-.01, 0., .001, .25, .5, .999, 1., 1.01)):
        source = [f32(-.4 + (i % 17) * .05) for i in range(frames)]
        prior = [f32(.1 + (i % 11) * .03) for i in range(frames)]
        cpu.vector(SOURCE, SOURCE_DATA, source)
        cpu.vector(DEST, DEST_DATA, prior)
        cpu.putf(DEST_DATA + 4 * frames, 123.)
        cpu.write(CHANNEL + 0x27C, bytes([monitoring, fading]))
        cpu.putf(CHANNEL + 640, initial)
        cpu.putu(STACK + 0x2C, frames)
        gain, flag = f32(initial), fading
        expected = list(source if monitoring or fading else prior)
        if fading:
            increment = STEP if monitoring else -STEP
            for i in range(frames):
                expected[i] = f32(source[i] * gain)
                gain = f32(gain + increment)
                if gain < 0.:
                    gain, flag = 0., 0
                    expected[i:] = [0.] * (frames - i)
                    break
                if gain > 1.:
                    gain, flag = 1., 0
                    break
        cpu.reg(4, CHANNEL)
        cpu.reg(6, SOURCE)
        cpu.reg(10, DEST)
        cpu.call(0x47D80, stop_before=0x47D98)
        actual = cpu.floats(DEST_DATA, frames)
        assert actual == expected, (frames, monitoring, fading, initial, expected, actual)
        assert cpu.getf(CHANNEL + 640) == gain
        assert cpu.read(CHANNEL + 0x27D, 1)[0] == flag
        assert cpu.floats(SOURCE_DATA, frames) == source
        assert cpu.getf(DEST_DATA + 4 * frames) == 123.
        comparisons += frames
        assertions += 4
        rows.append(dict(frames=frames, monitoring=monitoring, fading=fading,
                         initial_gain=f32(initial), final_gain=gain, final_fading=flag,
                         output=actual))
    result = dict(status='PASS', cases=len(rows), comparisons=comparisons, assertions=assertions,
                  step=STEP, fixtures=rows,
                  main_sha256=hashlib.sha256((ROOT / 'extracted/bin/lubadh_main').read_bytes()).hexdigest(),
                  distinct_instruction_addresses=len(cpu.coverage), coverage_addresses=[hex(a) for a in sorted(cpu.coverage)],
                  limitations=['Original post-color monitoring branch only; flags and gain seeded, producer and linked routing unrecovered.',
                               'Input and destination vectors have matching fixture frame counts; no invalid vector contract claim.',
                               'Negative/above-one initial gains are explicit branch fixtures, not recovered user-reachable states.',
                               'No complete Channel callback, input AntiAlias/history or final mix/effects connection.'])
    (ROOT / 'probes/monitor_mix_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: result[k] for k in ('status', 'cases', 'comparisons', 'assertions', 'step', 'distinct_instruction_addresses')}, indent=2))


if __name__ == '__main__':
    main()
