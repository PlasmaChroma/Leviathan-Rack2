# MP86 Halo modulation and delay-expiry scheduling

Continuation of `COLOR_HALO_ROUTING.md`, 2026-10-04. Evidence is the same
49,152-byte MP86 image, SHA-256
`31323684dacda3d6313565a1a47596005870d1defed55e0336fd419ca82040b9`.
Names A/B below refer to the 969/803-sample allpasses, not physical channels.
The executable reference is `reconstruction/modulation_components.hpp`.

## 1. Recovered state and initialization

| State | Address | Initial value |
|---|---|---|
| Shared uint32 random state | `0x20002acc` | 12345 |
| Random scaling float | `0x2000387c` | `0x2f800000`, exactly 2^-32 |
| A fractional position | `0x20001b2c` | `0x3dcccccd`, float32(.1) |
| A increment per stereo frame | `0x20004c6c` | `0x35d8172e`, float32(1.61e-6) |
| B fractional position | `0x20002b58` | float32(.1) |
| B increment per stereo frame | `0x20003854` | `0x35a66e13`, float32(1.24e-6) |
| A signed delay counter | `0x20003868` | 0 |
| B signed delay counter | `0x20002aac` | 0 |

The first six rows are direct stores at `0x0802385c..0x0802388a` in
`initialization.asm`, now executed in a byte-checked test slice. Counter zeros
are visible in the earlier initialization around `0x08023328..0x08023334`.
Do not seed the two walkers independently or randomize the seed on each load
while claiming this startup behavior.

## 2. Counter decrements and expiry

Every stereo frame, `0x08023f64..0x08023fa4` performs:

```text
A -= 1
if ratio_at_20003870 == float32(1): B = A
else: B -= 1
if signed(A) < 0: handle A expiry
if signed(B) < 0: handle B expiry
```

The equal-ratio branch is at `0x080253bc`. It copies the decremented A before
A's expiry/refill; B does not copy the newly refilled counter. Equality is
exact float equality. Both expiries can therefore run in the same frame, A
first. The test includes adjacent float values on either side of 1.

A expiry (`0x08025b2e..0x08025b5e`) refills A with signed truncation of
`float_at_20002b44 + float_at_2000388c`; B expiry
(`0x08025a9e..0x08025acc`) similarly uses `float_at_20001ad0 +
float_at_2000388c`. These are float32 additions before integer conversion.
A also writes 720 to `0x20004e80`, B to `0x200055d4`; these stores alone do not
establish a physical pulse width or output mapping.

With no intervening counter writes or synchronization effects, a refill to
nonnegative integer N expires after **N+1** later decrements. Zero does not
expire until it becomes negative. Control paths outside this sample prefix
also write the counters (for example `0x08026734..0x08026746`), so the interval
is not a universal free-running oscillator period.

Expiry then runs mode-dependent transition logic before always reaching that
walker's random update on the inspected expiry paths. Relevant branches are
`0x08025b18`, `0x08025fca`, `0x080263e0`, `0x080263fc`,
`0x080266e2`, `0x080266f4`, and `0x080267ae`. The random helpers intentionally
accept explicit expiry triggers; they do not simulate this event arbitration.
The finite reload helper also excludes out-of-range/NaN float-to-int behavior.

## 3. One random draw changes velocity, not position

A random update starts at `0x08025b5e`, B at `0x08025acc`. Each does:

```text
random = (random * 196314165 + 907633515) modulo 2^32
centered = fma(float32_unsigned(random), float32(2^-32), float32(-.5))
candidate = fma(centered, float32(2e-6), old_increment)
```

The unsigned-to-float conversion happens before scaling and can round large
integers to 2^32; do not replace it with a double-precision division or a
signed conversion. The candidate is replaced only when strictly outside
float32(+/-3e-6):

