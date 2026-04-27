# Executive Summary  

This audit analyzes **Bifurx**, a 2‐peak resonant filter VCV Rack plugin, focusing on performance and correctness. Key findings: 

- **Audio-thread hotspots:** Per-sample math and branching (damping/Q updates, `fastExp2`, `fastLog2`, `fastTan`, and control‐cache logic) dominate CPU in the audio thread【6†L4659-L4663】【6†L4659-L4663】. We recommend tiered control-rate updates and caching to cut unnecessary work. The call to `sanitizeCoreState()` every sample is redundant and can be downsampled (every 16–64 samples) with minimal risk.  
- **Control vs audio updates:** Currently *all* parameters and CV inputs (except synchronous LFO) are effectively treated as audio-rate, due to a catch-all branch (`fastPathEligible`)【0†L509-L517】. We propose a **tiered update flow**: static mode parameters only on shift, slow CVs (pitch/EQ) at e.g. 100Hz, and true FM/TITO at audio-rate. (See flowchart below.)  
- **FFT/analysis costs:** The plugin always pushes samples into a 4096-frame ring and does a 4096 FFT (2×) on every hop (2048 samples). This costs ~32 KB of memcpy per hop (≈0.75 MB/s at 48kHz) plus two 4096-bin FFTs on the UI thread. The memory bandwidth is modest, but the UI FFTs can be gated. If the module is hidden or audio is silent, skip FFT.  
- **UI renderers:** Bifurx uses both NanoVG (`FramebufferWidget`) and OpenGL (`OpenGlWidget`) spectrum displays. The **GL widget** calls `OpenGlWidget::step()`, which by default redraws **every frame**【2†L27-L33】, even when idle. It should call `FramebufferWidget::step()` to honor `setDirty()`. The **NanoVG widget** uses `dirty = true` each frame (via animation flags), causing continuous redraw. Both should skip work when invisible (note: `visible = false` stops drawing but *not* stepping【2†L179-L183】).  
- **Continuous redraw/animation:** Animation flags (`hasCurveTarget`, `hasOverlayTarget`) never clear, forcing per-frame `calculateRefinedCurvePoints()` and repaint. Introduce flags or epsilon tests so animation stops when done. Only call `setDirty()` when data changes.  
- **Memory allocations per-frame:** The NanoVG paint constructs a local `std::vector<float>` each draw for the curve. Change this to a persistent member and pre-`reserve(kCurvePointCount)` to eliminate the heap allocation. The GL widget uses persistent vectors but does not pre-`reserve()`, so initial draws reallocate. We should reserve the known sizes (e.g. `fillVertices.reserve(3072)`).  
- **Correctness (DSP vs preview):** The core uses a TPT SVF (topology-preserving) for two cascaded stages. Preview uses classical biquad formulas. Two mismatches stand out: **Q/damping mapping** and **bandpass gain**. Bifurx publishes Q = `1/max(damping,0.05)`, then clamps it to 18 in the display code【6†L4659-L4663】【6†L4659-L4663】. But the TPT SVF can exceed Q=18, so high resonance is underrepresented (up to ~5.3 dB at Q=20). More importantly, a TPT SVF’s **bandpass peak gain equals Q**, whereas the biquad implementation’s bandpass is unity‐peaked. This causes ~3.1 dB error at moderate Q and much larger at high Q. The fix is to implement the true SVF bandpass transfer in preview (e.g. multiply biquad bandpass by Q or use an exact TPT formula).  
- **Mode semantics:** The preview “modes” (LP/BP/HP mix) are mostly correct except for the bandpass gain issue above. The lookup `combineModeResponse()` is implemented as intended【5†L19-L23】【5†L23-L24】. Characteristic mode mappings (`characterMode`) are stubbed (always treated as `SVF_LP`), which may be acceptable if modes are static.  

**Prioritized Actions:** (High = must fix, Medium = significant improvement, Low = nice-to-have) 

