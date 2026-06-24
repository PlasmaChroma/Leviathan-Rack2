# Sil Review

## 1. Executive Summary

Sil is an automatic stereo mastering chain combining repair, low recovery, adaptive EQ, glue compression, stereo enhancement, saturation, true-peak limiting, and metering. It is ambitious and mostly sample-rate aware, but its opaque all-or-nothing processing, audio-thread deque operations, data races in visual buffers, and limited end-to-end testing prevent release confidence.

Release readiness: 6/10

## 2. Module Inventory

- Source/UI: `src/Sil.cpp`, `SilRepairBuffer.hpp`, `SilRepairKernel.hpp`; shared visual/SVG helpers. Asset: `res/sil.svg` plus shared icons.
- Params: Mastering enabled, Repair enabled. Inputs/outputs: stereo L/R. Lights (11): limiter, eight chain stages, mastering and repair.
- Menu/state: color scheme and debug micropeak capture/path; JSON saves color/mastering/repair. No expander.

## 3. DSP and Audio/CV Correctness

Coefficients, lookaheads, histogram and spectrum cadence update on sample-rate change (`Sil.cpp:1440-1469`). Bypass is latency-aligned and smoothly crossfaded. Right input does not normalize from left (`Sil.cpp:1478-1479`), so a mono cable produces left-only processing. External NaN/Inf is not sanitized and many stateful filters could remain poisoned. Only repair-kernel behavior has focused tests; no end-to-end ceiling, latency, bypass or sample-rate test exists.

## 4. UI, Panel, and Interaction Review

Waveform, M/S spectrum and chain LEDs communicate activity, but two switches provide little control or explanation of a highly consequential chain. The visual language is coherent. Stage tooltips/manual documentation need thresholds, latency and gain behavior; users cannot isolate or tune problem stages.

## 5. Performance Review

- Severity: High — `std::deque` monotonic-window push/pop occurs every sample; pre-resize/clear does not portably guarantee allocation-free future pushes (`Sil.cpp:938-951,2018-2034`).
- Severity: Medium — audio copies 2x2048 spectrum snapshots at 24 Hz; UI FFT is correctly off the audio thread (`Sil.cpp:2149-2173,766-833`).
- Severity: Medium — histogram arrays are written by audio and read directly by UI without synchronization (`Sil.cpp:2122-2149,2236-2256`).
- Severity: Medium — full chain uses many filters/nonlinear detectors per sample and needs multi-instance CPU benchmarks.

## 6. Stability and Rack Integration

Constructor preallocates most buffers and destructor closes debug output/deletes FFT. Sample-rate change avoids resizing limiter buffers but does resize rolling program buffers (`Sil.cpp:687-688,1440`), which Rack may invoke outside the process callback but still requires lifecycle verification. JSON is small but lacks a null guard. Repair availability is coupled to global debug mode.

## 7. Code Quality and Maintainability

Named constants and stage state structs help, but a 2,700-line single file and long `process()` make ordering and latency difficult to audit. Extract each stage behind a small, testable interface while retaining the signal chain.

## 8. Musical Usefulness

Sil can be valuable as a fast safety/polish tool, but “automatic mastering” requires predictable gain, stereo image and latency. Lack of stage controls is acceptable only with strong documentation and objective regression fixtures.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| SIL-001 | High | Audio RT | Deque mutation is not guaranteed allocation-free. | `Sil.cpp:938-951,2018-2034` | Replace with fixed-capacity ring monotonic queue. |
| SIL-002 | High | DSP | Mono left input does not normalize to right. | `Sil.cpp:1478-1479` | Normalize R from L when disconnected, or label stereo-only explicitly. |
| SIL-003 | Medium | Thread safety | Histogram buffers race between engine and UI. | `Sil.cpp:2122-2149,2236-2256` | Publish double-buffered snapshots with generation checks. |
| SIL-004 | High | Verification | No end-to-end limiter/latency/SR fixtures. | `Makefile`; only `sil_repair_spec` | Add chain-level reference and invariant tests. |
| SIL-005 | Medium | Robustness | Non-finite input can poison persistent filters. | `Sil.cpp:1478 onward` | Sanitize input and reset invalid stage state. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Use a fixed RT-safe peak queue; define/fix mono normalization; prove ceiling, latency and finite-output invariants.

### Should Fix Soon

Snapshot UI data safely, split/test stages, and document latency/gain.

### Nice to Have

Offer per-stage audition or conservative/intense profiles.

## 11. Suggested Tests

At 44.1-192 kHz: impulse latency, bypass null, sine/sweep gain, true-peak ceiling, DC, silence, NaN recovery, mono-left normalization, stereo correlation, preset/duplicate, toggles during transients, 20 instances, allocation tracing and reference audio fixtures. Verify UI at all zooms under ThreadSanitizer where feasible.

## 12. Final Verdict

Status: Experimental  
Primary blocker: unproven RT safety and mastering invariants  
Best next action: replace the deque and build end-to-end DSP fixtures
