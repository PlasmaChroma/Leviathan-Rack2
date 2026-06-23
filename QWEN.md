# Leviathan Rack Plugin Suite - Agent Context Primer

See also (other files):
- `Agents.md`
- `vcv-coding.md`

---

## Agent Operating Protocol

This repository is a real VCV Rack 2 plugin suite. Do not rely on general memory when exact local facts are available. The source tree, compiler errors, tests, existing code patterns, and Rack SDK headers are the source of truth.

### Mandatory Behavior

Before making non-trivial edits:

1. Inspect the relevant files first.
2. Identify the exact symbols, classes, params, ports, lights, widgets, assets, or JSON keys involved.
3. State the smallest viable change.
4. Prefer existing repository patterns over new architecture.
5. Do not invent Rack APIs, helper functions, build flags, filenames, module IDs, asset paths, or enum names.
6. If unsure whether something exists, search for it before using it.

While editing:

1. Make surgical patches.
2. Do not mix DSP, UI/layout, asset, serialization, and build-system changes unless explicitly requested.
3. Preserve released-module compatibility.
4. Preserve enum ordering for existing params, inputs, outputs, lights, and JSON persistence keys.
5. Avoid broad rewrites unless explicitly requested.

After editing:

1. Run the relevant test/build command when available.
2. Quote exact compiler/test errors.
3. Fix from evidence, not memory.
4. If blocked, report the specific blocker and the files inspected.

### Change Categories

Classify each task before editing:

- DSP-only
- UI/layout-only
- asset-only
- serialization-only
- build-system-only
- test-only
- documentation-only
- integration/refactor

Do not combine categories unless the user explicitly asks for an integrated change.

### Released Module Safety

Integral Flux, Proc, Temporal Deck, TD.Scope, and Undertow are released modules. For these:

- Do not reorder existing `ParamIds`, `InputIds`, `OutputIds`, or `LightIds`.
- Do not rename model slugs, module IDs, asset paths, or JSON keys.
- Do not change existing patch behavior casually.
- Append new enum values instead of inserting into existing enum order.
- Treat serialization and user patch compatibility as sacred.

### Evidence Over Assumption

Never invent:

- Rack API calls
- llama.cpp flags
- helper functions
- source filenames
- SVG anchor labels
- asset paths
- module IDs
- enum names
- JSON keys
- build targets

Use grep, local files, tests, build logs, and existing nearby code as ground truth.

### Mandatory First Response Format for Non-Trivial Coding Tasks

For non-trivial coding tasks, start with:

1. Files I need to inspect:
2. Existing patterns I will follow:
3. Risk category:
4. Planned smallest patch:
5. Build/test command I will run:

Do not edit until the relevant files have been inspected.

---

## Overview

The Leviathan plugin suite is a collection of VCV Rack 2 modules focused on experimental sound design, synthesis, and modular control. All modules share a common visual identity (dragon-themed aesthetic, cyan/purple color scheme) and a set of shared infrastructure utilities for graphics, panel positioning, and debugging.

**Current Released Modules:**
- **Integral Flux** - Dual slope generator with variable curve slew, attenuverters, signal mixing, and full audio-rate fidelity
- **Proc** - Generator, slew limiter, and envelope follower
- **Temporal Deck** - Virtual turntable with modern hybrid control
- **TD.Scope** - Expander: Waveform display for Temporal Deck
- **Undertow** - Compact sub-harmonic oscillator

**Current Unreleased Modules:**
- **Crownstep** - Checkers-driven sequencer (game rules: checkers, chess, Othello)
- **Bifurx** - Dual-peak multimode filter with live response and spectrum preview
- **Wyrm** - Custom drawable wavetable oscillator with FM, sync, and fold
- **Sil** - Automatic mastering module
- **Chronomaw** - Rack-native eight-output clock and modulation engine
- **Bulkhead** - Spatial room reverb

---

## Source File Summary

### Core Infrastructure

