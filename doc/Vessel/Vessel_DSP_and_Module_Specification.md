# Vessel — DSP and Module Specification

**Plugin:** Leviathan for VCV Rack  
**Working module name:** Vessel  
**Document status:** Implementation draft 0.2 — physical-model review incorporated

**Prepared:** 2026-09-30

## 0. Scope, evidence, and readiness

Vessel is a tunable, struck and friction-excited singing bowl. A single persistent mechanical model receives both kinds of excitation. Stereo audio and an energy display observe that model. Optional binaural-beat processing sits after it and must not alter its mechanics.

This document starts from the supplied **Singing Bowl Acoustics and Physical Modeling** report, stored here as `Singing Bowl Research.md` (originally named `Singing Bowl Research(1).md`). It retains the report's organizing ideas: modal/banded resonators, metal versus crystal, mallet-dependent impact/friction, real-time feasibility, and musical parameter mapping. It selects a modal implementation and supplies the equations and engineering decisions the report leaves open.

Evidence labels used here:

| Label | Meaning |
|---|---|
| **R — Report** | An approach or observation supported by the supplied report. |
| **V — Verification** | A narrowly scoped check against a primary source, identified in §21. |
| **D — Design** | A proposed Vessel engineering decision, equation, interface, or test. Not a recovered implementation. |
| **U — Uncalibrated** | A provisional value or behavior requiring measurements, listening, or numerical validation. |

Unless explicitly identified as R or V, specifications below are **D**. Numerical profile defaults are **U**, except the explicitly identified published metal frequencies. Values in seconds, kilograms, newtons, and meters are internally dimensioned modeling parameters; uncalibrated values must not be advertised as measurements of a commercial instrument.

**Ready for an offline prototype:** the paired modal core, energy-consistent integration, finite-mass strike, tangential-only friction model with a prescribed load parameter, control interface, stereo observation, and energy telemetry. This is not yet a validated physical reference instrument. See `Physical_Model_Review.md` for the review findings and `review_check_results.json` for the additional numerical diagnostics.

**Not yet established across the supported controls:** sustained moving-contact onset and saturation; forced mechanical response across sample rates; sufficient modal bandwidth for hard impacts; realistic metal/crystal calibration; mallet friction curves; acoustic radiation; CPU cost in Rack; alias rejection; and normal-contact/contact-loss behavior. Limited C++ default-contact trajectories now supplement the algebra checks, but neither constitutes an audio-quality or performance certification. Binaural processing remains optional and downstream of these questions.

**Do not replace missing calibration with fabricated measurements.** Preserve provenance at the individual descriptor-field level.

**Implementation progress (2026-09-30):** the standalone C++ modal bank, finite-mass strikes, moving friction, coupled strike/rub solver, event/drive/loss ledger, optional prescribed radial-load experiment, and offline render/characterization tools are now implemented. `Implementation_Status.md` records native Windows strike evidence, 26 passing Linux test groups, 51 limited contact-characterization runs, and a registered 16 HP Rack prototype with tuning/CV, controls, mean-energy display, settings serialization and the scalar dual-bowl tuning reference. The remaining calibration, mechanical convergence, live Rack/Windows integration and performance gates are recorded there. Default Metal/Suede starts from rest and develops a bounded several-rotation response; other profiles have materially different onset behavior. These results supplement the design oracles; they do not upgrade the synthetic descriptors to measured data.

---

## 1. Instrument contract

Vessel must provide tunable bowl frequency, a strike gate, strike velocity, a rotation gate, continuous rotation speed, bowl material selection, mallet material selection, stereo audio, and an energy bar.

**Basic-frequency tuning is required.** One PITCH/FINE control and V/oct input retune the bowl's full modal spectrum around the lowest pair's center frequency. Both strikes and rubbing excite this same tuned bowl. Tuning must preserve a ringing tail and active contact state; it must not restart the bowl or change rotation speed, pressure, or mallet properties. The initial range is 20–2000 Hz with C4 as the default; §6 defines the mapping and transition policy.

Add **normal pressure** as a first-class control. It is independent of speed. A knob is required; pressure CV is strongly recommended. The report already identifies force and speed as separate rubbing controls (R, source report lines 19 and 39).

The following behaviors are mandatory:

1. Strike and rotation may operate simultaneously. They are not mutually exclusive modes.
2. Retriggering adds an interaction to the existing bowl state. It never resets resonator phase or clears a ringing tail.
3. Releasing the rotation gate removes contact over a short ramp. It does not mute the bowl.
4. Zero rotation speed with contact still engaged can damp an already-ringing bowl; it is not equivalent to lifting the mallet.
5. Changing output level, stereo width, or binaural settings must not change the mechanical state or the energy bar.
6. Material changes affect model descriptors and contact behavior, not merely output EQ.

Initial scope is **one continuously sounding bowl per module**, with monophonic CV inputs and two monophonic audio outputs. Repeated strikes overlap mechanically on that bowl. Polyphonic cables and multiple independent bowls are later work, not implied by the stereo outputs.

The independent strike and rub ports represent **two virtual actuators** on one bowl. They may share a selected mallet profile, but do not represent one rigid mallet simultaneously following two incompatible trajectories. A single-mallet strike-to-rub transition with shared radial momentum belongs to the normal-contact extension. All contacts and observers are near the rim; an azimuth control is not a height or rim-to-base control.

Do not include a built-in reverb, tuned background oscillator, “healing frequency” mode, or automatic loudness compensation in the reference engine. Do not use an LFO to manufacture the primary bowl shimmer.

---

## 2. What is retained, clarified, and deliberately left open

### 2.1 Retained from the report

Use a small bank of damped resonances; excite it through impact and nonlinear rubbing; make material a structured set of modal/contact parameters; make strike position affect modal coupling; and profile the result instead of running a full shell simulation at audio rate. These are the report's proposed architecture, not a claim that any particular coefficient set is already known (R, lines 7–11, 19, 23–27, 31–43).

### 2.2 Essential clarification from the underlying literature

The paired families are A/B orientations of shell modes; **each has radial and tangential components**. They are not one radial resonator bank plus one unrelated tangential bank. The primary paper supplies the component shapes used in §4 and the metal frequencies in §5 (V1, pp. 640–642).

This is the only anatomical interpretation used by Vessel's reference engine. The supplied report's shorter description of “radial and tangential families” is not carried into code literally.

### 2.3 Claims the report does not establish numerically

The report does not provide a measured crystal spectrum, modal masses, per-mode acoustic decay times, friction coefficients, or a fitted radiation model. Its statements about crystal purity, harmonicity, and longer sustain are qualitative starting points, not descriptor data. Optical Q is not used to assign acoustic decay. The broad material claims remain unverified here.

Likewise, the report's illustrative `F = μ * v` expression is not adopted as the dry-friction law. Vessel explicitly defines a relative-velocity-dependent force in §9. The exact Rack DSP class names mentioned in the report are not dependencies; the core below is standalone C++.

The report's filter-operation estimate is not an estimate for this complete engine. Implicit contact, oversampling, stereo observation, parameter updates, and optional binaural processing all require separate profiling.

---

## 3. Chosen architecture

```text
Pitch / specimen / decay / imperfection
                    |
             Modal descriptor builder
                    |
Strike gate -> finite-mass normal impact ---------+
                                                  |
Rotate gate + speed + pressure -> friction contact +--> ONE paired modal state
                                                  |
                           feedback surface velocity <+
                                                       |
                                  +--------------------+------------------+
                                  |                                       |
                       Fixed stereo observations                  Mechanical energy
                                  |                                       |
                    Anti-alias downsampling                          Energy display
                                  |
                  Native stereo OR binaural-beat renderer
                                  |
                      Gain / DC handling / safety
                                  |
                              L and R
```

Use **7 mode pairs / 14 scalar coordinates** for the initial metal reference profile. Allow up to 12 pairs in fixed-capacity storage. Add modes only when a descriptor or convergence experiment justifies them. Do not multiply independent nonlinear contact solvers by the number of modes.

The bowl is linear between interactions. Its nonlinear behavior initially comes from the moving, velocity-dependent friction contact and the finite-mass nonlinear impact. Do not add global Duffing distortion or energy-dependent pitch bending by default. Such effects require independent evidence or an explicitly non-reference character control.

---

## 4. Mechanical representation and reciprocal coupling

### 4.1 Coordinates

For each scalar coordinate `j`, store mass `m_j > 0`, target frequency `f_j > 0`, amplitude decay time `T60_j > 0`, and two energy-normalized state variables `x_j`, `y_j`.

For fixed numerical coefficients:

\[
\dot x_j=\widehat\omega_j y_j,
\qquad
\dot y_j=-\widehat\omega_j x_j-2\widehat\sigma_j y_j
          +\sum_k b_{jk} F_k.
\]

The corresponding displacement and velocity coordinates are:

\[
q_j=\frac{x_j}{\sqrt{m_j}\widehat\omega_j},
\qquad
v_j=\frac{y_j}{\sqrt{m_j}}.
\]

`omegaHat` and `sigmaHat` are integration coefficients defined in §7; they are not simply `2πf` and `ln(1000)/T60` at finite sample rate. The stored energy is:

\[
E_{bowl}=\frac12\sum_j(x_j^2+y_j^2).
\]

This is the energy of Vessel's normalized mechanical model. It has an internally consistent energy scale but is not a calibrated acoustic sound-pressure measurement.

### 4.2 Paired mode shapes at the rim

Let `n = 2, 3, ...` be azimuthal order and `α_n` the specimen's orientation for that pair. With `β = n(θ−α_n)`, use:

\[
\phi^r_{n,A}=\cos\beta,
\quad \phi^r_{n,B}=\sin\beta,
\]
\[
\phi^t_{n,A}=-\frac{\sin\beta}{n},
\quad \phi^t_{n,B}=\frac{\cos\beta}{n}.
\]

