# arbhar reverse-engineering bundle

**Initial firmware-to-Rack research handoff · October 5, 2026**

Analyzed `arbhar_updater_2-13.gz` supplied by the user, associated with release 2.1.3. The package exposes a Pure Data application graph, ARM/Linux DSP externals, configuration, hardware notes, and an unverified C source snapshot. This bundle contains the evidence and selected native arithmetic reconstructions. **It is not a completed Rack module or a hardware-validated emulation.**

## Start here

| Artifact | Purpose |
|---|---|
| [Main report](ARBHAR_REVERSE_ENGINEERING.md) · [HTML](ARBHAR_REVERSE_ENGINEERING.html) | Platform, DSP, memory, controls, capture, effects, version caveats, and precise evidence boundaries. |
| [Rack implementation handoff](RACK_IMPLEMENTATION_HANDOFF.md) | Native architecture, control/event ownership, sample-rate policy, implementation phases, and instructions for Codex. |
| [Parameter and I/O bible](PARAMETER_BIBLE.md) · [JSON](tables/parameter_bible.json) | 20 control roles, 19 port roles, 48 keys found across parsed configuration variants, exact source-specific defaults, and unresolved laws. |
| [State machines](STATE_MACHINES.md) | Capture, grain lifecycle, Follow, gestures, onset, and persistence; recovered facts versus proposed structure. |
| [Validation plan](VALIDATION_PLAN.md) | Existing tests, required fixtures, oracle limitations, and fidelity gates. |
| [Reference kernels](reference/README.md) | Tested C++17 arithmetic and tables, CMake build, restricted-probe scope, and safety adaptations. |
| [Open questions](tables/open_questions.csv) | Fourteen prioritized issues with function/patch entry points and next actions. |

The forty-eight configuration keys include archived/experimental variants; they are **not forty-eight confirmed active stock user settings**. `null` in the JSON means absent or unproven, not inferred zero.

## Important findings

The player contains 82 storage records per instance, **not proven 82-note audible polyphony**. Six stereo layer pairs allocate 624,000 samples per channel. The window bank is reconstructed as 101 × 515 floats; the rational soft clipper is not `std::tanh`. Normal onset detection uses the connected `bonk~` path, not the bundled FFT tester. Source sketches and debug-data presence must not be mistaken for a complete recovered application source tree.

The two most important source conflicts are the 44-grain design sketch versus compiled storage/lifecycle logic, and the unreconciled delay macro scaling. The full report keeps both open instead of silently resolving them with guesses.

## Directory layout

```text
extracted/              184 original regular files, inert permissions
  *.pd                  readable application and test patches
  *.pd_linux            ARM ELF shared objects (do not execute)
  factoryPresets/        six named preset files
  Pin_Adc_Control_Info/  source-era interface notes

evidence/
  manifest.json         input identity, every file size/hash/original mode
  elf/<binary>/         metadata, disassembly, functions, annotated literals
  main_graph.txt        indexed main patch plus edges
  feedback_graph.txt    indexed delay patch plus edges

tables/
  parameter_bible.json  typed handoff data and explicit unknowns
  presets.json          exact parsed values from 17 config/preset files
  preset_matrix.csv     named factory comparison
  pd_netlists.json      all 59 parsed top-level Pd files and subcanvases
  extracted_float_tables.json
  reconstructed_window_bank.f32
  follow_probe.csv      12,288 restricted instruction-text reference cases
  *_results.json        scope-qualified test results
  open_questions.*      priority backlog

reference/              C++17 kernels, exact float literals, tests, CMake
tools/                  inert analysis, verification, and regeneration tools
```

## Validation performed

**184** extracted files matched the manifest. **5,727** parsed connections had valid endpoint indices. A restricted instruction-text probe covered one Follow-speed routine over **12,288** cases. Optimized and ASan/UBSan native builds each passed **107,421** checks, including the Follow fixture comparisons. There was **no original firmware/Pd execution, hardware audio comparison, or complete ARM emulation**.

## Reproduce the checks

From this bundle directory:

```sh
python tools/verify_manifest.py
python tools/probe_follow.py
cmake -S reference -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/test_kernels tables/follow_probe.csv
```

To additionally check the original archive:

```sh
python tools/verify_manifest.py --archive /path/to/arbhar_updater_2-13.gz
```

For a fresh inert extraction, choose a **new** directory:

```sh
python tools/safe_extract.py /path/to/arbhar_updater_2-13.gz /tmp/arbhar-evidence-copy
```

### Rebuild derived evidence

Requirements: Python 3, GNU `readelf`, LLVM `llvm-objdump` on PATH, and `beautifulsoup4` for the included HTML form parser. Native tests require C++17 and optionally CMake. The generated report HTML requires Pandoc. No package installation or network access is performed by these tools.

```sh
python tools/extract_evidence.py
python tools/annotate_assembly.py
python tools/build_structured_data.py
python tools/build_handoff_data.py
python tools/export_reference_tables.py
python tools/probe_follow.py
python tools/view_evidence.py arbharMain.pd 0 --edges
python tools/view_evidence.py arbhar_feedback.pd 0 --edges
```

These steps inspect bytes and text; they do not run any manufacturer process. Graph regeneration includes test patches, so an object count is not an active-runtime coverage count. `annotated/` literal type guesses and PLT annotations are convenient aids tied to this linker layout, not a general ARM decompiler.

## Reading conventions

F = file fact; S = static reconstruction; P = restricted original-instruction-text probe; T = native test; D = official documentation; R = recommendation. Function addresses are virtual addresses inside the explicitly named ELF. Missing source headers and hardware-library-only DWARF prevent claims of complete original application types.

Read [NOTICE](NOTICE.md) before reusing manufacturer-derived evidence or tables in a distributed product. Keep the original files read-only during implementation. The highest-value next work is grain allocation/scheduler behavior, capture/Dub transitions, and delay macro reconciliation.
