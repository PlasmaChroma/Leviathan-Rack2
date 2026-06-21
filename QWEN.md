# Leviathan Rack Plugin Suite - Context Primer

See also (other files): 
Agents.md
vcv-coding.md

## Overview

The Leviathan plugin suite is a collection of VCV Rack 2 modules focused on experimental sound design, synthesis, and modular control. All modules share a common visual identity (dragon-themed aesthetic, cyan/purple color scheme) and a set of shared infrastructure utilities for graphics, panel positioning, and debugging.

**Current Released Modules:**
- **Integral Flux** - Dual slope generator with variable curve slew, attenuverters, signal mixing, and full audio-rate fidelity
- **Proc** - Generator, slew limiter, and envelope follower
- **Temporal Deck** - Virtual turntable with modern hybrid control

**Current Unreleased Modules:**
- **TD.Scope** - Expander: Waveform display for Temporal Deck
- **Crownstep** - Checkers-driven sequencer (game rules: checkers, chess, Othello)
- **Bifurx** - Dual-peak multimode filter with live response and spectrum preview
- **Wyrm** - Custom drawable wavetable oscillator with FM, sync, and fold
- **Sil** - Automatic mastering module
- **Chronomaw** - Rack-native eight-output clock and modulation engine
- **Bulkhead** - Spatial room reverb
- **Undertow** - Compact sub-harmonic oscillator

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

### Undertow (Unreleased)
| File | Purpose |
|------|---------|
| `Undertow.hpp` / `Undertow.cpp` | Sub-harmonic oscillator |

### Sil (Unreleased)
| File | Purpose |
|------|---------|
| `Sil.cpp` | Automatic mastering |

### TDScope (Unreleased - Expander)
| File | Purpose |
|------|---------|
| `TDScope.hpp` | Temporal Deck expander waveform display |
| `TDScopeWidget.cpp` | UI widget |
| `TDScopeGL.cpp` | OpenGL rendering |

---

## VCV Rack Execution Model

### Threading Architecture

VCV Rack 2 uses a **dual-thread architecture** that is critical to understand when developing performance-critical audio modules:

| Thread | Purpose | Characteristics |
|--------|---------|-----------------|
| **Audio Thread** | `Module::process()` | Real-time, high priority, deterministic timing, no allocations, no I/O |
| **UI Thread** | `ModuleWidget::step()`, `ModuleWidget::draw()` | Non-real-time, handles user interaction, can allocate, can call OS APIs |

**Key Implications:**
- **Audio thread MUST NOT block** - any stalls cause audio dropouts/glitches
- **UI thread can be slow** - it's acceptable for UI updates to take longer
- **Thread separation is absolute** - `process()` and `step()/draw()` never run simultaneously on the same module
- **UI thread is single-threaded** - all widget operations happen on one thread

