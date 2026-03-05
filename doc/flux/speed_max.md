# Codex-Ready Spec: Maths-Approx BOTH CV + Input Perturbation + Speed Ceilings (Cycle vs Trigger)

## Objective

Update Integral Flux CH1/CH4 (“outer channels”) to more closely approximate Make Noise Maths behavior in three key areas:

1. **BOTH CV response**

   * Quasi-1V/oct feel at low speeds from the setpoint
   * Deviation starting ~200–300 Hz region
   * Strong compression approaching ~1 kHz (ceiling)
   * Slight “0V bump”: true neutral occurs at a small negative voltage

2. **Cycle + Signal IN interaction**

   * When cycling or running a triggered function, a patched Signal IN should **perturb** the integrator state (hardware-like “input injects into core”), instead of being ignored while the core is active.

3. **Maximum speed behavior**

   * Self-cycling should be limited to ~**1 kHz-ish** (hardware-like ceiling) regardless of curve knob, BOTH CV, or stage CV.
   * Trigger input should allow **faster retriggering** (target: up to **2 kHz** effective), by permitting retrigger during rise/fall and using a higher ceiling than Cycle mode.

This must apply consistently to:

* Cycle-mode LFO/VCO behavior
* Triggered function behavior
* Non-cycle slewing behavior (Signal IN patched while idle)

---

## Non-Goals

* Perfect electrical emulation of Maths (full analog model)
* Precision 1V/oct tracking across entire range (hardware does not do this)
* Changing Rise/Fall CV semantics or knob taper beyond what is specified here

---

## Definitions / Terms

* **Outer channels**: CH1 and CH4 in Integral Flux.
* **Cycle mode**: internal self-retrigger behavior (EOC loop equivalent).
* **Trigger mode**: external trigger input causes immediate retrigger.
* **Signal IN**: per-channel input jack used for slewing and (after this change) perturbation while running.
* **Stage times**: rise/fall times computed from knobs + CV + BOTH + curve shape scaling.

---

# Part A — Calibrated BOTH CV Response (Saturating Exponential)

## A1. Calibration Dataset

Measured on one physical Maths unit (Cycle frequency vs BOTH CV):

* 0V → 40.0 Hz
* 1V → 82.5 Hz
* 2V → 161.6 Hz
* 3V → 290.3 Hz
* 4V → 462.4 Hz
* 5V → 653.7 Hz
* 6V → 794.9 Hz

Observation: at 0V there is a small bump; neutral occurs slightly below 0V.

## A2. Frequency Model

Implement a saturating logistic-in-log2 model:

Given BOTH CV voltage `V`:

* `r = 2^(k * (V - V0))`
* `f(V) = f_off + f_max * (r / (1 + r))`

Use these fitted constants:

* `f_off  = 1.93157058` Hz
* `f_max  = 986.84629918` Hz
* `k      = 1.10815030`  (effective octaves per volt in the low region)
* `V0     = 4.15514297` V

Asymptote: `f_off + f_max ≈ 988.78 Hz`

## A3. Convert to a Time Scale Multiplier

Integral Flux computes stage times; BOTH CV must scale stage times.

Compute:

* `bothTimeScale(V) = f(V_neutral) / f(V)`

Where:

* `V_neutral = -0.05 V` (default; must be a single tunable constant)

Interpretation:

* At `V = V_neutral`, time scale is 1.0.
* Increasing V reduces time scale (faster).
* Decreasing V increases time scale (slower).

## A4. Voltage Range Handling

Even though the manual indicates ±8V nominal range, users may patch beyond it.

* Apply `softClamp8()` to `V` before computing `f(V)`
* The logistic model already saturates; soft clamping adds stability and prevents extreme modulation from feeling unbounded.

## A5. Clamps for Safety

Clamp the computed timeScale to a sane range:

* `bothTimeScale ∈ [1/BOTH_TIME_SCALE_MAX, BOTH_TIME_SCALE_MAX]`
* Use `BOTH_TIME_SCALE_MAX = 64.0` (6 octaves of stretch)

Guard:

* avoid divide-by-zero by clamping `f(V)` ≥ 1e-6

## A6. Integration Point

Replace the existing BOTH exponential mapping inside the stage-time caching path.

