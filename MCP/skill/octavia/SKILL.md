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

HTTP server on `localhost:7777`. Plugin: **Leviathan** | Module: **Octavia** | Start button → green orb.

Deep reference (read on demand): `references/tables.md` — module database by category,
quick-pick by task, troubleshooting matrix, CPU optimization.

## ALWAYS START HERE

```
vcv_get_status   →  {"running": true, "port": 7777}
```
If it fails → tell user to check Octavia + press START. Do not retry automatically.

## Editing Workflow

When the user asks to change the patch, the LLM may use the write tools directly.

1. Inspect the exact target immediately before editing: use `vcv_get_module` for parameters
   and ports, `vcv_list_cables` for routing, or filtered `vcv_list_library` before adding.
2. Translate the user's musical intent into concrete module IDs, parameter IDs/values,
   and port IDs/names. Never guess an ID, parameter range, or plugin/model slug.
3. Make the smallest coherent change. Prefer `vcv_set_parameters` when several parameter
   changes form one edit because it creates one vcv_undo step.
4. Check every write response for `error`, `failedIndices`, or partial bulk application.
5. Verify with the cheapest relevant read: `vcv_get_parameter`, `vcv_list_cables`,
   `vcv_list_modules`, or `vcv_get_signal_levels`. Report exactly what changed.

Write tools: `vcv_add_module`, `vcv_set_parameter`, `vcv_set_parameters`, `vcv_connect_cable`,
`vcv_connect_cables`, `vcv_disconnect_cable`, `vcv_disconnect_output`, `vcv_move_module`, `vcv_set_bypass`,
`vcv_set_module_state`, `vcv_undo`, `vcv_delete_module`, and `vcv_save_patch`.

### Safety and approval

- Normal reversible edits (parameters, cables, bypass, movement, module addition, state
  restoration) may proceed when the user clearly asked to edit or improve the patch.
- Before an exact state restoration, read and retain the current state until verification.
- `vcv_delete_module` is not undoable through Octavia. Confirm the module details and obtain
  explicit user approval naming the module before deleting it.
- `vcv_save_patch` overwrites the patch's existing file and is not undoable through Octavia.
  Check `vcv_get_patch_info` and obtain explicit user approval before saving.
- Do not silently broaden an edit. If the request is exploratory ("what could improve?"),
  advise first; only mutate when the user asks to apply or make changes.
- If verification fails, stop further writes, explain the partial state, and offer/use
  `vcv_undo` for reversible operations. A bulk cable call can partially apply and reports how
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
| Full patch audit | **3** | vcv_list_modules → vcv_list_cables → find_unpatched |
| Debug silence | **2** | vcv_get_signal_levels → vcv_get_module(suspect_id) |
| Read one module in depth | **1** | vcv_get_module(id) |
| Module recommendation | **1–2** | vcv_list_library(q=...) — ALWAYS filtered |

---

## Decision Trees

### "Analyze my patch"
1. vcv_list_modules → classify roles · 2. vcv_list_cables → trace chains · 3. find_unpatched → gaps
4. Report: signal flow map, polyphony path, bypassed modules, dead ends, improvement advice

### "I hear nothing"
1. vcv_get_signal_levels → first module with peak ≈ 0 in the audio chain
2. Trace backwards from AudioInterface: which upstream module is silent?
3. vcv_get_module(suspect) → Gate connected? Run on? Bypassed? VCA CV at 0?
4. GOTCHA: SEQ3 Trig out is a ~1ms TRIGGER — an ADSR with slow attack stays near-silent
5. Tell the user exactly what to check/turn

### "What should I improve?"
1. Audit (above) → 2. Check levels: healthy voices ±3–7V peak, >±8V too hot, ±10V clips
3. Give a prioritized manual to-do list with exact module names, knob names, target values

### "Change / improve my patch"
1. Inspect target modules + cables → 2. State the intended compact edit → 3. Apply writes
4. Verify topology/values/levels → 5. Summarize changes and mention `vcv_undo` availability

---

## Level Reference

| Level | Voltage (peak) |
|---|---|
| Clipping | > ±10 V |
| Hot | > ±8 V |
| Healthy voice | ±3 – ±7 V |
| Quiet / background | ±0.5 – ±2 V |
| Dead | < ±0.1 V |

**dB anchors:** ±10V = 0 dBFS · ±5V = -6 dBFS (voice target) · ±1.26V = -18 dBFS (0 dBVU)
**1V/oct:** C4 = 0V = 261.63 Hz · 1 semitone = 0.0833V · fifth = 0.5833V

**vcv_get_signal_levels fields:** peak = highest |V| in ~1s (headroom) · v = instantaneous ·
ch = poly channels · connected:false + peak>0 = stale peak.

---

## Tool Gotchas

- `vcv_list_library` without `plugin=` or `q=` returns 200k+ chars — **always filter**.
- Cache lag ~1s; all data is pull. Per-module CPU is not available (only whole-process).
- `vcv_get_module_state` returns the full preset JSON — useful to show the user a backup
  they can save before manual edits.
