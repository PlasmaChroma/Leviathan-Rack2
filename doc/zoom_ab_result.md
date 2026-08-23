# GL Zoom Experiment A/B Result

The experimental path that called `FramebufferWidget::render()` from inside the same widget's `draw()` was tested and retired.

With the path enabled, `load.png` showed multiple non-Leviathan modules whose controls and lights existed but whose cached panel layers never received a valid first render. With the path disabled, `load_control.png` showed those same module slots fully initialized without requiring zoom interaction.

This result is consistent with the source-level warning in `zoom_report.md`: invoking `render()` before Rack's base `FramebufferWidget::draw()` can bypass Rack's nested-framebuffer guard and interfere with an enclosing framebuffer pass. The experiment also caused its synthetic framebuffer scale to disagree with Rack's current world scale.

The failed path, runtime flag, and manual-render telemetry were removed rather than retained as an unsafe diagnostic.

The following safe work remains:

- Bifurx uses explicit cached-framebuffer invalidation rather than unconditional `OpenGlWidget::step()` redraws.
- Wyrm authored geometry and shader selection no longer depend continuously on Rack zoom.
- Debug-gated `gl_zoom` telemetry records context generation, dirty causes, zoom, framebuffer dimensions, draw cost/count, and Wyrm shader compile/link activity.

Future fixed-resolution work must use a Leviathan-owned, context-safe surface and composite its completed image without recursively invoking Rack framebuffer rendering from `draw()`.
