# Undertow Implementation Master

This document is the canonical implementation guide for `UNDERTOW`, a Leviathan-native sub-harmonic VCO. It consolidates:

- `doc/undertow.md`
- `doc/undertow-refined.md`
- `doc/undertow-dr.md`
- `doc/undertow-gem-research.md`

Use this document as the primary build reference. The older files remain useful background, but this file resolves the major conflicts between them.

## 1. Product Identity

`UNDERTOW` is a compact, sub-forward oscillator with three public audio voices:

- `SINE`: clean fundamental, triangle-derived rather than mathematically sterile.
- `SHAPE`: main timbre output, fundamental-forward with controlled even and odd harmonic growth.
- `SUB`: one-octave-down rectangular sub output with independent gate/reset behavior.

The module should feel simple, immediate, bass-capable, and performance-oriented. It should not become a many-mode oscillator, wavetable oscillator, or general waveshaper.

The design may learn from compact analog sub-timbral oscillators, especially triangle-core architecture and variable shape behavior, but it must remain Leviathan-native in naming, artwork, panel language, and implementation.

## 2. Canonical v1 Decisions

These decisions supersede conflicting older notes.

### Implement In v1

- 8HP target width.
- `SINE`, `SHAPE`, `SUB` outputs.
- `COARSE`, `FINE`, `LIN FM`, and `SHAPE` controls.
- `V/OCT`, `EXPO`, `LIN FM`, `SHAPE CV`, `SYNC`, and `S-GATE` inputs.
- Triangle-core oscillator architecture.
- Triangle-to-sine shaper for `SINE`.
- Concrete smooth asymmetric fallback stage for `SHAPE`.
- Rectangular divide-by-two `SUB`.
- Hard sync.
- S-GATE reset/gate behavior for `SUB` only.
- One lightweight waveform display.
- Monophonic v1. Polyphony is explicitly deferred.

### Defer Or Exclude From Canonical v1

- `RIPPLE` front-panel control.
- `MIX` output.
- User-facing quality modes.
- Multiple display modes.
- Broad sub-mode families.
- Through-zero FM.
- Analog character/drift modes.
- Soft sync.
- Literal visual imitation of any hardware panel.

`RIPPLE` from the early Undertow spec is a good future Leviathan-flavored extension, but it conflicts with the more faithful compact sub-timbral architecture. Do not implement it in the canonical v1.

## 3. Panel And Control Surface

Target width: `8HP`.

Use the repo's established panel/SVG component placement helpers rather than hardcoded pixel offsets. The panel should follow this hierarchy:

```text
top:     title + waveform display
middle:  pitch and timbre controls
lower:   modulation and pitch inputs
bottom:  audio outputs
```

Recommended control set:

```text
COARSE      FINE
LIN FM      SHAPE

Inputs:
LIN FM      SHAPE CV
EXPO        V/OCT
SYNC        S-GATE

Outputs:
SINE        SHAPE        SUB
```

`SHAPE`, `SUB`, and `SINE` output ordering is acceptable if the final visual design reads better that way. Do not add a triangle or pulse output in v1.

## 4. Implementation Files

Expected first-pass files:

- `src/Undertow.hpp`: module class, voice state, display state, enum definitions, DSP helpers that are cheap and local.
- `src/Undertow.cpp`: module implementation and DSP processing.
- `src/UndertowWidget.cpp`: widget, panel component placement, display widget, context menu, model declaration.
- `res/Undertow.svg`: panel art with component layer entries.

Registration work:

- Add `extern Model* modelUndertow;` to `src/plugin.hpp`.
- Add `p->addModel(modelUndertow);` in `src/plugin.cpp`.
- Add a `plugin.json` module entry with slug `Undertow`, name `Undertow`, and tags including `Oscillator`.

Use the existing module split patterns in the repo where helpful. Undertow is unreleased, so a clean file split is preferred over compressing everything into one file.

## 5. Enum Order

Use this exact v1 enum order. Undertow is unreleased, but this removes ambiguity for implementation handoff.

