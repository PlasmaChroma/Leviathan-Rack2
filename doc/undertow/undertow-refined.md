# Undertow — Implementation Companion v0.1

> **Purpose of this document:** This is a concrete implementation reference to accompany
> `undertow.md` (the design spec). Where the spec describes *intent and direction*, this
> document describes *specific algorithms, data structures, numeric constants, and
> implementation decisions*. When the two conflict, the spec takes precedence on musical
> goals; this document takes precedence on implementation method.

---

## 1. Panel Dimensions

- **Width:** 8HP
- **Component placement:** follow the repo's standard HP grid and jack/knob sizing conventions
- **Display area:** occupies the top portion of the panel, approximately the height of two knob rows
- The ASCII layout in the spec is directional. All final `x`/`y` values should be derived from the repo's panel placement helpers, not hardcoded pixel offsets.

---

## 2. Parameters, Ranges, and Defaults

All parameter definitions below should be registered in the module's `configParam` calls with these exact ranges and defaults. Names are as specified in the spec.

| Parameter   | Type         | Min    | Max    | Default | Unit / Notes                         |
|-------------|--------------|--------|--------|---------|--------------------------------------|
| `COARSE`    | Knob         | -4.0   | +4.0   | -1.0    | Volts offset added to V/OCT          |
| `FINE`      | Knob         | -0.083 | +0.083 | 0.0     | Volts offset (~1 semitone each way)  |
| `SHAPE`     | Knob         | 0.0    | 1.0    | 0.15    | Normalized, see §5                   |
| `RIPPLE`    | Knob         | 0.0    | 1.0    | 0.1     | Normalized, see §5                   |
| `LIN_FM_CV` | Attenuverter | -1.0   | +1.0   | 0.0     | Multiplied against LIN FM jack       |
| `SHAPE_CV`  | Attenuverter | -1.0   | +1.0   | 0.0     | Multiplied against SHAPE CV jack     |

**Why COARSE defaults to -1.0 V:** This places the oscillator around A2 (~110 Hz) with no V/OCT patched, which immediately communicates its sub-forward identity.

---

## 3. Input and Output Ports

Declare these in the order shown. The spec names are canonical.

**Inputs:** `V_OCT`, `LIN_FM`, `SHAPE_CV`, `SYNC`, `S_GATE`

**Outputs:** `SINE_OUT`, `SHAPE_OUT`, `SUB_OUT`

All outputs target **±5 V** (10 Vpp, centered at 0 V). The sub output may peak slightly higher (up to ±6 V / 12 Vpp) — this is intentional per the spec.

---

## 4. Per-Voice State

Use a dedicated struct for all per-voice DSP state. This is the canonical struct for v1:

```cpp
struct OscVoice {
    float phase       = 0.f;     // [0.0, 1.0)
    bool  subFlip     = false;   // divide-by-two state
    float dcState     = 0.f;     // DC blocking filter state (SHAPE output only)

    dsp::SchmittTrigger syncTrig;
    dsp::SchmittTrigger sGateTrig;

    // PolyBLEP correction state
    float polyBlepResidual = 0.f;  // carry-over from previous sample (sync/sub)
};
```

For monophonic v1, declare one `OscVoice voice` in the module struct.
For polyphonic implementation, use `std::array<OscVoice, 16>` and size to `inputs[V_OCT].getChannels()`.

---

## 5. Pitch Path

### 5.1 Frequency Computation

```cpp
// Per voice, per sample
float pitchV = params[COARSE].getValue()
             + params[FINE].getValue()
             + inputs[V_OCT].getVoltage(channel);  // channel = 0 for mono

float freq = 261.6255f * std::pow(2.0f, pitchV);  // C4 = 0V reference

// Linear FM: additive frequency deviation, depth-scaled
float linFmDepth = params[LIN_FM_CV].getValue();   // attenuverter [-1, 1]
float linFmIn    = inputs[LIN_FM].getVoltage(channel);
freq += freq * linFmDepth * linFmIn * 0.1f;        // 0.1 scaling factor keeps it musical

freq = clamp(freq, 8.18f, 20000.f);               // A0 to ~20kHz
```

