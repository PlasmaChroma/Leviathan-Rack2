# Bifurx render-worker lifecycle and source-layout implementation specification

**Target:** `PlasmaChroma/Leviathan-Rack2`, `expander` branch  
**Prepared:** September 22, 2026  
**Primary input:** `render-worker-lifecycle-plan.md`  
**Deliverable:** A staged, Codex-oriented implementation contract; not an implemented patch or a completed validation report.

## 0. Read this first

Implement the lifecycle refinement first, with a limited architectural extraction that makes ownership visible. Then split the remaining large presentation files in independently reviewable changes. Do not combine this work with a DSP redesign, graphics-backend rewrite, new job system, or audio-publication replacement.

The central design is:

```text
Audio module                         UI thread                         Worker thread
------------                         ---------                         -------------
Existing bounded publication  --->   BifurxSpectrumBase
                                     - observe published inputs
                                     - synchronous fallback
                                     - adopt targets and animate
                                             |
                                     BifurxRenderClient  ------------> BifurxUiRenderService
                                     - registration                    - one worker
                                     - submission bookkeeping         - bounded coalescing
                                     - three-slot payload pool         - claim / prepare / publish
                                     - snapshot polling                - immutable snapshots
                                             |
                                  NanoVG / OpenGL presentation
                                  - draw the UI-owned state
                                  - retain context-owned resources
```

The worker prepares CPU-side curve/FFT targets. It does not own widgets, run GL/NanoVG commands, mutate the live module, or animate the displayed curve.

### Source basis and interpretation

`[P]` refers to the supplied lifecycle plan. `[S1]`–`[S14]` identify inspected repository sources in Appendix A. Sections headed **Observed** describe the retrieved code. Statements using **must**, **shall**, or **proposed** below are implementation requirements or design choices, not claims that the current implementation already has those properties.

The inspection used branch-addressed raw sources, not a locally checked-out, commit-pinned repository. The branch can advance. Before implementing, record the local SHA, working-tree status, symbol locations, file counts, and current test baseline. Prefer the local checkout if its contents differ; document the difference rather than mechanically applying old line numbers.

No native build, runtime test, Rack session, or performance measurement was executed while preparing this specification. All validation below is work required of the implementation.

## 1. Goals and scope

### 1.1 Required outcomes

1. Make display registration, request ownership, completion, cancellation-by-unregistration, restartable stop, and terminal plugin shutdown explicit.
2. Preserve audible output, the audio thread's work, rendered targets, interpolation, overlay continuity, and all current supported render modes.
3. Make queued, in-flight, and completed states testable without relying on scheduler timing.
4. Consolidate the UI handoff into one owner rather than duplicating lifecycle policy across backend and visibility paths.
5. Reduce the architectural concentration in `Bifurx.cpp`, `Bifurx.hpp`, `BifurxUI.cpp`, and, in a later structural pass, `BifurxGL.cpp`.
6. Keep the existing module-level `Process`, `Step`, and `Draw` meanings and measure worker costs separately.

These extend the supplied plan's four work stages, without changing its non-goals. [P, lines 5–23, 25–59]

### 1.2 Explicit non-goals

Do not introduce a thread pool, per-display worker thread, lock-free queue, new FFT algorithm, new audio-side metadata/publication protocol, universal rendering framework, renderer redesign, or different pool capacity. Do not change filter equations, resampling behavior, panel assets, parameter IDs, JSON keys/defaults, licensing behavior, or the meaning of AUTO mode.

Do not unify the worker and synchronous execution paths merely to remove duplicated-looking code. First prove matching target preparation and preserve their distinct allocation/lifetime behavior. The synchronous path remains both a fallback and an independent parity reference.

Do not move GL resource cleanup into destructors or the render worker. Do not make a display release stop the shared service.

## 2. Observed implementation and architectural implications

### 2.1 Working inventory

Approximate physical line counts from the retrieved raw sources are navigation aids, not implementation quotas:

| Existing file | Approximate lines | Responsibility to consider |
|---|---:|---|
| `src/Bifurx.cpp` | 2,329 | DSP/module code mixed with preview and display implementation |
| `src/Bifurx.hpp` | 801 | Module, numerical, display, and UI-facing declarations |
| `src/BifurxUI.cpp` | 1,336 | NanoVG presentation, diagnostics, panel composition, UI coordination |
| `src/BifurxGL.cpp` | 1,781 | GL widget, resources, shaders, geometry, rendering |
| `src/BifurxWorker.cpp` | 266 | Service scheduling/lifecycle plus worker snapshot carry-forward |
| `src/BifurxRenderPrep.cpp` | 245 | CPU target preparation |
| `tests/bifurx_runtime_spec.cpp` | 2,479 | Numerical/runtime tests and source-inclusion harness |

[S1–S7]

The worker itself is not the primary file-size problem. The first useful extraction is the display controller, not an elaborate scheduler framework.

### 2.2 Observed lifecycle details worth tightening

The service represents a slot with `active`, `queued`, `hasPending`, and `inFlight`. Unregister erases the map entry but leaves queue cleanup to later worker traversal. Global shutdown is checked before admission locks in several methods; `submitLatest()` does not separately reject local stopping. A concurrent `stop()` returns early when joining is already in progress. These are source-level observations, not claims of reproduced user-facing failures. [S2]

The proposed response is to test the relevant interleavings, use one locked admission policy, bound queue entries during display churn, and make stop completion semantics explicit.

### 2.3 Existing test/build coupling

The runtime harness includes `Bifurx.cpp` directly. The GL test includes that runtime harness and `BifurxGL.cpp`; some tests also access implementation-local scratch helpers. The Makefile separately supplies worker/preparation sources. Mechanical source moves therefore require deliberate test linkage changes. [S7, S8, S9]

Keep structural edits separate from algorithm/lifecycle edits, and never compile a source separately in a target that already includes it textually.

## 3. Invariants and terminology

### 3.1 Ownership invariants

| Object | Owning/executing domain | Allowed sharing |
|---|---|---|
| Existing audio publication slots | Audio module | Existing bounded reader protocol only |
| Mutable producer payload slot | One UI-thread render client | Mutable only while exclusively reusable |
| Submitted payload lease | Pending/claimed request | `shared_ptr<const Payload>`; no mutation |
| Claimed request | Worker stack/job scope | No references into the service map |
| Completed snapshot | Immutable shared value | Service and UI may retain independent references |
| Displayed targets/animation/caches | UI-side spectrum base | Accessed by the active UI renderer |
| GL/NanoVG resources | UI graphics-context owner | Never transferred to worker requests |
| Slot map, queue, service lifecycle | Service mutex | No unprotected access |

The audio callback must not acquire a service mutex, construct a request, touch a payload pool, wait for a worker, or allocate because of a render request. Existing audio publication is unchanged. [P, lines 17–22]

### 3.2 Scheduling invariants

For each live display registration:

- At most one claimed job exists.
- At most one replaceable pending request exists.
- At most one queue entry exists.
- A queue entry means a pending job is ready to be claimed; an in-flight display with another pending request is not separately queued until its current job completes.
- Completion is retained independently of whether another job is queued or running.

For the service, there is only one executing job across all displays. A busy display must rejoin the tail rather than monopolize the worker.

Display IDs are opaque, nonzero, monotonically allocated identifiers for the lifetime of a service instance, including stop/start cycles. Zero is invalid. Never reset `nextDisplayId` on stop. On integer exhaustion, reject registration; never wrap and reuse an ID.

### 3.3 Distinguish four notions of progress

1. **Observed input:** latest publication read/observed by the UI.
2. **Accepted submission:** a request successfully admitted by the service.
3. **Completed result:** work prepared by the worker.
4. **Represented result:** the curve/overlay currently installed in the UI state.

