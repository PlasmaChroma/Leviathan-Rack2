# Vessel implementation status

Current milestone, 2026-09-30: the standalone C++11 engine includes moving tangential friction, simultaneous strike/rub solving, control transitions, whole-run energy accounting, and the optional prescribed radial-load sensitivity experiment. The first Phase 4 foundation now adapts that engine to host rates through a causal stereo decimator. Default Metal/Suede develops a bounded several-rotation response from rest, and the user has auditioned the earlier rubbing/coupled previews with positive feedback. Vessel is now registered as a selectable, monophonic Rack prototype with a 16 HP panel, controls/CV, mechanical-energy display and versioned settings serialization. The user has also approved the causal host-rate auditions. The scalar dual-bowl reference and zero-separation single-path optimization now add a 0–33 Hz structural tuning difference, shared performance controls, independent contact states and mean-energy reporting. The broader physical, calibration, and convergence gates remain open.

## First milestone: modal mechanics and strikes

- `src/vessel/Types.hpp`: fixed-capacity modal and specimen types, with 12-pair storage.
- `src/vessel/SeedProfiles.hpp`: generated immutable bowl/mallet data from the reviewed JSON seeds. Profile generation preserves coefficient values and handles Windows/Linux newline conventions deterministically.
- `src/vessel/ModalBank.hpp/.cpp`: paired energy-normalized state, cached exact-pole coefficients, collocated force/velocity ports, radial/tangential rim shapes, finite patch averaging, passive midpoint stepping, and a static-compliance accuracy diagnostic.
- `src/vessel/StrikeContact.hpp/.cpp`: unilateral 3/2-power contact potential, a factored discrete-gradient force that avoids cancellation, loading-only loss, and a bounded safeguarded scalar solver.
- `src/vessel/VesselEngine.hpp/.cpp`: persistent bowl state, relative-approach launches, latched active-mallet properties, additive retriggers, explicit speed-cap work, separation/retirement accounting, fixed stereo virtual pickups, and finite-state/output recovery.
- `tests/vessel_engine_spec.cpp`: native C++ mechanical tests, also included in `test-fast`.
- `tools/vessel/`: profile generator, standalone internal-rate WAV/JSON renderer, and a NumPy offline preview converter.

The DSP has no Rack dependency, dynamic container growth, disk logging, locks, or GUI access. Coefficient transforms and fixed contact/observer shapes are cached at configuration boundaries. A few square roots remain in the active nonlinear contact because the potential and its force must agree. Full energy auditing is an explicit option for tests/renders; enable it before events to obtain a complete cumulative ledger. The audio owner must serialize engine calls; no cross-thread engine access is provided yet.

`configure()` is a setup/control boundary, not an audio-rate parameter smoother. It preserves normalized bowl state and active compression, rejects invalid coefficients transactionally, and currently requires matching modal topology for an already configured bank. Active mallet parameters/angle stay latched until separation; bowl mass/rate changes rebuild its reciprocal projection. The Rack adapter now supplies host gates, smoothing, descriptor morphing, sleep and atomic UI telemetry; the host-rate adapter supplies causal rate conversion.

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
- Configuration computes a conservative angle-independent uniqueness certificate at the maximum supported 15 N pressure. Uncertified descriptors are rejected without mutating the prior bank or contacts. Material changes at this setup boundary still latch an active striker's own mallet; the Rack adapter now supplies a 100 ms descriptor morph between the two compatible seed topologies.
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

Basic-frequency tuning is a required instrument control, already implemented through `EngineSettings::frequency` and the renderer’s `--pitch HZ` option. It scales the complete specimen spectrum, with the geometric center of the lowest pair at the requested frequency. The default is C4 (261.625565 Hz), with a 20–2000 Hz initial range. The Rack interface exposes PITCH/FINE and V/oct, with 1.5 ms log-frequency smoothing and coefficient updates at approximately 1 kHz. Audio-rate FM is not a validated feature.

