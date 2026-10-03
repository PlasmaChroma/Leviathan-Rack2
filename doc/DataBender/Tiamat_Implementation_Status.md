# Tiamat implementation status

2026-10-03: Phases 1–4, the Phase 5 host transport, and Phase 5b Rack integration are implemented. Product name and source directory are **Tiamat**
and `src/Tiamat` (case intentional). The implementation follows the phased
contract in [Data_Bender_ImplementationSpec.md](Data_Bender_ImplementationSpec.md).

## Phase 1: reference foundation

Changed files:

- `src/Tiamat/TiamatState.hpp`: fixed renderer/coefficient rates, block and
  memory geometry, product defaults, separate control/clock/transition,
  reader/writer/Macro, Corrupt routing and output state types, small UI status.
- `src/Tiamat/TiamatRandom.hpp`: owned modulo-64-bit stream, explicit restart,
  exact modulo-255 normalization. This is the shared Macro/clock/Dropout stream;
  Vinyl's independent generators are now implemented in Phase 4.
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
phase. At the end of Phase 3, seed restart covered the shared Macro/clock stream;
Phase 4 now extends it to Vinyl without resetting filter history.

The test-only Time driver preserves the original coefficient-rate fixture
setup; production uses the normalized 48 kHz musical clock. An additional
optional original-instruction trace now matches 55,296 channel-frame states,
frequencies and raw samples through frozen Time expansion after the arithmetic
fix. That probe uses the existing explicit rand/powf/memset substitutions and
does not establish complete callback or physical hardware equivalence.

## Phase 4: Corrupt and complete fixed-rate output

Added `TiamatCorrupt.hpp/.cpp`, `TiamatOutput.hpp/.cpp`, and
`TiamatCore.hpp/.cpp`. Core composes the existing 96-frame channel-major Buffer
with interleaved Corrupt, aligned dry/wet mix, Tone and sequential crossfeed.
It supports in-place input/output and sanitizes nonfinite input. This is a
complete fixed-rate headless signal path, not yet a Rack module or rate bridge.

All five effects preserve persistent state and primary/retained routing.
Dropout consumes the Buffer engine's shared RNG after both channel passes;
Vinyl owns its separate dust LCG and two modulo-32-bit multiplicative streams.
Restart uses the specified seed-derived five initializer draws, clears only
stochastic counters/holds, and retains deterministic filter/playhead history.
Vinyl bypass pauses enabled-frame time. Effect switches retain state. Parameter
coefficients are cached until effect/amount changes. Invalid filter arithmetic
resets the affected small filter state. Decimate uses defined integer masking
and saturates only at the int32 conversion boundary, not at nominal +/-1 audio.

`tests/tiamat_corrupt_spec.cpp` and `tools/tiamat/generate_corrupt_fixtures.py`
cover 64 short model vectors supported by the original differential evidence,
with all 12 stored original short vectors cross-checked during extraction.
Three 288,096-frame Vinyl runs have exact packed-audio SHA256 agreement with
the original recorded evidence before fixtures are emitted. The native test
then checks every 96-frame audio hash and stochastic-state checkpoint. All five
31-frame forced rare layers compare directly with original outputs/dust holds.
Fixtures also retain seven original Dropout and 16 isolated output cases.
`tests/fixtures/tiamat/corrupt_v1.txt` is self-contained and about 1.1 MiB;
runtime tests need neither Python nor firmware. Fixture regeneration is stdlib
only and reads the local independent model/evidence, never executes firmware.

A focused precision discrepancy was resolved and recorded in spec section 8:
Dropout must evaluate `(u*u)*376`, rounding after each multiply. Original
instructions confirmed both threshold regression decisions. Optional
`tools/tiamat/probe_corrupt_boundaries.py` reproduces them with Unicorn 2.1.4
and only `rand()` substituted; output is `build/tiamat-dropout-boundaries.json`.

Output uses a setup-built 4,097-point quarter-sine table, avoiding per-frame
trigonometry. One million phase intervals measured peak gain error
1.1920929e-7 against float32 sine. A 57,600-frame moving-mix render through Tone
and width measured peak audio difference 4.7683716e-7 with exact smoother state.
Set `TIAMAT_AB_RENDER=1` on the Corrupt test to write the actual/reference float
WAVs to `build/tiamat-output-lookup.wav` and `build/tiamat-output-reference.wav`.
They were generated in this validation pass; no listening equivalence is claimed.
Destroy exp and DJ tanh remain exact reference-sensitive per-sample math rather
than introducing an unmeasured nonlinear approximation. All coefficient
transcendentals run only when the selected effect/amount changes.

Additional tests cover retained bypass/selection, persistent Dropout toggling,
cross-instance independence, restart without filter resets, input headroom,
nonfinite/extreme input recovery, dry Tone impulse response, asymmetric width,
400 blocks of complete composition with Macro/Freeze/effect/restart changes,
in-place rendering, and exact shared RNG order. The allocation guard observed
zero allocations/deletions in 2,500 complete-core blocks with changing controls.
Final native runs averaged 16.55–16.67 us per 96-frame block, maxima 46.1–47.8 us;
these are host/load-dependent observations, not realtime deadline guarantees.

