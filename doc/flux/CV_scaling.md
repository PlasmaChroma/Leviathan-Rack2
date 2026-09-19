# Integral Flux / Maths-Style Rise/Fall/BOTH CV Scaling Spec (Codex-Ready)

Goal: Implement **Maths-like** knob + CV behavior for CH1 & CH4 Rise/Fall/BOTH control such that
- CV influence feels **exponential (log-time domain)** rather than linear-time,
- BOTH behaves as documented: **positive voltage shortens** the entire function, **negative lengthens**, and it is **inverse** relative to Rise/Fall CV behavior,
- response is stable, clamp-safe, SIMD-friendly, and calibratable to a physical unit.

This spec describes only the **time-control law**. It does not define the integrator core, EOC/EOR, or shape curves beyond the time mapping.

---

## 1. Documentation Constraints (Ground Truth)

From Make Noise manuals:
- Rise and Fall are **independently voltage controllable**. :contentReference[oaicite:0]{index=0}  
- There is a **BOTH CV** input that “changes the rate of the entire function” and is **inverse** to Rise/Fall CV inputs:  
  **more positive → shorter**, **more negative → longer**. :contentReference[oaicite:1]{index=1}  
- Vari-Response shapes rise/fall behavior log/lin/exp; this spec covers only time scaling, not curvature. :contentReference[oaicite:2]{index=2}

No official “V/oct” scaling is specified in the docs; therefore this spec must be **tunable**.

---

## 2. Design Principle

### 2.1 Control-domain summation (log-time domain)
Maths behavior is best replicated by summing knob and CV in a **control domain** and then mapping that control to time via an **exponential law**.

**Do NOT** add CV directly to milliseconds/seconds.
**Do** combine controls before the mapping:
- `control = knob_bias + cv_influence`
- `time = map(control)`

This yields multiplicative time scaling (musical, “analog”).

### 2.2 Multiplicative time scaling
Rise/Fall times should scale roughly like:

`time ∝ 2^(-k * V)`  (or equivalently `time = baseTime / 2^(k*V)`)

Where:
- V is the relevant summed CV (RiseCV, FallCV, BothCV),
- k is a tunable coefficient (units: octaves per volt).

---

## 3. Inputs & Ranges

Per channel (CH1 and CH4), define:
- `RiseKnob` : normalized [0..1]
- `FallKnob` : normalized [0..1]
- `RiseCV`   : volts (recommended clamp to [-8, +8])
- `FallCV`   : volts (recommended clamp to [-8, +8])
- `BothCV`   : volts (recommended clamp to [-8, +8]) :contentReference[oaicite:3]{index=3}

Note: Physical Maths expects attenuation/attenuverting externally (CH2/CH3) for Rise/Fall CV if needed. :contentReference[oaicite:4]{index=4}  
In software we still accept full-scale CV and provide internal clamps.

---

## 4. Parameterization (Tunable Constants)

Define these constants (per channel, but can default global):

### 4.1 Base time range (knob endpoints)
- `T_RISE_MIN` (seconds)  : e.g. 0.0008
- `T_RISE_MAX` (seconds)  : e.g. 25.0
- `T_FALL_MIN` (seconds)  : e.g. 0.0008
- `T_FALL_MAX` (seconds)  : e.g. 25.0

(Defaults are placeholders; calibrate later.)

### 4.2 CV scaling coefficient
- `K_RISE` : octaves per volt (recommended starting point: 1.0)
- `K_FALL` : octaves per volt (recommended starting point: 1.0)
- `K_BOTH` : octaves per volt (recommended starting point: 1.0)

These determine “how much influence” voltage has.

### 4.3 Soft limiting
- `CV_CLAMP = 8.0` volts (hard clamp)
- `OCT_CLAMP = [+OCT_MAX, -OCT_MIN]` e.g. +/- 16 octaves equivalent multiplier ceiling/floor
- Optional: `SOFTSAT` curve for CV to mimic analog headroom (recommended)

