# TD.Scope Release No-Go Review

Current decision: **No, TD.Scope is not ready to release.**

The module is compiling at the object level and the fast test suite passes, but the current release risk is behavioral and architectural rather than a basic build failure. The Live-mode drag interaction is still not settled enough for a first public release, and the drag code still has duplicated paths that make future tuning harder than it needs to be.

## Implementation Notes for Follow-Up Work

These details matter if this review is handed to a smaller model or split into implementation tasks.

### Active Drag Path

The active mouse path is expected to be `TDScopeInputWidget`, not `TDScopeDisplayWidget`.

Widget stacking is built in `src/TDScopeWidget.cpp`:

- `createGlDisplay()` is added first
- `createDisplay()` is added second
- `createInput()` is added third

Rack event hit-testing should therefore hit `TDScopeInputWidget` first. Any drag-behavior fix should start with `TDScopeInputWidget::onDragStart()`, `TDScopeInputWidget::onDragMove()`, `TDScopeInputWidget::onDragEnd()`, and `TDScopeInputWidget::step()`.

Do not tune only `TDScopeDisplayWidget` and assume the change affects normal pointer input.

### Drag Coordinate Contract

Scope drag motion is vertical local widget motion. The input path currently converts Rack mouse delta into local Scope pixels with:

- `e.mouseDelta.y / currentRackZoom()`

This correction is important. Without it, Scope drag sensitivity changes with Rack zoom. Keep this correction unless the drag model is replaced with a more explicit local-coordinate calculation.

The intended lightweight Scope contract should be:

1. convert pointer motion into local Scope pixels
2. map local pixels through the current visible lag window
3. send a requested lag/velocity/hold state to Temporal Deck

Scope should not own transport policy beyond that if it can be avoided.

### Shared Drag Math Candidates

The best existing place for reusable Scope drag math is `src/TDScopeShared.hpp`.

Important helpers already there:

- `tdscope::computeScopeWindowLagSpan()`
- `tdscope::solveLagDragPlaybackLag()`
- `tdscope::computeLagDragVelocity()`

If the duplicate drag implementations are cleaned up, prefer moving pure calculations into `TDScopeShared.hpp` rather than adding another ad hoc helper in `TDScope.cpp`.

Keep UI event state in the widget. Keep pure lag/window math in shared helpers.

### Live vs Freeze vs Sample Mode

Do not treat all modes the same.

Sample mode:

- lag is constrained by loaded sample length and loop behavior
- forward/backward drag should be symmetric except for sample boundaries
- Live catch-up compensation should not apply

Live freeze mode:

- should behave like a stable frozen buffer
- forward/backward drag should feel close to symmetric
- catch-up compensation should not apply

Live non-freeze mode:

- buffer head continues to advance while dragging
- near-NOW behavior can need assist
- deep-buffer behavior should not feel like the user is fighting forward motion

The recent manual signal was that freeze mode feels good on Scope and platter. Use that as the baseline when isolating Live-only bugs.

### Stationary Hold Behavior

`TDScopeInputWidget` has stationary-hold behavior for the case where the mouse is still held down but movement has slowed or stopped.

Preserve this behavior when refactoring. It exists to avoid the "snap while still holding mouse-down" feeling.

Relevant state names in the input widget include:

- `lagDragStationaryHoldActive`
- `lagDragLastMoveTimeSec`
- `lagDragResidualLocalPixels`
- `lagDragRequestActive`

If these move into a shared drag controller, keep the distinction between:

- active drag
- active drag with movement
- active drag with stationary hold
- drag released

### Expander Request Contract

TD.Scope sends drag requests to Temporal Deck through `TemporalDeckExpanderProtocol::DisplayToHost`.

Important pieces:

- request validity is checked by magic/version/size
- lag drag active/hold/lag are packed into `reserved`
- velocity is sent as `lagDragVelocity`
- `requestSeq` is incremented when the request changes

Do not reorder or casually resize protocol fields without checking compatibility. TD.Scope is unreleased, but Temporal Deck is released, and the expander boundary touches it.

### Atomic Snapshot Risk

`TDScope::setLagDragRequest()` currently updates several atomics independently. If this is fixed, avoid adding locks in the audio process path.

Safer implementation shapes:

- double-buffer a small request struct and publish an index/sequence atomically
- use a sequence counter around the fields and retry on mismatch
- pack the small request into one or two atomics if practical

The goal is a coherent UI-to-process request without blocking the audio thread.

### Renderer Fallback Decision

The OpenGL renderer is the default. The render-mode menu is currently behind `isDragonKingDebugEnabled()`.

If exposing a release fallback:

- keep advanced debug toggles debug-gated
- expose only a simple user-facing fallback if needed
- avoid adding released params for this; use context menu or JSON state

Do not change parameter/input/output/light enum ordering for Temporal Deck. TD.Scope currently has no params/inputs/outputs, but the general release-compatibility rule still applies across released modules.

### Validation Commands

In this WSL-like environment, use:

- `make test-fast`
- `make -j 10 build/src/TDScope.cpp.o build/src/TDScopeGL.cpp.o build/src/TDScopeWidget.cpp.o`

Do not treat full `plugin.so` link failures here as authoritative regressions. Final plugin validation should happen in the Windows/MSYS2 Rack toolchain.

## Blocking Findings

### 1. Live Scope Drag Behavior Is Still Not Release-Stable

