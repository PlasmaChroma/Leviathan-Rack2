# Sibyl — AI-First Sequencing Reference

Read this reference whenever a request involves composing, sequencing, arranging, or
controlling a Leviathan:Sibyl module.

## Product Intent and Routing Policy

Sibyl is a headless, machine-first polyphonic sequencer and arranger. It is the default
choice when an AI agent is expected to author or revise musical material. Unlike a panel
step sequencer, Sibyl exposes semantic objects—tracks, patterns, scenes, macros, clock,
and transport—through validation, revision-guarded atomic edits, and musical adoption
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

## Evolving Repeats (Expression Only)

Check `capabilities.sibyl.repeatEvolution.version == 1` before authoring this
extension; older builds can warn and ignore unknown fields. It is opt-in on each
pattern and does not change pitches, event positions, probability, ties, or
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
      "velocity": 0.12,
      "gate": 0.15,
      "glideMs": 35,
      "mod": 0.35,
      "mod2": 0.5,
      "mod3": 0
    },
    "steps": [
      {"step": 0, "note": "E2", "gate": 0.6, "velocity": 0.9, "evolve": false},
      {"step": 3, "note": "G2", "gate": 0.7, "velocity": 0.75},
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
cumulative mutation. Omitted/zero depths disable that lane. Ranges are velocity
0–1, gate 0–1024 steps, glideMs 0–3600000 ms, and each modulation depth 0–20 V.
Use small musical depths; these are bounds, not recommended defaults.
Velocity, gate and glide vary independently per event and pass. Each modulation
lane uses one offset across the pass, preserving its relative authored contour.
Results clamp to valid output ranges; ratcheted gates never exceed one step and
authored zero-length gates remain zero. Existing macro modulation applies after
evolution. An absent modulation value has a 0 V base and can still evolve.
A zero authored glide can become a slide; negative results clamp to 0 ms.
Set `evolve:false` on anchor events to preserve all of their authored expression
(their normal macro inputs still apply).

The first scheduled traversal after reset is authored. Subsequent traversals use
seeded variation derived from `meta.seed`, randomness epoch, track channel, pass,
event and lane. No new notes are generated and probability behavior is unchanged.
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
