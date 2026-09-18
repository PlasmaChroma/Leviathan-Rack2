# Sibyl Expressive Composition — Codex Implementation Specification

**Target:** `PlasmaChroma/Leviathan-Rack2`, branch `lumin-render`  
**Prepared:** September 18, 2026  
**Status:** Proposed implementation contract; not an implementation or a test-results report.  
**Scope:** Surgical note editing, repeat conditions, scene variations, harmonic context, and independent modulation automation.

## 0. Execution contract

Extend Sibyl incrementally. Keep musical authoring, validation, compilation, and revision ownership in Sibyl; keep Octavia a transport/semantic bridge. Do not replace Sibyl's sequencer, add a second score engine, or move musical semantics into a Python agent adapter.

Implement and verify one milestone at a time. The release sequence is:

1. **P0 — Baselines and codec foundation.**
2. **P1 — Targeted notes, selectors, transforms, and edit preview.**
3. **P2 — Deterministic repeat conditions.**
4. **P3 — Scene-assignment musical overrides.**
5. **P4 — Independent MOD automation.**
6. **P5 — Harmonic context and harmony-relative notes.**
7. **P6 — Control-side voicing helper and cross-feature hardening.**

P4 and P5 can be implemented in either order after P3. They share the compiled timeline and routing contracts specified below. They must not delay shipping P1–P3.

Read the checked-out `AGENTS.md` first. Record `git rev-parse HEAD`, branch, and the dirty working-tree state in the implementation report. Preserve unrelated work, particularly the rendering work on `lumin-render`. Do not stage or commit changes. The source review for this specification used mutable branch URLs, not a verified commit hash; reconcile any drift before editing.

Normative terms **MUST**, **MUST NOT**, and **SHOULD** describe proposed behavior. Proposed symbols and filenames below are not claims that they already exist. JSON fragments demonstrate the proposed contract unless explicitly labeled legacy.

## 1. Source-grounded starting point

The following observations motivate the design. Source identifiers resolve in §17.

| Reviewed source | Relevant existing behavior |
|---|---|
| [S1] `src/SibylTypes.hpp` | `StepEvent` has one pitch representation and optional expression values but no note identity. `Pattern` has sparse steps, repeat evolution, and `eventIndexByStep`. Assignments have a pattern and optional phase override. |
| [S2] `src/SibylEdit.cpp` | Operations mutate a copied JSON document, then parse/compile the complete candidate. Pattern upserts replace a pattern rather than patch individual events. `set_scene_track` writes a pattern string or removes the assignment. |
| [S3] `src/SibylJSON.cpp` | Validation enforces unique step indices and track channels. Full/partial serializers advertise schema 2. Pitch compilation is static, with C4 at 0 V. |
| [S4] `src/Sibyl.cpp` | Accepted and active revisions differ. EDIT requires `expected_revision`; adoption is boundary-controlled. The runtime caches routes, publishes telemetry, and holds immutable snapshots using owner/hazard machinery. MOD state is written when a note plays. Portable import explicitly checks schema 2. |
| [S5] `src/SibylEvolution.hpp` | Evolution has its own traversal cursor and deterministic expression functions. Its first pass is authored rather than evolved. |
| [S6] `src/SibylAdoption.cpp` | Channel changes are inferred by comparing tracks, events, patterns, and assignments. Phase-preserving adoption can still close a changed channel's gate. |
| [S7] `src/SibylControl.hpp` | Semantic calls run on the UI thread. A successful EDIT is the bridge's commit point for one undo snapshot; VALIDATE is separate. |
| [S8] `src/SibylTiming.cpp` | Event lookup uses the compiled sparse index; timing helpers handle wrapped steps and scheduled onsets. |
| [S9–S12] Existing tests, Makefile, and AGENTS | Focused Sibyl suites, allocation checks, a module harness, and platform-specific build instructions already exist. Extend them. |

**Important baseline details:** the legacy probability key uses seed, randomness epoch, scene index, channel, and step—not a traversal counter. The current working-tree baseline also includes opt-in `evolution.probability`: a depth in 0…1 applies bounded per-event, per-pass variation to the authored probability, and a positive depth gives unprotected events a fresh deterministic trigger draw on subsequent evolution passes. The first pass retains the authored threshold and legacy draw. Omitted/zero depth and `evolve:false` retain the legacy threshold/draw behavior; existing macro contributions still apply. Restart reproduces the variation sequence. Preserve both paths while adding conditions, along with existing tie, swing, microshift, clock, and evolution behavior. This is an extension, not permission to retune the scheduler. [S4, S5, S8]

P0 must capture the actual working-tree baseline, including this probability-evolution implementation, rather than reconstructing a baseline from HEAD alone. Preserve `evolution.probability` through v2 migration, v3 serialization, editing, and capability reporting; it already exists in schema 2 and must not be classified as a v3-only field. Its capability check is membership of `probability` in `repeatEvolution.fields`, not merely `repeatEvolution.version == 1`.

## 2. Product requirements and boundaries

### 2.1 Required musical outcomes

An agent must be able to express each operation without reconstructing an unrelated pattern or arrangement:

- Lower the final four authored events by an octave; adjust a selected group of ghost notes; rotate or duplicate a rhythm safely.
- Play an event every fourth pattern traversal, only on the first traversal, or only during the final authored repeat of a scene.
- Reuse one bass pattern with different chorus transposition, velocity, gate, probability, or modulation offsets.
- Define a chord progression once; author chord-root, chord-third, indexed chord-tone, or nearest-chord-tone events against it.
- Drive any existing per-track MOD1/MOD2/MOD3 output continuously over musical time, including during rests, rejected notes, and scenes with no note-pattern assignment for that track.

### 2.2 Invariants

Keep one authored event per pattern step and one voice per track/channel. Do not introduce chords inside a single `StepEvent`. Voicing distributes events over existing tracks.

Keep existing port and parameter IDs unchanged. No new jacks, required cables, modules, plugin dependencies, sockets, or background services are needed.

Keep existing `expected_revision`, `apply_at`, `phase_policy`, accepted/active/pending revision semantics, and Rack undo integration. Do not equate acceptance with sounding immediately.

Keep heavy work off the audio thread: JSON, selection, transformation, pitch-set construction, voicing search, allocations, and destruction of published generations belong to the control side.

Preserve authored representations. A chromatic transpose must not silently convert a scale-degree or harmony-relative event into a fixed voltage. Editing probability must not disturb observations, evolution opt-outs, ties, glide, or other MOD lanes.

**Payload efficiency is a requirement; incremental recompilation is not a P1 requirement.** Keeping the existing full-candidate validation path is acceptable initially. Do not describe smaller network requests as eliminating all control-side compile cost.

## 3. Codec, versioning, and persistence

### 3.1 Version strategy

Introduce **composition schema 3**. Retain semantic **API version 1** and advertise feature extensions individually.

A canonical v3 composition includes `schemaVersion: 3` inside the composition object. Portable and GET envelopes also declare the version of the enclosed representation. Where both versions occur, they must agree. This small duplication makes standalone composition fragments self-describing without relying on an outer bridge envelope.

```json
{
  "format": "Leviathan.SibylComposition",
  "schemaVersion": 3,
  "composition": {
    "schemaVersion": 3,
    "meta": {"title": "Example"},
    "tracks": [],
    "patterns": {},
    "arrangement": [],
    "macros": {}
  }
}
```

Bare documents without a version remain legacy v2 inputs. An explicit v2 envelope without an inner version is valid. An unversioned input containing v3-only musical fields must fail with `schema_version_required`; do not merely warn and discard a condition, override, harmonic pitch, or automation lane.

Normalize envelope handling in one codec entry point. Update portable load/save, patch `dataToJson`/`dataFromJson`, VALIDATE, EDIT replacement, full/partial reads, and internal edit serialization together. Reject unsupported future versions before accepting any state change. Legacy patch-loading defaults remain supported.

GET results and ordinary saves use canonical v3 after migration. Explicit v2 export is optional, not required for P1. Any implemented down-export MUST refuse when it would lose musical semantics. Do not silently bake or strip features.

New code must load old patches without changing their sound. This does **not** promise that old binaries can safely load new patches: historical `.vcv` loaders may ignore unknown state. The portable v3 envelope, capabilities, and documentation must make that limitation clear.

### 3.2 Strictness and representation

New edit operations, selectors, conditions, overrides, harmonic objects, and automation objects have strict allowlists. Unknown fields and wrong scalar types are errors. Preserve the legacy unknown-field policy for untouched legacy sections; do not use that tolerance to ignore malformed new musical instructions.

Feature availability is distinct from document version. During staged rollout, known v3 fields belonging to an unimplemented feature must return `unsupported_feature`, never be ignored.

Numbers must be finite; integer fields must actually be JSON integers. Bounds must be checked before narrowing or arithmetic. Use checked 64-bit intermediate arithmetic for degree, octave, step offsets, repeat products, and duration calculations.

Canonical serialization sorts events by step and uses a deterministic order for new map-like sections. Preserve authored optional-value presence where it affects inheritance. Compiled caches, pointers, indices, and runtime counters are never serialized as authored score.

Round-trip preservation means semantic field/value preservation, not original whitespace, key order, or accidental unsupported metadata.

### 3.3 Stable event identity