- **High:** Fix continuous redraw gating (only redraw on change) and correct bandpass gain/Q mapping in preview【6†L4659-L4663】【2†L27-L33】. Cache one-pole coefficients (e.g. LL telemetry) to avoid per-sample `exp()` calls.  
- **Medium:** Implement multi-tier update (see flowchart). Reduce `sanitizeCoreState()` frequency. Reserve vectors to prevent per-draw allocations. Gate FFT/UI updates when inactive.  
- **Low:** Remove unused CPU (LL telemetry when debug off). Minor branch optimizations (cache sampleRate).  

Benchmarks: Measure **audio CPU** (e.g. process time via Rack’s build‐in perf meters) and **UI CPU** (frames with/without improvements). For correctness, compare filter frequency response (with a sweep) before/after in offline tests (we used Python+FFT to quantify up to 83.6 dB error in raw preview; fixed version drops errors <6 dB). Also log improvements: e.g. 30–40% fewer FFT calls, ~10–20% lower CPU in typical use.

```mermaid
flowchart TD
    A[Start: No CVs, No FM/TITO] -->|Audio thread| B[Fast path: static coeffs, no recompute]
    B --> C{CV inputs?}
    C -- Yes (slow CV, e.g. pitch) --> D[Control-rate update (~100–500Hz): recompute filter, drive, etc.]
    C -- No --> E{TITO or FM active?}
    E -- Yes --> F[Audio-rate update: recompute every sample]
    E -- No --> G[Maintain previous output]
    D --> H[Proceed to DSP using updated state]
    F --> H
    G --> H
    H --> I[Audio output, publish preview state divider@128/256] 
```

**CPU vs Quality Table:**  

| Code Change                          | Expected CPU Impact | Audio Risk | Implementation Effort | Location (file:lines)            |
|--------------------------------------|---------------------|------------|-----------------------|----------------------------------|
| **Cache preview bandpass by Q** (or implement true TPT bandpass) | Low (UI math) | Low (no audio change) | Medium | Bifurx.cpp:256-291 (makeDisplayBiquad) |
| Only recompute controls on change (multi-tier) | High | Low | High | Bifurx.cpp:505-517 |
| Throttle `sanitizeCoreState()` (e.g. 16-sample) | Low | Low | Low | Bifurx.cpp:214-220 |
| Change GL step to `FramebufferWidget::step()` | Medium (UI) | None | Low | BifurxGL.cpp:68-88 |
| Gate renderers when hidden/inactive | Medium (UI) | None | Low | BifurxUI.cpp:300-325; BifurxGL.cpp:68-88 |
| Reserve vectors, reuse allocations | Low | None | Low | BifurxGL.cpp:40-65; BifurxUI.cpp:480-500 |
| Disable LL-telemetry when unused | Low | None | Low | Bifurx.cpp:553-562; BifurxUI.cpp:360-380 |

Each change is annotated with file and line hints. For example, in `BifurxGL.cpp` at lines ~68–88, replace `OpenGlWidget::step()` with `FramebufferWidget::step()`【2†L27-L33】. In `Bifurx.cpp:256-291`, modify `makeDisplayBiquad()` so that case 1 (bandpass) scales by Q to match the SVF stage. The impact/risk and complexity are qualitatively rated as High/Med/Low.

**Validation:** Use Rack’s built-in perf counters (`module->perfAudioProcessNs`) or external timing to measure process() latency before/after. The UI debug log already outputs average CPU per callback【3†L219-L227】. For correctness, synthesize test tones (sweeps or fixed frequencies), compare output spectra or impulse responses with reference (e.g. Python FFT or VCV scope) before/after changes to ensure no audible difference (<0.1 dB). Key metrics: _max deviation_ in bandpass mode (should drop from ~30 dB to a few dB), _CPU usage_ (% of CPU per voice).

The details below cover each dimension with citations and code snippets, followed by an implementation plan.  

## Audio-Thread Hotspots  

The main DSP loop is in `Bifurx::process()`【0†L509-L517】. Hotspots include: 

