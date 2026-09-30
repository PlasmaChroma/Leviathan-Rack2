# Vessel implementation status

Current milestone, 2026-09-30: the standalone C++11 engine includes moving tangential friction, simultaneous strike/rub solving, control transitions, whole-run energy accounting, and the optional prescribed radial-load sensitivity experiment. The first Phase 4 foundation now adapts that engine to host rates through a causal stereo decimator. Default Metal/Suede develops a bounded several-rotation response from rest, and the user has auditioned the earlier rubbing/coupled previews with positive feedback. Vessel is not yet registered as a selectable Rack module. The broader physical, calibration, and convergence gates remain open.

## First milestone: modal mechanics and strikes

- `src/vessel/Types.hpp`: fixed-capacity modal and specimen types, with 12-pair storage.
- `src/vessel/SeedProfiles.hpp`: generated immutable bowl/mallet data from the reviewed JSON seeds. Profile generation preserves coefficient values and handles Windows/Linux newline conventions deterministically.
- `src/vessel/ModalBank.hpp/.cpp`: paired energy-normalized state, cached exact-pole coefficients, collocated force/velocity ports, radial/tangential rim shapes, finite patch averaging, passive midpoint stepping, and a static-compliance accuracy diagnostic.
- `src/vessel/StrikeContact.hpp/.cpp`: unilateral 3/2-power contact potential, a factored discrete-gradient force that avoids cancellation, loading-only loss, and a bounded safeguarded scalar solver.
- `src/vessel/VesselEngine.hpp/.cpp`: persistent bowl state, relative-approach launches, latched active-mallet properties, additive retriggers, explicit speed-cap work, separation/retirement accounting, fixed stereo virtual pickups, and finite-state/output recovery.
- `tests/vessel_engine_spec.cpp`: native C++ mechanical tests, also included in `test-fast`.
- `tools/vessel/`: profile generator, standalone internal-rate WAV/JSON renderer, and a NumPy offline preview converter.

The DSP has no Rack dependency, dynamic container growth, disk logging, locks, or GUI access. Coefficient transforms and fixed contact/observer shapes are cached at configuration boundaries. A few square roots remain in the active nonlinear contact because the potential and its force must agree. Full energy auditing is an explicit option for tests/renders; enable it before events to obtain a complete cumulative ledger. The audio owner must serialize engine calls; no cross-thread engine access is provided yet.

`configure()` is a setup/control boundary, not an audio-rate parameter smoother. It preserves normalized bowl state and active compression, rejects invalid coefficients transactionally, and currently requires matching modal topology for an already configured bank. Active mallet parameters/angle stay latched until separation; bowl mass/rate changes rebuild its reciprocal projection. Host gates, smoothing, descriptor morphing, sleep, thread telemetry, and production rate conversion belong to the future Rack adapter.

## First milestone validation (native Windows)

Native MSYS2 MINGW64, GCC 16.1.0, C++11, on this Windows machine:

| Check | Result |
|---|---|
| `make -j10 test-vessel` | Eight test groups pass. |
| Exact free-pole trace/determinant | Both profiles; five internal rates; 20 Hz, C4, 2 kHz; three decay multipliers. |
| Randomized collocated energy | 10,000 cases; maximum residual approximately `3.57e−15 J`. |
| Constant-force equilibrium | Digital static compliance agrees with the reported prewarping ratio. |
| Independent elastic ground impact | Correct energy, peak indentation, and restitution. Interpolated contact-time error shrinks from about `1.29e−4` to `8.09e−6` across 192/384/768 kHz. |
| Complete bowl impacts | 384 profile/mallet/velocity/rate trajectories; separation and retirement included; maximum cumulative energy residual approximately `4.53e−14 J`; no ordinary solver faults or speed caps. |
| Default C4 rate convergence | Eight bowl/mallet combinations at half velocity. Largest 192 kHz versus 768 kHz relative differences: transferred energy `2.02e−4`, impulse `4.56e−5`, peak force `7.38e−4`. |
| Event/material/rate transitions | Zero events, active contact material/angle latching, ringing retriggers, caps, and sample-rate changes pass their energy audit. |
| Stereo and tail | Observer changes preserve mechanics exactly; coincident channels null; a 120-second free tail remains finite and decays. |
| `make -j10 plugin.dll` | New engine translation units compile and link into the native Windows plugin. |
| `make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"` | Vessel passes. Suite stops in `sibyl_module_spec`: two existing fixture tests cannot load the absent `doc/Sibyl_v3_Example_Composition.json`. Later suite commands were not reached. |

