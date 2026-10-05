# Retrigger events, cooldown, delay queue and V/oct direction

Original `interpretButtonPress` events 2 and 8, complete `doRetrig` with nested
tap activation/fade operations, and the delayed-queue maintenance slice now
execute in offline probes. `probe_retrigger_events.py` passes 4,870 checks:
1,120 fresh-pool retrigger configurations, 32 persistent event-countdown
sequences, six delayed-queue sequences and 72 direction events. Coverage is
491 distinct original instruction addresses. This establishes these consumers,
not upstream physical gestures or complete callback audio.

## Harness scope

The harness uses the pinned `lubadh_main` ELF and restricted `ARMBytes` services.
The verified PLT symbol at 0x16074 is `std::chrono::_V2::steady_clock::now`.
Only that boundary is supplied synthetic signed 64-bit nanosecond timestamps;
no wall clock, appliance program, driver or script runs. Original manager
constructors, activation, kill and fade routines execute, rather than being
replaced with host behavior. The fresh-pool matrix independently predicts
timestamp, activation arguments and kill arguments. Existing allocation tests
cover pool saturation; that state is not part of this matrix.

```sh
python firmware/Lubadh/probes/probe_retrigger_events.py
```

The exact results are in `probes/retrigger_event_probe_results.json`.

## Pending-event suppression and delayed insertion

Event 2 in `Channel::interpretButtonPress` (0x3f1c4, branch 0x3f3a8):

1. If Channel+172032+0x328 is nonzero, return without decrementing anything.
2. If Channel+0x110 is nonzero, decrement it and return. An initial value of
   one therefore suppresses this event; the **next** eligible event can retrigger.
3. Read `RetrigDelay` from the linked preset at byte 56.
4. If zero, invoke original `doRetrig` immediately.
5. Otherwise append that integer to Channel's delay vector at +0x2cc/+0x2d0/+0x2d4.

The complete event runs with reserved vector capacity, so insertion is checked
without allocator-growth behavior. The event is not a periodic decrement of
the pending word: only an eligible event 2 consumes it. Other event producers
may also write the word, as shown in the preceding pot-speed findings.

## Delayed-queue countdown

The checked slice is `AudioEngine::process` 0x493d0 through the two-deck
traversal, stopping before 0x493f0. Every queued integer is decremented once per
invocation. If its decremented value equals zero, call `doRetrig`; afterward,
remove zero entries while preserving the order of surviving entries. Multiple
expiring entries produce multiple calls; cooldown may suppress later attempts.
The probe executes both deck traversals with the second queue empty and an
inert state object, checking complete first-queue countdown and compaction.

Queue insertion and maintenance are separate operations. Static callback
dataflow places `runIO` before deck audio processing and queue maintenance
after both decks and the display update. Thus an entry inserted during a
callback participates in that callback's final countdown. Audible delayed
activation applies to subsequent deck processing. The full callback and rendered
activation time have not been executed as one connected fixture.

Supplied zero/negative queue entries decrement away from zero and survive;
these are branch fixtures, not a claim that production presets create them.
Queue size is currently dynamically allocated in the appliance. A Rack design
should define a bounded event queue, overflow behavior and deterministic event
ordering rather than reproducing callback allocations.

### Documented milliseconds versus executed count

Factory preset comments and the editor label `RetrigDelay` as milliseconds;
the sequencing and Clockable presets specify 25. However, this consumer appends
the preset integer directly and the queue decrements it once per callback,
without a milliseconds conversion. The static successful parser path in
`preset_load` reads an integer at 0x17534 and stores it directly at Preset+56
at 0x17538. The isolated parser-to-shared-preset publication graph has not yet
been executed, so retain that integration check before claiming whole-firmware
timing. Do not silently equate the checked counter with milliseconds.

Given a supplied counter of 25 and fixed 128-frame callbacks at nominal
48 kHz, expiration takes 25 processed blocks, approximately 66.67 ms. This is
a conditional timing calculation, not a device measurement. The launch script
requests that JACK period/rate, while actual codec clock remains unresolved.
The native specification should choose an explicit legacy-count policy or a
deliberate millisecond-corrected improvement and provide separate vectors.

## Complete retrigger eligibility and activation

`doRetrig` at 0x380a0 compares elapsed steady-clock nanoseconds with the signed
literal `0x01312cff` = 19,999,999. It returns when elapsed is at most that value.
Exactly 20,000,000 ns is eligible. Negative elapsed values are also rejected.

For an eligible time interval, it first stores the new timestamp at
Channel+568/+572. It then returns if playback tracking mode Link+148 is 1
and `abs(Link.speed) < f32(0.1)`. Equality to that threshold is allowed.
Consequently an eligible but speed-blocked attempt consumes cooldown. So does
an eligible attempt in a state object whose label does not permit activation.

Activation is allowed only when the current state object's label at +4 is 2
or 3. Its direction is forward for speed>=0, reverse for speed<0; forward
position comes from Link+20 and reverse from Link+32. Duration is Link+100.
These are supplied initialized state fields; upstream state-label producers
remain unexecuted.

| Configuration | Checked operation |
|---|---|
| Eligible label 2/3, RetrigMode Link+144=0 | Kill playback manager, then activate its new head |
| Eligible label 2/3, RetrigMode=1 | Activate playback without first killing it |
| Playback tracking Link+148=1 | Set the new playback head's held step from Link+8 |
| Label 2, variable record Link+160=0, RetrigMode=0 | Also kill and activate the record manager |
| Other combinations | No additional record-manager activation |

The playback manager is Channel+872; record manager is Channel+3688. Both
actual constructors initialize the fresh pools. Native activation/fade routines
execute all the way to return. The tests compare activation/kill arguments,
rather than claiming the entire audible fade trajectory is checked here.
That trajectory has separate transport/render evidence.

## V/oct reverse event

Event 8 reaches branch 0x3f4e0. It does nothing for modes 0/1/2. For V/oct:

- Direction 0 becomes 1; direction 1 becomes 0. Supplied value 2 is retained.
- Negate the existing **target** speed, rather than rereading/negating the pot
  candidate. Current slew state is retained.
- Recalculate the increment and count with the same `SpeedSlewTime / 2.7`
  law recovered earlier, and publish indicator words 185/30 at Channel+656.

This checks the full event branch, including repeated numerical sign inputs.
It does not identify which physical gesture produces event 8, or its linked
forwarding/chord priority. Keep candidate, direction, target and current speed
separate in the native control model.

## Callback cadence and remaining work

Static evidence now ties both `runIO` and delayed queue maintenance to each
normal `AudioEngine::process` invocation. `processChannel` calls `processSpeed`
once for each deck. `AudioEngine::set_buffer_size` at 0x36610 publishes the
requested frame count and forwards it to both Channels; cadence is therefore
a callback contract, not a user-interface frame-rate contract. The existing
launch script requests 128 frames at 48 kHz. A future full callback fixture
must still check simultaneous event/audio ordering and adaptation to other
frame counts. Do not infer the actual device clock from literal DSP constants.

Remaining work includes event-2 and event-8 gesture producers, numeric control
state/deferred gates, clock averaging/timeout and simultaneous-edge priority,
linked asymmetric event consumption, connected recording-boundary/tail writes,
speed modulation and delayed activation rendered against persistent tape.
These findings move those contracts toward a complete specification; they do
not yet close the whole-instrument parity audit.