---

## 5. Mapping: Knob -> Base Time (no CV)

Use an exponential sweep so the knob feels like a time-constant control:

For each segment (Rise/Fall):
1) Convert knob to a log-time interpolation:
- `logT_min = ln(T_MIN)`
- `logT_max = ln(T_MAX)`
- `logT_knob = lerp(logT_min, logT_max, knob)`

2) Base time:
- `T_base = exp(logT_knob)`

This matches typical “wide-range time knob” ergonomics.

---

## 6. Mapping: CV influence in log-time domain

### 6.1 Polarity conventions
To match the BOTH description:
- RiseCV / FallCV: **positive voltage makes that segment longer** (slower), negative makes it shorter (faster).  
- BothCV: **positive makes entire function shorter** (faster), negative makes it longer. :contentReference[oaicite:5]{index=5}

This “inverse” relationship is implemented by applying opposite signs.

### 6.2 Compute effective voltages
Clamp inputs first:
- `vRise = clamp(RiseCV, -CV_CLAMP, +CV_CLAMP)`
- `vFall = clamp(FallCV, -CV_CLAMP, +CV_CLAMP)`
- `vBoth = clamp(BothCV, -CV_CLAMP, +CV_CLAMP)`

### 6.3 Convert volts to octave-shift multipliers
Define octave shift (positive = “more time”):
- `octRise = +K_RISE * vRise  + (-K_BOTH * vBoth)`
- `octFall = +K_FALL * vFall  + (-K_BOTH * vBoth)`

Explanation:
- Rise/Fall CV: +V increases time (octRise positive).
- Both CV: +V decreases time, so it subtracts octave shift.

Clamp octave shift:
- `octRise = clamp(octRise, -OCT_MIN, +OCT_MAX)`
- `octFall = clamp(octFall, -OCT_MIN, +OCT_MAX)`

Convert to time multipliers:
- `mRise = 2^(octRise)`
- `mFall = 2^(octFall)`

Final segment times:
- `T_rise = clamp(T_base_rise * mRise, T_RISE_MIN_HARD, T_RISE_MAX_HARD)`
- `T_fall = clamp(T_base_fall * mFall, T_FALL_MIN_HARD, T_FALL_MAX_HARD)`

Hard clamps:
- `T_*_MIN_HARD` should be >= 1 / sampleRate * safetyFactor (e.g. 2–4 samples) to prevent instability.
- `T_*_MAX_HARD` should cap runaway “infinite” time.

---

## 7. Optional: Analog-like soft saturation (recommended)

To better mimic “CV headroom compresses at extremes”:
Replace the hard clamp on volts with a soft-sat curve:

Example (smooth, cheap):
- `softClamp8(v) = 8 * tanh(v / 8)`

Then:
- `vRise = softClamp8(RiseCV)`
- `vFall = softClamp8(FallCV)`
- `vBoth = softClamp8(BothCV)`

Keep octave clamp as well.

---

## 8. Calibration Mode (to match a real unit)

Provide a calibration struct with a few measured points:

Per channel, measure on physical Maths:
- With Rise/Fall knobs at known positions (e.g. 0%, 50%, 100%),
- Apply BOTH CV at -5V, 0V, +5V (or -8/0/+8),
- Record resulting cycle frequency `f` or segment time.

Fit coefficients:
- Solve for `K_BOTH` that best fits:
  `T_total(V) ≈ T_total(0) * 2^(-K_BOTH * V)`
- Similarly fit `K_RISE`, `K_FALL` if you have isolated segment measurements.

If only total period is measurable:
- Use BOTH fits first (most audible and easiest to measure).
- Keep `K_RISE = K_FALL = K_BOTH` as a first-order approximation.

Expose a compile-time or JSON-config path:
- `maths_like.k_rise`
- `maths_like.k_fall`
- `maths_like.k_both`
- `maths_like.t_rise_min/max`
- `maths_like.t_fall_min/max`