```cpp
enum ParamId {
  COARSE_PARAM,
  FINE_PARAM,
  LIN_FM_PARAM,
  SHAPE_PARAM,
  PARAMS_LEN
};

enum InputId {
  V_OCT_INPUT,
  EXPO_INPUT,
  LIN_FM_INPUT,
  SHAPE_CV_INPUT,
  SYNC_INPUT,
  S_GATE_INPUT,
  INPUTS_LEN
};

enum OutputId {
  SINE_OUTPUT,
  SHAPE_OUTPUT,
  SUB_OUTPUT,
  OUTPUTS_LEN
};

enum LightId {
  SYNC_LIGHT,
  S_GATE_LIGHT,
  LIGHTS_LEN
};
```

The lights are optional visually, but keeping them in the enum from v1 gives the display/widget a stable place to expose sync/sub activity if desired.

## 6. Parameters

Register parameters in a stable enum order. Since Undertow is unreleased, ordering may be chosen cleanly now, but avoid churn after implementation begins.

| Parameter | Range | Default | Notes |
| --- | ---: | ---: | --- |
| `COARSE_PARAM` | implementation-defined pitch range | around bass/mid | Main pitch. Prefer musical range over literal raw voltage UI. |
| `FINE_PARAM` | `-0.2083` to `+0.2083` V | `0` | About +/-2.5 semitones. |
| `LIN_FM_PARAM` | `0` to `1` | `0` | Unipolar FM level, not attenuverter. |
| `SHAPE_PARAM` | `0` to `1` | `0` or slightly above | Manual shape when CV unpatched; attenuator when `SHAPE CV` patched. |

Recommended pitch mapping:

- Internal pitch reference: Rack convention `C4 = 0V`.
- `COARSE` may be stored in pitch volts or mapped from Hz. The UI label can show Hz if useful.
- Useful no-CV default should land in a bass/mid register, approximately A2/C3 territory.

Context menu v1:

```text
Coarse Tune
  Continuous
  Octave Stepped
```

If octave-stepped mode is implemented, snap the computed coarse pitch contribution, not the stored parameter value.

## 7. Ports

Inputs:

| Input | Behavior |
| --- | --- |
| `V_OCT_INPUT` | Rack-standard 1V/oct pitch input. |
| `EXPO_INPUT` | Bipolar exponential FM/transposition input. Add before exponential pitch conversion. |
| `LIN_FM_INPUT` | AC-coupled linear FM input. Scaled by unipolar `LIN FM` level. |
| `SHAPE_CV_INPUT` | Unipolar shape CV. Treat `0..8V` as canonical range. |
| `SYNC_INPUT` | Rising-edge hard sync for main core. |
| `S_GATE_INPUT` | Rising edge resets/enables sub; low gates sub off when patched. |

Outputs:

| Output | Target |
| --- | --- |
| `SINE_OUTPUT` | Centered, roughly `10Vpp`. |
| `SHAPE_OUTPUT` | Centered, roughly `10Vpp`, DC controlled. |
| `SUB_OUTPUT` | Centered, roughly `10-12Vpp`. |

When `S_GATE_INPUT` is unpatched, `SUB_OUTPUT` runs continuously.

## 8. Voice State

Use a dedicated voice state even for mono v1.

```cpp
struct UndertowVoice {
  float phase = 0.f;          // [0, 1)
  float tri = 0.f;            // current triangle core value
  bool subFlip = false;       // divide-by-two state
  bool subGateHigh = true;    // effective sub gate state

  float linFmHpState = 0.f;   // AC coupling state for linear FM
  float shapeDcX1 = 0.f;      // SHAPE DC blocker input history
  float shapeDcY1 = 0.f;      // SHAPE DC blocker output history

  dsp::SchmittTrigger syncTrig;
  dsp::SchmittTrigger sGateTrig;

  float subBlepResidual = 0.f;
  float syncBlepResidual = 0.f;
};
```

Polyphony is deferred beyond v1. Keep the voice struct clean enough that a later `std::array<UndertowVoice, 16>` conversion is straightforward, but do not implement poly behavior in the first pass.

## 9. Pitch Path

Compute pitch per sample so audio-rate FM behaves correctly.

```cpp
float pitchV = coarsePitchV + finePitchV + vOct + expoIn;
float baseFreq = dsp::FREQ_C4 * dsp::approxExp2_taylor5(pitchV);
```

