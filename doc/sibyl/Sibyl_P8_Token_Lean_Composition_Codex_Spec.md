# Sibyl P8 — Token-Lean Composition Pipeline

**Status:** design proposal for review; refined with live empirical baselines, two-tier architecture, and Python composition toolkit specification.  
**Baseline inspected:** `4115c0d396f47fc3ab4e37719d47d6f8535559ab`, with P1–P6 implemented, P7D microtonal engine active, and live Rack baselines established.  
**Objective:** reduce total agent context and interaction cost for composing and revising music without reducing musical control, correctness, or inspectability.

---

## 1. Product Decision

Make the common musical operation concise, while preserving an exact event-level escape hatch. Optimize the complete loop: discover capabilities, understand the score, author, preview, commit, verify, listen, revise, and resume later.

Do not replace the score with an opaque generative prompt. Do not require a new punctuation-heavy music language. Add a small, typed authoring layer that expands into Sibyl's existing canonical composition and uses its validator, atomic transactions, stable IDs, adoption rules, and playback engine.

The agent should spend tokens deciding the music, not repeating JSON keys, reconstructing unchanged sections, calculating tuning voltages, or reading irrelevant state. Compact authoring is optional; every supported expressive field remains available through ordinary event objects and semantic edits.

---

## 2. Two-Tier Architecture and Division of Responsibilities

Sibyl's agent interface is structured as a two-tier system. Token-lean optimization must exploit the strengths of each tier rather than treating the bridge as a monolithic black box.

```
┌──────────────────────────────────────────────────────────┐
│      AI Coding Agent (Antigravity / Codex / Claude)      │
└────────────────────────────┬─────────────────────────────┘
                             │  MCP Protocol (JSON-RPC over stdio)
                             ▼
┌──────────────────────────────────────────────────────────┐
│  Tier 1: Python MCP Server & Composition Layer           │
│  - Model-facing tool schemas and parameter exposure      │
│  - Serialization formatting (compact separators, no-indent)│
│  - Static fact and module ID caching (zero discovery tax)│
│  - Python Composition Toolkit (`sibyl_composer.py`):     │
│    * Euclidean & rhythmic shorthand generators           │
│    * P7 microtonal interval & chord builder (38/53-EDO)  │
│    * Deterministic compilation to canonical operations   │
│  - Client-side response projection and filtering         │
└────────────────────────────┬─────────────────────────────┘
                             │  Local HTTP REST (`http://127.0.0.1:34570`)
                             ▼