| File | Purpose |
|------|---------|
| `plugin.hpp` / `plugin.cpp` | Plugin initialization, module registration, runtime feature flags (dragonking debug mode), module teardown timing |
| `DebugTerminalTransport.hpp` / `DebugTerminalTransport.cpp` | Transport layer for sending performance metrics and debug data to external debug terminal server |
| `PanelSvgUtils.hpp` / `PanelSvgUtils.cpp` | Parse SVG panels to extract anchor points (circles/rects) for dynamic component placement, handles SVG transforms and gradients |
| `PanelAnchorAtlas.hpp` / `PanelAnchorAtlas.cpp` | Caching layer for panel anchor lookups to avoid repeated SVG parsing |
| `VisualAssets.hpp` / `VisualAssets.cpp` | Custom UI widgets: Knobs (Gear, Eclipse, Halo), Jacks (Magitek), Lights (LeviathanCyanPurple), Buttons (GoldButton), 3D SVG effects |
| `NvgGraphicsLifecycle.hpp` / `NvgGraphicsLifecycle.cpp` | Shared NanoVG context lifecycle management, cached image invalidation on context switch |
| `GlLifecycleUtils.hpp` / `GlLifecycleUtils.cpp` | Shared OpenGL resource validation helpers for program/buffer and texture/framebuffer pairs |

### Common Patterns / Utility Headers

| File | Purpose |
|------|---------|
| `MathHelpers.hpp` | Fast math utilities, clamp/wrap functions, common constants |
| `codec.hpp` | Audio codec utilities |
| `SilRepairBuffer.hpp` / `SilRepairKernel.hpp` | Sil module signal repair algorithms |
| `WavePreviewTracer.hpp` / `WavePreviewSimplifier.hpp` | Waveform tracing and simplification utilities |
| `TemporalDeckTest.hpp` | Test infrastructure for Temporal Deck |
| `BifurxWorker.hpp` / `BifurxRenderData.hpp` / `BifurxRenderPrep.hpp` | Bifurx visualization rendering pipeline |

---

## Module-Specific Files

### Temporal Deck (Released)
| File | Purpose |
|------|---------|
| `TemporalDeck.hpp` | Module class declaration with all parameters, inputs, outputs, lights, and public API |
| `TemporalDeck.cpp` | Main module implementation |
| `TemporalDeckEngine.hpp` | Core signal processing engine (separate for unit testing) |
| `TemporalDeckPlatterInput.cpp` | Platter scratch/touch handling |
| `TemporalDeckFrameInput.cpp` | Frame input processing |
| `TemporalDeckTransportControl.cpp` | Transport state machine (play/stop/loop) |
| `TemporalDeckSampleLifecycle.cpp` | Sample loading, saving, live conversion |
| `TemporalDeckSamplePrep.cpp` | Sample preparation and buffer management |
| `TemporalDeckArcLights.cpp` | Arc light UI visualization |
| `TemporalDeckUI.cpp` | UI widget implementation |
| `TemporalDeckExpanderProtocol.hpp` | Protocol for TD.Scope expander communication |
| `TemporalDeckMenuUtils.hpp` | Menu helper utilities |

### Crownstep ( unreleased )
| File | Purpose |
|------|---------|
| `CrownstepCore.hpp` | Core game rules: checkers, chess, Othello with AI, move generation, board state |
| `CrownstepModule.cpp` | Module implementation with CV/gate output mapping |
| `CrownstepPlayback.cpp` | Sequence playback logic |
| `CrownstepSerialization.cpp` | Patch state serialization |
| `CrownstepUI.cpp` | UI widget implementation |

### Integral Flux (Released)
| File | Purpose |
|------|---------|
| `IntegralFlux.cpp` | Module implementation (dual slope generators, mixing, slew limiting) |

### Proc (Released)
| File | Purpose |
|------|---------|
| `Proc.cpp` | Module implementation (function generator, slew limiter, envelope follower) |

