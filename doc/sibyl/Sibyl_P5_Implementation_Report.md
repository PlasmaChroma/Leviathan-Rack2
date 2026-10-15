# Sibyl P5 implementation report

Date: 2026-09-18. Baseline HEAD: `d1a02bc2b821438a5b7e723fba54bcc48516b9b8`,
branch `lumin-render`. The tracked working tree was clean at entry. Existing
untracked Samples/, WAD/, auth/, compile_commands.json, dragonking.txt, full.sh
and test-results/ were preserved. No staging, commits, installation or live Rack
changes were performed.

## Implemented

Schema-3 harmony progressions contain ordered chord markers, scientific roots and
explicit interval sets. Bindings support arrangement defaults, scene-repeat and
scene-visit clocks, looping or final-chord holding, explicit scene disabling and
restored inheritance. Harmony uses the automation score-prefix convention while
preserving the existing float scene scheduler.

Relative events support indexed chord tones, root/third/fifth roles and nearest
chord-tone selection within an inclusive register. Nearest references accept
static note, degree/octave or pitch voltage, with deterministic lower/higher
tie-breaking. Event and assignment transpose apply after selection. The compiler
rejects missing/ambiguous roles, unresolved registers, conflicting pitch forms,
invalid references and final out-of-domain pitches. Unassigned harmonic patterns
remain editable bank material with an unbound warning.

Scheduled sounding onsets determine chord selection, including microshift and
swing. Runtime lookup uses precompiled expression/chord pitches and binary search,
with no role resolution or voicing search in the audio thread. Sustains, glide
targets and ratchets retain onset pitch; subsequent events resolve independently.
Static events keep their compiled-voltage path. Compiled data remains owned by
the composition generation and follows the existing hazard/adoption discipline.

Expressions and chord definitions are interned. Compilation only adds contexts
reachable through actual scene assignments and enforces 128 progressions,
128 markers per progression, 4096 total markers, 1048576 pitch entries and a
combined 32 MiB additional compiled-storage budget with automation. Numeric
timeline precision failures reject explicitly. Capacity failure includes the
pitch-entry count; the compiler does not fall back to an audio-thread search.

New edit operations:

- `upsert_progression`, `delete_progression`
- `set_default_harmony`, `set_scene_harmony`, `inherit_scene_harmony`

They use existing atomic EDIT/VALIDATE, revision guards and undo/state codecs.
Referenced progression deletion returns `object_in_use` until earlier operations
detach its bindings. Existing note insertion/update/transposition operations now
accept harmonic events; degree transposition still requires degree events.

`progression` and `effective_context` GET views expose authored and derived data
separately. Context requires scene ID, zero-based repeat and a beat inside that
repeat. Up to 256 note projections include selected and transposed pitches, plus
authored gate/velocity/probability after scene overrides. Independent automation
values and scene offsets are reported separately, without inventing a held event
base or live macro reading. Counts/truncation and capabilities advertise bounds.
Ordinary static projections mark harmonic pitches as requiring context. The
voicing popup uses and labels current scene/time harmony.

Harmony dependencies mark affected melodic tracks, including future-scene
changes, while excluding unrelated static percussion. Pending replacement is
compared against the sounding revision. Same-harmony replacements preserve gates.

## Changed files

- New `src/SibylHarmonyTypes.hpp`, `src/SibylHarmony.hpp`: model and runtime lookup.
- New `src/SibylHarmonyJSON.hpp`, `src/SibylHarmonyEdit.hpp`,
  `src/SibylHarmonyView.hpp`: compiler, bounded tables, operations and projections.
- `src/SibylTypes.hpp`, `src/SibylJSON.cpp`, `src/SibylNoteEdit.cpp`,
  `src/SibylEdit.cpp`, `src/SibylAdoption.cpp`, `src/Sibyl.cpp`: integration,
  serialization, scheduled-onset pitch latching, dependencies and popup.
- `src/Octavia.cpp`, `MCP/mcp_server/Octavia_MCP.py`: existing bridge view/query
  forwarding for scene/time requests. No new route or MCP tool family.
- `tests/sibyl_harmony_cases.hpp`, `tests/sibyl_module_spec.cpp`,
  `tests/sibyl_codec_spec.cpp`, `tests/octavia_sibyl_contract_spec.py`, `Makefile`:
  feature coverage, updated schema gate and build dependencies.
- `MCP/skill/octavia/references/sibyl.md`: current authoring/read contract.

## Validation

Native Windows MINGW64, with `MSYSTEM=MINGW64`, `/mingw64/bin:/usr/bin` on PATH and
the installed Rack application runtime ahead of compiler runtime directories:

```sh
make -j10 plugin.dll test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 plugin.dll build/tests/sibyl_module_spec build/tests/sibyl_codec_spec build/tests/sibyl_legacy_golden_spec
export PATH="/c/Program Files/VCV/Rack2Pro:$PATH"
build/tests/sibyl_module_spec.exe
build/tests/sibyl_codec_spec.exe
build/tests/sibyl_legacy_golden_spec.exe
```

Full native `test-fast`: PASS, 109950 reported checks, zero failures, and Windows
plugin build/link PASS. Log: `test-results/p5-full-validation.log`.
The first plugin build caught a C++14 generic lambda that the C++17 harness
accepted; it was replaced with a C++11-compatible template before the passing run.

H01-H11 feature coverage passes: roots/roles, suspended/ambiguous chord rejection,
nearest ties, held and ratcheted pitch, exact/anticipated boundaries, unbound
patterns, binding inheritance, percussion-independent dependency masks and stable
canonical round trips. Additional tests exercise future-scene changes, pending
replacement, all supported test sample rates, physical-state serialization,
contextual reads, invalid expression forms, unreachable contexts and explicit
table exhaustion at 1048576 entries. Instrumented harmonic playback and adoption
paths have zero audio-thread allocations and deallocations.

The complete `doc/Sibyl_v3_Example_Composition.json` now compiles. Its relative
harmony, conditions, overrides and independent curves are all supported by P5;
P6 voicing generation is not needed to load this authored example.

Final focused verification after contextual-preview and precision refinements:
Windows plugin build, module suite and codec PASS. Legacy golden traces remain
identical at 44100, 48000 and 96000 Hz with evolution off and on. See
`test-results/p5-final-build.log`, `p5-final-module.log`,
`p5-final-codec.log`, and `p5-final-golden.log`. A final whitespace-only cleanup
removed an adoption-helper indentation warning without changing behavior.

Python bridge checks: server contract 11/11 PASS; Sibyl route contract 7/7 PASS.
MCP adapter byte-compilation and `git diff --check`: PASS.

## Remaining work

Live P5 Rack/MCP validation and musical listening have not run. The currently
running Rack was left intact. Native sanitizer runtimes were unavailable in
earlier milestone probes; no sanitizer success is claimed here. Measured CPU
comparison and the full combined edit/undo/performance integration exercise
remain P6 work. Automatic `voice_progression` materialization remains unsupported
until P6 and is not advertised as available.
