# SP67 Rack reconstruction specification — continuation, 2026-10-04

This continuation supersedes the initial report's **gap status**, not its
preserved binary or disassembly. The target is a reproducible digital core for
Rack. No firmware was flashed and no Rack module is implemented by this bundle.

## 1. What is now independently checked

`reference/sp67_extended.py` contains readable, instruction-scheduled equations.
`reference/sp67_arm.py` executes bounded slices of the original, hash-checked
firmware. `tests/test_arm_differential.py` supplies explicit registers and RAM,
executes the firmware, then compares its results with the authored equations.

| Path | Checks | Result |
|---|---:|---|
| Chaos A/B | 1,200 samples, state and two outputs | Bit-exact for tested inputs |
| Noise A/B | 1,200 samples, generator/filter/envelope/RNG state and outputs | Bit-exact for tested inputs |
| Normal phase, cross-FM, Sub/CV phase | 400 state transitions | Bit-exact for tested inputs |
| Six Sub/CV selections, SAM and SAO, A/B | 480 values | Bit-exact for tested inputs |
| A clipping and eight output stores | 200 clip pairs, 200 output frames | Bit-exact; both DMA halves |
| Standard synthesis, coefficient selection/ramping/cutoff | 384 samples | Mathematical model agrees within 0.000015508 absolute error |
| Capture writer | 24 cases | Exact writes, gating, peak, full-slot state |
| Clock interrupt callback | 12 cases | Exact flag/count updates including uint32 rollover |
| Startup mute | 3 boundary cases | All eight lanes zero; release threshold checked |
| Even-rate and B linked-pitch laws | 64 even-rate, 42 B pitch cases | Bit-exact, including piecewise boundary and LF branches |
| Joined synthesis/interaction/output path | 900 consecutive samples, all engines and raw interaction modes | Exact phase/RNG/state and Noise/Chaos outputs; standard output within tolerance |
| Slow ADC control routing | 100 six-slot cases | Exact normalized controls, FM indices and instruction-scheduled analysis rate/pole |
| Cold Noise clipping | 2 zero-envelope cases | Quiet NaN before clipping, positive clip endpoint afterward |
| Planar Array reader | 256 cases on both sides | Exact read addresses and valid-Array coefficient deltas; 46 cases deliberately expose logical out-of-bounds reads |
| Complete button handler | 10 capture start/stop cases, 6 interaction presses | Exact selected descriptor/state changes and raw interaction cycle |
| Clock ISR through complete UI handler | 216 cases | Side routing, countdown, offset wrap, period boundaries and rate/table selection |
| Clock initialization and restore | 1,023 table entries, 8 saved-period cases | Exact table values and both restored auxiliary rates |
| Calibration and fast controls | 1 default case, 6 saved-load cases, 508 pitch cases, 128 smoothing cases | Exact checked RAM layout, pitch coordinates and smoothed controls |
| SAM reference/detector banks | 320 side-samples, varying nonzero input and cutoff boundaries | Bit-exact references, conditioning, quadrature states and magnitudes |
| Complete SAM callbacks | 12 callbacks / 768 samples, silent and nonzero input, all interaction modes, both halves | Exact controls, detector/phase state and sine/Sub words; spectral audio maximum absolute error 0.000000097673 |
| Complete Noise/Chaos callbacks | 12 callbacks / 768 samples, mixed engines, all interactions, LF on either side | Bit-exact controls, oscillator/filter/phase/RNG state and every output word |
| Cold Noise through mute release | 200 callbacks / 12,800 samples at zero Partials | Every output word checked, including the positive clipping plateau and recovery |
| Clock countdown sequences | 1,728 handler calls | Repeated edges, held/released Shift snapshots, short-release event and scan continuation |
| Clock offsets into planar readers | 258 reads across 64- and 65-frame Arrays | Exact source addresses; 32 cases leave the logical Array after admitted clocks |
| Array/LF and shifted CV buttons | 64 mode cases, 24 auxiliary cases | Mode cycle, Shift threshold, LF toggles, auxiliary cycle and checked RAM persistence fields |
| Concurrent Array/shared-button edges | 3,456 event combinations plus 3,456 held-input callbacks | A then B then shared dispatch, A-priority shifted CV target, combined mode/capture/LF effects and no redispatch while held |
| Simultaneous Shift releases with Array/shared edges | 3,200 combinations, 1,600 held callbacks and two persistent press/hold sequences | A-first selection priority, retained Shift counters at Array/shared dispatch, exact Sub/CV and interaction mirrors, capture start/stop with both Shifts low, and reachable B stop-length underflow |
| Consumed Shift gesture clearing/re-arm | 32 persistent sequences | Both consumed gestures remain latched while any panel input stays high; all-low clears counters and a new shared edge advances interaction rather than Sub/CV |
| Calibration/display stage lifecycle | 200 boot decisions, 704 stable callbacks, 176 releases, 12 post-save caller continuations | Boot-button priority, stage retention/cancellation/advance, eight pending saves and exact reset behavior; physical save excluded |
| Calibration averaging and stages 1/2 | 9 slow-bank and 576 fast-bank updates, 492 endpoint cases | Shared ring index, all twelve sums/histories, strict update versus inclusive readiness thresholds, float32 reciprocal gains including zero/negative spans |
| Pitch-table calibration stages 3..11 | 532 independent cases, 36 persistent measurement steps, 18 UI advances, 972 generated-table pitch reads | Acceptance windows, exact reciprocal slopes, unconditional tail extrapolation, retained final slope and normal pitch consumption; stop before final flash save |
| Capture through audio and playback reader | 8 complete callbacks, 9 captured spectra, 2 readers | Start/free/clocked/skip/stop sequence, exact saved spectra, lengths, peaks and playback deltas |
| Stable GPIO clock admission | 256 cases | Asymmetric A/B admission and sequential hold-counter effects |
| Concurrent clocks, Array edges and capture | 6,144 UI-to-capture-tail cases and two persistent gesture sequences | Clock-before-button ordering, opposite-side selection, threshold changes, first-write scheduling and unsigned stop-length underflow on both sides |
| Reader arithmetic after capture underflow | 200 complete readers with explicit sparse source memory, 12 joined address prefixes and two joined save dispatches | Exact signed/wrapping addresses and coefficient deltas; original recording slot is the next save target; hardware contents at escaped addresses remain unknown |
| Clock-policy timeout | 32 boundary cases | Strict >6000 threshold, recording exception and same-callback edge override |
| Opposite-Shift Array selection | 640 cases | Release timing, all 16 slots, wrap, scan-offset reset and persistence mirror |
| Long holds and factory Array replacement | 64 gesture cases, 32 direct resets | Linear toggle and reset dispatch; exact 8×8 factory spectra, descriptor and copy boundary |
| Settings record codec | 256 pack and 256 unpack cases | Exact eight-word format, including arbitrary-bit and signed-field behavior |
| Settings restore and save bookkeeping | 6 scans, 4 defaults, 12 save-tail cases | Sentinel/cursor, 256-byte copies, partial defaults and post-call updates; hardware writes excluded |
| Array save slot dispatch | 34 cases | Both banks, every first-unsaved slot, no-unsaved path, exact filenames and marker-before-open |
| Array save mutation and PCM16 readback | 28 frames on each path | Exact float32 gain/mutation, stored halfwords and signed read scaling; file calls excluded |
| Mode gesture to save dispatch | 8 cases with explicitly busy transport | Readiness threshold, mirrored mode, SAM-to-SAO Array-save request and Chaos-to-SAM settings-save request |
| WAV sample conversion/alignment | 240 cases across five formats | Exact float32 values, reported count, all four alignments and PCM24 partial-group quirk |
| WAV format-field construction | 15 cases | Format tags/depths, fixed 192000 byte rate and alignment 4, separate write requests; file calls excluded |
| Array load admission/dimensions | 24 admission and 72 assignment cases | Strict >64 admission, 4096-only 8×8 special case, A/B/shared descriptors and markers |
| Array load copy after conversion | 18 cases | Fixed 256-byte copy despite short/zero returned count, independent cursor/progress updates |
| Follow/Sync LED pattern | 96 complete LED-routine calls | Raw 0 high, raw 1 low, raw 2 toggles with counter bit 13, across display/selection overrides |
| Tuning beacon windows and retained pins | 748 complete LED-routine calls | Exact ratio windows and GPIO writes, boundary neighbors, octave scaling and all prior two-pin states |
| Linear Array reader | 432 cases on both sides | Exact source addresses and fused Focus/ramp deltas, including the one-third bypass and single-frame wrap |
| Complete SAO playback callbacks | 12 callbacks / 768 samples, 2 live Array selections | Both reader modes, changing controls, all interactions and LF; exact working banks/state and sine/Sub words, spectral audio within tolerance |
| Button-driven complete mode cycles | 96 callbacks / 6,144 samples, 48 Array rising edges | Actual SAM/SAO/Noise/Chaos transitions, held-input suppression, persistent state and all eight outputs; continued analyzer updates and retained SAO deltas across mode changes |
| Receive registration and DMA-to-audio dispatch | 48 registration prefixes and 48 dispatches | Installed half/full wrappers, buffer offsets 0/128, status-clear writes and unchanged floating-point registers through audio entry; explicit DMA snapshots, no hardware transfer claim |
| Successful software receive start | 128 starts and 256 subsequent dispatches | Original DMA-start routine configures stream/buffer/count, receive state, interrupt masks and callbacks; half/full status snapshots reach offsets 0/128 without replacing those configured registers |
| Slide movement during clock scanning | 216 cases | Frozen anchor, strict 0.005 threshold, offset cancellation and A/B clamp-order difference |
| WAV parser boundaries | 576 cases | Signature status, signed tag/channel handling, format classification, chunk padding/wrap and loop termination; file reads excluded |
| Loader count and initial seek | 864 cases across A/B/shared paths | Depth/channel divisor, payload length and four-byte alignment |
| Reader request and short-read window | 140 cases | Four-byte lookahead, alignment shifting, error return and overlong output after partial reads |
| Joined multichannel PCM16 load | 3 two-frame sequences | Channel-dependent count but scalar copying, descriptor assignment and loop termination; explicit I/O boundaries |
| Initial slot descriptors | 32 descriptors | Both banks, offsets, 8-by-8 dimensions and initial marker 1 |
| Planar playback with retained slot tails | 192 cases | Exact deltas for two tail histories, both sides, empty/short/non-square/full lengths and fractional controls |
| Capture shrink with retained scan offset | 48 sequences | Start/write/stop preserves offset; subsequent reader addresses and deltas match contiguous storage, including cross-slot reads |
| Capture-to-SAO with unavailable persistence | 48 continuations and 48 manual takeovers | Actual mode gesture, save mutation, failure returns, slow controls and reader deltas; unchanged Slide preserves offset, moved Slide clears it |
| Pending-operation recovery | 256 callback prefixes plus complete SAO sequence | Byte increment/wrap, exact transport request bytes, clear-on-busy behavior and continued DSP |
| Linear playback with empty/retained physical storage | 960 cases | Exact addresses and deltas; empty negative-fraction extrapolation and 16 preceding-frame reads, both sides and slots 0/15 |
| Complete initializer with busy transport/DMA | 4 sequences | Audio-enable clear/set order, external command bytes, delay requests, DMA arguments and retained failed-read bytes |
| Disabled-audio callbacks | 48 callbacks | UI still runs, DSP/sample counter and output buffers hold; 48 eligible captures copy stale analyzer spectra |
| Cold Noise from complete initializer | 200 callbacks / 12,800 samples | Original enable/counter retained after busy drivers; exact DSP/output comparisons, 3811 unmuted NaN-before-clip frames |
| Complete WAV parsing with explicit read/seek fixture | 405 files, 14 edge files and 30 I/O errors | Full original wrapper/parser, chunk ordering, duplicate/missing/truncated chunks, exact metadata and error propagation |
| Parser-to-Array loading with explicit read/seek fixture | 120 sequences | Five formats, 1/2/3 channels, aligned/misaligned payloads, both sides and slots 0/15; exact imported coefficients and dimensions |
| Array save/reload with explicit file fixture | 96 files and 96 reloads | Exact finalized headers/payloads, in-place normalization, PCM wrapping, short-Array rejection and restored dimensions |
| Save payload faults and multiple dirty slots | 12 failed writes, 24 short writes, 2 multi-slot batches | Failed frames are omitted, successful short counts are ignored, markers persist, and dirty slots save in order |
| Display priority and mode/auxiliary indicators | 1020 priority cases and 2592 flag/counter cases | Exact four-channel PWM values, override flag and A/B GPIO writes through the complete indicator routine |

The run covers **9,577 distinct instruction addresses**, not 9,577 functions or
complete firmware coverage. Machine-readable counts and exact executed addresses
are in `analysis/continuation_tests.json`. The original 31 host tests also pass
on native Windows after adding the UCRT `fmaf` loader.

Unicorn 2.1.4's Cortex-M7 profile runs the single-precision slices. That profile
rejects the image's double-precision instructions, so standard synthesis uses
Unicorn's `UC_CPU_ARM_MAX` Thumb profile. This checks arithmetic/instruction
behavior; it does **not** establish the board's MCU identity. All slices use
flush-to-zero and default rounding. The comparisons cover finite, ordinary
float32 inputs, branch boundaries, bounded trajectories, two isolated cold
Noise quiet-NaN cases and the documented cold recovery sequence; they do not prove
all NaN/subnormal/exception cases, long chaotic trajectories, physical sound
equivalence, interrupts, or boot behavior. No vendor calls are silently stubbed.

## 2. Contract for a first Rack DSP core

Keep a fixed **48 kHz internal engine with 64-frame callbacks** for the first
compatibility implementation. Rack's buffer size must not change analysis,
control, coefficient-ramp, or capture cadence. Resample explicitly at the Rack
boundary; the choice of resampler is a new implementation decision.

Each side needs independent SAM detector banks, 64 working SAO coefficients and
their deltas, 64 SAM amplitudes, synthesis odd/even phases, a clean sine phase,
an analyzer phase, a Sub/CV phase, four Chaos oscillator records, two Noise
filter records, a Noise interpolation record, envelope state, and Array state.
Keep **two shared RNG streams**: one for Noise, another for Sub/CV random values.
Do not merge them or assign a separate Noise RNG to each side.

Per callback, retain the recovered control/reader setup before the sample loop.
Per sample, the important ordering is:

1. Read inputs and update the recovered control smoothing/pitch paths.
2. Evaluate both clean sine FM sources from their **old** phases.
3. Evaluate A Sub/CV and analyzer references; process B and then A detectors.
4. Synthesize A (standard/Noise/Chaos), clip A; synthesize B.
5. Apply the B interaction envelope when selected, then evaluate B Sub/CV.
6. Advance phases, including Sub/CV RNG crossings in **B then A** order.
7. Apply auxiliary offsets, startup mute, clipping/conversion and lane stores.
8. After the 64-frame loop, perform eligible capture writes and bookkeeping.

This is an integration outline, not a claim that the whole callback has been
emulated in all modes. The joined test runs the post-analysis path from `0x0802f9bc` through
all eight output stores at `0x0803038c`, preserving states across 900 samples.
Earlier analyzer/control work and UI transitions are outside that entry.
Two tests execute twelve complete calls to `0x0802e750` through return:
64 samples each, silent input or two distinct sinusoidal inputs, nonzero internal
FM, all three interactions, alternating DMA halves, varying per-sample fast ADC
codes, and persistent state. They independently check controls, exact detector
states/magnitudes, spectral/clean/Sub/analyzer phases, Sub RNG and every clean
sine/Sub word. All four spectral audio lanes are checked against the mathematical
synthesis model with a 0.0002 absolute tolerance; observed worst error is
`9.767245501279831e-08` in normalized digital output units. This covers these
bounded trajectories, not arbitrary input levels, frequency ratios or durations.
Setup uses the original initialization prefix through `0x0802c9ba`, including
its real memory-clear loops but stopping before peripheral setup, with explicit
calibration/ADC RAM and mapped register snapshots. It sets the ready counter
`0x20002eec=256`, audio-enabled byte `0x20002f66=1` and disables startup mute.
It does not emulate boot, interrupts, electrical GPIO or background flash writes.