The focused log is `build/vessel-validation.log`; the full-suite failure log is `build/vessel-test-fast.log`. The missing-fixture readers are in `tests/sibyl_harmony_cases.hpp` and `tests/sibyl_combined_cases.hpp`; neither their sources nor Sibyl's runtime implementation changed in this milestone.

These checks validate the stated numerical mechanics over the exercised cases. They do not establish real-bowl calibration, sufficient structural bandwidth for every hard/low-pitch strike, convergence over the full controls, realistic crystal identity, alias-free nonlinear output, or production CPU cost. Those remain the reviewed specification's gates.

## Run and listen

Inside the documented native MINGW64 environment:

```sh
make -j10 test-vessel build/tools/vessel_render plugin.dll
mkdir -p build/vessel-auditions
build/tools/vessel_render \
  --bowl metal --mallet suede --seconds 8 --retrigger-at 2.5 \
  --output build/vessel-auditions/metal-suede-raw.wav \
  --report build/vessel-auditions/metal-suede-mechanics.json
```

The renderer writes 192 kHz stereo float WAV by default. Values are physical virtual-pickup velocities in m/s. Its JSON records contact force/indentation, energy accounting, and numerical accuracy. The reported elapsed time includes auditing and file I/O and is not a Rack performance benchmark. `--help` lists the other prototype profiles and options.

For a 48 kHz listening preview, using a Python environment with NumPy:

```sh
python tools/vessel/make_preview.py \
  build/vessel-auditions/metal-suede-raw.wav \
  build/vessel-auditions/metal-suede-preview.wav \
  --normalize --report build/vessel-auditions/metal-suede-preview.json
```

The saved preview uses an offline Blackman-windowed sinc filter and removes its delay before integer-ratio downsampling. The measured filter stopband for the 192-to-48 kHz example is approximately −107 dB; this measures the **offline filter**, not aliasing already generated by nonlinear contact. `--normalize` sets preview peak to 0.8 after filtering and cannot change the mechanical render. A causal production resampler and its latency still need implementation and measurement.

Initial metal/suede (two strikes) and crystal-prototype/silicone (one strike) previews and mechanical reports were generated in `build/vessel-auditions/`. Each render has zero solver faults and zero caps. Listening fixtures are engineering auditions; no comparison with a measured real bowl was performed.

## Phase 3 reference implementation