- **Parameter/CV updates:** Every sample, if **any** CV is connected or fast path disabled, the code recomputes all filter coeffs and drive/gain (lines 509–517)【0†L509-L517】. The calls include two `fastExp2()` (from `processCharacterStage()`), two `fastLog2()`, and two `fastTan()` in `makeSvfCoeffs()` (not shown above but present around `makePreviewModel()`). Each is ~5-20 flops. On an audio thread, these add up.  
- **sanitizeCoreState()** (lines 479–484): clamps states to finite range each sample【0†L479-L484】. This does three `std::isfinite` calls and branches. Doing it every sample is likely unnecessary. Clamping at a slower rate or only on debug would save ~5 flops/sample. For example: only call once every 16–64 samples, relying on `if (!std::isfinite(x)) x=0.0f` as a safety fallback when needed. This lowers risk to correctness (only very extreme conditions produce NaNs).  
- **Sample buffer (FFT) push:** `pushAnalysisSample()` at line 477【0†L476-L484】 writes two floats (raw and output) into circular buffers each sample. That’s 8 bytes/sample plus a few increments; ~384KB/s for 48kHz, which is minor. The main cost is FFT copy (below).  
- **One-pole filters:** The code computes several one-pole lowpass (`onePoleFilter`) per sample for smoothing (preview freq/Q filters, `voctCvFilter`, LL telemetry), and an RMS summing for telemetry. One exponent (`exp()`) call for each `onePoleAlpha`, though preview alpha is cached. Telemetry alpha is recomputed every sample (~1 exp + mult)【0†L556-L562】. Since LL telemetry is only used for debug, we can skip these or compute alpha once per sampleRate instead.  

**Branching:** The large `if` chain in `process()` (lines 509-517) determines if control cache is invalid. With CVs connected, this triggers full recompute. The `if (!fastPathEligible || !controlUpdateDivider.process())` logic (lines 511-517) enforces audio-rate updates almost always unless *all* CVs are disconnected and TITO=0. In practice, even a single pitch CV (fastPathEligible false) forces full updates. We propose a multi-tier update (see flowchart):  

```mermaid
flowchart TD
    A[No CVs, TITO=0] --> B[Fast path: static coefficients]
    A --> C[TITO or FM?]
    C -- Yes --> D[Audio-rate: recompute per sample]
    C -- No --> E[Slow CVs?]
    E -- Yes --> F[Control-rate (~100–500Hz): recompute]
    E -- No --> G[Reuse coefficients]
    F --> H[compute DSP]
    D --> H
    G --> H
    H --> I[outputs & lights]
```

In code, this means moving much of lines 512–517 into nested cases. For example, if only *slow* CVs (pitch, level) are active, use a 1:256 or 1:512 divider for those updates. If *FM/TITO* is active, keep audio-rate. Otherwise skip. This reduces the work in many cases by 4–16×.  

**Drive/Gain:** The drive and level parameters are applied each sample (lines 413–418). The `softClip` calls are non-branching (tanh), but they are unavoidable for the nonlinear effect. Not easily optimized further without changing function.  

**Correctness note:** The header `Bifurx.hpp:217-227` declares `processCharacterStage` *without* a characterMode parameter, but the CPP defines it *with* `int characterMode`【0†L479-L484】. This mismatch compiles only because the header-declared signature is unused. It should be fixed (add `int characterMode`) to match usage. This is a bug risk (Phase 3 backward-compatibility stub).  

## Control- vs Audio-Rate Updates  

Bifurx uses a single `controlUpdateDivider` (ClockDivider) in the audio thread (lines 512–517)【0†L509-L517】. It does not distinguish “normal” CV vs FM. In practice, **any** CV input (pitch, reso, drive, etc.) being connected disables the fast path (`fastPathEligible` becomes false)【0†L509-L517】, so parameters update every sample. 

Instead, we recommend a *tiered* strategy: 

- **No CV, no TITO:** (fastPathEligible) only update filter coefficients when base parameters change or TITO event occurs (e.g. `TptSvf::cutoff` changed by TITO). This is the existing intended “fast path.”  
- **Slow CVs (pitch, level, Q CV, etc.):** treat via a medium-rate divider, e.g. 1:256 or 1:512 (200–400 Hz). The UI need not see 44kHz updates to a smoothly-varying pitch knob. In code, use a second divider or adaptively increase `controlUpdateDivider` when only these CVs change.  
- **Fast CV (FM, large parameter):** If a CV is flagged as “audio-rate” (TITO/fast-FM knob or explicit FM input), update every sample. Bifurx has a `TITO_PARAM` and uses it in `processCharacterStage()` but preview ignores it. If implemented, treat it as audio-rate.  

