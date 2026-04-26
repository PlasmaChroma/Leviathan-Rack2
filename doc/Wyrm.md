# Codex Spec: Custom Curve Oscillator Module

## Working Title

**Preferred working title:** **Wyrmform**

Rationale: The module’s identity is a drawn living waveform: a user-shaped curve that coils through phase space and becomes sound. “Wyrmform” fits the Leviathan plugin mythology without being too opaque in a module browser.

### Alternate naming options

- **Wyrmform** — mythic, waveform-adjacent, strong Leviathan branding.
- **Curveforge** — clear and functional; implies shaping/forging sound.
- **Sigil Oscillator** — magical/graphical; the waveform is a drawn sigil.
- **Runeform** — strong visual identity; waveform as rune/glyph.
- **Glyphosc** — compact and direct, but slightly less elegant.
- **Contour** — clean and musical, less branded.
- **Wavewright** — artisanal, friendly, clear.
- **Bendform** — emphasizes curvature/folding/modulation.

This spec will refer to the module as **Wyrmform** until renamed.

---

# 1. Module Summary

**Wyrmform** is a VCV Rack oscillator whose core waveform is defined by a user-editable periodic curve. The user edits **32 phase segments / control points** on a waveform grid. The grid’s horizontal axis represents one oscillator cycle. The vertical center line is **0V**, the top is **+5V**, and the bottom is **−5V**.

The module provides:

- A drawable 32-point periodic waveform editor.
- Built-in base shapes: sine, triangle, saw, reverse saw, pulse/square, and optional additional starter shapes.
- A lock/unlock editing model so users can browse stable shapes, then unlock and sculpt manually.
- Pitch input using **1V/oct**.
- FM input with built-in attenuation.
- Large frequency control for exact manual frequency dialing.
- Audio-rate mode and LFO mode.
- Optional integrated wavefolder.
- High-quality interpolation and bandlimiting.
- Emphasis priority:
  1. Audio quality.
  2. Usability.
  3. Performance.

The oscillator should be stable, musical, and suitable both as a conventional oscillator and as an experimental waveform sculptor.

---

# 2. Design Goals

## 2.1 Primary goals

1. **Custom waveform as first-class oscillator core**  
   The drawn curve is not a decorative waveshaper; it is the source waveform.

2. **Audio quality over naive drawing playback**  
   Do not simply step through 32 values at audio rate. The waveform must be interpolated into a smooth high-resolution representation, then bandlimited so sharp edges do not generate severe aliasing.

3. **Fast enough for Rack polyphony and patching**  
   Runtime oscillator playback should be cheap. More expensive waveform rebuild work should happen only when the shape changes, and should be throttled/deferred as needed.

4. **Immediate, tactile editing**  
   Drawing on the waveform grid should feel responsive. The sound should update quickly but not glitch violently during mouse movement.

5. **Musically useful factory shapes**  
   The base shapes should sound clean and calibrated. The user can use the module as a normal oscillator without drawing anything.

6. **Safe voltage behavior**  
   Nominal output range should be **±5V** before optional folder/output drive stages.

---

# 3. Proposed Panel / UI Layout

Target width suggestion: **14HP–18HP**.  
This module can be narrower if the editor is compact, but the curve editor is the soul of the module. Prefer giving the editor enough width to be comfortable.

Recommended layout:

1. **Top area:** module title + optional mode/status indicators.
2. **Large central waveform editor:** 32-segment grid, vertical range ±5V.
3. **Shape selector row beneath editor:** left arrow, shape name/display, right arrow, lock/unlock/draw button.
4. **Primary controls:**
   - Large **FREQ** knob.
   - Fine tune knob or context option.
   - FM attenuator.
   - Fold amount knob if wavefolder is included.
   - Optional level/output trim.
5. **Jacks:**
   - V/OCT input.
   - FM input.
   - SYNC input, optional but recommended.
   - RESET input, optional.
   - FOLD CV input, optional.
   - Main OUT.
   - Optional raw/unfolded OUT if panel space allows.

---

# 4. Controls, Inputs, Outputs

## 4.1 Parameters

### `FREQ_PARAM`

Large primary frequency knob.

Recommended behavior:

