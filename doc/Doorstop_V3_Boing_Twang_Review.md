# Doorstop V3: Recovering the boing and twang
## Source-grounded review and staged implementation brief

**Target:** `PlasmaChroma/Leviathan-Rack2`, branch `ds-v3-refinement`  
**Reviewed:** September 4, 2026  
**Branch history at review:** latest listed commit `7c433cf`, “More tuning around v3 body bending”  
**Primary listening baselines:** `DeepSwing` and `DeepBodyBend`  
**Status:** Static code review, isolated algebra/frozen-linearization probes, and proposed experiments. Not a compiled plugin evaluation, audio audition, or proven replacement model.

## 1. Decision

Refine the existing `DeepBodyBend` path rather than replacing V3 or globally lowering its frequency table. Preserve `DeepSwing` unchanged as a listening control. Keep the existing spring drawing and its audio-thread/UI-thread architecture.

The strongest hypothesis is a conflict between **deformation-driven pitch motion** and **the observer that makes that motion audible**. The bending path changes stiffness most at large deflection. Much of the audible body is emphasized near center crossing instead. Opening that observer indiscriminately can expose comparatively stationary surviving modes; suppressing all of those modes indiscriminately can remove the useful twang. This explains a plausible route between the two reported failure modes without claiming that source inspection proves what a listener hears. [S1]

The immediate goal is not “more bass,” “less metal,” or “a more elaborate continuum.” It is:

> Preserve a heavy, visibly bending spring while making its audible resonances flex through each swing, then let those resonances disappear before they become a detached stationary ring.

Implement diagnostics and two comparison-harness fixes first. Then change observation, selected modal motion, and selected decay independently. Leave cap normalization and a more rigorous contact/nonlinear mechanics revision for separate, controlled experiments.

## 2. Evidence and limits

The user's reported listening preference is the perceptual evidence. The inspected source establishes implementation behavior. `formula_probe.py` establishes only arithmetic and a frozen linearization of specified equations. None of those establishes that a proposed change sounds more like a real doorstop.

The attached August 26 **Doorstop Reference V3 Design** is an earlier proposal targeting `expander`, not a description of every behavior now present on this branch. Its useful principles remain shared state, finite excitation, comparison against recordings, and preservation of visual motion. Its long decay ranges, no-contact acceptance rule, and expectations about observer topology should remain hypotheses rather than authority over the current listening target. See its sections 5, 8, 10, 12, 15, and 17.

No new external acoustics claim is required for the immediate experiments below. Parameter ranges proposed here are search values, not measured properties of a universal doorstop.

## 3. Establish the correct baselines

The current names have distinct meanings. [S2, S3]

| Variant | Role in this investigation |
|---|---|
| `DeepSwing` | Approved comparison voice. Hard-hit deepening without the later continuum path. |
| `DeepContinuum` | Rotated paired stiffness, reaction-driven mount, revised observer. |
| `DeepBodyBend` | Continuum path plus stronger deformation-dependent body stiffness and broader velocity onset for deep character. Primary refinement base. |
| `DeepShortTail` | Continuum with shortened modal tails. Diagnostic control, not presumed improvement. |
| `DeepThickSpring` | Combines the short-tail and body-bending switches. Its name is not evidence that it is the best thick-doorstop approximation. |

Do not rewrite existing serialized variants. A new candidate should use an explicit experiment configuration or analysis-only parameters. Do not add a new enum value blindly: `usesContinuum()` currently relies on enum ordering. Audit that helper, router validation, serialization, menu entries, and rendering dispatch if a variant is added. [S2]

## 4. Why Body Bend is an important clue

`DeepSwing` starts its special strike character only above normalized velocity 0.65. The body-bending variants use a smooth transition across 0.20–1.00 instead. Ordinary external triggers without a velocity cable use 0.50. Thus the primary baseline can be tested in a regime where plain Deep Swing's special hard-strike behavior is absent. [S1, S4]

Isolated calculations of the current character-drive equations:

| Normalized velocity | Deep Swing drive | Body Bend drive |
|---:|---:|---:|
| 0.30 | 0.000 | 0.043 |
| 0.50 | 0.000 | 0.316 |
| 0.65 | 0.000 | 0.593 |
| 0.80 | 0.184 | 0.844 |
| 1.00 | 1.000 | 1.000 |

