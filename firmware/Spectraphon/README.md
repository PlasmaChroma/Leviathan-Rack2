# Spectraphon SP67 — reverse-engineering bundle

**Start with `Spectraphon_SP67_Analysis.html` or `docs/REPORT.md`.**

This bundle contains a read-only analysis of the uploaded Make Noise Spectraphon `sp67.dat`: exact binary preservation, a synthetic mapped ARM ELF, disassembly, table exports, instruction-derived DSP equations, a bounded Array/WAV inspection tool, and a staged Codex implementation handoff.

## What was recovered

The strongest findings are a quadrature harmonic SAM analyzer; two cascaded one-poles per quadrature; 64-value spectral storage with a 60-term normal processing ceiling; a 64-frame audio cadence and 48 kHz-assumed DSP math; distinct linear and planar Array readers; a Partials-controlled polynomial synthesis recurrence; a cubic compensation curve; and a coefficient-based PCM16 WAV save path with a probable header-field inconsistency.

Nine exact float32 tables are exported, including the 64 × 64 factory spectral Array. The curated map supplies 43 analyst-assigned function/data labels. The conservative code inventory has 306 candidate entries and 19 recorded unresolved issues. These are inventory statistics, not an original function count or proof of total code coverage.

## Important boundaries

No original source/debug symbols, bootloader, board schematic, hardware-generated Array file, hardware recording or second firmware revision was available. No ARM execution or Ghidra/IDA decompiler run was performed. The supplied ELF is an analysis wrapper, not a vendor ELF or a flashable replacement. The full Noise/Chaos algorithms, calibrated physical I/O mapping, phase/FM routing and complete UI/capture state machines remain partially reconstructed.

The reference Python implements selected equations and file operations, not a finished Spectraphon emulator. All 31 included host tests pass; those tests do not establish audio equivalence or physical SD-card compatibility.

## File integrity

```text
Original name: sp67.dat
Length:        178932 bytes
SHA-256:       b50a87b9bd4f6ad90007918a16e8a4d033e2236c863faec55f5585abe34927d9
Flash base:    0x08020000
Reset vector:  0x08036431 (Thumb entry at 0x08036430)
```

`SHA256SUMS.txt` verifies all delivered content except the checksum file itself. The original upload is preserved as `firmware/sp67.bin`. Extracted vendor data remain distinguishable from newly authored scripts/reports; no redistribution permission is asserted.

## Read / run

```bash
python tests/test_reference.py
python reference/sp67_reference.py inspect-wav /path/to/speca000.wav
python reference/sp67_reference.py extract-factory /tmp/factory_frames.json
```

For a complete file map and rebuild instructions, read `docs/TOOLS_AND_REPRODUCTION.md`. For an implementation-oriented continuation, read `docs/CODEX_HANDOFF.md`. For the highest-value remaining questions, inspect `analysis/open_questions.json`.

The most useful next input is an untouched Array WAV saved by a physical module. It would directly test the recovered payload model and the header inconsistency without needing a full emulator.
