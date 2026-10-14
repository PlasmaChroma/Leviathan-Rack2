# Doorstop Reference V3 Design
## A reduced helical-continuum model for an authentic spring-doorstop sound

**Target:** Leviathan Rack 2, `expander` branch  
**Proposed engine:** `ReferenceV3` / `HelicalContinuumEngine`  
**Status:** Implementation design and falsifiable experiment plan  
**Date:** 2026-08-26

---

## 1. Executive decision

The next Doorstop model should **not** be another scalar flex oscillator feeding an independently tuned modal bank, another noise-fed delay, or another variation of a twice-per-cycle amplitude gate.

The recommended competing model is:

> **Reference V3: a reduced two-plane helical continuum with a compliant rubber cap, geometry-derived modal participation, optional deterministic inter-coil contact, and radiation derived from structural acceleration and base reaction.**

The practical implementation is a **real-time reduced-order model (ROM)** rather than a brute-force coil-by-coil finite-element simulation. It retains a small set of structural coordinates, but those coordinates share one geometry:

- The same modes determine the visible tip motion.
- The strike enters through the rubber cap and the tip participation vector.
- Inter-coil contact acts on reconstructed physical gaps and projects force back into the same modes.
- The mount is driven by the spring’s reaction force rather than by an unrelated strike impulse.
- Audio is observed from distributed acceleration and base reaction, rather than made by multiplying a static resonant chord by a scalar “boing” envelope.

This is the first proposed Doorstop architecture in which the slow motion, metallic body, odd/even variation, contact, pitch evolution, and visual state can all be consequences of **one mechanical state vector**.

The existing models should remain available during development. They provide useful controls and make it possible to falsify the new architecture. Stripping them should happen only after Reference V3 wins repeatable RMS-matched listening comparisons across the full reference corpus—not merely after it produces better feature scores.

---

## 2. What the current module already gets right

The Doorstop module is not starting from failure. Several foundations are already strong.

### 2.1 The product architecture supports comparison

The current router cleanly preserves:

- `ReferenceV1`
- `ReferenceV2`
- the Legacy engine
- five Legacy sound choices: `Classic`, `CoupledBody`, `CoilContact`, `DispersiveSpring`, and `ProbabilisticMix`

The router also already propagates sample rate, specimen seed, break-in state, sleep state, and crossfaded engine transitions. That is exactly the infrastructure needed to introduce a true competing engine without destabilizing the existing module.

### 2.2 The visual behavior is useful and should be preserved

The existing front-panel visualization successfully communicates:

- strike direction and intensity
- large initial deflection
- decaying oscillation
- eventual return to rest
- rubber-cap motion

Reference V3 should continue returning the existing `Frame` structure. The visible scalar displacement should become a projection of the new model’s two-dimensional tip motion:

\[
x_{\text{visual}} = \mathbf{e}_{\text{panel}}^\mathsf{T}\mathbf{u}_{\text{tip}}
\]

This lets the front panel look essentially unchanged while the hidden physical state becomes much richer.

### 2.3 The reference-analysis pipeline is unusually good

The repository already contains:

- a manually audited multi-recording onset manifest
- batch reference analysis
- deterministic rendering
- population comparison
- critical material features
- matched listening fixtures
- exact within-group RMS matching
- explicit warnings that objective score is not a perceptual verdict

Reference V3 should extend this pipeline, not replace it.

---

## 3. Why the existing audio architectures have probably plateaued

### 3.1 Legacy: physical layers remain parallel effects

The Legacy engine contains several valuable experiments, but its structure is effectively:

\[
y =
y_{\text{macro}}
+y_{\text{modes}}
+y_{\text{impact}}
+y_{\text{contact or delay}}
\]

The low spring, four resonators, random impact burst, stochastic contact branch, and dispersive delay can influence one another in limited ways, but they are not different observations of one distributed object. The ear can therefore separate them into “low oscillator,” “metal resonator,” “noise,” and “effect.”

The `CoilContact` experiment is also intentionally inactive at ordinary strike levels in its current tests. That makes contact a hard-hit decoration rather than an explanation of the core doorstop identity.

### 3.2 Reference V1/V2: the correct evidence led to a synthetic shortcut

Reference V1/V2 made a major improvement by recognizing that:

- the visible flex is much slower than the audible metallic body
- metallic energy must persist between lobes
- the familiar boing has roughly two audible lobes per mechanical cycle
- motion should articulate stored high-frequency energy rather than simply output a 20 Hz oscillator

However, the implementation realizes this mostly as:

1. one scalar nonlinear flex oscillator
2. twelve independent resonators
3. direct strike injection into all resonators
4. explicit center-crossing reinforcement
5. a noise-driven fixed-delay texture
6. a scalar phase-derived radiation gate multiplied over modes and texture
7. an independently struck mount oscillator

In Reference V2 the gate becomes particularly narrow:

\[
g = 0.035 + 0.965\,e_{\text{radiation}}^2
\]

The final signal is then driven strongly into a bounded `tanh` stage. This can produce the correct *count* of lobes while still sounding like a stable metallic carrier chopped by tremolo and then saturated.

### 3.3 The production V2 path is less coupled than V1

`DarkRefinedV2` disables the existing junction coupling. That choice was rational—the old junction did not materially improve the corpus metrics—but it leaves V2’s macro flex and resonant body even more explicitly separated.

The correct conclusion is not “restore the old junction.” It is that the coupling needs a different topology: force participation derived from a shared geometry, not one hand-authored alternating mode-shape vector.

### 3.4 The strike is synthesized as noise rather than transmitted force

The present reference strike:

- immediately changes scalar spring velocity
- directly adds velocity to every resonator
- excites a filtered random-noise transient
- directly kicks a separate mount resonator
- starts a noise-fed texture loop

