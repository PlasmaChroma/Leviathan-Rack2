# Vessel DSP optimization audit — 2026-10-09

## Scope and status

Target: `PlasmaChroma/Leviathan-Rack2`, `expander` branch, particularly `src/vessel/` and `src/Vessel.cpp`. The branch was inspected through GitHub raw-source responses on the date above. This is **not a commit-pinned checkout**; compare the current source before integration.

This bundle contains isolated candidate kernels, reproducible mathematical checks, candidate FIR coefficients, and a standalone streaming FIR microbenchmark. **It is not a patch to Vessel.** The full Rack module was not compiled or benchmarked, the repository's tests were not run, and no end-to-end audio corpus or listening test was performed. No repository files were changed.

The source already has substantial optimization: SSE2 active modal updates, fused state update/finite checks/pickups, cached small-angle contact-orbit rotations, a symmetric SSE2 stereo decimator, single-engine operation at zero binaural separation, and host-rate composition of eligible passive tails. These are baselines to retain, not new recommendations.

## Current checkout reassessment — after the 109-tap trial

The original bundle below is historical. Subsequent integrated work retained
the four-accumulator 129-tap FIR, isolated-strike derivative caching, and
observer-only configuration reuse. The coupled derivative was rejected after
mixed/regressing callback measurements. The 109-tap filter remains experimental:
small tested audio differences, but only 0.51–0.66% median whole-module savings,
some regressions, and a +1.41 dB response change at 20 kHz on oversampled
44.1 kHz paths. See [its validation report](../vessel_fir109_validation.md).

Reinspection of the current source gives this order:

