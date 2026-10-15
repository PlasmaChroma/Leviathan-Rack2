# Doorstop V4 — Closed-Coil Contact Helix
## Research findings, mechanical design, and implementation handoff

**Project:** Leviathan-Rack2 / Doorstop  
**Reviewed branch:** `expander`  
**Reviewed source commit:** `2ac867729767c3e8691a371d2f914ecfb07d7326`  
**Review date:** September 26, 2026  
**Proposed engine:** `ContactHelixEngine` / `EngineMode::ReferenceV4`  
**Suggested menu label:** `Reference V4 — Closed-coil spring (experimental)`

**Evidence status.** This is a source review and a proposed design, not an implemented or auditioned engine. The local checkout at the commit above, its Makefile, and earlier project design documents were inspected. The inspected tracked sources match that commit; this design document is currently untracked. The plugin was not compiled and the reference recordings were not auditioned in this review. Source links below are commit-pinned. Record the implementation commit, any uncommitted source-diff hash, and generated model-data hash for subsequent renders. Proposed state counts, numerical tolerances, and search ranges below are engineering starting points, not measurements of a universal doorstop.

**Scope of this revision.** Retain the contact-helix hypothesis. Clarify prestress, contact quadrature and discrete work, make the fixed-contact comparison well-defined, and put runtime feasibility ahead of expensive acoustic fitting. This document specifies future implementation work; this review changes documentation only.

---

## 1. Decision

Build a new, opt-in **closed-coil, contact-aware nonlinear helical reduced-order model**. Preserve V2 Dark Refined, V3 Deep Swing, and V3 Stronger Body Bending as listening controls. Preserve the existing spring renderer, cap orientation, trails, and overflow behavior.

The important architectural change is not more modes, a darker frequency table, or a stronger pitch envelope. It is this:

> The spring's neighboring turns can touch at rest, open on part of their circumference while bending, and recontact as the deformation changes. Those changing constraints must act on the same mechanical state that produces both visible bending and audible vibration.

**Working hypothesis:** the present models approximate the timbre and motion separately enough that they can approach a boing without reproducing the changing constraints of a tightly wound spring. A model that preserves those constraints has a credible route to the missing repeated, flexing boing. That is a hypothesis to test, not a claim that coil impacts are the sole cause of doorstop sound.

The new engine must retain distributed wire vibration and geometric coupling. Contact is not a replacement for dispersion, a rubber cap, or the mounting boundary. Nor is it permission to add a train of synthetic clicks.

### What success means

At the module's ordinary trigger strength, a single excitation should produce a recognizable sequence of flexing boings with a metallic twang attached to the motion. Harder excitation should deepen or otherwise change that sequence in the direction supported by the target specimen. The result must not depend on an unrelated amplitude gate, a dominant limiter, or another strike being injected at every zero crossing.

The final authority is a level-matched comparison with a selected physical doorstop recording, not the sophistication of the equations.

## 2. Research basis and its limits

### 2.1 A doorstop is not necessarily an open compression spring

The earlier review attributes a construction with abutting wire convolutions, a tapered mounting end, and a resilient bumper tip to Fisher's doorstop patent. The patent text could not be independently retrieved in this revision, so that attribution remains provisional. Even if confirmed, it would establish one closed-coil construction, not the preload, dimensions, or acoustic behavior of every modern specimen. [R1]

**Design implication:** use a target with measured resting gaps. Permit contact at ordinary excitation. Do not import a requirement that contact must be absent at medium strikes.

### 2.2 Contact changes mechanics, not just attack noise

Holmes and colleagues' Slinky model shows why intercoil contact matters to the equilibrium shapes and stability of a highly flexible helix. It treats axial, shear, and rotational deformation through an energy formulation. This is static/quasi-static research on a different spring, not an acoustic validation of a doorstop. [R2]

Zhao and colleagues' nonlinear valve-spring study reports agreement between beam-based simulations and dynamic tests, including coil clash and associated force spikes. Its geometry, mounting, and excitation differ substantially from a flicked doorstop. It supports including contact in the mechanics, not transplanting its coefficients or impact timing. [R3]

**Design inference:** a closed-coil doorstop deserves a model in which separation and renewed contact change effective compliance and transfer energy into wire vibration. It is not enough to mix in an impact sample when a scalar bend threshold is exceeded.

### 2.3 Preserve helical wave behavior

Hamza, Ayadi, and Hadj-Taïeb analyze interacting axial and angular waves, reflections, and beating in axially excited helical compression springs. Their model is not a complete laterally flicked doorstop model, but it reinforces the need to retain coupled distributed dynamics. [R4]

**Design implication:** do not replace the wire with a single low oscillator and one resonant filter. A reduced model is appropriate only when its retained dynamics reproduce the relevant impulse responses and deformation-dependent behavior.

### 2.4 Derive nonlinear forces from energy

Ilijić and colleagues derive helical-spring elastic energy accounting for translation and rotation of the ends, and compare their predictions with steel-spring measurements. This supplies additional support for treating end conditions and deformation together rather than applying an unexplained pitch envelope. It does not specify the present contact model. [R5]

Bilbao, Ducceschi, and Webb provide a sound-synthesis precedent for interconnected mechanical systems with nonlinear potential-based interactions and explicit energy/stability analysis. Their particular numerical construction is not automatically a drop-in integrator for an arbitrary nonlinear helix. [R6]

**Design implication:** forces, contact geometry, and energy accounting must agree. Positive stiffness alone is not proof of passivity.

### 2.5 What the research does not establish

No source reviewed here directly measures how much of the desired doorstop's boing comes from contact, geometric pitch change, wave propagation, acoustic radiation, or the mounting surface. The proposed model keeps these mechanisms distinguishable. Do not hard-code a universal boing rate, universal downward pitch sweep, or universal phase relationship between sound maxima and visible position.

## 3. Current branch: relevant observations

The inspected V3 implementation uses twelve mode pairs, authored modal coefficients, deformation-dependent stiffness multipliers, and phase-dependent observation gains. Contact is reconstructed from a short modal sum; its closing speed and force projection use different participation assumptions. This is not a spatial collection of adjacent-turn gaps. The V3 visual displacement also depends on an envelope derived from final audio. [C1, C2]

V2 combines modal, flex, impact, mount, and dispersive components with a motion-related radiation gate. It remains a valuable perceptual baseline, not a topology to copy wholesale. [C3]

Two comparison issues are confirmed in the local sources: completed V3 pulse magnitude is retained and accumulated by subsequent strikes before sleep/reset clears it, and the renderer's preconditioned-output option configures the reference engine rather than the helical engine. Repair or explicitly isolate these before comparative renders. Preserve pre-fix fixtures; the pulse repair intentionally changes some retriggers, so it cannot also promise unchanged retrigger audio. [C1, C4]

The renderer sets initial break-in but does not lock it, so repeated strikes can change effective wear. Its V3 audition Makefile target includes Deep Swing but omits `v3-deep-body-bend`. Add explicit wear locking and the missing preferred baseline to the comparison harness. The current renderer divides output by 5 before writing float WAV; retain that documented module-voltage conversion and give new raw channels their own declared scales. [C4, C9]