The active Scope drag path still contains Scope-local Live-mode compensation:

- `src/TDScope.cpp`
- `TDScopeInputWidget::onDragMove()`
- One-sided compensation when dragging away from NOW in Live non-freeze mode

This path directly affects the behavior that has been under active tuning:

- forward/backward drag symmetry
- reaching NOW in the final Live buffer window
- deep-buffer drag resistance
- differences between Scope drag and Temporal Deck platter drag

Freeze mode currently feels better because it removes the Live drift/catch-up dynamics from the interaction. That is a useful signal: the base visual mapping may be close, but the Live-mode correction model is not yet proven.

Release should wait until Live-mode Scope drags feel consistently acceptable in these cases:

- dragging forward toward NOW from deep in the buffer
- dragging backward away from NOW
- dragging within the final 1000 ms near NOW
- dragging while still holding the mouse down after movement slows or stops
- comparing equivalent Scope and platter movements

### 2. Scope Has Two Separate Drag Implementations

There are currently two drag models in `src/TDScope.cpp`:

- `TDScopeDisplayWidget`
- `TDScopeInputWidget`

The normal widget stack places `TDScopeInputWidget` above the display widgets, so it is probably the active pointer-event path in normal use. However, the older display-widget drag path still exists and differs from the input-widget path.

Key differences:

- the display-widget path does not refresh the expander snapshot on every drag move
- the display-widget path does not implement the newer stationary-hold behavior
- the display-widget path has its own Live compensation logic
- the input-widget path has the current rack-zoom pixel correction

This is a release risk because the next person editing the drag behavior can easily fix one path and miss the other. Before release, Scope should have one drag implementation, or both widgets should call shared drag-solving helpers with no duplicated policy.

## High-Value Cleanup Before Release

### 3. Drag Request Publication Is Not a Coherent Snapshot

`TDScope::setLagDragRequest()` writes the drag request across several independent atomics:

- active flag
- stationary-hold flag
- lag samples
- velocity

`TDScope::process()` reads those fields separately when publishing the expander request to Temporal Deck.

This can produce mixed-frame requests, for example:

- new active flag with old lag samples
- new lag samples with old velocity
- stationary-hold state from a different UI event

This is probably rare, but drag feel is sensitive to small discontinuities. A release-ready request path should publish one coherent request snapshot, ideally with a sequence number or small lock-free double-buffer pattern.

### 4. Scope Still Owns Too Much Live Drag Policy

TD.Scope was intended to be a lightweight expander/interface model. The current Scope drag path still does some Live-mode behavioral correction locally before sending the request to Temporal Deck.

That makes the model harder to reason about:

- Scope has visible-window mapping concerns
- Temporal Deck has transport/playhead/catch-up concerns
- Live-mode correction currently straddles both sides

The cleaner release shape is for Scope to translate local pointer motion into an intended lag request, and for Temporal Deck or shared Temporal Deck interaction helpers to own Live transport compensation.

### 5. Release Users Have No Renderer Fallback Control

TD.Scope defaults to the OpenGL renderer, while the render-mode selection menu is currently debug-gated.

The OpenGL path has fallbacks internally, but if a user has driver-specific rendering problems, release users have no normal UI path to select a simpler renderer. This is not necessarily a blocker if OpenGL is considered production-ready, but it is a release decision that should be made explicitly.

Options:

- expose a simple non-debug renderer fallback item
- keep the debug-only menu but validate OpenGL across enough target systems
- default to the safest renderer for first release

### 6. GL Shader Failure Cleanup Needs a Pass

The field-shader initialization path in `src/TDScopeGL.cpp` has at least one failure branch where uniform lookup can fail after GL resources have been created.

The renderer falls back, so this is probably not user-visible in the common case. Still, release polish should include cleaning up partially-created resources and making retry behavior intentional.

### 7. Focused TD.Scope Tests Are Missing

`make test-fast` passes, but there are no focused tests for the current TD.Scope behavior surface.

Useful tests before release:

- Scope drag lag solving in Sample mode
- Scope drag lag solving in Live freeze mode
- Scope drag lag solving in Live non-freeze mode
- near-NOW behavior inside the final 1000 ms
- deep-buffer forward/backward drag symmetry
- expander request encoding/decoding for active, hold, lag, and velocity
- renderer mode serialization/migration

## Validation State

Validated during review:

- `make test-fast` passed
- TD.Scope object-level compilation passed/up-to-date for:
  - `build/src/TDScope.cpp.o`
  - `build/src/TDScopeGL.cpp.o`
  - `build/src/TDScopeWidget.cpp.o`

Not treated as authoritative in this environment:

- final plugin linking

This repo is being reviewed from a WSL-like environment, so final Windows/MSYS2 plugin build validation remains a separate required release step.

## Recommended Release Gate

TD.Scope should become a release candidate only after:

1. Scope has a single drag implementation or shared drag helper path.
2. Live-mode drag compensation is owned by Temporal Deck or a shared interaction helper, not duplicated inside Scope.
3. Drag requests are published as coherent snapshots.
4. The renderer fallback decision is made deliberately.
5. The shader failure path is cleaned up.
6. Focused TD.Scope fast tests cover the drag/request behavior.
7. Manual validation confirms Scope and platter drags feel consistent enough in Live mode.

Until then, the release answer is **no-go**.