Current pattern (conceptually):

* `bothScale = exp2(bothOct)`

New:

* `bothScale = bothTimeScaleFromCv(bothCv)`

Then continue to pass `bothScale` into `computeStageTime()` for both Rise and Fall.

This ensures BOTH affects both:

* cycling core
* non-cycle slew (because both use these stage times)

## A7. Implementation Reference (C++)

Add constants and helper functions (names may be adjusted to match style):

```cpp
static constexpr float BOTH_F_OFF_HZ = 1.93157058f;
static constexpr float BOTH_F_MAX_HZ = 986.84629918f;
static constexpr float BOTH_K_OCT_PER_V = 1.10815030f;
static constexpr float BOTH_V0_V = 4.15514297f;

static constexpr float BOTH_NEUTRAL_V = -0.05f;
static constexpr float BOTH_TIME_SCALE_MAX = 64.f;

static inline float bothHzFromCv(float v) {
    float r = std::pow(2.f, BOTH_K_OCT_PER_V * (v - BOTH_V0_V));
    return BOTH_F_OFF_HZ + BOTH_F_MAX_HZ * (r / (1.f + r));
}

static inline float bothTimeScaleFromCv(float v) {
    float vs = softClamp8(v);
    float f  = bothHzFromCv(vs);
    float f0 = bothHzFromCv(BOTH_NEUTRAL_V);

    float s = f0 / std::max(f, 1e-6f);
    return clamp(s, 1.f / BOTH_TIME_SCALE_MAX, BOTH_TIME_SCALE_MAX);
}
```

Then in stage time caching:

```cpp
float bothScale = bothTimeScaleFromCv(bothCv);
ch.cachedRiseTime = computeStageTime(riseKnob, riseCv, bothScale, shapeTimeScale);
ch.cachedFallTime = computeStageTime(fallKnob, fallCv, bothScale, shapeTimeScale);
```

---

# Part B — Signal IN Perturbs Running Core (Cycle/Triggered Function)

## B1. Problem

Currently, when `phase != IDLE`, the function generator integration path ignores Signal IN, so:

* Cycle ON + Signal IN patched → input has no effect (unlike hardware)

## B2. Desired Behavior

If Signal IN is patched while the outer channel is actively running (Rise/Fall):

* Map input voltage into the channel’s normalized integrator domain (`x ∈ [0..1]`)
* Gently pull the integrator state toward this mapped value each sample
* This produces the “input can bend the contour while it runs” behavior.

## B3. Coupling Law

Let:

* `x` be normalized integrator position derived from `ch.out`:

  * `x = (ch.out - V_MIN) / (V_MAX - V_MIN)`
* `x_in` be input voltage mapped similarly:

  * `x_in = clamp((softClamp8(inV) - V_MIN)/range, 0..1)`

Compute an attraction coefficient:

* `alpha = OUTER_INJECT_GAIN * (1 - exp(-dt / OUTER_INJECT_TAU))`
* clamp alpha to 0..1

Apply after the normal rise/fall integration step and before final clamp:

* `x += alpha * (x_in - x)`

## B4. Constants

Add:

* `OUTER_INJECT_GAIN = 0.55`
* `OUTER_INJECT_TAU  = 0.0015` seconds (1.5ms)

## B5. Integration Placement

Inside the function generator branch (phase != idle):

* compute `x_in` and `alpha` once per sample if signal is patched
* apply the perturbation in both rise and fall blocks after slope update

---

# Part C — Enforce Hardware-Like Speed Ceilings (Cycle vs Trigger)

## C1. Problem

When curve knob approaches linear/exp and BOTH + stage CV are high, stage times can shrink to ~100us, producing >1kHz (even multi-kHz) oscillation. This exceeds the intended Maths-like ceiling for self-cycling.

## C2. Desired Behavior

* Self-cycle (Cycle enabled) should not exceed **~1kHz**.
* External trigger should be allowed to exceed self-cycle ceiling up to **~2kHz**, to model “trigger can restart early” behavior.

This implies **two ceilings**:

* Cycle ceiling: `OUTER_MAX_CYCLE_HZ = 1000`
* Trigger ceiling: `OUTER_MAX_TRIGGER_HZ = 2000`