**On `std::pow` in the pitch path:** For v1 this is acceptable — pitch is computed once per sample (not inside an oversampled inner loop), and accurate V/oct tracking is required. Replace with a fast approximation only if profiling shows it is measurably costly in context.

### 5.2 Phase Advance

```cpp
float phaseInc = freq * args.sampleTime;
voice.phase   += phaseInc;
bool wrapped   = voice.phase >= 1.0f;
if (wrapped) voice.phase -= 1.0f;
```

`wrapped` is used downstream by the sub divide-by-two and the PolyBLEP correction.

---

## 6. Sine Core

Use a **512-entry lookup table** with **linear interpolation**. Build it once at module load:

```cpp
// Module-level or plugin-level static
static float sineTable[513];  // 513rd entry = sineTable[0] to close the loop

static void buildSineTable() {
    for (int i = 0; i <= 512; ++i)
        sineTable[i] = std::sin(2.f * M_PI * i / 512.f);
}
```

Lookup:

```cpp
inline float lut_sin(float phase) {
    // phase in [0.0, 1.0)
    float idx  = phase * 512.f;
    int   i    = (int)idx;
    float frac = idx - (float)i;
    return sineTable[i] * (1.f - frac) + sineTable[i + 1] * frac;
}
```

This is accurate to better than -80 dB THD, fast, and avoids `std::sin` in the audio loop. Upgrade to a higher-order (Hermite) interpolation only if harmonic analysis shows it is needed.

---

## 7. Shape Output Algorithm

The SHAPE output is **not** a sine-to-triangle crossfade. It is a sine core with
additive harmonic injection and a RIPPLE-driven soft-fold layer. Each stage is described
below.

### 7.1 Harmonic Injection (SHAPE control)

```cpp
float s  = lut_sin(voice.phase);           // fundamental
float h2 = lut_sin(fmod(voice.phase * 2.f, 1.f));  // 2nd harmonic
float h3 = lut_sin(fmod(voice.phase * 3.f, 1.f));  // 3rd harmonic

// Fixed harmonic mix — these ratios are tuned for musical behavior.
// 2nd harmonic adds "warm, resonant" character.
// 3rd harmonic adds "hollow, organlike" body.
// Together they keep the fundamental perceptually dominant.
float harmLayer = h2 * 0.45f + h3 * 0.30f;

// SHAPE blends from pure sine toward the harmonic layer.
// Use a squared curve for a more gradual lower half.
float shapeCurved = shape * shape;
float blended = s + shapeCurved * harmLayer;
```

`shape` here is the effective shape value after CV mixing:

```cpp
float shape = clamp(
    params[SHAPE].getValue() + params[SHAPE_CV].getValue() * inputs[SHAPE_CV_IN].getVoltage() * 0.1f,
    0.f, 1.f
);
```

### 7.2 Ripple Layer (RIPPLE control)

RIPPLE applies a bounded soft-fold nonlinearity on top of the blended signal. It
adds upper harmonics and a slight angular quality without crossing into hard clipping.

```cpp
// Drive amount increases quadratically so the lower half of the knob stays restrained
float rippleDrive = 1.f + ripple * ripple * 2.0f;  // range: [1.0, 3.0]
float driven      = blended * rippleDrive;

// Cubic soft clip: maps input smoothly toward [-1, 1]
// Formula: y = (3/2)x - (1/2)x³, valid for |x| <= 1
// Clamp input first to keep it in the cubic's well-behaved range
driven       = clamp(driven, -1.5f, 1.5f);
float folded = 1.5f * driven - 0.5f * driven * driven * driven;
```

At `ripple = 0`, `rippleDrive = 1.0` and `driven == blended`, so the cubic
produces near-unity output with no distortion (since `blended` is already bounded
by construction from the harmonic injection). The cubic only adds audible distortion
as RIPPLE increases.

### 7.3 DC Blocking

The harmonic injection can produce small DC offsets depending on waveform asymmetry.
Apply a 1-pole DC blocking highpass to the SHAPE output only:

```cpp
// Coefficient for ~5 Hz highpass at 44.1 kHz: R = 1 - (2π * 5 / 44100)
static constexpr float DC_BLOCK_R = 0.9993f;

inline float dcBlock(float x, float& state) {
    float y = x - state;
    state   = x - DC_BLOCK_R * y;  // IIR highpass, effectively: state += (x - y)
    return y;
}

float shapeOut = dcBlock(folded, voice.dcState);
```

Do not apply DC blocking to the SINE output — it must remain clean for FM use.

### 7.4 Output Scaling

```cpp
outputs[SINE_OUT].setVoltage(s * 5.f);
outputs[SHAPE_OUT].setVoltage(shapeOut * 5.f);
```

---

## 8. Sub Oscillator

### 8.1 Default Mode: Square at -1 Octave

The sub is a divide-by-two square derived from the main phase wrap:

```cpp
if (wrapped) {
    voice.subFlip = !voice.subFlip;
}
float subRaw = voice.subFlip ? 1.f : -1.f;
```

### 8.2 S-GATE Behavior

```cpp
bool gateRising = voice.sGateTrig.process(inputs[S_GATE].getVoltage());
bool gateHigh   = inputs[S_GATE].getVoltage() >= 1.f;

if (gateRising) {
    // Reset sub state on rising edge
    voice.subFlip = false;
}

// Gate high: pass sub signal. Gate low or unpatched: pass sub signal unaffected.
// When S-GATE is patched, gate-low silences the sub.
// When S-GATE is unpatched, sub always passes.
float subOut = 0.f;
if (!inputs[S_GATE].isConnected() || gateHigh) {
    subOut = subRaw;
}
```

**Why this gating policy:** A rising edge resets phase coherence. Gate-high enables
the output. When unpatched, the sub is always active — this matches the default
patch behavior described in the spec.

### 8.3 Sub Anti-Aliasing (PolyBLEP)

The sub square transitions are a significant aliasing risk. Apply a **first-order
PolyBLEP residual correction** at each transition point.

A square wave has transitions at `phase = 0` (rising) and `phase = 0.5` (falling),
but the sub fires only at every other wrap. Track transition polarity:

```cpp
// In the sub output path, apply PolyBLEP at the flip point
// The flip happens when `wrapped == true` and subFlip just changed.
// PolyBLEP correction for a unit-height square transition:

float polyBlep(float phase, float phaseInc) {
    if (phase < phaseInc) {
        float t = phase / phaseInc;
        return t + t - t * t - 1.f;   // correction for rising edge
    } else if (phase > 1.f - phaseInc) {
        float t = (phase - 1.f) / phaseInc;
        return t * t + t + t + 1.f;   // correction for falling edge
    }
    return 0.f;
}
```

Apply the correction to the raw sub square output. For v1, using a single-sample
PolyBLEP (not oversampled) is acceptable — it suppresses the worst aliasing without
CPU cost. If profiling allows, 2x oversampling of the sub path is a meaningful
quality upgrade for a later phase.

---

## 9. Hard Sync

On a rising edge at the SYNC input:

```cpp
bool syncRising = voice.syncTrig.process(inputs[SYNC].getVoltage());
if (syncRising) {
    // PolyBLEP-aware sync: record fractional position at the sync point for correction
    voice.polyBlepResidual = voice.phase / phaseInc;  // normalized fractional offset
    voice.phase   = 0.f;
    // Do NOT reset subFlip — sub continues from its current divide state.
    // If sub reset-on-sync is later desired, add it as a context menu option.
}
```

The discontinuity at sync creates an audible click at high frequencies. For v1,
apply the PolyBLEP residual from above to the SINE and SHAPE outputs on the
sample immediately following a sync event. This is not a perfect solution, but
it is the right cost-vs-quality tradeoff for a v1 with CPU efficiency as a constraint.

A more complete BLAMP (bandlimited ramp) correction should be considered in Phase 4
tuning if the sync artifact is musically unacceptable.

---

## 10. Display Architecture

### 10.1 Constraints

- **No audio-thread allocation.** The display buffer is pre-allocated.
- **No per-sample UI work.** Audio thread writes at a decimated rate.
- **Lock-free.** Use `std::atomic` for handoff.

### 10.2 Data Path

Declare this in the module struct:

```cpp
static constexpr int DISPLAY_SAMPLES = 128;

struct DisplayBuffer {
    float shape[DISPLAY_SAMPLES] = {};
    float sine[DISPLAY_SAMPLES]  = {};
    bool  subHigh                = false;
    bool  syncFlash              = false;
};

// Double-buffer: audio writes to `write`, UI reads from `read`
DisplayBuffer displayBufs[2];
std::atomic<int> displayReadIdx{0};
int              displayWriteIdx{1};
int              displayFillPos{0};
int              displayDecimator{0};
```

In the audio `process()` loop:

```cpp
static constexpr int DISPLAY_DECIMATE = 32;  // write one sample per 32 audio samples

if (++displayDecimator >= DISPLAY_DECIMATE) {
    displayDecimator = 0;
    DisplayBuffer& wb = displayBufs[displayWriteIdx];
    wb.shape[displayFillPos] = shapeOut;
    wb.sine[displayFillPos]  = s;

    if (++displayFillPos >= DISPLAY_SAMPLES) {
        displayFillPos = 0;
        wb.subHigh    = voice.subFlip;
        wb.syncFlash  = syncRising;
        // Publish: swap buffers atomically
        displayReadIdx.store(displayWriteIdx, std::memory_order_release);
        displayWriteIdx = 1 - displayWriteIdx;
    }
}
```

The UI widget reads `displayBufs[displayReadIdx.load(std::memory_order_acquire)]`
with no mutex. Because the UI only reads the inactive buffer and the audio only
writes the inactive buffer, there is no race, even without a full atomic copy.

### 10.3 Waveform Draw

In the widget's `draw()` call:

```cpp
const DisplayBuffer& db = module->displayBufs[module->displayReadIdx.load(...)];

// Draw faint sine reference first
nvgBeginPath(args.vg);
for (int i = 0; i < DISPLAY_SAMPLES; ++i) {
    float x = i / (float)(DISPLAY_SAMPLES - 1) * displayWidth;
    float y = centerY - db.sine[i] * halfHeight * 0.6f;  // scale down sine reference
    (i == 0) ? nvgMoveTo(args.vg, x, y) : nvgLineTo(args.vg, x, y);
}
nvgStrokeColor(args.vg, nvgRGBAf(1.f, 1.f, 1.f, 0.18f));  // faint white
nvgStroke(args.vg);

// Draw shape waveform on top
nvgBeginPath(args.vg);
for (int i = 0; i < DISPLAY_SAMPLES; ++i) {
    float x = i / (float)(DISPLAY_SAMPLES - 1) * displayWidth;
    float y = centerY - db.shape[i] * halfHeight * 0.85f;
    (i == 0) ? nvgMoveTo(args.vg, x, y) : nvgLineTo(args.vg, x, y);
}
nvgStrokeColor(args.vg, primaryColor);
nvgStroke(args.vg);

// Sub activity: draw a low horizontal bar when sub is high
if (db.subHigh) {
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, displayHeight - subBarHeight, displayWidth, subBarHeight);
    nvgFillColor(args.vg, subIndicatorColor);
    nvgFill(args.vg);
}

// Sync flash: brief bright tick at left edge
if (db.syncFlash) {
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, 2.f, displayHeight);
    nvgFillColor(args.vg, syncFlashColor);
    nvgFill(args.vg);
}
```

Colors should use the Leviathan palette. Do not hardcode hex values here —
reference the design token constants from the repo.

---

## 11. Serialization

These fields must round-trip through `dataToJson` / `dataFromJson`:

```cpp
json_t* dataToJson() override {
    json_t* root = json_object();
    // No extra state needed for v1 beyond param values (which Rack serializes automatically)
    // If coarse tune mode (continuous vs octave-stepped) is added, serialize it here:
    // json_object_set_new(root, "coarseTuneMode", json_integer(coarseTuneMode));
    return root;
}
```

For v1, the only context-menu state is `coarseTuneMode` (int: 0 = continuous,
1 = octave-stepped). Include the serialization stub now, defaulting to `0`, so it
does not need a schema migration later.

---

## 12. Polyphony