Use the repo/Rack fast exponential path rather than `std::pow()` in the shipped audio loop.

Linear FM:

```cpp
float lin = acCoupleLinFm(inputs[LIN_FM_INPUT].getVoltage(channel), voice);
float linAmt = params[LIN_FM_PARAM].getValue(); // 0..1
float linDrive = linAmt;

// Upper range gets a controlled overdrive-like compression rather than unlimited deviation.
float linScaled = lin * linDrive * kLinFmScale;
float freq = clamp(baseFreq + baseFreq * linScaled, kMinFreqHz, kMaxFreqHz);
```

Recommended constants:

```cpp
static constexpr float kMinFreqHz = 8.f;
static constexpr float kMaxFreqHz = 20000.f;
static constexpr float kLinFmScale = 0.10f;
```

AC-couple `LIN FM` with a one-pole highpass. Start around `5Hz` to remove DC without weakening audio-rate modulation.

## 10. Core Oscillator

Use a triangle-first core. Do not make the canonical implementation a pure sine LUT oscillator.

The cheapest acceptable implementation is a phase accumulator plus derived triangle:

```cpp
phase += freq * sampleTime;
bool wrapped = phase >= 1.f;
if (wrapped) {
  phase -= 1.f;
}

float tri = 4.f * std::fabs(phase - 0.5f) - 1.f;
```

Preferred refinement:

- generate the triangle from a bandlimited square/integrator model, or
- apply targeted BLAMP/PolyBLEP correction where discontinuities enter.

The direct phase triangle is acceptable for first bring-up. Before release, evaluate aliasing at high pitch, sync, and aggressive shape settings.

## 11. Sine Path

`SINE` should be derived from the triangle core through a calibrated shaper. It should be clean but not mathematically sterile.

Start with a low-cost polynomial triangle-to-sine approximation:

```cpp
inline float triToSine(float x) {
  // x in [-1, 1]. Tuned shaper, not final sacred math.
  const float x2 = x * x;
  return x * (1.5707963f - 0.6459641f * x2 + 0.0796926f * x2 * x2);
}
```

Normalize and tune by ear/scope so:

- `SINE` is roughly `10Vpp`.
- residual harmonics are low but not necessarily zero.
- no DC blocker is used on the sine output.

Output:

```cpp
float sine = triToSine(tri);
outputs[SINE_OUTPUT].setVoltage(5.f * sine, channel);
```

## 12. Shape Path

The `SHAPE` output is the identity voice. It must not be a plain sine-to-triangle crossfade and should not be only additive `h2/h3` injection.

Canonical v1 direction:

- Start from the sine-shaped node.
- Generate an alternate kinked/asymmetric triangle-like trajectory.
- Use a concrete smooth asymmetric fallback for the first implementation.
- Preserve the fundamental.
- Produce both even and odd harmonics.
- Apply DC control after the shape stage.

### Shape CV Semantics

`SHAPE_PARAM` has dual behavior:

```cpp
float shape;
if (inputs[SHAPE_CV_INPUT].isConnected()) {
  float cv = clamp(inputs[SHAPE_CV_INPUT].getVoltage(channel) / 8.f, 0.f, 1.f);
  shape = clamp(params[SHAPE_PARAM].getValue() * cv, 0.f, 1.f);
} else {
  shape = clamp(params[SHAPE_PARAM].getValue(), 0.f, 1.f);
}
```

This is intentionally not offset-plus-attenuverter. The tooltip should state: `Manual shape when SHAPE CV is unpatched; attenuates incoming SHAPE CV when patched.`

### Required v1 Fallback Algorithm

Implement this first. Do not leave `SHAPE` as placeholder functions.

```cpp
float sine = triToSine(tri);
float shapeCurve = shape * shape;

// Positive half gets a different bend from the negative half. This is the
// cheap v1 route to both even and odd harmonics without a hard splice.
float asym = tri + 0.22f * shape * (1.f - tri * tri);
float cubic = asym * asym * asym;
float kink = asym - 0.35f * shape * clamp(cubic, -1.f, 1.f);

// Add a small phase-dependent crease near high SHAPE values. It gives the
// endpoint a glitched-triangle feel while keeping the first implementation
// continuous enough to avoid a mandatory oversampling block.
float creasePhase = 1.f - 4.f * std::fabs(phase - 0.5f) * (1.f - std::fabs(phase - 0.5f));
float crease = clamp(creasePhase, -1.f, 1.f) * shapeCurve * 0.16f;
float alt = clamp(kink + crease, -1.2f, 1.2f);

float shaped = crossfade(sine, alt, shapeCurve);
shaped = dcBlockShape(shaped, voice);
```