The module uses 0.5 normalized velocity when no velocity cable is attached. Make that a principal acceptance case, not merely full-strength strikes. [C5]

The earlier September review already identified observer masking, stationary tails, contact reciprocity, and incomplete energy diagnostics. This proposal changes the physical contact topology rather than repeating that observer-retuning plan. [P1]

## 4. Non-negotiable design constraints

1. Existing engine identifiers, patches, and preferred baseline voices remain available. Do not silently reinterpret a V3 variant as V4.
2. Use one mechanical state for slow bending, wire motion, cap loading, contact, and the mechanically coupled mount.
3. Derive each contact's gap, relative velocity, and generalized force from the same geometry.
4. Keep the wire permanently connected. Neighboring turns making contact adds a constraint; it does not create or destroy the wire's material connectivity.
5. No repeated synthetic impulses synchronized to bend crossings. No free-running modulation oscillator. No strike-history amplitude accumulation masquerading as stored energy.
6. Audio observation and visual mapping may be calibrated independently, but neither may modify mechanics merely to improve its display or loudness.
7. No full finite-element solve, geometry rebuild, file access, allocation, or unbounded iteration in the audio callback.
8. Do not promote V4 until it wins a listening comparison at ordinary as well as strong excitation.

## 5. Physical target and reference capture

### 5.1 Pick one target before fitting a family

The current corpus manifest lists six recordings and onset windows. Use it for comparison, but select a specific thick-spring target subgroup first; do not fit an average of unlike doorstops. [C6]

For a physical specimen, record wire diameter, coil-radius/taper profile, turn count, resting pitch/gaps, free length, tip mass, mounting arrangement, and orientation. Measure the force required to begin opening the coils where possible. Do not infer nonzero initial tension solely from turns touching at rest.

A mounting screw, base socket, rubber tip, and surrounding panel can change boundary conditions. Treat them as part of the target definition. Add base/socket contact only if the target actually exhibits it; that is a separate interaction from turn-to-turn contact.

### 5.2 Minimum useful capture matrix

Capture several releases at low, ordinary, and strong excitation, in both lateral directions, with repeatable mounting. Include a small wire tap while the tip is held at several deflections, then released. Those held-bend responses are particularly useful for distinguishing changing resonance structure from amplitude modulation.

Record dry audio without automatic gain control, compression, or normalization. Use synchronized video with enough temporal resolution for the measured bend rate; choose the frame rate after a pilot capture. Aim for at least roughly twenty frames per fastest bend cycle when estimating phase, and explicitly assess exposure blur, rolling shutter, and audio/video offset. A twice-per-cycle sound envelope does not by itself distinguish maxima at center crossing from maxima at turning points.

Compare the normal mount with a more strongly damped mounting condition only as a diagnostic. Do not treat the damped condition as the same physical target.

### 5.3 Work without new recordings

Implementation can begin with the existing corpus and an explicitly synthetic nominal geometry. Missing dimensions must be labeled `estimated` or `unknown` in the model manifest. Do not ship generated coefficients as measured data. New synchronized capture is required before making strong claims about within-cycle acoustic phase.

## 6. Model architecture

### 6.1 Offline reference, reduced runtime

Use a three-dimensional curved-wire reference model with bending, twist, axial deformation, appropriate shear treatment, and neighboring-turn contact. A co-rotational Timoshenko-beam construction is the proposed offline reference. Validate its straight-beam, curved-wire, static compliance, rigid-motion, and mesh-convergence tests before using its results for audio.

The runtime engine uses a compact, fixed-coordinate reduction of this reference. It does not run the full beam mesh.

A starting investigation budget is **32–64 spring coordinates**, plus explicit cap and mount coordinates. This is a sizing hypothesis, not a minimum or a promise of real-time feasibility. Compare it with a higher-order offline reduction and retain enough contact-deformation response and audible bandwidth to converge. More coordinates may be necessary for accuracy; if their measured cost exceeds the runtime budget, revisit the reduction or stop rather than concealing missing compliance with impact gain. Benchmark the candidate solve during Stage B, before target fitting.

Keep near-degenerate bending partners when they actually occur. Do not force every retained mode into an invented split pair.

### 6.2 Shared state

Let `q` contain spring, cap, and mount generalized coordinates, and let `v = dq/dt`. For the fixed-mass runtime approximation:

```text
M * dv/dt + D(q,v) * v + grad U(q) = B(q) * F_external(t)
```

`M` is symmetric positive definite after constrained degrees of freedom are eliminated. Require `v^T D(q,v) v >= 0`. `U` includes the wire's elastic energy, cap/mount attachments, conservative contact, and conservative static loading such as gravity. Account for a load either in `U` or as external work, never both. Use one documented set of physical units, then mass-normalize internally if convenient. Export the complete mapping between internal and physical coordinates.

Define the reference equilibrium `q_eq` explicitly. Runtime coordinates may be offsets from it, but zero offsets must reconstruct that loaded geometry, including prestress. Positive definiteness of `M` and the residual stiffness alone does not establish stability of the assembled equilibrium; check the full constrained tangent where it exists and directional restoring behavior at unilateral-contact transitions.

A fixed-coordinate reduced approximation is intentional. If a later implementation uses a nonlinear coordinate reconstruction or rotating frame that induces configuration-dependent inertia, include the resulting mass and inertial terms. An energy proof for constant `M` cannot be reused after silently changing that assumption.

### 6.3 Useful runtime partition

Partition spring coordinates into low bending/configuration coordinates `l` and audible residual coordinates `z`. This is a numerical partition, not two independently triggered sound generators.

A concrete energy-based approximation to investigate is:

```text
w = z - z_eq(l)
U_wire(l,z) = V_min(l) + 0.5 * transpose(w) * K_body(l) * w
```

Here `V_min`, `z_eq`, and symmetric `K_body` are fitted to the wire-only reference within a declared displacement domain. `z_eq(l)` represents equilibrium shifts of the retained coordinates; it is not a separate animation path. Ensure `K_body(l)` remains positive definite in the valid domain and `V_min` is bounded below and supplies the required restoring behavior there. Preserve the wire-only gradient at the assembled equilibrium: it can be nonzero because it balances preload/contact. Centering coordinates on `q_eq` does not justify dropping that prestress force or its tangent terms.

For a scalar low coordinate `l_a`, the corresponding derivatives are:

```text
dU/dz   = K_body * w

dU/dl_a = dV_min/dl_a
          + 0.5 * transpose(w) * (dK_body/dl_a) * w
          - transpose(dz_eq/dl_a) * K_body * w
```

Use the negative of these gradients as forces. The low coordinate therefore feels the reaction associated with bending the audible subsystem. Omitting the last two terms would turn `l` back into an unaccounted frequency-modulation source.

Fit energy and derivatives together. Validate force and tangent-stiffness errors on held-out configurations. Do not fit a frequency lookup table alone. A more direct reduced beam-energy evaluation is the offline truth model and a fallback if this approximation is inadequate.

### 6.4 Avoid double-counting contact stiffness

The `U_wire` fit must not already contain the same interturn penalty stiffness that is then added explicitly at runtime. Use wire-only energy contributions when fitting this term. Contact-inclusive snapshots may inform the basis, but separate component energies when producing reduced coefficients.

