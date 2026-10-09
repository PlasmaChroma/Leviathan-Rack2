# Vessel performance plan

Date: 2026-09-30  
Status: the exact fused tail step is adopted; the rate selector remains a development experiment. Friction approximations remain offline; lower-rate fidelity is not yet approved.

## Objective

Reduce Vessel's processing cost substantially while retaining its evolving bowl character, responsive strikes/rotation, tuning accuracy and independent binaural motion. Small audible differences are acceptable for evaluation; a change in sound must be measured and auditioned before becoming the default.

The user originally reported 8–9% on Vessel's **per-module** Rack CPU meter and still considers the optimized module expensive. The user subsequently auditioned the rate selector: CPU cost drops nearly by half per downward rate step in the exercised patch; 48 kHz may be usable, with glitching reported while changing pitch. This is qualitative live feedback, not a controlled timing or fidelity result. There is no new numerical live-meter baseline yet. Offline DSP savings must not be presented as measured Windows/Rack savings.

## Current baseline

- Seven mode pairs per seed bowl, double-precision mechanics and implicit contact solving.
- Reference internal rate at least 176.4 kHz: 48 kHz host uses 192 kHz internally; 44.1 kHz uses 176.4 kHz.
- Cascaded 129-tap FIR decimation, now using mirrored storage and stereo SSE2 where available.
- Cached modal coefficients and orbit increments, contiguous hot coefficient arrays, and transactional tuning preparation that avoids copying FIR histories during ordinary retuning.
- Zero binaural separation folds to one bowl after the existing smoothing/fade. Nonzero separation retains two independent mechanical states.
- Unchanged controls already avoid repeated full configuration. They do not stop the mechanical simulation.
- Eligible unforced tails now use a fused internal-rate modal/pickup step with unchanged filtering.

The latest O3 Linux comparison measured approximately 5–7% lower single-bowl tail cost, 3–4% lower rubbing cost, and 49%/31% lower single/dual pitch-configuration cost from the most recent changes. All 26 Vessel test groups passed; a 1.62-million-frame before/after comparison matched serialized audio and energy records byte for byte. Full Linux linking passed. Native Windows/live Rack measurements remain open.

See [implementation status](doc/Vessel/Implementation_Status.md) for conditions and historical results. The current engine is a reference implementation, not a fully calibrated physical or perceptual ground truth; higher-rate convergence remains relevant.

## Recommended order

| Priority | Work | Expected benefit | Main qualification |
|---|---|---|---|
| 0 | Capture reproducible whole-module timing | Distinguishes DSP cost, control updates and spikes | Required before attributing live savings |
| 1 | Compare a lower internal-rate variant | Largest straightforward opportunity during active contact | Lower rate can change contact mechanics and aliasing |
| 2 | Build a dedicated fixed-parameter free-tail path | Large potential saving while bowls ring without contact | Must preserve filtering and seamless re-entry |
| 3 | Approximate friction functions with bounded error | Reduces expensive evaluations during rotation | Force and derivative must agree |
| 4 | Relax solver convergence using measured error budgets | May reduce iterations in difficult contact cases | Do not replace convergence with a hard iteration truncation |
| 5 | Prototype periodic geometry caching | Possible additional benefit during steady rotation | Existing recurrence is already inexpensive; measure first |
| Conditional | Move immutable preparation to a worker | Reduces configuration spikes | Little benefit to a stationary patch's steady DSP cost |

Implement and compare each experiment independently before combining winners. Retain the reference implementation for regression renders and auditions. Numerical targets below are **proposed engineering gates**, not established perceptual thresholds or promised savings.

## 0. Establish the live budget

Extend `tools/vessel/benchmark_dual.cpp` or add a Rack-linked companion harness to exercise the complete `Vessel::process()` callback, including controls, configuration and telemetry. Keep the existing engine-only benchmark for attribution.

Measure resting/sleeping, passive tail, sustained rubbing, repeated strikes while rubbing, pitch sweeps, material morphs and binaural fold/wake. Test both one and two bowls. Report average processing time and block-duration distributions, including p95/p99/max and any deadline misses. Separate average cost from occasional retuning/wake spikes.

Use release compiler flags, fixed sample rate/block size, identical patches and alternating before/after order. Record CPU, compiler, build flags, debug state and host settings. Capture the native Windows module meter under the same settings when available. Measure many instances as well as one; do not multiply a single-instance average and call it a proven capacity result.

When using Debug Terminal, retain the repository's every-callback Process measurement and Process/Step/Draw/DrawLayer reporting contract. Record instrumentation overhead separately. Worker CPU, if introduced, must be reported alongside callback CPU.

## 1. Lower internal rate: first controlled audio experiment

### Implementation

Add an explicit internal quality policy to `HostRateAdapter`, initially available to offline tooling. Preserve the present policy as Reference. Prototype an approximately 88.2/96 kHz policy: 44.1 kHz host uses 2x and 48 kHz uses 2x; never attempt to run below the host rate. At 48 kHz this halves the number of mechanical steps, but does not guarantee a 50% whole-module saving.

Do not merely replace the minimum-rate constant. Determine eligibility from the highest retained modal frequency, descriptor, host rate and contact behavior. Preserve the existing structural bandwidth validation. At high base frequencies, especially Crystal, the lower rate may reject modes or produce excessive compliance error; fall back to Reference rather than silently dropping modes. The current specification's provisional mechanical accuracy condition is stricter than the existing `0.40 * internalRate` validity limit.

Start with fixed quality per render/session. Automatic changes between striking and rubbing should follow only if the fixed-rate comparison succeeds. Dynamic rate changes need hysteresis, minimum dwell time and a transition that handles filter delay, solver coefficients and ongoing contact. The current adapter assumes factor changes occur with host-rate changes; same-host-rate quality changes must explicitly reconfigure the decimator and transition state.

### Validation and decision

Compare onset time, sustained RMS, dominant modes, brightness, beating, strike contact duration/rebound, slip/force traces, solver iterations, faults and energy residuals. Inspect aliasing against a finer-rate render with suitable offline downsampling; comparisons to the current reference alone cannot establish alias rejection.

Provisional screening targets: free-mode tuning within 0.1 cent and T60 within 1%; sustained level within 0.5 dB and onset within 5% in repeatable fixtures. Exceeding these triggers review, not hidden normalization. Nonlinear phase divergence makes sample-null error an inadequate quality metric.

Advance only if the variant delivers a substantial measured active-contact saving—initial target at least 25%—and the user accepts dry, level-matched auditions. Retain Reference for cases that fail. Do not make reduced quality the default solely because the solver remains stable.

## 2. Dedicated free-tail path

### Eligibility

Both strike and rubbing contact must be absent, including the engagement ramp and any prescribed radial forcing. Pitch, decay, material and other relevant coefficients must be settled. New strikes, rotation, modulation, rate changes or descriptor changes exit this path immediately at the correct host event boundary.

### Implementation

For each fixed, unforced mode, precompute the composition of its internal transition over one host interval. Advance its mechanical state directly at host rate. This preserves the mathematical fixed-parameter evolution up to rounding, without stepping the full contact engine several times per output sample.

Treat audio reconstruction as part of the design. Do not simply read the host-rate state and bypass the FIR: that changes gain/phase and can alias high modes. Prototype an analytic filtered modal output plus the finite transient from the existing FIR history, or another equivalent state-space realization. Account for every decimator stage's phase and latency.

