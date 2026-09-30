# Vessel — DSP and Module Specification

**Plugin:** Leviathan for VCV Rack  
**Working module name:** Vessel  
**Document status:** Implementation draft 0.1 — physical model first  
**Prepared:** 2026-09-30

## 0. Scope, evidence, and readiness

Vessel is a tunable, struck and friction-excited singing bowl. A single persistent mechanical model receives both kinds of excitation. Stereo audio and an energy display observe that model. Optional binaural-beat processing sits after it and must not alter its mechanics.

This document starts from the supplied **Singing Bowl Acoustics and Physical Modeling** report, supplied as `Singing Bowl Research(1).md`. It retains the report's organizing ideas: modal/banded resonators, metal versus crystal, mallet-dependent impact/friction, real-time feasibility, and musical parameter mapping. It selects a modal implementation and supplies the equations and engineering decisions the report leaves open.

Evidence labels used here:

| Label | Meaning |
|---|---|
| **R — Report** | An approach or observation supported by the supplied report. |
| **V — Verification** | A narrowly scoped check against a primary source, identified in §21. |
| **D — Design** | A proposed Vessel engineering decision, equation, interface, or test. Not a recovered implementation. |
| **U — Uncalibrated** | A provisional value or behavior requiring measurements, listening, or numerical validation. |

Unless explicitly identified as R or V, specifications below are **D**. Numerical profile defaults are **U**, except the explicitly identified published metal frequencies. Values in seconds, kilograms, newtons, and meters are internally dimensioned modeling parameters; uncalibrated values must not be advertised as measurements of a commercial instrument.

**Ready to implement:** the paired modal core, energy-consistent integration, finite-mass strike, prescribed-load friction model, control interface, stereo observation, and energy telemetry.

**Not yet established:** realistic metal/crystal profile calibration; the best mallet friction curves; audible quality; CPU cost in Rack; alias rejection; and the optional contact-loss and binaural implementations. Algebra checks included with this draft are not an audio-quality or performance certification.

**Do not replace missing calibration with fabricated measurements.** Preserve provenance at the individual descriptor-field level.

---

## 1. Instrument contract

Vessel must provide tunable bowl frequency, a strike gate, strike velocity, a rotation gate, continuous rotation speed, bowl material selection, mallet material selection, stereo audio, and an energy bar.

Add **normal pressure** as a first-class control. It is independent of speed. A knob is required; pressure CV is strongly recommended. The report already identifies force and speed as separate rubbing controls (R, source report lines 19 and 39).

The following behaviors are mandatory:

1. Strike and rotation may operate simultaneously. They are not mutually exclusive modes.
2. Retriggering adds an interaction to the existing bowl state. It never resets resonator phase or clears a ringing tail.
3. Releasing the rotation gate removes contact over a short ramp. It does not mute the bowl.
4. Zero rotation speed with contact still engaged can damp an already-ringing bowl; it is not equivalent to lifting the mallet.
5. Changing output level, stereo width, or binaural settings must not change the mechanical state or the energy bar.
6. Material changes affect model descriptors and contact behavior, not merely output EQ.

Initial scope is **one continuously sounding bowl per module**, with monophonic CV inputs and two monophonic audio outputs. Repeated strikes overlap mechanically on that bowl. Polyphonic cables and multiple independent bowls are later work, not implied by the stereo outputs.

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

Normal impact uses `φr`. Rubbing uses `φt`. Audio initially observes radial velocity through a separate output weighting.

### 4.3 Finite contact patch

An angularly uniform patch of full width `w` can use the following normalized spatial average:

\[
P_n(w)=\operatorname{sinc}(nw/2),\qquad
\operatorname{sinc}(z)=\frac{\sin z}{z},\quad\operatorname{sinc}(0)=1.
\]

Multiply both shapes in the pair by `P_n`. Wider patches reduce excitation of high spatial orders. Use the same patch-averaged shape for force injection and contact-velocity feedback.

This is an explicit spatial approximation, not a measured mallet contact-pressure distribution. Profiles may later replace it with integrated nonuniform patch shapes.

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

No audio-rate FM claim is made for draft 0.1. Test ordinary pitch CV and envelopes first. An eventual unsmoothed audio-rate mode needs a separate coefficient-update and aliasing budget.

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

---

## 8. Strike: finite moving mass and nonlinear contact

### 8.1 Primary model

Use a striker with effective mass `m_s`, signed inward velocity `v_s`, and compression `δ`. The bowl receives positive radial contact force `F_s`; the striker receives `−F_s`.

At a strike event, sample velocity CV and strike location once. If no striker is active, place it at first contact (`δ=0`) and set its velocity to current local bowl velocity plus the requested launch velocity. Do not clear bowl state.

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