The additional focused regression retunes 220 → 440 Hz during an active impact and 440 → 880 Hz during rubbing for both specimens and all four mallets. Every modal frequency follows the same ratio; mass and decay remain unchanged. Normalized tail state, mechanical energy, active compression/striker velocity, engagement and rotation angle survive unchanged at the update boundary. Continued simultaneous-contact runs preserve their energy ledger and rubbing speed without solver faults. All thirteen focused groups pass; the tuning log is `/tmp/vessel-tuning-tests.log`.

## Phase 4 foundation: causal host-rate adapter

The audition feedback supports proceeding toward a playable prototype. Crystal remains explicitly synthetic; meaningful crystal calibration needs reference recordings and contact/material measurements. Its shared-default failures and slow onset are still recorded rather than hidden by changing the seed data. Crystal voicing can follow when the live controls make comparative listening easier.

- `src/vessel/HostRateAdapter.hpp/.cpp` owns one mechanical engine and a fixed-capacity stereo decimator. Supported host rates are initially 32–192 kHz. It chooses the smallest factor in `{1,2,4,8}` yielding at least 176.4 kHz internally. All interactions advance at that rate. Already-detected host strike events are delivered once before the first substep; the Rack module now detects raw gate edges.
- Each 2× stage uses a symmetric 129-tap Blackman-windowed sinc, evaluated only on output samples with paired coefficients. Coefficients are generated at construction; processing has no allocations, locks or filter coefficient generation. Three fixed stages support up to 8×. At 1× the output path passes through without a decimation filter.
- The declared passband is through 0.40 of each stage's output rate and stopband starts at 0.50. A dense measured stage response gives approximately −75.29 dB maximum stopband amplitude and 0.00247 dB passband span. The tested cascade folded tones reject by at least 76.44 dB. These measurements characterize the filter, not aliasing already created inside the nonlinear solver.
- Output latency is fractional because emitted samples select the last internal lane while host frames are tagged at the first lane. The measured impulse centroids are 31.5/47.25/55.125 host samples for 2×/4×/8×. At 48 kHz, conventional FIR group delay is 1.00 ms and the host-tagged impulse delay is 0.984375 ms; at 32 kHz the tagged delay is 1.72265625 ms. No lookahead or zero-latency claim is made.
- Rate changes preserve normalized bowl/active striker state, reset filter delays, and blend from the last finite output into the new filter over 5 ms. Old delay samples are never reinterpreted at a new rate. Failed rate/descriptor updates leave the prior configuration intact. Reset clears both mechanics and filter/transition history. Mechanical faults silence and clear the output filter immediately.
- Filtered audio remains physical pickup velocity before Rack gain/DC handling. Published energy is current bowl mechanics, independent of delayed output and filter transients. Invalid speed/pressure inputs become safe finite targets; velocity rejection retains the core's existing policy.

### Host-rate validation

`make -j10 test-vessel` now runs the thirteen mechanical groups and three new host-rate groups. All sixteen pass on Linux. Tests measure filter response, folded-tone rejection, impulse latency, channel isolation and reset; compare all modal states/energy against a direct internal-rate reference at 32/44.1/48/88.2/96/176.4/192 kHz; verify single event delivery; and check transactional changes, active compression continuity, output blending and nonfinite controls. `make -j10 plugin.so` compiles and links the new adapter. The new host test also participates in `test-fast`; the previously recorded unrelated suite failures have not been repaired in this Vessel work.

Host validation logs are `/tmp/vessel-host-tests.log` and `/tmp/vessel-host-plugin-build.log`. Three host groups additionally pass under ASan/UBSan with leak detection disabled for the runner's ptrace restriction; log: `/tmp/vessel-host-sanitized.log`. Production CPU budgets and complete nonlinear aliasing measurements remain open. The subsequent Rack prototype adds sleep, pitch/control-rate smoothing, atomic telemetry, lifecycle/serialization and a panel, as recorded below.

### Audition the causal path

```sh
make -j10 build/tools/vessel_render_host
build/tools/vessel_render_host build/vessel-auditions/metal-suede-host-rub.wav \
  48000 261.625565 15 rub
build/tools/vessel_render_host build/vessel-auditions/metal-suede-host-coupled.wav \
  48000 261.625565 15 coupled
```