This comparison is not just “the same voice with stronger FM.” It changes when several existing hard-strike behaviors become active. A useful ablation therefore separates **the body-warp switch** from **the velocity-to-character mapping**.

For example, compare the Body Bend baseline with:

- identical broad velocity mapping, body warp disabled;
- identical mechanics and warp, observation changes only;
- original mapping, same body warp.

These configurations need not become permanent user-facing presets.

The shared global stiffness term can raise an isolated mode's frequency by at most `sqrt(1.035)`, approximately **1.735%**. In contrast, the body-warp contribution alone has limiting frequency multipliers of approximately **1.378, 1.334, 1.288, 1.241, 1.192, and 1.140** over pairs 1–6. These are theoretical bounds of individual factors, not measured pitch excursions. They exclude cap coupling, other stiffness terms, and the actual deformation reached in a render. [S1]

**Interpretation:** the stronger body-bending path has a meaningful mechanism for audible within-cycle pitch movement. Preserve that mechanism before looking for another set of static resonances.

## 5. First acoustic experiment: let the bend be heard

### 5.1 The observer still contains a sharp crossing emphasis

The source forms a phase-like ratio from low-pair speed and nominal-frequency-scaled displacement. It squares the ratio, then squares that result for the narrow crossing observer. The later continuum observer broadens this, but blends toward that narrow observer on stronger hits. [S1]

Call the ratio `u`. The narrow component is `u^4`.

For a simplified single-plane sinusoid whose actual frequency equals the nominal phase-normalization frequency:

| Phase after center crossing | `u` | `u^4` | Simplified gain `0.08 + 0.92 u^4` |
|---:|---:|---:|---:|
| 0° | 1.000 | 1.0000 | 1.0000 |
| 22.5° | 0.707 | 0.2500 | 0.3100 |
| 45° | 0.500 | 0.0625 | 0.1375 |
| 67.5° | 0.293 | 0.0074 | 0.0868 |
| 90° | 0.000 | 0.0000 | 0.0800 |

This calculation deliberately sets station depth to one and excludes the continuum blend, directional term, and other gains. It illustrates the narrow component, not the full output envelope.

The important interaction is that body warp is largest near deformation extrema while this component is smallest there. The proposed diagnosis is **observer masking of pitch motion**, not a proof that a crossing observer is physically impossible.

### 5.2 Deepening also changes the apparent gate width

The phase ratio uses bare modal `omega`. The actual low motion is altered by softening and cap coupling. For a sinusoid of actual angular frequency `Omega`, at equal displacement/speed phase the ratio depends on `Omega/(omega + Omega)`, not necessarily one half.

Consequently, reducing actual swing rate can make the observer narrower in mechanical phase even with an unchanged exponent. Do not compensate for this only by adding output gain.

### 5.3 Proposed observation-only experiment

Capture one unchanged trajectory from `DeepBodyBend`: modal positions, velocities, accelerations, character drive, and relevant low-state quantities. Reconstruct several output observers from the same capture offline. If storage is inconvenient, rerender deterministically and verify that the raw mechanical state is identical.

Compare:

1. Current observer.
2. Broader crossing shoulders, initially replacing the narrow exponent 4 with 2 in that branch only.
3. A broad crossing/bend mixture on the body families, initially testing a nonzero observation floor of roughly 0.15–0.25.
4. The same observer with phase normalization based on measured low-state motion rather than bare table `omega`.

These are diagnostic search values. They are not a proposed acoustically exact radiation equation. Normalize phase robustly across small amplitudes and use smooth fallbacks near rest. Do not take noisy numerical derivatives or infer physical phase from the displayed, amplitude-shaped displacement.

Keep the upper detail quieter than the principal twang-bearing families. Do **not** make every mode continuously loud. Level-match externally using a common onset and comparison window.

**Pass:** the pitch-flexing character becomes clearer without turning into steady ringing or obvious tremolo.  
**Fail:** only loudness, lobe width, or sustained bell exposure changes. In that case proceed to the residual-mode experiment; do not endlessly adjust the exponent.

