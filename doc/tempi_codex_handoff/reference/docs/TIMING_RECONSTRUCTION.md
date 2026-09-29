# Timing investigation — 28 September 2026

This follow-up narrows the Leading Tempo, ADC tempo-control, and timing-commit
gaps using the recovered image already in this package. No new external source
was needed. Manual-derived behavior remains represented in the main behavioral
specification; the manual PDF itself is not bundled.

Reproduce with `python3 tools/validate_timing.py` from `firmware/Tempi`.
Readable models are in `tools/timing_models.py`; the generated report is
`analysis/timing_validation.json`. These are instruction/model differential
tests, not independent hardware measurements or a complete board simulation.

## 1. Leading interval capture and consumption

ISR `0x088C–0x08C4` detects an RA4 low-to-high transition. It captures:

| RAM | Value |
|---|---|
| `0x586`, 4 bytes | elapsed ticks from `0x446` |
| `0x442`, 4 bytes | master countdown at the edge |
| `0x480`, 1 byte | master wave level at the edge |

It then zeros elapsed and sets measurement-enable `0x481` to 1. The ISR
increments elapsed by that enable byte at `0x113A–0x1146`. With measurement
initially disabled and elapsed zero, the first edge captures zero and starts
measurement. The next edge supplies an interval. This is not an average of a
window of external intervals.

Foreground `0x4CDE–0x4DA4` consumes a nonzero captured interval exactly once:

- Clear the capture mailbox before interpreting the interval.
- Reject a signed-negative interval or a positive interval below **399** ticks;
  clear measurement-enable and elapsed. This rejection branch does not itself
  clear the previously-accepted flag at `0x5A5`.
- Otherwise set `0x5A5 = 1` and compute
  `Hnew = clamp(trunc(interval / 2), 200, 0xFFFFFF)`.
- Zero is an empty mailbox, not a rejection event.

The differential model covers nonnegative signed-32-bit intervals; the negative
rejection branch is visible at `0x4D04–0x4D32` but is not included in its model
domain. Human Programming also writes capture-related fields, so these rules
describe this mailbox consumer, not exclusively physical jack events.

## 2. Clock-loss thresholds

For nonnegative elapsed counts and a valid requested half-period H,
`0x4C18–0x4CDC` clears measurement-enable, elapsed, and `0x5A5` when:

```text
elapsed >= 0xFFFFFF
OR
(previously accepted an interval AND elapsed > 15 * H)
```

The comparison against `15 * H` is strict: `H=8000, elapsed=120000` is retained;
`elapsed=120001` clears measurement. This is **7.5 requested full periods**,
not 15 full periods. Before any interval has been accepted, only the absolute
ceiling applies. Timeout occurs on the foreground service call that observes
the condition; it is not guaranteed to occur on the exact threshold tick.

With Tap enabled and no other pending candidate, loss of input leaves the
requested period unchanged: the master continues at the last requested rate.
Measurement is restarted by a later edge; a further edge supplies a fresh
interval. Do not model this as stopping all outputs or immediately restoring
the EEPROM tempo. Source selection beyond these paths still needs review.

## 3. Period replacement versus phase correction

At `0x4DA4–0x4E16`, a changed Hnew is assigned directly to requested H and to
the current reload value. There is no moving-average period filter on this
path. If the live master countdown exceeds Hnew, it is shortened to Hnew;
otherwise it is retained. Global timing-dirty is set.

The following correction at `0x4E32–0x4F58` changes the **current reload value**,
not requested H or the live countdown. Let R be the captured edge countdown
and L the captured wave level:

```text
forward = Hnew - R
while forward > floor(Hnew/2): forward = trunc(forward/2)
backward = R
while backward > floor(Hnew/2): backward = trunc(backward/2)

currentReload = L ? Hnew + forward : Hnew - backward
```

Arithmetic is signed 32-bit with stored results wrapping to 32 bits. In
particular, a negative `forward` is not made positive or clamped. This matters
for abrupt accelerations; substituting an absolute phase error changes the
recovered rule. The captured remainder is cleared after this path.

When Hnew equals requested H, `0x4DC4–0x4DC6` skips period replacement **and
phase correction**, clearing only the captured remainder at `0x4F5A`. Thus an
equal measured period is not a command to forcibly realign the master.

