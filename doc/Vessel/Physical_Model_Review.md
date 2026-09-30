# Vessel physical-model review

Reviewed 2026-09-30. Scope: the specification, source report, seed descriptors, and numerical oracle in this directory. The main specification has been revised in place to draft 0.2. This review does not implement or certify a Rack instrument.

## Assessment

The paired modal architecture is a strong starting point. Its force/velocity reciprocity, energy normalization, simultaneous-contact formulation, and separation of mechanical state from output processing are worth retaining. The central mathematical identities checked out. The existing numerical oracle passes when rerun.

The earlier readiness statement was too broad. The package establishes a passive numerical skeleton, not yet a faithful singing bowl. The highest risks are the omitted normal interaction, contact impedance under pole prewarping, unverified sustained moving-contact behavior, and insufficient structural bandwidth for impacts. No choice of stereo processing or output EQ will resolve those mechanical questions.

I retained the experimental numerical profiles rather than replacing uncalibrated parameters with new guesses. Changes below either correct an inconsistency, delimit an approximation, or make the necessary physical validation explicit. “Addressed” means the specification now handles the issue; it does not mean a production implementation has passed the new gate.

## Highest-priority findings

### 1. Rubbing pressure did not actually apply a normal force

**Impact:** high physical-fidelity risk. **Location:** §§9.2, 12, 18, 19.

The model uses pressure only as a multiplier in tangential friction. Calling it an ideal normal follower was inaccurate: even a follower that never loses contact applies a radial load and exchanges radial work with the bowl. The omission includes engagement transients, a travelling load, normal mechanical impedance, and modulation of friction by normal dynamics. It is broader than missing occasional rattling.

The specification now calls this a tangential-only proxy. It can be the first playable experiment, but a physically validated reference requires comparison against normal-force models. A prescribed radial-force experiment can be added without another nonlinear unknown: inject the known force before the existing port solve and add its work to the ledger. A compliant follower remains necessary for actual contact opening and normal-load feedback.

The geometry warning in §12 was good but underspecified. I added an explicit gap derivative and the corresponding orbit-actuator work. Sampling displacement at a moving angle introduces a spatial derivative in gap evolution; inserting that term into material velocity instead is wrong. Omitting trajectory work can break an otherwise convincing energy argument.

### 2. Correct free pitches concealed a distorted mechanical response

**Impact:** high, especially at upper tuning and during hard contact. **Location:** §§7.5, 16, 18.

The inverse-bilinear mapping is algebraically correct. However, fixing digital poles while keeping modal mass constant changes stiffness, damping, and forced-response residues. This matters because the resonators are inside a nonlinear mechanical loop.

For weak damping, the static-compliance ratio to the intended continuous oscillator is

`[π(f/Fint) / tan(π(f/Fint))]²`.

The additional checker confirms:

| Mode frequency / internal rate | Compliance relative to continuous target |
|---:|---:|
| 0.05 | 98.36% |
| 0.10 | 93.49% |
| 0.20 | 74.79% |
| 0.35 | 31.39% |
| 0.40 | 16.67% |

These are individual modal-compliance errors, not predicted total audible errors. They nevertheless show why a 0.40 rate ceiling is not a physical accuracy criterion. The crystal seed reaches about 46.6 kHz at the allowed 2 kHz base pitch, so this is relevant within the proposed controls, not only to hypothetical future modes.

The revised specification requires forced-mobility and contact-convergence comparisons and gives a provisional 2% modal-compliance target, with any broader band justified by measured contact sensitivity. The integrator can remain; the unresolved decision is its valid mechanical band and quality/rate policy. Increasing rate is one option, but its CPU cost must be measured before claiming the whole range is equally faithful.

### 3. Solver uniqueness and interface passivity do not establish singing

**Impact:** high; this is the instrument's central behavior. **Location:** §§7.6, 9.6, 18C.

