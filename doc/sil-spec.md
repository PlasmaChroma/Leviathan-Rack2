# Sil Engineering Spec

## Current State

Sil is a bounded adaptive stereo mastering chain. The current signal path is:

```text
Input -> Low-band mono recovery -> Impact Air -> Remove Mud -> Midrange Enhance
      -> pre-glue program analysis -> Glue -> Stereo Enhance -> Saturator
      -> stereo-linked oversampled peak limiter -> Mastering crossfade -> Output
```

This file describes the current implementation. Files in `doc/Sil/` and
`doc/sil.md` also contain historical design proposals, research, and superseded
tuning. They do not establish that every proposed feature was implemented.
In particular, there is no separate Repair/micropeak stage, LUFS targeting,
genre classifier, full phase alignment, or spectral restoration engine in the
current live chain.

Stereo inputs normalize an unconnected channel to the connected one. The Boost
control adds 0–6 dB to mastered audio and the limiter detector together, after
adaptive analysis, so adaptation cannot cancel it. Mastering bypass is delayed
dry L/R with a 10 ms crossfade; the analysis chain stays warm. Displays analyze
audible output, with spectrum panes showing Mid and Side.

## Stage behavior audit (2026-10-15)

Each live stage has an isolated entry point used by both `process()` and
`tests/sil_stage_behavior_spec.cpp`. This tests behavior at each stage rather
than inferring it from the final limiter-constrained output.

| Stage | Intended action and bounds | Corrections and verification |
| --- | --- | --- |
| Impact Air | Brief bass-transient-driven high shelf above 1 kHz, up to +0.75 dB | Uses recovered low-band Mid. A bass burst opens the shelf, a sustained tone settles, and sub-floor input stays inactive. Shelf transfer gain and stereo linking checked. |
| Remove Mud | Broad low-mid decongestion, 315 Hz bell/Q 0.75, at most −2.5 dB | Corrected detector frequency units. Stereo-linked L/R analysis avoids phase cancellation. Three calibrated narrow analysis bands reject isolated tones; a level gate ignores sub-floor material. Distributed 230/320/450 Hz buildup produces about 2.07 dB of cut, while the tested single tones stay below 0.16 dB. |
| Midrange Enhance | Conditional 1.45 kHz presence lift/Q 0.72, up to +0.85 dB | Corrected body/core/presence detector bands. The recessed-mid fixture receives about +0.79 dB; a supported core or strong upper presence suppresses it. Detector remains intentionally based on post-mud mono, as specified for this stage. |
| Glue | Gentle stereo-linked RMS compression; 1.5:1 ratio, 8 dB knee, ≤3 dB reduction and ≤2 dB partial makeup | Corrected 90 Hz sidechain cutoff; independent L/R filter energy and stereo program history prevent opposing channels from vanishing. Quadratic knee now starts below threshold and joins the ratio continuously. Threshold still follows pre-glue program level. |
| Stereo Enhance | Mid-only 350 Hz bell/Q 7.3, down to −2 dB; Side-only 6 kHz bell/Q 0.71, up to +2 dB | Corrected broad reference edges and added focused 350 Hz/6 kHz detectors. Bass no longer falsely drives the 350 Hz correction. Already-bright 6 kHz Side suppresses its own boost; duller Side can receive air. M/S isolation verified. |
| Saturator | Slow percentile-driven makeup ≤3.25 dB and drive 1–1.6, with limiter backoff | Empty history requests neutral settings instead of maximum activity. The shaper's small-signal gain exceeds unity even at drive 1, so the wet amount fades in with a smoothstep over the first 0.5 dB of makeup and ramps per sample between 512-sample updates. Previously a neutral/active switch stepped quiet material by about 1.9 dB. A 1 Hz tracker removes DC from the nonlinear contribution only, preserving the original bass/DC path. Feedback releases on silence. Slow controls keep advancing during idle for consistent wake-up. |
| Limiter | Stereo-linked sample ceiling −1 dBFS, with guarded intersample estimation | The detector window now includes the sample being emitted. The held demand passes through a 1 ms moving average, so attack is a linear ramp that completes by the first affected output sample instead of a one-sample gain step. An 8-phase, 64-tap/source-frame SIMD sinc detector replaces the short low-passing FIR. A 0.5 dB reconstruction margin covers finite interpolation and gain changes in the tested fixtures. |

### Detector frequency units

`OnePoleRC` and `DetectorBank4` take bilinear radians/sample. Named detector
edges use `rcCutoffForHz(hz, sampleRate) = 2*tan(pi*hz/sampleRate)`, calculated
only when configuring filters. Passing `hz/sampleRate` directly had placed
poles roughly 6.28 times too low. Independent tone checks now verify the −3 dB
single-filter edges and −6 dB paired HP/LP edges through 12 kHz at
44.1/48/96/192 kHz.

