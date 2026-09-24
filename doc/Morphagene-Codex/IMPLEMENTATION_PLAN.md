# Leviathan Chimera — Gated Implementation Plan

Read the main specification and `ACCEPTANCE_TESTS.md` first. This plan sequences one complete module, not a set of independent optional prototypes. Preserve the current checkout and its uncommitted work. Do not change branch, globally restructure rendering, or replace unrelated modules to make integration easier.

Each phase ends with a reviewable build/test checkpoint. Record real results and unresolved issues in `doc/Morphagene-Codex/IMPLEMENTATION_STATUS.md`. Do not report tests as passed without running them. A failed prerequisite blocks dependent phases; an environment limitation is recorded explicitly with the exact command and output. `REVIEW_NOTES.md` records revision-2 design corrections and outstanding integration proofs; it is not implementation status.

## Phase 0 — Inspect and establish the integration baseline

Read `AGENTS.md`, `Makefile`, manifest/model registration, plugin edition handling, existing test infrastructure, Octavia semantic dispatch, panel/label anchors, sample loaders, SRC dependency, patch asset hooks, executor shutdown, and snapshot utility contracts. Resolve the actual working branch/commit rather than assuming the researched `expander` files still match.

Produce a short integration note containing the existing paths/types to reuse, available decoder and Speex linkage, compiler standard/math flags, how non-RT jobs are dispatched, who calls `onSave` and `dataToJson`, how module duplication obtains assets, and how the plugin drains workers. Compare the assumptions in the specification against this checkout; explain necessary adaptations without silently changing behavior.

Prototype save failure and serialization paths early: Save/Save As, autosave without onSave, shutdown, stopped/bypassed saves, and same-process duplication of unsaved audio. The void save hook is not proof of host-level cancellation. Verify the section 20.3 fallback and record the runtime's behavior; never assume throwing is safe in every caller. Prove queue event-history capacity at 768 kHz and account for leased retired stores in memory admission before freezing the architecture.

**Deliver:** baseline build results; proposed new files; frozen-ID fixture; a dependency/lifecycle map; no destructive checkout operations.

**Exit gate:** current plugin builds or pre-existing failures are isolated; engine harness target can compile a trivial Rack-independent test; no guessed API remains in the integration plan.

## Phase 1 — Types, mathematical profile, and deterministic core harness

Add fixed capacities, stable enums, region/frame types, profile-v1 transfer functions, smoothers, xorshift32, cubic interpolation, region wrapping, window reference, and input/output PODs. Establish 64-bit logical time and double fractional source coordinates. The harness accepts explicit controls and frame events and captures state/output without GUI or filesystem access.

Implement the reference-vector test reader or port the supplied vectors into the existing test convention. Test tiny regions and non-finite inputs first. Establish allocation traps and sanitizer targets now, rather than after the engine is large.

**Deliver:** engine skeleton; profile math and independent tests; explicit rounding and onset/expiry conventions.

**Exit gate:** CTL-003..011/013 and isolated DSP-001/002/005/006/011/013 pass; no Rack headers in the DSP layer; IDs match section 3.

## Phase 2 — Reel ownership, snapshots, and background service foundation

Implement page storage, valid length, marker table and stable IDs, prefaulted allocation, full COW reserve, incremental reference capture/reclamation, immutable snapshot handles, generation/cancellation, and worker retirement. Start with small stores in tests, then one full-sized allocation test. Use an explicit core ownership guard for stopped-host maintenance; never substitute a plain-memory seqlock.

Implement service/control-dispatch queues with exactly documented producers and consumers. Add a synthetic “write every frame while snapshotting” test before real WAV I/O. Establish the per-module payload budget and assert it at handle transfers.

**Deliver:** `Reel`, `Jobs`, ownership diagram, synthetic immutable snapshot encoder, active/prepared/retired handle tracking.

**Exit gate:** IO-001..006/011/016..018 on synthetic data pass; no worker reads mutable sample pages; module removal cannot leave a worker holding a dangling module pointer. Instrumented callbacks show zero allocations.

## Phase 3 — First audible vertical slice: full-Splice playback and recording

Register exactly one `modelChimera` and `Chimera` manifest entry at the start of this phase, with a temporary developer panel. Wire one full-Splice read head, signed classic rate, stereo normalization, Slide, Play, direct live monitor, linear S.O.S., and independent Current/Append writer. Establish read-before-write ordering, fixed 48 kHz write advancement, valid-length handling, sample guards, and inop routing. A monolithic final module is not acceptable.

