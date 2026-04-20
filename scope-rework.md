# Scope Rework

## Problem

`TD.Scope` drag behavior is currently hard to tune because the interaction logic is split across two places:

- `TD.Scope` interprets the mouse gesture into lag motion, applies local shaping, and transmits a drag request.
- `TemporalDeck` interprets that request again and applies the actual tuned scratch behavior.

That creates two overlapping controller layers for a single interaction.

The result is predictable:

- position alignment is difficult to make exact
- responsiveness is difficult to tune cleanly
- live/sample/loop edge cases are harder to reason about
- scope drag can drift from platter drag behavior over time

## Current Architecture

### In `TD.Scope`

The current drag path in `src/TDScope.cpp` is doing more than a pure input device should:

- tracks drag anchor state
- maps cursor Y to lag space
- handles off-widget dragging
- applies jitter deadzone
- computes local filtered velocity
- performs local substep velocity shaping
- applies sample-loop wrap or clamp behavior
- transmits lag drag requests to `TemporalDeck`

### In `TemporalDeck`

The receiving path in `src/TemporalDeck.cpp` and realization path in `src/TemporalDeckEngine.hpp` already own the scratch behavior that has been heavily tuned:

- touch hold vs scratch state selection
- gesture freshness / motion hold behavior
- scratch target following
- live catch-up / write-head compensation
- loop playback realization
- interpolation selection
- final audio behavior

## Architectural Issue

The issue is not just implementation detail. It is a contract problem.

`TD.Scope` is acting partly like:

- a UI input device
- a scratch controller
- a transport realization layer

Only the first role belongs there.

`TemporalDeck` should remain the single authority for scratch behavior. That is where the tuning work already exists and where consistency with platter drag is maintained.

## Desired Architecture

### `TD.Scope` should be treated as a precise I/O device

Its job should be limited to:

1. determining the lag/time position the user is indicating
2. tracking drag continuity in UI space
3. emitting a clean request to `TemporalDeck`

### `TemporalDeck` should remain the behavior authority

Its job should continue to include:

1. scratch feel
2. motion response
3. smoothing behavior
4. live catch-up logic
5. loop realization
6. audio realization

## Recommended Contract

### Preferred conceptual contract

`TD.Scope` should send:

- a desired lag target
- optionally a raw gesture velocity estimate

`TemporalDeck` should decide how that target is realized.

### Stronger long-term contract

An even cleaner version would be:

- `TD.Scope` sends only desired lag target updates
- `TemporalDeck` derives all scratch response behavior from the stream of target updates

This would avoid duplicating velocity-shaping logic in the expander UI path.

## What `TD.Scope` Should Keep

The following responsibilities belong in `TD.Scope`:

- drag anchor bookkeeping
- scope-coordinate to lag-coordinate mapping
- live vs sample viewport interpretation for visible waveform mapping
- off-widget vertical continuation of the same lag-per-pixel mapping
- minimal jitter suppression needed for UI stability
- request transmission to the host module

## What `TD.Scope` Should Stop Owning

The following are suspect and should be reduced or removed where possible:

- local scratch-feel shaping
- platter-like motion logic duplicated in scope path
- local target rebasing against live lag
- local substep lag realization logic
- strong local velocity filtering intended to create scratch character
- loop realization policy if the host can own it cleanly

These are all ways `TD.Scope` becomes a second scratch controller.

## Current Symptom Interpretation

The recent tuning difficulties are consistent with this architectural split.

Examples:

- when position mapping is corrected, the feel still lags
- when the feel is tightened locally, the mapping can overshoot
- live mode introduces extra ambiguity because the scope viewport itself moves while dragging
- sample loop mode needs to match the host behavior exactly or boundary artifacts appear

These are not random bugs. They are symptoms of trying to solve the same control problem in two layers.

## Near-Term Refactor Direction

A practical near-term cleanup does not require a protocol redesign.

### Phase 1

Simplify `TD.Scope` to:

- compute target lag from cursor position
- keep drag anchoring stable
- send target lag updates with only minimal UI-side smoothing

Preserve the existing expander request protocol for now.

