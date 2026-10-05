# Lúbadh 2.1.0 — reverse-engineering evidence and native Rack handoff

Start with **`report/LUBADH_REVERSE_ENGINEERING.html`** (or its Markdown version). The bundle analyzes the user-supplied `lubadh-v2.1.0.tar.gz`; it is not a firmware update to install and not a finished Rack plugin.

## Read first

| Artifact | Purpose |
|---|---|
| `report/LUBADH_REVERSE_ENGINEERING.md` / `.html` | Main findings, equations, source addresses, confidence boundaries and unresolved questions. |
| `report/RACK_IMPLEMENTATION_HANDOFF.md` / `.html` | Native architecture, proposed IO contract, implementation phases and acceptance tests. |
| `report/PARAMETER_BIBLE.md` / `.html` | All 31 flattened preset fields with semantics and implementation traps. |
| `report/CONTINUATION_AUDIT.md` | Subsequent coverage audit, anti-alias probes, official manual findings and research priorities. |
| `report/TRANSPORT_FINDINGS.md` | Original-byte transport, overdub, fade, block-history and first-record tail contracts. |
| `report/LOOP_REGION_AND_ALLOCATION_FINDINGS.md` | Loop-control scaling, successive-call endpoints, engine/slot saturation and fade reclamation. |
| `report/BOUNDARY_TRANSITION_FINDINGS.md` | Ordinary-region callback transitions, outgoing/incoming head coordinates and slot-exhaustion handling. |
| `report/WRAPPED_SPLICE_AND_RENDER_FINDINGS.md` | Wrapped/physical seam transitions, fade-vector range handling and raw multihead splice rendering. |
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

For the subsequent coverage audit, newly probed anti-alias behavior, manual tail/routing findings, and the prioritized research plan, read **[`report/CONTINUATION_AUDIT.md`](report/CONTINUATION_AUDIT.md)**. Its additional 10,646 output comparisons, 24 setter assertions, and 26 dispatch cases are recorded separately in `probes/antialias_probe_results.json`; the original bundle summary and hash list retain their baseline scope.

The further **[`report/TRANSPORT_FINDINGS.md`](report/TRANSPORT_FINDINGS.md)** adds an original-byte Unicorn harness, a 6,503-comparison interpreter cross-check, 15,369 transport/write comparisons, first-record bookkeeping assertions, and block-history/scatter probes. These use initialized objects and selected call sequences; complete callback scheduling and logical loop transitions remain unvalidated. Reproduction commands and optional dependencies are in that report.

**[`report/LOOP_REGION_AND_ALLOCATION_FINDINGS.md`](report/LOOP_REGION_AND_ALLOCATION_FINDINGS.md)** extends this to original loop-region calculation and TapManager allocation/update routines. It records 18,880 region field comparisons and 5,454 allocation assertions, including saturation and held/stalled reclamation. Complete audible loop transitions remain open.

**[`report/BOUNDARY_TRANSITION_FINDINGS.md`](report/BOUNDARY_TRANSITION_FINDINGS.md)** checks 16,128 ordinary-region callback boundary configurations, including fractional replacement coordinates and direct-allocation exhaustion. Nested original fade/allocation/transfer/motion routines execute. That probe does not cover wrapped regions or downstream splice audio.

The follow-up **[`report/WRAPPED_SPLICE_AND_RENDER_FINDINGS.md`](report/WRAPPED_SPLICE_AND_RENDER_FINDINGS.md)** adds 33,696 wrapped-boundary configurations and 128 raw splice-render fixtures. It validates selected callback slices, not complete control-field publication, active-list scheduling or final module output.

**[`report/BOUNDARY_SOURCE_FINDINGS.md`](report/BOUNDARY_SOURCE_FINDINGS.md)** connects inline loop-region producers to actual callback boundary loads with 2,160 field comparisons. It verifies reset defaults and selected link/unlink pointer stores; full link-event handling and callback scheduling remain open.

**[`report/HEAD_ITERATION_FINDINGS.md`](report/HEAD_ITERATION_FINDINGS.md)** validates 144 complete per-manager boundary iterations and render-list refreshes. New heads join the refreshed render list but are not revisited during the original boundary snapshot. Full callback/audio integration remains open.

**[`report/RENDER_ITERATION_AND_RECORD_PROXIMITY.md`](report/RENDER_ITERATION_AND_RECORD_PROXIMITY.md)** executes the complete raw render traversal with 256 fixtures and 17,472 gain/audio comparisons. It recovers following-speed magnitude ramps and velocity-dependent attenuation near recording heads. Tape writes and final module output are not included.

**[`report/ENGINE_COUNT_GAIN_FINDINGS.md`](report/ENGINE_COUNT_GAIN_FINDINGS.md)** recovers the post-render engine-count gain and additive mix, with 820 blocks and 30,832 comparisons. Gain targets use approximately 0.81 per additional engine and transition over eight calls. The complete output chain remains open.

**[`report/PERSISTENT_PLAYBACK_PIPELINE.md`](report/PERSISTENT_PLAYBACK_PIPELINE.md)** joins raw rendering, output AntiAlias, engine-count gain and additive mixing in a continuous original callback slice. Four persistent sequences pass 12,864 comparisons across 96 blocks. Boundary generation, tape writes and downstream coloration are still separate.

**[`report/PERSISTENT_BOUNDARY_PIPELINE.md`](report/PERSISTENT_BOUNDARY_PIPELINE.md)** connects boundary-generated heads to that playback path and subsequent original manager updates. Eighty persistent sequences pass 202,080 numerical comparisons over 960 blocks, including repeated splices and reclamation. Actual tape writes and output coloration remain open.

Seven routines across six arithmetic groups passed 4,725 restricted original-instruction comparisons; an eighth routine, TapeAllpass, passed a separate 1,200-sample impulse comparison. **Total: 5,925 numerical comparisons.** This does not boot or validate the complete firmware, hardware, analog circuits, ARM floating-point environment, transport, or plate reverb.

The native C++ reference independently passed **8,154 invariant checks**. These are model tests, not additional hardware comparisons. Detailed limitations and tolerances are in the reports and result JSON.

## Re-run the numeric tests

Requirements: Linux-like environment, Python 3, GNU `c++filt`, and glibc `libm.so.6`. The restricted interpreter parses the included LLVM listing and supplied ELF; it is not a complete ARM emulator. No third-party Python package is required for the numeric tests.

```sh
python3 probes/test_recovered_dsp.py
python3 probes/probe_diffuser.py
python3 probes/probe_antialias.py
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

The original analysis could not retrieve the complete version 2 manual. The continuation audit subsequently downloaded it and inspected selected pages; its URL, hash, page coverage and findings are recorded in `report/CONTINUATION_AUDIT.md`. The PDF remains outside the bundle. The main report retains the original retrieval history.
