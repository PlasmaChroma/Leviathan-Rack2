# TD.Scope / TemporalDeck Drag Contract

## Purpose

This note defines the intended ownership boundary for scope-driven drag so future fixes do not split behavior across `TD.Scope` and `TemporalDeck`.

The core rule is:

- `TD.Scope` is a thin I/O surface.
- `TemporalDeck` owns playback behavior.

That means `TD.Scope` should report user intent, and `TemporalDeck` should decide what that intent means in live mode, sample mode, hold, resume, and release.

## Ownership

### TD.Scope owns

- detecting pointer press / drag / release
- converting pointer position into a lag target within the current visible scope window
- estimating optional gesture velocity from pointer movement
- sending drag state to `TemporalDeck`
- displaying the waveform and current view state reported by `TemporalDeck`

### TD.Scope does not own

- live-mode write-head compensation
- hold semantics
- resume-after-hold semantics
- sample-mode versus live-mode behavioral differences
- edge handling rules once a drag request has been sent
- playback-state correction or inertia compensation

### TemporalDeck owns

- interpretation of drag requests
- the scratch / hold / release state machine
- all live-mode semantics
- all sample-mode semantics
- write-head-relative behavior
- stationary hold behavior
- transition from hold back into active motion
- clamping, wrapping, and edge behavior
- smoothing / inertia / gesture freshness rules

## Message Contract

`TD.Scope` sends only the following logical fields to `TemporalDeck`:

- `active`
  - `true` while the pointer drag is active
  - `false` on release/cancel
- `lag_target`
  - requested playback lag in samples
  - interpreted as "distance from NOW" in live mode, or equivalent deck-owned lag domain in sample mode
- `gesture_velocity`
  - optional signed gesture velocity in samples/sec
  - positive means toward NOW (decreasing lag)
  - negative means away from NOW (increasing lag)

`TD.Scope` may send `gesture_velocity = 0` during stationary hold, but it must not change message meaning based on mode-specific assumptions.

## Required Behavioral Semantics

### Drag start

When the user presses in `TD.Scope`:

- `TD.Scope` sends `active = true`
- `TD.Scope` sends the lag under the pointer as `lag_target`
- `TemporalDeck` decides whether this begins scratch, direct hold, or another internal gesture state

### Stationary hold

When the pointer remains effectively still:

- playback time should remain pinned
- the waveform should not drift forward under the cursor in live mode
- sample mode should preserve its own equivalent "pinned" behavior

This is a `TemporalDeck` responsibility. `TD.Scope` should only continue reporting an active drag with a stable target and zero/near-zero velocity.

### Resume after hold

When the user begins moving again after a stationary hold:

- motion must continue from the held playback position
- motion must not jump by the amount of live buffer accumulated during the hold
- motion must not resume relative to stale touch-down state if the deck has already established a held playback position

This transition is owned by `TemporalDeck`.

### Live mode

In live mode:

- the write head continues to advance
- a stationary hold must pin playback time despite the advancing write head
- resumed motion must be computed from the held playback position, not from original press time and not from a `TD.Scope`-side compensation model

Any required write-head compensation belongs in `TemporalDeck`, not in `TD.Scope`.

### Sample mode

In sample mode:

- scope drag should feel direct and stable
- sample wrapping/clamping rules are deck-owned
- scope must not apply live-mode fixes or anchor tricks that alter sample behavior

### Release

When the user releases:

- `TD.Scope` sends `active = false`
- `TemporalDeck` exits its internal drag/hold state according to its own rules

## Implementation Rules

### Rules for TD.Scope

- Do not apply live-only write-head compensation in the UI layer.
- Do not special-case resume-after-hold behavior in the UI layer.
- Do not mutate drag anchor semantics based on guessed deck behavior.
- Do not implement separate live/sample behavior beyond basic coordinate-to-lag mapping.
- Keep drag output simple, stable, and mode-agnostic.

### Rules for TemporalDeck

- Treat scope drag as equivalent input intent to direct platter interaction.
- Keep the drag state machine authoritative on the deck side.
- Ensure hold and resume behavior are solved once in the deck, not re-solved in each input surface.
- Ensure the same contract works for both platter UI and scope UI wherever possible.

## Current Cleanup Implication

Before further drag fixes, re-audit recent `TD.Scope` drag changes against this contract.

Likely candidates to remove or relocate out of `TD.Scope`:

- one-sided live write-head compensation during drag
- hold/resume anchor hacks
- pinned-hold special state that exists only to correct deck behavior indirectly

Likely candidates to consolidate in `TemporalDeck`:

- stationary hold detection
- direct-hold versus scratch transition
- resume-after-hold semantics
- gesture freshness / revision handling for scope-driven drag

## Acceptance Test

The contract is satisfied when all of the following are true:

1. Press-and-hold in live mode keeps playback pinned with no forward drift.
2. Resuming from a live hold does not jump by elapsed live buffer time.
3. Slow movement toward and away from NOW works without barriers or stutter.
4. Sample-mode drag direction remains correct and stable.
5. `TD.Scope` no longer contains deck-semantic compensation logic for live scratch behavior.
