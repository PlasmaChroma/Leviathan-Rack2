# Moirai Gemini Qualification Plan

Date: 2026-08-26

This is the execution plan for an independent Gemini qualification pass over
Moirai v1. The implementation contract remains `doc/moirai_impl.md`; this file
defines how to test that contract and what evidence must be returned.

## 1. Execution directive

Gemini must act as a tester, not as an implementer.

- Read and obey the repository `AGENTS.md` before executing any command.
- Execute the gates in order.
- Do not stage or commit files.
- Do not run `make clean`, `make install`, or overwrite a user patch.
- Do not delete modules or cables.
- Do not change source code to make a test pass. Record a minimal reproduction
  and stop the affected gate.
- Use only a disposable Rack test patch for reversible module, cable, parameter,
  semantic-edit, undo, and reload tests.
- Before the first live edit, capture the Moirai module state with
  `vcv_get_module_state` and retain it in the report. Restore it at the end if
  the test patch is not disposable.
- `vcv_save_patch` requires explicit user approval even for a test. If approval
  is absent, mark only the save/reload case `BLOCKED-MANUAL`; do not fail Moirai.
- Never infer a module, parameter, input, or output ID. Resolve every ID from the
  live Rack process.
- Semantic state is not signal evidence. Any voltage, envelope-shape, or
  downstream-consequence claim requires a physical cable to an Octavia monitor.

Use these result labels:

- `PASS`: requirement directly observed with retained evidence.
- `FAIL-MOIRAI`: reproducible failure in Moirai behavior or its documented API.
- `FAIL-GATE`: a required repository gate failed outside Moirai.
- `BLOCKED-INFRA`: toolchain, Rack, Octavia, or test-fixture prerequisite absent.
- `BLOCKED-MANUAL`: a human action or explicit destructive/save approval is
  required.
- `NOT-RUN`: no attempt was made; include the reason.

Do not call the overall plan complete while any required item is `FAIL-*`,
`BLOCKED-*`, or `NOT-RUN`.

## 2. Required report

Write the final evidence to `doc/moirai_gemini_test_report.md`. Include:

1. date, repository path, commit/worktree identity, host shell, compiler target,
   Rack version, sample rate, and patch path;
2. one result row for every test ID in this plan;
3. exact commands or Octavia tool calls used;
4. the relevant response fields, revisions, module IDs, cable endpoints,
   snapshot IDs, and frame ranges;
5. failure reproduction steps and whether the failure repeated;
6. restoration actions and the final Moirai revision/state;
7. an overall verdict of `QUALIFIED`, `QUALIFIED WITH EXPLICIT EXCEPTIONS`, or
   `NOT QUALIFIED`.

Keep raw command output in an appendix or linked artifact. Do not replace
evidence with statements such as “looks good” or “all tests passed.”

## 3. Known starting point

The prior pass established the following. Gemini must verify rather than merely
copy these claims:

- Phases 0–5 are functionally complete.
- Direct WSL `make test-fast` passed on 2026-08-26.
- Every native MINGW64 Moirai test passed on 2026-08-26.
- The current `plugin.dll` is an x86-64 Windows PE DLL containing Moirai.
- Plugin tag validation passed.
- The aggregate native `test-fast` gate passes. An earlier
  `deepcache_archive_spec` viewport-hydration failure was corrected by accounting
  for the 16 queued previews plus one already-decoded in-flight preview.
- Live Phase 6 smoke was blocked because no Octavia server was listening at
  `localhost:34570`.

## 4. Gate A — repository and toolchain preflight

### A01 — preserve the worktree

Record `git status --short` and `git diff --stat`. Existing changes belong to
the user. Do not modify or remove unrelated files.

Pass evidence: the initial worktree state is present in the report.

### A02 — identify the authoritative compiler

From WSL, enter the documented native bridge and record:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   printf "%s\n" "$MSYSTEM" "$(command -v g++)" "$(g++ -dumpmachine)"'
```

Pass criteria:

- `MSYSTEM` is `MINGW64`;
- compiler is `/mingw64/bin/g++`;
- target is `x86_64-w64-mingw32`.

### A03 — verify required sources and assets

Confirm the Moirai source family, `res/Moirai.svg`, generated panel/label SVGs,
all `tests/moirai_*` tests, the MCP wrappers, and the Octavia Moirai reference
exist. Confirm `plugin.json` contains the `Moirai` model.

Pass evidence: a concise file inventory and the `plugin.json` model entry.

## 5. Gate B — automated validation

### B01 — native MINGW64 focused suite

Run:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 test-fast \
     RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"'
```

The Rack application directory must remain ahead of compiler runtime DLLs when
Rack-linked tests execute.

The Linux and MINGW64 suites share `build/tests`. If the native run reports a
missing Windows test binary after a direct WSL build, do not clean the tree.
Repeat once with `make -B -j10 test-fast` in the same MINGW64 environment to
force regeneration of native `.exe` harnesses.

Pass criteria:

- every `moirai_curves`, `moirai_adoption`, `moirai_compiler`, `moirai_edit`,
  `moirai_json`, `moirai_engine`, `moirai_module`, and Moirai panel-contract
  test passes;
