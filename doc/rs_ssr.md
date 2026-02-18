````markdown
# Rampage-Style Shaped Slew Replacement — Codex Spec v1.0
Target: Replace **Maths Channel 1 “Slew mode”** (the current hard rate-limiter) with a **Rampage-style shaped integrator**.
Scope: **Only** the “slew/lag when an external signal is patched” path for Channel 1. Do **not** change Ch.1 function-generator/cycle envelope logic in this phase.

---

## 0) Non-negotiable behavior
- Separate **Rise** and **Fall** behavior selected by the sign of `delta = in - out`.
- Continuous **Shape** control spans **Log ↔ Linear ↔ Exp**.
- Shape affects the **dynamics inside the integrator** (timing changes), not a post-wave-shape.
- Must be stable across sample rates: use `dt = args.sampleTime`.
- Must not clamp the input signal (allow bipolar and >10V); the integrator handles sign via `delta`.

---

## 1) Locate & replace (what to change)
In `Maths.cpp`, find Channel 1 “slew mode” block (currently does a hard slew limit like):
- compute `maxStep = riseRate*dt` or `fallRate*dt`
- `out += clamp(delta, -maxStep, +maxStep)`

**Replace that entire hard-slew section** with the shaped integrator defined below.

Do not keep the old limiter as a fallback.

---

## 2) Add helper: ShapedSlewRampage (or equivalent)
Add a small helper struct/class near the module code (inside `struct Maths` or as a file-local helper).

### State
- You may either:
  A) Keep using the existing `ch1Out` as state and implement the helper as a pure function, or
  B) Store `float out` inside helper and sync with `ch1Out`.
- Prefer A) to minimize disruption.

