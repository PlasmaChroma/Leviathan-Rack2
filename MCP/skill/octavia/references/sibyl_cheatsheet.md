# Sibyl Agent Cheatsheet

Dense operational quick-reference for authoring, sequencing, and inspecting Leviathan:Sibyl.

## 1. Orientation & Session Bootstrap

Call `vcv_sibyl_bootstrap` once at session start:
```json
{"params": {"module_id": null, "page_size": 8}}
```
* **Omitted `module_id`** resolves the unique Sibyl module. Explicit `module_id` validates against patch inventory without redirection.
* **Returns** bridge state, module ID, compact capabilities manifest, initial arrangement page (scenes + pattern summaries, no raw events), runtime transport/scene state, and verified `consistency.acceptedRevision`.
* **Inconsistent read** (`instance_changed` or `revision_mismatch`) requires retrying bootstrap; do not guess a revision.
* **Legacy fallback** (when bootstrap is unadvertised): call `vcv_get_status` -> `vcv_sibyl_resolve` -> `vcv_sibyl_get_capabilities(compact=true)` -> `vcv_sibyl_get_composition(view="arrangement", page_size=8)` -> `vcv_sibyl_get_status`.

---

## 2. P8 Columnar Event Batches (`insert_note_batch`)

Use `insert_note_batch` for token-efficient note entry. Wire format:
```json
{
  "op": "insert_note_batch",
  "pattern_id": "lead",
  "encoding": "columns_v1",
  "columns": ["tuned.step", "velocity"],
  "defaults": {"tuned": {"context": "arch_38"}, "gate": 0.8, "velocity": 0.75},
  "rows": [[0, 0.9], [13, 0.75], [22, 0.85]],
  "onsets": {"start": 0, "spacing": 2, "count": 16},
  "overrides": {
    "15": {"tuned": {"context": "arch_38", "ratio": "7/4"}, "gate": 0.0}
  },
  "collision": "replace"
}
```
* **Expansion order:** `defaults` applied first -> `rows` cycle until `count` events reached -> `overrides` replace whole named fields (including nested objects) on zero-based row indices.
* **Onsets:** `start` (0–1023), `spacing` (1–1024), `count` (1–1024). Conflicting `step` column with `onsets` is rejected.
* **Null cells:** `null` in a row omits that cell, preserving the default or inheritance.
* **Limits:** at most 1024 columns/rows/count.
* **Legacy fallback:** `tools.sibyl_composer.expand_note_batch(op)` automatically compiles into canonical `insert_notes` for pre-P8 bridges.

---

## 3. Structural Reuse (`clone_scene` & `clone_pattern`)

Reuse existing patterns and scenes rather than regenerating full note payloads:
```json
{
  "op": "clone_scene",
  "source_id": "verse",
  "id": "chorus",
  "patterns": "share",
  "track_overrides": {
    "bass": "bass_chorus",
    "lead": {"overrides": {"transposeSemitones": 12, "velocityScale": 1.2}}
  },
  "repeats": 2,
  "position": "after_source"
}
```
* `patterns: "share"` references source patterns; `patterns: "copy"` duplicates bank patterns with unique IDs.
* `clone_pattern`: `{"op": "clone_pattern", "source_id": "bass", "id": "bass_chorus"}` duplicates notes while preserving pattern-local IDs.
* `track_overrides` accepts a replacement pattern string ID or assignment object with `phaseMode` and `overrides`.

---

## 4. Two-Phase Commit (`prepare` + `handle`)

Avoid retransmitting operations twice across validation and execution:
```json
// Step 1: Validate with prepare
vcv_sibyl_validate({
  "module_id": 42,
  "expected_revision": 12,
  "prepare": true,
  "ttl_seconds": 60,
  "return_changes": false,
  "operations": [...]
})
// Response: {"ok": true, "valid": true, "handle": "prep_rev12_1", "expiresInSeconds": 60}

// Step 2: Commit by handle
vcv_sibyl_edit({
  "module_id": 42,
  "handle": "prep_rev12_1",
  "phase_policy": "preserve",
  "apply_at": "nextBeat",
  "response_profile": "receipt"
})
// Response: {"ok": true, "revision": 13, "activeRevision": 12, "appliedOperations": 3}
```
* If revision advances between validate and edit, the handle is rejected (`stale_prepared_handle` or `revision_conflict`).
* Replaying an already-committed handle returns the original receipt without duplicating edits or undo steps.

---

## 5. Verification & Response Profiles

Always supply `response_profile` on `vcv_sibyl_edit`:
* `"receipt"` (default): compact confirmation (`revision`, `activeRevision`, `appliedOperations`, `warnings`, and nonzero `changes` totals). ~90 chars.
* `"summary"`: per-operation counts without individual event dumps.
* `"full"`: detailed before/after diffs (use only for test probes).
* **Revision invariant:** `revision` is the accepted authored composition; `activeRevision` is currently sounding in DSP. An accepted edit with `activeRevision != revision` is pending its quantized boundary (`applyAt`); do not re-edit.
* **Audible invariant:** Acceptance confirms score compilation, not audible sound. Sound requires transport running, non-muted track, and physical cables.

---

## 6. Deterministic Python Scripting (`tools/sibyl_composer.py`)

For algorithmic, Euclidean, microtonal, or multi-track material, write and execute a local scratch script:
```python
import sys
from pathlib import Path
repo_root = Path("c:/msys64/home/Plasm/Leviathan")
sys.path.insert(0, str(repo_root))

from tools.sibyl_composer import SibylClient, SibylScore, PatternBuilder, EdoTuning

client = SibylClient()
mid = client.get_module_id()
status = client.get_status()
rev = status["revision"]

# 38-EDO scale with Bjorklund Euclidean distribution
edo = EdoTuning(38)
lead = PatternBuilder(16, pitch_context="arch_38")
lead.euclidean(pulses=7, tuned_step=0, gate=0.7, velocity=0.85)

score = SibylScore("Piece", bpm=124.0)
score.add_pattern("p_lead", lead)
receipt = score.two_phase_commit(client=client, expected_revision=rev)
print("Committed revision:", receipt.get("revision"))
```
* Keeps large intermediate calculations and JSON serialization out of conversation context.

---

## 7. Map to Detailed References

Consult detailed references only when actively authoring these features:
* **Repeat Conditions:** `references/sibyl.md#deterministic-repeat-conditions-p2`
* **Evolving Repeats:** `references/sibyl.md#evolving-repeats`
* **Relative Harmony & Chords:** `references/sibyl.md#harmonic-progressions-and-relative-pitches-p5`
* **Chord Voicing:** `references/sibyl.md#p6-generate-fixed-chord-voices`
* **Independent MOD Automation:** `references/sibyl.md#independent-mod-automation-p4`
* **Microtonal Pitch Systems (N-EDO & Scala):** `references/sibyl.md#p7a-native-static-pitch-systems-schema-4`
* **Audio Probes & Monitoring:** `references/monitoring.md`