### Wyrm (Unreleased)
| File | Purpose |
|------|---------|
| `Wyrm.hpp` | Wavetable oscillator with rocks (waveform sculpting), FM, sync, fold |
| `Wyrm.cpp` | Module implementation |
| `WyrmSand.hpp` / `WyrmSand.cpp` / `WyrmSandGL.cpp` | Sand visualization system (NanoVG and OpenGL backends) |
| `WyrmWaveEditor.cpp` | Wave editor widget |
| `WyrmWidget.cpp` | Main UI widget |

### Bifurx (Unreleased)
| File | Purpose |
|------|---------|
| `Bifurx.hpp` | Dual-peak multimode filter |
| `Bifurx.cpp` | Module implementation |
| `BifurxWorker.hpp` / `BifurxWorker.cpp` | Background visualization render worker |
| `BifurxRenderPrep.hpp` / `BifurxRenderPrep.cpp` | Render data preparation |
| `BifurxGL.cpp` | OpenGL rendering |
| `BifurxUI.cpp` | UI widget implementation |

### Chronomaw (Unreleased)
| File | Purpose |
|------|---------|
| `Chronomaw.hpp` | Clock and modulation engine with direct visual editing |
| `ChronomawWidget.cpp` | UI widget with timeline editor |
| `ChronomawEngine.cpp` | Core clock/modulation logic |
| `ChronomawClock.hpp` / `ChronomawTimeline.hpp` / `ChronomawState.hpp` | Internal state structures |

### Bulkhead (Unreleased)
| File | Purpose |
|------|---------|
| `Bulkhead.hpp` | Spatial room reverb |
| `BulkheadGeometry.cpp` / `BulkheadGeometry.hpp` | Room geometry and wall reflection calculations |
| `BulkheadWidget.cpp` | UI widget with interactive wall layout |

### Undertow (Released)
| File | Purpose |
|------|---------|
| `Undertow.hpp` / `Undertow.cpp` | Sub-harmonic oscillator |

### Sil (Unreleased)
| File | Purpose |
|------|---------|
| `Sil.cpp` | Automatic mastering |

### TDScope (Released - Expander)
| File | Purpose |
|------|---------|
| `TDScope.hpp` | Temporal Deck expander waveform display |
| `TDScopeWidget.cpp` | UI widget |
| `TDScopeGL.cpp` | OpenGL rendering |

---

## VCV Rack Execution Model

### Core Threading Rule

VCV Rack modules separate real-time DSP from UI rendering. Treat `Module::process()` as real-time audio/engine code and treat `ModuleWidget::step()` / `draw()` as UI-thread code.

| Area | Typical Methods | Rule |
|------|-----------------|------|
| Audio / engine | `Module::process()` | Real-time hot path. Must be deterministic, allocation-free, and non-blocking. |
| UI / widget | `ModuleWidget::step()`, `ModuleWidget::draw()` | Non-real-time, but still frequent. Avoid needless per-frame work. |
| Lifecycle / patch state | constructors, destructors, `onSampleRateChange()`, `dataToJson()`, `dataFromJson()` | Not normal sample-rate DSP, but still preserve thread safety and compatibility. |

**Important:** Assume audio/engine code and UI code can access the same module object concurrently. UI code must not directly read non-atomic state that `process()` writes. Use atomics, versioned snapshots, double buffering, or other explicit synchronization.

### Audio / Engine Code: `Module::process()`

`process()` is the real-time hot path. It should be written as if any unpredictable stall can cause audible dropouts.

**Allowed in `process()`:**
- Simple arithmetic and predictable branching
- Stack-local POD values
- Precomputed lookup tables
- Reading params/inputs and writing outputs/lights
- Lock-free atomics for small UI-visible state
- Fixed-size buffers allocated outside the hot path
- Rate-limited meter/light/debug state publication

**Avoid in `process()`:**
- Heap allocation: `new`, `malloc`, `std::vector::push_back`, etc.
- File I/O, network I/O, or OS calls that can block
- Logging, string formatting, JSON, filesystem checks
- Mutexes, locks, condition variables
- `std::unordered_map` lookups or data structures with unpredictable behavior
- Expensive debug timing unless protected by debug flags and rate limits
- Any operation that can block or unpredictably stall