Because this is observation-only, the raw mechanical trace should remain exactly unchanged. The current visual mapping depends on the output envelope, however, so use a frozen visual feed during this experiment or introduce the mechanical visual mapping in section 10.

## 6. Second acoustic experiment: stop stationary survivors dominating

The strong body-warp term applies to pairs 1–6. Several higher retained pairs therefore receive only the much smaller warp terms. Some comparatively high-Q families can survive as the deformation cue weakens. [S1]

Useful table landmarks, before specimen variation and coupling, are:

| Pair | Dark-scaled bare frequency | Baseline dark-scaled T60 |
|---:|---:|---:|
| 3 | 233.7 Hz | 4.000 s |
| 4 | 284.5 Hz | 5.500 s |
| 5 | 446.9 Hz | 7.000 s |
| 6 | 584.7 Hz | 6.500 s |
| 7 | 769.2 Hz | 5.336 s |
| 8 | 957.8 Hz | 2.808 s |
| 9 | 1192.3 Hz | 1.690 s |

These are coefficient landmarks, **not** the measured resonances or decays of the assembled nonlinear spring. The cap, damping coupling, and time-varying coefficients matter.

The existing short-tail switch begins at pair 3. It is therefore not simply removing a remote high-frequency sheen; it also shortens lower audible families. This is a plausible reason that reducing the bell can also remove desirable twang. It is not evidence that the switch is always worse. [S1]

### 6.1 First identify the offending survivors

Export per-pair observations and raw pair states. Compare 0–0.2, 0.2–0.8, 0.8–1.8, and 1.8–3.5 seconds, with a consistent strike onset. Determine which pair observations remain audible but nearly stationary after the low-motion or spectral-flexing cue is no longer prominent.

Do not identify “bell” solely with all energy above a chosen cutoff. The user wants metallic twang; that requires some audible metallic structure.

### 6.2 Then test two independent treatments

**Treatment A: a small amount of deformation dependence on selected residual families.** Extend the body-warp envelope beyond pair 6 with a smoothly tapering sensitivity. Do not copy the largest low-family coefficient to every mode. An initial diagnostic search could constrain the additional high-family frequency excursion to approximately 3–10% at a strong reference bend, rather than allowing an unbounded or uniform sweep. Measure actual excursions after rendering.

**Treatment B: selectively shorten the stationary late tail.** First adjust only the identified survivors, holding body motion and the main low/mid twang families fixed. Candidate upper-family T60s around 0.6–1.5 seconds are an audition range, not a claim about the real specimen. Preserve other existing values until a stem or ridge diagnostic gives a reason to change them.

A subsequent experiment may make extra damping increase smoothly as a mechanical motion envelope decays:

`d_i = d_active_i + (1 - M) * d_late_i`

Here `M` is a bounded, slowly tracked envelope of relevant low mechanical energy, not instantaneous displacement and not the final audio envelope. Use the existing coefficients as the baseline and avoid extra damping that chatters twice per swing. This is perceptual engineering unless a specific dissipative mechanism is modeled; label it accordingly.

**Pass:** residual ringing becomes less detached while the initial and middle boing/twang remain.  
**Fail:** the result becomes a thud, loses medium-strike identity, or merely moves the bell downward in pitch.

## 7. Separate character controls before further tuning

`hardStrikeDrive` affects low stiffness, damping, observation, gain, and visual enlargement, while following an exponential release of about 2.2 seconds. [S1]

For an isolated low coordinate, the softening factor gives:

`frequency factor = sqrt(1 - 0.58 H)`

At `H = 1`, this is approximately 0.648. As `H` decays, that contribution raises frequency back toward the unsoftened value. Other strain terms produce their own motion. Do not call the net result a downward pitch glide without measuring it.

**Refactor without initially changing behavior:** calculate named contributions for low-frequency softening, low damping, body warp, observation blend, and output trim. Preserve the current shared mapping as a compatibility default. Allow analysis overrides for each contribution separately.

This provides an interpretable tuning experiment. A change to “more bend” should not silently also become “less damping, a different observer, a louder attack, and more visual magnification.”