The striker may exchange energy with an already-moving bowl. Do not demand monotonic output peak versus strike velocity on an arbitrary ringing state. Monotonicity tests should start from rest and compare energy or integrated loudness.

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

`U` is imposed mallet tangential speed in m/s. It is not a frequency driving a sine oscillator. Evaluate contact shapes at midpoint angle. Preserve rotation angle across gate transitions; gate-on does not restart its phase. When the gate is off, freeze the displayed/contact orbit unless a future explicit free-spin option is enabled.

Default signed speed range is −2 to +2 rev/s, default +0.4. With SPEED CV patched, multiply the knob value by `clamp(V/5,−1,1)`. Thus 0 V stops, +5 V requests the knob speed, and −5 V reverses it. Unpatched multiplier is 1.

### 9.2 Normal-load approximation in the first reference engine

For the initial engine, normal load `N` is prescribed by the pressure control and a contact-engagement envelope:

\[
N=e_{contact}\,P,\quad0\le P\le15\ \text{N}.
\]

This is an ideal normal follower: it maintains the requested load but does **not** model radial mallet bounce, loss of rim contact, or rattling during rubbing. Normal static deformation is omitted in this approximation. Only tangential friction acts dynamically on the bowl while rotating.

This approximation is deliberate and must remain named in the implementation. Full normal compliance is a separate refinement in §12. Do not generate random clicks and label them modeled contact loss.

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

Fit impact spectra across several velocities and mallet materials next. If force measurements are unavailable, modal mass, contact stiffness, and radiation gain may be underdetermined; store that uncertainty instead of calling each fitted number physically unique.

Finally fit sustained-rub behavior over speed/pressure trajectories: onset time, steady amplitude, which mode dominates, spectral evolution, and response to stopping or lifting the mallet. Output EQ alone is insufficient to calibrate the feedback mechanics.

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

Set observer center to 20° and maximum separation initially to 30°. WIDTH moves symmetrically between coincident and separated observers. Keep the default strike angle at 22.5° relative to the nominal bowl orientation so it does not intentionally excite only one member of the lowest pair.

At WIDTH=0, L and R must null after subtraction to numerical tolerance. Stereo does not use independent per-ear modal detuning, separate friction noise, or reverb. Width changes only observation coefficients.

When only L is connected, provide a documented L/MONO result. Initial policy: `(L+R)/2` after the selected stereo renderer. Preserve the normal stereo signals when both are connected. Cancellation in a spatial or binaural sum is possible and is not “fixed” by energy normalization. Meter physical energy independently.

### 13.2 Binaural beats: explicit post-processing, default off

For this draft, “binaural” means an optional controlled interaural frequency difference, not a spatialized room simulation. Put true HRTF rendering in a separate future feature.

Do **not** simply detune two resonator readout banks driven by the same sustained force. In a driven linear system, those banks can still respond at the common driving frequency; different natural frequencies do not guarantee the intended interaural offset during continuous rubbing.

Instead construct an analytic signal from a mono observation of the physical bowl:

\[
z(t)=a(t)+i\,\mathcal H\{a(t)\}.
\]

For desired ear-to-ear difference `Δ` Hz:

\[
\dot\psi=\pi\Delta,
\quad L=\Re\{z e^{-i\psi}\},
\quad R=\Re\{z e^{+i\psi}\}.
\]

Each positive-frequency component is translated by `−Δ/2` and `+Δ/2`. This intentionally shifts the spectrum in Hz, rather than scaling all ratios as pitch transposition would. Mechanical motion and its natural beating remain those of the original bowl.

Initial BEAT range is 0–20 Hz, default 4 Hz when enabled. Integrate phase continuously under modulation. Avoid restarting phase at every strike. Limit the allowed shift so significant source components do not cross DC; suppress irrelevant DC/very-low-frequency content before the analytic transform.

Require a verified Hilbert/analytic implementation with matched I/Q delay and at least 60 dB unwanted-sideband rejection over a declared useful passband, initially 40 Hz to `min(18 kHz, 0.40 Fs)`. Choose IIR allpass or FIR implementation only after measuring rejection, transient behavior, latency, and CPU. Do not assume a short, arbitrary Hilbert FIR meets that low-frequency requirement.

Binaural mode uses separated ear signals, with no default crossfeed or unshifted common carrier. Its input is the mono observer; native WIDTH is therefore bypassed explicitly while this mode is selected. Compensate latency before crossfading native and binaural paths. At zero shift, bypass to the latency-matched mono path so approximate quadrature does not change the sound unnecessarily.