Do not collapse these into one sequence counter. In particular, observing or submitting an FFT frame does not mean the displayed overlay represents it.

Use the 64-bit request sequence for order within one registration. Preview and analysis sequences retain their existing publication types and semantics. Do not introduce naive numeric ordering for wrapping 32-bit publication counters; prefer the existing equality/change contracts unless a separately tested wrap-aware comparison is required.

## 4. Service state and public contract

### 4.1 Slot state model

After characterization tests are in place, use this model or a demonstrably simpler equivalent:

```cpp
enum class SlotPhase {
    Idle,
    Queued,
    InFlight
};

struct DisplaySlot {
    SlotPhase phase = SlotPhase::Idle;
    std::optional<BifurxUiRenderRequest> pending;
    std::shared_ptr<const BifurxUiRenderSnapshot> latestSnapshot;
};
```

This is an illustrative declaration, not a drop-in patch. Verify the project's effective C++ standard before using `std::optional`; its test recipes currently request C++17. [S9]

Map membership replaces `active`. `SlotPhase` replaces the independent `queued`/`inFlight` flags. Presence of `pending` replaces `hasPending`.

**Do not introduce a mutually exclusive `Completed` slot phase.** A completed snapshot may remain available while the slot is queued or in flight. Likewise, `InFlight` and `pending.has_value()` are a valid combination. The plan's registered → queued → in-flight → completed wording is a job lifecycle, not a complete exclusive slot state machine.

| Current slot | Event | Result |
|---|---|---|
| Absent | Register | Idle, no pending/result |
| Idle | Accept request | Queued, one pending, append ID |
| Queued | Accept replacement | Queued, replace pending, keep queue position |
| Queued | Worker claims | InFlight, pending emptied, remove queue entry |
| InFlight | Accept request | InFlight, one pending, no queue entry |
| InFlight + pending | Accept replacement | InFlight, replace pending |
| InFlight, no pending | Publish | Idle, latest result replaced |
| InFlight + pending | Publish | Queued, latest replaced, append ID at tail |
| Any live slot | Unregister | Absent, no future publication under that ID |
| Any live slot | Stop/shutdown | No longer discoverable; claimed work may finish but is discarded |

Debug/test builds must assert the invariants at transition boundaries. Avoid O(number-of-displays) invariant scans in production hot paths.

### 4.2 Service lifecycle model

Use an explicit execution state:

```cpp
enum class RunState { Stopped, Running, Stopping };
```

Keep a separate, mutex-protected terminal-admission latch such as `permanentlyClosed`. It is independent information: a service can be stopping either temporarily or permanently. Removing this distinction merely to minimize field count would make the contract worse.

| Execution state | Terminal latch | Start | Register/submit | Snapshot lookup |
|---|---|---|---|---|
| Stopped | false | Starts worker | Reject until started | None |
| Running | false | Idempotent success | Accept valid work | Current registration only |
| Stopping | either | Fail promptly | Reject | None |
| Stopped | true | Reject forever | Reject forever | None |

Rejecting registration on a never-started/stopped service is an **intentional API tightening**. Audit all callers before adopting it; production callers must establish successful start before registering. Do not accidentally preserve dormant accepted jobs that can never be processed.

Local test instances have their own terminal latch. Closing a local service must not poison the production singleton or another local instance. The production global shutdown entry point closes the singleton permanently.

### 4.3 Proposed API changes

Preserve class and global entry-point names. Permit minimal return-value changes so UI fallback can observe admission failures:

```cpp
bool start();
uint64_t registerDisplay();
void unregisterDisplay(uint64_t displayId);
bool submitLatest(BifurxUiRenderRequest request);
std::shared_ptr<const BifurxUiRenderSnapshot>
    getLatestSnapshot(uint64_t displayId) const;
void stop();                 // restartable
void shutdown();             // terminal; also joins
```

`start()` returns true when already running or successfully started; false when stopping, terminally closed, or thread creation failed. `submitLatest()` returns true only after ownership was accepted under the mutex. No request is accepted for ID zero or an absent slot.

The class remains noncopyable. Explicitly prohibit moving a running service and prohibit destruction while external callers are still executing member functions. Mutexes do not make use-after-destruction legal.

### 4.4 Admission and shutdown linearization

The authoritative check for running/terminal state must be inside the same mutex-protected critical section that admits registration or submission.

A pre-lock atomic may remain as a rejection fast path only if it has a justified benefit. It must not be the only gate. Prefer eliminating the current global-only shutdown dependency from local service operations in favor of the per-instance terminal latch.

The locked terminal-latch transition is the defined linearization point for service shutdown. Operations admitted before it may already have returned; operations ordered after it must reject. No result prepared before that point may be newly published afterward.

Call `shutdownBifurxRenderService()` at the beginning of the plugin's `destroy()` lifecycle, before unrelated settings persistence. Document that this closes and joins the production worker. This deliberately closes the admission window at the beginning of plugin teardown, rather than after other shutdown work. The retrieved `destroy()` currently calls the worker shutdown after persistence. [S10]

Do not reset the terminal latch from `start()`, settings changes, or a late widget callback.

### 4.5 Stop and join algorithm

Use three stages:

**Under the service mutex:** transition Running → Stopping; close admission for the run; detach the worker's thread handle to the stopping caller; detach/clear the queue and slots so lookups cannot expose them. Retain detached state in local retirement storage if necessary.

**Outside the mutex:** notify the worker; join the detached thread. Keep the implementation object and any required context alive until the join completes. Retire detached state outside the service mutex, preferably after join for the most conservative teardown order.

**Under the mutex again:** publish Stopped and notify lifecycle waiters. Preserve the terminal latch and ID allocator.

An already-claimed job may finish computation. Its publication check must fail because the run is stopping or its slot is absent. There is no cancellation flag embedded in FFT loops and no forced thread termination.

Sequential repeated start/stop/shutdown calls must be harmless. A concurrent second stop/shutdown must not report completion while the first caller is still joining. It may wait on a lifecycle condition variable with the mutex released. A concurrent terminal shutdown must set the terminal latch even if a restartable stop already owns the join. `start()` while Stopping returns failure rather than waiting.

Production must serialize intentional restart operations through its lifecycle owner. Concurrent destruction remains unsupported. Never invoke `stop()`/`shutdown()` from the worker thread itself; assert this precondition. Do not detach as a workaround.

### 4.6 Claim / prepare / publish

Make the three regions visually obvious in the implementation:

```text
claim under mutex
    validate service state and queue head
    move pending request to worker-local storage
    mark slot InFlight
    copy immutable previous snapshot handle

prepare without mutex
    resolve curve/overlay carry-forward
    allocate/fill a new snapshot
    run prepareCurveSnapshot()
    stamp completion

publish under mutex
    re-check running/terminal state and exact display ID
    discard if registration disappeared
    replace latest snapshot
    transition Idle or requeue at tail

retire superseded ownership without mutex
```

No references, iterators, pointers, or borrowed array views into `slots` may escape the claim critical section. Worker-local copies must own everything they need.

Use narrow helper functions where they clarify these regions. Do not create a generic task/executor abstraction.

### 4.7 Queue boundedness under churn

Unregistration must remove an outstanding ready-queue entry rather than relying indefinitely on tombstone draining. The proposed initial implementation keeps `std::deque<uint64_t>` and removes the ID on queued unregister. This is O(number of queued displays) on a teardown path, not a per-frame traversal.

Prove:

```text
readyQueue.size() <= live registered displays
count(queue, displayId) <= 1
```

A test must hold one job in flight while repeatedly registering, submitting, and unregistering other displays. The queue must not grow with the number of destroyed displays.

Do not introduce an intrusive linked list or lock-free queue preemptively. If measured teardown contention makes deque removal unacceptable, record the evidence and propose a separate container change.

