# Codex starting instruction

Use the following as the implementation task in the actual Leviathan checkout. Put this package in a documentation directory accessible to the agent; do not overwrite the repository's existing `AGENTS.md`.

---

Implement **Chimera**, the complete Morphagene-inspired Leviathan module specified by this package inside the existing Leviathan VCV Rack plugin.

First read the repository's `AGENTS.md`, inspect the current branch/dirty status without modifying it, and read:

1. `Leviathan_Morphagene_Codex_Spec.md` — normative behavior, architecture, numerical profile, ownership, serialization, UI, and semantic API.
2. `ACCEPTANCE_TESTS.md` — normative verification cases, not already-passed results.
3. `IMPLEMENTATION_PLAN.md` — gated sequence and integration checks.
4. `reference_vectors.json` and `generate_reference_vectors.py` — mathematical reference anchors.
5. `Morphagene_Tech_Brief.md` — source behavior and its explicit uncertainties.
6. `REVIEW_NOTES.md` — revision-2 corrections, choices, and required early integration proofs.

The actual repository APIs and safety conventions take precedence over guessed file signatures in the spec. Preserve existing modules, enum values, JSON, edition identity, renderer infrastructure, and unrelated user changes. Do not change branches or reset files. Explain any integration adjustment while preserving the behavioral contract.

Work through the phases in order with build/test checkpoints. Create `doc/Morphagene-Codex/IMPLEMENTATION_STATUS.md` showing completed phases, changed files, executed commands/results, and remaining work. Prove the save-hook failure/autosave/shutdown behavior in Phase 0 before building persistence around it. Do not stop permanently at a granular playback demo and call the module finished: recording topology, transport/options, sample-rate bridge, safe patch assets, cached display, and semantic control are all part of the requested implementation. Do not stage or commit files.

Keep DSP Rack-independent and split by responsibility. The 48 kHz core owns mutable sample pages; all reads occur before that frame's write. No allocation, final-owner free, blocking lock, filesystem access, JSON/string formatting, or thread join is permitted in the audio path. Do not substitute unsafe snapshots or absolute-path-only sample persistence to accelerate implementation.

Where the brief is uncertain, implement profile 1 exactly as specified and label it a software design choice. Do not invent claims that the hardware uses the chosen curves, windows, clock stride, gain law, PM scale, or RIFF marker layout. Future sound refinements need a new profile rather than silently retuning saved patches.

Run focused tests as each phase lands and the complete suite at completion. Report actual pass/fail/not-run, including environment limitations; never claim a build or sanitizer run that was not executed. Produce a final changed-file summary, tested portable demonstration patch using generated audio, measured performance, and clear remaining limitations.

---

## Resuming after a context reset

Read the implementation-status file, inspect the working diff, rerun the last phase's focused tests, and continue at the first incomplete gate. Do not assume a status checkbox substitutes for current test results. Keep accepted but unfinished work visible rather than restarting the module or replacing prior architecture wholesale.
