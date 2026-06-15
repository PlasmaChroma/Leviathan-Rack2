# Integral Flux — Rendering Performance Review

Last updated: 2025-06-15

Source: `src/IntegralFlux.cpp` (2164 lines, single-file module)

---

## 1. Architecture Overview

Integral Flux is a Make Noise Maths emulation with two "outer" function-generator/slew channels (CH1, CH4), two "inner" attenuverter channels (CH2, CH3), and a mixing bus (SUM/OR/INV). The rendering pipeline spans three domains:

| Domain | Thread | Rate | Key Cost |
|--------|--------|------|----------|
| DSP `process()` | Audio | 48 kHz+ | Warp math, BLEP, timing laws |
| Widget `step()` | UI | ~60 Hz | Preview point rebuild, tracer capture |
| Widget `draw()` | UI | ~60 Hz | NanoVG/GL paths, knob overlays, debug HUD |

The module carries per-frame performance instrumentation (`gIntegralFluxGearDrawNsThisFrame`, `gIntegralFluxEclipseDrawNsThisFrame`) piped through the debug terminal transport, making draw cost directly observable in telemetry.

---

## 2. DSP Hot Path — What Runs Every Sample

### 2.1 Per-Sample Core

Each call to `process()` (lines 1056–1226) executes:

1. **`applyRequestedTimingUpdateDiv()`** — atomic load + compare (cheap).
2. **`processOuterChannel()` × 2** — the dominant cost center.
3. **Signal BLEP** — `ch1.signalBlep.process()` and `ch4.signalBlep.process()` (`MinBlepGenerator<16,16>`).
4. **Gate BLEP** — `ch1.gateBlep.process()` and `ch4.gateBlep.process()` (when enabled).
5. **Attenuverter scaling** — 4× `attenuverterGain()` + clamp (trivial).
6. **Mix bus** — 4× bus voltage read + fmax chain (gated by `mixOutputsConnected || lightTick`).
7. **11 output writes** + light updates at 120 Hz.

### 2.2 `processOuterChannel()` Internals (lines 658–940)

This is the critical path. Per channel, per sample:

| Operation | Frequency | Notes |
|-----------|-----------|-------|
| `SchmittTrigger.process()` × 2 | Every sample | Trigger + cycle button edges |
| `computeStageTime()` × 2 | **Gated by `timingTick`** | Rise + fall; uses `exp2_taylor5` (3×), `softClamp8` (`std::tanh`), `shapeKnobTimeCurve` (LUT) |
| `slopeWarp()` | Every sample | Quadratic: `1 + k*x²` or `1/(1+k*x²)` — **no transcendentals** |
| `slopeWarpScale()` | Cached per shape change | 16-sample numerical integration of reciprocal warp — expensive but amortized |
| `updateActiveStageTimes()` | Every sample when interpolating | 2 adds + decrement |
| `remapPhasePosForStageTimeChange()` | On timing change only | Clamp arithmetic |
| Preview state publish | Gated by `updatePreviewChannel` timer | ~60 Hz |
| Preview dot publish | Gated by `previewDotPublishTimer` | ~120 Hz |

### 2.3 What Changed Since Earlier Audits

Several recommendations from `doc/flux/codex-performance.md` and `doc/flux/unified_perf_plan.md` have been **implemented**:

| Recommendation | Status |
|----------------|--------|
| Cache `slopeWarpScale()` per shape | ✅ Implemented (lines 797–802) |
| Cache `computeStageTime()` via dirty flags | ✅ Implemented (lines 709–761) with epsilon-based change detection |
| Knob curve LUT to avoid per-sample `std::pow` | ✅ Implemented — `initKnobCurveLut()` precomputes 4096-entry taper table |
| Replace `std::pow(2,x)` with `exp2_taylor5` | ✅ Implemented in `computeStageTime()` and `computeShapeTimeScale()` |
| Timing update decimation (`timingUpdateDiv`) | ✅ Implemented with interpolation dezipper |
| BLEP toggle for EOR/EOC | ✅ Implemented — `bandlimitedGateOutputs` context menu |
| BLEP toggle for signal outputs | ✅ Implemented — `bandlimitedSignalOutputs` context menu |
| Light updates at control rate | ✅ Implemented — 120 Hz timer (`LIGHT_UPDATE_INTERVAL`) |
| Mix bus skip when disconnected | ✅ Implemented — `mixOutputsConnected` gate |
| Remove non-ideal mix saturation | ✅ Removed — `softSatSym`/`softSatPos` no longer present in code |