- `FrictionContact.hpp/.cpp`: regularized velocity-weakening friction and analytic derivative, bounded Newton/bisection solver, and persistent midpoint-angle rim ports. Small angle recurrences avoid per-sample trigonometry for the supported factory orders/speeds; periodic normalization bounds drift. The curve remains the analytic reference rather than an independently approximated force/derivative pair. Gaussian evaluation and unsaturated adhesion still use transcendental functions to retain the reference law; production optimization must match these mechanics and certificates.
- `ContactSolver.hpp/.cpp`: nested outer strike/inner friction solve on one shared midpoint modal state, with a finite bracket and bounded iteration fallback. After twelve Newton iterations the solvers use bisection; this prevents slow edge convergence observed in the Crystal/Silicone maximum-pressure reversal test. The 12-iteration production objective is not yet a proven worst-case budget.
- `VesselEngine::setRotation()`: independent engagement, signed speed and pressure targets. Engagement rises linearly over 10 ms and releases over 5 ms; speed and pressure use cached 10 ms one-pole smoothing. Midpoint speed drives both slip and orbit. Orbit continues through release and freezes only when contact is fully lifted; engaging again preserves angle. Targets are finite/range-checked transactionally.
- Configuration computes a conservative angle-independent uniqueness certificate at the maximum supported 15 N pressure. Uncertified descriptors are rejected without mutating the prior bank or contacts. Material changes at this setup boundary still latch an active striker's own mallet; smooth descriptor morphing remains future work.
- The energy ledger now includes hand work, interface friction loss, and signed prescribed radial work. Stationary friction dissipates mechanical energy; zero physical load produces exactly zero force. Observer changes and disabling the energy audit preserve the rubbing state bit-for-bit.
- `EngineSettings::prescribedRadialLoad` adds the known inward load before solving contacts and audits its midpoint work. Default remains the explicitly labelled tangential-only proxy. This experiment provides neither compliant normal following nor contact loss.
- The offline renderer accepts rotation, stop, lift, pressure/speed, and radial-load options. Reports contain half-second energy/RMS/work windows, force/energy/deformation diagnostics, and per-step iteration histograms. For coupled steps, the friction histogram records the accepted inner solve's iteration count; the maximum also includes inner trials. These are not total solver-operation counts.
- `tools/vessel/characterize.cpp`: reproducible 15-second seed/rate, pressure/speed, regularization, radial-load, coupled-strike, constant-friction-control, and pitch diagnostics. Results are in `friction_characterization_results.csv`, with environment and interpretation in `friction_characterization_metadata.json`.

### Validation performed (Linux)

This continuation ran on Linux kernel 6.8 with a matching Linux Rack SDK, GCC 12.3.0, C++11. No Windows bridge is mounted in this environment; these results do not replace a native Windows validation of the new contact sources.

| Check | Result |
|---|---|
| `make -j10 test-vessel` | All thirteen groups pass, including the original eight strike groups and live basic-frequency retuning. |
| Independent friction oracle | 10,000 randomized roots agree with a separate bracket-only implementation; force oddness, derivative, interface passivity and negative-slope bound pass. |
| Moving ports | 400,000 forward/reverse steps agree with direct midpoint trigonometric ports, including normalization boundaries. |
| Independent coupled oracle | 1,000 random-state nested roots agree with a separate bisection/long-double potential-quotient reference; maximum one-step energy residual about `4.25e−17 J`. |
| Stopped/lifted contact | At zero hand speed, rubbing loses energy with zero hand work and damps a ringing bowl more than lifting it. Zero initial pressure leaves the strike trajectory unchanged. |
| Sustained and perturbed runs | Both default proxy and radial-load experiment pass 12-second startup/late-energy checks, followed by reversal, pressure changes, periodic strikes, material/rate switches and lift. Maximum cumulative residual about `5.71e−12 J`; zero faults/caps. |
| Maximum-pressure regression | All eight seed combinations at 176.4/384 kHz pass 100 ms runs with strikes and reversal at 15 N. These are transition tests, not 15 N sustained-tone certification. |
| ASan + UBSan | Twelve test groups pass with no reported address/undefined-behavior errors. Leak detection was disabled because LeakSanitizer cannot operate under this runner's ptrace environment. |
| 51 characterization trajectories | All complete without faults/caps; maximum cumulative residual about `8.95e−11 J`, maximum step residual about `5.94e−17 J`. |
| `make -j10 plugin.so` | Full plugin compiles and links with both new contact translation units. |
| `make -j10 test-fast` | Vessel passes. Sandboxed run stops at localhost port allocation. Retried outside the sandbox: stops in the unmodified Sibyl harness on two absent-fixture failures plus a concurrent adoption/display/reclamation assertion. Later suite commands were not reached. |

Logs are `/tmp/vessel-tests.log`, `/tmp/vessel-sanitized.log`, `/tmp/vessel-plugin-build.log`, and `/tmp/vessel-test-fast-unsandboxed.log`. The fixture readers still reference the absent `doc/Sibyl_v3_Example_Composition.json`; Sibyl sources were not changed.

### Characterization findings and limits