### Pure function signature (recommended)
```cpp
static float processRampageSlew(
    float out,
    float in,
    float riseKnob01,
    float fallKnob01,
    float shape01,     // 0..1 (panel), will remap to -1..+1
    float riseCvV,     // volts
    float fallCvV,     // volts
    float bothCvV,     // volts (bipolar)
    float dt
);
````

---

## 3) Parameter/CV mapping (Maths-ish UI, Rampage-ish internal law)

We emulate Rampage’s `tau = minTime * 2^(rateExp)` but adapt to Maths UI:

* Rise knob: 0..1
* Fall knob: 0..1
* Shape knob: 0..1 (0=log, 0.5=lin, 1=exp)
* Rise CV input: volts
* Fall CV input: volts
* BOTH CV input: volts (bipolar; positive should make it faster)

### Constants (tunable but fixed for v1.0)

```cpp
static constexpr float MIN_TIME = 0.01f;  // seconds (tune later if needed)
static constexpr float VREF     = 10.0f;  // voltage reference used by Rampage law
static constexpr float E        = 2.718281828f;
```

### Convert knob/CVs into exponent `rateExp` in [0, 10]

Choose stage controls based on `delta` sign:

* if `delta > 0`: stageKnob = riseKnob01, stageCV = riseCvV
* else if `delta < 0`: stageKnob = fallKnob01, stageCV = fallCvV
* else return out (no change)

Compute exponent contributions:

```cpp
float baseExp  = 10.f * stageKnob;                 // knob 0..1 -> 0..10
float stageExp = clamp(stageCV, 0.f, 10.f);        // volts -> 0..10 (unipolar)
float bothExp  = -clamp(bothCvV, -8.f, 8.f) / 2.f; // bipolar: +V => faster => subtract exponent
float rateExp  = clamp(baseExp + stageExp + bothExp, 0.f, 10.f);
float tau      = MIN_TIME * powf(2.f, rateExp);
```

Notes:

* This keeps your old BOTH behavior conceptually: `time *= 2^(-both/2)` is equivalent to adding `(-both/2)` to the exponent.
* If your module has attenuverters on CVs, apply them before the clamps above.

---

## 4) Rampage-style shaped slope law (must match these formulas)

Let:

```cpp
float delta    = in - out;
float absDelta = fabsf(delta);
float sgn      = (delta >= 0.f) ? 1.f : -1.f;
```

Compute candidate slopes in V/s:

### Linear baseline

```cpp
float lin = sgn * (VREF / tau);
```

### Log branch (shapeSigned < 0)

```cpp
float logv = sgn * (4.f * VREF) / tau / (absDelta + 1.f);
```

### Exp branch (shapeSigned > 0)

```cpp
float expv = (E * delta) / tau;
```

### Shape remap and blend

Remap panel shape 0..1 → signed [-1..+1]:

```cpp
float shapeSigned = clamp(shape01 * 2.f - 1.f, -1.f, 1.f);
```

Blend (keep these constants):

```cpp
float dVdt;
if (shapeSigned < 0.f) {
    float mix = clamp((-shapeSigned) * 0.95f, 0.f, 1.f);
    dVdt = lin + (logv - lin) * mix;
} else if (shapeSigned > 0.f) {
    float mix = clamp(( shapeSigned) * 0.90f, 0.f, 1.f);
    dVdt = lin + (expv - lin) * mix;
} else {
    dVdt = lin;
}
```

---

## 5) Integrate + overshoot protection

Euler integrate:

```cpp
float prevOut = out;
out += dVdt * dt;
```

Overshoot snap (recommended, prevents ringing when very fast):

```cpp
if ((in - prevOut) * (in - out) < 0.f) {
    out = in;
}
```

Return `out`.

---

## 6) Wiring into Channel 1 slew path

In the Channel 1 slew-mode branch, do:

* Read input signal being slewed: `in = inputs[INPUT_1_INPUT].getVoltage();` (or your actual CH1 signal input)
* Read stage knobs:

  * `riseKnob01 = params[RISE_1_PARAM].getValue();`
  * `fallKnob01 = params[FALL_1_PARAM].getValue();`
  * `shape01    = params[LIN_LOG_1_PARAM].getValue();`
* Read CV inputs:

  * `riseCvV = inputs[CH1_RISE_CV_INPUT].getVoltage();`
  * `fallCvV = inputs[CH1_FALL_CV_INPUT].getVoltage();`
  * `bothCvV = inputs[CH1_BOTH_CV_INPUT].getVoltage();`

Then:

```cpp
ch1Out = processRampageSlew(ch1Out, in, riseKnob01, fallKnob01, shape01, riseCvV, fallCvV, bothCvV, dt);
```

Do not clamp `in` and do not clamp `ch1Out` (unless your module already has output limiting; if so keep it at the very end).

---

## 7) Acceptance checks (Codex must implement a minimal test harness or debug assertions)

Create a small offline test function (or unit test) that runs at dt = 1/48000 for a step input:

### A) Linear: 0→10V completes in ~tau

Force:

* shape01 = 0.5 (shapeSigned ~ 0)
* tau forced to 1.0s (override mapping in test)
  Then: out reaches 10V at ~1s (within a small tolerance).

### B) Exp: approaches target exponentially with rate e/tau

Force:

* shape01 = 1.0 (shapeSigned = +1)
* tau = 1.0
  Then delta should decay ~exp(-(E/tau)t).
  Check at t = 1/E: delta ≈ delta0 / e.

### C) Rise/Fall split uses different times

Set Rise knob fast, Fall knob slow; verify rising step is faster than falling step.

---

## 8) Tuning knob (allowed post-merge)

After behavior matches qualitatively, only tune:

* `MIN_TIME` (0.001, 0.01, 0.1) to adjust overall time range.
  Do not change the slope formulas or blend constants unless explicitly instructed.

---

## 9) Implementation instruction to Codex

* Make the smallest patch that fully replaces the Ch.1 slew limiter.
* Keep the existing envelope/cycle logic untouched.
* Ensure compile passes and no unused-variable warnings.

```

If you paste that spec + your `Maths.cpp` into Codex with:  
**“Implement exactly this spec; replace the Channel 1 slew limiter; don’t change envelope/cycle code”**  
…Codex should land very cleanly.

If you want, I can also generate a **one-paragraph “Codex command header”** you put above the spec (Codex responds well to a short imperative preface).
::contentReference[oaicite:0]{index=0}
```
