# Sibyl P8 — Token-Lean Composition Pipeline

**Status:** design proposal for review; not implemented functionality.  
**Baseline inspected:** `4115c0d396f47fc3ab4e37719d47d6f8535559ab`, with P1–P6 implemented and P7 specified separately.  
**Objective:** reduce total agent context and interaction cost for composing and revising music without reducing musical control, correctness, or inspectability.

## 1. Product decision

Make the common musical operation concise, while preserving an exact event-level escape hatch. Optimize the complete loop: discover capabilities, understand the score, author, preview, commit, verify, listen, revise, and resume later.

Do not replace the score with an opaque generative prompt. Do not require a new punctuation-heavy music language. Add a small, typed authoring layer that expands into Sibyl's existing canonical composition and uses its validator, atomic transactions, stable IDs, adoption rules, and playback engine.

The agent should spend tokens deciding the music, not repeating JSON keys, reconstructing unchanged sections, calculating tuning voltages, or reading irrelevant state. Compact authoring is optional; every supported expressive field remains available through ordinary event objects and semantic edits.

## 2. What already exists, and where to improve

These findings are from local source and documentation, not measured token benchmarks.

| Pipeline stage | Existing support | P8 opportunity |
|---|---|---|
| Discovery | Capability document lists features, operations, limits, and revision | Small feature manifest; fetch detailed contracts only when needed |
| Orientation | Composition summary, pattern/scene/progression views | Bounded arrangement map and dependency-aware object summaries |
| Reading notes | Selectors, field projection, revision-bound pagination | Preserve and promote these; do not invent a second selector language |
| Authoring | Full patterns, insert/update/duplicate/rotate notes, track defaults | Compact event batches and edit-time phrase reuse |
| Arrangement | Shared patterns, scene repeats and assignment overrides | Safe pattern/scene cloning and partial pattern-property edits |
| Expression | Conditions, evolution, automation curves, relative harmony | Teach reuse of these features instead of spelling out each repetition |
| Harmony | `voice_progression` materializes fixed notes | Reuse it; extend to P7 contexts rather than generating note lists in the agent |
| Preview | VALIDATE accepts operations and can return changes | Optional prepared transaction avoids resending a large validated payload |
| Commit | Revision guard, atomic edit, bounded change reports | Concise receipts with drill-down details; avoid redundant full readback |
| Verification | Accepted/active/pending revisions; focused reads; physical monitoring | Separate structural verification from adoption and audible verification |
| Continuation | Persisted canonical composition and semantic views | Concise, reconstructible orientation after context loss |

Current source pointers: `MCP/skill/octavia/references/sibyl.md`, `MCP/mcp_server/Octavia_MCP.py`, `src/Sibyl.cpp`, `src/SibylEdit.cpp`, `src/SibylNoteEdit.cpp`, `src/SibylAssignmentEdit.hpp`, `src/SibylVoicingEdit.hpp`, and `src/SibylJSON.cpp`.

The present MCP wrappers return JSON text. Measure the actual model-visible serialization before changing the wrapper: escaped envelopes or duplicate structured/text renderings can erase application-level savings. Do not assume either duplication or savings without a captured trace.

## 3. Non-negotiable invariants

1. Existing APIs and full canonical reads remain available. New request/response formats require capability negotiation; existing clients retain their defaults.
2. Compression must not quantize pitch, timing, velocity, probability, modulation, or automation. Preserve omitted versus explicit values, exact authored ratios, pitch representation, and inheritance.
3. All compact inputs expand on the control side. No new parsing, allocation, recursive recipe evaluation, or search in the audio thread.
4. Every mutation retains revision checking, atomic validation, collision checks, one undo unit, and the chosen adoption/phase policy. Compact requests do not bypass limits.
5. Stable IDs, conditions, seeded probability, and evolution remain unchanged for equivalent edits. Reuse must have explicit ID and randomness semantics.
6. Reports must distinguish authored, derived, accepted, pending, and actually sounding state. A successful edit is not proof of audible output.
7. No silent truncation. Every bounded list has a total and explicit continuation or omission information. Errors and material warnings cannot disappear in a compact response.
8. P7 native steps, scale degrees, cents, ratios, periods, and legacy semitones remain distinct. Neither 38-EDO nor 53-EDO requires the agent to compute voltages.

