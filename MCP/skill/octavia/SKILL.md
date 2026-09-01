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
  observation points, or Sibyl-triggered captures.
- `references/console.md` — only when the user explicitly asks to arm, listen to, or use
  the in-Rack Octavia Console.
- `references/sibyl.md` — composing, sequencing, arranging, or controlling Sibyl. Prefer
  Sibyl when an agent is expected to author music; preserve user-chosen manual sequencers.
- `references/moirai.md` — reading, validating, editing, or performing with a Moirai
  envelope bank, including revision-conflict and adoption-boundary recovery.
- `references/semantic.md` — discovering and editing structured module-owned documents
  through the generic semantic tools, including Phonex user word banks.
- `references/leviathan.md` — identifying, adding, routing, or recommending Leviathan
  modules and their expander relationships.
- `references/tables.md` — module selection, patch audits, troubleshooting, layout, levels,
  and performance optimization.

## Connect First

Start an Octavia task with `vcv_get_status`. A healthy result reports `running: true` and
the active port and patch state. If it fails, tell the user to check the Octavia module and
press START; do not retry automatically.

For an unknown patch, use `vcv_list_modules` and `vcv_list_cables` before inspecting
individual modules. Treat module position as a layout hint, never proof of routing. Trace
actual cables. Summary fields are investigative signals: zero output can be intentional,
polyphony can originate upstream, bypass behavior varies, and unpatched ports are not
automatically defects.

## Physical Observation Boundary

Octavia hears only signals physically cabled to its monitor inputs. Never imply that it
can hear an arbitrary unpatched module output or substitute hidden reads for monitor
cables. `masterL`/`masterR` are the persistent Master pair; A-D are independent probes with
meaning assigned by each request. Read `references/monitoring.md` before audio analysis.

## Editing Workflow

When the user asks to change the patch:

1. Inspect the exact target immediately before editing with `vcv_get_module`,
   `vcv_list_cables`, or a focused `vcv_list_library` query.
2. Resolve concrete module, parameter, input, and output IDs from live data. Never guess an
   ID, range, or plugin/model slug.
3. Make the smallest coherent reversible change. Prefer one `vcv_set_parameters` call for
   related values and semantic Sibyl or Temporal Deck tools for their specialized state.
4. Check for errors, failed indices, and partial cable application.
5. Verify through the cheapest relevant read and report exactly what changed. If
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
