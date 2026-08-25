# Octavia AI Membrane — Current Implementation Notes

Audit date: 2026-08-24  
Audited branch: `zoom_and_shaders` at `1f885f88`  
Design reference: `doc/AI_rack_membrane.md`

## Summary

The architecture proposed in `AI_rack_membrane.md` remains a strong direction, but the
implementation is still at the precursor stage.

- Octavia's Rack-control and MCP tool plane is mature and useful today.
- Octavia Console is a functional human-to-agent mailbox and transcript UI.
- The protocol-neutral AI membrane—the sidecar, normalized sessions and events, inference
  adapters, ACP support, permissions, cancellation, and backend discovery—has not yet been
  implemented.

The existing system supplies good foundations and should be evolved rather than replaced.

## Current status

| Area | Status | Notes |
|---|---|---|
| Rack HTTP control surface | Strong | Supports module and cable inspection, patch editing, audio analysis, undo, save, Temporal Deck, and Sibyl operations. |
| MCP tool plane | Strong | Exposes roughly 30 tools spanning inspection, mutation, analysis, Console, and semantic sequencing. |
| Local security boundary | Partial but sound | The server binds to loopback and supports `OCTAVIA_TOKEN`; authentication remains optional and is not automatically provisioned. |
| Realtime isolation | Strong foundation | Console text traffic is explicitly kept outside the audio processing path. |
| Console panel and transcript | Functional prototype | Provides attachment detection, prompt entry, send control, scrolling transcript, and simple agent status. |
| Console transport | Functional legacy path | Provides status, bounded long-poll prompt retrieval, and one complete response per prompt. |
| Console MCP integration | Functional | Provides `vcv_octavia_console_status`, `vcv_octavia_console_wait`, and `vcv_octavia_console_respond`. |
| Protocol-neutral sessions | Missing | No `BackendDescriptor`, `Session`, `Turn`, normalized `Event`, or capability vocabulary exists in code. |
| Incremental streaming | Missing | Responses are accepted only as complete strings, limited to 16,384 characters. |
| Octavia AI Bridge sidecar | Missing | No self-contained helper executable currently exists. |
| Raw-model inference adapters | Missing | No OpenAI-compatible, Ollama, vLLM, hosted-provider, or gateway adapter exists. |
| ACP integration | Missing | No ACP client, agent subprocess lifecycle, capability negotiation, or `codex-acp` integration exists. |
| Permission handling | Missing | There is no Console-facing agent approval flow. |
| Turn cancellation | Missing | There is no semantic or transport-level cancellation exposed by Console. |
| Backend discovery and profiles | Missing | There is no backend availability, authentication state, model selection, or capability UI. |
| Failure recovery and event replay | Missing | There is no normalized SSE event log, resumable cursor, session recovery, or degraded backend lifecycle. |

Approximate maturity, for orientation rather than project accounting:

- Octavia Rack/MCP control plane: **75–85%** of a robust environment interface.
- Console mailbox: **about 70%** of its limited prompt/job purpose.
- Complete `AI_rack_membrane.md` vision: **about 15–25%**, primarily reusable foundations rather than the membrane itself.

## Document drift to correct

The current-state inventory in `AI_rack_membrane.md` describes functionality that is not
present in this checkout.

### Background workers and SSE

The document says the Console mailbox contains workers, leases, claims, event IDs, and an
SSE event feed. The current implementation contains only:

- stable prompt IDs;
- a bounded prompt queue;
- bounded long-poll waiting;
- one complete response per prompt;
- simple agent state;
- bounded transcript accumulation;
- mailbox registration by module ID.

No Octavia worker registration, heartbeat, lease, claim token, Console SSE route, or
`Last-Event-ID` implementation was found in the current branch or the other locally
available repository branches. References to those facilities inside `cpp-httplib` are
library capabilities, not an Octavia Console implementation.

### Persisted Console state

The document says Console persists a `backgroundWorkerEnabled` flag. The current
`OctaviaConsole` implementation has no `dataToJson()` or `dataFromJson()` override and
appears to persist no Console-specific state.

### Codex connector

The document refers to an existing Python Codex app-server connector. The current `MCP/`
tree contains a general MCP-to-Octavia HTTP adapter, not a Codex app-server bridge.
Currently, an already-running MCP-capable agent must explicitly enter Console Mode and
poll the mailbox. Installation also requires Python 3.10 or newer plus the MCP runtime
dependencies.

### Branch and snapshot

The design document describes a crawl of the `expander` branch rather than a pinned local
commit. Future current-state claims should name an exact commit so architectural decisions
can be separated from repository drift.

## Foundations worth preserving

Several existing choices align closely with the proposed membrane:

- MCP is independent from ordinary Console inference.
- Rack exposes musical capabilities rather than provider-specific AI abstractions.
- The Console owns a bounded visual transcript.
- Prompt, response, queue, and transcript sizes are bounded.
- Mailbox synchronization and HTTP activity remain outside `process()`.
- Multiple Console modules are independently addressable by Rack module ID.
- The existing prompt/complete-response routes can remain as a compatibility shim.
- Sibyl demonstrates the value of semantic, revision-aware APIs for agent authorship.
- Loopback HTTP is already an established and inspectable local process boundary.

## Recommended first implementation slice

Begin with the Foundation and Mailbox Generalization stages. Do not start by implementing
many providers.

1. Correct the current-state section of `AI_rack_membrane.md`.
2. Define protocol-neutral `BackendDescriptor`, `Session`, `Turn`, and `Event` types without
   importing ACP, OpenAI, or provider-specific vocabulary into Rack.
3. Add incremental `text.delta` and explicit terminal events to the mailbox while adapting
   the existing `postResponse()` path into a one-turn legacy session.
4. Add bounded event sequencing and cursor-based replay before connecting a real provider.
5. Add a dedicated asynchronous Rack-side Bridge client; keep all networking, parsing,
   waiting, and callbacks unreachable from the audio thread.
6. Build the smallest external Bridge proof of concept with:

   - one OpenAI-compatible local backend, such as Ollama or vLLM;
   - one ACP agent through `codex-acp`;
   - the existing Octavia MCP server as the independent Rack capability plane.

This is enough to test the architectural thesis before investing in provider discovery,
credential stores, rich artifacts, multimodal input, or a polished backend browser.

## Minimum proof-of-concept acceptance criteria

- A local model streams incremental text into the existing Console transcript.
- An ACP agent creates a session and streams normalized activity and text events.
- The ACP agent can inspect Rack through the existing MCP tools.
- A permission request reaches Console and receives an explicit decision.
- Cancellation reaches both an ACP turn and at least one HTTP inference backend.
- Restarting or killing the helper does not disturb Rack audio processing.
- An interrupted event stream resumes from a sequence cursor without duplicate transcript text.
- No API key, OAuth token, or bridge secret is serialized into a `.vcv` patch.
- No networking, JSON parsing, subprocess operation, wait, or provider callback is reachable
  from the audio `process()` path.
- A mock future provider can be added outside the plugin with no C++ changes.

## Validation performed during this audit

- `octavia_console_mailbox_spec`: passed.
- `octavia_job_control_spec`: passed.
- The Python MCP contract suite was not executed because `pytest` was unavailable in the
  active Python environment.

No implementation files were changed during the audit.