Example, old H=8000, live countdown=4000, captured R=4000:

| Captured interval | New requested H | Reload if L=0 | Reload if L=1 |
|---:|---:|---:|---:|
| 16000 | 8000 | unchanged | unchanged |
| 16002 | 8001 | 4001 | 10001 |
| 32000 | 16000 | 12000 | 22000 |

## 4. ADC tempo-control function

`0x627C` is now reduced for a supplied, ascending 16-entry calibration array
T and the recovered period table V. Actual per-unit thresholds, analog
voltage scaling, and seconds per timer tick remain unresolved.

The helper first reads the ADC wrapper and uses **previous accepted ADC**, P,
at `0x44A` to calculate a movement deadband:

```text
d = min(10, floor(P/50) + 2)    # for 10-bit ADC inputs
```

An invalid sample `0xFFFF`, or a sample inside the inclusive interval
`[max(0,P-d), P+d]`, returns `2 * requestedH` and retains P. A sample outside
that interval becomes the new P and sets the movement flag `0x5AD`.

For a moved sample x, return the following **candidate full-period value**:

```text
x <= T[0] + d:  2 * V[0]
x > T[15]:     V[15]
x == T[j]:     V[j]                 # after the low-end branch
otherwise, T[j] < x < T[j+1]:
    slope = trunc((V[j] - V[j+1]) / (T[j+1] - T[j]))
    result = V[j+1] + slope * (T[j+1] - x)
```

The low-end doubling is present in the executed code; do not silently replace
it with V[0]. Also, the slope is truncated **before** multiplication. Generic
floating-point interpolation with rounding at the end differs from firmware.

The caller clears the candidate's low bit at `0x4B5C–0x4B6A`, divides by two,
and applies the half-period clamp. For example, the fastest table candidate
250 yields H=200 after clamping, not 125.

Synthetic thresholds `T[j]=32+64*j`, P=0, current H=8000:

| ADC | Returned candidate | Caller half-period |
|---:|---:|---:|
| 34 | 400000 | 200000 |
| 35 | 199032 | 99516 |
| 64 | 189984 | 94992 |
| 96 | 180000 | 90000 |
| 992 | 250 | 200 |
| 993 | 250 | 200 |

In `0x4B16`, a changed control candidate is admitted only when Tap is disabled
and measurement-enable is zero. A nonzero accepted capture later in the same
service overrides the control candidate. The helper itself still runs while
measurement is enabled, so its ADC-memory update is separate from admission
of the candidate. The tests cover these three combinations; they do not claim
all Follow-mode or Human Programming source arbitration is solved.

## 5. ISR timer reload order

The master and all six channel countdown fragments execute:

```text
remaining -= 1
if signed(remaining) <= 0:
    level ^= 1
    remaining = currentReload
    currentReload = nextReload
```

For the master, nextReload is requested H. For a channel it is the `next`
field in its 12-byte timer record. Thus changing only nextReload is not the
same as immediately replacing the running countdown. At expiry the countdown
first receives the **old current** value. The new value is promoted afterward.
Foreground code can also modify these fields; this rule alone does not specify
the final edge schedule after an edit.

## 6. Timing-commit numerical guards

The fragment `0x2540–0x26A4` in `0x234A` skips the current lane's later commit
work (branches to `0x32E8`) if:

1. Signed master remaining < **76** ticks.
2. Signed channel remaining < **76** ticks.
3. Channel remaining is in the inclusive interval
   **[channel next half-period - 75, channel next half-period + 75]**.

The tests exercise all six lane addresses and both boundaries of these guards
with channel next half-period 800. For example, remaining 725 and 875 defer;
724 and 876 pass that guard if the other conditions pass.

These are necessary safety gates, not a complete commit predicate. Earlier
UI/history flags may already skip a lane, and later ratio/phase/history logic
still chooses the transition. Do not replace the full scheduler with these
guards or convert 75/76 ticks into milliseconds without a measured timebase.

## Still open

- Complete `0x234A` history selection and phase/ratio transition equations.
- Human tap quantization and its interaction with the capture mailbox.
- Full Follow/source-priority transitions and interrupt interleavings.
- Actual ADC calibration and physical timer rate.
- Exact UI and MOD transition tables, mutation boundary normalization/seeding.