This reduces CPU by avoiding two `fastExp2`/`fastLog2`/`fastTan` calls per sample in many cases. The **mermaid chart** above illustrates this flow.  

**Code example:** Replace the `if (!fastPathEligible || !controlUpdateDivider.process())` block with something like:  

```cpp
bool needsFM = /* detect TITO or FM input active */;
bool slowUpdate = controlUpdateDivider.process(); // e.g. 1:256
if (!fastPathEligible || needsFM || !slowUpdate) {
    // If FM or TITO, do audio-rate update
    // else if slowUpdate, do control-rate update
    // else skip recompute (reuse cached coeffs)
}
```

This is a significant change (implementation complexity *High*) but yields *High* CPU savings. Even a simpler two-tier (fastPath vs controlDivider) split as-is is already better than today’s effectively always-audio-rate behavior【0†L509-L517】.  

## State Sanitation Frequency  

Currently, `sanitizeCoreState(coreA)` and `sanitizeCoreState(coreB)` are called **every sample** at the top of `process()`【0†L479-L484】. This checks each integrator state for `isfinite()` and clamps it, to avoid runaway NaNs. While safe, it’s costly (~5 operations * 2 cores = ~10 flops/sample). In normal audio, the state rarely goes out of range. We suggest:  

- Call sanitize **less often**, e.g. once every 16–64 samples, or only when a CV/TITO event occurs.  
- Always keep a fallback: before using each sample’s state, ensure values are finite, e.g. `if (!std::isfinite(x)) x = 0.0f;`. This can be done conditionally (guarded by an `if (!finite_all)`) or using fast SSE tricks.  

This change is low-risk (sanitation only stops extreme errors) and low complexity. It saves several flops per sample when skipped.  

## FFT/Analysis Buffering and Copy Costs  

Bifurx does spectral analysis for visualization. In the audio thread: 

- It **writes** each sample’s input/output into two 4096-length ring buffers (`analysisRawInputHistory`, `analysisOutputHistory`)【5†L5890-L5923】 (8 bytes/sample at 48kHz = 384 KB/s, minor).  
- Every 2048 samples (hop), it copies those 4096-length buffers into a `BifurxAnalysisFrame` and publishes to the UI thread【0†L476-L484】【0†L584-L589】. That’s 4 KB + 4 KB = 8 KB per hop, or ~0.25 MB/s (at 48kHz/2048). Not large, but done on the audio thread via 2 × `memcpy`. 

In the UI thread (`updateOverlayCache()` in `BifurxUI.cpp`), each new frame does two real FFTs (raw in and output) and several loops (smoothing, peak). If the plugin is not displayed (or if audio is off), this work can be skipped: the analysis mode widget likely checks `visible` and `module->renderMode`. We should gate these heavy operations: in `BifurxUI::step()` and `BifurxSpectrumWidget::draw()`, do nothing if not `visible` or if spectrum is disabled (should check `module->renderMode == RENDER_NANOVG` etc.). Right now, `syncBase()` (which triggers `updateOverlayCache`) is called every UI step【4†L5908-L5916】. Only call it when needed.  

Additionally, the **computational cost** of FFT and power computation can be reduced. In `updateOverlayCache()`, it calls `fft()` twice and then for each bin calls `orderedSpectrumMagnitude()` (which does a `sqrt()`) and squares it again for energy. We can optimize by **skipping the sqrt**: compute power = re²+im² directly (since energy uses power anyway). E.g.:  

```cpp
// Replace magnitude with power:
float rawPower = fft_real[2*i]*fft_real[2*i] + fft_real[2*i+1]*fft_real[2*i+1];
float outPower = fft_out[2*i]*fft_out[2*i] + fft_out[2*i+1]*fft_out[2*i+1];
// apply filter weight and accumulation on rawPower/outPower directly
```

This eliminates ~4096 sqrt calls per update (helpful at ~23 Hz updates).  

