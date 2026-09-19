# P7C/P7D — native harmony, voicing and interchange

Implementation date: 2026-09-18. Completes the two remaining implementation stages
of the P7 pitch-system specification. Live Rack acceptance remains distinct from
the automated Windows validation described below.

## Implemented behavior

- Native progressions bind a `pitchContext`; chord roots use `rootPitch`, and
  ordered tones carry stable IDs, explicit roles, and step/cents/ratio intervals.
  Native and legacy chord representations cannot be mixed within a progression.
- Harmonic events support index, role or `toneId`, context-period displacement,
  and precise nearest-tone references/registers. Unequal-grid step shifts retain
  lattice identity; off-grid tones reject step transforms even when the shift is
  zero. Octave displacement remains an absolute 2/1 interval.
- Native final voltages are compiled per reachable event/chord/assignment.
  Identical route programs share storage. Playback performs a bounded lookup;
  it does not parse ratios, allocate, or solve chords. Adoption compares audible
  results and harmonic dependencies. MOD automation remains independently routed.
- Cross-pattern copies pin inherited contexts in native nearest references and
  range endpoints, unless `pitch_context_policy:"destination"` is requested.
  Harmonic chord binding follows the destination scene and is reported explicitly.
- `voice_progression` accepts precise native register endpoints and emits fixed
  `pitchV` notes. Native candidate coverage uses tone identity, crossing uses
  precise voltage, and optimization uses milli-cent ticks (ties toward lower).
  Distinct precise candidates remain distinct even when scoring ticks coincide.
  Squared costs use 64-bit arithmetic, leap bounds come from candidate extrema,
  and all search passes share the existing transaction budgets. Candidate/DP
  scratch has a separate 64 MiB bound. Reports identify
  `voice_progression_micro_v1`, units, source context/tuning and per-note provenance.
  Existing legacy voicing objectives and report fields remain unchanged.
- `tuning_catalog` exposes the specified 13 octave divisions, 13 equal divisions
  of 3/1, and the seven-position just-ratio table. These are ordinary definitions;
  requesting the catalog does not populate a composition.
- `map_intervals` returns nearest steps, target/realized cents, signed errors,
  and collision diagnostics. Collisions return `scale_mapping_collision`.
- `import_tuning_scl` imports up to 256 KiB of text into an ordinary table tuning.
  `export_tuning_scl` returns normalized text through the existing query API.
  Ratios survive export; equal grids export precise decimal cents. The implicit
  unison and terminal period are represented correctly. Invalid syntax and valid
  but unsupported scale shapes have distinct errors. Descriptions over the
  existing 2048-byte metadata limit return a clear capacity error.
- Native schema fields require schema 4. The MCP adapter and both Octavia query
  routes forward the new views/arguments. Panel pitch summaries now show a
  conventional note plus deviation to 0.1 cent rather than whole-cent rounding.