P8 is primarily an API/authoring extension. Do not bump the persistent score schema solely to add a wire encoding or temporary preview handle. If implementation introduces persistent fields, version those deliberately and document migration separately.

## 4. Measure the entire task first

Create replayable baseline workflows using the best existing semantic APIs, not intentionally verbose whole-score replacements. Compare equivalent initial/final scores, expressive behavior, and verification confidence.

Required scenarios:

- Compose an eight-track, multi-section piece using reused patterns, independent automation, probability/evolution, and relative harmony.
- Change the final four notes of a phrase, including a conflict/recovery case.
- Build a chorus from a verse with a changed bass register, denser percussion, and a final-repeat fill.
- Create and revise both 38-EDO and 53-EDO material through P7, including exact-ratio exceptions and native voicing.
- Resume an unfamiliar existing composition after context loss and make one focused edit.
- Diagnose a sounding mismatch without fetching every note or dumping raw audio/monitor data.
- Author an irregular, highly expressive passage with little repetition; compact mode must not constrain it.

Record separately: tool schema/reference context, outbound arguments, inbound results, exposed intermediate text, call count, retries, latency, and resulting score size. If provider usage includes cached input or reasoning, report those separately; do not infer hidden usage from JSON length. Record tokenizer and version when available. Bytes/characters are useful fallback measurements but must not be labeled tokens.

Provisional acceptance targets, to revisit once a baseline exists: at least 50% less tool payload token volume for repetitive initial authoring, and at least 30% less total model-visible context for the multi-section and resume/edit workflows. These are targets, not measured claims. Report each scenario and regressions; an average must not hide a more expensive common task. Correctness and expressive equivalence are release gates even if a percentage target needs revision.

## 5. P8A — Smaller reads, receipts, and documentation

### 5.1 Progressive discovery

Add a negotiated compact capability manifest with API/schema versions, feature versions, limits needed for safe requests, and a contract fingerprint. Detailed operation schemas and examples are requested by feature/topic. Cache contracts by server instance/build and fingerprint, never by module ID alone; reconnect, build changes, or an unknown fingerprint invalidate the cache. Revisions and runtime status are not cached as capability facts.

Split the agent reference into a short composing entry point and focused references for note edits, conditions/evolution, arrangement, automation, harmony/voicing, pitch systems, and recovery. Keep a single authoritative definition per contract. The entry point should explain when to load each reference rather than embed every example.

### 5.2 Arrangement and object summaries

Provide a bounded arrangement view containing scene order/duration/repeats, track-to-pattern assignments and overrides, harmony bindings, and referenced automation/pitch-context IDs. Include note counts, lengths/resolutions, and reference counts for patterns. Do not include note arrays by default.

Offer field projection and pagination for large object inventories using the existing revision-bound cursor approach. Describe inherited versus explicit bindings unambiguously. A summary is an orientation aid, not a substitute for an exact event read when editing an unfamiliar detail.

### 5.3 Response profiles

Negotiate `response_profile: "receipt" | "summary" | "full"` for new clients; preserve old response defaults. Receipt includes success/error, accepted/active/pending revisions, apply boundary, phase policy, per-operation counts, affected object IDs, warnings, and explicit omission totals. Event-ID mappings and detailed before/after values are opt-in or fetched through bounded revision-bound report pages.

An edit response may already prove that a targeted structural change was accepted. Do not require a second full pattern read merely to repeat that fact. Focused readback remains necessary for derived results not covered by the receipt or when diagnosing a mismatch. Adoption status and audible checks remain separate.

Report retrieval must not depend on an unbounded history. Return an explicit expired-report error when bounded retention no longer contains it, with recovery through current focused views. Full output is still subject to hard limits and pagination, not an unlimited response mode.

## 6. P8B — Compact exact authoring

### 6.1 Columnar event batches

Add a versioned batch encoding to note insertion/pattern creation. The proposed shape below is illustrative, not a currently callable operation:

```json
{
  "op": "insert_note_batch",
  "pattern_id": "lead",
  "encoding": "columns_v1",
  "columns": ["step", "tuned.degree", "velocity"],
  "rows": [[0,0,0.8], [3,2,0.65], [6,4,0.9], [10,3,0.7]],
  "defaults": {"gate": 1.5},
  "expect_count": 4
}
```

