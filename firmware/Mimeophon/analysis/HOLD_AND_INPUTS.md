# MP86 Hold windows and raw control acquisition

Continuation, 2026-10-04. Same MP86 image/hash as `MODULATION_SCHEDULER.md`.
Reference helpers are in `reconstruction/hold_control_components.hpp`.
This pass completes the local Hold transport/retarget stages and traces raw
Hold input and clock qualification. Pin addresses below are firmware facts;
panel labels, jack circuitry, polarity at the physical connector and voltage
thresholds are not established by these register reads alone.

## 1. Hold transport and entry/exit requests

At `0x080241a4..0x080241c8`, the cached Hold snapshot in `[sp+16]` chooses
between normal cursor motion and Hold offset motion. The auxiliary latch is
`0x20004dcc`; pair offsets/statuses use the A/B mapping in
`EVENT_TRANSITIONS.md`.

```text
if cached_hold != 0:
    offset_A += block_drift
    offset_B += block_drift
    if hold_latch == 0:
        offset_A = offset_B = 0
        status_A = status_B = 1
        hold_latch = 1
else:
    cursor = (cursor - 1) & main_ring_mask
    if hold_latch == 1:
        hold_latch = 0
        status_A = status_B = 1
```

Out-of-line evidence: `0x080253fa..0x0802543a` and
`0x08025d84..0x08025d90`. Entry resets the offsets after adding drift, so the
first held frame starts at zero. Exit advances the ordinary cursor before
requesting both head changes. Both request stores can overwrite a fading
status; there is no idle guard here. Retained recording values and final
ring stores still follow the behavior documented in `RACK_RECONSTRUCTION.md`.

The drift is the live-in register s24, prepared **once before the sample
loop**. For binary control flags: normal mode has drift 0, Hold without Flip
has drift -1, Hold with Flip has drift +1. Evidence:
`0x08023e70..0x08023e8a`, `0x08025f1c..0x08025f36`,
`0x0802635c..0x08026366`, `0x080263aa..0x080263dc`.
This differs from the cached Hold snapshot, which is refreshed late in each
frame. The helper accepts drift separately rather than recomputing it from a
newly sampled Hold/Flip pair on every call.

## 2. Retargeting wraps an offset between two endpoints

Later, a fresh read of `0x20004f94` at `0x08024276..0x0802427c` enters the
retarget stage only when it is exactly 1. The bounded stage is
`0x080259ea..0x08025a9a` with branches at `0x08025e74..0x08025ea6`,
`0x08025ebc..0x08025ed8`, `0x08025ee4`, and `0x08025f18`.

Its inputs are the already calculated/clamped delay targets and:

| Input | Source |
|---|---|
| Anchor | float at `0x200038a0` |
| Window control | s11, loaded from `0x20001b58` at `0x08024052` |
| Zone scale | s5, loaded from `0x20002ac0` at `0x080241cc` |
| Current integer token | `0x20003890` |

The shared margin is `(window_control * zone_scale) * 8`, with two distinct
float32 multiplies. For each pair, A then B:

```text
if status != 2:
    high = (anchor + current_delay_target) + margin
    low = margin + anchor
    if stored_offset > high:
        stored_offset = low
        status = 1
    else if stored_offset < low:
        stored_offset = high
        status = 1
```

The endpoints are inclusive: exact equality does not request. This wraps to
the opposite endpoint and discards overshoot; it is not a clamp or modulo
remainder operation. Status 1 still permits wrapping, while status 2 skips it.
Skipping retargeting **does not freeze offset motion**: the earlier transport
stage still adds drift during Hold.

After both pairs, both delay targets are overwritten with their stored
offsets, even when a pair is fading. Both saved integer tokens are copied from
the current token only if **neither pair has status 2**. If either is fading,
neither token is updated here. This cross-pair condition is easily lost in an
implementation that treats the channels as fully independent.

The following consumer at `0x08024288` can use new requests immediately in
the same frame, preserving existing gains as established in the event pass.
This ordering differs from reverse-window requests generated after that
consumer. The reference retarget helper is unconditional: its caller must
select the exact live-Hold==1 branch.

### Concrete local sequence

For anchor 260, window control .5, zone scale 1, and ordinary target lengths
A=100/B=200, margin=4:

1. Enter Hold: offsets become zero, both statuses become 1, cursor stays put.
2. Retarget: zeros lie below 264, so offsets/targets become 364 and 464.
3. Consume: select the other head in each pair, set statuses to 2, retain gains.
4. Next held frame with drift -1: offsets become 363 and 463. Retarget skips
   wrapping while fading, but copies those moving offsets to the targets.
5. Exit Hold: cursor decrements, latch clears, both statuses become 1 again.

This sequence is tested as a composition of local helpers. Rendering and its
status writes are not part of that test, so it is not a full audio trace.

## 3. Raw Hold input polling

The state-writing routine starts at `0x08026fb4`, reached through the callback
trampoline at `0x08027398` and peripheral-handler call at `0x08022ee6`.
Its Hold fragment is `0x08027004..0x08027042`, with tails at
`0x0802712a..0x0802712e` and `0x08027156..0x08027160`.

Define bit A = `0x40020410 & 512`, bit B = `0x40020810 & 2`.
The poll counter is `0x20005158`, and the output word is `0x20004f94`:

| Stable bits during this fragment | Counter operation | Hold operation |
|---|---|---|
| A high, B low | Increment | Toggle if the new count is exactly 10 |
| A low, either B | Reset to 0 | None |
| A high, B high | Retain count | Toggle if the retained count is exactly 10 |

Toggle means 0 becomes 1, 1 becomes 0, and other output words remain unchanged.
There is no counter saturation in this fragment. Thus continued A-high/B-low
polls pass through 10 once, but A-high/B-high can repeatedly toggle a retained
count of 10. This surprising behavior is present in the instructions and
tested; no corrective debounce rule has been substituted.

The CPU performs multiple register reads within this fragment. Tests supply
stable register snapshots; asynchronous pin changes between those reads are
not modeled. The callback cadence has not been resolved to milliseconds, so
10 polls must not silently be interpreted as ten audio samples or ten ms.
The exported vector table identifies TIM2 at `0x08028230`, which supplies
handle `0x20005610` to `0x08022dc4`; its update-event path reaches the callback
above. Initialization `0x08026d98..0x08026dbc` sets peripheral base
`0x40000000`, handle +4 to 0 and handle +12 to 4499. The called setup routine
copies those fields to peripheral offsets +40 and +44 at
`0x08022d1e..0x08022d20`, i.e. `0x40000028 = 0` and
`0x4000002c = 4499`. The remaining task is to connect these timer settings to
the configured clock tree, rather than infer a poll rate from audio cadence.

## 4. Flip writer and secondary gesture boundary

Static tracing of the next fragment (`0x08027042..0x08027074`,
`0x08027130..0x08027154`, `0x08027332..0x08027334`) shows that bit B also
controls a second counter, `0x20004fb8`:

- B high increments it. Above 16000, if A is also high, the counter clears
  and a mode word at `0x20004f8c` increments, with values above 3 wrapping to 0.
- B low with a signed count greater than 10 clears the counter. If
  `0x20005100` is zero, the Flip word `0x200037f4` changes from zero to 1,
  or from any nonzero value to zero.
- If `0x20005100` is nonzero, that release instead branches to the
  persistence-related path at `0x08027262`, including flash-controller work.
  This is outside the bounded input tests and reference helpers.
- B low with count <=10 does not clear that counter in this fragment.

The earlier part of the routine compares the current mode with a saved mode,
updates it through `0x08026d3c`, and sets `0x20005100` when they differ.
Do not model the physical interaction as two independent immediate boolean
switches without accounting for this shared-input and mode-save behavior.
This pass establishes the writer paths, not the full user gesture/state
machine or physical gate/button correspondence.

## 5. Raw clock qualification uses the audio-block timebase

`0x080239ca..0x080239d6` increments the uint32 call counter at `0x20005878`
once per four-frame DSP call. The clock prefix at
`0x08023af8..0x08023b20` reads bit `0x40021810 & 1024`, qualification count
`0x20004c5c`, and high count `0x200056c4`:

```text
if raw_bit_high and qualification == 5:
    high_count += 1
    if high_count == 1:
        elapsed = now - last_accepted_timestamp  # uint32 subtraction
        if signed(elapsed) > 240: enter accepted handler
        else: enter short-interval continuation
else:
    qualification += 1
    if signed(qualification) > 5: qualification = 5
    high_count = 0
```

The first-high branch is `0x08025f9a..0x08025fb2`. Last timestamp is
`0x20002b48`. Threshold 240 is in DSP calls: nominally 20 ms at 48 kHz,
with acceptance requiring strictly greater than 240. Qualification can grow
on high samples too; it is not simply a five-low-sample edge detector.

The bounded reference returns the exact exit class: ordinary (`0x08023b20`),
short interval (`0x08025fb6`), or accepted-handler entry (`0x080264e8`). It
does not mark clock mode itself. Static inspection of the accepted handler
shows timestamp update, qualification reset, comparison with the previous
period using a +/- (previous_period >> 7) tolerance, possible pair requests,
counter alignment, and a write of 1 to clock mode at `0x0802663a`.
Those downstream operations are not yet included in a single validated helper.

The ordinary continuation checks time since the last timestamp against 48000
calls (nominally four seconds) at `0x08023b20..0x08023b46`. It sets
`0x20003898` when overdue; a later manual-control-change branch can clear clock
mode via `0x08026194..0x080266de`. A timeout should therefore not be simplified
to unconditional immediate clock-mode exit without tracing that dependency.

## 6. Validation and next integration boundary

The native runner adds `tests/test_hold_control.cpp` and four byte-checked
instruction slices: Hold transport, Hold retargeting, raw Hold polling and
the raw clock prefix. Cases cover all request/fade combinations, both sides
of each float boundary, exact equality, cursor/counter wrap, unrelated GPIO
bits and nonbinary flags. Whole memory maps are compared; retarget status
registers and clock exit addresses are checked too. Previous tests also run
after the translator's N/Z handling for shifts/logical operations is updated.

These checks use finite float inputs, representable coordinates, stable GPIO
snapshots and host arithmetic. They are not Cortex-M7/hardware execution.
Actual counts and hashes are in `continuation_validation.json` and the
continuation manifest. No plugin audio engine or physical input mapping is
claimed complete.

Next work: join accepted-clock acquisition with target-period selection;
finish the secondary input/mode-save logic and establish callback cadence;
then integrate rendering/status writes with the already recovered event
stages over complete frames. Keep those boundaries distinct from the local
Hold windows and raw Hold polling now covered by differential tests.