- Audio mode range: **20 Hz to 20 kHz**.
- LFO mode range: **0.01 Hz to 100 Hz**.
- Use exponential mapping.
- Display exact frequency in the parameter label / tooltip where possible.
- Fine adjustment should be available via normal Rack modifier gestures.

Suggested mapping:

```cpp
freq = minFreq * std::pow(maxFreq / minFreq, normalizedKnob);
```

Where:

- Audio mode: `minFreq = 20.0f`, `maxFreq = 20000.0f`.
- LFO mode: `minFreq = 0.01f`, `maxFreq = 100.0f`.

### `FINE_PARAM` optional

Fine tune in cents.

Recommended range:

- `-100` to `+100` cents.

If panel space is tight, this can be a context menu option or omitted initially.

### `FM_ATTEN_PARAM`

Bipolar or unipolar FM attenuator.

Recommended:

- Bipolar attenuation range: `-1.0` to `+1.0`.
- Center position = no FM.
- Supports both positive and inverted FM depth.

### `FOLD_PARAM`

Wavefolder amount.

Recommended range:

- `0.0` = bypassed / no folding.
- `1.0` = moderate musical folding.
- Optionally allow stronger internal drive above 1.0 if mapped through a nonlinear response.

The folder should not be part of the first core oscillator correctness test. Treat it as a post-core stage.

### `LFO_MODE_PARAM` or context menu item

Audio/LFO mode toggle.

Recommended:

- Use a small switch or context menu item.
- When toggling modes, preserve the knob’s normalized position but recalculate the mapped frequency range.
- The user should not get an explosive jump to 20kHz when toggling back and forth if possible. A context menu option can decide whether to preserve normalized knob position or preserve absolute frequency.

### `LOCK_PARAM` / editor state

Waveform editor lock state.

- Locked: user cannot accidentally draw over the current shape.
- Unlocked: mouse drag edits waveform.
- Browsing factory shapes while locked replaces the waveform with the selected factory shape.
- Unlocking converts the current selected shape into an editable custom waveform.
- Once edited, shape label should show `Custom` or `Custom*`.

---

## 4.2 Inputs

### `VOCT_INPUT`

1V/oct pitch input.

Behavior:

- `0V` should correspond to the current FREQ knob value unless a different pitch reference is intentionally chosen.
- Frequency formula:

```cpp
frequency = baseFrequency * std::pow(2.0f, voctVoltage + fineCents / 1200.0f + fmContribution);
```

`baseFrequency` comes from the FREQ knob mapping.

### `FM_INPUT`

Audio-rate FM input with attenuation.

Recommended default interpretation:

- Exponential FM in volts/oct contribution.
- `fmContribution = FM_INPUT_voltage * FM_ATTEN_PARAM * fmScale`.
- Recommended initial `fmScale = 1.0f`, meaning 1V at full attenuation equals 1 octave.

Optional context menu:

- `FM Mode: Exponential / Linear`

For v1, exponential FM is enough and is consistent with V/OCT mental models.

### `SYNC_INPUT` optional but recommended

Hard sync/reset input.

- Rising edge resets oscillator phase to `0.0`.
- Use Schmitt trigger.
- Should work in audio and LFO mode.

### `FOLD_CV_INPUT` optional

CV modulation for folder amount.

- Sum with `FOLD_PARAM`.
- Clamp to safe range.
- Recommended scale: 0–10V maps to 0–1 additional fold amount.

---

## 4.3 Outputs

### `OUT_OUTPUT`

Main oscillator output.

- Nominal range: **±5V**.
- In folder mode, output may exceed ±5V internally but should be clamped/soft-limited or normalized back to musically safe levels.
- Avoid hard clipping unless it is an intentional sound design choice.

### `RAW_OUTPUT` optional

Pre-folder output.

- Useful if the module includes a folder but users want simultaneous clean and folded waveforms.
- Include only if panel space allows.

---

# 5. Waveform Editor Requirements

## 5.1 Editing model

The editor contains **32 editable vertical values**, representing one periodic cycle.

Internal storage:

```cpp
static constexpr int NUM_POINTS = 32;
float points[NUM_POINTS]; // each in [-1.0f, +1.0f]
```

Mapping:

- `points[i] = -1.0f` => −5V.
- `points[i] = 0.0f` => 0V.
- `points[i] = +1.0f` => +5V.

Important: Internally store normalized values. Convert to volts at output.

## 5.2 Periodicity