### Phase 2

Reduce local scope-side behavior shaping:

- minimize deadzone tuning to the amount needed for stationary holds
- reduce or remove local substep realization behavior
- reduce or remove local velocity shaping that is trying to produce scratch feel

### Phase 3

Let `TemporalDeck` more directly interpret the target stream:

- derive motion response centrally
- preserve parity with platter drag
- make scope drag another front-end into the same scratch behavior model

## Design Principles

1. One behavior authority
   `TemporalDeck` should be the single source of truth for scratch response.

2. Exact intent at the edge
   `TD.Scope` should focus on sending the correct target position.

3. No duplicate feel layers
   If both UI and host shape the same interaction, tuning will drift and become unstable.

4. Stable coordinate systems
   Scope drag should use a stable mapping while dragging; moving viewport feedback should be avoided.

5. Parity with platter behavior
   Scope drag should feel like a different input surface for the same deck behavior, not a separate scratch engine.

## Suggested Code Review Focus

### `src/TDScope.cpp`

Review every piece of logic after coordinate-to-lag conversion and ask whether it is:

- necessary UI stability work
- or duplicated behavior shaping

### `src/TemporalDeck.cpp`

Review expander lag-drag request handling and identify what logic is already sufficient to own response shaping centrally.

### `src/TemporalDeckEngine.hpp`

Review how incoming lag targets become scratch motion and ensure the host remains the authority for realization.

## Proposed Outcome

After rework, `TD.Scope` should behave like:

- an accurate waveform-position input surface
- with stable drag mapping
- and minimal preprocessing

`TemporalDeck` should behave like:

- the only owner of the scratch response model
- regardless of whether the input source is platter drag or scope drag

That is the cleanest path to making scope drag both accurate and consistent with the deck behavior that is already tuned.

## Implementation Checklist

This section is intended to be executable by another model or engineer without needing to rediscover the architecture from scratch.

The goal is not to redesign the protocol immediately. The goal is to simplify the current implementation so `TD.Scope` behaves like a precise positioning input and `TemporalDeck` remains the behavior authority.

---

### 1. Establish the target contract

#### Objective

Make the working contract explicit before changing behavior:

- `TD.Scope` computes and transmits intent.
- `TemporalDeck` realizes that intent using the existing scratch behavior model.

#### Required decision

For the near-term implementation, use this contract:

- `TD.Scope` transmits:
  - target lag
  - optional raw or lightly filtered velocity estimate
- `TemporalDeck` owns:
  - hold-vs-scratch interpretation
  - motion freshness
  - scratch-follow behavior
  - live compensation
  - audio realization

#### Deliverable

Add or preserve comments in the relevant code paths making this contract explicit:

- `src/TDScope.cpp`
- `src/TemporalDeck.cpp`

The point is to prevent future reintroduction of scratch-feel logic into scope.

---

### 2. Audit the current scope drag path and classify each behavior

#### Objective

Identify which parts of `TDScope` are:

- required for coordinate conversion and UI stability
- or duplicated controller behavior that should be reduced or removed

#### Files

- `src/TDScope.cpp`

#### Audit items

Review and classify the following pieces of state and logic:

- drag anchor state
- cursor position accumulation
- jitter deadzone
- local filtered velocity
- substep velocity handling
- target lag wrapping/clamping
- any rebasing logic
- any cached lag-per-pixel transform
- any viewport-freezing logic during drag

#### Deliverable

For each item above, produce one of these outcomes in code comments or implementation notes:

- keep as-is because it is pure I/O or necessary UI stability
- simplify because it is too behavior-heavy
- move conceptually to host ownership
- remove because it duplicates host behavior

The review should be explicit enough that a future model can tell which logic is transitional versus intentional.

---

### 3. Reduce `TD.Scope` to stable coordinate-to-target conversion

#### Objective

Make `TD.Scope` primarily responsible for converting cursor motion into desired lag position.

#### Required behavior

Scope drag should keep only the minimum local machinery needed to answer:

- where in lag/time space is the user pointing right now?
- what target lag corresponds to that point?

#### Keep

