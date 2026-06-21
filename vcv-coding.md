# VCV Rack 2 Plugin Development Guide

This document compiles essential information for developing VCV Rack 2 plugins, based on deep research into the VCV Rack 2 SDK and real-world implementations.

---

## Table of Contents

1. [Core Architecture](#1-core-architecture)
2. [Threading Model](#2-threading-model)
3. [Module System](#3-module-system)
4. [Graphics and Rendering](#4-graphics-and-rendering)
5. [Performance Optimization](#5-performance-optimization)
6. [Testing and Debugging](#6-testing-and-debugging)
7. [Common Patterns and Anti-Patterns](#7-common-patterns-and-anti-patterns)

---

## 1. Core Architecture

### Plugin Structure

VCV Rack 2 plugins follow a standard structure with two entry points:

```cpp
// plugin.hpp
#pragma once
#include "rack.hpp"

using namespace rack;

extern Plugin* pluginInstance;

// Declare module models
extern Model* modelModuleName;

// Runtime feature flags
bool isDragonKingDebugEnabled();
void refreshDragonKingDebugEnabled();

// Plugin lifecycle
void init(Plugin* p);
void destroy();
```

```cpp
// plugin.cpp
#include "plugin.hpp"

Plugin* pluginInstance = NULL;

void init(Plugin* p) {
    pluginInstance = p;
    
    // Register module models
    p->addModel(modelModuleName);
    
    // Additional initialization
}

void destroy() {
    // Cleanup shared resources
}
```

### Module Class Structure

Modules inherit from `rack::Module` and define control IDs using enums:

```cpp
struct ModuleName : Module {
    // Control IDs - ordered for serialization stability
    enum ParamId {
        PARAM_A,
        PARAM_B,
        PARAMS_LEN
    };
    
    enum InputId {
        INPUT_A,
        INPUT_B,
        INPUTS_LEN
    };
    
    enum OutputId {
        OUTPUT_A,
        OUTPUTS_LEN
    };
    
    enum LightId {
        LIGHT_A,
        LIGHTS_LEN
    };
    
    // Public API
    void onSampleRateChange() override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    void process(const ProcessArgs& args) override;
    
    // Module state
    float stateVariable;
};
```

### Lifecycle Events

| Event | Thread | Method | Notes |
|-------|--------|--------|-------|
| Plugin load | UI | `Plugin::init()` | Register all Module Models |
| Module created | UI | `Module` constructor | Initialize parameters, state |
| Sample rate change | UI | `onSampleRateChange()` | Recompute sample-rate values |
| Audio processing | Audio | `process()` | Real-time DSP |
| UI update | UI | `ModuleWidget::step()` | Sync UI state (~60Hz) |
| UI render | UI | `ModuleWidget::draw()` | Render widgets |
| Module destroyed | UI | `Module` destructor | Cleanup resources |

---

## 2. Threading Model

VCV Rack 2 uses a **dual-thread architecture**:

| Thread | Purpose | Constraints |
|--------|---------|-------------|
| **Audio Thread** | `process()` | Real-time, no allocations, no I/O, must not block |
| **UI Thread** | `step()`, `draw()` | Non-real-time, can allocate, can call OS APIs |

### Critical Rules

1. **`process()` and `step()/draw()` NEVER run simultaneously** on the same module
2. **UI thread is single-threaded** - all widget operations happen on one thread
3. **Audio thread MUST NOT block** - stalls cause audio dropouts/glitches

### Thread-Safe Communication

**Use `std::atomic` for cross-thread communication:**

```cpp
struct Module : Module {
    // Lock-free communication to UI thread
    std::atomic<float> audioProcessTimeNs {0.f};
    std::atomic<uint32_t> version {1};
};

// Audio thread - writer
void process(const ProcessArgs& args) override {
    // ... process audio ...
    
    const uint64_t elapsedNs = /* ... */;
    audioProcessTimeNs.fetch_add(elapsedNs, std::memory_order_relaxed);
    version.fetch_add(1, std::memory_order_relaxed);  // Publish
}

// UI thread - reader
void step() override {
    float timeNs = audioProcessTimeNs.load(std::memory_order_relaxed);
    uint32_t ver = version.load(std::memory_order_relaxed);
    
    if (ver != lastVersion) {
        updateDisplay();
        lastVersion = ver;
    }
}
```

---

## 3. Module System

### Control Management

Controls are created using enum-based IDs:

```cpp
// In widget constructor
ModuleNameWidget(ModuleName* module) : ModuleWidget(module) {
    this->module = module;
    
    // Load SVG panel
    panelSvg = visual_assets::loadPluginSvgCached("res/moduleName.svg");
    
    // Create knob from SVG anchor
    addParam(ParamWidget::create<LeviathanHaloKnob2>(
        panel_svg::loadPointFromSvgMm(panelSvg, "PARAM_A"),
        module, ModuleName::PARAM_A
    ));
    
    // Create input jack
    addInput(InputWidget::create<MagitekInputJack>(
        panel_svg::loadPointFromSvgMm(panelSvg, "INPUT_A"),
        module, ModuleName::INPUT_A
    ));
    
    // Create output jack
    addOutput(OutputWidget::create<MagitekOutputJack>(
        panel_svg::loadPointFromSvgMm(panelSvg, "OUTPUT_A"),
        module, ModuleName::OUTPUT_A
    ));
    
    // Create light
    addChild(LightWidget::create<LeviathanCyanPurpleLight>(
        panel_svg::loadPointFromSvgMm(panelSvg, "LIGHT_A"),
        module, ModuleName::LIGHT_A
    ));
}
```

### JSON Serialization

```cpp
json_t* dataToJson() override {
    json_t* root = json_object();
    json_object_set_int(root, "version", 1);
    json_object_set_int(root, "paramAValue", params[PARAM_A].getValue());
    json_object_set_int(root, "inputMode", inputMode);
    return root;
}

void dataFromJson(json_t* root) override {
    int version = json_integer_value(json_object_get(root, "version"));
    if (version >= 1) {
        params[PARAM_A].setValue(json_integer_value(json_object_get(root, "paramAValue")));
        inputMode = json_integer_value(json_object_get(root, "inputMode"));
    }
}
```

### Sample Rate Handling

```cpp
void onSampleRateChange() override {
    applySampleRateChange(APP->engine->getSampleRate());
}

void applySampleRateChange(float sampleRate) {
    // Recompute sample-rate dependent values
    buffer.setSampleRate(sampleRate);
    phaseStep = targetRate * (1.0f / sampleRate);
    filterCoefficients = computeFilterCoefficients(sampleRate);
}
```

---

## 4. Graphics and Rendering

### NanoVG Graphics

**Shared lifecycle management** (VisualAssets.cpp):

```cpp
namespace nvg_gfx_lifecycle {

bool resetOwnedNvgImage(NVGcontext*& ownerVg,
                        int& handle,
                        int& cachedWidth,
                        int& cachedHeight,
                        NVGcontext* currentVg,
                        bool deleteCurrentHandle) {
    if (deleteCurrentHandle && currentVg && ownerVg == currentVg && handle >= 0) {
        nvgDeleteImage(currentVg, handle);
    }
    ownerVg = nullptr;
    handle = -1;
    cachedWidth = 0;
    cachedHeight = 0;
    return true;
}

} // namespace nvg_gfx_lifecycle
```

**Usage:**
```cpp
// Never delete handles from different NVGcontext
// Cache dimensions and validate before reuse
// On context switch, invalidate and lazily rebuild
```

### OpenGL Integration

**Shared validation helpers** (GlLifecycleUtils.hpp):

```cpp
namespace gl_lifecycle {

bool isValidProgramBufferPair(GLuint program, GLuint buffer) {
    return program != 0 && buffer != 0 && glIsProgram(program) && glIsBuffer(buffer);
}

bool isValidTextureFramebufferPair(GLuint texture, GLuint framebuffer) {
    return texture != 0 && framebuffer != 0 && glIsTexture(texture) && glIsFramebuffer(framebuffer);
}

} // namespace gl_lifecycle
```

### SVG Panel Anchors

SVG panels use specially tagged elements for dynamic placement:

```xml
<!-- In SVG (Inkscape) -->
<circle inkscape:label="PARAM_A" cx="4700.0" cy="1600.0" r="1mm" />
<rect inkscape:label="ENHANCE_BACKGROUND" x="0" y="0" width="10000" height="3800" />
```

**Anchor system** (PanelSvgUtils.hpp/cpp):

```cpp
// Get anchor point for component placement
Vec jackPosPx = mm2px(panel_svg::loadPointFromSvgMm(
    panelSvg, "PARAM_A"
));

// Get rect for background/highlight
math::Rect backgroundRect;
panel_svg::loadRectFromSvgMm(panelSvg, "ENHANCE_BACKGROUND", &backgroundRect);
```

### Custom Widgets

**Knobs:**
```cpp
struct LeviathanHaloKnob2 : app::SvgKnob {
    Config config;
    GlowArcWidget* glowArc = nullptr;
    LightArcWidget* lightArc = nullptr;
    
    void onDragMove(const DragMoveEvent& e) override {
        // Custom drag behavior
    }
};
```

**Jacks:**
```cpp
struct MagitekInputJack : app::SvgPort {
    MagitekInputJack() {
        setSvg(Svg::load("res/magitek_input.svg"));
    }
};
```

**Lights:**
```cpp
template <typename TBase = GrayModuleLightWidget>
struct TLeviathanCyanPurpleLight : TBase {
    TLeviathanCyanPurpleLight() {
        this->addBaseColor(nvgRGB(0x00, 0xc6, 0xe4));  // Cyan
        this->addBaseColor(nvgRGB(0xa8, 0x62, 0xff));  // Purple
    }
};
```

---

## 5. Performance Optimization

### Audio Thread Constraints

**DO NOT in `process()`:**
- ❌ Allocate memory (`new`, `malloc`, `std::vector::push_back`)
- ❌ File I/O
- ❌ String formatting
- ❌ `std::unordered_map` lookups
- ❌ Block on locks or I/O

**DO in `process()`:**
- ✅ Use LUTs instead of `powf()`, `expf()`, `sinf()`
- ✅ Cache computed values
- ✅ Use `std::max()`/`std::min()` instead of `clamp()`
- ✅ Minimize branches
- ✅ Pre-allocate buffers in constructor

### Fast Math Approximations

```cpp
namespace levi_math {

inline float fastTanh(float x) {
    const float x2 = x * x;
    if (x2 < 9.f) {
        return x * (27.f + x2) / (27.f + 9.f * x2);
    }
    return (x > 0.f) ? 1.f : -1.f;
}

inline float fastExp2(float x) {
    return rack::dsp::exp2_taylor5(clamp(x, -24.f, 24.f));
}

inline float fastLog2(float x) {
    union { float f; uint32_t i; } vx = {x};
    float y = (float)vx.i;
    y *= 1.1920928955078125e-7f;
    return y - 126.94269504f;
}

} // namespace levi_math
```

### Caching Pattern

```cpp
struct ChannelState {
    bool warpScaleValid = false;
    float cachedShapeSigned = 0.f;
    float cachedWarpScale = 1.f;
    
    bool stageTimeValid = false;
    float cachedRiseKnob = 0.f;
    float cachedFallKnob = 0.f;
    float cachedRiseTime = 0.01f;
    float cachedFallTime = 0.01f;
};

void updateCachedValues() {
    if (!cachedWarpScaleValid || cachedShape != shape) {
        cachedShape = shape;
        cachedWarpScale = computeWarpScale(shape);
        cachedWarpScaleValid = true;
    }
}
```

### Pre-allocation Pattern

```cpp
struct Module : Module {
    Module() {
        // Pre-allocate all buffers
        buffer.resize(MAX_BUFFER_SIZE);
        fftBuffer.resize(FFT_SIZE);
        windowBuffer.resize(FFT_SIZE);
    }
    
    std::vector<float> buffer;
    std::vector<float> fftBuffer;
    std::vector<float> windowBuffer;
};
```

### Performance Profiling

```cpp
// Audio thread profiling
void process(const ProcessArgs& args) override {
    const uint64_t perfStart = std::chrono::steady_clock::now();
    
    // ... audio processing ...
    
    const uint64_t elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - perfStart).count();
    
    perfAudioProcessNs.fetch_add(elapsedNs, std::memory_order_relaxed);
    perfAudioSampledCount.fetch_add(1u, std::memory_order_relaxed);
}

// UI thread profiling
void step() override {
    const uint64_t stepStart = std::chrono::steady_clock::now();
    ModuleWidget::step();
    
    const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - stepStart).count()) * 1e-6f;
    
    uiStepMsEma = (uiStepMsEma > 0.f) 
        ? (uiStepMsEma + (stepMs - uiStepMsEma) * 0.18f) 
        : stepMs;
}
```

---

## 6. Testing and Debugging

### Unit Testing

**Test structure:**

```cpp
struct TestResult {
    std::string name;
    bool pass = false;
    std::string detail;
};

TestResult testSomeFunctionality() {
    // Setup
    Engine engine;
    engine.reset(48000.f);
    
    // Execute
    engine.process(input);
    
    // Verify
    bool pass = engine.output == expectedOutput;
    return {"Description", pass, "detail message"};
}

int main() {
    std::vector<TestResult> tests;
    tests.push_back(testSomeFunctionality());
    
    int passed = 0, failed = 0;
    for (const auto& test : tests) {
        if (test.pass) {
            printf("[PASS] %s\n", test.name.c_str());
            passed++;
        } else {
            printf("[FAIL] %s: %s\n", test.name.c_str(), test.detail.c_str());
            failed++;
        }
    }
    
    printf("\n%d/%d tests passed\n", passed, tests.size());
    return failed > 0 ? 1 : 0;
}
```

**Build and run:**
```bash
make test-fast  # Runs all tests in tests/*_spec.cpp
```

### Debug Terminal System

**External server** (`tools/debug_terminal/server.py`) receives UDP packets:

```cpp
// Submit metrics from UI widgets
void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 TimingRangeUs processUs,
                                 TimingRangeUs stepUs,
                                 TimingRangeUs drawUs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid);
```

**Configuration** (`res/dragonking.txt`):
```json
{
  "debug": true,
  "PreviewWidgetOptions": false,
  "clockworkDragLogging": false,
  "temporalDeckLifetimeLogging": false,
  "moduleTeardownLogging": false
}
```

### Runtime Feature Flags

```cpp
bool isDragonKingDebugEnabled();
bool isDragonKingPreviewWidgetOptionsEnabled();
bool isClockworkDragDebugLoggingEnabled();
bool isTemporalDeckLifetimeLoggingEnabled();
bool isModuleTeardownLoggingEnabled();
```

---

## 7. Common Patterns and Anti-Patterns

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

### Release Compatibility

For released modules (Integral Flux, Proc, Temporal Deck):

- **Parameter/input/output/light IDs must not be reordered**
- **Patch state serialization format must remain stable**
- **Append to enum lists rather than inserting in middle**

---

## Additional Resources

### Project Structure

```
Leviathan/
├── src/                    # Source files
│   ├── plugin.cpp         # Plugin initialization
│   ├── plugin.hpp         # Plugin declarations
│   ├── VisualAssets.cpp   # Custom widgets
│   ├── VisualAssets.hpp
│   ├── PanelSvgUtils.cpp  # SVG anchor parsing
│   ├── PanelSvgUtils.hpp
│   ├── ModuleName.cpp     # Module implementation
│   └── ModuleName.hpp
├── res/                    # Resources
│   ├── ModuleName.svg     # SVG panels
│   └── panel_anchor_atlas.bin
├── tests/                  # Unit tests
│   └── module_spec.cpp
└── tools/
    └── debug_terminal/    # Debug terminal server
```

### Build Commands

```bash
make           # Build plugin
make test-fast # Run unit tests
```

### Thread-Safe Communication Pattern

```cpp
// Module (audio thread writes, UI thread reads)
struct PreviewSharedState {
    std::atomic<float> riseTime {0.01f};
    std::atomic<float> fallTime {0.01f};
    std::atomic<uint32_t> version {1};
};

void publishPreviewState(PreviewSharedState& shared, 
                        float riseTime, float fallTime) {
    shared.riseTime.store(riseTime, std::memory_order_relaxed);
    shared.fallTime.store(fallTime, std::memory_order_relaxed);
    shared.version.fetch_add(1, std::memory_order_relaxed);  // Publish!
}

// UI widget (reads via atomics)
void step() override {
    float riseTime = shared.riseTime.load(std::memory_order_relaxed);
    float fallTime = shared.fallTime.load(std::memory_order_relaxed);
    uint32_t version = shared.version.load(std::memory_order_relaxed);
    
    if (version != lastVersion) {
        rebuildPreview();
        lastVersion = version;
    }
}
```

---

*This guide was compiled from deep research into VCV Rack 2 plugin development techniques and real-world implementations in the Leviathan Rack Plugin Suite.*