┌──────────────────────────────────────────────────────────┐
│  Tier 2: VCV Rack Embedded C++ Bridge (`Octavia.cpp` /   │
│          `Sibyl*.cpp`)                                   │
│  - Real-time audio engine safety (zero hot-path work)    │
│  - Canonical validation, revision guards, undo units     │
│  - Native columnar expansion (`columns_v1`) & cloning    │
│  - Microtonal pitch systems (P7D) and voicing solver     │
│  - Physical signal monitoring and status reporting       │
└──────────────────────────────────────────────────────────┘
```

### 2.1 Division of Responsibilities Matrix

| Responsibility Area | Tier 1: Python MCP & Helper Tier (`Octavia_MCP.py` / `sibyl_composer.py`) | Tier 2: Embedded C++ (`Octavia.cpp` / `Sibyl*.cpp`) |
|---|---|---|
| **Model Context Serialization** | Strips `indent=2` whitespace tax; formats compact JSON (`separators=(',', ':')`) or tabular readbacks | Emits compact, unpadded JSON text on local HTTP socket |
| **Tool Schemas & Discovery** | Exposes consolidated/progressive schemas; suppresses 56-tool system-prompt flood | Serves machine-level capabilities and feature limits |
| **Rack Discovery Caching** | Caches module IDs, tuning catalogs, and static limits to eliminate large `/modules` scans | Answers targeted module and status queries |
| **Response Profiles** | Strips redundant empty collections, nulls, and unrequested fields before LLM ingestion | Emits bounded `receipt`, `summary`, or `full` payloads |
| **Musical Lifting & Shorthand** | Generates Euclidean patterns, P7 EDO chords, arpeggios, and automation envelopes | Executes canonical operations without needing procedural generative code |
| **Batch Expansion** | Validates rectangular array shapes and compiles shorthands | Executes canonical `insert_note_batch` expansion in control thread |
| **Score State & Audio** | Completely detached from audio thread; purely handles data synthesis & wire protocol | Owns canonical score, revision lock, undo stack, and real-time DSP |

---

## 3. What Already Exists, and Where to Improve

| Pipeline stage | Existing support | P8 opportunity |
|---|---|---|
| Discovery | Capability document lists features, operations, limits, and revision | Small feature manifest; fetch detailed contracts only when needed; cache module identity |
| Orientation | Composition summary, pattern/scene/progression views | Bounded arrangement map and dependency-aware object summaries |
| Reading notes | Selectors, field projection, revision-bound pagination | Preserve and promote these; strip indentation whitespace; compact tabular projection |
| Authoring | Full patterns, insert/update/duplicate/rotate notes, track defaults | Columnar event batches (`columns_v1`) and Python composition helpers |
| Arrangement | Shared patterns, scene repeats and assignment overrides | Safe pattern/scene cloning and partial pattern-property edits |
| Expression | Conditions, evolution, automation curves, relative harmony | Python envelope contours; reusable pattern condition templates |
| Harmony & P7 | `voice_progression` and P7D microtonal pitch systems | Python EDO chord builders (38/53-EDO); ratio exception helpers; voicing orchestration |
| Preview | VALIDATE accepts operations and can return changes | Optional prepared transaction avoids resending a large validated payload |
| Commit | Revision guard, atomic edit, bounded change reports | Concise receipts with drill-down details; avoid redundant full readback |
| Verification | Accepted/active/pending revisions; focused reads; physical monitoring | Separate structural verification from adoption and audible verification |
| Continuation | Persisted canonical composition and semantic views | Concise, reconstructible orientation after context loss |

---

## 4. Non-Negotiable Invariants

1. Existing APIs and full canonical reads remain available. New request/response formats require capability negotiation; existing clients retain their defaults.
2. Compression must not quantize pitch, timing, velocity, probability, modulation, or automation. Preserve omitted versus explicit values, exact authored ratios, pitch representation, and inheritance.
3. All compact inputs expand on the control side (in Python adapter/helper or C++ control thread). No new parsing, allocation, recursive recipe evaluation, or search in the audio thread.
4. Every mutation retains revision checking, atomic validation, collision checks, one undo unit, and the chosen adoption/phase policy. Compact requests do not bypass limits.
5. Stable IDs, conditions, seeded probability, and evolution remain unchanged for equivalent edits. Reuse must have explicit ID and randomness semantics.
6. Reports must distinguish authored, derived, accepted, pending, and actually sounding state. A successful edit is not proof of audible output.
7. No silent truncation. Every bounded list has a total and explicit continuation or omission information. Errors and material warnings cannot disappear in a compact response.
8. P7 native steps, scale degrees, cents, ratios, periods, and legacy semitones remain distinct. Neither 38-EDO nor 53-EDO requires the agent to compute voltages manually.
9. Persistent score schema integrity: P8 is primarily an API/authoring extension. Do not bump the persistent score schema solely to add a wire encoding or temporary preview handle.

---

## 5. Measured Baseline Ground Truth

Baselines were measured against a live VCV Rack testbed (`v2.12.0`, Sibyl module `2984289508820114`, P7D active) using `tools/measure_sibyl_p8_baselines.py`. See [`doc/Sibyl_P8_Baseline_Measurement_Report.md`](Sibyl_P8_Baseline_Measurement_Report.md) for full traces.

### 5.1 Empirical Baseline Metrics

| Stage / Scenario | Calls | Outbound (Bytes) | Inbound (Bytes) | Est. In Tokens | Cumulative Latency | Key Finding |
|---|:---:|:---:|:---:|:---:|:---:|---|
| **Discovery & Handshake** | 2 | 0 | 56,045 | ~14,011 | 50.7 ms | `GET /modules` dumped 55.6KB just to locate Sibyl |
| **Scenario 1: 8-Track Multi-Section Comp** | 6 | 5,781 | 5,730 | ~1,432 | 216.9 ms | Key boilerplate (`step`, `gate`, etc.) accounted for >50% of payload |
| **Scenario 2: Phrase Revision & Conflict** | 6 | 567 | 2,902 | ~726 | 209.4 ms | Rejection was compact (104B); recovery status/read was 2.9KB |
| **Scenario 3: Verse to Chorus Transformation** | 4 | 2,515 | 7,648 | ~1,912 | 165.0 ms | Inbound scene verification read alone consumed 6,452B |
| **Scenario 4: P7 38-EDO / 53-EDO Microtonality** | 7 | 1,807 | 4,934 | ~1,234 | 249.6 ms | Native retuning report was 1,325B; tuning math held exact |
| **Scenario 5: Context Loss Resume & Edit** | 6 | 203 | 6,368 | ~1,592 | 249.3 ms | Re-orientation consumed 5,705B before a 203B edit |
| **Scenario 6: Sounding Mismatch Diagnosis** | 3 | 0 | 11,649 | ~2,912 | 83.7 ms | Effective context (7.1KB) + voltages (3.8KB) dominated |
| **Scenario 7: Irregular Expressive Passage** | 4 | 1,523 | 14,544 | ~3,636 | 135.3 ms | Full note readback (`fields: ["full"]`) was 13.5KB |
| **TOTAL** | **38** | **12,396** | **109,820** | **~27,455** | **1,357.7 ms** | **Inbound context is >8.8x larger than outbound authoring** |

*Canonical score size in memory after full workflow:* **25,825 bytes**.

---

## 6. P8A — Smaller Reads, Receipts, Serialization, and Discovery

### 6.1 Progressive Discovery and Schema Compression
- **Problem:** Currently, the agent receives schemas for 56 distinct MCP tools on every prompt turn, and re-queries full capabilities (2,722B) on reconnect.
- **Specification:**
  - Introduce a lightweight manifest entry point (`vcv_sibyl_manifest`) returning only API version, schema version, feature flags, and contract fingerprints (<300 bytes).
  - Detailed parameter schemas for specialized subsystems (`pitch_systems`, `voicing`, `automation`, `conditions`) are fetched by topic on demand.
  - Python MCP adapter caches contracts by server instance and contract fingerprint. Reconnect or fingerprint change invalidates cache.

### 6.2 Targeted Module Lookup (Zero Discovery Tax)
- **Problem:** In the baseline, `GET /modules` cost **55,661 bytes** simply to find Sibyl.
- **Specification:**
  - Python MCP adapter caches the resolved Sibyl module ID across turns for the lifetime of the session.
  - Expose a direct targeted endpoint `/sibyl/resolve` or allow tool calls to omit `module_id` when exactly one Sibyl module exists in the patch, resolving it internally in Python without roundtripping whole-patch inventories to the model.

### 6.3 Whitespace and Serialization Standard
- **Problem:** Python `json.dumps(..., indent=2)` consumes 30% to 45% of inbound tokens in indentation spaces and newlines.
- **Specification:**
  - Mandate unindented JSON serialization (`separators=(',', ':')`) for all model-visible responses by default.
  - For high-density array reads (`view="notes"`), offer an optional compact columnar or tabular projection format.

### 6.4 Arrangement and Object Summaries
- **Specification:**
  - Provide a bounded arrangement view containing scene order/duration/repeats, track-to-pattern assignments, overrides, and referenced automation/pitch context IDs.
  - Exclude note arrays by default. Include pattern note counts, resolutions, and reference counts.
  - Summary views provide an orientation packet for context recovery without re-reading hundreds of individual event objects.

### 6.5 Response Profiles (`receipt` | `summary` | `full`)
- **Specification:**
  - Negotiate `response_profile: "receipt" | "summary" | "full"`.
  - `receipt`: Returns status (`ok: true`), accepted/active revisions, apply boundary, operation counts, and affected object IDs (<150 bytes).
  - Detailed before/after values and event-ID mappings are opt-in.
  - Edit responses prove structural acceptance directly; no follow-up read is required merely to confirm that accepted changes were stored.

---

## 7. P8B — Compact Exact Authoring

### 7.1 Columnar Event Batches (`columns_v1`)

Add a versioned columnar batch encoding for note insertion and pattern authoring:

```json
{
  "op": "insert_note_batch",
  "pattern_id": "lead",
  "encoding": "columns_v1",
  "columns": ["step", "note", "velocity", "gate"],
  "rows": [
    [0, "C3", 0.9, 1.0],
    [3, "Eb3", 0.75, 0.8],
    [6, "G3", 0.8, 0.8],
    [10, "Bb3", 0.85, 1.2]
  ],
  "defaults": {"gate": 0.8, "velocity": 0.8},
  "expect_count": 4
}
```

- **Allowlist & Typing:** Column names come from a versioned allowlist (`step`, `note`, `gate`, `velocity`, `probability`, `ratchets`, `glideMs`, `mod`, `mod2`, `mod3`, `tuned.step`, `tuned.degree`, `tuned.ratio`, `harmonic`).
- **Expansion Mechanics:**
  - Expansion order: batch defaults $\rightarrow$ row values $\rightarrow$ sparse overrides.
  - Native C++ engine implements `insert_note_batch` atomically in the control thread.
  - Python MCP adapter validates rectangular array dimensions and provides optional client-side expansion fallback for legacy Rack builds.
- **Escape Hatch:** Any unsupported field or irregular event falls back to canonical event objects without penalty.

### 7.2 Typed Rhythmic Repetition
- Support an optional bounded arithmetic onset series (`start`, `spacing`, `count`) paired with a cyclic expression array.
- Expands to ordinary events at edit time on the control side.
- No dynamic pattern generators in the audio thread.

---

## 8. P8C — Safe Phrase and Section Reuse

Exploit existing structural primitives before authoring repetitive event lists:

- **Partial Pattern Property Edits:** Update length, resolution, evolution, or pitch context without retransmitting notes. Shrinking a pattern never silently deletes out-of-bounds notes.
- **Pattern Cloning:** Clone a pattern into a new destination with explicit source revision and optional transform operations (e.g., transpose, rotate).
- **Scene Cloning (`share` vs. `copy`):**
  - `patterns: "share"`: Reuses pattern IDs across scenes with scene assignment overrides for variation.
  - `patterns: "copy"`: Clones underlying patterns with a documented destination ID mapping.
- **Materialization Policy:** All reuse operations materialize into self-contained canonical score state. No complex, fragile runtime motif dependency graphs.

---

## 9. P8D — Python Composition Toolkit and P7 Integration

To maximize agent efficiency beyond the wire protocol, P8 introduces a dedicated Python composition library (`tools/sibyl_composer.py`). This toolkit is completely decoupled from the FastMCP server infrastructure, allowing it to be used either through direct agent scratch scripting or via high-level macro tools.

### 9.1 Design Philosophy & Two Consumption Modes
1. **Agent Scratch Scripting Mode:**
   - Instead of emitting thousands of JSON characters across multiple chat turns, the model writes a concise 5-to-10 line Python script utilizing `sibyl_composer.py`.
   - The script runs locally in milliseconds, compiles high-level musical intent into canonical atomic edit operations, and submits them directly to Rack.
   - Prompt context drops from ~6,000 characters of raw JSON to ~250 characters of expressive Python code.
2. **MCP Macro Mode:**
   - The MCP server exposes concise macro endpoints (e.g., `vcv_sibyl_compose_progression`, `vcv_sibyl_compose_pattern`) that delegate to `sibyl_composer.py` internally.

### 9.2 Generative & Musical Shorthands
- **Euclidean Rhythm Generator:**
  - `PatternBuilder.euclidean(pulses, steps, fill_note, ...)`: Deterministically distributes rhythmic pulses across a pattern grid using Bjorklund's algorithm, with support for rotation, velocities, and ratchet accents.
- **Cyclic Chord Tone / Arpeggio Generator:**
  - Generates arpeggiator patterns over active harmonic degrees or tone roles (`[root, third, fifth, octave]`) with user-specified contours (up, down, ping-pong, random walk).
- **Automation Contours:**
  - `AutomationBuilder.envelope(start_beat, end_beat, start_v, peak_v, end_v, shape="smoothstep")`: Generates smooth parametric curves for filter sweeps, resonance, and modulation depth.
- **Arrangement Scaffolding:**
  - Assembles multi-track scene progressions (e.g., Intro $\rightarrow$ Verse $\rightarrow$ Chorus $\rightarrow$ Outro) with automated track-to-pattern assignment mappings and scene-level register/velocity overrides.

### 9.3 Deep Integration with P7 Microtonal Pitch Systems
The toolkit lifts the computational and theoretical burden of microtonality off the agent while compiling into exact P7 structures:

- **$N$-EDO Interval & Step Derivation:**
  - Translates acoustic musical intervals (unisons, fifths, major/minor thirds, subminor thirds, harmonic sevenths) into exact step indices for arbitrary equal temperaments (e.g., 38-EDO, 53-EDO, 19-EDO, 31-EDO) and non-octave periods.
  - *Example:* For 53-EDO, automatically maps a septimal dominant chord to `{root: 0, third: 17, fifth: 31, seventh: 43}` plus exact ratio exceptions (`ratio: "7/4"`).
- **P7 Native Progression Builder:**
  - Assembles valid P7 `upsert_progression` structures complete with lattice `rootPitch`, typed tone roles (`root`, `third`, `fifth`, `seventh`, `extension`), and step/cents/ratio intervals.
- **P7 Native Voicing Orchestration:**
  - Interfaces directly with Sibyl's C++ `voice_progression` DP solver (`voice_progression_micro_v1`), configuring target register extrema (`min`/`max` voltage intervals) and voice spread across assigned tracks.

### 9.4 Deterministic Compilation Guarantee
- The toolkit contains zero stochastic hallucination: every helper method deterministically compiles into 100% valid, atomic canonical Sibyl operations (`upsert_pattern`, `insert_note_batch`, `upsert_progression`, `upsert_scene`, `update_scene_assignment`).
- Full unit-test coverage ensures that the generated operations adhere strictly to Sibyl schema version 4 and pass all C++ validator rules.

---

## 10. P8E — Prepared Transactions (Two-Phase Commit)

For large or solver-heavy edits:

1. **Prepare:** Agent sends operations with `expected_revision`. Server validates candidate and returns a bounded preview + opaque handle + expiry TTL.
2. **Commit:** Agent sends handle + `expected_revision`. Server atomically commits the prepared candidate as one undo unit.
3. **Idempotence & Recovery:** Retrying an already-committed handle returns the original receipt while retained. Stale revisions or expired handles reject cleanly without side effects.

---

## 11. Whole-Pipeline Operating Policy

Recommended agent operating loop under P8:

1. **Discover & Orient:** Fetch compact manifest; verify cached module ID. Read arrangement summary and runtime status.
2. **Reuse First:** Prefer shared patterns, scene overrides, conditions, or evolution before spelling out repeated note arrays.
3. **Shorthand & Composition Lifting:** Use `sibyl_composer.py` or `insert_note_batch` for complex phrases, Euclidean rhythms, and P7 microtonal chord progressions.
4. **Atomic Commit:** Submit single transaction with `response_profile: "receipt"`.
5. **Verify Acceptance:** Use receipt for structural confirmation; inspect focused projections or physical signals only when diagnosing mismatches.
6. **Continuation:** On context loss, reconstruct state from the arrangement summary and runtime status without dumping full score JSON.

---

## 12. Validation Gates and Target Baselines

| Milestone | Target Area | Measured Baseline | P8 Acceptance Gate |
|---|---|---|---|
| **P8A** | Module Discovery Tax | 55,661 bytes | $\le 150$ bytes (via targeted lookup / caching) |
| **P8A** | Context Loss Orientation (Sc. 5) | 5,705 bytes inbound | $\le 1,600$ bytes (>70% context reduction) |
| **P8A** | Serialization Whitespace | Indented (`indent=2`) | Unindented (`separators=(',', ':')`) |
| **P8A** | Verification Receipts (Sc. 2) | 2,902 bytes inbound | $\le 500$ bytes |
| **P8B** | Initial Authoring (Sc. 1) | 5,781 bytes outbound | $\le 2,800$ bytes (>50% payload reduction) |
| **P8C** | Section Variation (Sc. 3) | 10,163 bytes total (out+in) | $\le 5,000$ bytes (>50% context reduction) |
| **P8D** | Python Composition Lifting | Manual JSON authoring | Shorthand script $\le 300$ bytes; 100% P7 compilation |
| **All** | Audio Thread & DSP Integrity | Zero audio-thread overhead | Invariant: zero allocations or parsing in audio thread |
| **All** | P7 Microtonal Fidelity | Exact ratios & native steps | Invariant: 38-EDO/53-EDO precision completely preserved |

---

## 13. Phased Delivery Order

1. **P8A:** Compact serialization (strip `indent=2`), module lookup caching, compact manifest discovery, arrangement summary view, `receipt` response profile.
2. **P8B:** Columnar event batches (`insert_note_batch` with `columns_v1`), row defaults, arithmetic onset expansion.
3. **P8C:** Partial pattern property updates, pattern cloning, scene cloning (`share` vs `copy`), and composite transform pipelines.
4. **P8D:** Python Composition Toolkit (`tools/sibyl_composer.py`), Euclidean generators, P7 microtonal chord & progression builders, and deterministic compilation test harness.
5. **P8E:** Prepared transactions (`prepare` $\rightarrow$ handle $\rightarrow$ `commit`), handle TTL, and idempotent retry recovery.