**Recommendation: implement polyphony in v1** using the `std::array<OscVoice, 16>`
approach. The state struct is small (< 64 bytes per voice), the audio path is
already written per-voice, and poly adds meaningful value for a pitch oscillator.

```cpp
int channels = std::max(1, inputs[V_OCT].getChannels());

for (int c = 0; c < channels; ++c) {
    // All per-voice computation runs here, indexed by c
    // Mono CVs (SYNC, S_GATE, LIN_FM when not poly) broadcast to all voices:
    float voct     = inputs[V_OCT].getVoltage(c);
    float linFmIn  = inputs[LIN_FM].getVoltage(inputs[LIN_FM].isPolyphonic() ? c : 0);
    float shapeCvIn= inputs[SHAPE_CV_IN].getVoltage(inputs[SHAPE_CV_IN].isPolyphonic() ? c : 0);
    bool  syncSig  = inputs[SYNC].getVoltage(0);   // always mono-broadcast
    bool  sGateSig = inputs[S_GATE].getVoltage(0); // always mono-broadcast
    // ... compute and write outputs
}

outputs[SINE_OUT].setChannels(channels);
outputs[SHAPE_OUT].setChannels(channels);
outputs[SUB_OUT].setChannels(channels);
```

Display remains single-channel (voice 0) in v1. No change to display architecture.

If polyphony meaningfully complicates the schedule, drop it from v1 and merge it
in Phase 4. Do **not** partially implement it (e.g., poly V/OCT with mono sub output).
Either all three outputs are poly or none are.

---

## 13. Context Menu

v1 menu, implemented with standard Rack menu items:

```
──────────────────────────
 Coarse Tune
  ● Continuous
  ○ Octave Stepped
──────────────────────────
```

Octave-stepped behavior: snap `params[COARSE].getValue()` to the nearest integer
(whole volt = whole octave) when computing `pitchV`. The knob still moves
continuously; snapping is applied in the audio thread, not to the param itself.

---

## 14. Anti-Aliasing Summary

| Risk Point              | v1 Strategy                                      | Deferred Option          |
|-------------------------|--------------------------------------------------|--------------------------|
| Sub square transitions  | PolyBLEP (single-sample, no oversampling)        | 2x oversample sub path   |
| Hard sync discontinuity | PolyBLEP residual on SINE/SHAPE sample after sync| BLAMP correction         |
| Shape at high SHAPE+RIPPLE | Soft cubic clip keeps output bounded         | Oversampled shaper path  |
| High-pitch SINE aliasing| LUT resolution adequate to ~8kHz; acceptable above | Oversample entire core |

Do not implement multiple quality tiers or expose them to the user in v1.
The strategies above are the v1 shipped defaults.

---

## 15. Acceptance Criteria — Numeric Addendum

These complement §13 of the spec with concrete pass/fail values:

| Criterion                              | Target                                    |
|----------------------------------------|-------------------------------------------|
| V/oct tracking error                   | < ±2 cents over 5 octaves (A1–A6)        |
| SINE output amplitude                  | 9.5–10.5 Vpp at all pitches in range     |
| SHAPE output amplitude                 | 9.0–10.5 Vpp; no persistent DC > 50 mV  |
| SUB output amplitude                   | 10.0–12.0 Vpp                            |
| CPU cost (Release build, single instance) | < 3% of one core at 44.1 kHz on reference hardware |
| CPU cost (8 instances)                 | < 20% of one core                        |
| Display frame impact                   | No measurable audio xrun introduced by display at 60 fps |

---

## 16. What This Document Does Not Decide

The following remain open and should be resolved during implementation:

- Exact panel artwork, typography, and color tokens (defer to Leviathan design pass)
- Whether the PolyBLEP sync correction is audibly sufficient or needs BLAMP (measure in Phase 4)
- Exact sub indicator geometry (bar vs. pulse dot — visual judgment call in Phase 3)
- Whether `SHAPE` uses a squared or cubic response curve (start squared, tune by ear)
- Whether `h2 : h3` ratio of `0.45 : 0.30` is the best musical balance (start here, tune by ear)

Decisions above this line are **implementation-stable**. Decisions below this line
are **tuning-deferred** and should be iterated on with audio in the loop.