Add `StepEvent::id` and an authored `Pattern::nextNoteId` allocator counter.

```json
{
  "length": 16,
  "resolution": "1/16",
  "nextNoteId": 4,
  "steps": [
    {"id": "n1", "step": 0, "degree": 0},
    {"id": "n2", "step": 4, "degree": 2},
    {"id": "n3", "step": 8, "degree": 4}
  ]
}
```

IDs are unique **within a pattern**, 1–64 UTF-8 bytes, with no control characters. Existing entity-ID compatibility must not be broadened or narrowed accidentally. Auto-generated IDs are `n1`, `n2`, and so forth. The counter is a checked positive integer through `INT32_MAX`; skip occupied custom IDs and never wrap.

Legacy migration assigns IDs in ascending step order, deterministically, then stores the next unused counter. Merely compiling or reading an already normalized document must not regenerate IDs. Movement, transpose, and partial updates preserve IDs; duplication creates fresh IDs.

For legacy `upsert_pattern` calls omitting note IDs, reuse an old event's ID at the same step; allocate IDs for newly occupied steps. This is a compatibility convenience, not inference that a moved event is the same note. Clients requiring identity across moves must use IDs or targeted transforms. Explicit IDs are authoritative and must pass uniqueness checks.

Keep the allocator high-water mark for targeted edits and ordinary pattern updates. Deleting an auto-ID does not make it available for automatic reuse. Authoritative document replacement/import defines a new document's identity history. Counter exhaustion is a structured error.

IDs are for editing and inspection. Do not change the legacy probability/evolution keys to use them in this project.

### 3.4 Pitch-preserving transpose field

Add optional event `transposeSemitones`, default 0, integer range −120…120. It is an offset applied after resolving the event's pitch representation. It must not replace `note`, `degree`, `pitchV`, or `harmonic`.

The final pitch pipeline is:

```text
resolved event pitch
+ event.transposeSemitones / 12
+ scene assignment overrides.transposeSemitones / 12
= effective pitch voltage
```

Validate all reachable effective pitches against the supported output range −10…10 V, including harmonic contexts. Do not clamp pitches into an unintended register. Omit a zero offset in canonical serialization.

## 4. Transaction, preview, and inspection contracts

### 4.1 Preserve the existing mutation envelope

```json
{
  "expected_revision": 42,
  "apply_at": "nextBeat",
  "phase_policy": "preserve",
  "operations": [
    {
      "op": "transpose_notes",
      "pattern_id": "bass_main",
      "selector": {"order": "stepDesc", "limit": 4},
      "expect_count": 4,
      "semitones": -12
    }
  ]
}
```

The example is a Sibyl EDIT payload, not a claim about a Python tool's wrapper signature. Discover the checked-out bridge wrappers and forward the payload through their existing routes.

Operations execute sequentially on a private candidate. Each selector resolves against the candidate **at that operation**, not repeatedly during mutation. Earlier operations are visible to later operations. No mutation becomes accepted until the entire transaction validates and compiles.

One accepted EDIT advances the accepted revision once and creates one undo unit through the existing bridge. Failed validation, capacity failure, selection mismatch, or revision conflict leaves accepted state, active state, pending adoption, undo history, and ID counters unchanged.

Retain existing pending-revision supersession semantics: an edit based on the latest accepted revision includes earlier accepted edits even when those edits have not sounded. Do not invent a second queue of musical revisions.

A semantically unchanged but otherwise valid EDIT can follow the existing accept-new-revision path and emit a `no_effect` warning. Do not return a fake mutation success without performing the acceptance that Octavia interprets as its commit point.

### 4.2 Edit preview belongs in VALIDATE

Extend VALIDATE with an operation-preview form:

```json
{
  "expected_revision": 42,
  "operations": [
    {
      "op": "rotate_notes",
      "pattern_id": "hats",
      "selector": {"all": true},
      "steps": 2
    }
  ],
  "return_changes": true
}
```

Keep existing candidate-validation input supported. Exactly one of `candidate` or `operations` may be supplied. Operation preview requires `expected_revision` and runs the same selection, transformation, validation, and compilation logic as EDIT, without publishing or consuming IDs.

Use `ok:true, valid:false` for a well-formed validation request whose candidate fails. Use structured `ok:false` for malformed requests and stale revisions. Include real warnings rather than substituting an empty warning list.

**Do not implement preview as an EDIT with `dry_run:true`.** That conflicts with the current bridge commit/undo contract. No new generic semantic operation is needed.

### 4.3 Compact change reports

Add `changes` to successful edits and operation previews:

```json
{
  "ok": true,
  "revision": 43,
  "activeRevision": 42,
  "pendingRevision": 43,
  "applyAt": "nextBeat",
  "phasePolicy": "preserve",
  "changes": [
    {
      "operationIndex": 0,
      "patternId": "bass_main",
      "matched": 4,
      "updated": 4,
      "inserted": 0,
      "deleted": 0,
      "noteIds": ["n13", "n14", "n15", "n16"]
    }
  ],
  "warnings": []
}
```

The reported active/pending values are actual observed state, not constants copied from the requested revision. Include source-to-created ID mappings for duplication when requested. Include displaced IDs for replacement collisions. Bound ID lists at 128 by default and expose `idsTruncated` plus the total; include all IDs only through a bounded explicit detail option. Never return a whole composition by default.

### 4.4 Targeted reads

Extend GET_COMPOSITION with `view:"notes"`, `pattern_id`, a selector, a field projection, `page_size` ≤256, and an opaque cursor. Bind cursors to revision, pattern, selection, and ordering; a cursor used after a revision change returns `revision_conflict`.

Default fields: `id`, `step`, authored pitch fields, `transposeSemitones`, `velocity`, `probability`, and `condition`. Support a full-event projection for precise edits. A derived effective-pitch projection requires `scene_id`, zero-based `scene_repeat`, and `beat` within that repeat when pitch depends on harmony. Label derived values separately from authored fields.

An empty read result is valid. An empty mutation selection is an error by default. Do not expose storage-vector indices as durable note identifiers.

### 4.5 Structured errors

Use the established `{ok:false,error:{code,path,message}}` style. New codes include:

`schema_version_required`, `unsupported_schema`, `unsupported_feature`, `invalid_selector`, `selection_empty`, `selection_count_mismatch`, `note_not_found`, `duplicate_note_id`, `step_collision`, `inherited_value_requires_track`, `unsupported_pitch_transform`, `pitch_out_of_range`, `invalid_condition`, `unresolved_harmony`, `ambiguous_chord_role`, `automation_conflict`, `off_grid_harmony`, `capacity_exceeded`, and `voicing_unsatisfiable`.

Paths should identify the operation and field, such as `operations[1].selector.ids[2]`. Use identifiers in messages, not huge note arrays. Return bounded diagnostic counts and actionable limits.

## 5. P1 — Targeted note editing and transforms

### 5.1 Selector grammar

Selectors support these mutually compatible restrictions:

| Field | Meaning |
|---|---|
| `ids` | Nonempty list of exact note IDs; duplicate or missing IDs are errors. |
| `steps` | Nonempty list of authored integer grid steps; duplicate values are errors. A rest is simply not selected. |
| `step_range` | Two integers `[start, end)` within `[0, pattern.length]`. No implicit wrapping. |
| `where` | Bounded typed predicates on `velocity`, `probability`, `gate`, `evolve`, and `tie`. |
| `order` | `stepAsc` default or `stepDesc`. |
| `limit` | Positive integer ≤1024, applied after filtering and ordering. |
| `all` | Must be `true`; explicitly selects all notes before order/limit. Cannot coexist with other restrictions. |

Restrictions combine by intersection. `order` plus `limit` without a restriction is valid, supporting “last four notes.” An otherwise empty selector is invalid; require `all:true` for whole-pattern transformations.

Numeric predicates allow `eq`, `lt`, `lte`, `gt`, and `gte`; multiple comparisons are ANDed. Boolean predicates are literal booleans. No arbitrary JSONPath, regex, JavaScript, scripting, runtime expressions, or recursive Boolean query language.

Predicates inspect authored event values. Unset velocity/gate do not match numeric predicates; a pattern may be reused by tracks with different defaults. Probability omitted on an event means 1; `evolve` omitted means true; `tie` omitted means false. Explicit effective-value editing is addressed below, not hidden in selectors.

`expect_count`, when present on an operation, checks the final selected cardinality before any modification. Default mutation behavior on zero matches is `selection_empty`. An explicit `allow_empty:true` permits a no-effect transaction with a warning.

### 5.2 Operations

| Operation | Required operation-specific fields | Semantics |
|---|---|---|
| `update_notes` | `pattern_id`, `selector`, at least one of `set`, `unset`, `adjust` | Change only named authored fields. |
| `insert_notes` | `pattern_id`, `notes` | Insert full events; allocate omitted IDs. Default collisions fail. |
| `delete_notes` | `pattern_id`, `selector` | Delete selected events, not their containing pattern. |
| `transpose_notes` | `pattern_id`, `selector`, exactly one of `semitones` or `degrees` | Preserve pitch representation and note identity. |
| `rotate_notes` | `pattern_id`, `selector`, `steps` | Simultaneous circular movement of selected notes. |
| `duplicate_notes` | `pattern_id`, `selector`, `offset_steps` | Copy selected events to shifted steps with fresh IDs. |