The waveform is periodic. Interpolation from point 31 to point 0 must wrap smoothly.

Do not treat the waveform as an open-ended envelope. It is a closed cyclic contour.

## 5.3 Mouse behavior

When unlocked:

- Click sets nearest control point or interpolates between neighboring points depending on draw mode.
- Drag paints across points.
- Dragging quickly should fill all passed indices, not leave gaps.
- Vertical mouse position maps linearly to `[-1, +1]`.
- Clamp values to `[-1, +1]`.

Recommended draw behavior:

1. On mouse down, record starting index/value.
2. On drag, compute new index/value.
3. If index changed, linearly interpolate edited point values between previous index and current index so fast drags create continuous strokes.
4. Mark waveform dirty.

## 5.4 Grid display

The grid should show:

- Horizontal center line = 0V.
- Top/bottom boundaries = ±5V.
- Optional faint horizontal divisions at ±2.5V.
- 32 segment divisions or every 4th/8th division to avoid visual clutter.
- Current waveform as a bright polyline/curve.
- Editable points as small handles when unlocked.
- Locked state visually obvious: handles hidden or dimmed, lock icon active.

## 5.5 Shape selector

Below the editor:

- Left arrow cycles previous factory shape.
- Right arrow cycles next factory shape.
- Center label displays selected shape name.
- Lock/unlock button controls editability.

Factory shapes v1:

1. Sine
2. Triangle
3. Saw
4. Reverse Saw
5. Square / Pulse 50%
6. Pulse 25%
7. Soft Pulse
8. Double Sine / Second Harmonic
9. Folded Sine starter shape
10. Random Smooth, optional

When a factory shape is selected, populate the 32 control points from the ideal shape function, then rebuild the bandlimited tables.

---

# 6. DSP Architecture

## 6.1 Core recommendation

Use a **bandlimited wavetable oscillator** derived from the user’s 32-point curve.

Do not directly evaluate a 32-segment piecewise waveform at runtime as the final output. That approach will alias badly when curves contain sharp edges or corners.

Recommended architecture:

1. User edits 32 normalized points.
2. Convert those points into a high-resolution single-cycle source table using periodic interpolation.
3. Remove DC offset and normalize safely.
4. Build a bank of bandlimited mipmap tables for different pitch ranges.
5. At audio rate, select the correct table for current frequency and interpolate table samples.
6. Apply optional wavefolder as a post-process stage.
7. Output ±5V nominal signal.

This is the best balance of sound quality and CPU cost.

---

# 7. Interpolation from 32 Points to Source Table

## 7.1 Source table size

Recommended:

```cpp
static constexpr int SOURCE_TABLE_SIZE = 2048;
```

Optional high-quality mode:

```cpp
static constexpr int SOURCE_TABLE_SIZE = 4096;
```

2048 should be sufficient for most waveforms, especially after mipmapped bandlimiting.

## 7.2 Interpolation method

Recommended v1 method:

**Periodic Catmull-Rom cubic interpolation** between the 32 points.

Benefits:

- Smooth enough for drawn curves.
- Simple to implement.
- Periodic wrapping is straightforward.
- Better than linear interpolation without being too heavy.

Function outline:

```cpp
float catmullRom(float p0, float p1, float p2, float p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * (
        2.0f * p1 +
        (-p0 + p2) * t +
        (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) * t2 +
        (-p0 + 3.0f*p1 - 3.0f*p2 + p3) * t3
    );
}
```

Because Catmull-Rom can overshoot, clamp output to `[-1.0f, +1.0f]` after interpolation.

Alternative later improvement:

- Use monotonic cubic interpolation if overshoot becomes musically problematic.
- Add editor option: `Interpolation: Smooth / Linear / Hold`.

For v1, implement only Smooth/Catmull-Rom unless a linear mode is trivial.

---

# 8. Bandlimiting Strategy

## 8.1 Why wavetable mipmapping

Arbitrary user curves may contain corners, hard edges, pulse transitions, or drawn discontinuities. These create high harmonics. If played naively, those harmonics exceed Nyquist and alias.

For an arbitrary drawable oscillator, the most robust approach is:

- Build a frequency-domain representation of the source cycle.
- For each table band, keep only harmonics that are safe for that pitch range.
- Reconstruct a table for that band.
- Select the appropriate table at playback time.

