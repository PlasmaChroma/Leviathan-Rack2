# Tools, artifact map and reproduction

## 2026-10-04 continuation

The current DSP definition is `docs/RACK_RECONSTRUCTION.md`. New files:

* `reference/sp67_extended.py`: float32/FMA translations for Noise, Chaos,
  phase/cross-FM, interaction modes, planar Array deltas and auxiliary outputs.
* `reference/sp67_arm.py`: hash-checked, bounded original-instruction execution.
* `reference/sp67_file_fixture.py`: explicit in-memory file-operation substitutions
  for connected original parser/loader/save tests; substituted functions are
  excluded from instruction coverage. This does not emulate FatFs or media.
* `tests/test_arm_differential.py`: original-instruction test groups, exact state
  comparisons except explicitly tolerance-based standard-synthesis comparisons.
  The current group count is recorded in `analysis/continuation_tests.json`.
* `analysis/continuation_tests.json`: counts, error and executed addresses.
* `analysis/continuation_status.json`: resolved contracts and remaining work.
* `docs/CODEX_HANDOFF.md`: current subsystem-to-definition/test map, runtime and
  storage requirements, host-policy boundaries, and reconciliation of all 15
  original behavioral questions. The original `analysis/open_questions.json`
  remains historical; it is not the current list of unsolved digital paths.
* `SHA256SUMS.continuation.txt`: current bundle manifest; the original manifest
  remains a record of the initial delivery.

Run from the bundle root, preferably in a separate Python environment:

```sh
python -m pip install -r requirements-analysis.txt
python tests/test_reference.py
python tests/test_arm_differential.py --report analysis/continuation_tests.json
python scripts/continuation_manifest.py --check
```

The differential suite requires Unicorn 2.1.4 and fails if it is absent; it
does not silently skip. No Capstone, Java, Ghidra, Rack runtime or compiler is
required. Float32/float64 FMA use the host C library (`libm` or Windows UCRT).
The M7 versus MAX CPU-profile limitation and exact scope are described in the
specification. Firmware, tables and original ELF remain unchanged. No full
Rack plugin build was needed for this analysis-only change.

If intentionally changing the bundle, inspect the changes and regenerate its
current manifest with `python scripts/continuation_manifest.py`. The original
combined HTML/Markdown and ZIP are historical snapshots and are not regenerated
by this continuation.

## Quick start

From the extracted bundle root:

```bash
python scripts/export_assets.py
python tests/test_reference.py
python reference/sp67_reference.py extract-factory /tmp/factory_frames.json
```

These commands use Python 3.10+ and the standard library. Some numerical tests need a host C library providing `fmaf`, normally available through libm on Linux. They do not execute ARM instructions.

For disassembly in an environment with LLVM's ARM target support:

```bash
python scripts/inspect_firmware.py 0x08030cd4 0x08030fd2
python scripts/build_inventory.py
```

The script currently loads `/usr/lib/x86_64-linux-gnu/libLLVM-19.so`. Adjust that explicit path for another installation. No LLVM binary or font files are bundled. The renderer used for the HTML edition needs the separately installed Python `mistune` package; the Markdown reports remain directly readable without it.

## Artifact map