`set` can update any authored StepEvent field except `id` and `step`. Moving time belongs to the transforms. Setting one pitch representation atomically removes the other pitch representations and any representation-specific companion fields. Setting `octave` alone is valid only on degree events. `harmonic` has its own nested octave and does not use the legacy top-level octave.

`unset` accepts optional fields, including gate, velocity, probability, MOD values, condition, and transpose offset. It restores their documented defaults/inheritance. It cannot remove `id`, `step`, or the sole pitch representation. A field cannot appear in more than one of `set`, `unset`, or `adjust` in the same operation.

`adjust` accepts numeric expression fields with `{multiply, add}`; missing multiplier/addend defaults to 1/0, respectively. Compute `new = old * multiply + add`. It does not accept pitch, step, ratchets, or Boolean fields.

```json
{
  "op": "update_notes",
  "pattern_id": "snare",
  "selector": {"where": {"velocity": {"lt": 0.4}}},
  "adjust": {"probability": {"multiply": 0.6}},
  "expect_count": 3
}
```

For adjustment, omitted probability resolves to 1 and omitted MOD values resolve to 0. Adjusting inherited velocity/gate requires `resolve_defaults_for_track:"trackId"`; validate the track exists, use its default, and materialize the adjusted value explicitly on the note. Do not apply current scene overrides, macros, or evolution during destructive authoring transforms.

Default out-of-range adjustment behavior is an error. `clamp:true` explicitly allows clamping expression fields to their authored valid range and returns affected counts. Pitch, step, integer overflow, and structural errors are never silently clamped.

### 5.3 Transpose semantics

`semitones` adds to `transposeSemitones`; reject overflow or an out-of-range resulting offset/pitch. An octave down is exactly −12 semitones for note, degree, voltage, and harmonic representations.

`degrees` updates only degree events. A mixed selection containing another representation fails atomically with `unsupported_pitch_transform`. Do not round voltages or infer a scale degree. Use the composition's existing degree conventions, including negative degree wrapping.

Unknown future pitch types must fail rather than becoming fixed voltages. A representation-conversion operation is outside P1.

### 5.4 Rotation and duplication semantics

For rotation, destination step is `floorMod(sourceStep + steps, pattern.length)`. Resolve all sources first, compute all destinations, remove the selected sources conceptually, then check collisions against **unselected** events. A swap among selected notes is valid. Preserve microshift and all other event fields.

Collision policy is `collision:"error"` by default. Optional `collision:"replace"` explicitly permits deletion of unselected destination events and reports their IDs. There is no implicit merge, stacking, partial success, or order-dependent winner.

Duplication retains sources, assigns fresh destination IDs, and preserves event data including observations and evolution opt-outs. Report `observations_copied` when observation-bearing events are duplicated. Support an explicit `copy_observations:false` to strip them from the copies; do not strip by accident.

`destination_pattern_id` defaults to the source pattern. The destination must already exist, possibly created earlier in the transaction. Source and destination resolutions must match. `wrap:false` is the default; out-of-bounds destinations fail. `wrap:true` uses the destination pattern's length. Duplicate destinations are errors even under replacement collision policy.

Inserted and duplicated notes must respect pattern limits, one event per step, and final pitch/condition validation. An invalid fifth copy must not leave four successful copies behind.

### 5.5 P1 implementation boundaries

Keep selector and transform code in a focused helper such as proposed `SibylNoteEdit.hpp/.cpp`, called by `applyCompositionEdit`. Reuse the same helper for preview. Do not duplicate transform semantics in bridge wrappers.

Preserve the existing edit engine until evidence justifies replacing it. Its serialization/reparse round trip must now retain IDs, transpose offsets, and every later feature in this specification.

## 6. P2 — Repeat conditions

### 6.1 Authored grammar

An absent condition means always eligible. A condition is an AND of at most eight simple tests; no nested expression tree.

```json
{"condition":{"all":[{"scope":"patternPass","every":4,"offset":4}]}}
```

```json
{"condition":{"all":[{"scope":"sceneRepeat","is":"last"}]}}
```

```json
{"condition":{"all":[{"scope":"patternPass","is":"first"}]}}
```

Scopes are `patternPass`, `sceneRepeat`, and `arrangementLoop`. A test uses either `every` plus optional `offset`, or `is`, never both. `every` is 1…1024; `offset` is 1…`every`, default 1. Public ordinals are **one-based**:

```text
eligible = (ordinal - offset) mod every == 0
```

Use floor-mod semantics. `every:4, offset:4` means passes 4, 8, 12. `every:4, offset:1` means 1, 5, 9.

`is:"first"` is legal for all three scopes. `is:"last"` is legal only for `sceneRepeat`, where total repeats are explicitly authored. There is no intrinsic last pass of a freely looping pattern.

A final-repeat fill is the sceneRepeat/last condition; no additional fill-control UI is necessary. Live fill buttons, previous-event conditions, Boolean OR, and arbitrary expressions are deferred.

### 6.2 Counter definitions

**sceneRepeat:** one-based ordinal within the current scene visit. An ordinary authored repeat increments it. Entering a scene anew sets it to 1, including a manual jump to the same scene.

**arrangementLoop:** one-based count of automatic complete arrangement traversals since arrangement reset/stop. Automatic wrap increments it; manually selecting scene zero does not. A non-looping ending does not increment it.

**patternPass:** a timing-derived traversal ordinal for the currently routed pattern. It is not a count of notes heard, successful probability checks, or calls to `EvolutionCursor::observe`.

Rules for patternPass:

| Transition | Required behavior |
|---|---|
| Explicit pattern/scene/arrangement restart | First traversal starts at 1. |
| Natural wrap within a pattern | Advance once per crossed traversal, even if every event is absent or rejected. |
| Ordinary repeat of the same scene | Follow continuing pattern time; do not invent a pattern reset. |
| Enter destination with `restart` | Restart patternPass at 1. |
| Enter destination with `continue`, same pattern | Preserve phase and pass. |
| Enter destination with `continue`, different pattern | Preserve the phase policy but rebase the new pattern's current partial traversal as pass 1. |
| Enter with `alignGlobal` | Derive ordinal from the aligned global nominal cycle: `floor(globalBeats / patternDuration) + 1`. |
| Pause/external-clock hold | Freeze musical counters. |
| Reseed or randomness-only restart | Do not reset condition counters. |
| Phase-preserving edit | Preserve current pass; timing changes rebase cycle coordinates without inventing a traversal. |
| `restartChanged` / `restartAll` adoption | Reset affected condition cursors consistently with restarted pattern phases. |

Continue to observe every scheduled authored event with the legacy evolution cursor even when a new condition rejects it; do not make evolution progression depend on condition success. Empty-traversal counting for conditions still comes from time, not that event-driven legacy cursor.

Use a dedicated compact condition cursor, not the legacy evolution cursor. Retaining a separate cursor avoids changing repeat evolution's established reset/first-pass behavior.

### 6.3 Microshift, boundaries, and evaluation order

An event belongs to its **nominal** pattern traversal, including an event anticipated before a wrap by negative microshift. Compute the event's pass from its nominal step and the cursor's phase/cycle mapping. Do not use only the sample's current wrapped phase, and do not advance a global pass early because one anticipated event was encountered.

sceneRepeat conditions describe the scene-repeat interval containing the event's scheduled onset. An anticipated pattern event before a scene-repeat boundary still occurs in the earlier scene repeat. At an exact scene boundary, apply existing destination-scene ordering first; do not fire an event against stale scene context.

At each scheduled onset:

```text
identify event and timing context
-> evaluate deterministic condition once
-> if eligible, evaluate existing probability formula with scene overrides/macros
-> latch played/skipped result for this event and all its ratchets
-> on play, resolve pitch/expression and publish any observation trigger
```

A condition cannot be bypassed by a probability macro. No random draw is needed for conditions. Do not evaluate conditions again per sample or per ratchet.

A condition-false event follows the existing rejected-event rest behavior: no new pitch, velocity, event MOD, or observation; gate closes at that scheduled onset. Automation remains independent.

A condition-false tie must not extend a previous note merely because `tie:true` is present. The tie look-ahead must consult deterministic condition eligibility for the prospective event. Preserve the legacy probability/tie policy for unconditioned events; do not broaden this task into a tie rewrite. A skipped tied event must not resurrect a low gate.

### 6.4 Boundary arithmetic and observability

Advance counters from bounded arithmetic, not loops over missed beats or generated events. Use checked/saturating counters with a documented ceiling; never wrap a signed counter into negative time. Condition evaluation needs only small modulo operations at event onsets.

Expose `conditionPasses` and `arrangementLoop` through coherent telemetry, separate from legacy `evolutionPasses`. Their ordinals are one-based. Preserve existing telemetry field conventions rather than silently renumbering old status fields.

## 7. P3 — Scene-specific pattern variations

### 7.1 Assignment schema

Keep legacy string assignments valid. Extend object assignments without cloning their patterns:

```json
{
  "pattern": "bass_main",
  "phaseMode": "continue",
  "overrides": {
    "transposeSemitones": 12,
    "velocityScale": 1.15,
    "velocityOffset": 0.0,
    "probabilityScale": 0.85,
    "probabilityOffset": 0.0,
    "gateScale": 0.9,
    "gateOffset": 0.0,
    "modOffset": 0.5,
    "mod2Offset": 0.0,
    "mod3Offset": 0.0
  }
}
```

