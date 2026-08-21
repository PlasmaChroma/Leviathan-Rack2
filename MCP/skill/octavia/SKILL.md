---
name: octavia
description: >
  Inspect, analyze, and edit VCV Rack patches live through the Octavia module
  (Leviathan) via the vcv-rack MCP server. Use when the user wants to list or add
  modules, read or change knob values, trace or rewire cables, debug silence, audit a
  patch, save changes, or improve a patch. Trigger on mentions of "VCV Rack",
  "Octavia", modular synth patches, Eurorack-in-software, or vcv_* tools.
---

# VCV Rack — Leviathan Octavia

HTTP server on `localhost:7777`. Plugin: **Leviathan** | Module: **Octavia** | Start button → full-brightness Octopus.

Deep reference (read on demand): `references/tables.md` — module database by category,
quick-pick by task, troubleshooting matrix, CPU optimization.

## ALWAYS START HERE

```
vcv_get_status   →  {"running": true, "port": 7777, "patch": {"path": "...", "hasSavePath": true}}
```
If it fails → tell user to check Octavia + press START. Do not retry automatically.

## Editing Workflow

When the user asks to change the patch, the LLM may use the write tools directly.

1. Inspect the exact target immediately before editing: use `vcv_get_module` for parameters
   and ports, `vcv_list_cables` for routing, or filtered `vcv_list_library` before adding.
2. Translate the user's musical intent into concrete module IDs, parameter IDs/values,
   and port IDs/names. Never guess an ID, parameter range, or plugin/model slug.
3. Make the smallest coherent change. Use `vcv_set_parameters` for one or multiple parameter
   changes because it creates one clean vcv_undo step.
4. Check write responses for `error`, `failedIndices`, or partial bulk application.
5. Verify with the cheapest relevant read: `vcv_get_module`, `vcv_list_cables`,
   `vcv_list_modules`, or `vcv_get_signal_levels`. Report exactly what changed.

Write tools: `vcv_add_module`, `vcv_set_parameters`, `vcv_connect_cables`,
`vcv_disconnect_cable`, `vcv_update_module`, `vcv_layout_modules`, `vcv_set_module_state`,
`vcv_undo`, `vcv_delete_module`, `vcv_save_patch`, and `vcv_reset_loudness`.
Cables default to white; `vcv_connect_cables` accepts optional `color` name ('red', 'green', 'blue', 'yellow', etc.) or hex ('#ffffff').

### Cluster Anchoring & Layout Guidelines

- **Cluster Anchor Rule**: VCV Rack is an unbounded 2D canvas. **Never assume a patch begins at `(row: 0, hp: 0)`.** Patches are frequently built far out in the canvas (e.g. `row 5`, `hp 2000`).
- **Preserve Viewport & Coordinates**: Before placing new modules or rearranging a patch, find the cluster's bounding anchor (the minimum `row` and `posX` across existing active modules). Always place new or rearranged modules **relative to the existing cluster's coordinates** so the patch remains in the user's active viewport.
- **Atomic Multi-Module Layout**: Use `vcv_layout_modules` for multi-module rearrangement. It validates collisions in advance, moves all modules together, and creates a single clean `vcv_undo` step.
- **Functional Row Lanes**: When arranging across rows, use neighboring rows within the active cluster (e.g. Cluster Row $N$ for Sources/CV, Row $N+1$ for Mixing/FX/Mastering) with left-to-right signal flow per row.

### Safety and approval

- Normal reversible edits (parameters, cables, bypass, movement, module addition, state
  restoration) may proceed when the user clearly asked to edit or improve the patch.
- Before an exact state restoration, read and retain the current state until verification.
- `vcv_delete_module` is not undoable through Octavia. Confirm the module details and obtain
  explicit user approval naming the module before deleting it.
- `vcv_save_patch` overwrites the patch's existing file and is not undoable through Octavia.
  Check patch save status via `vcv_get_status` and obtain explicit user approval before saving.
- Do not silently broaden an edit. If the request is exploratory ("what could improve?"),
  advise first; only mutate when the user asks to apply or make changes.
- If verification fails, stop further writes, explain the partial state, and offer/use
  `vcv_undo` for reversible operations. A cable call can partially apply and reports how
  many cables succeeded; vcv_undo that many times if rollback is requested.

---

## Core Mental Model — Signal Topology

Classify modules into roles from the model name alone; reason about signal flow from
`vcv_list_modules` + `vcv_list_cables` without calling vcv_get_module on every module.

