# Sibyl â€” AI-First Sequencing Reference

Read this reference whenever a request involves composing, sequencing, arranging, or
controlling a Leviathan:Sibyl module.

> [!TIP]
> **Fast Token-Efficient Authoring:** For rapid session orientation, P8 columnar note batches, structural scene reuse, two-phase commit (`prepare: true` -> `handle`), and minimal receipts, consult [`sibyl_cheatsheet.md`](sibyl_cheatsheet.md). Use this document for detailed musical semantics, condition scopes, microtonal tuning systems, and harmonic solver rules.

## P8 Token-Efficient Authoring Index

- **Session Bootstrap:** `vcv_sibyl_bootstrap` provides compound discovery and consistency-checked orientation in a single read.
- **Columnar Event Batches:** `insert_note_batch` with `columns_v1`, arithmetic `onsets`, and sparse `overrides`.
- **Structural Reuse:** `clone_scene` with `patterns: "share"` or `"copy"`, and `clone_pattern`.
- **Two-Phase Commit:** `vcv_sibyl_validate(prepare=true)` returns `handle`, committed atomically via `vcv_sibyl_edit(handle=...)`.
- **Response Profiles:** `response_profile: "receipt"` reduces edit responses to minimal aggregate counts.
- For complete operational schemas and examples, see [`sibyl_cheatsheet.md`](sibyl_cheatsheet.md).


## Product Intent and Routing Policy

Sibyl is a headless, machine-first polyphonic sequencer and arranger. It is the default
choice when an AI agent is expected to author or revise musical material. Unlike a panel
step sequencer, Sibyl exposes semantic objectsâ€”tracks, patterns, scenes, macros, clock,
and transportâ€”through validation, revision-guarded atomic edits, and musical adoption
boundaries.

Do not substitute Octavia's `Control A` or `Control B` outputs for Sibyl when authoring
musical material. Those outputs are reserved for temporary, bounded diagnostic stimuli
that require exact alignment with an Octavia recording or analysis capture.

Each step can author three independent modulation lanes: `mod` (panel MOD1), `mod2`, and
`mod3`. Schema v2 expresses every lane directly in modular volts from -10 V to +10 V;
there is no track-level range transform. Use separate lanes when a composition should
sequence multiple parameters or stages of a modular voice.
Macro amounts and clamps targeting these lanes are likewise expressed in volts.

Gate values are durations measured in pattern steps. Use values above `1` for sustained
notes instead of emitting redundant tied events. Reserve `tie: true` for legato continuation
or pitch changes without retriggering. Ratcheted events require an explicit gate no greater
than `1`, because their gate fraction applies within each ratchet slice.

Scenes may carry an optional `description` containing their musical intent. Prefer a
concise description that explains the section's role rather than restating its name or
enumerating its patterns. Sibyl displays the active scene description in its lower text
box and falls back to the composition-level `meta.prompt` when it is absent.

An event may schedule an exact-frame Octavia observation of physically cabled monitor
inputs:

```json
"observation": {
  "octaviaModuleId": 1234,
  "monitors": ["A", "B"],
  "preFrames": 4800,
  "postFrames": 12000,
  "label": "filter transition"
}
```

Resolve the Octavia module ID from the live patch; never guess it. Valid monitor names are
`masterL`, `masterR`, `A`, `B`, `C`, and `D`. The combined frame window must fit Octavia's
262144-frame rolling history. The marker publishes when the event passes its probability
check and sounds, at the event's swing/microshift-adjusted onset, and only once even when
the event ratchets. Physical cables determine what is captured. Use sufficient post-roll
for envelopes, effects, and buffered processors whose audible response follows the event.

Prefer Sibyl when the user asks the agent to:

- compose melodies, bass lines, rhythms, harmonies, or complete pieces;
- create or revise patterns and named song sections;
- arrange repetitions, transitions, or multi-track material;
- make structural musical changes during playback;
- produce deterministic compositions that another agent can inspect and continue.

Prefer a conventional visible sequencer when the user explicitly wants to:

- enter or edit individual steps directly on the Rack panel;
- manipulate the sequence manually as the primary workflow;
- see a grid or playhead for learning or performance;
- preserve an existing sequence authored in another sequencer.

Do not dismantle a user's existing sequencing workflow solely to substitute Sibyl. When
the user has not selected a sequencing interface and expects the agent to compose, Sibyl
is the default.

## Standard Agent Workflow

1. Call `vcv_get_status` once to verify Octavia.
2. Call `vcv_list_modules` and locate `Leviathan:Sibyl`. Never guess its module ID.
3. Call `vcv_sibyl_get_capabilities` for that module.
4. Read `vcv_sibyl_get_composition(view="summary")` and `vcv_sibyl_get_status`.
5. Use the latest accepted `revision` as `expected_revision` for an edit.
6. Validate unfamiliar, broad, or structural candidates before editing.
7. Send one coherent atomic `vcv_sibyl_edit`; prefer semantic operations over replacing
   the whole composition.
