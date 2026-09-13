# Adaptive visual update pilot

Enable Dragon King debug, then open Integral Flux's context menu and enable
**Adaptive contour updates (all Flux, pilot)** under Preview Visual. This is a
session-only switch, off by default, shared across Flux instances. Select the
NanoVG preview renderer; the OpenGL preview does not participate. Reopening the
menu shows the sampled FPS and pressure level. No patch serialization changes.

The reusable UI-thread sampler in `src/render/RackVisualFramePressure.hpp`
observes Rack frame-start intervals once per frame across consumers. The target
is the lower of the configured positive frame limit and monitor refresh rate,
with a 60 Hz fallback if neither is available. A half-second moving average,
sustained overload threshold, and slower recovery reduce rapid switching. Gaps
over 250 ms reset sampling. This is a heuristic: background throttling below
that threshold and host-specific frame caps can still look like overload.

Flux uses the default policy: pressure levels 1/2/3 cap contour refreshes at
30/15/10 Hz. Level zero preserves the existing direct-draw/100 ms settlement
path. Under pressure, geometry changes remain pending until the next refresh
slot, while the previous Rack-owned framebuffer is composited. The last pending
geometry is rendered even if modulation stops. Styling, highlighting, transform,
size, and context invalidation bypass the geometry delay. Refresh slots have
different phases across contours. Existing framebuffer timers remain nested in
the existing module draw accounting.

The pilot only gates NanoVG contour rasterization. Point rebuilding, tracer
history, markers, labels, knobs, and the other module visuals still run normally.
It introduces no raw GL ownership or screen capture and does not cache cables.
Nested framebuffer rendering and rotated/skewed transforms retain direct draw.

## Live evaluation

Compare the same patch with the toggle off/on, using several visible Flux
instances with continuously modulated rise/fall/shape. Check FPS and existing
Process/Step/Draw telemetry after each setting has stabilized. Repeat with
tracers off to isolate contour savings. The refresh frames can cost more than
direct drawing, so measured benefit is required before broad rollout.

Verify final contours after modulation stops, highlighted control interaction,
zoom and module movement, theme changes, and window close/reopen. Confirm that
cables, markers, and labels remain live. Try a deliberate 30 FPS cap and a pause
longer than 250 ms. Controller unit tests cover timing but do not establish GPU
speedup or visual correctness in a running Rack window.