- the complete aggregate command exits zero.

If the corrected Deepcache viewport-hydration assertion regresses, run
`build/tests/deepcache_archive_spec.exe` three times in the same native runtime
environment. Report the aggregate failure as `FAIL-GATE`, the isolated results,
and all Moirai cases as passed. Do not relabel it as a Moirai failure.

### B02 — authoritative Windows plugin

Run the incremental authoritative build:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 plugin.dll'
```

Pass criteria:

- command exits zero;
- `file plugin.dll` reports PE32+ x86-64 Windows DLL;
- the DLL timestamp is not older than changed Moirai sources;
- `strings plugin.dll` contains `leviathan.moirai.envelope-bank` and
  `res/Moirai.panel.svg`.

### B03 — metadata validation

Run:

```sh
python3 tools/validate_plugin_json_tags.py
```

Pass criterion: exit zero with all Rack v2 tags valid.

## 6. Gate C — live Rack discovery

### C01 — connect exactly once

Call `vcv_get_status` first. A healthy response must report `running: true`, the
configured port, API version, and patch information.

If it fails, stop all live work immediately. Instruct the user to start Rack,
place/enable Octavia, and press START. Do not retry automatically. Mark Gate C
and all later live gates `BLOCKED-INFRA`.

### C02 — discover the actual patch

Call `vcv_list_modules` and `vcv_list_cables`. Locate exact instances of:

- `Leviathan:Moirai`;
- `Leviathan:Octavia`;
- a suitable polyphonic gate/modulation source, preferably Sibyl;
- any downstream module used for consequence measurement.

Call `vcv_get_module` for each relevant instance and record all resolved port
IDs/names. If Moirai is absent, do not add it to a non-disposable patch. Mark
the live gates blocked and state the required fixture.

### C03 — preserve state

Call `vcv_get_module_state` for Moirai. Read the undo stack with
`vcv_undo(status_only=true)`. Record the accepted baseline state and undo depth.

## 7. Gate D — semantic API, validation, and revision safety

### D01 — discovery and read views

Call, in order:

1. `vcv_moirai_get_capabilities`;
2. `vcv_moirai_get_status`;
3. `vcv_moirai_get_bank(view="summary")`;
4. `vcv_moirai_get_bank(view="full")`;
5. `vcv_moirai_get_program` for one referenced program;
6. lane views for A and B;
7. channel views for channels 0 and 15.

Pass criteria:

- capability identity is `leviathan.moirai.envelope-bank`;
- API/schema versions and limits are coherent;
- accepted, active, and pending revisions are internally consistent;
- every lane/channel reference resolves to an existing program;
- the full document can serve as a restoration candidate.

### D02 — non-mutating validation

Validate the complete accepted bank unchanged with `vcv_moirai_validate`.
Then validate a private candidate with one deliberately invalid bounded field,
chosen from the returned capabilities.

Pass criteria:

- accepted candidate validates;
- invalid candidate returns structured code/path details;
- accepted revision, active revision, playback, and undo depth do not change.

### D03 — stale revision rejection

Read the current accepted revision. Submit one harmless supported edit using a
stale `expected_revision`. Derive the operation shape from live capabilities;
do not invent it.

Pass criteria:

- response is `revision_conflict` and reports the accepted revision;
- bank, accepted revision, active revision, and undo depth do not change.

### D04 — atomic rollback

Using the current revision, submit two ordered operations in one transaction:
one valid harmless operation followed by one deliberately invalid operation.

Pass criteria:

- the transaction is rejected with an indexed structured issue;
- neither operation is committed;
- revision and undo depth do not change.

### D05 — successful transaction and undo

Submit one harmless, visible semantic edit with `apply_at="immediate"` and
`active_voice_policy="finishCurrent"`.

Pass criteria:

- accepted revision increments exactly once;
- active revision reaches the accepted revision;
- precisely one undo entry is created;
- `vcv_undo` restores the exact prior bank;
- a fresh status/read confirms restoration.

Do not use raw module-state replacement for this test.

## 8. Gate E — adoption boundaries and commands

Perform each case from a freshly read accepted revision. Restore after each
case. Poll status only while a documented pending revision exists.

### E01 — `nextTrigger`

Submit a harmless edit with `nextTrigger`. Confirm accepted revision advances
while active revision remains old. Use `vcv_moirai_command(action="trigger")`
on a resolved lane/channel. Confirm adoption occurs before the triggered voice
starts and pending revision clears.

### E02 — `allIdle`

Start a long voice, submit an `allIdle` edit, and confirm it remains pending
while any voice is active. Use the normal gate/release path or the Moirai reset
command to reach idle, then confirm adoption.

### E03 — `nextClock`

With a physically resolved clock cable, submit a `nextClock` edit. Confirm an
ordinary trigger does not adopt it and the next clock edge does. If no safe
clock source exists, mark only this case `BLOCKED-INFRA`.

### E04 — active-voice policy

During a long release, verify `finishCurrent` preserves the old generation for
the sounding voice while new triggers use the adopted generation. Separately,
verify `restartActive` is rejected outside `immediate` and is accepted with
`immediate`.

### E05 — command isolation

Exercise `select`, `trigger`, and `reset` commands.

Pass criteria:

- selected lane/channel telemetry changes coherently;
- trigger and reset cross to the audio engine;
- commands do not alter bank revision or create undo entries.

## 9. Gate F — polyphony, modulation, clock, and physical observation

### F01 — patch topology

Using resolved live port IDs, verify or create only in the disposable fixture:

```text
Sibyl GATE/VELOCITY/M1/M2/M3 -> Moirai GATE/VEL/M1/M2/M3
Sibyl CLOCK                  -> Moirai CLOCK
Moirai A                     -> Octavia monitor A
Moirai B                     -> Octavia monitor B
downstream consequence       -> Octavia monitor C or D
```

Call `vcv_list_cables` again and retain the exact endpoints as evidence. Cable
operations can partially apply; stop on the first failure and report the
applied count.

### F02 — monitor boundary

Call `vcv_octavia_get_monitors`.

Pass criteria:

- every monitor used below is physically connected;
- reported physical channel counts and sample rate match the fixture;
- no claim is made about an unpatched output.

### F03 — 16-channel behavior

Drive 16 gate channels with distinguishable activity. Verify both Moirai A and
B report 16 channels and that selected channel telemetry changes independently
for channels 0 and 15. Check mono Velocity/M1/M2/M3 broadcast, matching-poly
behavior, and missing-channel neutral behavior in separate observations.

### F04 — reset, EOC, and output modes

Verify reset produces idle zero output and no spurious EOC. Exercise `0_10`,
`0_5`, and `bipolar_5` on a controlled program, checking expected electrical
ranges at a physical monitor. Restore the original lane modes afterward.

### F05 — immutable snapshot

Create one snapshot containing A, B, and the chosen downstream consequence with
enough post-roll to include the full envelope response. Poll only if pending.
Analyze the same snapshot at least twice.

Pass criteria:

- snapshot has one stable ID and one frame range;
- repeated analysis does not recapture or change those fields;
- envelope peaks, DC, RMS, and clipping are plausible for configured modes;
- A/B/downstream comparisons use groups from the same frozen snapshot.

### F06 — exact-frame Sibyl observation

If the disposable Sibyl fixture already supports an authored observation
marker, trigger it and read `vcv_octavia_get_triggered_snapshots`. Resolve the
request to a complete snapshot and analyze it. Confirm the trigger frame is the
sounding event frame and that loss/expiry of observation data never changes
Moirai playback.

Do not broadly rewrite a user's Sibyl composition merely to create this case.
Mark it blocked if the safe fixture is absent.

## 10. Gate G — panel, persistence, and lifecycle smoke

These are live visual/manual cases. Gemini must retain screenshots or explicit
human confirmations; source inspection alone is insufficient.

### G01 — model and panel

Confirm Moirai appears in the browser, occupies 12 HP, uses the generated panel
without missing artwork, and places every control/port on its SVG anchor. Check
both themes if available.

### G02 — display and manual controls

Confirm the display follows selected lane/channel and shows accepted, active,
pending, voice, and phase information coherently. Verify the manual trigger
with GATE unpatched in the disposable fixture.

### G03 — factory preset menu and Rack history

Apply one context-menu factory preset. Confirm it creates exactly one revision
and one Rack history entry. Manually Undo and Redo and confirm exact bank
restoration in both directions through semantic reads.

### G04 — patch persistence

Only with explicit approval and a disposable patch having a save path: save,
reload, and verify the complete authored bank, revision, assignments, output
modes, clock policy, and safe derived runtime state. Otherwise mark
`BLOCKED-MANUAL`.

### G05 — graphics lifecycle

Ask the user to close and reopen the Rack/DAW window. After reopening, verify
the panel, labels, display, and controls render and update without stale NanoVG
handles, blank regions, crashes, or repeated errors. Reconnect with
`vcv_get_status` only after the user confirms the window is open; this is a new
explicit connection attempt, not automatic retry of a failed preflight.

## 11. Restoration and verdict

### R01 — restore the fixture

Undo every test edit/cable operation in reverse order or restore the retained
baseline Moirai state if explicitly authorized. Do not save restoration into a
user patch without approval.

Confirm:

- final accepted bank matches baseline;
- no pending revision remains;
- final undo state is explained;
- no test-only cables or modules remain in a non-disposable patch.

### R02 — verdict rules

Use `QUALIFIED` only when B01–B03, C01–C03, D01–D05, E01–E05, F01–F05,
G01–G03, and restoration pass. F06, G04, and G05 may yield `QUALIFIED WITH
EXPLICIT EXCEPTIONS` only when their block is external/manual and all automated
contracts covering the same state remain green.

Any reproducible Moirai defect, failed authoritative plugin build, failed tag
validation, incomplete restoration, or unexplained state mutation yields
`NOT QUALIFIED`.

Any future aggregate failure prevents an unqualified Definition of Done until
the full native `test-fast` command exits zero, even when all Moirai tests pass.
Report that distinction prominently.
