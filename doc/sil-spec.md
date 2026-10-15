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

## Low-Band Mono Recovery

Implemented in `src/sil/SilLowBandRecovery.hpp`, before Impact Air and the rest of
the mastering chain. The final stereo-linked limiter remains downstream.

### Goal and limits

Preserve coherent bass stereo and restore a centered waveform when low-band
cancellation would otherwise remove it from the mono sum. This is a phase-reference
fold, not a delay estimator or full phase alignment of independent sources.
A perfectly inverted pair has no uniquely correct original polarity: the initial
reference is Left, with hysteretic channel selection as described below.

### Crossover

- Matched fourth-order Linkwitz-Riley low-pass/high-pass at **120 Hz**.
- Each branch is two cascaded Butterworth sections, implemented with TPT
  state-variable filters for stability at high sample rates.
- Coefficients are prewarped and computed only on sample-rate changes.
- Low and high are phase matched. With recovery inactive, their sum has a flat
  magnitude response and an all-pass phase response. It is not sample-identical
  to the input and is not a linear-phase crossover.
- The high branch retains its stereo signal. The transition is gradual: at full
  recovery, a 60 Hz Side tone is about 24.6 dB down; at 120 Hz it is 6 dB down.
  Frequencies just below 120 Hz are not completely mono.

### Detection and smoothing

Track low-band L/R energy and cross energy with a 100 ms exponential average.
Evaluate correlation and reference-gain targets every 16 samples. Audio filters,
energy followers, and controller smoothing run every sample.

```text
correlation >= 0.70 -> no recovery
correlation <= 0.00 -> full recovery
otherwise           -> recovery = 1 - correlation / 0.70
```

Attack into recovery is 50 ms; release is 250 ms. Silence below `1e-10 V²`
combined low-band energy releases recovery. A smooth confidence gate suppresses
activation from negligible treble leakage: the low/full-band energy ratio fades
from inactive at -40 dB to fully active at -30 dB.

### Recovering the waveform

Simply removing Side cannot recover canceled Mid. Instead:

1. Keep a stable channel reference, initially Left. Change to the other channel
   only when its smoothed low-band energy is more than four times the current
   channel's (about 6 dB). This avoids chattering on equal-level stereo bass.
2. When changing channels under negative correlation, invert the incoming
   reference polarity to preserve the existing waveform's orientation.
3. Smooth the reference weights and gain over 50 ms. Normalize the reference
   toward the per-channel average low-band energy, with a maximum gain of
   `sqrt(2)` to avoid amplifying a vanishing reference without bound.
4. Crossfade Mid toward this reference while attenuating Side:

```text
recoveredMid  = mid + recovery * (reference - mid)
recoveredSide = side * (1 - recovery)
outL = highL + recoveredMid + recoveredSide
outR = highR + recoveredMid - recoveredSide
```

Equal-level inverted bass therefore becomes a centered low-band waveform at the
original per-channel level. Single-channel bass is centered at equal-power level.
Unrelated L/R bass is reduced to the selected reference as recovery approaches
full; this necessarily changes the low-band mix and its mono sum. Channel handoff
and rapid program changes can cause temporary level variation. This stage does
not promise recovery of every independent source in a complex canceled mixture.

Impact Air receives the **recovered** low-band Mid, so restored bass can drive its
transient detector.

### Feedback amount

The panel percentage, indicator, and history show **low-band Side power removed
as a percentage of full-band input power**, rather than the fold control. The
fold control can reach 100% for any nonpositive correlation, even when bass is
only a small part of the mix; displaying it directly looks like an on/off switch.

```text
sidePower = (LL + RR - 2*LR) / 4
removedSidePower = sidePower * (1 - sideGain*sideGain)
feedback = clamp(removedSidePower / ((inputLL + inputRR)/2), 0, 1)
```

The input and low-band energy estimates use the same 100 ms average. The meter
measures full-band input after the crossover's neutral all-pass response so a
steady bass tone does not create a phase-lag-induced ratio fluctuation. Feedback
updates every 16 samples, with 50 ms attack and 250 ms release smoothing, and
releases to zero on silence. It does not affect the audio controller.

Examples: fully corrected uncorrelated deep bass reads about 50%; inverted deep
bass approaches 100%. Inverted 40 Hz bass mixed with an equal-power coherent
3 kHz tone reads about 49%; reducing bass amplitude to one tenth reads about 1%.
The percentage is independent of overall level above the silence floor. It
measures removed stereo power, not recovered Mid power or output correlation,
and does not guarantee mono at the crossover.

### Lifecycle, bypass, and performance

- Construction/sample-rate changes configure and reset the crossover/controller.
- Warm mastering bypass keeps the recovery state running and passes delayed dry
  L/R through the established mastering crossfade. Bypass does not apply the
  crossover to the dry signal.
- No allocation, worker dependency, lookahead, or per-sample transcendental
  functions are introduced. Square roots run at the 16-sample control rate.

### Verification

`make test-sil-low-band-recovery test-sil-boost test-sil-perf` checks:

- Mono/coherent stereo magnitude and balance, 10 Hz through 10 kHz.
- Matched crossover response and cancellation recovery at 44.1/48/96/192 kHz.
- Inverted, quarter-cycle, unequal-level, and single-channel bass.
- Simultaneous canceled bass and high-frequency stereo content.
- Feedback scaling with phase, crossover frequency, bass share, and overall
  level, plus silence/reset release.
- Smoothed, polarity-preserving reference handoffs; recovery release; silence;
  reset and sample-rate reconfiguration.
- Restored mono fundamental through the complete Sil chain, warm bypass, and
  limiter containment with loud recovered bass.
- Updated full-chain regression references and idle wake equivalence.