The radial/tangential form is V1, equation (5). Per-pair orientation offsets and finite contact patches are Vessel design additions. Pair orientations are fixed specimen data, not per-strike randomness.

Take positive radial displacement as **outward**, and positive tangential displacement along increasing `θ`. Normal impact uses a signed radial projection: an external mallet travelling inward uses `φs = −φr`, including the same sign in force injection and feedback. Its scalar force and striker velocity are positive inward. Rubbing uses `φt`. Audio initially observes outward radial velocity through a separate output weighting. This sign convention also applies to a future inward normal rubbing force.

The shapes assume a small-displacement, approximately inextensional rim: `φr + ∂φt/∂θ = 0`. This is a reduced shell/ring approximation, not a general measured eigenvector for every bowl shape. The normalization is unit peak **radial** displacement; effective masses must use that same normalization and already include all motion of the mode. Do not add a second tangential mass or multiply mass by `1+1/n²` after fitting. A change of eigenvector normalization requires the corresponding mass transformation.

The omitted support and rigid-body degrees of freedom are treated as constrained. Support losses are included in fitted T60, rather than claiming a freely floating bowl. Crystal and deep/thick bowls may need measured radial/tangential ratios or additional meridional mode families; validate the seven rim-bending pairs before generalizing them.

### 4.3 Finite contact patch

An angularly uniform patch of full width `w` can use the following normalized spatial average:

\[
P_n(w)=\operatorname{sinc}(nw/2),\qquad
\operatorname{sinc}(z)=\frac{\sin z}{z},\quad\operatorname{sinc}(0)=1.
\]

Multiply both shapes in the pair by `P_n`. Wider patches reduce excitation of high spatial orders. Use the same patch-averaged shape for force injection and contact-velocity feedback.

This is an explicit spatial approximation, not a measured mallet contact-pressure distribution. Profiles may later replace it with integrated nonuniform patch shapes.

For friction, this is a **single aggregate contact port**, not distributed local stick/slip. In general, applying a nonlinear friction law to average slip differs from integrating local friction over a patch. Retain this inexpensive approximation for narrow patches; compare it with spatial quadrature if width materially affects mode selection. Keep `N` and `F` as total forces, not force per radian. Width is independent of pressure in this prototype; real contact area and indentation are not yet coupled.

### 4.4 Reciprocity is mandatory

For a contact `k`, define:

\[
b_{jk}=\frac{\phi_{jk}}{\sqrt{m_j}},
\qquad
v_k=\sum_j b_{jk}y_j.
\]

The **same** `b_jk` must inject force and read surface velocity. Then the contact's power delivered to the bowl is `F_k v_k`. Independent arbitrary gains on feedback and force injection would break the energy accounting.

Radiation/output gains do not belong in `b_jk`. Do not normalize `b` every sample to equalize loudness as the mallet moves. Spatial nodes and changing contact admittance are intentional.

---

## 5. Modal descriptors and initial specimens

### 5.1 Metal starting point with published frequencies

Use the following **frequency pairs only** from Bowl 2 in V1, Table I, p. 640:

| Azimuthal order | A, Hz | B, Hz |
|---:|---:|---:|
| 2 | 310.2 | 312.1 |
| 3 | 828.1 | 828.8 |
| 4 | 1503.4 | 1506.7 |
| 5 | 2328.1 | 2340.1 |
| 6 | 3303.7 | 3312.7 |
| 7 | 4413.2 | 4416.4 |
| 8 | 5635.4 | 5642.0 |

Do not identify the complete Vessel preset as a measured reproduction. Its masses, losses, orientation offsets, contact parameters, radiation gains, and playable tuning are separate provisional choices.

Normalize each pair using:

\[
f_{ref}=\sqrt{310.2\cdot312.1},
\quad r_n=\frac{\sqrt{f_{n,A}^{ref}f_{n,B}^{ref}}}{f_{ref}},
\quad d_n=1200\log_2\frac{f_{n,B}^{ref}}{f_{n,A}^{ref}}.
\]

Store original frequencies and provenance alongside derived ratios. Do not replace the published numbers with rounded ratios.

### 5.2 Crystal starting point: explicitly synthetic

The supplied report contains no measured crystal modal table. Until one is obtained, use a **Crystal / prototype** descriptor with an analytic ring-like sequence:

\[
r_n=\frac{n(n^2-1)/\sqrt{n^2+1}}{2(2^2-1)/\sqrt{2^2+1}},
\qquad n=2\ldots8.
\]

This is a geometry-inspired prototype, **not a quartz-specific material law** and not evidence that crystal bowls follow this exact spectrum. Its different decay and contact settings are sound-design choices. A crystal preset must not be implemented by forcing the modes onto an integer harmonic series merely because the report describes a “purer” sound.

### 5.3 Initial uncalibrated per-pair values

Both members initially share mass, nominal decay, and radiation gain. These defaults are also included in `vessel_seed_profiles.json`.

| Order | Metal mass, kg | Metal T60, s | Crystal mass, kg | Crystal T60, s | Metal radiation gain | Crystal radiation gain |
|---:|---:|---:|---:|---:|---:|---:|
| 2 | 0.090 | 24.0 | 0.060 | 35.0 | 1.00 | 1.00 |
| 3 | 0.085 | 14.0 | 0.056 | 19.0 | 0.55 | 0.32 |
| 4 | 0.080 | 9.0 | 0.052 | 10.0 | 0.33 | 0.16 |
| 5 | 0.075 | 6.0 | 0.048 | 6.5 | 0.22 | 0.10 |
| 6 | 0.070 | 4.5 | 0.044 | 4.5 | 0.15 | 0.07 |
| 7 | 0.065 | 3.2 | 0.040 | 3.0 | 0.10 | 0.045 |
| 8 | 0.060 | 2.5 | 0.038 | 2.0 | 0.07 | 0.030 |

Modal masses are effective coordinate masses. Their sum is not the bowl's total mass. Do not derive them by dividing a bowl's total weight equally among resonators.

The initial effective rim radii are 0.076 m for Metal and 0.10 m for Crystal, both designated **U** here. These set tangential speed from rotation; they do not change automatically with V/oct tuning.

### 5.4 Required descriptor fields

```cpp
struct ModePairDescriptor {
    int azimuthalOrder;
    double centerRatio;
    double splitCents;
    double orientationRadians;
    double massA, massB;
    double t60A, t60B;
    double radiationA, radiationB;
};

struct BowlDescriptor {
    int descriptorVersion;
    const char* stableId;
    double effectiveRimRadius;
    int pairCount;
    std::array<ModePairDescriptor, 12> pairs;
    // Provenance and calibration metadata live in the offline source data.
};
```

Descriptors are versioned and deterministic. Store the selected descriptor version in patches. Updating a preset's coefficients later must not silently change older patches.

---

## 6. Tuning, imperfection, damping, and state continuity

### 6.1 Pitch

Use C4 = 261.625565 Hz at 0 V with the pitch knob at its reference position:

\[
f_0=\operatorname{clamp}\left(261.625565\;2^{p+V_{oct}+c/1200},\;20,\;2000\right).
\]

`p` is the coarse offset in octaves; `c` is fine tuning in cents. V/oct behavior follows the Rack convention (V2). The permitted range is a Vessel design choice.

The pitch label means the **geometric center of the lowest mode pair**, not every audible peak and not an assurance that rubbing always selects the lowest mode.

For imperfection amount `I`, initially 0–2 with default 1:

\[
f_{n,A}=f_0r_n2^{-I d_n/2400},
\qquad f_{n,B}=f_0r_n2^{+I d_n/2400}.
\]

At `I=0`, preserve both coordinates with identical frequencies. Do not collapse the pair: the two orientations are still needed for moving contact and observation.

Pitch is a **musical retuning control**, not a literal geometry-rescaling operation. Keep rim radius and modal masses fixed during V/oct modulation. Do not silently couple octave transposition to rotation speed, pressure, or mallet contact stiffness.

### 6.2 Decay

`T60` means time for a mode's free-decay amplitude envelope to fall by 60 dB:

\[
\sigma_{target}=\frac{\ln1000}{T60},\qquad r=e^{-\sigma_{target}h}.
\]

For a lightly damped oscillator, the familiar continuous-time relation is:

\[
Q\approx\frac{\pi f T60}{\ln1000}.
\]

Use T60 as the user-facing calibration primitive. Do not interpolate arbitrary Q values and assume the same decay in seconds at all pitches.

The DECAY control multiplies all nominal T60 values by a common logarithmically mapped factor, initially 0.25–4.0. Clamp individual values to 0.1–120 s. Holding decay time fixed while changing pitch is an intentional musical policy, not a claim about physically resizing a bowl.

An optional DAMP control adds nonnegative loss. It must never set negative damping to fake sustain; sustain comes from contact work.

### 6.3 Parameter transitions

Maintain `x,y` when retuning so the normalized stored energy does not jump. This changes the displacement interpretation as stiffness changes; it is an energy-preserving musical retune, not a literal impulsive deformation of a physical bowl.

Use approximately 1–2 ms smoothing for pitch in log-frequency space, 10 ms for speed/pressure targets, and 100 ms for material transitions. Update modal integration coefficients through nonnegative `omegaHat,sigmaHat`, not by interpolating arbitrary direct-form filter coefficients.

A material switch between matched A/B orders may interpolate log frequency, log positive mass, log positive decay, and radiation/contact parameters while holding normalized state continuous. Do not swap A/B identities. When topology differs, use a separately specified state projection or a short dual-engine transition; do not reinterpret array slots.

**Active-strike exception:** latch striker mass, stiffness, exponent, loading damping, patch width, and strike angle at contact creation, and keep them until separation. A material change updates the rubbing contact and the next striker. Do not interpolate an active striker's mass or contact potential: that changes its kinetic or elastic energy outside the stated ledger. If a future model permits such changes, explicitly account for parameter work. For example, changing `k_s` at fixed compression adds `Δk_s * max(δ,0)^(p+1)/(p+1)` joules.