### 4.8 Retire large ownership outside the mutex

Moving a `shared_ptr` is cheap; releasing its final owner can run a destructor/deallocator. Do not assume replacing `pending` or `latestSnapshot`, or erasing a slot, is always free.

Move superseded pending requests, snapshots, or extracted map nodes into local retirement variables under the lock, then destroy them after unlocking. Preserve the predecessor snapshot handle while a worker needs it. Keep the implementation simple and avoid a new background reclamation mechanism.

The worker must not retain its payload in a completed snapshot. Otherwise completed results could pin all producer slots and defeat the pool bound.

## 5. Payload inheritance, completed carry-forward, and validity

### 5.1 Preserve the three-slot producer pool

The UI client owns the existing lazy three-slot pool. A slot is writable only when the client is its only owner and no submitted/claimed request can read it. After submission, treat all reachable payload storage as immutable.

Pool acquisition is nonblocking. Exhaustion does not grow the pool and does not wait. A preview-only request may still be submitted; an uncopied analysis sequence remains outstanding for a later UI tick.

The worker receives only the immutable lease. The only bulk audio-frame copy remains the UI copy from existing publication slots into the selected pool payload. Preserve `sizeof(BifurxUiRenderRequest) < 256` and the lazy, reusable synchronous scratch behavior. [P, lines 9–12; S3, S7]

### 5.2 Pending inheritance matrix

Apply inheritance inside the scheduling lock before replacing `pending`:

| Incoming request | Existing pending request | Required result |
|---|---|---|
| Contains a freshly copied eligible payload | Any | Use incoming payload |
| Curve-only | Has payload, same sample rate and current registration | Transfer/copy the immutable lease and its analysis sequence |
| Curve-only | Has payload, different sample rate | Do not inherit it |
| Curve-only | No payload | Remain curve-only |
| Any | Different/unregistered display | Never share pending state |

Transfer the payload's analysis sequence with its lease. Never claim a newer analysis sequence merely because a preview request was newer.

This proposal preserves the normal single-UI-producer ordering rather than inventing an ordering protocol for arbitrary competing producers. If any existing caller can submit older payloads after newer ones, characterize that caller and define a wrap-aware policy before extending the service.

### 5.3 Completed overlay carry-forward

Pending inheritance and completed carry-forward are different operations.

When claiming a curve-only request, capture the last completed snapshot for that same registration. Outside the mutex, retain its overlay arrays, dynamic top reference, and represented analysis sequence only when sample rates match.

This is required for this sequence:

```text
A: analysis job is already claimed
B: newer curve-only request becomes pending
A: completes with an overlay
B: is claimed and carries A's completed overlay forward
```

Submission-time inheritance alone cannot cover that case.

If no compatible completed overlay exists, leave overlay validity false. Do not fabricate a represented FFT sequence or carry old-rate arrays into the new axis.

### 5.4 Reuse of a completed curve

Reusing an unchanged curve requires both the matching preview sequence and a compatible sample rate within the same live registration. Do not let an equal sequence number bypass sample-rate validation.

Set `skipCurvePrep` only as the result of that validated reuse decision. Do not trust a bare caller-provided skip flag to establish validity without an initialized compatible curve target.

Retain the latest request's preview metadata in the result, even when its numerical curve is reused. Metadata and the visible curve must remain coherent.

### 5.5 Epochs without audio-publication redesign

Use a new registration ID to invalidate worker work when the display changes module binding, the observed analysis generation changes, or the input sample-rate domain changes. No separate globally reusable widget-address key is acceptable.

A UI client may keep its currently bound module identity, analysis generation, and sample rate as UI-owned validity metadata. Do not add a new audio-side generation scheme. Reuse the already-published generation and existing sample-rate information.

Read/check validity at UI handoff boundaries. If the generation changes while copying inputs, discard that candidate request and re-enter synchronization on the next bounded UI pass. Do not spin until the audio thread becomes stable.

This does not make independently published preview and FFT inputs an atomic pair. The plan does not authorize adding sample-rate metadata to audio frames or a new publication protocol. Preserve the eligibility guarantees actually provided by the existing APIs, validate request/previous-snapshot rates and observed generation, and do not claim stronger cross-stream temporal consistency. If characterization reveals an audio-publication gap that cannot be solved at the UI boundary, document it as a separately scoped issue rather than silently changing the callback.

## 6. UI handoff ownership

### 6.1 Introduce `BifurxRenderClient`

Add a small, UI-thread-confined composition object, provisionally `BifurxRenderClient`. It owns:

- The service reference and current display ID.
- Request numbering and last accepted preview/analysis submission bookkeeping.
- Last applied worker request/preview/analysis bookkeeping and snapshot cache.
- The three payload slots, pool cursor, and submit timing.
- Registration acquisition/release and admission-failure reporting.

It must not own the module, perform GL calls, decide Rack viewport visibility, implement filter math, or become a second copy of `BifurxSpectrumState`.

Keep the client noncopyable/nonmovable initially. Make service injection possible for deterministic tests; production binds to `bifurxRenderService()`.

Illustrative interface shape:

```cpp
class BifurxRenderClient {
public:
    explicit BifurxRenderClient(BifurxUiRenderService& service);
    ~BifurxRenderClient();

    bool ensureRegistered();
    void release();
    std::shared_ptr<BifurxUiRenderPayload> tryAcquirePayload();
    bool submit(BifurxUiRenderRequest request);
    std::shared_ptr<const BifurxUiRenderSnapshot> pollLatest() const;

    // Narrow read-only accessors for sequence state and existing metrics.
    // Adoption acknowledgement belongs here; target-array mutation does not.
};
```

Finalize exact signatures after moving the existing methods. Prefer passing small values/references over a generic callback framework. Do not add a heap-allocated client per frame or a mutex around UI-local counters.

### 6.2 One release implementation

All paths delegate to the same client release operation: worker OFF, inactive backend, hidden display, module rebinding, generation/rate invalidation, and destruction.

Release must exchange the live ID to zero, unregister that ID at most once, reset client request/adoption/submission metadata, drop snapshot ownership, reset pool entries/cursor, and clear worker-specific diagnostics.

Repeated release of an already-clean client must be a no-op. A test-created client with lazily allocated payload slots but no registration still needs its local resources cleaned; do not use `id == 0` as the sole proof that no resources exist.

Releasing worker ownership is not the same as clearing valid displayed state. Preserve the current curve and compatible overlay on a worker-mode or renderer transition. Generation/rate/module invalidation may require clearing presentation validity; keep that reason-specific policy in the display controller, not hidden inside client release.

### 6.3 Centralize presentation activity before child stepping

The module widget computes existing effective visibility/window/viewport state and the selected backend. Before calling child `step()` methods, provide each spectrum base with whether it is the active presentation owner.

Only the visible selected backend may acquire/retain a worker registration. Inactive/hidden backends release through the client and do not perform synchronous target preparation just because they are being stepped by a parent.

Preserve the existing audio-analysis subscription and heartbeat/watchdog mechanism independently of worker offload: a visible synchronous renderer still needs published analysis. Do not make subscriber count depend on worker ON/OFF.

Preserve browser/no-engine preview behavior. The private module used to position/configure browser controls is not authorization to run a live render worker or audio subscription.

### 6.4 Recommended tick order

1. Resolve presentation activity and module binding.
2. Observe existing analysis generation and preview/sample-rate validity; release/invalidate once when necessary.
3. Read published preview state and observe the analysis publication sequence using the existing APIs.
4. Resolve the current worker policy, preserving OFF/AUTO/ON/INHERIT behavior.
5. If active and worker-desired, establish registration. If start/registration fails, use synchronous preparation for this tick.
6. On the synchronous path, prepare any latest preview or analysis not represented by the current targets.
7. On the worker path, submit changed inputs with an eligible payload lease when available; advance accepted-submission metadata only after success.
8. Poll and adopt a valid new completed snapshot for this registration.
9. Apply presentation-only settings, then animate toward the installed targets.
10. Report `contentChanged`, including the final settling frame, and invalidate backend caches only as required.