This is superior to trying to apply minBLEP only at known discontinuities, because user-drawn curves may have arbitrary slope discontinuities and complex corners.

## 8.2 Table bank

Recommended:

```cpp
static constexpr int NUM_TABLES = 10; // or 11/12
static constexpr int TABLE_SIZE = 2048;
```

Each table corresponds to a maximum fundamental frequency range. For each table, determine the maximum allowed harmonic count:

```cpp
maxHarmonic = floor((sampleRate * 0.5f * safetyFactor) / maxFundamentalForThisTable);
```

Recommended `safetyFactor = 0.90f` to leave margin.

Table selection at runtime can be based on frequency:

```cpp
int tableIndex = chooseTableForFrequency(frequency, sampleRate);
```

A simple octave-based mapping is acceptable:

- Table 0: very low frequencies, many harmonics.
- Table 1: one octave higher, half as many harmonics.
- Table N: high frequencies, few harmonics.

## 8.3 Rebuild method

Preferred v1 implementation options:

### Option A: Use an FFT if existing project dependencies make this easy

1. Build full source table.
2. FFT to frequency domain.
3. For each mip table:
   - Copy bins up to max harmonic.
   - Zero bins above max harmonic.
   - Inverse FFT.
4. Normalize table.

### Option B: Use direct Fourier projection if avoiding FFT dependency

Since rebuild happens only when the waveform changes, a direct Fourier transform may be acceptable for `2048` samples and ~10 tables if throttled.

But avoid doing direct DFT every audio frame. Rebuild only on waveform dirty events and rate-limit during mouse drag.

### Option C: Initial fallback if no Fourier implementation is desired

Apply a windowed low-pass smoothing filter per table. This is less ideal and should be considered a fallback only.

Codex should inspect the existing Leviathan/Rack project for available FFT utilities before choosing final implementation.

## 8.4 Rebuild throttling

When user draws:

- Update the visual curve immediately.
- Rebuild audio tables at a limited rate, e.g. every **20–40 ms** during active drag.
- Force rebuild on mouse release.

This prevents CPU spikes and audio glitches during drawing.

Potential state:

```cpp
bool waveformDirty = true;
bool rebuildPending = true;
float rebuildTimer = 0.0f;
static constexpr float REBUILD_INTERVAL = 0.03f;
```

In `process()`:

- Accumulate `args.sampleTime`.
- If dirty and timer exceeds threshold, rebuild tables.
- On mouse release, request immediate rebuild.

Important: Avoid allocating memory during audio processing.

---

# 9. Runtime Oscillator Playback

## 9.1 Phase accumulation

Use normalized phase `[0, 1)`.

```cpp
phase += frequency * args.sampleTime;
phase -= std::floor(phase);
```

For sync:

```cpp
if (syncTrigger.process(inputs[SYNC_INPUT].getVoltage())) {
    phase = 0.0f;
}
```

## 9.2 Table lookup

For selected table:

```cpp
float pos = phase * TABLE_SIZE;
int i0 = (int)pos;
float frac = pos - i0;
int i1 = (i0 + 1) & (TABLE_SIZE - 1); // if power-of-two table size
float sample = crossfade(table[i0], table[i1], frac);
```

Linear interpolation is likely sufficient for runtime lookup with 2048+ table size. Cubic table interpolation is optional but probably unnecessary.

## 9.3 Table crossfading

To avoid timbral jumps when frequency crosses a table boundary, preferably crossfade between adjacent mip tables.

Simpler v1 acceptable behavior:

- Choose nearest appropriate table with no crossfade.
- If audible stepping occurs during pitch sweeps, add crossfading.

Recommended final behavior:

```cpp
float sampleA = lookup(tableA, phase);
float sampleB = lookup(tableB, phase);
float sample = crossfade(sampleA, sampleB, tableBlend);
```

## 9.4 Frequency calculation

Suggested formula:

```cpp
float baseFreq = mapFreqKnob(params[FREQ_PARAM].getValue(), lfoMode);
float voct = inputs[VOCT_INPUT].isConnected() ? inputs[VOCT_INPUT].getVoltage() : 0.0f;
float fm = inputs[FM_INPUT].isConnected()
    ? inputs[FM_INPUT].getVoltage() * params[FM_ATTEN_PARAM].getValue() * fmScale
    : 0.0f;
float fine = params[FINE_PARAM].getValue() / 1200.0f;
float pitch = voct + fm + fine;
float frequency = baseFreq * std::pow(2.0f, pitch);
frequency = clampFrequency(frequency, lfoMode, args.sampleRate);
```