All override fields are optional. Identity defaults are scale 1, offset 0, transpose 0. Proposed authored bounds: scales 0…4; velocity/probability offsets −1…1; gate offset −1024…1024 steps; MOD offsets −20…20 V; transpose −120…120 semitones. Reject nonfinite values and invalid types.

These are performance transforms of a pattern instance, not edits to pattern data. Two tracks or scenes using the same pattern may have different overrides simultaneously. A pattern read must show authored values; a scene-context preview must show effective values separately.

### 7.2 Precedence and clamping

At a played event, use the existing evolved expression as input. Apply assignment scale/offset, then existing performance macro contributions:

```text
velocity = clamp(evolvedVelocity * velocityScale
                 + velocityOffset + globalVelocityMacro + trackVelocityMacro, 0, 1)

probability = clamp(evolvedProbability * probabilityScale
                    + probabilityOffset + globalProbabilityMacro + trackProbabilityMacro, 0, 1)

gateBase = evolvedGate * gateScale + gateOffset

unautomated MOD at onset = clamp(evolvedEventMod + sceneModOffset
                                + sampledTrackModMacro, -10, 10)
```

Gate shaping keeps existing gate units, ratchets, ties, and live gate-macro semantics. Clamp valid gate durations according to the existing ratchet/non-ratchet domain. When a non-identity gate override produces a nonpositive gate after applicable macro contribution, the new override path must remain silent, not turn zero into a minimum pulse. Preserve the legacy no-override path exactly; characterize any pre-existing zero-gate behavior before changing it.

Probability evolution produces the bounded threshold before scene scaling/offsets and macro contributions. Use the existing opted-in or legacy trigger draw as appropriate; conditions must not substitute their own pass counter for the evolution pass or random key.

Identity overrides must sound the same as absent overrides. Local event and scene transposes add; neither changes the global scale. Conditions are a separate eligibility stage, never scaled by probability overrides.

### 7.3 Editing assignments

Add `set_scene_assignment` with `scene_id`, `track_id`, and `assignment` (full object, string, or null). It replaces the entire assignment explicitly.

Add `update_scene_assignment` for partial updates to an existing assignment:

```json
{
  "op": "update_scene_assignment",
  "scene_id": "chorus",
  "track_id": "bass",
  "set": {
    "overrides": {"transposeSemitones": 12, "velocityScale": 1.15}
  }
}
```

Partial `overrides` objects merge named override fields; do not replace unspecified overrides. `set.pattern` and `set.phaseMode` update only those fields. `unset` accepts `phaseMode`, `overrides`, or allowlisted leaf names such as `overrides.velocityScale`; it cannot remove the pattern. Normalize a legacy string assignment into an object before merging. Missing assignments fail; creation uses `set_scene_assignment`.

Retain `set_scene_track` compatibility. Because its old semantics replace the assignment with a string, return `assignment_attributes_cleared` when that call discards phase/override attributes. Agent documentation must prefer the new operations for musical variations.

Compare and compile all override fields. Do not omit them from change detection or let a scalar default accidentally mask an authored override.

## 8. P4 — Independent MOD automation

### 8.1 Scope

Automate only the existing MOD1, MOD2, and MOD3 outputs in the first implementation. No pitch, gate, velocity, tempo, arbitrary module parameters, MIDI, or new outputs are included.

A curve's clock is musical time, not event execution. A probability-zero pattern, condition-false notes, rests, and an unassigned note route must not stop it.

### 8.2 Schema

Add a top-level `automation` object keyed by automation ID:

```json
{
  "automation": {
    "filter_rise": {
      "target": {"track": "bass", "lane": "mod"},
      "scope": {"scene": "build"},
      "clock": "sceneVisit",
      "mode": "replace",
      "enabled": true,
      "transitionMs": 0,
      "points": [
        {"beat": 0, "value": 0, "shape": "linear"},
        {"beat": 32, "value": 5}
      ]
    }
  }
}
```

Here 32 quarter-note beats represents eight bars only under a 4/4 authoring convention. Sibyl does not gain a time-signature subsystem in this project; agents must translate bars explicitly.

`target.lane` is `mod`, `mod2`, or `mod3`; `target.track` must resolve to an existing track. The lane values are volts, not normalized fractions.

Allowed scope/clock combinations:

| Scope | Clock | Coordinate |
|---|---|---|
| `{"scene":"id"}` | `sceneRepeat` | Current scene phase, restarting each authored repeat. |
| `{"scene":"id"}` | `sceneVisit` | `(repeatOrdinal − 1) * lengthBeats + scenePhase`. |
| `{"arrangement":true}` | `arrangement` | Score-position prefix of prior scenes/repeats plus current scene position. |

Arrangement coordinate is **score position**, not the ever-increasing global clock phase. Manual scene jumps relocate it; automatic arrangement loops restart it. A 32-beat sceneVisit ramp can span four 8-beat repeats without resetting.

`mode` is `replace` (default) or `add`. `enabled` defaults true. `transitionMs` defaults 0, allowed 0…1000, and controls ownership/adoption handover—not normal interpolation.

### 8.3 Points and interpolation

Require 1…1024 points, first beat exactly 0, strictly increasing beat coordinates, finite values in −10…10 V, and all points within the scope's duration. A point at the scope endpoint is permitted as an interpolation endpoint. There is no duplicate-time point or implicit sorting of malformed curves.

Interpolation is chosen by the **left** point for the interval to its successor:

- `step`: hold the left value; take the new value at the right timestamp.
- `linear`: `a + (b − a) * u`.
- `smoothstep`: `a + (b − a) * (3u² − 2u³)`.

Default shape is `linear`. A one-point curve is constant. After the final point, hold its value until the scope ends. The last point has no outgoing segment; specifying a nondefault shape there yields an unused-field warning rather than invented behavior.

Normal scopes are half-open intervals. At an exact scene-repeat or scene boundary, the destination coordinate owns the sample. Thus an endpoint can define the limiting value approached by a ramp without guaranteeing an extra old-scope sample at the endpoint. At a non-looping arrangement stop, hold the terminal endpoint value.

No per-sample `pow`, `exp`, spline fitting, allocations, or JSON walking. Compile segment duration reciprocals and polynomial coefficients on the control side. Use an indexed segment cursor; bounded binary search handles seeks and discontinuities.

### 8.4 Ownership and note interaction

At most one enabled arrangement curve and one enabled scene curve may target a track/lane. Multiple curves at the same specificity for that target are `automation_conflict`. The scene curve takes precedence for its entire scene visit; do not sum or partially splice it with the arrangement curve.

The selected curve owns the output lane for its entire scope, including its held tail:

```text
replace mode:
  out = clamp(curve(t) + sceneModOffset + currentTrackModMacro, -10, 10)

add mode:
  out = clamp(heldAuthoredEvolvedEventMod + curve(t)
              + sceneModOffset + currentTrackModMacro, -10, 10)
```

Maintain a **raw event MOD base** on every successful event, including while no curve owns the lane, so enabling automation mid-note has a valid base. Keep it separate from the legacy final output latch; otherwise scene offsets/macros will be counted twice. In add mode, successful notes update that raw base. Rejected notes do not. In replace mode, notes never cancel the curve, even when they explicitly contain a MOD value. Repeat evolution does not alter the curve itself.

On a new pattern route/restart, initialize the automated lane's event base to zero until a played event supplies it. Preserve it when continuing the same route. An automated track without a pattern uses zero base and still drives its MOD output. Deleting a track invalidates referencing automation until the transaction removes or retargets it.

When no curve owns a lane, preserve the old note-latched MOD and macro behavior. Do not globally convert all legacy MOD macros into continuous controllers as a side effect of this feature.

This is intentionally not a universal “last writer wins” stack: ownership plus replace/add mode makes event-versus-curve interaction explicit.

### 8.5 Transport and handover

Pause/external-clock hold freezes the curve's musical coordinate. Physical macros remain live on curve-owned lanes; freezing musical time does not freeze external control voltage.

Stop/reset relocates the curve to the transport's reset coordinate. Manual scene changes sample the destination curve immediately at the destination score coordinate. Panic closes gates without inventing a curve reset.

Adopting an edited curve with phase preservation evaluates it at the current musical coordinate, not beat zero. For `transitionMs:0`, take the new output immediately. With a positive transition, blend from the last emitted voltage to the newly evaluated moving destination value over `ceil(ms * sampleRate / 1000)` samples. Start at blend weight zero and reach one at that sample count. Apply output limiting once after blending; no per-sample allocation.

On curve removal or scope handover, use the outgoing curve's transition duration when there is no incoming curve, and the incoming duration when there is one. Repeated edits restart the blend from the actual current output, not an older target. Sample-rate changes recompute remaining transition duration in seconds.

Automation-only edits should not close note gates. They have their own dependency/change mask and cursor adoption, distinct from note-timing changes.

### 8.6 Editing and reads

Add `upsert_automation` and `delete_automation`, plus `view:"automation"` and summary counts/targets. Full curve replacement is acceptable: curves are compact compared with sampled per-note sweeps. Dedicated breakpoint editing is deferred.

