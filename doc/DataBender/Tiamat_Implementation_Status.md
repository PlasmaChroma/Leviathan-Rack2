# Tiamat implementation status

2026-10-03: Phase 1 complete. Product name and source directory are **Tiamat**
and `src/Tiamat` (case intentional). The implementation follows the phased
contract in [Data_Bender_ImplementationSpec.md](Data_Bender_ImplementationSpec.md).

## Phase 1: reference foundation

Changed files:

- `src/Tiamat/TiamatState.hpp`: fixed renderer/coefficient rates, block and
  memory geometry, product defaults, separate control/clock/transition,
  reader/writer/Macro, Corrupt routing and output state types, small UI status.
- `src/Tiamat/TiamatRandom.hpp`: owned modulo-64-bit stream, explicit restart,
  exact modulo-255 normalization. This is the shared Macro/clock/Dropout stream;
  Vinyl's independent generators remain Phase 4 work.
- `src/Tiamat/TiamatControls.hpp/.cpp`: ideal Rack voltage mapping and overscan,
  previous-block Repeats exponent, both strict Micro octave-snap passes,
  independent effective mode flags, Mix endpoint target, Silence/Traverse,
  canonical width, normalized buffer frequency target and output Tone setup.
- `src/Tiamat/TiamatCorruptTables.hpp`: original Decimate tables for Phase 4.
- `tools/tiamat/generate_reference_fixtures.py`: offline standard-library
  extractor with `--check`; never executes or packages firmware.
- `tests/fixtures/tiamat/reference_v1.txt`: compact text records with firmware
  identity, source, setup, frame/draw counts and substitutions in the header.
- `tests/tiamat_reference_spec.cpp`: strict fixture loader and headless tests.
- `Makefile`: source discovery, dedicated `test-tiamat`, fixture generation and
  verification targets, fast-suite membership and local floating-point flags.
- `.gitignore`: narrow exception for the compact Tiamat reference fixture;
  other local/downloaded fixtures remain ignored.

Ownership remains deliberately separate from Rack/UI. The mapper accepts one
resolved block snapshot; it does not read ports, process buttons, schedule
events or mutate sample storage. Its transcendental calculations run at block
control cadence, never once per audio sample. DSP code has no firmware,
Unicorn, Python, graphics or Rack dependency.

Reference checks verify six tables as packed float32 values, the first twelve
outputs and exact 256-draw final states of each of four original-instruction
RNG cases, 30 Micro pitch/indicator fixtures and 19 Time fixtures. Tests also
cover independent RNG instances/restart, strict snap boundaries and adjacent
floats, every Repeats exponent and its block lag, clock ratio zones, normalized
capture geometry, Mix/Freeze target behavior, Micro depth bypass, full Silence,
inactive-path clearing, product defaults and nonfinite control inputs.

Tone's coefficient initialization is an offline float32 equation oracle, not
an executed-firmware capture. The operation order was checked against the
instruction listing at `0x08017acc`, including `vfma.f32` for `b*b-1`; its two
coefficient words match exactly on native MINGW64. The earlier output probe
bypasses Tone and is not evidence for its coefficients. Only the exp/pow-backed
control fixtures allow two float32 ULPs because their original probes replace
libm with Python math. Table words, RNG states, snapping/indicator decisions,
and Tone words use exact comparisons. No audio tolerance has been introduced.

Very large finite Bend CV is bounded to [-126, 120] octaves before exponentiation
to prevent intermediate overflow. Requested rates still exceed the reader's
eventual +/-8 clamp; ordinary and very slow negative CV remain supported.
This is defensive nonphysical-input handling, not an alternate musical voicing.

## Validation

- Native MINGW64 `make -j10 test-tiamat`: passed, including fixture freshness.
- Native MINGW64 `make -j10 plugin.dll`: linked successfully with Tiamat sources;
  final follow-up reports up to date.
- Native MINGW64 `make -j10 test-fast
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: Tiamat passed; suite
  stopped in `sibyl_module_spec` with `P5 complete companion
  expressive-composition fixture compiles` and `P6 combined source compiles`
  failures. No Sibyl source was changed. Remaining tests after that failure
  were not run by this invocation. Full output: `build/tiamat-validation.log`.
- No installation or Rack UI/audio smoke test: there is no registered Tiamat
  Rack model or panel yet. Nothing staged or committed.

## Next phases and remaining decisions

Phase 2 is next: zeroed sample planes, shared capture/history memory, signed
reader, Micro subdivision/traversal, exact normal windows and safe degenerate
endpoints. Port those only after reading their buffer and character evidence;
use frozen expansion/history and reader state fixtures as the exit conditions.

Phase 3 adds the normalized production clock, event order, Freeze acceptance,
Macro scheduling and shared draw consumption. Phase 4 adds all Corrupt DSP,
Vinyl generator restart policy, retained routing, mix/Tone processing and width.
Phase 5 adds the host-rate bridge, bounded commands, allocation/retirement,
Rack module/widget, persistence/reset, panel assets and installed validation.

The state structs here are typed foundations, not a claim that those algorithms
or transitions are implemented. No integrated callback emulation exists in the
reference evidence. Panel width/art direction remain deferred; branding is
settled as Tiamat. Waveform/display work stays outside v1.

The specification's evidence links currently use paths predating its move to
`doc/DataBender`: firmware evidence actually lives in `../../firmware/Data_Bender`
and the Windows build guide is `../windows_build_from_wsl.md`. This phase read
those actual files without changing the user's in-progress specification.