8. For quantized changes, poll status for a bounded interval. If `activeRevision` still
   lags, report the pending revision and boundary rather than waiting indefinitely. A
   successful response does not imply the edit is sounding yet.
9. Re-read the affected pattern, scene, or summary to verify the result.

Use `nextScene` for major structural rewrites only when transport is expected to reach
another scene boundary. Use `nextBeat` for responsive musical edits and `immediate` for
emergency or explicitly requested changes. Default to `preserve` phase policy unless the
musical intent requires changed patterns or the entire arrangement to restart.

## Evolving Repeats

Check `capabilities.sibyl.repeatEvolution.version == 1` before authoring this
extension; older builds can warn and ignore unknown fields. It is opt-in on each
pattern and does not change pitches, event positions, ties, or
ratchet counts. Existing compositions retain their previous playback behavior.

Add an `evolution` object to a full pattern supplied through `upsert_pattern`:

```json
{
  "op": "upsert_pattern",
  "id": "acid",
  "pattern": {
    "length": 16,
    "resolution": "1/16",
    "evolution": {
      "probability": 0.2,
      "velocity": 0.12,
      "gate": 0.15,
      "glideMs": 35,
      "mod": 0.35,
      "mod2": 0.5,
      "mod3": 0
    },
    "steps": [
      {"step": 0, "note": "E2", "gate": 0.6, "velocity": 0.9, "evolve": false},
      {"step": 3, "note": "G2", "gate": 0.7, "velocity": 0.75, "probability": 0.7},
      {"step": 7, "note": "B2", "gate": 0.65, "glideMs": 60},
      {"step": 11, "note": "D3", "gate": 0.5, "mod": 1.5}
    ]
  }
}
```

This is a complete illustrative pattern, not a partial update. To enhance an
existing pattern, read it first and preserve its events, resolution and length
when adding `evolution`. Validate the candidate, use the latest revision, then
verify the accepted pattern. Use `restartChanged` if the user wants to hear the
authored first pass before evolution begins; otherwise prefer `preserve`.

Each depth is the maximum **plus/minus offset** from its authored value, not a
cumulative mutation. Omitted/zero depths disable that lane. Ranges are probability and velocity
0â€“1, gate 0â€“1024 steps, glideMs 0â€“3600000 ms, and each modulation depth 0â€“20 V.
Use small musical depths; these are bounds, not recommended defaults.
Probability, velocity, gate and glide vary independently per event and pass. Each modulation
lane uses one offset across the pass, preserving its relative authored contour.
Results clamp to valid output ranges; ratcheted gates never exceed one step and
authored zero-length gates remain zero. Existing macro modulation applies after
evolution. An absent modulation value has a 0 V base and can still evolve.
A zero authored glide can become a slide; negative results clamp to 0 ms.
Set `evolve:false` on anchor events to preserve all of their authored expression
(their normal macro inputs still apply).

The first scheduled traversal after reset is authored. Subsequent traversals use
seeded variation derived from `meta.seed`, randomness epoch, track channel, pass,
event and lane. No new notes are generated.
These expressive values affect sound only where their Sibyl outputs are patched;
for example, velocity needs a velocity-sensitive destination.

`status.evolutionPasses` maps track IDs to zero-based runtime pass counters. A pass
advances when the next scheduled event belongs to a new pattern cycle, or when an
automatic scene entry restarts/realigns that track's phase. Scene repeats and
arrangement loops therefore keep moving, including a one-scene looping patch.
Continue-phase scene entries retain the ongoing pattern traversal. Empty/muted
tracks have no scheduled events and do not advance. Counters belong to channels,
not individual pattern IDs, so an automatic scene change does not reset them.
A changed-length replacement under `preserve` rebases phase without inventing an
extra variation pass.

Explicit stop/reset, scene selection/restart, pattern restart, randomness restart,
and adoption with `restartChanged`/`restartAll` reset the affected evolution
cursors; pause/play preserves them. Full arrangement reset also resets the
randomness epoch. With restart-phase tracks, this reproduces the same phrase and
variation sequence; continue/alignGlobal phase policies retain their existing
timing semantics. Reseed changes the
variation seed without restarting phase. Runtime counters are not saved: reload
starts from the authored first traversal. An early microshifted event belongs to
its nominal pattern cycle, even when scheduled before that cycle's grid boundary.

## Composition State Versus Runtime State

These controls are intentionally distinct:

| State | Persistent | Revisioned | Meaning |
|---|---:|---:|---|
| Composition `transport.running` | Yes | Yes | Whether a saved/reloaded composition starts playing |
| Runtime play/pause/stop | No | No | Live performance state only |
| Composition `transport.loop` | Yes | Yes | Whether the arrangement wraps after its final scene |

Reload starts at the beginning using saved `transport.running`; an MCP client is not
required for playback. With `transport.loop: false`, Sibyl closes gates and stops after
the final scene. Runtime transport commands do not alter the saved composition or create
an undo entry.

## Clock and Hardware Precedence

- With CLOCK unpatched, Sibyl runs from composition `meta.bpm`.
- With CLOCK patched, external pulses drive time.
- After `externalTimeoutMs` without a pulse, `onExternalStop` controls behavior:
  `hold` preserves position, `freeRun` continues from observed external timing, and `internal`
  falls back to `meta.bpm`.
- With RUN patched, its voltage determines effective play/pause state and takes precedence
  over API runtime commands.

When diagnosing unexpected stopping, inspect `transport.loop`, `transport.running`, CLOCK
and RUN connections, `externalTimeoutMs`, `onExternalStop`, the current scene, and status
`running` before changing anything.

## Revision and Transaction Rules

- Always obtain the latest revision immediately before an edit.
- A stale `expected_revision` must be re-read and reconsidered, not blindly retried.
- `revision` is the latest accepted composition; `activeRevision` is currently sounding.
- Pending quantized edits are normal and survive runtime transport commands.
- Group dependent changes in one transactionâ€”for example create a replacement pattern,
  reassign its scene, then delete the old pattern atomically.
- Prefer focused semantic operations. Use `replace_composition` chiefly for initialization,
  deliberate full replacement, restoration, or contract testing.

## Integration-Test Hygiene

Tests that replace the composition can leave Sibyl configured to start automatically,
stop after a non-looping arrangement, or use special clock policies if the Rack patch is
later saved. At the end of an invasive live test, either restore the prior composition or
explicitly tell the user what state remains. Never save the Rack patch without permission.

## Common Failure Interpretations

- `revision_conflict`: another edit was accepted; re-read before deciding what to change.
- `object_in_use`: update or remove references in the same atomic transaction.
- Accepted edit with `activeRevision` lag: wait for its adoption boundary.
- Playback begins on Rack load without a client: saved `transport.running` is true.
- Playback stops at the final scene: `transport.loop` is false.
- Playhead freezes with CLOCK patched: inspect external timeout policy and clock signal.
- API play appears ineffective with RUN patched low: hardware RUN has precedence.

### Probability evolution

Before authoring `evolution.probability`, check that
`capabilities.sibyl.repeatEvolution.fields` contains `probability` (version 1
alone does not guarantee this extension). A step's `probability` is its trigger
chance from 0 to 1, defaulting to 1. Pattern `evolution.probability` is a maximum
plus/minus offset from that chance, from 0 to 1. For example, a step probability
of 0.7 and evolution depth of 0.2 vary the threshold between 0.5 and 0.9.

The first traversal uses the authored threshold and original seeded draw.
Subsequent traversals with positive probability depth use independent seeded
threshold variation and a fresh seeded trigger draw. Automatic scene repeats and
arrangement loops therefore vary note presence; restart reproduces the sequence.
Macro probability offsets apply after evolution, clamped to 0–1. One draw governs
the entire event, including its ratchets and observation marker.

Omitted/zero probability depth retains the previous trigger decisions, even when
other expression lanes evolve. `evolve:false` preserves both the authored chance
and original draw (macro offsets still apply). Use probability 1 with this flag
for dependable anchors. Evolving probability 0 can become audible, and evolving
probability 1 can skip; use zero gate or event protection for deliberate silence.

## Schema 3 note editing (P0–P1)

Check `capabilities.sibyl.noteEditing.version == 1`. Sibyl accepts schema 2 and 3,
migrates old notes to stable pattern-local IDs, and saves canonical schema 3.
A new binary loads old compositions without altering playback; old binaries are
not guaranteed to understand new documents. The proposed full v3 example in
`doc/` is supported through P6. Deterministic edit-time voicing materialization is available.
Probability evolution is retained from schema 2.

Read `view="notes"` with `pattern_id`, optional `selector`, `fields` (`"full"`
or a list), `page_size` (1–256), and returned `cursor`. Default selection is all
notes. Cursors bind the accepted revision and query; re-read after any edit.
`effectivePitchV` in a field list adds a separate `derived` object for static
pitch including event transpose. Full fields return authored values only.

Use the latest accepted revision and `expect_count` for targeted changes:

```json
{
  "expected_revision": 42,
  "operations": [{
    "op": "transpose_notes",
    "pattern_id": "bass",
    "selector": {"order": "stepDesc", "limit": 4},
    "expect_count": 4,
    "semitones": -12
  }],
  "return_changes": true
}
```

Send this through VALIDATE for preview. The MCP wrapper accepts `operations`,
`expected_revision`, and `return_changes` directly, or preserves the old
`candidate` payload form. Preview consumes no IDs, revision, or undo unit.
Submit the same operations through EDIT, omitting `return_changes` and supplying
the desired `apply_at`/`phase_policy`; a valid edit remains one undo unit.

Available operations: `update_notes` (`set`, `unset`, numeric `adjust`),
`insert_notes`, `delete_notes`, `transpose_notes` (semitones or degree-only
`degrees`), `rotate_notes`, and `duplicate_notes`. Semitone transpose adds an
offset and preserves the original note/degree/voltage representation.
Selectors intersect IDs, steps, `[start,end)` step ranges, and typed predicates;
`all:true` explicitly selects the whole pattern. No unrestricted expression DSL.

For ghost notes use `where:{"velocity":{"lt":0.4}}` and
`adjust:{"probability":{"multiply":0.6}}`. Missing velocity/gate do not match
numeric predicates. Adjusting those inherited values requires
`resolve_defaults_for_track`. `unset` restores inheritance/defaults.

Rotation preserves IDs; duplication generates new IDs. Both reject collisions
unless `collision:"replace"` is explicit. Duplicates default to no wrapping;
use `wrap:true` deliberately. Duplicating observation markers emits a warning;
`copy_observations:false` strips them. Request `return_id_mapping:true` for the
source-to-created mapping. Reports bound IDs to 128 by default; operation
`report_id_limit` allows 1–1024. Entire transactions are limited to 256 operations.
Use `changes` counts and truncated-ID totals to verify edits without rereading a
whole composition. Expected empty mutation selections fail unless
`allow_empty:true` is explicit. Numeric clamping requires `clamp:true`.


## Deterministic repeat conditions (P2)

Check `capabilities.sibyl.conditions.version == 1` before authoring conditions.
Schema 3 events accept `condition:{"all":[...]}`: an AND of at most eight tests.
An absent condition or an empty `all` array is always eligible. Each test has a
`scope` (`patternPass`, `sceneRepeat`, or `arrangementLoop`) and either `every`
(integer 1-1024) with optional `offset` (1-every, default 1), or `is:"first"`.
`is:"last"` is available only for sceneRepeat and means the final authored repeat.
Unknown keys, mixed test forms and non-integer counts are rejected.

For an accent on passes 4, 8, 12:
```json
{"op":"update_notes","pattern_id":"hats","selector":{"ids":["n4"]},
 "set":{"condition":{"all":[{"scope":"patternPass","every":4,"offset":4}]}}}
```
For a protected final-repeat fill, set both
`condition:{"all":[{"scope":"sceneRepeat","is":"last"}]}` and `evolve:false`.
Conditions still apply to protected events. Use `unset:["condition"]` to remove
eligibility restrictions. Insert, duplicate, full/notes reads and portable/patch
serialization preserve conditions through the normal composition codec.

`conditionPasses` and `arrangementLoop` in status are one-based; existing
`evolutionPasses` and `sceneRepeat` retain their original numbering. Counters
saturate at 4503599627370495. Pattern passes advance with musical time even for
empty or silent patterns. Negative microshift uses the event's nominal pattern
traversal but the scene repeat containing its scheduled onset. Conditions gate
the entire event (including ratchets and observation); macros cannot bypass
them. Rejected ties do not extend a previous note or resurrect a closed gate.

Ordinary scene repeats keep pattern time. Destination restart starts pass 1;
continue retains the same pattern's pass, or rebases a different pattern's
current partial traversal to pass 1. alignGlobal uses the global nominal cycle.
Explicit pattern/scene/arrangement restarts reset condition passes. Preserve
adoption retains the current pass while rebasing timing; restartChanged and
restartAll reset affected phases and passes. Pause/external hold freezes them;
reseed/randomness restart does not reset them. Only automatic arrangement wrap
increments arrangementLoop; arrangement reset/stop returns it to 1.

Evolution still observes every scheduled event, including condition-rejected
events. Conditions do not replace its counter, threshold variation or RNG key.


## Scene assignment variations (P3)

Check `capabilities.sibyl.sceneOverrides.version == 1`. Reuse a shared pattern
with scene-local `overrides` rather than cloning or rewriting its notes:

```json
{"op":"update_scene_assignment","scene_id":"chorus","track_id":"bass",
 "set":{"overrides":{"transposeSemitones":12,"velocityScale":1.15}}}
```