A third complete-callback test checks 12 consecutive Noise/Chaos callbacks,
including both matching and mixed engine pairs, all three interactions, LF on
either side, varying fast ADC codes and signed audio words controlling even
tuning. All eight output words and the persistent controls, engine state,
phases and both RNGs match exactly. Its warm Noise state is explicit. Both
SAO flags are set; valid 8-by-8 synthetic Arrays are supplied because the
callback still runs the selected Array readers before Noise/Chaos dispatch.
These engines do not consume the resulting coefficient ramps. Engine/LF
selection uses RAM fixtures, not complete selection gestures.

A separate cold case retains the initializer's Noise state and mute flag,
and checks 200 complete callbacks through recovery; see section 6. The original
case supplies the ready counter and audio-enable byte. A second case obtains
both from the complete initializer with explicit busy transport/DMA and a
synthetic tick source. Neither emulates a complete board boot.
Keep physical ADC/panel conversion behind an adapter. The standard recurrence and SAM
equations remain in `DSP_SPEC.md` and `sp67_reference.py`.

### Complete button-driven mode cycles

`test_full_button_mode_cycles_preserve_dsp_state` joins the real UI handler,
mode comparison, persistence/recovery and audio processing for 32 consecutive
callbacks in each interaction mode 0/1/2: **96 callbacks, 6,144 samples and 48
Array rising edges**. Each side completes two SAM → SAO → Noise → Chaos → SAM
cycles. A and B presses are staggered or simultaneous, and selected presses
remain high for another callback to check no retrigger. Both DMA halves and
changing slow controls are exercised. The test changes GPIO snapshots, not
mode/engine RAM, after initialization.

The initial state uses original DSP initialization, explicit saved synthetic
8-by-8 Arrays at initialized A/B offsets, periodic Sub/CV mode 5, inactive LF
and capture, warm finite Noise state, sample counter 256 and disabled startup
mute. The original peripheral and flash-program calls return busy via their
explicit handle state; no file function is substituted, and no successful
hardware persistence is claimed. All Array markers start nonzero, so mode-entry
save scans reach settings saving without serializing Array payloads.

Execution stops before slow-control processing to compare unchanged detector
histories, spectral banks, Noise/Chaos records and phases across the actual
UI/persistence prefix, then resumes the same callback through return. Independent
per-sample equations check detector states/magnitudes, working coefficients,
Noise/Chaos state, control smoothing, phases and both RNGs exactly. All eight
digital outputs are checked: clean/Sub and Noise/Chaos words exactly, standard
spectral output against the mathematical model within 0.0002. The measured
maximum is `2.7567148208618164e-05`, recorded as
`mode_cycle_standard_max_absolute_error` in the report.

Three continuity rules are material to a port:

* **Analyzer rates persist outside SAM.** SAM control setup writes A/B rates at
  `0x200023ec/0x200023f0`. SAO, Noise and Chaos skip that setup but continue
  advancing analyzer phases with the stored rates. Changing Slide in those
  modes does not update the analyzer rate; returning to SAM recomputes it.
* **Detector histories continue updating in oscillator modes.** The reference
  and detector code still runs. SAM setup computes poles in `s22` (A) and
  `s23` (B); oscillator setup skips those assignments. In these callbacks the
  detector instead consumes those registers' entry values. The test explicitly
  varies A entry poles over 0.125/0.25/0.375 and B over 0.5/0.625/0.75, and
  matches all detector histories and magnitudes. The callback saves/restores
  d8..d15, including these two registers, so they are not automatically the
  previous callback's computed SAM poles. The complete interrupted caller/FPU
  context is not established by this fixture. A Rack model must make this
  dependency explicit; assuming frozen detectors or silently reusing the last
  SAM pole does not reproduce the checked instruction path.
* **Standard synthesis advances the SAO working bank even in SAM.** The A loop
  at `0x08030e42` and its B counterpart add retained reader deltas to active
  working coefficients before loading the mode-selected synthesis source.
  SAM uses the analyzer magnitude bank for its sound, but those unused working
  coefficients keep changing. SAM does not recompute the reader deltas, so the
  last oscillator-mode delta persists. Noise/Chaos do not perform this standard
  working-bank update, although oscillator control setup still computes their
  reader deltas. The next SAO entry interpolates from the resulting retained
  working bank; do not reset it on mode selection.

These rules refine the integration outline above. The checked cycles do not
include capture, Shift/long-hold gestures, linear readers, successful peripheral
I/O or a physical device's entry pole-register values.

### Receive callback registration and incoming floating-point context

`test_receive_registration_dma_dispatch_preserves_fp_context` executes receive
setup at `0x08025ce4` through the pending DMA-start call at `0x08021cb0`.
The ready/unlocked receive-handle fixture at `0x20014918` supplies its peripheral
base and DMA link. For buffer `0x30000440`, count 256, the original setup installs:

| DMA-handle offset | Callback / value |
|---|---|
| +60 | `0x08025e2d` (Thumb wrapper at `0x08025e2c`) |
| +64 | `0x08025e89` (Thumb wrapper at `0x08025e88`) |
| +76 | `0x08025e95` (error wrapper; not executed by these checks) |
| +80 | zero |

The pending DMA-start arguments are `(dmaHandle, peripheralBase+28,
0x30000440, 256)`. Stopping there verifies registration without substituting a
successful DMA start. The next stage explicitly supplies circular DMA1 stream
register/status snapshots: streams 0..7, half/full status masks 16/32 shifted
by 0/6/16/22 within the appropriate status word, and enabled control bits.

The original common handler at `0x080223a8` reaches the registered wrapper,
which calls `0x08032680` for half status and `0x08032678` for full status.
Those leaf wrappers pass callback word offsets **0 and 128**, respectively,
to `0x0802e750`. The expected status-clear write is also checked. Each stream
and event runs with three distinguishable patterns across all 32 scalar
floating-point registers, giving 48 registration prefixes and 48 dispatches.
Every register reaches audio entry unchanged, including s22/s23. The dispatch
test stops before the audio prologue; the prior mode-cycle test covers their
subsequent use and preservation by the complete callback.

This closes the possible explanation that these tested wrappers initialize the
oscillator-mode analyzer poles. They do not. It does not determine values
inherited from reset/bootloader, other interrupted execution or actual exception
entry. The application's normal main path calls the initializer at
`0x08034798` and reaches the self-loop at `0x080347a2`; that idle loop does not
assign floating-point registers. This is static path evidence, not an executed
whole-board startup. DMA handles, request timing, interrupt nesting and hardware
floating-point context behavior remain outside the register-snapshot fixture.

### Successful software receive start

`test_receive_start_success_and_audio_dispatch` continues the original receive
routine through `0x08021cb0` and back to its caller. Its 128 cases cover DMA1
streams 0..7, receive configuration word +4 values 0..3, protocol word +68
values 0/8, and peripheral enable bit 16 initially clear/set. The supplied DMA
handle is ready/unlocked, circular, and linked to the receive handle; request-mux
objects are explicit mapped register fixtures with the optional generator absent.
No callee return is substituted.

For buffer `0x30000440` and count 256, successful return is zero and establishes:

| State/register | Observed result |
|---|---|
| Receive handle +120 | Buffer address |
| Receive handle +124/+126 | Both 16-bit counts are 256 |
| Receive handle +144/+145 | Lock 0, state 34 |
| DMA handle +52/+53 | Lock 1, state 2 |
| Both error words | Zero |
| DMA stream +4/+8/+12 | Count 256, source peripheral+28, destination buffer |
| DMA stream control | Initial `0x40100` becomes `0x11f` |
| DMA status clear | `63 << streamShift` at status-base+8 |
| Request-mux clear | Supplied mask copied to the clear register |
| Receive peripheral control | Bits 16 and 17 set; unrelated tested bit 7 retained |

The receive interrupt mask is ORed into the existing peripheral word at +16.
For configuration 2/3 the base mask is 97; for 0/1 it is 5. Protocol 8 adds
bit 4 only when configuration is 1/3, producing 21/113 respectively. The same
four callback slots listed above are installed. All 32 scalar floating-point
registers remain unchanged across successful start.

Each resulting configured stream then receives explicit half and full interrupt
status snapshots. The original handler reaches audio entry with offsets 0 and
128 in all 256 dispatches, still preserving the floating-point registers.
Only status and CPU entry state are supplied for these dispatches; the test does
not replace the startup-configured stream control or callback pointers. This
closes the successful software receive-start path for the stated configurations.
It does not simulate physical transfers, write-one-to-clear hardware behavior,
exception entry, other DMA directions, request generators, or error interrupts.
For Rack, the useful contract remains the alternating half-buffer callback
offsets; MCU handle locks and register writes belong to the transport adapter.

### Initialization and the audio-enable gate

The complete initializer at `0x0802c4e0` clears the input/output DMA buffers,
sets sample counter `0x20002eec` and audio-enable byte `0x20002f66` to zero,
then calls external-device configuration at `0x080327a8`. That routine requests
delays of 2 and 10 ticks and sets audio-enable to 1 **before** the initializer
requests any DMA transfer. The delay routine adds the byte at `0x20000000` to
each finite request; its elapsed comparison uses unsigned tick subtraction.
The tests supply that byte as 1 and advance tick RAM at `0x2000204c` on every
original tick-reader entry. Starting ticks 0/0xfffffffa and steps 1/10 cover
ordinary progress and wraparound, without replacing delay or driver functions.
They establish ordering, not wall-clock timing or interrupt behavior.

The configuration routine sends each following register/value pair as three
bytes `9e, register, value`, then sends `9e, register` and exchanges
`9f, register` for a two-byte response:

`03:00, 04:40, 05:46, 06:22, 08:read-only, 0d:28, 1e:80,
delay(2), 02:00, 1c:02, 1d:02, delay(10), audio-enable=1`.

All numbers in the register/value list are hexadecimal. The read-only entry
omits the three-byte write but retains the send/exchange. The driver handle is
`0x20014bb8`; these bytes alone do not identify the external device. With that
handle explicitly locked, every original send/exchange returns busy, yet the
routine still enables audio and copies the unchanged receive byte into its
status mirror. Four unready transmit handles and one unready receive handle
also return busy; audio-enable remains 1 and the counter remains 0. Consequently,
audio-enable indicates a software branch decision, **not confirmed peripheral
readiness**. Successful transport and DMA activation are not established here.

The subsequent transfer requests use a count of 256 and these buffer/handle
pairs, in order:

| Direction | Buffer | Handle |
|---|---|---|
| Transmit | `0x30001040` | `0x200149b0` |
| Transmit | `0x30000c40` | `0x20014880` |
| Transmit | `0x30000840` | `0x200147e8` |
| Transmit | `0x38000000` | `0x20014750` |
| Receive | `0x30000440` | `0x20014918` |

In the callback, UI handling, pending-operation recovery, display updates,
mode comparison and the two button countdown decrements precede the enable
check. If audio-enable is zero, slow controls, the entire sample loop and sample
counter advancement are skipped. Existing output buffers are **left unchanged**,
not overwritten with zeros. Capture processing still follows: an eligible
active capture can write the previous analyzer spectrum again even though no
new analysis occurred. Forty-eight full callbacks cover both DMA halves,
counters 0/128/129, every A/B active-capture combination, and two consecutive
calls per fixture. They check frozen DSP state/output buffers and all 48 stale
spectrum writes. Clock-gated capture and arbitrary simultaneous UI gestures are
outside these disabled-callback fixtures.

## 3. Register and control contracts

The slow-control instruction test establishes slot routing and normalized
values. Slide/Focus naming is cross-checked against the manufacturer's cheat
sheet: Chaos Slide controls feedback and Focus controls ratio; Noise Slide
controls the first low-pass stage and Focus the following high-pass stage.
The earlier continuation incorrectly named the two Noise filter arguments in
reverse. Their equations were unchanged; `noise_step()` now uses `slide, focus`.

| Meaning at slice entry | A | B |
|---|---|---|
| Chaos/Noise Partials | `s1`, from `0x20000880` | `s2`, from `0x200023fc` |
| Slide: Chaos feedback / Noise low-pass rate | `s4`, pointer at `sp+132` | `s12`, pointer at `sp+136` |
| Focus: Chaos ratio / Noise high-pass fraction | `s6`, `0x20002e58` | `s14`, `0x20002e54` |
| Odd rate | `s5` | `s15` |
| Even rate | `s3` | `s19` |
| Internal FM source | float at `sp+28` | `s25` |
| FM index | `0x20001140` | `0x20002e50` |
| Odd phase | `0x2000241c` / `s18` | `0x20002424` |
| Even phase | `0x20000900` / `s16` | `0x20002420` |

The six uint16 slow ADC slots at `0x30000020` route as follows:

| Slot | Role | Destination |
|---:|---|---|
| 0 | A Slide | `0x20002e5c`, pointed to by `sp+132` |
| 1 | A Focus target | `0x20002e60`, smoothed to `0x20002e58` |
| 2 | B Focus target | `0x20002e68`, smoothed to `0x20002e54` |
| 3 | B Slide | `0x20002e64`, pointed to by `sp+136` |
| 4 | A FM | `0x20001140` |
| 5 | B FM | `0x20002e50` |

Each normalized slot is `float32(raw-offset[j])*gain[j]`; offsets begin at
`0x20001380`, gains at `0x20001200`. The first four clamp to [0,1]. FM uses
`60*u^4`, with separately rounded squarings and no clamp in the checked path.
Per-sample Focus smoothing at `0x0802edb6–0x0802ee10` is
`FMA(target-old, float32(0.01), old)`. This is independently checked in 128
fast-control cases and complete callbacks. Clock-offset cancellation is checked
separately in 216 cases; see the Slide handoff rule below.

### Fast controls and calibration

Four uint16 codes per frame start at `0x30000040`. For callback word offset h
and sample n, the frame address is `0x30000040 + 4*h + 8*n`.

| Fast slot | Role | Offset/gain index |
|---:|---|---:|
| 0 | A Partials | 9 |
| 1 | A pitch | 8 (offset only; separate pitch slopes) |
| 2 | B Partials | 11 |
| 3 | B pitch | 10 (offset only; separate pitch slopes) |

Partials target is `max(0, float32((raw-offset)*gain))`, with **no upper clamp**
in this path, followed by the same 1% per-sample FMA smoothing as Focus.
`partials_control()` and `smoothed_control()` preserve this schedule.

`default_calibration()` specifies the fallback at `0x0802c684–0x0802c8c2`:
all 12 offsets are 2000; gains have bits `0x37840803`; each side's first eight
pitch slopes have bits `0x3e808312` (approximately 0.251). Each side's first
eight integer breakpoints are `[40,8120,16200,24280,32360,40440,48520,56600]`.
The remaining eight entries in each pitch table are not written by this fallback.

For pitch, set `x=raw-offset`. Choose region j=0..7 by comparing x against
breakpoints 1..7: equality belongs to the higher region. Region 0 computes
`float32((x-breakpoint[0])*slope[0])`. Regions 1..7 compute
`FMA(float32(x-breakpoint[j]), slope[min(j,5)], 2048*j)`.
Truncate the result to an integer and clamp negative coordinates to zero.
There is no explicit high clamp. The use of slope 5 for regions **5, 6 and 7**
is confirmed on both sides with deliberately distinct slopes. Do not replace
it with slope[j] in an equivalence implementation. The tests cover both sides,
every breakpoint at -1/0/+1, ADC endpoints and randomized ordinary inputs.

A breakpoints/slopes are at `0x20001300/0x20001280`; B at
`0x200012c0/0x20001240`. `pitch_coordinate()` returns the coordinate consumed
by the exp2 laws in section 4. It requires monotonic, sane calibration and
uint16 input; corrupt saved calibration is not part of its comparison contract.

The saved calibration block starts at **`0x080e0000`, outside this firmware
image**. Its module-specific bytes are unavailable. Synthetic source words
verify this loader layout, with offsets relative to that flash base:

| Flash offset / count | Destination |
|---|---|
| 32 / 12 words | offsets `0x20001380` |
| 96 / 12 words | gains `0x20001200` |
| 160 / 8, 288 / 8 words | A breakpoints, first and second halves |
| 192 / 8, 320 / 8 words | B breakpoints, first and second halves |
| 224 / 8, 352 / 8 words | A slopes, first and second halves |
| 256 / 8, 384 / 8 words | B slopes, first and second halves |

