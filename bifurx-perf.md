# Bifurx Performance Notes

## Status

- Optimization #1 is now implemented in `src/Bifurx.cpp` and `src/Bifurx.hpp`.
- Preview bookkeeping (including target-motion `std::log2` work and preview-state construction) now runs only on preview publish ticks (or first publish), instead of every audio sample.
- Settle-hold timing is preserved in sample units via an accumulated sample counter (`previewSampleAccum`), so the instant-settle behavior remains consistent with the prior sample-based thresholds.
- `previewPitchCvConnected` still updates per sample for telemetry, and adaptive cooldown decrement remains sample-based.

## Current read

Rack's built-in module CPU meter is expected to reflect `Bifurx::process()` on the audio thread, not the OpenGL/NanoVG display path. A reading around 3.3% should therefore be treated first as an audio-thread cost problem.

Recent GL display work may affect UI/GPU smoothness, but it is unlikely to be the main source of Rack's CPU percentage unless Rack is specifically measuring UI separately.

Primary hot path:

- `src/Bifurx.cpp`
- `Bifurx::process(const ProcessArgs& args)`

## Main low-risk opportunity: move preview bookkeeping off the per-sample path

The filter core already has a fast path for stable controls:

- `fastPathEligible`
- `controlUpdateDivider`
- cached SVF coefficients

However, even when audio processing is fast-path eligible, `process()` still performs preview/display bookkeeping every sample. This includes:

- preview target smoothing
- preview target motion detection
- adaptive publish checks
- preview state construction
- some atomics
- `std::log2()` calls for display-target motion detection

The clearest expensive display-only section is around:

- `Bifurx.cpp`: `pTFqA`, `pTFqB`, `pTQA`, `pTQB`, `pTBal`
- `tMAOct`, `tMBOct`, `tMOct`
- `previewFreqAFiltered`, `previewFreqBFiltered`, `previewQAFiltered`, `previewQBFiltered`, `previewBalanceFiltered`
- `publishPreviewState(pS)`

Important observation:

This work feeds the spectrum/filter preview display. It does not directly affect audio output.

## Recommended first optimization

Move most preview smoothing and adaptive checks behind the existing preview dividers:

- `previewPublishDivider`
- `previewPublishSlowDivider`

Current divider constants:

- `kPreviewPublishFastDivision = 128`
- `kPreviewPublishSlowDivision = 256`

Current code computes `perTick` late:

```cpp
const bool perTick = pPitchCvConn ? previewPublishSlowDivider.process() : previewPublishDivider.process();
```

But the preview smoothing and motion detection happen before that. The change should be:

1. Compute `pPitchCvConn` and `perTick` before preview smoothing.
2. Only run preview smoothing, target-motion detection, adaptive checks, and state publishing when:
   - preview is uninitialized, or
   - `perTick` is true, or
   - a forced/adaptive immediate update is needed.
3. Use an effective smoothing alpha for skipped samples so visual timing stays similar.

For example:

```cpp
const int previewDivision = pPitchCvConn ? kPreviewPublishSlowDivision : kPreviewPublishFastDivision;
const float perSampleAlpha = pPitchCvConn ? previewFilterAlphaSlow : previewFilterAlpha;
const float effectiveAlpha = 1.f - std::pow(1.f - perSampleAlpha, float(previewDivision));
```

Then use `effectiveAlpha` when updating the preview filtered values on divider ticks.

Potential issue:

`std::pow()` should not run every sample. Either compute it only on divider ticks, or derive/update cached effective alphas when sample rate changes. Since the preview alpha is only recalculated when sample rate changes, effective alpha can be cached as well.

Expected benefit:

- Removes per-sample preview `std::log2()` calls.
- Reduces display-only math on the audio thread.
- Reduces preview-state construction frequency.
- Does not change the audio signal path.

## Specific code area to refactor

Current section starts after audio output is written:

```cpp
const float out = applyLevelOutputStage(modeOut, level, softLimitingEnabled);
outputs[OUT_OUTPUT].setChannels(1);
outputs[OUT_OUTPUT].setVoltage(out);
```

The preview block begins around:

```cpp
const float pTFqA = clamp(freqA0, 4.f, 0.46f * args.sampleRate);
const float pTFqB = clamp(freqB0, 4.f, 0.46f * args.sampleRate);
```

The expensive display-only target-motion lines are:

```cpp
const float tMAOct = std::fabs(std::log2(std::max(pTFqA, 1.f) / std::max(previewPrevTargetFreqA, 1.f)));
const float tMBOct = std::fabs(std::log2(std::max(pTFqB, 1.f) / std::max(previewPrevTargetFreqB, 1.f)));
```

These should not be paid every sample if the preview is only published every 128 or 256 samples.

## Secondary opportunity: cheaper analysis history writes

`pushAnalysisSample(in, out, modeOut)` runs every sample.