A real strike or flick is a finite force applied through the rubber cap or wire. It excites each structural mode according to that mode’s displacement at the contact point. The attack may contain microscopic stochastic roughness, but white noise should not be its primary physical cause.

### 3.5 The mount is not currently a boundary condition

The mount oscillator is directly excited at strike time and then runs independently. A real baseboard or wall is excited by the spring’s reaction force. It should therefore respond throughout the motion and remain phase-locked to the structural state.

This could be perceptually important. A recorded spring doorstop is not only a wire radiating into air; it is a wire transmitting force into a base, wall, door, or floor trim. The corpus contains different mounting surfaces, so some inter-recording variation may be boundary variation rather than spring variation.

### 3.6 The current tests prove safety and broad spectral placement, not identity

The existing tests are effective at proving:

- finite and bounded processing
- settling and sleep behavior
- crossing activity
- rough band-energy corridors
- deterministic specimen behavior
- V2 darkness relative to V1
- exact router behavior
- safe engine switching

They do not prove that listeners identify the result as a spring doorstop. The repository’s own comparison tool correctly warns that a higher objective score cannot establish ordinary spring-steel material identity.

---

## 4. A critical unresolved question: crossing or maximum bend?

The measured reference work establishes a strong result:

- the high-frequency envelope has approximately two lobes per slower mechanical cycle

That does **not**, by itself, establish the phase of those lobes.

Both of these signals repeat twice per cycle:

\[
|x(t)|
\]

and

\[
|v(t)|
\]

Therefore an audio-only observation at approximately \(45.5\ \text{lobes/s}\) with an underlying motion near \(22.2\ \text{cycles/s}\) is compatible with at least two distinct mechanisms:

- **center-crossing / maximum-velocity radiation**
- **maximum-curvature / maximum-bend radiation or contact**

They are separated by roughly one quarter of a mechanical cycle. The current Reference engines commit to the first interpretation. If the real object is dominated by the second—or by a mixture of base reaction, curvature radiation, and occasional contact—the pulse rate can be exactly correct while the gesture still feels physically wrong.

This should be treated as a first-class experimental question, not another ear-tuned constant.

### 4.1 Recommended synchronized capture

Record a representative real doorstop with:

- 120 or 240 fps fixed-camera video
- a high-contrast marker on the rubber cap
- 48 kHz audio
- a visible/audible synchronization event
- soft, medium, and hard strikes
- strikes in both directions
- at least 8–10 takes per condition

Add `tools/analyze_doorstop_motion_phase.py` to:

1. track tip displacement
2. smooth and differentiate it to estimate velocity
3. extract the \(>500\ \text{Hz}\) audio envelope
4. detect lobe peaks
5. calculate lobe phase relative to displacement and velocity
6. report whether the distribution clusters near crossings, extrema, or neither

This single experiment may explain more than another month of modal tuning.

---

## 5. Proposed model: `HelicalContinuumEngine`

### 5.1 Conceptual network

```text
             finite-duration finger / strike force
                            |
                            v
                 +---------------------+
                 | compliant rubber cap|
                 | mass + viscoelastic |
                 | attachment          |
                 +----------+----------+
                            |
                    generalized tip force
                            |
                            v
        +------------------------------------------------+
        | Reduced tapered helical rod                    |
        |                                                |
        | - two orthogonal lateral planes                |
        | - near-degenerate mode pairs                   |
        | - axial/torsional residual modes               |
        | - shared energy-dependent stiffness            |
        | - geometry-derived tip/contact/radiation maps   |
        +-----------+----------------------+-------------+
                    |                      |
        inter-coil contact forces          | base reaction
        projected back into modes          v
                    |              +--------------------+
                    +------------->| compliant mount /  |
                                   | small panel body   |
                                   +----------+---------+
                                              |
                         +--------------------+--------------------+
                         |                                         |
                 distributed wire acceleration              base reaction sound
                         |                                         |
                         +--------------------+--------------------+
                                              |
                                          audio output
```

### 5.2 Design goals

Reference V3 must:

- preserve the successful visual timing and settle behavior
- sound recognizably like a doorstop at soft and medium strikes, even with contact disabled
- allow contact to add hard-strike roughness without defining the entire boing
- generate odd/even lobe differences without manually alternating gains
- create lobe articulation without multiplying the entire body by one scalar AM gate
- derive strike, contact, mount, and radiation from one mechanical state
- remain deterministic for a given specimen seed
- remain safe at 44.1, 48, 88.2, 96, and 192 kHz
- support additive retriggers
- fit comfortably inside Rack’s audio-thread budget

### 5.3 Non-goals

The first production implementation does not need:

- full 3D nonlinear finite-element integration at audio rate
- exact material-science prediction from manufacturer dimensions
- room acoustics or convolution
- a user-facing laboratory of contact coefficients
- visible 3D precession on the panel
- physically exact acoustic radiation from boundary-element analysis

The objective is a reduced model that preserves the perceptually decisive causal relationships.

---

## 6. Structural state

Use mass-normalized generalized coordinates:

\[
\mathbf{q}\in\mathbb{R}^{N}, \qquad
\dot{\mathbf{q}}\in\mathbb{R}^{N}
\]

with the nominal structural equation:

\[
\ddot{\mathbf{q}}
+
\mathbf{C}\dot{\mathbf{q}}
+
\mathbf{K}(\varepsilon)\mathbf{q}
=
\mathbf{B}_{\text{tip}}\mathbf{F}_{\text{tip}}
+
\mathbf{H}^{\mathsf T}\mathbf{F}_{\text{contact}}
+
\mathbf{B}_{\text{mount}}\mathbf{F}_{\text{mount}}
\]

where:

- \(\mathbf{K}\) is modal stiffness
- \(\mathbf{C}\) is frequency-dependent damping
- \(\varepsilon\) is a shared strain-energy proxy
- \(\mathbf{B}_{\text{tip}}\) projects cap force into structural modes
- \(\mathbf{H}\) maps modal displacement to potential contact gaps
- \(\mathbf{B}_{\text{mount}}\) provides optional bidirectional mount coupling

