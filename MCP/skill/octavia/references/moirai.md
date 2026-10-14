# Moirai Semantic Envelope-Bank Workflow

Read this reference when configuring or controlling a live `Moirai` module. Moirai is a
dual-lane, 16-channel polyphonic envelope bank. It is connected to Sibyl or other modules
with ordinary Rack cables; it has no hidden expander bus and no private observation path.

## Discover and read before editing

1. Call `vcv_get_status`, then locate the exact Moirai instance with `vcv_list_modules`.
2. Read `vcv_moirai_get_capabilities` and `vcv_moirai_get_status`.
3. Read the smallest useful bank view. Use summary for orientation, program/lane/channel
   for focused work, and full before a broad replacement or restoration.
4. Use the returned accepted revision as `expected_revision`. Do not infer it from the
   active revision: a valid edit may still be waiting at its musical adoption boundary.

## Validate and edit

Validate a candidate bank with `vcv_moirai_validate` when authoring a full document. Apply
incremental work with `vcv_moirai_edit`; its ordered operations are one atomic transaction
and one Rack undo entry. A rejected transaction changes nothing.

Choose adoption deliberately:

- `immediate` changes the generation on the next audio sample.
- `nextTrigger` adopts before all gate rises on the boundary sample.
- `allIdle` waits until no voices remain active.
- `nextClock` waits for Moirai's next clock edge.

`finishCurrent` lets sounding voices retain their old generation while new triggers use
the adopted bank. `restartActive` is valid only with `immediate` and restarts sounding
voices from their current values using corresponding programs in the new bank.

On `revision_conflict`, read status/bank again, rebase the intended operations onto the
new accepted document, and retry with its revision. Never blindly increment a stale
revision. Poll status when the edit succeeds but `pendingRevision` remains non-null.

## Performance commands and patching

Use `vcv_moirai_command` for trigger, reset, and inspected-voice selection. Commands do not
change the bank revision and do not create Rack undo entries. Resolve live port IDs before
patching. Sibyl Gate, Velocity, and modulation outputs commonly feed Moirai Gate, Velocity,
and M1-M3; Moirai A/B feed downstream CV destinations.

To hear or measure Moirai, physically cable the relevant output to Octavia Master L/R or
Monitor A-D. For polyphonic cables the observation boundary records channel 0. Use frozen
snapshot tools for repeatable analysis and comparisons; semantic access never substitutes
for a monitor cable.