The analysis history length is `kFftSize = 4096`, which is a power of two. Current write position update:

```cpp
analysisWritePos = (analysisWritePos + 1) % kFftSize;
```

This can become:

```cpp
analysisWritePos = (analysisWritePos + 1) & (kFftSize - 1);
```

This is small but safe.

Larger question:

Should analysis history be collected when the spectrum overlay is hidden or unavailable? If not, the module can skip some analysis work. This needs a UI/feature decision because disabling collection may make the display take one FFT window to refill when re-enabled.

## Secondary opportunity: analysis publishing cost

`publishAnalysisFrame()` copies three 4096-sample buffers into the double-buffered analysis frame:

- raw input
- output
- response output

This happens after the analysis ring fills and then every hop:

- `kFftHopSize = kFftSize / 2`
- currently every 2048 samples

This is not likely the main 3.3% cost, but it can create periodic spikes. If spikes show in debug telemetry, options are:

- increase hop size for lower update rate
- use one contiguous rolling frame copy strategy
- only publish when the display is visible
- lower `kFftSize` if visual resolution can tolerate it

## Secondary opportunity: fast-path eligibility

Current fast path:

```cpp
const bool fastPathEligible = titoNeutral && !voctConnected && !fmConnected && !slowCvConnected;
```

This is conservative and correct for audio-rate modulation. But it means simply connecting VOCT/FM disables the fast path even if the CV source is slow.

Potential future option:

- Add a user-facing or internal tier where slow CV is smoothed and coefficients update at control rate.
- Keep true audio-rate behavior available.

Risk:

This changes response behavior for CV modulation and should not be the first optimization unless needed.

## SM/XM driven-mode coefficient cost

When TITO is outside the neutral deadband, SM/XM audio-rate cutoff coupling is active:

```cpp
cutoffA = freqA0 * fastExp2(clamp(modA, -2.5f, 2.5f));
cutoffB = freqB0 * fastExp2(clamp(modB, -2.5f, 2.5f));
```

This makes each stage use dynamic cutoff coefficients instead of the stable `cachedCoeffsA` / `cachedCoeffsB` path. The first optimization for this path is now implemented:

- cache SM/XM dynamic coefficients per stage
- refresh only when cutoff movement exceeds a very small threshold
- force refresh on sample-rate or damping changes
- keep SM/XM coupling audio-rate, but do not force the full base-control update block to run every sample solely because TITO is active
- use `fastLog2()` for the remaining span boundary calculation

Current tuning constants:

- `kTitoCoeffRelativeUpdateThreshold = 2.5e-4f`
- `kTitoCoeffAbsoluteUpdateThresholdHz = 0.002f`

If SM/XM still costs too much after testing, the next options are higher risk:

- add a quality option for exact per-sample SM/XM coefficients versus cached/quantized SM/XM coefficients
- smooth or band-limit the coupling signal before cutoff modulation
- approximate coefficient updates by modulating an already-computed `g` value instead of rebuilding full SVF coefficients

## Mode-specific cost

Most modes process two SVF stages. Mode 10 uses the resample filter chain:

```cpp
resampleFilterCore.process(...)
```

If Rack's CPU meter reads high mainly in mode 10, profile that separately. It may require a different optimization plan than the standard dual-SVF modes.

## GL/UI display cost

The GL display has its own performance concerns, but it should be treated separately from Rack's audio CPU percentage.

Recent GL work added:

- FFT fill soft caps
- a crest smoothing stroke pass
- thicker gold filter curve/peak traces

These may affect UI/GPU time but should not materially affect `Bifurx::process()`.

If UI performance becomes the issue, use Bifurx's existing debug/perf telemetry rather than Rack's module CPU percentage.

Relevant fields:

- `lastDrawMsEma`
- `lastDrawVertexCount`
- `lastCurvePrepUs`
- `lastOverlayPrepUs`

## Suggested first patch

Make one small audio-thread-only change:

1. Add cached effective preview alphas:
   - `previewFilterAlphaEffective`
   - `previewFilterAlphaSlowEffective`
2. Recompute them when `previewFilterAlpha` changes due to sample-rate change.
3. Move preview target smoothing, target-motion detection, adaptive checks, and `BifurxPreviewState` construction behind `perTick || !previewFilterInitialized`.
4. Keep `publishPreviewState()` behavior semantically equivalent from the UI perspective.
5. Compile and compare Rack CPU meter before/after with the same patch, mode, sample rate, and cables.

Expected result:

The display may update at the same publish cadence, but the audio thread should spend less time preparing display state between publishes.

## Guardrails

- Do not change audio output behavior in the first pass.
- Do not change filter coefficient math except where it is clearly display-only.
- Do not move work from audio thread to UI thread if it requires locks.
- Avoid heap allocation in `process()`.
- Keep existing debug telemetry intact so before/after measurements remain comparable.
