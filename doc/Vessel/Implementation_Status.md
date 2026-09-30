# Vessel implementation status

First implementation milestone, 2026-09-30: the standalone paired modal core, finite-mass strike contact, full impact/event energy ledger, and offline strike renderer are implemented in C++11. The native Windows build and focused tests pass. Vessel is not yet registered as a selectable Rack module; the next mechanical milestone is the moving friction loop and simultaneous-contact solver.

## Implemented

- `src/vessel/Types.hpp`: fixed-capacity modal and specimen types, with 12-pair storage.
- `src/vessel/SeedProfiles.hpp`: generated immutable bowl/mallet data from the reviewed JSON seeds. Profile generation preserves coefficient values and handles Windows/Linux newline conventions deterministically.
- `src/vessel/ModalBank.hpp/.cpp`: paired energy-normalized state, cached exact-pole coefficients, collocated force/velocity ports, radial/tangential rim shapes, finite patch averaging, passive midpoint stepping, and a static-compliance accuracy diagnostic.
- `src/vessel/StrikeContact.hpp/.cpp`: unilateral 3/2-power contact potential, a factored discrete-gradient force that avoids cancellation, loading-only loss, and a bounded safeguarded scalar solver.
- `src/vessel/VesselEngine.hpp/.cpp`: persistent bowl state, relative-approach launches, latched active-mallet properties, additive retriggers, explicit speed-cap work, separation/retirement accounting, fixed stereo virtual pickups, and finite-state/output recovery.
- `tests/vessel_engine_spec.cpp`: native C++ mechanical tests, also included in `test-fast`.
- `tools/vessel/`: profile generator, standalone internal-rate WAV/JSON renderer, and a NumPy offline preview converter.

The DSP has no Rack dependency, dynamic container growth, disk logging, locks, or GUI access. Coefficient transforms and fixed contact/observer shapes are cached at configuration boundaries. A few square roots remain in the active nonlinear contact because the potential and its force must agree. Full energy auditing is an explicit option for tests/renders; enable it before events to obtain a complete cumulative ledger. The audio owner must serialize engine calls; no cross-thread engine access is provided yet.

`configure()` is a setup/control boundary, not an audio-rate parameter smoother. It preserves normalized bowl state and active compression, rejects invalid coefficients transactionally, and currently requires matching modal topology for an already configured bank. Active mallet parameters/angle stay latched until separation; bowl mass/rate changes rebuild its reciprocal projection. Host gates, smoothing, descriptor morphing, sleep, thread telemetry, and production rate conversion belong to the future Rack adapter.

## Validation performed

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

## Next mechanical gate

Implement the moving tangential contact, speed/pressure transitions, certified implicit friction law, and coupled strike/rub solve on this same modal state. Verify whole-run drive/loss accounting, stopped-contact damping, startup and bounded saturation over several rotations, regularization/rate sensitivity, and the prescribed radial-load comparison. Continue modal-bandwidth and contact calibration work while the independent harness is easy to change. After that gate, add the Rack controls, energy telemetry, panel anchors, serialization, and causal resampler.
