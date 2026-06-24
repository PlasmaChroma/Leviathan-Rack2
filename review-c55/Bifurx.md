# Bifurx Review

## 1. Executive Summary

Bifurx is a dual-peak, 11-mode SVF/filter instrument with drive, span/balance modulation, TITO character, spectrum and response displays. DSP intent is strongly represented and has the suite's best filter test coverage. Complexity in rendering/worker lifecycle, monophony, and production debug options remain release risks.

Release readiness: 8/10

## 2. Module Inventory

- Source/UI: `Bifurx.cpp/.hpp`, `BifurxUI.cpp`, `BifurxGL.cpp`, render-data/prep/worker files. Shared GL/NanoVG/visual/SVG helpers. Assets: `res/bifurx.svg`, `res/icon/Vahdrim'Keth.svg`, shared `res/proc.svg` anchors/icons.
- Params (12): Mode, Level, Frequency, Resonance, Balance, Span, FM amount, Span attenuverter, TITO, previous/next/menu. Inputs (6): audio, V/Oct, FM, resonance/balance/span CV. Output: audio. Lights (6): bipolar FM/span and TITO sides.
- Menu/state: modulation quality, colors, renderer/offload, self-oscillation, limiting, FFT/response overlays and debug logging; JSON saves visual/DSP options and creation time. No expander.

## 3. DSP and Audio/CV Correctness

The TPT SVF path sanitizes audio, bounds CV/frequency, recalculates coefficients by modulation quality, and resets caches on sample-rate change (`Bifurx.cpp:737-1027`). Bypass is configured. It is mono only. Control-rate modes intentionally decimate slow CV, so high-frequency modulation differs by quality setting and should be documented. Output finiteness is well covered by existing specs.

## 4. UI, Panel, and Interaction Review

The response/spectrum display and mode readout communicate a complex filter well and fit the Leviathan aesthetic. Three render paths plus a worker/offload choice create a large compatibility surface. Browser-preview behavior and fallback paths need validation on systems without usable OpenGL.

## 5. Performance Review

- Severity: Medium — audio thread copies three 2048-sample analysis frames every 1024 samples (`Bifurx.cpp:713-735`), creating periodic spikes.
- Severity: Medium — the global worker uses mutexes, queues and shared allocations; lifecycle is deliberate but needs teardown stress (`BifurxWorker.cpp`).
- Severity: Medium — GL/NanoVG/worker variants multiply GPU and memory costs; debug timing is sampled rather than per-sample (`Bifurx.cpp:779`).
- Severity: Low — DSP fast paths and cached coefficients are effective.

## 6. Stability and Rack Integration

`destroy()` explicitly shuts down the render service, reducing static-order risk. Worker slots unregister and rendering has fallback modes. JSON clamps modes and migrates legacy keys. Test missing-context/headless construction, rapid delete/duplicate, and GL context loss.

## 7. Code Quality and Maintainability

The renderer is decomposed, but core `Bifurx.cpp` remains 1,786 lines and contains dense one-line statements that impair review. The shared worker is justified; additional renderer modes should be resisted until existing paths have a compatibility matrix.

## 8. Musical Usefulness

Wide span, balance, filter families, drive, self-oscillation, and live response make Bifurx distinctive and useful. Defaults pass audio sensibly. Modulation-quality tradeoffs and display-only modes need concise user documentation.

## 9. Bugs and Risk Register

| ID | Severity | Area | Finding | Evidence | Suggested Fix |
| -- | -------- | ---- | ------- | -------- | ------------- |
| BIFURX-001 | Medium | Audio RT | Periodic 3x2048 frame copies occur in `process()`. | `Bifurx.cpp:713-735,1010` | Decimate/rotate immutable blocks or move capture preparation off-thread. |
| BIFURX-002 | Medium | Integration | Three renderer paths and worker teardown broaden failure modes. | `BifurxGL.cpp`; `BifurxWorker.cpp`; `plugin.cpp:137-140` | Add GL-loss/headless/delete stress tests and retain NanoVG fallback. |
| BIFURX-003 | Medium | DSP | Module is forced mono. | `Bifurx.cpp:579,784-959` | Document mono or add per-channel SVF state. |
| BIFURX-004 | Low | Maintainability | Core implementation is dense and oversized. | `Bifurx.cpp` | Format and separate analysis publication from DSP. |

## 10. Recommended Fix Plan

### Must Fix Before Release

No proven DSP blocker; complete renderer/lifecycle stress validation.

### Should Fix Soon

Reduce analysis-copy spikes, document modulation quality/mono behavior, and test headless fallback.

### Nice to Have

Refactor dense core code and consider polyphony.

## 11. Suggested Tests

Keep existing 41 filter/runtime tests; add 44.1-192 kHz sweeps, audio-rate CV by quality mode, bypass/preset/duplicate, NaN inputs, renderer switching, GL loss, browser preview, rapid create/delete, 50 instances, and spectrum memory profiling.

## 12. Final Verdict

Status: Near release-ready  
Primary blocker: renderer/worker compatibility validation  
Best next action: stress teardown/render fallbacks and flatten audio analysis spikes
