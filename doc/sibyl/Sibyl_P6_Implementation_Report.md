# Sibyl P6 implementation and test handoff

Date: 2026-09-18. Branch `lumin-render`, HEAD
`d1a02bc2b821438a5b7e723fba54bcc48516b9b8`. P5 was already present as uncommitted
work and was preserved. No staging, commits, installation, or live Rack changes
were performed during P6.

## Implemented

`voice_progression` is available through the existing edit/validate routes and
advertised in capabilities as `voice_progression_v1`. It materializes fixed notes
on 1-8 existing tracks, with inclusive registers, noncrossing voices, root/third
coverage, exact grid validation, sustained gates, restart assignments and stable
note IDs. `createOnly` is the default; explicit `replace` reports affected bank
references while preserving other scenes' assignment objects. Reports describe
materialization provenance and the unoptimized last-to-first loop seam.

The solver first minimizes missing pitch classes. Because every complete
candidate can connect to every candidate at an unbounded leap, this first stage
is exactly the sum of independent per-chord minima. It then binary-searches the
minimum feasible leap ceiling and optimizes additive absolute movement, squared
movement, initial midpoint displacement and whole-path lexicographic pitch order
under that ceiling. It never discards a prefix on a running-maximum score.

Enumeration stops at 200,000 visited partial assignments per request or 2,048
valid candidates per chord. Every evaluated DP edge across all passes and all
operations counts toward the request's 2,000,000-transition budget. Exhaustion
returns `capacity_exceeded`, never an approximate result. Search and JSON work
remain entirely outside `process()`.

Main additions: `src/SibylVoicing.hpp`, `src/SibylVoicingEdit.hpp`, report support
in `SibylEdit.hpp`/`SibylNoteEdit.cpp`, edit dispatch/capabilities, voicing and
combined test headers, a process benchmark harness and fixture generator. The
Octavia Sibyl guide and implementation-status document describe the new contract.
No new MCP route, output port, parameter enum, or runtime voice allocator was added.

## Validation

Authoritative native Windows MINGW64 build and full `test-fast`: PASS. The suite
reports `checks=109950 failures=0`, and the Sibyl module suite passes its additional
individual assertions. Final focused plugin/module/codec/golden verification
also passes after the last structured-error and boundary-test refinements.

Logs:

- `test-results/p6-full-validation.log`
- `test-results/p6-final-build.log`
- `test-results/p6-final-module.log`
- `test-results/p6-final-codec.log`
- `test-results/p6-final-golden.log`

Native commands, in MINGW64 with the repository as working directory:

```sh
make -j10 plugin.dll test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 plugin.dll build/tests/sibyl_module_spec build/tests/sibyl_codec_spec build/tests/sibyl_legacy_golden_spec
export PATH="/c/Program Files/VCV/Rack2Pro:$PATH"
build/tests/sibyl_module_spec.exe
build/tests/sibyl_codec_spec.exe
build/tests/sibyl_legacy_golden_spec.exe
```

W01-W07 coverage includes deterministic registers/coverage, common-tone retention,
unsatisfiable and off-grid rejection, exact request-wide capacity limits,
materialization independence, and the future-large-leap regression. An independent
exhaustive complete-path oracle agrees with the staged solver in 400 seeded
cases. Further checks cover malformed inputs, replacement references/IDs,
preservation of other scene assignments and rollback after a later operation fails.

The companion expressive composition plus three generated chorus voices runs
through the full 48-beat arrangement at 44.1, 48 and 96 kHz. Assertions cover
side-effect-free preview, identical preview/commit reports, `nextBeat` adoption,
editing an accepted-but-pending revision, harmonic bass, chorus transposition,
every-fourth-pass accents, evolving hats, protected final-repeat fills, generated
gates, the 32-beat MOD rise and patternless smoothstep. Audio-thread `new/delete`
instrumentation reports zero allocations and deallocations, including adoption.
Rack state round trips and undo/redo **snapshot restoration** preserve IDs and
authored expression fields. Actual Rack UI undo/redo remains a live-test item.

All six legacy output hashes remain identical at 44100/48000/96000 Hz with
evolution off and on. Python server contracts pass 11/11; Sibyl bridge contracts
pass 7/7. Python byte compilation and `git diff --check` pass.

Native sanitizer probing still resolves neither `libasan.a` nor `libtsan.a` to
an installed library. No new ASan/TSan success is claimed; the older historical
schema-2 sanitizer results are not P5/P6 sanitizer acceptance.

Built `plugin.dll` SHA-256:
`A96D806C1D7D65245C26487C326FF456160F0F2A4CD9D046989B8B2A75A694B5`.

## Measured processing cost

Machine: Intel Core i9-9900K, Windows, native MSYS2 GCC 16.1.0. Identical benchmark
source compiled against retained baseline and final processing sources with
`-std=c++17 -O3 -funsafe-math-optimizations -march=nehalem`, matching the plugin's
optimization settings. Rack's installed runtime precedes compiler DLLs on PATH.
These are process-only measurements, not Rack UI/audio-device measurements.