`update_scene_assignment` edits an existing assignment. A legacy string is
promoted to an object; `set.pattern` and `set.phaseMode` replace only those fields.
Named `set.overrides` leaves merge with existing leaves. Use `unset:["phaseMode"]`,
`unset:["overrides"]`, or a leaf such as `unset:["overrides.velocityScale"]` to
restore defaults. Setting and unsetting overlapping fields is rejected.

`set_scene_assignment` takes `assignment` as a complete object, pattern-ID string,
or null (remove the route). Use it for creation or intentional full replacement.
Both operations require `scene_id` and `track_id`, accept only documented fields,
and participate in normal revision-guarded EDIT / nonmutating VALIDATE preview.
The legacy `set_scene_track` still replaces the whole assignment; its warning
contains `assignment_attributes_cleared` if phase/override attributes were lost.

Override fields and bounds:
- `transposeSemitones`: integer -120 to 120, added to event transpose; effective
  assigned pitches must remain within -10 to 10 V.
- `velocityScale`, `probabilityScale`, `gateScale`: 0 to 4, default 1.
- `velocityOffset`, `probabilityOffset`: -1 to 1, default 0.
- `gateOffset`: -1024 to 1024 pattern steps, default 0.
- `modOffset`, `mod2Offset`, `mod3Offset`: -20 to 20 V, default 0.

Evolution runs first, then scene scale/offset, then sampled performance macros.
Velocity/probability clamp to 0-1 and MOD lanes to -10 to 10 V. Gate retains live
macro timing and existing ratchet/non-ratchet bounds. A nonidentity gate transform
with nonpositive gate after the live macro remains silent, including ties and
ratchets. Absent/identity overrides preserve the legacy zero-gate minimum pulse.
Conditions remain an independent eligibility stage and cannot be bypassed.

Pattern and ordinary scene reads stay authored and compact. For a static scene
projection, GET `view:"scene"`, `id:"chorus"`, `fields:["effectiveExpressions"]`.
`derived.assignmentNotes` maps track IDs to note IDs/steps and effective pitch,
velocity, probability, gate duration and MOD values. These are authored/pass-zero
values with scene overrides, before evolution and live macros, without predicting
condition/probability outcomes. Full scene data remains under `scene` separately.
For time-specific harmonic pitches use `view:"effective_context"` as described below.


## Independent MOD automation (P4)

Check `capabilities.sibyl.automation.version == 1`. Schema 3 accepts an
`automation` map keyed by stable IDs. Use `upsert_automation` with `id` and a
complete `automation` object; `delete_automation` takes `id`. Both participate
in revision-guarded edits and nonmutating VALIDATE previews.

```json
{"op":"upsert_automation","id":"rise","automation":{
  "target":{"track":"bass","lane":"mod"},
  "scope":{"scene":"chorus"},"clock":"sceneVisit",
  "mode":"replace","transitionMs":20,
  "points":[{"beat":0,"value":0},{"beat":32,"value":5}]}}
```

Targets are existing tracks and `mod`, `mod2`, or `mod3`. Patternless tracks
can drive automation. Scene clocks use `sceneRepeat` (resets each repeat) or
`sceneVisit` (spans all repeats). An arrangement curve uses
`scope:{"arrangement":true}` and `clock:"arrangement"`; its position follows the
score, including manual scene jumps. Scene curves override arrangement curves
for their entire visit. Duplicate enabled owners at the same specificity fail.

Points use finite beats and volts (-10 to 10), beginning at beat zero in strictly
increasing order through the scope endpoint. Left-point `shape` selects `step`,
`linear` (default), or `smoothstep`. A single point is constant; the final value
holds. A nondefault final shape produces an unused-field warning.

`replace` (default) uses curve + scene offset + live macro. `add` includes the
held authored/evolved event MOD base once. Rejected notes leave that base alone;
new routes/restarts clear it and continuing the same route preserves it. Curves
keep moving through skipped notes. Pause/external hold freezes their coordinate,
while owned-lane macros remain live. Unowned lanes retain note-latched behavior.

`enabled` defaults true. `transitionMs` defaults zero and accepts 0-1000 ms.
Incoming ownership/edit transitions blend from the last emitted voltage toward
the moving target; removal uses the outgoing duration. Automation-only edits
preserve note gates and current musical position under phase preservation.

GET `view:"automation"`, `id:"rise"`, with optional
`sample_beats:[0,8,16,24,32]` returns authored data and separate derived samples,
duration and segment count. Samples are curve values, before event bases,
scene offsets, macros and output limiting. Summary reads include automation
counts and targets. Limits: 512 curves, 1024 points per curve, 16384 total points,
48 output lanes and 256 requested sample coordinates. Read current capabilities
for limits rather than assuming future versions retain them.