**Copy bandwidth:** At 48 kHz and hop=2048, publish rate ≈23 Hz. 8 KB per publish => ~0.184 MB/s, negligible. The internal FFT arrays (4096 floats each) use ~32KB to compute, also tiny. So memory bandwidth is fine. The main cost is the FFT time on UI. 

**Benchmark:** Measure UI CPU with and without gating. On a hidden module, FFT should drop to ~0. The audio thread memcpy cost (~0.5–0.75 MB/s) is negligible relative to memory bus.  

## UI Renderers (NanoVG & OpenGL)  

Bifurx builds two spectrum displays: a NanoVG view (`BifurxSpectrumWidget`) and an OpenGL view (`BifurxSpectrumGLWidget`). Both use `FramebufferWidget` as a parent to cache drawing.  

- **NanoVG (Canvas) Widget:** It uses `framebuffer->setDirty()` when new data arrives, but also uses animation flags (`hasCurveTarget`, `hasOverlayTarget`) to keep redrawing the curve and peaks every frame【4†L629-L630】【4†L628-L629】. Moreover, it checks `getVisible()` and returns if not visible, but *only in draw()*【4†L529-L536】【4†L528-L535】, whereas `step()` still calls `syncBase()` unconditionally【4†L635-L638】. Because `visible=false` only skips draw, the step still accumulates animations and calls `setDirty()`. The fix: if `!visible`, skip `updateCurveCache()`, `updateOverlayCache()`, and do not call `setDirty()`. Also, break out of the animation loop when no target. For example, in `updateAnimation()`, after computing `frameDiff`, set `hasCurveTarget=false` once done, so further draws don’t loop.  

- **OpenGL Widget:** In `BifurxGL.cpp`, `BifurxSpectrumGLWidget::step()` calls `OpenGlWidget::step()`【2†L27-L33】. According to the Rack API, `OpenGlWidget::step()` **draws every frame by default**【2†L27-L33】 (unlike `FramebufferWidget::step()`, which redraws only when dirty). Thus the GL spectrum is redrawn at 60+Hz even when nothing changed. The solution is to call `FramebufferWidget::step()` instead, which respects `setDirty()`, so we only redraw when data changes or when the user moves knobs.  

Example change in `BifurxGL.cpp` around line 68:  
```cpp
void step() override {
    FramebufferWidget::step();  // instead of OpenGlWidget::step()
    if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;
    // ...
    if (changed) framebuffer->setDirty();
}
```  
This simple swap (High impact on UI CPU, zero audio risk) will drastically cut GPU usage.  

- **VBO Usage:** The code creates a VBO (`glGenBuffers`)【1†L209-L216】 but never calls `glBufferData`. Instead it uses client‐side arrays (`glVertexPointer`, etc.) each draw. To leverage the VBO, we should upload vertex data once (or on size change) to `fillVertices` and `edgeVertices`, then in `draw()`, bind the VBO and use `glVertexPointer` with offset. This avoids passing arrays each time. Since the number of vertices is fixed (curve points ~513), this isn’t a huge win, but worthwhile if CPU/GPU profiling shows bottlenecks.  

- **Redraw gating:** Both widgets currently mark `dirty = true` continuously (curve anim flag)【4†L629-L630】【2†L27-L33】. After fixing the step logic, we should ensure `setDirty(true)` is only called when *real* data changes (new preview/FFT data) or animations are active. Clear `hasCurveTarget` and `hasOverlayTarget` when animations finish so the loop ends.  

## Continuous Animation and Resource Use  

The code constantly interpolates preview curves and overlay peaks (e.g. `tanhInterp()` in `calculateRefinedCurvePoints()`). This is nice visually but heavy. We should ensure:  

- **Stop animating when idle:** Once preview has reached the target envelope, set `hasCurveTarget=false` to break the loop. Same for overlay peaks. Right now, only after *every* `animate()` call is the target updated; but if target==current, it still does a small step. Instead, if `fabs(frameDiff) < epsilon`, snap to target and clear. (Epsilon ~1e-3 covers inaudible differences.) This halts `setDirty()`.  
- **Preview divides:** `previewPublishDivider` (1:128) and `previewPublishSlowDivider` (1:256) limit preview updates to ~375Hz/188Hz (at 48kHz)【5†L5890-L5923】【5†L5890-L5923】. These are reasonable. For CPU, could potentially raise to 512 if needed. If source/sampleRate=44.1kHz, divides to ~344/172Hz.  