These two 48 kHz float WAVs were generated directly through the causal host adapter, with zero faults/caps and complete energy accounting. They contain unnormalized pickup velocity, not calibrated Rack volts. The coupled case strikes at 0.05/8 s and lifts contact at 12 s. The original offline preview converter is not involved.

## Playable Rack prototype

`src/Vessel.hpp`, `src/Vessel.cpp` and `src/VesselWidget.cpp` wrap the validated engine/host path. Registration in `plugin.json` and `plugin.cpp` makes **Vessel** available in the module browser after building/loading this plugin. The master artwork is `res/Vessel.svg`; its split runtime assets and panel-anchor atlas are generated. The 16 HP panel uses existing Eclipse2 knobs, Magitek2 jacks and manual/latching buttons. BOWL uses the shared two-position PlasmaSwitch (0 = Metal, 1 = Crystal prototype), preserving the parameter ID and descriptor morph behavior. The bar uses primitive NanoVG drawing and atomic scalar telemetry; it owns no graphics handles.

### Controls and behavior

- PITCH sets the lowest pair-center frequency, 20–2000 Hz, default C4. FINE spans ±100 cents; V/OCT adds pitch. Total frequency is clamped. Log pitch has a 1.5 ms smoothing time constant, with coefficient updates at approximately 1 kHz; this is a prototype control-rate path, not certified audio-rate FM.
- STRIKE uses 0.1/1 V Schmitt thresholds. An initially high cable triggers once; held gates do not repeat. Manual/cable edges in the same sample coalesce. Patched 0–10 V VELOCITY replaces the knob; zero velocity skips the event.
- ROTATE is a latch OR a sustained gate. SPEED is signed ±2 rev/s, with patched ±5 V multiplying the knob. PRESSURE is 0–15 N, with patched 0–10 V multiplying the knob. Existing core engagement/speed/pressure transitions remain in force.
- BOWL selects Metal or uncalibrated Crystal prototype; MALLET selects Wood, Suede, Silicone or Felt. Compatible descriptors morph over 100 ms while ringing/contact state continues. An active striker retains its latched contact material until separation. No seed calibration data changed.
- DECAY spans 0.25–4×, IMPERFECTION 0–2× pair splitting, WIDTH 0–1 and LEVEL 0–2×. WIDTH/LEVEL/output cable state do not drive the mechanics. The initial output mapping is 8 volts per m/s of pickup velocity times LEVEL, with an emergency ±20 V clamp. This gain is provisional rather than measured acoustic calibration.
- BOWL ENERGY publishes bowl-state energy at about 200 Hz, independently of LEVEL/WIDTH. The fixed prototype reference is 0.02 J, chosen near the characterized default late rubbing energy; the −60…0 dB display uses 5 ms attack/150 ms release. It is an engineering reference, not an acoustic-loudness measure.
- After 100 ms below `2e−14 J` with no active strike/contact, processing sleeps and outputs silence. The tiny remaining state is frozen; strike/rotation wakes immediately. Sleep ignores LEVEL and output connections. Bypass mutes output while mechanics continue.
- Patch settings retain base tuning, controls, ROTATE latch and stable specimen IDs (schema 1, bowl version recorded). Mechanical state starts at rest; manual STRIKE is saved/restored as zero. Unknown future schemas are ignored. Reset restores quiet defaults and clears mechanics/filter state on the audio owner.
- Debug-only Process/Step/Draw/DrawLayer telemetry follows the shared baseline helpers. The UI reads atomics and never accesses or advances the engine.

### Validation performed (Linux)

`make -j10 test-vessel` passes **21 groups**: thirteen mechanics, three causal host-rate and five new Rack-adapter groups. The Rack-linked tests exercise gates/retriggers, mono channel behavior, CV sanitation, smoothed octave/fine tuning, material morph while rubbing, live rate changes, exact mechanical independence from WIDTH/LEVEL, bypass mute, serialization, reset/sleep/wake, and zero C++ heap operations in the exercised initial/rubbing/strike/morph/pitch/rate/reset callbacks. Heap interception does not instrument direct C-library allocations or every Rack host path.