### Execution Cycle

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Audio Thread (process)                       │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ 1. Audio input sampling (if needed)                          │   │
│  │ 2. Module::process() called for ALL modules                  │   │
│  │ 3. Audio output rendering                                    │   │
│  └──────────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ Repeat at audio buffer boundary (~1ms at 48kHz)              │   │
│  └──────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
                              │
                              │ (separate thread)
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        UI Thread (step + draw)                      │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ 1. ModuleWidget::step() - UI state updates                   │   │
│  │    - Read module state via atomics/caching                   │   │
│  │    - Update widget state                                     │   │
│  └──────────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ 2. ModuleWidget::draw() - Rendering                          │   │
│  │    - Render UI with NanoVG/OpenGL                            │   │
│  │    - Submit debug metrics                                    │   │
│  └──────────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │ 3. Wait for next frame (~16.7ms at 60fps)                    │   │
│  └──────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
```

### Communication Between Threads

**Safe Patterns:**
1. **`std::atomic<T>`** - Lock-free communication for simple types
2. **`std::shared_ptr<T>`** - Shared ownership with thread-safe reference counting
3. **Double buffering** - Separate read/write buffers for complex data
4. **Batched updates with version numbers** - UI only updates when version changes

**Unsafe Patterns:**
- Raw pointers/shared references to non-atomic data
- `std::unordered_map`/`std::vector` without synchronization
- Non-atomic reads/writes to shared data

### Timing Considerations

| Operation | Typical Frequency | Audio Thread Safe? |
|-----------|-------------------|-------------------|
| `process()` | Audio sample rate (44.1kHz-192kHz) | ✅ Required |
| `step()` | UI refresh rate (~60Hz) | ❌ Not audio thread |
| `draw()` | UI refresh rate (~60Hz) | ❌ Not audio thread |
| `onSampleRateChange()` | On sample rate change | ✅ Safe (not real-time) |
| `dataToJson()` / `dataFromJson()` | On save/load | ✅ Safe (not real-time) |

### Performance Guidelines

**Audio Thread (`process()`):**
- ✅ Use LUTs instead of `powf()`, `expf()`, `sinf()`
- ✅ Cache computed values (warp scale, timing, etc.)
- ✅ Use `std::max()`/`std::min()` instead of `clamp()` when possible
- ✅ Minimize branches (predictable execution)
- ❌ No allocations (`new`, `malloc`, `std::vector::push_back`)
- ❌ No file I/O
- ❌ No string formatting
- ❌ No `std::unordered_map` lookups

**UI Thread (`step()`, `draw()`):**
- ✅ Can allocate memory
- ✅ Can use complex algorithms
- ✅ Can call OS APIs
- ❌ Still should avoid allocations in hot paths (per-frame)
- ✅ Use EMA (exponential moving average) for smooth timing metrics

### Module-Widget State Relationship

**Design Pattern:** Module and Widget are separate objects with a clear separation of concerns:

```cpp
struct IntegralFlux : Module {
    // State that UI needs to read
    std::atomic<float> perfAudioProcessNs {0};
    std::atomic<float> perfUiRenderMs {0.f};
    
    // Thread-safe communication with UI
    struct PreviewSharedState {
        std::atomic<float> riseTime {0.01f};
        std::atomic<float> fallTime {0.01f};
        std::atomic<uint32_t> version {1};
    };
    PreviewSharedState previewCh1;
};

struct IntegralFluxWidget : ModuleWidget {
    // Widget-specific state (UI thread only)
    float uiStepMsEma = 0.f;
    float uiDrawMsEma = 0.f;
    
    void step() override {
        // Read module state via atomics (thread-safe)
        float uiMs = module->perfUiRenderMs.load(std::memory_order_relaxed);
        // ... update UI
    }
    