### 6.1 Two-plane lateral mode pairs

The most important new state is not “more modes.” It is **paired lateral motion in two orthogonal planes**.

For each retained lateral family \(p\):

\[
\mathbf{q}_p =
\begin{bmatrix}
q_{p,x}\\q_{p,y}
\end{bmatrix}
\]

Use a small stiffness anisotropy:

\[
\mathbf{K}_p =
\mathbf{R}(\theta_p)
\begin{bmatrix}
\omega_{p,+}^2 & 0\\
0 & \omega_{p,-}^2
\end{bmatrix}
\mathbf{R}(\theta_p)^\mathsf T
\]

with:

\[
\omega_{p,\pm} =
\omega_p(1\pm\delta_p)
\]

A strike not perfectly aligned to one principal axis excites both members. Their small frequency split and different damping produce:

- elliptical tip trajectories
- slow precession
- physically coherent beating
- changing projection toward the microphone
- non-identical adjacent lobes
- specimen-specific asymmetry without arbitrary alternating gain

Helical-spring vibration literature supports coupled wave families, orthogonal transverse motion, torsional/axial coupling, resonance, and beating. The exact doorstop model will differ from any one analytical paper, but a one-coordinate flex oscillator is structurally too impoverished.

### 6.2 Residual torsional and axial modes

Not every retained mode needs a visible lateral pair. The offline model may identify modes dominated by:

- wire torsion
- axial coil deformation
- local high-order bending
- cap motion
- base compliance

These can remain scalar generalized coordinates. They still participate in the same tip, contact, and radiation maps.

### 6.3 Shared nonlinear stiffening

Avoid twelve unrelated energy warps. Define a common strain proxy:

\[
\varepsilon =
\mathbf{q}^{\mathsf T}\mathbf{S}\mathbf{q}
\]

and update:

\[
\mathbf{K}(\varepsilon)
=
\mathbf{K}_0
+
h(\varepsilon)\mathbf{K}_1
\]

where \(h\) is bounded and updated at control rate.

This creates a coherent early tightness and downward relaxation while allowing different modes to respond according to their actual strain participation. The individual mode shifts should be consequences of \(\mathbf{K}_1\), not independent decorative percentages.

---

## 7. Rubber-cap and strike model

### 7.1 Cap as a compliant lumped body

Represent the rubber cap in two dimensions:

\[
m_c\ddot{\mathbf{c}}
=
\mathbf{F}_{\text{strike}}
-
\mathbf{F}_{\text{tip}}
\]

with:

\[
\mathbf{F}_{\text{tip}}
=
k_c(\mathbf{c}-\mathbf{u}_{\text{tip}})
+
d_c(\dot{\mathbf{c}}-\dot{\mathbf{u}}_{\text{tip}})
+
k_{c3}\|\mathbf{c}-\mathbf{u}_{\text{tip}}\|^2
(\mathbf{c}-\mathbf{u}_{\text{tip}})
\]

and:

\[
\mathbf{u}_{\text{tip}}
=
\mathbf{\Phi}_{\text{tip}}\mathbf{q}
\]

This cap state naturally:

- softens the attack
- limits high-frequency transmission on gentle taps
- changes the hard-strike spectrum
- provides a believable low thump
- prevents the initial transient from being a generic noise burst
- transmits force into each mode according to tip participation

A simplified first pass may omit \(k_{c3}\) and use a linear Kelvin–Voigt attachment.

### 7.2 Finite-duration excitation

Use a raised-cosine strike rather than a one-sample velocity jump or white-noise packet:

\[
F(t)=
\frac{F_{\max}}{2}
\left[
1-\cos\left(\frac{2\pi t}{T_{\text{strike}}}\right)
\right]
\]

for \(0\le t\le T_{\text{strike}}\).

Suggested initial range:

- hard flick: \(0.4\)–\(0.9\ \text{ms}\)
- medium strike: \(0.8\)–\(1.6\ \text{ms}\)
- soft tap: \(1.4\)–\(2.5\ \text{ms}\)

The normalized velocity input controls both force magnitude and pulse duration. Harder events should be stronger and generally shorter/brighter.

### 7.3 Strike direction

The panel exposes one signed strike dimension, but the physical model needs a small second-plane component. Derive a deterministic specimen misalignment:

```cpp
Vec2 direction {
    sign,
    specimenOutOfPlaneBias
};
direction = normalize(direction);
```

The bias should be small enough that the visual still reads as a left/right strike, but large enough to excite paired motion. It should be derived from `specimenSeed`, not fresh random noise on every strike.

A later experiment may add tiny deterministic strike-to-strike variation, but it should not be part of the first falsifiable model.

---

## 8. Geometry-derived inter-coil contact

### 8.1 Contact must act on the continuum, not a parallel noise source

For each candidate contact pair \(j\), precompute a modal gap vector \(\mathbf{h}_j\):

\[
g_j =
g_{j,0}
+
\mathbf{h}_j^\mathsf{T}\mathbf{q}
\]

where:

- \(g_{j,0}\) is the resting clearance
- \(g_j>0\) means separated
- \(g_j<0\) means penetration in the reduced geometry

The penetration and closing speed are:

\[
\delta_j=[-g_j]_+
\]

\[
\dot{\delta}_j=
-\mathbf{h}_j^\mathsf{T}\dot{\mathbf{q}}
\]

Use a compliant unilateral force:

\[
F_j =
\max\left(
0,\;
k_j\delta_j^{\alpha}
+
d_j\delta_j^{\alpha}\dot{\delta}_j
\right)
\]

with \(\alpha\) initially in the range \(1.5\)–\(2.2\).

Project the force back into the modes by virtual work:

\[
\mathbf{Q}_{\text{contact}}
=
-\sum_j \mathbf{h}_jF_j
\]

This is a central design rule:

> Do not take a numerical derivative of contact force and inject that derivative into an unrelated high-mode bank.

The contact force itself enters the shared structural equations. Broadband transients then emerge because a localized, rapidly changing force excites all participating structural coordinates.

### 8.2 Contact is optional for the core identity

The no-contact V3 variant must already sound like a doorstop at ordinary strikes.

Contact should contribute:

- hard-strike grit
- occasional asymmetric rattle
- short high-frequency sprays
- stronger specimen variation
- wear-dependent behavior

It should not be required to generate every audible lobe. If contact is the only reason the model becomes recognizable, it is probably being used as an effect rather than as a physical detail.

### 8.3 Contact phase is an empirical question

If synchronized capture shows high-frequency lobe peaks near maximum bend, inter-coil contact or curvature-sensitive radiation becomes more plausible.

If peaks occur near center crossing, base reaction, maximum velocity, and orientation-dependent radiation become more plausible.

Reference V3 supports both without hard-coding either conclusion.

---

## 9. Mount and boundary model

### 9.1 Drive the mount from reaction force

Estimate base reaction from the structural state:

\[
R_{\text{base}}
=
\mathbf{r}_q^\mathsf{T}\mathbf{q}
+
\mathbf{r}_v^\mathsf{T}\dot{\mathbf{q}}
+
\mathbf{r}_a^\mathsf{T}\ddot{\mathbf{q}}
\]

The exact observer coefficients can be exported from the offline model or fitted from the low-order mode shapes.

Drive one or more mount/body resonators with this reaction:

\[
m_b\ddot{b}
+
c_b\dot{b}
+
k_bb
=
R_{\text{base}}
\]

A one-way mount is acceptable for the first V3 prototype: the spring drives the wall/body, but the wall/body does not yet feed back into the spring.

A later refinement may add bidirectional boundary compliance. The key immediate improvement is causal phase-locking: the mount should respond to what the spring is doing, not simply to the initial trigger.

### 9.2 Separate specimen and mounting identity

`specimenSeed` should derive correlated traits for both:

**Spring traits**

- overall stiffness scale
- modal split
- damping tilt
- cap compliance
- contact clearances
- handedness/asymmetry

**Mount traits**

- panel/body resonance frequencies
- mount damping
- spring-to-base transmission
- air/base radiation balance

This lets a single seed describe a coherent “doorstop mounted on this surface,” rather than independently randomizing every resonator.

---

## 10. Radiation model: observation instead of gating

### 10.1 Distributed acceleration observer

At a small set of virtual stations \(s_j\), reconstruct acceleration:

\[
\mathbf{a}_j
=
\mathbf{\Phi}_j\ddot{\mathbf{q}}
\]

Then calculate a near-field radiation proxy:

\[
y_{\text{wire}}
=
\sum_j
w_j
\mathbf{d}_{\text{mic}}^\mathsf{T}
\mathbf{R}_j(\mathbf{q}_{\text{low}})
\mathbf{a}_j
\]

where \(\mathbf{R}_j\) is a low-order orientation update derived from the large lateral state.

This produces time-varying radiation because the physical spring geometry moves. Crucially, it is **mode-specific and sign-sensitive**. It can change phase and spectral balance, rather than simply turning every component up and down together.

A six- or eight-station observer is inexpensive:

- 6–8 station reconstructions
- 16–24 retained coordinates
- roughly 100–200 multiply-adds per sample before optimization

### 10.2 Base and cap observations

The final physical signal is a mixture:

\[
y =
g_{\text{wire}}y_{\text{wire}}
+
g_{\text{base}}y_{\text{base}}
+
g_{\text{cap}}y_{\text{cap}}
\]

where:

- \(y_{\text{wire}}\) is distributed structural acceleration
- \(y_{\text{base}}\) is filtered base reaction / panel response
- \(y_{\text{cap}}\) is a small cap acceleration or transmitted-force component

There is no global `radiationGate`.

### 10.3 Why this can create the boing lobes

The lobe pattern can arise from several coherent mechanisms in the same model:

- low-mode orientation changing the projection of high-mode acceleration
- elliptical or precessing motion from split lateral pairs
- alternating base reaction
- shared strain-dependent stiffness
- intermittent contact on hard strikes
- different directional sensitivity of individual mode shapes

The model is allowed to produce a twice-per-cycle envelope, but it is no longer forced to manufacture one by multiplying a static chord by \(|v|\).

---

## 11. Obtaining the reduced model

### 11.1 Recommended definitive path: offline eigenmodel

Add:

```text
tools/generate_doorstop_rom.py
src/DoorstopHelixROMData.hpp
```

The generator should:

1. Construct a nominal tapered helical wire centerline.
2. Divide it into spatial beam elements.
3. Assign steel density, Young’s modulus, shear modulus, wire radius, pitch, taper, and cap mass.
4. Clamp the base.
5. Assemble mass and stiffness matrices.
6. Solve:

   \[
   \mathbf{K}\boldsymbol{\phi}
   =
   \omega^2\mathbf{M}\boldsymbol{\phi}
   \]

7. Mass-normalize retained eigenvectors.
8. Select modes by:
   - frequency range
   - tip participation
   - base reaction
   - radiation participation
9. Export:
   - mode frequencies
   - damping starting values
   - tip participation
   - station shapes
   - contact-gap maps
   - base-reaction observer
   - mode classifications and pair relationships
10. Write a deterministic `constexpr` header.

The plugin has no SciPy, Eigen, or runtime finite-element dependency. Only the checked-in generated arrays ship.

### 11.2 Prototype path: analytic surrogate

Before building the offline eigenmodel, implement a fast hypothesis test using:

