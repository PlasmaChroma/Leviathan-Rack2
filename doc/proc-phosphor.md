# Proc phosphor tracer experiment

Enable **Preview Visual → Phosphor tracer (experimental)** in Proc's context
menu while Dragon King debug mode is active. The phosphor toggle and settings
are hidden otherwise. This also enables Preview Tracer. Turning phosphor off returns to the
existing tracer; turning Preview Tracer off disables both.

**Phosphor settings** provides persistence (0.2, 0.6, or 1.2 seconds to roughly
1% brightness), deposit strength, and additive buildup. Disable additive buildup
for more restrained source-over blending. Existing patches default to the old
tracer. All new choices are saved per module.

The shared `visual/PhosphorPreview.hpp` component alternates two GPU textures:
decay the previous texture into the next, then deposit a historical curve when
the preview has moved visibly. Deposits are limited to at most 24 per second.
The fixed-size CPU geometry is uploaded through GL drawing only when depositing;
there is no CPU image rasterization, pixel decay loop, or per-frame image upload.
The crisp waveform, grid, highlights, frequency text, and dot stay separate.
After the trail expires the transparent framebuffer stops requesting redraws.

Phosphor uses soft-edged amber ribbons. Repeated paths accumulate brightness;
stationary shapes and common rate changes that preserve geometry do not deposit.
Context changes and framebuffer resizing reset history. Allocation failure falls
back to the existing tracer. Integral Flux is not changed by this experiment.

For visual/performance comparison, try slow rise/fall changes, rapid shape-knob
sweeps, and continuous CV modulation; also let the controls settle and toggle the
mode several times. Check low/high zoom and Rack window close/reopen in a DAW.
Compare whole-module Process, Step, and Draw telemetry under the same conditions.
The new backend is experimental: successful compilation and settings tests do
not establish a performance win or replace visual review in Rack.
