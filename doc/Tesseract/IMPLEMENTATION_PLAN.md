# TEMPI implementation work order (specification 1.1.0)

This is an execution index for `TEMPI_VCV_RACK_CODEX_SPEC.md`, not a second
behavior specification. Read the cited sections before each step. Do not
implement the whole module in one translation unit or one unverified pass.
No Tempi production source or build targets exist merely because they are
named here: create them at the indicated step.

## Before coding

1. Read root `AGENTS.md`, main spec §§1–5 and §25.2. Inspect the actual branch.
2. Run `python3 doc/tempi_codex_handoff/tools/check_handoff.py` and
   `python3 doc/tempi_codex_handoff/tools/check_review_contract.py`.
3. Record existing `make -j10 plugin.dll` and `test-fast` results. In this
   Windows checkout use MINGW64 and the installed Rack runtime from §25.2.
   Record baseline failures separately; do not fix unrelated modules.
4. Keep recovered reference/fixture files unchanged. Additional expected
   outputs belong in a clearly P-labeled new file. Never regenerate expected
   outputs during a test merely to make it pass.

## Work packets

Every packet must build with the production C++ standard (currently C++11).
Tests call production functions, not copied snippets. Add the listed targets
to the existing Makefile and use its Rack-linked test runner where needed.
Keep a single `TEMPI_CORE_SOURCES` list for production core test linkage.

| Packet / original phase | Read | Deliverable and gate |
|---|---|---|
| T00 / Phase0 | §§3–5,19.1,25.2 | Add `src/tempi/TempiTypes.hpp`, `tests/tempi/`, factory loading/generation and `test-tempi-fixtures`. Check all64 factory programs and both default memory layers. Register a minimal Tempi adapter/model only once; no UI-completeness claim. |
| T01 / Phase1 | §6 | `TempiMath`: defined wrap/truncation, ratio, phase, GCD/LCM, PRNG. `test-tempi-math` calls production math for all original CSV rows and arithmetic boundaries. Port exact ISR/guard helpers as reference functions. |
| T02 / Phase1 | §§4.2–4.5,7,11.1 | `TempiLeading`: Schmitt priming, tick conversion, interval producer, loss, capture helper, tempo table, single-winner arbitration. `test-tempi-leading`: LEAD/ADC/ISR, same-H source priority, invalid interval then reacquisition. |
| T03 / Phase2 | §§8.1–8.5,18.1 | `TempiScheduler`: common master and canonical lanes, rational deadlines, saturation and bounded overflow. `test-tempi-scheduler`: constant-H edges, phase parity, sample-boundary timestamps and long-run oracle. No UI dependency. |
| T04 / Phase2 | §§8.6–9,15.5 | Add pending commits, old-HIGH completion and output shaping. Test COMMIT, OUT and FRAME cases, including a displaced destination HIGH while its canonical source is LOW. Gate: six-lane engine preserves untouched channels. |
| T05 / Phase3 | §§5,10,16,21.4 | `TempiMemory`: saved/working layers, all transactions, clipboards, dirty comparison, mutation and masked restore records. `test-tempi-memory`: MEM/RNG; verify exact draw consumption and record/restore without needing Rack history yet. |
| T06 / Phase3 | §§11,15,18.2 | `TempiSelector` and `TempiPerformance`: observation versus base, freeze, pending proposal, Shift mapping, MOD latch, both Run displacements. `test-tempi-performance`: SEL/SHIFT/RUN, same-time selection conflicts and no-op routing. |
| T07 / Phase4 | §§12–13 | `TempiHuman`: four fixed taps with captured phase, history/activity lifetimes, exact candidate/tie rules, Channel1 Leading Tap. `test-tempi-human`: HUMAN cases and review policy vectors. No delayed GUI timestamps. |
| T08 / Phase4 | §14,§18.2 | `TempiGestures`: fixed page/gesture state, press ownership, consumption, deferred Human action and threshold ties. `test-tempi-gestures`: every page action and GEST replay, including context-menu State Edit and focus loss. |
| T09 / Phase5 | §§3,18–19,21 | Complete adapter and serialization: separate display/persistence snapshots, per-frame processing, lifecycle, JSON repairs, version recovery and custom masked undo. `test-tempi-rack`: PATCH/RACK/SNAP/HIST, allocations trap and ownership stress. Core tests stay headless. |
| T10 / Phase5 | §§4,14.5–14.6,20 | Build master SVG, existing controls/themes, lights, numeric inspector, help overlay and accessible commands. Regenerate split panels/anchor atlas if used. Manual gate: every command in the UI inventory is reachable; null-module preview is safe. |
| T11 / Phase6 | §17,§18 | `TempiSelectBus`: parser and semantic executor; UI hex injection. `test-tempi-bus`: BUS cases, packet splits at every byte boundary, exact status recovery, all128 F4 data values and source-sensitive copy order. No external hardware bridge required. |
| T12 / Phase7 | §§22–26 | Add aggregate `test-tempi` depending on all Tempi tests, plus replay/performance targets. Run supported-rate60-second and event-driven24-hour traces, sanitizer tests and native plugin link. Finish manual, architecture, fidelity notes, demonstrations and requirement-result matrix. |

