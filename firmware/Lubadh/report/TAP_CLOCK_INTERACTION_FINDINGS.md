# Tap tempo, reset and shared clock state

Complete original tap, clock and reset-event consumers match an independent
persistent model over 216 sequences, 3,240 calls and 23,352 comparisons. A
separate 120-case callback slice checks tap-span publication and the complete
clock-division-limit consumer. Combined coverage is 248 original instruction
addresses. Physical gesture producers, linked forwarding and audible motion
scheduling remain open.

## Provenance and reproduction

`probes/probe_tap_clock_interaction.py` uses the original main ELF with SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
It executes complete `tempoTap` at 0x37120, `tempoClock` at 0x3eed8 and
`interpretButtonPress` at 0x3f1c4 with event 9. System time and signed-int64
conversion use the explicit hooks described in
`EXTERNAL_CLOCK_ESTIMATOR_FINDINGS.md`. No appliance program is launched.

```sh
python firmware/Lubadh/probes/probe_tap_clock_interaction.py
```

Results and representative traces are in
`probes/tap_clock_interaction_probe_results.json`. The reference carries its
own timestamp, first-tap gate, speed/slew state and four-entry clock window.
Expected outputs are not read from executed state. The integer reset deque is
an initialized one-node fixture; this does not check multi-node reset freeing.

Sequences cover all four speed modes, direction values 0/1/2, both initial
first-tap gates, spans 1/1,000/100,000 and slew settings 0/2.9/25. Each sequence
interleaves 15 tap/clock/reset calls, including zero/negative intervals, a
timeout-rejected clock and a 15-second tap interval. Current slewed speed is
supplied as 0.75 throughout; processing between calls is not simulated.

## Shared timestamp and reset

Tap and external clock use the same signed nanosecond timestamp at
Channel+464/+468. Each non-reset call replaces it with the current supplied
system time. Consequently a tap affects the next clock interval and a clock
affects the next tap interval. Even a timeout-rejected clock advances it.

Channel+480 gates the first tap. When nonzero, a tap primes the shared timestamp,
clears the gate and returns without changing speed, slew or clock history.
Clock calls leave this gate unchanged. Therefore a clock between reset and the
first tap does not make that tap calculate speed.

Event 9 clears the integer deque beginning at Channel+484 and sets the gate to
1. It leaves the shared timestamp, float clock-history deque at Channel+524,
target and slew state unchanged in these fixtures. This is a consumer event
number, not a recovered physical gesture name. Static branches in
`Application::interpretButtons` can dispatch event 9 locally or to both linked
decks; the complete producer and its chord priority have not been executed.

## Measured tap speed

For a tap with the first-tap gate clear, let T be the signed tap span at
Channel+172032+0x4f4 and F(x) denote binary32 rounding. The checked arithmetic is:

```
interval = F(F(nowNanoseconds - previousNanoseconds) / F(1e9))
duration = F(interval * F(49170.25390625))
magnitude = clamp(F(F(T) / duration), F(0.01), 4)
```

There is no separate timeout branch in this complete tap consumer. A long
interval still calculates and clamps speed. Positive supplied spans with a
zero interval produce the upper clamp; negative intervals produce the lower
clamp. These discontinuity fixtures describe arithmetic, not normal hardware
gestures. Zero/negative spans and NaNs are outside the sequence matrix.

Mode 3 uses nonzero Channel+236 for reverse; other modes use latest raw speed
pot below 2048. It applies that sign to magnitude, then publishes target and
the existing slew law described in `SPEED_TABLE_AND_SLEW_FINDINGS.md`. Current
speed remains unchanged until the processing consumer advances the slew.

## Tap-span and division-limit publication

The original callback slice at 0x48c80 runs through span publication and the
complete `checkClkDivs` call at 0x3724c, stopping before 0x48cb0. Fixtures supply
S24 as raw integer region-length bits, current division and linked live speed.
Static upstream code at 0x47cb4 obtains these region bits from Link+92; for the
unlinked pointer Link=Channel+0x24, that aliases Channel+128. The full upstream
callback path is not executed here.

Let L be supplied region length, C current division at work+0x4d8, and V linked
live speed. The checked span publication is:

```
T = trunc(F(F(L) / F(C))) if C != 0 else trunc(F(L))
work[0x4f4] = T
```

`checkClkDivs` then calculates a floating ratio with a binary32 numerator and
binary64 denominator, using the literal binary64 2.7:

```
numerator = F(F(T / 2) * F(1000))
ratio = double(numerator) / (abs(double(F(V))) * 2.7 * 49170.25390625)
limit = 5 if ratio >= 5 else trunc(ratio) if ratio > 2 else 2
work[0x4d4] = limit
```

Zero speed yields an infinite positive ratio for the positive-span fixtures
and publishes limit 5. The 120 cases cover regions 128/1,000/49,170/100,000/
16,777,219, divisions 0/1/3/64 and speeds 0/-0.125/0.125/-1/1/4. The large
region checks binary32 integer rounding before division. Division limit,
current division, preset clock resolution and tap span are separate fields;
do not merge their contracts.

## Specification consequence

A literal compatibility mode must preserve shared tap/clock timing, priming
after reset, clock-history retention and the distinct span/division rules.
A native improvement may give tap and clock separate monotonic timestamps or
explicit reset histories, but that changes observable state transitions and
needs a documented mode and acceptance vectors. These tests close the consumer
interaction; they do not yet establish the user-reachable gesture or jack path.
