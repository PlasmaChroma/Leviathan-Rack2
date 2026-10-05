# External-clock estimator

Complete original `Channel::tempoClock` now matches an independent persistent
estimator over 216 sequences, 64,800 pulses and 389,232 comparisons. Coverage
is 248 original instruction addresses. Tests include all factory clock settings
(Average=4, Resolution=64, Timeout=0/3), alternative settings, warm-up, jitter,
timeout boundaries, zero/negative intervals and deque node rotation.

This closes estimator arithmetic and selected initialization contracts. Clock
jack/button producers, linked event ordering, asynchronous lost-clock behavior
and connection to audible transport remain open.

## Provenance and reproduction

`probe_external_clock.py` uses the pinned original main ELF through `ARMBytes`.
The complete function at 0x3eed8 executes with its original deque allocation,
append/pop and sum operations. Only verified PLT boundaries are supplied:

| Address | Service | Explicit fixture behavior |
|---|---|---|
| 0x15918 | `system_clock::now` | Synthetic signed 64-bit nanosecond timestamp |
| 0x15eac | `__aeabi_l2f` | Host conversion of supplied signed int64 to binary32 |

This uses **system clock**, whereas retrigger cooldown uses steady clock. No
clock, driver, appliance process or script is launched. Timestamp values are
within the exact integer range of host doubles; this is not a claim about every
possible int64 conversion or the original runtime library's implementation.

The deque header is an initialized fixture, not a reconstructed complete
Channel object. Original constructor timestamp slice 0x3d844–0x3d860 executes,
ending before 0x3d864. Original preset-window resize 0x3e428–0x3e44c also executes,
ending before 0x3e450. The reference independently carries its interval window
and expected speed state; it does not use executed state to compute expected
outputs. Each 300-pulse sequence crosses 128-float deque-node boundaries.

```sh
python firmware/Lubadh/probes/probe_external_clock.py
```

Results and representative warm-up/timeout traces are in
`probes/external_clock_probe_results.json`.

## Timestamp, timeout and first interval

The timestamp at Channel+464/+468 is initialized to constructor system time.
`tempoClock` obtains a new timestamp and computes:

```
interval = f32(f32(nowNanoseconds - previousNanoseconds) / f32(1e9))
previousNanoseconds = nowNanoseconds
```

It always updates the timestamp before timeout rejection. If Channel+472 is
enabled and the rounded interval is strictly greater than the signed integer
Timeout at Preset+36, it returns without changing window, target, increment or
remaining slew count. Equality to the timeout is accepted. The static preset
publication at 0x3de48–0x3de64 enables this flag when Timeout is nonzero.
That publication graph is not executed in this probe.

A rejected long interval is not accumulated with the next interval: the next
pulse starts from the rejected pulse's timestamp. This consumer does not stop
transport by itself. No special first-pulse branch exists inside `tempoClock`:
the first interval is elapsed time since the initialized timestamp, and it is
accepted or rejected by the same rule. The physical first-clock producer may
change other state; it remains to be checked.

Zero and negative supplied intervals are accepted when they do not exceed the
timeout. These fixtures expose system-time/discontinuity behavior, not a claim
that a hardware clock normally emits those values. Invalid Average values and
NaN intervals are outside this probe.

## Window initialization and averaging

Preset resize uses `Average` entries when Resolution>0, otherwise one entry.
Freshly grown entries are zero. On every accepted pulse, append the interval,
remove the oldest entry and sum the surviving entries in chronological order
with binary32 addition. The window retains its fixed length as deque nodes
rotate and are allocated/released. Resizing an existing nonempty window may
preserve some history; complete preset-change histories remain untested.

Define N=Preset.Average, R=Preset.Resolution, C=current clock division,
L=Link+40 logical period, and S=the binary32 interval sum. The checked law is:

```
durationSamples = f32(f32(S / f32(N)) * f32(49170.25390625))
selectedDivision = R if R != 0 else C
numerator = f32(f32(L) / f32(selectedDivision)) if selectedDivision != 0 else f32(L)
magnitude = clamp(f32(numerator / durationSamples), f32(0.01), 4)
```

The literal 49170.25390625 is the recurring DSP timebase; it does not establish
the hardware clock. Averaging starts with zeros and always divides by N. With
a fresh N-entry window, a constant first accepted interval therefore contributes
only 1/N of the steady duration, before speed clipping.

Resolution zero deliberately changes the stored window to one interval, but
the denominator still divides that interval by **Average**. For Average>1,
this multiplies the calculated speed relative to a one-interval average. The
probe independently verifies this behavior; do not replace N with window length
when reproducing literal vectors. A corrected native law must be an explicit
adaptation, with separate tests.

The arithmetic fixture supplies logical period 100,000 samples, Division 0/3,
Average 1/4/16 and Resolution 0/4/64. Positive, zero and negative interval
fixtures exercise both speed-clamp endpoints. The tests do not prove every
possible period, division or floating-point edge.

## Direction and slew publication

For modes other than V/oct, negate the clamped magnitude if the latest raw pot
value is below 2048. V/oct instead negates when Channel+236 is nonzero. This
matches the earlier tap-speed consumer's nonzero direction test, while the
pot-speed consumer tests direction equality to one.

Publish the signed target at Channel+44, then set increment/count using the
recovered truncated `SpeedSlewTime / f32(2.7) + 1` law. Current slew state at
Channel+728 is retained. This estimator does not advance playback speed:
the later `processSpeed` callback consumes the ramp and adds flutter/touch.
Timeout-rejected pulses preserve the existing ramp setup.

## Native specification requirements and remaining work

The specification can now define and test clock warm-up, FIFO averaging,
timeout equality/recovery, resolution/division selection, direction and slew.
Keep legacy behaviors separate from deliberate improvements such as monotonic
event timing, averaging only measured intervals or corrected Resolution-zero
averaging. A native module also needs explicit event ordering and a bounded
allocation-free averaging buffer; reproducing deque allocation in Rack audio
processing is unnecessary.

Still required: RecordJack clock-role edge production, reset/first-edge flag
graphs, preset-change history, true missing-clock behavior between pulses,
linked asymmetric clock events, tap-tempo gesture interaction and clock-driven
motion/render integration. The passing estimator is not evidence that those
whole-instrument contracts are complete.
