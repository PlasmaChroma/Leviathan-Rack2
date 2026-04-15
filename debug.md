# Bifurx Debug Handoff (2026-04-15)

## Current focus
Filter-curve peak stability in `Notch + Notch`, especially during/after frequency movement and hard stop turnarounds.

## What is implemented right now
- Dedicated curve debug CSV logging (not Rack `log.txt`), with context-menu toggle:
  - `Log Curve Debug`
  - writes to `asset::user()/Leviathan/Bifurx/curve_debug/curve_debug_<timestamp>.csv`
- Logging moved to UI `step()` path (instead of draw path).
- CSV includes:
  - `preview_seq`, `preview_updated`, `analysis_seq`, `analysis_updated`
  - span/reso/balance fields
  - `peak_*_y_curve` and `peak_*_y_marker`
  - `ui_frame_ms`
- Dynamic preview publish improvements:
  - slow divider currently `256`
  - adaptive publish path still active for fast movement
- Curve visual tuning currently active:
  - range remains `-48/+48 dB`
  - visual slew limiter active (`kCurveVisualSlewDbPerSec = 170`)
  - curve smoothing is linear/eased at `0.20` in widget step
- **Turnaround-specific preview smoothing boost was removed** (at user request).
- **Instant settle logic is active** and now applies to manual sweeps too:
  - `kPreviewInstantSettleMotionOctThreshold = 2e-5`
  - `kPreviewInstantSettleHoldSamples = 96`
  - key branch: `previewInstantSettleNow`

## Key findings from recent logs
- `curve_debug_1776277648740.csv`: very good stability (`yJump > 8px ~0.2%`).
- `curve_debug_1776278753298.csv`: worse tails (`yJump > 8px ~2.2%`, `max ~13.9px`) after motion-compensated curve tweak.
- Motion-compensated curve tweak was removed afterward.
- `curve_debug_1776280236534.csv` (manual sweep + hard stops):
  - showed slow settle after stops (large post-stop drift),
  - revealed instant-settle was effectively not helping manual stops,
  - fixed by removing CV-only gating from instant-settle condition.

## What needs validation next (first step when resuming)
Run the same **manual sweep with hard stops** again and analyze the new CSV.

Primary check:
- After each hard stop, does peak Y settle quickly (near-immediate), instead of continuing to drift for hundreds of ms?

Secondary checks:
- `yJump > 8px` and `p99` max-step vs prior logs.
- Ensure balls stay on curve (`peak_y_marker ~= peak_y_curve`, only tiny clamp exceptions acceptable).

## Useful file/anchors
- Main file: `src/Bifurx.cpp`
- Instant settle constants: near `kPreviewInstantSettleMotionOctThreshold`, `kPreviewInstantSettleHoldSamples`
- Instant settle branch: `previewInstantSettleNow`
- Debug CSV schema/header: `BifurxSpectrumWidget::startCurveDebugCapture()`

## Build status
- `make -j4` passes after latest changes.