Clamp recommendations:

- Audio mode upper clamp: `min(20000.0f, sampleRate * 0.45f)`.
- LFO mode upper clamp: `100.0f` or context-configurable.
- Lower clamp: `0.0f` or `0.001f` depending on whether stopped/near-stopped behavior is desired.

---

# 10. LFO Mode

LFO mode should use the same waveform but slower frequency mapping.

Recommended behavior:

- Same output range: ±5V.
- Frequency range: 0.01 Hz–100 Hz.
- V/OCT still works unless disabled by context option.
- Sync/reset still works.
- Bandlimited table selection still works, but at LFO rates it will naturally select the richest table.

Optional context menu:

- `LFO Output: Bipolar ±5V / Unipolar 0–10V`

For v1, keep bipolar ±5V unless unipolar is easy and musically desired.

---

# 11. Wavefolder Stage

## 11.1 Placement

Wavefolder is post-oscillator:

```cpp
raw = oscillatorSample * 5.0f;
folded = applyFolder(raw, foldAmount);
out = folded;
```

## 11.2 Folder behavior

Recommended v1: simple, stable wavefolder with soft saturation.

A musical folding function:

```cpp
float foldSignal(float x, float threshold) {
    // x in volts or normalized units
    // Repeated reflection around +/- threshold
}
```

Practical normalized version:

1. Convert raw normalized sample `[-1, +1]`.
2. Apply pre-gain based on fold amount:

```cpp
float drive = 1.0f + foldAmount * 4.0f;
float x = rawNormalized * drive;
```

3. Fold into `[-1, +1]` by reflection.
4. Apply gentle tanh softening.
5. Output as volts.

Pseudo-code:

```cpp
float foldNormalized(float x) {
    // Fold repeatedly into [-1, +1]
    while (x > 1.0f || x < -1.0f) {
        if (x > 1.0f) x = 2.0f - x;
        if (x < -1.0f) x = -2.0f - x;
    }
    return x;
}

float applyFolder(float xNorm, float amount) {
    if (amount <= 0.0001f)
        return xNorm;

    float drive = 1.0f + amount * 5.0f;
    float y = foldNormalized(xNorm * drive);
    y = std::tanh(y * (1.0f + amount * 0.5f)) / std::tanh(1.0f + amount * 0.5f);
    return clamp(y, -1.2f, 1.2f);
}
```

Note: Wavefolding itself creates new harmonics and can alias at high frequencies. For highest quality, consider oversampling the folder stage.

## 11.3 Oversampling recommendation

V1 acceptable:

- Folder works without oversampling but is conservative.

Higher-quality option:

- 2x or 4x oversampling for folder when fold amount > 0.
- Context menu: `Folder Quality: Eco / High`.

Do not block v1 on oversampling unless existing project utilities already make this easy.

---

# 12. Preset and State Serialization

Must serialize:

- 32 point values.
- Current selected factory shape index.
- Whether current waveform is custom.
- Editor locked/unlocked state.
- LFO mode.
- Optional interpolation mode.
- Optional FM mode.
- Optional folder quality mode.

Use Rack JSON state methods:

```cpp
json_t* dataToJson() override;
void dataFromJson(json_t* rootJ) override;
```

Represent points as a JSON array of floats.

Example:

```json
{
  "points": [0.0, 0.195, 0.383, ...],
  "shapeIndex": 0,
  "custom": false,
  "editorLocked": true,
  "lfoMode": false,
  "interpolationMode": "smooth",
  "fmMode": "expo"
}
```

Backward compatibility:

- If points array missing or malformed, default to sine.
- If points array length is not 32, default to sine or resample defensively.

---

# 13. Factory Shape Generation

Factory shapes should populate `points[32]` and then trigger table rebuild.

Use normalized phase:

```cpp
float phase = (float)i / NUM_POINTS;
```

## 13.1 Sine

```cpp
points[i] = std::sin(2.0f * M_PI * phase);
```

## 13.2 Triangle

Range ±1:

```cpp
points[i] = 4.0f * std::fabs(phase - 0.5f) - 1.0f;
// optionally invert if desired orientation feels backwards
```