Selection at `0x0802c66c` checks header word 1237. With that header, the full
saved block loads when signed mode `0x200144d4<=0` or readiness field
`0x200144d0!=0`. Otherwise fallback applies. Mode 3 then replaces only offsets
and gains from flash even without the matching header. Six cases check these
branches; they do not identify every calibration-menu gesture or validate bad
flash data. Keep these raw fields internal. Rack can use the exact fallback
for a digital reference; measured volts-to-ADC conversion and analog summing
of coarse/fine/CV remain outside the available evidence.

All slice entry register values and referenced stack/RAM fields are concretely
initialized by the tests. Other registers are not reusable public interfaces.

### Calibration averaging and endpoint equations

Positive calibration stages maintain twelve 64-entry integer histories at
`0x20001400`, twelve running sums at `0x200013c0`, and a **shared** index at
`0x20002f00`. Each six-channel update reads the old entry at that index,
replaces it with the new sample, sets `sum = sum-old+new`, and advances the
index as `(index+1)&63`. The first six histories update on the callback's
slow-control path; the other six update on its per-sample path. They do not
have separate indices. A schedule of one slow update followed by 64 fast
updates therefore advances the shared index by one modulo 64 overall.

| Index | Calibration sample |
|---:|---|
| 0 | A Slide slow ADC |
| 1 | A Focus slow ADC |
| 2 | B Focus slow ADC |
| 3 | B Slide slow ADC |
| 4 | A FM slow ADC |
| 5 | B FM slow ADC |
| 6 | `-(signedAudioWordA >> 15)` |
| 7 | `-(signedAudioWordB >> 15)` |
| 8 | A pitch fast ADC |
| 9 | A Partials fast ADC |
| 10 | B pitch fast ADC |
| 11 | B Partials fast ADC |

The fast DMA slot order itself is A Partials, A pitch, B Partials, B pitch;
the table above is the calibration-history order. Audio words use an
arithmetic right shift before negation. These are digital sample definitions,
not volts or instructions for physically applying a calibration signal.

`calibration_ring_step()` defines each bank update. Tests join original slow
slice `0x0802eb68..0x0802ec88` and fast slice `0x0802f0a4..0x0802f154` with
persistent RAM, seeded histories and initial indices 0/1/63. Three simulated
blocks per initial index check all twelve sums, every history word and the
shared index after every update: nine slow and 576 fast updates. This schedule
test does not execute the surrounding complete calibration audio callback.

Stages 1 and 2 maintain `high[12]` at `0x20001340`, `offset[12]` at
`0x20001380`, and float32 `gain[12]` at `0x20001200`. Let `u` be the current
sample with the calibration polarity above, and `mean = sum >> 6` using
arithmetic shift. For each channel independently:

```text
stage 1: if u > 60000: high = mean
stage 2: if u < 8000:
             offset = mean
             gain = float32(1 / float32(high - offset))
```

Otherwise that channel's existing fields remain unchanged. The current
sample selects whether to update; the running average supplies the stored
value. They are not interchangeable. The fast path updates its sums before
performing this decision. In raw audio-word coordinates, stage 1's test is
`(word >> 15) < -60000`; stage 2's is `(word >> 15) > -8000`.

Readiness differs at equality: stage 1 requires all six current samples in
the bank to be **>=60000**; stage 2 requires all six to be **<=8000**. Thus
exactly 60000/8000 can satisfy readiness without refreshing a stored value.
The slow path first writes both auxiliary pins low, then writes both high if
its bank is not ready. The fast path branches to the common GPIO-high writer
when its bank is not ready; when ready it makes no corresponding low write.
Retain the ordered pin writes rather than replacing them with an independent
per-bank LED boolean. The fast endpoint tests stop at those branch targets.

`calibration_endpoints()` defines these updates and readiness. The 492 cases
cover both stages, both banks, every channel, current values around both
thresholds, independently supplied means, preserved fields and float32 gain
bits. Sixty cases supply endpoint spans -16777217/-1/0/1/16777217: conversion
to float32 precedes division, negative spans yield negative gains, and zero
spans yield positive infinity. No guard or saturation is inserted by the
literal definition. These deliberately supplied states do not establish that
a normal physical calibration sequence reaches every tested span. Pitch-table
calibration follows below; successful flash persistence remains separate work.

### Pitch-table calibration stages 3 through 11

`calibration_pitch_tables()` specifies the original path from `0x0802f15e`
through its return to normal processing at `0x0802f1aa`. It consumes the
averaged pitch channels, not instantaneous ADC values: for A/B respectively,
`u = (sum[8 or 10] >> 6) - offset[8 or 10]`. Sums and offsets use the addresses
above. Each side has 16 signed integer breakpoints P and 16 float32 slopes S,
at the addresses listed in the pitch-coordinate contract.

Stage 3 accepts **26 <= u <= 1999**. On acceptance it sets only `P[0]=u`;
on rejection it leaves the entire table unchanged. Slopes are not modified in
this stage, and there is no tail extrapolation yet.

For stages 4..11, let `j=stage-3` (1..8). Accept only when
**8100*j - 1500 < u < 8100*j + 1500**; both endpoints are excluded. Accepted
measurements set:

```text
P[j] = u
S[j-1] = float32(2048 / float32(P[j] - P[j-1]))
```

Rejected measurements retain both of those entries. After that decision,
**whether accepted or rejected**, the firmware extrapolates the remaining
table independently for each side:

```text
step = P[j] - P[j-1]
for k = j+1 .. 15:
    P[k] = P[k-1] + step
    S[k-1] = S[j-1]
```

Consequently, rejection does not mean the whole table is untouched: stale
entries at j/j-1 still determine the rewritten tail. Earlier breakpoints and
slopes remain intact, and `S[15]` is never written by these stage paths. A zero
measured interval produces positive infinity; a negative interval produces a
negative slope. The reference covers ordinary signed arithmetic without
int32 overflow, including float32 rounding before reciprocal division.

Both auxiliary GPIO pins are first written high; each accepted side then
writes its own pin low (A mask 64, B mask 128). The stage does not advance
automatically when both sides are accepted. Stage advancement remains the
Shift-release procedure defined in section 4.

The 532 independent cases cover stage 3's inclusive boundaries, every later
stage's strict lower/upper limits and nearby values, independent A/B outcomes,
all 16 words in each table, and adjacent sentinels. Forty cases deliberately
supply previous breakpoints giving spans -16777217/-1/0/1/16777217. These are
arithmetic tests, not a claim about physically reachable calibration states.

Two persistent sequences start with fallback first-half tables and explicit
second-half contents. Each stage receives a rejected-side trial and then
accepted measurements (36 steps total). Original GPIO handling performs a B
Shift press, ten held callbacks and release for every advance (18 total).
The final stage-11 release reaches stage 12 and the original calibration-save
entry; execution stops before flash I/O. After each accepted measurement,
972 total original A/B pitch-coordinate reads consume the generated RAM tables
at ADC endpoints and breakpoint boundaries. No table reload is substituted
between calibration and consumption.

The normal pitch-coordinate path still uses only P[0..7] and S[0..5], reusing
S[5] for regions 5, 6 and 7. Do not replace that independently checked behavior
with S[region] just because calibration populates more entries. In particular,
stage 11 updates P[8] onward and S[7] onward, outside those normal-path reads.
These checks establish digital calculations and their consumption; actual
voltage targets, device calibration bytes, full calibration audio trajectories
and successful persistence remain unverified.

### Instruction-scheduled SAM analysis

`analysis_parameters_exact()` replaces the initial explanatory mapping when
checking bits. Set `q=trunc(float32(Slide*8191))`, then
`inc=float32(2^(q>>11) * float32(exp2Table[q&2047]*floatFromBits(0x39da740e)))`.
Width is `FMA32(-Focus,floatFromBits(0x3f7ae148),1)`. Compute width² and its
product with -3.14 as two float64 multiplications, then
`pole=min(float32(FMA64(inc,curvature,1)),floatFromBits(0x3f7ffeb0))`.
The host reference calls native `fma` as well as `fmaf` to retain this rounding.
Pole uses the callback's normalized Focus target; the per-sample smoothed Focus
field is a separate control used by other engines.

`analysis_references()` starts with interpolated sine/cosine values at the old
analysis phase. For each sequence use the two initial terms
`c1=cos(theta)`, `c2=FMA(c1,2*c1,-1)`, `s1=sin(theta)`,
`s2=float32(s1*(2*c1))`, then `term[n]=FMA(2*c1,term[n-1],-term[n-2])`.
All products, sums and FMA operands use the instruction-scheduled float32 code.
The firmware computes 64 references even when fewer detector terms are active.

`sam_detector_step()` checks both complete banks at `0x0802f29c–0x0802f9bc`,
including input conditioning. With u=float32(12*input),
`conditioned=float32(u+FMA(previousConditioned,float32(.995),-previousU))`.
For each sine or cosine reference r and stored two-stage state z1/z2:

```text
temporary = FMA(r, -conditioned, z1)
nextZ1 = FMA(conditioned, r, float32(pole*temporary))
nextZ2 = FMA(pole, float32(z2-nextZ1), nextZ1)
energy = FMA(cosineZ2, cosineZ2, float32(sineZ2*sineZ2))
```

Magnitude applies the literal unsigned bit square root from `DSP_SPEC.md`.
The supported normal-input tests preserve exact states and magnitude bits.
Active term count is `active_terms(inc)`: zero at inc>=float32(.1125), otherwise
groups of four admitted below float32(.45), capped at 60. **Inactive states and
magnitudes retain their previous values**; do not silently zero them.

| Bank data | A | B |
|---|---|---|
| Cosine references, 64 floats | `0x20002640` | `0x20002940` |
| Sine references, 64 floats | `0x20002540` | `0x20002840` |
| Cosine states, 64 pairs | `0x20000220` | `0x20000620` |
| Sine states, 64 pairs | `0x20000020` | `0x20000420` |
| Magnitudes, 64 floats | `0x20002b40` | `0x20002d40` |
| Previous u / conditioned | `0x20002e74 / 0x20002e70` | `0x20002e6c / 0x20001160` |

## 4. Normal phases, clean sine outputs and cross-FM

Evidence: `0x0802f1da–0x0802f27c`, `0x0802ffb6–0x080301ce`, with alternate Sub/CV
joins at `0x080305f2` and `0x0803061c`. Code: `phase_step()`;
`normal_phase_step()` is its mode-0 wrapper.

Let `L(p)` be the 8192-entry sine lookup with float32 linear interpolation.
At the beginning of the sample:

```text
FM source into A = L(B clean sine phase)
FM source into B = L(A clean sine phase)
A clean sine phase += A odd increment
B clean sine phase += B odd increment
```

The clean sine phases do not receive the internal FM increment. For ordinary
phase advance, in approximate algebra (the code preserves actual rounding):

```text
dA = AoddInc * Aindex * oldBcleanSine
AoddNext  = AoddPhase  + AoddInc  + dA
AevenNext = AevenPhase + AevenInc + dA

dB = BoddInc * Bindex * oldAcleanSine
BoddNext  = BoddPhase  + BoddInc  + dB
BevenNext = BevenPhase + BevenInc + dB
```

The even phase receives the **odd-rate FM increment** in this path. Do not
substitute its own rate. When a side's two increments compare exactly equal,
the even next phase additionally moves toward the odd next phase by
`float32(0.0005) * (oddNext-evenNext)`, before wrapping.

Wrap is `x-trunc(x)`, **not** `x-floor(x)`. Negative phase values remain
possible; LUT indices wrap with a bit mask and fractional interpolation is
also truncation-based. Preserve this behavior in an equivalence core.

Chaos has its own oscillator phases and per-channel rate modulation before
this common phase tail; its modulation schedule must not be replaced with
the standard-phase formula.

### Interaction modes: joined digital behavior

Use raw selector values from byte `0x20002f54`: **0 = independent/off,
1 = Follow, 2 = Sync**. The mapping is now cross-checked by the complete LED
routine and the manufacturer's indicator descriptions, as detailed below. The
complete button handler verifies the unshifted press cycle **0 → 1 → 2 → 0**.
It mirrors the value to persisted configuration at `0x200134bc` and marks the
configuration dirty at `0x20002eb8`. These are RAM changes, not a checked flash save.

* Mode 0: independent pitch; ordinary phase update; synthesis ratios 1.
* Mode 1: linked B pitch as below; ordinary phase update. Standard/Noise
  dispatch overwrites stored synthesis ratios with 1.
* Mode 2: linked B pitch; stored odd/even ratios are `BoddInc/AoddInc` and
  `BevenInc/AevenInc`. B standard/Noise synthesis scales its phase coordinates
  by these ratios. Entry `0x080308a6` substitutes A's odd/even increments for
  B's spectral phase advance, including the odd-rate FM multiplier. B's clean
  sine and Sub/CV retain B's true rates. The equality test that admits the
  even-phase pull still compares **B's** odd/even increments.

Mode 2 additionally multiplies B's odd/even output by envelopes derived from
A's respective **old** spectral phases. Define `triangle=2*p` when `p<=0.5`,
otherwise `2-2*p`; gain is 1 when `triangle>float32(0.05)`, otherwise
`20*triangle`. This also applies to Chaos output, although Chaos does not use
the standard/Noise phase-ratio multipliers. Negative phase is not clamped;
negative gain is possible. Preserve that for comparison tests.

The 900-sample joined test changes all three engines, interactions and auxiliary
modes while carrying state forward. It checks ratios, engine state, phase tail,
RNG streams and stores together. This is hard-sync-like phase-coordinate
behavior, not an observed explicit reset-edge operation in this path. It does
not prove all shifted gestures or all combinations of frequency ratios.
The ratio helper currently requires positive, nonzero denominator increments.

### Indicator mapping and tuning beacon

