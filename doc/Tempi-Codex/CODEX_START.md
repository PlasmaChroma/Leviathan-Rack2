# Codex implementation task

Implement the TEMPI 71 behavioral recreation described in [Leviathan_Tempi_Codex_Spec.md](Leviathan_Tempi_Codex_Spec.md) within this Leviathan checkout. `Tempi` is the working name, not a finalized public identity.

Read repository `AGENTS.md`, inspect the dirty worktree, then read this package, `doc/Leviathan-Standard.md`, and the firmware sources in specification section 2. Preserve unrelated changes. Do not stage or commit. Do not modify the firmware evidence to make the implementation appear correct.

Follow the implementation gates in order. Keep `doc/Tempi-Codex/IMPLEMENTATION_STATUS.md` with requirement coverage, source confidence, unresolved questions, profile decisions, changed files, and commands actually run. The specification's tests are requirements, not already-passing results. Resolve gaps through the local disassembly and harness where possible; record remaining approximations explicitly. Never promote an isolated arithmetic fixture into a claim of complete hardware equivalence.

Implement a Rack-independent timing/state core, six physical output lanes, all memory and performance functions, Human and Machine Programming, and the software-equivalent Select Bus receiver. The six-lane visualization is a first-release requirement and must develop alongside the engine. A clock-divider demo, static display, incomplete memory model, or unlabelled tap approximation does not complete this task.

Use the proposed Rack profile only for the unresolved behavior it explicitly covers. Recovered behavior takes precedence. Profile changes that affect existing patches require versioning. Verify the running Rack version's save/load and lifecycle contracts before choosing cross-thread persistence ownership. UI and JSON must never read concurrently mutated engine arrays without a proven handoff.

Use the existing panel anchors, split SVG workflow, shared controls, theme system, graphics lifecycle helpers, and diagnostics. Keep audio work bounded; no locks, allocations, deallocation, JSON, files, or sockets in `process()`. Follow the native Windows build instructions for authoritative `plugin.dll` validation; WSL-focused tests alone do not establish that result.

At completion deliver the integrated module, generated packaged panel assets, focused tests, example patches, user documentation, measured performance, and an honest fidelity/status report. If reconstruction remains incomplete, finish the specified prototype behavior and report the affected fidelity gate as open; do not call the faithful-recreation milestone complete.
