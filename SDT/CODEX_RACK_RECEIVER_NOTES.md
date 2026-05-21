# CODEX Rack Receiver Notes

## Purpose

This document is for the eventual VCV Rack side of the SDT-SL bridge.

The current bridge can use trowaSoft `cvOSCcv`, which means no native Rack receiver is required for v0 experimentation. If this repository later gets a dedicated Leviathan Rack module that receives SDT-SL directly, use these notes to keep the implementation Rack-safe.

## Recommended Development Path

Use stages:

1. External Python conductor sends OSC to trowaSoft `cvOSCcv`.
2. Rack patches validate the eight-lane semantics musically.
3. If useful, add a dedicated unreleased Rack module or helper that receives the same lane contract.
4. Only after the contract stabilizes, consider deeper integration with existing modules.

Do not start by changing released modules. Integral Flux, Proc, and Temporal Deck already have compatibility obligations.

## Native Module Shape

A dedicated Rack receiver should be simple:

- eight CV outputs matching the lane contract
- optional eight activity lights
- host/port configuration from UI or JSON
- one background receiver thread
- one lock-free or mutex-protected lane snapshot
- audio thread reads latest values only

Avoid audio-thread networking. Sockets, blocking reads, DNS, allocations, and JSON parsing must stay outside `process()`.

## Threading Rules

Rack `process()` should do only cheap operations:

- copy atomic or pre-buffered floats
- clamp values
- optionally slew-limit values
- write outputs and lights

The receiver thread may:

- open sockets
- parse OSC
- update the latest lane snapshot
- record last-message timestamps

If a mutex is used, keep the critical section tiny and never let the audio thread block on I/O. Prefer atomic floats for the eight lane values if the code stays simple.

## Debug Gating

Debug and developer functionality should follow the repository pattern:

```cpp
isDragonKingDebugEnabled()
```

Use the existing debug terminal style only for observability, not for live CV transport. `tools/debug_terminal/server.py` and `src/DebugTerminalTransport.*` are useful references for small diagnostic packets, environment-based host/port configuration, and guarded developer behavior.

Do not expose a network listener or noisy UI diagnostics in normal release mode unless the user explicitly asks for that behavior.

## UI and Panel Patterns

If a new module gets a panel, use the established component-placement pattern:

- place controls/components through the SVG component layer where practical
- use `PanelSvgUtils` helper functions
- avoid hand-positioning drift when existing helpers already solve it

For a v0 receiver, the UI can be utilitarian:

- output jacks for all eight lanes
- lane labels
- activity/status lights
- optional port display or menu item

## Serialization

For an unreleased receiver module, JSON can be straightforward.

Recommended fields:

- `host`
- `port`
- `scale`
- `slewMs`
- `bipolarDrift`
- `lastProgramHint` if useful

If any receiver behavior is later embedded into released modules, append new params/inputs/outputs/lights rather than reordering enums.

## Testing Strategy

Prefer fast, focused tests:

- lane clamping
- message-to-lane mapping
- stale-message timeout behavior
- JSON round trip for receiver settings
- slew behavior if added

In WSL, do not treat full plugin link failures as authoritative. Source-level checks and focused tests are the expected validation path.

## Failure Behavior

Receiver failure should be musically safe:

- unknown OSC address: ignore
- malformed payload: ignore
- missing messages: hold briefly, then optionally decay or zero
- shutdown: zero outputs if possible
- port bind failure: leave outputs at zero and show debug/status indication

Do not crash Rack because the conductor is absent.

## Future Integration Ideas

After the eight-lane contract proves useful, possible integrations include:

- an SDT-SL receiver module with direct CV outputs
- an expander that maps lanes into a specific unreleased module
- a Python dry-run recorder that writes lane traces for tests
- a Rack-side trace replay mode gated by `isDragonKingDebugEnabled()`
- a minimal bridge from debug terminal packets to external visualization

Keep these secondary. The first useful artifact is a stable conductor contract that can make Rack respond predictably.