## 13.3 Saw

```cpp
points[i] = 2.0f * phase - 1.0f;
```

## 13.4 Reverse Saw

```cpp
points[i] = 1.0f - 2.0f * phase;
```

## 13.5 Square / Pulse 50%

```cpp
points[i] = phase < 0.5f ? 1.0f : -1.0f;
```

## 13.6 Pulse 25%

```cpp
points[i] = phase < 0.25f ? 1.0f : -1.0f;
```

## 13.7 Soft Pulse

Use tanh or a sine-smoothed transition before table rebuild.

---

# 14. Module Class Structure

Suggested classes/structs inside one module file if that is current Leviathan style:

```cpp
struct Wyrmform : Module {
    enum ParamIds { FREQ_PARAM, FM_ATTEN_PARAM, FOLD_PARAM, LFO_PARAM, NUM_PARAMS };
    enum InputIds { VOCT_INPUT, FM_INPUT, SYNC_INPUT, FOLD_CV_INPUT, NUM_INPUTS };
    enum OutputIds { OUT_OUTPUT, RAW_OUTPUT, NUM_OUTPUTS };
    enum LightIds { LFO_LIGHT, LOCK_LIGHT, NUM_LIGHTS };

    float points[32];
    bool editorLocked = true;
    bool customWaveform = false;
    int shapeIndex = 0;
    bool lfoMode = false;

    WavetableBank wavetableBank;
    float phase = 0.0f;
    dsp::SchmittTrigger syncTrigger;

    bool waveformDirty = true;
    float rebuildTimer = 0.0f;

    void process(const ProcessArgs& args) override;
    void rebuildWavetables(float sampleRate);
    void setFactoryShape(int index);
    json_t* dataToJson() override;
    void dataFromJson(json_t* rootJ) override;
};
```

Suggested helper:

```cpp
struct WavetableBank {
    static constexpr int TABLE_SIZE = 2048;
    static constexpr int NUM_TABLES = 10;
    float tables[NUM_TABLES][TABLE_SIZE];
    bool valid = false;

    void buildFromPoints(const float* points, int pointCount, float sampleRate);
    float lookup(float phase, float frequency, float sampleRate) const;
};
```

Avoid heap allocations in `process()`.

---

# 15. Widget Class Structure

Suggested editor widget:

```cpp
struct WyrmformEditor : Widget {
    Wyrmform* module = nullptr;

    void draw(const DrawArgs& args) override;
    void onButton(const event::Button& e) override;
    void onDragMove(const event::DragMove& e) override;
    void onDragEnd(const event::DragEnd& e) override;

    int xToPointIndex(float x) const;
    float yToValue(float y) const;
    Vec pointToScreen(int index, float value) const;
};
```

If using NanoVG:

- Draw grid first.
- Draw center line.
- Draw interpolated/high-res preview curve if possible.
- Draw raw 32-point handles when unlocked.
- Draw lock overlay if locked.

Important: The editor should not rebuild wavetables directly. It should mutate module points and call a lightweight method such as:

```cpp
module->setPoint(index, value);
module->requestWaveformRebuild(false);
```

On drag end:

```cpp
module->requestWaveformRebuild(true);
```

---

# 16. Visual Preview Accuracy

The editor should show what the oscillator approximately emits.

Preferred:

- Draw the interpolated source curve, not only the 32-point polyline.
- When locked, show a clean curve.
- When unlocked, show both the smooth curve and handles.

Optional advanced overlay:

- If high-frequency bandlimited playback differs significantly from the drawn shape, show a subtle second overlay representing the current playback table at the current frequency.
- This is not required for v1.

---

# 17. User Interaction Details

## 17.1 Lock / unlock behavior

Initial default:

- Shape: Sine.
- Editor: Locked.
- Output: clean sine oscillator.

When user presses unlock:

- Current shape remains.
- Handles appear.
- Shape label becomes editable/custom aware.

When user draws:

- `customWaveform = true`.
- Shape label should become `Custom` or `Custom*`.

When user selects another factory shape:

- If custom edits exist and editor is unlocked, either:
  - immediately replace shape, or
  - ask via context?  

Rack modules usually avoid modal confirmations. Recommended:

- Selecting a factory shape replaces the current waveform.
- Undo should capture the change if feasible.

