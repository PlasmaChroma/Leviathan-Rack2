# Sil Repair Micropeak Notes

## Purpose

This file records recent empirical findings from Sil Repair micropeak debugging. It is intentionally narrower than `Repair_Spec.md`: these are observations from captured CSV logs and the threshold implications they suggest.

The current detector should continue to be treated as a conservative sample-defect detector, not as a broad transient enhancer or AI-artifact classifier.

## Current Debug Capture Format

Sil's DragonKing debug CSV capture logs rows under the Rack user folder:

```text
Leviathan/Sil/micropeak_debug_<timestamp>.csv
```

Rows are tagged by `event_type`:

```text
repair     -> at least one channel passed the micropeak candidate detector
near_miss  -> locally prominent peak was worth logging but did not pass repair gates
```

The CSV includes per-channel measurements:

- `peak`
- `near`
- `guard`
- `local_mean`
- `neighbor_drop`
- `isolation`
- `neighbor_ratio`
- `neighbor_share`
- detector gate flags: `pass_peak`, `pass_drop`, `pass_ratio`, `pass_share`, `pass_isolation`
- `local_prominent`

The GUI debug readout currently shows:

```text
L: <left repair count> R: <right repair count> NM: <near-miss count>
```

As currently implemented, the near-miss counter only increments while CSV logging is active. That is acceptable for now because near-miss accounting is a debug facility, not shipped user-facing metering.

## Current Detector Shape

The live repair detector is intentionally narrow. A channel must satisfy all of these gates:

```text
peak >= 0.40 full-scale       -> 2.0 V when full scale is 5.0 V
peak - near >= 0.060 full-scale -> 0.3 V
peak >= 1.35 * near
near and guard <= 0.70 * peak
peak / local_mean >= 2.6
```

Repair replaces only the delayed center sample with a linear interpolation between its immediate neighbors:

```text
center = 0.5 * (prev1 + next1)
```

This means the intended target is a high-energy, locally isolated, needle-like sample defect. Broad transients, dense distortion, and clustered musical edges should usually fail.

## Capture: Autechre `t1a1`, Repair-Only CSV

File observed:

```text
micropeak_debug_1778207057.csv
```

Summary:

- `278` repair events.
- First hit at `13.419s`; last at `596.772s`.
- Sample rate was `48 kHz`.
- Repair lookahead was `48 samples`, matching the intended `1 ms` window.
- Channel split:
  - both channels: `136`
  - left only: `75`
  - right only: `67`

Candidate-channel medians:

```text
Left  peak ~= 2.61 V, drop ~= 1.35 V, isolation ~= 3.16, depth ~= 0.63
Right peak ~= 2.57 V, drop ~= 1.37 V, isolation ~= 3.12, depth ~= 0.64
```

Interpretation:

`t1a1` can produce genuine detector-positive micropeak events. These are not barely passing: the typical hit is above the 2 V peak threshold and has substantial immediate-neighbor drop. Some events cluster into short microbursts, so the detector is catching regions of intentionally sharp digital/percussive activity as well as isolated one-off samples.

This does not automatically mean every detected event is an unwanted defect. It means this material contains shapes that match the current defect model.

## Capture: AI Track Normal Playback, No Scratch

File observed:

```text
micropeak_debug_1778210197.csv
```

Summary:

- `8161` rows.
- All rows were `near_miss`.
- `0` repair events.
- Duration was about `37.97s`.
- Candidate combos were all `(0, 0)`.

Median near-miss profile:

```text
Left  peak ~= 1.19 V, drop ~= 0.206 V, isolation ~= 1.69, ratio ~= 1.185, share ~= 0.847
Right peak ~= 1.19 V, drop ~= 0.204 V, isolation ~= 1.67, ratio ~= 1.180, share ~= 0.852
```

Rows above the 2 V peak threshold:

```text
Left  peak >= 2.0 V: 141 rows
Right peak >= 2.0 V: 134 rows
```

Even those rows failed `ratio`, `share`, and `isolation`; most also failed `drop`.

Interpretation:

The AI track produced many locally prominent peaks, but they were mostly low-amplitude and broad relative to their neighboring samples. The detector was not missing obvious repair candidates; it was rejecting shapes that do not match an isolated single-sample defect.

Lowering the main repair threshold globally would likely make this track generate many repair candidates, but those candidates would mostly be broad musical/transient structures rather than clear sample defects.

## Capture: AI Track Plus Temporal Deck Clean Scratch