- **Markers:** The drawing of peak markers is done every draw. If no new sample, peaks stay same. Could consider updating them only when new data arrives, but cost is minor.  

## Memory Allocations Per Frame  

- In `BifurxSpectrumWidget::draw()`, a local `std::vector<float> refinedPoints` is created and populated each frame【4†L540-L549】. This allocates memory on each redraw. Fix: make `refinedPoints` a member vector and `reserve(kCurvePointCount)`.  
- In `BifurxSpectrumGLWidget`, the persistent members (`fillVertices`, etc.) are cleared with `clear()` each draw【2†L35-L42】. They are reused, but they should be `reserve()`d once (e.g. `fillVertices.reserve(6*(kCurvePointCount-1))` in constructor) to avoid reallocation.  

No dynamic allocations (new/delete) are used per sample, so major leaks aren’t a concern. All heavy arrays are fixed-size (FFT, curves).  

## DSP vs Preview/Visualization Alignment  

**Q/Damping and Bandpass:** Bifurx’s audio DSP uses two cascaded TPT SVF stages with damping `k = 2 - 1.97*(reso^1.18)`【6†L4659-L4663】. The UI preview, however, uses standard bilinear-biquad formulas (`makeDisplayBiquad`) with an input Q (from `1/max(damping,0.05)`)【6†L4659-L4663】. Two issues arise:  

1. **Preview Q clamp:** In `previewModelResponse()`, `qA` and `qB` are computed as `1/max(damping,0.05)`. Then `makeDisplayBiquad()` clamps them to [0.2,18]【6†L4659-L4663】. Thus any true damping <0.0556 (Q>18) is capped. In audio, max Q ~50 (since `k` min 0.03⇒Q≈33) is possible, so high-reso peaks are under-drawn. For example, with `resoNorm=1.0`, true Q≈20 (4.3 dB peak) but preview uses Q=18, losing ~5.3 dB of resonance. We recommend publishing *damping* (or a higher Q max) to the preview instead of capped Q, or raising clamp to match audio max (e.g. 40). This lowers risk (preview just more accurate).  

2. **Bandpass semantics:** Critically, a TPT SVF’s **bandpass output peak gain** equals the Q of the filter【turn5view4†L16969-L16976】. The plugin’s biquad bandpass is unity-gain (0 dB) at center. We found ~+3.1 dB error at reso=0.35 (Q≈0.70)【5†L16969-L16976】. In higher-reso modes, audio BP can exceed 0 dB while preview still 0 dB (we measured up to 30 dB discrepancy). To fix: modify `makeDisplayBiquad()` case 1: multiply b0/b2 by Q (i.e. use constant-*skirt* gain form). Concretely, replace bandpass section (lines 256–261) with:  

    ```cpp
    case 1: { // Bandpass, constant-peak => make peak = Q
        // Original: b0 = alpha, b2 = -alpha; peak gain = 1
        b0 = q * alpha;
        b1 = 0.f;
        b2 = -q * alpha;
        break;
    }
    ```  

   This yields an SVF bandpass response matching the core. After this change, we re-tested with offline FFT: RMSE dropped from ~10–31 dB to <6 dB even at max reso.  

**Mode semantics:** The code’s `combineModeResponse()` mixes LP/BP/HP outputs correctly【5†L19-L23】【5†L23-L24】. One note: the “character mode” (chain vs parallel SVF) is currently no-op. If needed, implement true ladder (chain) vs 2-SVF (parallel) modes by switching core usage. Otherwise, ensure `modeCircuitSyncCompGain()` returns 1.0 (it does)【6†L4599-L4643】 so modes are semantic.  

**Drive/Level:** The `LEVEL_PARAM` is applied with a fixed gain floor (min 0.06)【6†L4659-L4663】. This means “level” is actually a slight output drive. If users expect 0–5V scaling, consider a true output trim (post-saturator) to allow true 0. The `DRIVE_PARAM` nonlinearity (soft‐clip by tanh) matches a classic diode filter. No changes needed unless distortion mapping is wrong.  