Maintain enough state to resume the full reference path correctly. A transition must not require guessing stale FIR history. First optimize a single bowl; then support each binaural bowl independently and test interaction with the existing single/dual fades.

### Validation and decision

Compare full filtered output against the stepped reference for long tails, changing observation width, near-Nyquist modes, strikes at arbitrary host frames and repeated enter/exit cycles. Require continuous phase, correct tuning/decay and no event displacement. Proposed tail-only null target: residual RMS at least 100 dB below the reference where its level is measurable. Very quiet tails also need an absolute error check to avoid meaningless relative ratios.

Aim for at least 40% lower eligible-tail cost before accepting the extra state/transition complexity. This path provides no steady rubbing saving and must be reported separately.

## 3. Faster friction evaluation

`FrictionContact.cpp` evaluates Gaussian weakening and regularized `tanh` repeatedly inside the root solver. Prototype bounded rational/polynomial approximations or shared tables of these normalized functions.

- Evaluate approximation error over both arguments' active domains, concentrating resolution near the narrow zero-slip region. Preserve odd/even symmetry and the existing saturation behavior.
- Compute force and its derivative from a consistent interpolant or formula. Independently approximated derivatives can undermine Newton convergence.
- Preserve nonnegative frictional dissipation, a verified negative-slope bound and the uniqueness margin. Recompute or conservatively enlarge certificates if the approximation changes their bound.
- Start with normalized shared tables independent of pressure, pitch and bowl state. Keep double precision for the solver and state initially; do not combine this change with a float conversion.
- Keep the precise evaluator as an oracle. Retain finite-state checks, bracketing and fallback behavior.

Proposed local force-error target: below `1e-5 * max(1 N, load * muS)` across the tested domain, with separately reported derivative error. This is a starting point for the sweep; long-run behavior and listening determine acceptance. Measure total callback improvement as well as time per function evaluation.

Afterward, consider a validated internal solver entry point that skips repeated validation of immutable mallet properties. Public/untrusted entry points must retain validation, and dynamic slip/load/state checks must remain.

## 4. Solver error budget

The friction solver currently uses a residual tolerance of `1e-12 + 1e-11 * max(1, force scale)`. Sweep tolerance multipliers of 10, 100 and 1,000 independently of the friction approximation. Treat strike-only, friction-only and coupled solving separately.

Collect iteration histograms, cumulative energy residuals, onset statistics and spectra, including near-zero speed, direction reversal, high pressure and hard impacts. Preserve force brackets, convergence checks and maximum-iteration failure handling. A failed solve must not be committed just to meet a CPU budget.

Use force residual and timestep to derive a conservative local work-error estimate; track accumulated error rather than assuming a small force error is automatically inaudible. Choose the smallest relaxation that produces useful savings. Reject settings that add sustained hiss, alter sticking behavior materially, cause tail growth or increase recovery events.

## 5. Cache geometry, not an assumed repeating sound

Constant knobs do not imply a periodic complete state. Split modal frequencies, contact dynamics and binaural differences produce evolving beating, so one rotation's audio is not generally reusable as the next rotation's output.

A feasible experiment is a periodic table of contact-port geometry indexed by angle. Build it when the bowl geometry or mallet footprint changes, and interpolate during rotation. Compare 512/1,024/2,048 phase entries before selecting a size. Table resolution must account for the highest azimuthal order, not merely the rotation speed.

Use the same interpolated port for force injection and velocity observation. Compute admittance consistently from that port; independently interpolating an admittance table can break the collocated relationship that the energy argument relies on. Pressure and instantaneous bowl velocity still require live contact solving.

Measure against the already-cached orbit recurrence. Reject the table if interpolation, memory traffic or rebuilding costs erase its savings. Full-cycle audio reuse, frozen modal envelopes or sample replay should be an explicitly designed Freeze feature, not an invisible automatic optimization.

## Offloading policy

### Suitable work

Potential worker jobs include immutable lookup-table generation, descriptor preparation, large analysis tasks and optional cached render generation. Move them only when profiling shows meaningful callback spikes.

Publish completed objects through a bounded, nonblocking handoff. Tag requests/results with a generation so stale work cannot replace newer controls. Coalesce rapid changes; keep the last valid configuration while work is pending. The audio thread must never wait for a worker or allocate/free a large payload. Retire objects away from the audio callback using the project's existing ownership patterns.

The current private preparation helpers contain a live modal-state snapshot and are synchronous. They cannot simply be called on a worker against the live engine. Split immutable coefficient preparation from live state application first; preserve current mechanical state at the actual adoption boundary. Define how delayed adoption interacts with smoothing and active contacts.

### Defer live simulation offload

Moving one bowl to a worker at sample granularity adds scheduling/synchronization to a tightly timed path. Block processing can amortize that overhead, but requires a deliberate buffering/latency design and correct handling of sample-timed CV/events. It can reduce callback time while leaving total CPU unchanged or higher.

Do not make worker completion a condition for finishing the current audio callback. Investigate buffered rendering only as a separate architecture experiment with an explicit latency budget, underrun policy, CPU accounting and many-instance test. GPU offload is not a priority for these small recurrent workloads.

## Common acceptance matrix

Use deterministic fixtures for Metal and Crystal with Wood, Suede, Silicone and Felt. Cover base frequencies 55, 110, 261.625565, 440, 880, 1,760 and the 20/2,000 Hz boundaries where supported. Include positive/negative/zero speed, pressure from light touch through 15 N, low/high strike velocity, repeated impacts during rubbing, and long settled rotation followed by lift.

Exercise host rates 32, 44.1, 48, 88.2, 96, 176.4 and 192 kHz; binaural separation 0, 1, 10 and 33 Hz; width extremes; tuning/pressure/speed sweeps; material morphs; and fold/wake during compressed contact. Use focused cases for development, then the full relevant matrix before promotion. Run extended settled cases for at least 60 seconds to expose slow drift and repeating artifacts.

For each candidate, retain:

1. Source/build identity, compiler flags and exact controls.
2. CPU averages and callback/block distributions, with reference/candidate order alternated.
3. Mechanical and solver diagnostics, including fault/cap counts and energy errors.
4. Raw and level-matched audition WAVs, spectra and a report of measured differences.
5. User audition result before selecting a default quality tradeoff.

Run `make -j10 test-vessel` and full plugin linking for integration changes. Extend tests for the new behavior rather than weakening existing reference assertions. Approximate paths need explicit, justified tolerances. Verify the Windows build and live Rack behavior on the supported environment before claiming a production performance result.

## Implemented test control

The context menu now offers nominal 48/96/192 kHz quality families, saving selection with the patch and defaulting to Reference. It reports the actual rate and fallback. Switching deliberately clears the ringing state; rotation resumes, and isolated tails need another strike. Both bowls share a rate, with transactional fallback to a higher factor when configuration validation fails. The lower-rate policies pass integration tests, but still need the contact/aliasing comparisons and user auditions described above. See [implementation status](doc/Vessel/Implementation_Status.md).

## Friction-tolerance screening results (2026-09-30)

`tools/vessel/sweep_solver_tolerance.py` now builds isolated engine copies for friction residual tolerances 1x/10x/100x/1,000x. The production solver is unchanged. Strike-only and coupled outer convergence criteria, iteration caps, bracketing and recovery are unchanged in the experiment.

