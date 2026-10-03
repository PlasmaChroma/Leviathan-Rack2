#!/usr/bin/env python3
"""Optional original-code diagnosis; not needed by shipping code or tests.

Requires Unicorn 2.1.4 and the supplied firmware. Compare with the trace produced
by TIAMAT_TRACE=1 build/tests/tiamat_buffer_spec. Stops at the first mismatch.
Same prepared driver/substitutions as probe_time_memory.py; no full callback.
"""
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'firmware/Data_Bender/analysis/buffer_engine'))
from probe_engine import Engine, POOL
from unicorn import UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_R8
from generate_buffer_fixtures import FIELDS


def F(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


e = Engine(N=2400, speed=1, window=0)
e.track = False
for ch in (0, 1):
    e.wf(0xf8 + 4*ch, 1)
initial = struct.pack('<32768f', *[(-1 if (i//32)%2 else 1)*(.1+.5*i/32768) for i in range(32768)])
for ch in (0, 1):
    e.u.mem_write(POOL + ch*32768*4, initial)
e.wb(0x11a, 1)
e.wb(0x11b, 1)
native = iter((ROOT / 'build/tiamat-native-trace.txt').read_text().splitlines())
recent = []


def check(u, address, size, data):
    ch = u.reg_read(UC_ARM_REG_R8)
    frame = e.ri(0x128 + ch*4) + 1
    s = e.snapshot(ch)
    expected = [ch, frame] + [s[k] for k in FIELDS] + [bits(e.rf(0x50)), bits(e.rf(0x1f0+ch*4)), bits(e.rf(0x1f8+ch*4))]
    try:
        actual = list(map(float, next(native).split()))
    except StopIteration:
        raise SystemExit('Native trace ended before the reference run; regenerate it with passing tests.')
    recent.append((expected, actual))
    if len(recent) > 5:
        recent.pop(0)
    if actual != expected:
        previous_frequency = struct.unpack('<f', struct.pack('<I', int(recent[-2][0][-3])))[0]
        delta = F(e.rf(0x54) - previous_frequency)
        print('Target bits', bits(e.rf(0x54)), 'previous', previous_frequency,
              'fused', bits(F(previous_frequency + float(delta)*F(.001))),
              'separate', bits(F(previous_frequency + F(delta*F(.001)))))
        print('Fields:', 'channel frame', *FIELDS, 'frequency_bits previous_bits prior_bits')
        for ref, got in recent:
            print('original', ref)
            print('native  ', got)
        raise SystemExit('First divergence')


e.u.hook_add(UC_HOOK_CODE, check, begin=0x080051be, end=0x080051be)
smooth_ms = F(1000*2400/96028)
phase = 0
for n, blocks, marker in ((2400, 32, .71), (4800, 256, .75)):
    for b in range(blocks):
        target = F(96028/n)
        period_us = int(1e6/target)
        period_ms = F(period_us*F(.001))
        pending = False
        for _ in range(96):
            smooth_ms = F(F(smooth_ms*F(.999869167804718)) + period_ms*F(.00013083219528198242))
            guard = abs(F(period_ms-smooth_ms)) > 5
            phase += target/96028
            if phase >= 1:
                phase -= 1
                if not guard:
                    pending = True
        e.wb(0x110, int(guard))
        e.wf(0x54, F(1/F(period_us*F(.000001))))
        if pending:
            e.wb(0x119, 1)
            e.wi(0x120, 0)
        e.process(n=96, kind='constant', value=marker if (b//4)%2 == 0 else -marker)
print('Matched 55,296 channel-frame states, frequencies and raw samples through frozen Time expansion.')