Convert to minimum periods:

* `MIN_CYCLE_PERIOD = 1 / OUTER_MAX_CYCLE_HZ`
* `MIN_TRIG_PERIOD  = 1 / OUTER_MAX_TRIGGER_HZ`

## C3. Enforce Using Total Period (Rise + Fall)

Enforce a minimum total period, not just per-stage min, and scale rise/fall proportionally to preserve ratio:

Helper:

```cpp
static inline void enforceOuterSpeedLimit(float& riseTime, float& fallTime, float minPeriod) {
    riseTime = std::max(riseTime, 1e-6f);
    fallTime = std::max(fallTime, 1e-6f);
    float p = riseTime + fallTime;
    if (p < minPeriod) {
        float s = minPeriod / p;
        riseTime *= s;
        fallTime *= s;
    }
}
```

## C4. Ceiling Selection Logic

Within `processOuterChannel()` after active times are updated:

1. Compute local `riseTime` / `fallTime` from active times.
2. Determine whether a trigger edge occurred this sample.
3. Choose minPeriod:

   * if trigger edge fired: use `MIN_TRIG_PERIOD`
   * else if cycle is enabled: use `MIN_CYCLE_PERIOD`
   * else: optional: no ceiling, OR use trigger ceiling when in triggered mode; recommended: apply trigger ceiling when running a triggered segment, and no ceiling only in pure slew mode if desired.

**Required behavior for your stated goal:**

* Cycle ON must always cap to 1k unless a trigger edge occurs.
* Trigger edge must use 2k cap even if cycle is ON.

## C5. Trigger Behavior Policy (must enable “retrigger during fall”)

To permit external triggers to outrun the self-cycle:

* Trigger edge must restart the function immediately regardless of current phase:

  * if rising or falling, restart rise from current output value or restart from baseline (choose one; see below)
* Recommended: restart rise using current output as the starting point (more analog-feeling; avoids discontinuity).

Codex must implement:

* edge detection
* immediate retrigger path executed before per-sample integration

## C6. Apply the limiter to BOTH modes

Use the clamped `riseTime` / `fallTime` locals in:

* function generator integration
* slew limiter processing (optional but recommended if you want “hardware speed sanity” for slewing too)

This ensures the curve knob and CV can’t create physically implausible ultra-fast response.

---

# Part D — Acceptance Tests

## D1. Calibration Curve (LOG side, modest CV)

With curve knob toward LOG, and same test conditions as the hardware capture:

* Cycle frequency should approximately match the 0–6V chart within ±2–3%.

## D2. Overdrive Scenario (LIN/EXP side + BOTH +10V)

With curve knob near linear/exp and BOTH = +10V:

* Cycle frequency must not exceed ~1kHz when Cycle is running without trigger edges.

## D3. Trigger Override

Under same high-speed settings:

* Applying trigger edges faster than the self-cycle must allow up to ~2kHz effective retriggering (as long as triggers are actually arriving at that rate).
* Behavior must remain stable and not explode.

## D4. Signal Injection

With Cycle ON and Signal IN patched:

* input must audibly and visibly perturb the running contour.
* A slow sine into Signal IN should warp the cycle.
* Audio into Signal IN should introduce timbral grit / modulation without destabilization.

---

# Implementation Order (Recommended)

1. Implement BOTH calibrated time scale (Part A).
2. Implement cycle vs trigger ceilings (Part C) — fixes the “4.21kHz” issue.
3. Implement Signal IN perturbation (Part B) — adds the hardware feel.

---

# Notes / Tunables

Expose these as constants initially (not UI):

* `BOTH_NEUTRAL_V` (default -0.05V)
* `OUTER_MAX_CYCLE_HZ` (default 1000)
* `OUTER_MAX_TRIGGER_HZ` (default 2000)
* `OUTER_INJECT_GAIN` (default 0.55)
* `OUTER_INJECT_TAU` (default 0.0015)

Later, optional: move “trigger ceiling” into your 2× digital mode while keeping cycle ceiling fixed.

---

If you want, I can also rewrite this spec into a “renderer-hardened” markdown (no tables, no fancy math notation, just ASCII + code fences) so it never breaks in the UI and Codex ingests it cleanly.
