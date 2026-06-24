# TD.Scope Review

## 1. Executive Summary

TD.Scope is a dedicated Temporal Deck expander that renders live/sample waveform history and sends interactive lag-drag requests back to its host. Protocol validation and snapshot design are thoughtful, but unconditional per-sample performance timing and a very large multi-renderer UI make it unsuitable for release without profiling fixes and stress validation.

Release readiness: 6/10

## 2. Module Inventory

- Source/UI: `TDScope.cpp/.hpp`, `TDScopeWidget.cpp`, `TDScopeGL.cpp`, `TDScopeShared.hpp`; shared GL/NanoVG lifecycle and expander protocol. Asset: `res/tdscope.svg`.
- Params/inputs/outputs: none. Lights: Link and Preview. Left expander consumes `HostToDisplay`; writes `DisplayToHost` through the neighboring Temporal Deck right-expander buffers.
- Menu/state: mono/stereo/invert, voltage range/auto, brightness/colors, framebuffer cache and debug render/rate modes. JSON saves all view/render settings.

## 3. DSP and Audio/CV Correctness

Protocol messages validate magic/version/size and link staleness is sample-rate-derived (`TDScope.hpp:414-577`). UI-to-engine drag fields use a sequence snapshot. It is intentionally non-audio. Direct writes into the neighbor's producer buffer follow Rack's expander contract but deserve a hot-plug race test. No sample-rate callback is required.

## 4. UI, Panel, and Interaction Review

Attach/wait status, stereo lanes, colors, ranges and direct waveform dragging are clear. Rendering spans standard NanoVG, tail raster, OpenGL and shader implementations across more than 4,700 lines. Fixed vertical supersampling ignores zoom (`TDScope.cpp:18-23`), so blur/aliasing must be visually checked.

## 5. Performance Review

- Severity: High — `steady_clock::now()` is called twice on every audio sample unconditionally (`TDScope.hpp:414-577`).
- Severity: High — large vectors, raster buffers and multiple renderer caches can consume substantial UI memory (`TDScope.cpp:49-145`, `TDScopeGL.cpp`).
- Severity: Medium — framebuffer caching is optional and drag marks every frame dirty; large path/raster updates need GPU profiling.

## 6. Stability and Rack Integration

Neighbor identity uses model pointer or slug and messages are versioned. NanoVG image ownership has explicit cleanup. Test module deletion/reordering during drag, missing GL, headless/browser preview, and host version mismatch. The module correctly shows an attach prompt when standalone.

## 7. Code Quality and Maintainability

The protocol and geometry helpers are well isolated, but renderer complexity dominates. Keep one guaranteed NanoVG path and one optional accelerated path; the current debug modes are too costly to maintain as product features.

## 8. Musical Usefulness

As an interactive waveform/navigation surface, TD.Scope substantially extends Temporal Deck rather than duplicating a generic scope. It has little standalone value, appropriately communicated by the panel.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| TDSCOPE-001 | High | Audio RT | Unconditional timing on every process call. | `TDScope.hpp:414,575-577` | Guard/sparsely sample metrics. |
| TDSCOPE-002 | High | UI/Memory | Four rendering/cache strategies create large state and compatibility surface. | `TDScope.cpp:49-145`; `TDScopeGL.cpp` | Establish budgets; retain a single fallback and one accelerated path. |
| TDSCOPE-003 | Medium | Visual | Supersample function returns a fixed density regardless of zoom. | `TDScope.cpp:18-23` | Implement bounded zoom adaptation or document/test fixed density. |
| TDSCOPE-004 | Medium | Verification | No Rack hot-plug/GL lifecycle tests. | Existing test list | Add host-expander and renderer lifecycle integration tests. |

## 10. Recommended Fix Plan

### Must Fix Before Release

Remove unconditional audio-thread timing and validate hot-plug/renderer fallback.

### Should Fix Soon

Reduce renderer variants and define UI memory/GPU budgets.

### Nice to Have

Adaptive zoom density and a smaller non-debug menu.

## 11. Suggested Tests

Attach/detach/reorder during drag; delete either module; malformed/stale protocol; 44.1-192 kHz; mono/stereo transitions; sample/live preview; save/load/duplicate; NanoVG-only/headless/GL context loss; 50 scopes; 25%-400% zoom; raster allocation and frame-time profiling.

## 12. Final Verdict

Status: Experimental  
Primary blocker: unconditional audio-thread instrumentation and renderer complexity  
Best next action: remove timing overhead and prove one fallback/one accelerated renderer
