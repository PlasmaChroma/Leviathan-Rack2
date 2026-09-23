# Leviathan Morphagene — Codex specification package

Start with **`CODEX_START.md`** for the task text to give an implementation agent. The main specification is **`Leviathan_Morphagene_Codex_Spec.md`**. This is a design and verification package, not a compiled VCV Rack module or recovered Morphagene firmware.

## Contents

| File | Role |
|---|---|
| `Leviathan_Morphagene_Codex_Spec.md` | Full behavior, DSP profile, state machines, file/thread ownership, Rack integration, persistence, panel, and Octavia contract. |
| `IMPLEMENTATION_PLAN.md` | Eleven gated phases, from checkout inspection through release verification. |
| `ACCEPTANCE_TESTS.md` | 125 individually identified acceptance cases and harness/reporting conventions. |
| `reference_vectors.json` | Machine-readable mathematical anchors for the chosen software profile. |
| `generate_reference_vectors.py` | Standard-library generator/checker for those anchors; no dependency on Rack. |
| `Morphagene_Tech_Brief.md` | User-supplied behavioral source, included unchanged. |
| `CODEX_START.md` | Paste-ready agent task and instructions for resuming work. |
| `PACKAGE_VALIDATION.md` | Checks performed on this package, with explicit limits. |
| `SHA256SUMS.txt` | Checksums of package files, excluding this checksum file itself. |

## Verification provided with this package

Run:

```sh
python generate_reference_vectors.py --check reference_vectors.json
```

This checks the reference file against the formulas and internal anchor assertions. It does **not** build or test a module. The implementing agent must create the engine/Rack test harnesses and execute the acceptance matrix.

## Important boundaries

The source brief supplies the Morphagene behavior and explicitly leaves several low-level primitives uncertain. The spec turns those unknowns into a versioned **software profile**, clearly marked as design choices. Its clock stride, control curves, window correction, interpolation, pan/gain law, PM scale, marker-file profile, and button choreography are not claimed as recovered hardware details.

Selected public Leviathan source files and the Rack SDK informed integration guidance; the actual current checkout and `AGENTS.md` remain authoritative. No local plugin build, running Rack validation, physical-hardware comparison, or sanitizer run occurred while drafting this package.

The intended result is one original Leviathan stereo instrument with all of the supplied brief's control/mode roles, portable saved audio, bounded real-time processing, a cached display, and existing Octavia semantic control—not an independently branded standalone plugin or a playback-only granular prototype.
