# GL Resource Validation Notes

## Context

TD.Scope draw logging showed recurring draw-time spikes in the `validate_clear_us` bucket.
After splitting that bucket, the spikes were isolated to `resource_validate_us`, not framebuffer
size queries, viewport setup, or clear calls.

The expensive path was per-draw GL resource validation through:

- `glIsProgram()`
- `glIsBuffer()`
- `glIsTexture()`

These calls are driver queries. On some drivers they can serialize or do real validation work,
so they are not safe to treat as cheap steady-state checks.

## Observed Behavior

Before throttling validation, logs showed multi-millisecond spikes in `resource_validate_us`.
Clear calls were not the source:

- `transparent_clear_us` stayed very small.
- `scoped_clear_us` stayed very small.
- `resource_validate_us` owned the large `validate_clear_us` spikes.

After throttling validation to run immediately after reset and then periodically, the
multi-millisecond validation spikes disappeared. Remaining validation cost appeared only on
periodic validation rows.

## Current Interpretation

If normal GL context teardown is handled correctly, steady-state `glIs*` validation is mostly
guarding an abnormal edge case: GL objects becoming invalid without Rack delivering a context
destroy event and without local state being reset.

For normal operation, the primary lifecycle protections should be:

- Reset/invalidate local GL handles on `onContextDestroy()`.
- Avoid destructor-time GL cleanup unless Rack's GL context is definitely current.
- Lazily create resources in the current draw-time context.
- Reset/rebuild locally if creation or draw setup fails.

## Recommended Policy

Use an event-driven fast path for production:

- Validate immediately after local GL resource state has been reset.
- Validate immediately after resource creation if needed.
- Do not call `glIs*` every steady-state draw.
- Rely on `onContextDestroy()` and local reset paths for normal context lifecycle.

Keep periodic validation as a debug diagnostic, not a production steady-state cost:

```cpp
if (glValidationRequired) {
  validateGlResourcesForCurrentContext();
} else if (isDragonKingDebugEnabled() && validationCountdownExpired()) {
  validateGlResourcesForCurrentContext();
}
```

`ScopeDrawLogging` can also be treated as a debug mode that enables periodic validation while
collecting lifecycle/performance data.

## Tradeoff

Removing steady-state validation reduces driver query stalls and improves predictable draw cost.
The risk is delayed detection if a host or driver invalidates GL resources without normal context
destroy notification. That is expected to be rare. In that case, debug-periodic validation remains
available to diagnose the issue.

## Follow-Up

TD.Scope currently uses throttled validation. A likely next refinement is to make the periodic
validation path conditional on DragonKing debug or `ScopeDrawLogging`, leaving production draws
with startup/context/reset validation only.