- the current measured mode centers
- analytic cantilever-like lateral shapes
- deterministic pair splitting
- hand-authored but smooth tip and station participation
- 6–8 contact gap vectors
- the new cap, mount-reaction, and radiation topology

Call this analysis variant:

```text
v3-paired-surrogate
```

If the paired surrogate does not outperform V2 in matched listening, do not assume a more elaborate finite-element generator will rescue it. Revisit the phase measurement and radiation observer first.

### 11.3 Do not derive physics from panel pixels

The widget’s turn count and visible length are artistic/rendering values. The ROM should use a nominal physical specimen measured from the intended object or a representative real doorstop.

Only the normalized tip projection should be mapped back into the existing visual coordinate system.

---

## 12. Initial implementation scale and tuning ranges

These are bootstrapping ranges, not claims about a universal doorstop.

| Quantity | Initial target | Search range |
|---|---:|---:|
| Retained structural coordinates | 16–24 | 12–32 |
| Low lateral pair center | 22.0 Hz | 18–26 Hz |
| Low-pair frequency split | 1.0% | 0.3–3.0% |
| Audible lateral pair split | 0.4% | 0.05–1.5% |
| Audible structural band | 180–5000 Hz | 100–8000 Hz |
| Mid-band T60 | 4.5–7.0 s | 2.5–9.0 s |
| Upper-band T60 | 0.8–4.5 s | 0.4–6.0 s |
| Strike duration | 1.1 ms | 0.4–2.5 ms |
| Cap attachment resonance | 90 Hz | 60–180 Hz |
| Contact exponent \(\alpha\) | 1.8 | 1.4–2.4 |
| Nominal contact onset | hard strikes | medium-hard to extreme |
| Radiation stations | 6 | 4–10 |
| Internal substeps | 2× | 1×–4× |
| Control block for stiffness updates | 16 samples | 8–32 |
| Final output ceiling | ±5 V | fixed module contract |

For the surrogate, begin with the current Reference mode centers only as candidate ridge locations. Do not assume every current frequency deserves to survive the geometry-derived model.

---

## 13. Real-time integration

### 13.1 Substepped semi-implicit update

At 48 kHz, use two internal substeps:

\[
h=\frac{1}{2f_s}
\]

For each substep:

1. Evaluate strike force.
2. Reconstruct tip state.
3. Evaluate cap force.
4. Evaluate contact gaps and forces.
5. Calculate generalized modal force.
6. Integrate modal velocities.
7. Integrate modal positions.
8. Integrate cap and mount states.
9. Reconstruct accelerations for radiation.
10. Accumulate the substep output.

A mass-normalized mode update can begin as:

```cpp
modeAcceleration[i] =
    generalizedForce[i]
    - damping[i] * modeVelocity[i]
    - stiffnessScale[i] * omegaSq[i] * modePosition[i];

modeVelocity[i] += h * modeAcceleration[i];
modePosition[i] += h * modeVelocity[i];
```

At two-times oversampling, the highest retained 4–5 kHz modes remain comfortably below the explicit stability boundary. Contact stiffness still requires bounded coefficients and stress testing.

### 13.2 Control-rate work

Every 8–32 output samples:

- update shared stiffness hardening
- update break-in effective coefficients
- update any slowly varying radiation orientation approximation
- update sleep thresholds
- update diagnostics

Avoid per-sample transcendental calls. Frequency, damping, and rotation coefficients should be cached.

### 13.3 Energy accounting

Track:

\[
E_{\text{struct}}
=
\frac{1}{2}\dot{\mathbf{q}}^\mathsf{T}\dot{\mathbf{q}}
+
\frac{1}{2}\mathbf{q}^\mathsf{T}\mathbf{K}\mathbf{q}
\]

plus cap, mount, and contact potential energy.

The energy model should be used to:

- verify damping is non-increasing without external force
- scale **new** strike force when the ceiling is approached
- prevent pathological retrigger accumulation
- derive `Frame.energy`
- aid sleep detection

Do not clamp every mode independently unless recovering from a fault. Independent clamps change the object’s spectral balance and can sound like hidden compression.

### 13.4 Final conditioning

During model development, render both:

- `physical_preconditioned` — before saturation
- `module_output` — after DC removal and output limiting

Reference V2 applies strong drive before `tanh`. That can mask whether an improvement came from the physical model or from nonlinear conditioning.

Reference V3 should initially use:

- a slow DC blocker
- one calibrated linear output gain
- a gentle safety saturator reached only by pathological/retrigger peaks
- the existing ±5 V output contract

---

## 14. Suggested C++ structure

```cpp
namespace doorstop {

class HelicalContinuumEngine {
public:
    void setSampleRate(float sampleRate);
    void setSpecimenSeed(std::uint32_t seed);
    void setBreakIn(float amount);
    void setBreakInLocked(bool locked);

    void reset();
    void resetMotion();
    void restoreFactoryFresh();

    void strike(float normalizedVelocity);
    Frame process(float sampleTime);

    bool isSleeping() const;
    float getVisualMaximumDisplacement() const;

#if defined(DOORSTOP_REFERENCE_ANALYSIS)
    void setAnalysisVariant(HelicalAnalysisVariant variant);
    const HelicalDiagnostics& getDiagnostics() const;
#endif

private:
    static constexpr int MODE_COUNT = 20;
    static constexpr int CONTACT_COUNT = 8;
    static constexpr int RADIATION_STATION_COUNT = 6;
    static constexpr int SUBSTEPS = 2;

    struct ModalState {
        float position = 0.f;
        float velocity = 0.f;
        float acceleration = 0.f;
    };

    std::array<ModalState, MODE_COUNT> modes {};

    Vec2 capPosition {};
    Vec2 capVelocity {};
    Vec2 mountPosition {};
    Vec2 mountVelocity {};

    StrikePulse strikePulse {};
    EffectiveTuning tuning {};
    HelicalDiagnostics diagnostics {};

    float breakIn = 0.f;
    bool breakInLocked = false;
    std::uint32_t specimenSeed = 1u;

    void updateEffectiveTuning();
    void beginStrikePulse(float normalizedVelocity);
    void processSubstep(float h);
    Vec2 reconstructTipPosition() const;
    Vec2 reconstructTipVelocity() const;
    void evaluateContactForces(
        std::array<float, MODE_COUNT>& generalizedForce);
    float evaluateRadiation() const;
    float calculateEnergy() const;
    bool allFinite() const;
};

} // namespace doorstop
```