Also avoid double-counting tip mass: export the bare wire reduction and add the explicit cap once, or export an assembled model and omit the corresponding extra runtime inertia. The proposed first implementation uses the bare wire plus an explicit cap.

## 7. Spatial contact: the defining V4 feature

### 7.1 Geometry and candidates

Construct the rest wire centerline from the target radius/taper and pitch profile. Generate candidate contacts between nearby portions of adjacent turns. Exclude topologically adjacent segments on the same uninterrupted piece of wire from self-collision testing. For a tapered helix, do not assume that identical angular coordinates on adjacent turns are always the closest points.

The reference model should evaluate all relevant neighboring-turn candidates. The runtime reduction may use a reduced set of contact patches with fixed nonnegative quadrature weights, selected against reference force, compliance, and audio-band response errors. Contact count is not a free cosmetic quality setting: too few patches can change mechanics.

Give every physical pair a unique identity so the same interaction is not counted from both sides. Declare whether patch weights represent physical integration measure or dimensionless reduction weights, and give `k_j` and `c_j` compatible units. With the unweighted patch laws below, use `U_contact = sum_j a_j * Phi_j` and `Q_contact = sum_j a_j * J_j^T * F_j`, including the same `a_j >= 0` in damping work and diagnostics. Weights must not vary with the active set without accounting for their potential derivatives/work.

Validate candidate coverage over the entire declared bend/twist domain, including closest-point migration. If non-neighboring turns can collide there, include them or reduce that domain. Fixed capacity does not justify selecting only the rest-state closest pairs and missing later contacts.

Keep candidate capacity fixed at runtime. Do not silently discard contacts when many turns close simultaneously. A capacity failure is a diagnostic failure requiring a larger representation or a revised reduction.

### 7.2 Consistent gap convention

For contact `j`, define:

```text
g_j(q)       > 0 : separated
g_j(q)       = 0 : touching
g_j(q)       < 0 : compliant penetration
J_j(q)           = d g_j / d q
v_gap_j          = J_j * v
delta_j          = max(-g_j, 0)
```

A positive contact force opens the gap. Its generalized force is:

```text
Q_contact_j = transpose(J_j) * F_j
```

This sign follows directly from the gap convention. Use the same Jacobian for gap velocity and force projection. A force at a physical contact acts equally and oppositely on the two wire locations before reduction; modal forces follow by projection.

For two reconstructed wire points with separation vector `r(q)` and combined contact radius `d`, a simple patch approximation is:

```text
g(q) = norm(r(q)) - d
J(q) = transpose(r / norm(r)) * d r / d q
```

Use segment/patch closest-point geometry in the reference. Resolve degeneracies and changes of closest feature without discontinuous force jumps. Do not use arbitrary unrelated participation vectors for receiving high modes.

### 7.3 Conservative normal force and passive damping

A first compliant law is:

```text
Phi_j(delta) = k_j / (p_j + 1) * delta^(p_j + 1)
F_elastic_j  = k_j * delta^p_j
F_damping_j  = c_j * delta^p_j * max(-v_gap_j, 0)
F_j          = F_elastic_j + F_damping_j
```

Apply it only where `delta > 0`, with `k > 0`, `c >= 0`, and `p >= 1`. For the elastic term, include `Phi_j` in `U`; do not apply that force twice.

The extra damper cannot attract separated surfaces. During closure its power is:

```text
P_damping = F_damping * v_gap <= 0
```

A piecewise-linear contact spring (`p = 1`) is a useful first diagnostic. A smoother nonlinear law can be fitted later. Do not call `p = 1.5` an exact Hertz law for every possible curved-wire contact geometry. Contact compliance is an effective reduced parameter until validated.

Use a potential-consistent smoothing near zero if needed; differentiating the smoothed potential must produce the implemented elastic force. Do not add an independent smoothing filter to contact force and continue claiming exact work balance.

### 7.4 Resting contact and preload

Solve the assembled static equilibrium before generating runtime initial conditions. Store the equilibrium configuration and equilibrium contact loads. At rest, internal wire forces, contact forces, mounting reactions, and modeled static loads must balance.

Touching does not imply a universal positive preload. Support zero-load touching and measured initial tension. A compliant preload may use a small equilibrium penetration, but it must be balanced by the wire's prestress; inserting a negative rest gap alone would cause an artificial startup explosion.

As the spring bends, different contact patches may unload, open, or close. The closed set is not assumed to remain fixed, and its changes are not scheduled from an oscillator phase.

### 7.5 Friction is a later, isolated addition

Start with normal compliance and damping. Add interturn friction only after normal-contact comparisons show a benefit or recordings clearly require it.

A regularized sliding law can use:

```text
F_t = -mu * F_n * tanh(v_t / v_epsilon)
```

with consistent tangential relative velocity and equal-and-opposite force projection. This is dissipative sliding, not a complete static-friction model. A stick-slip extension needs a tangential storage variable and an explicit energy budget. No random scratch noise in the first V4 voice.

## 8. Cap, excitation, and mount

### 8.1 Cap attachment

Model the tip cap's mass and relative compliance explicitly. For cap position `r_cap(q)` and reconstructed wire-tip position `r_tip(q)`, both in the same physical frame, with a declared rest attachment offset `d_cap`:

```text
e_cap = r_cap(q) - r_tip(q) - d_cap
U_cap = 0.5 * transpose(e_cap) * K_cap * e_cap
```

A relative-velocity damper must act through the same attachment Jacobian. Include the reaction on the spring and any participating mount coordinates. Choose the rest offset/prestrain consistently with the assembled equilibrium; do not subtract an absolute tip position from a cap displacement. Start with the translational coordinates necessary for the target; add cap rotation/inertia when measurements show it matters.

Do not insert a fixed 90–105 Hz cap resonance simply because an earlier model used one. Fit or estimate mass and compliance with uncertainty.

### 8.2 Default trigger: bounded flick

Keep existing normalized bipolar velocity semantics and the ordinary no-cable value of 0.5. Use a short, finite force through the cap. Start with an impulse-normalized raised-cosine pulse:

```text
F(t) = direction * I(v) / T(v) * (1 - cos(2*pi*t/T(v)))
       for 0 <= t <= T(v)
```

Its continuous-time impulse is `direction * I(v)`, in newton-seconds. Normalize the actual discrete pulse using the solver's force quadrature so `sum_n h_n * F_bar_n = direction * I(v)`; include the timestep, not just the sample sum. Map `abs(v)` smoothly to requested impulse and duration. Zero/nonfinite velocity is a no-op. A first audition search can cover approximately 0.5–3 ms, but capture the actual target's excitation before treating any duration as physical truth.

Track pulse age in seconds and remaining impulse, independent of substep count. A tabulated cumulative pulse integral can preserve impulse when an interval is split for a solver retry or the host rate changes. Never restart or renormalize the already delivered part. Account for external work using the same force and cap displacement as the mechanical step. Equal impulse does not imply equal work on a spring that is already moving.

Reverse the entire intended excitation direction for a mirrored bipolar strike. Any persistent directional asymmetry must be an explicit specimen trait.