## Phase 4 validation

- Native MINGW64 `make -j10 test-tiamat plugin.dll`: all four Tiamat suites and
  fixture freshness passed; authoritative Windows plugin link succeeded.
  Final log: `build/tiamat-phase4-final.log`.
- WSL GCC AddressSanitizer + UndefinedBehaviorSanitizer, leak detection enabled:
  final Corrupt/output/full-core suite passed, including all 864,288 long Vinyl
  frames, safety endpoints and the additional Dropout regression cases.
- DJ model-comparison peak error: 0 on native MinGW, 1.1920929e-7 under WSL GCC;
  within the existing DJ-specific 2e-5 contract. Other effect fixtures remain
  exact; no tolerances were widened.
- Native `test-fast` with `RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`:
  Tiamat passed; the same unrelated Sibyl P5 companion and P6 combined-source
  fixture failures stop the suite. Log: `build/tiamat-phase4-fast.log`.
  The final Dropout fix was subsequently covered by the full Tiamat suite and
  sanitizer run. No Sibyl files changed; tests after its failure did not run.
- No Rack installation or UI/audio smoke test yet: module registration,
  host-rate transport and panel remain Phase 5. Nothing staged or committed.

## Phase 5a: host-rate transport and preparation ownership

Added `src/Tiamat/TiamatRateBridge.hpp`, `TiamatTransport.hpp` and
`tests/tiamat_rate_bridge_spec.cpp`. This is the transport portion of Phase 5;
Rack registration, panel, patch persistence and module reset are not implemented
yet. `BufferBlockInput` now accepts a secondary-settings snapshot applied after
restore-default commands and before event resolution.

The bridge uses Rack's existing Speex resampler at quality 5. Construction and
destruction happen off audio. At 48 kHz SRC is bypassed but the input/output
block transport retains exactly 96 frames of delay, including dry audio. Other
rates use `ceil(96*hostRate/48000)` frames of transport plus the measured Speex
input/output group delays. L/R remain aligned; unpatched R normals to L, with
V/5 and 5*y conversions only at the host boundaries. Both unpatched inputs
remain silent. Nonfinite input/output is contained.

Host controls are retained in bounded history and sampled from the source time
of the first frame in each core block, including input SRC delay. Host-rate
Schmitt edges are transported to the first eligible renderer frame after their
timestamp. Pulses shorter than a host block are not missed; clock edges less
than one core frame apart are rejected before quantization. The existing
96-frame event-class ordering and clock-request coalescing remain in the Core.
Initial/high gates contribute levels without synthesizing rises. Replacement
preserves Schmitt state inside the hysteresis band.

Transport has one audio owner and one serialized preparation owner. Call
`prepare()` regularly on the UI or a service worker, never in audio processing.
It publishes prepared bridges and reclaims retired ones through lock-free
atomic pointer slots. `step()` returns temporary silence while a requested rate
is unavailable, retains the live Core, then adopts the replacement and fades in
over 240 core frames after transport priming. Preparation must be stopped and
audio quiescent before destruction. The host-facing Rack wrapper will own this
lifecycle; no background thread is created implicitly by the transport.

Accepted but unrendered UI commands survive bridge replacement, including
commands in an incomplete old block and commands arriving during preparation.
Old audio-timeline gate edges are abandoned and current gate levels are reseeded.
Command and event queues have fixed bounds; exhaustion exposes a fault and
silence rather than silently losing an action. The wrapper still needs to
surface/recover that fault and provide its UI-to-audio command producer.

Validation covers ten seconds at each of 8/44.1/48/88.2/96/192/768 kHz, with
exact renderer-frame counts, bounded FIFOs, no underruns, matching stereo,
aligned parameter snapshots, single-host-frame gates and once-only commands.
Observed impulse peak delays in host frames were 96/169/96/326/352/704/2816;
fractional-rate predictions differ by less than one host frame. The fade is
checked independently at 44.1/48/96/192 kHz. Frozen captured samples survive a
48-to-96 kHz replacement bit-for-bit. Tests also exercise overload signaling,
invalid rates, short clock rejection, hysteresis seeding, command handoff and
concurrent preparation/adoption/retirement across repeated rate requests.

- Native `test-tiamat-transport` passes with the installed Rack runtime. It is
  now part of `test-fast`. The existing `test-tiamat` remains Rack-independent.
- WSL GCC ASan/UBSan with leak detection passes the complete transport suite.
  This builds the already-present `../Rack/dep/speexdsp/libspeexdsp/resample.c`
  locally with instrumentation, not a replacement resampler. Linker wrappers
  guard C malloc/calloc/realloc/free as well as C++ new/delete: no process-time
  allocation or deletion was observed. Native guards cover C++ allocations;
  the externally linked Rack DLL is not allocator-instrumented.
  Log: `build/tiamat-phase5a-sanitize.log`.
- WSL GCC ThreadSanitizer passes the focused replacement/concurrent-preparation
  case (`TIAMAT_TRANSPORT_CONCURRENCY_ONLY=1`) with instrumented local Speex,
  using `setarch x86_64 -R` and `TSAN_OPTIONS=halt_on_error=1`. This covers the
  exercised ownership exchanges, not every possible scheduling interleaving.
