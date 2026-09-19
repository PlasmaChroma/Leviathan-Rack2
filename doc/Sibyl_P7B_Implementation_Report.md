# Sibyl P7B — Native composing operations

P7B extends the P7A core pitch model. Native harmony/voicing and Scala/preset interchange remain P7C/P7D work. No live Rack patch is modified by this implementation phase.

## Implemented

- Typed `transposeSteps`, `transposePeriods`, and `transposeCents` on events and scene assignments. Presence survives partial updates, unset, and serialization. Semitones retain 100-cent meaning; native periods follow the tuning's repeat period.
- `transpose_notes.interval` accepts steps, periods, cents, or ratio. Ratio transforms intentionally accumulate cents and retain the requested ratio in the report. Existing semitone/degree operations continue to work.
- Step shifts reject incompatible pitch representations, including silent/probability-zero events. Replacing a pitch representation retains offsets and rejects incompatible retained steps instead of dropping them.
- `retune_notes` supports nearest, preserve, and reinterpret modes, tuning/scale targets, lower/higher ties, maximum grid error, bounded before/after/error/residual reports, and existing selectors/count guards. Source pitches include event offsets and exclude scene offsets. Dynamic harmonic events reject.
- Definition upsert/delete operations for tunings, scales, and contexts; default/pattern context setters. Definition references validate against the completed transaction. Retuning reads its prerequisites at its operation position.
- Explicit destination-context duplication, with source-context preservation remaining the default and copy policy reported.
- Paged `pitch_systems` and targeted `pitch_context` queries, projected `pitchDetails`, and MCP capability/view support. Existing effective-context projections and the voicing display use the scene's compiled pitches.
- Metadata-only and unused scale changes leave unrelated channels alone. Consumed tuning/degree-scale changes participate in existing adoption handling.

## Runtime and storage

Unequal-table scene shifts are compiled from native coordinates on the control side; they are never approximated as `period / divisions` offsets. Playback uses a bounded indexed lookup through the existing cached track route.

Identical pattern/pitch-offset combinations share an immutable compiled table across scenes/tracks. Distinct tables are checked against the 1,048,576 combined pitch-entry limit before allocation. Retained capacities and native context storage contribute to the combined 32 MiB pitch/harmony/automation budget.

Legacy playback with no new fields retains its old arithmetic path. Existing legacy harmony gains universal period/cents offsets but does not gain native lattice identity; native harmonic step transforms belong to P7C.

## Validation

Focused native checks cover transposition units, unequal-table scene shifts, all retuning modes, exact ties, maximum errors, cross-context copying, representation replacement, partial override edits, final-graph definition validation, rollback, metadata/scale adoption masks, stale query cursors, shared tables, and capacity rejection.

Rack-linked module tests verify 38/53-EDO scene offsets at actual pitch output, quantized adoption, agreement with effective-context queries, and no audio-thread allocations/deallocations. Existing module and legacy golden tests remain required.

The first full-suite attempt found an incorrect new test assumption: an invalid `EditResult` can retain a rejected diagnostic candidate. The rollback test now verifies the public contract—failure and an unchanged accepted base—rather than requiring that private candidate pointer to be null. No publication or rollback defect was found.

The final Windows `plugin.dll` build and complete `test-fast` run passed (exit 0; suite summary 109,950 checks, zero failures). All six legacy golden hashes are unchanged. The MCP wrapper contract suite passed all 12 tests.

Final native build/suite log: `test-results/p7b-final-validation.log`. MCP wrapper contracts: `MCP/tests/test_server_contract.py`. The generated `plugin.dll` is not installed into the running Rack process by this phase; live P7 acceptance remains pending.

## Next

P7C adds explicit native chord roots/tones/roles, precise harmonic nearest selection, native-period harmony, context-aware compile keys, and microtonal voicing. P7D adds preset discovery, interval mapping, Scala interchange, further performance/capacity work, and live integration.
