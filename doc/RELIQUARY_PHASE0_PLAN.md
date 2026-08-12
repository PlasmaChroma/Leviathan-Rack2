# Reliquary Phase 0 — Dependency and Feasibility Plan

**Project:** Leviathan-Rack2

**Component:** Reliquary

**Document status:** Active preimplementation plan, Draft 0.1

**Date:** 2026-08-12

---

## 1. Authority and purpose

This plan governs the experiments, measurements, decision records, and formal
go/no-go gates that precede production Reliquary implementation.
[RELIQUARY_SPEC.md](RELIQUARY_SPEC.md) owns product behavior and architectural
requirements. [RELIQUARY_UNRAR_SPEC.md](RELIQUARY_UNRAR_SPEC.md) owns the
candidate archive-reader design and hostile-input contract.

Phase 0 exists to retire the risks that would otherwise force a redesign after
module code begins:

- dependency-license incompatibility;
- inability to observe effective S-DSP events;
- inability to access stable audio RAM and physical voice state;
- unacceptable emulator or archive binary size;
- unsafe or unbounded archive behavior;
- inability to meet Rack's real-time budget;
- incorrect assumptions about isolated voice monitoring or speed control;
- cross-platform build failure.

Phase 0 may produce disposable command-line harnesses, narrowly scoped adapters,
patch files, fixture-generation notes, and measurement reports. It shall not
create the production module, freeze Rack parameter IDs, or commit to a panel.

---

## 2. Exit condition

Phase 0 is complete only when:

1. The exact archive and SPC runtime dependencies are approved or rejected with
   written license and provenance records.
2. At least one archive-reader path passes the required RSN corpus and safety
   gates.
3. At least one SPC runtime passes authentic playback, event observation,
   deterministic reset, audio-RAM access, and real-time performance gates.
4. The minimum C++11 project-owned interfaces are frozen for Phase 1.
5. Selected-voice monitoring and speed control are each explicitly approved or
   deferred; neither may remain an implicit promise.
6. Platform, CPU, memory, latency, and stripped-size results are recorded with
   reproducible commands and fixture hashes.
7. The accepted decisions are written back to the governing specifications.

Passing Phase 0 authorizes Phase 1 engineering. It does not by itself approve a
public release.

---

## 3. Current baseline

At the start of this plan:

- Reliquary has no production source, panel asset, model registration, or test
  target in the repository.
- The product architecture exists in `RELIQUARY_SPEC.md`.
- Embedded UnRAR is a candidate described by
  `RELIQUARY_UNRAR_SPEC.md`, not an approved dependency.
- Leviathan declares GPL-3.0-or-later.
- The Rack plugin build selects C++11. Individual offline test tools may use a
  newer standard, but their convenience shall not leak into production APIs.
- WSL is suitable for editing, unit tests, corpus tests, and source-level
  measurements, but it is not authoritative for the final Windows plugin link.
- The user's Windows/MSYS2 Rack toolchain is authoritative for the Windows
  binary. Matching native or cross toolchains are required for other release
  platforms.

Every Phase 0 report shall name the repository revision or dirty-tree state used
for the measurement so later results are comparable.

---

## 4. Required decision records

Phase 0 shall produce these records under `doc/` or in a single consolidated
Phase 0 report:

| Record | Required decision |
|---|---|
| R0 | Archive dependency license/provenance approval or rejection |
| R1 | SPC runtime candidate license/provenance approval or rejection |
| R2 | Archive-reader selection and pinned version |
| R3 | SPC runtime selection and pinned version |
| R4 | Frozen C++11 archive and runtime adapter boundaries |
| R5 | Effective event model and timestamp origin |
| R6 | Resampler quality/cost selection and latency |
| R7 | Real-time, memory, load-latency, and binary-size result |
| R8 | Selected-voice monitoring approval or deferral |
| R9 | Speed semantics approval or deferral |
| R10 | Platform support result and remaining release-only verification |

Each record includes date, exact inputs, alternatives considered, measurements,
decision, rejected alternatives, consequences, and the specification sections
updated by the decision.