**Rack callback caution:** Treat `Module::process(const ProcessArgs& args)` as a per-sample callback unless the local Rack SDK or existing repository code proves otherwise. Do not invent an `args.sampleCount` buffer loop.

### UI Code: `ModuleWidget::step()` and `draw()`

UI code is not real-time audio code. It may perform more complex work than `process()`, but it still runs frequently and should avoid unnecessary per-frame allocations or blocking work.

**Allowed in UI code:**
- Widget state updates
- Rendering with NanoVG/OpenGL
- Reading module state through atomics or explicit thread-safe snapshots
- Preparing visualization data
- Rate-limited debug metric submission
- Lazy validation/recreation of graphics resources

**Avoid in UI hot paths:**
- Repeated SVG parsing
- Repeated image loading
- Heavy allocation every frame
- Blocking file/network operations
- Direct reads of non-atomic module state that can be written by `process()`

### Cross-Thread Communication

**Safe patterns:**
1. **`std::atomic<T>`** for simple scalar state
2. **Versioned snapshots** for several related values
3. **Double buffering** for complex data
4. **Preallocated ring buffers** for one-way event/data transfer
5. **UI-owned cached copies** rebuilt from atomic/shared snapshots

**Cautions:**
- `std::shared_ptr` has thread-safe reference counting, but the pointed-to object is not automatically thread-safe.
- `memory_order_relaxed` is fine for many independent meters/previews, but do not use it blindly for multi-field invariants.
- If several related values must be read consistently, publish them with a version/counter or snapshot protocol.
- Avoid raw pointers/shared references to mutable non-atomic data across audio/UI boundaries.

### Preferred Preview Publication Pattern

Audio/engine side:

```cpp
struct PreviewSharedState {
    std::atomic<float> riseTime {0.01f};
    std::atomic<float> fallTime {0.01f};
    std::atomic<float> curveSigned {0.f};
    std::atomic<uint32_t> version {1};
};

void publishPreviewState(PreviewSharedState& shared, float rise, float fall, float curve) {
    shared.riseTime.store(rise, std::memory_order_relaxed);
    shared.fallTime.store(fall, std::memory_order_relaxed);
    shared.curveSigned.store(curve, std::memory_order_relaxed);
    shared.version.fetch_add(1u, std::memory_order_release);
}
```

UI side:

```cpp
void step() override {
    const uint32_t version = shared.version.load(std::memory_order_acquire);
    if (version != lastVersion) {
        const float rise = shared.riseTime.load(std::memory_order_relaxed);
        const float fall = shared.fallTime.load(std::memory_order_relaxed);
        const float curve = shared.curveSigned.load(std::memory_order_relaxed);

        rebuildPreviewFrom(rise, fall, curve);
        lastVersion = version;
    }
}
```

For simple meters where exact consistency is not important, relaxed atomics are fine. For related multi-field state, prefer a versioned publication protocol.

### Module-Widget State Relationship

**Design pattern:** Module and Widget are separate objects with a clear separation of concerns.

```cpp
struct IntegralFlux : Module {
    // Audio/engine-owned state.
    std::atomic<float> perfAudioProcessNs {0.f};

    struct PreviewSharedState {
        std::atomic<float> riseTime {0.01f};
        std::atomic<float> fallTime {0.01f};
        std::atomic<uint32_t> version {1};
    };

    PreviewSharedState previewCh1;
};

struct IntegralFluxWidget : ModuleWidget {
    // Widget-owned state. UI thread only.
    float uiStepMsEma = 0.f;
    float uiDrawMsEma = 0.f;

    void step() override {
        ModuleWidget::step();
        // Read module state through atomics/snapshots only.
    }

    void draw(const DrawArgs& args) override {
        ModuleWidget::draw(args);
        // Draw UI. Can use complex operations, but avoid per-frame churn.
    }
};
```