## Harmonic progressions and relative pitches (P5)

Check `capabilities.sibyl.harmony.version == 1`. A schema-3 `harmony` object
contains a keyed `progressions` map and optional `default` binding. Progressions
have positive `lengthBeats` and 1-128 ordered `chords`, each with a unique `id`,
`beat`, scientific-note `root`, and sorted `intervals`. First beat is zero; the
last marker precedes the endpoint. Intervals start at zero, contain 1-8 integers
from 0 to 36, and cannot repeat a pitch class.

```json
{"op":"upsert_progression","id":"changes","progression":{
  "lengthBeats":8,"chords":[
    {"id":"c","beat":0,"root":"C3","intervals":[0,4,7]},
    {"id":"a","beat":4,"root":"A2","intervals":[0,3,7]}]}}
```

`set_default_harmony` takes `binding:{"progression":"changes",
"clock":"arrangement","loop":true}`. A null binding removes the default.
`set_scene_harmony` takes `scene_id` and a binding using `sceneRepeat` or
`sceneVisit`; null explicitly disables harmony in that scene.
`inherit_scene_harmony` takes `scene_id` and removes the scene field to restore
inheritance. `delete_progression` requires references to have been detached by
prior operations in the same transaction. `loop` defaults true; false holds the
final chord. All operations use ordinary revision-guarded EDIT/VALIDATE.

Each event has exactly one of note, degree, pitchV or harmonic. Relative examples:

```json
{"step":0,"harmonic":{"kind":"tone","index":0,"octave":0}}
{"step":4,"harmonic":{"kind":"tone","role":"third","octave":1}}
{"step":8,"harmonic":{"kind":"nearest","reference":{"note":"D3"},
 "range":{"min":"C3","max":"C5"},"tieBreak":"lower"}}
```

Tone requires exactly one index (0-7, no wrapping) or role (root, third, fifth).
Third means a unique pitch class 3/4; fifth means a unique 6/7/8. Missing or
ambiguous roles fail. Octave defaults zero and accepts -10 to 10 subject to final
pitch bounds. Nearest considers chord pitch classes in the inclusive register,
uses a static note/degree/pitchV reference, and resolves equal distances with
lower (default) or higher. Selection does not depend on prior probability draws.
Event and scene transposition happen after selection.

Harmony is selected at the scheduled sounding onset, including microshift/swing,
and latched through sustains, glides and ratchets. A subsequent tied event can
select a new pitch. Assigned relative patterns require valid effective harmony;
unassigned bank patterns are retained with `unbound_harmony_pattern` warnings.
Static percussion retains its compiled pitch path and is excluded from purely
harmonic dependency changes.

GET `view:"progression", id:"changes"` returns the authored progression plus
chord intervals, pitch classes and resolvable roles. GET
`view:"effective_context", scene_id:"chorus", scene_repeat:0, beat:2` returns
current binding/chord and up to 256 authored note projections. Repeat is zero-based
and beat is in `[0, scene.lengthBeats)`. `selectedPitchV` precedes transposition;
`effectivePitchV` includes event and assignment transposition. These projections
are context values, not predictions of event eligibility or physical macros.
They include `totalNotes` and `truncated` when the bound is reached. Note
projections also include authored gate, velocity and probability after scene
overrides. `derived.automation` lists active curve values and scene offsets;
add-mode event bases and live macros are deliberately not inferred. Ordinary
note/static-scene projections mark relative pitches `requiresContext` rather than
inventing a universal voltage. The voicing popup labels its current scene/time
harmony projection.

Limits: 128 progressions, 128 chords per progression, 4096 total markers,
1048576 interned expression/chord pitch entries, and a combined 32 MiB compiled
storage budget with automation. Unsupported/unrepresentable timelines fail
explicitly. `voice_progression` provides fixed-note materialization as described below.


### P6: generate fixed chord voices

Use `voice_progression` in the ordinary `sibyl_edit` operations array, or preview
that array with `sibyl_validate` and `expected_revision`:

```json
{"op":"voice_progression","progression_id":"main_changes","scene_id":"chorus",
 "resolution":"1/16","gate_ratio":0.95,"write_policy":"createOnly",
 "voices":[
   {"track_id":"chord_low","pattern_id":"chorus_low","min":"C2","max":"C3"},
   {"track_id":"chord_mid","pattern_id":"chorus_mid","min":"C3","max":"C4"},
   {"track_id":"chord_high","pattern_id":"chorus_high","min":"C4","max":"C5"}]}
```

Supply 1-8 distinct existing tracks and distinct destination patterns in
low-to-high voice order. Registers are inclusive scientific note names. Overlap
is allowed; generated pitches never cross. Scene and progression lengths must
match, markers must lie exactly on the requested grid, and the resulting pattern
must contain 1-1024 steps. Grid mismatches return `off_grid_harmony`.