Use a small fixed-capacity pool of independent active pulses. Completed pulses are retired. Specify the supported event rate, maximum pulse duration, and simultaneous manual/external events, then size the pool against those bounds. Sum simultaneous forces before the mechanical solve with deterministic event ordering. On overload, reject the newest pulse, count it, and preserve accepted pulses; require zero overflows in ordinary acceptance tests. Keep the rejected event distinguishable from applied impulse in diagnostics.

Retriggers preserve all mechanical positions and velocities. A new strike can reinforce or oppose existing motion. Do not assert that every harder retrigger must produce a larger instantaneous output peak.

### 8.3 Pluck-release reference

Provide an analysis excitation that starts from a statically deflected equilibrium and releases the hold. Record its initial stored energy as externally supplied preparation work. This tests the spring's natural return without relying on a broadband cap hit.

Do not replace the state of an already moving spring with a precomputed pluck pose. A future interactive hold-and-release feature must use an explicit virtual-finger constraint or force, with its work accounted for. No new front-panel interaction is required for V4's first release.

### 8.4 Mechanically coupled mount

Represent the relevant mount compliance using a small number of coordinates and positive stiffness/damping. Couple it to the wire boundary through relative displacement and velocity, so reaction acts in both directions. Calibrate against the actual mounting condition.

Retain moving-boundary/interface deformation shapes in the bare-wire reduction. Fixed-base modes alone cannot represent a moving attachment by simply adding a mount oscillator afterward. Assemble the resulting inertial cross terms in `M` before normalization; do not silently assume diagonal spring-plus-mount mass. Check a prescribed base-motion response against the full reference.

Keep a rigid-mount diagnostic. A one-way response filter may be useful as an observation transfer, but it is not a mechanically coupled mount and must be named accordingly. Do not trigger a separate low thump oscillator to manufacture body weight.

## 9. Observing sound without manufacturing the boing

### 9.1 First observer

Start with fixed, signed observations of distributed wire motion and the coupled mount. Form several mechanical observation channels, then apply stable, causal, low-order transfer filters:

```text
y_raw(t) = H_wire * s_wire(t) + H_mount * s_mount(t) + H_cap * s_cap(t)
```

`*` denotes filtering, not necessarily multiplication. The channels may combine physically located normal velocity, acceleration, and boundary reaction. They are an approximation to measured pressure, not a claim that one acceleration sample is an exact acoustic radiation solution.

Fit channel weights and transfer responses against held-out recordings. Keep sign and phase information; do not take the absolute value of every contribution. Do not place a global `abs(velocity)^n` gate after the body.

A configuration-dependent observer is a later experiment. Derive it from geometry or identified radiation behavior and constrain its complexity. The first question is whether the mechanical model produces the repeated flexing sound through a substantially stationary observer.

### 9.2 Avoid hiding missing modes

Contact forces are mechanical diagnostic channels in newtons, not automatically independent acoustic stems. In a nonlinear system there is no unique additive 'contact audio' that can be muted without defining a separate approximation.

If high-frequency contact response is missing, first improve the reduced basis or validated residual compliance. A stable observation filter driven by reaction force can approximate unresolved radiation, but it must be calibrated and separately identifiable; it must not become another long-lived pitched voice mixed in until the spectrum looks right. A feed-forward force-to-audio filter is not a mechanical power port, so calling that filter passive does not establish passivity of the mechanics.

### 9.3 Output conditioning

Expose separate raw-observer, linear pre-limiter, and module-voltage taps. Normalize none of them secretly. Give every diagnostic channel a unit or a declared scale.

Use a DC blocker and fixed output calibration. The final safety limiter should normally be inactive during a reference-level ordinary strike. Fit and compare timbre before limiting. Maintain a bounded module output for pathological excitation, but flag safety interventions in the analysis renderer.

Observe dynamic departures from equilibrium or initialize static observation/filter states at their steady values. A nonzero resting mount reaction or contact preload must not create an artificial startup/reset click.

### 9.4 Decay

Fit dissipative terms from the target's mechanical and bandwise decays. Do not equate authenticity with either long metallic tails or blanket high-frequency suppression. Preserve the body/midrange that carries twang while preventing an unjustified stationary tail from taking over after the principal gesture.

Bulk damping, cap loss, mount loss, and contact loss should remain separately inspectable. Any extra state-dependent loss must remain dissipative. Do not use an output-envelope follower to shut off arbitrary modes in the initial physical model.

## 10. Offline reduction and parameter fitting

### 10.1 Reference construction

Build the helix, boundary, tip, and candidate-contact geometry from a manifest. Validate the no-contact static/eigen response, then the contact equilibrium. Refine the spatial mesh until the chosen response bandwidth and contact compliance converge. The display's drawn turn count and dimensions are not mechanical measurements.

Collect rest, held-bend, torsional/axial perturbation, and contact-loaded snapshots. Include both directions. Separate training and validation configurations.

### 10.2 Basis construction

Combine relevant vibration modes with static tip-load and contact-load deformation vectors. Include snapshots from open and contacting configurations. Remove dependent vectors and mass-orthonormalize the result. All runtime states use this single basis; do not switch modal banks when the active contact set changes.

A simple truncation to the lowest eigenmodes can be insufficient for local contact compliance. Validate the response to forces at the actual contact locations, not just the rest-state frequency list. Positive weighted contact reduction must also converge against the full contact candidate set.

### 10.3 Exported model data

Export the mass/stiffness representation, reference equilibrium, shape/reconstruction maps, cap and mount attachment maps, contact geometry and weights, wire-energy approximation and derivatives, observation maps, damping parameters, model validity bounds, and provenance.

The manifest must include schema/version, generator version, source commit/diff hash, geometry and generated-data hashes, parameter origins (`measured`, `fitted`, `estimated`, `unknown`), fit/validation recording IDs and content hashes, dimensional conventions, intended sample-rate/solver settings, and error metrics. Unknown required physical values must be replaced by labeled estimates before generation. Model data are not sample recordings.

The first prototype uses one immutable nominal dataset. Define any later seed/wear family as bounded parameter sets with validated equilibrium, positive mass, contact coverage, and validity limits. A seed is not permission to perturb matrices independently or interpolate reduced bases that use different coordinates. If a coefficient change needs an equilibrium solve, prepare its result outside the audio callback or use prevalidated data; a sleeping audio callback is still an audio callback.

### 10.4 Fitting order

First fit static force-deflection and low motion. Then fit contact opening/closure behavior and held-bend response. Next fit the relevant wire response and decay. Fit acoustic observation and final gain last.

Use a small number of interpretable adjustments with bounds. Do not let a large, freely time-varying output filter compensate for incorrect mechanics. Do not claim a unique recovered geometry from audio alone; parameter combinations can be unidentifiable.

Hold out entire recordings or specimens where possible, not just neighboring strikes from the same file. Choose the nominal voice on the target subgroup and evaluate robustness separately on the wider corpus.

## 11. Numerical design and real-time constraints

### 11.1 Reference integrator

Build a double-precision offline energy-audited solver first. For the fixed-mass model, a discrete-gradient formulation provides a concrete reference:

```text
q1 - q0 = h * v_bar
M * (v1 - v0) / h = -G_U(q0,q1) - D_bar * v_bar + Q_external_bar
v_bar = (v0 + v1) / 2
G_U(q0,q1) dot (q1 - q0) = U(q1) - U(q0)
```

With an accurately solved step and consistent external work, this gives:

```text
E1 - E0 = h * Q_external_bar dot v_bar
          - h * transpose(v_bar) * D_bar * v_bar
```

Here `E = 0.5 * v^T M v + U(q)`. If contact damping is represented separately from `D_bar`, add its nonpositive work explicitly on the right-hand side. A midpoint gradient is not, in general, a discrete gradient of an arbitrary nonlinear potential.

For a weighted contact patch, a concrete discrete chain rule is:

```text
Delta_q = q1 - q0
Delta_g = g(q1) - g(q0)
J_bar * Delta_q = Delta_g
psi(g) = Phi(max(-g, 0))
F_elastic_bar = -(psi(g1) - psi(g0)) / Delta_g
Q_elastic_bar = a * transpose(J_bar) * F_elastic_bar
v_gap_bar = Delta_g / h
F_damping_bar = c * chi * max(-v_gap_bar, 0)
```

`chi >= 0` approximates `delta^p` during the interval and is zero for an entirely separated step. This gives `Q_elastic_bar dot Delta_q = -a * (psi(g1) - psi(g0))` and damping work `a * F_damping_bar * Delta_g <= 0`. Use the derivative limit for the scalar quotient at small `Delta_g` and a tested discrete gap gradient for `J_bar`; the midpoint normal generally does not satisfy the chain rule for a nonlinear gap. Resolve a contact that closes and reopens within a step through temporal convergence/substeps, not endpoint gap signs alone.

The energy identity is a property of the specified discrete equations and solve accuracy, not a guarantee that an arbitrary iteration count will satisfy them. Handle small increments without catastrophic cancellation; use analytic component differences or carefully tested stable formulas.

### 11.2 Production solver

Attempt a bounded, fixed-step reduced implicit solve with preallocated storage. Exploit the constant linear part and low-rank/local nonlinear couplings; do not refactor a large dense full-system matrix without first benchmarking that cost. Benchmark the all-contacts-active case and engine crossfades, not only idle or weak strikes.

Use the offline solver to establish whether a cheaper potential-based/energy-quadratized or split method meets the same motion, audio, and work-balance tolerances. A cheaper method is acceptable only after these comparisons. Stability alone is not enough: numerical damping can erase the boing.

Set a hard iteration/retry budget. A bounded retry with smaller substeps must roll back the complete state, including pulse time and diagnostics. A fallback recovery is a fault path, never the routine way hard strikes remain bounded. Count it and require zero ordinary-operation recoveries.

Specify scaled residual and work-balance tests for accepting a step. Commit mechanical state, observer/decimator updates, pulse consumption, and accumulated work only after acceptance; keep rejected-attempt counters separate. On exhausted retries, nonfinite state, or departure from the model's validated domain, discard the failed step and enter a bounded recovery to the stored equilibrium with a short output fade. Record discarded energy and pending impulse as a fault, never as physical damping. Preserve seed/wear metadata. The offline renderer must mark such a render failed so quiet recovery cannot appear to be a successful decay.

### 11.3 Sampling and aliasing

Use substepping plus a real antialias decimator. Averaging two internal outputs is not the acceptance criterion. Start investigations near 176.4/192 kHz internal rate for 44.1/48 kHz hosts, then determine the smallest sufficient rate by comparison with a higher-rate reference.

Keep mechanical integration rate, observation rate, and host-output decimation explicit. Filter latency must be reported and aligned for listening comparisons. A stable contact solve can still alias; reduce aliasing through validated smooth compliance, adequate sampling, and output filtering rather than a hidden nonlinear-force clamp.

As an initial schedule, use 4 internal steps at 44.1/48 kHz, 2 at 88.2/96 kHz, and 1 at 192 kHz, subject to convergence. Solver retries subdivide an internal interval; the observer must still feed a uniform clock to its decimator, or use a validated resampling path. Do not push irregular retry samples through a fixed-rate filter. Specify behavior for other Rack sample rates and preserve mechanical state and undelivered impulse across rate changes; prepare filter coefficients outside hot loops and account for filter-state transition latency.

### 11.4 CPU and lifecycle

Use fixed-capacity arrays and model data loaded or prepared outside the audio callback. Do not regenerate the reduction on `setSpecimenSeed`, on every strike, or on every `setBreakInLocked` call. Repeated setters with unchanged values must be cheap; the adapter calls the lock setter every sample. Prefer cached pulse tables, polynomial potentials, and shared fast-math helpers. Retain an expensive norm or transcendental only where contact geometry/accuracy requires it and benchmarking supports it. An approximated potential, force, and Jacobian must remain mutually consistent.

Start with an engineering objective of no more than roughly three times V3's active DSP cost on the same machine, but treat that as a target to measure, not a predicted result. Identify the V3 variant, compiler flags, host rate, coordinate/contact counts, iteration limit, and active render duration. Record absolute time per host sample and worst observed processing-block time as well as the ratio, including V4-plus-outgoing-engine transitions. A dense 32–64-coordinate nonlinear refactorization at every internal step is not an assumed viable implementation. If the measured budget cannot support an accurate reduction, report the tradeoff instead of silently degrading the contact model.

Sleeping must depend on the complete dynamic state and pending excitation, not the selected audio tap or limiter output. Test velocity, deviation from the stable equilibrium, and excess energy `E - U(q_eq)` with declared absolute floors and a continuous quiet hold. Static preload energy and nonzero balanced contact forces do not prevent sleep. Require a quiet canonical observer/decimator tail before clearing dynamic state; every tap is a read-only observation of the same processed state. Preserve the stored static equilibrium when resetting motion and account for the tiny discarded residual energy.

For observer/EQ experiments, disable automatic sleep over a fixed capture duration, or replay identical recorded mechanical observations through the alternate filters. This prevents different filter tails from changing the mechanical trace through different sleep times. Production tap-selection tests retain the canonical sleep path and must reach sleep at the same sample.

### 11.5 Parameter changes and wear

Changing masses, stiffness, preload, or contact geometry can change stored energy. Freeze physical coefficients during controlled comparisons. For the first production version, apply accumulated wear-related coefficient updates only at a safe resting boundary; retain accumulated break-in metadata while motion is active. Explicit restore/new-specimen operations may use the module's established reset policy.

Do not silently move equilibrium or retune a loaded contact network during an active swing and call the resulting energy passive.

## 12. Visual integration: keep the drawing

Keep `buildSpringGeometry()`, its constant-length arc, coil offsets, tip orientation, trail rendering, cached idle geometry, and overflow handling. [C7]

Feed the existing `Frame` with a bounded mechanical projection:

```text
x_tip = panel_projection * mechanical_tip_displacement
frame.displacement = D_max * tanh(G * x_tip / D_max)
```

Use tip displacement relative to `q_eq`, with a fixed low-motion projection/filter if needed to prevent unresolved wire-frequency motion aliasing into the UI. Calibrate `G` and `D_max` against the complete existing visual mapping: `visualTipTravel()` already applies `48 * tanh(0.75 * displacement)`. The proposed extra saturation is optional; avoid accidentally compressing the gesture twice. Compute it at telemetry rate or use a tested fast approximation. A normalized derivative may populate `frame.velocity`; calculate it consistently or document it as an intensity cue. Do not substitute another oscillator for the low mechanical motion.

