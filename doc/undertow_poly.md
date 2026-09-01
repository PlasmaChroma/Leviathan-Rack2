# UNDERTOW Polyphony — Brief Implementation Contract

**Status:** Proposed implementation contract  
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
- An unconnected pitch, modulation, shape, or sync input supplies `0 V`.
- An unconnected `S-GATE` supplies its existing normal value of `10 V` to every
  voice.

No wrapping between channels other than Rack's standard mono broadcast is
introduced.

### Voice independence

Replace the single `VoiceState` with a fixed-capacity collection containing one
state per possible Rack channel. Every voice owns independent:

- oscillator phase;
- sub-oscillator phase/flip state;
- LIN FM high-pass history;
- analog-character envelope;
- SYNC and S-GATE Schmitt triggers;
- SINE, SHAPE, and SUB MinBLEP generators.

SYNC, S-GATE, LIN FM history, phase resets, and MinBLEP corrections on one
channel must never alter another channel.

No allocation, locking, or container resizing may occur in `process()`.

### Voice lifecycle

Channel 0 must preserve the current monophonic startup and continuous-running
behavior.

When the active channel count expands, each newly active voice is reset to a
deterministic default state before its first sample. When the count contracts,
inactive voices produce no output. Reactivating a previously removed channel is
treated as a newly active voice rather than resuming frozen oscillator history.

### Parameters, display, and lights

Panel parameters remain global and apply equally to every voice.

- The frequency display reports channel 0.
- The shape preview reports channel 0.
- The SYNC light reacts when any active voice receives a sync edge.
- The S-GATE light is illuminated when any patched active S-GATE channel is
  high.
- The coarse-step light remains global.

UI telemetry must not require scanning inactive channels.

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
the voice loop. Expensive work that is identical for all voices must remain
outside that loop.

## Compatibility

UNDERTOW is released. This work must not reorder, remove, or insert parameter,
input, output, or light IDs. It must not change patch serialization or context
menu settings.

Existing monophonic patches must retain their current tuning, waveform,
MinBLEP, sync, sub-gate, analog-character, and output-level behavior.

## Required tests

A focused module contract must verify:

1. The largest channel count on any of the six inputs determines all three
   output channel counts.
2. With no connected inputs, all outputs remain monophonic.
3. Monophonic modulation, SYNC, and S-GATE inputs broadcast to polyphonic
   voices.
4. Polyphonic V/OCT produces independently pitched output channels.
5. Per-channel SYNC affects only the addressed voice.
6. Per-channel S-GATE affects only the addressed SUB output.
7. LIN FM filter history and MinBLEP state do not leak between voices.
8. Channel contraction and later expansion reset reactivated voices safely.
9. All outputs remain finite and bounded under randomized 1–16 channel input.
10. The existing monophonic signal path remains behaviorally unchanged.

Validation must include the focused test, routine `test-fast`, and an
authoritative native Windows `plugin.dll` build.

## Non-goals

This first implementation does not add:

- voice allocation, note stealing, or gate-to-pitch pairing;
- unison, detune, stereo spreading, or per-voice panel controls;
- SIMD-specific oscillator code;
- polyphonic parameter modulation beyond the existing inputs;
- any panel, port, enum, or serialization changes.