`gate_ratio` defaults to 0.95 and accepts 0-1. Events have one ratchet and gates
proportional to the chord interval; legacy minimum gate behavior still applies.
`write_policy` defaults to `createOnly`, which rejects an existing destination
pattern or any existing destination assignment. Explicit `replace` overwrites
those patterns and replaces target assignments with restart assignments, clearing
their previous overrides. Other scenes' assignment objects remain unchanged,
but references to a replaced bank pattern hear the new notes. Preview reports
these references (up to 128 plus total/truncation), destination counts, and search
cost. Replacement retains stable note IDs at matching steps.

The named `voice_progression_v1` algorithm prioritizes chord-tone coverage,
smallest maximum leap, absolute movement, squared movement, initial distance from
register midpoints, then lexicographic pitches. Root coverage is required; a
unique third is required with two or more voices. The last-to-first loop seam is
not optimized, and the report says so explicitly. Impossible registers/coverage
return `voicing_unsatisfiable`; exceeding 200,000 visited partial assignments,
2,048 candidates per chord, or 2,000,000 total DP transitions per request returns
`capacity_exceeded`. No partially optimized result is committed.

Generated events are ordinary **fixed pitches**. Later progression edits do not
rewrite them. Rerun with `replace` to revoice, or author `harmonic` events when
automatic harmonic following is desired. Preview consumes no IDs/revision; all
generated patterns and assignments participate in the same atomic edit and undo.


## P7A: native static pitch systems (schema 4)

Check `capabilities.sibyl.pitchSystems.staticTunedNotes` and its `stage` before
using this extension. P7A imports schema 2/3/4 and writes schema 4. Schema support
alone does not imply native harmony, native scene transforms, retuning, Scala
interchange, or preset queries; those are later P7 milestones.

Composition-owned `pitchSystems` contains `tunings`, `scales`, `contexts`, and an
optional `defaultContext`. Equal tunings accept divisions 1-1024 and a period
interval (ratio or cents). Table tunings accept 1-1024 increasing positions,
starting at unison and excluding the repeated endpoint. A scale selects native
steps; a context binds a tuning, optional scale, and one anchor: conventional
`note`, `pitchV`, or positive `frequencyHz`.

Use exactly one event pitch representation. Native examples are
`"tuned":{"step":31}`, `"tuned":{"degree":4}`, or
`"tuned":{"ratio":"3/2"}`. Optional `periods` repeats the tuning period, which
need not be an octave. Context resolution is explicit `tuned.context`, then
pattern `pitchContext`, then composition `defaultContext`. Conventional `note`,
legacy degree, and `pitchV` ignore that chain. A semitone remains 100 cents.

In 53-EDO, step 31 is approximately a pure fifth; an authored 3/2 ratio remains
exact and is not snapped. In 38-EDO, step 22 approximates that fifth. Full/pattern/
note reads preserve authored coordinates. Request `effectivePitchV` in projected
note fields for the compiled static value. Pattern-bank duplication pins an
inherited source native context when copying into a different pattern.

Until later milestones, create definitions through a full schema-4 composition
and author native notes through existing pattern/note operations. Do not invent
P7 edit operation names merely because they appear in the implementation spec.


## P7B: native pitch editing and scene transforms

Check `pitchSystems.stage == "P7B"` (or a later advertised feature set),
`nativeTransforms`, `retuneNotes`, and `definitionEditing`. Native harmony and
voicing remain separate capabilities. All operations use normal revision-guarded
EDIT/VALIDATE, one atomic transaction, and the existing adoption policies.

Definition operations: `upsert_tuning {id,tuning}`, `delete_tuning {id}`,
`upsert_pitch_scale {id,scale}`, `delete_pitch_scale {id}`,
`upsert_pitch_context {id,context}`, `delete_pitch_context {id}`,
`set_default_pitch_context {context_id}`, and
`set_pattern_pitch_context {pattern_id,context_id}`. Null context_id removes the
binding. Related definition/reference edits validate against the final graph;
read-dependent operations such as retuning require their definitions beforehand.

`transpose_notes` accepts exactly one of existing `semitones`, existing `degrees`,
or `interval` containing exactly one `steps`, `periods`, `cents`, or `ratio`.
Steps require native step/degree identity. Periods use the native context period,
or an octave for legacy pitches. Cents/semitones remain absolute intervals.
Ratio transposition intentionally becomes an accumulated cents offset and reports
the requested ratio. Degree transposition changes a degree coordinate only.

