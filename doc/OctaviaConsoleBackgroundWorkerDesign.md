# Octavia Console Background Worker Design

Status: proposed

## Purpose

The Octavia Console currently supports an explicitly armed agent by way of bounded MCP
long-polling. This works, but the agent host may stop listening when its active turn ends.
The Console therefore needs an optional background-worker path that can notify a resident
supervisor and let that supervisor start agent work asynchronously.

The first known worker backend is Codex `app-server`. The Console protocol must not depend
on Codex: Claude Code, Antigravity, a notification-only service, or another future host
must be able to implement the same worker contract.

The existing MCP tools and interactive workflow must continue to work throughout the
change:

- `vcv_octavia_console_status`
- `vcv_octavia_console_wait`
- `vcv_octavia_console_respond`

The central invariant is:

> Every accepted prompt remains pending until one valid completion is recorded. A
> disconnect, timeout, retry, or duplicate request must neither lose the prompt nor allow
> two consumers to process it concurrently under valid claims.

This is an at-least-once delivery system with exclusive, leased processing. Exactly-once
agent execution cannot be guaranteed across process failure, but exactly one completion
is accepted for each prompt ID.

## Goals

- Preserve the current interactive MCP workflow and request schemas.
- Require explicit, persisted user consent before a background worker may claim prompts.
- Wake connected workers without constant short polling.
- Keep prompts available across transport and worker disconnects.
- Arbitrate interactive and background consumers through one mailbox.
- Make retries, duplicate completion, and stale workers safe and observable.
- Keep all mailbox and HTTP work off Rack's audio thread.
- Keep agent-host details outside the Rack plugin and MCP tools.

## Non-goals

- Octavia will not launch Codex, Claude Code, or Antigravity directly.
- Octavia will not manage agent credentials, model selection, or conversation history.
- The first version will not promise prompt survival across Rack process termination.
- The event stream will not contain prompt text or replace the mailbox.
- Enabling a worker will not broaden the agent's permission to delete modules, save a
  patch, or perform other destructive actions.

## User-facing policy

The Console context menu gains an **Agent handling** section:

```text
Agent handling
  ● Interactive listener
  ○ Allow background worker (experimental)
```

`Interactive listener` is the default. It accepts the existing bounded-poll workflow and
rejects background worker registration and claims.

`Allow background worker` permits a registered external worker to claim prompts. The
interactive workflow remains available in this mode; the first consumer to obtain a valid
claim owns the prompt. A background-only mode is not needed initially.

The option is saved in the module's patch JSON. It is a permission, not an assertion that
a worker exists. Enabling it must not make the panel display `ARMED` until a worker has a
live registration lease.

## Architecture

```text
Octavia Console UI
        |
        v
Mailbox: queue + claims + completion records
        |
        +---- existing HTTP long-poll ---- MCP tools ---- interactive agent
        |
        `---- event stream + worker API ---- supervisor ---- host adapter
                                                   |
                                                   +-- Codex app-server
                                                   +-- Claude Code (future)
                                                   `-- Antigravity (future)
```

The mailbox is authoritative. Event delivery only tells a supervisor that it should
inspect the mailbox. Missing, duplicated, or replayed events cannot change prompt
correctness.

The supervisor is a separate process. It maintains the worker lease, receives events,
claims prompts, invokes its configured host adapter, posts the result, and acknowledges
completion. A Codex adapter may retain and resume an app-server worker thread, but those
thread IDs never appear in Octavia's protocol.

## Mailbox model

### Prompt record

Each accepted prompt has:

- monotonically increasing `promptId` within the module instance;
- immutable prompt text;
- state: `queued`, `claimed`, or `completed`;
- optional claim owner, opaque claim token, and claim expiry;
- terminal response text and error flag after completion;
- timestamps for submission, claim, and completion where practical.

Completed prompt bodies may be removed from the active queue, but the mailbox retains a
small bounded completion record containing at least the prompt ID and outcome. This makes
duplicate completion idempotent instead of reporting a misleading “not pending” error.

### Worker registration

A background supervisor registers against one Console module and receives an opaque
worker ID or registration token. Registration has a renewable lease. Heartbeats extend
the lease; disconnecting the event stream alone does not immediately invalidate it, since
a transient reconnect must not create panel-state churn.

Initial values should be conservative and configurable in code:

- worker lease: 30 seconds;
- heartbeat interval: 10 seconds;
- background claim lease: 60 seconds;
- legacy interactive claim lease: 10 minutes.

The implementation must use `steady_clock` for lease calculations. Wall-clock timestamps
may be exposed for diagnostics but must not control expiry.

### Claim behavior

Claiming a queued prompt is atomic under the mailbox mutex. A successful background claim
returns the prompt text, claim token, and expiry. A competing claim reports that the prompt
is already claimed without exposing its text.

A claim owner may renew or release its claim. When a claim expires, the prompt returns to
`queued` and a new availability event is emitted. A late completion from the expired owner
is rejected if another owner has since claimed the prompt.

The claim token, rather than worker ID alone, authorizes renew, release, and completion.
This distinguishes separate attempts by the same worker after expiry.

### Completion behavior

Completion is accepted only when:

- the prompt is pending and the supplied claim is current; or
- the prompt is owned by the compatible legacy interactive path described below.

The first accepted completion becomes authoritative, removes the prompt from the active
queue, updates the transcript, and records the terminal outcome. Repeating the same
completion operation returns success without duplicating the transcript. A conflicting
second completion returns a conflict.

New worker completion requests include a stable operation ID so a supervisor can retry
after losing the HTTP response.

## Existing MCP compatibility

The three existing MCP tool input schemas remain unchanged.

### Status

`GET /console/{moduleId}/status` retains all current fields. New fields may be added for
diagnostics, including background permission, live worker count, queued count, and claimed
count. Existing clients can ignore them.

### Wait

`GET /console/{moduleId}/prompt?after=...&waitMs=...` continues to return either the current
prompt object or `{"prompt":null}`. Internally, returning a prompt atomically assigns a
synthetic legacy-interactive claim so a background worker cannot process it concurrently.
Already claimed prompts are skipped.

Because the existing response call has no claim token, the legacy claim is tied to the
prompt ID and owner type. Only one legacy wait receives it. Its longer lease preserves the
current expectation that an agent can perform substantial work before responding. If that
lease expires and another consumer claims the prompt, the late legacy response is rejected
rather than overwriting the newer attempt.

A wait timeout does not set or preserve `ARMED`. It proves only that one HTTP request was
open briefly, not that an agent remains available.

### Respond

`POST /console/{moduleId}/response` retains its current body. It completes only a current
legacy-interactive claim or, during migration, an unclaimed prompt produced by the old
implementation. Duplicate responses matching an already recorded completion return
success. Conflicting responses return HTTP 409.

This compatibility path may be tightened only in a future versioned API.

## Background worker HTTP API

Exact paths and JSON names may be refined during implementation, but the semantic surface
is:

```text
POST /console/{id}/workers/register
POST /console/{id}/workers/{workerId}/heartbeat
POST /console/{id}/workers/{workerId}/unregister
GET  /console/{id}/events
POST /console/{id}/prompts/{promptId}/claim
POST /console/{id}/prompts/{promptId}/renew
POST /console/{id}/prompts/{promptId}/release
POST /console/{id}/prompts/{promptId}/complete
```

All endpoints use Octavia's existing localhost server and token authentication. Background
registration and claims return 403 unless the module option is enabled. Disabling the
option revokes worker registrations, releases their claims, and makes those prompts queued
again.

The worker API must use explicit status codes:

- 200/201: success, including an idempotent retry;
- 403: background processing is not enabled;
- 404: Console or prompt does not exist;
- 409: claim or completion ownership conflict;
- 410: claim or worker lease expired.

## Event stream

The initial transport should be Server-Sent Events because workers need one-way wakeups
and ordinary HTTP remains useful for inspection and mutation. The implementation may use
another persistent transport later without changing mailbox semantics.

Events include no prompt text. Example event kinds are:

- `prompt.available` with module ID and prompt ID;
- `claim.expired` with module ID and prompt ID;
- `worker.revoked`;
- a periodic keepalive comment for connection health.

Each event has a monotonically increasing event ID. Reconnection may supply the last seen
ID for replay from a small bounded event buffer. If replay is unavailable, the server emits
a `resync` event and the worker lists or claims currently queued prompts. Therefore event
loss never loses work.

The event stream must be implemented in the control/HTTP layer. It must not block the Rack
audio thread, module `process()`, or UI `step()`.

## Panel state

Transport state and prompt state should be derived rather than assigned opportunistically.
Suggested display precedence is:

1. `DETACHED` when the Console is not attached to Octavia.
2. `ERROR` for the latest terminal or local validation error.
3. `WORKING` when any prompt has a valid claim.
4. `QUEUED` when prompts exist but none is currently claimed.
5. `REPLY` after a successful completion with no pending work.
6. `ARMED` when a background worker lease is live or an explicitly tracked interactive
   listener session is live.
7. `READY` otherwise.

For the first compatibility revision, interactive long-poll requests are not sufficient to
display `ARMED`; only a durable registration/lease can make that claim honestly. If a
future MCP client supports a persistent subscription, it can register as an interactive
listener using the same lease mechanism.

## Persistence and restart behavior

The `Allow background worker` preference persists with the module. Worker registrations,
claim tokens, and lease deadlines never persist.

The first implementation keeps prompt and transcript data in memory, matching the current
Console. “Durable mailbox” in this design means durable across request, transport, and
worker failure while Rack remains running. Persisting prompt text in a patch would have
privacy and surprising-save implications and is explicitly deferred.

On Rack or module restart:

- the worker reconnects and obtains a new registration;
- all previous claims are invalid;
- no panel state implies that the old worker is still connected;
- prompt survival is not promised until a separate journal design is approved.

## Security and safety

- Background processing is disabled by default per Console module.
- Existing Octavia token authentication applies to events and worker operations.
- Prompt text is returned only after a successful claim.
- Tokens and claim secrets are never shown in the panel, transcript, or normal status call.
- The supervisor must bind host control endpoints to loopback or authenticated local IPC.
- Agent execution preserves the same tool approval and destructive-action rules as the
  interactive Octavia workflow.
- Rate and queue limits remain enforced to prevent an unattended prompt storm.

## Implementation sequence

### Phase 1: mailbox correctness and compatibility

- Add explicit queued/claimed/completed records and lease expiry.
- Stop deriving `ARMED` from `waitForPrompt()`.
- Preserve existing MCP schemas and verify the current Console workflow.
- Add idempotent completion bookkeeping and concurrency tests.

This is the first rollback boundary: no background API is required to ship the mailbox
corrections.

### Phase 2: Console policy and worker API

- Add and persist the context-menu option.
- Add registration, heartbeat, claim, renew, release, and completion endpoints.
- Derive panel state from worker and prompt state.
- Test option disable/re-enable, worker expiry, and competing consumers.

### Phase 3: asynchronous event delivery

- Add SSE connection management, replay IDs, keepalives, and resynchronization.
- Verify prompt delivery after idle periods and across stream reconnects.
- Confirm there is no measurable audio-thread or UI-step cost.

### Phase 4: Codex reference adapter

- Run or connect to Codex app-server through local authenticated IPC.
- Retain a dedicated Octavia worker thread and resume it after supervisor reconnect.
- Translate claimed prompts into `turn/start` calls and terminal agent messages into
  completion calls.
- Keep Codex thread IDs and app-server details entirely within this adapter.

Other host adapters can then be evaluated against the same supervisor contract.

## Required tests

Mailbox unit tests:

- submit, claim, renew, release, expire, and complete;
- simultaneous claims yield exactly one winner;
- expired-owner completion cannot replace a new valid claim;
- duplicate identical completion is idempotent;
- conflicting completion is rejected;
- disabling background mode revokes workers and requeues their prompts;
- queue and text limits remain enforced.

Compatibility tests:

- current status/wait/respond sequence succeeds without new arguments;
- wait timeout does not leave `ARMED` behind;
- a legacy claim excludes a background claim;
- a background claim is not returned by legacy wait;
- old MCP clients ignore additive status fields.

Integration tests:

- an idle SSE subscriber receives a newly submitted prompt event;
- reconnect and resync find every queued prompt;
- worker death causes lease expiry and re-delivery;
- one response appears in the transcript after retrying completion;
- Rack remains responsive while streams are connected;
- Codex app-server can process a claimed prompt and return a terminal response.

## Open questions

- Whether the supervisor belongs in the existing Python MCP package or in a separate small
  executable. A separate process gives clearer lifetime ownership; shared libraries may
  still be appropriate.
- Whether SSE support in the current embedded HTTP library is sufficiently robust for long
  lived connections on every Rack platform.
- Whether a future version should journal queued prompts outside the patch for process
  restart durability.
- Which control hooks, if any, Claude Code and Antigravity provide for their host adapters.

