# Tiamat implementation status

2026-10-03: Phases 1–3 implemented and headlessly validated. Product name and source directory are **Tiamat**
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

## Phase 2: deterministic buffer

Phase 2 focused validation (2026-10-03): `test-tiamat` passes natively with
7,680 original reader calls, eight window vectors, all recorded transitions of
seven Micro engine runs, both frozen history-overlap cases, and all recorded
checkpoints/memory-change counts of ten 76,800-frame Time trajectories. No
fixture tolerance was widened. Safe zero-length, short-window and maximum
capacity endpoints have separate product-contract tests. The 2,000-block
allocation guard detected no process-time allocations or deletions; one native
run measured 13.30 us mean / 31.8 us maximum for 96-frame blocks with rapidly
changing controls (timings are host/load-dependent, not hard realtime limits).

Phase 2 files: `TiamatBuffer.hpp/.cpp`, `TiamatMath.hpp`, the buffer test and
fixture generator/fixture, optional `probe_buffer_transition.py`, and Makefile
integration. The fixed-size public entry processes left then right. Constructor
and destructor own plane allocation/clearing/retirement outside processing.
Reduced capacities, arbitrary partitions and mutable preparation access are
test-only paths. Requested/active Freeze, pending acceptance, resizing and
write/read bank guards are retained in typed state for Phase 3 integration.

Two exactness findings were resolved: during Time instability the writer emits
at its old phase before wrapping to an accepted shorter extent; and native
MinGW `fmaf(.001f, -19.92576026916504f, 39.9317626953125f)` returned word
1109370296 where the original instructions/oracle return 1109370297. The latter
caused later capture timing differences. `TiamatMath.hpp` now explicitly uses
the oracle's binary64-intermediate, float32-rounded multiply-add expression.
The first divergent frame was located with an isolated original-code probe;
the regression vector and all long trajectories now pass exactly. The optional
probe needs Unicorn 2.1.4; routine tests and shipping code do not.

## Phase 3: events, Macro and production clock

Added `TiamatMacro.hpp`, `TiamatClock.hpp/.cpp`, `TiamatEvents.hpp` and
`TiamatBufferEngine.hpp/.cpp`, plus `tiamat_events_spec.cpp`, its compact fixture
and generator. The headless engine composes block controls, ordered button/gate
events, clock scheduling, retained-Corrupt draws and Buffer processing. Its
output is explicitly wet **before Corrupt/mix/Tone**; it is not a finished
effect or a variable-host-rate adapter.

Macro capture and subdivision decisions preserve original helper order, sticky
slew, thresholds, weighted rate table, unreachable silence-table endpoint,
Shared-mode copying and RNG consumption. Fixtures match all 68 direct helpers
and 20 complete block-partition cases, including exact final state, draw counts,
first stereo-difference frames and peak interchannel differences. Twenty Freeze
fixtures match acceptance order/frame and final state for crossing, silence,
countdown and channel-major cases. Gate latches and a held momentary Freeze
button are separate state, so releasing one does not cancel the other.

The production clock uses renderer seconds, median-of-three input intervals,
all nine ratios, explicit divider edge counts before block coalescing, internal
and external reset policies, loss/reacquisition, normalized capture geometry,
capacity indication and bounded arithmetic catch-up. Internal 2 Hz produces
20 boundaries in ten seconds, with settled capture near 24,000 frames. It
ignores invalid/too-short edges and does not double-trigger x3 boundaries due
to binary64 accumulation. Equal-time comparisons allow eight binary64 ULPs,
capped at a quarter-period; this is arithmetic coincidence handling, not a
musical timing quantizer. Time edits preserve fractional scheduled progress;
source switches start a new period. External mode waits for its first edge,
then maintains the specified estimate/loss continuation.

The fixed event block has stable class/timestamp/arrival ordering and reports
overflow instead of silently discarding commands. This is an audio-owner value
interface; a UI-to-audio mailbox and the latency-aware host transport still
belong to Phase 5. Full Rack reset will swap prepared cleared state in that
phase. Seed restart currently covers the shared Macro/clock stream; Phase 4
must extend it to Vinyl's separate generators without resetting filter history.

The test-only Time driver preserves the original coefficient-rate fixture
setup; production uses the normalized 48 kHz musical clock. An additional
optional original-instruction trace now matches 55,296 channel-frame states,
frequencies and raw samples through frozen Time expansion after the arithmetic
fix. That probe uses the existing explicit rand/powf/memset substitutions and
does not establish complete callback or physical hardware equivalence.

## Validation

- Native MINGW64 `make -j10 test-tiamat`: passed, including fixture freshness.
- Native MINGW64 `make -j10 plugin.dll`: linked successfully with all Tiamat
  sources. Focused validation log: `build/tiamat-phase3-final.log`.
- WSL GCC 11.4 AddressSanitizer + UndefinedBehaviorSanitizer, leak detection
  enabled: buffer and event suites passed. Includes full-capacity safety,
  768,000 Time-trajectory frames, Macro/Freeze fixtures and clock/event integration.
- Original-instruction focused trace: 55,296 channel-frame comparisons passed.
- Native MINGW64 `make -j10 test-fast
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: Tiamat passed; suite
  stopped in `sibyl_module_spec` with `P5 complete companion
  expressive-composition fixture compiles` and `P6 combined source compiles`
  failures. No Sibyl source was changed. Remaining tests after that failure
  were not run by this invocation. The same failures recur after this push.
  Full output: `build/tiamat-phase3-fast.log`.
- No installation or Rack UI/audio smoke test: there is no registered Tiamat
  Rack model or panel yet. Nothing staged or committed.

## Next phases and remaining decisions

Phase 4 is next: all five Corrupt effects, Vinyl generator restart policy,
retained routing, mix/Tone processing and width. Compare original short vectors
and long Vinyl rollovers/pause/rare events before integrating the output chain.
Phase 5 adds the host-rate bridge, bounded commands, allocation/retirement,
Rack module/widget, persistence/reset, panel assets and installed validation.

No integrated callback emulation exists in the reference evidence. Panel
width/art direction remain deferred; branding is
settled as Tiamat. Waveform/display work stays outside v1.

The specification's evidence links currently use paths predating its move to
`doc/DataBender`: firmware evidence actually lives in `../../firmware/Data_Bender`
and the Windows build guide is `../windows_build_from_wsl.md`. This phase read
those actual files without changing the user's in-progress specification.
