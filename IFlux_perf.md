Dragon King Leviathan, yes — Integral Flux looks *much healthier* than an early-pass module. The big wins are already present: knob taper LUT, stage-time caching, optional timing-update division, optional MinBLEP, unified CH1/CH4 processing, light-rate limiting, and preview state handoff through relaxed atomics. The remaining opportunities are mostly “death by small cuts” in the audio thread plus one surprisingly large UI/audio boundary issue.

## Executive take

I would prioritize these in this order:

| Priority | Optimization                                                              |                                     Expected payoff |     Risk |
| -------: | ------------------------------------------------------------------------- | --------------------------------------------------: | -------: |
|        1 | Cache atomic performance flags once per `process()` call                  |                          Small-to-medium, always-on | Very low |
|        2 | Rate-limit preview dot publishing instead of atomic stores every sample   |             Medium-to-high, especially many modules |      Low |
|        3 | Stop calling preview update logic every sample                            |                                              Medium |      Low |
|        4 | Cache signal-injection `exp()` coefficient                                | Medium when signal input is patched during FG/cycle | Very low |
|        5 | Reduce preview widget point/LUT sizes and cache formatted frequency text  |                                       Medium UI win |      Low |
|        6 | Add a fast idle path for unpatched/non-cycling outer channels             |                             Medium in quiet patches |   Medium |
|        7 | Precompute constants and remove duplicate divisions in FG rise/fall paths |                                               Small | Very low |
|        8 | Optional: specialize linear-shape slew/FG path                            |                     Small-to-medium depending usage |   Medium |

The only thing I’d call a true “missed optimization” rather than refinement is the preview dot publishing: the engine currently stores dot X, Y, and visibility atomically every sample for both preview widgets. That is six atomic stores per audio sample even though the UI only needs them at display rate, and the dot is hidden above a few Hz anyway. The shared preview state is well-designed, but that particular handoff is too hot.

## 1. Atomic flags: load once, pass locals through the hot path

You already store performance toggles as atomics, which is appropriate because the UI menu mutates them while audio is running. But the audio path often reads `bandlimitedSignalOutputs`, `bandlimitedGateOutputs`, and `timingInterpolate` directly inside conditional expressions. Even if this compiles cleanly, direct atomic conversion is not the same as your explicit relaxed-load intent, and it repeats those loads in hot branches.

**Codex target:**

At the top of `process()`:

```cpp
const bool blSignal = bandlimitedSignalOutputs.load(std::memory_order_relaxed);
const bool blGate = bandlimitedGateOutputs.load(std::memory_order_relaxed);
const bool interpTiming = timingInterpolate.load(std::memory_order_relaxed);
```

Then pass those into `processOuterChannel(...)`, and use them for final BLEP processing too:

```cpp
ch1Result = processOuterChannel(args, ch1, ch1Cfg, previewCh1, previewUpdateCh1, timingTick, blSignal, blGate, interpTiming);
ch4Result = processOuterChannel(args, ch4, ch4Cfg, previewCh4, previewUpdateCh4, timingTick, blSignal, blGate, interpTiming);

float ch1OutRendered = ch1.out + (blSignal ? ch1.signalBlep.process() : 0.f);
float ch4OutRendered = ch4.out + (blSignal ? ch4.signalBlep.process() : 0.f);
float eorOut = (ch1.gateState ? 10.f : 0.f) + (blGate ? ch1.gateBlep.process() : 0.f);
float eocOut = (ch4.gateState ? 10.f : 0.f) + (blGate ? ch4.gateBlep.process() : 0.f);
```

This is not glamorous, but it aligns the code with its own performance architecture.

## 2. Preview dot atomics are too hot

Right now `publishPreviewDot()` does three relaxed atomic stores, and `process()` calls it for CH1 and CH4 every sample. That means at 48 kHz you are doing about **288,000 atomic stores per second per module** just for preview dot position. Meanwhile the widget hides the dot above roughly 2–2.4 Hz, so most of those stores are meaningless at audio-rate cycle speeds.

**Recommendation:** add a separate preview-dot timer, probably 60 Hz or 120 Hz. Only publish dot state on that tick, plus immediately when phase becomes idle/active so visibility does not linger.

A clean structure:

```cpp
static constexpr float PREVIEW_DOT_INTERVAL = 1.f / 60.f;
float previewDotTimer = 0.f;
bool previewDotTick = false;
```

In `process()`:

```cpp
previewDotTimer += args.sampleTime;
if (previewDotTimer >= PREVIEW_DOT_INTERVAL) {
    previewDotTimer -= PREVIEW_DOT_INTERVAL;
    if (previewDotTimer >= PREVIEW_DOT_INTERVAL)
        previewDotTimer = 0.f;
    previewDotTick = true;
}
```

Then only compute `computeDotX()` and call `publishPreviewDot()` when `previewDotTick` is true, or when a cached `lastDotVisible` changes. This also removes two divisions per channel per sample from `computeDotX()`.

This is my highest-confidence meaningful gain.