The first screen used eight fixtures at Reference and Economy, eight seconds per trajectory and two audit-disabled timing repeats. Cases cover both bowls, all mallets, pressure up to 15 N, very slow motion, reversal, repeated coupled strikes, a pitch sweep, high tuning and 33 Hz binaural separation. A separate audited trajectory captures stereo pickup velocity, energy residuals and iteration counts; audio-file writes and auditing are excluded from the timed trajectories. Output is physical pickup velocity rather than Rack output voltage. This tool does not measure the Rack callback or guarantee a settled response in every fixture.

| Friction tolerance multiplier | Median processing saving across 16 cases | Faults in candidate runs | Largest final absolute ledger residual |
|---|---:|---:|---:|
| 10x | -0.39% | 0 | 3.56e-12 J |
| 100x | -1.01% | 0 | 3.36e-12 J |
| 1,000x | 0.14% | 0 | 4.58e-12 J |

Negative savings indicate slower measured runs. Unpinned sequential timings varied by several percent; neither the gains nor regressions establish a reliable broad effect. The stronger evidence is iteration count: ordinary Reference rubbing was already close to two friction iterations and remained close to two after relaxation. The 1,000x setting reduced average friction iterations in the difficult slow Felt and Economy silicone-reversal cases, but yielded only small case-specific timing improvements in this screen. Largest relative waveform-error RMS among candidates was about -116 dB; these short comparisons are not a long-run perceptual/convergence certification.

**Decision:** retain the production tolerance. Relaxation offers insufficient measured broad benefit to prioritize over free-tail processing or faster friction-function evaluation. No approximate path was enabled in Rack.

The Economy metal pitch sweep changed internal rate twice while Reference stayed fixed, with no observed solver faults. Filter resets on those automatic factor changes are a plausible source of the user's pitch glitch; the live patch has not been reproduced, so the cause is not confirmed. Before lower quality becomes a production default, instrument actual factor changes and faults in the affected pitch range, then evaluate hysteresis/dwell and better transitions. Keep the current rate selector as a development experiment in the meantime.

Reproduce the screen:

```sh
python3 tools/vessel/sweep_solver_tolerance.py --seconds 8 --repeats 2
```

For an order check, use a separate output directory and `--reverse`. The runner requires NumPy and a compiler supporting the recorded x64 flags. Reports live under `build/vessel-solver-sweep`: `environment.json`, `comparison.json`, per-scale `metrics.csv` and stereo `.f64` streams. The metadata records the engine-source hash, compiler, flags and duration/repeats. Longer runs, spectral/onset measurements and controlled whole-module timing remain later gates rather than claims from this screen.

## Free-tail and friction-function experiments (2026-09-30)

### Adopted: exact fused internal-rate tail step

Vessel now automatically uses a fused unforced step when there is no active striker, rotation is lifted and the engagement ramp has reached zero. `ModalBank::advanceFree()` combines the existing modal recurrence, finite-state checks and both pickup projections into one pass. It preserves the original double-precision operation order, including the zero-force addition. Internal sample cadence, decimator coefficients/history, pickup phase and latency are unchanged.

This is a simpler first stage than the composed host-interval path described in §2: it does not batch/skip internal samples, add analytic-filter reconstruction state or require a new fade. Controls continue their existing internal smoothing; a strike or renewed rotation immediately returns to the full contact engine. Audited processing retains the original step so the complete energy accounting remains available. The scalar full-step oracle is selectable only through the C++ testing helper `setFastTailEnabled(false)`.

All **30 Vessel groups pass**: 13 mechanics, 5 host, 4 dual, 7 Rack and 1 offline approximation-curve screen. The new host group compares every output sample and energy value byte for byte, with periodic byte comparisons of modal states, at seven host rates and all three quality policies. It exercises strike/rotation re-entry, tuning, width and changed speed/pressure across 210,000 host frames. Existing dual/fold/wake and allocation checks also pass. ASan/UBSan passes all five host groups and the dense offline curve screen. Full Linux `plugin.so` linking succeeds; the target is up to date. Native Windows and actual live-meter savings are unverified.

### Tail timing

`tools/vessel/benchmark_fast_paths.cpp` compares the fused step with the full-step oracle in the same executable, with auditing disabled. Two seconds of warmup follow a strike, then four seconds of passive-tail processing are timed. Each case has three repetitions with alternating variant order. The table averages two complete runs, using 48 kHz host, C4 Metal/Suede, Linux GCC 12.3, O3/nehalem and strict math. Timings are unpinned and exclude Rack control/UI work. No build/test work ran concurrently.

| Internal rate | Full-step µs/host frame | Fused µs/host frame | Reduction |
|---|---:|---:|---:|
| 48 kHz | 0.0956 | 0.0579 | 39.43% |
| 96 kHz | 0.2023 | 0.1522 | 24.79% |
| 192 kHz | 0.4445 | 0.3175 | 28.57% |

Results: `build/vessel-tail-final.csv`, `build/vessel-tail-final-repeat.csv`, `build/vessel-tail-comparison.json`. The earlier preliminary run used a larger in-development core containing the friction experiment; final timings above use the retained production core. These savings apply to eligible tails, not continuous rotation. This stage is worth keeping despite the plan's original 40% target for a more complex composed/filter-reconstruction path: it adds no filter-transition/history machinery and preserves tested output exactly.

### Retained offline: cubic friction approximations

Added reproducible generated monotone Hermite tables and an offline evaluator under `tools/vessel/experiments`. Generation uses high-precision Decimal functions to avoid platform-libm drift in checked-in table values. Derivatives come from the same cubics; the generator computes a conservative negative-slope certificate from quadratic derivative extrema. The dense screen checks force/derivative error, frictional dissipation, force limits, derivative consistency and rounded lookup boundaries. Worst measured absolute errors are approximately 1.31e-9 N and 0.00412 N/(m/s). The tables are not compiled into the plugin.

`tools/vessel/probe_fast_friction.py` builds isolated analytic/cubic engines and uses the eight-fixture trajectory harness at Reference and Economy. Eight-second audited captures and two audit-disabled timing repeats were run twice in analytic/candidate/candidate/analytic order. A second experiment replaces only tanh while retaining analytic Gaussian weakening (`--hybrid`).

| Candidate | Median CPU saving across 16 cases | Slow Felt case saving | Observed candidate faults | Largest relative waveform-error RMS |
|---|---:|---:|---:|---:|
| Gaussian + tanh cubics | -1.27% | 13.84–15.16% | 0 | approximately -137 dB |
| Tanh cubic only | 0.06% | 7.58–7.71% | 0 | approximately -180 dB |

Several ordinary rubbing cases slowed slightly; unpinned small effects remain uncertain. Largest final absolute ledger residuals were below 4e-12 J. These short trajectories establish useful screening evidence, not long-run calibration/aliasing or listening approval. **Decision: retain analytic friction in the plugin**, since neither approximation produced a general CPU benefit. Both prototypes remain available for investigation of slow-contact workloads.

Reports: `build/vessel-friction-probe`, `build/vessel-friction-hybrid-probe`. Logs: `/tmp/vessel-friction-probe.log`, `/tmp/vessel-friction-hybrid-probe.log`, `/tmp/vessel-fast-path-final-verification.log`, `/tmp/vessel-tail-host-sanitized.log`, `/tmp/vessel-friction-screen-sanitized.log`.

Reproduce:

```sh
make -j10 test-vessel build/tools/vessel_benchmark_fast_paths
build/tools/vessel_benchmark_fast_paths > build/vessel-tail-final.csv
python3 tools/vessel/probe_fast_friction.py
python3 tools/vessel/probe_fast_friction.py --hybrid --output build/vessel-friction-hybrid-probe
```

The rate selector remains a development control. Further tail savings may come from host-interval composition with equivalent filtering. For continuous rotation, prioritize another measured profile and modal SIMD/loop restructuring before adding larger lookup caches or background audio buffering. Configuration offloading still addresses spikes rather than a stable patch's main DSP cost.

## Next validation deliverable

Build an offline comparison tool for Reference versus the eligible 88.2/96 kHz variant, with identical input events and separately measured aliasing, contact behavior and CPU. Produce a small audition set: metal/suede sustained rotation, wood impact, crystal rotation, a high-pressure reversal, and 33 Hz binaural rotation. Include a high-pitch case that exercises fallback.

Use that evidence to decide whether lower rate is a viable quality option. The first fused free-tail stage is now adopted, as recorded above. Host-interval composition remains a possible later tail experiment. The friction-function and tolerance screens do not justify a production change; use a new measured profile to choose the next continuous-rotation optimization.


## Shared binaural decimator (2026-10-05)

Implemented one stereo FIR cascade in `DualBowlAdapter`, retaining two independent
`VesselEngine` contact solvers and modal states. In settled binaural operation,
only left-bowl L and right-bowl R are filtered. Single mode filters both left-bowl
pickups. A 50 ms internal-rate linear fade switches the right input across zero
separation, with uninterrupted FIR history and sleep after fade-out. Wake clones
pre-retune mechanics; interrupted fades reverse from their current mix. No change
to modal precision, SIMD instruction requirements, quality defaults or contact
physics. The shared-force proposal in `doc/Vessel/Binaural_Refined.md` is not adopted.