**Key principles:**
1. Module state is owned by `Module`; audio/engine code may update it.
2. Widget state is owned by `ModuleWidget`; UI code may update it.
3. Cross-thread communication must use atomics/snapshots/buffers.
4. UI should never directly read non-atomic module members that `process()` writes.

### Lifecycle Notes

| Event | Typical Method | Notes |
|-------|----------------|-------|
| Plugin load | `Plugin::init()` | Register all module models. |
| Module instance created | Module constructor | Can allocate/init state. Preserve released-module compatibility. |
| Sample rate change | `onSampleRateChange()` | Recompute sample-rate dependent values. |
| Audio processing | `process()` | Real-time hot path. No blocking/allocation. |
| UI update | `ModuleWidget::step()` | Sync UI state from module through safe communication. |
| UI render | `ModuleWidget::draw()` | Render widgets, submit rate-limited metrics. |
| Save/load | `dataToJson()` / `dataFromJson()` | Preserve JSON keys for released modules. |
| Module destroyed | destructors | Free resources carefully. Avoid cross-context graphics deletion bugs. |

### Rate-Limited Updates

Use rate limiting for lights, meters, debug metrics, and preview publication instead of doing expensive work every sample.

```cpp
lightUpdateTimer += args.sampleTime;
if (lightUpdateTimer >= 1.0f / 120.0f) {
    lights[LED].setBrightness(brightness);
    lightUpdateTimer = 0.f;
}
```

For counter-based throttling:

```cpp
if (++updateCounter >= UPDATE_DIVISOR) {
    updateCounter = 0;
    // Publish lightweight UI/debug state here.
}
```

### Debug Timing

Debug timing in hot paths must be guarded, rate-limited, and removable. Prefer existing project debug infrastructure and feature flags over ad hoc logging.

Do not add unconditional `std::chrono`, string formatting, logging, or debug terminal submission to `process()` unless explicitly requested and protected by debug checks.

---

## Common Patterns & Shared Infrastructure

### Debug Terminal System

The project uses an external debug terminal server to monitor performance metrics in real-time without in-Rack overhead.

**Location:** `tools/debug_terminal/`

**Key Files:**
- `server.py` - Python server that listens for UDP packets
- `protocol.py` - Message format definitions
- `model.py` - Data models for metrics
- `render.py` - Visualization rendering

**Usage:**
Modules submit timing metrics using functions from `DebugTerminalTransport.hpp`:
- `submitTDScopeUiMetrics()` - Scope UI timing
- `submitTemporalDeckUiMetrics()` - Temporal Deck UI timing
- `submitBifurxUiMetrics()` - Bifurx UI timing
- `submitWyrmMetrics()` - Wyrm timing
- `submitIntegralFluxMetrics()` - Integral Flux timing
- `submitProcMetrics()` - Proc timing
- `submitUndertowMetrics()` - Undertow timing

**Debug Feature Flags:**
Controlled via `res/dragonking.txt` JSON file:
```json
{
  "debug": true,
  "PreviewWidgetOptions": false,
  "clockworkDragLogging": false,
  "temporalDeckLifetimeLogging": false,
  "moduleTeardownLogging": false
}
```

**Runtime Checks:**
```cpp
isDragonKingDebugEnabled()
isDragonKingPreviewWidgetOptionsEnabled()
isClockworkDragDebugLoggingEnabled()
isTemporalDeckLifetimeLoggingEnabled()
isModuleTeardownLoggingEnabled()
```

### Panel SVG Anchor System

Modules use SVG panels with specially tagged elements that define anchor points for dynamic component placement.

**Pattern in SVG:**
```xml
<circle inkscape:label="jack_IN1" cx="15.5mm" cy="25.0mm" r="1mm" />
<rect inkscape:label="ENHANCE_1" x="0mm" y="0mm" width="100mm" height="380mm" />
```

**Usage:**
```cpp
// In module constructor or UI setup
panelSvg = visual_assets::loadPluginSvgCached("res/TemporalDeck.svg");
magitekInputjack = addParam(ParamWidget::create<MagitekInputJack>(
    panel_svg::loadPointFromSvgMm(panelSvg, "jack_IN1"), 
    module, TemporalDeck::INPUT_L_INPUT));
```