| Role | Examples | Characteristic |
|---|---|---|
| **Audio Source** | VCO, Noise, wavetable oscillators | No audio input needed; generate signal |
| **CV Source** | LFO, ADSR, Clocked, SEQ3, random generators | Produce control voltages |
| **Processor** | VCF, VCA, delays, reverbs, waveshapers | Transform audio/CV |
| **Mixer** | MixMasterJr, VCMixer, Mix4 | Combine multiple signals |
| **Sink** | AudioInterface2, main outs | Terminal — no output into rack |
| **Sequencer** | SEQ3, step sequencers | Step-based pattern generators |
| **Bridge** | Leviathan Octavia | Ignore in analysis — always exclude |

Audio flows left-to-right (posX). Trace: Source → Processor(s) → Mixer → Sink.

**Infer WITHOUT extra calls:** polyOut=0 → no active output · polyOut>1 → polyphony starts
here · bypassed=true → passes through · no outgoing cables → dead end · Processor with no
incoming cables → missing source.

---

## Call Budgets

| Task | Calls | Sequence |
|---|---|---|
| Understand unknown patch | **2** | vcv_list_modules → vcv_list_cables |
| Full patch audit | **3** | vcv_list_modules → vcv_list_cables → vcv_find_unpatched |
| Spectral / Mix / Quality audit | **3–4** | spectrum → reset loudness → let audio play ≥3 sec → loudness; repeat spectrum only if feedback is suspected |
| Debug silence | **2** | vcv_get_signal_levels → vcv_get_module(suspect_id) |
| Read one module in depth | **1** | vcv_get_module(id) |
| Module recommendation | **1–2** | vcv_list_library(q=...) — ALWAYS filtered |

---

## Decision Trees

### "Analyze my patch"
1. vcv_list_modules → classify roles · 2. vcv_list_cables → trace chains · 3. vcv_find_unpatched → gaps
4. Report: signal flow map, polyphony path, bypassed modules, dead ends, improvement advice

### "Analyze my mix / sound quality"
1. vcv_analyze_audio(mode='spectrum') → check frequency bands (sub/bass/mid/air), hum, standing resonances
2. vcv_reset_loudness → let the patch play for at least 3 seconds → vcv_analyze_audio(mode='loudness')
   to check momentary, short-term, and integrated LUFS, crest factor, L/R balance, and stereo phase correlation
3. Report: sonic balance, phase health, identified resonances, and gain staging recommendations

**Keep responses compact:** use the default spectrum summary. Request `include_spectrum=true` only
when choosing a precise corrective frequency. A feedback warning requires a rising narrow peak across
successive spectrum reads; repeat the spectrum check after a short musical passage when warranted.

### "I hear nothing"
1. vcv_get_signal_levels → first module with peak ≈ 0 in the audio chain
2. Trace backwards from AudioInterface: which upstream module is silent?
3. vcv_get_module(suspect) → Gate connected? Run on? Bypassed? VCA CV at 0?
4. GOTCHA: SEQ3 Trig out is a ~1ms TRIGGER — an ADSR with slow attack stays near-silent
5. Tell the user exactly what to check/turn

### "What should I improve?"
1. Audit (above) → 2. Check levels: healthy voices ±1.5–3.5V peak, >±4V hot, ±5V reaches Octavia full scale
3. Give a prioritized manual to-do list with exact module names, knob names, target values

### "Change / improve my patch"
1. Inspect target modules + cables → 2. State the intended compact edit → 3. Apply writes
4. Verify topology/values/levels → 5. Summarize changes and mention `vcv_undo` availability

---

## Level Reference

| Level | Voltage (peak) |
|---|---|
| Octavia full-scale overrange | > ±5 V |
| Hot | > ±4 V |
| Healthy voice | ±1.5 – ±3.5 V |
| Quiet / background | ±0.25 – ±1 V |
| Dead | < ±0.1 V |

**dB anchors:** ±5V = 0 dBFS · ±2.5V = -6 dBFS (voice target) · ±0.63V = -18 dBFS (0 dBVU)
**1V/oct:** C4 = 0V = 261.63 Hz · 1 semitone = 0.0833V · fifth = 0.5833V

**vcv_get_signal_levels fields:** peak = highest |V| in ~1s (headroom) · v = instantaneous ·
ch = poly channels · connected:false + peak>0 = stale peak.

---

## Tool Gotchas

- `vcv_list_library` requires `plugin=` or `q=` to prevent an oversized response.
- `vcv_layout_modules` coordinates are absolute: always anchor to the active patch cluster's existing row and HP base offset, never arbitrarily reset to (0, 0).
- Cache lag ~1s; all data is pull. Per-module CPU is not available (only whole-process).
- `vcv_get_module_state` returns the full preset JSON — useful to show the user a backup
  they can save before manual edits.