The reconstructed baseline is commit
`d0c810ba9b0052deafa58836e94d123ef5ca0f44`, identified in the P0 report. Its exact
uncommitted probability-evolution additions were not retained separately; the
comparison fixtures disable evolution and all new features. This limitation
prevents claiming an exact reconstruction of that entire historical working tree.

Each run warms up one simulated second, then times eight simulated seconds one
process call at a time. Five alternating baseline/final repetitions cover each
legacy fixture at 48 kHz; three repetitions cover each expressive fixture at
each supported test rate. The table gives medians of run percentiles and the
largest observed individual call. Values include timer overhead and are in us.
The Windows clock quantizes these observations to approximately 0.1 us.

| Workload | Rate | Median | p95 | p99 | Worst observed |
|---|---:|---:|---:|---:|---:|
| Baseline, 1 legacy channel | 48000 | 0.2 | 0.2 | 0.2 | 68.0 |
| Final, 1 legacy channel | 48000 | 0.2 | 0.2 | 0.3 | 79.0 |
| Baseline, 16 legacy channels | 48000 | 1.2 | 1.5 | 1.8 | 76.7 |
| Final, 16 legacy channels | 48000 | 1.2 | 1.6 | 2.0 | 88.5 |
| Final, 16 expressive channels | 44100 | 1.8 | 2.6 | 3.2 | 230.1 |
| Final, 16 expressive channels | 48000 | 1.8 | 2.6 | 3.1 | 44.9 |
| Final, 16 expressive channels | 96000 | 1.8 | 2.6 | 3.1 | 117.0 |
| Final, maximum-capacity fixture | 44100 | 1.6 | 2.4 | 2.8 | 60.4 |
| Final, maximum-capacity fixture | 48000 | 1.6 | 2.3 | 2.8 | 78.9 |
| Final, maximum-capacity fixture | 96000 | 1.6 | 2.3 | 2.7 | 127.8 |

All 38 runs recorded zero tracked allocations and deallocations. Legacy median
cost is unchanged at the available timer resolution. Sixteen-channel p95/p99
increase by 0.1/0.2 us; baseline and final run ranges overlap. Those small tail
differences include added fixed condition/routing work plus measurement noise;
this does not establish zero overhead. The sporadic worst calls include normal
desktop scheduling/preemption and do not establish a hard real-time upper bound.

Legacy fixtures use a fully populated 1024-event pattern at `1/64`, assigned to
1 or 16 channels. The expressive fixture adds conditions/probability evolution,
128 harmonic markers, and 48 moving MOD curves with 12,288 total points. The
maximum-capacity fixture uses 16 channels, 1024 event slots, `1/64t`, 16 ratchets,
48 moving MOD curves, 1024 points in individual curves and exactly 16,384 total
points. Its point distribution differs from the uniform expressive fixture, so
the two rows are workload measurements rather than a monotonic cost comparison.

Reproduce fixtures with `python tools/sibyl_benchmark_fixtures.py`, and build the
current harness with `make build/tests/sibyl_process_benchmark`. Run the executable
with a fixture path and sample rate. For the baseline, archive the baseline
commit's `src/` into a separate directory and compile the **same** harness against
that directory and its Sibyl translation units, excluding `SibylNoteEdit.cpp`
(which did not yet exist). Retained local commands/sources and raw measurements
are under `test-results/p6-benchmark/`, including `build.sh` and `results.json`.

## Ready for live integration

Implementation and offline validation are complete through P6. The running Rack
and installed plugin have not been updated by this work. Install the new build
and restart Rack before testing; refresh the MCP server too if its P5 forwarding
changes have not yet been loaded. P6 itself adds no Python adapter change.

Use `doc/Sibyl_v3_Example_Composition.json`, then preview and commit the operation
array in `doc/Sibyl_P6_Voicing_Operations.json` with the current accepted revision.
That array defines three existing tracks before invoking the helper in the same
transaction. Live acceptance should verify:

1. Capabilities advertise harmony and `voice_progression_v1` with the documented
   budgets; preview produces fixed-note provenance without advancing revision.
2. Commit at `nextBeat`, edit again while pending, then read the active/accepted
   revision and generated notes after adoption.
3. Listen through the build and chorus: changing bass harmony, repeated-section
   accents, evolving hats, protected finale, long MOD movement and generated
   sustained chorus voices.
4. Save/reload the Rack patch and portable composition, then exercise actual Rack
   UI undo/redo for the whole voicing edit. Check IDs and all three assignments.
5. Observe the live Rack process/step/draw telemetry and physical signals under
   the user's normal audio settings. Offline microbenchmarks do not replace this.


## Live follow-up

The subsequent live test is recorded in [Sibyl_P5_P6_Live_Integration_Report.md](./Sibyl_P5_P6_Live_Integration_Report.md). It verified expressive playback and found an output reconnection defect. The fix is built and regression-tested; the updated DLL hash and remaining Rack refresh check are in that report. The earlier hash and no-install/no-live statements above describe the original implementation milestone.