This is an optional synthesis effect for headphones. Do not attach claims about health outcomes, entrainment efficacy, or a guaranteed subjective experience. It does not replace the core bowl physics and is not required for the first implementation milestone.

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
| L/MONO, R | Two audio outputs; one bowl, no automatic polyphony. |
| ENERGY display | Physical bowl-state energy, not output RMS. |

Context-menu settings may include strike angle, observer azimuth, quality, optional damping, and later binaural enable/rate. Keep material selection on the panel, not only in a menu.

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

Modes above host Nyquist can still participate internally when supported; the output decimator removes their audible aliases. Do not delete them merely because the host output cannot represent them: contact admittance and strike response can depend on them.

Use a measured anti-alias decimator with declared passband, stopband, and latency. Compare complete nonlinear renders against a higher-rate reference. Stable time integration alone does not guarantee low aliasing.

### 16.2 Event and control timing

Detect gates at host sample rate and deliver events once at the first corresponding internal substep. Do not repeat a strike once per oversample lane. Sample velocity at that event's host sample. Gate/velocity differences caused by upstream patch timing remain the user's signal timing; do not add an undocumented lookahead.

Update smoothed motion and pressure every internal step. Descriptor changes may use a control-rate builder, initially about 1 kHz, with interpolation of positive integration parameters between updates. Verify pitch-CV artifacts before optimizing further.

Oversampling filters and optional binaural filters add latency. Report or document measured latency and keep corresponding output/telemetry event semantics consistent. Do not claim zero latency.

### 16.3 Sample-rate changes

Rebuild coefficients and rate-conversion state outside the hot loop where the Rack lifecycle permits. Retain finite normalized bowl state, reinitialize rate-dependent history safely, and apply a short output transition if needed. Do not reinterpret old filter delays as samples at the new rate.

A reference engine must behave safely at every tested rate, with matched free pitches and intended T60. Similar nonlinear onset and contact behavior require separate convergence tests, not just matching poles.

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

### B. Impact

From rest, render at least 16 strike velocities for every factory combination. Compare transferred bowl energy, integrated level, contact duration, rebound, and transient spectrum. Test repeated strikes during ringing, retrigger during contact, maximum gate rate, zero velocity, and simultaneous manual/cable triggers.

Audit bowl + striker + contact energy during isolated collision. Confirm that soft/hard materials differ through interaction, not merely an output filter. Solver faults or regular reliance on the 4 m/s cap are failures of ordinary presets.

### C. Rubbing

Sweep speed, pressure, pitch, material, mallet, and direction. Record onset time, steady energy, dominant mode, spectrum, force extrema, solver iterations, and stability margin `Ytt L`. Include startup without a preliminary strike and startup while a tail is already ringing.

For `U=0`, verify friction contributes no positive mechanical work. For `N=0`, verify friction force is exactly zero. Stop motion while maintaining pressure, then lift contact separately; these must produce different, intentional behaviors. Inspect whether slow material changes move the usable pressure/speed region discontinuously.

Initial factory usability targets, not established facts: default Metal/Suede should produce clear sustained tone within about 0.5–5 s; a neighborhood around its default speed and pressure should remain usable. Set final targets by comparison with chosen recordings rather than forcing every setting to sound identical.

### D. Coupled strike and rub

Implement a whole-system energy ledger for simultaneous contacts and run random-state one-step tests. Then render sustained rubbing with periodic strikes. Compare the nested reference solver and any optimized solver. Check that contact work is counted once, that separation does not create impulses, and that contact-parameter changes do not cause faults.

This test group is mandatory before calling the reference DSP complete. The supplied algebra script covers randomized one-step simultaneous contacts only; it does not cover sustained trajectories, production solvers, or real-time operation.

### E. Stereo and binaural

At WIDTH=0, subtract L from R and require numerical silence before unequal external routing. Changing observer position/width must leave the mechanical state bit-identical in a deterministic run. Check mono summing explicitly.

For the optional binaural renderer, verify measured `Δ` during both a free tail and steady rubbing. Measure unwanted sidebands, transient artifacts, transition latency, and drift under beat-rate CV. Enabling binaural must leave bowl energy and mechanical state unchanged.

### F. Rates, aliasing, and performance

Test at host 44.1, 48, 88.2, 96, 176.4, and 192 kHz, plus 32 kHz if supported. Free modal frequencies should match targets within 0.1 cent after settling; T60 estimates should agree within 1% when measured in a suitable noise-free interval. Nonlinear renders need perceptual and spectral convergence checks because sample-for-sample identity is not expected across rates.

Render high-force strikes, sharp pressure changes, and fast rubbing against a higher-rate reference. Measure folded components; define final alias acceptance thresholds from these comparisons, not from the oversampling factor alone.

