# UNDERTOW Polyphony — Brief Implementation Contract

**Status:** Implemented and covered by `tests/undertow_module_spec.cpp`
**Module:** `LEVIATHAN // UNDERTOW`  
**Scope:** Basic Rack polyphony for the existing oscillator and outputs

## Goal

UNDERTOW shall produce one independent oscillator voice per active polyphonic
input channel. The largest channel count presented by any input determines the
channel count of every audio output.

## Locked behavior

### Channel count

For every call to `process()`:

```text
voiceCount = clamp(max(1, channel count of every input), 1, PORT_MAX_CHANNELS)
```

All six existing inputs participate:

- `V/OCT`
- `EXPO`
- `LIN FM`
- `SHAPE CV`
- `SYNC`
- `S-GATE`

With no inputs connected, UNDERTOW remains a one-channel oscillator. `SINE`,
`SHAPE`, and `SUB` must always expose the same `voiceCount`.

### Input broadcasting

Each input is read with Rack's normal polyphonic broadcasting behavior:

- A polyphonic input supplies its corresponding channel.
- A monophonic input is broadcast to every active voice.
- If a polyphonic input has fewer channels than `voiceCount`, channels beyond
  that input's channel count supply `0 V`. They do not repeat, wrap, or hold its
  last available channel.
- An unconnected pitch, modulation, shape, or sync input supplies `0 V`.
- An unconnected `S-GATE` supplies its existing normal value of `10 V` to every
  voice.

The `10 V` normal applies only when the entire `S-GATE` port is unconnected. If
`S-GATE` is patched polyphonically with fewer channels than `voiceCount`, its
missing channels receive `0 V`, like every other patched polyphonic input.

Implementation must use semantics equivalent to Rack's
`getNormalPolyVoltage(normalVoltage, channel)`. No channel wrapping other than
Rack's standard mono broadcast is introduced.

### Voice independence

Replace the single `VoiceState` with a fixed-capacity collection containing one
state per possible Rack channel. Every voice owns independent:

- oscillator phase;
- sub-oscillator flip and gate-history state;
- LIN FM high-pass history;
- analog-character envelope;
- SYNC and S-GATE Schmitt triggers;
- SINE, SHAPE, and SUB MinBLEP generators.

SYNC, S-GATE, LIN FM history, phase resets, and MinBLEP corrections on one
channel must never alter another channel.

A monophonic SYNC voltage broadcasts to every active voice, but each voice
detects the edge and calculates its resulting MinBLEP correction using only its
own trigger, phase, waveform, and MinBLEP state. Therefore the voices reset on
the same sample while their corrections may differ according to their state
immediately before the edge.

This refactor preserves the existing hard-sync phase reset, MinBLEP correction,
and event-timing behavior. It does not add a crossfade, blend window, slew, or
other SYNC smoothing, and it does not revise the existing sub-sample timing
estimate. Any such change requires separate characterization because it would
alter the released monophonic sound.

No allocation, locking, or container resizing may occur in `process()`.

### Voice lifecycle

The module tracks the previous active voice count. It begins at `1`, matching
UNDERTOW's existing monophonic startup. Channel 0 must preserve the current
monophonic startup and continuous-running behavior and must never be reset
merely because another channel appears or disappears.

When the active channel count expands, each newly active voice is reset to a
deterministic default state before processing its first sample:

- oscillator phase `0`;
- sub flip low/false and prior S-GATE-high history false;
- LIN FM high-pass history `0`;
- analog-character envelope `0`;
- fresh/reset SYNC and S-GATE Schmitt triggers;
- empty/reset SINE, SHAPE, and SUB MinBLEP generators.

The newly active voice then processes the current input voltages normally on
that same sample. Consequently, an already-high SYNC or S-GATE voltage is seen
by its fresh Schmitt trigger as a rising edge, matching a newly constructed
monophonic voice.

When the count contracts, inactive voices are not processed and produce no
output channels. Their stored state is irrelevant. Reactivating a previously
removed channel resets it as above rather than resuming frozen oscillator
history. A direct expansion across multiple channels resets every newly active
channel in the half-open range `[previousVoiceCount, voiceCount)`.

### Parameters, display, and lights

Panel parameters remain global and apply equally to every voice.

- The frequency display remains behaviorally unchanged and reports channel 0's
  pitch-derived frequency at the same point in the signal path as today (after
  coarse, fine, V/OCT, and EXPO, but before LIN FM).
- The shape preview and its published shape amount remain behaviorally
  unchanged. They inspect channel 0 only, including channel 0's `SHAPE CV`.
  Polyphonic channels must not be averaged, combined, or allowed to alter the
  preview.
- The SYNC light reacts when any active voice receives a sync edge.
- The S-GATE light is illuminated when the port is patched and any active
  S-GATE channel is at least `1 V`. Missing channels on a shorter patched
  polyphonic S-GATE are low. An unpatched normalized S-GATE does not illuminate
  the light.
- The coarse-step light remains global.

Light smoothing and brightness behavior remain otherwise unchanged. UI
telemetry and light aggregation may scan active voices but must not scan
inactive channels.

Channel 0 is the only voice that publishes frequency-display or shape-preview
state. Higher-numbered voices perform only the audio and event work required to
produce their three output channels and contribute boolean SYNC/S-GATE light
aggregation. They must not perform duplicate preview tracing, preview-cache
work, display publication, or other channel-0 UI work.

