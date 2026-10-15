# Sibyl P3 implementation report

Date: 2026-09-18. Baseline: `6a7ce84916257f9c48434a87eacf6081e2950d04`
(`Sibyl P2 expressive`), branch `lumin-render`. The user committed P2 before this
milestone; the tracked working tree was clean at entry.

Initial untracked paths: Samples/, WAD/, auth/, compile_commands.json,
dragonking.txt, full.sh, scratch_inspect_coronal.py, test-results/. Preserved.
The initial inventory is retained in test-results/p3-initial-working-tree.txt.
No staging or commits were performed.

## Implemented

Schema 3 scene assignments accept all ten specified override fields: additive
transpose; velocity, probability and gate scale/offset; and three MOD offsets.
The common codec validates finite values, integer transpose, authored bounds and
effective assigned pitch within -10 to 10 V. Authored field presence survives
serialization and partial edits, including explicit identity values and empty
override objects. Unversioned/v2 inputs with overrides fail rather than lose them.
Legacy assignments without new fields retain their existing compatibility policy.

Added strict set_scene_assignment (full object/string/null replacement) and
update_scene_assignment (existing assignment only). Partial override leaves merge;
phase/pattern changes preserve untouched fields. Unset supports phaseMode,
overrides, and named override leaves. Conflicting parent/leaf changes, malformed
fields, missing references and invalid final candidates are rejected atomically.
The legacy set_scene_track remains supported and returns the warning code
assignment_attributes_cleared when replacing an assignment carrying attributes.
The semantic response's existing string warning format includes that code in its
message; the internal EditResult retains the structured code too.

Playback caches the assignment override pointer alongside the existing route.
Evolution precedes assignment transforms, then existing macros and output-domain
clamps. Event and scene transposes add without rewriting authored pitch. Each
scene/track instance can vary a shared pattern independently. Conditions remain
an independent eligibility stage. Comparison covers every override and its
authored presence for adoption dependency detection.

Absent and identity overrides preserve legacy output. The old authored-zero-gate
path produces a minimum gate pulse; that behavior is retained and characterized
in the module harness. A nonidentity gate override yielding nonpositive duration
after the live gate macro instead forces the gate low, including ties/ratchets.
Positive live gate macro contributions retain existing duration behavior.

Capabilities advertise sceneOverrides.version 1 and both assignment operations.
No MCP wrapper or bridge schema change was needed: the existing typed fields and
opaque semantic operations already carry the new requests.

## Inspection choice

P3 uses the existing scene view for optional static effective-expression preview:
`view:"scene", id:"chorus", fields:["effectiveExpressions"]`.
Authored scene data remains separate from derived.assignmentNotes, which maps
tracks to note IDs/steps and base effective pitch, probability, velocity, gate
duration and MOD values. The expressionContext label explicitly excludes later
evolution and unsampled live macros; it does not predict eligibility or playback.
Ordinary scene/pattern reads stay compact and authored. The time-specific
effective_context view remains staged with harmony rather than advertising a
partially implemented later interface.

## Changed files

- New src/SibylOverrides.hpp: compiled override values, presence and transforms.
- New src/SibylAssignmentEdit.hpp: control-side assignment operations.
- src/SibylTypes.hpp, src/SibylJSON.cpp/.hpp: schema, strict override validation,
  round-trip persistence, assigned-pitch validation and optional static projection.
- src/SibylEdit.cpp: operation dispatch and legacy replacement warning.
- src/Sibyl.cpp: cached route, expression/gate application, capability and scene
  projection request handling.
- src/SibylAdoption.cpp: structural assignment comparison including all overrides.
- New tests/sibyl_override_cases.hpp and tests/sibyl_module_spec.cpp: regression
  coverage using the existing Rack-linked and allocation-instrumented harness.
- Makefile: incremental dependencies on new implementation/test headers.
- MCP/skill/octavia/references/sibyl.md: authoring recipes and preview semantics.
- This report.

## Verification

All build commands used native C:/msys64/usr/bin/bash.exe with MSYSTEM=MINGW64
and PATH=/mingw64/bin:/usr/bin. Rack-linked execution places
/c/Program Files/VCV/Rack2Pro before compiler runtimes.

- Focused builds and executions: sibyl_module_spec, sibyl_note_edit_spec and
  sibyl_codec_spec: PASS.
- `make -j10 plugin.dll test-fast
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: PASS, including native
  Windows plugin linking. The aggregate runner reported 109950 checks and zero
  failures; separate module suites and deterministic PHONEX ROM check also passed.
  Log: test-results/p3-native-validation.log.
- Final tightening of empty pattern IDs in new operations: rebuilt plugin.dll,
  sibyl_module_spec, sibyl_edit_spec and sibyl_note_edit_spec; all three test
  executables passed. Logs: test-results/p3-final-focused-build.log,
  p3-final-module.log, p3-final-edit.log and p3-final-note-edit.log.
- V01: shared verse/chorus pattern differs by exactly +1 V in the chorus while
  local event transpose remains additive; simultaneous tracks vary independently.
- V02: velocity .6 * 1.2 + .1 + .1 produces 9.2 V.
- V03/V05: scalar examples and running evolved-probability fixtures verify scene
  scale/offset before macro; positive overrides cannot bypass false conditions.
- V04: partial edits preserve phase and unspecified overrides; removals restore
  defaults; replacement and legacy warnings behave as specified.
- Codec/bounds/unknown-field rejection, transaction rollback, unchanged pattern
  data/IDs, every override's change mask, static projection, preview nonpublication,
  pending revision semantics and patch-state restoration: PASS.
- Zero/negative gate silence, identity legacy zero-gate behavior, live gate macro,
  ties, ratchets, and all MOD lane offsets/clamping: PASS.
- Identity overrides match absent overrides sample-for-sample through evolution.
  Frozen legacy hashes remain exact at 44.1/48/96 kHz, probability evolution off/on.
- Audio-thread allocations and deallocations stay zero in P3 playback, scene
  transition, adoption and restored-state fixtures.
- git diff --check: PASS.

## Remaining scope

P4 independent MOD automation is next in the default sequence; P5 harmony and P6
voicing/final hardening remain deferred. The complete v3 example still needs those
features. No live Rack or listening test was performed for P3. The DLL is built
locally but not installed into running Rack; no patch save occurred. Native Rack
UI undo/redo remains untested, as previously recorded.

No CPU percentile benchmark or sanitizer pass was added in this milestone.
Allocation and waveform checks are not a CPU-performance claim. Native sanitizer
libraries remain unavailable according to the earlier probes; final performance
and broader cross-feature stress work remains P6 scope.
