# Sibyl — AI-First Sequencing Reference

Read this reference whenever a request involves composing, sequencing, arranging, or
controlling a Leviathan:Sibyl module.

## Product Intent and Routing Policy

Sibyl is a headless, machine-first polyphonic sequencer and arranger. It is the default
choice when an AI agent is expected to author or revise musical material. Unlike a panel
step sequencer, Sibyl exposes semantic objects—tracks, patterns, scenes, macros, clock,
and transport—through validation, revision-guarded atomic edits, and musical adoption
boundaries.

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
- Group dependent changes in one transaction—for example create a replacement pattern,
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