**Remaining DSP cost** is dominated by:
- `slopeWarp()` — pure arithmetic, runs every sample, already lean
- `softClamp8()` — calls `std::tanh()` inside `computeStageTime()` for BOTH CV, but only runs on `timingTick`
- `MinBlepGenerator::process()` — runs every sample when BLEP is enabled; `<16,16>` template means 16-tap FIR per process call

### 2.4 `std::tanh` Residual

The only remaining `std::tanh` call in the hot path is `softClamp8()` at line 256, used by `bothTimeScaleFromCv()` at line 266. This only fires on `timingTick` (not every sample when `timingUpdateDiv > 1`), and only when BOTH CV actually changed beyond `CV_CACHE_EPS`. Effective cost: **near zero at /8 or /16 decimation**.

### 2.5 `std::exp` Residual

One call to `std::exp` remains at line 1063:
```cpp
const float injectAlphaBase = OUTER_INJECT_GAIN * clamp(1.f - std::exp(-args.sampleTime / OUTER_INJECT_TAU), 0.f, 1.f);
```
This runs every sample but `sampleTime` and `OUTER_INJECT_TAU` are effectively constant — the result could be cached once at sample rate change. Impact is minimal but it's the cleanest remaining win.

---

## 3. UI Rendering Pipeline

### 3.1 Widget Hierarchy

```
IntegralFluxWidget (ModuleWidget)
├── Panel SVG (framebuffered by Rack)
├── TorxScrew × 4 (framebuffered by Rack)
├── PreviewRecessFrameWidget × 2 (inside FramebufferWidget, draw-once)
├── WavePreviewWidget × 2 (extends OpenGlWidget)
│   ├── NanoVG mode: immediate draw
│   └── OpenGL mode: framebuffer + GL_TRIANGLE_STRIP
├── IntegralFluxGearKnob × 0 (not currently instantiated)
├── IntegralFluxHalo2Knob × 4 (rise/fall knobs)
├── IntegralFluxCurveHalo2Knob × 2 (shape knobs)
├── IntegralFluxEclipse2Knob × 4 (attenuverter knobs)
├── GoldButton × 2
├── MediumLight × 8
├── Magitek2InputJack × 14
└── Magitek2OutputJack × 11
```

### 3.2 `PreviewRecessFrameWidget` (lines 1713–1751)

Renders the inset well/border around each wave preview. Uses NanoVG:
- 1 filled rect (gradient)
- 3 stroked paths (border highlight/shadow)

**Key optimization already applied**: Wrapped in a `FramebufferWidget` with `dirtyOnSubpixelChange = false` (line 1756). This means the recess frame draws **once** and is cached as a texture. Zero per-frame cost. Good.

### 3.3 `WavePreviewWidget` — The Core Rendering Cost

This is the most performance-sensitive UI element. Two instances (CH1 + CH4). Supports dual render backends.

#### 3.3.1 `step()` (lines 1524–1596) — Runs ~60 Hz

1. Reads preview state atomics from audio thread (lock-free).
2. If version changed, calls `rebuildPoints()`:
   - Builds two segment LUTs (`buildSegmentLut()`, 512 entries each) — calls `slopeWarp()` ×1024 total via midpoint integration.
   - Maps 128 points into pixel coordinates.
   - Captures old curve into tracer history.
3. In OpenGL mode: calls `setDirty()` + `FramebufferWidget::step()`.

**Cost concern**: `buildSegmentLut()` calls `slopeWarp()` 512 times per segment × 2 segments = **1024 warp evaluations per preview rebuild**. This only runs on version change (gated to ~60 Hz max), but if both channels rebuild simultaneously during knob twiddling, that's 2048 warp evaluations per UI frame. Each warp is just `1/(1+k*x²)` or `1+k*x²` — fast, but not free at that count.