Use tiny buffers to prove the TLA recurrence, one-frame read/write ordering, initial recording, append capacity, and exact start/stop frame inclusion. Add DC conditioning only after the unconditioned recurrence tests are stable.

**Deliver:** useful record/loop/reverse/overdub instrument at 48 kHz; no granular or clock shortcuts hidden in the writer.

**Exit gate:** REC-001..006/009/010/016/017 and DSP-004/016 pass (snapshot reload portions complete in Phase 7). Recorded frame count is independent of pitch and Play state. This is a milestone, not completion of the requested module.

## Phase 4 — Finite Genes, Morph, Dynamic Enveloping, and derived outputs

Add finite Gene duration, the distinct primary cycle, density/onset phase, four musical slots, bounded transition readers/residuals, unity-density edge policy, configured signed chord ratios, deterministic pan, and common stereo normalization. Add envelope/ramp CV and EOSG timing from actual natural completion events.

Test finite-duration invariance against speed and source travel at full-Splice size side by side. Use a tagged source to distinguish old/new regions and a constant source to expose unintended gain growth or periodic amplitude holes.

**Deliver:** complete unclocked granular playback; all profile primitives remain isolated and versioned.

**Exit gate:** DSP-003..020, OUT-001..005/009, and REC-018 pass; at most eight readers; no secondary voice controls the primary selection cycle; no generic Hann-grain engine standing in for the specified model.

## Phase 5 — Transport arbitration, Clock, buttons, and remaining firmware options

Add Organize hysteresis and stable-knob/Shift arbitration, queued/immediate selection, all Play modes, host-independent timestamp ordering, Clock estimator/Gene Shift/Stretch/hybrid behavior, clocked recorder arms, exact same-frame precedence, REC/SPLICE/SHIFT button release/chord state, Reel Mode selection, rsop, gain switching, options transactions, and PM signal-presence/routing.

Test every option as behavior, not just a serialized checkbox. Test connected-but-stopped Clock separately from disconnected Clock. Keep the proposed clock stride, PM scale, and button choreography visibly labeled software design choices.

**Deliver:** all original control names and mode options function at 48 kHz; complete explicit state machines.

**Exit gate:** all TRN tests, REC-007/008/011..014, all OPT tests, and OUT-006..008 pass; no double onset on an edge or hidden gate/button-chord conflation.

## Phase 6 — Host-rate bridge and realtime hardening

Prepare Speex converters and fixed FIFOs outside real-time execution. Integrate delayed continuous-control sampling, host-edge timestamp capture, logical core event mapping, output CV/EOSG alignment, 48 kHz bypass, and sample-rate-change handoff. Implement bypass and lifecycle behavior against the actual Rack API.

Instrument queue bounds, maximum work per callback, SRC delay, and converter ownership. Stress highest source-read rates, smallest Genes, lowest/highest host rates, snapshots, and changing sample rates while recording. Treat 8 and 768 kHz as boundedness/recovery extremes; assess practical real-time playback on the required 44.1–192 kHz rates, and report host overload separately from module cost. Keep all diagnostics fixed-size on the audio side.

**Deliver:** host-rate-independent 48 kHz Reel engine, predictable event alignment, zero-allocation callbacks.

**Exit gate:** RT-001..011 pass at required rates; delays are measured rather than assumed; audio timing and event timing remain coupled through SRC. Record any performance-target miss separately from functional failures.

## Phase 7 — WAV/banks, portable patches, destructive editing, and undo

Implement bounded RIFF reader/writer and strict/convenience import around verified existing dependencies. Add frame-based cues, 32-slot filename mapping, immutable cache files, module-relative embedded assets, manifest/schema migration, coherent SaveBundle fencing, atomic staged commits, autosave checkpoints, missing-source diagnostics, and safe duplication.

Add worker-side erase/compact/clear, stable marker metadata edits, file-backed undo/redo and pre-record checkpoint restoration. Confirm destructive operations and external overwrites independently. Test with original files moved away and with a clean temporary plugin data directory.

Do not return from the save hook before referenced audio assets are complete. Do not use absolute import paths or giant base64 JSON fields as the default persistence implementation. Test paused-host saves and full banks before UI polish.

