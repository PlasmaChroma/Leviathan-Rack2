# Bifurx Performance Notes

## Current Status

Bifurx now has a useful split between audio-thread cost, display preparation cost, and UI/GL draw cost. Rack's built-in module CPU meter should still be treated as the primary audio-thread reference, but the debug terminal now has enough audio timing detail to correlate settings changes with Rack's meter.

Current important behavior:

- `Modulation Quality` is exposed in the context menu and defaults to `Balanced`.
- The old `Control Update` state is still loaded for legacy patches and maps to the new quality states.
- Resampling mode is hidden from the UI because it does not fit the current model well enough.
- Preview/display bookkeeping no longer runs at full audio rate when it only needs to feed the display.
- Analysis frame publishing no longer copies full FFT buffers on the audio thread; the audio thread publishes the current write position and the UI assembles frames from the ring buffers.
- The debug terminal `Audio us` metric now resets on each submitted interval, so it behaves like a rolling measurement instead of a lifetime average.
- Audio timing is sampled using `kPerfMeasureDivision = 17`, intentionally avoiding phase lock with the 8/16 sample modulation dividers.

## Implemented Optimizations

Audio-thread housekeeping:

- Preview state smoothing and target-motion checks are gated by preview publish ticks.
- Preview settle timing is preserved with accumulated sample counts instead of assuming per-sample preview work.
- Analysis ring wrap uses a power-of-two mask instead of modulo.
- Analysis publishing uses `analysisPublishedWritePos` and `analysisPublishSeq` instead of audio-thread frame copies.
- Debug timing avoids measuring every sample and only runs when Dragon King debug is enabled.

Control and coefficient work:

- Slow controls are cached behind the modulation-quality update gate.
- `SPAN`, `BALANCE`, and `RESO` CV shaping no longer runs every sample in `Balanced`/`High` unless the quality setting requires it.
- SM/XM driven-mode coefficient prep uses thresholded coefficient caching.
- TITO no longer forces the full base-control recompute path every sample solely because SM/XM coupling is active.
- `fastLog2()` is used for the remaining span boundary calculation where the approximation is acceptable.

Debug terminal cleanup:

- Removed low-value terminal fields such as `P-Seq`, `A-Seq`, and `Verts`.
- Added `Audio us` as the primary audio-thread timing field.
- Kept UI/display timing fields separate from audio timing.

## Modulation Quality

The current menu is:

- `Modulation Quality`
- `Balanced`
- `High`
- `Exact`

The purpose of this menu is to make the tradeoff user-facing: higher modulation tracking quality costs more CPU.

Current behavior:

- `Balanced`
  - Default.
  - Slow-control divider: 16 samples.
  - Normal SM/XM coefficient refresh thresholds.
  - Intended for normal envelopes, LFOs, manual movement, and most patches.

- `High`
  - Slow-control divider: 8 samples when slow CV is connected.
  - SM/XM coefficient refresh thresholds are stricter than `Balanced`.
  - Intended for patches where modulation feel matters but exact audio-rate control math is not necessary.

- `Exact`
  - Slow-control divider: 1 sample.
  - SM/XM coefficient refresh thresholds are effectively zero.
  - Intended for maximum tracking at the highest CPU cost.

Current important limitation:

- `V/OCT` and `FM` are still treated as audio-rate inputs when connected.
- The quality setting mainly affects slower control paths such as `SPAN`, `BALANCE`, `RESO`, and SM/XM coefficient refresh policy.

## Measurement Notes

Useful confirmed reference measurement:

- SPAN CV patched only:
- `Balanced`: about `0.28 us`
- `High`: about `0.31 us`
- `Exact`: about `0.63 us`

This now correlates with Rack's built-in module CPU meter after the debug metric reset/sampling fix.

Interpretation:

- If only `IN` and `OUT` are patched, modulation quality should not move `Audio us` much because the expensive modulated control paths are not active.
- To see the setting matter in Rack, patch a modulation source into `SPAN`, `BALANCE`, or `RESO`.
- `FM` and `V/OCT` are not the main expected proof case for this setting because those are currently treated as audio-rate by design.
- UI/GL work should be judged with the UI timing fields, not Rack's module CPU meter.

## Next Steps If Needed

1. Validate remaining expensive cases with repeatable Rack patches.
   Use at least three fixtures: plain `IN -> OUT`, `SPAN` CV modulation, and worst-case `SPAN + FM` modulation. Compare Rack CPU and debug `Audio us` for each quality setting.

2. If `Balanced` and `High` are still too close in real patches, make `High` more explicitly expensive.
   The current `High` behavior only drops to an 8-sample divider when slow CV is connected. If that is too subtle, make `High` always use the 8-sample divider and consider stricter SM/XM thresholds.

3. If heavy `FM` or `V/OCT` patches remain the dominant cost, consider a separate audio-rate input policy.
   This is higher risk than the current quality menu because users may expect pitch inputs to track at audio rate. A safe option would need to preserve exact tracking as an available mode.

4. If SM/XM modes are still too expensive, tune the coefficient refresh policy before changing the filter algorithm.
   First adjust the relative/absolute thresholds by quality mode. Only after that consider coupling smoothing or approximating coefficient updates, because those are more likely to affect sound.

5. If baseline cost remains high with no modulation patched, focus on core filter algorithm work.
   Most low-risk housekeeping has already been moved out of the per-sample path. Further wins in the plain path are likely to come from algorithmic changes, not more display-prep cleanup.

6. If visual performance becomes the issue, keep it separate from audio optimization.
   Use `lastDrawMsEma`, `lastCurvePrepUs`, and `lastOverlayPrepUs` rather than `Audio us`. Rack's module CPU meter should not be used as the main GL/UI metric.

## Recommended Priority

The next best optimization depends on the failing case:

- Plain mode too high: inspect the core dual-SVF path and output stage.
- `SPAN`/`BALANCE`/`RESO` CV too high: tune `Modulation Quality` divider behavior and thresholds.
- SM/XM too high: tune dynamic coefficient refresh thresholds by quality mode.
- `FM`/`V/OCT` too high: decide whether an optional non-exact pitch-input policy is acceptable.
- UI feels heavy but Rack CPU is fine: optimize GL/FFT drawing, not `Bifurx::process()`.

Default recommendation:

Keep `Balanced` as the default because it now gives a measurable CPU win on modulated slow-control patches while preserving normal modulation feel. Use `Exact` as the escape hatch for users who prefer maximum tracking over CPU.

## Guardrails

- Do not remove the exact path; quality must remain user-selectable.
- Do not add locks, allocations, or UI ownership dependencies to `process()`.
- Do not change Rack parameter/input/output enum ordering for released modules.
- Keep legacy JSON compatibility for the old `controlUpdateMode` state.
- Treat WSL plugin link failures as non-authoritative; validate with focused compiles/tests here and final plugin linking in the Windows/MSYS2 toolchain.