File observed:

```text
micropeak_debug_1778208893.csv
```

Summary:

- `104` repair events.
- First hit at `281.157s`.
- Last hit at `294.468s`.
- All detections occurred in about `13.31s` after scratching began.
- The preceding normal AI-track playback produced no repair events.

Candidate-channel medians during Clean scratch:

```text
Left  peak ~= 2.13 V, drop ~= 1.20 V, isolation ~= 3.65, depth ~= 0.65
Right peak ~= 2.19 V, drop ~= 1.25 V, isolation ~= 3.95, depth ~= 0.68
```

Interpretation:

Temporal Deck scratching on the Clean setting can synthesize lower-amplitude but highly isolated micropeak-like events. These are quieter than many `t1a1` hits, but more mathematically isolated. The current detector naturally catches them.

This is an important contrast: the same AI source that produces no repair events during normal playback can become detector-positive when interactive scratch motion creates sharper discontinuities.

## Capture: Autechre `t1a1`, Near-Miss CSV

File observed:

```text
t1a1-nearmiss.csv
```

Summary:

- `1400` rows.
- `1387` near misses.
- `13` repair events.
- Duration was about `30.42s`.

Autechre near-miss medians:

```text
Left  peak ~= 1.63 V, drop ~= 0.296 V, isolation ~= 1.31, ratio ~= 1.22, share ~= 0.94
Right peak ~= 1.64 V, drop ~= 0.302 V, isolation ~= 1.31, ratio ~= 1.22, share ~= 0.94
```

Hot near misses:

```text
Left  peak >= 2.0 V: 460 rows
Right peak >= 2.0 V: 478 rows
Left/right peak >= 3.0 V: 127 rows each
```

Most hot near misses failed `share` and `isolation`, meaning the surrounding samples were also large.

Some Autechre near misses were one gate away from repair, commonly failing only `isolation`. These are hotter than the AI near misses and closer to the current repair boundary, but they still look too broad or too embedded in surrounding transient energy to repair safely.

## Cross-Capture Interpretation

Recent captures separate into three useful classes:

```text
1. True repair hits
   High peak, high immediate-neighbor drop, strong isolation.

2. Hot broad near misses
   High peak, but nearby samples are also large, so share/isolation fail.
   `t1a1` produces many of these.

3. Low-level shaped near misses
   Locally prominent, sometimes cleanly shaped, but below the absolute peak floor.
   The AI track produces many of these.
```

The current detector is behaving consistently across those cases.

Autechre tends to be hotter and more clustered. It crosses the repair threshold sometimes and often gets close, but many near misses are rejected because they are embedded in real transient structure.

The AI track tends to produce many smaller local prominences. Some have reasonable shape, but most are below the 2 V peak floor and/or too broad. Normal playback does not produce repair hits; scratch interaction can create detector-positive events.

## Threshold Policy Implications

Do not lower the main repair threshold globally based only on the AI near-miss file.

Reasons:

- The AI file produced thousands of near misses in under 40 seconds.
- Most were below the absolute peak floor.
- Even high AI near misses failed ratio/share/isolation.
- Lowering sensitivity would likely turn many ordinary local transients into repairs.

The current threshold is conservative, but that conservatism appears intentional and useful. Repair should continue to target high-confidence sample defects.

If a future mode wants to address the AI near-miss class, it should probably not be the current one-sample interpolation repair. Better options would be separate and softer:

- near-miss metering only,
- a very shallow gain-smoothing mode,
- high-band dynamic smoothing,
- or a separate AI-artifact smoother that does not rewrite center samples.

## Open Questions

- Should near-miss counting run whenever Repair is enabled, even when CSV logging is off?
- Should the debug CSV include a rolling event density or cluster ID to make burst analysis easier?
- Should the detector include separate labels for `hot_broad_near_miss` and `low_peak_near_miss`?
- Should scratch-generated micropeaks be considered defects to repair or artifacts of intentional interactive playback?
- Should repair hits in music like `t1a1` be auditioned via delta monitoring before changing thresholds?

## Working Conclusion

The current micropeak detector is not failing to see AI artifacts. It is making a narrow decision: only high-energy, highly isolated sample spikes are safe enough for one-sample repair.

The recent CSVs support keeping Repair conservative. Near misses should be used as debug evidence and possibly as input to a separate, gentler artifact-smoothing design, not as a reason to make the one-sample repair path broadly more aggressive.
