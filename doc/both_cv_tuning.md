# Integral Flux — BOTH CV Saturating Scaling Spec

Version: 1.1
Scope: Replace ONLY the BOTH-CV time-scale mapping used for CH1/CH4 stage time caching.

## 1. Goal
Replace the current pure exponential BOTH-CV multiplier with a calibrated saturating model that better matches measured hardware behavior.

Required outcomes:
- +BOTH CV shortens rise/fall times (faster)
- -BOTH CV lengthens rise/fall times (slower)
- Diminishing returns near the high-speed region
- A tunable neutral offset so 0V can be slightly fast relative to true neutral

This change must affect both:
- Function-generator mode (cycle/triggered)
- Slew mode

That is achieved by changing `bothScale` at the shared stage-time layer only.

## 2. Existing Integration Point
In `processOuterChannel()` (stage-time dirty block), replace current `bothScale` logic with:

```cpp
float bothScale = bothTimeScaleFromCv(bothCv);
```

No other timing cache/control-flow changes.

## 3. Calibration Dataset
Measured cycle frequency vs BOTH CV:

| CV (V) | Hz   |
|-------:|-----:|
| 0      | 40.0 |
| 1      | 82.5 |
| 2      | 161.6|
| 3      | 290.3|
| 4      | 462.4|
| 5      | 653.7|
| 6      | 794.9|

## 4. Model
Use a saturating logistic-in-log2 frequency curve:

```cpp
f(v) = f_off + f_max * (r / (1 + r))
r    = 2^(k * (v - v0))
```

Fitted constants:

```cpp
static constexpr float BOTH_F_OFF_HZ       = 1.93157058f;
static constexpr float BOTH_F_MAX_HZ       = 986.84629918f;
static constexpr float BOTH_K_OCT_PER_V    = 1.10815030f;
static constexpr float BOTH_V0_V           = 4.15514297f;
static constexpr float BOTH_NEUTRAL_V      = -0.05f;
static constexpr float BOTH_TIME_SCALE_MAX = 64.f;
```

Convert to time scale:

```cpp
bothScale(v) = f(BOTH_NEUTRAL_V) / f(v)
```

## 5. Reference Implementation
Add near existing timing/CV helpers:

```cpp
static inline float bothHzFromCv(float v) {
    float x = BOTH_K_OCT_PER_V * (v - BOTH_V0_V);
    float r = rack::dsp::exp2_taylor5(x);
    return BOTH_F_OFF_HZ + BOTH_F_MAX_HZ * (r / (1.f + r));
}

static inline float bothTimeScaleFromCv(float v) {
    float vs = softClamp8(v);
    float f  = bothHzFromCv(vs);
    float f0 = bothHzFromCv(BOTH_NEUTRAL_V);
    float scale = f0 / std::max(f, 1e-6f);
    return clamp(scale, 1.f / BOTH_TIME_SCALE_MAX, BOTH_TIME_SCALE_MAX);
}
```

Then in stage-time caching:

```cpp
float bothScale = bothTimeScaleFromCv(bothCv);
```

## 6. Guardrails
- Do not change Rise/Fall CV law.
- Do not change knob->time mapping.
- Do not change shape (log/lin/exp) mapping.
- Do not alter stage-time cache invalidation or interpolation behavior.

## 7. Validation
1. Cycle mode should approximately match dataset points above (initial tolerance ±2%).
2. Slew mode should speed up with positive BOTH and slow with negative BOTH.
3. High positive BOTH should show clear compression (diminishing returns).
4. `BOTH_NEUTRAL_V` must remain a single easy-to-tune constant.