The actual vector type should use an existing lightweight Leviathan/Rack type or a tiny local POD, not heap-allocated math objects.

---

## 15. Analysis variants required before production

Reference V3 should be developed as one engine with compile-time analysis variants:

| Variant | Purpose |
|---|---|
| `v3-paired-dry` | paired structural ROM, cap, no contact, wire radiation only |
| `v3-paired-base` | add reaction-driven mount/body |
| `v3-paired-contact` | add deterministic contact |
| `v3-no-pairs` | collapse each pair to one plane; falsifies the 2D hypothesis |
| `v3-fixed-observer` | disable geometry-varying radiation |
| `v3-velocity-observer` | observer biased toward center-crossing/base reaction |
| `v3-curvature-observer` | observer biased toward maximum bend/contact |
| `v3-full` | intended production candidate |
| `v3-linear-output` | full mechanics before final saturation |

These variants are more informative than another dozen arbitrary frequency/gain presets. Each one removes a physical mechanism and asks whether recognizability disappears.

---

## 16. Low-risk Reference V2.1 rescue experiments

Reference V3 is the recommended architecture, but several existing changes are worth testing because they are inexpensive and may reveal exactly which assumption is wrong.

### 16.1 Sweep radiation phase before rewriting the engine

Add an analysis-only phase parameter:

\[
r_\phi(t)=
\left|
\cos(\phi)\,x_n(t)
+
\sin(\phi)\,v_n(t)
\right|
\]

Render:

```text
v2-phase-000
v2-phase-015
v2-phase-030
v2-phase-045
v2-phase-060
v2-phase-075
v2-phase-090
```

where:

- \(0^\circ\) is displacement/extrema dominated
- \(90^\circ\) is velocity/crossing dominated

RMS-match the outputs. If one phase region suddenly sounds much more physical, the current failure is at least partly geometric timing rather than modal content.

This is a diagnostic shortcut, not the final V3 radiation implementation.

### 16.2 Promote the existing `BoingRefined` behavior into a production candidate

The repository already contains an analysis-only refinement with:

- broader radiation shoulders
- reduced initial impact level
- stronger correlated pitch motion
- an energy-correlated texture-delay sweep

Production `DarkRefinedV2` does not enable the delay sweep and continues using the narrower squared gate. Create `ReferenceV2_1` or an analysis profile that combines the complete `BoingRefined` path with production serialization.

### 16.3 Replace random impact with a deterministic force pulse

Without changing the full V2 architecture:

- create a 0.4–2.5 ms raised-cosine pulse
- feed it through one or two compliant low-pass states
- use the result as modal excitation
- retain at most 1–5% seeded roughness

This should make the attack read as “one object was struck,” rather than “noise and resonators began simultaneously.”

### 16.4 Replace the independent mount kick

Drive `mountVelocity` or a new mount resonator from a spring-reaction proxy:

```cpp
float reaction =
    reactionFromFlex * acceleration
    + reactionFromModes * modalAccelerationSum;
```

Do not directly kick the mount in `strike()`.

### 16.5 Add paired copies of selected modes

As a V2-compatible experiment, duplicate 4–6 important resonances:

```cpp
fA = fCenter * (1.f - split);
fB = fCenter * (1.f + split);
```

Excite and observe them with two seeded orientation vectors. This will not make V2 a unified continuum, but it directly tests whether two-plane beating/precession is a missing perceptual cue.

### 16.6 Audition the final drive independently

Reference V2 uses a high output drive before `tanh`. Render drive values such as:

```text
3.0, 5.0, 7.0, 10.5
```

and RMS-match externally. The most authentic physical model may not be the one that survives the most aggressive saturation.

### 16.7 Do not restore the old junction unchanged

The existing junction was correctly removed from V2 after failing to move the corpus metrics. If V2 is recoupled, use:

- acceleration-based force transfer
- spatial participation coefficients
- energy accounting
- reaction-driven mount coupling

Do not simply turn the previous alternating mode-shape junction back on.

---

## 17. Validation plan

### 17.1 Continue using the full corpus

The manifest currently represents six separate recordings with many manually reviewed hits. Use population ranges, not only the clean `81458` specimen.

The clean single hit is valuable for detailed ridge and lobe analysis, but a model tuned only to it risks becoming “that recording synthesizer” rather than a general spring-doorstop model.

### 17.2 Extend the feature set

Add these material and motion features:

#### Phase-aware motion features

- high-band lobe phase relative to model tip displacement
- high-band lobe phase relative to model tip velocity
- phase drift over the decay
- lobe-width evolution
- odd/even lobe spectral correlation
- skip-one versus adjacent lobe correlation

#### Continuity features

- inter-lobe trough / lobe-peak ratio in:
  - 180–500 Hz
  - 500–1200 Hz
  - 1200–3000 Hz
- percentage of frames with persistent ridge energy
- number of modes remaining coherent after 1 s and 2 s

#### Attack features

- 0–5 ms crest factor
- 0–20 ms spectral centroid
- attack-to-sustain spectral distance
- high-band noise-like flatness
- time to first structural lobe

#### Structural evolution

- ridge-frequency descent by band
- pair beating / sideband spacing
- base-reaction-to-wire-energy ratio
- contact event count and total contact impulse
- visible full-cycle frequency