---

## 5. Workstream A — licensing and provenance

This workstream runs before dependency source enters the production tree.

### 5.1 Archive candidate

For the exact official UnRAR release proposed for evaluation:

- record upstream URL, version, archive SHA-256, import date, complete license,
  acknowledgements, and intended local patch policy;
- compare its restrictions with Leviathan's GPL-3.0-or-later distribution and
  static-link design;
- determine whether a valid distribution path exists, whether an exception is
  required and grantable, or whether the candidate must be rejected;
- document who reviewed the determination and its scope;
- do not treat retention of notices as proof of compatibility.

If embedded UnRAR is rejected, shortlist replacement readers behind
`IRsnArchiveReader` and repeat the same license, format-coverage, security,
binary-size, and portability analysis before archive code proceeds.

### 5.2 SPC runtime candidates

Build a candidate matrix containing:

- upstream project and pinned revision/release;
- license and redistribution conditions;
- SPC snapshot fidelity and known compatibility scope;
- S-DSP/SPC700 architecture and native output rate;
- effective register/event observability;
- read-only audio-RAM access;
- physical voice snapshots;
- deterministic reset or state-copy support;
- per-voice pre-summation access;
- build language/standard and platform history;
- estimated source modifications and maintenance burden;
- stripped static size before and after Leviathan integration.

A candidate failing license approval is removed before technical adaptation,
regardless of feature quality.

---

## 6. Workstream B — fixture and reference corpus

### 6.1 SPC corpus

Create a manifest for synthetic or clearly redistributable SPC fixtures that
exercise:

- silence and simple sustained tone;
- KON/KOFF timing and retrigger on one physical voice;
- all eight physical voices;
- physical voice replacement;
- one timbre moving between physical voices;
- simultaneous siblings using one BRR identity;
- ADSR and GAIN modes;
- looping and non-looping BRR samples;
- noise, pitch modulation, echo, and sample end;
- directory or BRR data changed at runtime;
- malformed BRR traversal boundaries;
- metadata duration/fade variants.

For every fixture, record provenance, SHA-256, expected metadata, expected event
sequence where known, and whether an audio reference is exact or tolerance
based. Copyrighted soundtrack captures may supplement private listening tests
but shall not be required for repository tests or CI.

### 6.2 RSN corpus

Use the corpus categories and provenance rules in the archive specification.
Each valid archive contains synthetic SPC-shaped or valid synthetic SPC data
with known hashes. Include solid archives whose unrelated preceding entries
expand enough to prove that discarded bytes count toward the cumulative limit.

### 6.3 Reference integrity

The corpus manifest itself is versioned. Changing a fixture, expected hash,
tolerance, or authoring method requires a reviewable manifest diff; silently
re-recording expected output after a failure is forbidden.

---

## 7. Workstream C — archive feasibility spike

After Gate G0 approves a candidate:

1. Build the upstream-equivalent library/object set in `RARDLL` mode without
   production integration.
2. Record baseline stripped object/library size and exported symbols.
3. Implement the smallest C++11 project-owned harness capable of opening one
   archive, reading headers, capturing `RAR_TEST` output, and closing safely.
4. Add checked 64-bit size reconstruction, dictionary-unit conversion, bounded
   callbacks, cancellation, solid-history accounting, SPC classification, and
   stable project error mapping.
5. Run the complete archive corpus under ordinary tests and available memory,
   address, and undefined-behavior sanitizers.
6. Measure archive load time, cancellation latency, retained bytes, total actual
   expanded bytes, and peak resident memory.
7. Verify no temporary file or external process is used.
8. Link privately into a minimal plugin-shaped binary and inspect dynamic
   dependencies and exported symbols.
9. Measure untrimmed and safely trimmed stripped-size deltas.

The spike shall test a callback exception path explicitly and prove that no C++
exception crosses the C ABI.

---

## 8. Workstream D — SPC runtime feasibility spike

For each technically viable, license-approved candidate:

1. Build an unmodified or minimally adapted baseline.
2. Load the simple corpus and render deterministic native-rate audio.
3. Record audio hashes for exact cases and bounded comparison metrics for cases
   where implementation details legitimately differ.
4. Demonstrate read-only access to the complete 64 KiB audio-RAM view.
5. Capture physical voice state and timestamped register/effective events into a
   caller-owned bounded buffer.
6. Distinguish KON writes from effective key-on boundaries.
7. Prove a bounded ENVX/OUTX delivery strategy—timestamped state events,
   caller-owned per-native-frame state, or bounded render quanta—that meets the
   one-Rack-sample alignment target without allocation.
8. Reset to the original snapshot repeatedly and compare audio, events, and
   visible state.
9. Exercise repeated load/render/reset/destroy cycles under sanitizers where
   available.
10. Record all upstream modifications needed for observability.
11. Reject candidates requiring allocation, locks, logging, or unbounded
    callbacks in the render path unless those behaviors can be removed cleanly.

The spike must demonstrate that the information required for stable logical
mapping exists. It need not implement the Phase 2 production router.

---

## 9. Workstream E — selected-voice monitoring

This workstream is optional for Core MVP selection but mandatory before SOLO
L/R is promised.

Evaluate in this order:

1. Capture the selected physical voice's pre-summation stereo contribution from
   the primary render.
2. If unavailable, test an internal checkpoint/masked-render/restore strategy.
3. Reject any method that externally advances the runtime twice, perturbs the
   primary mix, shifts events, or leaves different state.

For a checkpoint-based method, compare against one ordinary render over the
approved corpus:

- bit-identical main audio where the baseline is deterministic;
- identical post-call CPU, DSP, RAM, timer, and event-visible state;
- identical event sequence and timestamps;
- stem alignment to the exact primary native frames;
- CPU increase within the architecture budget;
- no cost beyond one predictable capability branch while disabled.

Result R8 is binary: **approved with named method and limits**, or **deferred**.
“Probably possible later” is deferred.

---

## 10. Workstream F — speed semantics

Do not expose a generic `setTempo()` merely because a candidate core provides
one. For each plausible mechanism, measure and document:

- whether SPC700 timers, driver tempo, DSP sample playback, pitch, envelope
  timing, echo, metadata duration, and event clocks scale;
- whether the mechanism is deterministic across reset and sample-rate changes;
- supported multiplier range and behavior at zero or negative CV;
- CPU/resampler cost;
- musical usefulness on the approved corpus.

Result R9 is either a single precise transport policy suitable for a later
SPEED/SPEED CV specification, or deferral. Speed is not a Core MVP requirement.

---

## 11. Workstream G — Rack adapter and real-time proof

Construct a minimal, non-panel harness around the preferred runtime to prove:

- native rendering and deterministic allocation-free resampling;
- bounded event draining;
- fixed-size voice and lane-state snapshots;
- worker construction and single-owner audio-thread publication;
- generation-based rejection of stale loads;
- two-runtime audio crossfade with staged-only CV authority;
- worker-side reclamation of retired runtimes and collections;
- Rack sample-rate change handling;
- no allocation, file access, mutex wait, or destructor work in the simulated
  `process()` path.

Instrumentation itself shall be debug-gated or compiled into the standalone
harness so production measurements distinguish normal cost from measurement
cost.

---

## 12. Measurement protocol

Every performance result records:

- CPU model, core count, and power/performance mode;
- operating system and whether the environment is WSL;
- compiler and complete relevant flags;
- Rack SDK version;
- dependency version and local patch hash;
- sample rate and block size;
- fixture manifest version;
- warm-up duration, measurement duration, and repetition count;
- average, median, 95th percentile, 99th percentile, and maximum callback time;
- underrun/deadline-miss count;
- peak resident memory and retained source bytes;
- stripped object, library, plugin-binary, and packaged-size deltas.

Minimum real-time configurations are 44.1, 48, and 96 kHz at Rack block sizes
16, 64, and 256 where the harness permits them. The governing 48 kHz/64-sample
targets come from the architecture specification. Test one ordinary instance,
one transition, one approved monitor if available, and eight ordinary
instances.