Keep audio activity available for brightness or the meter, separately from position. Preserve the existing meter's user-facing envelope policy. Generalize the module's V3-specific visual-tracking test so V4 can request the same physical-style tracking without pretending to be V3.

Observer/EQ/output-gain changes must leave the mechanical trace and displayed trajectory unchanged under the common capture/sleep policy in Section 11.4. Test at 30, 60, and 120 Hz UI updates, with audio-time-based publication and the existing thread-safe handoff. The drawn constant-length arc is a calibrated projection, not a reconstruction suitable for contact geometry. Preserve the graphics lifecycle helpers and module-level `Process`, `Step`, and `Draw` telemetry meanings; gate additional developer telemetry with `isDragonKingDebugEnabled`.

## 13. Repository integration

### 13.1 New files

Suggested organization, with only necessary compilation units in the existing source build path:

```text
src/DoorstopContactHelixEngine.hpp
src/DoorstopContactHelixEngine.cpp
src/doorstop_v4/ModelData.hpp
src/doorstop_v4/GeneratedModelData.hpp
src/doorstop_v4/State.hpp
src/doorstop_v4/Potential.hpp
src/doorstop_v4/Contact.hpp
src/doorstop_v4/Integrator.hpp
src/doorstop_v4/Excitation.hpp
src/doorstop_v4/Observer.hpp
src/doorstop_v4/Diagnostics.hpp

tools/doorstop_v4/build_model.py
tools/doorstop_v4/fit_model.py
tools/doorstop_v4/analyze_render.py
tools/doorstop_v4/model_manifest.schema.json

tests/doorstop_contact_helix_engine_spec.cpp
tests/doorstop_contact_helix_energy_spec.cpp
tests/doorstop_contact_helix_compat_spec.cpp
```

Keep generated coefficient definitions included in one translation unit. Share algorithm helpers where genuinely reusable, but do not redesign every historical Doorstop engine as part of this work.

### 13.2 Public engine contract

Mirror the existing engine-facing lifecycle: sample rate, reset motion, factory restore, seed, break-in/lock, signed strike, `process()` returning `Frame`, sleeping, and maximum visual displacement. Keep detailed physical diagnostics separate from `Frame`.

Append `ReferenceV4` before `EngineMode::Count` without renumbering older values: V1 = 0, Legacy = 1, V2 = 2, V3 = 3, V4 = 4. Add explicit V4 dispatch in the router; the binary `referenceEngine()` helper must not accidentally route V4 to V1. [C8]

Update sample-rate propagation, condition propagation, strike routing, sleep queries, reset/factory operations, visual maximum, and all transition paths. Preserve the established destination-reset and bounded two-engine crossfade policy. Do not introduce a three-engine transition just for V4.

### 13.3 Persistence and menu

Add serialized engine string `referenceV4`. Preserve existing V1/V2/V3 strings and legacy `soundModel` integer meanings; `engineMode` itself currently parses strings only. A newly constructed/reset module selects V1, while missing, invalid, or unknown `engineMode` in loaded JSON currently falls back to Legacy. Test those distinct entry paths explicitly. [C5]

Serialize a V4 model-data revision with a defined first-revision default for missing data. For an unavailable revision, use a documented bundled V4 fallback and expose the mismatch in diagnostics; do not reinterpret the patch as a different engine silently. Preserve metadata through load/save without serializing live solver/contact state. Verify seed/wear restoration against the chosen dataset.

Add one experimental V4 voice, not a menu of twelve tuning guesses. Keep contact-off, frozen-contact, observer, and integration experiments in the analysis renderer. No new panel controls are necessary.

### 13.4 Renderer and baseline repairs

Extend `tools/doorstop_reference_render.cpp` with explicit V4 selection, true raw/pre-limiter/module taps, physical traces, and metadata. Reject unsupported combinations rather than silently ignoring them. Render modes should identify their actual engine, tap, specimen, effective wear, excitation, model-data revision, and solver settings.

Keep the completed-pulse and tap-routing repairs as separately reviewable changes. Leave all staging and commits to the user, as required by `AGENTS.md`. Preserve original baseline fixtures as well as corrected-harness fixtures so changed retrigger excitation is not confused with a new acoustic result. Add a V3 raw tap through separate diagnostics while keeping canonical module output, visual envelope, and sleep logic intact. Do not bundle baseline retuning into these fixes.

Add Rack-independent V4 engine/energy/router tests to `TEST_BINS_NON_RACK` and the `test-fast` recipe; update each standalone renderer/test link rule that now needs the V4 translation unit. Put JSON/module-adapter coverage in the existing Rack-linked `doorstop_runtime_spec` path, which currently belongs to `test-rack`, not `test-fast`. Run that focused test when changing persistence. Validate implementation with native MINGW64 `make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"` and a full incremental `make -j10 plugin.dll`; follow [the Windows build instructions](doc/windows_build_from_wsl.md) when invoking through WSL. Documentation-only review does not require those builds. Pin source/diff and model data before acceptance renders. [C9]

## 14. Experiments that discriminate mechanisms

| Experiment | Purpose | Important control |
|---|---|---|
| Full V4 vs fixed-contact linearization | Test opening/reclosing rather than simply added stiffness | Match static equilibrium and small-signal response as far as the diagnostic permits |
| Full V4 vs contact disabled | Test total contribution of contact | Recompute equilibrium; do not release preload accidentally and call it an acoustic difference |
| Nonlinear wire energy vs local linear wire energy | Test geometric contribution | Keep contact and observation settings identified |
| Coupled mount vs rigid mount | Identify boundary contribution | Do not retune wire damping at the same time |
| Fixed vs geometry-dependent observer | Test observation contribution | Mechanical trajectory must be identical |
| Flick vs prepared pluck release | Test excitation dependence | Record initial/external energy, not just peak force |
| Ordinary reduction vs higher-order reference | Detect truncation artifacts | Match geometry, potentials, contact law, and excitation |

A fixed-contact control is particularly important: contact-off alone changes the structure, so it cannot cleanly establish that the *changing* contact set is what matters. Define the control as a Taylor expansion of the contact potential about a declared preloaded/held equilibrium, retaining its constant force and tangent (including gap-curvature terms), while leaving the chosen wire model unchanged. Match the assembled equilibrium and local response where this is well-defined; document any replacement of contact damping. A completely linearized assembled model is a separate diagnostic that also removes wire nonlinearity.

At zero-load touching, the `p = 1` unilateral law has no unique two-sided tangent; use declared compression-side/open-side controls or a measured preloaded/held state. For `p > 1`, the tangent stiffness there is zero, so its unique linearization misses finite-amplitude contact stiffening. Do not invent a nonzero matched contact stiffness in either case. A bilateral tangent extension may exert tensile force away from its expansion point; label it an analysis approximation with a limited range, not physical contact. Never freeze contact forces independently of the control potential or report their work as zero while positions change.