Do not simply force a downward sweep: retain the existing motion as the baseline until a target recording or the approved voice establishes the appropriate relaxation direction.

## 8. Definite implementation issues to fix before trusting comparisons

### 8.1 Completed-pulse amplitude is reused

In `strike()`, pulse magnitude is accumulated. In the substep processor, the remaining count runs down but the completed pulse magnitude is not cleared. The dynamic state clear eventually removes it, but another strike while the engine is still active can add to the amplitude of an already completed excitation. [S1]

For nonoverlapping 0.50 strikes, with no intervening state clear, the source's pulse-amplitude arithmetic produces:

`507.5 -> 1015 -> 1522.5 -> 2030 -> 2200 -> 2200`

That is not just preserving the spring's stored energy. It changes the newly applied external force.

**Required fix:** preserve modal/cap/mount state across retriggers, but retire completed excitation pulses. For actual pulse overlap, use a small fixed-capacity pulse pool with signed directions, or an explicitly bounded equivalent policy. Starting a new pulse should not restart and reuse the entire completed pulse history.

**Tests:** equal requested strikes separated by more than the pulse duration have equal new applied impulse at fixed velocity and sample rate; alternating signs apply the intended signed excitation; overlapping pulses remain finite and bounded. Do not demand equal resulting audio from an already moving spring.

### 8.2 V3 ignores the advertised preconditioned selection

The renderer parses `--output-tap preconditioned`, but sets that option on the reference engine, not the helical engine. The V3 branch still takes the ordinary `outputVolts` result. The phase option likewise configures V2 rather than V3. [S5]

**Required fix:** provide explicit V3 taps or reject unsupported combinations. Prefer named taps with documented units:

- raw observer sum;
- DC-blocked, linearly scaled pre-limiter signal;
- final module volts;
- per-pair and mount observer components.

There must be no silently ignored option. Do not globally normalize a raw physical tap as though it were already ±5 V audio.

**Tests:** a deliberately overdriven trajectory demonstrates the limiter transfer between raw/pre-limiter and module taps; low-amplitude signals have the expected small-signal gain. Selecting a tap must not change the mechanical trajectory, sleeping behavior, or visual state.

### 8.3 Default audition targets omit later V3 contenders

The default V3 audition target includes probe, dark boing, and deep swing, but not the later continuum/body-bend/thick-spring variants. [S6]

Add an explicitly named V3 refinement target that includes the current preferred baselines and the actual experiment under evaluation. Write the branch/commit, seed, break-in, velocity, sample rate, observer configuration, and normalization window into the result metadata.

### 8.4 Existing spectral tests do not establish doorstop identity

`measureBodyBellEnergy()` compares a roughly low-passed body component against the difference of two low-pass signals spanning a broad midrange. Existing tests also emphasize boundedness, level growth, distinctness, visual activity, and short-tail reduction. [S7]

These are useful regression tests. They are not a perceptual classifier. Adding low-frequency energy or deleting useful midrange can improve the ratio without improving recognizability.

Retain safety tests. Separate them from acoustic acceptance. A passive physical strike can interfere with existing motion; requiring every harder retrigger to produce a larger output peak at every phase is a product-response policy, not a universal mechanical invariant.

## 9. Cap, contact, and energy: important mechanics caveats

These are real structural issues, but do not bundle all of them into the first timbre experiment.

### 9.1 The cap shifts the actual low modes

The dynamic equations effectively give each cap axis unit mass in the chosen coordinates, while attaching it to a modal sum. The two axes have different participation magnitudes. Thus bare pair frequencies and splits do not equal those of the assembled cap-plus-spring system. [S1]

The accompanying numerical probe builds the source's linear stiffness matrix for seed 77, dark tuning, zero break-in, continuum coupling, no contact, no damping, and frozen zero deformation warps. It includes the cap stiffness, but excludes the one-way mount because that mount does not change spring eigenfrequencies.

Its lowest calculated frequencies are approximately:

| Frozen character drive | Two lowest assembled frequencies |
|---:|---:|
| 0 | 16.52 and 19.00 Hz |
| 1 | 10.76 and 12.37 Hz |

The bare low coordinates are around 23.5–23.9 Hz before softening. These are frozen-linearization results, not a render or a claim that a hard real trajectory sits at either value.