`make -j10 plugin.so` compiles and links the full Linux plugin. `make validate-plugin-json` and the shared panel background contract pass. A static panel composite was rendered with the actual knob/jack artwork and inspected for label/control collisions; preview: `build/vessel-auditions/vessel-panel-preview.png`. This is not a live Rack screenshot.

Logs: `/tmp/vessel-rack-tests.log` and `/tmp/vessel-rack-plugin-build.log`. The existing broader `test-fast` failures recorded above remain unresolved; the complete suite is not claimed to pass. The plugin was not installed. Native Windows module/UI validation and live Rack audio/window-reopen checks remain to be performed. These prototype checks do not establish the 32-instance CPU target, full-range pitch/bandwidth convergence, nonlinear alias rejection or measured crystal identity.

### Live Octavia check (2026-09-30)

The user's running Rack patch exposes Vessel through Octavia bridge 2.12.0 at 48 kHz. Actual cables route Vessel L/R directly to Octavia Master L/R, as well as to MixMasterJr; the observations therefore isolate Vessel from the mixer and Puffy. No patch parameters or cables were changed by the agent. A three-second resting capture contained exactly zero samples on both channels. A subsequent 30-second user-played capture (Rack frames 37956096–39396095) had peaks 3.218315/3.149465 V, RMS 0.634117/0.631670 V, DC offsets 0.000013/0.000019 V, zero detected clipping, and no detailed-analysis issues or detected 50/60 Hz hum. Controls read afterward showed Metal/Wood, ROTATE engaged, +0.4 rev/s, 2.5 N and fine tuning +4 cents.

A separate three-second metal-rotation capture (frames 41200103–41344102) had RMS 1.722633/1.747460 V, peaks 3.093906/3.112015 V, DC offsets 0.000634/0.000178 V, zero detected clipping, stereo balance +0.124286 dB and correlation 0.742657. These measurements establish functioning direct stereo output in the exercised live session. They do not establish full-range stability, precise tuning accuracy, solver-fault counts, window-reopen behavior or callback performance; debug mode was disabled. Captures were ephemeral and no patch was saved. Live audio has now been checked; the other integration gates above remain open.

## Dual-bowl reference (2026-09-30)

The user selected two physical simulations rather than pitch-shifting one output. `src/vessel/DualBowlAdapter.hpp/.cpp` owns two scalar `HostRateAdapter` instances, each with its own mechanics, contact solver, energy ledger and causal filter history. Shared controls/events are delivered once to each. The left output takes the lower bowl's left pickup; the right takes the upper bowl's right pickup. WIDTH remains observer placement; no output normalization feeds back into either bowl.

- Appended `BINAURAL_PARAM` (ID 13): **0–33 Hz total pair-center separation**, default zero. PITCH/FINE/V/oct set the center; frequencies are center ±half the separation. The effective center is clamped inward to `[20+Δ/2, 2000−Δ/2]`, preserving separation and the engine range. The existing PITCH range remains 20–2000 Hz.
- Live separation changes use a 10 ms one-pole smoother at the existing roughly 1 kHz control boundary. Both normalized mechanical states survive configuration changes while active. The always-dual reference retains divergent histories at zero; the default optimized path now fades to one bowl and discards the dormant history, as detailed below. A zero-difference pair from identical rest states matches the original single-bowl stereo output bit-for-bit.
- Existing parameter IDs and old patch defaults are preserved. Saved separation restores through Rack parameters. The development panel adds BINAURAL ΔHz, retains the PlasmaSwitch material selector and displays both actual pair-center frequencies. Controls are shifted upward to reserve a centered footer for the standard `Leviathan_Logo_S2.png` artwork, using the shared raster-image helper and SVG rect anchor as in Puffy. Panel width compacting/expander work remains deferred.
- The energy bar/raw energy now report `(E_left+E_right)/2`, preserving the old unison scale. Independent energy atomics remain available. Sleep requires both bowl energies below threshold and both contacts inactive. Reset clears both mechanics and filter histories. Bypass/output gain retain their prior policies.
- Pair configuration is transactional through fixed-size adapter copies: if the higher bowl rejects a descriptor, neither side adopts it. Passing `false` to the adapter constructor keeps the scalar reference processing both simulations even at zero difference. The default now enables single-mode fold/wake. No SIMD or shared FIR/coefficient optimization is claimed.

