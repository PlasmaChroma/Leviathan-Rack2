# Sibyl P2 implementation report

Baseline: `d0c810ba9b0052deafa58836e94d123ef5ca0f44`, branch `lumin-render`.
Date: 2026-09-18. This milestone continues the existing uncommitted P0/P1 and
probability-evolution work after its live Rack check.

Initial tracked modifications: MCP/mcp_server/Octavia_MCP.py,
MCP/skill/octavia/references/sibyl.md, MCP/tests/test_server_contract.py, Makefile,
src/Octavia.cpp, src/Sibyl.cpp, src/SibylAdoption.cpp, src/SibylEdit.cpp/.hpp,
src/SibylEvolution.hpp, src/SibylJSON.cpp/.hpp, src/SibylTypes.hpp,
tests/sibyl_adoption_spec.cpp, tests/sibyl_evolution_spec.cpp,
tests/sibyl_json_spec.cpp, tests/sibyl_module_spec.cpp.

Initial untracked paths: Samples/, WAD/, auth/, compile_commands.json,
doc/Sibyl_Expressive_Composition_Codex_Spec.md,
doc/Sibyl_P0_P1_Implementation_Report.md, doc/Sibyl_v3_Example_Composition.json,
doc/bifurx-octavia-listening-handoff.md, dragonking.txt, full.sh,
scratch_inspect_coronal.py, src/SibylNoteEdit.cpp/.hpp, test-results/,
tests/sibyl_codec_spec.cpp, tests/sibyl_legacy_golden_spec.cpp,
tests/sibyl_note_edit_spec.cpp. Unrelated work was preserved; nothing staged or
committed. A later working-tree inventory is in test-results/p2-working-tree.txt.

## Implemented

Schema 3 events support strict `condition.all` arrays of up to eight AND tests.
Scopes are patternPass, sceneRepeat and arrangementLoop. Tests accept integer
every/offset, first, or sceneRepeat-only last. Omitted offset is one; empty all
is the identity AND (always eligible). Unknown fields, invalid combinations and
wrong scalar types report invalid_condition. Legacy/unversioned condition input
still reports schema_version_required. Later milestone fields stay rejected.

Conditions round-trip through the common codec, full/notes reads, note updates,
inserts and duplicates; unset restores unconditional eligibility. The existing
atomic edit and VALIDATE preview paths remain in use. Condition edits participate
in note-channel adoption change detection. The existing MCP operation payloads
carry these fields without adding a protocol or Python musical semantics.

A fixed-size compiled condition and a separate timing-derived cursor implement
eligibility. Counters advance across silent and empty traversals, independently
of the legacy evolution cursor. Phase transitions, explicit restarts, preserve
adoption and timing rebases follow the P2 table. Pause/clock hold freeze counters;
randomness operations leave them alone. Automatic arrangement wraps increment a
separate ordinal, while manual scene selection does not. Stop/arrangement reset
return it to one. All new ordinals saturate at 4503599627370495 (2^52 - 1).

Anticipated events use their nominal pattern cycle, without advancing the public
cursor early. Scene conditions use scheduled-onset context with destination-scene
ordering at a boundary. Conditions gate the complete event before probability;
ratchets, macros and observations cannot bypass rejection. Conditioned tie
look-ahead is latched at the preceding onset rather than evaluated per sample.
Evolution still observes rejected events and retains its original probability
threshold/draw sequence. Status publishes conditionPasses and arrangementLoop
under the existing coherent telemetry sequence; old status numbering is unchanged.

## Files changed in P2

- New src/SibylCondition.hpp: fixed-size tests, bounded ordinal arithmetic,
  timing cursor and eligibility evaluation.
- src/SibylTypes.hpp, src/SibylJSON.cpp, src/SibylNoteEdit.cpp: event representation,
  strict parser/serializer, staged feature gate and targeted-edit support.
- src/Sibyl.cpp, src/SibylAdoption.cpp: transport/adoption counters, onset and tie
  eligibility, telemetry, capabilities and musical change comparison.
- New tests/sibyl_condition_cases.hpp; tests/sibyl_module_spec.cpp and
  tests/sibyl_note_edit_spec.cpp: Rack-linked P2 regression coverage.
- Makefile: explicit helper/test-header dependencies for incremental builds.
- MCP/skill/octavia/references/sibyl.md: authoring syntax, counter semantics,
  feature discovery and reset behavior. This report.

## Verification

Native MINGW64 environment: C:/msys64/usr/bin/bash.exe, MSYSTEM=MINGW64,
PATH=/mingw64/bin:/usr/bin. Rack-linked tests use the installed
/c/Program Files/VCV/Rack2Pro runtime ahead of compiler DLLs.

- Focused build and executions of sibyl_module_spec, sibyl_note_edit_spec and
  sibyl_codec_spec: PASS after updating the old condition-unsupported fixture.
- Final `make -j10 plugin.dll test-fast
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: PASS, including the
  scheduled-onset boundary correction. Log:
  test-results/p2-native-validation-boundary.log. The aggregate test runner
  reported 109950 checks and zero failures; the other focused test executables
  and deterministic PHONEX ROM check also passed.
- Final empty-AND identity hardening: rebuilt plugin.dll, sibyl_module_spec and
  sibyl_legacy_golden_spec; ran both native executables: PASS. Logs:
  test-results/p2-final-focused-build.log, p2-final-module.log and
  p2-final-golden.log (all under test-results/).
- New module checks cover strict grammar, round-trip, targeted set/unset,
  invalid-condition transaction rollback, saturation, AND scopes, every-fourth
  timing at 44.1/48/96 kHz within one sample, first-pass reset, final-repeat and
  single-repeat eligibility, silent/empty counting, negative microshift, false
  ties/observations, macro/ratchet rejection, destination phase modes, adoption
  rebasing, stop/non-loop ending, randomness separation and external hold.
  A sub-sample onset before an ordinary repeat boundary retains the scheduled
  earlier repeat; destination scene entry still uses destination context. Empty
  AND conditions preserve unconditional tie look-ahead semantics.
- R17 compares later eligible events with an unconditional evolution baseline;
  evolution pass, played decision and played velocity agree.
- Frozen legacy sample hashes at 44.1/48/96 kHz, probability evolution off/on:
  unchanged. Existing probability restart/persistence tests remain in the suite.
- Audio-thread allocation and deallocation counters remain zero across the new
  processing, skipped-event, scene/transport and adoption fixtures.
- Initial expanded run exposed test setup errors: an intermediate preserve edit
  already rebased phase, and Rack test inputs need their connection channels set
  directly. Corrected fixtures exercise scene transitions independently and
  assert the positive probability macro actually enables an eligible event.
- git diff --check: PASS.

## Limits and next milestone

P3 scene assignment overrides are next. P4-P6 remain deferred and unadvertised.
The complete example still requires later features. P2 was tested with Rack-linked
native executables; no P2 live Rack, listening or native Rack UI undo/redo test was
performed. The new plugin is built locally, not installed into the running Rack
process. No port/parameter IDs changed. No patch save was performed.

No new CPU-duration benchmark was recorded. The baseline evidence remains output
equivalence and allocation behavior; final cross-feature/performance stress work
belongs to P6. ASan/UBSan/TSan remain unavailable on the installed native toolchain
as established by the P0/P1 linker probes; they were not rerun or claimed passing.