Keep the current order where practical during extraction. Any changed order must be justified by a named regression test, particularly around dynamic scale and snapshot adoption.

### 6.5 Synchronous catch-up, including rejected offload

When worker mode turns off, do not wait for its current job or for new audio. Compare observed preview progress with the preview represented by the curve, and observed/published analysis progress with the analysis represented by the overlay. Re-read the latest eligible frame through the existing publication API when necessary.

This must work even if the UI already observed and submitted that frame but never adopted a worker result. A successful synchronous copy advances the represented sequence to the actual copied sequence, not a guessed publication counter.

A transient copy failure or pool exhaustion must not mark analysis as handled. Retry on a later UI tick without a blocking loop. If no valid frame exists after generation/rate invalidation, show the correct curve and an invalid/cleared overlay until an eligible frame exists.

Start/registration/submission failure must not leave the display stuck in an assumed worker path. For rejected submissions, do not advance accepted-submission counters; release/reconcile the invalid session and synchronously catch up when the UI is still active. During terminal plugin teardown, the service must reject new registrations/submissions. Rejection must not schedule additional render ticks or blocking retries; any already executing UI callback remains subject to the host/widget lifetime rules.

### 6.6 Snapshot adoption predicate

Adopt only if:

```text
client is registered and presentation is active
snapshot exists
snapshot.displayId == current client.displayId
snapshot.requestSeq > last applied request sequence
snapshot belongs to the currently valid sample-rate/generation binding
snapshot has a valid curve target
```

Do **not** require `snapshot.requestSeq == latestSubmittedRequestSeq`. An intermediate completion must remain useful under sustained updates. [P, lines 19–21, 45–49]

Install curve arrays and matching displayed-preview metadata together. For analysis-only updates with an unchanged curve, do not restart its interpolation or invalidate marker/refinement geometry unnecessarily. Update overlay validity/sequence only when a compatible overlay was actually installed.

A snapshot handle obtained before unregister can remain alive as an immutable C++ value. Unregister cannot revoke external shared ownership. The contract is that the service no longer exposes it and the UI refuses to install it into a released/new registration.

### 6.7 Graphics context lifecycle is separate

A context loss invalidates GL/NanoVG resources, not necessarily a CPU snapshot's mathematical validity. Do not re-register on every framebuffer resize, zoom change, or shader-cache rebuild.

When a host window closes and the display becomes inactive or is destroyed, release through the normal UI lifecycle. On reopening, obtain a fresh registration if the prior one was released, rebuild context resources through existing helpers, and catch up normally. Do not introduce worker-side context ownership.

### 6.8 Preserve additional visual contracts during extraction

Keep the dynamic FFT-top reference available even while fixed scaling is selected, so scaling toggles need no new audio. Keep input-spectrum response preparation wherever it currently affects normal spectrum coloring, even when the optional response line is hidden. Preserve first-target initialization, subsequent interpolation, curve revisions, marker metadata, and the final dirty settling frame. [S1, S4]

These are parity constraints, not invitations to retune the visuals.

## 7. Deterministic characterization and regression tests

### 7.1 Test organization and scheduling controls

Start the new coverage in `tests/bifurx_runtime_spec.cpp`, as the plan requests. Preserve the existing coalescing, pool, synchronous fallback, numerical, and audio tests. Do not move the whole harness at the same time as changing service behavior.

Add a narrow test seam around the real service, compiled only for tests. A suitable implementation uses a test peer and optional scheduling hooks for **after claim** and **before publication**, plus observable claim/publication/discard events. Hooks that wait must execute outside the service mutex. Keep `prepareCurveSnapshot()` real in these tests; control scheduling rather than replacing the work with a fake result.

Use condition variables or an equivalent gate to establish the interleaving. A test must know that a job has been claimed before unregistering it. Sleeping for a guessed number of milliseconds is not a substitute.

A test peer may provide read-only inspection by copying a small diagnostic view under the service mutex: execution state, terminal latch, registered IDs, phases, pending sequences, queue contents, and publication/discard counters. The only additional mutation seams should be tightly scoped test-only setup, such as placing a stopped service at the ID-exhaustion boundary or injecting thread-start failure. Do not expose arbitrary mutable scheduling internals or public debug endpoints. Compile any layout-affecting test macro consistently in every translation unit of the test executable.

The fixture must release every blocked hook before stopping/joining, including assertion-failure and exception paths. A subprocess/test-runner deadline is the final safeguard against a genuinely broken join. Per-wait deadlines are hang guards, not performance assertions. Keep service lifetime longer than client lifetime, and keep hook/gate storage alive until the worker has joined.

The current runtime harness has a plain test debug flag. Set such test-global flags before starting a service and restore them only after all affected workers have joined; do not introduce a test-only data race while testing lifecycle safety. [S7]

### 7.2 Required service test matrix

The names below are proposed test names; existing tests may be extended when that makes the scheduling and assertions clearer.

| ID / proposed test | Deterministic setup | Required assertions |
|---|---|---|
| W01 `registrationRequiresRunningService` | Use a fresh local service, then start it | Before start: registration is zero and submission rejected. After start: valid nonzero ID; repeated start succeeds without a second worker |
| W02 `onePendingRequestPerDisplay` | Hold A in flight; submit many updates for A | One claimed job, one pending request containing the newest accepted update, no queue entry for the in-flight A |
| W03 `queuedReplacementKeepsPosition` | Hold A; queue B then C; replace B repeatedly | B retains its queue position; no duplicate B entry; B's claimed request is its latest replacement |
| W04 `unregisterQueuedDisplay` | Hold A; queue B with an owned payload; unregister B | B disappears from queue/map, polling B returns none, pending ownership is released, and B is never claimed |
| W05 `unregisterInFlightDisplay` | Hold A immediately before publication; unregister A | A may finish, but no result is published/exposed for A; a fresh ID is different and receives only its own work |
| W06 `retainedSnapshotCannotCrossRegistration` | Poll and retain A's snapshot; unregister; reuse the same UI client/object with B | Old shared value can stay alive, but cannot be adopted under B; service lookup for A returns none |
| W07 `stopDiscardsPendingAndInFlightWork` | Hold A in flight, with A pending and B queued; call stop on another thread | Observe Stopping; start/register/submit reject; lookup returns none; release A; stop returns only after join; pending work never starts |
| W08 `restartKeepsIdsUnique` | Complete W07; start/register/submit again | New ID exceeds prior IDs; only new work appears; repeated stop/start/stop cycles finish cleanly |
| W09 `concurrentStopWaitsForJoin` | Hold A; first stop owns join; launch second stop | Neither stop reports completion before A is released and the worker exits; no deadlock or double join |
| W10 `terminalShutdownDuringStop` | First caller is performing restartable stop; second calls shutdown | Terminal latch becomes set; both return after join; future start/register/submit reject permanently |
| W11 `localShutdownIsIsolated` | Shutdown one local service; start another local service | Second service operates normally; no test accidentally poisons a process-global gate |
| W12 `queueBoundedDuringDisplayChurn` | Hold A; repeatedly register, submit, and unregister many other displays | Queue length is bounded by live registrations, not historical registrations; destroyed displays do not retain pending leases |
| W13 `floodedDisplayDoesNotStarvePeer` | Hold A; flood its pending updates; queue B; release A and continue flooding | B is claimed before A's requeued pending job; B completes while A continues receiving updates; all results have correct IDs and increasing per-display request sequences |
| W14 `pendingCurveOnlyInheritsMatchingPayload` | Hold another display; queue analysis request for A; replace with a curve-only request at the same rate | Payload identity and its analysis sequence survive; rendered targets correspond to that payload, with the newer preview |
| W15 `pendingPayloadDoesNotCrossRate` | Repeat W14 with a different incoming preview sample rate | Old payload is not inherited; old-rate overlay is not reported valid under the new rate |
| W16 `curveOnlyCarriesCompletedPredecessor` | Hold analysis job A1; submit curve-only A2; complete A1, then claim A2 | A2 retains A1's compatible completed overlay and analysis sequence, even though A1 was not completed at A2 submission time |
| W17 `completedCarryForwardRejectsRateMismatch` | Complete an overlay, then submit a curve-only request at another rate | No completed overlay carry-forward; axis and curve use the new rate |
| W18 `equalPreviewSequenceDoesNotBypassRateCheck` | Deliberately reuse a preview sequence with another sample rate in a local-service test | Reuse is rejected; new valid curve/axis targets are calculated, rather than retaining the old-rate curve |
| W19 `ownershipRetiresOutsideServiceMutex` | Use instrumented test leases/retirement observations | Final payload/snapshot retirement does not occur inside the scheduling lock; tests never depend on a destructor reentering production APIs |
| W20 `startFailureLeavesStoppedService` | Use a narrow test-only thread-start failure injection | Start reports failure; no joinable orphan or accepted dormant work; UI can choose synchronous fallback |
| W21 `shutdownWinsAgainstLateAdmission` | Hold registration/submission callers immediately before their admission lock, without holding the service mutex; complete terminal closure; then release callers | Neither operation admits work after the terminal linearization point; a pre-lock fast-path result cannot bypass the authoritative check |
| W22 `displayIdExhaustionNeverWraps` | Use a test peer to place the ID allocator at its exhaustion boundary while stopped, then start/register | Exhaustion rejects registration without issuing zero, wrapping, or reusing an earlier ID |