An automation preview returns bounded samples or segment summaries in volts and beats. It must not perform a full audio render or return an unbounded sampled array. Projection and point counts belong in capabilities.

## 9. P5 — Harmonic context and relative pitches

### 9.1 Progression model

Add a top-level harmony section:

```json
{
  "harmony": {
    "progressions": {
      "main_changes": {
        "lengthBeats": 16,
        "chords": [
          {"id":"c1", "beat":0,  "root":"C3", "intervals":[0,4,7]},
          {"id":"c2", "beat":4,  "root":"A2", "intervals":[0,3,7]},
          {"id":"c3", "beat":8,  "root":"F2", "intervals":[0,4,7]},
          {"id":"c4", "beat":12, "root":"G2", "intervals":[0,4,7]}
        ]
      }
    },
    "default": {
      "progression": "main_changes",
      "clock": "arrangement",
      "loop": true
    }
  }
}
```

A progression is a finite musical-time timeline. Chords start at strictly increasing beat positions; first beat is 0 and the last is less than `lengthBeats`. Each chord persists until the next marker or the end. IDs are unique within the progression.

Roots use the existing scientific-note convention. Intervals are a sorted, unique list of 1…8 integer semitone offsets, 0…36, starting with 0. Duplicate pitch classes within a chord are disallowed in this first model; octave doublings belong to voicing. The root plus intervals must remain in the supported pitch domain before per-event register offsets.

Initial schema uses explicit roots/intervals. Roman-numeral notation, arbitrary chord-symbol parsing, adaptive scales, microtonal chord tables, and automatic harmonic analysis are out of scope.

A scene may carry its own `harmony` binding, with `progression`, `clock` (`sceneRepeat` or `sceneVisit`), and `loop`. An absent scene binding inherits the default. An explicit scene `harmony:null` disables it. Defaults use the arrangement clock only. Reuse the automation coordinate definitions; do not build another incompatible beat model.

`loop:true` selects progression time modulo its length. `loop:false` holds the final chord after progression end. Scene binding takes precedence over default for the entire scene.

### 9.2 New pitch representation

Extend PitchType with `HARMONIC`. Exactly one of `pitchV`, `degree`, `note`, or `harmonic` must be present. Existing pitch representations retain their existing interpretation.

Chord-root and indexed-tone examples:

```json
{"id":"n1","step":0,"harmonic":{"kind":"tone","index":0,"octave":0}}
```

```json
{"id":"n2","step":4,"harmonic":{"kind":"tone","role":"third","octave":1}}
```

`kind:"tone"` requires exactly one of `index` or `role`. Index is zero-based within the authored intervals; it does not wrap. Octave is an integer −10…10, default 0, subject to final pitch bounds.

Roles are deliberately limited: `root` selects interval 0; `third` selects the unique interval whose pitch class is 3 or 4; `fifth` selects the unique interval whose pitch class is 6, 7, or 8. Missing or ambiguous roles are validation errors. A suspended chord has no implicit third. Other/custom roles use an index. This prevents “third” from silently meaning the second entry of an arbitrary chord collection.

Nearest-chord-tone example:

```json
{
  "id":"n3",
  "step":8,
  "harmonic": {
    "kind":"nearest",
    "reference":{"note":"G3"},
    "range":{"min":"C3","max":"C5"},
    "tieBreak":"lower"
  }
}
```

`reference` contains one static pitch representation: note, degree with optional octave, or pitchV. It cannot contain another harmonic expression. `range` uses inclusive scientific-note bounds. `tieBreak` is `lower` (default) or `higher`.

Consider octave equivalents of the current chord's pitch classes within the range; choose minimal absolute semitone distance from the static reference. No candidates means `unresolved_harmony`. Do not choose from the global scale instead. Nearest-tone selection is not based on the last successfully played note, which would make probability change harmonic interpretation.

Apply event and assignment transposition **after** harmonic selection. Register constraints apply to the pre-transpose selection, while final pitch bounds still apply to the output. Context previews must expose both so a transposed register is not misleading.

### 9.3 Timing and runtime semantics

Resolve harmony at the event's **scheduled sounding onset**, using the corresponding score coordinate. Derive that coordinate from the scheduled beat, not merely the sample's potentially overshooting phase; a delayed boundary sample must not select a later chord for an earlier onset. This is distinct from patternPass, which identifies the nominal traversal. An anticipated event just before a chord change uses the old chord; one exactly on the chord boundary uses the new chord.

Latch resolved pitch at event onset. A chord change does not retune a sustained event, glide target, or individual ratchets. A subsequent tied event can resolve a new chord pitch according to existing tie/glide behavior.

Static notes retain their cheap compiled voltage path. Harmonic events reference a compiled expression/context lookup. Build pitch candidates and resolve roles outside `process()`. Cache/intern repeated harmonic expressions and chord definitions; do not allocate the full note-by-all-chords Cartesian product without a budget check.

Compile only contexts reachable through actual scene assignments. An unassigned harmonic pattern may remain in the bank with an `unbound_harmony_pattern` warning. Assigning it to a scene with no effective harmony, an absent role, or an unreachable register must fail the transaction.

If compile-table capacity would be exceeded, return `capacity_exceeded` with counts. Do not silently switch to an unbounded audio-thread search. A bounded lookup/binary search over precompiled context ranges is allowed at note onsets.

### 9.4 Harmonic editing and inspection

Add `upsert_progression`, `delete_progression`, `set_default_harmony`, and `set_scene_harmony`. Binding setters accept a full binding or null with the inheritance/disable semantics above. Deletion of a referenced progression is `object_in_use` unless prior operations detach its references.

Provide progression and effective-context views, including chord IDs, beat intervals, roles, pitch classes, and effective track pitches. The voicing popup/preview must not blindly display `compiledPitchV` as a universal value for harmonic events. Use a defined scene/time context or explicitly label the display as authored/relative.

Harmony changes must participate in dependency analysis for every track using affected harmonic events. Unrelated fixed-pitch percussion tracks must not be marked as harmonically changed.

## 10. P6 — Control-side voicing helper

### 10.1 Purpose and limits

Provide `voice_progression` as a deterministic **edit-time helper**, not a runtime voice allocator. It distributes a progression across already defined tracks. It does not create Rack modules, cables, output channels beyond existing tracks, or polyphonic events.

The helper emits ordinary fixed-pitch events. This is a deliberate materialization: changing the source progression later does not silently rewrite generated notes. Agents that require automatic harmonic following should author `harmonic` events instead; rerun the helper when they want newly optimized voicings. Report materialization/progression provenance in its change report and documentation.

### 10.2 Operation shape

```json
{
  "op": "voice_progression",
  "progression_id": "main_changes",
  "scene_id": "chorus",
  "resolution": "1/16",
  "gate_ratio": 0.95,
  "write_policy": "createOnly",
  "voices": [
    {"track_id":"chord_low", "pattern_id":"chorus_low", "min":"C2", "max":"C3"},
    {"track_id":"chord_mid", "pattern_id":"chorus_mid", "min":"C3", "max":"C4"},
    {"track_id":"chord_high","pattern_id":"chorus_high","min":"C4","max":"C5"}
  ]
}
```

Require 1…8 voices, distinct track IDs, distinct destination pattern IDs, and voices ordered low-to-high. All tracks must exist. `createOnly` fails on existing destination patterns or an existing assignment for a target track in the scene; `replace` permits replacement explicitly and reports affected references.

For the first helper, the scene's `lengthBeats` must equal the progression length, and each chord marker must lie exactly on the requested grid. Pattern length must be integral and ≤1024. Reject off-grid times with `off_grid_harmony`; never silently quantize. One repeated scene can repeat the generated progression.

Emit one event per chord marker per voice. Gate length is the chord interval in pattern steps multiplied by `gate_ratio` (0…1), subject to existing gate limits. Ratchets are 1. Attach generated patterns to the specified scene with restart phase behavior. Do not modify other scenes' assignments.

### 10.3 Deterministic voice leading

Enumerate chord-tone candidates within each inclusive voice register. Require noncrossing voices; equal pitches are permitted but disfavored. Require root coverage, and third coverage where a unique third exists and there are at least two voices. When there are enough voices, prefer coverage of all available chord pitch classes.

Choose the sequence of voicings by bounded dynamic programming. The objective is a lexicographically ordered tuple:

1. Minimize total missing available chord pitch classes.
2. Minimize the largest inter-chord per-voice leap.
3. Minimize total absolute semitone movement.
4. Minimize total squared semitone movement.
5. Minimize initial displacement from register midpoints.
6. Break exact ties by lexicographic MIDI-pitch sequence in declared voice order.

Implement the maximum-leap objective with an exact staged solver: first find the minimum coverage cost; then find the smallest integer leap ceiling that still admits that cost; finally optimize the remaining additive tuple under that ceiling. A bounded binary search over the leap ceiling is sufficient. Count transitions across **all** solver passes against the request budget. Do not keep a single lexicographically best running-maximum label per state: a future larger leap can equalize earlier maxima and make that pruning incorrect.

These objectives are proposed musical defaults, not universal music-theory laws. Keep them named/versioned as `voice_progression_v1`. No random choices, unordered-container iteration dependence, platform-specific `std::hash`, or hidden learned model.

The default optimization covers adjacent chords within the progression, not the last-to-first loop seam. Report that boundary explicitly. Circular voice-leading optimization is deferred.

