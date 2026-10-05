# Vessel — Refined Binaural Engine Specification & Implementation Plan

**Module:** Vessel (VCV Rack)  
**Document Status:** Shared-decimator implementation accepted; original shared-force proposal below is superseded  
**Authors:** Nexora Lumineth & Dragon King Leviathan  
**Target File:** `doc/Vessel/Binaural_Refined.md`  

---

## Accepted implementation (2026-10-05)

Keep **two independent nonlinear bowl engines**. Only their output filtering is
combined: one existing double-precision SSE2 `StereoDecimator` receives bowl 1's
left pickup and bowl 2's right pickup. No shared-force/slave resonator, AVX path,
precision reduction, or quality-default change is part of this implementation.

At zero separation, bowl 1 supplies both pickups. On a positive-to-zero crossing,
retune both engines without clearing their physical states, linearly fade the
right filter input from bowl 2 to bowl 1 over 50 ms at the internal sample rate,
then sleep bowl 2. Preserve the shared filter history. On wake, clone bowl 1's
pre-retune mechanics (including an active striker), retune independently, and
reverse the fade. Interrupted fades continue from the current mix. Positive-to-
positive separation changes do not restart the fade. Initial dual configuration
starts fully dual, as before.

The steady routes retain reference audio exactly. Moving the fade before filtering
intentionally changes transition samples relative to the old post-filter fade;
phase-dependent level dips remain possible. Host/internal-rate changes retain the
existing filter-reset and 5 ms output recovery policy. Configuration rejection is
transactional, and a processing fault clears the shared filter and mutes both
outputs before recovery. Energy telemetry still describes the independent bowls,
weighted by the current transition mix.

Validation retains independent `HostRateAdapter` references for steady audio and
mechanics, adds an internal-rate mix/filter oracle for interrupted transitions,
and checks 50 ms fold/wake timing and eventual single-bowl filter equivalence.
See `vessel-performance.md` for measured costs and validation results.

**Historical proposal follows.** Its shared-force architecture, performance
forecast, and unconditional physical/perceptual guarantees are not adopted.

### Shared-force screening result (2026-10-05)

An offline prototype now tests the historical proposal without changing the live
engine. Ten 60-second fixtures at 48 kHz host / 192 kHz internal rate show roughly
32–36% active-rubbing CPU savings relative to the independent engines with their
already-shared decimator. However, sustained right-channel levels and spectra
change substantially: Metal/Suede is 23 dB quieter at 10 Hz separation, and
Crystal/Suede is 43 dB quieter at 33 Hz. In the latter case the dominant
fundamental-region right peak moves from about 278.2 Hz to 246.8 Hz, close to the
245.2 Hz left peak. Independently configured modal poles do not guarantee an
independently sustained pitch under copied contact forces.