The negative-slope bound and force bracket are sound. But a uniquely solved dissipative friction interface can either damp the bowl or feed it from imposed hand motion. Sustained sound requires negative incremental damping in the appropriate operating region, followed by a physically plausible bounded nonlinear state.

I added a local linearization: friction contributes `Φ'(s0) b bᵀ` to normalized modal damping. A negative slope can overcome losses. The additional diagnostics find maximum eigenvalue real parts between approximately 2.04 and 4.37 per second over 32 frozen angles for default Metal/Suede at 192 kHz. This supports plausibility, not a measured onset time: an orbiting contact changes orientation continuously, so it requires periodic-system analysis or direct trajectories. The different seed combinations also have substantially different growth rates; usability cannot be inferred from a common pressure knob range.

The revised gate distinguishes a transient or slow travelling-force response from self-sustained oscillation. It requires several complete rotations, perturbations of the established tone, cycle-averaged drive/loss balance, and bounded modal displacement. Neither the original nor the new checker performs that full experiment.

### 4. Regularized adhesion has a separate resolution problem

**Impact:** high for stopped rubbing, high pressure, and small slip. **Location:** §§7.6, 18C.

The uniqueness bound concerns the most negative slope. The positive slope at zero slip is `N μs/ε`, which can be much larger. An implicit midpoint step is passive but does not strongly suppress arbitrarily stiff relaxation. In a damping-only limit its multiplier approaches −1 as stiffness increases, allowing alternating numerical motion.

At 15 N, 192 kHz, C4, and the diagnostic angle, the seed values of `Ytt Φ'(0)` reach about 3.38 for Crystal/Silicone, despite a tiny negative-slope uniqueness number. That is a reason to inspect force/slip trajectories, not proof that the preset already produces audible chatter. The specification now requires epsilon/rate sweeps and reports this positive-slope measure separately. Exact static friction would need a different model; shrinking epsilon alone is not a validated substitute.

### 5. Material interpolation could inject unreported energy during impact

**Impact:** concrete energy-accounting defect. **Location:** §§6.3, 8, 10.

Holding normalized bowl state preserves its chosen energy during retuning, but it does not preserve a striker's kinetic energy when mass changes or contact energy when stiffness/exponent changes. As a numerical example, at 0.2 mm compression, changing the seed stiffness from Suede to Wood adds approximately **0.043 J** to the potential at unchanged compression. No hand or strike work in the original identity paid for that change.

The revised rule latches active striker mass, potential parameters, damping, footprint, and location until separation. Material changes affect the current rubbing law and the next impact. Any future active-contact morph must explicitly account for parameter work. Launches, retriggers, caps, emergency retirement, and model truncation also receive separate ledger entries. The prototype's relative launch-speed policy is now named so it is not mistaken for a fixed laboratory-frame strike velocity.

## Other substantive findings and corrections

### Structural bandwidth must converge independently of the timestep

Seven pairs can be a useful model, but a hard impact can probe omitted structural modes and residual compliance. At low tuning the retained upper frequency falls with the fundamental, while mallet stiffness and mass stay fixed. More oversampling cannot restore those missing modes. The specification now requires separate modal-enrichment and time-step studies, examining force, impulse, duration, rebound, and rubbed onset, rather than just output spectrum. Twelve pairs remain a budget, not a proof of sufficiency. Unsupported high-frequency modes must not be invented as measurements.

### Spatial normalization, force signs, and contact geometry needed explicit contracts

The radial/tangential shapes were correct but the inward striker convention was not reconciled with outward radial modal displacement. The revised coupling uses the inward projection `−φr` consistently for both force and feedback. Modal masses are tied to unit peak radial eigenvector normalization and already include all components of a mode's motion. Support and rigid-body motion are treated as constrained, with support loss folded into measured decay.

An azimuthal footprint applied to one nonlinear average-slip port is an aggregate approximation. It does not reproduce distributed local sticking, and its width is not yet derived from indentation or pressure. The revised text says so and requests spatial quadrature comparison if width becomes important.

### Gate release specified inconsistent motion