Bound candidate enumeration at 200,000 visited partial assignments per request, 2,048 valid candidates per chord, and 2,000,000 evaluated DP transitions. Exhaustion returns `capacity_exceeded`, not a silently pruned result claiming optimality. Impossible constraints return `voicing_unsatisfiable`. Preview reports register and coverage failures before commit.

Everything emitted by the helper participates in the same atomic candidate and one undo transaction. No partial pattern creation on failure.

## 11. Runtime architecture, compilation, and adoption

### 11.1 Proposed implementation types

The following signatures describe boundaries, not a required binary layout:

```cpp
struct NoteSelection;
struct NoteEditReport;
struct ConditionContext;
struct CompiledCondition;
struct CompiledHarmonyContext;
struct HarmonicPitchRef;
struct CompiledAutomationLane;
struct AutomationCursor;
struct CompiledSceneRoutes;

// Control-side: may allocate; returns structured diagnostics.
SelectionResult resolveNoteSelection(
    const Pattern& pattern, const NoteSelection& selector);

EditResult applyCompositionEdit(
    const Composition& base, json_t* operations, int revision);

// Pure, bounded, allocation-free helpers.
bool evaluateCondition(
    const CompiledCondition& condition,
    const ConditionContext& context) noexcept;

float resolveHarmonicPitch(
    const CompiledHarmonyContext& context,
    const HarmonicPitchRef& pitch) noexcept;

float evaluateAutomation(
    const CompiledAutomationLane& lane,
    double musicalBeat,
    AutomationCursor& cursor) noexcept;
```

Use fixed arrays for per-channel runtime state and per-scene channel routes. Authored vectors/maps and compiled coefficient vectors may live inside the immutable Composition generation. Do not change the publication protocol merely to introduce a separately owned compiled object.

A route may contain a valid track and automation/harmony context even when it has no pattern. Separate output-lane evaluation from the existing early-return path for absent note assignments.

### 11.2 Compilation pipeline

Normalize/migrate → strict validation → ordered edits on private candidate → final reference validation → compile pattern indices → compile conditions → compile harmonic dependencies → compile automation segments/routes → compute adoption dependency masks → accept immutable generation.

Only after every stage succeeds may acceptance occur. Runtime indices are rebuilt after note movement or insertion. Empty patterns must still have a correct rest index and timing duration.

Precompute scene-prefix durations for arrangement time, per-scene/per-channel route tables, harmonic expression lookup indices, condition enums, and automation coefficients on the control side. Normal per-sample execution must not scan all scenes, patterns, events, or curves.

The old edit/parse architecture may combine normalization and compilation internally; the logical stages above do not require a wholesale file/class rewrite.

### 11.3 Per-sample order

Preserve established transport/clock/adoption boundary ordering. Extend the effective route path in this order:

```text
acquire protected immutable generation
apply due transport/adoption and resolve effective scene
update bounded musical coordinates
read macro contributions
for each existing channel:
    process scheduled note onsets using cached route, condition, probability,
        harmonic/static pitch, evolved expression, and scene overrides
    update gate/ratchet/glide using existing contracts
    evaluate independently owned MOD lanes, including patternless routes
write existing outputs and publish bounded telemetry
release/clear appropriate read hazards according to existing ownership protocol
```

Do not retrospectively modify an already latched ratchet or glide because a chord or condition counter changed between its pulses.

### 11.4 Adoption and comparisons

Update authored equality/serialization for every new field. Keep document equality distinct from audible dependency changes: ID normalization alone should not close gates or restart patterns.

Extend note-channel dependency masks for conditions, event transpose, scene expression overrides, and reachable harmony changes. Conservative existing note-gate closure on real note-channel changes is acceptable. Do not claim that `phase_policy:"preserve"` guarantees legato edits.

Use a separate MOD-lane change mask for automation-only edits so filter-curve editing does not stop the bass. Rebase automation cursors and apply the handover rule at adoption. Unchanged lanes retain phase/held state.

Compiled route pointers must be replaced/rebound with their owning Composition generation. Do not retain an old pattern, segment, chord, string buffer, or coefficient pointer after its generation is reclaimable. Extend the existing hazard/owner discipline; no shared-pointer decrement that might destroy a generation may be added to `process()`.

Tests must include future-scene-only edits and replacement of a pending accepted revision. No worker callback may publish a candidate after its expected revision has become stale.

### 11.5 Budgets and numeric rules

Proposed additional limits, advertised in capabilities:

| Resource | Initial bound |
|---|---:|
| Operations per transaction | 256 |
| Selected events per pattern operation | 1024 |
| Condition tests per event | 8 |
| Automation definitions | 512 |
| Points per automation curve | 1024 |
| Total automation points | 16,384 |
| Simultaneously driven MOD lanes | 48 |
| Harmony progressions | 128 |
| Chords per progression | 128 |
| Total chord markers | 4096 |
| Precomputed harmonic expression/chord pitch entries | 1,048,576 |
| Extra compiled storage for these new features | 32 MiB |
| Targeted-read page | 256 events |

Keep existing validated track/pattern/scene limits. Payload limits must not exceed the existing bridge limits or the 16 MiB portable-document ceiling. Budget checks apply before building large derived tables, and the additional-storage budget does not retrospectively reject a legacy score merely for its existing base storage.

Use doubles for new musical coordinates and compiled segment times. Preserve existing scene timing numerics for legacy playback; introducing a new double cache must not subtly change old boundary samples. Use analytic coordinate rebasing for large elapsed times, not an unbounded loop that walks skipped scenes or repeats. New curve/chord evaluators sample the current interval directly when multiple breakpoints fall between samples; they must not replay every skipped breakpoint. Validate that authored adjacent beat coordinates remain distinct in compiled double precision. At very large score positions, use rebased/hierarchical coordinates or reject a new timeline whose smallest interval cannot be represented; do not collapse it silently.

### 11.6 Real-time proof obligations

No allocations, frees, locks, file/network I/O, JSON, regex, voicing search, chord-symbol parsing, or dynamic container growth in `process()`. No per-sample string-based dispatch for the new features.

Bound work by active tracks/lanes, scheduled events, and logarithmic seeks within the stated caps. Normal monotonically advancing automation uses cached segments; jumps may use binary search rather than walking all crossed breakpoints.

Benchmark the baseline and final build on the same machine, build mode, patch, and sample rate. Report median, p95, p99, and worst observed process duration, plus allocations/deallocations and active feature counts. New-feature-disabled performance should remain within measurement noise or have an explicitly explained regression. Do not substitute invented universal microsecond targets for actual measurements.

## 12. File-level implementation plan and agent interface

### 12.1 Existing files to extend

| File | Responsibility in this project |
|---|---|
| `src/SibylTypes.hpp` | Authored identity/transpose/conditions/overrides/harmony/automation types and generation-owned compiled data. |
| `src/SibylJSON.hpp/.cpp` | Version normalization, strict new validators, compile integration, round-trip serializers, targeted/contextual reads. |
| `src/SibylEdit.hpp/.cpp` | Operation dispatch, atomic edit reports, allocator preservation, new collection operations. |
| `src/Sibyl.cpp` | Capabilities, VALIDATE edit preview, runtime cursors, route binding, independent MOD evaluation, persistence and minimal preview adaptation. |
| `src/SibylAdoption.hpp/.cpp` | Musical dependency comparisons, condition rebasing, independent MOD change/adoption masks. |
| `src/SibylTiming.hpp/.cpp` | Shared coordinate/boundary helpers where appropriate; preserve existing onset helpers. |
| `src/SibylEvolution.hpp` | Prefer no behavior change. Consult it to preserve expression/cursor semantics. |
| `src/SibylControl.hpp` | Preserve the semantic operation set and commit-point meaning; document additive request/view support. |
| Existing Sibyl tests and `Makefile` | Add feature fixtures, dependencies, regression coverage, and runnable targets. |

Suggested focused helpers: `SibylNoteEdit`, `SibylConditions`, `SibylAutomation`, `SibylHarmony`, and `SibylVoicing`, each split into header/source only when warranted. Keep pure DSP/selection helpers testable without a live Rack host. Do not force every suggestion into a new class hierarchy.

Locate bridge schemas, MCP wrappers, examples, and the repository's Octavia/Sibyl skill references using repository search before editing them. Their locations were not established by this source review. Update their accepted view/operation shapes and compact usage examples; do not invent a new MCP tool for each transform.

### 12.2 Exact collection-operation payload names

| New operation | Fields in addition to `op` |
|---|---|
| `upsert_automation` | `id`, `automation` (full curve object) |
| `delete_automation` | `id` |
| `upsert_progression` | `id`, `progression` (full progression object) |
| `delete_progression` | `id` |
| `set_default_harmony` | `binding` (object or null; null removes default harmony) |
| `set_scene_harmony` | `scene_id`, `binding` (object or null; null explicitly disables harmony in that scene) |
| `inherit_scene_harmony` | `scene_id` (removes the scene-specific field, restoring inheritance) |

Distinguish disabling from inheritance; do not overload null to mean both. Full object replacements follow the existing upsert convention. ID fields are outside their keyed object, not redundantly embedded inside it.