In nonlinear dynamics, mechanics-changing ablations will not preserve identical trajectories. Only observation-only comparisons should demand identical state. Report changed initial energy and equilibrium where applicable.

Do not declare victory because contact makes the result noisier. Listen for an improved, coherent boing sequence. Periodic ticks, dry rattling, or a buzz layered over an unchanged bell are negative outcomes.

## 15. Diagnostics and acceptance tests

### 15.1 Mechanical correctness

Test balanced rest with and without preload, symmetry under transformations actually supported by the chosen geometry/mount, mass-normalization equivalence, force/energy gradient consistency, contact-Jacobian finite-difference agreement, non-attractive separation, and equal-and-opposite contact action before reduction. Reversing the full excitation vector is mandatory; exact mirror symmetry of a chiral helix with particular end attachments is not assumed.

With external drive off, verify the full energy/work balance within the specified solver tolerance. Include wire, contact, cap, and mount energy, dissipated work, and any parameter-change work. Do not substitute a modal activity proxy or the UI energy bar.

Check single-contact rebound, simultaneous contacts, grazing contact, opening from preload, and force-deflection response at several held bends. Check the validity envelope of the reduced model; an energy-stable but geometrically implausible trajectory is still a failed model.

Freeze numerical acceptance thresholds in the model/test manifest before acoustic fitting. Define a fixed positive energy scale `E_ref` from a declared reference excitation, an absolute near-rest floor, scaled state/force residuals, and capture duration. Track both per-step error and accumulated `R(t) = E(t) - E(0) - W_external(t) + W_dissipated(t)`; parameter changes, reset/sleep truncation, and recovery require separate ledger entries. A small final residual must not hide cancelling large step errors.

Record numeric limits and measured results for gradient/Jacobian error, equilibrium residual, impulse error, energy/work error, penetration relative to wire diameter, held-bend force/frequency error, runtime-versus-refined motion/audio error, and CPU cost. For sampling convergence, halve the reference timestep and refine the reduction/contact set independently. Define the compared bandwidth and delay alignment; separate integration error from aliasing. Do not call a stage passed while these tolerances are still unspecified.

### 15.2 DSP and lifecycle

Test 44.1, 48, 88.2, 96, and 192 kHz; low, ordinary, and full-strength strikes; both signs; spaced and overlapping retriggers; seed/lock/restore behavior; repeated engine switching; and sample-rate changes while active.

Verify equal new applied impulse for equal requested nonoverlapping flicks, independent of earlier completed pulses. Include an active pulse across a host-rate change and a deliberately rejected solver step; neither may duplicate or lose accepted impulse. Test simultaneous opposite strikes, zero/nonfinite input, pool overflow, and solver/domain recovery counters. Verify that changing the output tap does not change mechanics, sleeping, excitation, or display, and that a loaded resting contact network reaches exact-zero output without erasing preload. Verify no allocations or locks in the callback, including reset/seed/wear paths, and bounded work in all contact/transition cases.

Cross-platform deterministic event order and within-tolerance floating-point results are required; bitwise identity across different math libraries is not assumed.

### 15.3 Listening protocol

Compare against V2 Dark Refined, V3 Stronger Body Bending, V3 Deep Swing at hard strike, and the selected real target. Use fixed-gain renders to inspect dynamics and separately external level-matched renders to compare timbre. Use a documented onset-relative active window, apply its single gain to the complete clip, and preserve silence/decay rather than normalizing each subwindow.

Prioritize velocity 0.5, then test approximately 0.3, 0.65, 0.8, and 1.0. Use the existing seeds 1, 77, and 3076668551 for continuity, with additional seeds reserved for validation. Freeze effective break-in during the comparison.

Assess recognizability, within-boing pitch movement, repeated-lobe continuity, attached metallic twang, late stationary ringing, and hard/soft-strike transitions. A low/body-to-bell energy ratio is useful descriptive data, not the perceptual acceptance criterion.

Randomize and conceal engine labels, record per-clip gains and filter-delay alignment, and keep the target recording, velocity, and seed matrix fixed. Archive listener ratings and the decision rule before choosing a winner. The user's audition determines the preferred voice; automated metrics or an unauditioned render cannot satisfy this gate. A single-listener preference is useful product evidence, not a population-level authenticity result.

### 15.4 Analysis outputs

Capture physical low motion, held-bend state, contact gaps/loads, contact activity, applied force and impulse, cap and mount motion, component energies/losses, mechanical observation channels, pre-limiter signal, final output, and all safety/solver counters.

Analyze low-motion cycles independently of audio-envelope cycles. Track time-frequency ridges, bandwise envelope phase, and late stationary energy with uncertainty when crossings, overlapping modes, or contact transients make tracking ambiguous. Compare high-rate and runtime outputs for aliasing and numerical damping.

## 16. Implementation stages and exit gates

### Stage A — Trustworthy baseline and minimal runtime shell

Pin source, capture baseline fixtures, isolate pulse/tap fixes, add V4 engine routing and a silent/linear test implementation, and preserve the renderer. Establish common diagnostics, wear locking, level-matching tools, the acceptance manifest, and a native CPU baseline. Keep an incomplete/silent V4 shell in the analysis/test path or behind the existing developer gate until it is playable.

**Exit:** correct selection, persistence, transitions, taps, excitation, and unchanged old-voice tuning/first-strike behavior; document the intentional pulse-repair difference in affected retriggers. No authenticity claim.

### Stage B — Contact-constrained reduced prototype

Build a nominal linear curved-wire reference, mass-normalized reduction, explicit cap/mount, and reciprocal neighboring-turn contact. Include contact-load basis enrichment and balanced rest. Use a fixed observer. Begin at deflections where the reference approximation is credible; label the geometry estimated until measured.

This is the cheapest version that tests the new contact topology without first implementing every nonlinear material/geometric refinement.

**Exit:** numerical tests pass, the all-contacts-active reduced solve has a measured feasible runtime cost, and opening/reclosing yields a meaningful audible improvement over the declared fixed-contact control at ordinary excitation, or produces a well-diagnosed negative result. A negative result outside the model's valid deformation range is not decisive. Infeasible cost or a converged negative result triggers reassessment before further fitting.

### Stage C — Nonlinear geometry and measured target

Add the validated nonlinear wire-energy approximation, full reciprocal derivatives, and target-derived geometry/preload where available. Compare against the higher-resolution nonlinear reference. Keep the fixed observer first.

**Exit:** the model reproduces useful low-motion and held-bend-response trends without arbitrary time-dependent stiffness drives. Contact and geometry effects are distinguishable.

### Stage D — Acoustic fitting

Fit losses and observation transfers. Evaluate cap/mount contributions, prepared pluck release, and optional friction separately. Reject improvements that merely add chatter or remove all twang.

**Exit:** blind level-matched preference for V4 over the preferred existing baselines in the target subgroup, including ordinary strikes. Keep real recordings in the comparison.

### Stage E — Production

Complete optimization against the reference renders and energy traces preserved since Stage B. Validate reduction size, solver cost, antialiasing, seed family, lifecycle, and patch compatibility. Freeze generated model data and publish provenance.

**Exit:** acceptable measured CPU, no ordinary-operation solver/safety failures, stable behavior across sample rates and retriggers, preserved visual quality. Keep V4 opt-in until the listening decision supports a default change.