For W13, “B eventually completes after A stops flooding” is insufficient. The schedule must demonstrate fairness while A remains active. Correctness rests on claim order and ownership, not a tight timing threshold.

Use actual prepared targets for selected W14–W18 assertions, not only sequence fields. For example, preserve the existing known-gain analysis fixture and verify its expected measured response. Validate every relevant curve point/overlay entry where practical, including sample-rate-sensitive axis values.

### 7.3 Required UI handoff tests

Test the render client with a local service, and the display controller with real publication and preparation. Visibility selection can be exercised with a lightweight activity coordinator before running the real GL integration test.

| ID / proposed test | Required assertion |
|---|---|
| U01 `workerOffCatchesUpWithoutNewAudio` | Publish preview and analysis; submit without adopting; disable offload; run one successful synchronization tick without new audio; latest published inputs are represented synchronously |
| U02 `intermediateCompletionIsAdopted` | Submit N, then N+1; complete N while N+1 is withheld; UI installs N rather than waiting for equality with latest submission; N+1 can subsequently replace it |
| U03 `releaseIsIdempotent` | Repeated release unregisters a live ID exactly once, resets counters/diagnostics/pool ownership, and preserves valid displayed targets unless explicitly invalidated |
| U04 `hiddenBackendCannotReregisterDuringChildStep` | Parent marks one/both backends inactive before child stepping; inactive child steps cannot acquire registrations or perform hidden synchronous catch-up |
| U05 `rapidBackendAndWorkerTransitions` | Alternate NanoVG/GL activity and OFF/ON/AUTO/INHERIT while preview and FFT advance; only the active eligible client is registered; old IDs never supply new targets |
| U06 `generationResetInvalidatesSameRateResults` | Reset existing analysis generation without changing sample rate; an old job completed afterward cannot restore pre-reset overlay data |
| U07 `moduleRebindUsesNewIdentity` | Reuse the same UI object for another module binding; acquire a new ID and prevent adoption from the previous module |
| U08 `poolExhaustionRetriesUnrepresentedAnalysis` | Pin all three producer leases; observe newer analysis; submit curve-only when useful; free a lease; a later tick copies/submits the outstanding FFT without requiring another audio frame |
| U09 `rejectedSubmissionDoesNotAdvanceAcceptedState` | Force stopping between preparation and admission; no accepted-analysis/preview counter advances; active UI takes synchronous fallback rather than freezing |
| U10 `registrationFailureUsesSynchronousPath` | Reject start/registration through a local stopped/stopping/terminal service; preserve active display behavior without a wait on worker completion |
| U11 `workerReleaseDoesNotUnsubscribeVisibleAnalysis` | Visible synchronous mode keeps the existing analysis subscription/heartbeat contract; hiding/removing a module still releases it through the existing owner |
| U12 `browserPreviewDoesNotStartWorker` | Browser/no-engine preview renders its authored preview without accidentally registering the private control-preview module as a live display |
| U13 `analysisOnlyUpdatePreservesCurveAnimationCaches` | Analysis progress alone does not restart the unchanged curve or discard marker/refinement caches unnecessarily |
| U14 `scaleToggleUsesStoredDynamicReference` | Toggle fixed/dynamic FFT scale without new audio or worker work; targets use the retained dynamic reference and current selected scale |
| U15 `lastAnimationFrameInvalidatesDisplay` | The final approach/settling step marks content changed where required; neither backend freezes one frame before the final target |

Where “one tick” appears, prepare a stable valid publication so the existing bounded copy succeeds. Do not demand that an unsuccessful concurrent publication copy block until it succeeds.

### 7.4 Preserve the existing reference tests

Retain and extend, rather than replace, these current tests:

- `testWorkerAnalysisFramePoolIsBoundedAndReusable`.
- `testSynchronousFftScratchIsLazyAndThreadLocalReusable`.
- `testWorkerLatestRequestRetainsOwnedAnalysisPayload`.
- `testDisablingWorkerCatchesUpPendingPreviewAndAnalysis`.

Keep all current filter, reset, modulation, output-stage, scale, preview, and numerical coverage. Test name/location changes must not silently drop cases from the runner. [S7]

For target parity, compare the same preview, FFT frame, sample rate, and prior smoothing state. Exact equality is appropriate for pure data moves. For recomputation, use existing justified numerical tolerances or document a new tolerance from the computation being tested. Do not demand identical time histories between a coalescing worker and a synchronous path that processed every intermediate frame; instead compare equivalent preparation inputs and preserve each path's established smoothing semantics.

### 7.5 Audio-thread isolation checks

No production worker instrumentation belongs in `process()`. Review the audio call graph and preserve existing audio regression outputs under identical deterministic inputs.

An optional test-only thread-local allocation guard may track allocations made by the test's audio callback after normal initialization. It must ignore allocations on UI/worker threads and must not globally forbid the intentional lazy FFT/pool allocation. Pair that guard with a service-access assertion or test counter to prove the callback never enters the service. This supplements code review; it does not replace existing numerical tests.

## 8. Source architecture and file migration

### 8.1 Direction: smaller ownership boundaries, not arbitrary line quotas

Keep the first implementation in flat `src/Bifurx*.hpp/.cpp` files. A later `src/bifurx/` directory is reasonable, but directory relocation is not needed to fix ownership or reduce source concentration. Flat new `.cpp` files fit the current root source wildcard; nested files would need additional production, test, and export configuration. [S9, S12]

Do not create a giant `BifurxInternal.hpp` that re-exports the old dependency graph. Do not split hot audio code into many translation units merely to hit a line-count target. Favor one reason to change per unit and an acyclic include graph.

### 8.2 Recommended target layout

The following is the intended architectural destination, implemented in stages. Existing DSP helper headers remain unless a specific dependency requires a change.

