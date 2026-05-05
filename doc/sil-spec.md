# Sil Engineering Spec

## Current State

Sil is currently implemented as a stereo mastering shell with realtime visual feedback and a fixed output safety limiter.

Implemented:

- Stereo L/R inputs and L/R outputs.
- SVG component anchors for jack placement.
- Histogram display from recent output samples.
- Dual FFT spectrum displays for left and right output.
- Context menu color schemes for visualizers.
- Fixed stereo-linked output limiter.
- Limiter ceiling: `-1.0 dBFS`.
- Yellow limiter activity LED driven by limiter gain reduction.

Current design direction:

- Realtime operation.
- Zero added latency by default.
- Slow adaptive decisions may use rolling history.
- Peak safety remains handled by the final limiter.

## Product Goal

Sil should behave like a conservative automatic mastering module for VCV Rack.

The module should improve output reliability and translation without requiring the user to tune a full mastering chain manually. It should favor transparent, low-risk processing over aggressive loudness maximization.

Primary defaults:

- Streaming-safe ceiling.
- Mono-compatible bass.
- Stable stereo image.
- Low CPU cost suitable for realtime Rack use.
- No latency unless a future optional mode explicitly adds it.

## Analysis Model

Sil can keep a rolling analysis history, but this history is not lookahead.

Good uses for a rolling 10-second baseline:

- RMS and loudness trend.
- Crest factor.
- Low-band correlation.
- Low-band side energy.
- Average limiter gain reduction.
- Spectral tilt.
- Slow adaptive threshold or amount decisions.

Bad uses:

- Catching future transients.
- Replacing limiter attack.
- True-peak prediction.
- Fast EQ movement.

Rule:

Historical analysis may steer slow controls. It must not be required for instantaneous safety.

## Next Feature: Low-Band Mono Recovery

### Goal

Restore bass punch and mono compatibility when left and right low-frequency content is weakly correlated or out of phase.

This should be more adaptive than a static "Bass Mono" switch, but simpler and safer than full phase alignment.

### Concept

Process only the low band below `120 Hz`.

Use mid/side control:

```text
mid  = 0.5 * (lowL + lowR)
side = 0.5 * (lowL - lowR)
```

If low-band correlation is healthy, preserve the low-band side signal.

If correlation becomes poor, reduce low-band side gain smoothly.

Then reconstruct:

```text
lowL = mid + side * sideGain
lowR = mid - side * sideGain
```

High-frequency content should pass through unchanged.

### Signal Flow

```text
input L/R
-> split low/high around 120 Hz
-> low-band correlation + side-energy analysis
-> adaptive low-band side attenuation
-> recombine low/high
-> final -1.0 dBFS limiter
-> output L/R
```

### Crossover

Initial implementation should use a low-risk IIR crossover.

Recommended first version:

- 2-pole low-pass per channel for low-band extraction.
- High band derived as `input - low`.
- Cutoff: `120 Hz`.
- Coefficients update on sample-rate change.

This is not a perfect Linkwitz-Riley crossover, but it is simple and zero-latency. If the behavior is useful, later versions can upgrade to a 4th-order Linkwitz-Riley split.

### Correlation Analysis

Track low-band correlation using smoothed energy terms:

```text
sumLL += lowL * lowL
sumRR += lowR * lowR
sumLR += lowL * lowR
correlation = sumLR / sqrt(sumLL * sumRR + epsilon)
```

Use one-pole smoothing rather than block processing for the first implementation.

Suggested time constant:

- Fast enough to react to bass phase problems.
- Slow enough to avoid modulation artifacts.
- Start around `100 ms`.

### Side Gain Mapping

Map correlation to a low-band side gain target.

Initial conservative mapping:

```text
correlation >= 0.70 -> sideGain = 1.0
correlation <= 0.00 -> sideGain = 0.0
otherwise           -> linear interpolation
```

This preserves normal stereo bass when it is coherent and collapses only risky bass toward mono.

Optional refinement:

- Also reduce side when low-band side energy is much higher than mid energy.
- This catches cases where correlation is not strongly negative but bass width is still excessive.

### Smoothing

Smooth `sideGain` before applying it.

Suggested starting values:

- Attack toward mono: `50 ms`.
- Release back to stereo: `250 ms`.

Reasoning:

- Bad low-band phase should be corrected quickly.
- Returning stereo width can be slower to avoid pumping.

### UI

First version can be automatic and always enabled.

Possible later UI:

- Status LED: `BASS MONO` or `LOW SAFE`.
- Small gain-reduction style meter for low-band side attenuation.
- Context menu item to disable low-band mono recovery if needed.

Initial implementation should avoid adding panel controls unless the behavior needs user tuning.

Initial status UI requirement:

- Add a `LOW_RECOVERY_LIGHT` anchor in the SVG near the limiter status area.
- Add a short SVG label next to it, preferably `LOW SAFE`.
- LED brightness should indicate how much low-band recovery is being applied.
- Use `brightness = 1.0 - smoothedLowBandSideGain`.
- LED off means low-band stereo is being preserved.
- Partial brightness means low-band side is being reduced.
- Full brightness means the low band is effectively collapsed to mono.

### Safety

Guardrails:

- Keep high band untouched.
- Keep processing stereo-linked and deterministic.
- Avoid allocations in `process()`.
- Avoid worker thread dependencies for this feature.
- Recompute filter coefficients only on sample-rate change or cutoff changes.
- Keep final limiter after this stage.

### Verification

Basic tests by patch:

- Mono bass: output should remain essentially unchanged.
- Hard-panned high-frequency content: should remain stereo.
- Low-frequency L/R inverted sine below 120 Hz: side should reduce and mono punch should recover.
- Stereo bass above 120 Hz: should be mostly unaffected.
- Silence: no NaNs, no gain jumps.

Manual Rack checks:

- Patch a low sine into L and inverted low sine into R.
- Confirm low-band recovery audibly restores centered bass.
- Confirm limiter still catches output peaks.
- Confirm high-frequency stereo material does not collapse.