## 17. Explicit stop conditions

Do not continue adding mechanisms indefinitely. Reassess the design when a converged contact model offers no advantage over the properly defined fixed-contact controls, when realistic target parameters require an implausible observer to sound right, when only a limiter makes the boing audible, or when a converged model cannot meet the measured runtime budget. Ordinary strikes leaving the validity domain or invoking solver recovery are also failed gates, even when the resulting audio is finite.

If contact is not decisive, the same measured reduced model can test geometric coupling, distributed propagation, and mounting response without reverting to unrelated voice layers. A sample playback of the real target is a useful authenticity control, not evidence that the physical model has succeeded.

## 18. Condensed Codex handoff

> Implement a new opt-in `ReferenceV4` / `ContactHelixEngine` in Doorstop on `expander`. Preserve V2 Dark Refined and all V3 variants as baselines, and preserve the existing spring drawing. The new model must represent a continuous helical wire with spatial neighboring-turn contacts that may be closed at rest, open during bending, and reclose. Derive each gap, its velocity, and force projection from the same geometry; balance preload at rest. Use a compact geometry-derived, contact-enriched reduced basis, explicit cap/mount coupling, and wire/contact forces derived from a common energy. Do not use a global crossing gate or a hard-strike envelope as the source of the boing. First isolate existing pulse-retirement and renderer-tap defects, then implement and test the contact-constrained prototype with a fixed observer. Compare opening/reclosing against a fixed-contact linearization and contact-off controls before expanding nonlinear geometry. Add full work/energy diagnostics, signed impulse-normalized excitation, proper antialiasing, mechanical-only visual displacement, explicit router/serialization support, and ordinary-velocity listening acceptance. Never label generated coefficients measured unless their manifest records the measurements, and never label the new engine authentic solely because its mechanics are more elaborate.

> Use the reviewed commit as the source baseline and record subsequent source/data hashes. Preserve quadrature weights and discrete contact work consistently, keep pulse impulse invariant through retries/rate changes, sleep relative to the loaded equilibrium, and define the fixed-contact control at unilateral boundaries. Prove bounded CPU cost in Stage B. Apply physical coefficient updates only from prepared, validated data at a safe boundary. Follow native Windows validation and leave all staging/committing to the user. Implementation proceeds through the gates above; this handoff does not mark any gate complete.

---

## References and source map

### Physical research

**[R1]** Bernard E. Fisher, *Doorstop*, US2462174A, published February 22, 1949. The earlier review cites its construction description; full text could not be re-fetched in this revision. Recheck the attributed details before relying on them as verified construction evidence. It is not an acoustic experiment.  
https://patents.google.com/patent/US2462174A/en

**[R2]** Douglas P. Holmes, Andy D. Borum, Billy F. Moore III, Raymond H. Plaut, David A. Dillard, *Equilibria and Instabilities of a Slinky: Discrete Model*, 2014. DOI 10.1016/j.ijnonlinmec.2014.05.015; author manuscript arXiv:1403.6809.  
https://arxiv.org/abs/1403.6809

**[R3]** Jianwei Zhao, Zewen Gu, Quan Yang, Jian Shao, Xiaonan Hou, *Dynamic Finite Element Model Based on Timoshenko Beam Theory for Simulating High-Speed Nonlinear Helical Springs*, Sensors 23(7), 3737 (2023). DOI 10.3390/s23073737. Abstract rechecked through the authors' university repository; publisher/PMC full-text fetches were blocked in this revision. No numerical valve-spring parameters are transferred here.  
https://www.mdpi.com/1424-8220/23/7/3737  
https://research.lancaster-university.uk/en/publications/dynamic-finite-element-model-based-on-timoshenko-beam-theory-for-/

**[R4]** Anis Hamza, Sami Ayadi, Ezzeddine Hadj-Taïeb, *The natural frequencies of waves in helical springs*, Comptes Rendus Mécanique 341 (2013), 672–686. DOI 10.1016/j.crme.2013.09.006. Publisher abstract/references inspected; not a lateral-doorstop validation.  
https://comptes-rendus.academie-sciences.fr/mecanique/articles/10.1016/j.crme.2013.09.006/

**[R5]** Saša Ilijić, Ana Babić, Dora Ivrlač, Andrew DeBenedictis, *The nonlinearity of helical springs: An energy-based approach*, American Journal of Physics 93, 932–942 (2025). DOI 10.1119/5.0252682. Abstract inspected; used for its stated energy/end-condition scope, not undocumented equations.  
https://arxiv.org/abs/2510.16960

**[R6]** Stefan Bilbao, Michele Ducceschi, Craig J. Webb, *Large-scale real-time Modular Physical Modeling Sound Synthesis*, DAFx 2019, paper 22.  
https://dafx.de/paper-archive/2019/DAFx2019_paper_22.pdf

### Inspected source files (pinned to reviewed commit)

**[C1]** `src/HelicalContinuumEngine.cpp`: modal coefficients, `strike()`, `processSubstep()`, contact, stiffness/observation, energy proxy, and visual feed.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/HelicalContinuumEngine.cpp

**[C2]** `src/HelicalContinuumEngine.hpp`: V3 surrogate state, pair count, tuning variants, and engine contract.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/HelicalContinuumEngine.hpp

**[C3]** `src/ReferenceSpringEngine.cpp`: V2 component mixing, phase-related observation, and output conditioning.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/ReferenceSpringEngine.cpp

**[C4]** `tools/doorstop_reference_render.cpp`: V3 selection and output-tap dispatch.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/tools/doorstop_reference_render.cpp

**[C5]** `src/Doorstop.cpp`: input normalization, telemetry, V3-specific visual tracking, and serialization.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/Doorstop.cpp

**[C6]** `tools/doorstop_reference_manifest.json`: recording paths and onset windows. Its existence is not a claim that these recordings were auditioned in this review.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/tools/doorstop_reference_manifest.json

**[C7]** `src/DoorstopWidget.cpp`: `buildSpringGeometry()` and existing visual construction.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/DoorstopWidget.cpp

**[C8]** `src/DoorstopEngineRouter.hpp/.cpp` and `src/DoorstopEngine.hpp`: mode ordering, routing, transitions, and `Frame`.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/DoorstopEngineRouter.hpp  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/DoorstopEngineRouter.cpp  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/src/DoorstopEngine.hpp

**[C9]** `Makefile`: standalone renderer dependencies, V3 audition variants, and fast/Rack test registration.  
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/2ac867729767c3e8691a371d2f914ecfb07d7326/Makefile

### Prior project documents

**[P1]** [*Doorstop V3: Recovering the boing and twang — Source-grounded review and staged implementation brief*](doc/Doorstop_V3_Boing_Twang_Review.md), September 4, 2026. Targets the earlier `ds-v3-refinement` review, not a description of every later branch state.

**[P2]** [*Doorstop Reference V3 — Helical Continuum Design*](doc/Doorstop_ReferenceV3_Helical_Continuum_Design.md). Its paired-continuum proposal predates the current V3 surrogate. In particular, its contact-off acceptance requirement is not adopted as a physical premise of V4.
