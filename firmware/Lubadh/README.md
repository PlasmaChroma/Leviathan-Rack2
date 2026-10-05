# Lúbadh 2.1.0 — reverse-engineering evidence and native Rack handoff

Start with **`report/LUBADH_REVERSE_ENGINEERING.html`** (or its Markdown version). The bundle analyzes the user-supplied `lubadh-v2.1.0.tar.gz`; it is not a firmware update to install and not a finished Rack plugin.

## Read first

| Artifact | Purpose |
|---|---|
| `report/LUBADH_REVERSE_ENGINEERING.md` / `.html` | Main findings, equations, source addresses, confidence boundaries and unresolved questions. |
| `report/RACK_IMPLEMENTATION_HANDOFF.md` / `.html` | Native architecture, proposed IO contract, implementation phases and acceptance tests. |
| `report/PARAMETER_BIBLE.md` / `.html` | All 31 flattened preset fields with semantics and implementation traps. |
| `tables/factory_presets.json` | Parsed values of all ten factory presets, retaining original key spellings. |
| `tables/factory_preset_matrix.csv` / `.md` | Side-by-side preset differences. |
| `tables/parameter_bible.json` | Machine-readable parameter definitions and per-preset values. |
| `tables/recovered_constants.json` | Hashes, timebases, capacities, float bits, coefficients and topology constants. |
| `tables/reconstructed_xfade_table.csv` / `.f32le` | Reconstructed hybrid fade lookup table; host cosf, not a device-memory dump. |
| `tables/unresolved_questions.json` / `.csv` | Prioritized remaining fidelity work. |
| `native/recovered_kernels.hpp` | Portable C++17 reference equations; not a complete instrument. |
| `probes/*results.json`, `probes/*vectors.csv` | Numerical comparisons and reproducible stimuli. |
| `evidence/functions/*.asm` | 272 selected application/DSP function listings. |
| `evidence/*symbols*`, `*strings*`, `*direct_calls*`, `*disassembly*` | Full static evidence for all eight executables. |
| `extracted/` | Original package contents, kept as inert research evidence. |

## What was actually tested

Seven routines across six arithmetic groups passed 4,725 restricted original-instruction comparisons; an eighth routine, TapeAllpass, passed a separate 1,200-sample impulse comparison. **Total: 5,925 numerical comparisons.** This does not boot or validate the complete firmware, hardware, analog circuits, ARM floating-point environment, transport, or plate reverb.

The native C++ reference independently passed **8,154 invariant checks**. These are model tests, not additional hardware comparisons. Detailed limitations and tolerances are in the reports and result JSON.

## Re-run the numeric tests

Requirements: Linux-like environment, Python 3, GNU `c++filt`, and glibc `libm.so.6`. The restricted interpreter parses the included LLVM listing and supplied ELF; it is not a complete ARM emulator. No third-party Python package is required for the numeric tests.

```sh
python3 probes/test_recovered_dsp.py
python3 probes/probe_diffuser.py
```

Native reference checks require a C++17 compiler:

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Werror native/test_kernels.cpp -o /tmp/lubadh_kernel_tests
/tmp/lubadh_kernel_tests
```

The compiled development executable is deliberately not included in the ZIP. Source and the recorded result are included.

## Regenerate static evidence / tables

Evidence files are already present. To rebuild them, install an ARM-capable LLVM objdump plus GNU readelf/c++filt. Do not use an x86-only objdump and assume an empty listing means no code.

```sh
llvm-objdump -d --demangle extracted/bin/lubadh_main > evidence/main_disassembly.txt
python3 tools/inspect_firmware.py
python3 tools/annotate_asm.py
python3 tools/build_evidence.py
python3 tools/parse_presets.py
python3 tools/make_handoff_tables.py
```

`build_evidence.py` accepts an optional `LLVM_OBJDUMP` environment variable. `make_handoff_tables.py` computes the supplied archive's hash when the archive is available beside this folder; otherwise it reuses the archived hash already recorded in `recovered_constants.json`. The extracted binaries/presets are sufficient for all numeric probes.

The preset parser targets the exact supplied Hjson subset; it is not a general Hjson library. The generated fade table uses host cosine and floating arithmetic, so its provenance remains reconstruction, not original-libm bit identity.

## Safety and reuse

**Do not run anything under `extracted/bin/` or `extracted/scripts/`.** Those are original appliance binaries and system/USB/update scripts, not tools for this analysis computer. Their executable permissions have been removed in the bundle. The scripts contain system-changing operations.

Keep firmware, vendor branding and vendor assets separate from a new module's distributable implementation. Technical analysis is not a grant of redistribution rights. The native reference supplies independently expressed equations and requires further integration/validation.

The full 2.0 manual was located but not successfully retrieved/read in full. The official product/update pages and 2.1 quickstart were inspected; their role and URLs are documented in the main report. No fetched PDF is silently represented as bundled when it was not downloaded.
