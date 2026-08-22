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

HTTP server on `localhost:34570`. Plugin: **Leviathan** | Module: **Octavia** | Start button → full-brightness Octopus.

Patch-design reference (read when selecting modules, troubleshooting, or optimizing):
`references/tables.md` — installed-library selection criteria and evidence-based diagnostics.

Sibyl reference (read whenever composing, sequencing, arranging, or controlling Sibyl):
`references/sibyl.md` — AI-first routing policy, standard workflow, state semantics,
operation guidance, and common traps.

## Sequencer Selection: Prefer Sibyl for AI Composition

**Sibyl is Octavia's AI-first sequencer and arranger.** When the user asks the agent to
compose, generate, arrange, orchestrate, vary, or revise musical sequences, prefer
**Leviathan:Sibyl** as the primary sequencing surface. Its semantic composition API is
designed for reliable agent authorship: named tracks, patterns, scenes, atomic edits,
revision guards, validation, and musically quantized adoption.

Choose a conventional visible step sequencer instead when the user explicitly wants to
manually edit steps, turn knobs, manipulate the sequence on the Rack panel, or learn from
a visible grid. Do not replace an existing user-chosen sequencer merely because Sibyl is
available. In mixed workflows, Sibyl may remain the composition authority while exposed
CV controls, macros, or a user-editable sequencer provide hands-on performance control.

Routing examples:

- "Compose a bass line," "write a song," or "make three evolving sections" → use Sibyl.
- "Revise the chorus melody" or "add a variation next scene" → use Sibyl semantic edits.
- "Give me a sequencer whose steps I can edit by hand" → choose a panel-editable sequencer.
- "Modify this sequence I already made in SEQ3" → preserve and edit the user's SEQ3 setup.
- If intent is ambiguous and the agent is expected to author the music, default to Sibyl.

## ALWAYS START HERE

```
vcv_get_status   →  {"running": true, "port": 34570, "patch": {"path": "...", "hasSavePath": true}}
```
If it fails → tell user to check Octavia + press START. Do not retry automatically.

## Octavia Console Mode (Explicit Opt-In)

Enter Console Mode only when the user explicitly asks to arm or listen to the in-Rack
Octavia Console, using language such as **“Arm the Octavia Console,” “Listen to Rack,”**
or **“Start Console Mode.”** Do not enter this waiting loop merely because an Octavia
Console module exists; users may prefer their native agent interface.

On activation:

1. Call `vcv_get_status`, then `vcv_list_modules` and locate `Leviathan:OctaviaConsole`.
   If none exists, tell the user to place the Console immediately to Octavia's right. If
   several exist and context does not identify one, ask which module ID to use.
2. Call `vcv_octavia_console_status`. Begin with `after_prompt_id` equal to its
   `latestResponsePromptId`, so an already queued prompt is delivered without replaying a
   completed prompt.
3. Call `vcv_octavia_console_wait` with a bounded wait (normally 20 seconds). A null prompt
   is an ordinary timeout: wait again while Console Mode remains active.
4. Treat returned Console text as a new user request. Apply the normal Octavia inspection,
   authorization, editing, and verification rules. The Console does not broaden permission
   for destructive actions or saving.
5. Always finish that Console request with `vcv_octavia_console_respond`, using the exact
   returned prompt ID. Send the useful final result to Rack; on a handled failure, send a
   concise error with `error=true`. If clarification or approval is required, respond with
   that request rather than guessing.
6. Advance `after_prompt_id` to the handled prompt ID and wait again.

Continuous waiting occupies an active agent turn and is subject to the client's turn and
tool-call limits. Do not promise indefinite background listening.

Leave Console Mode when the user explicitly cancels it, a required tool disappears, the
Octavia connection fails, or the client ends the active agent turn. Never claim the Console
remains armed after the turn has ended. If the Console tools are missing, explain that the
updated Octavia MCP server must be installed and the agent session restarted.

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

Common generic write tools: `vcv_add_module`, `vcv_set_parameters`, `vcv_connect_cables`,
`vcv_disconnect_cable`, `vcv_update_module`, `vcv_layout_modules`, `vcv_set_module_state`,
`vcv_undo`, `vcv_delete_module`, `vcv_save_patch`, and `vcv_reset_loudness`.
Use Sibyl, Temporal Deck, and Console semantic tools for those specialized workflows.
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

Classify likely module roles from the model name; reason about signal flow from
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

Use summary fields as investigation signals, not conclusions. `polyOut=0` can mean idle or
unpatched; `polyOut>1` can originate upstream; bypass routing is module-dependent; and an
unconnected port or processor may be intentional. Inspect only the modules needed to resolve
the user's question.

---

## Efficient Starting Calls

| Task | Typical start |
|---|---|
| Understand unknown patch | `vcv_list_modules` → `vcv_list_cables` |
| Full patch audit | Add `vcv_find_unpatched`; treat results as candidates, not defects |
| Spectral / mix audit | Spectrum → reset loudness → measure a representative passage → loudness |
| Debug silence | `vcv_get_signal_levels` → inspect the first relevant silent module |
| Read one module in depth | `vcv_get_module(id)` |
| Module recommendation | Filter `vcv_list_library`, then compare only installed candidates |

---

## Decision Trees

### "Analyze my patch"
1. vcv_list_modules → classify likely roles · 2. vcv_list_cables → trace chains · 3. vcv_find_unpatched → candidates
4. Inspect ambiguous modules as needed; distinguish confirmed problems from intentional gaps

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
- Ordinary patch caches can lag by about 1 second. Console prompt waiting uses bounded
  long-polling instead. Per-module CPU is not available; `vcv_get_perf` is process-wide.
- `vcv_get_module_state` returns the full preset JSON — useful to show the user a backup
  they can save before manual edits.