Core orchestration (`TempiCore`) grows as each tested component arrives. A
later packet may add a dependency method, but must not bypass an earlier
component's tests or implement a competing copy of its rules in the widget.
Parser tests may be developed earlier; production integration is T11.

## API and ownership checkpoints

- T00: fixed enums/types; no strings, JSON or Rack includes in core headers.
- T02–T04: timestamp domains named in fields (`sample`, `tick`, `masterCycle`);
  documented rounding at each conversion; no raw absolute-phase float.
- T05: program equality compares fields; all operations specify changed fields,
  whether activation is required, and whether PRNG advanced.
- T06–T08: parser, selector and gesture states remain separate. A page flag
  must not secretly become the source of Run permission or overwrite memory.
- T09: one writer per queue, one consumer per snapshot exchange, generation
  rejection on reset/load, and an actual working Rack undo route. Serialization
  must pass a concurrent save/edit test before UI polishing.
- T10: reuse the SVG components/anchor/theme patterns. No off-thread NanoVG or
  GL work, no clocks advanced from drawing, no implementation jargon in normal
  user controls. Advanced policy diagnostics can live in the inspector.
- T11: whole-packet admission is atomic; a rejected injection leaves already
  accepted parser state intact. Verify command effects, not only parser logs.

## Per-packet completion record

Maintain `doc/tempi/implementation-status.md` with one row per T00–T12:
`status | changed files | exact commands executed | result/log | remaining work`.
List main-spec acceptance IDs covered by each test. A compiling stub is not a
passing musical test. Use `not run` for unavailable platform or host checks.

After each packet: run its focused target, dependent earlier targets affected
by changed code, and native `plugin.dll` when adapter/production linkage changed.
Do not repeatedly run unrelated expensive suites after unchanged passing code.
At T12 run the aggregate and routine repository suite, distinguishing baseline
failures from new ones. Do not install, stage or commit unless separately asked.

## Minimal replay interface

Fixtures use a fixed sample rate, an initial validated persistent payload, and
events with integer `sample` and monotonic `sequence`. Supported kinds are
port connect/value changes, panel/keyboard edges and typed semantic commands.
Unspecified inputs retain their previous values. Events at the same sample
follow the spec's bus -> gesture -> UI ordering, with FIFO within a source.

Reports contain sample index, relative virtual tick, event kind, zero-based
destination/index, internal/physical distinction and diagnostics. Independent
Python `Fraction` oracles generate expected musical deadlines offline. Keep
those expected traces checked in and immutable during test execution. Rate
changes specify the new duration for the following sample interval, per §18.1.

Required demonstrations: factory polymetric clocks; Shift plus Alternate
Run/Stop; unsaved editing followed by Store/Recall. The matching Rack patches
must use the actual plugin/model slug after registration, not guessed IDs.

## Stopping and resuming a work session

Stop at a packet boundary when possible. Record exact failing test IDs and
next files to touch. Resume from this record and the existing code, not by
recreating completed components. An evidence discrepancy gets one explicit
issue with inputs/expected/actual; it is not permission to relabel a P policy
as recovered firmware or omit an entire feature.