**Consequence:** do not tune visual rate, lobe rate, or pair beating from the frequency table alone. Measure the trajectory. Do not “correct” cap mass immediately and inadvertently remove the approved deep motion.

A later normalization pass should explicitly define cap mass, modal effective mass, displacement scaling, and two-plane geometry, then recalibrate against the approved trajectory.

### 9.2 Contact reconstruction and projection are not reciprocal

The continuum contact coordinate is a short weighted sum of modal displacements, while its closing speed and its projected force use different participation assumptions. The non-continuum branch also adds a raw per-substep contact-force difference to higher modes. [S1]

This does not prove audible instability, but it means contact is not automatically an energy-consistent internal interaction. A raw sample-to-sample force difference is also timestep-dependent; do not “fix” it by blindly dividing by the timestep and injecting a potentially enormous derivative.

For a deliberately simple bend-clearance coordinate, choose one vector `a` and consistently use:

`z = a^T q`  
`delta = max(abs(z) - gap, 0)`  
`deltaDot = sign(z) * a^T v`  
`Q_contact = -a * sign(z) * F(delta, deltaDot)`

Use a nonnegative, unilateral contact magnitude with bounded dissipative behavior. This construction uses the same geometry for penetration, speed, and generalized force. Include whatever higher-mode participation is needed in the same `a`; do not apply force into unrelated coordinates afterward.

Start with one consistent contact coordinate as a control. Add several spatial gap vectors only when the single-coordinate experiment and actual target behavior justify it. Test contact-off, existing contact, and reciprocal contact at the same mechanics/observer settings.

The earlier document's contact-off requirement should not be a veto. It is a useful ablation, but the supplied material does not establish that contact must be absent or unimportant at medium excitation for the desired physical specimen.

**Sign caution when consulting the earlier design:** a gap written as `g = g0 + h^T q` has generalized repulsive force `+h F` for a positive force opening the gap. Do not copy a minus sign without checking the gap convention. The bend-clearance equations above use a different, explicitly defined convention.

### 9.3 The energy meter is not a passivity certificate

The current diagnostic energy does not include the full cap attachment potential, all nonlinear contributions, or the complete rotated/time-dependent stiffness energy. Treat it as an activity proxy. [S1]

Changing a stiffness coefficient in time can perform work. For a coordinate energy `E = (v^2 + k(t) q^2)/2`, the stiffness-change contribution is `0.5 * kDot * q^2`. Positive, bounded stiffness alone does not guarantee non-increasing energy.

If stronger body coupling is retained and expanded, a later consistent model can derive both high-mode stiffness and reciprocal low-mode force from a common potential. For low vector `l`, body vectors `q_p`, and a bounded function `h(r^2)`:

`U_body = 0.5 * sum_p [1 + lambda_p h(l^T l)] * q_p^T K_p q_p`

The corresponding body force is the negative gradient with respect to `q_p`. The additional low force is:

`Q_l,body = -h'(l^T l) * l * sum_p lambda_p q_p^T K_p q_p`

That reciprocal term is what prevents the low bend from serving only as an unaccounted parameter-modulation source. Any separately time-varying softening law still needs its own work accounting.

This is a mechanics refinement path, not a prerequisite for hearing whether the observer experiment helps.

## 10. Visual contract: preserve the good drawing, improve its state feed

`DoorstopWidget.cpp` already reconstructs a bounded constant-length arc, maps coil offsets along its local normal, rotates the cap, supports trails and overflow, and caches stable geometry. There is no reason to replace that drawing for these experiments. [S3]

The current engine's displayed displacement is not a raw mechanical measurement. It multiplies the low coordinate by an audible-output envelope and character-dependent gains. Thus an observer/EQ/gain change can alter the visible excursion despite identical underlying mechanics. [S1]

### Required separation

Publish raw or consistently normalized low mechanical displacement/velocity for diagnostic use. Feed the established drawing with a bounded mechanical mapping, calibrated to preserve the approved excursion and settling behavior. Keep audio activity available separately for brightness or an activity indicator.

A possible mapping is:

`displayX = Dmax * tanh(G * panelProjection(lowState) / Dmax)`