**Helper Functions:**
- `panel_svg::loadPointFromSvgMm()` - Get center point of circle/ellipse
- `panel_svg::loadRectFromSvgMm()` - Get rect position/size
- `panel_svg::findRectsWithIdSubstringMm()` - Find all rects matching pattern
- `panel_svg::loadCircleFromSvg()` - Get circle center/radius

**Anchor Atlas:**
The `PanelAnchorAtlas` system caches parsed anchors to avoid repeated SVG parsing. Atlases are stored in the plugin's `res/` directory as binary files.

### Visual Assets

**Custom Knobs:**
- `GearKnobInvertSized` - Clockwork gear effect with active ring
- `TinyClockworkGearKnob` / `BipolarTinyClockworkGearKnob` - Smaller versions
- `EclipseKnob` - Progress ring with shadow layer
- `Eclipse2Knob` - LED ring variant
- `LeviathanHaloKnob` / `LeviathanHaloKnob2` - Glowing arc design with bloom effects
- `ClockworkGearKnob` / `BigClockworkGearKnob` - Dual cogwheel effect

**Custom Jacks:**
- `MagitekInputJack` / `MagitekOutputJack` - Simple SVG ports
- `Magitek2InputJack` / `Magitek2OutputJack` - Animated jack with hover spin

**Custom Lights:**
- `LeviathanCyanPurpleLight` - Cyan (0xc6e4) + purple (0xa862ff) dual-color
- `TLeviathanCyanPurpleLight<TBase>` - Template variant

**Custom Buttons:**
- `GoldButton` - 3D button with press overlay and fixed bezel

**3D SVG Effects:**
- `visual_assets::createSvgRect3DEffectWidget()` - Adds 3D gradient/shadow to SVG rects
- `visual_assets::createPreviewFrameEnhancementWidget()` - Frame enhancement
- `visual_assets::createLeviathanFooterLogoWidget()` - Footer dragon logo

### Graphics Lifecycle Management

**NanoVG (VisualAssets.cpp):**
- Use `nvg_gfx_lifecycle::resetOwnedNvgImage()` to safely recreate images on context switch
- Cache image dimensions and validate before reuse
- Never delete handles from a different NVGcontext

**OpenGL (GlLifecycleUtils.hpp):**
- Use `gl_lifecycle::isValidProgramBufferPair()` / `isValidTextureFramebufferPair()` before drawing
- Validate resources lazily in draw/step time, not in destructors
- Module-specific reset graphs kept local

### Preview System & Thread-Safe Communication

**Pattern:** The preview system demonstrates lock-free communication between audio and UI threads:

```cpp
// Module (audio thread writes, UI thread reads)
struct PreviewSharedState {
    std::atomic<float> riseTime {0.01f};
    std::atomic<float> fallTime {0.01f};
    std::atomic<float> curveSigned {0.f};
    std::atomic<uint32_t> version {1};
};

void publishPreviewState(PreviewSharedState& shared, float riseTime, float fallTime, float curveSigned) {
    shared.riseTime.store(riseTime, std::memory_order_relaxed);
    shared.fallTime.store(fallTime, std::memory_order_relaxed);
    shared.curveSigned.store(curveSigned, std::memory_order_relaxed);
    shared.version.fetch_add(1, std::memory_order_relaxed);  // Publish!
}

// UI widget (reads via atomics)
void step() override {
    float riseTime = shared.riseTime.load(std::memory_order_relaxed);
    float fallTime = shared.fallTime.load(std::memory_order_relaxed);
    uint32_t version = shared.version.load(std::memory_order_relaxed);
    
    if (version != lastVersion) {
        rebuildPreview();  // Only rebuild when data changes
        lastVersion = version;
    }
}
```

**Key Principles:**
1. **Batched atomic writes** - All related values written before version increment
2. **Version-based invalidation** - UI only updates when version changes
3. **`memory_order_relaxed` sufficient** - Values are independent, no ordering needed
4. **Lock-free** - No mutexes, no blocking, no allocations