Authored note and scene-assignment fields `transposeSteps`, `transposePeriods`,
and `transposeCents` preserve presence and types through partial edits. Existing
`update_scene_assignment` can set/unset these leaves. Unsupported step shifts
reject even for notes with probability zero. Changing pitch representation retains
offsets unless explicitly unset, so incompatible retained steps reject.

`retune_notes` uses the normal pattern/selector/expect_count fields plus required
`target_context`, `mode` (nearest, preserve, reinterpret), and `target` (tuning,
scale). Optional `tie_break` is lower/higher, default lower; `max_error_cents`
limits source-to-grid error even in preserve mode. Nearest snaps; preserve writes
an explicit residual cents offset; reinterpret retains native coordinates while
changing context. Source pitches include event offsets but exclude scene offsets.
Dynamic harmonic events reject. Reports include bounded per-note before/after,
grid error, residual, total and truncation information.

Cross-pattern duplication defaults to pinning the source native context. Explicit
`pitch_context_policy:"destination"` removes copied context binding so destination
inheritance applies. Time rotation still affects only temporal steps.

Read `view:"pitch_systems"` with `id:"tunings"|"scales"|"contexts"` (default tunings)
and page_size 1-128 (default 16). Reuse the returned cursor with the same revision
and query. `view:"pitch_context",id:<context>` returns that context and its tuning/
scale definitions. Add `pitchDetails` to a notes field projection for context,
unit, authored coordinates, effective voltage, nominal frequency and conventional
note deviation. These labels never quantize playback. `effective_context` reports
scene-transformed voltage; raw note projections are before scene overrides.


### P7C/D: native harmony and interchange

Capabilities now advertise `pitchSystems.stage:"P7D"`, native harmony and Scala
interchange. Native progression example (requires an existing context `c`):

```json
{"pitchContext":"c","lengthBeats":4,"chords":[
  {"id":"I","beat":0,"rootPitch":{"tuned":{"step":0}},"tones":[
    {"id":"r","interval":{"steps":0},"roles":["root"]},
    {"id":"m","interval":{"steps":17},"roles":["third"]},
    {"id":"f","interval":{"steps":31},"roles":["fifth"]}
  ]}
]}
```

The 0/17/31 example is a 53-EDO major triad. Roles are explicit, unique and
case-sensitive. Native/legacy markers cannot mix within one progression.
Step intervals require a lattice root; ratio/cents tones remain off-grid.
Select a harmonic tone by exactly one `index`, `role` or `toneId`. Optional
`periods` repeats the context period; `octave` remains an absolute octave and
cannot coexist with `periods`. Nearest references and range endpoints accept
`{"tuned":{...}}`, `{"note":"C4"}` or `{"pitchV":0}`; conventional string register
endpoints still work. Event-pattern/default context inheritance applies to tuned
references, while the progression context determines chord tones.

`voice_progression` retains the existing operation shape. For native progressions,
voice `min`/`max` accept precise static endpoints as above. The report algorithm is
`voice_progression_micro_v1`, scoring uses 0.001 cent, and provenance names source
context/tuning, chord/tone IDs and period displacement. Output notes are fixed
absolute voltages: later tuning changes do not retune materialized notes. Root
coverage is required; explicit third coverage is required with at least two voices.
Capacity failures are errors, never a silently truncated voicing.

Use these bounded read-only views through `vcv_sibyl_get_composition`:

- `view:"tuning_catalog"`: all 15 factory definitions; optional `id` selects one,
  e.g. `53edo`, `38edo`, `13edt`, or `just7`.
- `view:"map_intervals",context_id:"c",intervals:[{"ratio":"5/4"},{"ratio":"3/2"}]`:
  steps, signed errors and collision diagnostics. Optional `tie_break` is
  `lower` (default) or `higher`. At most 1024 requested intervals. A collision
  returns `ok:false`, `scale_mapping_collision`, and diagnostics; no edits occur.
- `view:"export_tuning_scl",id:"t"`: normalized Scala text for an authored tuning.

`{"op":"import_tuning_scl","id":"t","text":"description\n2\n3/2\n2/1\n"}`
imports a table with positions 1/1 and 3/2 and period 2/1. Optional `name` supplies
metadata. The text limit is 256 KiB; descriptions must fit 2048 bytes and names
256 bytes. Comments, CRLF, empty descriptions, cents, ratios and bare integer
ratios are supported. The last entry is the period, not a duplicate table position.
Errors distinguish `invalid_scala` from `unsupported_tuning_shape`; nothing is
silently sorted or octave-normalized. Anchors remain separate context definitions.

Copying harmonic notes uses destination harmony. Inherited tuned nearest
references/range endpoints are pinned to the source context by default; explicit
`pitch_context_policy:"destination"` makes them inherit the destination context.