Choose `G` from the current approved visual trajectory, not from final audio level. For subpixel motion, use a smooth mechanical visibility curve rather than an unrelated animation oscillator. Do not expose all high modal coordinates as visible shaking.

If display velocity is intended as a derivative, differentiate the chosen mapping consistently. If it is only a motion-intensity cue, name and document it as such rather than asserting physical derivative units.

Preserve the energy-bar behavior in `DoorstopVisualFeedback.hpp` unless separately requested: it intentionally does not recharge between strikes. A monotonic display bar is a UI policy, not a substitute for mechanical energy accounting. [S8]

### Regression contract

- An observer-only or output-trim change must not alter raw mechanical state.
- With the new mechanical display mapping, it must not alter the displayed spring trajectory either.
- The same low state drives audible deformation cues and visible bending; there is no independently timed cosmetic oscillator.
- Large strokes, cap orientation, trails, optional overflow, cached idle rendering, and eventual rest remain intact.
- No geometry calculation or heap allocation is introduced into the sample loop.
- Existing telemetry cadence and thread-safe publication remain; diagnostics must not introduce data races.
- Check 30/60/120 Hz UI updates separately from audio sample-rate tests. Use actual simulated timestamps, not UI frame count, for motion phase.

## 11. Diagnostics and listening protocol

### Required diagnostic capture

Use a test-only callback, fixed-capacity observation structure, or offline renderer capture. Avoid file writing from Rack's audio thread.

Capture the low pair's raw positions/velocities, cap state, strain and body-warp values, character drive, contact coordinate/force, actual new applied force, pair observations, mount observation, raw observer output, pre-limiter output, and final volts.

For energy investigations, calculate full component energies and accumulated external work in a separate diagnostic path. Do not assert passivity from the current activity meter.

### Comparisons

First use seeds `1`, `77`, and `3076668551`, then the broader existing population. Freeze break-in. Test velocities 0.30, 0.50, 0.65, 0.80, and 1.00. Include positive/negative strikes and spaced/overlapping retriggers.

Use both a fixed-gain comparison, which preserves dynamic response, and an externally level-matched comparison, which isolates timbre. Match a specified onset-relative active window and apply that gain to the complete clip; do not let extra silence in a long render choose the gain. Record the method.

Compare a chosen thick-spring target subgroup first, then the full corpus for robustness. The existing manifest has multiple recordings and onset windows, but this review has not auditioned those files and does not label any one of them the definitive specimen. [S9]

### Better acoustic measurements

Track low-motion cycles from raw mechanics. Track output modulation separately by frequency band. Measure within-cycle ridge movement, lobe width and phase, inter-lobe continuity, and late stationary-ridge energy. Distinguish frequency modulation of a mode from amplitude-modulation sidebands introduced by the observer.

Use short windows for onset/within-cycle evolution and longer windows for late ridge structure. Do not use one FFT setting to answer both questions. Record uncertainty when a ridge cannot be tracked through a contact transient or low-SNR tail.

Avoid replacing the listening decision with a new scalar score. The decisive question is whether the result sounds like a heavy coiled doorstop, not whether it became spectrally darker or more inharmonic.

## 12. Implementation order and exit conditions

### Stage A — Freeze baselines and repair the test harness

Save existing baseline renders and visual traces before behavior changes. Fix retired-pulse reuse and implement real V3 output taps. Add the correct refinement audition target. Preserve old variant serialization. Verify unit tests; explicitly document any retrigger tests whose old result relied on stale pulse accumulation.

**Exit:** every comparison identifies its true output tap, excitation, seed, state, and variant. Nonoverlapping repeated strikes do not reuse completed external-force amplitude.

### Stage B — Observer only

Run the three/four observation treatments on identical mechanics and a frozen or mechanical-only visual feed. Keep current modal frequencies and decay values.

**Exit:** a blind preference or a clear negative result about revealing pitch flex at the bend. No new physics layer is added merely because the first candidate fails.

### Stage C — Residual mode motion and decay

Identify stationary survivors. Test modest added deformation sensitivity and selected tail shortening independently, then combine only successful treatments. Preserve the body families that carry the preferred sound.