New GET view names are `notes`, `automation`, `progression`, and `effective_context`. `automation`/`progression` use `id`; `effective_context` requires `scene_id`, zero-based `scene_repeat`, and `beat` within that repeat. For this view, `beat` is in the half-open interval `[0, scene.lengthBeats)` and `scene_repeat` must identify an authored repeat. The view returns authored context and derived base outputs/pitches, not a prediction of a random performance or unsampled physical macro inputs. Existing runtime `sceneRepeat` status numbering remains unchanged; new condition ordinals remain explicitly one-based.

For operation preview, use the normal VALIDATE route. For curve sampling, use the automation view with an optional bounded `sample_beats` array of at most 256 absolute positions in the curve's chosen coordinate. Do not introduce a new rendering/recording endpoint.

### 12.3 Capabilities and documentation

Keep `apiVersion:1`; report maximum supported `schemaVersion:3`, `supportedSchemaVersions:[2,3]`, new view names, edit-operation names, and the precise resource limits.

Feature flags should be versioned and shipped only when functional:

```json
{
  "noteEditing":{"version":1,"stableIds":true,"previewViaValidate":true},
  "conditions":{"version":1,"scopes":["patternPass","sceneRepeat","arrangementLoop"]},
  "sceneOverrides":{"version":1},
  "automation":{"version":1,"lanes":["mod","mod2","mod3"],"modes":["replace","add"]},
  "harmony":{"version":1,"relativePitch":true},
  "voicing":{"version":1,"algorithm":"voice_progression_v1","materializesNotes":true}
}
```

These entries belong inside Sibyl's existing capability envelope. Preserve `repeatEvolution` and existing operations. Do not advertise harmony merely because its types compile, or voicing before its optimizer and capacity tests pass.

Documentation must distinguish authored notes, scene-transformed notes, harmonic-context pitches, and measured sounding state. Include one compact recipe per user outcome and make `expected_revision` + `expect_count` the default targeted-edit workflow.

## 13. Acceptance tests

Tests below are contractual examples. Implement them against real helpers and the existing module harness; syntax-checking example JSON alone is not sufficient.

### 13.1 Identity, codec, and transaction safety

| ID | Given / When | Required result |
|---|---|---|
| C01 | Load the same legacy pattern twice | Same generated IDs and next counter; identical legacy audio at the same sample rate. |
| C02 | Move a note, save, reload, then edit by ID | The same musical event is targeted at its new step. |
| C03 | Delete `n3`, insert another note, reload, insert again | No automatic reuse of `n3`; counter remains monotonic. |
| C04 | Round-trip a note carrying every supported field | Observations, three MOD lanes, evolve flag, condition, transpose, and pitch representation survive. |
| C05 | Validate operations allocating IDs, then commit at the same revision | Preview changes/IDs match commit; preview consumed no IDs or undo entry. |
| C06 | Valid first operation followed by a collision/invalid condition | Entire edit rejected; accepted/active/pending state and allocator unchanged. |
| C07 | Stale expected revision, including after an accepted-but-pending edit | `revision_conflict`; no lost edits or partial application. |
| C08 | Portable v2/v3, bare v2, bare explicit v3, mismatched/future versions | Valid forms normalize; mismatches/future versions fail before publication. |
| C09 | Unversioned input contains a condition or automation | `schema_version_required`, not a warning followed by dropped music. |
| C10 | Undo/redo a multi-operation expressive edit | One undo unit; complete authored feature state restored; coherent revisions. |
| C11 | ID-only canonicalization or unchanged edit | No unintended gate restart from identity alone. |
| C12 | Auto-ID counter meets occupied custom IDs or its maximum | Skip occupied IDs deterministically; exhaustion fails without wrapping or partial acceptance. |
| C13 | Reserved future feature field during staged rollout, or a misspelled new field | `unsupported_feature` or strict field error; no silent discard. |

### 13.2 Selection and transforms

| ID | Given / When | Required result |
|---|---|---|
| N01 | Notes at steps 0, 4, 8, 12, 15; select last four and transpose −12 | Step 0 unchanged; remaining four offset by exactly −1 V; IDs/expression intact. |
| N02 | Mixed degree/note/voltage/harmonic selection; semitone transpose | All retain their pitch representation. |
| N03 | Same mixed selection; degree transpose | Atomic `unsupported_pitch_transform`. |
| N04 | Ghost velocities 0.2, 0.35, 0.8 with probabilities 0.5, 1, 1; multiply selected probabilities by 0.6 | First two become 0.3 and 0.6; accent stays 1. |
| N05 | Unset velocity/gate on a shared pattern; numeric adjust | Fails without explicit track-default context; materializes correct value with it. |
| N06 | Rotate selected steps 0, 2, 14 by +2 in length 16 | Destinations 2, 4, 0; simultaneous movement succeeds. |
| N07 | Destination occupied by an unselected note | Default fails; explicit replace deletes exactly the reported destination. |
| N08 | Duplicate near boundary | Default rejects overflow; explicit wrap produces documented destinations/fresh IDs. |
| N09 | Selection cardinality changes unexpectedly | `expect_count` prevents the edit. |
| N10 | Unset probability after previously setting it | Probability returns to default 1, not zero. |
| N11 | Update a pitch representation | Mutually exclusive pitch fields cleaned atomically; unrelated expression intact. |
| N12 | Reuse a notes-view cursor after editing | Revision conflict rather than shifted or duplicate pagination. |

### 13.3 Conditions and musical timing

| ID | Given / When | Required result |
|---|---|---|
| R01 | every 4 / offset 4 across 12 pattern traversals | Eligibility only on 4, 8, 12. |
| R02 | first-pass event over several passes and an explicit pattern restart | Plays on the first pass each time the specified cursor resets. |
| R03 | Scene with 4 repeats and last-repeat condition | Eligible only during repeat 4; single-repeat scene is both first and last. |
| R04 | Four-beat pattern inside a 16-beat scene | Four pattern passes occur within one scene repeat. |
| R05 | All probabilities zero, then inspect counters | Counters advance with time despite silence. |
| R06 | Scene's first repeat silent; second repeat enabled | Eligibility is not delayed by the missing earlier notes. |
| R07 | Negative microshift on next-cycle step zero | patternPass uses its nominal next traversal; sceneRepeat uses scheduled-onset interval. |
| R08 | restart/continue/alignGlobal and pattern replacement | Exact counter/reset table in §6.2 is satisfied. |
| R09 | Last-repeat fill followed by an unexpected manual scene jump | No retroactive fill; only authored-repeat semantics were promised. |
| R10 | Condition false with a positive probability macro and ratchets | No ratchet fires; condition is not bypassed. |
| R11 | Condition-false observation/tie | No observation; no phantom tie extension or resurrected gate. |
| R12 | Reseed/randomness-only restart | Existing RNG behavior changes/resets as before; condition time does not reset. |
| R13 | Legacy evolution with and without unrelated new features | First-pass and deterministic later-pass expression goldens preserved. |
| R14 | Conditions reject all authored events for several passes | Legacy evolution observation still runs; condition and evolution counters are not accidentally coupled. |
| R15 | Positive probability-evolution depth across repeats, restart, and save/load | Baseline threshold/draw sequence is preserved; first traversal remains authored and restart/reload reproduces the sequence. |
| R16 | Omitted/zero probability depth, or `evolve:false`, with other new features present | Baseline legacy thresholds/draws remain unchanged; enabling other evolution lanes does not renew probability draws. |
| R17 | Conditions reject events during opted-in probability evolution | Evolution still observes scheduled events; later eligible events use the same evolution pass and draw as the baseline at that traversal. |

### 13.4 Scene overrides and automation

| ID | Given / When | Required result |
|---|---|---|
| V01 | One pattern reused by verse and octave-up chorus | Pattern contents unchanged; chorus differs by exactly +1 V. |
| V02 | Velocity 0.6, scale 1.2, offset 0.1, macro 0.1 | Effective velocity 0.92, output 9.2 V within float tolerance. |
| V03 | Probability 0.5, scale 0.8, offset 0.1, macro 0.1 | Effective probability 0.6; false conditions still exclude the event. |
| V04 | Partial assignment update on a phase-overridden assignment | Phase and unspecified overrides preserved. |
| V05 | Authored probability 0.5 evolves to 0.7; scene scale 0.8, offset 0.1, macro 0.1 | Effective probability is 0.76, not 0.6; identity overrides retain the probability-evolution baseline. |
| A01 | 0→5 V linear curve over 32 beats | 0, 1.25, 2.5, 3.75, 5 V at coordinates 0, 8, 16, 24, 32 where in scope/terminal hold. |
| A02 | Same curve with all notes rejected | Same curve samples as A01. |
| A03 | Existing track but no scene pattern assignment | Gates remain low; MOD curve still moves. |
| A04 | 8-beat scene repeated four times; sceneVisit vs sceneRepeat clocks | Visit curve spans 32 beats; repeat curve resets every 8 beats. |
| A05 | replace mode with conflicting explicit note MOD values | Curve retains ownership. |
| A06 | add mode: raw event base 1 V, curve 2 V, scene offset 0.5 V, macro 0.25 V | Output 3.75 V; offsets are not double-counted. |
| A07 | No active curve | Legacy note-latched MOD/macro output reproduced. |
| A08 | Scene-specific curve over arrangement curve | Scene curve owns the whole visit; duplicate same-scope target rejected. |
| A09 | Pause, external hold, jump, reset, arrangement loop, terminal stop | Coordinate and endpoint rules match §8.5. |
| A10 | Automation-only edit halfway through a ramp | New curve sampled at current position; note gate does not close. |
| A11 | Nonzero handover and repeated edits | Starts from actual last output; reaches moving target at defined duration. |
| A12 | Invalid duplicate points, NaN, out-of-range values, unresolved target | Atomic validation failure. |
| A13 | Enable add-mode automation midway through a held legacy note | Raw authored/evolved MOD base is available; no loss or double application of previously sampled macros. |

