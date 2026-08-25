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
5. Request raw spectrum only when exact corrective frequencies matter.

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
