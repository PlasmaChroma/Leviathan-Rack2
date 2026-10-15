# Sibyl P4 implementation report

Date: 2026-09-18. Baseline HEAD: `6a7ce84916257f9c48434a87eacf6081e2950d04`
(`Sibyl P2 expressive`), branch `lumin-render`, with the completed P3 working-tree
changes already present. Those changes and unrelated untracked files were
preserved. Entry inventory: `test-results/p4-initial-working-tree.txt`.
No staging, commits, installation, or live Rack changes were performed.

## Implemented

Independent schema-3 automation for all 48 MOD lanes, including tracks without
pattern assignments. Scene-repeat, scene-visit and arrangement clocks use score
coordinates; scene owners take precedence over arrangement owners. Curves support
step, linear and smoothstep interpolation, endpoint/tail holding, enabled state,
replace/add mode and 0-1000 ms transitions.

Raw authored/evolved event MOD values are maintained separately from legacy final
output latches. Add mode applies the base, curve, scene offset and live macro once;
replace mode retains curve ownership through note events. Rejected notes leave
the base unchanged. Pattern restarts/new routes clear it; same-route continuation
preserves it. Unowned lanes retain legacy note-latched behavior.

Transition state retains indices and scalars, not old composition pointers.
Handover starts from actual emitted output, follows the moving incoming target,
and limits after blending. Removal uses the outgoing duration; repeated edits
restart from current output. Sample-rate changes preserve remaining duration.
Automation-only dependency masks preserve note gates and musical position.
Future-scene edits and pending replacement compare against the sounding revision.

Control-side compilation validates fields, references, ordered finite points,
scope bounds, owner conflicts and capacity before publication. Limits are 512
curves, 1024 points/curve, 16384 total points, 48 lanes, 32 MiB compiled storage
budget and 256 preview coordinates. Polynomial coefficients, reciprocals and
scene/lane routing are precomputed. Playback uses cached segment indices with
binary search on discontinuities, without scanning all definitions per sample.

New public operations: `upsert_automation` and `delete_automation`. GET
`view:"automation"` accepts `id` and optional `sample_beats`, returning authored
data separately from sampled curve values and segment/duration metadata. Summary
counts/targets and capabilities advertise implemented features and limits.
The existing MCP and Octavia routes forward the typed sample query.

Scene lengths retain their authored double value for automation validation,
serialization and score prefixes, while the legacy float scheduler is unchanged.
This prevents fractional endpoints such as 0.7 from failing a save/edit round trip.

## Files

- `src/SibylAutomation.hpp`, `src/SibylAutomationJSON.hpp`: curve model, compiler,
  sampler, handover state, codec and bounded projection.
- `src/SibylTypes.hpp`, `src/SibylJSON.cpp`: composition storage and round trip.
- `src/Sibyl.cpp`, `src/SibylAdoption.hpp`, `src/SibylAdoption.cpp`: playback,
  route/restart bases, automation masks, reads and capabilities. Routing is rebound
  to the adopted generation before applying simultaneous transport requests.
- `src/SibylEdit.cpp`: atomic curve upsert/delete through the common compiler.
- `src/Octavia.cpp`, `MCP/mcp_server/Octavia_MCP.py`: typed query forwarding.
- `Makefile`, `tests/sibyl_automation_cases.hpp`, `tests/sibyl_module_spec.cpp`,
  `tests/sibyl_codec_spec.cpp`, `tests/octavia_sibyl_contract_spec.py`: dependencies,
  feature tests and contracts. The unsupported-feature codec fixture now uses
  harmony, because automation is implemented.
- `MCP/skill/octavia/references/sibyl.md`: authoring/read documentation.

## Validation

Authoritative native MINGW64 commands, from the repository directory with
`MSYSTEM=MINGW64` and `/mingw64/bin:/usr/bin` on PATH:

```sh
make -j10 plugin.dll test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10 build/tests/sibyl_module_spec
export PATH="/c/Program Files/VCV/Rack2Pro:$PATH"
build/tests/sibyl_module_spec.exe
```

- Windows `plugin.dll` build/link: PASS.
- Native `test-fast`: PASS, 109950 checks, zero failures.
  Log: `test-results/p4-full-validation.log`.
- Final focused module suite after additional cross-rate/all-lane tests: PASS.
  Logs: `test-results/p4-final-module-build.log`, `test-results/p4-final-module.log`.
- Legacy golden traces remain identical at 44100, 48000 and 96000 Hz with
  evolution both off and on.
- P4 covers named sweep samples, rejected notes, patternless routing, all clocks,
  scene priority, add/replace arithmetic, live paused macros, terminal hold,
  external hold, jumps/reset/loop, gate-preserving edits, pending replacement,
  future-scene masks, removal, interrupted/moving-target blends, sample-rate
  transition changes, invalid input/capacity, fractional round trips and views.
- Final tests exercise all 48 lanes and a 1024-point seek. New ramp output agrees
  across 44.1/48/96 kHz within one sample plus float rounding. An initially strict
  cross-rate assertion was corrected to account for the beat-zero first sample
  and float output precision; DSP behavior was not altered to satisfy it.
- Instrumented processing, adoption, seeks and transitions: zero audio-thread
  allocations and deallocations in the tested paths.
- `MCP/.venv/Scripts/python.exe MCP/tests/test_server_contract.py`: PASS (11 tests).
- `MCP/.venv/Scripts/python.exe tests/octavia_sibyl_contract_spec.py`: PASS (6 tests).
- `git diff --check`: PASS.

## Remaining validation and scope

Live Rack installation, bridge round trip and listening were not run for P4.
Native sanitizer runtimes were unavailable in the earlier milestone probes;
no sanitizer success is claimed here. The final integrated musical fixture and
measured performance comparison remain P6 work. The implementation bounds work
and verifies allocation behavior, but this milestone does not claim a CPU benchmark.

P5 harmony and P6 voicing remain unsupported. The complete companion v3 example
still depends on those later features.