    void draw() override {
        // Draw UI (can use complex operations)
    }
};
```

**Key Principles:**
1. **Module state** - owned by `Module`, accessed by audio thread (and UI via atomics)
2. **Widget state** - owned by `ModuleWidget`, accessed only by UI thread
3. **Cross-thread communication** - use `std::atomic` with appropriate memory ordering
4. **No direct access** - UI should never directly read non-atomic module members

### Module Lifecycle

| Event | Thread | Method Called | Notes |
|-------|--------|---------------|-------|
| Plugin load | UI | `Plugin::init()` | Register all Module Models |
| Module instance created | UI | `Module` constructor | Initialize parameters, set up state |
| Sample rate change | UI | `Module::onSampleRateChange()` | Recompute sample-rate dependent values |
| Audio processing | Audio | `Module::process()` | Real-time audio DSP |
| UI update | UI | `ModuleWidget::step()` | Sync UI state from module |
| UI render | UI | `ModuleWidget::draw()` | Render widgets, submit metrics |
| Module destroyed | UI | `Module` destructor | Cleanup resources |

**Critical Notes:**
- **Constructor** runs on UI thread - can allocate, can use complex initialization
- **Destructor** runs on UI thread - can free resources, no real-time constraints
- **`process()`** is the ONLY method running on audio thread
- All other methods (`step()`, `draw()`, constructor, destructor) run on UI thread

### Audio Buffer Processing Pattern

**Typical `process()` implementation:**
```cpp
void process(const ProcessArgs& args) override {
    // 1. Read inputs (voltage levels)
    float in1 = inputs[INPUT_1].getVoltage();
    float in2 = inputs[INPUT_2].getVoltage();
    
    // 2. Process at sample rate (loop over audio buffer)
    for (int i = 0; i < args.sampleCount; ++i) {
        // Process one sample
        float out = processSample(in1, in2, args.sampleTime);
        
        // 3. Write outputs
        outputs[OUTPUT_1].setVoltage(out, i);
    }
    
    // 4. Update lights (rate-limited to 120Hz)
    lightUpdateTimer += args.sampleTime;
    if (lightUpdateTimer >= 1.0f / 120.0f) {
        lights[LED].setBrightness(brightness);
        lightUpdateTimer = 0.f;
    }
}
```

**Key Points:**
- **`args.sampleCount`** - Number of samples in current buffer (typically 8-128)
- **`args.sampleTime`** - Time per sample (1.0 / sampleRate)
- **Loop over buffer** - Process each sample individually
- **Lights/controls** - Updated at reduced rate (not per-sample)

**Common Pattern - Rate-Limited Updates:**
```cpp
// Only update complex UI state at reduced rate
if (updateCounter++ >= UPDATE_DIVISOR) {
    updateCounter = 0;
    // Expensive UI update here
}
```

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

**Released Modules (Integral Flux, Proc, Temporal Deck):**
Changes must preserve backward compatibility:
- Parameter/input/output/light IDs must not be reordered
- Patch state serialization format must remain stable
- User-visible behavior should not change unexpectedly

**Unreleased Modules (Crownstep, Bifurx, Wyrm, etc.):**
Compatibility constraints are looser but still should follow patterns established in released modules.

**Safe Pattern:** Always append to enum lists rather than inserting in middle.

---

## Performance Measurement & Debugging

**Audio Thread Performance Tracking:**
```cpp
// In process():
const uint64_t perfStart = std::chrono::steady_clock::now();

// ... audio processing ...

const uint64_t elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now() - perfStart).count();
perfAudioProcessNs.fetch_add(elapsedNs, std::memory_order_relaxed);
perfAudioSampledCount.fetch_add(1u, std::memory_order_relaxed);
```

**UI Thread Performance Tracking:**
```cpp
// In step():
const uint64_t stepStart = std::chrono::steady_clock::now();
ModuleWidget::step();
const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now() - stepStart).count()) * 1e-6f;
uiStepMsEma = (uiStepMsEma > 0.f) ? (uiStepMsEma + (stepMs - uiStepMsEma) * 0.18f) : stepMs;
```

**Debug Terminal Submission:**
- Metrics submitted to external Python server via UDP
- Timing ranges (min/max) submitted for process/step/draw
- Only submitted when `isDragonKingDebugEnabled()` returns true
- Controlled by `res/dragonking.txt` configuration

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
| Add knob to panel | Use `visual_assets::createSvgRect3DEffectWidget()`, add param with Magitek knob |
| Position jack from SVG | `panel_svg::loadPointFromSvgMm(svgPath, "label")` |
| Debug timing in hot path | Use `PreviewBuildLogTimer`, `ModuleTeardownTimer` |
| Submit metrics to debug terminal | Call `submit*_Metrics()` from UI widgets |
| Add new game rule | Implement in CrownstepCore.hpp, add to IGameRules interface |
| Create custom knob | Extend `EclipseKnob`, `GearKnobInvertSized`, or `LeviathanHaloKnob2` |
| Thread-safe UI update | Use `std::atomic` with version number, UI polls for changes |
| Rate-limited process() update | Use counter with `timingUpdateDiv` to reduce CPU |
| Light update at reduced rate | Timer-based update, typically 120Hz max |

---

*This primer was generated for Leviathan Rack Plugin Suite v2.8.1*