## 17.2 Undo support

Nice-to-have:

- Use Rack history if practical for shape changes.
- At minimum, avoid hundreds of undo actions during drag. Treat one drag gesture as one edit.

V1 can ship without full waveform undo if implementation complexity is high, but it should not corrupt patch state.

---

# 18. Audio Quality Acceptance Criteria

1. **Sine factory shape**
   - Should sound clean across the audio range.
   - Output should be approximately ±5V.
   - DC offset should be near 0.

2. **Saw / square factory shapes**
   - Should be audibly bandlimited compared to naive discontinuous waveforms.
   - High notes should not produce severe metallic aliasing unrelated to the intended waveform.

3. **Custom sharp shape**
   - Drawing abrupt corners should not explode into uncontrolled aliasing.
   - Some brightness is expected; severe foldback is not.

4. **Pitch tracking**
   - 1V increase at V/OCT should double frequency.
   - Test at 55Hz, 110Hz, 220Hz, 440Hz, 880Hz.

5. **LFO mode**
   - Slow oscillation should be stable and not drift visibly.
   - Sync reset should restart phase cleanly.

6. **Wavefolder**
   - Fold amount 0 should be bit-identical or very close to raw output.
   - Increasing fold should add harmonics without clipping harshly by default.

---

# 19. Performance Acceptance Criteria

1. No heap allocation inside `process()`.
2. Wavetable rebuilds only happen when waveform changes, sample rate changes, or mode/settings requiring rebuild change.
3. Drawing should not freeze Rack UI.
4. During mouse drag, table rebuild should be throttled.
5. With folder disabled, runtime cost should be close to a normal wavetable oscillator.
6. With folder enabled, cost should remain reasonable. Oversampling, if added, should be optional.

---

# 20. Edge Cases

Handle these safely:

- Sample rate changes.
- Patch load before widget exists.
- Malformed JSON state.
- All points set to 0.
- All points set to +1 or −1.
- Very high V/OCT input.
- Very negative V/OCT input.
- FM input causing extreme frequency modulation.
- User drawing while audio engine runs.
- Changing factory shape while oscillator is playing.

For degenerate all-zero waveform:

- Output silence, no divide-by-zero normalization.

For constant nonzero waveform:

- DC removal may turn it into silence. That is acceptable if documented internally, but consider whether users expect DC-capable LFO behavior.

Recommendation:

- In audio mode, remove DC from generated tables.
- In LFO mode, consider preserving intentional DC only if unipolar/asymmetric modulation use is desired.
- For v1, remove DC for oscillator purity, but this should be revisited if the module becomes a modulation source as much as an audio oscillator.

---

# 21. DC Offset and Normalization

After building the source table:

1. Compute mean.
2. Subtract mean to remove DC.
3. Find peak absolute value.
4. If peak is above epsilon, normalize to `[-1, +1]`.
5. If peak is below epsilon, fill table with 0.

Pseudo-code:

```cpp
float mean = 0.0f;
for (int i = 0; i < TABLE_SIZE; ++i)
    mean += source[i];
mean /= TABLE_SIZE;

float peak = 0.0f;
for (int i = 0; i < TABLE_SIZE; ++i) {
    source[i] -= mean;
    peak = std::max(peak, std::fabs(source[i]));
}

if (peak > 1e-6f) {
    float gain = 1.0f / peak;
    for (int i = 0; i < TABLE_SIZE; ++i)
        source[i] *= gain;
} else {
    for (int i = 0; i < TABLE_SIZE; ++i)
        source[i] = 0.0f;
}
```

Question for later product decision:

- Should there be a context option `Preserve DC in LFO Mode`?

Not required for v1.

---

# 22. Context Menu Options

Recommended context menu items:

- `Mode: Audio / LFO` if not a panel switch.
- `Interpolation: Smooth / Linear` optional.
- `FM Mode: Exponential / Linear` optional, expo default.
- `Folder Quality: Eco / High` optional if oversampling added.
- `Preserve DC in LFO Mode` optional later.
- `Reset Phase on Shape Change` optional later.

Keep v1 focused. Do not overbuild context options before the core oscillator feels excellent.

---

# 23. Development Milestones

## Milestone 1: Static wavetable oscillator