Measurements run at least five times after warm-up. The report retains all runs
or explains discarded outliers; it does not publish only the best run.

---

## 13. Formal gates

### G0 — dependency legality and provenance

Pass requires approved written records for the exact archive and SPC runtime
candidates, complete source hashes/licenses, and a viable Leviathan distribution
path. Failure blocks source import for that candidate.

### G1 — archive correctness and containment

Pass requires all mandatory archive fixtures, checked limits, actual discarded
solid-byte accounting, deterministic error mapping, cancellation, no temporary
files, no external process, no leaked public symbols, and an approved stripped
size delta.

### G2 — runtime fidelity and observability

Pass requires recognizable/reference-compatible stereo output, deterministic
reset, complete audio-RAM view, physical voice snapshots, effective event
timing, bounded event draining, and no sanitizer failure in the exercised
boundary.

### G3 — mapping sufficiency

Pass requires evidence that BRR identity can be resolved at effective note-on
and that a timbre moving between physical voices can be observed without
mixed-audio pitch detection. This is an information-sufficiency proof, not the
production lane router.

### G4 — real-time architecture

Pass requires no prohibited audio-thread operation, no deadline miss in the
controlled single-instance and transition tests, compliance with the initial
average/99th-percentile budgets, bounded queues, and worker-side reclamation.

### G5 — selected-voice monitor

Optional. Passing approves Phase 4 SOLO L/R with the named implementation.
Failure or incomplete evidence records deferral and removes SOLO L/R from the
committed panel/runtime scope.

### G6 — speed policy

Optional. Passing records one deterministic semantic contract. Failure or
ambiguous semantics records deferral and keeps SPEED/SPEED CV out of scope.

### G7 — platform feasibility

Pass requires source-level and focused-test success in the development
environment plus authoritative Windows/MSYS2 compilation of the selected
dependencies and adapter. macOS and Linux release targets require recorded
native or approved cross-build evidence before release, even if unavailable
during the earliest local spike.

### G8 — production authorization

Pass requires G0 through G4 and G7, explicit outcomes for G5 and G6, completed
records R0–R10, frozen C++11 interfaces, and governing-spec updates. Only then
may Phase 1 production module work begin.

---

## 14. Execution order

1. Freeze the fixture-manifest format and candidate-report template.
2. Complete archive and runtime license/provenance records.
3. Build the synthetic SPC and RSN seed corpus.
4. Run archive and SPC baseline spikes independently after their respective
   legal gates pass.
5. Select the preferred runtime and archive reader provisionally.
6. Prove event/mapping sufficiency.
7. Prove resampling, handoff, transition, reclamation, and real-time budgets.
8. Evaluate selected-voice monitoring and speed without allowing either to
   block the Core MVP runtime.
9. Complete authoritative Windows/MSYS2 verification and record remaining
   platform release work.
10. Update the governing specifications, freeze interfaces, and issue the G8
    decision.

If a mandatory gate fails, return to candidate selection rather than weakening
the gate silently. A revised budget or architecture is valid only after a
written decision record explains why it remains musically and operationally
acceptable.

---

## 15. Phase 0 deliverables

The completed phase hands Phase 1:

- pinned and approved dependency manifests;
- license/provenance decision records;
- synthetic fixture manifests and hashes;
- archive and SPC feasibility harnesses or clearly labeled disposable spike
  results;
- frozen C++11 `IRsnArchiveReader` and `ISpcRuntime` contracts;
- selected event capacity and overflow policy;
- selected resampler and documented latency/reset policy;
- measured CPU, memory, latency, binary-size, and platform reports;
- explicit SOLO and SPEED decisions;
- updated `RELIQUARY_SPEC.md` and `RELIQUARY_UNRAR_SPEC.md` with no unresolved
  mandatory Phase 1 dependency.

The governing principle is simple: Phase 0 converts assumptions into evidence.
Production code begins only after the evidence supports a coherent, legal,
bounded, and real-time-safe design.