## Implementation shape

The intended implementation is a small refactor of `Undertow::process()`:

1. Compute global parameter-derived values once per sample outside the voice
   loop.
2. Determine `voiceCount` from the maximum input channel count.
3. Reset any newly activated voice states.
4. Process active voices independently with `getPolyVoltage(channel)` semantics.
5. Write all three outputs by channel and set their channel counts once.
6. Publish channel-0 display values and aggregated light states once.

`getShapeAmount()` must become channel-aware or its calculation must move into
the voice loop. The value published to the existing preview must be precisely
the channel-0 result. Expensive work that is identical for all voices must
remain outside that loop.

Output channel counts must be set once per output per `process()` call, outside
the voice loop. Stale voltages in storage above the published output channel
count are unobservable and need not be cleared.

The scalar voice loop is the required first implementation. In particular:

- store voice state contiguously in a fixed-capacity container such as
  `std::array<VoiceState, PORT_MAX_CHANNELS>`;
- read global parameters and atomic context-menu options once per `process()`
  call, not once per voice;
- cache each input's connection state and channel count outside the voice loop;
- compute sample-rate-dependent coefficients, including the LIN FM high-pass
  coefficient, once per `process()` call and reuse them for every voice;
- process exactly `[0, voiceCount)` and do not clear, update, or inspect inactive
  voice DSP state;
- accumulate `anySyncRising` and `anyPatchedSGateHigh` as booleans during the
  active voice loop, then update each light once;
- preserve the stable module-level Debug Terminal `Process` metric as the total
  cost of all active voices rather than publishing per-voice macro metrics.

SIMD is not required for this work. The independent trigger, wrap, gate,
sub-flip, and MinBLEP branches make full-loop SIMD a poor initial fit. A later
optimization may vectorize a measured hotspot without changing this contract,
provided scalar-equivalent behavior and voice independence are retained.

## Compatibility

UNDERTOW is released. This work must not reorder, remove, or insert parameter,
input, output, or light IDs. It must not change patch serialization or context
menu settings.

Existing monophonic patches must retain their current tuning, waveform,
MinBLEP, sync, sub-gate, analog-character, and output-level behavior.
This includes the frequency display, shape preview, light behavior, and all
existing context-menu combinations. Polyphony must not change channel 0 merely
because higher-numbered voices are active.

## Required tests

A focused module contract must verify:

1. The largest channel count on any of the six inputs determines all three
   output channel counts.
2. With no connected inputs, all outputs remain monophonic.
3. Every one of the six inputs can independently establish `voiceCount`.
4. Monophonic modulation, SYNC, and S-GATE inputs broadcast to all active
   voices.
5. A shorter polyphonic input supplies `0 V` on its missing channels and never
   wraps its values. A shorter patched S-GATE is specifically verified to be
   low on missing channels rather than using its unpatched `10 V` normal.
6. Polyphonic V/OCT produces independently pitched output channels.
7. Per-channel SHAPE CV changes only its addressed SHAPE output, while the
   published preview value continues to follow channel 0 only.
8. Per-channel SYNC affects only the addressed voice, and the SYNC light
   aggregates edges across active voices.
9. Per-channel S-GATE affects only the addressed SUB output, uses `>= 1 V` as
   its light threshold, and the S-GATE light aggregates patched active voices.
10. LIN FM filter history, analog-character envelope, Schmitt trigger state,
    sub flip/history, and all three MinBLEP states do not leak between voices.
11. Expansion initializes new voices to the specified default state. A high
    SYNC or S-GATE on the first active sample produces the specified fresh-
    trigger behavior.
12. Channel contraction and later expansion reset reactivated voices rather
    than restoring frozen state, without resetting channel 0.
13. During randomized 1–16 channel input, every output sample is finite, and
    SHAPE and SUB remain within their existing `[-5 V, +5 V]` clamps. Do not add
    a new SINE output clamp as part of this work.
14. The pre-refactor monophonic capture in `tests/undertow_module_spec.cpp`
    remains unchanged. It covers free running, modulation, SYNC, S-GATE,
    analog character, both coarse modes, display publication, preview
    publication, and light behavior using tolerant trace summaries plus
    full-trace fingerprints quantized to `0.1 mV`.
15. The deterministic hard-sync stress sweep remains finite and below its
    `15.5 V` corruption guard. The captured pre-refactor peak is approximately
    `12.3484 V` across 44.1, 48, 96, and 192 kHz; 10, 261.63, 997, and 10000 Hz;
    analog character enabled and disabled; and repeated SYNC edges spanning
    oscillator phases. The guard is a test limit with approximately 25% margin,
    not an output clamp or a promised nominal signal level.
16. The preview remains channel-0-only when higher channels are changed.

The focused module contract must be registered in the Makefile's routine
`test-fast` suite. Validation must include that focused test, the complete
routine `test-fast` suite, and an authoritative native Windows `plugin.dll`
build.

## Non-goals

This first implementation does not add:

- voice allocation, note stealing, or gate-to-pitch pairing;
- unison, detune, stereo spreading, or per-voice panel controls;
- polyphonic parameter modulation beyond the existing inputs;
- SIMD-specific oscillator code or hybrid SIMD/scalar event handling;
- any panel, port, enum, or serialization changes.