Pair orientation is a coordinate-basis decision. Keep orientations identical across interpolated prototype descriptors. A future change of orientation requires a defined state/basis transformation; it must not silently rotate the spatial vibration pattern while calling the operation ordinary mass/frequency interpolation. `I=0` removes frequency splitting only; exact rotational-symmetry tests must additionally equalize A/B masses and damping within each pair.

No audio-rate FM claim is made for draft 0.2. Test ordinary pitch CV and envelopes first. An eventual unsmoothed audio-rate mode needs a separate coefficient-update and aliasing budget.

During a musical retune with a strike contact active, retain its compression state continuously and advance it from relative port velocity. Do not reconstruct a new compression by subtracting reinterpreted displacements after changing `omegaHat` or mass. This preserves the chosen normalized contact state; it is another reason not to describe live retuning as a literal geometric resizing operation.

---

## 7. Numerical modal core: passive, collocated, exact free poles

### 7.1 Why not an unspecified resonant biquad

A resonator used only for audio output can hide its normalization. A resonator inside a friction loop cannot: force-to-velocity scaling, contact admittance, and energy matter. Implement the explicit state update below rather than choosing a resonator by ear and guessing its feedback gain.

### 7.2 Digital pole placement

For internal time step `h`, desired damped modal frequency `f`, and `T60`:

\[
r=e^{-\ln(1000)h/T60},\quad \vartheta=2\pi fh,
\quad D_p=1+2r\cos\vartheta+r^2,
\]
\[
\widehat\sigma=\frac{2}{h}\frac{1-r^2}{D_p},
\qquad
\nu=\frac{4}{h}\frac{r\sin\vartheta}{D_p},
\qquad
\widehat\omega=\sqrt{\widehat\sigma^2+\nu^2}.
\]

These are an inverse-bilinear mapping of the desired digital poles. Evaluate `1-r²` with `expm1` or an equivalently stable expression for long decay times.

Set:

\[
a_j=h\widehat\omega_j/2,\quad
D_j=1+h\widehat\sigma_j+a_j^2,\quad d_j=h/(2D_j).
\]

Freeze coefficients and contact shapes at the internal step's midpoint. Then:

\[
\bar y_j=\frac{y_j-a_jx_j+(h/2)\sum_kb_{jk}F_k}{D_j},
\]
\[
x_j'=x_j+h\widehat\omega_j\bar y_j,
\qquad y_j'=2\bar y_j-y_j.
\]

In the unforced, fixed-parameter case, the discrete poles are exactly `r exp(±iϑ)`, to floating-point accuracy. The forced response remains a discretized physical approximation; correct free poles do not make all nonlinear behavior sample-rate invariant.

### 7.3 Collapse the contact solve to small ports

Before solving forces, compute:

\[
\bar y_j^0=(y_j-a_jx_j)/D_j,
\quad v_k^0=\sum_jb_{jk}\bar y_j^0,
\]
\[
Y_{kl}=\sum_jb_{jk}b_{jl}d_j.
\]

Then all midpoint contact velocities are:

\[
\bar v_k=v_k^0+\sum_lY_{kl}F_l.
\]

`Y` is symmetric positive semidefinite by construction. With one rubbing contact, the nonlinear solve is scalar. With a simultaneous striker, it is only a two-port problem. Modal work remains O(number of coordinates).

### 7.4 Energy identity

The modal step satisfies:

\[
E_{bowl}'-E_{bowl}
=h\sum_kF_k\bar v_k
 -2h\sum_j\widehat\sigma_j\bar y_j^2.
\]

This is a design invariant. It provides a direct test for wrong signs, inconsistent coupling, integration bugs, and spurious energy.

Use double precision for the reference implementation, contact solver, and energy accounting. Optimize storage or SIMD only after the reference tests pass. Never rely on output clipping to make an unstable contact loop appear stable.

### 7.5 Exact free poles do not fix mechanical impedance

The inverse-bilinear construction changes effective stiffness and damping when masses are held fixed. In the weak-loss limit,

\[
\widehat\omega\simeq\frac{2}{h}\tan(\pi f h),\qquad
\frac{C_{static,digital}}{C_{static,target}}
\simeq\left[\frac{\pi f h}{\tan(\pi f h)}\right]^2.
\]

Here `Cstatic = 1/(m omega²)` is modal static compliance under a constant generalized force; the continuous target uses `omega0 = sqrt((2πf)² + sigma_target²)`. At `f/Fint = 0.10, 0.20, 0.40`, the compliance ratios are approximately `0.935, 0.748, 0.167`. Thus §16's 0.40 ceiling is a numerical guard, **not a fidelity guarantee**. Prewarping also alters modal forced-response residues and the response seen by the nonlinear contact. Changing output EQ cannot repair these effects.

Before freezing quality settings, compare the full collocated force-to-velocity response, static compliance, impact duration/rebound, and rubbed onset against the continuous model at a successively finer internal step. Use the same physical masses, contact parameters, and modes in the comparison. A provisional mechanical-band target is less than 2% per-mode static-compliance error, corresponding roughly to `f/Fint < 0.055` in the weak-loss limit, unless a documented contact-convergence study justifies a wider band. This is a design tolerance, not a measured perceptual threshold. Frequencies outside it are allowed only with an explicit accuracy qualification or higher-rate evidence; do not silently change masses to repair one response metric.

### 7.6 Resolve adhesion as well as proving passivity

For the regularized zero-slip branch, define `K_v = Φ'(0) = N μs/ε` and `χ = Ytt K_v`. This positive slope does not threaten root uniqueness, but can make contact dynamics stiff. In a local damping-only limit, the midpoint multiplier is `(1−χ)/(1+χ)`: for large `χ` it tends toward −1, not zero. Therefore a passive step may retain alternating numerical motion instead of accurately resolving rapid adhesion relaxation. The negative-slope certificate `Ytt L` says nothing about this effect.

Track `χ`, compare `ε/2`, `ε`, and `2ε` at finer internal rates, and inspect slip/force histories as well as audio. Do not call epsilon a harmless numerical constant or shrink it without a resolution study. An eventual exact static-friction or bristle model requires its own state, work law, and solver; it cannot be obtained simply by taking epsilon toward zero at a fixed rate.

---

## 8. Strike: finite moving mass and nonlinear contact

### 8.1 Primary model

Use a striker with effective mass `m_s`, signed inward velocity `v_s`, and compression `δ`. The bowl receives inward contact force of magnitude `F_s` through `b_js = −P_n φr_j/sqrt(m_j)`; the striker receives `−F_s` in its inward coordinate. The port velocity `v_s^0` below is the free **bowl** inward velocity, not the striker velocity; use distinct variable names such as `strikerVelocity` and `bowlStrikePortFreeVelocity` in code.

At a nonzero strike event, sample velocity CV and strike location once. If no striker is active, place it at first contact (`δ=0`) and set its inward velocity to the current inward bowl port velocity plus the requested launch velocity. This specifies **relative approach speed**, not fixed laboratory-frame mallet speed; it intentionally gives consistent attack opportunity on a ringing bowl. Record the actual incoming kinetic energy. Do not clear bowl state. A zero-velocity event creates no contact and makes no change to an existing one.

An initial mapping is:

\[
u=\operatorname{clamp}(V_{velocity}/10,0,1),
\qquad v_{launch}=1.0\,u\ \text{m/s}.
\]

When velocity CV is unpatched, use the VELOCITY knob instead. Zero velocity produces no impact. Gate height above threshold does not affect strength. Launch velocity is linear in `u`; the associated striker kinetic energy is consequently quadratic before interaction with the moving bowl.

Only one striker contact is active initially. A retrigger during contact adds launch velocity to the active striker; it does not reset it. Bound absolute striker speed to 4 m/s as an explicit overload policy and increment a diagnostic when that bound is used. New strike-location settings take effect when a new contact is created, not halfway through an existing collision.

### 8.2 Contact potential

Use:

\[
U_s(\delta)=\frac{k_s}{p+1}\max(\delta,0)^{p+1},\qquad p=1.5\ \text{initially}.
\]

Here `k_s` has units N/m^p. The exponent and stiffness are mallet-model choices, not universal Hertz parameters for every bowl/mallet geometry.

For the finite step, use the discrete-gradient elastic force:

\[
F_e=\begin{cases}
\dfrac{U_s(\delta')-U_s(\delta)}{\delta'-\delta}, & \delta'\ne\delta,\\
k_s\max(\delta,0)^p, & \delta'=\delta.
\end{cases}
\]

Use a stable near-equality evaluation. Add loading-only damping:

\[
F_d=c_s\max((\delta'-\delta)/h,0)
\]

when either endpoint is in compression; otherwise use zero damping. Then `F_s = F_e + F_d ≥ 0`. This damper dissipates energy during compression and does not produce an attractive force on separation. It is an explicit approximation to mallet losses.

### 8.3 Compression equation

Let `s` identify the strike port and `t` the tangential rubbing port in §7:

\[
\delta'=\delta+h\left[v_s-v_s^0-
\left(Y_{ss}+\frac{h}{2m_s}\right)F_s-Y_{st}F_t\right].
\]

The striker velocity update is:

\[
v_s'=v_s-hF_s/m_s.
\]

After separation (`δ'≤0` and relative separation velocity outward), retire the striker. Do not clamp away its contact potential before the discrete-gradient step completes. Retiring the detached striker removes its remaining kinetic energy from the simulated bowl system; it does not inject that energy into the bowl.

Specify the retirement test using the **end-of-step** relative velocity, evaluated with the fixed strike port. Audit boundary-crossing steps separately: the discrete-gradient force is an interval force and can be nonzero over a step whose final endpoint has separated. This is not permission to keep applying force to a detached striker on subsequent steps. Compare contact duration, impulse, and restitution at finer steps; energy conservation alone does not exclude early numerical reflection during an underresolved hard impact.

The striker may exchange energy with an already-moving bowl. Do not demand monotonic output peak versus strike velocity on an arbitrary ringing state. Characterize the velocity response from rest using energy or integrated loudness; strict monotonic transfer is not an invariant of arbitrary nonlinear contact/resonance combinations.

### 8.4 Debugging fallback, not the reference mallet

A normalized, finite raised-cosine force pulse may be retained as a diagnostic exciter:

\[
F(t)=\frac{J}{T_c}\left[1-\cos(2\pi t/T_c)\right],\quad0\le t\le T_c.
\]

Its integral is impulse `J`. Numerically normalize its sampled area. It is useful for isolated modal tests, but it has no mallet rebound or loading feedback and must not silently replace the primary striker in a completed reference engine.

---

## 9. Rotation: moving contact and self-excited friction

### 9.1 Kinematics

Represent rotation speed as signed revolutions per second `rps`:

\[
\dot\theta=2\pi\,rps,\qquad U=2\pi R_{eff}\,rps.
\]

`U` is imposed mallet tangential speed in m/s. It is not a frequency driving a sine oscillator. Evaluate contact shapes at midpoint angle. Preserve rotation angle across gate transitions; gate-on does not restart its phase. Use the **same smoothed speed** for `U` and angle advance. Continue that advance while the release envelope is nonzero; freeze the orbit only after contact is fully lifted, unless a future explicit free-spin option is enabled. Freezing angle immediately on gate-off while retaining nonzero `U` during release would specify incompatible contact trajectories.

This assumes sliding translation along the rim with no separate axial mallet spin or rolling. If mallet spin is added, orbital speed and surface slip must be separate variables. For fixed model parameters the material surface velocity is `Σ φt_j(θ) v_j`, not the total derivative of a moving displacement sample. Do not insert `θdot Σ (∂φt_j/∂θ) q_j` into the friction feedback. The orbit changes which material point is contacted; it is not itself extra material vibration velocity.

Default signed speed range is −2 to +2 rev/s, default +0.4. With SPEED CV patched, multiply the knob value by `clamp(V/5,−1,1)`. Thus 0 V stops, +5 V requests the knob speed, and −5 V reverses it. Unpatched multiplier is 1.

### 9.2 Normal-load approximation in the first reference engine

For the initial engine, normal load `N` is prescribed by the pressure control and a contact-engagement envelope:

\[
N=e_{contact}\,P,\quad0\le P\le15\ \text{N}.
\]

This is a **tangential-only contact proxy with a prescribed load parameter**, not a literal ideal normal follower. Only tangential friction acts dynamically on the bowl while rotating. A real follower also applies radial force, even when contact never opens. The prototype omits its static deformation, moving radial forcing, radial impedance, and normal-to-tangential load feedback, as well as bounce and rattling. It is a controlled first experiment, not a complete mechanical rubbing model.

This approximation is deliberate and must remain named in the implementation. Full normal compliance is a separate refinement in §12. Do not generate random clicks and label them modeled contact loss.

Before describing Vessel as a physically validated bowl, compare this proxy with a model that includes radial normal work. A useful intermediate experiment adds the known prescribed inward load through `b_jn = −P_n φr_j/sqrt(m_j)` before solving the tangential/strike ports. Its work `h N vbar_n` must appear in the energy ledger; prescribed pressure can excite the bowl when engaged or moved. The normal force is known in that experiment, so it need not add another nonlinear unknown. This experiment is not included in the current two-port oracle and does not replace the compliant follower/contact-loss model.

Use approximately 10 ms engagement and 5 ms release. Keep target speed and contact engagement independent. At zero speed with nonzero `N`, friction can remove energy from the moving bowl.

### 9.3 Friction law

Define midpoint relative slip:

\[
s=U-\bar v_t.
\]

The proposed regularized, velocity-weakening curve is:

\[
\mu(s)=\mu_k+(\mu_s-\mu_k)\exp[-(s/v_c)^2],
\]
\[
F_t=\Phi(s)=N\mu(s)\tanh(s/\epsilon),
\]

with `μs ≥ μk ≥ 0`, `vc > 0`, and `ε > 0`.

The Gaussian weakening curve is a Vessel design choice. `μs` names the low-speed coefficient, but this smooth formulation does **not** implement an exact static-friction constraint: at exactly zero slip its force is zero. Describe the low-slip region as regularized adhesion or pseudo-sticking, not mathematically exact sticking.

The model can transfer energy from imposed hand motion into resonances while dissipating energy at the sliding interface. Its power identity is:

\[
F_t\bar v_t=F_tU-F_ts,
\qquad F_ts\ge0.
\]

With `U=0`, friction cannot add mechanical energy when evaluated consistently. With `N=0`, force is exactly zero.

### 9.4 Implicit solve

For no active strike:

\[
F_t=\Phi(U-v_t^0-Y_{tt}F_t).
\]

Equivalently, solve:

\[
R(F_t)=F_t-\Phi(\Delta-Y_{tt}F_t)=0,
\quad\Delta=U-v_t^0.
\]

Bracket the force in `[-N μs, +N μs]`. Use previous force as an initial guess, clamped to the bracket; apply safeguarded Newton with a bisection fallback.

For Newton, use the analytic derivative rather than a noisy finite difference:

\[
\mu'(s)=-\frac{2s}{v_c^2}(\mu_s-\mu_k)e^{-(s/v_c)^2},
\]
\[
\Phi'(s)=N\left[\mu'(s)\tanh(s/\epsilon)+
\frac{\mu(s)}{\epsilon}(1-\tanh^2(s/\epsilon))\right],
\quad R'(F_t)=1+Y_{tt}\Phi'(s).
\]

For the chosen curve, a sufficient bound on the magnitude of the negative force slope is:

\[
L=N\sqrt{2/e}\,\frac{\mu_s-\mu_k}{v_c}.
\]

Therefore `Ytt * L < 1` is a sufficient uniqueness condition for the scalar root. Certify **`Ytt * L ≤ 0.9`** over supported profiles, pressure, material interpolation, and sample rates. The steep positive slope near zero slip does not invalidate this bound.

Do not change friction coefficients automatically to hide a failed certificate. Reject an invalid descriptor in development, increase the declared internal rate, or explicitly restrict the supported parameter range.

### 9.5 Startup and saturation

Gate-on produces a finite contact transition. From a resting bowl, nonzero relative speed already yields a nonzero contact force, so startup need not depend on a hidden sustaining oscillator. A seeded microscopic perturbation may be used only for a diagnosed numerical symmetry problem and must be documented and disableable.

Do not multiply sustained audio by an arbitrary speed envelope to simulate rubbing. The usable speed/pressure region must be established by parameter sweeps. Some combinations may fail to sing, suppress the fundamental, or select another mode. A first preset must nevertheless offer a broad, easy-to-find usable region.

No perpetual additive noise is required in the reference engine. A later roughness model must enter as a bounded contact perturbation with documented energy input, not as stereo hiss added after synthesis.

### 9.6 Singing onset is a separate physical test

Neither `F_t s ≥ 0` nor `Ytt L < 1` proves self-excitation. The first establishes interface dissipation; the second establishes uniqueness of a numerical root. For a frozen contact and a steady-sliding equilibrium, linearize at slip `s0` and let `a_f = Φ'(s0)`. With normalized tangential coupling vector `b_t`, the perturbation equations are

\[
\delta\dot x=\widehat\Omega\delta y,\qquad
\delta\dot y=-\widehat\Omega\delta x
 -\left(2\widehat\Sigma+a_f b_t b_t^T\right)\delta y.
\]

A negative friction slope can overcome modal losses. For an isolated scalar mode this requires `−a_f b_jt² > 2 sigmaHat_j`; the multimode test uses eigenvalues of the complete linearized matrix. At rest under sliding, `s0 = U` after static deflection settles. At very high speed the Gaussian weakening slope tends to zero, so stronger motion need not make a stronger sustained tone.

The review diagnostics find positive frozen-angle growth for the default Metal/Suede conditions, approximately 2.04–4.37 per second over 32 angles at 192 kHz. This is encouraging, **not a prediction of actual rotating onset time**: the moving contact is a time-periodic system, with modal orientation and splitting competing with rotation. Use a time-domain perturbation-growth test or Floquet analysis around its low-amplitude periodic response, then full nonlinear runs to establish saturation and mode selection.

Acceptance must separate (1) a gate-on transient, (2) slow forced response to travelling load, and (3) a sustained friction-driven oscillation. Measure cycle-averaged input work, dissipation, and energy after startup, and after small perturbations of the established tone. A bounded tone must balance average drive and loss without relying on protection. Record modal displacement relative to rim radius and contact indentation; states outside the small-deformation regime invalidate the linear-shell interpretation even if the solver is stable.

---

## 10. Simultaneous strike and rotation solver

Both contacts act on the same midpoint modal state. Do not calculate their forces from two separately advanced versions of the bowl.

With an active striker, solve:

\[
F_t=\Phi(U-v_t^0-Y_{ts}F_s-Y_{tt}F_t),
\]
\[
F_s=F_e(\delta,\delta'(F_s,F_t))+F_d(\delta,\delta').
\]

### Reference algorithm

Use a bracketed **outer scalar strike-force solve**. For each trial `F_s ≥ 0`, solve the inner friction equation from §9 with:

\[
\Delta=U-v_t^0-Y_{ts}F_s.
\]

Then compute `δ'` and the strike-force residual. Under the friction uniqueness condition and the positive-semidefinite port admittance, increasing trial strike force decreases the compression response. The convex contact potential and loading-only damper make this a suitable monotone outer solve.

More explicitly, with `a_f = Φ'(s)` and `A_s = Yss + h/(2 m_s)`,

\[
\frac{dF_t}{dF_s}=\frac{-a_f Y_{ts}}{1+a_fY_{tt}},\qquad
-\frac1h\frac{d\delta'}{dF_s}
=A_s-\frac{a_fY_{st}^2}{1+a_fY_{tt}}>0.
\]

For negative `a_f`, the correction increases effective midpoint admittance; for nonnegative `a_f`, positive semidefiniteness bounds the subtraction by `Yss`, leaving the positive striker term. This proves the outer compression monotonicity on the certified friction branch. Handle the contact/damper branch boundaries without taking an unguarded Newton step across them.

Start the lower bracket at zero. Build a finite upper bracket by bounded expansion, starting from a physically scaled estimate or the last contact force. In a debug/reference build, at most 24 doublings are allowed; failure is a solver fault, not permission for an unbounded loop. Establish tighter energy-based or profile-specific bounds before optimization.

Use force tolerance `1e-8 + 1e-7 * max(1, relevantForceScale)` N as an initial target. Also check compression residual and the per-step energy residual; force convergence alone is not sufficient near separation. All iteration limits are fixed. Log iteration histograms in the offline harness.

A practical initial production budget is up to 12 safeguarded iterations for each scalar solve, with a bounded high-accuracy fallback path. **This budget is a starting point, not a proven worst case.** An optimized two-variable Newton solver may replace nesting after matching the reference solutions and maintaining the same failure behavior.

### Failure policy

For an isolated convergence failure, do not apply the last unconverged force at full strength. Enter a bounded dissipative recovery: remove active contact forces, retire or release the problematic contact, retain finite bowl state, and expose a diagnostic. A nonfinite state resets the affected voice and silences its output immediately. These are emergency policies, not acceptable normal timbre-shaping mechanisms.

Default presets must produce zero solver faults in the acceptance sweep. A fallback that routinely changes the sound is a failed implementation.

### Whole-system energy audit

For fixed coefficients and no new strike event:

\[
\Delta\left(E_{bowl}+\tfrac12m_sv_s^2+U_s(\delta)\right)
=hF_tU-hF_ts
-2h\sum_j\widehat\sigma_j\bar y_j^2
-hF_d\frac{\delta'-\delta}{h}.
\]

The last term is nonpositive because the loading damper only acts for positive compression velocity. Account separately for energy introduced by launch/retrigger events and removed by retiring a detached striker.

For a newly created striker, add its actual `m_s v_s²/2` as external launch energy. For an active-striker retrigger, record `m_s (v_after²−v_before²)/2` using velocities **after the overload policy**; it can be signed when the striker is rebounding. A speed cap or emergency contact retirement removes or changes modeled energy and must have its own ledger entry. Emergency removal of a compressed contact discards its remaining potential; do not report that as frictional loss. Parameter work, mode retirement, and state resets likewise require separate entries. The identity above covers fixed contact parameters, not unreported coefficient mutation.

The included numerical checks verify the modal identity, friction root/passivity, isolated striker energy balance, and 200 randomized **one-step simultaneous-contact** energy balances using a slow nested reference solver. They do not test sustained trajectories, production iteration budgets, or the native Rack implementation.

---

## 11. Bowl and mallet materials

### 11.1 Material is a descriptor, not a single brightness number

Bowl descriptors own modal ratios, splitting, damping, effective masses, spatial orientations, effective radius, and radiation weights. Mallet descriptors own effective moving mass, impact stiffness and loss, contact-patch width, and friction parameters.

For later calibrated versions, contact coefficients should be keyed by **bowl/mallet pair**. The prototype may share a mallet's friction curve across bowls, but that limitation must be recorded. Hardness does not automatically determine friction.

### 11.2 Uncalibrated mallet seeds

Every value in this table is U. `k` uses `p=1.5`; `w` is full angular patch width; `ε=1e−4 m/s` initially for all four profiles.

| Mallet | Mass kg | k, N/m^1.5 | c, N·s/m | w, rad | μs | μk | vc, m/s |
|---|---:|---:|---:|---:|---:|---:|---:|
| Wood | 0.035 | 2.0e8 | 0.8 | 0.025 | 0.40 | 0.20 | 0.12 |
| Suede-wrapped | 0.045 | 1.0e7 | 1.5 | 0.070 | 0.65 | 0.30 | 0.20 |
| Silicone | 0.050 | 3.0e6 | 2.0 | 0.100 | 0.90 | 0.40 | 0.18 |
| Felt | 0.035 | 3.0e5 | 2.0 | 0.160 | 0.35 | 0.25 | 0.20 |

Default mallet is Suede-wrapped. These are intended to create distinguishable initial experiments, not an empirical ranking of real mallet products.

A material selector should not promise that every combination sings equally readily. The released factory combinations must be tuned for usability, but weak friction differences must not be compensated with a hidden tonal oscillator.

### 11.3 Calibration sequence

Fit modal frequencies and doublet splitting from freely decaying recordings first. Fit amplitude decay independently for each mode, avoiding early impact and late noise floor. Do not fit beating as a rapidly fluctuating damping coefficient.

Fit overlapping A/B decays jointly when a bandpass cannot resolve the splitting; a single filtered envelope may contain beating rather than one exponential. Use multiple strike and observation azimuths to avoid missing a mode at a node. Record bowl dimensions, support/hand contact, and recording positions. Frequencies/T60 from audio do not identify absolute mobility. Where possible, use calibrated force and displacement/velocity (or accelerance) measurements at known locations to identify modal residues and masses under the normalization in §4.2. Include residual flexibility/inertance from omitted modes in the fit when the measured frequency range requires it.

Fit impact spectra across several velocities and mallet materials next. If force measurements are unavailable, modal mass, contact stiffness, and radiation gain may be underdetermined; store that uncertainty instead of calling each fitted number physically unique.

Finally fit sustained-rub behavior over speed/pressure trajectories: onset time, steady amplitude, which mode dominates, spectral evolution, and response to stopping or lifting the mallet. Output EQ alone is insufficient to calibrate the feedback mechanics.

Identify a crystal specimen by geometry and actual material, rather than treating the retail word “crystal” as a constitutive model. A synthetic ring spectrum plus longer decay is a useful prototype voice, but cannot establish quartz/fused-silica identity. Fit both material profiles against dry strikes **and** rub trajectories before promoting them from prototype status; do not postpone every physical calibration task until after the module interface is complete.

Record which parts of any reference sound are bowl, mallet, room, microphone, processing, and performance. Obtain permission for any recordings or datasets distributed with the plugin.

---

## 12. High-fidelity refinement: normal compliance and contact loss

The prescribed-load model cannot reproduce radial mallet bounce during rotation. Add that deliberately after the simpler model is stable, rather than baking unvalidated three-way coupling into the first milestone.

The extension adds a radial follower mass `m_r`, a hand position or applied normal-force controller, and a unilateral normal-contact potential. Its radial force must act back on the bowl through `φr`; the same solved normal force then bounds tangential friction. When the gap opens, normal force and friction both become zero.

A continuous-time starting structure is:

\[
m_r\ddot z=P_{hand}-F_n,
\quad F_n\ge0,
\quad F_t=F_n\mu(s)\tanh(s/\epsilon).
\]

The normal gap is derived from mallet position and the bowl surface at the moving contact location. A hand/handle compliance and damping model is needed so a force-driven mallet does not accelerate away indefinitely while detached. Define its potential and external work, not just a spring constant in code.

**Important geometry distinction:** local material surface velocity is the modal velocity projected at the current contact point. The time derivative of displacement sampled at a moving angle also contains `∂q/∂θ * θdot`. These are not interchangeable. Gap evolution, surface slope, and work done by the travelling normal constraint must be treated consistently before claiming passivity for this extension.

For a fixed-coefficient inward port `c_j(θ) = −P_n φr_j(θ)`, an explicit reduced-geometry example is `w_n = Σ c_j q_j`, `δ_n = z−w_n`, with `z` positive inward. Then

\[
\dot\delta_n=\dot z-\sum_j c_j\dot q_j
 -\dot\theta\sum_j(\partial_\theta c_j)q_j.
\]

Besides the hand term `P_hand zdot`, the combined follower/bowl/contact energy balance contains trajectory work `−F_n θdot Σ (∂θ c_j) q_j`. It must be supplied/absorbed by the prescribed orbit actuator, or recovered through a consistent geometric tangential reaction in a more complete contact model. A flat-rim friction work term alone does not account for it. Use a discrete chain rule for both modal displacement and moving contact geometry; this continuous expression does not prescribe an already-validated discrete solver.

Implement it as an explicit new contact-model version with a small coupled normal/tangential solve. Extend the energy ledger to include follower kinetic energy, handle/contact potentials, and hand/trajectory work. Do not assume the one-dimensional uniqueness certificate in §9 automatically proves the full model stable.

Acceptance requires repeatable contact opening/reclosing, no attractive normal force, zero friction when detached, bounded normal-force transients, and parameter/sample-rate convergence. Until those tests pass, label this mode experimental. Rattling is not a release requirement for the first playable Vessel, but this is the principal next step for physical fidelity.

---

## 13. Stereo observation and optional binaural feature

### 13.1 Native stereo: one bowl, two observation positions

Initially approximate acoustic output using weighted radial velocities at two fixed rim angles:

\[
a_L=\sum_n G_n\left[
\phi^r_{n,A}(\theta_L)\frac{y_{n,A}}{\sqrt{m_{n,A}}}
+\phi^r_{n,B}(\theta_L)\frac{y_{n,B}}{\sqrt{m_{n,B}}}\right],
\]

and similarly for R. Distinct A/B radiation gains may be used when calibrated.

This is a spatially meaningful **rim-velocity observer**, not a complete far-field radiation solution or an HRTF. Output weights can later fit microphone responses without changing the mechanics.

Do not multiply the observation shapes by the mallet patch factor. Radiation is independent of the excitation footprint. A calibrated acoustic observer may need frequency-dependent magnitude/phase and elevation dependence; two local rim velocities are virtual pickups, not two physical microphone pressures. Radiation loss is already part of measured total damping and must not be added a second time when fitting output filters. Keep observation fixed in bowl coordinates so rotating vibration patterns can produce natural amplitude variation, including in the zero-splitting limit; verify that behavior from the solved states rather than adding an LFO.

Set observer center to 20° and maximum separation initially to 30°. WIDTH moves symmetrically between coincident and separated observers. Keep the default strike angle at 22.5° relative to the nominal bowl orientation so it does not intentionally excite only one member of the lowest pair.

For a single bowl, WIDTH=0 makes both observer readouts identical. While both bowls are active, output subtraction nulls only when the mechanical states are also identical. Once zero separation has folded to one bowl, WIDTH=0 again nulls both outputs regardless of the discarded bowl history. WIDTH changes only observation coefficients; it does not mix, detune or resynchronize the two bowls.

When only L is connected, provide a documented L/MONO result. Initial policy: `(L+R)/2` after the selected stereo renderer. Preserve the normal stereo signals when both are connected. Cancellation in a spatial or binaural sum is possible and is not “fixed” by energy normalization. Meter physical energy independently.

### 13.2 Binaural tuning: two independent physical bowls

The user-selected reference architecture supersedes the earlier analytic frequency-shifter proposal. BINAURAL is a **0–33 Hz difference between the lowest modal-pair centers**, default zero. For center frequency `f_c` and requested difference `Δ`, configure two complete independent bowl/contact simulations at `f_L = f_c − Δ/2` and `f_R = f_c + Δ/2`. Both receive the same strike events, velocity, rotation, pressure, specimen, mallet, decay and imperfection controls. Each interaction responds to its own bowl; there is no shared prescribed force or pitch/frequency-shifted readout.

Each bowl retains independent normalized modal state, striker compression/velocity, friction/contact state, energy ledger and causal output-filter history. Equal tuning from identical rest states and identical inputs gives identical mechanics. At settled zero difference, the optimized path crossfades the right output over 50 ms to the surviving lower bowl’s right pickup, then stops the second simulation. Both states run during the fade; the second independent history is discarded after it finishes. On a later nonzero request, copy the surviving bowl’s complete mechanical/contact and stereo filter history, retune the two states independently, and fade the right output back over 50 ms. Interrupted fades reverse continuously without replacing a still-active state. Pass `false` to `DualBowlAdapter` to retain the always-dual scalar reference. The left output takes the left observer of the lower bowl; the right takes the right observer of the upper bowl. WIDTH continues to set observer placement independently of tuning.

Keep both pair centers in the validated 20–2000 Hz engine range by constraining the effective center to `[20 + Δ/2, 2000 − Δ/2]`. This preserves the requested separation at tuning boundaries. The Rack display reports the two actual pair centers. Separation changes use a 10 ms one-pole smoother at the existing approximately 1 kHz coefficient boundary; PITCH keeps its 1.5 ms log smoother.

This control specifies structural tuning, not a guaranteed exact difference between dominant sustained spectral peaks: pair imperfection, nonlinear contact response and which split mode dominates can change the measured audible difference. Shared settings do not imply equal instantaneous energy. At full dual mix, display the arithmetic mean of the two physical bowl energies using the existing reference scale. During transitions, use `E_left + 0.5 m (E_right − E_left)`, with `m` the dual-output fade fraction; at single mode this becomes the surviving bowl energy. Separate effective channel energy telemetry remains available for diagnostics. LEVEL and WIDTH remain independent of the mechanics.

`DualBowlAdapter` retains a scalar reference built from two complete causal adapters, including transactional pair configuration, and now defaults to the zero-separation single-path optimization. Measure cost and response before SIMD/shared-coefficient optimization; retain independent nonlinear solver state. True HRTF/room rendering remains separate future work. This feature makes no health or entrainment claim.

---

## 14. Rack controls, inputs, and outputs

The following interface is a starting contract. Physical panel width and artwork are intentionally not frozen.

| Control/input | Initial behavior |
|---|---|
| PITCH + FINE | Lowest pair-center tuning; default C4; fine ±100 cents. |
| V/OCT | Added to coarse pitch, 1 V/octave; finite/clamped total frequency. |
| STRIKE button + STRIKE gate | Rising-edge events; manual and cable events in the same sample coalesce to one launch. |
| VELOCITY knob/input | Knob default 0.5; patched CV replaces knob, 0–10 V → 0–1; sampled only at strike. |
| ROTATE latch + ROTATE gate | Logical OR; input is a sustained gate, not a trigger-to-toggle. Latch default off. |
| SPEED knob/input | Signed knob −2…+2 rev/s, default +0.4; patched ±5 V multiplies knob by −1…+1. |
| PRESSURE knob/input | Knob 0–15 N, default 2.5; patched 0–10 V multiplies pressure by 0–1. |
| BOWL | Metal / Crystal prototype; stable descriptor IDs and versions. |
| MALLET | Wood / Suede-wrapped / Silicone / Felt. |
| DECAY | 0.25…4 multiplier, logarithmic; default 1. |
| IMPERFECTION | Pair splitting multiplier 0…2; default 1. |
| WIDTH | Native stereo observer separation; default 0.7. |
| LEVEL | Output gain only; no effect on contact or energy. |
| BINAURAL ΔHz | Two physical bowl pair centers separated by 0–33 Hz, default 0; symmetric around effective PITCH. |
| L/MONO, R | Lower-bowl left observer and upper-bowl right observer; no automatic polyphony. The reference preserves these signals regardless of cable state. |
| ENERGY display | Physical bowl-state energy, not output RMS. |

Context-menu settings may include strike angle, observer azimuth, quality and optional damping. Keep material selection on the panel, not only in a menu.

Use Schmitt behavior with approximately 0.1 V low / 1 V high thresholds for gates (V2). A held strike gate causes one event. Rotation remains active while its gate is high. Negative gate voltages are low; nonfinite values are sanitized.

At module initialization, a high external strike gate is treated as one initial rising edge; this policy must be tested and documented. Manual button state is not serialized as a held strike. The ROTATE latch may be serialized, so a loaded patch can resume rubbing from a resting mechanical state.

Declare parameter/input/output IDs once and preserve them. Additional controls append IDs. Do not reshuffle enums when changing the panel. Mechanical state, unless a future explicitly versioned state-resume feature is implemented, starts at rest on patch load; settings and specimen identity persist.

---

## 15. Energy bar, visualization, and output protection

### 15.1 Energy

Publish:

\[
E_{bowl}=\tfrac12\sum_j(x_j^2+y_j^2),
\qquad
E_{dB}=10\log_{10}\left(\frac{E_{bowl}+E_{floor}}{E_{ref}}\right).
\]

At full dual mix, apply this equation to `E_display = (E_left + E_right)/2`; each energy is computed from its own normalized modal state. During the single/dual fade, use the weighting in §13.2; in single mode report the surviving bowl energy. This preserves the single-bowl scale at unison. The physical pair total while both exist is the sum, not the mean.

Map an initial −60…0 dB range to 0…1, with modest display smoothing, approximately 5 ms attack and 150 ms release. `Eref` is a fixed calibration value per descriptor version, not an auto-normalizer that chases recent maxima. Determine it from reference test renders; no acoustic-loudness equivalence is implied.

The minimum UI is the requested Doorstop-like energy bar. It must remain visible through output cancellation, width changes, or LEVEL=0 because the bowl may still contain energy. During an unforced passive tail, raw bowl energy cannot grow; the displayed bar must not “recharge” just because an audio beat reaches a peak.

The striker's incoming kinetic energy is not part of the normal BOWL ENERGY bar. A separate debug display may show total bowl + striker + contact energy. Do not confuse those metrics.

### 15.2 Optional bowl animation

Show mallet angle, engagement, strike activity, and a strongly magnified projection of the lowest modal pair. High-frequency motion cannot be meaningfully sampled directly at UI frame rate; use an envelope or stable phase-aware visualization rather than drawing aliased raw oscillation.

The UI observes DSP state only. It never integrates a second mechanical simulation. Closing the module UI must not change audio, force, random state, or timing.

### 15.3 Thread boundary

Publish finite, bounded telemetry at a modest rate, initially 100–200 Hz, plus immediate strike/sleep changes. Prefer atomic scalar fields or a properly synchronized bounded snapshot mechanism. Do not use a plain non-atomic struct seqlock that permits concurrent C++ reads/writes of the same storage.

The audio path must not allocate, lock a UI mutex, log to disk, or access NanoVG. A cached static panel and inexpensive bar/mallet overlay are sufficient. Visual design can follow existing Leviathan conventions without importing Doorstop's particular physical equations.

### 15.4 Gain and safety

Calibrate default output near the usual Rack audio range of ±5 V for typical playing (V2), while preserving headroom for harder strikes. Do not place a continuously working tanh saturator inside the friction loop or use it to conceal excessive modal energy.

A post-render DC blocker and transparent, optional emergency protection may be used. Keep raw observer, post-resampler, and final-output debug taps. Nonfinite detection operates on mechanical state as well as audio. Default reference renders should not spend sustained time in protection.

---

## 16. Sample rate, oversampling, control timing, and sleep

### 16.1 Internal rate

Run the **entire nonlinear interaction loop**, not only its final output, at the internal rate. Initial default is the smallest factor in `{1,2,4,8}` giving at least 176.4 kHz for host rates of 32 kHz and above. This gives 4× at 44.1/48 kHz and 2× at 88.2/96 kHz.

An economy setting may later use a lower declared rate after comparison. A high-reference offline setting should use still finer steps for convergence testing. These are design budgets, not claims that 4× is automatically alias-free.

Keep modal frequencies below 0.40 of internal sample rate. When an extended descriptor exceeds the supported range, smoothly taper that mode's contact coupling and output observation between 0.35 and 0.40 of internal rate, then retire it. Removed state energy is explicit model-truncation loss. Do not wrap a pole, fold it into the audible band, or hard-clamp several modes to the same frequency.

Apply §7.5's mechanical-response check separately from this ceiling. The seeded crystal's top pair at a 2 kHz fundamental is approximately 46.6 kHz, already around 0.264 of 176.4 kHz, well inside the guard but outside the proposed accurate-compliance band. Do not advertise the entire pitch range as equally faithful on the basis of free-pole tests. Mode retirement is reversible only by creating a zero-energy state on re-entry (with smooth coupling); never resurrect discarded state. Any alternative dormant-state policy must be specified and tested explicitly.

Modes above host Nyquist can still participate internally when supported; the output decimator removes their audible aliases. Do not delete them merely because the host output cannot represent them: contact admittance and strike response can depend on them.

Converge **modal bandwidth and time step independently**. More oversampling does not recover omitted structural modes. Compare successively enriched modal bases with the same contact law, including contact impulse, peak force, duration, rebound, and rubbing onset. Hard-strike seeds and low fundamental tuning especially require this check because the retained modal bandwidth shrinks with pitch. If convergence fails with available measured modes, use a documented passive residual model, additional supported modes, or restrict/soften that interaction explicitly. Never invent high modes and label them measured. The fixed capacity of 12 pairs is an engineering budget subject to this test.

Use a measured anti-alias decimator with declared passband, stopband, and latency. Compare complete nonlinear renders against a higher-rate reference. Stable time integration alone does not guarantee low aliasing.

**Prototype progress (2026-09-30):** `HostRateAdapter` implements the stated factor policy for 32–192 kHz host rates and causal stereo decimation. Its thirteen mechanical plus three host-adapter test groups pass on Linux; output-filter response and fractional host-tagged latency are measured in `Implementation_Status.md`. The first Rack control smoothing, module/UI and settings lifecycle are implemented and covered by five Rack-linked test groups. Live Rack/window-reopen and Windows checks, performance and complete nonlinear aliasing characterization remain open. User acceptance of the dry auditions supports continuing prototype integration; Crystal remains uncalibrated.

### 16.2 Event and control timing

Detect gates at host sample rate and deliver events once at the first corresponding internal substep. Do not repeat a strike once per oversample lane. Sample velocity at that event's host sample. Gate/velocity differences caused by upstream patch timing remain the user's signal timing; do not add an undocumented lookahead.

Update smoothed motion and pressure every internal step. Descriptor changes may use a control-rate builder, initially about 1 kHz, with interpolation of positive integration parameters between updates. Verify pitch-CV artifacts before optimizing further.

Oversampling filters and optional binaural filters add latency. Report or document measured latency and keep corresponding output/telemetry event semantics consistent. Do not claim zero latency.

### 16.3 Sample-rate changes

Rebuild coefficients and rate-conversion state outside the hot loop where the Rack lifecycle permits. Retain finite normalized bowl state, reinitialize rate-dependent history safely, and apply a short output transition if needed. Do not reinterpret old filter delays as samples at the new rate.

A reference engine must behave safely at every tested rate, with matched free pitches and intended T60. Similar nonlinear onset and contact behavior require separate convergence tests, not just matching poles.

An active striker must retain its latched material parameters and compression across a rate change. Recompute only step-dependent coefficients and continue the relative-velocity compression update; do not reconstruct overlap from reinterpreted bowl displacement. Log any intentional energy loss in the rate-transition policy.

### 16.4 Sleep

Sleep only after contacts are inactive, bowl energy remains below a documented threshold for a hold time, and resampler/output-filter tails have drained. Use an initial energy threshold around `1e−12 Eref` and at least 100 ms hold, subject to listening at long decay settings.

Wake immediately on strike, active contact, or an appropriate control transition. Do not truncate audible tails because LEVEL=0 or because an observer happens to sit at a node. With no output connected, skip dispensable output rendering, but preserve ongoing mechanical state and control behavior.

---

## 17. Suggested implementation layout and data flow

These are proposed component boundaries, not claims about currently existing files in the repository. Codex should inspect the current Leviathan tree before selecting exact paths and adapters.

```text
src/Vessel.cpp                      Rack adapter, config, lifecycle
src/VesselWidget.cpp                UI and controls
src/vessel/VesselEngine.hpp/.cpp    One bowl, contacts, step scheduling
src/vessel/ModalBank.hpp/.cpp       Coefficients, state, port admittance
src/vessel/StrikeContact.hpp/.cpp   Mass, compression, contact potential
src/vessel/FrictionContact.hpp/.cpp Moving angle, load, scalar friction law
src/vessel/ContactSolver.hpp/.cpp   Scalar/two-port reference solvers
src/vessel/BowlProfiles.hpp         Generated immutable descriptor data
src/vessel/MalletProfiles.hpp       Generated immutable contact data
src/vessel/Radiation.hpp/.cpp       Mono/stereo observers only
src/vessel/Binaural.hpp/.cpp        Optional later analytic-signal renderer
src/vessel/Telemetry.hpp            Finite snapshot/atomic interface
src/vessel/State.hpp                Versioned settings and stable IDs

tools/vessel/                      Profile generation, fitting, renders
tests/vessel/                     Unit, integration, performance tests
```

Keep the engine independent of Rack so the offline harness can run exactly the same DSP. Use fixed-capacity arrays, no audio-thread container growth, and deterministic inputs. Generated descriptors should be checked into source with their human-readable source data and provenance.

### Internal-step order

```text
1. Apply a pending host strike event once; update contact engagement.
2. Advance smoothed parameters and choose midpoint mallet angle.
3. Build/update modal coefficients and radial/tangential coupling vectors.
4. Compute free midpoint modal velocities and the small contact admittance.
5. Solve friction alone, or the coupled striker/friction problem.
6. Advance every modal coordinate once with the converged forces.
7. Advance striker compression/velocity; process completed separation.
8. Commit rotation angle; perform finite and energy-ledger checks.
9. Form required output observations; feed anti-alias downsamplers.
10. Publish decimated telemetry when due.
```

The control adapter must distinguish a new event from a held gate. Contact and observer code must never accidentally advance the modal state twice in one internal sample.

---

## 18. Acceptance and characterization plan

### A. Modal algebra and passive mechanics

Verify exact complex poles against requested `f,T60` for fixed parameters. Verify the collocated energy identity to double-precision tolerance, including random signed modal states and port gains. Verify nonincreasing unforced energy and zero output from an initially zero state with no interaction.

Test long tails for at least 120 s. Test all-zero imperfection without deleting B modes. Test coefficient transitions and descriptor interpolation with finite, nonnegative losses. Check that reciprocity and the positive-semidefinite admittance survive every optimization.

Add equal-pair rotational covariance tests: rotating specimen/contact/observer coordinates consistently must preserve the result, and changing the arbitrary A/B basis must preserve port work and total pair energy. Test inward-strike signs, physical units, and eigenvector/mass renormalization. Compare forced mobility and compliance at multiple rates; free poles alone are insufficient. Test raw energy independently from any display smoothing.

### B. Impact

From rest, render at least 16 strike velocities for every factory combination. Compare transferred bowl energy, integrated level, contact duration, rebound, and transient spectrum. Test repeated strikes during ringing, retrigger during contact, maximum gate rate, zero velocity, and simultaneous manual/cable triggers.

Audit bowl + striker + contact energy during isolated collision. Confirm that soft/hard materials differ through interaction, not merely an output filter. Solver faults or regular reliance on the 4 m/s cap are failures of ordinary presets.

Use complete contact trajectories starting at zero compression, including separation and retirement, across soft/hard seeds, low/high pitch, and finer rates. For strikes from rest, transferred bowl energy cannot exceed actual launch energy after losses. Do not require strict monotonic transfer across arbitrary stiffness/resonance interactions without evidence. Compare impulse, rebound velocity, maximum indentation, and contact duration as well as the ledger. Switch materials at maximum compression to verify the latching rule. Exercise signed retrigger work, capped launches, zero events, and active-contact sample-rate changes.

### C. Rubbing

Sweep speed, pressure, pitch, material, mallet, and direction. Record onset time, steady energy, dominant mode, spectrum, force extrema, solver iterations, and stability margin `Ytt L`. Include startup without a preliminary strike and startup while a tail is already ringing.

For `U=0`, verify friction contributes no positive mechanical work. For `N=0`, verify friction force is exactly zero. Stop motion while maintaining pressure, then lift contact separately; these must produce different, intentional behaviors. Inspect whether slow material changes move the usable pressure/speed region discontinuously.

Measure `Ytt Φ'(0)` as well as `Ytt L`, and sweep regularization jointly with internal rate. Verify that changing speed updates slip and orbit consistently during gate release. Compare frozen-angle linearized growth with actual rotating trajectories; neither can substitute for the other. Include a perfectly degenerate pair with a moving contact and fixed observer to distinguish rotation-induced envelope variation from mistuned-doublet beating. Test several complete rotations after onset and after speed/pressure perturbations, recording average hand work, losses, boundedness, and modal displacement. Characterize hysteresis by sweeping controls in both directions.

Initial factory usability targets, not established facts: default Metal/Suede should produce clear sustained tone within about 0.5–5 s; a neighborhood around its default speed and pressure should remain usable. Set final targets by comparison with chosen recordings rather than forcing every setting to sound identical.

### D. Coupled strike and rub

Implement a whole-system energy ledger for simultaneous contacts and run random-state one-step tests. Then render sustained rubbing with periodic strikes. Compare the nested reference solver and any optimized solver. Check that contact work is counted once, that separation does not create impulses, and that contact-parameter changes do not cause faults.

This test group is mandatory before calling the reference DSP complete. The supplied algebra script covers randomized one-step simultaneous contacts only; it does not cover sustained trajectories, production solvers, or real-time operation.

Extend the ledger to launch, cap, retirement, coefficient/contact changes, and any known normal force before using it as a whole-run energy audit. Freeze active strike material properties. Compare the tangential-only proxy, prescribed radial-load experiment, and compliant normal contact on matched conditions before making a full physical-fidelity claim. Audible agreement from one force/speed setting is insufficient.

### E. Stereo and binaural

At zero separation from identical rest states and WIDTH=0, subtract L from R and require numerical silence before unequal external routing. Changing observer position/width must leave both mechanical states bit-identical in a deterministic run. Check mono summing explicitly.

For dual-bowl tuning, verify exact pair-center separation and compare each output against an independently tuned complete simulation. Measure dominant spectral offsets during free tails and steady rubbing separately from the requested pair-center offset. Exercise smooth retuning, zero crossings, both energy ledgers, material/rate transitions and boundary clamping. Different tuning may change contact trajectories and energy; observer/output changes must not.

### F. Rates, aliasing, and performance

Test at host 44.1, 48, 88.2, 96, 176.4, and 192 kHz, plus 32 kHz if supported. Free modal frequencies should match targets within 0.1 cent after settling; T60 estimates should agree within 1% when measured in a suitable noise-free interval. Nonlinear renders need perceptual and spectral convergence checks because sample-for-sample identity is not expected across rates.

Render high-force strikes, sharp pressure changes, and fast rubbing against a higher-rate reference. Measure folded components; define final alias acceptance thresholds from these comparisons, not from the oversampling factor alone.

Use separate rate-convergence and modal-truncation studies, then combined convergence. Keep physical parameters fixed while refining the discretization. Report the tested mechanical-frequency band, parameter ranges, and remaining differences rather than calling every stable render accurate. Re-evaluate the 32-instance performance target if those studies require more bandwidth or a different contact model.

Benchmark core, scalar friction, active impact, observer, resampler, UI, and optional binaural separately. Record mean and worst block time, iteration percentiles, sample rate, compiler flags, CPU model, and build revision. Budget worst-case simultaneous impact/rubbing, not only a sleeping instance.

After matching the double-precision oracle, cache coefficient transforms and use angle recurrences/lookup tables for moving shapes where appropriate. Any friction approximation must preserve oddness, force bounds, `F s ≥ 0`, and a certified negative-slope bound; Newton must use a derivative consistent with the implemented approximation. Preserve the discrete-gradient potential/force relationship rather than approximating the two independently. In Rack, use the repository's debug gating and `Process`, `Step`, `Draw`, `DrawLayer` telemetry semantics; report optional component costs after them.

A provisional objective is 32 active default-rate monophonic instances without overruns in a controlled one-core-equivalent benchmark on the chosen reference desktop, with documented headroom. This is a target, not a measured capability. Revisit it after the first native harness exists.

### G. Lifecycle and regression

Test patch save/load, randomization, reset, engine bypass, output cable changes, UI open/close, material changes during tails, sample-rate changes, sleep/wake, and nonfinite inputs. Stable descriptor IDs must preserve old-patch behavior. GUI activity must not change deterministic audio.

---

## 19. Implementation phases and completion gates

| Phase | Deliverable | Gate before proceeding |
|---|---|---|
| 1. Mechanical foundation | Paired modal bank, descriptor format, exact-pole integrator, offline render harness, energy ledger, diagnostic force pulse. | Pole, passive-energy, rotational-covariance, and forced-mobility tests pass; deterministic output; explicit accurate mechanical band. |
| 2. Finite-mass strike | Nonlinear contact potential, latched mallet profiles, strike velocity and retrigger semantics. | Complete collision/event energy audit; separate modal/rate convergence; comparison with dry strike references. |
| 3. Singing contact | Moving tangential shape, prescribed load parameter, implicit friction, simultaneous strike/rub solver. | Sustained moving-contact tone and stable saturation; full energy audit; rate/epsilon/mode sweeps; dry rub reference comparisons and normal-force sensitivity experiment. |
| 4. Rack instrument | Controls, stereo, energy bar, serialization, rate adaptation, telemetry, performance work. | Lifecycle, long-tail, stereo-invariance, and real-time tests. |
| 5. Physical calibration | Measured crystal descriptor; improved metal losses/radiation; calibrated material-pair contacts. Continue calibration begun in phases 2–3. | Documented reference comparisons and provenance; no invented measurement claims; validated descriptor normalization. |
| 6. Normal-contact fidelity | Radial normal-contact model and contact loss; necessary to claim these behaviors, optional for a labelled playable proxy. | Extended energy/geometry audit, normal-force comparison, contact-loss acceptance, and reference trajectories. |
| 7. Dual-bowl reference | Independent bowl/contact state, shared controls, 0–33 Hz structural tuning difference, smooth transitions. | Independent reference equivalence, two energy ledgers, state continuity, output spectra and measured scalar cost. |

Do not spend early development effort on a large animated bowl or binaural controls before Phase 3 demonstrates convincing dry strike and rubbed audio. The key uncertainty is the playable nonlinear interaction and calibration, not panel artwork.

---

## 20. Explicit open decisions and risks

**Crystal identity:** the initial descriptor is synthetic. Obtaining a suitable reference recording set is the most important remaining material-data task.

**Friction calibration:** static/kinetic coefficients, transition velocity, regularization, pressure range, and patch shape strongly influence onset and mode selection. A plausible numerical range does not establish audible realism.

**Normal contact:** the first engine uses pressure only to set friction strength. It omits the radial force itself, not merely intermittent contact. The required sensitivity experiment must determine when that omission is acceptable; noise or amplitude modulation cannot correct it.

**Force normalization:** changing modal masses or contact coupling to “make it louder” changes the feedback physics. Tune output gain separately and retain the energy ledger.

**Model truncation:** adding or removing upper modes changes contact admittance. Compare force and onset behavior as well as the output spectrum.

**Numerical versus physical accuracy:** the integrator is energy-consistent and has the selected free poles. Prewarped stiffness, contact stiffness, regularization, and truncated mobility still affect contact trajectories. Root uniqueness, passivity, self-excitation, bounded saturation, and physical fidelity are separate requirements.

**Dual-bowl cost:** the scalar two-simulation reference has been measured in limited offline Linux runs (see `Implementation_Status.md`). SIMD/shared-coefficient optimization and the Rack 32-instance budget remain open. No analytic-signal or pitch-shifter path is used.

**Project integration:** this draft does not assert that the proposed file names or helper classes already exist in Leviathan. Inspect and reuse appropriate live repository conventions without transplanting unrelated Doorstop dynamics.

---

## 21. Sources and verification record

### Supplied basis

**R1.** `Singing Bowl Research.md` (original attachment name `Singing Bowl Research(1).md`), title *Singing Bowl Acoustics and Physical Modeling*. Relevant source lines: acoustics 3; resonator architecture 7–11; material/mallet ideas 15–19; feasibility 23–27; control mapping 31–43. The report's final source statement does not include a complete traceable bibliography. It is therefore an architectural starting point, not a source of measured descriptor coefficients.

### Narrow primary-source checks

**V1.** Octávio Inácio, Luís L. Henrique, José Antunes, *The Dynamics of Tibetan Singing Bowls*, Acta Acustica united with Acustica 92 (2006), 637–653. Used here for the A/B radial/tangential distinction, rim mode shapes, and the explicitly identified Bowl 2 frequency pairs. PDF pages containing the table and equations were visually inspected.

```text
https://iypt.ru/wp-content/uploads/2024/10/The-dynamics-of-Tibetan-singing-bowls.pdf
```

**V2.** VCV Rack Manual, *Voltage Standards*, consulted 2026-09-30. Used for 1 V/octave, Schmitt-trigger conventions, and typical ±5 V audio. Control ranges and normalizations in this document are Vessel-specific choices.

```text
https://vcvrack.com/manual/VoltageStandards
```

**V3.** VCV Rack Manual, *Plugin API Guide*, consulted 2026-09-30. Consulted for the host/plugin setting; the DSP architecture and proposed class boundaries are independent design choices, not copied Rack classes.

```text
https://vcvrack.com/manual/PluginGuide
```

### Tests actually performed for this draft

The accompanying `vessel_numerical_checks.py` uses NumPy and a fixed seed. It is a small numerical oracle, not a real-time engine. The saved `numerical_check_results.json` records:

| Check | Cases | Largest observed error/result |
|---|---:|---:|
| Complex modal pole placement | 192 | 3.51e−16 absolute complex error |
| Collocated modal energy identity | 10,000 | 1.51e−14 absolute normalized-energy error |
| Unforced modal energy | 10,000 | All tested changes negative |
| Implicit friction residual | 3,000 | 3.64e−14 N (review rerun) |
| Friction dissipation `F * slip` | 3,000 | No negative result |
| Isolated nonlinear impact energy | 200 | 8.62e−18 J |
| Simultaneous strike/rub one-step energy | 200 | 6.61e−18 J |
| Simultaneous-contact strike-force residual | 200 | 3.55e−15 N |
| Conservative seed-profile uniqueness bound | 8 bowl/mallet combinations | Maximum `Ytt L` < 0.000975 |

These checks support the algebraic implementation directions in §§7–10. They do **not** establish long-term coupled behavior, a convincing singing tone, calibrated crystal behavior, real-time performance, alias rejection, or finished Rack compatibility. Those remain the explicit implementation and validation gates above.

### Draft 0.2 review checks (2026-09-30)

The original oracle was rerun successfully with Python/NumPy 2.3.5 on Windows. `vessel_review_checks.py` and `review_check_results.json` add reproducible checks of compliance warping, the outer-solve derivative, pair-basis port invariance, frozen-angle sliding linearizations for all eight seed combinations, and an 8,192-step stationary-contact passive trajectory (42.7 ms at 192 kHz). The largest energy-ledger residual in that trajectory was approximately `6.41e−19 J`. A deliberately hypothetical compressed-spring material switch quantifies why contact parameters must latch; no seed values were fitted or changed.

These additional checks are narrow diagnostics, not the sustained rotating, complete impact, aliasing, or production-performance tests requested in §18. `Physical_Model_Review.md` records the remaining gates. Primary-source rechecking confirmed V1 Table I and equation (5); all new stability, compliance, geometry, and event-accounting derivations are Vessel design analysis rather than claims that V1 implemented this numerical scheme.