#### 3.3.2 NanoVG Draw Path (lines 1598–1696) — Default

When `previewRenderMode == 0`:

1. **Tracer trails** (if enabled):
   - Curve cache mode: iterates up to 11 trail frames, each calling `wave_preview::simplifyPath()` + NanoVG `moveTo`/`lineTo`/`stroke`. With stride=2 and 128 points, each trail emits ~64 vertices after simplification.
   - Frame cache mode: Uses `WavePreviewBufferedTracer` — rasterizes trails into a CPU pixel buffer, uploads to NanoVG image texture. Heavier capture but lighter draw.

2. **Main waveform**: Calls `wave_preview::simplifyPath()` on 128 points with tolerance 0.02. The simplifier does slope-based decimation and typically emits 30–60 vertices for smooth curves, more for sharp corners.

3. **Dot marker** (when visible): Two NanoVG circles — one fill, one stroke. ~26 vertices each.

4. **Frequency label**: Single `nvgText()` call.

**Per-frame NanoVG path budget (one preview, tracers on)**:
- Up to 11 trail strokes × ~32 vertices each = ~352 vertices
- 1 main curve stroke × ~60 vertices = ~60 vertices  
- 2 dot circles × ~26 = ~52 vertices
- Total: **~464 NanoVG vertices per preview, ~928 for both channels**

This is moderate. The main concern is that **each trail is a separate stroke call** — NanoVG state changes (stroke color, begin/end path) are relatively expensive compared to vertex throughput.

#### 3.3.3 OpenGL Draw Path (lines 1377–1420) — Optional

When `previewRenderMode == 1`:

1. Sets up orthographic projection.
2. Trails: `drawGlRibbon()` per frame — `GL_TRIANGLE_STRIP`, 128/stride=2 = ~64 quads per trail.
3. Main curve: `drawGlRibbon()` with stride=1 — 128 quads.
4. Dot: Two `GL_TRIANGLE_FAN` circles, 24 segments each.

**Performance note**: Uses immediate-mode OpenGL (`glBegin`/`glEnd`), which is deprecated but functional. Ribbon normal computation calls `std::sqrt()` per vertex (line 1302). With 128 points:
- Main ribbon: 128 sqrt calls
- Each trail: ~64 sqrt calls × up to 11 trails = 704 sqrt calls worst case
- **Total: ~832 sqrt calls per preview per frame** in the worst case

The `ribbonNormal()` function (lines 1288–1304) does:
```cpp
const float invLen = 1.f / std::sqrt(len2);
```
This is a classic candidate for `__builtin_sqrtf` or an RSQRT approximation, but at ~60 Hz frame rate this is unlikely to be a bottleneck.

#### 3.3.4 `drawGlDot()` (lines 1328–1375)

Draws position dot in GL mode. Uses `std::cos` and `std::sin` in a loop of 25 iterations × 2 fans = **100 trig calls per dot per frame**. These could be precomputed into a static vertex ring. Minor but unnecessary.

### 3.4 Custom Knob Draw Wrappers

Three knob wrapper types add performance instrumentation:

| Type | Base Class | What It Measures |
|------|-----------|-----------------|
| `IntegralFluxGearKnob` (line 1764) | `BigClockworkGearKnob` | `gIntegralFluxGearDrawNsThisFrame` |
| `IntegralFluxHalo2Knob` (line 1774) | `LeviathanHaloKnob2` | Same counter (shared) |
| `IntegralFluxEclipse2Knob` (line 1794) | `Eclipse2Knob` | `gIntegralFluxEclipseDrawNsThisFrame` |

Each wrapper measures the base class `draw()` time via `std::chrono::steady_clock::now()`. The EMA smoothing in the parent `IntegralFluxWidget::draw()` (lines 2032–2055) provides the telemetry.

**Note**: `IntegralFluxGearKnob` is defined but **never instantiated** in the widget constructor. The rise/fall and curve knobs use `IntegralFluxHalo2Knob` / `IntegralFluxCurveHalo2Knob` instead. The gear knob type appears to be vestigial from an earlier visual design. Its draw timer contribution is non-zero only if `BigClockworkGearKnob` was previously used — currently the timing counter conflates gear + halo knob draw.