Benchmark core, scalar friction, active impact, observer, resampler, UI, and optional binaural separately. Record mean and worst block time, iteration percentiles, sample rate, compiler flags, CPU model, and build revision. Budget worst-case simultaneous impact/rubbing, not only a sleeping instance.

A provisional objective is 32 active default-rate monophonic instances without overruns in a controlled one-core-equivalent benchmark on the chosen reference desktop, with documented headroom. This is a target, not a measured capability. Revisit it after the first native harness exists.

### G. Lifecycle and regression

Test patch save/load, randomization, reset, engine bypass, output cable changes, UI open/close, material changes during tails, sample-rate changes, sleep/wake, and nonfinite inputs. Stable descriptor IDs must preserve old-patch behavior. GUI activity must not change deterministic audio.

---

## 19. Implementation phases and completion gates

| Phase | Deliverable | Gate before proceeding |
|---|---|---|
| 1. Mechanical foundation | Paired modal bank, descriptor format, exact-pole integrator, offline render harness, energy ledger, diagnostic force pulse. | Pole and passive-energy tests pass; deterministic output. |
| 2. Finite-mass strike | Nonlinear contact potential, mallet profiles, strike velocity and retrigger semantics. | Isolated collision energy audit and velocity/material renders pass. |
| 3. Singing contact | Moving tangential shape, prescribed pressure, implicit friction, simultaneous strike/rub solver. | Usable sustained tone, full energy audit, convergence and parameter sweeps. |
| 4. Rack instrument | Controls, stereo, energy bar, serialization, rate adaptation, telemetry, performance work. | Lifecycle, long-tail, stereo-invariance, and real-time tests. |
| 5. Physical calibration | Measured crystal descriptor; improved metal losses/radiation; calibrated material-pair contacts. | Documented reference comparisons and provenance; no invented measurement claims. |
| 6. Fidelity refinement | Optional radial normal-contact model and contact loss. | Extended energy/geometry audit and contact-loss acceptance. |
| 7. Optional binaural | Verified analytic transform, ear-frequency shifting, smooth transitions. | Steady-rub and free-tail frequency-offset tests; no mechanical changes. |

Do not spend early development effort on a large animated bowl or binaural controls before Phase 3 demonstrates convincing dry strike and rubbed audio. The key uncertainty is the playable nonlinear interaction and calibration, not panel artwork.

---

## 20. Explicit open decisions and risks

**Crystal identity:** the initial descriptor is synthetic. Obtaining a suitable reference recording set is the most important remaining material-data task.

**Friction calibration:** static/kinetic coefficients, transition velocity, regularization, pressure range, and patch shape strongly influence onset and mode selection. A plausible numerical range does not establish audible realism.

**Normal contact:** the first engine maintains prescribed pressure and cannot reproduce intermittent radial contact. This is a declared limitation, not something to disguise with noise or amplitude modulation.

**Force normalization:** changing modal masses or contact coupling to “make it louder” changes the feedback physics. Tune output gain separately and retain the energy ledger.

**Model truncation:** adding or removing upper modes changes contact admittance. Compare force and onset behavior as well as the output spectrum.

**Numerical versus physical accuracy:** the integrator is energy-consistent and has the selected free poles. It still approximates contact trajectories; oversampling and convergence tests remain necessary.

**Optional binaural cost:** low-frequency analytic-signal quality, latency, and CPU have not been chosen or measured. Keep the interface optional until this is resolved.

**Project integration:** this draft does not assert that the proposed file names or helper classes already exist in Leviathan. Inspect and reuse appropriate live repository conventions without transplanting unrelated Doorstop dynamics.

---

## 21. Sources and verification record

### Supplied basis

**R1.** `Singing Bowl Research(1).md`, title *Singing Bowl Acoustics and Physical Modeling*. Relevant source lines: acoustics 3; resonator architecture 7–11; material/mallet ideas 15–19; feasibility 23–27; control mapping 31–43. The report's final source statement does not include a complete traceable bibliography. It is therefore an architectural starting point, not a source of measured descriptor coefficients.

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
| Implicit friction residual | 3,000 | 3.73e−14 N |
| Friction dissipation `F * slip` | 3,000 | No negative result |
| Isolated nonlinear impact energy | 200 | 8.62e−18 J |
| Simultaneous strike/rub one-step energy | 200 | 6.61e−18 J |
| Simultaneous-contact strike-force residual | 200 | 3.55e−15 N |
| Conservative seed-profile uniqueness bound | 8 bowl/mallet combinations | Maximum `Ytt L` < 0.000975 |

These checks support the algebraic implementation directions in §§7–10. They do **not** establish long-term coupled behavior, a convincing singing tone, calibrated crystal behavior, real-time performance, alias rejection, or finished Rack compatibility. Those remain the explicit implementation and validation gates above.