Normalize or soft-limit after tuning if the shape path exceeds the target voltage range. This formula is intentionally modest; it creates a usable first version that can be measured, heard, and iterated.

Acceptance for this fallback:

- `shape = 0`: output should match `SINE` closely.
- mid `shape`: output has audible harmonic motion without losing pitch center.
- high `shape`: output is brighter and more angular, with both even and odd harmonics.
- persistent `SHAPE` DC after blocking stays below `50mV`.
- `SHAPE` output stays roughly `9.0-10.5Vpp` after final output scaling.

### Deferred Splice Model

The more faithful switch/splice model belongs after the fallback is working:

```cpp
float sine = triToSine(tri);
float alt = glitchTriangleCandidate(phase, tri, sine, shape);
float window = shapeSpliceWindow(phase, shape);
float shaped = crossfade(sine, alt, window);
shaped = dcBlockShape(shaped, voice);
```

Only move to this model after the v1 fallback is stable and there is time to handle anti-aliasing at the splice boundary. If the splice has a hard step, apply PolyBLEP/minBLEP correction or local oversampling.

## 13. Minimum Viable DSP Loop

The first working implementation should follow this exact structure before adding refinements:

```cpp
void processVoice(const ProcessArgs& args, UndertowVoice& voice) {
  float pitchV = coarsePitchV() + finePitchV() + inputs[V_OCT_INPUT].getVoltage();
  pitchV += inputs[EXPO_INPUT].isConnected() ? inputs[EXPO_INPUT].getVoltage() : 0.f;

  float baseFreq = dsp::FREQ_C4 * dsp::approxExp2_taylor5(pitchV);
  float lin = acCoupleLinFm(inputs[LIN_FM_INPUT].getVoltage(), voice);
  float freq = clamp(baseFreq + baseFreq * lin * params[LIN_FM_PARAM].getValue() * kLinFmScale,
                     kMinFreqHz, kMaxFreqHz);

  bool syncRising = voice.syncTrig.process(inputs[SYNC_INPUT].getVoltage());
  if (syncRising) {
    voice.phase = 0.f;
  }

  voice.phase += freq * args.sampleTime;
  bool wrapped = voice.phase >= 1.f;
  if (wrapped) {
    voice.phase -= 1.f;
  }

  float tri = 4.f * std::fabs(voice.phase - 0.5f) - 1.f;
  float sine = triToSine(tri);
  float shape = effectiveShapeAmount();
  float shaped = shapeFallback(voice.phase, tri, sine, shape, voice);

  if (wrapped) {
    voice.subFlip = !voice.subFlip;
  }
  float sub = computeSubOutput(voice);

  outputs[SINE_OUTPUT].setVoltage(5.f * sine);
  outputs[SHAPE_OUTPUT].setVoltage(5.f * shaped);
  outputs[SUB_OUTPUT].setVoltage(6.f * sub);
}
```

This is intentionally mono and omits final anti-alias correction. Add sub/sync correction after this basic loop is functional and tested.

## 14. DC Blocking

Apply DC blocking to `SHAPE` only.

```cpp
inline float dcBlockShape(float x, UndertowVoice& voice) {
  static constexpr float R = 0.9993f; // about 5 Hz at 44.1 kHz
  float y = x - voice.shapeDcX1 + R * voice.shapeDcY1;
  voice.shapeDcX1 = x;
  voice.shapeDcY1 = y;
  return y;
}
```

Do not DC-block `SINE`; it should remain stable and clean for downstream FM/filter use.

## 15. Sub Path

`SUB` is a rectangular divide-by-two signal one octave below the main oscillator.

```cpp
if (wrapped) {
  voice.subFlip = !voice.subFlip;
}

float subRaw = voice.subFlip ? 1.f : -1.f;
```

S-GATE behavior:

```cpp
bool sGatePatched = inputs[S_GATE_INPUT].isConnected();
float sGateV = sGatePatched ? inputs[S_GATE_INPUT].getVoltage(0) : 10.f;
bool sGateHigh = sGateV >= 1.f;
bool sGateRising = voice.sGateTrig.process(sGateV);

if (sGateRising) {
  voice.subFlip = false;
}

float sub = (!sGatePatched || sGateHigh) ? subRaw : 0.f;
```

S-GATE affects only `SUB`. It does not reset `SINE`, `SHAPE`, or the main oscillator phase.

Output target:

```cpp
outputs[SUB_OUTPUT].setVoltage(6.f * sub, channel);
```

Use `5.f` instead if testing shows `12Vpp` is too dominant for Rack patches, but start with a strong sub.

## 16. Sync

`SYNC` is hard sync only in v1.

```cpp
bool syncRising = voice.syncTrig.process(inputs[SYNC_INPUT].getVoltage(0));
if (syncRising) {
  // Capture pre-reset state if using correction.
  voice.phase = 0.f;
}
```

Do not reset `subFlip` on main sync for v1. Sub reset is owned by S-GATE.

Anti-aliasing:

- Minimum: avoid obvious clicks or broken output on sync.
- Preferred: PolyBLEP/minBLEP/BLAMP-style correction around reset discontinuities.
- Defer user-facing sync modes.

## 17. Anti-Aliasing Strategy

Target anti-alias effort where Undertow actually creates discontinuities or nonlinear bandwidth:

| Risk | v1 Strategy | Later Upgrade |
| --- | --- | --- |
| `SUB` square edges | PolyBLEP | 2x sub oversampling |
| hard sync reset | PolyBLEP/minBLEP or BLAMP residual | better measured correction |
| SHAPE splice/switch | smooth transfer or boundary BLEP | local 2x/4x shaper oversampling |
| high-pitch triangle | evaluate direct triangle; improve if needed | integrated bandlimited square core |
| nonlinear SHAPE | keep bounded and smooth | ADAA or local oversampling |

Do not expose quality tiers in v1. Ship one good default.

## 18. Polyphony Policy

Mono v1 is a hard decision. Do not implement polyphony in the first pass.

Later polyphony policy:

- `V/OCT` channel count determines output channel count.
- `EXPO`, `LIN FM`, and `SHAPE CV` may be poly; mono CVs broadcast.
- `SYNC` and `S-GATE` should broadcast mono unless a clear poly behavior is implemented.
- `SINE`, `SHAPE`, and `SUB` outputs must all have the same channel count.
- Display remains voice 0 only.

Do not partially implement polyphony later.

## 19. Display Architecture

Use one display mode in v1: waveform preview.

Display should show:

- `SHAPE` trace as the primary line.
- faint `SINE` reference.
- small `SUB` activity indicator.
- short sync flash/tick.

No OpenGL path is required for v1.

### Display Data Contract

No audio-thread allocations. No locks. Use double-buffered display snapshots.

```cpp
static constexpr int DISPLAY_SAMPLES = 128;
static constexpr int DISPLAY_DECIMATE = 32;

struct UndertowDisplayBuffer {
  float shape[DISPLAY_SAMPLES] = {};
  float sine[DISPLAY_SAMPLES] = {};
  bool subHigh = false;
  bool syncFlash = false;
};

UndertowDisplayBuffer displayBufs[2];
std::atomic<int> displayReadIdx {0};
int displayWriteIdx = 1;
int displayFillPos = 0;
int displayDecimator = 0;
```

Audio thread writes only to the inactive buffer. When full, publish the readable index with `memory_order_release`. UI reads with `memory_order_acquire`.

Only voice 0 should feed the display in v1.

### UI Draw

Use a custom `Widget` with NanoVG. Wrap in `FramebufferWidget` if the display can redraw at a capped cadence without looking stale.

Draw order:

1. Opaque display background.
2. Faint center/reference grid if desired.
3. Faint sine reference.
4. Primary shape waveform.
5. Sub activity bar/dot.
6. Sync flash.

Use Leviathan palette constants or local module style helpers. Do not hardcode a foreign hardware color identity.

## 20. Serialization