### Testing Framework

Tests are written using a custom specification-style framework in `tests/`.

**Test Structure:**
```cpp
struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

TestResult testSomething() {
  // ... test logic
  return {"Description", pass, "detail message"};
}

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testSomething());
  // ... run all tests
  // Print [PASS]/[FAIL] with details
}
```

**Build Command:** `make test-fast` (runs all tests in `tests/*_spec.cpp`)

**Key Test Files:**
- `crownstep_spec.cpp` - Game rules (checkers, chess, Othello)
- `temporaldeck_*_spec.cpp` - Various Temporal Deck subsystems
- `bifurx_*_spec.cpp` - Filter rendering
- `panel_svg_utils_spec.cpp` - SVG parsing

### Common Module Patterns

**Module Teardown Timing:**
```cpp
struct ModuleTeardownTimer {
  ModuleTeardownTimer(const char* moduleName);
  ~ModuleTeardownTimer();
  void begin(int moduleId);
};
// Member in module: ModuleTeardownTimer teardownTimer {"ModuleName"};
```

Writes teardown duration to `~/Leviathan/module_teardown.csv` when enabled.

**Preview Build Logging:**
```cpp
PreviewBuildLogTimer timer{"ModuleName", module};
// Marks panel/anchor loading times
timer.markPanelDone();
timer.markAnchorsDone();
```

Logs panel and anchor loading times to INFO when debug enabled.

---

## Build Environment Notes

### WSL / Windows Environment
- Primary development target: Windows VCV Rack plugin builds via MSYS2
- WSL builds are for development/testing only
- Do not treat `plugin.so` link failures in WSL as regressions
- Final authoritative builds are done by the user in Windows/MSYS2

### Linux Environment
- Full plugin builds expected to work with Rack SDK
- Full plugin linking should be verified during development

### Test Commands
- `make test-fast` - Run all unit tests
- `make` - Build plugin (authoritative on Linux, developmental in WSL)

---

## Release Compatibility Considerations

**Released Modules (Integral Flux, Proc, Temporal Deck, TD.Scope, Undertow):
Changes must preserve backward compatibility:
- Parameter/input/output/light IDs must not be reordered
- Patch state serialization format must remain stable
- User-visible behavior should not change unexpectedly

**Unreleased Modules (Crownstep, Bifurx, Wyrm, etc.):**
Compatibility constraints are looser but still should follow patterns established in released modules.

**Safe Pattern:** Always append to enum lists rather than inserting in middle.

---

## Performance Measurement & Debugging

### General Rule

Performance instrumentation should not compromise real-time safety. Prefer existing project debug infrastructure, rate limits, and runtime feature flags. When adding new metrics, keep the audio-thread path as small and predictable as possible.

### Audio Thread Metrics

Audio-thread metrics are allowed only when guarded and lightweight. Prefer counters, accumulated nanoseconds, and rate-limited publication. Avoid unconditional logging, allocation, string formatting, or debug terminal submission from `process()`.

```cpp
if (isDragonKingDebugEnabled()) {
    const auto perfStart = std::chrono::steady_clock::now();

    // ... audio processing ...

    const auto elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - perfStart).count();
    perfAudioProcessNs.fetch_add(static_cast<float>(elapsedNs), std::memory_order_relaxed);
    perfAudioSampledCount.fetch_add(1u, std::memory_order_relaxed);
}
```

If timing itself becomes measurable overhead, use coarser sampling instead of timing every process call.

### UI Thread Metrics

UI timing can be more detailed, but still avoid per-frame allocation and blocking work.

```cpp
const auto stepStart = std::chrono::steady_clock::now();
ModuleWidget::step();
const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now() - stepStart).count()) * 1e-6f;
uiStepMsEma = (uiStepMsEma > 0.f) ? (uiStepMsEma + (stepMs - uiStepMsEma) * 0.18f) : stepMs;
```