The manufacturer identifies steady illumination as Follow and flashing as
Sync; off means neither. Its frequency section describes Follow as B tracking
A with a pitch offset, and Sync as retaining that relationship while synchronizing
the spectral outputs. These descriptions match the recovered raw-state DSP
and GPIO patterns. Sources: [Buttons and Display](https://www.makenoise-manuals.com/spectraphon/spectraphon-manual-buttons.html)
and [Frequency and Partials](https://makenoise-manuals.com/spectraphon/spectraphon-manual-frequency.html),
accessed 2026-10-04 through the manufacturer's linked manual site.

Ninety-six tests execute `0x0802d464` through return. At GPIO base
`0x58020400`, pin mask `0x8000` is driven high for raw 0, low for raw 1,
and low iff bit 13 of `0x20002eec` is set for raw 2. Eight counter values
cover both edges and uint32 endpoint; normal display, startup-display override,
and either Array-selection display are included. Combined with the documented
off/steady/flashing semantics, this identifies an active-low indicator and
the enum mapping above. The counter advances per audio sample, so bit 13
gives nominal 8192-sample half-cycles, or a 0.34133-second full cycle at 48 kHz.
Physical brightness, wiring and optical timing have not been measured.

`indicator_register_values()` defines the rest of this routine's display
selection and mode/auxiliary pin outputs. These are register values, not inferred
LED colors or measured brightness. Its contract covers override/slot values
0..15, engine values 0..2, uint32 sample counters and the documented A/B flags.
The 1020 priority cases and 2592 mode/auxiliary cases execute the complete
original routine through return and check its exact PWM words, override flag
and ordered GPIO writes.

The four PWM compare words are 0 when the corresponding pattern bit is set,
otherwise 127:

| Pattern bit | Register | Normal-engine source |
|---|---|---|
| 3 | `0x40001834` | A engine bit 1 |
| 2 | `0x4000083c` | A engine bit 0 |
| 1 | `0x40000040` | B engine bit 1 |
| 0 | `0x4000003c` | B engine bit 0 |

Pattern selection is ordered:

1. Positive `0x200144d4`: use its low four bits while sample-counter bit 15 is
   clear; use pattern zero while bit 15 is set. Set `0x20002ef0=1`.
2. Otherwise, if A selection flag `0x20002f08>0`, use A slot `0x20002430` and
   set `0x20002ef0=1`.
3. Otherwise, if B selection flag `0x20002f04>0`, use B slot `0x20000b20` and
   set `0x20002ef0=1`.
4. Otherwise use `(Aengine<<2)|Bengine` and clear `0x20002ef0`.

Thus A selection wins when both flags are positive. Slot patterns are the
**zero-based slot number**, with no added one. Selection displays do not blink
in this routine. Positive override uses 32768-sample half-cycles, nominally
1.36533 seconds per full cycle at 48 kHz. This field is also the calibration
stage, with entry/release behavior defined below. Other direct PWM writers
remain separate from this routine's contract.

The mode pins use GPIO base `0x58020c00`, masks 4096 (A) and 8192 (B). A pin is
low when that side's raw SAO byte is nonzero, or when its capture-active value
is nonzero and sample-counter bit 12 is set; otherwise it is high. Capturing
in SAM therefore toggles the pin every 4096 samples, nominally a 0.17067-second
full cycle. These mode-pin writes continue during display overrides.

Auxiliary pins use GPIO base `0x58020800`, masks 64 (A) and 128 (B):

- If the positive override field is active, neither auxiliary pin is written.
  Preserve prior pin state; do not treat an omitted write as off.
- Otherwise, a mismatch between the stored linear halfword and live linear
  word makes the pin high iff sample-counter bit 13 is set.
- When those values match, let F be `(ShiftSnapshot==1 or LF==1)` and P be
  `(pulseCountdown>0)`. The pin is high iff **F equals P**, and low otherwise.
  Exact comparisons against 1 matter for noncanonical raw flag values.

The stored linear halfwords are `0x20013490/+2`; live words are
`0x20002ec4/0x20002ec0`. Shift snapshots at `0x20002f60/+1` come from GPIO
`0x58021800` masks 64/128 in the preceding UI handler. LF words are
`0x200023d0/0x20000820`, and pulse countdowns are `0x2000242c/0x20002428`.
The previously checked phase path assigns 10/20 on A/B Sub/CV crossings for
non-0/non-5 auxiliary modes. Indicator evaluation precedes the once-per-callback
countdown decrement. These are pulse-display counters, distinct from the
Array clock/capture countdowns. The positive override suppresses their pin
writes; an Array-selection display does not. As these blink phases use the DSP
sample counter, disabling audio freezes them even while UI callbacks continue.

The same routine computes the tuning ratio as float32 B/A from stored odd
increments `0x200023e8` / `0x200023e0`, after each side's LF scaling. It runs
before the callback's new pitch calculations, therefore observes the previous
stored rates. For finite positive ratios, power-of-two scaling places the
ratio in [1,2). `tuning_beacon_action()` returns the following GPIO action,
using strict comparisons against the listed float32 bounds:

| Normalized ratio | Action |
|---|---|
| 0.99 < ratio < 1.01 | Green pin low |
| 1.24 < ratio < 1.26 | Red pin low |
| 1.323 < ratio < 1.343 | Green pin low |
| 1.49 < ratio < 1.51 | Green pin low |
| 1.59 < ratio < 1.61 | Red pin low |
| 1.656 < ratio < 1.676 | Red pin low |
| Otherwise | Both pins high |

Exactly zero takes the green-pin action without normalization. Negative,
nonfinite, subnormal and zero-denominator cases are outside this helper's
comparison contract. The manual's green octave/fourth/fifth and red third/sixth
descriptions provide color names; the actual numerical windows come from the
firmware. Do not substitute symmetric cents windows or an equal-tempered pitch
classifier. For example, a folded ratio of 1.2 is outside these windows, while
its reciprocal folds near 1.6667 and takes the red action.

The green/red actions clear only mask `0x100` / `0x400` at GPIO base
`0x58020000`. **They preserve the other pin.** The off action sets both high.
Thus the result is a pin-state update, not a stateless color: a direct red-to-green
sequence can leave both pins low until an off update. The 748 tests check
187 ratios, including float32 neighbors of each bound and octave-scaled values,
from all four initial two-pin states. This proves retention in this routine;
it does not measure the combined optical color or all other firmware pin writers.

### Calibration stage and display override lifecycle

`0x200144d4` is not merely a display timer: positive values also select
calibration branches in the audio callback. Call it `calibrationStage` when
porting the control state. `0x200144d0` is a separate shortcut flag. This
section defines digital entry/exit decisions, not the calibration measurement
algorithm or the physical input values to use during calibration.

The original boot decision at `0x08034582`, with both fields initially zero,
has this priority:

1. If either signed clock count at `0x20002ee8/0x20002ee0` exceeds 1, bypass
   the button choices and retain stage/shortcut zero.
2. Both Shift GPIOs high: branch to `0x080347ec`, the defaults/reset path;
   this decision itself leaves stage/shortcut zero.
3. Shift A high: stage 3, shortcut zero; shared-button state is not examined.
4. Otherwise: stage 1 if Shift B is high, else 0. Then, if the shared
   CV/interaction button is high, set stage 1 and shortcut 1.

The test covers 200 combinations: each count in {-1,0,1,2,INT32_MAX}, all
four Shift pin combinations and both shared-button states. It stops before
the common startup continuation (`0x0803458a`) or defaults/reset body; no
ADC startup, clock arrival timing or physical boot is inferred from it.

For stages 1 through 11, an isolated Shift release with the opposite Shift
low and no clocks or other button edges behaves as follows:

| Released Shift | Initial hold counter | Result |
|---|---:|---|
| A | Tested 0,1,8,9,10,49,50,100 | Clear stage to 0 |
| B | <=9 | Clear stage to 0 |
| B | >9 | Increment stage by 1 |

The 176 release cases check those eight hold counts, both sides and all
11 stages. Advancing **2 to 3** or **11 to 12** calls calibration save
`0x0802d330`; eight cases stop at its entry before executing flash operations.
No automatic stage advance or expiry occurs in another 704 checked callbacks
with stable Shift levels and initial holds 0/49/50/100 (four callbacks per
combination). This is not a test of long-hold thresholds or arbitrary duration.

Twelve separate caller continuations start after the unexecuted save call,
supplying return status 0 or 2 and shortcut values 0/1/2. Neither continuation
inspects the returned status. After entering stage 3, **only shortcut exactly
1** clears the stage; otherwise it stays at 3. After entering stage 12, it
always clears. The shortcut flag itself is retained. These are caller rules,
not evidence that calibration data was written successfully.

Clearing a positive stage also zeros all five bytes at each of
`0x20002f50`, `0x20002f58`, and `0x20002f60`: UI mode/interaction bytes and
previous/current button snapshots. It does not just hide the display. In
the checked post-save clear paths, the loop writes all five indicator GPIOs
high five times: auxiliary masks 64/128, mode masks 4096/8192, and interaction
mask 32768. Subsequent normal indicator/DSP processing is a separate step.
Do not conflate these cleared raw UI bytes with the separate cached DSP modes.

### Recovered even-rate and linked-pitch laws

`even_increment()` checks `0x080306da–0x0802ef10` and
`0x08030722–0x0802f074`, including their piecewise joins. In SAM, even rate is
the odd rate. In SAO, with calibrated signed control x and odd increment d:

```text
if x < float32(0.3): offset = x * float32(0.0001)
else: offset = float32(0.0001) + (d-float32(0.0001))
              * (x-float32(0.3)) * floatFromBits(0x3fb6db6e)
even increment = 0.5 * (d+offset)
```

Complete Noise/Chaos callbacks confirm that this same SAO even-increment law
feeds both engines. Noise then evaluates its even carrier at twice the odd
phase-coordinate scale (`16384` versus `8192`), so its carrier frequency spans
approximately unity to an octave above the odd carrier over x=0..1. Do not
multiply the shared even increment by two before every engine. Chaos consumes
the recovered increment directly in its coupled oscillator equations; its
complex output need not have one unambiguous fundamental.

The curve is not clamped here. x is derived from the negated signed audio-input
word shifted right by 15, minus calibration offset, multiplied by calibration
gain (slots 6/7). This traces the digital route without assigning jack voltages.
The tiny discontinuity at the 0.3 branch is retained by the compiled schedule.

`b_pitch_increment()` checks `0x0802ef3e–0x0802f068`. After piecewise pitch
calibration yields nonnegative integer q, unlinked B frequency is
`1.021974921 * exp2Table[q&2047] * 2^((q>>11)+4)` Hz. Both nonzero interaction
modes instead use `AunscaledHz * 0.06125000119 * exp2Table[q&2047] * 2^(q>>11)`.
Here AunscaledHz is the A frequency **before A's own LF division**. Divide by
48000 to obtain B's increment, then divide by 256 if B's LF flag is set.
The reference preserves the multiplication order. These laws do not establish
physical pitch calibration; the Follow/Sync enum labels are independently
cross-checked above.

## 5. Chaos: ordered four-oscillator network

Evidence: A `0x0802fa04–0x0802fc16`; B `0x0802fc96–0x0802ff00`.
Code: `chaos_step()`. Each side has four 16-byte records with phase at byte 0
and output at byte 12. A starts at `0x200022bc`; B at `0x200022fc`.

In readable algebra, `depth=9.9*Partials`, `feedback=0.2*feedbackControl*depth`,
and `ratio=1+21*ratioControl`. The ratio selects adjacent integers with fixed
parity; a sine-table-derived blend morphs between their sine outputs. This
lookup is not interpolated between adjacent sine-table entries.

Let `do/de` be the FM-adjusted odd/even increments, `qo=do/4`, `qe=de/4`, and
`p1..p4`, `y1..y4` the stored phases and outputs. Use the following order:

```text
p1 += qo + qo*feedback*(0.1*oldY2 + 2*oldY3)
wrap p1; y1 = ratioMorph(p1)
p3 += do + depth*do*(2*y1 - 0.1*oldY4)
wrap p3; y3 = sineTable(p3)
p2 += qe + feedback*qe*(2*oldY4 + 0.1*y3)
wrap p2; y2 = ratioMorph(p2)
p4 += de + depth*de*(2*y2 - 0.1*y1)
wrap p4; y4 = sineTable(p4)
odd  = 0.3*y1 + 0.6*y3
even = 0.3*y2 + 0.6*y4
```

Both positive feedback signs above are intentional. The executable version
preserves float32 rounding, fused operations and old/new dependencies. A's odd
FM addition is separately rounded; B's is fused. Algebraically equivalent
reassociation can quickly diverge in this network.

## 6. Noise: generator, two filters, carrier multiplication

Evidence: A `0x08031038–0x0803128a`, B `0x08031310–0x08031588`, including
`0x0803185c/0x0803186e/0x080318aa/0x080318bc/0x08031ed0/0x08031edc` joins.
Code: `noise_step()`.

Generator records are six floats `[phase, step, speed, start, difference, value]`:
A at `0x2000239c`, B at `0x200023b4`. Initialization sets both speed fields to 1.
A's update uses its speed field; B's compiled path adds the step directly.

```text
q = trunc(float32(Partials * 25000))
rate = exp2Table[q & 2047] * (1 << (q >> 11)) * (slow ? 0.0625 : 4)
step = rate / 48000
phase = (oldPhase > 10 ? 0 : oldPhase) + step * speed  [A]
phase = (oldPhase > 10 ? 0 : oldPhase) + step          [B]
```

On `phase>=1`, advance shared uint32 state at `0x200023cc` once:
`seed=seed*0x0bb38435+0x3619636b` modulo 2^32. The conversion is
`r=FMA(float32(seed), floatFromBits(0x2f80000d), -0.5)`. Set `start=oldValue`,
`difference=2*r-oldValue`, then truncate-wrap phase. A cosine-shaped table
weight interpolates start/difference. Even if a step exceeds one cycle, this
path consumes only **one** random value per sample.

Two six-float filter records use `[coefficient, unused, band, low, high, sum]`.
A records begin at `0x2000233c` and `0x2000236c`; B at `0x20002354` and
`0x20002384`. Let the records be U and V:

```text
U.coefficient = noiseRateTable[trunc(Slide*255)] * 2.953097105 * step
U.low  += U.coefficient * U.band
V.coefficient = Focus * 0.9 * U.coefficient
V.low  += V.coefficient * V.band
V.high  = U.low - 0.5*V.band - V.low
V.band += V.coefficient * V.high
V.sum   = V.high + V.band
U.high  = randomInterpolatedValue - (U.low + 1.2*U.band)
U.band += U.coefficient * U.high
U.sum   = U.high + U.band
```

Notice that V uses the just-updated U.low, while U.band is updated afterward.
The output source is V.high. An absolute-value follower uses attack 0.1 and
release approximately 0.00005; the normalization gain is `1.1/envelope`.
Follower addresses: A `0x20002e98`, B `0x200011c0`.

Multiply normalized V.high by interpolated sine carriers: odd phase ×8192,
even phase ×16384. B additionally multiplies those positions by the stored
interaction ratios in raw mode 2. Tests cover both B gain-loading branches and
their joined synthesis/phase/output path. A still computes the normal phase-tail
FM contribution. Final audio clipping happens outside this Noise slice.

There is no epsilon guard in the recovered normalization divide. With zero
filter/envelope state and generator `[0,0,1,0,0,0]`, tested A/B slices produce
quiet NaN before clipping. The subsequent VMINNM chooses the numeric +1.5
operand, so both clip schedules produce their **positive** endpoint. The
reference clips explicitly reproduce this quiet-NaN case.

The joined cold test executes the initialization prefix through `0x0802c9ba`
with zero-initialized RAM and saved Noise selections, then supplies valid Array
backing memory, ready counter 256 and audio-enable 1. It leaves the original
mute flag set. Partials and FM raw codes are 2000 (zero after default offset),
pitch codes are A=26000/B=30000, Slide/Focus codes are A=31000/24000 and
B=41000/37000, input audio is zero, interaction is independent and LF is off.
No engine state is warmed or replaced after initialization.

Across 12,800 processed samples, every digital output word matches the
reference. Samples 1..7937 remain muted; the clearing sample still stores
zeros. Both Noise outputs first become finite before clipping on sample
12005. Consequently, samples 7938..12004 produce the positive clip endpoint
on all four spectral lanes: 4067 frames, approximately 84.73 ms at 48 kHz.
The generator/filter state then recovers normally in this trajectory.
This proves a digital plateau for the specified fixture, not audible startup
DC on a physical module: board startup timing, analog output coupling and other
control positions were not measured.

A second 12,800-sample comparison starts from the **complete** initializer,
using the explicit busy-driver/tick contract in section 2. It retains the
initializer's audio-enable=1 and counter=0 rather than replacing them. Callback
delivery and ADC/Array inputs remain test fixtures because DMA start returned
busy. Under those same Noise controls, all output words and state comparisons
still pass: samples **1..8193** are muted, the first finite preclip sample is
again **12005**, and samples **8194..12004** produce the positive spectral clip
endpoint. This is **3811 frames**, approximately 79.40 ms at 48 kHz. The extra
256 muted samples relative to the earlier fixture follow directly from its
different initial counter. Keep both results tied to their entry contracts;
neither is a physical startup measurement.

For Rack, define a zero/nonpositive-envelope fallback that returns zero before
the normalization divide. This is a recommended adapter policy, not a recovered
firmware equation; keep a literal mode if comparing against the instruction
reference. The branch also avoids spending work on a known silent state.

## 7. Sub/CV modes and phase cadence

Code: `auxiliary_value()` and `normal_phase_step()`. Raw mode numbers:

| Mode | Value before final offset/scaling |
|---:|---|
| 0 | SAM: 4 × input envelope. SAO: default periodic waveform |
| 1 | Held random value |
| 2 | Linear interpolation from previous to current random value |
| 3 | Interpolated 1024-entry triangle table |
| 4 | Negated interpolated 1024-entry ramp table |
| 5 | Default periodic waveform regardless of SAM/SAO |

The default waveform is asymmetric in this image. A reads the extracted
shaped table at `0x08039674`. B reads sine at `0x08037674`, multiplies by
`1.5 + (65536-rawPitchCode)*2^-15`, then uses the fused cubic clip. The tests
preserve this rather than assuming two identical implementations.

Modes 0 and 5 advance Sub/CV phase by half the side's clean odd rate. Modes
1–4 use the stored auxiliary increments A `0x200023f8`, B `0x200023f4`.
Positive crossings subtract one and advance the shared Sub/CV RNG at
`0x20002eb0`; negative crossings add one without a new random value. This is
a single crossing correction, not an arbitrary modulo loop. Modes 1–4 also
set pulse counters A=10 / B=20 at `0x2000242c/0x20002428`.

The random conversion constant here is `0x3000000d`, twice Noise's constant,
with bias -1. After waveform calculation, modes 1–4 add **1.0** before final
output conversion. These are unipolar internal values; modes 0/5 are not
given this offset.

## 8. Exact digital output lanes

Pointer setup at `0x0802ec94–0x0802ed12` and stores at
`0x080302b8–0x0803038c` were executed for half offsets 0 and 128.

| Pair base | First word | Second word | Conversion |
|---|---|---|---|
| `0x30000c40` | A odd | A even | A cubic clip, signed fixed-point ×2^30 |
| `0x30000840` | B even | B odd | B cubic clip, signed fixed-point ×2^30 |
| `0x30001040` | A clean sine | A Sub/CV | sine ×2^30; aux ×858993472 |
| `0x38000000` | B Sub/CV | B clean sine | aux ×858993472; sine ×2^30 |

For frame n of a half-buffer, address is `pairBase + 4*halfWordOffset + 8*n`,
plus 4 for the second word. Conversion truncates toward zero. The normal-range
integer conversion tests avoid saturation; analog volts and converter wiring
are still outside this result.

Both audio clips clamp x to ±1.5 and implement `x*(1-k*x*x)`,
`k=floatFromBits(0x3e17b426)`. A fuses the multiply-subtract; B separately rounds
the multiplies and subtraction. Use `clip_a()` / `clip_b()` when checking bits.

Startup flag `0x20002ec8==1` writes zeros to **all eight** lanes. On a sample
whose counter is greater than 8192, the flag clears but that sample still
writes zeros; subsequent samples use the normal stores. This corresponds to
about 171 ms at nominal rate, subject to counter initialization/callback entry.

## 9. Capture writer and clock facts

### Mode buttons and shifted controls

The complete GPIO handler is checked on both sides with initial Shift counters
0, 49, 50 and 51. When Shift is already high, the handler increments its counter
before dispatching a new Array-button edge: initial 49 becomes 50 (unshifted
path), initial 50 becomes 51 (shifted path). The dispatch comparison is `>50`,
nominally about 68 ms at the 64/48000 callback cadence. This specifies digital
counter behavior, not contact debounce or electrical polarity.

For a side that is not capturing:

| Button action | Before | After |
|---|---|---|
| Array, unshifted | SAM: raw mode 0, engine 0 | SAO: raw mode 1, engine 0 |
| Array, unshifted | SAO: raw mode 1, engine 0 | Noise: raw mode 1, engine 1 |
| Array, unshifted | Noise: raw mode 1, engine 1 | Chaos: raw mode 1, engine 2 |
| Array, unshifted | Chaos: raw mode 1, engine 2 | SAM: raw mode 0, engine 0 |
| Shift + Array | SAM | Start capture |
| Shift + Array | Any oscillator engine | Toggle the side's LF flag; retain engine |

LF persists through the ordinary mode cycle. The tests verify engine/LF
mirrors at configuration offsets +16/+20 and +32/+36 from `0x2001348c`.
The raw SAM/SAO byte is mirrored later by the audio callback; the handler
test alone is not proof of a completed flash save. Capturing SAM uses the
start/stop behavior below rather than this inactive-side table.

Shift plus the shared CV/interaction button increments the selected side's
Sub/CV mode. SAM cycles **0,1,2,3,4,5,0**; SAO/Noise/Chaos skip 0 on wrap,
cycling **1,2,3,4,5,1**. A preexisting oscillator mode 0 moves to 1.
The 24 cases check all six starting values for each side and SAM/SAO flag,
the +8/+12 configuration mirror, dirty flag and unchanged interaction mode.
Mode 0 is the SAM envelope; mode 5 selects the periodic waveform even in SAM.
Section 4 gives the checked Follow/Sync indicator mapping.

### Concurrent Array and shared-button edges

The complete handler processes pending rising edges in this order:
**Array A, Array B, shared CV/interaction** (`0x0802dd60`, `0x0802ddce`,
`0x0802de3a`). Each consumed event is cleared. These are sequential actions
within one callback, not an exclusive choice of one action per callback.

The shared button examines the resulting Shift counters: if A exceeds 50,
change A's Sub/CV selection; otherwise if B exceeds 50, change B's; otherwise
cycle interaction. **Both Shifts held past the threshold select A only.**
An earlier shifted Array action sets its hold counter to 1601 and does not
consume the shared-button action. For example, simultaneous Array A/B/shared
edges with both Shifts initially at 50 start both SAM captures and advance
only A's Sub/CV mode; in oscillator modes they toggle both LF flags and
advance only A's Sub/CV mode. Without Shift, both engine cycles and the
interaction cycle can occur together.

The auxiliary wrap decision reads the cached SAM/SAO words at
`0x20002e94/0x20002e90`. The UI handler does not update those words when it
changes the raw mode bytes; callback mode comparison follows later. Keep
these fields separate in a literal implementation. These tests initialize
raw and cached modes consistently; they do not establish arbitrary mismatched
state behavior.

The 3,456 combinations cover all four initial modes on each side, initial
Shift counters 0/49/50 with stable GPIO snapshots, each pair or all three
Array/shared rising edges, Sub/CV initial values 0/5, and all three interaction
modes. Captures start inactive; clocks, Shift edges and long-hold actions are
absent. Instruction traces check dispatch order; state checks cover raw modes,
engines, LF, capture flags, Sub/CV, interaction, hold counters, cached modes,
engine/LF/Sub mirrors and cleared pending events. Each case executes a second
callback with unchanged GPIO: no button redispatch occurs, and the checked
mode/capture/selection state and mirrors remain unchanged. Concurrent Shift
release/selection, long-hold, clock and already-active capture combinations
remain outside this specific contract.

### Simultaneous Shift releases and coincident Array/shared presses

`test_simultaneous_shift_releases_with_array_edges` checks 3,200 cases with
both previous Shift snapshots high, both current inputs low, previous gesture
bytes 1, SAM on both sides, no clock events, and every combination of Array/shared
rising edges and active-capture flags. Each initial hold counter is
one of 0/9/10/49/50/51/499/500/1499/1500. Slots 0 and 1 have dimensions 65-by-1,
Sub/CV mode is 5, initial clock countdowns/policies are zero, and scan offsets
are 7. The original handler runs to `0x0802dce4` for release-state comparison,
then resumes to its normal return for Array dispatch and capture checks.

For this contract, let HA/HB be the initial counters. The release rules are:

1. A release selects B's next slot if HB >49 and HA is 51..1499. Otherwise,
   if HB <=49 and HA <500, A admits a clock-like event.
2. If A selected B, B's normal release action is suppressed. Without any Array
   or shared input high, B's hold counter clears to zero. With any such input high,
   B's consumed-gesture path increments the assigned 1601 and clamps it to 1600.
3. Otherwise B release selects A's next slot if HA >49 and HB is 51..1499,
   setting A's counter to 1601. If HA <=49 and HB <500, B admits a clock-like
   event. Ordinary release branches retain their counters until a later callback.

Selection resets that side's scan offset to zero. An admitted clock-like event
sets policy 1 and countdown 1 for active capture, otherwise 2 for the supplied
65-frame first dimension; mode 5 advances the existing scan offset by one.
The test checks these intermediate values before Array dispatch.

Array dispatch still compares the retained counter with 50, even though the
current Shift input is low. A rising Array edge with counter >50 starts or
stops capture using the **now-selected descriptor**. At <=50 it enters SAO
only if that side is not capturing. Thus releasing both Shifts and pressing
Arrays in the same sampled callback must not be modeled as independent,
automatically unshifted Array presses.

The shared CV/interaction edge runs last and uses those same counters. It targets
A if A's counter is >50, otherwise B if B's is >50; otherwise it advances the
interaction enum. In these SAM fixtures the targeted Sub/CV value wraps 5 to 0,
while the untargeted side stays at 5. An unshifted shared edge wraps the initial
interaction enum 2 to 0. Both runtime values and settings mirrors are checked,
along with all pending Array/shared event words being cleared. For example,
initial HA=50/HB=51 selects A via B's release, sets HA=1601, then targets A's
Sub/CV on the shared edge despite both current Shift inputs being low.
All 1,600 shared-edge cases execute another complete handler call with unchanged
GPIO: no Array/shared dispatch recurs, and the checked modes, captures,
descriptors, Sub/CV values and settings mirrors remain unchanged.

`test_persistent_simultaneous_shift_release_capture` reaches the behavior from
actual GPIO press/hold sequences without injecting counters or capture flags.
After holding both Shifts for 61 handler calls, releasing both while pressing
both Arrays selects only B's next slot and starts capture on A slot 0/B slot 1.
Repeating the gesture with both captures already started on slot 0 instead
stops them. With no capture-writer calls between start and stop, A length is
zero, while B length becomes **67,107,840**:
`uint32(oldSlotOrigin - newSlotOrigin) >> 6`. The active flags clear and both
raw modes remain SAM. A subsequent held-Array callback does not redispatch.
This independently reaches the invalid-descriptor hazard through physical-button
snapshots; no external clock or descriptor injection is required. It is not an
audio/capture-rate simulation or proof of electrical switch timing. Other initial
Sub/CV/engine states, calibration, and prior consumed-gesture states beyond the stated sequences
remain separate combinations.

### Consumed Shift gestures and re-arming

`test_consumed_gesture_release_and_rearm` reaches both gesture bytes equal to 4
by holding both Shifts for 61 complete handler calls, then pressing both Arrays
to start capture. It supplies no hold counters or capture-active flags. The test
then exercises all 32 high/low combinations of Shift A, Shift B, Array A, Array B
and the shared button, with another callback at each unchanged snapshot.

For this reached state, **any one of the five panel inputs still high keeps both
consumed Shift gestures latched**. Releasing a Shift alone does not clear its
counter. Held callbacks clamp both counters to 1600; a new shared edge still
uses the shifted A-priority path (Sub/CV 5 to 0 in this fixture). Array inputs
were already high when capture started, so this stage supplies held/falling
Array inputs, not fresh Array presses. Neither unchanged snapshot redispatches
the button actions.

One callback with all five inputs low clears both gesture bytes and both Shift
counters to zero. A subsequent shared rising edge advances interaction from
0 to 1, updates its settings mirror, and leaves both Sub/CV values unchanged.
Holding that press does not advance interaction again. Both captures remain
active throughout: clearing a consumed gesture does not itself stop capture.
This gives the Rack input adapter a tested all-buttons-released re-arm rule for
the reached dual-capture state. It does not establish every mixed gesture-byte,
engine, calibration or pending-clock combination.

### Array selection and long holds

The 640 selection cases execute the complete handler with one Shift held and
the opposite Shift falling from its previous high snapshot. No Array/CV edge
or pending clock is supplied. Let H be the target side's initial hold counter
and R the released opposite counter. Selection occurs when **51 <= R <= 1499**
and the held counter observed by the release branch exceeds 49. A is processed
first, so that observed counter is H+1 for target A and H for target B.
The tests use H in {49,50,51,300}, R in {0,50,51,1499,1500}, and all 16 slots.

The selected slot becomes `(slot+1)&15`; its clock scan offset resets to zero,
its configuration mirror is updated, and settings become dirty. This is an
opposite-Shift **release** gesture, not an increment on initial press.
Selection lives at `0x20002430` / `0x20000b20`, scan offsets at
`0x20002438` / `0x20002434`, and the persisted slots at `0x200134b4/+4`.

The 64 long-hold cases use both Shift snapshots high and stable, idle gesture
state, and no Array/CV or clock event. The target's incremented hold must be
**>1500 and <=1600**, and greater than the opposite counter observed at that
point. If the observed opposite counter is <=300, the target linear/planar
flag toggles, settings become dirty, and target hold becomes 1601 to suppress
repetition. If the opposite counter is >300, the selected Array is replaced
with factory spectra and a save request follows. A's earlier increment affects
the value seen by B: initial opposite=300 remains 300 for A's decision but is
301 for B's. These thresholds are checked around 1499/1500/1501/1600 and
0/299/300/301. The 1501-block threshold is nominally 2.00133 seconds at the
recovered cadence; this is not a measured physical debounce duration.

Factory replacement functions `0x080333c4` / `0x080333fc` are checked through
return for both sides and every slot. They copy exactly **16,384 bytes** from
`0x08045a98` (the exported 64×64 coefficient table: 64 frames of 64 floats),
preserve the selected descriptor's word offset, set dimensions to **8×8**, and
clear its save marker. Data beyond that copy remain untouched; they do not
clear an entire 1024-frame slot. The destination uses fixed bank bases
`0x60c01000` / `0x60001000` plus four times the descriptor offset.

Twelve tested long-hold gestures reach that exact replacement and the pending
Array-save call at `0x0802e6a6` / `0x0802e686`, with side argument 0/1.
Execution stops before calling `0x08033af8`: file writing and subsequent gesture
completion are not asserted. Thus “delete” is a factory-content replacement in
the checked RAM path; successful persistence to media remains a separate question.

### Capture and playback sequence

Verified writer slices: A `0x080320e8` to `0x0802e7e8`; B `0x0803219a` to
`0x0802e7fc`. These are entered only for the side's capture-active value 1.

| Field | A | B |
|---|---|---|
| Capture active | `0x20002e84` | `0x20002e80` |
| Capture policy | `0x20002ed8` | `0x20002ed4` |
| Countdown | `0x20002ed0` | `0x20002ecc` |
| Write position in float words | `0x20002e7c` | `0x200011a0` |
| Accumulated peak | `0x20002e9c` | `0x200011e0` |
| Source amplitudes | `0x20002b40` | `0x20002d40` |

Policy 0 writes at every eligible callback. Policy 1 writes only when the
countdown equals **1**. Other policy values skip writes. Each write copies
exactly 64 floats, advances the word position by 64, and tracks the maximum
coefficient. It does not take an absolute value when tracking that maximum.

When the post-write position equals `descriptor.offset + 65536`, capture
stops, policy clears, and dimensions become 1024 × 1. This is an **equality**
check. A Rack wrapper should validate offsets/capacity rather than reproduce
unchecked writes for corrupt descriptors. A valid uninterrupted free capture
therefore reaches capacity after 1024 callbacks, nominally **1.36533 seconds**.
UI/media work can affect wall time; this number is the DSP-cadence duration.

Ten tests execute the complete button handler `0x0802d900` through return,
including original GPIO-read instructions against explicit register snapshots.
With SAM selected, Shift already held and its counter initialized to 51, a
new Array-button press enters `0x0802dda2` / `0x0802de0e`: it starts from the
selected slot offset, sets active=1 and dimensions 1024 × 1, clears the save
marker and peak, and preserves the current capture policy.

The same gesture while active enters stop at `0x0802e4c6` / `0x0802e264`:
active becomes 0 and dimension1 is `(writePosition-offset)>>6`, with dimension2
unchanged. A clears policy; **B preserves it** on this path. Both preserve peak
and save marker. Zero written frames produce dimension1=0 without a minimum
clamp. The isolated suite supplies positions for 0, 1, 17 and 1023 frames.

Another 48 sequences begin with a 1024-frame descriptor and scan offset
0, 63 or 1023. They execute the full start handler, 0/1/2/65 original frame
writes, button release and the full stop handler, for both sides and slots
0/15. **Start and stop both preserve the scan offset.** This can leave an
offset larger than the new logical length. A subsequent direct planar-reader
call verifies all source addresses and all 64 deltas against supplied contiguous
RAM. It can cross the physical slot boundary, as detailed in section 10.
The initial direct-reader portion isolates retained state. Each sequence now
continues through unchanged slow controls, a SAM-to-SAO gesture, explicit media
failure paths and the SAO reader, as described below. It does not generate a
complete post-transition audio callback or assert physical media behavior.

A separate eight-callback test keeps one initialized machine alive through
both sides' actual start/stop gestures, nonzero SAM input, all three interaction
modes and alternating DMA halves. It starts free capture, records three
callbacks, admits clocks on both sides for a fourth frame, skips a callback
without a clock, admits an A-only clock for its fifth frame, then stops both.
The clocks follow a release and re-press of Shift, with stable snapshots before
admission. The test checks every prior captured frame after each callback,
the untouched next frame, write positions, active flags and accumulated peaks.
Each frame is the **final detector spectrum of that callback**, not an average
of its 64 sample-wise spectra.

The final descriptors are A=5×1 and B=4×1; A's stop clears clock policy and B's
preserves it. Both original planar readers then consume those captured spectra
at explicit normalized coordinates (Focus=.25, Slide=.75, clock offset reset to
zero), producing bit-exact coefficient deltas. This joins capture to the reader;
it does not exercise an actual SAM-to-SAO mode gesture or save/reload the data
through physical media. All eight outputs and detector/phase state of these
callbacks are also checked with the same tolerances as the SAM integration test.

### Save dispatch and playback-affecting mutation

An additional eight-case test executes an unshifted Array gesture and then the
callback's mode-comparison slice beginning at `0x0802e77e`. This comparison runs
**before slow controls**, after the UI handler and LED routine. The earlier
description of intervening slow-control work was incorrect. It covers both sides, SAM-to-SAO and Chaos-to-SAM,
with readiness counter `0x20002eec` equal to 128 or 129. At 128 the comparison
is bypassed; at 129 the new mode is mirrored into settings and the pending
operation byte at `0x20002f65` becomes 1.

The test explicitly sets the transport handle's lock byte at `0x20014c38` to
1, causing original `0x08026ea8` / `0x08027138` instructions to return early.
No calls are replaced. Despite that condition, SAM-to-SAO continues to
`0x08033af8` and the first unsaved Array's file-open request. Chaos-to-SAM
instead reaches the settings packer before flash unlock. This defines the
tested busy-transport path; the longer failure-path sequences below now join
capture to the SAO reader. Successful peripheral transactions and media
completion remain unverified.

The 48 capture-shrink sequences now continue on the same machine through these
steps, without changing the captured descriptor, scan offset or working bank:

1. Process slow controls in SAM with Slide and its stored anchor both 1.
   The old offset remains unchanged.
2. Release Shift/Array and press Array unshifted through the original handler.
   It selects SAO and preserves the offset.
3. Execute `0x0802e77e` continuously through `0x0802eb5c`, including the save
   routine, all of its called failure paths and subsequent slow-control readers.
   The fixture has no registered filesystem (`0x20002090=0`), transport lock
   byte `0x20014c38=1`, flash-program handle busy byte `0x20002064=1`, readiness
   129 and audio-enable 1. Flash registers are ordinary mapped RAM, not a flash
   device model. The real filesystem path returns 12; the real program routine
   returns busy before writing. No call is replaced or intercepted.
4. Compare captured RAM after save-time normalization and every SAO delta.
   **The old offset survives all the way to that reader.** File-open failure
   does not prevent the saver from scaling the live coefficients. The settings
   cursor advances from `0x081e0000` to `0x081e0020` despite the busy program
   return. This is an executed failure path, not successful persistence.
5. Change the side's Slide ADC from 32768 to 32400 with gain 1/32768 and
   re-execute slow controls through the reader. The .01123046875 movement clears
   the offset, and all deltas match the new manual position at offset zero.

These comparisons supply synthetic analyzer frames and calibrated ADC inputs;
they do not model elapsed time during I/O or produce all audio samples through
the transition. They establish callback ordering, retained scan state, save-time
RAM mutation and subsequent reader behavior under the stated failure contract.

### Pending-operation recovery on following callbacks

`pending_operation_step()` defines the callback prefix for quiet UI inputs.
At `0x0802e764`, a nonzero byte at `0x20002f65` increments with uint8 wrap;
zero stays zero. The original UI handler then runs. If the resulting byte is
greater than 2, `0x08032224` issues recovery requests, clears the byte and
branches back to `0x0802e77a` for the normal LED/mode/control/audio work. This
branch does **not** return early or bypass the sample loop.

All 256 initial byte values are compared through the original callback prefix,
including real UI processing and busy-handle transport returns. Requested
bytes, observed at original callee entries without replacing calls, are:

| Call | Command bytes | Other argument |
|---|---|---|
| `0x08026ea8` | `9e 0e 00` | Length 3 |
| `0x08026ea8` | `9e 05` | Length 2 |
| `0x08027138` | `9f 05` | Length 2, receive buffer `0x20002f88` |

The fixture keeps transport lock byte `0x20014c38=1`, so each request follows
its real busy return. Recovery nevertheless clears the operation byte. A mode
change that leaves byte 1 therefore produces 2 at the next quiet callback and
0 after recovery in the following callback. Initial 255 wraps directly to zero
without recovery; that boundary is tested, not a claim that normal gestures
produce 255. Command requests do not establish their physical device effects
or transaction timing, particularly while busy.

The separate 12-callback SAO sequence starts with byte 1 and the same busy
transport fixture. Its second callback executes recovery; all callbacks still
advance the sample counter by 64. Working coefficients, deltas, phase/RNG state,
sine/Sub words and spectral outputs pass their existing comparisons through
that callback. This establishes continued digital DSP for the checked busy
path; successful transport may have external effects absent from this model.

The Array saver scans all 16 descriptors of the requested side from slot 0;
it does not restrict the scan to the currently selected slot. Nonzero markers
are skipped. Thirty-four entry-to-boundary cases cover every possible first
unsaved slot and the no-unsaved case on both sides. Before the first file-open
call, the selected zero marker becomes **slot+1**, and the generated filename
is `speca000.wav`..`speca015.wav` or its B equivalent. When every marker is
nonzero, control proceeds to settings save. A nonzero marker therefore does
**not** prove a successful file write: it is set before opening the file.
These tests stop at the first open, so do not establish later-slot behavior
after a media error.

`array_save_frame()` defines the numerical transformation of one frame:

```text
gain = float32(1/trackedPeak) if 0 < trackedPeak < 1 else 1
scaled[i] = float32(coefficient[i] * gain)
ArrayRAM[i] = scaled[i]
pcm16Bits[i] = trunc(float64(scaled[i]) * 32767) & 0xffff
loaded[i] = float32(signed16(pcm16Bits[i]) / 32768)
```

The peak is the **shared per-side tracked capture peak**, not a freshly computed
per-slot maximum. Gain is rounded before multiplication. Scaling mutates the
live Array, as well as filling the scratch frame used for file conversion;
the RAM values retain scaled float32 precision, not PCM16 quantization.
The original instructions at `0x08033b82` / `0x08033cf0` through the first
seek call, and the separate post-seek conversion slices, check 28 frames:
two consecutive frames on each side for peaks -1, 0, .125, .3, the float32
immediately below 1, 1 and 2. Frame/scratch/output writes and the untouched
Array suffix are checked. Coefficients include positive and negative values
beyond unity: the stored halfword **wraps**, with no int16 saturation. Inputs
outside the finite int32 conversion domain remain outside this contract.

Each resulting 128-byte PCM frame is then passed through the original signed
16-bit read-conversion slice `0x08033490..0x080334ba`, supplying an explicit
successful-read buffer and byte count. All 28 readbacks agree bit-for-bit with
the formula. These are separate numerical slices, not simulated filesystem
success. The safe `encode_pcm16()` interchange tool still rejects overflow and
does not mutate caller data; use the new helper when implementing literal
firmware save behavior. See [ARRAY_FORMAT.md](ARRAY_FORMAT.md) for file layout.

### Connected save bytes, reload and write faults

`MemoryWriteFileFixture` extends the explicit file-boundary method used by the
parser tests below. It substitutes open/read/write/seek/truncate/sync/close,
records every operation, and excludes those function entries from instruction
coverage. Its declared policy creates/truncates files on open, grows seeks with
zero-filled bytes, and snapshots bytes on successful close. The firmware's
save/header/finalizer instructions remain original. This is **not successful
FatFs or hardware-media execution**; zero-filled gaps belong to the fixture.

Ninety-six cases execute the Array saver from `0x08033af8` until the final
settings-save call at `0x0802d280`: both sides, slots 0/7/15, lengths 0/1/2/64
frames, and tracked peaks 0/.25/1/2. The resulting bytes match independently
constructed headers and `array_save_frame()` payloads exactly. For N frames:

```text
offset 0:  RIFF, uint32(36 + 128*N), WAVE
offset 12: fmt , uint32(16), uint16(1), uint16(1), uint32(48000),
           uint32(192000), uint16(4), uint16(16)
offset 36: data, uint32(128*N)
offset 44: N consecutive 128-byte PCM16 coefficient frames
```

Integers are little-endian. Total length is `44+128*N`; byte rate 192000 and
block alignment 4 remain inconsistent with ordinary mono PCM16 at 48 kHz.
Filename/open flags, normalized live RAM, unchanged suffix and the prewritten
slot+1 marker are also checked. The final settings-save routine is a separate
boundary; these tests do not establish flash persistence.

Each generated file then passes through the complete original parser and A/B
loader under explicit read/seek substitutions. Empty and one-frame files are
**saved successfully in the fixture but rejected by the loader's >64-scalar
admission test**, preserving its old descriptor/data. Two-frame files restore
2-by-1 dimensions; 64-frame files restore 8-by-8. Imported coefficients match
the signed PCM16 readback formula above, not the higher-precision normalized
RAM values. Thus literal save/load is quantizing and need not be a lossless
round trip even with successful file operations.

Two further save invocations have slots 0, 7 and 15 dirty simultaneously, with
different content and one side-wide tracked peak. All three files are produced
in slot order, applying that same peak-derived gain to each; other markers are
unchanged. The saver continues scanning after closing each file.

Payload failure tests use three-frame Arrays on both sides. Status 1 or 9 is
injected at each of the first/middle/last 128-byte writes (12 cases). The source
cursor advances and RAM normalization still occurs for the failed frame, but
the successful-sample counter does not advance. The next frame therefore uses
the failed frame's file position. Final payload/header length covers the two
successful frames, with the failed frame omitted. The marker is not restored
to zero, and closing/settings-save dispatch still follow.

Another 24 cases return status zero with reported write counts 0/1/64/127.
The saver ignores the count and advances by a full 64 coefficients. In the
fixture this leaves zero-filled gaps while the data chunk advertises all three
frames. A short final write also exposes ordering in header finalization: the
RIFF size is written from the file length **before** the final seek extends it
to the advertised data end. Its size remains `292+reportedCount` rather than
420 in those final-short-write fixtures, even though final file length is 428.
Real media gap contents and driver short-write conditions remain unverified.
For Rack persistence, check both status and byte count and only mark a slot
saved after the complete operation succeeds; keep the literal failure behavior
separate when comparing against this reference.

Import arithmetic and descriptor setup now have additional instruction checks,
detailed in `ARRAY_FORMAT.md` sections 6–7. PCM24/32 use the literal gain
`0x2ffffff6` after integer-to-float32 conversion; float32 input is copied.
PCM24 reports only complete groups of four even when it writes a partial group.
The loader's local admission comparison requires more than 64 samples, while
its dimension assignment truncates non-special lengths to `sampleCount>>6`.
Its post-conversion copy always transfers 64 floats, even after a zero return.
Rack should validate frame completeness, capacity and read success explicitly;
these quirks do not supply safe malformed-file semantics. Header format-field
construction also now confirms the documented byte-rate/alignment anomaly by
original-instruction execution, still without a physical saved-file fixture.

Another 576 cases now define parser boundaries: exact RIFF/WAVE signatures,
signed format-tag/channel handling, format-code selection and retention,
chunk advancement, and termination. A zero parser return does not prove a
valid file: signature mismatch and EOF with missing chunks can both return
zero after successful read statuses. Odd `fmt ` chunks omit the padding applied
to unknown/data chunks. Tag 3 forces depth 32; unsupported PCM depths retain
the previous format code. See `ARRAY_FORMAT.md` section 6 for the full contract
and test boundaries. Rack import should validate the supported mono format,
payload dimensions and complete reads itself. The connected tests below now
cover complete parsing and selected import paths with explicit file substitutions;
successful filesystem execution remains separate.

Loader count and read-window checks now bridge more of those boundaries.
The count divides payload bytes by `floor(depth*channels/8)`, but conversion
still consumes scalar samples without downmixing. Joined mono/stereo/three-channel
PCM16 sequences verify the resulting first-128-scalar import and dimensions.
The reader requests a four-byte lookahead and removes it only after a full
read. A one-byte-short read can write 67 PCM8 values or 65 PCM16/PCM24 values
for a nominal 64-value request. Unaligned short reads can include supplied old
scratch bytes. These rules and the exact test scope are defined in
`ARRAY_FORMAT.md`; Rack should keep payload/frame validation explicit.

### Complete file parsing and connected Array import

`reference/sp67_file_fixture.py` defines an **explicit file-I/O substitution**,
not a FatFs or device emulator. Original wrapper/parser instructions run until
entry to read (`0x0802b264`) or seek (`0x0802b944`). The fixture then supplies
bytes/count/status, updates the supplied file object's size/position, and resumes
at the original return address. It preserves unread destination bytes, clamps
read-only seeks to EOF, and permits injected error statuses. Every operation is
recorded; substituted function instructions are excluded from instruction
coverage. Parser stack scratch begins with explicitly supplied 0xa5 bytes.
Bounded read sizes, instruction counts and I/O-call counts prevent an invalid
file from issuing unbounded host reads or hanging the test.

The original wrapper `0x08033ab0` and complete parser `0x08033094` are exercised
on 405 synthetic files: PCM8/16/24/32 and float32; 1/2/3 channels; fmt lengths
16/18/40; fmt-before-data, data-before-fmt, or preceding odd JUNK/clm chunks;
and RIFF sizes 0/actual/0xffffffff. Rate, depth, channel count, format code,
advertised data length, start and unpadded end all match independent expected
metadata. An unsupported trailing format chunk is ignored once both required
chunks have been found. Every read/seek boundary of the ancillary-chunk file
also propagates supplied status 1 or 9 immediately, with no subsequent I/O.

Fourteen complete edge files additionally establish:

- Invalid signatures, an empty RIFF body, and missing fmt/data can return zero
  while leaving some or all output fields unchanged.
- Before both required chunks are found, a duplicate fmt or data chunk replaces
  the earlier metadata. After both are found, later duplicates are not read.
- A 17-byte fmt payload with standard even padding leaves data undiscovered;
  the corresponding unpadded file is parsed successfully by this firmware.
- Advertised data length is accepted without checking that its sample payload
  exists: a 256-byte data declaration with only three bytes supplies end=300.
- After a normal 16-byte fmt, a final four-byte `data` header lacking its size
  reuses **16** from the previous chunk-header scratch. It returns data start=44
  and end=60 even though the file ends at byte 40. This is a concrete scratch
  history observed through the complete parser, not an inferred default value.

Another 120 sequences start at the A/B loader's successful-open continuation
(`0x08034b66` / `0x08034d66`) and stop before close. They join the original
wrapper/parser, channel-based count, aligned seeks, sample converter and frame
copies for five formats, 1/2/3 channels, fmt lengths 16/18, both banks and slots
0/15. All import the first 128 scalar coefficients, produce dimensions 2-by-1
and marker 1, preserve the descriptor offset, and leave the following frame
untouched. This confirms scalar import rather than deinterleaving across the
five formats, including two-byte payload misalignment and EOF short reads.
Open, close, directory traversal, real FatFs, media timing and physical files
remain outside these connected tests. A safe Rack importer should retain its
explicit format/channel/size/completeness checks rather than interpreting the
firmware's zero parser status as validation.

### Clock admission and timing

Slide movement cancels a nonzero clock offset during slow-control processing.
`slide_clock_tracking()` defines the float32 order checked in 216 executions
of `0x0802e86c–0x0802eb5c`. When the incoming offset is zero, the tracking anchor
is replaced by the previous callback's clamped Slide value. While the offset
is nonzero, that anchor stays frozen. A movement **strictly greater than
float32(0.005)** clears the offset; equality preserves it. On the cancellation
call the old anchor remains stored, and the following zero-offset call refreshes
it. This measures displacement from the anchor, not accumulated travel.

A measures `abs(float32(rawNormalizedSlide-anchor))` before clamping Slide;
B measures `abs(float32(clampedSlide-anchor))` after clamping to [0,1]. Both
readers receive the clamped value. The distinction matters for calibrated
controls outside [0,1]. Tests cover both sides, offsets 0/1/63, two gains,
three ADC values, endpoint anchors and the threshold's adjacent float32 values.
They check the stored anchor, movement, Slide and offset exactly; exceptional
floating-point inputs and physical volts-to-ADC mapping are outside this test.

Clock interrupt `0x08032688`: argument **64 feeds A**, incrementing the uint32
count at `0x20002ee8` and setting flag `0x20002ee4`; argument **128 feeds B**
at `0x20002ee0/0x20002edc`. The earlier notes' A/B labels were reversed.
The ISR addresses themselves were correct; the 216 joined ISR/UI cases now
prove which Array and capture fields each flag drives. Other arguments do
neither. This is digital behavior, not a voltage threshold specification.

UI processing at `0x0802d946` increments elapsed block counters once per
callback. The `>6000` branch restores capture policy 0 only when capture is
inactive: nominally just over eight seconds, not an unconditional recording
timeout. Thirty-two tests check initial elapsed values 5998..6001, both sides,
capture on/off and a fresh admitted edge. Expiry is evaluated before the edge;
an admitted edge in that same call sets policy back to 1 and elapsed to zero.
Without an edge, an active capture retains policy 1 beyond the threshold.
On the clock-handling path at `0x0802e2f0` / `0x0802e39e`, countdown is
`ceil(dimension1/64)`, or 1 while capturing, and policy is set to 1. The joined
tests initialize stable Shift GPIO snapshots (`0x58021810=192`, previous states
both 1), execute the original ISR and then the complete handler. This isolates
clock admission from a simultaneous physical button edge.

Do not assume both sides admit every pending flag. A 256-case truth table
executes the ISR and complete handler with stable previous/current Shift
snapshots, idle gesture state, no Array/CV button, hold counters in {0,49,50,51},
and neither/either/both clock flags. Let SA/SB be the high Shift snapshots,
HA/HB the initial hold counters, and EA/EB the pending flags. Within this contract:

```text
A admitted = EA and HB <= 49
HA seen by B = HA if EA else (HA+1 if SA else 0)
B admitted = EB and SB and (HA seen by B <= 49)
```

The B path clears a flag without admission when its Shift snapshot is low;
the A low/idle path can still admit its flag. A's earlier processing changes
the hold value observed by B, so simultaneous flags can differ from separate
calls at the threshold. All pending flags are consumed in these cases.
These are literal digital branches, not a claim about the physical relationship
between a clock jack, its interrupt and the sampled GPIO levels. Broader hold
ranges, previous-level transitions and active gesture state need their own
admission contracts before extending this truth table.

For auxiliary modes 0/5, the admitted edge advances the Array offset by one;
an offset >= dimension1*dimension2 becomes zero. For modes 1–4, the auxiliary
rate uses the float at `0x60000000+4*period` when period is 1..1023, or
`float32((1/64)/period)` for 1024..5999. Period 0 or >=6000 preserves the rate.
The routing tests provide distinguishable table values to verify selection.
A separate initialization test runs `0x0802c536–0x0802c556`: entries 1..1023
are exactly `float32((1/64)/index)`; entry 0 is left untouched. Thus
`auxiliary_clock_rate()` describes the initialized live-rate law. The measured period
includes the handler's initial elapsed-counter increment. The handler stores
`trunc(float32(period*float32(1.333333254)))` into the corresponding persisted
configuration word, marking dirty when changed, then resets elapsed to zero.

Restore at `0x0802c556–0x0802c5a8` converts a saved nonnegative period word into
`floor(3*saved/4)` for the tested ordinary range. It uses the table below 1024
and direct division at/above 1024. A resulting nonpositive index reads table
entry 0; this initializer does not define its value. Unlike live admission,
the restore division has no 5999 cutoff. The executable checks supply entry 0
explicitly and include both sides and the 1023/1024 boundary. Large corrupt
saved values are outside that contract: the actual conversion uses a signed
26-bit extraction after a wrapping integer multiply.

Countdowns decrement at handler entry and clamp at zero. The 1,728-call sequence
test checks both sides, modes 0/1/5, dimensions 65/129/1024, capture on/off and
held/released GPIO snapshots over 24 calls each. Repeated admitted edges reload
the countdown. Holding Shift suppresses the intervening Array scan steps;
with both Shift snapshots low, a positive remaining countdown advances the
offset once per call in modes 0/5. Modes 1..4 leave the offset alone.

A short Shift release itself takes the admission path: it reloads countdown,
sets policy, advances the offset or updates the auxiliary rate, and resets
elapsed time without a new ISR event. The tested release occurs immediately
after an admitted edge, with hold counters below 50; it must not be mistaken
for a second hardware clock interrupt. Longer gestures and concurrent edges
still need separate coverage. The opposite Shift hold counter can suppress
admission after 49; the long clock-to-reader test therefore holds only the
target Shift snapshot high and leaves the opposite snapshot low.

### Concurrent clocks, Array buttons and capture

The 6,144-case joined test executes the original clock ISR, complete UI handler,
then both active checks and capture writers at `0x0802e7de–0x0802e7fc`.
Its contract is both Shift GPIOs stably high, gesture bytes both 1, initial
hold counters in {0,49,50,51}, SAM on both sides, auxiliary mode 0, slots 0/1
with initialized offsets, and neither/either/both pending clocks. It combines
all nonempty Array-button masks, capture and policy masks, and inactive slot-0
first dimensions 8/65. Analyzer spectra are explicit inputs; the intervening
DSP body and subsequent persistence are not executed in this test.

Clock handling precedes Array rising edges. With initial holds HA/HB and
pending clocks EA/EB, the tested prefix is:

```text
if EA:
    if HB <= 49: admit A clock
    elif HA >= 51: select next B slot; HB = 1601
else: HA += 1

if B slot was selected:
    HB = 1600  # selection gesture 4 skips B clock processing
elif EB:
    if HA <= 49: admit B clock
    elif HB >= 51: select next A slot; HA = 1601
else: HB += 1
```

All pending clock flags are consumed. Selection resets that side's scan offset
and updates its settings mirror. This is a bounded rule for the stated holds
and gesture state; do not extend the `>=51` branches to long holds. The
subsequent Array edges run A then B, using these **updated** hold counters and
selected descriptors. Thus a clock at initial hold 50 can prevent the ordinary
increment to 51: the Array edge then takes the unshifted path. An inactive SAM
side enters SAO; an already-recording side does not stop on that unshifted edge.

An admitted clock sets policy 1 and countdown 1 when already recording,
otherwise `ceil(oldDimension1/64)`. Starting capture afterward changes the
descriptor to 1024-by-1 without recomputing that countdown. Consequently the
8-frame fixture writes its first spectrum in the same capture tail, while the
65-frame fixture has countdown 2 and skips that tail. Policy 0 writes freely;
policy 1 writes only at countdown 1, as in the isolated writer definition.

**Reachable descriptor underflow:** selection is not inhibited by an active
capture. If the opposite clock selects slot 1 and the same callback contains a
shifted Array stop, the stop subtracts the newly selected base from the old
capture cursor:

```text
selected.dimension1 = uint32(oldCaptureCursor - selected.offset) >> 6
```

For slot 0 cursor `origin+1088` and slot 1 base `origin+65536`, the result is
67,107,857 frames. The second dimension, marker and old cursor survive; the
original slot's descriptor remains 1024-by-1. A clears capture policy on stop;
B retains it. The selected descriptor can therefore remain marked saved despite
this mutation.

Two additional persistent sequences reproduce the underflow on A and B without
injecting active-capture state or hold counters: hold the target Shift, press
Array to start, release, hold both Shifts, then deliver the opposite clock with
the target Array rising edge. Every intervening original capture tail writes
and checks a supplied spectrum. These sequences validate reachability in the
digital handler, not physical GPIO timing or playback of the invalid descriptor.

For Rack, ordinary-length buffer margins below are insufficient for this state.
Keep all physical Array accesses bounded and define an explicit invalid-descriptor
policy; that policy is a host safety decision, not an observed firmware clamp.
The reader arithmetic and next save target are checked below. Actual values or
faults at escaped hardware addresses, complete mode-entry processing in this
state, and saving an oversized descriptor after a later dirty-marking action
remain unresolved.

## 10. Array addressing and exact ramp deltas

### Raw reader arithmetic for oversized descriptors

`array_reader_addresses()` is a diagnostic, non-dereferencing definition for
both reader address prefixes, including the oversized descriptors produced by
the clock/stop sequence. It returns source byte addresses in interpolation
order (linear two sources; planar 00/01/10/11), float32 fractions, planar grid
size, and signed dimension product. It does not enlarge the safe storage
adapters' accepted 0..1024-frame domain.

Both readers multiply **dimension1 by dimension2 modulo 2^32** and interpret
the result as signed for comparisons. They do not clamp this product to slot
capacity. Subsequent shifts, additions and subtractions also wrap at 32 bits;
the final address is `uint32(bank + 4*(descriptorOffset + coefficientOffset))`.
Use unsigned operations to implement wrap explicitly in a C++ translation,
with signed interpretation only at the corresponding comparisons.

The linear reader forms `float32(int32(n-1))`, uses one fused multiply-add with
Slide and the signed scan offset, and truncates to an integer frame index.
It compares each of index/index+1 with signed n before subtracting the wrapped
`n<<6` coefficient span once. Its interpolation fraction uses the float32
position minus its float32 truncation, so huge positions can have no fractional
resolution even when Slide is fractional.

The planar reader searches integer grid sizes until a signed square reaches n
when n>0; for n<=0 it uses grid 2 and unscaled controls. With the positive
products tested here, this is `ceil(sqrt(n))`. For n=536,862,856 it executes
23,171 grid iterations, versus at most 32 for ordinary full-slot descriptors.
Coordinate arithmetic follows the ordinary planar rule, but its wrap comparison
is performed on **signed coefficient offsets**, against `int32(n<<6)`, rather
than comparing unbounded frame indices to n. That distinction is essential
when the span overflows. The diagnostic helper excludes positive n above
46340 squared, where the grid search's signed-square overflow needs separate
analysis, and excludes saturated linear float-to-int conversions.

For the 17-frame stop example, dimension1=67,107,857 and the retained second
dimension is 8: n=536,862,856 and the coefficient span is `0xfff82200`, signed
-515,584. With A's initialized bank `0x60c01000`, slot-1 offset 65,536 and scan
offset zero, original instructions produce these source byte addresses:

| Reader | Slide / Focus | Sources in interpolation order |
|---|---|---|
| Linear | 0 / 0 | `0x60c41000`, `0x60c41100` |
| Linear | 1 / 1 | `0x60a49000`, `0x60a49100` |
| Planar | 0 / 0 | `0x60e38800`, `0x60e38900`, `0x613e0b00`, `0x613e0c00` |
| Planar | 1 / 1 | `0x61429000`, `0x61429100`, `0x61988d00`, `0x61988e00` |

These can escape the ordinary A bank `[0x60c01000,0x61001000)`, including below
its beginning. The test matrix contains both sides and both readers, dimensions
0-by-1/8-by-8/1024-by-1 and both reached 17/54-frame underflow descriptors,
five Slide/Focus pairs, and scan offsets 0/1023: **200 cases, 80 oversized**.
Each compares the original prefix's pointer registers and fractions, then maps
only explicit source pages and checks all source reads and all 64 coefficient
deltas through return. That supplied sparse memory is a test fixture, not an
assertion that these addresses are backed by RAM or contain those values on the
device. The read order remains linear 0/1 and planar 00/10/01/11 per coefficient.

The two persistent capture sequences also continue directly into twelve reader
prefixes using their actual resulting descriptors, stopping before any source
read. Separate continuations run the original save scanner until file open:
the dirty **original slot 0** is selected as `speca000.wav` / `specb000.wav`,
and its descriptor is still 1024-by-1. The oversized selected slot 1 retains
marker 1. Thus the next save request does not automatically target or repair
the oversized selected descriptor. No file operation is executed by these two
continuations.

### Linear reader

`linear_deltas()` executes the compiled interpolation, Focus transform and
delta schedule. Both readers at `0x0802cd0c` / `0x0802ce5c` are tested through
return in 432 cases: lengths 1, 2, 7, 64, 65 and 1024; Slide 0, .37 and 1;
clock offsets 0 and n-1; Focus 0, .5, 1 and the float32 one-third value with
its immediate neighbors. Arrays contain positive and negative coefficients;
the current working bank is independently nonzero. Every source read and all
64 output deltas are checked.

For n frames, position is `FMA(float32(n-1), Slide, float32(clockOffset))`.
Truncate for the first frame index and subtract that integer for the fraction.
The second index is first+1. Each index >=n subtracts n **once**. A single-frame
Array therefore reads its same frame twice. The helper rejects accesses outside
the logical Array and excludes empty descriptors; it does not generalize the
firmware's single subtraction into modulo arithmetic.

Each coefficient interpolates with `value=FMA(b-a, fraction, a)`, where b-a
rounds to float32 first. Set `k=float32(.75*Focus)` and
`gain=float32(1.25-k)`. If value<=0 or **k==.25 exactly**, delta is
`float32(float32(value-current)/64)`. Otherwise:

```text
exponent = focus_exponent_1024[min(trunc(Focus*1024), 1023)]
integer = signed32(bits(value) + 0xc0876c0b)   // wrapping integer addition
encoded = trunc(FMA(exponent, float32(integer), float32(1064866816)))
approximation = floatFromBits(encoded)
delta = float32(FMA(gain, approximation, -current) / 64)
```

This is the firmware's fast bit-domain power approximation. The Focus bypass
uses the rounded product, not a tolerance around one-third. The existing
`focus_transform()` / `linear_read()` helpers return rounded target values;
subtracting current from those targets introduces an extra rounding and is
not the exact delta schedule. Use `linear_deltas()` for equivalent ramp setup.

### Planar reader

Code: `planar_coordinates()` and `planar_deltas()`. Both complete readers at
`0x0802cfac` and `0x0802d0f8` were executed through return with memory-read
tracing: 256 cases, dimensions with products 0..1024, endpoints, fractional
positions and nonzero clock offsets. Tests independently check all 256 float
reads per call, the stored grid size and valid-Array output deltas.

For positive frame count n, let `g=ceil(sqrt(n))`, `h=g-1`,
`x=float32(float32(Focus*h)+clockOffset)` and `y=float32(Slide*h)`.
Unlike the linear reader, x uses separate multiply/add, not FMA. Let xi/yi
truncate x/y and tx/ty be their fractional parts. Row 0 is yi unless yi>=g,
then yi-h. Row 1 is yi+1 unless yi>=h, then yi+1-h. The four flattened indices
are `row0*g+xi`, that value+1, `row1*g+xi`, that value+1. Subtract n **once**
from each index >=n. The reader does not validate the result against n.

For each coefficient, using rounded float32 subtractions and fused operations:

```text
low  = FMA(b-a, tx, a)
high = FMA(d-c, tx, c)
delta = float32(FMA(high-low, ty, float32(low-current)) * (1/64))
```

Subtracting current before the final FMA is significant; computing a rounded
target and then calling `ramp_delta()` can differ. The earlier `planar_read()`
remains a mathematical target-value helper, not the exact delta schedule.

Empty descriptors take a fallback with g=2, unscaled Focus/Slide and n=0;
the reader still reads external RAM and writes deltas. A single-frame Array
also reads a neighboring logical frame even though its interpolation weight
is zero. Two-frame and other short/non-square Arrays can escape logical bounds
with some endpoint/offset combinations. The 46 deliberately invalid logical
cases in the suite execute against initialized backing RAM to establish read
addresses; they do not validate those descriptors as user-reachable states.
Physical slot capacity and logical frame count are different bounds.

The joined clock-to-reader test now establishes more than an arbitrary offset
fixture. It executes an ISR and the complete UI handler for each clock, then
runs the original planar reader with Slide=Focus=1. Offsets and hold counters
are not rewritten between steps. A 64-by-1 descriptor produces 64 in-bounds
steps on each side. A 65-by-1 descriptor, a length representable by the capture
writer/stop path, cycles normally through offsets 0..64 but reads beyond its
65 logical frames at offsets **49..64**. Both sides reproduce all actual read
addresses, including indices up to 80. Thus admitted clock offsets can reach
the reader's logical-bound escapes; bounds protection cannot rely solely on
validating the initial descriptor. This remains a digital handler/reader test,
not an electrical clock or complete capture-to-playback hardware test.

### Portable storage contract for planar playback

The retained-slot tests now establish coefficient values as well as addresses.
`planar_slot_deltas()` takes the logical frame count separately from a full
**1024-by-64 float32 slot**. It applies the original coordinate/wrap arithmetic
and reads the resulting physical frames, including retained data beyond the
logical end. It validates actual source indices before reading. The earlier
`planar_deltas()` still deliberately rejects logical-bound escapes and remains
useful for checking strictly bounded data; it is not the complete storage model.

The 192 original-reader cases cover both sides, lengths 0/1/2/3/5/63/64/65/255/
257/1023/1024, four control/offset combinations and two tail histories. Changing
only the tail from zero to .875 changes deltas in 24 paired cases. All 64 deltas
match exactly, including fractional interpolation and the empty-descriptor
fallback. This also exposed and fixed input rounding in the coordinate helper:
host control values must become float32 before multiplication, matching the
firmware registers.

For **1 <= n <= 1024, normalized controls and 0 <= clockOffset < n**, every
source stays within the physical slot. This follows from the recovered index
formula, rather than claiming exhaustive firmware execution over every input:
for grid g >= 2, both row indices are at most g-1, and the integer horizontal
coordinate is at most g-1+clockOffset. A raw frame index is therefore at most
`g*g+clockOffset`. If it is below n, it is at most n-1; otherwise the reader's
single subtraction of n leaves at most `g*g-1`. Since g=ceil(sqrt(n)) <= 32,
both branches are at most 1023. The special n=1 case reaches frame 1, still
inside the slot. For n=0 with offset zero, the fallback reaches at most frame 4.
Unbounded/stale offsets are excluded from this proof and remain checked by the
adapter; an escaped physical index raises an explicit error in the reference.

The Rack storage definition is therefore to retain **all 1024 physical frames
per slot**, and keep logical dimensions separately. Capture, loading and factory
replacement overwrite their written prefixes; shrinking a descriptor does not
clear its tail. Do not add logical wrapping or permanent zero-padding, because
those change the checked output. Thirty-two initializer descriptors independently
confirm the 65536-float spacing, A offsets `slot*65536`, B offsets
`1048576+slot*65536`, dimensions 8-by-8 and marker 1. Those 32 slots require
8 MiB, but independent slot allocations are insufficient when capture leaves
a stale offset, as described below. The main routine
requests zero-fill of the larger external regions before populating them;
this does not imply that shortened slots retain zero tails after later writes.

Patch persistence must retain tail state when exact playback after a shrink is
required. A WAV containing only the logical prefix cannot preserve that entire
RAM history. Empty planar playback is covered here; the corresponding linear
behavior is defined below. Neither permits unchecked host reads.

### Capture shrink and contiguous bank storage

The capture-shrink sequence closes one part of the stale-offset gap. Keeping
offset 1023 while recording 0, 1, 2 or 65 frames leaves that same offset after
stop. All 16 such combinations across both sides and slots 0/15 read past
frame 1023 when the resulting descriptor is passed directly to the planar
reader at Slide=Focus=1. For 65 frames, sources reach frame 1039. These are
unchecked adjacent-memory reads, not a wrap into the selected slot. Slot 0
therefore reads the following slot; slot 15 reads the region after the last
configured slot. The test supplies that adjacent memory explicitly and checks
every resulting delta. The extended failure-path mode sequence in section 9
also reaches the SAO reader with unchanged Slide and the old offset intact;
subsequent manual movement cancels it. Hardware adjacent data and successful
media behavior remain unverified.

`planar_storage_deltas()` extends the reference to a contiguous physical
storage window with a descriptor origin. It keeps original arithmetic, reads
neighboring frames when present, and rejects missing physical storage. Use this
bank-level model for state transitions; the slot-only helper remains valid for
its narrower checked bounds and intentionally rejects these crossings.

For **0 <= clockOffset <= 1023** and normalized controls, storage needs can
still be bounded without arbitrary host reads. For n>=2, the earlier argument
gives at most `g*g+1023-n`, or n-1 on the branch without subtraction. Since
`g*g-n <= 2*g-2 <= 62`, the maximum is frame **1085**. The n=1 case reaches at
most 1024, and n=0 at most 1027. Thus contiguous storage for 16 slots plus
**64 trailing guard frames per side** covers this expanded planar contract
(8 MiB plus 32 KiB total). This is a formula-derived allocation bound, not an
exhaustive state-machine proof that offsets always remain <=1023. Keep the
runtime physical-index check, preserve neighboring slot contents and guard RAM,
and do not silently clear an offset merely because capture reduced the length.
Offsets beyond that contract require a larger explicit backing region or an
explicit invalid-state response; neither helper issues an unchecked host read.

### Empty linear Arrays and leading storage

The linear reader has no empty-descriptor fallback. Its signed conversion of
`n-1` produces -1 when n=0, so its position is the fused float32 expression
`clockOffset - Slide`. It truncates toward zero, retains the signed fractional
remainder, and subtracts n only once from each index that is >=n. For an empty
descriptor, that subtraction changes nothing.

With offset zero, Slide=0 reads frames 0 and 1 with fraction zero. For
0<Slide<1 it reads those same frames with fraction **-Slide**, extrapolating
rather than interpolating. At Slide=1 it reads frames **-1 and 0**, with fraction
zero: the preceding physical frame supplies the target. This is not silence
or a wrap to the last frame of the selected slot. Focus still applies its usual
transform, followed by the exact coefficient-ramp delta calculation.

`linear_coordinates()` and `linear_storage_deltas()` define this behavior.
The latter takes the descriptor origin within an explicitly supplied physical
storage window, checks absolute source indices before access, and rejects
missing leading or trailing storage. The strict logical `linear_deltas()`
helper continues to reject empty Arrays. In 960 complete original-reader
comparisons, both sides and slots 0/15 match every source address and all 64
delta words across lengths 0/1/2/65/1024, offsets 0/1/1023, four Slide values
(including the float immediately below 1), and four Focus values. Sixteen
cases check the actual preceding-frame reads. Fixtures give that frame explicit
nonzero contents; these checks do not infer its contents on hardware.

For lengths 0..1024, normalized Slide and offsets 0..1023, the linear source
range is bounded by **-1..1024**. For n>=1, position is nonnegative and at most
`n-1+1023`; the one subtraction bounds both resulting indices by 1023. For
n=0, position ranges from -1 to 1023 and the second source can reach 1024.
Together with the planar bound above, a compact bank therefore needs **one
leading frame, 16 contiguous 1024-frame slots, and 64 trailing guard frames per
side**: 8 MiB plus 32 KiB plus 512 bytes total. Interior preceding frames are
the prior slot's real tail; only the beginning of each bank needs extra leading
storage. Keep logical dimensions separate and preserve guard contents as state.
This remains a bounded storage contract, not a whole-state-machine proof of
the clock-offset range or a measurement of hardware RAM history. The concurrent
clock/stop sequence above can generate dimensions far beyond 1024, so these
margins must not be treated as sufficient for every reachable UI state.

### Complete SAO playback sequence

A 12-callback test starts from the original DSP initializer, supplies two distinct
64-frame synthetic Arrays per side, and runs the complete original audio
callback through return for 768 samples. Each callback changes the slow
Slide/Focus inputs and the combination of A/B linear flags. The sequence covers
all three interactions, both DMA halves, LF on either side, and fast pitch
changes that alter the active harmonic count. The initial mode and Array data
are explicit RAM fixtures. At zero-based callbacks 3 and 7, GPIO and hold-counter
fixtures produce opposite-Shift release gestures inside the original handler,
selecting slot 1 on A and then B. Both the live selection and settings mirror
are checked. No handler or media call is replaced.

Selection preserves the working coefficient bank. The newly selected Array
supplies the next ramp target, with deltas calculated from the carried-over
coefficients. The full callback comparisons verify this through both switches;
there is no bank reset in these tested selection paths.

The sequence also begins with pending-operation byte 1. It checks progression
to 2, recovery to 0 during the second callback, and continued processing of all
64 samples through the original busy-transport recovery path (section 9).

The reference calculates deltas from the previous working bank before the
sample loop, then advances only coefficients below the sample's active-term
cutoff. A's standard cutoff uses its **even** increment and B's uses its **odd**
increment in this compiled path. Inactive coefficients retain their prior
values, so a changing cutoff need not complete every 64-sample ramp. Full-bank
bit comparisons check this behavior, along with the exact stored deltas,
controls, phases, RNG state and all sine/Sub output words. The four spectral
lanes are compared against the independent polynomial synthesis model using
the same absolute tolerance of 0.0002 as the isolated standard tests. The
observed maximum absolute error is **0.000004380942**, with active-term counts
ranging from 4 to 60. Both are recorded in `analysis/continuation_tests.json`.

This joins slow/fast controls, both readers, persistent coefficient ramps,
standard synthesis, interaction phase routing, clipping and all eight outputs.
Long trajectories, broader combinations of selection gestures and hardware
audio comparison remain outside this particular sequence.

## 11. Saved settings format and restore behavior

Executable codec: `pack_settings()` / `unpack_settings()` in
`reference/sp67_extended.py`. Settings RAM starts at `0x2001348c`: four
halfwords (A/B mode, A/B linear flags), then eleven words (A/B auxiliary mode,
A/B engine, A/B period, A/B LF, A/B Array slot, interaction). The packed buffer
starts at `0x2001346c`; each record is eight little-endian uint32 words.

| Word | Packed value |
|---|---|
| 0 | A mode==1 at bit 16; B mode==1 at bit 0; A linear==1 at bit 20; B linear==1 at bit 4 |
| 1 / 2 | A / B auxiliary mode OR (engine << 16) |
| 3 / 4 | A / B period OR (LF << 16) |
| 5 / 6 | A / B selected Array slot |
| 7 | Raw interaction mode |

Packing starts at `0x0802d280` and is checked up to `0x0802d2fc`, before the
flash-unlock call. The disassembly inventory splits its body into
`fn_0802d284.asm`; that split is not a second logical packer. Boolean tests use
the original halfwords and compare to exactly 1. Low fields are **not masked
to 16 bits before OR**; a period above 65535 can overlap LF. Results truncate
to 32 bits. The codec deliberately adds no validation.

Decode at `0x08033312..0x08033362` reads A/B mode as four-bit fields. B linear
is also a four-bit field; A linear instead uses an arithmetic right shift of
the signed flags word by 20 and stores its low halfword. Auxiliary modes and
periods decode as unsigned low halves; engines and LF decode as signed high
halves. Slots and interaction retain all 32 bits. The 256 cases each for pack
and unpack include 128 ordinary valid configurations plus independent
arbitrary-bit coverage. Raw codec behavior does not make corrupt states valid
Rack settings: validate enums, periods and slots at the Rack serialization boundary.

Restore at `0x080332e4` scans `0x081e0000..0x081f0000` in **32-byte steps**,
stopping when a record's first word is `0xffffffff`. The cursor is stored at
`0x20002ebc`. For every occupied record it calls the real copy routine with
**256 bytes**, not 32, into `0x2001346c`. That copy overlaps the decoded settings
area and includes subsequent records; decode afterward restores the 52 settings
bytes from the last packed record. The six synthetic-flash cases use
0, 1, 2, 17, 2047 and 2048 occupied records, checking the cursor, copied bytes
and total writes. Synthetic mapped bytes beyond the region permit observation
of the final over-read; their contents are not a claim about a real board.

An immediately erased first record leaves the existing packed buffer intact
and decodes it. **This scan alone does not select defaults for an empty region.**
A completely full region takes a maintenance path at `0x08033364`; the scan
test stops before the hardware call. Separately, the default assignments at
`0x08033372..0x08033396` set both modes to 1, both periods to 500, and auxiliary,
engine, LF, slot and interaction fields to zero. They **leave both linear flags
untouched**, verified with four prior-value pairs. The following erase is not
executed by these tests.

The post-program-call tail `0x0802d30a..0x0802d31a` advances the cursor by 32
and clears dirty flag `0x20002eb8`. Twelve cases cover three cursors and return
values 0..3: the tail does not inspect that return value. This verifies only
bookkeeping, not flash programming, locking, completion or failure handling.
Rack can serialize the decoded settings directly; reproducing this flash log
and its overlapping memory copies is unnecessary for patch-state persistence.

## 12. What still blocks a faithful complete module

The remaining list is explicit in `analysis/continuation_status.json`.
[CODEX_HANDOFF.md](CODEX_HANDOFF.md) maps each implementable subsystem to its
executable definitions/tests and reconciles all 15 original behavioral questions.

* **Remaining UI gestures:** Follow/Sync enum labels now have LED-instruction
  and manufacturer-documentation support. The joined DSP path and unshifted
  button cycle are checked for all modes, including complete SAM and Noise/Chaos
  callbacks. Mode/LF, shifted Sub/CV, Array selection, linear toggles and reset
  dispatch are checked. Concurrent Array/shared edges with stable Shifts now
  have checked ordering and held-input suppression. Stable held-Shift clocks
  combined with Array edges and active capture now have checked ordering,
  including reachable oversized descriptors. Simultaneous Shift releases with
  Array edges now have checked A-first selection and capture ordering, including
  persistent button-only underflow reproduction. Shared-edge targeting/mirrors
  and held suppression are checked for the stated SAM contract. Other gesture/
  engine-state release combinations and broader long holds still need coverage. The
  calibration/display stage now has checked boot and isolated release rules;
  endpoint and pitch-table calculations are also checked. Full calibration audio
  trajectories, physical saving and other direct display writers remain separate
  gaps.
  Ordinary unshifted mode cycles now have complete audio comparisons for
  simultaneous/staggered Array presses across all three interaction modes;
  this does not close the remaining Shift/capture combinations.
* **Physical panel/CV mapping:** digital calibration defaults, loader layout,
  pitch breakpoints and control smoothing are defined in section 3. External
  CV combinations and device-specific calibration require additional evidence. Digital
  lane roles are known; jack voltage gain/polarity/filtering still needs hardware.
* **Array bounds and capture gestures:** retained contiguous-bank storage now
  reproduces checked logical and cross-slot reads after capture shrink without
  host out-of-bounds access. Capture-to-SAO entry and manual cancellation now
  agree through explicit media failure paths. Oversized capture-stop descriptors
  now have checked raw reader arithmetic and next-save dispatch, but physical
  contents/faults at escaped addresses and a Rack recovery policy remain open.
  Check successful persistence, remaining concurrent gestures and successful
  recovery transport. The pending-operation
  prefix and full SAO processing through busy recovery are now checked. The eight-callback capture sequence and
  subsequent readers are checked, as are scan continuation and clock expiry.
  Settings pack/decode and restore scanning are defined in section 11. Mode
  gestures through explicitly busy transport reach save dispatch, and Array
  save mutation/conversion have exact numerical checks; physical flash and
  successful media operations remain outside those checks.
* **Real saved Array fixture:** validate the recovered WAV header/payload against
  an untouched module-generated file.
* **Integrated audio:** run controlled sparse-spectrum, FM, Noise and Chaos
  recordings against a physical module. Bounded instruction tests are much
  stronger than algebra-only probes, but are not an audio equivalence claim.
  Resolve callback-entry FPU context for oscillator-mode detector poles: the
  new mode-cycle tests explicitly supply s22/s23, and show continued histories
  affecting later SAM operation. Their physical entry values remain unknown.

A Rack prototype can now implement all three synthesis engines, all three raw
interaction modes, internal FM, auxiliary generation, linear/planar coefficient deltas
and the capture writer from explicit definitions. Keep
the unresolved UI/calibration layer replaceable. Do not invent analog
voltages or call the prototype a complete hardware-faithful recreation yet.
