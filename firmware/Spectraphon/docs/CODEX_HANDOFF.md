# Spectraphon SP67 — current Rack implementation handoff

Updated 2026-10-04. This replaces the initial staged handoff: Noise, Chaos,
internal cross-FM, digital output lanes and ordinary capture scheduling now have
original-instruction comparisons. They are available to implement; they should
not be replaced with unrelated placeholder algorithms because the initial report
listed them as unresolved.

Read [RACK_RECONSTRUCTION.md](RACK_RECONSTRUCTION.md) as the current behavioral
contract, [continuation_status.json](../analysis/continuation_status.json) for
remaining gaps, and [continuation_tests.json](../analysis/continuation_tests.json)
for measured validation. This bundle defines and tests digital behavior; it does
not contain a Rack module or establish physical audio equivalence.

## Implementation scope and evidence

The target includes SAM analysis, linear and planar SAO, Noise, Chaos, independent/
Follow/Sync interaction, internal cross-FM, six Sub/CV modes, clocks, capture,
Array import/export, settings and the documented controls/indicators. Keep the
physical panel/CV adapter separate because volts-to-code and output electrical
gains are not established by the supplied image.

Definitions in the following table are in
[sp67_extended.py](../reference/sp67_extended.py), except the standard polynomial
probe in [sp67_reference.py](../reference/sp67_reference.py). Test names refer to
[test_arm_differential.py](../tests/test_arm_differential.py). Several complete
behaviors are specified by original-handler fixtures plus prose rather than a
standalone Python state-machine implementation; those are identified explicitly.

| Subsystem | Executable definition / integration evidence | Boundary to preserve |
|---|---|---|
| Standard oscillator | `polynomial_synthesis`; `test_standard_recurrence_and_ramps`, `test_full_nonzero_sam_callbacks_all_outputs`, `test_full_sao_callbacks_linear_and_planar` | Mathematical synthesis uses an error tolerance; do not claim every standard output word is bit-exact |
| SAM | `analysis_parameters_exact`, `analysis_references`, `sam_detector_step`; complete SAM callback tests | Independent analyzer phase, input conditioning, two cascaded filters per quadrature, 60 active terms, 64 stored coefficients |
| Noise | `noise_step`; `test_full_noise_chaos_callbacks_all_outputs`, `test_cold_noise_from_complete_initializer` | Shared Noise RNG, A/B order and cold nonfinite transient; any repair is an explicit Rack deviation |
| Chaos | `chaos_step`; `test_chaos_both_sides`, complete mixed-engine callbacks | Four oscillator records per side and instruction-scheduled state updates; no substitute generic chaotic oscillator |
| Pitch, phases, interaction and FM | `pitch_coordinate`, `b_pitch_increment`, `even_increment`, `phase_step`, `interaction_ratios`, `interaction_gate`; `test_joined_synthesis_interaction_and_outputs` | Clean sine sources use old phases; B Sync spectral drive differs from B clean/Sub rate; truncation-based negative wrap |
| Sub/CV | `auxiliary_value`, `phase_step`, `auxiliary_clock_rate`; `test_auxiliary_shapes_and_startup_mute`, `test_clock_countdown_sequences` | Separate shared Sub/CV RNG, B-before-A crossings, asymmetric default waveforms, modes 1..4 output offset |
| Controls/calibration | `default_calibration`, `partials_control`, `smoothed_control`, `calibration_ring_step`, `calibration_endpoints`, `calibration_pitch_tables` | Digital code domain is defined; saved device values and analog summing are not supplied |
| SAO readers/storage | `linear_storage_deltas`, `planar_storage_deltas`, `slide_clock_tracking`; `test_capture_shrink_preserves_scan_offset`, complete SAO tests | Retained neighboring storage, single subtraction, empty/oversized descriptor boundaries; avoid per-slot automatic modulo |
| Raw overflow diagnosis | `array_reader_addresses`; `test_raw_reader_addresses_after_capture_underflow` | Returns wrapped addresses only; never use them as unchecked host pointers |
| Composed mode transitions | `test_full_button_mode_cycles_preserve_dsp_state`; reconstruction section 2 | Actual button edges through all engines; retained analyzer rates, continued detectors and working-bank updates; explicit callback-entry pole registers |
| Callback registration/dispatch | `test_receive_registration_dma_dispatch_preserves_fp_context`, `test_receive_start_success_and_audio_dispatch`; reconstruction section 2 | Original receive setup, successful software DMA start and circular DMA1 half/full dispatch preserve incoming floating-point registers; physical transfers and exception context are not modeled |
| Capture, buttons and clocks | Section 9 and original-handler tests, including `test_capture_gestures_audio_clock_and_planar_playback`, `test_concurrent_clocks_array_buttons_and_capture_tail` | No complete authored UI state-machine helper yet; port the documented order and preserve its tested contracts |
| Indicators | `indicator_register_values`, `tuning_beacon_action`; full indicator-routine tests | Register duty/pin values are checked; physical colors/brightness and all other direct writers are not |
| Digital outputs/startup | `clip_a`, `clip_b`; `test_clipping_and_output_lanes`, complete callbacks, `test_disabled_audio_callbacks_preserve_dsp_but_continue_capture` | Eight independent lanes, per-side rounding, mute ordering, disabled DSP with continued capture |
| Files/settings | `array_save_frame`, `array_decode_sample_buffer`, `array_load_count`, `pack_settings`, `unpack_settings`; complete parser/import/save fixture tests | The bounded tool intentionally rejects unsafe files; explicit file substitutions do not establish real media or flash success |