| File or group | Responsibility after migration | Migration priority |
|---|---|---|
| `BifurxTypes.hpp` — new | Shared constants and value-only preview/analysis/model data needed across audio/display/preparation; no widget ownership | During header split |
| `BifurxModule.hpp` — new | Rack module declaration, parameter/port enums, audio state, publication interface | During header split |
| `Bifurx.cpp` — retain | Audio processing, publication implementation, and tightly coupled hot DSP helpers | Keep hot path together |
| `BifurxState.cpp` — new | Construction/configuration, serialization, non-hot module setup and settings methods | After client/display extraction |
| `BifurxPreview.hpp/.cpp` — new | Preview model construction, analytical response and preview-only probe helpers | After tests; verify helper sharing |
| `BifurxDisplay.hpp/.cpp` — new | `BifurxSpectrumBase`, synchronous preparation, target adoption, display state/animation and its current CPU caches | First substantive extraction |
| `BifurxRenderClient.hpp/.cpp` — new | UI-side registration, request bookkeeping, pool leases, polling and worker metrics | First substantive extraction |
| `BifurxRenderData.hpp` — retain | Request, payload and immutable snapshot contracts; include narrow value headers | Narrow dependencies |
| `BifurxWorker.hpp/.cpp` — retain | Single-worker scheduling, lifecycle, coalescing and claim-time carry-forward | Harden before moving callers |
| `BifurxRenderPrep.hpp/.cpp` — retain | Numerical curve/FFT target preparation and existing shared overlay-target helper | No algorithm rewrite |
| `BifurxDebug.hpp/.cpp` — new | Curve/performance recorder helpers, output formatting and diagnostic storage | Later UI structural pass |
| `BifurxSpectrumNanoVG.hpp/.cpp` — new | NanoVG spectrum widget and directly related draw helpers | Later UI structural pass |
| `BifurxUI.cpp` — retain | Module-widget composition, panels/menus, activity coordination, subscription ownership and whole-module timing boundaries | Keep integration policy visible |
| `BifurxGL.cpp` — retain | GL widget orchestration and context-bound resources/lifecycle | Avoid simultaneous rewrite |
| `BifurxGLShaders.hpp/.cpp` — optional later | Named shader sources without resource ownership | First low-risk GL size reduction |
| `BifurxGLGeometry.hpp/.cpp` — optional later | Pure CPU mesh construction only, if profiling and coupling justify extraction | Do not split solely for line count |
| `Bifurx.hpp` — transitional | Compatibility umbrella while callers migrate to precise headers | No new code; shrink or retire after audit |

Do not require every optional file to exist. In particular, shader and geometry extraction is a separate structural follow-up, not a prerequisite for lifecycle correctness.

### 8.3 Mechanical extraction order and boundaries

**First move display implementation out of `Bifurx.cpp`.** Move the spectrum-base methods, their synchronous scratch helper, target preparation adapter, browser display initialization, animation, and existing marker/refinement implementation into `BifurxDisplay.cpp`. Initially keep declarations compatible. This removes display responsibilities from the audio-heavy source without changing execution order.

**Then move worker-client state and operations.** Transfer `ensureWorkerRegistration()`, release, payload acquisition, submission bookkeeping, snapshot polling and timing ownership to `BifurxRenderClient`. Keep target-array installation, latest/displayed-preview distinction and animation in the display controller. Introduce wrappers temporarily where this keeps the migration reviewable; remove redundant wrappers after both backends have migrated.

**Then split declarations and preview/setup code.** Extract shared value types before making `BifurxRenderData.hpp` stop including the full module/display header. Move module declarations to `BifurxModule.hpp`. Preserve existing public names, enum values and namespace aliases where required by tests/UI. Move preview-only model/probe functions to the preview unit. Move non-hot module setup/JSON methods to the state unit only after confirming which anonymous helpers they use.

**Finally separate UI diagnostics and NanoVG presentation.** Keep module-wide timing start/end at their original enclosing operations, regardless of where recorder implementation lives. Expose a narrow spectrum factory or interface rather than forcing `BifurxUI.cpp` to see backend-internal implementation details.

When a helper is used by both DSP and preview, do not casually turn a previously inlined local helper into a cross-translation-unit call in the audio hot path. Keep genuinely shared small mathematical helpers in a narrow inline header, or retain them with the hot implementation and move only preview-specific work. Record any changed linkage/inlining in the performance review.

### 8.4 Intended dependency direction

```text
A -> B means that A may depend on B.

BifurxRenderData    -> BifurxTypes
BifurxModule        -> BifurxTypes + existing DSP helper headers
Bifurx.cpp / State  -> BifurxModule
BifurxPreview       -> BifurxTypes + narrow mathematical helpers
BifurxRenderPrep    -> BifurxRenderData + BifurxPreview
BifurxWorker        -> BifurxRenderData + BifurxRenderPrep
BifurxRenderClient  -> BifurxWorker + BifurxRenderData
BifurxDisplay       -> BifurxModule + BifurxRenderClient + preview/preparation
NanoVG / GL / UI    -> BifurxDisplay + BifurxModule
```

These are allowed dependencies, not a requirement that every edge become a direct header include. Use forward declarations where complete types are unnecessary. Keep the low-level value types independent of the module, service, client and widgets.

Hard constraints: worker service/data headers do not include widget declarations, GL APIs, or the compatibility umbrella; audio module declarations do not own a render client; the renderer never owns an audio publication slot; the client has no backend dependency. `BifurxRenderPrep.cpp` may continue using Rack's FFT support—this is not a project to remove all Rack dependencies.

### 8.5 GL preservation rules

Retain the current distinction between step-time fixed-surface work and draw-time consumption. Do not move fixed-surface rendering into `draw()` while making a cosmetic source split. Preserve dirty tracking, adaptive surfaces, resource retirement, context-generation checks, shader setup, buffer reuse and fallback behavior. The retrieved GL widget deliberately avoids unconditional dirty behavior in its stepping path. [S6]

Extract shader strings first if a low-risk reduction is desired. Keep ownership-sensitive GL setup/teardown together until the lifecycle tests and real GL test are green. A source split must not create shared mutable global GL resources or make resources outlive their owning context.

### 8.6 Test linkage migration

The runtime and premium-license tests currently include `Bifurx.cpp`, and the GL test includes the runtime harness plus `BifurxGL.cpp`. [S7, S8, S13]

During early behavior changes, preserve that harness and add only the new separately compiled sources it needs. Maintain an explicit list so each implementation is present exactly once. Do not simultaneously include and compile an extracted source.

During the structural phase, migrate toward conventional linkage:

```text
tests/support/bifurx_test_runtime.hpp/.cpp
    shared fixtures, plugin stubs, initialization, reusable assertions

tests/bifurx_runtime_spec.cpp
    test cases + its own main

tests/bifurx_gl_spec.cpp
    GL fixture + its own main, not another test's main

tests/bifurx_license_spec.cpp
    premium-specific stubs/setup + its own main
```

Keep the existing test entry-point filenames. Keep premium stubs separate wherever their DRM configuration differs. Replace implementation-local scratch access with a narrow test-only observation seam rather than exporting private helpers as general production API.

Define shared Makefile variables for the Bifurx numerical/module/display test sources and dependencies, with separate additions/flags for runtime, GL and premium configurations. Preserve generated dependency tracking, existing optimization flags and each target's defines. A common source list does not mean sharing binary objects compiled under incompatible DRM or test-hook macros.

### 8.7 Pro export and bootstrap integration

The Pro synchronizer has an explicit `SOURCE_FILES` closure. Add every new production source/header and remove retired entries in the same structural change. The premium validation preparation consumes that closure, so missing entries can break isolated validation even when the main plugin builds. [S12, S14]