### Debug Terminal Submission

- Metrics are submitted to the external Python server via UDP.
- Timing ranges may be submitted for process/step/draw.
- Submit only when `isDragonKingDebugEnabled()` returns true.
- Prefer UI-thread/rate-limited submission over audio-thread submission.
- Controlled by `res/dragonking.txt` configuration.

---

## Development Workflow

1. **SVG Panels** - Design in Inkscape, tag anchor elements with `inkscape:label`
2. **Module Logic** - Implement in module source files
3. **UI Widgets** - Create custom widgets using VisualAssets patterns
4. **Anchor Atlas** - Run `generate_panel_anchor_atlas.py` to create binary anchors
5. **Testing** - Run `make test-fast` for unit tests
6. **Debug** - Enable debug features via `res/dragonking.txt`
7. **Build** - Compile in target environment (Windows/MSYS2 for final builds)

---

## External Dependencies

- **VCV Rack 2 SDK** - Core framework
- **Python 3** - Debug terminal server, anchor atlas generation
- **Nanovg** - 2D rendering (bundled with Rack)
- **OpenGL** - GPU rendering for complex visualizations

---

## Common Pitfalls to Avoid

### Audio Thread Violations
| Mistake | Impact | Fix |
|---------|--------|-----|
| Allocating memory in `process()` | Audio dropouts/glitches | Pre-allocate, reuse buffers |
| File I/O in `process()` | Stalls, dropouts | Load files in constructor |
| String formatting in `process()` | Unpredictable timing | Pre-compute, cache |
| `std::unordered_map` lookup | Allocation, unpredictable | Use arrays/LUTs |

### UI Thread Violations
| Mistake | Impact | Fix |
|---------|--------|-----|
| Reading non-atomic module state in UI | Race conditions | Use atomics or copy on UI thread |
| Blocking operations in UI | Stuttering UI | Use async patterns |
| Complex allocations every frame | Fragmentation, stutter | Reuse buffers, pre-allocate |

### Threading Model Misunderstandings
| Mistake | Impact | Fix |
|---------|--------|-----|
| Assuming `process()` and `draw()` run together | Incorrect synchronization | Use atomics for cross-thread |
| Thinking Rack uses multi-core | Single audio engine | All `process()` sequential |
| Assuming UI is multi-threaded | Race conditions | UI is single-threaded |

---

## Key Design Principles

1. **Performance First** - Fast math, lookup tables, cached values in hot paths
2. **Shared Infrastructure** - Common patterns across modules (VisualAssets, DebugTerminal)
3. **Test-Driven** - Unit tests for core logic (engine, game rules, signal processing)
4. **Visual Consistency** - Dragon theme, cyan/purple palette, custom widgets
5. **Debuggability** - Runtime feature flags, debug terminal transport, teardown timing
6. **Backward Compatible** - Released modules preserve enum ordering and serialization

---

## Quick Reference: Common Tasks

| Task | Pattern |
|------|---------|
| Add new module | Add Model declaration in plugin.hpp, add to init() in plugin.cpp, create module source |
| Add knob to panel | Use existing VisualAssets knob classes and SVG anchors; add params with repository-local patterns |
| Position jack from SVG | `panel_svg::loadPointFromSvgMm(svgPath, "label")` |
| Debug timing in hot path | Use `PreviewBuildLogTimer`, `ModuleTeardownTimer` |
| Submit metrics to debug terminal | Call `submit*_Metrics()` from UI widgets |
| Add new game rule | Implement in CrownstepCore.hpp, add to IGameRules interface |
| Create custom knob | Extend `EclipseKnob`, `GearKnobInvertSized`, or `LeviathanHaloKnob2` |
| Thread-safe UI update | Use `std::atomic` with version number, UI polls for changes |
| Rate-limited process() update | Use counter with `timingUpdateDiv` to reduce CPU |
| Light update at reduced rate | Timer-based update, typically 120Hz max |

---

*This primer was revised for Leviathan Rack Plugin Suite v2.8.1 agent use.*