## 3. Preview update logic should not run every sample

`updatePreviewChannel()` is well-defended against excessive publishing, but it is still called every audio sample per outer channel. Inside it, you check knob deltas, update timers, compute relative differences, and possibly publish state. The actual preview curve only needs UI/control-rate updates.

**Recommendation:** call `updatePreviewChannel()` only on a preview tick, not every sample.

You can share the same `previewTick` as the dot update, or use separate rates:

```cpp
static constexpr float PREVIEW_STATE_INTERVAL = 1.f / 30.f;
static constexpr float PREVIEW_DOT_INTERVAL = 1.f / 60.f;
```

I would use **30 Hz for curve/state** and **60 Hz for the moving dot**. Manual knob interaction will still feel immediate enough, especially because Rack UI itself is not audio-rate.

This reduces per-sample preview cost without changing the actual DSP.

## 4. Cache the signal-injection exponential

When signal input is patched while the outer channel is actively generating, you compute:

```cpp
float a = 1.f - std::exp(-dt / OUTER_INJECT_TAU);
injectAlpha = OUTER_INJECT_GAIN * clamp(a, 0.f, 1.f);
```

That value depends only on sample time and constants, not the signal. So during cycling/FG with a patched signal, you pay for `std::exp()` every sample unnecessarily.

**Recommendation:** compute once per `process()` call, or cache when sample rate changes:

```cpp
float injectAlphaBase = OUTER_INJECT_GAIN * clamp(
    1.f - std::exp(-args.sampleTime / OUTER_INJECT_TAU),
    0.f,
    1.f
);
```

Then inside the channel:

```cpp
injectAlpha = injectAlphaBase;
```

Even better, use `std::expf` if available/appropriate for float math:

```cpp
1.f - std::expf(-dt / OUTER_INJECT_TAU)
```

This is an easy win and should be behavior-identical except for tiny float/double differences.

## 5. The UI preview is oversampled for its physical size

The preview widget uses `POINT_COUNT = 320` and `PREVIEW_LUT_SIZE = 1024`. On every curve rebuild, it builds two 1024-entry LUTs and samples 320 points. On every draw, it emits 320 NanoVG line segments per preview widget. For two previews, that is 640 line segments per frame before dots/text.

For a tiny preview box, 320 points is almost certainly excessive.

**Recommendation:**

```cpp
static constexpr int POINT_COUNT = 128;
static constexpr int PREVIEW_LUT_SIZE = 512;
```

If that still looks identical, try:

```cpp
static constexpr int POINT_COUNT = 96;
static constexpr int PREVIEW_LUT_SIZE = 256;
```

Because the widget is small and the curve is smooth, this should survive visually. The Culture-grade move is to make this adaptive:

```cpp
POINT_COUNT ≈ clamp(int(box.size.x * 1.5f), 64, 160)
```

But fixed 128 is simpler and likely good.

Also, `draw()` formats frequency text with `std::snprintf()` every frame. Cache the rendered `freqText` in `step()` when `lastFreqHz` changes meaningfully or when preview version changes.

## 6. Cache the module pointer in `WavePreviewWidget`

Each preview widget does an ancestor lookup every `step()`:

```cpp
if (ModuleWidget* moduleWidget = getAncestorOfType<ModuleWidget>()) {
    modulePtr = moduleWidget->getModule<IntegralFlux>();
}
```

This is UI-thread work, not audio-thread work, but it is unnecessary. You construct the preview widgets from `IntegralFluxWidget`, where the module pointer is already available.

**Recommendation:**

```cpp
struct WavePreviewWidget : Widget {
    IntegralFlux* modulePtr = nullptr;

    WavePreviewWidget(IntegralFlux* module, int channel) {
        modulePtr = module;
        this->channel = channel;
    }
};
```

Then:

```cpp
WavePreviewWidget* ch1Preview = new WavePreviewWidget(module, 1);
WavePreviewWidget* ch4Preview = new WavePreviewWidget(module, 4);
```

Tiny win, clean code.

## 7. Fast idle path for inactive outer channels

Even when an outer channel is idle, unpatched, and not cycling, `processOuterChannel()` still reads rise/fall/shape params and CV inputs, checks timing cache dirty state, updates preview logic, computes shape state, and only later discovers there is no signal work to do.

This is defensible because preview needs to reflect knobs/CV, but once preview is moved to control-rate, the audio path can bail earlier.

**Pattern:**

After reading only the things needed to detect activity:

```cpp
bool signalPatched = inputs[cfg.signalInput].isConnected();
bool active = ch.phase != OUTER_IDLE || cycleOn || signalPatched || trigAccepted;
bool needControlRefresh = timingTick || previewTick;

if (!active && !needControlRefresh) {
    if (ch.gateState) {
        ch.gateState = false;
    }
    ch.slewDir = 0;
    ch.out = 0.f;
    return {cycleOn};
}
```

Then only read rise/fall/shape/CV when either active or control-refreshing.

This could matter a lot in large patches where Integral Flux instances are present but not always actively slewing/cycling.