Update the shutdown order in the main plugin. Also audit the Pro bootstrap template and the actual existing Pro-owned `src/plugin.cpp`: normal synchronization deliberately does not overwrite that bootstrap after setup. Updating a template alone therefore does not update an existing Pro installation's shutdown sequence. Do not silently overwrite Pro-owned metadata, DRM code or bootstrap files as a workaround. [S12]

Where the Pro checkout is part of the implementation workspace, make the corresponding small bootstrap change through its normal ownership workflow. Otherwise document that separate required change and do not claim Pro shutdown integration is validated.

Use `--dry-run`/`--check` to inspect export coverage without modifying the sibling repository. A nonzero `--check` caused by intentionally unsynchronized edits means drift, not automatically a broken script. Apply synchronization only as part of the user's authorized implementation workflow.

## 9. Metrics and performance acceptance

### 9.1 Preserve the public timing boundaries

`Process` continues to represent the module's audio processing. `Step` continues to represent the module's existing complete step work, including applicable step-time backend work. `Draw` continues to represent its existing module draw boundary. Do not make these numbers look better by moving the timing boundary inward or attributing renderer work only to a new field. [P, line 23; S6]

Keep worker preparation, FFT and submission diagnostics separately named. Recorder extraction may move formatting/storage but must not redefine the measured scopes.

### 9.2 Distinguish queue wait from end-to-end latency

**Observed:** `workerQueueLatencyMs()` currently calculates completed time minus request-submission time, which includes worker preparation; it is not an isolated queue-wait measurement. The UI submission timestamp is recorded before its analysis-copy work. `workerSnapshotAgeMs()` also has a specific existing stale-snapshot condition rather than being an unconditional age of every cached snapshot. [S1]

Preserve compatibility of existing diagnostic fields in this refactor. Add clearly named fields rather than silently changing an existing CSV/debug field's interpretation:

| Proposed diagnostic | Definition |
|---|---|
| `workerSubmitMs` | UI submission/preparation scope currently measured; state precisely whether analysis copy and mutex acquisition are included |
| `workerMutexWaitMs` | Instrumented service-lock acquisition delay for the relevant UI operation |
| `workerMutexHoldMs` | Time spent inside that operation's scheduling critical section |
| `workerQueueWaitMs` | Worker claim time minus service acceptance time of the final coalesced request |
| `workerPrepareMs` | Completion time minus worker preparation start |
| `workerEndToEndMs` | Completion time minus original request submission timestamp |
| Existing snapshot-age field | Retain its current meaning; add a differently named unconditional age only if useful |

A replacement has a new acceptance timestamp while preserving queue position. Define queue wait against the final accepted request, not against the first superseded request's timestamp. Use one consistent monotonic time source across comparable timestamps. Keep timestamps in a compact request/snapshot envelope and preserve the request-size assertion.

Diagnostic instrumentation must follow existing debug/performance enablement, avoid file I/O under the service mutex, and never add clocks or counters to the audio callback. Detailed queue/phase/lease counters can remain test-only unless production evidence requires them.

### 9.3 Measurement protocol

Record baseline and candidate using the same pinned code environment, Rack version, sample rate, buffer settings, compiler/options, debug configuration, patch, viewport, zoom and display hardware. Separate cold initialization from warmed steady state so lazy FFT creation does not contaminate normal submission comparisons.

Use at least these scenarios: one visible module, several visible modules, and a heavier multi-module case; NanoVG and OpenGL; worker ON and OFF; static and continuously changing parameters; steady FFT input and active audio; rapid visibility/backend changes. Include the sample rates already supported by the existing suite, with ordinary 44.1/48/96 kHz interactive checks where available.

Record distributions, not only averages: median and upper-tail module Step/Draw, submit time, mutex wait/hold, actual queue wait, preparation time, snapshot age, and pool-acquisition misses. Record the number of samples and how debug output was collected.

The acceptance rule is **no reproducible regression outside measured baseline variability** in normal rendering or UI service contention. Re-run an apparent regression under controlled conditions; do not dismiss it solely because it is below an arbitrary percentage. Lifecycle correctness tests must never pass/fail based on these benchmark timings. If deterministic correctness improves but a normal workload slows measurably, optimize or revise the refactor before accepting it. [P, lines 53–55]

## 10. Build, integration, and manual validation

### 10.1 Native build workflow

Use the checkout's documented native MINGW64 workflow for the authoritative Windows result. A Linux/WSL `.so` or syntax-only check does not validate `plugin.dll`. The repository documentation identifies native MINGW64 and its fast suite as the relevant workflow. [S11]

From the native environment, record the actual environment and then run the configured targets. The example runtime path is from the repository documentation; retain the checkout's actual SDK/configuration rather than inventing a new one.

```sh
git status --short
git rev-parse HEAD
printf 'MSYSTEM=%s\n' "$MSYSTEM"
g++ -dumpmachine

make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10
```

Verify that the resulting native `plugin.dll` was actually rebuilt from the candidate sources and record its path/timestamp or hash. Do not report a stale artifact as an authoritative successful build. Use incremental builds normally; investigate dependency errors instead of automatically using a clean build to hide them.

Run the existing separate Bifurx GL target in an environment with its real context requirements satisfied:

```sh
make test-bifurx-gl RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
```

Confirm exact target names against the local Makefile before execution. If SDK/runtime, graphics context, native toolchain, or DRM prerequisites are unavailable, record the blocked validation precisely. Do not replace it with a weaker test and label it equivalent.

Run the premium validation targets when available, particularly after changing source closure or module-header organization. Keep the real DRM configuration and existing negative-license coverage intact. Do not disable licensing to make a structural test build pass.

For a configured sibling Pro checkout, a non-mutating export audit is:

```sh
python3 tools/sync_bifurx_to_pro.py --dry-run
python3 tools/sync_bifurx_to_pro.py --check
```

Interpret drift as described in Section 8.7. Verify one-definition/linkage correctness for the module, preview helpers and factories in all affected targets; do not assume the Linux-only spelling of an existing ODR target validates Windows.

### 10.2 Manual Rack matrix

| Scenario | Check |
|---|---|
| Rapid add/remove of Bifurx | No accumulating queue/pool ownership, stale curve flash from another module, or teardown hang |
| Window close/reopen and minimize/restore | Inactive displays stop requesting; reopening rebuilds graphics resources and catches up without stale registration adoption |
| Repeated NanoVG/OpenGL switching | Only the selected visible backend submits; no frozen curve or overlay; existing presentation settings preserved |
| Repeated worker OFF/ON/AUTO transitions | OFF immediately catches up from published state without waiting for new audio; ON resumes without discontinuous invalid metadata |
| Several visible modules with one heavily modulated | Other displays continue receiving results; queue does not become an update backlog |
| Sample-rate changes and module reset | No old-rate axis/overlay or pre-reset worker result is restored |
| Fixed/dynamic FFT-scale and response-line toggles | Stored reference/response behavior remains correct without requiring fresh audio solely for a presentation toggle |
| Browser preview | No unnecessary worker startup or live-analysis subscription; authored preview remains intact |
| Plugin/host shutdown with active work | Worker joins before relevant state destruction; late UI callbacks cannot admit work |

The test fixture may force scheduling for automated cases, but do not ship artificial delays in production to make manual testing easier.

### 10.3 Required implementation report

Report: starting SHA and candidate revision/diff; files moved versus behavior changed; exact commands and outcomes; new test coverage; baseline/candidate metrics; native artifact identity; manual scenarios exercised; Pro export/bootstrap status; and any blocked checks.

Use separate statuses such as **passed**, **failed**, **not run**, and **blocked**. “Builds” is not evidence that lifecycle interleavings or visual parity passed.

## 11. Execution phases and acceptance gates

### Phase 0 — Pin and characterize