These are allowed responsibilities in `TDScope`:

- drag-start anchor state
- stable lag-per-pixel mapping during a drag
- off-widget continuation of that same mapping
- mapping from visible scope Y coordinates to lag coordinates
- minimal jitter suppression if needed for stationary hold quality

#### Simplify or remove

These should be simplified where possible:

- locally simulated scratch response behavior
- behavior that tries to make scope feel like platter by reproducing platter math
- local multi-stage target shaping beyond position mapping

#### Deliverable

Refactor `src/TDScope.cpp` so the drag path is easy to read in this sequence:

1. update cursor state
2. convert cursor state to desired lag target
3. apply only necessary domain constraints
4. compute optional lightweight velocity estimate
5. transmit request

If the flow reads like a second scratch engine, it is still too complex.

---

### 4. Decide where loop semantics belong

#### Objective

Clarify whether sample-loop wrap behavior should be implemented in scope or only in the host.

#### Current issue

Scope drag currently needs behavior parity with platter drag at the loop boundary, otherwise ratcheting or sticking occurs.

That means one of two things is true:

- either scope must remain aware of loop wrap in order to express a correct target
- or the host must own all wrap interpretation from a more abstract target representation

#### Near-term recommendation

For the current protocol, it is acceptable for `TD.Scope` to keep loop-aware target wrapping if that is the smallest path to behavior parity.

However:

- keep it narrowly defined as target-domain normalization
- do not let it grow into a broader realization layer

#### Deliverable

Document the rule directly in `src/TDScope.cpp`:

- loop logic here is allowed only to normalize the target lag into the host’s legal domain
- loop playback character remains host-owned

If possible, isolate this into a very small helper with a narrow name such as target normalization rather than scratch behavior.

---

### 5. Minimize scope-side velocity shaping

#### Objective

Stop using `TD.Scope` to generate scratch character.

#### Why

The deck already contains tuned scratch behavior. Scope-side velocity shaping should exist only to provide a usable control signal, not to recreate the feel model.

#### What to evaluate

In `src/TDScope.cpp`, inspect:

- the deadzone constant
- the filtered velocity update
- any substep-based velocity computation
- any hold-settling behavior

#### Desired end state

Velocity generated by scope should be one of these:

- a raw estimate from recent target motion
- a lightly filtered estimate for stability
- or eliminated entirely if host-side derivation proves sufficient

It should not be a rich behavior layer that tries to synthesize platter feel.

#### Deliverable

Add inline comments documenting the intended limitation of scope-side velocity logic:

- provide a stable control signal
- not a full scratch response model

If the velocity path still needs multiple tuning constants, justify each one in terms of signal stability rather than feel design.

---

### 6. Review the expander request receive path in `TemporalDeck`

#### Objective

Confirm that the host already owns the interpretation layer and identify what additional centralization is feasible.

#### Files

- `src/TemporalDeck.cpp`

#### Focus region

Review the lag-drag request receive path and classify:

- what behavior is already host-owned
- what assumptions it makes about the quality of scope-provided velocity
- whether it can derive more from target evolution directly

#### Specific questions

- Can host-side logic derive enough motion behavior from target-lag updates without needing strong scope-side filtering?
- Is `lagDragRequestVelocity` optional in practice, or is the current host behavior strongly dependent on it?
- Can the host smooth or interpret target updates in a way that preserves parity with platter drag?

#### Deliverable

Produce a code comment block or implementation note near the receive path stating:

- which parts of drag behavior are intentionally host-owned
- whether transmitted velocity is advisory or essential
- whether future simplification can reduce dependence on scope-side shaping

---

### 7. Review the scratch realization path in `TemporalDeckEngine`

#### Objective

Verify that the engine remains the final authority for motion and audio behavior, regardless of input source.

#### Files

- `src/TemporalDeckEngine.hpp`

#### Review questions

- Which parts of scratch motion are already common across platter drag and scope drag?
- Are there any branches that cause scope drag to diverge unnecessarily from platter drag?
- Does the engine assume a specific gesture style that might be forcing excess preprocessing in scope?

#### Deliverable