The mud guard uses analysis centers near 216/306/433 Hz, Q 6, normalized for
noise bandwidth. Spread fades broadness between 3 and 7 dB. These are analysis
filters; the correction remains a single gentle 315 Hz bell. Focused Stereo
Enhance detectors use Q 3 at 350 Hz and Q 1.5 at 6 kHz. Their Q values need not
match the correction EQ: they distinguish target concentration from broad
reference content.

### Feedback percentages

The percentages describe different stage actions and are not interchangeable
measures of perceived loudness or the share of the mix changed.

| Readout | Meaning before display smoothing |
| --- | --- |
| Low-frequency mono | Fraction of full-band input power removed from low-band Side; detailed below |
| Impact Air | Applied shelf gain / 0.75 dB |
| Remove Mud | Applied cut magnitude / 2.5 dB |
| Midrange Enhance | Applied lift / 0.85 dB; linear, without the previous square-root display mapping |
| Glue | Actual gain reduction / 3 dB, before partial makeup |
| Stereo Enhance | Mean of normalized Mid-cut and Side-lift controls; one move at maximum reads 50%, both read 100% |
| Saturator | 55% normalized makeup plus 45% normalized drive, gated by recent audible signal; no artificial 1.45x display multiplier |
| Limiter | Fraction of output amplitude removed by the applied linked gain, including the sample guard |

### Limiter lookahead and limits

The reconstruction detector has 32 samples of group delay. Detector lookahead
is `max(34, round(sampleRate*0.0005))`. The attack ramp adds `round(sampleRate*0.001)`
samples. Audio and bypass delay is their sum, capped at 512 samples:

| Sample rate | Lookahead + ramp | Delay | Time |
| --- | --- | --- | --- |
| 44.1 kHz | 34 + 44 | 78 samples | 1.77 ms |
| 48 kHz | 34 + 48 | 82 samples | 1.71 ms |
| 96 kHz | 48 + 96 | 144 samples | 1.50 ms |
| 192 kHz | 96 + 192 | 288 samples | 1.50 ms |

The peak window holds each demand for D+1 samples, for an audio delay of D. The
applied gain is the L-sample moving average of the held, released gain. A demand
pushed at time p protects outputs from p+L through p+D. Every average ending in
that span therefore contains only held values, which keeps the ramp at or below
the demanded gain. The detector
always records history. Two block maxima and the reconstruction kernel's L1
norm allow convolution to be skipped only when every reconstructed phase is
provably below the limiting threshold; an always-convolving reference tests
that shortcut across abrupt level/Boost changes.

Independent 16x, 129-tap windowed-sinc output reconstruction checks starts,
stops, near-Nyquist tones, an impulse, bursts, noise, and multitone input at
44.1/48/96/192 kHz. The worst tested reconstructed output peaks range from
−1.15 to −1.35 dBFS. The sample guard remains at −1 dBFS. This is a finite,
conservative intersample estimator, not a certified true-peak or codec guarantee.
Mastering bypass remains dry and does not promise the mastered ceiling.

The saturator operates at base sample rate and can alias; it is not an
oversampled analog model. The spectral stages are bounded heuristics, not
semantic judgments about which instruments or harmonics a mix should retain.
The program reference is an audible-sample stereo RMS estimate, not LUFS.
Listening checks with representative music remain useful alongside these
repeatable signal tests.

Local release-flag throughput checks on a deterministic bass/treble signal
measured about 110 ns/sample for quiet material versus 100 ns before the audit,
and about 180 ns/sample during active processing versus 100 ns. These are
machine-specific measurements with debug timing disabled, not realtime CPU
budgets. The stronger detector costs more when its conservative bound cannot
skip reconstruction; SIMD pairing, mirrored history, and independent FIR
accumulators reduce that cost.

Validation entry points:

```sh
make test-sil-stages test-sil-low-band-recovery test-sil-boost test-sil-perf
make build/tests/sil_limiter_peak_window_spec
build/tests/sil_limiter_peak_window_spec
```

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
   Once channel correlation becomes positive, if low-band Mid is significant
   (above 1% of the mean channel energy) and the reference correlates with it
   below −0.5, flip the reference polarity to follow Mid. Handoff continuity
   alone could otherwise leave an inverted reference that later cancels
   partially correlated bass (measured −10 dB). Retain continuity throughout
   negatively correlated folds, including unequal channel levels: realigning
   there made the smoothed reference cross zero and caused a 12.7 dB bass dip.
   Bidirectional dominance sweeps now stay within 1.4 dB per 40 Hz cycle at
   44.1–192 kHz.
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
