# Sibyl P8 — Baseline Measurement Report

**Date:** 2026-09-19  
**Target:** Leviathan Sibyl Module (`2984289508820114`) on VCV Rack `v2.12.0` via Octavia bridge (`port 34570`)  
**Baseline Commit/State:** Live testbed rack with P1–P6 implemented and P7D active (`voice_progression_micro_v1`, 38-EDO/53-EDO, relative harmony, automation lanes).  
**Harness:** `tools/measure_sibyl_p8_baselines.py` (telemetry captured in `test-results/p8-baseline/`).

---

## 1. Executive Summary

In accordance with **Section 4 of `doc/Sibyl_P8_Token_Lean_Composition_Codex_Spec.md`**, all 7 required scenarios were executed against the live testbed Rack using the existing semantic APIs (P1–P7) to establish pre-P8 ground truth.

- **Total API Transactions:** 38 calls across the 7 scenarios + discovery.
- **Total Inbound Context Consumed:** 109,820 characters (109,820 UTF-8 bytes, ~27,455 tokens).
- **Total Outbound Payload Transmitted:** 12,396 characters (12,396 UTF-8 bytes, ~3,099 tokens).
- **Total Roundtrip Latency:** 1,357.7 ms cumulative across all 38 network/control transactions.
- **Final Resulting Score Size:** 25,825 characters (25,825 UTF-8 bytes) for the compiled canonical composition document (`final_composition.json`).

---

## 2. Quantitative Scenario Breakdown

| Scenario | Calls | Outbound (Chars/Bytes) | Inbound (Chars/Bytes) | Est. Out Tokens | Est. In Tokens | Cumulative Latency (ms) |
|---|:---:|:---:|:---:|:---:|:---:|:---:|
| **Discovery & Handshake** | 2 | 0 | 56,045 | 0 | 14,011 | 50.7 |
| **Scenario 1: 8-Track Multi-Section Composition** | 6 | 5,781 | 5,730 | 1,445 | 1,432 | 216.9 |
| **Scenario 2: Phrase Revision & Conflict Recovery** | 6 | 567 | 2,902 | 142 | 726 | 209.4 |
| **Scenario 3: Verse to Chorus Transformation** | 4 | 2,515 | 7,648 | 629 | 1,912 | 165.0 |
| **Scenario 4: P7 38-EDO / 53-EDO Microtonality** | 7 | 1,807 | 4,934 | 452 | 1,234 | 249.6 |
| **Scenario 5: Context Loss Resume & Focused Edit** | 6 | 203 | 6,368 | 51 | 1,592 | 249.3 |
| **Scenario 6: Sounding Mismatch Diagnosis** | 3 | 0 | 11,649 | 0 | 2,912 | 83.7 |
| **Scenario 7: Irregular Expressive Passage** | 4 | 1,523 | 14,544 | 381 | 3,636 | 135.3 |
| **TOTAL** | **38** | **12,396** | **109,820** | **3,099** | **27,455** | **1,357.7 ms** |

*Note: Est. tokens calculated via standard 4 chars/token heuristic. Actual bytes and characters are authoritative.*

---

## 3. Detailed Scenario Observations & P8 Opportunities

### Discovery & Handshake
- **Observation:** `GET /modules` returned **55,661 bytes** just to locate the Sibyl module ID among loaded modules.
- **P8A Opportunity:** Bounded module discovery / direct capability manifest with contract fingerprinting avoids large rack sweeps.

### Scenario 1: Initial Multi-Track Composition
- **Observation:** Outbound composition payload was **5,781 chars** to author 8 tracks, relative harmony, 5 patterns, automation curve, and 4 scenes. The key repeating overhead stems from repeating step JSON keys (`"step"`, `"gate"`, `"velocity"`, `"note"`) across 40+ steps.
- **P8B Opportunity:** With columnar event batches (`columns_v1`), `[step, note, gate, velocity]` tabular rows with batch defaults will cut outbound payload by >50%.

### Scenario 2: Phrase Revision & Conflict Recovery
- **Observation:** Stale revision rejection was crisp (HTTP 200 with `ok: false, error: "revision_mismatch"`), requiring only 104 chars inbound. Re-syncing and targeted updating cost 567 chars outbound and 2,902 chars inbound across status and report reads.
- **P8A/P8B Opportunity:** A concise `response_profile: "receipt"` with targeted before/after diffs will drop verification inbound overhead.

### Scenario 3: Verse to Chorus Transformation
- **Observation:** Transforming verse to chorus required **2,515 chars** outbound to author new dense hat and snare fill patterns and update scene assignments. Inbound scene verification (`fields: ["effectiveExpressions"]`) was **6,452 chars**!
- **P8C Opportunity:** Pattern cloning with arithmetic repetition or transform filters, plus bounded scene summary views, will eliminate authoring duplicate patterns from scratch.

### Scenario 4: P7 Microtonal Composition & Retuning
- **Observation:** Setting up 38-EDO tuning, scale, context, native progression, and pattern cost 1,367 chars outbound. Retuning to 53-EDO via `retune_notes` took 440 chars outbound and returned a 1,325-character report.
- **P8 Invariant Check:** P7 microtonality executed seamlessly on the live engine without any loss of tuning precision or step identity.

### Scenario 5: Context Loss Resume & Focused Edit
- **Observation:** To re-orient after memory loss, the agent consumed **5,705 chars** across `capabilities` (2,722 chars), `summary` (1,775 chars), and `status` (585 chars), plus note paging (623 chars), just to execute a single 203-character note edit.
- **P8A Opportunity:** Progressive discovery with cached contract fingerprints and bounded arrangement summaries will shrink orientation payload from ~5.7k chars to <1.5k chars (>70% reduction).

### Scenario 6: Sounding Mismatch Diagnosis
- **Observation:** Querying `effective_context` for a scene/beat returned **7,174 chars**, and `modules/voltages` returned **3,881 chars**. Total inbound context was 11,649 chars.
- **P8A Opportunity:** Bounded effective context and targeted monitor queries.

### Scenario 7: Irregular Expressive Passage
- **Observation:** Authoring 16 bespoke, non-repetitive micro-timed notes took 1,523 chars. Reading back full note definitions (`fields: ["full"]`) was **13,557 chars** inbound.
- **P8 Invariant Check:** P8 must preserve full unconstrained expressivity without loss of precision when compact repetition is not applicable.

---

## 4. Acceptance Target Baseline Comparison

| Metric Target | Spec Target | Measured Baseline | Target Under P8 |
|---|---|---|---|
| Repetitive Initial Authoring (Sc. 1) | $\ge 50\%$ payload reduction | 5,781 chars outbound | $\le 2,890$ chars |
| Multi-Section & Chorus (Sc. 3) | $\ge 30\%$ total context reduction | 10,163 chars total (out+in) | $\le 7,114$ chars |
| Context Loss Resume & Edit (Sc. 5) | $\ge 30\%$ total context reduction | 6,571 chars total (out+in) | $\le 4,600$ chars |
| Sounding Mismatch Diagnosis (Sc. 6) | Efficient targeted diagnostics | 11,649 chars inbound | Bounded monitor/status summary |