---

## 9. Implementation Notes (VCV Rack / SIMD Friendly)

- Prefer computing `2^x` using `exp2f(x)` (or SIMD exp2) for speed and accuracy.
- Keep all time calculations in seconds; convert to per-sample coefficients later.
- Ensure the time mapping is **sample-rate invariant**:
  - compute coefficients from `T_rise`, `T_fall` each block or when parameters change.
- Avoid zipper noise:
  - Slew the computed `octRise/octFall` or `T_rise/T_fall` with a short smoothing filter (e.g. 1–5 ms).
  - Smoothing should occur **after** mapping but **before** core coefficient calculation.

---

## 10. Expected Behavioral Checks (Unit Tests / Assertions)

### 10.1 BOTH polarity check (required)
Given same knob settings and no Rise/Fall CV:
- If `BothCV = +5V`, then `T_rise` and `T_fall` must both be **smaller** than at 0V.
- If `BothCV = -5V`, then both must be **larger** than at 0V. :contentReference[oaicite:6]{index=6}

### 10.2 RiseCV polarity check (recommended)
Given fixed BothCV=0 and FallCV=0:
- If `RiseCV = +5V`, then `T_rise` must be **larger** than at 0V.
- If `RiseCV = -5V`, then `T_rise` must be **smaller** than at 0V.

### 10.3 Multiplicative scaling check (critical)
For a fixed knob:
- The ratio `T(V=+1) / T(V=0)` should be approximately constant for different knob positions
  (i.e., CV acts multiplicatively, not additively).

---

## 11. Pseudocode (Reference)

```c++
struct MathsTimeLaw {
    float tMinRise, tMaxRise;
    float tMinFall, tMaxFall;
    float kRise, kFall, kBoth;     // octaves per volt
    float cvClamp;                // 8.0f
    float octMin, octMax;         // e.g. 16
    bool  softSat;                // true recommended
};

inline float softClamp8(float v) {
    return 8.f * tanhf(v / 8.f);
}

inline float knobToBaseTime(float knob01, float tMin, float tMax) {
    float logMin = logf(tMin);
    float logMax = logf(tMax);
    float logT = logMin + (logMax - logMin) * knob01;
    return expf(logT);
}

void computeRiseFallTimes(
    const MathsTimeLaw& law,
    float riseKnob01, float fallKnob01,
    float riseCV, float fallCV, float bothCV,
    float& outRiseSec, float& outFallSec
){
    float vR = law.softSat ? softClamp8(riseCV) : fminf(fmaxf(riseCV, -law.cvClamp), law.cvClamp);
    float vF = law.softSat ? softClamp8(fallCV) : fminf(fmaxf(fallCV, -law.cvClamp), law.cvClamp);
    float vB = law.softSat ? softClamp8(bothCV) : fminf(fmaxf(bothCV, -law.cvClamp), law.cvClamp);

    float baseRise = knobToBaseTime(riseKnob01, law.tMinRise, law.tMaxRise);
    float baseFall = knobToBaseTime(fallKnob01, law.tMinFall, law.tMaxFall);

    // octave shifts: + makes longer, - makes shorter
    float octRise = (law.kRise * vR) - (law.kBoth * vB);
    float octFall = (law.kFall * vF) - (law.kBoth * vB);

    octRise = fminf(fmaxf(octRise, -law.octMin), law.octMax);
    octFall = fminf(fmaxf(octFall, -law.octMin), law.octMax);

    float mRise = exp2f(octRise);
    float mFall = exp2f(octFall);

    outRiseSec = baseRise * mRise;
    outFallSec = baseFall * mFall;

    // hard safety clamps (choose your own hard limits)
    outRiseSec = fminf(fmaxf(outRiseSec, 1e-5f), 120.f);
    outFallSec = fminf(fmaxf(outFallSec, 1e-5f), 120.f);
}