**Eclipse shadow tracking**: Lines 2050–2055 separately track `visual_assets::eclipseShadowDrawCount()` and `visual_assets::eclipseShadowDrawNs()`. With 4 eclipse knobs, each potentially drawing a shadow, this is worth monitoring.

### 3.5 `IntegralFluxWidget::step()` (line 1812)

Measures `ModuleWidget::step()` time. This cascades into all child `step()` calls including both `WavePreviewWidget::step()` instances. The EMA (`uiStepMsEma`) feeds into debug terminal telemetry.

### 3.6 `IntegralFluxWidget::draw()` (line 2032)

1. Resets per-frame timing counters.
2. Calls `ModuleWidget::draw()` (cascades into all children).
3. Computes draw EMA.
4. Publishes combined `uiMs = step + draw` to `perfUiRenderMs` atomic (read by debug terminal).
5. If debug enabled: submits telemetry at 8 Hz + renders debug instance ID label (2 NanoVG text calls).

The debug HUD (lines 2071–2085) is behind `isDragonKingDebugEnabled()` and adds 2 `nvgText()` calls when active. Negligible.

---

## 4. Cross-Thread Data Flow

### 4.1 Audio → UI (Lock-Free)

| Data | Mechanism | Rate |
|------|-----------|------|
| Preview waveform shape | `PreviewSharedState` (6 atomics + version counter) | ~60 Hz publish |
| Preview dot position | `dotXNorm` / `dotYNorm` / `dotVisible` atomics | ~120 Hz publish |
| Performance counters | `perfAudioProcessNs`, `perfAudioSampledCount` | Accumulated, read at 8 Hz |

All cross-thread communication uses `std::memory_order_relaxed` atomics. This is correct for this use case — the UI side tolerates stale reads since it's purely cosmetic. The version-number gating (`version != lastVersion`) prevents unnecessary `rebuildPoints()` calls when nothing changed.

### 4.2 UI → Audio (Lock-Free)

| Data | Mechanism |
|------|-----------|
| Timing update divider | `requestedTimingUpdateDiv` atomic |
| Feature toggles | `bandlimitedGateOutputs`, `bandlimitedSignalOutputs`, `timingInterpolate`, etc. |
| Preview render mode | `previewRenderMode`, `previewTracerEnabled`, `previewTracerCacheMode` |

All atomics with relaxed ordering. Correct for configuration data.

---

## 5. Rendering Performance Assessment

### 5.1 What's Already Well-Optimized

1. **Preview recess frame** — cached in framebuffer, zero per-frame cost.
2. **Preview rebuild gating** — version-stamped, only rebuilds on actual parameter change.
3. **Tracer capture rate-limiting** — `TRAIL_MIN_CAPTURE_INTERVAL_SEC = 1/24` prevents excessive trail accumulation.
4. **Path simplification** — `wave_preview::simplifyPath()` reduces NanoVG vertex count by skipping collinear points.
5. **Mix bus skip** — only computed when outputs connected or light tick fires.
6. **Light decimation** — 120 Hz, not audio rate.
7. **Dot publish throttle** — 120 Hz max.

### 5.2 Remaining Concerns — Ranked

#### P1: NanoVG Trail Stroke Count

With tracers enabled (default on) and curve-cache mode (default), up to **11 separate NanoVG stroke calls per preview** per frame. Each stroke call triggers NanoVG's internal tessellator and GPU draw call. At 2 previews × 11 trails = **22 stroke calls** worst-case.

**Mitigation options**:
- Already available: `TRAIL_DRAW_STRIDE = 2` halves trail vertex count.
- Already available: user can disable tracers via context menu.
- Possible: merge all trails into a single NanoVG path with alpha-varied segments (reduces draw calls from 11 to 1, but complicates color variation).
- Possible: switch to frame-cache mode by default, which rasterizes trails into a single texture blit.

#### P2: `buildSegmentLut()` Per-Rebuild Cost

Each preview rebuild computes 2 × 512-entry LUTs via midpoint integration. The `slopeWarp()` function is arithmetic-only (no transcendentals), so this is fast, but it's still **1024 function calls + 1024 clamp operations** per channel per rebuild. With knob twiddling, both channels could rebuild every frame.