## Runtime structure and callback order

Use a fixed 48 kHz internal core and a 64-sample control/capture cadence for the
first comparison implementation. Resampling to Rack's host rate is an adapter
decision. Changing only pitch divisors at another sample rate would leave the
filter, smoothing, capture, UI and timeout timebases wrong.

Retain per-side analyzer histories/phases, SAM amplitudes, SAO working and delta
banks, odd/even/clean/Sub phases, envelope, Noise and Chaos states, selected
Array and scan offset, mode/engine/LF flags, capture cursor/peak/policy/countdown,
and UI counters/snapshots. Preserve the two independent shared RNG streams.
Follow the RAM initialization checked by the initializer tests; do not reset
engine state on a mode change unless the actual transition performs that reset.
The button-driven cycles now check this through complete audio callbacks.
Analyzer phases and histories continue outside SAM; oscillator detector poles
depend on callback-entry s22/s23 in the checked path, not necessarily the prior
SAM poles. Standard synthesis also advances the retained SAO working bank while
SAM reads analyzer magnitudes for sound. Preserve these separate states and
declare the unresolved physical entry-register dependency explicitly.
The checked receive-registration and DMA dispatch wrappers preserve all 32
scalar floating-point registers through audio entry. They supply no default
pole values; remaining context evidence must come from before that boundary.

The callback outline is defined in reconstruction sections 2 and 9:

1. Process UI and clocks, pending-operation recovery, indicators, mode comparison
   and button pulse countdowns in their recovered order.
2. If audio is enabled, process slow controls and reader setup, then 64 samples.
   Per sample, update fast controls/pitch; evaluate both old clean-sine FM
   sources; evaluate references and detectors; synthesize A then B; apply B's
   interaction envelope; update phases and B-then-A Sub/CV crossings; apply
   offsets/mute/clipping/conversion and store all lanes.
3. Run eligible capture writers after the sample loop. When audio is disabled,
   the firmware still processes UI/capture and leaves old outputs and analyzer
   spectra intact. A disabled callback can therefore capture stale spectra.

Use the complete-callback tests as the integration oracle. Their warm state,
synthetic ADC/GPIO inputs, busy transport and finite trajectories are explicit.
Injected engine selection in those tests does not prove every UI gesture that
could reach that state. Preserve the actual float32/FMA schedule where an exact
comparison is claimed; choose tolerances only where the existing oracle does.

## Array storage and host boundaries

Represent each side as contiguous retained physical frames plus separate slot
metadata. A vector containing only each slot's logical frames cannot reproduce
checked cross-slot reads after capture shrink. For ordinary lengths 0..1024,
normalized controls and scan offsets 0..1023, the combined storage contract needs
one leading frame, 16 contiguous 1024-frame slots and 64 trailing frames per side
(8 MiB plus 32 KiB plus 512 bytes across both sides). Initial guard contents are
not recovered hardware history; their initialization must be declared.

Those margins are conditional, not a proof that every UI state stays in bounds.
Clock-driven selection during capture followed by stop can produce an enormous
unsigned dimension. The raw readers multiply both dimensions and wrap address
arithmetic; tested addresses can leave the entire ordinary bank. Validate every
physical access and bounded import before dereferencing. Keep descriptor
metadata distinct from host allocation size. Rejecting an invalid descriptor,
holding the previous spectrum or another recovery policy is a Rack design
choice, not a recovered firmware clamp; choose and document it explicitly.

Do not allocate, parse files, block on locks or execute grid-search loops driven
by corrupt dimensions in the audio thread. Prepare bounded Array snapshots
outside the audio path and publish through the repository's existing safe
handoff pattern. Account for the recovered save-time in-place normalization:
serializing a detached copy without applying its corresponding live-state
mutation changes subsequent playback. Hardware flash logging itself is not
needed for Rack patch persistence; serialize validated decoded settings.

Keep separately declared host policies for nonfinite startup samples, missing
hardware calibration/voltage scaling, resampling and malformed files/settings.
These policies must not silently redefine the firmware equations or make tests
pass by replacing reached values with more convenient defaults.

## Status of the original open questions

[open_questions.json](../analysis/open_questions.json) contains 15 questions from
the initial pass and remains a historical inventory. This table reconciles all
15 against current evidence. The 19 unresolved CFG issues mentioned elsewhere
are a different inventory, not 19 open behavioral questions.

