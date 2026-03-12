Dragon King Leviathan — I can absolutely draft a **Codex-ready SIMD implementation spec** with a **runtime toggle**. One quick note: my internal loader is only seeing the two performance review markdowns right now (the `.cpp` uploads aren’t currently accessible in the search index), so this spec is written to be **surgically pluggable** into your existing `IntegralFlux` codebase without depending on exact line numbers. If you want me to “patch against your exact file,” re-uploading the `.cpp` once more after this spec will let me write an exact diff.

What follows is a robust, practical SIMD architecture that mirrors the best parts of Rampage’s approach (SIMD + mask/blend) while respecting Integral Flux’s richer behavior and the MinBLEP reality. The plan also aligns with the known hotspots: transcendental math and per-sample recomputation .

---

## Codex-Ready Spec: Integral Flux SIMD Engine (Optional Toggle)

### Goals

* Add **optional SIMD DSP path** using `rack::simd::float_4`.
* Preserve **existing scalar behavior** when SIMD is off.
* SIMD path must:

  * Support Rack **polyphony up to 16 channels** per relevant input/output.
  * Avoid per-lane branching via **masking and blending**.
  * Keep MinBLEP scalar but only invoked per-lane when needed (hybrid).
* Be safe: identical results *within reasonable tolerance* when SIMD is enabled (tiny differences acceptable if approximations are introduced for speed).

### User-Facing Toggle

* Add a context-menu setting:
  **“Performance: SIMD engine”** (default: OFF initially)
* Store as:

  * `bool simdEnabled = false;` in module state.
  * Persist with JSON:

    * `dataToJson()` and `dataFromJson()` serialize `"simdEnabled"`.

---

# 1) Data Model Refactor (Polyphonic SIMD State)

### New Struct: `OuterChannelStateSIMD`

Replace scalar state for “outer channels” (CH1 and CH4) with a SIMD-aware state that can hold **16 lanes** as `float_4 lanes[4]`.

```cpp
struct OuterChannelStateSIMD {
    // 16-lane vectors: 4 vectors * 4 lanes = 16
    simd::float_4 out[4];          // current output / integrator state
    simd::float_4 phase[4];        // 0..1 phase
    simd::float_4 isRising[4];     // mask encoded as 0 or 1 float (or use simd::mask)
    simd::float_4 gateState[4];    // EOR/EOC gate state (float 0/1)
    simd::float_4 prevGateState[4];

    // Cached timing (per lane)
    simd::float_4 riseTime[4];
    simd::float_4 fallTime[4];

    // Cached “dirty” inputs for timing (per lane)
    simd::float_4 lastRiseKnobCv[4];
    simd::float_4 lastFallKnobCv[4];
    simd::float_4 lastBothCv[4];

    // Optional: cached warp scale / shape parameters per lane if needed
    simd::float_4 cachedShape[4];
    simd::float_4 cachedWarpScale[4];
    simd::float_4 warpScaleValid[4]; // 0/1 floats

    // Hybrid BLEP arrays (scalar, 16 lanes)
    dsp::MinBlepGenerator<16,16> gateBlep[16];
    dsp::MinBlepGenerator<16,16> signalBlep[16]; // if you keep signal BLEP
};
```

### Lane Utilities

Provide helpers:

* `static inline int vecCountFromChannels(int channels) { return (channels + 3) / 4; }`
* `static inline simd::float_4 load4(const float* p, int remaining)`
  Loads up to 4 floats and pads unused lanes (0).
* `static inline void store4(float* p, simd::float_4 v, int remaining)`

---

# 2) SIMD Math Primitives (Inline, No std::pow/tanh)

Because transcendental math is a primary hotspot , the SIMD path should **not call `std::pow` or `std::tanh`**.

### Required SIMD Functions

Implement these as `static inline`:

1. **Exp2 approximation**

* Replace `pow(2, x)` with Rack’s approximation where possible.
* If Rack provides a SIMD-friendly exp2 approximation, use it; otherwise:

  * Implement a small polynomial exp2 approximation that works with `simd::float_4`.
  * Or compute exp2 lane-wise only in the “recompute timing” section (decimated), not in the per-sample integrator.

2. **Fast tanh / saturation**

* Replace `std::tanh` with a polynomial rational approximation, SIMD-friendly:

  * `tanh(x) ≈ x * (27 + x²) / (27 + 9x²)`
* Clamp extreme inputs to keep stable.

3. **Clamp / abs / min / max**

* Use `simd::fmin`, `simd::fmax`, `simd::fabs` (or equivalent).

---

# 3) Control-Rate Timing Updates (SIMD-Compatible)

A key win is to stop recomputing timing every sample .

### Clock Divider

Add to module:

```cpp
dsp::ClockDivider simdCtrlDivider;
```

Initialize:

* `simdCtrlDivider.setDivision(16);` (tunable 8/16)

On each process:

* If `simdCtrlDivider.process()`:

  * Update cached rise/fall times for all active lanes.
  * Update warp scale cache if shape changed.

### Dirty Threshold

Even within the divided tick, you can avoid recomputing per-lane timing if inputs didn’t change:

* Compare `fabs(curr - last) > epsilon` per lane, create mask, and only update lanes that changed using `simd::ifelse`.

---

# 4) SIMD Core Slew/Envelope Step (Mask + Blend, No Branches)

### Function: `processOuterChannelSIMD(...)`

Inputs per vector-group:

* `in` (signal input, if slew mode uses it)
* `riseTime`, `fallTime`
* `shape`, `cycle`, `gate`, etc.

#### Rise/Fall Selection Without Branching

Compute `delta = target - out`.

Create mask:

* `maskRising = (delta > 0)`
  Then:
* `time = simd::ifelse(maskRising, riseTime, fallTime)`
* `rate = dt / time` (or equivalent depending on your integrator)

#### Integrator Update

Use a rate-limited integration step akin to Rampage:

* `step = clamp(delta, -maxStep, +maxStep)`
* `out += step`

Where:

* `maxStep = rate * someScale`

#### Shape/Warp

Apply warp shaping using SIMD primitives and `ifelse` blending.
Avoid `pow` unless exponent is constant (use multiply).

---

# 5) Hybrid MinBLEP (Scalar, Lane-Selective)

MinBLEP is scalar. Do not try to vectorize it.

### Rule

* SIMD core computes *what should happen*.
* BLEP is only invoked for lanes that changed state this sample.

#### Detect Transitions

`gateChangedMask = (gateState != prevGateState)` per lane.

Then:

* For each vector group `g` (0..vecCount-1):

  * Extract mask bits lane-wise:

    * `for lane=0..3: if laneChanged: gateBlep[laneIndex].insertDiscontinuity(...)`

#### BLEP Processing Cost Control

Do **not** call `blep.process()` for every lane every sample if inactive.
Track an `activeCountdown[16]`:

* When you insert a discontinuity, set to e.g. 32.
* Each sample: if `activeCountdown[i] > 0`, process BLEP and decrement; else skip.

This addresses the “BLEP running continuously” cost called out in the reviews .

---

# 6) Audio I/O and Polyphony

### Channel Counts

For each outer channel:

* Determine `channels = max(inputChannels, 1)` and clamp to 16.
* Set output channels accordingly.

### Vector Group Loop

```cpp
int vc = vecCountFromChannels(channels);
for (int g = 0; g < vc; ++g) {
    int base = g * 4;
    int rem = std::min(4, channels - base);

    simd::float_4 in = load4(inputs[...].getVoltages() + base, rem);
    // ... process ...
    store4(outputs[...].getVoltages() + base, out, rem);
}
```

---

# 7) Dual-Path Dispatch (Scalar vs SIMD)

In `process(const ProcessArgs& args)`:

```cpp
if (!simdEnabled) {
    processScalar(args);
} else {
    processSIMD(args);
}
```

Both paths must:

* Produce the same outputs
* Honor the same mode flags
* Update lights (lights can remain scalar / control rate)

---

# 8) Performance Modes & Safety

Because Integral Flux has richer behavior (non-ideal mix, warps), include optional sub-toggles for SIMD mode:

* **SIMD Core** (main toggle)
* **SIMD uses approximations** (on by default in SIMD mode)

  * fast exp2
  * fast tanh
* **SIMD disables signal BLEP** (optional)
* **SIMD EOR/EOC BLEP** (optional)

This matches the “risk ranking” strategy from the performance docs: cache timing, add fast tanh, optionally disable BLEP .

---

# 9) Benchmark Harness (In-Module)

Add a compile-time guarded instrumentation block:

* counts of timing recomputes
* counts of BLEP active samples
* time spent in `processSIMD` vs scalar

This directly targets the “verify hotspots with profiling” guidance .

---

## What Codex Should Implement First (Minimum Viable SIMD)

To keep this from becoming a “rewrite the world” moment, the MVP SIMD should do:

1. SIMD vector loop for outer channels (up to 16 polyphony)
2. Mask/blend rise/fall logic
3. Control-rate timing updates (divider)
4. Hybrid gate BLEP lane-selective (optional at first)
5. Toggle + JSON persistence

Then you benchmark.

---

### Why this will materially reduce CPU

* SIMD reduces per-lane overhead by grouping lanes.
* Control-rate timing removes frequent expensive recomputation .
* Avoiding `std::pow`/`std::tanh` removes major hotspots .
* Lane-selective BLEP stops paying the “BLEP tax” continuously .

---

If you want, paste (or re-upload) the current `processOuterChannel()` and `processUnifiedShapedSlew()` bodies you’re using right now, and I’ll tailor this spec into an **exact Codex patch plan** (function signatures, state fields, and the precise mapping from your scalar variables to SIMD lanes) so Codex can implement it with minimal hallucination risk.