Document how incoming lag targets become realized scratch motion, including:

- target following
- catch-up behavior
- edge handling
- loop handling
- interpolation path selection

The purpose is to make it obvious that this is the behavior authority layer.

---

### 8. Define the stable drag mapping model

#### Objective

Make the intended drag mapping model explicit so future edits do not reintroduce moving-viewport feedback.

#### Required design rule

During an active scope drag:

- the coordinate transform used for drag-to-lag conversion should remain stable
- the user should not be chasing a viewport transform that moves under the cursor

#### Accepted model

Use a drag-start-frozen mapping for the duration of a drag unless there is a proven reason to do otherwise.

#### Explicit non-goal

Do not recompute the drag position mapping against a moving live viewport every frame unless the behavior is deliberately designed and justified.

#### Deliverable

Add or preserve comments in `src/TDScope.cpp` describing:

- why the mapping is frozen at drag start
- why this is distinct from the scope’s visible rendering updates

---

### 9. Separate positioning correctness from feel tuning

#### Objective

Prevent future tuning work from mixing two separate concerns.

#### Positioning correctness means

At `1.00x` sensitivity:

- the target lag change should correspond to the visible scope lag-per-pixel mapping
- off-widget continuation should preserve that same slope
- sample/live mode should both obey the same conceptual rule

#### Feel tuning means

- how quickly the host responds to target changes
- how much smoothing is applied
- how gesture velocity influences scratch behavior
- how live catch-up interacts with forward motion

#### Deliverable

Document in comments or implementation notes that:

- scope owns correctness of the target mapping
- host owns most of the feel

This should be treated as an invariant during future tuning.

---

### 10. Preserve parity with platter drag where it matters

#### Objective

Scope drag should not become identical in UI mechanics to platter drag, but it should converge to the same host behavior authority.

#### Interpretation

Parity means:

- both inputs ultimately feed the same scratch realization logic
- both respect the same loop and live behavior semantics
- both produce comparable audio behavior when asking for equivalent motion

Parity does not mean:

- both inputs must use the same screen-space math
- scope should fake circular platter interaction

#### Deliverable

When reviewing changes, reject any scope-side logic whose only justification is “make it feel like the platter” if it duplicates behavior already centralized in the host.

---

### 11. Implement in low-risk phases

#### Phase A: comment and contract cleanup

- annotate the current scope and host paths
- document intended ownership boundaries
- identify duplicated behavior logic

#### Phase B: scope simplification

- reduce scope path to stable target conversion plus minimal signal cleanup
- preserve existing protocol
- keep loop normalization only if needed

#### Phase C: host centralization

- make host less dependent on rich scope-side velocity shaping if possible
- ensure scope drag and platter drag converge on the same realization behavior

#### Phase D: final tuning

- tune only the residual response constants
- do not reintroduce architectural duplication to solve feel issues

#### Deliverable

Track each phase separately in commits or documented change batches so regression review is easier.

---

### 12. Add regression scenarios

#### Objective

Future work on this path needs a stable set of manual regression scenarios.

#### Required manual checks

At minimum, test the following:

1. Live mode, `1.00x` sensitivity
   - short upward and downward drags
   - long drags
   - forward drag near NOW

2. Sample mode, non-loop
   - drag near sample start
   - drag near sample end
   - off-widget drag beyond both vertical bounds

3. Sample mode, loop enabled
   - rewind across start into end
   - forward drag across end into start
   - repeated wrap passes in both directions

4. Sensitivity scaling
   - `0.50x`
   - `1.00x`
   - `2.00x`

5. Directionality
   - downward drag means older audio / higher lag
   - upward drag means newer audio / lower lag

6. Responsiveness
   - stationary hold should remain stable
   - small motions should not feel artificially delayed
   - scope drag should not feel materially “softer” than necessary compared to platter-driven host behavior

#### Deliverable

Keep this regression list in the doc and update it as architecture changes. This path is too subtle to rely on memory.

---

### 13. Suggested success criteria

A reasonable definition of success for the rework is:

- `TD.Scope` drag position is predictable and visually aligned at `1.00x`
- scope drag does not need hidden compensating scalars to feel correct
- scope drag code reads like a positioning controller, not a second scratch engine
- `TemporalDeck` remains the single authority for scratch feel and audio behavior
- parity with platter drag is maintained through shared host realization rather than duplicated UI-side logic

---

### 14. Suggested follow-up questions for implementation

If another model picks this up, these are the first questions it should answer before modifying behavior again:

1. Which remaining lines in `TDScope.cpp` are still shaping feel instead of just expressing intent?
2. Is transmitted velocity actually necessary, or just currently convenient?
3. Can target normalization be separated cleanly from behavior realization?
4. Can the expander receive path in `TemporalDeck.cpp` assume scope is now a cleaner intent source?
5. What is the smallest next deletion in `TDScope.cpp` that makes the architecture cleaner without breaking parity?

## Finalized Execution Plan

This section is the authoritative implementation plan.

The intent is to stabilize direction before more tuning edits happen. If implementation choices conflict with this section, this section should win unless the architecture itself is being revised.

---

### Final decision: ownership boundaries

#### `TD.Scope` owns

- drag start / drag end state
- stable drag coordinate transform for the duration of a drag
- conversion from scope Y position to desired lag target
- off-widget continuation of the same lag-per-pixel mapping
- minimal jitter suppression needed to avoid noisy stationary holds
- transmission of drag requests to the host
- target-domain normalization only where strictly required for correctness under the current protocol

#### `TD.Scope` does **not** own

- scratch feel design
- platter-style motion character
- rich response shaping
- scratch realization policy
- live catch-up policy
- interpolation behavior
- audio behavior

#### `TemporalDeck` owns

- interpretation of drag requests as playable scratch motion
- hold-vs-scratch behavior
- motion freshness / gesture lifetime behavior
- live catch-up and compensation logic
- loop playback realization
- audio interpolation selection and final sound

---

### Final decision: near-term protocol stance

The expander protocol will remain unchanged for this rework pass.

We will continue to send:

- target lag
- velocity estimate

However, velocity is to be treated as a support signal, not the primary place where scratch feel is designed.

The long-term direction remains:

- make target lag the primary intent signal
- reduce dependence on scope-side behavior shaping over time

---

### Final decision: drag mapping model

During an active scope drag, the drag mapping must use a transform frozen at drag start.

That frozen transform includes:

- the draw-space Y reference used by the drag
- the lag-per-pixel slope
- the relevant lag anchor information used to preserve waveform grab behavior

The live viewport may continue updating visually, but it must not redefine the active drag transform mid-gesture.

Reason:

- recomputing against a moving viewport creates unstable feedback and overcorrection
- a drag should feel like holding a point in waveform space, not chasing a moving coordinate system

---

### Final decision: target model

Scope drag should conceptually behave as a waveform-position grab.

Meaning:

- the user is not merely “nudging playback by some delta”
- the user is indicating a target point in the displayed waveform/time space
- scope should preserve the relationship between the grabbed waveform point and the playback target through the drag

This is the correct conceptual difference between:

- a pure relative nudge model
- a waveform-grab model

The waveform-grab model is the intended design direction.

---

### Final decision: loop semantics

Under the current protocol, `TD.Scope` may normalize target lag into loop-valid space in sample loop mode.

This is allowed only as target-domain normalization.

It is not permission for scope to own playback realization behavior.

Practical rule:

- wrapping in scope is acceptable if needed so the requested target remains continuous and valid
- loop playback character remains a `TemporalDeck` responsibility

---

### Final decision: velocity policy

The scope-side velocity estimate should remain lightweight.

Allowed purpose:

- improve stability and continuity of the control signal

Disallowed purpose:

- recreating platter feel inside `TD.Scope`

Implementation rule:

- if a velocity constant exists in scope code, its justification should be signal stability or anti-jitter behavior
- if its justification is scratch character, it likely belongs in `TemporalDeck`

---

### Final decision: deadzone policy

Deadzone in `TD.Scope` is allowed only for stationary hold stability.

It should not be used as a primary tuning tool for feel.