### Validation and cost

`make -j10 test-vessel` passes **25 groups**: thirteen mechanics, three host, three dual-reference and six Rack groups. New tests compare both outputs/energies bit-for-bit against two independently configured references at seven host rates and separations 0/1/10/33 Hz; check exact structural tuning, endpoints, single event delivery per bowl, rejected pair updates, independent state through zero/material/rate changes and reset; run four 15-second coupled/lift trajectories with independent energy accounting; and exercise Rack smoothing, serialization, mean-energy telemetry and heap-free callbacks including dual retuning. The largest cumulative energy residual in those trajectories is about `6.11e−12 J`, with zero faults/caps. Full Linux `plugin.so` linking and panel background checks pass. The new dual code has not yet been auditioned in live Rack or validated on native Windows.

`tools/vessel/benchmark_dual.cpp` measures audit-disabled, warmed offline processing at 48 kHz/192 kHz internal, C4 Metal/Suede, 33 Hz difference. Three four-second repeats per case follow a three-second warmup; single/dual order alternates. Setup, disk I/O, Rack controls and UI are excluded; configuration is measured separately. Linux GCC 12.3.0, C++11 `-O2 -fno-fast-math -fno-unsafe-math-optimizations`, Intel Core Ultra 7 165H. Wall-clock timing is unpinned, frequency scaling/background activity are uncontrolled, and these are means rather than worst-block budgets.

| Case | Single µs/host frame | Dual µs/host frame | Ratio |
|---|---:|---:|---:|
| Passive tail | 0.7191 | 1.6330 | 2.27× |
| Rubbing plus periodic strikes | 1.1975 | 2.3207 | 1.94× |

A separate 4,000-update loop averages about 3.154 µs per dual configuration. This is a baseline for optimization, not evidence that the 32-instance/5% target is met. Results: `build/vessel-dual-benchmark.csv`; focused/build logs: `/tmp/vessel-dual-tests.log`, `/tmp/vessel-dual-build.log`.

### Auditions and tuning interpretation

```sh
make -j10 build/tools/vessel_render_dual build/tools/vessel_benchmark_dual
build/tools/vessel_render_dual build/vessel-auditions/metal-suede-dual-10hz.wav 48000 261.625565 15 coupled 10
build/tools/vessel_render_dual build/vessel-auditions/metal-suede-dual-33hz.wav 48000 261.625565 15 coupled 33
build/tools/vessel_benchmark_dual > build/vessel-dual-benchmark.csv
```

The two WAVs contain causal, unnormalized physical pickup velocity, with accompanying JSON reports. Both strike at 0.05/8 seconds and lift at 12 seconds. Both bowls have zero faults/caps and cumulative ledger residual magnitude below `2.45e−12 J`. At 33 Hz, their pair centers are 245.125565 and 278.125565 Hz; final energies are about 0.0016573 and 0.0016364 J. At 10 Hz, final energies are about 0.0018555 and 0.0026666 J. Matching controls do not guarantee matching energies.

FFT peak estimates over seconds 9–12 (Hann window with log-magnitude parabolic interpolation) give dominant left/right tones approximately 255.91/267.42 Hz at the 10 Hz setting and 244.41/278.90 Hz at 33 Hz. Their dominant differences are approximately 11.50 and 34.49 Hz, respectively: the requested control is exact **pair-center separation**, while imperfection and nonlinear response can select different split modes. This is not a guaranteed exact binaural beat-rate generator. Offline reports: `build/vessel-auditions/metal-suede-dual-{10,33}hz-spectrum.json`.

## Zero-separation single-path optimization (2026-09-30)

The user requested an off-mode CPU comparison. `DualBowlAdapter` now defaults to a single simulation at zero separation from startup. After a nonzero session settles to exactly zero, both bowls continue for a **50 ms linear crossfade** of the right output from the upper bowl's right pickup to the surviving lower bowl's right pickup; the second simulation then stops. Left output/mechanics are never replaced. This preserves WIDTH and the surviving tail/contact. The existing Rack separation smoother reaches its exact-zero snap first (roughly 175 ms from 33 Hz with no further modulation), followed by the 50 ms fade.