Column names come from an explicit versioned allowlist, not unrestricted JSON paths. All rows have exactly the declared width; duplicate/conflicting columns reject. A column may carry a complete typed object (such as `harmonic`, `tuned`, `condition`, or `observation`) where that is clearer than flattened fields. No overloading plain integers to mean both notes and degrees.

Expansion order: batch defaults, row fields, then explicitly indexed sparse row overrides. Defaults are authoring shorthand materialized into the inserted events, not a new live inheritance layer. Omitted fields continue to use existing score inheritance. Reject overlapping representations and conflicting parent/child fields rather than guessing. Null is not a new omission marker; reject it wherever the existing field contract rejects it. For irregular omissions use sparse overrides or ordinary object events.

The canonical event object remains the escape hatch for every supported field. Unsupported future fields are rejected precisely until the encoding advertises them, never dropped. Error paths identify the source row/column and canonical field. An optional expansion preview returns selected rows only; a full expansion should not be the normal response.

Use existing ID allocation rules; preview consumes no IDs. Deterministic expansion order is row order, with the same ordering/duplicate-step constraints as canonical insertion. Apply existing per-pattern, per-transaction, and compiled-state limits to expanded output, with preflight checks before large allocations.

### 6.2 Typed rhythmic repetition

Support an optional bounded arithmetic onset series (`start`, `spacing`, `count`) paired with an explicit pitch/expression cycle. Units are pattern steps; count and cycle length are explicit. Cycle repetition is opt-in, never inferred from mismatched array lengths. Reject off-grid positions, overflow, collisions, and out-of-pattern events using existing contracts.

Expand to ordinary events at edit time. Do not add an audio-thread pattern generator. Euclidean rhythms or other musical generators can be added later only with versioned deterministic algorithms and evidence of token savings; they are not required to deliver P8.

## 7. P8C — Reuse phrases and sections safely

First exploit existing shared patterns, scene assignment overrides, conditions, evolution, automation, and relative harmony. A repeated section with different intensity often needs only an assignment override or condition, not another note list.

Add the missing structural conveniences:

- Partial pattern-property updates for length/resolution, evolution, and P7 pitch context, with existing notes preserved and revalidated. Shrinking a pattern never silently deletes notes.
- Clone a pattern into a new destination, with explicit source revision and transform operations. Preserve source context by default under P7's copy rules; destination retuning is explicit.
- Clone a scene with `patterns: "share" | "copy"` explicitly selected. Shared patterns remain shared; copy requires a complete destination ID mapping. Report all affected references before replacement.
- Apply a bounded series of existing transforms to a copied/selected phrase in one transaction. Reuse current selector, transposition, rotation, expression, and collision semantics.

These are edit-time conveniences, not a persistent graph of dependent motifs. The result is self-contained canonical state. Later source edits do not change a materialized copy; shared pattern references do. This distinction must appear in the preview/receipt.

Cloning copies musical fields and handles observation markers according to explicit copy policy with the existing warnings. Pattern-local note IDs and allocation counters follow a documented deterministic rule; new duplicate events receive fresh IDs. Report that different pattern/track identities can alter seeded variation: identical copied notes are not a promise of identical random playback across tracks.

Persistent linked motif derivation is deferred. It creates dependency propagation, local-override, identity, adoption, and debugging costs that may exceed its token savings. Consider it only after measuring the materialized approach.

## 8. P8D — Prepare once, commit by handle

For large or solver-heavy edits, add optional prepared transactions:

1. Prepare sends operations once with `expected_revision` and requested adoption/phase policy.
2. Server expands and validates the candidate and returns a bounded preview plus an opaque handle, base revision, expiry, and contract/build identity.
3. Commit sends the handle and the same expected revision. It atomically accepts exactly that prepared candidate as one undo unit.

The handle binds the complete transaction and policies. It is not permission for unspecified later edits and cannot be combined with additional operations at commit. Preview does not advance revisions, publish a snapshot, allocate persistent IDs, or create undo state. Prepared candidates reserve no authority over later user edits.

Reject stale revision, expired handle, changed module instance/build, or incompatible policy without any partial mutation. Do not automatically rebase or silently recompute a different result. If the client loses the commit response, querying/retrying that handle must distinguish committed from uncommitted and return the original receipt while retained; never apply twice. After retention expires, report unknown/expired and require current-state inspection, not blind replay.