## 8. Remove duplicate divisions in rise/fall integration

In both RISE and FALL you calculate `dt / riseTime` or `dt / fallTime` twice: once as `dpPhase`, then again as `dp`. Use the same value and clamp only for the slope step.

Change:

```cpp
float dpPhase = dt / riseTime;
ch.phasePos += dpPhase;
float dp = clamp(dt / riseTime, 0.f, 0.5f);
```

to:

```cpp
float dpPhase = dt / riseTime;
ch.phasePos += dpPhase;
float dp = std::min(dpPhase, 0.5f);
```

Same for fall.

Also define:

```cpp
static constexpr float OUTER_RANGE = OUTER_V_MAX - OUTER_V_MIN;
static constexpr float OUTER_RANGE_INV = 1.f / OUTER_RANGE;
```

Then stop recomputing `OUTER_V_MAX - OUTER_V_MIN` in hot paths. It appears in slew, active FG, preview-dot scaling, and phase re-anchor logic.

## 9. Function-local static configs: tiny guard overhead

`ch1Cfg` and `ch4Cfg` are function-local `static const` objects inside `process()`, initialized with `std::log2(...)`. This is not catastrophic, but it may introduce a thread-safe static initialization guard check in the process path.

**Recommendation:** precompute these constants:

```cpp
static constexpr float OUTER_LOG_SHAPE_SCALE_LOG2 = 2.64385619f;  // log2(6.25)
static constexpr float OUTER_EXP_SHAPE_SCALE_LOG2 = -1.f;         // log2(0.5)
```

Then either keep the local statics with constexpr-only initialization or move configs to member/static constants. This is micro-optimization, but easy.

## 10. Gate BLEP can be connection-aware

You already made gate BLEP optional and defaulted it off, which is probably correct. But if enabled, it will still do transition insertion and per-sample `gateBlep.process()` even if EOR/EOC outputs are not connected. The lights only need `gateState`, not the BLEP-rendered gate voltage.

**Recommendation:** treat BLEP as active only when the corresponding gate output is connected:

```cpp
bool ch1GateBlepActive = blGate && outputs[EOR_1_OUTPUT].isConnected();
bool ch4GateBlepActive = blGate && outputs[EOC_4_OUTPUT].isConnected();
```

Pass per-channel booleans into `processOuterChannel()`. Use raw gate state for lights regardless.

This is low priority because gate BLEP is default false, but it makes the option safer in heavy patches.

## 11. Constructor-time SVG parsing is probably okay, but cache if patch load feels slow

The widget constructor calls `panel_svg::loadPointFromSvgMm()` many times for individual element IDs. If that helper reparses the SVG file per call, module creation will be much slower than necessary. This does not affect audio performance after construction, but it can affect patch load and module duplication.

**Recommendation:** only optimize this if patch load feels sluggish:

1. Parse `flux.svg` once into a point/rect cache.
2. Or generate final positions into C++ and keep SVG lookup only behind a dev flag.
3. Or modify `PanelSvgUtils` to cache parsed SVG documents by path.

The current approach is developer-friendly. I would not touch it unless load-time profiling points here.

## What I would ask Codex to implement first

```markdown
# Integral Flux performance cleanup pass

Target file: IntegralFlux.cpp

Goals:
1. Preserve audio behavior.
2. Reduce audio-thread overhead from atomics and preview handoff.
3. Keep all existing context-menu options and patch compatibility.

Tasks:
- Load bandlimitedSignalOutputs, bandlimitedGateOutputs, and timingInterpolate once per process() using memory_order_relaxed.
- Pass those bools into processOuterChannel() instead of reading atomics inside channel processing.
- Add preview state and preview dot timers in IntegralFlux.
- Call updatePreviewChannel() only on preview-state ticks, not every sample.
- Publish preview dots only on preview-dot ticks, and publish immediate false/visibility changes on phase idle transitions if necessary.
- Cache signal injection alpha from sampleTime once per process() or per sample-rate change.
- Replace duplicate dt/riseTime and dt/fallTime divisions with reused dpPhase.
- Add constexpr OUTER_RANGE and OUTER_RANGE_INV.
- Reduce WavePreviewWidget POINT_COUNT from 320 to 128 and PREVIEW_LUT_SIZE from 1024 to 512; verify visual parity.
- Pass IntegralFlux* directly into WavePreviewWidget constructor and remove getAncestorOfType() from step().
- Cache frequency label text in WavePreviewWidget instead of snprintf every draw.
```

## Bottom line

Integral Flux is already in “competent optimized Rack module” territory. The remaining dragon-gold is mostly in **not letting UI preview concerns leak into the audio thread**. The two biggest changes I’d make are:

1. **Move preview state/dot publishing to control-rate.**
2. **Use local relaxed-loaded booleans for atomic performance flags.**

After that, the next gains are cleanups: cached injection alpha, smaller preview geometry, fewer duplicate divisions, and an optional idle fast path. These should make the module feel more scalable without destabilizing the carefully tuned Maths-like behavior.
