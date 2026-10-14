# Spectraphon SP67 — reverse-engineering bundle

**For the current Rack implementation definition, start with [docs/RACK_RECONSTRUCTION.md](docs/RACK_RECONSTRUCTION.md).** The combined HTML/Markdown reports and ZIP are snapshots of the initial analysis; they do not include the continuation below.

[The current implementation handoff](docs/CODEX_HANDOFF.md) maps subsystems to
reference functions and tests, defines runtime/storage boundaries, and reconciles
all 15 original behavioral questions. Use it instead of the initial report's
unresolved-path lists when planning the Rack port.

## Instruction-checked continuation (2026-10-04)

The continuation adds executable definitions for both Noise and Chaos engines,
phase/cross-FM routing, all three raw interaction modes, all six Sub/CV selections,
digital output lanes, planar Array deltas and capture scheduling/writes.
Bounded original-instruction execution checks
these against the preserved firmware. Standard synthesis is also independently
checked against its mathematical reconstruction. See
`analysis/continuation_status.json` for remaining gaps and
`analysis/continuation_tests.json` for measured coverage (9,584 instruction
addresses; 90 differential test groups, including complete SAM, SAO and Noise/Chaos
callbacks, a 12,800-sample cold Noise trajectory, clock scan sequences and
clock-to-planar bounds checks, mode/LF/CV gestures and an integrated capture
sequence, Array selection/factory reset, settings pack/restore, save-time
mutation, five-format WAV conversion, loader/header boundaries, Follow/Sync
indicator mapping, tuning-beacon windows, exact linear/planar ramp setup,
live Array switching, Slide's cancellation of clock offsets, and WAV parser
signature/format/chunk/termination boundaries, loader channel arithmetic,
joined scalar imports, short-read window behavior, initial slot descriptors and
planar playback against retained physical-slot tails, and capture-shrink
sequences with cross-slot reads into bounded contiguous storage, actual SAO
entry through persistence failures, subsequent manual Slide takeover, and
operation-byte recovery through complete SAO audio processing, and empty linear
playback with checked preceding-frame access and negative-fraction extrapolation,
complete initializer enable/DMA ordering through busy returns, disabled-audio
capture behavior, and a second cold Noise trajectory retaining the initializer's
zero sample counter and audio-enable state, plus complete WAV parsing and
parser-to-Array imports through explicitly substituted in-memory read/seek calls,
and complete save/header generation, reload, write-fault and multi-slot save
sequences through explicit file-operation substitutions, and complete indicator
routine checks for display priority, PWM words and mode/auxiliary pin behavior,
and concurrent Array/shared-button order, shifted target priority and held-input
suppression, plus calibration-stage boot choices, stable-input retention,
Shift-release transitions and separate post-save caller reset checks, twelve-channel
calibration averaging, stage 1/2 endpoint/gain equations, and stage 3..11 pitch-table
measurement/extrapolation joined to original button advances and pitch conversion,
and concurrent clocks/Array buttons/capture tails, including persistent gesture
reproduction of unsigned stop-length underflow after clock-driven Array selection,
plus signed/wrapping reader addresses and exact deltas for those oversized
descriptors with explicit sparse source memory, and joined next-save dispatch,
plus complete button-driven mode cycles with retained analyzer rates, oscillator
detector register dependence, and SAM updates of retained SAO working banks,
and original receive registration, successful software DMA start and dispatch preserving incoming floating-point
registers through the two audio half-buffer entries, plus simultaneous Shift
release/Array/shared-edge ordering, shared targeting and held-input suppression,
and persistent button-only capture-stop underflow, plus consumed-gesture
clearing and shared-button re-arming across all 32 panel input snapshots,
including consumed clock edges and subsequent asymmetric clock recovery).
`reference/sp67_ui.py` now provides reusable Array/shared action-state models,
checked against original instruction tails including raw/cached mode mismatches
and exact settings-mirror updates. The enclosing GPIO/clock/Shift state machine
remains defined by the guide and integration fixtures.
`button_actions` composes those actions in firmware order; `button_inputs` connects
the checked Array/shared edge sampler to dispatch and event clearing. The newer focused
UI checks are recorded separately in `analysis/ui_action_tests.json`; they
preserve the full-suite baseline above rather than claiming another full run.
All original 31 host tests also pass.

This bundle contains a read-only analysis of the uploaded Make Noise Spectraphon `sp67.dat`: exact binary preservation, a synthetic mapped ARM ELF, disassembly, table exports, instruction-derived DSP equations, a bounded Array/WAV inspection tool, and a staged Codex implementation handoff.

## What was recovered

The strongest findings are a quadrature harmonic SAM analyzer; two cascaded one-poles per quadrature; 64-value spectral storage with a 60-term normal processing ceiling; a 64-frame audio cadence and 48 kHz-assumed DSP math; distinct linear and planar Array readers; a Partials-controlled polynomial synthesis recurrence; a cubic compensation curve; and a coefficient-based PCM16 WAV save path with a probable header-field inconsistency.

Nine exact float32 tables are exported, including the 64 × 64 factory spectral Array. The curated map supplies 43 analyst-assigned function/data labels. The conservative code inventory has 306 candidate entries and 19 recorded unresolved issues. These are inventory statistics, not an original function count or proof of total code coverage.

## Important boundaries

No original source/debug symbols, bootloader, board schematic, hardware-generated Array file, hardware recording or second firmware revision was available. The continuation executes explicit instruction slices with Unicorn; it does not boot/emulate the whole board. No Ghidra/IDA decompiler run was performed. The supplied ELF is an analysis wrapper, not a vendor ELF or a flashable replacement. Follow/Sync labels now have LED-instruction and manufacturer-documentation support. Calibrated physical I/O, full WAV/parser admission, successful media operations and the complete UI/capture state machine remain incomplete.

The reference Python implements selected equations and file operations, not a finished Spectraphon emulator. All 31 included host tests pass; those tests do not establish audio equivalence or physical SD-card compatibility.

## File integrity

```text
Original name: sp67.dat
Length:        178932 bytes
SHA-256:       b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9
Flash base:    0x08020000
Reset vector:  0x08036431 (Thumb entry at 0x08036430)
```

`SHA256SUMS.txt` is preserved as the **initial delivery manifest**; amended reports/scripts no longer match those initial hashes. `SHA256SUMS.continuation.txt` verifies the current bundle, excluding itself and Python caches. Regenerate it with `python scripts/continuation_manifest.py`, or verify it with `--check`. The original upload is preserved unchanged as `firmware/sp67.bin`. Extracted vendor data remain distinguishable from newly authored scripts/reports; no redistribution permission is asserted.

## Read / run

```bash
python tests/test_reference.py
python -m pip install -r requirements-analysis.txt
python tests/test_arm_differential.py --report analysis/continuation_tests.json
python reference/sp67_reference.py inspect-wav /path/to/speca000.wav
python reference/sp67_reference.py extract-factory /tmp/factory_frames.json
```

For a complete file map and rebuild instructions, read `docs/TOOLS_AND_REPRODUCTION.md`. For the implementation definition, read `docs/RACK_RECONSTRUCTION.md`. The initial handoff and `analysis/open_questions.json` remain useful historical context; current resolution status is in `analysis/continuation_status.json`.

The most useful next input is an untouched Array WAV saved by a physical module. It would directly test the recovered payload model and the header inconsistency without needing a full emulator.
