# Octavia Machine Listening and Monitoring

Read this reference for signal observation, snapshots, comparison, spectrum, loudness,
measurement, or Sibyl-triggered capture.

## Observation model

Octavia retains six physically cabled channels on one Rack-frame timeline:

- `masterL` and `masterR` are the persistent Master pair and drive live LUFS/dBFS meters.
- `A`, `B`, `C`, and `D` are independent mono probes. They have no inherent stereo or
  before/after pairing; each request assigns their interpretation.

Use monitor discovery to verify connection, physical channel count, sample rate, rolling
state, snapshot generation, and active analysis users. Physical cables are the sensory
boundary; Octavia cannot hear an unpatched output.

Choose the least expensive observation that answers the question:

1. Use ordinary signal levels for silence and gain staging.
2. Use a snapshot for repeatable RMS, peak, crest, DC, clipping, or spectrum analysis.
3. Capture every comparison point in one snapshot for an identical frame window.
4. Use a triggered measurement for multi-second loudness, clipping, RMS, or correlation.
5. Use an ephemeral bounded analysis capture when a longer window is useful but no
   archival file is needed.
6. Use a bounded recording, or analysis capture with `save: true`, when exact evidence
   must be retained for reproducible external analysis.
7. Request raw spectrum only when exact corrective frequencies matter.

## Snapshot and analysis workflow

```text
GET /audio/monitors
POST /audio/snapshot       {monitors, preMs, postMs, label}
GET /audio/snapshot/{id}   poll only while post-roll is pending
POST /audio/analyze        {snapshotId, channels or stereo, detail, includeSpectrum}
POST /audio/compare        {snapshotId, reference, target, detail}
```

Valid groups include mono `{"channels":["A"]}` and stereo
`{"stereo":{"left":"A","right":"B"}}`. Comparisons report absolute results and
explicit `target - reference` deltas. `levelNormalizedBandsDb` separates tonal change from
pure gain change. Stereo analysis reports balance, correlation, mid/side energy, and
side-to-mid ratio. Negative correlation describes only the selected pair.

Snapshots are immutable: repeated analysis of one ID must not recapture audio. Frame
metadata identifies what was heard. Post-roll completes outside the audio thread.
`analysis_busy` and `measurement_busy` indicate bounded work; wait or reduce the request
instead of flooding retries. The bounded snapshot pool can expire old IDs.

For before/after work, cable reference and target simultaneously—A before a filter and B
after it, for example—and compare one snapshot. For stereo, use two explicit pairs such as
stereo(A,B) and stereo(C,D). Never assume A/B are a pair from their names.

Legacy `/audio/0` and `/audio/1` remain Master L/R. Their analyzers use the frozen backend.
Legacy loudness reset → play → read arms an explicit Master measurement rather than reading
an unbounded always-running accumulator.

## Bounded analysis and recording workflow

```text
POST /audio/capture         {channels or stereo, seconds, detail, includeSpectrum, save, label}
POST /audio/capture         {reference, target, seconds, detail, includeSpectrum, save, label}
GET /audio/capture/{id}     poll until complete or failed
POST /audio/recording       {monitors, seconds, label}
GET /audio/recording/{id}   poll until complete or failed
```

`seconds` is required and bounded to 0.1–30.0. Analysis captures are ephemeral by default:
the audio thread fills a preallocated buffer, a dedicated worker analyzes it, and the raw
samples are released without creating files. Set `save: true` only when the identical
analyzed frames should also be archived. Omitted `monitors` are derived from the requested
analysis groups. Explicit monitors must include every analyzed channel.

The recording route remains explicitly archival. Omitted recording monitors default to
Master L/R; otherwise select only the physical Master/A-D inputs needed for the experiment.
One bounded capture can be active at a time. Saved captures export an interleaved IEEE
float32 WAV and JSON sidecar under the Rack user directory at
`Leviathan/Octavia/Recordings`.

WAV samples are raw Rack volts, not normalized audio. The sidecar preserves sample rate,
exact first and last Rack engine frames, channel order, and connection masks. A sample-rate
change during capture fails the recording rather than producing mislabeled data. Use the
returned paths for offline spectrum, harmonic, alias, and transition analysis; do not infer
access to any signal that was not physically cabled to a selected Octavia monitor.

### Frame-synchronized control outputs

Octavia exposes two independent 16-channel polyphonic outputs, `Control A` and
`Control B`. They are diagnostic instruments, not general-purpose sequencers: use them
only for a temporary, bounded sanity check or measurement whose stimulus must align
exactly with an Octavia capture. Defer authored musical sequences, clocks, arrangements,
and persistent automation to Sibyl, or preserve the user's explicitly chosen sequencer.

A bounded recording or analysis capture may include a `control` program:

```json
{
  "monitors": ["A", "B", "C", "D"],
  "seconds": 6,
  "label": "four-way-response",
  "control": {
    "settleMs": 100,
    "static": {"A": [5, 8, 9, 10]},
    "events": [
      {"port": "B", "channel": 0, "offsetMs": 1000,
       "durationMs": 1, "voltage": 10}
    ]
  }
}
```

Static voltages are asserted on the audio thread before capture. `settleMs` is the
bounded pre-capture interval during which downstream modules can stabilize. Event offsets
are relative to the exact first recorded Rack frame and are converted to integer frames
when the request is armed. Values are raw Rack volts and must remain within +/-10 V.

Use a polyphonic splitter when different Control channels must reach separate monophonic
inputs. A common trigger may be fanned physically from one Control channel. The response
and saved sidecar report the control-start frame, capture-start frame, requested event
offsets, executed event frames, static channel vectors, and Control-output connection
masks. A scheduled output is still meaningful only when physically cabled into the patch.
Monitor discovery includes a `controls` array with each output's live connection state and
active polyphonic channel count; verify it before arming a stimulus-driven capture.
Control outputs return to their idle state when the bounded session ends. Treat their
cables and any helper modules as a disposable test harness: do not save them into the
patch, and do not remove pre-existing modules or cables without the user's authorization.

## Sibyl-triggered observation

A Sibyl event can request a capture at its exact sounding onset:

```json
"observation": {
  "octaviaModuleId": 1234,
  "monitors": ["A", "B"],
  "preFrames": 4800,
  "postFrames": 12000,
  "label": "filter transition"
}
```

Resolve the Octavia ID from the live patch. The marker publishes only when the
probabilistic event plays, at its swing/microshift-adjusted onset, and once per onset rather
than per ratchet. Its trigger remains the authored event frame even if an envelope, effect,
or buffered processor responds later; choose post-roll to include that consequence.
Request-to-snapshot mappings appear at `/audio/triggered-snapshots`.

## Panel indication

A-D LEDs show machine attention, not amplitude: off is disconnected, dim is rolling,
a flash marks capture, and bright/pulsing marks detailed analysis. Master meters remain
continuous and separate.
