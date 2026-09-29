# TEMPI Codex handoff

Start with **TEMPI_VCV_RACK_CODEX_SPEC.md**. It is the implementation contract. The `reference/` folder contains the earlier evidence, not an alternate set of implementation defaults.

## Contents

- `TEMPI_VCV_RACK_CODEX_SPEC.md`: full source architecture, arithmetic, timing, controls, memory, policies, Rack integration, acceptance tests, and staged implementation plan.
- `reference/`: original consolidated analysis, evidence matrix, selected technical notes, and the supplied Python numerical models.
- `fixtures/`: unchanged recovered analysis fixtures, a complete generated module-data example, and explicitly labeled software-policy examples.
- `tools/check_handoff.py`: standard-library-only integrity and reference-consistency check.
- `provenance/original_harness_rerun.log`: the successful fresh source-harness rerun performed while preparing this handoff.
- `PACKAGE_SHA256SUMS.txt`: packaged file checksums.
- `VALIDATION_NOTES.md`: exactly what was and was not tested, plus an optional C++ arithmetic-snippet smoke check.

## Start

```bash
python3 tools/check_handoff.py
```

Give Codex the main specification, this folder, and the actual Leviathan/Rack workspace. Ask it to implement the specification in that workspace, beginning with Phase 0 and keeping the tests passing. The document does not assume an uninspected branch or widget API.

The handoff checker verifies reference consistency and packaging. It does not compile a Rack module, exercise a future C++ implementation, or validate physical hardware. The original PIC harness's successful offline comparisons also do not establish full-device equivalence. The timing_validation.json file contains the original reported timing-test results; a complete fresh timing-harness run was not finished during this handoff.

## Important corrections and boundaries

The factory enable mask is `0x3F`. The older memory note's `0xFF` prose is stale. F4 data below 64 copies to editable memory; all data 64..127 select the persistent-store path. The main specification defines a complete Rack-side Store All policy for that path.

Human quantization, the physical-to-virtual timebase, parts of timing commit, source arbitration, and some gesture mappings remain explicitly named software policies. Their test vectors are not disguised firmware facts. The default payload's 32 kHz virtual timebase is a software choice.

The `returned_u16` column in the original random CSV is a historical column name; its tested returned value is the selected eight-bit PRNG output. Preserve fixture provenance rather than silently renaming source columns.

No firmware, update WAV, synthetic EEPROM binary, manufacturer panel art, or font file is included. The full original archive remains useful for further instruction-level analysis but is not needed at runtime.