Bound handle count, total retained bytes, TTL, and solver/expansion work; publish limits. Prepared state lives on the control side and is not serialized into a Rack patch. Account for candidate and diagnostic storage separately from the audio snapshot budget. Restart invalidates handles. Legacy direct EDIT remains the inexpensive route for small, well-understood changes.

## 9. Whole-pipeline operating policy

Recommended negotiated workflow:

1. Connect and discover the exact module. Fetch a compact manifest once per instance/build, loading only relevant feature references.
2. Read arrangement summary and accepted/active status. Fetch exact objects or projected notes only where needed.
3. Choose shared arrangement references, overrides, conditions, evolution, curves, or voicing before spelling out repeated events.
4. Send one coherent atomic edit. Use prepare/commit for a substantial candidate that benefits from inspection; direct EDIT still performs validation.
5. Use the receipt to verify accepted structural changes. For quantized adoption, use bounded status checks; never spin until a stopped transport reaches an impossible boundary.
6. Inspect focused effective context when derived harmony/tuning needs checking. For audible verification, use physically connected monitor inputs and bounded observations; summaries cannot prove signal routing or sound quality.
7. Revise the selected events/objects rather than retransmitting the piece.

After revision conflicts, fetch the changed objects or fresh summary and reconsider. Do not reuse stale selectors, handles, or cached values by assuming unrelated changes. A future delta endpoint may help, but P8 does not require an unbounded event log.

For continuation after context loss, reconstruct an orientation packet from canonical state: title/intent, arrangement, track roles if authored, relevant feature versions, object IDs, and accepted/active state. Do not pretend inferred musical intent is authored fact. Avoid duplicating the entire composition into a separate persistent prose memory.

## 10. Validation and release gates

| Area | Required evidence |
|---|---|
| Expressive equivalence | Compact and canonical inputs yield equivalent normalized events, IDs where applicable, and playback traces; cover ties, ratchets, microshift, probability, conditions, evolution, all MOD lanes, automation, and observations |
| P7 fidelity | Native steps/degrees/periods, exact ratios/cents, context inheritance, detuning, scene transforms, and native voicing remain distinct; include 38-EDO and 53-EDO |
| Error behavior | Invalid widths, conflicting pitches, unknown columns, invalid defaults, expansion overflow, bad clones, and collisions reject atomically with useful source paths |
| Reuse | Shared versus copied behavior, copy contexts, observation policy, IDs, and seeded variation are explicit and tested |
| Prepared edits | No preview side effects; stale/expired/restarted handle failure; duplicate commit recovery; unchanged undo/adoption semantics; bounded memory |
| Reads and reports | Projection and paging preserve exact fields; stale cursors fail; all truncation is visible; compact reports retain warnings and revision state |
| Compatibility | Old request defaults and schema imports remain supported; capability negotiation prevents use against older builds |
| Performance | No added hot-path work; bounded expansion/report/preparation memory; native Windows build and relevant existing suites pass |
| Token savings | Replayed whole-workflow traces, tokenizer/configuration, per-scenario results, retries, call count, latency, and limitations are published |

Use meaningful equivalence/property tests against canonical operations, not only examples that mirror the encoder. Retain legacy golden tests and P7 integration coverage. Live testing must distinguish semantic acceptance, actual adoption, output voltages, and physically monitored audio.

## 11. Delivery order and scope decisions

**P8A:** baseline measurement, reference splitting, compact discovery, arrangement views, response profiles. Useful independently of P7 and likely the lowest-risk savings.

**P8B:** compact event batches and bounded onset repetition; canonical expansion and equivalence coverage.

**P8C:** partial pattern edits, explicit cloning, and transform composition; integrate P7 copy/context semantics.

**P8D:** prepared transactions and recovery; whole-workflow benchmark and live verification.

Implement against the actual P7 contract when available. Do not invent a temporary 12-TET-only compact pitch syntax that must be replaced immediately afterward. Keep optional features only when measurements justify their maintenance and cognitive cost.

Not required: arbitrary scripting/eval, natural-language interpretation inside Rack, hidden random generation, base64/compressed score blobs for the agent to decipher, shortened aliases for every field, or a second playback engine. Dense notation that creates more mistakes and retries is not a successful token optimization.

**Review decision:** prioritize semantic reuse and smaller context first, exact compact authoring second, and transaction payload reuse third. Preserve an intelligible canonical score throughout.