Rack serializes parameters automatically. Serialize only non-param state.

Recommended v1 JSON:

```cpp
json_t* dataToJson() override {
  json_t* root = json_object();
  json_object_set_new(root, "coarseTuneMode", json_integer(coarseTuneMode));
  return root;
}

void dataFromJson(json_t* root) override {
  json_t* j = json_object_get(root, "coarseTuneMode");
  if (j) {
    coarseTuneMode = clamp(int(json_integer_value(j)), 0, 1);
  }
}
```

Keep schema simple. Undertow is unreleased, but still avoid unnecessary state churn once patches exist.

## 21. Performance Rules

Hot audio path:

- No allocations.
- No locks.
- Avoid scalar `std::pow`, `std::sin`, `std::tanh`, `std::exp` in per-sample code.
- Prefer `dsp::approxExp2_taylor5`, small LUTs, polynomial shapers, cached coefficients, and simple one-pole filters.
- Keep anti-alias correction targeted rather than oversampling the whole module by default.

Display path:

- decimate audio data before publishing.
- UI reads snapshots only.
- no expensive UI recomputation from audio buffers.

## 22. Testing And Validation

Functional:

- `SINE`, `SHAPE`, and `SUB` outputs produce expected voltages with no CV patched.
- `V/OCT` tracks within `+/-2 cents` over at least 5 octaves.
- `SYNC` hard-resets the main phase reliably.
- `S-GATE` resets/enables/disables only the sub path.
- `SHAPE CV` changes knob semantics correctly.
- `LIN FM` is AC-coupled and does not drift pitch from DC input.
- JSON state round-trips.

Audio:

- `SINE` is roughly `10Vpp`, centered, and clean.
- `SHAPE` remains sine-like at low values.
- `SHAPE` gains both even and odd harmonics at higher values.
- `SHAPE` stays fundamental-forward.
- `SUB` is strong and stable.
- hard sync and high SHAPE do not produce unacceptable aliasing.

Performance:

- no audio-thread allocations under steady processing.
- single instance is cheap in Rack CPU meter.
- many instances remain practical.
- display does not cause audio xrun or UI stalls.

Visual:

- display responds to pitch and shape changes.
- sine reference is readable but secondary.
- sub and sync indicators are visible but not dominant.

## 23. Implementation Phases

Phase 1:

- module skeleton
- panel SVG/component layout
- params/ports
- mono phase core
- `SINE`, `SUB`, and direct fallback `SHAPE`

Phase 2:

- pitch path with `V/OCT`, `EXPO`, `LIN FM`
- AC-coupled linear FM
- shape CV dual semantics
- output scaling

Phase 3:

- required fallback SHAPE algorithm
- SHAPE DC block
- S-GATE behavior
- hard sync

Phase 4:

- PolyBLEP/minBLEP/BLAMP correction for sub/sync/shape discontinuities
- optional advanced splice SHAPE model
- waveform display
- coarse tune context menu
- serialization

Phase 5:

- tuning by ear and scope
- CPU profiling
- aliasing review
- confirm polyphony remains deferred or schedule it as a separate follow-up
- panel art polish
- `make test-fast` and focused oscillator tests where practical

## 24. Open Tuning Decisions

These are not implementation blockers:

- exact triangle-to-sine transfer constants
- exact SHAPE endpoint geometry
- how hard/glitchy maximum SHAPE should be
- whether SHAPE needs local oversampling before release
- whether `SUB` should ship at `10Vpp` or `12Vpp`
- whether polyphony belongs in a later release
- exact display styling

Resolve these with audio in the loop.

## 25. Non-Goals

Do not:

- copy another manufacturer's panel or brand language.
- add `RIPPLE` to canonical v1.
- add a generic wavefolder mode.
- add a triangle or pulse output.
- expose internal calibration trimmers as normal controls.
- ship multiple quality menus before a single good default exists.
- put waveform visualization work in the audio hot path.
- implement polyphony in v1.

## 26. Canonical Summary

Build Undertow as a compact sub-harmonic VCO: triangle at the core, shaped sine as the center, a variable asymmetric SHAPE output for animated harmonics, a strong separately gated sub, and a small efficient display. The first release should be narrow, stable, fast, and musically immediate.
