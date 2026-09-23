# Sibyl P0â€“P1 implementation report

Baseline: `d0c810ba9b0052deafa58836e94d123ef5ca0f44`, branch `lumin-render`.

Initial tracked modifications: Sibyl probability evolution in src/Sibyl.cpp,
SibylEvolution.hpp, SibylJSON.cpp, SibylTypes.hpp; tests/sibyl_adoption_spec.cpp,
sibyl_evolution_spec.cpp, sibyl_json_spec.cpp, sibyl_module_spec.cpp;
MCP/skill/octavia/references/sibyl.md. These changes are part of the baseline.
Initial untracked paths: Samples/, WAD/, auth/, compile_commands.json,
doc/Sibyl_Expressive_Composition_Codex_Spec.md,
doc/Sibyl_v3_Example_Composition.json, doc/bifurx-octavia-listening-handoff.md,
dragonking.txt, full.sh, scratch_inspect_coronal.py, test-results/. Preserved.

Before codec edits, native MINGW64 built and ran sibyl_legacy_golden_spec against
that working tree. The fixture hashes all output voltages over five seconds at
44.1, 48, and 96 kHz, with realistic increasing frame indices, both with and
without probability evolution. Frozen expected values are in the test. This is
a Rack-linked offline playback regression, not a live Rack/listening test.

## Implemented scope

P0 codec/identity foundation and P1 authoring operations are implemented. Schema
2 and bare legacy inputs normalize through the same entry point as schema 3,
portable envelopes, patch state, replacement edits, and validation. Canonical
saves/reads advertise schema 3. Unsupported later musical fields fail explicitly.
Existing probability evolution remains supported in schema 2 and 3.

Events have pattern-local stable IDs and representation-preserving semitone
offsets. Pattern allocators retain their high-water marks on ordinary upserts;
legacy full-pattern clients reuse IDs at the same step. Note movement preserves
identity; duplication allocates fresh IDs. Exhaustion fails atomically. The
INT32_MAX counter is a terminal allocation boundary: an allocation requiring an
unrepresentable next counter fails rather than wrapping.

Added update_notes, insert_notes, delete_notes, transpose_notes, rotate_notes,
and duplicate_notes; typed selectors, expected cardinality, inherited-default
resolution, explicit clamping, simultaneous movement/collision checks,
observation-copy controls, and bounded change reports. Duplication cannot
replace its own selected sources, even under collision:replace.

VALIDATE operation preview uses the same edit implementation without publication,
ID consumption, or bridge undo. Notes reads provide projections and revision/query
bound paging; static effective pitch is labeled under derived. Accepted EDIT
responses report observed active/pending revisions. Musical change masks ignore
identity-only edits but include transpose.

The agent wrappers and both legacy/generic GET routes forward typed note queries.
Musical interpretation remains in Sibyl. Existing EDIT-only bridge undo behavior
is preserved. The wrappers require a restart/reload to expose their new schemas.

## Changed files in this milestone

- src/SibylTypes.hpp, SibylJSON.hpp/.cpp: identity, transpose, version codec,
  migration, structured diagnostics, persistence serialization.
- src/SibylEdit.hpp/.cpp and new SibylNoteEdit.hpp/.cpp: note operations,
  selectors, atomic reports, bounded reads.
- src/Sibyl.cpp, SibylAdoption.cpp: request handling, preview/capabilities,
  portable/patch persistence, transpose dependency detection.
- src/Octavia.cpp, MCP/mcp_server/Octavia_MCP.py: typed query and preview transport.
- MCP/skill/octavia/references/sibyl.md: schema-3 authoring recipes and limits.
- Makefile; new tests/sibyl_codec_spec.cpp, sibyl_note_edit_spec.cpp,
  sibyl_legacy_golden_spec.cpp; extended sibyl_module_spec.cpp and
  MCP/tests/test_server_contract.py.
- This report. Earlier probability-evolution files remain modified as recorded
  above; unrelated files and rendering work were preserved.

## Verification

All make commands ran through native C:/msys64/usr/bin/bash.exe with
MSYSTEM=MINGW64 and PATH=/mingw64/bin:/usr/bin. Rack-linked binaries used
/c/Program Files/VCV/Rack2Pro ahead of compiler runtimes.

- Baseline: make -j10 build/tests/sibyl_legacy_golden_spec, then run its .exe:
  PASS; froze six output hashes before codec changes.
- make -j10 plugin.dll test-fast
  RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro": PASS, native plugin link
  and full routine suite. The three new test targets are included in test-fast.