Default Metal/Suede at C4, +0.4 rev/s and 2.5 N crosses the diagnostic `1e−4 J` bowl-energy threshold at about 1.49 s without a strike. Mean bowl energy over 12–15 s is about `0.01635 J`; the 9–12 to 12–15 s mean grows by about 1.1%. The fundamental pair carries nearly all the late modal energy. The constant-friction control (`muS = muK`) stays near `3.13e−7 J` and never crosses that onset threshold, distinguishing self-excited growth from the travelling-force/engagement response. These energy metrics are not psychoacoustic onset or real-bowl calibration.

The local 0.3/0.4/0.5 rev/s × 2/2.5/3 N grid crosses the same threshold in about 0.98–4.23 s; −0.4 rev/s also starts. Several nondefault cases continue to evolve appreciably at 15 s. Factory seeds are not equally playable: Crystal/Wood never crosses the threshold at the shared default; Metal/Wood and Metal/Felt take roughly 7–8 s; Crystal/Felt takes roughly 10.6 s. Metal/Silicone has substantial late-energy variation. No seeds were altered to conceal these findings.

At C4, 192 versus 768 kHz differences in the 12–15 s mean energy and observer RMS remain below about 0.034% across the eight seed combinations. For default Metal/Suede the energy difference is about 0.0016%. Halving/doubling its regularization parameter changes late mean energy by at most about 0.05% in these runs. These averaged diagnostics do not measure folded spectral components or cover the full pitch/control range.

Prescribed radial load advances default onset to about 1.00 s and changes late mean energy by about 0.18%. Its signed work is separately accounted; this narrow result does not justify dropping normal interaction across all settings. The maximum conservative radial-displacement bound is about 0.7% of rim radius at default C4, but about 8.6% at 20 Hz. The low-pitch small-deformation interpretation needs review; that bound is neither an actual maximum over rim angle nor measured shell strain.

### Reproduce and listen

```sh
make -j10 test-vessel build/tools/vessel_render build/tools/vessel_characterize
build/tools/vessel_characterize > build/vessel-characterization.csv
mkdir -p build/vessel-auditions
build/tools/vessel_render --velocity 0 --rotate-at 0.05 --seconds 15 \
  --output build/vessel-auditions/metal-suede-rub-raw.wav \
  --report build/vessel-auditions/metal-suede-rub-mechanics.json
python3 tools/vessel/make_preview.py \
  build/vessel-auditions/metal-suede-rub-raw.wav \
  build/vessel-auditions/metal-suede-rub-preview.wav --normalize
```

Saved 48 kHz previews now include `metal-suede-rub-preview.wav` and `metal-suede-coupled-preview.wav` in `build/vessel-auditions/`. The coupled preview includes strikes at 0.05/8 s and contact lift at 12 s. Listening against a real dry bowl has not been performed.

## Basic-frequency tuning

Basic-frequency tuning is a required instrument control, already implemented through `EngineSettings::frequency` and the renderer’s `--pitch HZ` option. It scales the complete specimen spectrum, with the geometric center of the lowest pair at the requested frequency. The default is C4 (261.625565 Hz), with a 20–2000 Hz initial range. The Rack interface will expose PITCH/FINE and V/oct; log-frequency smoothing and efficient CV coefficient updates still belong to that adapter.

The additional focused regression retunes 220 → 440 Hz during an active impact and 440 → 880 Hz during rubbing for both specimens and all four mallets. Every modal frequency follows the same ratio; mass and decay remain unchanged. Normalized tail state, mechanical energy, active compression/striker velocity, engagement and rotation angle survive unchanged at the update boundary. Continued simultaneous-contact runs preserve their energy ledger and rubbing speed without solver faults. All thirteen focused groups pass; the tuning log is `/tmp/vessel-tuning-tests.log`.

## Phase 4 foundation: causal host-rate adapter

The audition feedback supports proceeding toward a playable prototype. Crystal remains explicitly synthetic; meaningful crystal calibration needs reference recordings and contact/material measurements. Its shared-default failures and slow onset are still recorded rather than hidden by changing the seed data. Crystal voicing can follow when the live controls make comparative listening easier.