- The native plugin links with the updated Core/settings command path. The
  bridge/transport headers are instantiated by the native Rack-linked test;
  no Tiamat Rack model is registered yet.
- Final native `make -j10 test-fast
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: **PASS**, test summary
  109,950 checks / zero failures, followed by successful Phonex ROM freshness
  validation. Log: `build/tiamat-phase5a-fast-final.log`. The focused four core
  suites and fixture freshness also passed (`build/tiamat-phase5a-final.log`).
- `git diff --check` passes. No staging, commits, installation or Rack UI smoke
  test were performed in this phase.

The recurring Sibyl P5/P6 failures were stale test paths, not DSP/compiler bugs.
Both now read `doc/sibyl/Sibyl_v3_Example_Composition.json`, report missing files
explicitly, and pass, including the previously skipped combined module checks.
The subsequently reached Phonex panel assertion now checks the existing themed
label helper and both theme text assets. No Sibyl/Phonex production behavior was
changed. The native test runner also prefers `.exe` on MSYS/MINGW/Cygwin when a
same-named extensionless Linux artifact is present, preserving Linux selection
behavior. No artifacts were deleted to conceal that mixed-toolchain condition.

## Earlier phase validation

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

Phase 5b now registers the Tiamat model, provides the Rack module/widget,
bounded command queue/status snapshots, canonical persistence, prepared
full-reset replacement and a functional 18 HP panel/menu. Live Rack audition,
DAW window reopen checks, CPU/block-cost measurement and final artwork remain.

No integrated callback emulation exists in the reference evidence. The 18 HP
panel is functional artwork pending in-Rack review; branding is settled as
Tiamat. Waveform/display work stays outside v1.

The specification's evidence links currently use paths predating its move to
`doc/DataBender`: firmware evidence actually lives in `../../firmware/Data_Bender`
and the Windows build guide is `../windows_build_from_wsl.md`. This phase read
those actual files without changing the user's in-progress specification.

## Phase 5b: Rack integration

- `TiamatRuntime.hpp`: one preparation worker per module owns allocation,
  clearing, bridge preparation and retirement. Full reset/patch load swaps a
  cleared Core at a block boundary (or abandons the old partial block during a
  rate change). Audio waits silently if preparation is incomplete. Commands
  carry reset generations; obsolete commands are reclaimed even while waiting
  so repeated loads cannot fill the queue with superseded work. No widget is
  needed for preparation to continue. Control callers serialize through a
  mutex; audio uses bounded queues and fixed-size SPSC status snapshots.
- `TiamatModule.hpp/.cpp`: six primary knobs/CVs, six action buttons, clock and
  four gate inputs, mono-per-port stereo audio with L-to-R normaling, direct
  bypass, effective tooltips and mode/effect/Freeze/clock/fault lights. Level
  gate contributions light Bend/Break without changing their stored flags.
- `TiamatPersistence.hpp/.cpp`: schema/algorithm 1, canonical secondary values,
  independent mode flags and full-width uint64 seed strings. Malformed values
  default or clamp before narrowing. No Freeze request, audio, transient clock,
  filter or RNG position is saved. Primary knobs use Rack serialization.
- `TiamatWidget.cpp`: secondary sliders and behavior switches, decimal/hex seed
  entry, restart, secondary defaults and fault recovery. Shared Rack/repository
  graphics components; no module-specific GPU resources. `res/Tiamat.svg` is
  the editable 18 HP master, with generated outlined labels, separate themed
  input/output text and background, panel and atlas.
- `tests/tiamat_module_spec.cpp`: Rack-linked persistence, uint64/malformed
  input, independent flags, momentary/level controls, bypass/channel normaling,
  reset/rate-change adoption, overflow recovery and concurrent menu/reset/audio
  coverage. Thread-local C++ allocator guards detect process allocation or
  deletion; externally linked Rack DLL C allocations are not instrumented.
- `tests/tiamat_runtime_spec.cpp`: verifies nonempty recorded memory, full
  clearing of both memory planes after reset, and repeated concurrent reset,
  setting and sample-rate replacements. Both new tests join `test-fast` and
  `test-tiamat-module`; the original core tests remain Rack-independent.

Validation: native `make -j10 test-fast dist` with the installed Rack runtime
passed, including the two new Tiamat tests; the existing summary reports
109,950 checks and zero failures. Windows `plugin.dll` linked and
`dist/Leviathan-2.9.3-win-x64.vcvplugin` contains the Tiamat model and six SVG
assets. Final log: `build/tiamat-phase5b-final.log`. `git diff --check` passes.
WSL GCC
ASan/UBSan with leak detection and GCC TSAN (`setarch x86_64 -R`) passed the
runtime reset/settings/rate replacement test with locally instrumented Speex.
Logs: `build/tiamat-phase5b-asan.log`, `build/tiamat-phase5b-tsan.log`.
The functional panel master was rasterized and visually checked; this is not
an in-Rack widget/audio smoke test. No installation, staging or commits.