- Final follow-up after collision/default hardening: make -j10 plugin.dll and
  build/tests/sibyl_{codec,note_edit,json,edit,module,legacy_golden}_spec:
  PASS. Ran all six resulting .exe files: PASS.
- Golden traces match at 44.1, 48, 96 kHz with and without probability evolution.
  They cover five seconds of swing, microshift, ties, glides, ratchets, scene
  repeats, arrangement looping and all output voltages, with increasing frames.
- Module integration covers preview/commit report equality, accepted-but-pending
  supersession, stale preview rejection, invalid preview nonmutation, notes
  projections, undo/redo state restoration, future-version rejection, and zero
  audio-thread allocations/deallocations through P1 adoption and playback.
- Python syntax compilation of Octavia_MCP.py: PASS.
- python tests/octavia_sibyl_contract_spec.py: 5 tests PASS.
- python MCP/tests/test_server_contract.py: 11 tests PASS.
- git diff --check: PASS.
- Sanitizer capability probes with g++ -fsanitize=address,undefined and
  -fsanitize=thread: unavailable; native linker cannot find lasan/lubsan/ltsan.
  Sanitizer test execution therefore SKIPPED, not claimed as passing.

During development, a codec fixture exposed the existing mismatch between
omitted pattern length validation and the compiler's default length of 16.
Normalization now supplies consistent length/resolution defaults. An existing
portable-envelope assertion was updated from schema 2 to canonical schema 3.
The focused suites pass after both changes.

## Remaining scope and limits

P2–P6 are not implemented or advertised: conditions, scene overrides, continuous
MOD automation, harmony and voicing are intentionally rejected. The full v3
example remains a future integration fixture, not a loadable P1 score. Operation
reports cover targeted note edits; existing legacy operations retain their
existing behavior. report_id_limit is the explicit bounded detail option
(1–1024, default 128); return_id_mapping requests duplicate provenance.

The subsequent live Rack integration check is recorded below. No listening check
or native Rack UI undo/redo action was performed. No new audio scheduler path was
added in this slice.
CPU-duration percentiles and a before/after timing benchmark were not recorded;
the baseline evidence here is waveform and allocation behavior, not a timing
claim. P6 still requires a separately retained/reconstructed baseline build for
the specified final performance comparison and expanded multi-feature stress
coverage. Cross-sample-rate one-sample timing comparisons are also deferred;
golden comparisons are exact within each sample rate.

No files were staged or committed. The user refreshed Rack and the MCP server
before the live check below; the running plugin exposed the new capabilities.

## Live Rack integration — 2026-09-18

PASS for the exercised MCP → Octavia → Sibyl paths in Rack 2.12.0. Used an
uncabled disposable Sibyl (1223480987059994), leaving existing modules and cables
untouched. Verified schema 3, noteEditing v1, and probability evolution support.

- Loaded a small three-note fixture; canonical IDs were n1/n2/n3, nextNoteId 4.
- Previewed an atomic duplicate n2 → n4 followed by a transpose of n4. Full
  composition, revision, nextNoteId, and Octavia undo stack remained unchanged.
  Commit produced the same IDs and operation counts as preview.
- Paginated notes across two pages; a cursor from revision 1 was rejected after
  editing. A stale expected_revision was also rejected.
- Verified pending supersession while running: revision 4 waited for nextScene
  with activeRevision 3; an edit against accepted revision 4 produced revision 5
  with activeRevision still 3 and nextBeat adoption. Later status showed active
  revision 5, no pending revision, running transport, and no runtime error.
- Octavia undo restored the prior authored transpose (20 instead of 19), kept
  IDs and nextNoteId, and advanced the revision to 6.
- Exported canonical JSON to test-results/sibyl-p01-live-roundtrip.json, read it
  back from disk, and reloaded through replace_composition at revision 6.
  Revision 7's complete composition equaled the exported composition exactly.
  This tests the MCP composition round trip, not Rack's file-menu workflow.
- Targeted projection returned n4's original E3 spelling, transposeSemitones 19,
  and derived effectivePitchV approximately 0.916667.
- Undid all remaining test writes and the module addition. After asynchronous
  removal completed, module inventory returned to the original nine modules;
  Octavia undo stack was empty. No patch save was performed.

Native Rack undo/redo remains unverified: vcv_undo uses Octavia's own reversible
write stack, and the bridge exposes no redo command. This live check does not
replace the deferred listening, performance, or full P6 validation work.
