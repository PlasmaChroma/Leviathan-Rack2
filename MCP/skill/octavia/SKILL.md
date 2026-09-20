---
name: octavia
description: >
  Inspect, analyze, and edit VCV Rack patches live through the Leviathan Octavia
  bridge and vcv-rack MCP tools. Use for VCV Rack, Octavia, modular-synth patch
  inspection or editing, module and cable operations, signal diagnosis, monitoring,
  Sibyl sequencing, Temporal Deck control, and the in-Rack Octavia Console.
---

# VCV Rack — Leviathan Octavia

Octavia is the live control and observation bridge. Its default HTTP endpoint is
`localhost:34570`; `vcv_get_status` is authoritative for the active server and patch.

## Route to the Relevant Reference

Read only the references required for the current task:

- `references/monitoring.md` — snapshots, comparison, spectrum, loudness, physical
  observation points, frame-synchronized diagnostic control, or Sibyl-triggered captures.
- `references/console.md` — only when the user explicitly asks to arm, listen to, or use
  the in-Rack Octavia Console.
- `references/sibyl_cheatsheet.md` — rapid Sibyl session bootstrap, P8 columnar note batches,
  structural scene reuse, two-phase commit, receipts, and scratchpad scripting. Start here for Sibyl tasks.
- `references/sibyl.md` — detailed musical reference: repeat conditions, probability evolution,
  microtonal N-EDO/Scala systems, relative harmony, and chord voicing.
- `references/moirai.md` — reading, validating, editing, or performing with a Moirai
  envelope bank, including revision-conflict and adoption-boundary recovery.
- `references/semantic.md` — discovering and editing structured module-owned documents
  through the generic semantic tools, including Phonex user word banks.
- `references/leviathan.md` — identifying, adding, routing, or recommending Leviathan
  modules and their expander relationships.
- `references/tables.md` — module selection, patch audits, troubleshooting, layout, levels,
  and performance optimization.

## Connect First

Start an Octavia task with `vcv_get_status` (or `vcv_sibyl_bootstrap` for Sibyl
tasks when advertised, which checks bridge availability, resolves module ID, and gathers
session orientation in one step). A healthy result reports `running: true` and the active
port and patch state. If it fails, tell the user to check the Octavia module and press
START; do not retry automatically.

For an unknown patch audit, use `vcv_list_modules` and `vcv_list_cables` before inspecting
individual modules. For targeted work on known modules, query scoped cables with
`vcv_list_cables(params={"module_id": ...})` to avoid dumping whole-patch graphs. Treat
module position as a layout hint, never proof of routing. Trace actual cables. Summary
fields are investigative signals: zero output can be intentional, polyphony can originate
upstream, bypass behavior varies, and unpatched ports are not automatically defects.

## Task Presence

When the connected build exposes `vcv_octavia_set_presence`, use it to hold the
panel icon for a meaningful phase of work rather than following every HTTP call:

- `inspecting`: examining the patch, routing, or physically monitored signals.
- `thinking`: planning or preparing actions to execute later.
- `working`: actively editing Rack.

Call with `state` and optional `lease_ms` (default 30000; range 1000–300000).
Choose enough time for asynchronous work; repeat to renew or change state. Use
`state="auto"` when finished to release early. The lease otherwise expires back
to automatic, and server stop clears it. `vcv_octavia_get_presence` reports the
target and remaining lease. These are agent-authored task labels, not automatic
observation of model reasoning. Presence changes do not save or alter musical
patch state. The latest setter wins; coordinate agents rather than competing for
this single display. Older bridges may lack the tools/routes; continue the task
without them rather than treating presence as required infrastructure.

Example for a user-requested patch enhancement (tool-call pseudocode):

```text
vcv_get_status()
vcv_octavia_set_presence(params={"state":"working","lease_ms":60000})
  Apply authorized edits; renew before expiry if work is ongoing.
vcv_octavia_set_presence(params={"state":"auto"})
  Return to automatic presence before reporting completion.
```

Presence states are coarse-grained task labels (`inspecting`, `thinking`, `working`),
not mandatory micro-steps. Skip phases the task does not need, and avoid rapid presence
cycling for quick or single-action tasks. If work outlasts its lease, renew the same
state before expiry. Release with `state="auto"` on early exit when bridge is reachable.

## Physical Observation Boundary

Octavia hears only signals physically cabled to its monitor inputs. Never imply that it
can hear an arbitrary unpatched module output or substitute hidden reads for monitor
cables. `masterL`/`masterR` are the persistent Master pair; A-D are independent probes with
meaning assigned by each request. Read `references/monitoring.md` before audio analysis.

## Diagnostic Control Boundary

Octavia's `Control A` and `Control B` outputs are temporary, frame-synchronized test
stimuli for bounded sanity checks and measurements where exact alignment with an Octavia
capture matters. Do not use them to author musical sequences, clocks, arrangements,
generative patterns, or persistent patch behavior. Use Sibyl when the agent is expected to
create or revise a sequence, while preserving an explicit user-chosen sequencer. Read
`references/monitoring.md` before using the Control outputs.

## Editing Workflow

When the user asks to change the patch:

1. Inspect the exact target immediately before editing with `vcv_get_module`,
   scoped `vcv_list_cables(params={"module_id": ...})`, or a focused `vcv_list_library` query.
2. Resolve concrete module, parameter, input, and output IDs from live data. Never guess an
   ID, range, or plugin/model slug.
3. Make the smallest coherent reversible change. Prefer one `vcv_set_parameters` call for
   related values, two-phase commit with `response_profile="receipt"` for Sibyl edits,
   and Temporal Deck tools for sample playback.
4. For algorithmic, Euclidean, microtonal, or multi-track sequencing, write and execute a
   local scratch script using `tools/sibyl_composer.py` (`SibylScore`, `PatternBuilder`,
   `two_phase_commit`) via terminal rather than emitting massive multi-turn JSON payloads.
5. Check for errors, failed indices, and partial cable application.
6. Verify through the cheapest relevant read and report exactly what changed. If
   verification fails, stop further writes and offer or use `vcv_undo` as appropriate.

Common writes include module addition, parameter changes, cable connection/disconnection,
bypass or movement, layout, state restoration, and undo. Cable operations may partially
succeed; their response reports the applied count. `vcv_connect_cables` accepts a color
name or hex value and otherwise defaults to white.

## Authorization and Recovery

- Reversible edits are allowed when the user clearly asks to edit or improve the patch.
  Exploratory requests authorize inspection and recommendations, not mutation.
- Before exact restoration or a broad uncertain edit, retain the current module state.
  `vcv_get_module_state` returns full preset JSON suitable for a user-held backup.
- `vcv_delete_module` is not undoable through Octavia. Confirm the exact module and obtain
  explicit approval naming it before deletion.
- `vcv_save_patch` overwrites the existing patch file and is not undoable through Octavia.
  Check `hasSavePath` and obtain explicit approval before saving.
- Do not silently broaden an edit. If a reversible operation partially succeeds, explain
  the state and use the reported applied count when undoing.

## Tool Invariants

- Filter `vcv_list_library` by `plugin` or `q` to avoid an oversized response.
- For multiple moves, use `vcv_layout_modules`; coordinates are absolute and must remain
  anchored to the existing patch cluster rather than an assumed origin.
- Ordinary patch caches can lag by about one second.
- `vcv_get_perf` is process-wide; do not claim per-module CPU attribution.
- Console waiting is bounded long-polling and follows `references/console.md`.