A subsequent nonzero configuration copies the running adapter’s complete mechanics, latched striker/contact state, orbit/control state, energy ledger and both stereo FIR/transition histories into the second adapter, then tunes each bowl separately and crossfades the right output back over 50 ms. It does not catch up stale state or inject a new strike. Reversing a fade while both states are active retains their histories and reverses the mix ramp. Reset returns to an appropriate resting single/dual state. Pair updates remain transactional. `secondBowlActive()` and `dualMix()` expose runtime mode for tests/tooling.

The fade uses a convex linear mix, avoiding an equal-power boost when both paths coincide. Different phases/energies can still cause temporary cancellation or a level change; listening remains necessary. During the fade the energy display uses `E_left + 0.5 mix (E_right − E_left)`; full dual shows the mean and single shows the surviving bowl energy. Effective right-channel energy mirrors the survivor when single. This is display weighting, not output normalization or force feedback. Cloning branches the per-engine ledger lineage; it is not a conservation claim for physically creating/discarding a second bowl.

**Validation:** 26 Vessel groups pass (13 mechanics, 3 host, 4 dual/single and 6 Rack). A new independent filtered-history oracle exercises startup zero, active compressed-strike wake, divergent-to-single transitions, interrupted/reversed fades, repeated shared strikes, and width/material/rate changes. Outputs and surviving states match bit-for-bit; the second state is confirmed inactive after settling. Rack tests confirm the knob smoother actually reaches/stops the second path, and C++ heap interception includes fold and wake callbacks. The full Linux plugin links. Native Windows and live Rack checks of this optimization remain open. Logs: `/tmp/vessel-single-tests.log`, `/tmp/vessel-single-build.log`.

### Off-mode timing

The benchmark now compares a direct single adapter, always-dual zero, startup optimized zero, folded zero and dual 33 Hz. The always-dual-zero and folded-zero cases first run **matched three-second histories at 33 Hz**, then request zero. They each receive the same further three-second warmup and four-second measurement, with three repeats and alternating order. This isolates processing cost after the fade; it excludes its brief dual-work interval and clone/configuration spikes. Other environment/flags and unpinned wall-clock limitations are the same as the reference timing above. Results are `build/vessel-single-benchmark.csv`; no 32-instance Rack performance certification is implied.

| Case | Direct single µs/frame | Always-dual zero | Startup zero | Folded zero | Dual 33 Hz | Folded saving vs always-dual zero |
|---|---:|---:|---:|---:|---:|---:|
| Passive tail | 0.7462 | 1.5441 | 0.7740 | 0.7620 | 1.5719 | 50.65% |
| Rubbing/periodic strikes | 1.2285 | 2.4094 | 1.2217 | 1.2671 | 2.4925 | 47.41% |

The optimized/folded paths are close to direct single-adapter cost. The separate dual configuration loop averages 3.742 µs/update in this run. These scalar results establish useful off-mode savings in the exercised cases; they do not measure Rack UI, worst callback latency, native Windows or broader controls.

### Transition audition

```sh
make -j10 build/tools/vessel_render_dual build/tools/vessel_benchmark_dual
build/tools/vessel_render_dual build/vessel-auditions/metal-suede-single-transitions.wav 48000 261.625565 15 transition 33
build/tools/vessel_benchmark_dual > build/vessel-single-benchmark.csv
```

The WAV starts at 33 Hz, folds at 3 seconds, wakes at 6 seconds with 10 Hz, folds at 9 seconds, wakes at 11 seconds with 33 Hz, and folds at 13 seconds. It strikes at 0.05/8 seconds and lifts rotation at 12 seconds. These are direct adapter targets; the separate Rack 10 ms separation smoother is not in this offline harness. The final effective left/right energies agree at about 0.0033363 J, with zero reported final faults/caps and ledger residual about `−3.02e−12 J`. The report’s final right values refer to the surviving single path, not a sum over discarded histories.

## Baseline DSP optimization (2026-09-30)