Record the local revision and existing dirty work. Inventory call sites for registration, release, stop, shutdown, activity, worker metrics and all tests/build/export references. Run the baseline fast suite and native build; collect baseline rendering metrics. Confirm current `RenderData` fields and sample-rate/generation semantics against the checkout.

**Gate:** Source/test/build inventory is recorded. Baseline failures are distinguished from new regressions. No broad edits have begun.

### Phase 1 — Lifecycle tests before cleanup

Add scheduling hooks and the service/UI characterization cases from Section 7, retaining existing coverage. Tests that expose an intended contract gap may initially fail; identify those failures explicitly rather than weakening their assertions. Keep production behavior unchanged apart from test observability.

**Gate:** Interleavings can be reached deterministically; timeouts only guard hangs; current-contract tests run and known gaps are named.

### Phase 2 — Harden the service in place

Implement locked admission, explicit lifecycle state, terminal shutdown, complete stop/join semantics, bounded queue removal on unregister, and claim/prepare/publish ownership. Introduce minimal boolean admission results. Simplify slot flags only after tests cover the transitions. Preserve the one-worker policy, pending inheritance and completed carry-forward. Change main-plugin shutdown order and record Pro bootstrap implications.

**Gate:** Service tests pass; no new audio-side changes; all computation and joining are outside the scheduling lock; latest snapshots remain immutable and useful.

### Phase 3 — Extract display/client and consolidate UI handoff

First move spectrum-base implementation mechanically; then extract client ownership; then route backend/visibility/generation release through one implementation. Keep these as distinguishable diffs or commits in the user's normal workflow. Implement synchronous fallback for rejected offload and the explicit intermediate-snapshot adoption rule. Update production/test/export source lists with each move.

**Gate:** UI tests, existing fast suite and authoritative native build pass; both backends retain output/animation behavior; initial performance comparison shows no regression.

This is the first complete delivery milestone: lifecycle hardening plus a substantial reduction in `Bifurx.cpp`'s mixed responsibilities.

### Phase 4 — Complete the header/UI architectural split

Introduce narrow types/module/preview headers, move non-hot setup/serialization and UI diagnostics/presentation, and regularize test linkage. Remove transitional duplicate responsibilities and umbrella includes from worker/data paths. Validate premium source closure and separate Pro-owned bootstrap status.

**Gate:** Dependency direction is enforceable, affected targets link each definition exactly once, full native/GL/premium checks are passed or precisely marked blocked, and module-level timing boundaries are unchanged.

### Phase 5 — Optional GL structural reduction

Extract shader sources, and only then pure geometry where justified. Keep context lifecycle and step/draw placement intact. Folder relocation is a separate optional housekeeping change, not part of the lifecycle gate.

**Gate:** Real GL integration and manual context/renderer tests pass; no performance regression; no new resource-lifetime coupling.

### Phase 6 — Final acceptance

Repeat the fast suite, native build, relevant GL/premium tests, manual matrix and benchmark protocol on the final candidate. Compare against Phase 0. Deliver the implementation report and a short ownership/lifecycle comment near the service and client declarations.

Do not call the project complete while required native or manual checks are merely presumed. An unexecuted check is an explicit remaining validation item, not proof of correctness.

## 12. Definition of done and review checklist

The required lifecycle milestone is done when:

1. Audio behavior/publication and per-callback worker overhead are unchanged; no callback waits, requests or worker-driven allocation were added.
2. Every registration has bounded claimed/pending/queued work; queue storage is bounded during display churn; a flooded display cannot starve a queued peer.
3. Unregister and both kinds of shutdown close the specified ownership paths; no old ID/result is visible to a new registration; concurrent stop waits for the actual join.
4. Matching-rate payload inheritance and completed-overlay carry-forward pass target-level tests; mismatched-rate/generation results are rejected.
5. Intermediate snapshots can be adopted; worker OFF and admission failures catch up synchronously from existing published inputs without waiting for the worker or new audio.
6. One UI client owns each display's registration/pool/counters; visibility/backend changes cannot reacquire through an inactive child step.
7. Existing module Process/Step/Draw meanings remain intact; separately named worker diagnostics reveal rather than hide costs.
8. The agreed native build, fast/GL tests, Rack exercises and before/after measurements are documented with their actual results.

The architectural milestone additionally requires narrow header dependencies, updated test linkage and Pro export closure, no unexplained audio-hot-path linkage changes, and a clear account of optional work not performed.

### Frequent incorrect implementations to reject

Reject any solution that replaces coalescing with an unbounded job queue; uses widget addresses as IDs; resets the service ID allocator on restart; joins under the mutex; destroys worker implementation state before joining; advances accepted-analysis state after a rejected request; blocks the UI until its latest submission completes; drops every intermediate completion; equates observed FFT sequence with represented FFT sequence; clears a valid overlay on every worker toggle; silently moves work outside the published timing scopes; or relies on source moves alone as proof of lifecycle correctness.

## Appendix A — Source registry

Repository source basis, retrieved September 22, 2026. Paths are relative to the branch root below. These are branch-addressed references, not a substitute for recording a pinned implementation SHA.

```text
https://github.com/PlasmaChroma/Leviathan-Rack2/tree/expander
https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/expander/
```

| Ref | Source | Relevant material |
|---|---|---|
| P | User-provided `render-worker-lifecycle-plan.md`, 60 lines | Goal; required behavior; four work stages; scope control |
| S1 | `src/Bifurx.cpp` | `BifurxSpectrumBase::syncBase`, worker registration/release/pool/submit/adoption methods, synchronous overlay preparation, animation and preview/module implementation |
| S2 | `src/BifurxWorker.hpp`, `src/BifurxWorker.cpp` | `BifurxUiRenderService`, slot scheduling, start/stop, global shutdown, pending/previous-snapshot carry-forward |
| S3 | `src/BifurxRenderData.hpp` | Immutable lease request, payload, snapshot, metadata and request-size assertion |
| S4 | `src/BifurxRenderPrep.hpp`, `src/BifurxRenderPrep.cpp` | Worker scratch, curve preparation and shared `prepareOverlayTargetsFromSpectra` helper |
| S5 | `src/Bifurx.hpp` | Current combined declarations, publication types, spectrum state/base and module fields |
| S6 | `src/BifurxUI.cpp`, `src/BifurxGL.cpp` | Visibility/backend switching, subscription/heartbeat, debug timing scopes, NanoVG/GL widgets and context-bound rendering |
| S7 | `tests/bifurx_runtime_spec.cpp` | Existing runtime/coalescing/pool/fallback tests and source-inclusion harness |
| S8 | `tests/bifurx_gl_spec.cpp` | Real-context GL integration and source-inclusion coupling |
| S9 | `Makefile` | Source globs, runtime/GL/premium test recipes, flags/dependencies and native build targets |
| S10 | `src/plugin.cpp` | Global `destroy()` shutdown order |
| S11 | `AGENTS.md` | Repository's stated native MINGW64 validation workflow and module-level metric conventions |
| S12 | `tools/sync_bifurx_to_pro.py` | Explicit `SOURCE_FILES`, bootstrap templates, destination ownership and dry-run/check behavior |
| S13 | `tests/bifurx_license_spec.cpp` | Premium-specific configuration, direct module-source inclusion and existing license regression coverage |
| S14 | `tools/prepare_bifurx_premium.py` | Premium validation tree prepared from the synchronizer's source closure |

## Appendix B — Implementation handoff

Implement Phases 0–4 in order, preserving the behavior and acceptance gates in this document. Treat Phase 5 as optional and do not use it to delay a verified lifecycle milestone. Keep behavioral edits distinguishable from mechanical moves. Use the real local source revision, preserve unrelated working-tree changes, and report actual validation results rather than inferred success. Do not introduce a new scheduler, audio-publication protocol, or graphics framework under this task.