Write valid frames only to the embedded float32 WAVs. Rack already applies level-1 Zstandard compression to the `.vcv` archive, so benchmark representative patch sizes and save/load time before considering a second codec. Track autosave/checkpoint disk usage separately because those working files are not compressed by the patch archive.

**Deliver:** portable patches and bank interchange, explicit error/recovery paths, recording-safe saves.

**Exit gate:** all IO/WAV/STA cases pass. Use disk-failure injection. An internal marker roundtrip is not advertised as physical Morphagene compatibility.

## Phase 8 — Original panel, cached display, and production widget

Keep the model and manifest entry registered in Phase 3; replace its temporary developer panel with the production widget. Use the current Leviathan panel/labels/anchors and existing knobs/jacks. Implement the 28HP starting layout, accessible states, gain/options/reel menus, waveform cache and bounded playhead overlays, null-module rendering, and visible job/error state.

Build waveform peaks off audio, incrementally publish record changes, and avoid scanning all samples from `draw()` or rebuilding textures every frame. Respect current GL context ownership; no bespoke shader system is required.

Author `res/Chimera.svg` and regenerate both split assets plus the anchor atlas. Use the shared NanoVG/GL lifecycle helpers and preserve module-total Process/Step/Draw telemetry behind the existing debug gate.

**Deliver:** polished original light/dark-compatible module, user documentation, no placeholder ports or silent options.

**Exit gate:** GUI-001..005 pass; instrument actual frame costs under max markers/voices and offscreen/context transitions; existing modules' rendering is unaffected.

## Phase 9 — Octavia semantic integration and transaction safety

Implement the existing semantic-control interface with capabilities, authored document, validate/edit, status, and explicit commands. Use the same service/command path as UI, expected revisions, applied acknowledgements, finite idempotency cache, and existing filesystem authorization boundaries. Do not add a new MCP/network server inside the module.

Exercise complete agent workflows: import → choose Splice → set grain/pitch/clock options → arm record → inspect actual state → stop → checkpoint/export. Ensure state is compact enough for repeated agent use and does not expose raw audio accidentally.

**Deliver:** production semantic adapter and compact documented command examples.

**Exit gate:** API-001..006 pass; retries cannot double-toggle recording; status does not claim success while a load/transaction is only queued.

## Phase 10 — Regression, packaging, calibration ledger, and release review

Run the repository's required `test-fast` suite and module-specific integration/slow/sanitizer tests. Follow `AGENTS.md`: native MINGW64 `plugin.dll` is authoritative on Windows; native Rack-linked tests use `make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`. `test-rack` remains work in progress and is not a blanket prerequisite; run relevant cases only when intentionally targeted, documenting pre-existing failures. Run ASan/UBSan/TSAN in supported configurations (a separate compatible Linux harness may be needed), without substituting a Linux link for Windows validation. Measure core, SRC, snapshot, memory, and GUI costs on named hardware. Perform a manual patch/session across record, Shift, reverse, finite grain, overlap, clock stretch, TLA, PM, export, reload, duplication, undo, and module deletion. Do not stage or commit files.

Publish a conformance checklist, known limitations, exact behavior of every option, source-derived versus software-defined calibration ledger, and public attribution/naming status. No claim of recovered firmware, unknown hardware gain/window/interpolation fidelity, or byte-identical markers is allowed without new evidence.

**Deliver:** implementation summary listing changed files, builds/tests and actual outcomes, artifacts, remaining failures, and measured performance. Include a portable demonstration patch using generated audio only.

**Exit gate:** main specification section 26 complete; all mandatory acceptance cases executed and passing; optional physical-hardware comparison explicitly pending or supported by actual measurements.

## Review traps that must block approval

A generic granular player that cannot rerecord its processed output; a writer tied to playback speed; finite Genes whose duration changes with pitch; an always-on dry Reel voice hiding gaps; a full Hann replacing short default windows; a stationary Organize knob undoing Shift; per-secondary-voice selection commits; implicit input summation/polyphony; thread-safe-looking plain-memory races; audio-thread last-owner destruction; background patch save that races the archive; changed assets in the user's original import directory; accepted jobs reported as applied; massive module constructors in the browser; and render-time whole-Reel scans.

These are architectural failures, not polish items. Resolve them before claiming feature completion.