The old text froze angle when the gate went low but ramped contact away over 5 ms, potentially retaining tangential slip speed at a stationary contact location. Angle and imposed speed now use the same smoothed trajectory throughout release and freeze only when the contact is fully lifted. Independent strike and rub ports are explicitly two virtual actuators on one bowl, not one mallet in two places.

### Crystal identity and absolute mobility remain unresolved

The crystal ring formula is a useful synthetic profile, not a material identification. Modal masses, impact stiffness, and radiation gains are not separately identifiable from arbitrary uncalibrated microphone recordings. The revised calibration plan prioritizes normalized force-response measurements where available, multiple azimuths, joint fitting of doublets, support conditions, and actual specimen/material identity. It moves dry strike/rub comparisons into the early DSP gates rather than postponing all calibration until after Rack integration.

The original report's optical-Q analogy should remain excluded. I left that supplied report unchanged as source material; the specification already rejects using it to assign acoustic damping.

### Acoustic observation is still a virtual pickup

Two rim velocities do not constitute a calibrated acoustic pressure field. Radiation weights may require frequency-dependent phase and magnitude; radiation loss must not be counted twice if decay measurements already include it. The mallet footprint belongs in coupling, not observation. Natural rotation-induced envelope variation must arise from the solved paired states and fixed observer, including a symmetric-pair test, rather than added modulation. Pair orientation changes need a coordinate/state policy even when energy happens to stay constant.

### The nested contact solver's monotonicity claim holds

I derived the effective compression admittance after eliminating friction and checked positivity in 10,000 randomized positive-semidefinite port systems spanning both friction-slope signs. The striker inertia leaves a strictly positive term. This supports retaining the nested reference solver. It does not establish a 12-iteration production budget or accurate separation trajectories; those remain empirical implementation gates. The specification now includes the derivation and clarifies endpoint separation and interval contact force.

## Evidence and checks actually performed

The primary paper's [Table I and equation (5)](https://iypt.ru/wp-content/uploads/2024/10/The-dynamics-of-Tibetan-singing-bowls.pdf) agree with the copied Bowl 2 frequencies and paired component shapes. Its contact model includes radial interaction, and its analysis discusses rotating radiation patterns, supporting the distinction between the proposed proxy and fuller mechanics. The [Rack voltage standards](https://vcvrack.com/manual/VoltageStandards) agree with the proposed V/oct baseline, typical audio level, and Schmitt thresholds. These sources do not validate Vessel's seeds or numerical scheme.

The derivations, numerical warnings, and revised design policies above are review analysis, not additional claims attributed to those sources.

Executed on Windows using the bundled Python runtime and NumPy 2.3.5:

- `vessel_numerical_checks.py`: all existing pole, energy, friction, impact, and simultaneous-contact assertions passed; result JSON refreshed.
- `vessel_review_checks.py`: all added assertions passed. It records the compliance ratios, 10,000 outer-solve cases, 1,000 pair-basis port checks, eight profile combinations at 32 frozen angles, and a stationary-contact passive trajectory.
- The stationary-contact trajectory spans only 42.7 ms. Its energy decreased each step, with maximum ledger residual about `6.41e−19 J`. It does not establish a long tail or sustained singing.
- JSON parsing and repository whitespace/diff checks were performed after the edits.

No Rack source, panel, or plugin binary changed. No native plugin build, audio audition, full impact trajectory suite, long-duration rotating render, alias measurement, or CPU benchmark was performed. The revised specification explicitly requires those relevant implementation tests before release. No files were staged or committed.

## Recommended next implementation gate

Build the independent engine/harness through finite-mass strikes and moving friction before panel work. First establish complete event/contact energy accounting and mechanical convergence; then produce dry strike and several-rotation rub renders across a small measured reference set. Compare the tangential-only and normal-force models while that harness is still inexpensive to change. A convincing, converged, bounded physical interaction is the gate for proceeding to the Rack instrument, not the number of passing one-step algebra assertions.