### 13.5 Harmony and voicing

| ID | Given / When | Required result |
|---|---|---|
| H01 | C3 major → A2 minor; root-relative event | Effective roots C3 → A2 without modifying the pattern. |
| H02 | Same progression; `role:"third"` | E3 → C3 before event/scene transpose. |
| H03 | Third-relative event against a suspended/no-third chord | Explicit validation failure, not an invented third. |
| H04 | Nearest reference D3 in C major, lower tie-break | C3 chosen over equally distant E3. |
| H05 | Same nearest reference with higher tie-break | E3 chosen. |
| H06 | Root/chord changes while an event sustains or ratchets | Existing latched pitch remains; next event uses current chord. |
| H07 | Microshift just before and exactly on chord change | Scheduled onset selects old/new chord respectively. |
| H08 | Harmonic pattern assigned without effective harmony | Rejected; unassigned bank pattern can remain with warning. |
| H09 | Scene binding disabled vs inherited | Null disables; removal restores default inheritance. |
| H10 | Progression edit and dependency mask | Harmonic tracks affected; fixed-pitch percussion not harmonically changed. |
| H11 | Same note/chord fixture serialized with reordered object keys | Identical resolved pitches; no unordered-container dependence. |
| W01 | Valid voicing request | In-range, noncrossing, deterministic notes across existing tracks. |
| W02 | Common tones can be retained with no coverage loss and lower motion | Optimizer retains them according to the versioned objective. |
| W03 | Impossible register/coverage constraint | Atomic `voicing_unsatisfiable`; no destination patterns created. |
| W04 | Off-grid chord marker or length >1024 requested steps | Rejected without implicit quantization. |
| W05 | Search/compile capacity exceeded | Structured error, not truncated “optimal” output. |
| W06 | Progression changed after materialization | Baked notes remain unchanged; relative notes follow new harmony. |
| W07 | Future large leap equalizes two prefixes with different earlier maximum leaps | Staged solver returns the globally correct lower-motion result, not a prematurely pruned path. |

### 13.6 Integration and nonfunctional gates

Build a fixture combining a shared harmonic bass pattern, chorus octave override, every-fourth-pass accents, probability-evolving hats, a protected final-repeat fill, and a 32-beat MOD sweep. Preview, commit at nextBeat, edit again while pending, save/load, and undo/redo. Check event IDs, pitches, eligibility, gates, and MOD values at named musical positions.

Run legacy and new sample harnesses at 44.1, 48, and 96 kHz. Compare legacy output sample-for-sample against the baseline **at the same sample rate**. Compare cross-rate musical timing within one sample at each rate, not by demanding identical raw sample arrays across different rates.

Instrument allocation and deallocation during ordinary processing, adoption, scene changes, skipped events, chord boundaries, curve boundaries, and stale-generation reclamation. Every audio-thread count must stay zero. Stress simultaneous control edits and processing with the existing thread-sanitizer target where supported; include an address/undefined-behavior sanitizer pass where the toolchain supports it.

Exercise maximum track/channel and representative high-density pattern/curve cases. Bounded seek and table-capacity tests must demonstrate that increasing inactive scene/curve count does not add a full scan to ordinary per-sample processing.

Update the module harness so advancing samples carry realistic increasing frame indices when observation timing is asserted; do not accidentally test every event at frame zero.

## 14. Milestone deliverables and verification

| Milestone | Exit criteria |
|---|---|
| P0 | Record baseline/source revision; freeze golden fixtures; normalize codec entry points; migration/round-trip tests green. |
| P1 | All note operations, targeted reads, compact reports, and VALIDATE preview usable through existing bridge routes; C/N tests green. |
| P2 | Conditions and separate counters integrated with timing, ties, telemetry, and transport; R tests green. |
| P3 | Scene overrides, partial assignment edits, and change detection working; V tests green. |
| P4 | Independent curves, scope clocks, mode ownership, handover, and MOD-only adoption working; A tests green. |
| P5 | Progressions, relative pitches, context views, and harmonic dependency validation working; H tests green. |
| P6 | Bounded voicing materialization, W/integration tests, documentation, and final performance comparison complete. |

Stage feature-dependent test rows with the feature they exercise. For example, P1 runs mixed static-pitch N tests; their harmonic variant becomes mandatory in P5. P0/P1 codec tests cover fields implemented so far, then gain full-feature fixtures as each milestone lands. Do not advertise unsupported later features to make an early test pass.

Routine validation starts with focused targets and `make test-fast`, using the environment documented by the current `AGENTS.md`. The reviewed tree includes `test-sibyl-tsan`. Verify actual target names after adding helper source dependencies. [S11, S12]

Native Windows/MSYS2 verification uses the matching MINGW64/Rack environment and a real plugin DLL build; Linux verification uses a matching Linux SDK. Do not label a Linux-only build as authoritative Windows validation. Keep Rack-linked runtime resolution consistent with the repository instructions. [S12]

For every milestone, report changed files, implemented operations/fields, commands run, actual pass/fail/skip results, remaining known limitations, and measured performance when applicable. Do not claim live Rack, Windows, sanitizer, or listening validation unless it actually ran.

## 15. Non-goals and deliberate exclusions

No full piano-roll editor, live MIDI capture, new audio outputs, per-track polyphonic note arrays, adaptive temperament, arbitrary automation of other Rack modules, tempo maps, LLM execution in DSP, probabilistic harmony inference, new bridge protocol, or renderer redesign.

Do not silently replace the old RNG, change the meaning of scene phase modes, auto-clone shared patterns, rewrite every pattern when a scene override changes, or generate thousands of artificial note events to approximate a sweep.

Persistent linked generated voicings, circular voicing optimization, event-condition OR/NOT trees, live fill controls, full expression scripting, and automation breakpoint surgery can be later extensions. They are not prerequisites for the five musical capabilities requested here.

### 15.1 Companion worked example

`Sibyl_v3_Example_Composition.json` accompanies this specification. It is a **proposed schema-3 fixture**, not a file for the current schema-2 release. It combines a harmonic bass, probability-evolving hats, a protected last-repeat fill, an octave-up chorus, a 32-beat sweep across four scene repeats, and a patternless automation track. Use it for future parser/module tests after implementing the required features. Its syntax and internal references were checked during document preparation; it has not been run through an implemented v3 Sibyl compiler or heard in Rack.

## 16. Suggested first Codex instruction

> Implement P0 and P1 of this specification against the current `lumin-render` checkout. Read AGENTS.md, record HEAD and working-tree changes, and inspect the actual Sibyl/Octavia interface before editing. Preserve unrelated rendering work. Add the codec/identity foundation, targeted note operations, selectors, compact change reports, and edit preview via VALIDATE; do not implement harmony or automation yet. Keep the existing atomic accepted/active revision and undo contracts. Add and run the relevant C/N tests plus the existing focused Sibyl tests and test-fast, then report changed files, exact verification results, and any deviations. Do not stage or commit.

## 17. Reviewed source manifest

These URLs were read during specification preparation. All are mutable branch references. The local checkout and its recorded commit are the authority for implementation. Line locations are useful starting points, not permanent anchors.

- **[S1]** `src/SibylTypes.hpp` — StepEvent, Pattern, TrackAssignment, Composition.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylTypes.hpp`
- **[S2]** `src/SibylEdit.cpp` — operation dispatch and applyCompositionEdit round trip.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylEdit.cpp`
- **[S3]** `src/SibylJSON.cpp` — schema validation, static pitch helpers, serializers.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylJSON.cpp`
- **[S4]** `src/Sibyl.cpp` — state/persistence; routing/adoption; event loop around lines 1267–1410; request handling around 1470–1750.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/Sibyl.cpp`
- **[S5]** `src/SibylEvolution.hpp` — EvolutionCursor and evolveExpression.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylEvolution.hpp`
- **[S6]** `src/SibylAdoption.cpp` — sameEvent, samePattern, channel change masks.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylAdoption.cpp`
- **[S7]** `src/SibylControl.hpp` — semantic bridge and EDIT commit contract.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylControl.hpp`
- **[S8]** `src/SibylTiming.cpp` — sparse lookup and scheduled timing.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylTiming.cpp`
- **[S9]** `tests/sibyl_edit_spec.cpp` — existing atomic edit test structure.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/tests/sibyl_edit_spec.cpp`
- **[S10]** `tests/sibyl_module_spec.cpp` — module harness, evolution/allocation/transport/tie tests.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/tests/sibyl_module_spec.cpp`
- **[S11]** `Makefile` — Sibyl test/build dependencies and sanitizer target.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/Makefile`
- **[S12]** `AGENTS.md` — build environment, performance, compatibility, and no-commit instructions.  
  `https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/AGENTS.md`