The Scala adapter follows the [official Scala format](https://www.huygens-fokker.org/scala/scl_format.html).
It accepts semantic UTF-8 text; it does not open paths or implement `.kbm` mapping.

## Automated validation

The new native test cases exercise 38- and 53-EDO role selection, negative period
displacements, scene transforms, fixed voicing materialization, retained native
representation, schema gating, context-preserving copies and rollback. An unequal
tritave case checks lattice transposition across its period boundary and precise
nearest selection. Scala cases cover 1/38/53/1024-position exports, implicit unison,
comments, CRLF, blank descriptions, ratios, bare integers, non-octave periods,
invalid syntax and unsupported shapes. A separate exhaustive full-path oracle
checks 400 deterministic native voicing cases.

Rack-linked module cases check actual native pitch/gate output at 44.1, 48 and
96 kHz, nonmutating preview, quantized adoption, zero playback/adoption allocations
and frees, patch serialization, undo/redo-state restoration, and query dispatch.
These invoke the real module implementation linked to the installed Rack runtime.

Native Windows `plugin.dll` linking and `test-fast` passed. The suite reports
**109,950 checks and zero failures**; the new assertion-based codec/edit/oracle
cases also pass. The MCP suite passes **13 tests**, including execution of the
actual query-forwarding function with encoded interval objects. All six legacy
hashes remain unchanged:

```text
0 44100 2018724515009493274
0 48000 10000105836002149975
0 96000 10338453823389365619
1 44100 782236348153378706
1 48000 16270975918290080511
1 96000 8983016345456801156
```

Logs: `test-results/p7cd-complete-validation.log`,
`test-results/p7cd-final-edit.log`, and `test-results/p7cd-final-module.log`.
The complete run follows scratch-budget hardening. Final review then tightened
two-entry harmonic capacity accounting and malformed query/tie-break rejection;
codec, editing/oracle, Rack-linked module and all six golden cases passed again
in `test-results/p7cd-boundary-validation.log`, with another successful DLL link.
The plugin was built in the repository, not installed into the user's Rack
installation. `git diff --check` is clean.

Final `plugin.dll` SHA-256:
`0C3C5AEE6B7C7196ECCD3C02466BB2DBF5382EDD1213E2E9592A6C65EB6C13EA`.

## Matched process benchmarks

The same release-like harness was compiled against HEAD
`4115c0d396f47fc3ab4e37719d47d6f8535559ab` and the completed P7 sources, using
native MINGW64, `-O3 -funsafe-math-optimizations -march=nehalem`, the same Rack
runtime and 48 kHz. Three interleaved runs per workload measured 384,000 process
calls each after one simulated second of warm-up. Values below are medians
across the three runs, in nanoseconds; timer overhead is included.

| Workload | Baseline median / p95 / p99 | P7 median / p95 / p99 |
|---|---:|---:|
| Legacy, one channel | 200 / 200 / 200 | 200 / 200 / 200 |
| Legacy, 16 channels | 1200 / 1300 / 1300 | 1200 / 1300 / 1300 |
| Expressive legacy harmony, 16 channels | 1800 / 1900 / 2100 | 1800 / 1900 / 3100 |
| Native 38-EDO, 16 channels | n/a | 1800 / 1900 / 3000 |
| Native 53-EDO, 16 channels | n/a | 1800 / 1900 / 2900 |

All 24 runs reported **zero allocations and zero frees** during measurement.
Native fixtures retain the expressive workload's 1024-event pattern, 128 chord
markers, conditions/probability evolution, and 48 MOD curves. Only the tuning
in use appears in their pitch-system definitions.

Legacy median/p95 costs are unchanged at the measured timer resolution.
The expressive p99 is higher in these samples: baseline runs span 2000-3100 ns,
P7 2800-3200 ns. This is not evidence of zero overhead. Worst individual calls
across workloads ranged up to 275.1 microseconds and include desktop scheduling;
these measurements do not establish a hard real-time bound or replace live
performance checks under the user's audio settings.

Reproduce fixtures with `tools/sibyl_p7_benchmark_fixtures.py`. The current
harness target is `build/tests/sibyl_process_benchmark`. Retained baseline
sources, build command, interleaved runner and raw results are under
`test-results/p7-benchmark/` (`build-baseline.sh`, `run.py`, `results.json`).

## Live boundary

The Octavia connection check returned “Cannot reach the Octavia module.” No live
bridge, physical CV recording, listening, actual Rack UI undo/redo, or patch-file
save/reopen result is claimed. The current running Rack was not modified or
restarted. `dataToJson`/`dataFromJson` state restoration is covered offline and is
not a claim about interaction with the Rack history UI.

The implementation is ready for a refreshed Rack/MCP session to exercise
[the native fixture](./Sibyl_P7CD_Example_Composition.json). No files were staged
or committed.


## Live follow-up — 2026-09-19

The refreshed rack passed live semantic operations, bridge undo, preset-state
restoration, and six physically cabled CV/gate captures covering 38/53-EDO,
scene transforms, materialized voicing, and unequal tritave playback. See the
[live integration report](./Sibyl_P7_Live_Integration_Report.md) for measured
errors, retained evidence, final rack state and remaining manual boundaries.
The earlier unreachable-bridge statement records the implementation session.