`make -j6 test-vessel plugin.so` passed on native Linux. After adding the final
transition-duration group, the focused dual suite passed all five groups. Steady
output and energy remain bit-for-bit equal to two independent host adapters at
seven host rates, three quality policies and four separations. Transition tests
cover an active-strike wake, interrupted fades, width/material/rate changes,
transactional rejection, exact 50 ms timing for tails and rubbing, FIR settling
to the single reference, and positive-to-positive retuning without a new fade.
The Rack allocation trap passed. All five dual groups also passed AddressSanitizer
and UndefinedBehaviorSanitizer (`ASAN_OPTIONS=detect_leaks=0`; LeakSanitizer cannot
run under this environment's process tracer). These are automated checks, not a listening
certification; native Windows and live Rack audition remain unverified.

The existing `vessel_benchmark_dual` was built before and after with the same
Linux GCC O3/nehalem strict-math flags, then run before/after/after/before without
concurrent builds or tests. Each invocation internally averages three alternating
fixture orders. At 48 kHz host, Reference quality (192 kHz internal), Metal/Suede,
33 Hz separation, averages across the two invocations were:

| Workload | Before µs/host frame | Shared FIR µs/host frame | Reduction |
| :--- | ---: | ---: | ---: |
| Passive tail | 0.6160 | 0.4961 | 19.5% |
| Coupled rubbing | 1.4355 | 1.3253 | 7.7% |

Timings are unpinned and exclude Rack/UI work. Tail timings varied appreciably;
these are indicative fixture results, not universal savings. Factor-one processing
has no FIR work to remove. Raw results: `build/vessel-shared-before.csv`,
`build/vessel-shared-after.csv`, and their `-repeat.csv` counterparts. Existing
benchmark column `folded_savings_percent` compares zero-separation sleep with
always-dual processing; it is not the shared-filter saving in the table above.


## Shared-force resonator screen (2026-10-05)

**Decision: retain two independent contact solvers.** Sharing the lower bowl's
committed generalized forces with a differently tuned linear upper resonator
saves CPU, but changes sustained channel balance and can lose the intended
second driven pitch. This is a possible alternative sound model, not a
transparent binaural optimization. No production DSP or defaults changed.

Added `tools/vessel/probe_shared_force.py` and its C++ harness. The runner copies
`src/vessel` to a temporary directory, injects guarded extraction of the actual
committed force vector, and adds a fused forced recurrence only to that copy.
Zero force is explicit on unforced steps. The candidate retains independent
modal coefficients, states, observer projection and energy-dependent damping,
plus the same shared stereo FIR as the current reference. Both sides receive
identical controls; the second resonator has no contact feedback. The reference
is the existing always-dual adapter, including at zero separation for the control
case. The prototype preserves full per-internal-sample forcing; it does not
collapse forces to one host sample.

Ten fixtures ran for 60 seconds each at 48 kHz host, Reference quality / 192 kHz
internal, fixed descriptors, 0.4 rev/s and 2.5 N. Rubbing starts at time zero
without a strike and lifts at 55 seconds. Impact cases strike once at 50 ms with
normalized velocity 0.5 and never rotate. The table uses seconds 45–55 for rubbing
level/spectral comparison. Peak estimates use a 10-second Hann window with
log-magnitude parabolic interpolation, searching 0.6–1.4 times center pitch;
they are dominant fundamental-region peaks, not exact modal-frequency estimates
or a comprehensive modal tracker.

| Fixture | CPU saving | Right RMS change vs independent reference | Reference right peak | Shared-force right peak |
| :--- | ---: | ---: | ---: | ---: |
| Metal/Suede, 1 Hz | 34.7% | −10.8 dB | 262.92 Hz | 263.52 Hz |
| Metal/Suede, 10 Hz | 35.1% | −23.0 dB | 267.42 Hz | 267.02 Hz |
| Metal/Suede, 33 Hz | 34.7% | −18.8 dB | 278.92 Hz | 277.24 Hz |
| Crystal/Suede, 33 Hz | 35.6% | −42.8 dB | 278.19 Hz | 246.77 Hz |
| Metal/Felt, 33 Hz | 33.6% | −38.3 dB | 278.99 Hz | 277.31 Hz |
| Crystal/Wood, 33 Hz | 31.8% | −7.1 dB | 278.29 Hz | 245.27 Hz |
| Metal/Suede, 55 Hz center / 33 Hz separation | 35.2% | −10.8 dB | 71.65 Hz | 71.38 Hz |

The left waveform is exactly unchanged in every captured fixture. At zero
separation the right waveform also matches exactly. Crystal/Suede's dominant
interaural peak difference falls from about 33.02 Hz to 1.60 Hz during late
rubbing. After lift its weak right tail returns near its own natural frequency:
this supports the distinction between preserving resonator poles and preserving
the driven sound. Metal's right pitch does not collapse in the same way, but its
level, sidebands and dominant split-mode selection change. No automatic gain
normalization was used in the experiment; boosting a channel would not repair
the altered excitation spectrum or replace missing independent feedback.

For isolated Wood and Silicone impacts at 33 Hz separation, seconds 0.04–0.30
show right RMS changes of −0.41 / −0.20 dB and relative right waveform-error RMS
of 7.46% / 7.46%. Late peaks match at about 278.98 Hz. These results are more
promising than rubbing but are not a perceptual equivalence certificate. Timed
impact cases measure late passive tails: their 11.4% / 11.1% savings do **not**
measure savings during the brief strike/contact solve. A strike-only production
policy would additionally need to handle strikes during rubbing and accumulated
state differences on contact re-entry.

No observed faults occurred. Maximum sampled reference cumulative energy
residual was 5.28e-11 J; maximum slave force-port residual was 3.20e-12 J. The
slave audit includes its own input work: `E_slave + damping_loss − input_work`.
It does not incorrectly reuse the master's hand/striker work or claim combined
physical energy conservation. Stability alone does not establish audio quality.
Separate audited-versus-fused checks match output and slave energy exactly over
4,096 host frames per fixture, including repeated strikes, direction reversal
and a 15 N pressure change. Those short checks validate implementation arithmetic;
they do not establish long-run sound quality under reversal/high pressure.

Timing uses three repetitions of a three-second continuation from established
state at 50 seconds, alternating reference/candidate order. Auditing, file I/O
and state-copy setup are outside timing. Both paths include the same 1 kHz damping
updates; candidate telemetry includes both modal energies. Linux GCC 12.3,
O3/nehalem, strict math, unpinned timing. The force-extraction instrumentation
branch remains in the copied reference core. These are approximate offline costs,
not Windows/Rack callback results. Full fade/wake, live tuning, wider controls,
aliasing/convergence and listening certification are outside this screen.

Reproduce:

```sh
python3 tools/vessel/probe_shared_force.py --seconds 60 --repeats 3
python3 tools/vessel/probe_shared_force.py --verify-only
```

Artifacts are under `build/vessel-shared-force`: full stereo `.f64` captures,
`metrics.csv`, `comparison.json`, build/source hashes in `environment.json`, and
separate verification records. The original long capture predates the additional
verification-only option; its recorded harness/runner hashes identify that
version. The current runner also checks a settled-state audited/fused continuation
before timing. No DSP equations changed between these harness versions.
The final runner passed a separate 20-second, one-repeat end-to-end screen in
`build/vessel-shared-force-smoke`, including both startup and settled-state
audited/fused checks. Rubbing savings remained about 33–35% and the substantial
level changes persisted. Wood passive-tail timing regressed by 14% in that short
run while Silicone saved 9%; this further limits any general tail-speed claim
from unpinned timings. The table above uses the longer, three-repeat screen.

For audition, `_raw.wav` pairs use one common gain for headroom, preserving the
reference/candidate level difference. `_matched.wav` pairs match **whole-stereo**
RMS and use a common safety gain; they preserve channel imbalance. These PCM16
previews are physical pickup velocities scaled for playback, not Rack output
volts. Eight-second impact clips include attack; ten-second rub clips use seconds
45–55. Examples:

- [Metal/Suede reference](../../build/vessel-shared-force/metal_suede_33_reference_raw.wav)
  and [shared-force candidate](../../build/vessel-shared-force/metal_suede_33_shared_raw.wav).
- [Crystal/Suede reference](../../build/vessel-shared-force/crystal_suede_33_reference_raw.wav)
  and [shared-force candidate](../../build/vessel-shared-force/crystal_suede_33_shared_raw.wav).
- [Spectral comparison](../../build/vessel-shared-force/shared-force-spectra.png).

The next quality-preserving optimization investigation should retain independent
contact feedback. Fixed-rate quality comparisons and exact modal/filter tail
composition remain better candidates; they have their own convergence and
transition requirements. This screen does not authorize selecting an approximate
path as the production default.


## Composed passive-tail prototype (2026-10-05)

**Historical prototype result: pursue integration.** The production integration
and its validation are recorded in the following section. Combining
unforced modal steps and evaluating the equivalent filtered pickup removes most
eligible-tail DSP cost without lowering the internal model's quality setting.
This preserves independent bowl dynamics and the existing FIR response, unlike
the rejected shared-force resonator.

### Architecture

`tools/vessel/experiments/ComposedTailHost.hpp` wraps an unchanged host adapter in
an isolated source copy. For each mode it caches the original internal-step 2×2
matrix, its power over one host interval, and the FIR cascade's matrix-polynomial
response. The final pickup is evaluated from the current modal state using that
response. Each cascade stage uses the appropriate dilated backward transition;
the composition preserves the original filter phase and latency. Modes above
host Nyquist remain represented internally and receive the original filter
attenuation. No mode removal, coefficient retuning, sample-rate switch, new
contact law or reduced-precision state is involved.

Entry runs the normal path until all filter history comes from homogeneous
motion: 65/97/113 host frames for factors 2/4/8 after full tail eligibility.
There is no transition fade because the two paths describe the same filtered
trajectory. Exit now uses a streaming history handoff, replacing the original
synchronous reconstruction (whose largest sampled whole re-entry call was
18.7 µs). Contact is processed on the current sample. The actual stream enters
an empty ordinary FIR; a frozen-coefficient, fictitious continuation of the old
homogeneous tail enters a second empty FIR. Output is:

`FIR_zero(actual) + analytic_filtered(old_tail) - FIR_zero(old_tail)`

The correction supplies the old history contribution until the FIR's finite
support has drained. This is a linear-filter identity; the actual contact solver
still uses the full current physical state. There is no contact deferral or
crossfade. The fictitious tail and its filter are retired after 65/97/113 host
frames for factors 2/4/8 (about 1.35/2.02 ms for 96/192 kHz at a 48 kHz host).
The handoff copies fixed-size state/cache arrays and clears two filter histories;
it no longer evaluates every mode for every old history position at once.

Same-rate pitch/width/material changes preserve the old baseline snapshot while
preparing the new modal observer. Rate/factor changes discard the correction,
matching the ordinary adapter's existing history-reset and output-transition
policy. Subsequent strikes and rubbing use the current actual state during the
handoff; failed configuration leaves the prototype and future stream untouched.

Eligibility requires no active striker, no rotation or engagement, auditing off,
and zero additional high-energy damping. High-energy tails stay on the existing
path until the extra damping vanishes; those coefficient changes are not treated
as fixed linear motion. Controls retain their ordinary internal smoothing even
when modal steps are composed. Factor-one processing retains the ordinary path.

Coefficient preparation initially expanded an equivalent 897-tap cascade kernel.
Evaluating each FIR stage directly as a matrix polynomial reduced setup cost:
the quick screen's maximum dropped from 116.7 to 35.1 µs. The final full screen's
maxima ranged from 19.0 to 25.6 µs over two full screens; these unpinned
maxima are not guaranteed bounds. Preparation
is outside steady-tail timing. Production integration must avoid rebuilding this
cache on every modulation tick: use settled-control eligibility or an appropriate
immutable-preparation handoff. The measured saving does not justify moving this
setup work indiscriminately into the audio callback.

### Accuracy and lifecycle screen

`tools/vessel/probe_composed_tail.py` grants private access only in a temporary
core copy; production files are unchanged. The final normal run passes 214 cases:

- Both seed bowls, seven host rates and three quality policies at four pitches
  (20 Hz, C4, 880 Hz and 2 kHz), including automatic factor fallback.
- Eight 60-second tails at 32/48 kHz hosts, plus synthetic top modes placed at
  0.399 of the selected internal rate to test near-guard filtering.
- Strike/rub entry and re-entry, reversal/high pressure, pitch and width changes,
  material changes, host/quality changes, reset, high-energy fallback and
  transactional rejection. Both filtered samples and modal states are compared.
- Four fixed-33-Hz binaural trajectories with two prototype wrappers, compared
  against the current `DualBowlAdapter` and its **shared** decimator. This verifies
  independence and linear-filter equivalence at fixed separation; it does not yet
  validate composition through the shared adapter's fold/wake fade.
- Eight additional handoff stress cases, each with twelve interruptions:
  consecutive strikes, re-strikes in the middle and at the end of the handoff,
  overlapping rubbing, pitch/width/material changes during the correction,
  rate changes and transactional rejection while handing off.
- Four twelve-second strike/rub/lift auditions at Balanced and Reference, and
  alternating-order timing fixtures. A separate re-entry benchmark measures
  single and simultaneous 8/32-bowl onset and continuation.

Maximum relative stereo error RMS was 2.36e-11 (about **−212.6 dB**), maximum
pickup sample error 1.96e-11 m/s, maximum modal-state error 5.17e-12 and maximum
energy discrepancy 3.85e-13 J. The gates were relative RMS below 1e-5 (−100 dB),
or absolute RMS below 1e-12 m/s for quiet signals; peak error below 1e-9 m/s;
state/energy discrepancy below 1e-8. These are equivalence checks against the
existing algorithm, not a claim that the physical model itself is calibrated.
No observed faults or trapped C++ allocations occurred in the exercised process,
configuration and streaming-handoff paths. Fixed unforced cases additionally
require non-increasing energy with a 1e-14 J roundoff allowance.

The quick matrix also passes AddressSanitizer and UndefinedBehaviorSanitizer.
Quick mode shortens fixed/long tails, but retains the full transition, paired and
audition trajectories. Leak detection is disabled because LeakSanitizer cannot
run under this environment's process tracer. Sanitized timing is not used for
performance claims.

### Tail timing

48 kHz host, Metal/Suede, Linux GCC 12.3, O3/nehalem and strict math. Single-bowl
benchmarks begin from seeded low-energy modal states; paired benchmarks begin
from a settled actual strike/rub trajectory. Five two-second continuations per
variant alternate order; auditing, setup, file writes and comparisons are outside
timing. Both variants include 1 kHz high-energy-damping updates. Times exclude
Rack module/control/UI overhead and are unpinned.

| Eligible tail | Internal rate | Reference µs/frame | Prototype µs/frame | Saving |
| :--- | ---: | ---: | ---: | ---: |
| Single bowl | 96 kHz | 0.1362 | 0.0382 | 71.9% |
| Single bowl | 192 kHz | 0.2956 | 0.0390 | 86.8% |
| Two bowls, shared-FIR reference | 96 kHz | 0.2277 | 0.0758 | 66.7% |
| Two bowls, shared-FIR reference | 192 kHz | 0.4931 | 0.0809 | 83.6% |

Crystal results were similar: 71.9% / 86.6% single and 66.8% / 83.5% paired.
Factor one has no meaningful saving. These figures apply only during eligible
passive tails; they do not reduce continuous rubbing or active impacts.

### Streaming-handoff timing

A dedicated benchmark starts from composed, seeded Metal/Suede tails and repeats
strike or rub onset 200 times at a 48 kHz host. It measures setup alone, the whole
onset call (including setup and contact), and each subsequent handoff call.
Reference and candidate order alternate. Copies, sample collection, allocation
checks outside processing, and quantile calculations are outside the timed
regions. The ordinary single-bowl adapter is the reference here; simultaneous
instances are independent bowls, not a shared-FIR binaural adapter. The separate
setup-only test uses a copy and cannot simply be subtracted from the onset time
because cache conditions differ. All durations include clock overhead.

| One bowl, median µs/call | 96 kHz | 192 kHz |
| :--- | ---: | ---: |
| Handoff setup only | 0.15 | 0.16 |
| Strike onset, reference → candidate | 0.64 → 0.98 | 1.06 → 1.52 |
| Rub onset, reference → candidate | 0.38 → 0.70 | 0.75 → 1.18 |
| Strike continuation, reference → candidate | 0.41 → 0.48 | 0.82 → 1.00 |
| Rub continuation, reference → candidate | 0.35 → 0.42 | 0.71 → 0.89 |

One-bowl candidate onset p99 was 1.11/1.64 µs for strike and 0.93/1.60 µs
for rub at 96/192 kHz. The extra continuation work ends after the handoff.
The original full history reconstruction is absent; steady-tail savings remain
substantial despite a small added eligibility-dispatch cost.

Memory/cache effects matter with simultaneous re-entry. For 32 bowls, median
whole strike onset was 21.13 → 41.86 µs at 96 kHz and 36.07 → 61.05 µs at
192 kHz. Candidate p99 was 45.03/73.76 µs; setup alone had medians of
15.58/16.23 µs for the whole group. Rub onset medians were 12.88 → 33.85 µs
and 26.15 → 50.54 µs. These bursts are much smaller per bowl than reconstructing
all histories, but are not zero. The extra fixed filter storage/clearing and
cache behavior need evaluation in the shared adapter architecture.

The full trajectory screen's largest sampled whole contact-exit call was
8.8 µs, compared with 18.7 µs in the original reconstruction run; this is not a
controlled worst-case comparison. In the dedicated benchmark, scheduling
outliers also affected ordinary reference processing and later continuation
calls. All timings are unpinned and are **not certified bounds**. Native/live
Rack callback distributions remain open. Synchronous coefficient preparation
is a separate unresolved cost (largest sampled configuration: 21.9 µs in this
screen), not fixed by streaming the FIR handoff.

### Reproduction and artifacts

```sh
python3 tools/vessel/probe_composed_tail.py --output build/vessel-composed-tail-streaming
ASAN_OPTIONS=detect_leaks=0 python3 tools/vessel/probe_composed_tail.py --quick --sanitize --output build/vessel-composed-tail-streaming-sanitized
```

Normal outputs: `build/vessel-composed-tail-streaming/{environment,summary}.json`,
`metrics.csv`, `reentry_timing.csv` (median/p99/max and sample counts), full stereo float64 reference/candidate/difference captures, and
PCM16 audition WAVs using a single common playback gain. All four PCM16 pairs
are byte-identical; float64 differences are retained for numerical inspection.

- [Metal, 96 kHz reference](../../build/vessel-composed-tail-streaming/bowl0_quality1_reference.wav)
  and [prototype](../../build/vessel-composed-tail-streaming/bowl0_quality1_candidate.wav).
- [Crystal, 96 kHz reference](../../build/vessel-composed-tail-streaming/bowl1_quality1_reference.wav)
  and [prototype](../../build/vessel-composed-tail-streaming/bowl1_quality1_candidate.wav).

Remaining integration work: move the reusable tail/cache helpers into the shared
adapter architecture, preserve both independent engines, validate zero-separation
fold/wake and interrupted fades, avoid cache-preparation overhead during ongoing
modulation, preserve audit/fault accounting, and profile real Rack callback
spikes/many-instance behavior. Native Windows and live listening/window checks
remain open. No production path or quality default was changed by this screen.

## Integrated passive tails (2026-10-05)

**Enabled in the production `DualBowlAdapter`.** The integrated path preserves
both independent physical engines and the single shared stereo decimator. No
quality-default, control-ID, patch-schema or output-latency change is involved.
`HostRateAdapter` remains an ordinary internal-rate reference. The adapter has
an explicit `setComposedTailEnabled(false)` oracle switch for offline A/B work;
no new user control or serialized setting was added.

### Bounded cache preparation and immediate contact

`PassiveTail` caches the original internal recurrence, its host-interval power,
and the FIR cascade's equivalent observer. Preparation starts only during
eligible, settled passive motion after a complete homogeneous-history interval.
It evaluates **at most 16 polynomial terms per host call across both bowls**,
with fixed-size matrix initialization at mode/stage boundaries. There are no
new transcendental calls, allocations, locks or worker handoffs. Configuration
invalidates the cache with constant work; ordinary processing continues while
it is rebuilt. The existing modal/contact configuration remains synchronous,
but the prototype's extra all-at-once FIR cache preparation is gone.

For the 14-mode seed bowls at a 48 kHz host, initial binaural composition starts
after approximately 6.1 ms at 96 kHz or 11.4 ms at 192 kHz of eligible, stable
motion. Subsequent contact exits can reuse the cache after filter warm-up.
Continuous tuning prevents adoption of incomplete/stale cache data. Auditing,
active contact, extra high-energy damping, factor one and changing binaural
mixes use ordinary processing.

Strikes and rubbing are delivered on the current sample. The shared filter
rebuilds naturally using the streaming correction described above. Its frozen
baseline retains the old coefficients, observer and single/dual routing through
pitch/material/width changes and fold/wake reversals. Rate changes discard the
correction under the existing filter reset policy. The 50 ms binaural fade
continues at internal rate and can reverse while the correction is active.

Storage is fixed: `sizeof(DualBowlAdapter)` is 54,304 bytes in this Linux build,
including the extra baseline filter and four model/snapshot caches. Snapshot
copies and filter clears still cost time on contact entry; the optimization
removes the nested history-reconstruction work, not every entry cost.

### Integrated accuracy and lifecycle checks

`vessel_passive_tail_spec` is part of `test-vessel` and `test-fast`. It compares
the same shared adapter with composition enabled and disabled:

- 504 bowl/rate/quality/pitch/separation combinations, followed by immediate and
  repeated contact events; relative RMS error 2.07e-14, peak sample error
  2.94e-14 m/s and maximum modal-state discrepancy 8.26e-15.
- Four 60-second binaural tails; combined relative RMS error 1.17e-11
  (about −218.6 dB), with all sample/state/energy gates passing.
- Single/dual fold and wake, interrupted fades, strikes during the history
  handoff, mid-handoff tuning/material/width/rate changes, transactional rejection,
  audit and optimization toggles, high-energy fallback and reset.
- Continuous 1 kHz retuning never adopts a cache; composition resumes after
  tuning settles. Injected nonfinite state preserves recovery/fault counting.
- No trapped C++ allocations during the exercised processing/configuration.

Gates retain the prototype's relative RMS <1e-5 or quiet absolute RMS <1e-12 m/s,
peak <1e-9 m/s and state/energy discrepancy <1e-8. Existing fold tests retain
bit-identical rubbing checks and allow <1e-10 m/s roundoff for composed tails.
The complete Vessel suite, including its Rack-linked module tests, passes.
The focused integrated suite also passes ASan/UBSan (leak detection disabled).
The full native Linux `plugin.so` links with no compiler diagnostics.

The broader `test-fast` run is **not fully green**: the unchanged
`theme_service_spec` reports 12 assertions. An Octavia recording assertion
passed when rerun in isolation; all checks after the theme test were then run
to completion and passed. The initial sandbox localhost bind failure was
resolved by running socket tests outside the sandbox. These results do not
constitute native Windows or live audio-driver validation.

### Integrated timing

Metal/Suede, 48 kHz host, Linux GCC 12.3, O3/nehalem and strict math. Reference
and candidate use the same adapter with the composed path disabled/enabled.
Auditing is off. Timing runs execute without concurrent build/test jobs.

Core DSP, arithmetic mean of seven two-second continuations, including 1 kHz
high-energy-damping updates:

| Route | Internal rate | Reference µs/frame | Composed µs/frame | Saving |
| :--- | ---: | ---: | ---: | ---: |
| Single bowl | 96 kHz | 0.1400 | 0.0466 | 66.7% |
| Binaural, 33 Hz | 96 kHz | 0.2287 | 0.0820 | 64.2% |
| Single bowl | 192 kHz | 0.2999 | 0.0457 | 84.8% |
| Binaural, 33 Hz | 192 kHz | 0.4716 | 0.0842 | 82.1% |

Rack-linked **whole `Vessel::process()`**, arithmetic mean of seven one-second
continuations from actual strikes, including control and visual publication
work with debug timing disabled:

| Route | Internal rate | Reference µs/frame | Composed µs/frame | Saving |
| :--- | ---: | ---: | ---: | ---: |
| Single bowl | 96 kHz | 0.1633 | 0.0686 | 58.0% |
| Binaural, 33 Hz | 96 kHz | 0.2644 | 0.1059 | 59.9% |
| Single bowl | 192 kHz | 0.3329 | 0.0720 | 78.4% |
| Binaural, 33 Hz | 192 kHz | 0.5278 | 0.1171 | 77.8% |

Steady rubbing measurements ranged from a 2.1% slowdown to a 1.3% speedup across
the whole-module fixtures; this run provides no evidence of a material rubbing
speedup. The tail savings apply only while passive composition is eligible.

Whole-module binaural re-entry (one instance, median / p99, µs per call):

| Event | Internal rate | Ordinary | Composed handoff |
| :--- | ---: | ---: | ---: |
| Strike onset | 96 kHz | 1.37 / 2.15 | 1.77 / 2.62 |
| Strike onset | 192 kHz | 2.24 / 2.73 | 2.76 / 3.23 |
| Rub onset | 96 kHz | 0.80 / 1.06 | 1.17 / 1.34 |
| Rub onset | 192 kHz | 1.47 / 2.03 | 1.99 / 2.12 |
| Strike continuation | 96 kHz | 0.79 / 1.34 | 0.91 / 1.51 |
| Strike continuation | 192 kHz | 1.60 / 1.77 | 1.85 / 2.06 |

The added median strike-continuation cost is about 0.12/0.25 µs during the short
96/192 kHz handoff. Incremental cache preparation for binaural tails measured
0.28/0.52 µs median for the **whole core call**, versus 0.23/0.47 µs ordinary;
p99 candidate preparation calls were 0.48/0.58 µs. Cache preparation is no longer
a ~22 µs synchronous addition to configuration.

For 32 simultaneous binaural modules, whole-module strike-onset medians were
50.29 → 77.72 µs at 96 kHz and 75.58 → 105.66 µs at 192 kHz. Candidate p99
was 95.85/137.08 µs. The fixed snapshots/clears and cache footprint still matter
at this scale. These are measurements, **not guaranteed callback bounds**;
unpinned scheduling outliers occur in both variants. Headless module timing
excludes Rack's scheduler, GUI contention and the audio driver. Live listening,
underrun monitoring and native Windows timing remain unverified.

### Reproduction

```sh
make -j4 test-vessel
make -j4 all
make -j4 build/tools/vessel_benchmark_passive_tail build/tools/vessel_benchmark_passive_module
mkdir -p build/vessel-passive-tail-integrated
build/tools/vessel_benchmark_passive_tail > build/vessel-passive-tail-integrated/timing.csv
build/tools/vessel_benchmark_passive_module > build/vessel-passive-tail-integrated/module_timing.csv
```

`build/vessel-passive-tail-integrated/` contains both CSVs (mean/median/p99/max),
environment/source hashes, sanitizer output and the complete validation logs.
The earlier offline prototype remains available for historical comparison;
its paired reference explicitly disables the now-integrated composition path.

## Active-contact modal fusion and SSE2 (2026-10-08)

The non-audited active-contact path now combines modal commit, finite-state
validation and both pickup projections. On SSE2 targets it updates two modes
at a time in double precision. Each mode retains the scalar expression order,
and each pickup sums modes in the original order. Other targets use the fused
scalar loop. Audit calculations retain the existing reference path. Solver
tolerances, sample rates, friction functions, control cadence and passive-tail
composition are unchanged.

Native Windows measurements: Intel Core i9-9900K, MSYS2 MinGW GCC 16.1.0,
`-O3 -march=nehalem -fno-fast-math -fno-unsafe-math-optimizations`, Rack 2 Pro
runtime, 48 kHz host. The baseline was built from revision
`26bb55bbec4c49a6f526ef19415e746cb83cb282` before the modal edits. Retained
executables were run serially, with no concurrent builds/tests: baseline,
candidate, candidate, baseline, baseline, candidate (a preliminary scalar-only
experiment also ran before the first SIMD candidate).

`tools/vessel/benchmark_active_module.cpp` measures the complete headless
`Vessel::process()` callback with debug timing disabled. Each case has three
independently initialized repetitions with one second of warmup and one second
of measurement. Below are medians of the three executable-run means. Rub speed
is 0.2 rev/s, pressure 2.5 N, pitch C4. Coupled cases add a strike every 250 ms.

| Crystal/Wood case | Internal rate | Before us/frame | After us/frame | Saving |
| :--- | ---: | ---: | ---: | ---: |
| Single, rub | 96 kHz | 0.6628 | 0.6325 | 4.58% |
| Binaural 33 Hz, rub | 96 kHz | 1.2105 | 1.1530 | 4.75% |
| Single, rub + strikes | 96 kHz | 0.6643 | 0.6397 | 3.71% |
| Binaural 33 Hz, rub + strikes | 96 kHz | 1.2213 | 1.1680 | 4.37% |
| Single, rub | 192 kHz | 1.3471 | 1.2799 | 4.98% |
| Binaural 33 Hz, rub | 192 kHz | 2.4487 | 2.3787 | 2.86% |
| Single, rub + strikes | 192 kHz | 1.3354 | 1.3200 | 1.16% |
| Binaural 33 Hz, rub + strikes | 192 kHz | 2.4445 | 2.3304 | 4.67% |

Eight matching Metal/Suede cases measured 0.43–6.22% savings. These small
unpinned wall-clock differences are subject to scheduling and frequency noise;
the smallest differences do not establish a reliable gain. They are not live
Rack CPU-meter measurements or worst-case callback guarantees.

All 16 cases across all six runs have identical output/energy fingerprints in
separate untimed traces covering Rub release, strike re-entry and pitch changes.
The fast-path regression compares every modal state and pickup exactly against
the scalar commit over 2,000 randomized-force steps per bowl/rate, with extra
damping, and injects NaN and both infinities into every mode to verify detection.
Raw CSVs and the summary are in `test-results/vessel-active-*.csv` locally.
The native Windows `plugin.dll` build and complete `test-vessel` suite passed,
including the new exact-state and nonfinite regression. Live Rack audition and
CPU-meter comparison were not performed in this pass.

Build the reusable benchmark with
`make -j10 build/tools/vessel_benchmark_active_module`. On Windows run its `.exe`
with the installed Rack runtime directory first on PATH, as for Rack-linked tests.

## Further active-use screening (2026-10-09)

No additional production optimization was retained from this pass. The prior
modal fusion/SSE2 implementation remains the baseline (revision
`4e063cda49283c81378bc7e042ba6da6a67fec75`). Candidates were:

- Contact-loop fusion: calculate free modal motion, tangential velocity and
  tangential admittance in one loop, retaining each reduction's scalar order.
- Prepared friction: validate the unchanged mallet descriptor at configuration
  and reuse it for rubbing-only solves. Dynamic input checks, the uniqueness
  certificate, divisions, analytic functions and solver tolerances remain intact.
- Linked control reads: skip the six fallback reads/clamps immediately replaced
  by a valid V.Tune message. Keep control cadence and retained-parameter writes.

The first two were screened separately and together. Contact-only median changes
across the 16 fixtures ranged from approximately 10% slower to 5% faster;
contact plus caching ranged from 2.5% slower to 7.6% faster. A candidate retaining
only caching and the control-read change was also built. These mixed results
did not establish a broad benefit. The expander-read candidate was compared
with and without that change while holding the engine candidate constant.

Measurements use the same native Windows machine/compiler and callback harness
as the preceding section, with three serial executable runs per main comparison
and reversed ordering. A second set pinned the benchmark launcher/children to
logical CPU 2 (affinity mask 4), then restored the launcher's affinity. Pinning
did not stabilize results: individual case medians still changed sign, and some
differences exceeded 20%. These results cannot distinguish small gains from
host noise or code-layout effects, and should not be quoted as achieved speedups.
No builds/tests were run concurrently with the timing passes.

All compared audio/energy trace fingerprints matched the corresponding baseline.
This is a screened behavioral comparison, not proof covering every possible
patch. Candidate DSP and candidate-only tests were removed rather than promoting
them on inconclusive timing. The local artifacts remain under
`test-results/vessel-next-*`, `vessel-linked-*`, and `vessel-pinned-*`; the latest
candidate patch is `test-results/vessel-next-candidates.patch`.

Inspection also found that the FIR already uses symmetric taps, mirrored history
and two-channel SSE2. No replacement FIR was implemented. Rack parameter setters
are plain float stores; change-detection branches were not pursued. Cached
reciprocal substitutions were not pursued because they change rounding.

The retained benchmark addition is `--expander`, which supplies a static valid
V.Tune message and measures Vessel's linked callback. It excludes V.Tune's own
callback and Rack's message-flip scheduling. Run it with the Rack runtime first
on PATH. Future investigation should obtain a stable sampled profile of active
rubbing before investing in more small source-level rearrangements.
After restoring production DSP, the native Windows `plugin.dll` build and full
`test-vessel` suite passed; the retained benchmark target also compiled.


## Four-accumulator output FIR (2026-10-09)

The SSE2 streaming decimator now uses four independent accumulators with the
existing 129 coefficients. The original serial SSE2 implementation remains
available through `VESSEL_SERIAL_FIR` for exact oracle tests and paired callback
benchmarks. The scalar fallback and mechanical reductions retain their order.

On Linux/Core Ultra 7 165H, the actual decimator measured 13.9–18.1% faster at
factors 2/4/8. Across six paired whole-module runs per mode, median per-fixture
savings were 2.01% standalone and 1.27% with a static V.Tune message. One linked
Metal/Suede fixture regressed 1.21%; small callback differences remain noisy.
All 32 fixture audio/energy fingerprints matched. The complete Vessel suite,
strict serial/scalar host-rate checks, ASan/UBSan host-rate checks (leak detection
disabled for the ptrace environment), and full Linux plugin build passed.
Windows and live Rack results are pending.

See [the FIR experiment report](vessel_fir_optimization.md) for test bounds,
measurement limitations, reproduction targets and local evidence.
