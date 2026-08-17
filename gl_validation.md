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

After disabling steady-state validation, the multi-millisecond validation spikes disappeared.

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

- Do not call `glIs*` during steady-state draws.
- Rely on `onContextDestroy()` and local reset paths for normal context lifecycle.
- Lazily recreate resources on the next draw after a reset.

Set `"extraGlValidation": true` in `res/dragonking.txt` to enable per-draw validation as a
diagnostic. `"ExtraGlValidation": true` is also accepted for consistency with the other
DragonKing debug flags. It defaults to false.

## Tradeoff

Removing steady-state validation reduces driver query stalls and improves predictable draw cost.
The risk is delayed detection if a host or driver invalidates GL resources without normal context
destroy notification. That is expected to be rare. In that case, debug-periodic validation remains
available to diagnose the issue.

## Current Implementation

TD.Scope, Bifurx, and Wyrm gate their steady-state `glIs*` checks behind
`extraGlValidation`. Normal production draws use lifecycle-driven reset and lazy recreation.

TD.Scope CSV logging includes `extra_gl_validation` so validation-enabled frames are explicit.
When that column is `0`, `resource_validate_us` should be zero because the expensive validation
function did not run. If `resource_validate_us` is large while `extra_gl_validation` is `0`, that
is a logging or instrumentation bug rather than real GL resource validation cost.