The user reports **8–9% on Vessel’s per-module CPU meter**, even after folding to one bowl. The current Octavia status check could not reach the live bridge, so this pass measures offline DSP rather than that Rack callback. No plugin was installed or live patch changed.

An instrumented Linux `gprof` run identified the stereo decimator, engine step, moving contact orbit and modal loops as substantial work. Its self-time percentages are approximate sampling/instrumentation evidence, not production callback budgets. Profile: `/tmp/vessel-gprof-baseline.txt`.

Changes:

- Mirror each FIR ring to remove inner-loop wrap branches; SSE2 computes left/right in parallel with the original ordered arithmetic. A portable scalar backend remains available (`VESSEL_SCALAR_FIR`). All 129 coefficients, filter cadence and latency remain unchanged.
- Cache tangential gains at configuration and orbit half-step coefficients while the exact increment is unchanged. Skip unused normal projections and inactive contact dot products.
- Replace per-step remainder with bounded angle wrapping under the existing supported speed/rate limits.
- Expose small modal loops for compiler inlining; retain coefficient generation and finite-state validation in the strict-math implementation.

Internal rates, mode count, contact laws, nonlinear solve tolerances and iteration safeguards are unchanged. The mirrored history adds **6,192 bytes per host adapter**, or 12,384 bytes per two-bowl adapter. Configuration/wake copies include that extra fixed storage; the separate configuration loop increased from 3.175 to 3.880 µs/update in this comparison. Audio processing still performs no C++ heap operations in the exercised Rack callbacks.

### Paired offline timings

The before executable was retained before these optimizations. Both executables ran serially without concurrent build/test work, using the same benchmark and the environment/warmup/repeat conditions documented above. This is unpinned wall-clock timing with uncontrolled frequency scaling/background activity, not native Windows or live Rack evidence.

| Case | Path | Before µs/host frame | After µs/host frame | Reduction |
|---|---|---:|---:|---:|
| Passive tail | Startup zero | 0.7416 | 0.5204 | 29.83% |
| Passive tail | Folded zero | 0.7312 | 0.5217 | 28.66% |
| Passive tail | Dual 33 Hz | 1.4495 | 1.0370 | 28.46% |
| Rubbing/periodic strikes | Startup zero | 1.1290 | 0.9162 | 18.85% |
| Rubbing/periodic strikes | Folded zero | 1.1429 | 0.9287 | 18.74% |
| Rubbing/periodic strikes | Dual 33 Hz | 2.2821 | 1.8673 | 18.17% |

The optimized fold still saves approximately 49–50% relative to always running two bowls at zero. Results: `build/vessel-perf-before.csv`, `build/vessel-perf-after.csv`, `build/vessel-perf-comparison.json`. These savings do not directly predict the displayed 8–9% or establish the 32-instance target; Rack control/configuration work and native compiler behavior require a separate live comparison.

**Validation:** all 26 Vessel groups pass, and the full Linux `plugin.so` links and is up to date. The host tests additionally compare the new filter bit-for-bit with an independent implementation of the original wrapped scalar kernel over 10,000 randomized stereo samples at each factor 1/2/4/8. The portable scalar FIR build passes all three host groups. ASan/UBSan passes those three groups for the mirrored/SSE buffer revision; that sanitizer run preceded the final orbit-cache and inlining edits. Final focused/build logs: `/tmp/vessel-perf-tests.log`, `/tmp/vessel-perf-build.log`; scalar/sanitizer logs: `/tmp/vessel-scalar-fir-tests.log`, `/tmp/vessel-fir-sanitized.log`. Existing unrelated full-suite failures remain outside this pass.

## Modal layout and transactional tuning preparation (2026-09-30)

Following the fidelity-focused review, the modal bank now caches its five hot coefficient fields in contiguous arrays. The update loops preserve their scalar operation order and precision; coefficient generation remains unchanged. This adds 960 bytes per bowl (1,920 bytes per dual adapter).