### 17.3 Add diagnostic stems

The render harness should optionally write:

```text
full.wav
wire.wav
base.wav
cap.wav
contact.wav
low-pair.wav
audible-modes.wav
preconditioned.wav
```

The model should sound increasingly like the final object as coherent layers are added. If `contact.wav` or `base.wav` is the only recognizable part, the continuum is not yet doing its job.

### 17.4 Listening protocol

For each candidate:

- same source strike velocity
- same specimen seed
- exact group RMS matching
- hidden filenames/order
- compare dark, balanced, and bright fixtures
- include soft, medium, and hard strikes
- include retriggers
- compare both short excerpts and full decays

The listening question should not merely be “which sounds nicer?” Use:

1. Which sounds most like an ordinary spring doorstop?
2. Which sounds most like one object rather than layered synthesis?
3. Which has the most believable relation between initial hit and later motion?
4. Which remains recognizable at a soft strike?
5. Which hard-strike roughness sounds caused by the object rather than added as noise?

### 17.5 Production acceptance criteria

Reference V3 is ready to become the default only when:

- it remains recognizably doorstop-like with contact disabled at medium velocity
- enabling contact improves hard hits without adding periodic ticks at medium velocity
- paired motion wins against the `v3-no-pairs` ablation
- the geometry-varying observer wins against `v3-fixed-observer`
- the mount is audibly useful when driven by reaction, but does not dominate every specimen
- its lobe rate and decay fall within the corpus corridors
- inter-lobe high-band energy does not collapse unnaturally
- it wins blind matched listening against Reference V1, Reference V2, and the best Legacy model
- CPU and safety tests pass at all supported sample rates

---

## 18. Unit and stress tests

Add `tests/doorstop_helical_engine_spec.cpp`.

### 18.1 Mechanical invariants

- zero state remains zero without forcing
- unforced energy is non-increasing within numerical tolerance
- contact force is zero for positive gap
- contact force never pulls surfaces together
- cap attachment force is equal and opposite
- specimen seed produces deterministic coefficients
- break-in changes coefficients without state drift
- `restoreFactoryFresh()` restores exact factory coefficients

### 18.2 Audio safety

- finite output for 20 s after maximum strike
- finite output under repeated 100 Hz triggers
- finite output under alternating ±1 velocity
- output remains within ±5.0001 V
- no NaN after sample-rate changes while ringing
- clean recovery from injected non-finite state
- sleep entered only after all structural, cap, mount, and contact states are quiet

### 18.3 Sample-rate consistency

At 44.1, 48, 96, and 192 kHz:

- low-pair frequency within tolerance
- selected modal peaks within tolerance
- T20/T60 within tolerance
- contact onset velocity within tolerance
- spectral centroid within a broad normalized corridor
- no material change caused solely by sample rate

### 18.4 Router and serialization

Add:

```cpp
EngineMode::ReferenceV3
```

with serialized name:

```text
referenceV3
```

Verify:

- old patches retain their existing engine
- unknown strings still fall back safely
- V3 state receives seed, break-in, lock, sample rate, and reset calls
- steady V3 routing equals direct V3 engine output exactly
- transitions among V1, V2, V3, and Legacy remain bounded
- V3 does not reset merely because another engine was selected and later revisited, unless that is the router’s established policy

---

## 19. Repository integration

### 19.1 New files

```text
src/HelicalContinuumEngine.hpp
src/HelicalContinuumEngine.cpp
src/DoorstopHelixROMData.hpp

tests/doorstop_helical_engine_spec.cpp

tools/generate_doorstop_rom.py
tools/analyze_doorstop_motion_phase.py
```

### 19.2 Modified files

```text
src/DoorstopEngineRouter.hpp
src/DoorstopEngineRouter.cpp
src/Doorstop.cpp
src/DoorstopWidget.cpp
tools/doorstop_reference_render.cpp
tools/compare_doorstop_variants.py
Makefile / test build definitions as applicable
```

### 19.3 Router refactor note

The current router helper treats every non-Legacy reference mode as one of two `ReferenceSpringEngine` instances. Adding a third engine class requires replacing that binary helper with explicit per-mode dispatch.

Prefer small private dispatch methods:

```cpp
Frame processEngine(EngineMode mode, float sampleTime);
void strikeEngine(EngineMode mode, float velocity);
void resetEngineMotion(EngineMode mode);
void applyConditionTo(EngineMode mode);
bool engineIsSleeping(EngineMode mode) const;
```

This avoids forcing unrelated engines behind inheritance or virtual dispatch in the audio path.

### 19.4 Context-menu naming

Recommended display:

```text
Engine
  Reference V3 — Helical Continuum
  Reference V2 — Dark Refined
  Reference V1 — Articulated Modal
  Legacy
```

Keep V3 opt-in until it wins the listening decision.

---

## 20. Implementation order

### Stage 0 — Resolve the phase ambiguity

1. Add V2 radiation-phase sweep.
2. Capture synchronized real motion/audio.
3. Determine whether lobe energy aligns with crossing, extrema, or a mixed/variable phase.
4. Render V2 pre- and post-saturation.

**Exit condition:** the team can state what mechanical phase the real lobe envelope follows, or can show that it varies by band/specimen.

### Stage 1 — Paired surrogate, no contact

1. Implement two-plane low motion.
2. Add paired audible modes.
3. Add compliant cap and finite force pulse.
4. Add fixed distributed acceleration observer.
5. Preserve existing `Frame` visual output.
6. Render `v3-paired-dry` and `v3-no-pairs`.

**Exit condition:** paired motion has an audible advantage under blind matched comparison.

### Stage 2 — Geometry-varying radiation and base reaction

1. Add low-state-dependent station orientation.
2. Add base reaction observer.
3. Drive the mount/body from reaction.
4. Compare crossing-biased and curvature-biased observers.

