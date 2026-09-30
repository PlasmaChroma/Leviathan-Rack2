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