- `src/vessel/HostRateAdapter.hpp/.cpp` owns one mechanical engine and a fixed-capacity stereo decimator. Supported host rates are initially 32–192 kHz. It chooses the smallest factor in `{1,2,4,8}` yielding at least 176.4 kHz internally. All interactions advance at that rate. Already-detected host strike events are delivered once before the first substep; raw gate edge detection remains the future Rack module's responsibility.
- Each 2× stage uses a symmetric 129-tap Blackman-windowed sinc, evaluated only on output samples with paired coefficients. Coefficients are generated at construction; processing has no allocations, locks or filter coefficient generation. Three fixed stages support up to 8×. At 1× the output path passes through without a decimation filter.
- The declared passband is through 0.40 of each stage's output rate and stopband starts at 0.50. A dense measured stage response gives approximately −75.29 dB maximum stopband amplitude and 0.00247 dB passband span. The tested cascade folded tones reject by at least 76.44 dB. These measurements characterize the filter, not aliasing already created inside the nonlinear solver.
- Output latency is fractional because emitted samples select the last internal lane while host frames are tagged at the first lane. The measured impulse centroids are 31.5/47.25/55.125 host samples for 2×/4×/8×. At 48 kHz, conventional FIR group delay is 1.00 ms and the host-tagged impulse delay is 0.984375 ms; at 32 kHz the tagged delay is 1.72265625 ms. No lookahead or zero-latency claim is made.
- Rate changes preserve normalized bowl/active striker state, reset filter delays, and blend from the last finite output into the new filter over 5 ms. Old delay samples are never reinterpreted at a new rate. Failed rate/descriptor updates leave the prior configuration intact. Reset clears both mechanics and filter/transition history. Mechanical faults silence and clear the output filter immediately.
- Filtered audio remains physical pickup velocity before Rack gain/DC handling. Published energy is current bowl mechanics, independent of delayed output and filter transients. Invalid speed/pressure inputs become safe finite targets; velocity rejection retains the core's existing policy.

### Host-rate validation

`make -j10 test-vessel` now runs the thirteen mechanical groups and three new host-rate groups. All sixteen pass on Linux. Tests measure filter response, folded-tone rejection, impulse latency, channel isolation and reset; compare all modal states/energy against a direct internal-rate reference at 32/44.1/48/88.2/96/176.4/192 kHz; verify single event delivery; and check transactional changes, active compression continuity, output blending and nonfinite controls. `make -j10 plugin.so` compiles and links the new adapter. The new host test also participates in `test-fast`; the previously recorded unrelated suite failures have not been repaired in this Vessel work.

Host validation logs are `/tmp/vessel-host-tests.log` and `/tmp/vessel-host-plugin-build.log`. Three host groups additionally pass under ASan/UBSan with leak detection disabled for the runner's ptrace restriction; log: `/tmp/vessel-host-sanitized.log`. Production CPU budgets, sleep, pitch/control-rate smoothing, telemetry synchronization, Rack lifecycle/serialization and UI remain future work.

### Audition the causal path

```sh
make -j10 build/tools/vessel_render_host
build/tools/vessel_render_host build/vessel-auditions/metal-suede-host-rub.wav \
  48000 261.625565 15 rub
build/tools/vessel_render_host build/vessel-auditions/metal-suede-host-coupled.wav \
  48000 261.625565 15 coupled
```

These two 48 kHz float WAVs were generated directly through the causal host adapter, with zero faults/caps and complete energy accounting. They contain unnormalized pickup velocity, not calibrated Rack volts. The coupled case strikes at 0.05/8 s and lifts contact at 12 s. The original offline preview converter is not involved.

## Remaining gates

Continue mode-count/bandwidth convergence independently of time-step refinement, broader pitch/pressure/regularization sweeps, longer perturbation/hysteresis runs, dry recording comparisons and contact calibration. Measure actual nonlinear spectral aliasing and controlled callback performance; the new output-filter measurements alone do not establish either. Audit-disabled timings and audited renderer elapsed times are different measurements; the 32-instance target is not established. Next prototype work is Rack controls, pitch smoothing, energy telemetry, panel anchors and serialization on the causal host path. Normal compliance/contact loss and binaural processing remain separate later work.