- Implement module skeleton.
- Implement FREQ, V/OCT, OUT.
- Implement sine factory shape.
- Use a single high-res source table.
- Confirm pitch and voltage behavior.

## Milestone 2: 32-point waveform editor

- Draw editor grid.
- Display 32 points.
- Support unlock + mouse drawing.
- Generate source table from Catmull-Rom interpolation.
- Serialize/deserialize points.

## Milestone 3: Factory shape selector

- Add left/right arrows.
- Add shape label.
- Implement base shapes.
- Ensure shape changes trigger table rebuild.

## Milestone 4: Bandlimited mip tables

- Implement table bank.
- Rebuild from source table.
- Select table based on frequency.
- Test saw/square at high notes.

## Milestone 5: FM and sync

- Add FM input + attenuator.
- Add sync input if panel includes it.
- Validate stability under modulation.

## Milestone 6: LFO mode

- Add LFO range.
- Confirm slow phase stability.
- Consider unipolar option later.

## Milestone 7: Wavefolder

- Add fold knob.
- Add optional CV input.
- Add folder post-processing.
- Consider oversampling if aliasing is objectionable.

## Milestone 8: Polish

- Improve editor visuals.
- Add lock/unlock affordance.
- Add tooltips/labels.
- Add final acceptance tests.

---

# 24. Testing Plan

## 24.1 Manual tests

1. Load module. Confirm sine output at default frequency.
2. Patch output to scope. Confirm ±5V.
3. Sweep FREQ knob. Confirm stable pitch response.
4. Patch sequencer or keyboard into V/OCT. Confirm octave tracking.
5. Select saw and square. Sweep into high range. Confirm bandlimited behavior.
6. Unlock editor and draw arbitrary shape. Confirm output updates without freezing.
7. Save patch, reload patch. Confirm custom waveform persists.
8. Toggle LFO mode. Confirm slow waveform output.
9. Enable fold. Confirm fold amount 0 does not change sound, and fold >0 adds harmonics.

## 24.2 Code-level tests / debug utilities

If the project has debug logging or test harness support:

- Verify table mean near 0 after DC removal.
- Verify table peak near 1 after normalization.
- Verify no NaN/Inf when all points equal 0.
- Verify frequency doubles for +1V V/OCT.
- Verify table rebuild does not allocate during process.

---

# 25. Implementation Notes for Codex

1. Respect the existing Leviathan plugin style and file organization.
2. If this project convention is one source file per module, keep Wyrmform in one `.cpp` unless there is already a shared wavetable utility system.
3. Prefer clear, maintainable C++11.
4. Avoid introducing heavy dependencies unless already available in Rack SDK or the plugin.
5. Audio thread must remain real-time safe.
6. Avoid dynamic allocation, locks, file IO, or expensive rebuilds inside the hot audio path.
7. The editor/widget can request changes, but DSP state should be owned by the module.
8. When in doubt, prioritize clean audio over visual cleverness.

---

# 26. Open Product Questions

These are not blockers, but should be decided before final UI polish:

1. Should the module be primarily an oscillator, or equally a modulation/LFO source?
2. Should LFO mode preserve DC/asymmetry for drawn modulation shapes?
3. Should wavefolder be included in v1, or added after the core oscillator is excellent?
4. Should there be a second output for raw waveform vs folded waveform?
5. Should factory shape browsing be undoable?
6. Should interpolation modes be exposed, or should the module enforce one high-quality smooth mode?
7. Should the shape editor support snapping to grid?
8. Should random/smooth random waveform generation be included?

Recommendation:

- Build v1 around the cleanest possible drawable oscillator.
- Include FM and LFO mode.
- Include wavefolder only if panel space and DSP quality remain comfortable.
- Keep advanced editor features for v1.1.

---

# 27. Recommended v1 Feature Cut

If scope needs tightening, ship v1 with:

- 32-point editor.
- Smooth periodic interpolation.
- Factory shape selector.
- Lock/unlock.
- FREQ knob.
- V/OCT input.
- FM input + attenuator.
- LFO mode.
- Main output.
- Bandlimited wavetable bank.

Defer:

- Wavefolder oversampling.
- Raw secondary output.
- Multiple interpolation modes.
- DC preserve options.
- Random waveform generators.
- Visual overlay of selected mip table.

This keeps the essence intact: **draw a waveform, hear it cleanly, play it musically**.