| Original question | Current evidence | Still needed |
|---|---|---|
| Real saved WAV header/payload | Exact emitted bytes, header anomaly, reload and write-fault behavior checked with explicit file fixtures (§9) | An untouched hardware-saved WAV and actual media acceptance |
| Complete phase/FM/even routing | Authored phase/rate equations plus joined and complete callbacks (§§3–4) | Physical external-CV summing and broader extreme trajectories |
| Eight lanes to jacks/gains | Every digital lane identity, scale and ordinary conversion checked (§8) | Electrical gain/polarity/filtering and physical converter wiring |
| Capture scheduling/count/timeout | Start/free/clocked/stop, 1024-frame capacity, countdown and >6000 expiry checked (§9) | Remaining event combinations and successful media/transport joins |
| Calibration/panel mapping | Defaults, saved loader, twelve histories, endpoints, pitch tables and UI calibration lifecycle checked (§§3–4) | Device-specific saved calibration and volts-to-code mapping |
| Recurrence/FMA verification | Original standard slices and complete SAM/SAO callbacks compared with stated tolerances (§§1–2) | Broader trajectories and physical audio comparison; do not relabel tolerance as bit equality |
| Complete Noise equations | Both sides, shared RNG, filters, mixed-engine callbacks and cold startup checked (§6) | Physical audio equivalence and longer/extreme trajectories |
| Chaos update order | Ordered reference, both sides and mixed-engine callbacks checked (§5) | Long trajectory/physical comparison, not a new algorithm translation |
| Planar logical edges | Logical escapes, cross-slot retained storage and oversized wrapped addresses established (§10) | Hardware contents/faults at escaped addresses, remaining offset/state invariants |
| Buttons/long presses/persistence | Mode, LF, Sub/CV, selection/reset, concurrent subsets, settings codec/scan and file operations checked (§§9–11) | Complete composed state machine, remaining simultaneous releases/long holds and successful hardware persistence |
| SD transport/media failures | File-level failure fixtures and real busy-driver returns checked (§9) | Actual filesystem/disk transport, electrical transfer success and media behavior |
| MCU/memory/converter hardware | Firmware addresses/configuration request bytes and software buffer roles known (§2) | Physical board identification and populated memory map |
| Bootloader update/authentication | Lower flash is absent from supplied image | Separate bootloader evidence; not required to run the independent Rack core |
| Cross-version differences | SP67 is the only analyzed image | Another genuine firmware version |
| Trailing zeros | Exact bytes and hash preserved | Linker/version evidence for their purpose |

## Implementation acceptance and remaining analysis

A complete first digital port should exercise all recovered engines, interaction
modes, Sub/CV choices, ordinary capture/clock behavior, both Array readers and
serialization through one stateful integration layer. No corresponding C++/Rack
implementation has been created in this analysis task. Port the existing
reference comparisons before claiming that such an implementation is complete.

Acceptance requires the 64-sample cadence at a declared rate; initialized and
persistent states; correct shared RNG ordering; all eight lanes; exact state
comparisons where available and measured standard-synthesis error; retained
Array storage with bounded accesses; and explicit handling of unrecovered host
boundaries. Patch persistence must retain the chosen compatibility policies.
Do not weaken acceptance to SAM/SAO-only because the original handoff proposed
that smaller starting point.

Ordinary unshifted mode-entry gestures now run through complete audio in all
three interaction modes. Simultaneous Shift releases with Array/shared edges now have
3,200 checked combinations, 1,600 held-input callbacks and two actual press/hold sequences: A-first release
can select B's next slot before capture stop, independently reproducing unsigned
length underflow without a clock. Retained counters can make those Array edges
shifted even with both Shift inputs low. See reconstruction section 9 and
`test_simultaneous_shift_releases_with_array_edges` /
`test_persistent_simultaneous_shift_release_capture` for the bounded contract.
The consumed-gesture lifecycle also has 32 persistent sequences: after dual
capture starts, any panel input held high retains both gesture latches; an
all-low callback clears their counters and re-arms unshifted shared-button
interaction changes. Capture itself remains active. See
`test_consumed_gesture_release_and_rearm`; mixed initial latch states and
pending clocks are outside that contract.
The most useful next analysis is callback-entry FPU
context, other prior-gesture/engine-state Shift releases and long-hold/capture transitions, and
successful persistence/transport continuations where the binary permits a
bounded model. Further work on electrical calibration or escaped physical
memory needs additional evidence; extra ordinary arithmetic tests cannot prove
those hardware properties.

For hardware validation, obtain an untouched saved Array; sparse-spectrum and
SAM tone/amplitude-step recordings; controlled internal/external FM comparisons;
clocked capture sequences; and calibrated jack voltage measurements. None of
those physical experiments is claimed by the emulator or synthetic file suite.

## Reproduction

See [TOOLS_AND_REPRODUCTION.md](TOOLS_AND_REPRODUCTION.md) for dependencies and
commands. The current report records 88 original-instruction test groups and
31 host tests, with 9,577 distinct instruction addresses. This is measured
coverage of the named contracts, not complete firmware coverage. The separate
continuation manifest protects current artifacts; initial combined reports and
ZIP remain historical snapshots.