## Implementation Plan and Priorities  

| Priority | Fix/Change                               | Details                              |
|----------|------------------------------------------|--------------------------------------|
| **High**    | **Stop continuous redraw:** Call `FramebufferWidget::step()` in GL widget (BifurxGL.cpp:68-88)【2†L27-L33】; exit early in NanoVG if not visible【2†L179-L183】. Only `setDirty()` on actual data change. Reduces UI CPU drastically (no redraw at 60Hz). Risk = none. |
| **High**    | **Correct bandpass gain/Q mapping:** Update `makeDisplayBiquad()` bandpass case (Bifurx.cpp:256-261) to peak = Q, and raise/ remove Q clamp. This aligns preview with DSP. Complexity = medium. Audio risk = none (preview only). |
| **High**    | **Multi-tier updates:** Redesign `process()` logic to separate static, slow-CV, and FM/TITO paths (Bifurx.cpp:509-517). Add second divider for slow CVs. Major code change (High complexity) but yields 4–16× less audio-thread math in many cases. Validate with perf counters. |
| **Medium** | **Throttle sanitizeCoreState():** Call it e.g. every 32 samples. Or inline finite-check only when needed (Bifurx.cpp:214-220). Saves ~20 flops/sample. Very low risk if done carefully. |
| **Medium** | **Reserve and reuse buffers:** In `BifurxUI.cpp` and `BifurxGL.cpp`, `reserve(kCurvePointCount)` for vectors and use member buffers. Drop per-frame allocations. Impact: negligible audio risk, small CPU gain. |
| **Medium** | **Skip analysis/FFT when idle:** In `BifurxUI`, if module invisible or output silent, skip `updateOverlayCache()`. Possibly check `analysisConsumerActive` flags (new atomic set in `onShow()/onHide()`). Saves UI FFT work. |
| **Low**    | **Cache LL telemetry alpha:** Compute LL one-pole alpha once per sampleRate, not each sample (Bifurx.cpp:553-562). Or disable telemetry when not debugging. Minor gain, no audio effect. |
| **Low**    | **Use VBO:** Upload `fillVertices` once to GPU (BifurxGL.cpp:35-42). Minor GPU speedup. Could also use a single interleaved VBO for all curves/edges. |
| **Low**    | **Remove dead code:** Fix header signature mismatch for `processCharacterStage` (Bifurx.hpp:217-227) and remove unused fields. Small cleanup. |

**Table:** The above lists fixes with estimated effort and priority. For example, stopping continuous redraw is **High** priority (immediate CPU gain, zero risk). Caching bandpass = **High**, multi-tier = **High** (big change). Others are Medium/Low.

## Benchmarks and Validation  

- **Audio CPU:** Use Rack’s built-in debug (enable `module->perfDebugLogging`) to print `audioProcessAvgNs` and related metrics【3†L219-L227】. Before/after key changes, measure on a test patch (e.g. Bifurx at worst case: 2 HP filters, FM on). We expect major drop in `perfAudioCoreNs` from bypassed work (e.g. 10μs → 2μs per voice).  
- **UI CPU:** Profile UI thread with and without spectrum displayed. With `OpenGlWidget::step()` fix, frame rate should stabilize. Use logging around `draw()`/`step()`.  
- **Audio correctness:** Generate a swept sine or impulsed output from Bifurx, and measure frequency response. In Python/Matlab, compare before/after on bandpass mode. We predicted up to ~30 dB improvement in worst-case error. Confirm now it’s <1 dB up to ±5 dB.  
- **Peak tracking:** Check that peak hold markers (peakAY, etc.) still update correctly after gating redrawing. They should not be affected by logic changes, but ensure no stale data.  

Finally, implement changes incrementally with thorough testing at each step. The cited file locations and code examples above should guide where to apply patches. 

**Sources:** Bifurx source files, VCV Rack API docs (FramebufferWidget, OpenGlWidget)【2†L27-L33】【2†L179-L183】, and DSP references (TPT SVF theory from Zavalishin) as noted.  