Zero-separation audio matches exactly; force-port energy residuals remain small.
Numerical stability therefore does not rescue the quality claim. Isolated
Wood/Silicone strikes retain their late pitches with smaller level changes, but
their attacks differ and no listening approval is claimed. **Do not promote
shared-force rubbing as a transparent optimization.** See the detailed
[screening results](vessel-performance.md#shared-force-resonator-screen-2026-10-05)
and `tools/vessel/probe_shared_force.py` for reproducible evidence and auditions.

---

## 1. Executive Summary

### 1.1 Problem Statement
In Vessel's current baseline, activating binaural separation (`BINAURAL_PARAM` $> 0\text{ Hz}$) instantiates and executes two independent, fully nonlinear physical bowl simulations (`HostRateAdapter left_` and `right_`). Each instance performs:
1. 14 scalar modal coordinates with symplectic state integration.
2. Nonlinear 3/2-power Hertzian contact solving with discrete gradient evaluation.
3. Nonlinear Stribeck friction root-finding with Newton-Raphson bisection.
4. Up to 8x oversampling with a cascaded 129-tap multi-stage Blackman-windowed sinc FIR stereo decimator.

Because both bowls run full independent solvers and separate decimators, **binaural mode increases DSP time by +94%** (jumping from $1.20\ \mu\text{s}$ to $2.32\ \mu\text{s}$ per host frame during rubbing). Furthermore, half of each decimator's output is discarded: Bowl 1's right channel and Bowl 2's left channel are never routed to the module outputs.

### 1.2 The Solution: Unified Master-Slave Architecture
This plan merges two synergistic architectural optimizations:
* **Option 1 (Master Solver + Slave Linear Resonator):** Bowl 1 (Left, tuned to $f_c - \Delta/2$) acts as the physical Master, solving the nonlinear contact dynamics with the mallet. The computed physical forces $F_{\text{strike}}(t)$ and $F_{\text{friction}}(t)$ are broadcast to Bowl 2 (Right, tuned to $f_c + \Delta/2$). Bowl 2 acts as a pure linear resonator bank, advancing its modes without any iterative root-finding.
* **Option 3 (Unified Stereo Decimator):** A single `StereoDecimator` processes $\{ v_{\text{obs, L}}(\text{Master}), \, v_{\text{obs, R}}(\text{Slave}) \}$ in parallel using packed 128-bit SSE2 SIMD instructions. Redundant decimation channels are eliminated.

### 1.3 Expected Impact
* **CPU Reduction:** $\approx 40\text{--}45\%$ reduction in dual-bowl processing time. Dual-bowl cost drops from $2.32\ \mu\text{s}$ to $\approx 1.35\text{--}1.40\ \mu\text{s}$ per host frame, bringing binaural mode within $\approx 15\%$ of single-bowl cost.
* **Physical & Perceptual Integrity:** Both ears hear genuine, independently resonating physical modal spectra with correct harmonic dispersion and mode-pair splits. The shared contact interaction mirrors the real-world acoustic ritual of a practitioner striking or rubbing two bowls with a single physical gesture.
* **Real-Time Safety:** Guaranteed passivity; zero dynamic allocations in the audio thread; glitch-free 50 ms transition between single-bowl and dual-bowl states.

---

## 2. Architecture & Mathematical Formulation

```text
                     ======================================================
                                   MASTER BOWL (Left: fc - Δ/2)
                     ======================================================
                     Mallet Controls (Speed, Pressure, Strike Gate, Velocity)
                                              |
                                              v
                                   [ Orbit & Kinematics ]
                                              |
                                              v
                              [ Nonlinear Contact Solvers ]
                                - Hertzian Strike Solver
                                - Stribeck Friction Solver
                                              |
                       +----------------------+----------------------+
                       |                                             |
                       v                                             v
               Normal Force F_s(t)                         Friction Force F_f(t)
                       |                                             |
                       +----------------------+----------------------+
                                              |
                                              v
                                 [ Modal Projection b(θ) ]
                                              |
                                 Modal Generalized Force f(t)
                                              |
                       +----------------------+----------------------+
                       |                                             |
                       v                                             v
              [ Master Modal Bank ]                         [ Slave Modal Bank ]
             Modes at fc - Δ/2 (Left)                      Modes at fc + Δ/2 (Right)
                       |                                             |
                       v                                             v
               Observer L Pickup                             Observer R Pickup
                       |                                             |
                  v_obs_L(t)                                    v_obs_R(t)
                       |                                             |
                       |                  [ Crossfader ]             |
                       |         (v_obs_R_Master <-> v_obs_R_Slave)  |
                       |                         |                   |
                       v                         v                   |
             +-------------------------------------------------------+
             |                 SINGLE STEREO DECIMATOR               |
             |       Stage 1/2/3 Cascaded 129-tap Blackman Sinc      |
             |             Lane 0: Left    Lane 1: Right             |
             +-------------------------------------------------------+
                                         |
                                         v
                                  Host Stereo Audio
```

### 2.1 Modal Coupling & Force Invariance
Let azimuthal order be $n \ge 2$, orientation be $\alpha_n$, and mode mass be $m_{j}$. For a contact at angle $\theta$ with angular patch width $w$:
$$b_{j, \text{radial}}(\theta) = \pm \frac{\text{patch}(n, w)}{\sqrt{m_j}} \cos(n(\theta - \alpha_n))$$
$$b_{j, \text{tangential}}(\theta) = \frac{\text{patch}(n, w)}{n \sqrt{m_j}} \cos(n(\theta - \alpha_n))$$

Because Bowl 1 (Master) and Bowl 2 (Slave) share the exact same physical bowl specimen (Metal Bowl 2 or Crystal Ring) and mallet material:
1. Azimuthal orders $n$, orientations $\alpha_n$, and modal masses $m_A, m_B$ are **identical** between Master and Slave.
2. The modal coupling vectors $\mathbf{b}_{\text{strike}}$ and $\mathbf{b}_{\text{rub}}(\theta)$ are **strictly identical** between both bowls.
3. Therefore, the scalar contact forces $F_{\text{strike}}(t)$ and $F_{\text{friction}}(t)$ solved by the Master map directly to the **identical generalized modal force vector** $\mathbf{f}(t)$:
   $$\mathbf{f}(t) = \mathbf{b}_{\text{strike}} F_{\text{strike}}(t) + \mathbf{b}_{\text{rub}}(\theta(t)) F_{\text{friction}}(t)$$

### 2.2 Slave Linear Integration
The Slave bowl has its own modal bank coefficients calculated at $f_R = f_c + \Delta/2$:
$$\omega_{j, R} = \text{hypot}(\sigma_{j, R}, \nu_{j, R}), \quad a_{j, R} = \frac{1}{2} h \omega_{j, R}, \quad D_{j, R} = 1 + h \sigma_{j, R} + a_{j, R}^2$$

For each internal sample step $t_k \to t_{k+1}$:
1. **Free Midpoint:**
   $$y_{j, \text{free}} = \frac{y_j(t_k) - a_{j, R} x_j(t_k)}{D_{j, R}}$$
2. **Force Injection (Direct from Master):**
   $$y_{j, \text{mid}} = y_{j, \text{free}} + \frac{h}{2 D_{j, R}} f_j(t)$$
3. **State Advance:**
   $$x_j(t_{k+1}) = x_j(t_k) + h \omega_{j, R} y_{j, \text{mid}}$$
   $$y_j(t_{k+1}) = 2 y_{j, \text{mid}} - y_j(t_k)$$

**Computational Complexity:**
* Exactly 9 arithmetic operations (multiply/add/subtract) per scalar mode.
* For 14 modes (7 pairs): $14 \times 9 = 126$ FLOPs per step.
* Zero divisions, zero transcendental evaluations, zero conditional bracketing branches.

### 2.3 Single Stereo Decimator & Glitch-Free Crossfading
The decimated stereo stream requires Left from Master and Right from Slave. However, when $\Delta = 0\text{ Hz}$, Vessel operates in single-bowl mode where Master produces true stereo via its own spatial observers $L$ and $R$.

To support seamless transitions:
$$\text{input}_{\text{decimator}}(t) = \begin{bmatrix} v_{\text{obs, L}}(\text{Master}, t) \\ (1 - \alpha) v_{\text{obs, R}}(\text{Master}, t) + \alpha v_{\text{obs, R}}(\text{Slave}, t) \end{bmatrix}$$
where $\alpha \in [0, 1]$ is the smoothed `dualMix` state transitioning over $50\text{ ms}$.

* When $\alpha = 0$: The Slave modal bank is paused (`rightActive_ = false`). The single Master bowl feeds both channels of the decimator.
* When $\alpha = 1$: Master feeds Left; Slave feeds Right. Master skips computing $v_{\text{obs, R}}$.
* The crossfade occurs at the internal oversampled rate prior to decimation, preserving filter memory and preventing audio-rate phase steps or clicks.

---

## 3. Structural Design & Code Interfaces

### 3.1 Architecture Overview

```
src/vessel/
  ├── Types.hpp                 (Existing modal and descriptor types)
  ├── ModalBank.hpp/.cpp        (Existing linear modal bank)
  ├── StrikeContact.hpp/.cpp    (Existing Hertzian contact solver)
  ├── FrictionContact.hpp/.cpp  (Existing Stribeck friction solver)
  ├── ContactSolver.hpp/.cpp    (Existing coupled solver)
  ├── VesselEngine.hpp/.cpp     (Refined: exposes solved modal force vector)
  ├── HostRateAdapter.hpp/.cpp  (Refined: supports master-slave dual processing)
  └── DualBowlAdapter.hpp/.cpp  (Refined: orchestrates master engine + slave bank)
```

### 3.2 Refinement in `VesselEngine`
`VesselEngine` needs to output the computed modal forces and allow slave stepping:

```cpp
struct StepForces {
    ModalVector force;
    double strikeForce = 0.0;
    double frictionForce = 0.0;
    bool active = false;
};

// In VesselEngine:
// step() returns EngineFrame as before, but also makes StepForces available
// to DualBowlAdapter, or returns a unified MasterFrame.
```

### 3.3 Refinement in `DualBowlAdapter`
Instead of wrapping two independent `HostRateAdapter`s:
```cpp
class DualBowlAdapter {
public:
    explicit DualBowlAdapter(bool allowSingle = true) noexcept;
    bool configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                   const EngineSettings& centerSettings, double separationHz,
                   double hostRate, ProcessingQuality quality = ProcessingQuality::Balanced) noexcept;
    void reset() noexcept;
    DualBowlFrame process(const HostControls& controls) noexcept;

    const VesselEngine& engine() const noexcept { return master_; }
    // Telemetry and energy reporting
    double meanEnergy() const noexcept;
    double leftEnergy() const noexcept { return master_.bowl().energy(); }
    double rightEnergy() const noexcept { return rightActive_ ? slaveBank_.energy() : master_.bowl().energy(); }

private:
    VesselEngine master_;               // Solves physics and integrates Left bowl
    ModalBank slaveBank_;               // Integrates Right bowl (linear only)
    ModalVector slaveObserverR_;        // Right observer for slave bowl
    StereoDecimator decimator_;         // Single shared stereo decimator
    // State & transition management
    bool allowSingle_ = true;
    bool rightActive_ = false;
    double dualMix_ = 0.0;
    double centerFrequency_ = 261.625565;
    double separationHz_ = 0.0;
    double hostRate_ = 0.0;
};
```

---

## 4. Step-by-Step Implementation Tasks

### Phase 1: Engine Force Extraction & Slave Resonator Path
* **Task 1.1:** Add `StepForces` struct and accessor in `VesselEngine` to capture the solved modal force vector $\mathbf{f}(t)$ during `step()`.
* **Task 1.2:** Add `ModalBank::advanceForced(const ModalVector& force, const ModalVector& observer, double& velocity)` as an optimized single-pass method that computes midpoint, commits force, and reads observer velocity in a single cache-friendly loop.
* **Task 1.3:** Create a standalone test fixture in `tests/vessel_engine_spec.cpp` validating that driving a second `ModalBank` with extracted master forces produces stable, bounded, dissipative vibration matching continuous physical resonance.

### Phase 2: Refactoring `DualBowlAdapter`
* **Task 2.1:** Update `DualBowlAdapter` member layout: replace `HostRateAdapter right_` with `ModalBank slaveBank_`, `slaveObserverR_`, and a single `StereoDecimator decimator_`.
* **Task 2.2:** Update `DualBowlAdapter::configure()` to configure `master_` at $f_c - \Delta/2$, `slaveBank_` at $f_c + \Delta/2$, and initialize `decimator_` once. Maintain transactional failure guarantees: if either fails validation, neither is modified.
* **Task 2.3:** Update `DualBowlAdapter::process()`:
  1. Process controls through `master_` to obtain `masterFrame` and `forces`.
  2. If `rightActive_`: advance `slaveBank_` using `forces`, obtaining $v_{\text{slave, R}}$.
  3. Form `StereoSample { masterFrame.leftVelocity, (1 - dualMix) * masterFrame.rightVelocity + dualMix * v_slave_R }`.
  4. Push sample through `decimator_`.
  5. Manage `dualMix` fade-in / fade-out ($50\text{ ms}$ ramp).
* **Task 2.4:** Update `DualBowlAdapter::reset()` to reset master, clear slave bank, and reset the single decimator.

### Phase 3: Test Suite Updates & Validation
* **Task 3.1:** Update `tests/vessel_dual_bowl_spec.cpp`:
  * Adapt `referenceEquivalence()`: verify that Master and Slave match expected separate frequencies ($f_c \pm \Delta/2$) and harmonic dispersion.
  * Verify that `dualMix` crossfade is completely click-free and monotonic.
  * Verify that zero separation ($\Delta = 0$) idles the slave bank and routes single-bowl stereo cleanly.
  * Verify long-tail decay and energy dissipation residuals remain $< 10^{-10}\text{ J}$.
* **Task 3.2:** Run `tools/vessel/benchmark_dual.cpp` to measure CPU time before and after the refactor. Verify the predicted $\approx 40\text{--}45\%$ reduction in host frame time.
* **Task 3.3:** Run all existing test binaries (`test-fast`, `vessel_module_spec`, `vessel_host_rate_spec`, `vessel_engine_spec`).

### Phase 4: Rack Integration & Audition Verification
* **Task 4.1:** Verify `Vessel.cpp` panel behavior:
  * Energy bar displays correct mean energy: $\frac{1}{2}(E_{\text{master}} + E_{\text{slave}})$.
  * Frequency readout displays accurate $f_L = f_c - \Delta/2$ and $f_R = f_c + \Delta/2$.
  * Pitch and fine-tuning CV smoothly update both bowls.
  * Binaural knob sweep ($0 \to 33\text{ Hz}$) transitions smoothly without audio dropouts or pops.
* **Task 4.2:** Perform listening auditions across all 4 mallets (Wood, Suede, Silicone, Felt) and both bowl materials (Metal, Crystal) under striking and rubbing.

---

## 5. Verification Gates & Success Criteria

| Gate | Target / Criterion | Verification Method |
| :--- | :--- | :--- |
| **Numeric Stability** | Cumulative energy residual $< 10^{-10}\text{ J}$ over 15s | `tests/vessel_dual_bowl_spec.cpp` sustained energy audit |
| **Passivity** | Slave bowl energy strictly non-increasing when $F(t) = 0$ | Automated unforced tail test |
| **Click-Free Transitions** | Max derivative step during $50\text{ ms}$ crossfade $< 10^{-3}\text{ V/sample}$ | Boundary crossfade sweep probe |
| **Zero Separation Sleep** | Slave bank consumes 0 FLOPs when $\Delta = 0$ | Verified via `rightActive_ == false` |
| **Performance Benchmark** | Dual rubbing CPU $< 1.45\ \mu\text{s/frame}$ (down from $2.32\ \mu\text{s}$) | `build/tools/vessel_benchmark_dual` |
| **Zero Heap Allocations** | Zero `new`/`malloc` during dual processing, crossfading, retuning | Trapped allocation test in `vessel_module_spec` |