| Path | Contents / caveat |
|---|---|
| `README.md` | Starting point and scope |
| `docs/REPORT.md` | Historical initial integrated analysis; use the continuation for current coverage |
| `docs/DSP_SPEC.md` | Initial equations plus pointers to current instruction-checked definitions |
| `docs/ARRAY_FORMAT.md` | Memory descriptors, RIFF format, save/read behavior |
| `docs/CONTROL_IO_MAP.md` | Interface semantics and partially recovered routing |
| `docs/CODEX_HANDOFF.md` | Current implementation contract, evidence map and original-question reconciliation |
| `docs/SOURCES.md` | External sources, provenance and limits |
| `Spectraphon_SP67_Analysis.html` | Combined report, readable offline |
| `firmware/sp67.bin` | Exact copy of uploaded bytes |
| `firmware/sp67_analysis_mapped.elf` | Synthetic address-mapped analysis wrapper |
| `tables/*.bin` | Exact extracted little-endian float32 arrays |
| `tables/*.csv` | Addressed values and raw float bits |
| `tables/factory_spectra_frames.csv` | Factory bank in 64-coefficient rows |
| `analysis/identity.json` | Size/hash/startup identity |
| `analysis/symbols_curated.csv` | Analyst-assigned names with confidence |
| `analysis/dsp_regions.csv` | Important blocks inside the large DSP function |
| `analysis/memory_map_curated.csv` | Working RAM and external-memory map |
| `analysis/findings.json` | Structured findings and confidence |
| `analysis/open_questions.json` | Historical 15-question inventory; reconciled in the current handoff |
| `analysis/vector_table.csv` | All 166 vector words |
| `analysis/function_candidates.csv` | Heuristic entry/size/FP-count inventory |
| `analysis/call_graph.csv` | Direct and boundary edges; not a complete indirect call graph |
| `analysis/literal_xrefs.csv` | PC-relative literal references |
| `analysis/cfg_issues.csv` | Unresolved branches/switches/decode concerns |
| `analysis/strings.*` | Raw printable strings, including false positives from code/data |
| `analysis/entropy_4k.csv` | Byte entropy and zero fraction in 4 KiB windows |
| `analysis/numerical_metrics.json` | Helper-derived curve/error measurements |
| `analysis/test_results.json` | Host-test results and explicit scope |
| `disassembly/fn_*.asm` | Candidate-function disassembly, not original source |
| `disassembly/reachable_and_candidate_code.asm` | Combined candidate code |
| `disassembly/linear_sweep_UNVERIFIED.asm` | Includes inline data misread as code; not authoritative |
| `reference/sp67_reference.py` | Bounded WAV tool and DSP probes |
| `tests/test_reference.py` | 31 host-side consistency tests |
| `SHA256SUMS.txt` | Integrity manifest for delivered files |

## Importing into a reverse-engineering tool

The synthetic ELF is ELF32 little-endian ARM with a load segment at `0x08020000` and Thumb reset entry `0x08036431`. Its `.firmware` section contains every uploaded byte, including vector table, code, literal pools, numerical tables and terminal zeros. It is intentionally **not split into authoritative original linker sections**.

Let the tool read the ELF mapping. Confirm little-endian ARM/Cortex-M interpretation and Thumb context at the actual executable entries. Use the reset handler and vector targets as trusted entry points; do not disassemble the entire unsplit section as though it were all code. Apply the table extents from the manifest as data definitions. Add RAM blocks separately where required for analysis; the ELF load segment is not a dump of runtime RAM.

The ELF has analyst-created function/object symbols. They are useful hints, not recovered debug symbols. No Ghidra/IDA decompiler run was completed in this environment, so no original C output or validated Ghidra project is claimed. The ELF header and payload were checked with `readelf` and the host tests.

Never flash the analysis ELF. It is a research wrapper, not a vendor update file.

## Inventory limitations

The inventory combines vector roots, direct-call targets, pointer candidates and aligned prologue heuristics. Its control-flow walker follows common conditional branches and a subset of compiler switch-table patterns. Indirect branches, unusual entry points and function boundaries remain uncertain. The explicit issue CSV should guide subsequent decompiler work.

A manual correction removes a false boundary at `0x080336f4`: the WAV initializer actually begins at `0x080336f0`, before its push instruction. This illustrates why the inventory's candidate count must not be mistaken for the original function count.

## Reproduction scope

Table bytes, hashes, ELF payload mapping and script output can be reproduced
directly from the upload. The differential suite additionally executes bounded
original-instruction paths and complete callbacks with explicit RAM/register
and file fixtures, comparing exact state or stated numerical tolerances. This
establishes the reported digital contracts; analog levels, physical file
acceptance, successful peripherals and complete whole-board behavior still need
additional evidence. Keep those scopes separate when evaluating test results.