This means:

- deadzone may be retuned after architecture cleanup
- deadzone should not be used to compensate for mapping or host-response problems

---

### Execution order

Implement in the following order and do not skip ahead unless blocked.

#### Step 1. Freeze architecture with comments and invariants

Before further behavioral tuning:

- add or preserve comments in `src/TDScope.cpp` documenting that scope is a positioning input layer
- add or preserve comments in `src/TemporalDeck.cpp` documenting that host behavior authority remains central
- mark any scope-side logic that is transitional or suspected duplication

Exit criterion:

- another engineer can read the code and tell which layer owns behavior

#### Step 2. Simplify `TD.Scope` to target generation

Refactor `src/TDScope.cpp` so the drag path reads in this order:

1. update drag cursor state
2. compute desired lag target from stable drag mapping
3. normalize target into legal domain if needed
4. compute only lightweight support velocity if still required
5. transmit request

Exit criterion:

- the code path no longer reads like a second scratch engine

#### Step 3. Audit scope-side shaping and remove nonessential parts

Inspect and reduce:

- rebasing logic
- platter-like behavioral shaping
- multi-stage local realization logic
- excessive substep behavior
- velocity shaping that is stronger than needed for signal quality

Exit criterion:

- remaining scope-side logic can be justified as either mapping or signal stability

#### Step 4. Reassess host receive path assumptions

In `src/TemporalDeck.cpp`, inspect the expander drag receive path and determine:

- what host logic already centralizes behavior successfully
- whether scope-side velocity is being over-relied upon
- whether target updates can drive more of the behavior centrally

Exit criterion:

- documented understanding of whether transmitted velocity is advisory or essential

#### Step 5. Tune only after ownership cleanup

Once the architecture is cleaner, retune only these categories in order:

1. position mapping correctness
2. responsiveness / latency feel
3. stationary hold stability
4. boundary behavior in live/sample/loop cases

Exit criterion:

- feel tuning is no longer compensating for an unclear architecture

---

### Explicit implementation rules

These are hard rules for this rework pass.

1. Do not add new arbitrary scale multipliers to compensate for unclear ownership.
2. Do not reintroduce platter-style controller math in `TD.Scope` unless there is a documented architectural reason.
3. Do not let moving live viewport updates redefine the active drag transform mid-gesture.
4. Do not use deadzone or smoothing constants to hide mapping errors.
5. Do not move scratch-feel ownership away from `TemporalDeck`.
6. If a scope-side behavior is added, document why it cannot reasonably live in the host.

---

### Deliverables for the execution pass

A complete implementation pass should leave behind:

1. updated `src/TDScope.cpp` with simplified intent-focused drag logic
2. updated comments in `src/TDScope.cpp` and `src/TemporalDeck.cpp` clarifying ownership
3. any necessary cleanup in `src/TemporalDeck.cpp` receive-path comments or logic
4. preserved behavior parity at loop boundaries and live-mode edge conditions
5. a clean build
6. an updated regression check against the scenarios listed in this document

---

### Definition of done

This plan is considered successfully executed when all of the following are true:

1. `TD.Scope` drag code is clearly an input-intent layer, not a second scratch controller.
2. `TemporalDeck` remains the obvious authority for scratch response behavior.
3. Scope drag at `1.00x` no longer depends on hidden compensating scalars.
4. Live mode, sample mode, and sample-loop mode all behave consistently under the same architectural rules.
5. Remaining tuning discussions are about host response feel, not architectural confusion.

---

### Hand-off note for another model or engineer

If another model continues this work, it should begin by reading only these files in this order:

1. `scope-rework.md`
2. `src/TDScope.cpp`
3. `src/TemporalDeck.cpp`
4. `src/TemporalDeckEngine.hpp`

And it should answer these questions before editing behavior again:

1. Which scope-side logic is still doing more than intent generation?
2. Which parts of the host already provide the needed central behavior authority?
3. What is the smallest next deletion or simplification in `TDScope.cpp` that makes the ownership boundary cleaner?
4. Is the next tuning issue truly in scope mapping, or is it now a host-response issue?