**Exit condition:** lobe articulation emerges without a global AM gate and tracks the measured phase.

### Stage 3 — Contact

1. Add 4–8 modal gap vectors.
2. Implement compliant unilateral force.
3. Tune contact to appear mainly on hard strikes.
4. Add contact diagnostics and stems.

**Exit condition:** hard strikes improve while medium strikes remain free of periodic ticking.

### Stage 4 — Offline ROM

1. Build nominal tapered helix eigenmodel.
2. Export mode shapes and participation arrays.
3. Replace surrogate coefficients.
4. Refit only global scales, damping, and observation mix.
5. Re-run full corpus and listening suite.

**Exit condition:** geometry-derived V3 is at least as recognizable as the surrogate and more robust across specimens/velocities.

### Stage 5 — Production hardening

1. sample-rate tests
2. retrigger stress
3. CPU benchmarking
4. serialization
5. router transitions
6. break-in mapping
7. final gain and limiter calibration

---

## 21. Decision matrix

| Path | Effort | Chance of incremental improvement | Chance of definitive convergence | Main risk |
|---|---:|---:|---:|---|
| Retune current V2 frequencies/gains | Low | Medium | Low | another attractive synthetic twang |
| V2 phase sweep + `BoingRefined` production path | Low | High diagnostic value | Low–medium | still a gated parallel architecture |
| Scalar clash + modal-bank convergence model | Medium | Medium | Medium | contact remains a side-chain effect |
| Paired helical surrogate ROM | Medium | High | High | observer/phase may still be wrong |
| Geometry-derived helical ROM + contact | High | High | Highest | offline modeling and calibration effort |
| Full nonlinear coil-by-coil runtime FEM | Very high | Unknown | Not proportionate | stability, CPU, tuning complexity |

The recommended path is:

> **Run the V2 phase probe immediately, then implement the paired surrogate as Reference V3. Add contact only after paired motion and physical radiation are proven. Replace surrogate shapes with an offline geometry-derived ROM only after the topology wins by ear.**

---

## 22. What would falsify this proposal?

Reference V3 should be abandoned or substantially revised if:

- `v3-no-pairs` sounds equally physical after strict RMS matching
- fixed and geometry-varying radiation observers are indistinguishable
- synchronized capture shows no stable relationship between motion phase and lobe energy
- contact is required at every velocity to make the model recognizable
- the base-reaction component dominates identity so strongly that the spring ROM contributes little
- the analytic surrogate loses badly to V2 even after the phase and output-conditioning variables are controlled
- the geometry-derived ROM produces a more accurate spectrum but a less recognizable object

A failed V3 experiment would still be useful. It would identify which presumed physical cue is not perceptually decisive and prevent further accumulation of plausible-but-independent subsystems.

---

## 23. Final recommendation

The current Doorstop engines have explored the obvious one-dimensional reductions thoroughly. More modal tuning is unlikely to create a definitive result because the main missing information is not another frequency—it is **how a spatial helical object stores, transfers, and radiates energy**.

Reference V3 should introduce three structural ideas at once:

1. **two-plane paired motion**
2. **forces projected through shared geometry**
3. **audio observed from acceleration and reaction rather than a global gate**

The rubber cap, mount, contact, visual state, and metallic body then become parts of one network.

The shortest meaningful experiment is not the full finite-element generator. It is:

- measure real lobe phase
- build the paired surrogate
- remove the scalar radiation gate
- derive the output from distributed acceleration and base reaction
- compare it blind against V2

That experiment can tell whether the project has finally crossed from “very sophisticated boing synthesizer” into “this is a doorstop.”

---

## Appendix A — Source map

### Leviathan repository

- `src/Doorstop.cpp`
- `src/Doorstop.hpp`
- `src/DoorstopEngine.cpp`
- `src/DoorstopEngine.hpp`
- `src/DoorstopEngineRouter.cpp`
- `src/DoorstopEngineRouter.hpp`
- `src/ReferenceSpringEngine.cpp`
- `src/ReferenceSpringEngine.hpp`
- `src/DoorstopWidget.cpp`
- `tests/doorstop_engine_spec.cpp`
- `tests/doorstop_reference_engine_spec.cpp`
- `tools/analyze_doorstop_reference.py`
- `tools/audit_doorstop_corpus.py`
- `tools/compare_doorstop_variants.py`
- `tools/doorstop_reference_manifest.json`
- `tools/doorstop_reference_render.cpp`
- `doc/ReferenceSpring.md`
- `doc/ReferenceSpringImplementation.md`
- `doc/dr-spring1.md`
- `doc/dr-spring2.md`
- `doc/ds-gem.md`
- `doc/DS_Break-in.md`

Repository branch:

https://github.com/PlasmaChroma/Leviathan-Rack2/tree/expander

### Physical-modeling references

A. Hamza, M. S. Abbes, T. Fakhfakh, and M. Haddar, “The natural frequencies of waves in helical springs,” *Comptes Rendus Mécanique*, 341 (2013), 672–686.

https://doi.org/10.1016/j.crme.2013.09.006

S. Bilbao, M. Ducceschi, and C. Webb, “Large-scale real-time Modular Physical Modeling Sound Synthesis,” Proceedings of DAFx-19, 2019.

https://dafx.de/paper-archive/2019/DAFx2019_paper_22.pdf

V. Yıldırım, “Axial Static Load Dependence Free Vibration Analysis of Helical Springs Based on the Theory of Spatially Curved Bars,” *Latin American Journal of Solids and Structures*, 13(15), 2016.

https://doi.org/10.1590/1679-78253123

K. Michalczyk, “Analysis of Lateral Vibrations of the Axially Loaded Helical Spring,” *Journal of Theoretical and Applied Mechanics*, 53(3), 2015.

https://doi.org/10.15632/jtam-pl.53.3.745