**Exit:** the metallic tail no longer detaches perceptually, without replacing twang with a thud. Soft and medium strikes remain useful.

### Stage D — Mechanics consistency

Run reciprocal-contact and cap-normalization experiments separately. Add correct work/energy diagnostics. Introduce conservative reciprocal body coupling only with comparison to the approved audible/visual trajectory.

**Exit:** any claimed physical improvement has both numerical support and no unacceptable perceptual regression. More formal mechanics is not automatically a better sound.

### Stage E — Production

Benchmark at 44.1, 48, 88.2, 96, and 192 kHz, plus stress retriggers. Include comparisons across substep counts for the revised contact law. Check deterministic seeds, finite output, proper settling, state transitions, patch compatibility, and unchanged idle-rendering cost. Only promote a new default after the listening decision.

## 13. Minimal Codex task statement

> Work on `ds-v3-refinement`. Preserve `DeepSwing` and `DeepBodyBend` as baselines; do not replace the spring widget or retune all modal frequencies. First fix reuse of completed strike-pulse amplitude and add genuine V3 raw/pre-limiter/module output taps, with tests. Add analysis-only overrides separating the existing velocity-character mapping, body warp, observer width/phase mix, and selected pair damping. Capture low mechanical state and per-pair observations. Run observer-only comparisons on identical mechanics, then identify and treat stationary late survivors without blanket short-tail damping from pair 3 upward. Keep visual bending driven by the low mechanical state rather than final output amplitude, while preserving the existing approved drawing and motion scale. Treat contact reciprocity, cap mass normalization, and conservative nonlinear coupling as separate experiments, not bundled changes. Document changed behavior and evidence; do not declare authenticity from a body/bell energy ratio.

## 14. Source map

Source links target the requested branch, which can move. The reviewed branch history listed `7c433cf` as its newest commit. Function names are the durable navigation landmarks.

- **S1:** [`src/HelicalContinuumEngine.cpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/src/HelicalContinuumEngine.cpp). Modal arrays; `updateSpecimenCoefficients`, `strike`, `processSubstep`, `calculateEnergy`, `process`.
- **S2:** [`src/HelicalContinuumEngine.hpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/src/HelicalContinuumEngine.hpp). Variant enum, member state, feature predicates.
- **S3:** [`src/DoorstopWidget.cpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/src/DoorstopWidget.cpp). `buildSpringGeometry`, cached rendering, V3 context-menu labels.
- **S4:** [`src/Doorstop.cpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/src/Doorstop.cpp). Input velocity normalization, process/telemetry publication, persistent selection.
- **S5:** [`tools/doorstop_reference_render.cpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/tools/doorstop_reference_render.cpp). Option parsing and reference/helical dispatch.
- **S6:** [`Makefile`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/Makefile). Renderer build and audition targets.
- **S7:** [`tests/doorstop_helical_engine_spec.cpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/tests/doorstop_helical_engine_spec.cpp). Body/bell filters, velocity/retrigger expectations, visual and short-tail tests.
- **S8:** [`src/DoorstopVisualFeedback.hpp`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/src/DoorstopVisualFeedback.hpp). Monotonic meter display policy.
- **S9:** [`tools/doorstop_reference_manifest.json`](https://github.com/PlasmaChroma/Leviathan-Rack2/blob/ds-v3-refinement/tools/doorstop_reference_manifest.json). Recording paths and audited onset windows.
- **S10:** User attachment, **Doorstop Reference V3 Design**, August 26, 2026, original target `expander`. Especially sections 5, 8, 10–12, and 17. This is prior design guidance, not a measured verdict on the present variants.
- **S11:** [`Branch commit history`](https://github.com/PlasmaChroma/Leviathan-Rack2/commits/ds-v3-refinement/), read September 4, 2026.

## 15. Reproducibility files

The accompanying `formula_probe.py` and JSON output reproduce the numerical tables and frozen cap-plus-mode eigenvalue calculation. Python 3 and NumPy are required. They do not fetch source, compile C++, render the nonlinear engine, model all coupling effects, or evaluate audio quality. Constants were transcribed from the reviewed source; verify them against a newer branch before reusing the results.