Engine and host configuration now have private synchronous preparation/apply phases. Preparation retains a small modal-bank snapshot and checks the candidate coefficients and friction certificate. The dual adapter prepares both required bowls before applying either. Ordinary tuning leaves the large FIR histories and contact ledgers in place; a sample-rate change resets the decimator and starts the existing output transition only after successful preparation. Waking the second bowl still clones the complete running left adapter **before** applying either retune, so its compressed contact and pickup/filter histories preserve the previous behavior. These helpers are confined to the audio owner and are not an asynchronous preparation API.

The rejected-update test now compares 2,048 subsequent frames with an untouched adapter after left-valid/right-invalid preparation, for both an already-active right bowl and a proposed wake from single mode. Audio, energies, faults and activation/fade state must remain identical. All **26 Vessel groups pass**, including allocation checks and active-strike wake/rate/material transitions. Full Linux `plugin.so` linking succeeds and the target is up to date. Log: `/tmp/vessel-preparation-final-build-tests.log`.

An independent before/after executable comparison additionally matched the complete serialized audio, left/right energies and energy-residual records byte for byte over **1,624,080 host frames**. It covered seven host rates, both bowl profiles, four mallets, positive/negative rotation, strikes, lift, pitch/width changes and fold/wake transitions, with auditing enabled and no faults. The temporary probe is `/tmp/vessel-astra-review/probe.cpp`; compared streams are `/tmp/vessel-astra-review/baseline.bin` and `/tmp/vessel-preparation-after.bin`. This establishes numerical equivalence over those exercised trajectories, not every possible input.

### Timing

The benchmark now defaults to plugin-like `-O3 -march=nehalem` on x64 with strict math; `VESSEL_BENCH_OPT_FLAGS` allows an explicit alternative. Two complete runs per revision were taken serially in before/after/after/before order, without concurrent builds/tests. Each processing case retains its three warmed repetitions; separate 4,000-update loops measure pitch configuration at zero and 33 Hz separation. The table averages the two runs. These unpinned Linux wall-clock results remain subject to frequency scaling/background variation and exclude Rack UI/control overhead. They should not be combined directly with the earlier O2 timing tables.

| Case | Path | Before µs | After µs | Reduction |
|---|---|---:|---:|---:|
| Passive tail | Startup zero | 0.4790 | 0.4556 | 4.90% |
| Passive tail | Folded zero | 0.4840 | 0.4484 | 7.36% |
| Passive tail | Dual 33 Hz | 0.9728 | 0.9412 | 3.25% |
| Rubbing/periodic strikes | Startup zero | 0.8242 | 0.8018 | 2.71% |
| Rubbing/periodic strikes | Folded zero | 0.8287 | 0.7984 | 3.66% |
| Rubbing/periodic strikes | Dual 33 Hz | 1.6588 | 1.6119 | 2.82% |
| Single pitch configuration | Per update | 2.6995 | 1.3872 | 48.61% |
| Dual pitch configuration | Per update | 3.9255 | 2.7182 | 30.76% |

Processing rows are per host frame; configuration rows are per update. Source snapshot/baseline executable: `/tmp/vessel-preparation-before`. Results: `build/vessel-preparation-before.csv`, `build/vessel-preparation-after.csv`, their `-repeat.csv` counterparts and `build/vessel-preparation-comparison.json`. No model bandwidth, oversampling, contact curve or convergence tolerance was reduced. The isolated pickup-SIMD experiment was not adopted. Native Windows performance and the live Rack module meter remain unverified; no plugin installation was performed.

## Remaining gates

Continue mode-count/bandwidth convergence independently of time-step refinement, broader pitch/pressure/regularization sweeps, longer perturbation/hysteresis runs, dry recording comparisons and contact calibration. Measure actual nonlinear spectral aliasing and controlled callback performance; the new output-filter measurements alone do not establish either. Audit-disabled timings and audited renderer elapsed times are different measurements; the 32-instance target is not established. Next integration work is a live Rack audition and window-reopen smoke test, native Windows verification of the module/UI, and measured callback budgets. The first Rack controls, pitch smoothing, energy telemetry, panel anchors and settings serialization are implemented. Normal compliance/contact loss and spatial HRTF rendering remain separate later work. Dual-bowl tuning, SIMD FIR processing and cached orbit preparation are now implemented; further optimization follows controlled native/live callback measurement.