| Walker | Candidate > +3e-6 | Candidate < -3e-6 |
|---|---|---|
| A | `0x35ae7ba9`, +1.3e-6 | `0xb5a10fb0`, -1.2e-6 |
| B | `0x361a59b3`, +2.3e-6 | `0xb60cedba`, -2.1e-6 |

Otherwise the candidate is kept. This is an asymmetric reset into the range,
not clipping to +/-3e-6. Negative-limit branches are at `0x08025dde` and
`0x08025df6`; in-range stores are at `0x08026862/0x0802686a`.
When both expire, the shared generator advances twice, assigning the first
draw to A and the next to B. An independent PRNG for each allpass changes the
trajectory even with the same starting seed.

## 4. Position integration and boundary turns

Later in the same stereo frame, `0x08024e2c..0x08024f10` advances A then B:

```text
position = increment + position
if position > float32(.35) or position < float32(.05):
    boundary = the crossed limit
    increment = -increment
    position = fma(increment, float32(2), boundary)
```

Exact equality with a limit does not turn. On crossing, the overshoot is
discarded; the reset is neither a clamp nor a geometric reflection of the
actual overshoot. The position then drives that same frame's short-allpass
read in the already traced Halo tail. Its coordinate is
`cursor + position * buffer_length` before wrapping and linear interpolation.

The fractional ranges correspond to approximately 48.45..339.15 samples for
A and 40.15..281.05 for B. With the short-buffer cursor moving forward, these
are forward address offsets; the age of an already written sample is roughly
`length - offset`, not `offset`. Do not relabel these fractions as ordinary
backward delay times without preserving the ring convention.

## 5. Consequences for a Rack reconstruction

- Preserve two timescales: velocity perturbations follow delay-counter
  expiries, while positions integrate every stereo frame. Rate-dependent
  changes to the expiry schedule change the random trajectory.
- Preserve shared draw ordering, startup values, asymmetric resets and the
  exact unity-ratio branch. A generic sine LFO or per-sample white noise
  removes recovered behavior.
- Keep the modulation active along this audited sample path even if a UI
  control makes Halo inaudible. No Halo-amount gate appears in these slices;
  a full engine still needs to establish every surrounding path.
- Prefer the proposed internal 48 kHz engine for initial equivalence work.
  At a different native rate, both position increments and the delay/event
  schedule need adaptation. Rescaling an increment alone is insufficient.
- The helpers allocate nothing and use uint32 arithmetic. Explicit `std::fma`
  preserves the audited arithmetic; benchmark its production implementation
  together with the existing performance requirements.

## 6. Validation and remaining boundary

`tools/test_rack_components.py` now includes `tests/test_modulation.cpp`.
Five additional byte-checked slices cover initialization, both random
updates, both position integrators, and the counter-decrement prefix. The
translator keeps floating compare state separate from integer compare state:
the firmware's `VCMP; SUBS; VMRS; BEQ` sequence must branch on the saved
floating comparison after VMRS restores it.

Tests check exact state bits, positive/negative reset branches, adjacent
boundary floats, long accumulated trajectories and uint32 counter wrap.
The long trajectories supply explicit synthetic expiry schedules; they are
not a simulated full-device run. Counter refill interval tests are analytic
helper tests, not instruction execution of the entire expiry handler.
Actual test counts and compiler information are recorded in
`continuation_validation.json`, with slice extents in `color_trace_audit.json`.

These are host-executed translations of selected decoded instructions, not a
Cortex-M7 emulator or hardware reference. Exceptional floats, full CPU flags,
external counter writers, physical control mapping and startup of the complete
audio path remain outside their scope. The subsequent
[event pass](EVENT_TRANSITIONS.md) joins these random updates with the complete
sample expiry prefix, including request priority and busy-status handling,
then traces request consumption and reverse windows. The later
[Hold/input pass](HOLD_AND_INPUTS.md) closes Hold retargeting and validates raw
Hold polling/clock qualification. Accepted-clock processing, physical-input
correspondence and full-frame integration remain open.