| Priority | Remaining lead | Evidence and next decision gate |
| --- | --- | --- |
| Deferred | Matched-pole configuration transform | `ModalBank::configure` still computes the original exp/expm1/trig/hypot mapping. First measure full pitch-modulation callbacks and configuration cost; then compare the algebraic candidate independently. Target configuration spikes, not steady rubbing. |
| Completed | Friction-law work and arithmetic | Profiling led to retained per-solve reciprocals and cubic tanh. Final full-callback medians improved 3.05% for ordinary rubbing and 16.85% for slow Felt, with one small ordinary regression. |
| Deferred with retuning | Cache pitch-independent physical terms | Pitch-only updates still recompute split factors, masses, decay terms and contact footprint terms. Isolate this from the transform experiment; require correct invalidation for material/mallet morphs, decay, imperfection and rate changes. Preserve orbit resynchronization. |
| Not retained | Gaussian Hermite lookup | Tested independently and with tanh; gains were weaker/mixed. Gaussian remains analytic. See the fast-friction report before repeating these trials. |
| Not retained | Exact control/meter math caches | Byte-identical 256,000-frame trace, but final paired active-callback medians were +0.07% standalone and -0.23% with V.Tune. Patch remains offline; see [the control-math experiment](../vessel-performance.md#control-math-cache-experiment-2026-10-09-not-retained). |

The matched-pole standalone check was rerun locally: 20,020 configurations,
maximum relative omega/sigma difference 8.23e-16 and maximum absolute inverseD
difference 4.44e-16. This reproduces the original coefficient result; it does
not yet measure integrated speed or prove trajectory equivalence. The current
`setAdditionalDamping` path must remain coherent: `inverseD = Q/4` applies only
to the base mapping, before extra damping. Compare mode frequency/T60, long
tails, active-contact retuning, high-energy damping and transactional rejection
against the original implementation before retaining a transform change.

Prepared mallet validation and contact-loop fusion already had mixed or
inconclusive **native Windows** results in [the earlier investigation](../vperf-10-9-26.md).
Those tests did not exhaust reciprocal caching or coupled-contact preparation,
but repeating the same variants without new profiling evidence is low priority.
Likewise, defer 105-tap FIR work until shorter-filter tradeoffs are worthwhile;
the 109-tap trial does not establish that 105 taps cannot help, but lowers its
priority. Lower-rate mechanics remains a separate quality-mode investigation.

Pitch/retuning optimization is now explicitly deferred: fixed tuning and free
V/oct modulation remain supported, with retuning acoustics to be revisited
separately. The first friction profiling pass is complete; see
[the contact-work report](../vessel_contact_work_profile.md). Across 96 fixtures,
Gaussian `exp` evaluation ran on every law call, while `tanh` use varied strongly
with rubbing speed. Existing Newton convergence was efficient. The subsequent [fast-friction experiments](../vessel_fast_friction_validation.md)
retained reciprocals plus cubic tanh after numerical and full-callback checks;
Gaussian lookup remains offline.
Keep counters out of timing builds and retain normal debug Process timing.

## Original recommended order

1. Establish an actual callback baseline, including timing distributions and total constitutive-law evaluations during overlapping strike/rub contacts.
2. Preserve the existing 129-tap filter and try four independent accumulators. Cache invariant physical parameters and exact repeated products; split observer-only changes from full mechanical configuration.
3. Test the 109-tap FIR candidate as the conservative shorter-filter option. Test 105 taps separately when the small change near 20 kHz is acceptable.
4. Integrate the stable strike-gradient derivative into both isolated and coupled strike solvers, preserving all bracketing and recovery behavior.
5. Test prepared friction parameters and rub-only loop specialization. Benchmark the repository's existing friction approximation experiments before selecting approximations.
6. Consider 48 kHz as a separately validated quality mode, not an assumed equivalent implementation of the 96 kHz contact mechanics.

## Files

- `numerical_checks.py`: rebuild FIR coefficients and frequency-response metrics; validate the strike derivative against 85-digit arithmetic; calculate mode-rate limits and small-damping compliance ratios.
- `numerical_results.json`: results from that script.
- `FirCoefficients.hpp` and `*.csv`: original-formula 129-tap reference and equiripple candidates.
- `StrikeGradientCandidate.hpp`: cached discrete-gradient force and cancellation-free derivative for nonnegative starting compression.
- `MatchedPoleCandidate.hpp`: half-angle matched-pole coefficient transform, for zero additional damping.
- `candidate_checks.cpp` / `candidate_check_results.json`: compare the candidate matched-pole transform against the existing formula and compare cached strike forces against the existing expression.
- `fir_microbenchmark.cpp` / `fir_benchmark_results.json`: isolated x86/SSE2 symmetric streaming 2x stereo decimator, comparing one accumulator with four and alternative tap counts.
- `environment.json`: machine/compiler context and limitations.

## Reproduction

Python dependencies: NumPy, SciPy, and mpmath. Run from this directory:

```sh
python3 numerical_checks.py

g++ -std=c++11 -O3 -march=nehalem \
  -fno-fast-math -fno-unsafe-math-optimizations -ffp-contract=off \
  -Wall -Wextra candidate_checks.cpp -o candidate_checks
./candidate_checks > candidate_check_results.json

g++ -std=c++11 -O3 -march=nehalem \
  -fno-fast-math -fno-unsafe-math-optimizations -ffp-contract=off \
  -Wall -Wextra fir_microbenchmark.cpp -o fir_microbenchmark
./fir_microbenchmark > fir_benchmark_results.json
```

The microbenchmark uses SSE2 intrinsics and is x86-specific. The two candidate mathematical headers are ordinary C++11. Substitute platform-appropriate build flags rather than treating `-march=nehalem` as a deployment requirement. Keep strict floating-point behavior when comparing to the oracle. Timing on a shared virtualized/container machine is noisy; rerun on the actual audio machine with the production toolchain.

## FIR candidates and interpretation

The current formula is a normalized symmetric 129-tap Blackman-windowed sinc with normalized cutoff 0.225. It is **not** a sparse true-halfband filter. It already exploits symmetry, mirrored history, output-phase-only evaluation, and SSE2 left/right lanes.

Measured bands are 0–0.2 times the input rate for passband and 0.25–0.5 for stopband. At 96 kHz input these mean 0–19.2 kHz and 24–48 kHz. The scripts include exact band edges in addition to a 1,048,576-bin grid.

Candidates 81/93/101/105 use Remez with bands `[0, .2, .25, .5]`, desired gains `[1, 0]`, weights `[1, 1.82]`, and DC normalization. The **109-tap candidate** uses a wider passband: bands `[0, .2045, .25, .5]`, weights `[1, 3]`, again normalized at DC. This protects the response near 20 kHz better than the 105-tap design.

Approximate results; the JSON contains full precision:

| Filter | Symmetric products/channel/output | Passband span (dB) | Worst stopband (dB) | Mean stopband power (dB) | Gain at 20 kHz, input 96 kHz (dB) |
|---|---:|---:|---:|---:|---:|
| Existing-formula 129 | 65 | 0.002465 | -75.29 | -90.65 | -0.0554 |
| Candidate 109 | 55 | 0.002018 | -88.23 | -91.26 | -0.0493 |
| Candidate 105 | 53 | 0.000978 | -90.18 | -93.21 | -0.2020 |
| Candidate 93 | 47 | 0.002327 | -82.65 | -85.68 | -0.2600 |

“Mean stopband power” is the mean squared transfer magnitude across the defined stopband, expressed in dB. It models uniformly distributed stopband input power; it is not the alias power of an actual bowl signal. These are design metrics, not pointwise dominance over the entire spectrum or proof of perceptual equivalence. The transition band changes, as does delay.

The conservative 109-tap candidate reduces symmetric products by 15.38%. The 105-tap candidate reduces them by 18.46%; 93 taps reduces them by 27.69% but has **worse aggregate stopband leakage** than the reference despite better worst-case rejection. Do not select the 93-tap design on its single worst-stopband number alone.

The source's output-tag delay convention gives 31.5 host samples for 129 taps at 2x, 26.5 for 109, 25.5 for 105, and 22.5 for 93. This is distinct from simply dividing the FIR's input-sample group delay by two; account for decimator output phase.

### Integration requirements

- Keep output cadence and coefficient symmetry consistent.
- Replace the hardcoded `128` in `DualBowlAdapter::configure()` tail-history sizing with the appropriate expression based on `StereoDecimator::taps - 1`.
- Review all uses of tap count, equivalent FIR construction, delay, warmup, passive-tail capture, and tail reentry/handoff.
- Test factors 1, 2, 4, and 8, and host rates 44.1, 48, and 96 kHz. The final-stage passband at a 44.1 kHz host is different in absolute hertz.
- Different tap counts require latency-aligned audio comparisons. A changed filter is not bit-exact.
- Four accumulators preserve coefficients but change floating-point summation order. Existing exact-bit FIR tests must become an explicit two-level suite: the retained strict oracle plus tightly bounded candidate comparison.
- The FIR is outside the physical feedback loop. This makes reassociation safer than changing the reductions used to determine implicit contact force. External patched feedback can still react to the changed output delay.
- Once an eligible passive tail uses the composed host-rate path, the streaming FIR is bypassed. Do not expect a shorter filter to accelerate that already-composed steady tail.

The standalone random-input test found a maximum difference of `4.4408920985e-16` between one- and four-accumulator evaluations of the same coefficients. This is a finite test, not a universal error bound. Timing results in the JSON are **not Vessel callback timings**.

## Strike derivative: derivation and integration

For starting compression `d0 >= 0`, candidate end compression `d1`, stiffness `k`, and potential

`V(d) = (2/5) k max(d, 0)^(5/2)`,

the force is the discrete gradient

`G = (V(d1) - V(d0)) / (d1 - d0)`.

The current force already has a cancellation-free factorization when both compressions are nonnegative. The opportunity is its derivative: the current solver recomputes a discrete gradient and subtracts nearly equal forces, with a separate near-equality approximation.

Let `a = sqrt(d0)`, `b = sqrt(d1)`. Then

`G = (2k/5) (b^4 + a b^3 + a^2 b^2 + a^3 b + a^4) / (a+b)`,

and the exact partial derivative is

`dG/dd1 = (k/5) (3b^3 + 6ab^2 + 4a^2b + 2a^3) / (a+b)^2`.

At `a=b>0`, this is `(3/4) k a`; at `a=b=0`, both force and derivative are zero. For `d1<0`, use `G=V(d0)/(d0-d1)` and `dG/dd1=V(d0)/(d0-d1)^2`.

Cache `sqrt(d0)`, `V(d0)`, and stiffness outside the trial loop. Return the force and derivative from the same evaluation and reuse them for residual/Newton calculations in both `solveStrike()` and `solveContacts()`. The header preserves the current positive-compression force expression's order.

Validation performed: 4,804 cases against 85-digit arithmetic, including equal, nextafter, near-equal, negative-end, and endpoint cases. Maximum relative errors were below `6e-16`. A separate 100,000-case C++ force comparison had zero differing values under the recorded strict flags. This does **not** prove full-solver trajectory equivalence or a CPU speedup. Keep all safeguarded brackets, finite checks, no-unconverged-commit behavior, retirement rules, and ledger tests. The improved derivative can change iteration paths.

## Matched-pole transform

Let `h=1/F`, `sigma0=log(1000)/T60`, `theta=2*pi*f*h`, and `r=exp(-sigma0*h)`. Define

```
d = -expm1(-sigma0*h)
r = 1-d
s = sin(theta/2)
c = cos(theta/2)
P = d*d + 4*r*s*s
Q = d*d + 4*r*c*c
```

Then the current zero-extra-damping coefficients can be generated as

```
a = sqrt(P/Q)
omega = 2*a/h
sigma = (2/h)*d*(2-d)/Q
inverseD = Q/4
admittanceWeight = (h/2)*inverseD
```

This is algebraically equivalent, not bit-equivalent, to the current transform. It removes a separate `exp`, the `hypot(sigma, nu)` construction, and a reconstructed denominator reciprocal. Do not use inaccurate direct subtraction `1-r*r` at very long T60. The inverse-denominator identity applies **only before additional damping**. Preserve the existing coherent additional-damping update.

Across 20,020 configurations, the maximum relative omega/sigma difference from the current expression was about `8.23e-16`, and maximum absolute inverseD difference was `4.44e-16`. This is a local coefficient check, not a long-tail or energy-ledger test.

These calculations are cached at configuration boundaries, not executed per mode per internal tick. Their benefit is strongest during pitch/control modulation and callback spikes. Separate observer-only stereo-width changes from a full mechanical-bank rebuild first. Observer changes still require appropriate filtered-observer/history handling.

## Friction and callback opportunities

For the friction residual `R(F)=F-phi(delta-YF)`, `R'(F)=1+Y phi'(slip)`. The current uniqueness certificate bounds this derivative below by 0.1. Therefore, for the certified law, the force error is at most ten times residual magnitude. This gives an error-budget framework, not permission to arbitrarily loosen tolerance.

Prepare immutable mallet parameters once, including validated descriptor fields and useful reciprocals. Keep dynamic finite/range checks at the step boundary. Preserve a public fully checked solver for tests and untrusted inputs. Measure whether the compiler already removes duplicate work.

The coupled solver's `innerIterations` records the maximum single inner solve, **not their sum**. Add total friction-law evaluations, bracket expansions, Newton steps, and fallback steps to dedicated profiling builds. Warm-starting successive inner trials is a possible next experiment, but retain all safeguards and compare event costs and sound.

The repository already has Hermite approximation experiments for exp/tanh; do not add another unvalidated generic fast-tanh shortcut. Any replacement needs consistent force/derivative evaluation and a renewed negative-slope certificate. Preserve near-zero regularization, which affects sticking/startup.

A rub-only path can fuse free-midpoint and port reductions, avoid unnecessary temporary arrays, and specialize force injection. Keep scalar reduction order first. Cache `h * omega[j]` using the exact existing expression at configuration time rather than deriving it through a differently rounded `2*a`.

At host rate, consider load-before-exchange for the mostly-false reset atomic, change-only writes to expander-retained parameters, and diagnostic histogram gating. Do not indiscriminately downsample gates or velocity inputs. Keep production callback measurements separate from per-sample debug timing and atomic telemetry. Do not enable global fast-math or remove finite checks.

## Why 48 kHz is not simply the same mechanics at half cost

The current mode transform already matches linear modal frequency and T60 across sample rates. The main differences are contact impedance/admittance and nonlinear aliasing, not an elementary oscillator pitch error.

The source requires every mode to lie strictly below `0.4*internalRate`. Using the supplied highest mode ratios and default imperfection, with zero binaural separation:

| Bowl | Approximate maximum center at 48 kHz | At 96 kHz |
|---|---:|---:|
| Metal | 1058.85 Hz | 2117.71 Hz |
| Crystal | 823.08 Hz | 1646.16 Hz |

These are modal-band constraints, not guarantees that every other configuration certificate passes. Nonzero binaural separation raises the upper bowl frequency; account for it. Default quality is nominally 96 kHz at a 48 kHz host, but high crystal fundamentals can already force 192 kHz. Always record the actual internal rate.

For small damping, the mapped oscillator frequency is approximately `2*F*tan(pi*f/F)`. Relative static compliance is therefore approximately `(pi*f/F / tan(pi*f/F))^2`. At a 10 kHz mode this is about 0.728 at 48 kHz versus 0.929 at 96 kHz. This is a mechanical-compliance ratio, **not output attenuation or pitch detuning**.

A post-filter cannot remove aliasing already folded into the audio band by lower-rate nonlinear contact. Nor is simply deleting ultrasonic modes necessarily harmless: those modes participate in contact admittance and coupled force. A contact-only oversampling scheme would need a passive multirate coupling, not a frozen 48 kHz bowl with a 96 kHz tanh evaluation.

The source's existing passive-tail composition is the favorable hybrid: preserve the high-rate mechanical trajectory while composing multiple linear steps and the output observer into one host-rate step. Keep that optimization and target the remaining active-contact costs.

## Full-module acceptance matrix

Run the repository's own test and benchmark targets before and after each independent change. Existing targets include `make test-vessel` and `make build/tools/vessel_benchmark_active_module`; the latter requires a working Rack SDK/environment.

Cover all four mallets and both bowl materials, center frequencies near 100/261.6/800/1500/2000 Hz, zero/nonzero binaural separation, several host rates, all quality modes, sustained rubbing, cold startup, hard/soft impacts, overlapping strike/rub, stopping/restarting contact, high-energy release, and long quiet tails. Time slow and fast pitch/control modulation, not only static cases. Record actual internal rate, callback mean/median/p95/p99/p99.9/max, configuration calls, total law evaluations, and solver/recovery faults.

Use deterministic stimuli. Compare the strict oracle first for state/energy-ledger behavior; compare candidate sound through latency-aligned recordings, mode frequencies/T60, attack envelopes, integrated band energies, startup success, and alias-sensitive spectra. Sample-by-sample long-term nulls are informative but insufficient for nonlinear trajectories that can diverge while remaining perceptually similar. Validate with listening as well.

## Source map

Repository base: https://github.com/PlasmaChroma/Leviathan-Rack2/tree/expander

Relevant inspected paths:

- `src/Vessel.cpp`, `src/Vessel.hpp`
- `src/vessel/VesselEngine.cpp`, `src/vessel/ModalBank.hpp`, `src/vessel/ModalBank.cpp`
- `src/vessel/FrictionContact.cpp`, `src/vessel/StrikeContact.cpp`, `src/vessel/ContactSolver.cpp`
- `src/vessel/HostRateAdapter.cpp`, `src/vessel/HostRateAdapter.hpp`, `src/vessel/DualBowlAdapter.cpp`
- `src/vessel/PassiveTail.hpp`, `src/vessel/PassiveTail.cpp`, `src/vessel/SeedProfiles.hpp`
- `tools/vessel/benchmark_active_module.cpp`, `tools/vessel/experiments/FrictionApproximation.hpp`
- `tests/vessel_host_rate_spec.cpp`, `tests/vessel_fast_paths_spec.cpp`, `Makefile`

Filter-design algorithm reference: SciPy official `scipy.signal.remez` documentation at https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.remez.html . This bundle's candidate choices and measurements are original numerical work, not claims made by that documentation.