**Mitigation**: The LUT could be cached per (shapeSigned, rising) pair and reused until shape actually changes. The version bump already guards against unnecessary rebuilds, but within a single rebuild the two LUTs are always recomputed even if only rise/fall time changed (not the curve shape).

#### P3: GL Ribbon `std::sqrt` Per Vertex

The `ribbonNormal()` function computes a square root per vertex. For main curve + 11 trails, worst case is ~832 sqrt calls per preview. At ~60 Hz × 2 previews = ~100K sqrt/sec. This is within CPU budget but not free.

**Mitigation**: `fast_rsqrt()` approximation or precompute normals only when points change (currently normals are recomputed every `drawFramebuffer()` call even if points haven't moved).

#### P4: GL Dot Trig Calls

`drawGlDot()` computes `cos`/`sin` 50 times per dot per frame. These could be replaced with a static 25-vertex unit circle table scaled at draw time.

#### P5: `injectAlphaBase` `std::exp` Per Sample

Line 1063 computes `std::exp(-sampleTime / TAU)` every sample. Since sample rate changes are extremely rare, this should be cached at `onSampleRateChange()` or similar.

#### P6: Buffered Tracer Pixel Fade Loop

When using frame-cache tracer mode, `WavePreviewBufferedTracer::fade()` iterates over **all pixels** in the raster buffer every draw frame (lines 197–216 of `WavePreviewTracer.hpp`). With `rasterScale = 2.0` and a ~60×34px widget, that's ~8160 pixels with per-pixel bit manipulation (`>> 24`, `& 0xff`, multiply, shift). Not catastrophic at 60 Hz but adds up — especially since this runs even when the tracer trails are nearly invisible.

#### P7: `dynamic_cast` Per Frame

`IntegralFluxWidget::draw()` at line 2039 uses `dynamic_cast<IntegralFlux*>(module)` every frame. Since the widget constructor guarantees the module type, a `static_cast` would be safe and avoid the RTTI overhead (~20–40ns per cast on Linux).

#### P8: Signal Injection `std::tanh` Per Sample

When a signal is patched to CH1 or CH4 input, the `softClamp8()` call at line 847 (`clamp((inSoft - OUTER_V_MIN) / range, 0.f, 1.f)` preceded by `softClamp8(signalIn)`) invokes `std::tanh` **every sample per channel** — this is NOT gated by `timingTick`. With both channels receiving signal, that's 2× `std::tanh` per sample (~40–80ns). A polynomial approximation (e.g., `x * (27 + x²) / (27 + 9x²)`) would eliminate this.

### 5.3 Telemetry Coverage

The module's performance instrumentation is thorough:

| Metric | What It Tracks | Where Reported |
|--------|----------------|----------------|
| `uiStepMsEma` | Total `step()` cost | Debug terminal `UI ms` |
| `uiDrawMsEma` | Total `draw()` cost | Debug terminal `UI ms` (summed) |
| `gearDrawUsEma` | Halo knob draw cost | Debug terminal |
| `eclipseDrawUsEma` | Eclipse knob draw cost | Debug terminal |
| `eclipseShadowDrawUsEma` | Eclipse shadow overlay cost | Debug terminal |
| `perfAudioProcessNs` / `perfAudioSampledCount` | Per-sample audio cost | Debug terminal `Aud us` |

This is excellent — the instrumentation makes before/after measurement of any optimization trivial.

---

## 6. Comparison with Other Modules

The Integral Flux rendering architecture is significantly simpler than, e.g., Wyrm (which has an always-live waveform editor, sand simulation, and multi-backend rendering). The key differences:

| Aspect | Integral Flux | Wyrm |
|--------|--------------|------|
| Live-rendered widgets | 2 small previews | 1 large editor |
| Per-frame vertex budget | ~928 NVG / ~256 GL | Thousands (columns + body) |
| Framebuffer caching | Preview recess only | Multiple layers |
| Audio→UI data volume | ~20 bytes atomic | ~kilobytes (waveform data) |
| Backend options | NanoVG or GL | 4 sand backends |

Integral Flux's UI cost should be **modest** in absolute terms. The DSP is the historically expensive side, and most DSP optimizations from the performance plan have already landed.

---

## 7. Actionable Recommendations

### Quick Wins (Low Risk)

| # | Change | Expected Gain | Effort |
|---|--------|---------------|--------|
| 1 | Cache `injectAlphaBase` on sample rate change instead of computing `std::exp()` every sample | Eliminates ~48K exp/sec per module | Trivial |
| 2 | Precompute static GL dot circle vertices (25-entry unit circle table) | Eliminates ~6K trig calls/sec per module | Trivial |
| 3 | Guard `buildSegmentLut()` so rise/fall LUTs only recompute when `curveSigned` actually changed (not just when version bumps) | Halves LUT rebuild frequency during rise/fall-only sweeps | Easy |
| 4 | Replace `dynamic_cast<IntegralFlux*>` with `static_cast` in `IntegralFluxWidget::draw()` | Eliminates RTTI overhead (~20–40ns/frame) | Trivial |

### Medium Wins (Moderate Risk)

| # | Change | Expected Gain | Effort |
|---|--------|---------------|--------|
| 5 | Default tracer to frame-cache mode (single texture blit vs. 11 NanoVG strokes) | Reduces per-preview draw calls from ~12 to ~2 | Tuning/validation |
| 6 | Use RSQRT approximation in `ribbonNormal()` for GL path | ~4× faster normal computation | Easy, may affect visual smoothness |
| 7 | Remove `IntegralFluxGearKnob` type — it's defined but never instantiated, and its draw counter is conflated with halo knob timing | Cleaner code, clearer telemetry | Trivial |
| 8 | Replace signal injection `std::tanh` with polynomial approximation | Eliminates per-sample transcendental when signal patched | Easy, verify soft-clamp accuracy |

### Structural (Higher Effort)

| # | Change | Expected Gain | Effort |
|---|--------|---------------|--------|
| 9 | Move preview LUT build to a background task or cache globally per (shape, rising) pair | Eliminates frame stalls during rapid knob sweeps | Medium |
| 10 | Consider making OpenGL the default preview renderer and removing NanoVG fallback | GL ribbon is inherently cheaper than NanoVG tessellation for line strips | Validation |

---

## 8. Status of Prior Optimization Recommendations

Cross-referencing `doc/flux/codex-performance.md`, `doc/flux/gemini-performance.md`, and `doc/flux/unified_perf_plan.md`:

| Prior Recommendation | Current Status |
|---------------------|----------------|
| Cache timing constants | ✅ Done — epsilon-based dirty checks + decimated `timingTick` |
| Replace `std::pow(2,x)` with fast approx | ✅ Done — `exp2_taylor5` throughout |
| Knob taper LUT | ✅ Done — 4096-entry `knobCurveLut` |
| Fast `tanh` or removal | ✅ Done — mix saturation removed; only `softClamp8` remains, gated by timing tick |
| BLEP toggle | ✅ Done — separate toggles for gates and signals |
| Control-rate decimation | ✅ Done — configurable `/1` to `/32` with interpolation |
| SIMD for CH1+CH4 | ❌ Not implemented — would require structural refactor |
| LUT for `slopeWarp()` | ❌ Not implemented — warp is now `1+k*x²` (arithmetic only), LUT would add complexity for marginal gain |
| `dsp::ClockDivider` | ⚡ Implemented differently — manual counter + tick pattern instead of Rack's ClockDivider class |

---

## 9. Summary

Integral Flux's rendering pipeline is already in good shape. The major DSP optimizations from earlier audits have landed. The remaining rendering costs are:

1. **NanoVG tracer strokes** — moderate, user-toggleable, could default to frame-cache mode.
2. **Preview LUT rebuilds** — bounded by version gating, could be further refined.
3. **Minor per-sample residuals** — `std::exp` in inject alpha, cacheable.
4. **GL-mode sqrt/trig** — functional but not optimally tight.

The module's built-in telemetry system makes it straightforward to measure the impact of any change. The cross-thread data flow is clean, lock-free, and correctly ordered for the use case.

No critical performance defects were identified. The recommendations above are refinements, not fixes.
