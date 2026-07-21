# ApertureLight context-menu bleed

## Summary

Leviathan's custom Aperture LEDs can remain visible through a VCV Rack context
menu. The artifacts look like small colored points or short lines. When the
underlying LED brightness changes, the artifact can flicker with it.

The current implementation contains no menu-aware suppression. An experiment
that removed the nested bloom `FramebufferWidget` did not fix the problem and
has been reverted. The original cached bloom and visual treatment are restored
while the actual compositing cause remains under investigation. **Bleed-through
is present in the current implementation:** constant-brightness LEDs leave
constant-color marks in context menus, and changing LEDs produce flickering
marks.

The current test keeps the socket and unlit lens in a static normal-pass
framebuffer. The core and crescent use a separate small, dynamic framebuffer,
composited once with locally scoped screen blending in the normal pass. The
upper-left specular glint is then drawn separately with the same locally scoped
normal-pass screen blend, restoring its independent additive contribution. Only
bloom remains in Rack's screen-blended light layer. This avoids re-rasterizing
static geometry for every changing LED and does not make LEDs aware of menus.

Reference captures:

- `bleed.png`: initial yellow and green artifacts through an Undertow menu.
- `bleed2.png`: two yellow artifacts through an Integral Flux menu.

## Confirmed observations

- The artifacts correspond to custom Aperture LEDs behind the menu.
- A changing LED produces a changing or flickering artifact.
- A constant-brightness LED produces a constant-color artifact. This rules out
  cache invalidation as a complete explanation.
- Rack's native LEDs do not show a perceptible version of the problem in the
  same situation.
- Suppressing the complete illuminated Aperture pass whenever a menu is open
  removes the artifacts.
- Global suppression is visually undesirable because every Aperture LED goes
  dark while any context menu is open.
- Suppressing only LEDs spatially covered by a menu avoided the global blackout,
  but remained a rendering-policy hack tied to menu location.

## Rack rendering order

This does not appear to be a conventional scene-tree Z-order error.

Rack's `RackWidget::draw()` performs the relevant work in this order:

1. Draw rack rails, modules, and normal widget content.
2. Draw module light layer 1.
3. Draw plugs and cables.
4. Return to `Scene::draw()`, which later draws scene-level children such as
   `ui::MenuOverlay` and its `ui::Menu`.

Context menus are created beneath a `ui::MenuOverlay` attached directly to the
scene. The menu is therefore submitted after rack modules and their lights.

Blendish's compiled-in default menu inner color uses alpha `0.902`, but Rack's
`ui::refreshTheme()` replaces the menu color with the selected UI background,
whose alpha is `1.0` in the inspected Rack source. We therefore cannot assume
that the menu body is translucent in the user's runtime. The captures also put
artifacts inside the menu body, not only on its antialiased rounded boundary.

This leaves an unresolved discrepancy: normal Rack scene submission places the
menu after the rack light pass, and an opaque source-over menu body should fully
replace pixels beneath it. Either the runtime menu samples are not actually
opaque, or some Aperture pixels reach the target after the menu has been drawn.
The current evidence does not yet distinguish those cases.

## What differs from a native Rack LED

Rack's `LightWidget::drawLayer()` uses screen blending for layer 1:

```cpp
nvgGlobalCompositeBlendFunc(vg, NVG_ONE_MINUS_DST_COLOR, NVG_ONE);
```

A native LED draws relatively simple foreground and halo geometry in that pass.
`LeviathanApertureLight` is more aggressive visually:

- it has a bright colored core;
- it can produce a white-hot specular glint;
- it draws a wider bloom;
- its bloom was previously supplied through a nested `FramebufferWidget` cache;
- its transfer curve makes the glow visible early and the hot component strong
  near peak brightness.

These differences can make ApertureLight pixels much hotter than native LED
pixels. The nested framebuffer was tested as the strongest obvious
architectural difference from Rack's native LED path. Removing it did not
change the bleed, so it is not the root cause by itself.

It is not yet proven that NanoVG blend-state leakage is the root cause. The
dynamic flicker proves that the live illuminated pass—not only static panel
artwork—is visible.

## Experiments performed

### Move Aperture illumination to the normal widget pass

The custom light was temporarily drawn in `draw()` while Rack's inherited light
layer was bypassed. This changed the ordering but did not fully remove the
artifact. It also departed unnecessarily from Rack's native light lifecycle.

This approach was removed.

### Explicitly restore source-over blending

Both Aperture implementations were changed to restore
`NVG_SOURCE_OVER` after their layer-1 pass. This removed the larger two-artifact
appearance during testing, but also reduced or removed the desired tiny lens
highlights.

This approach was removed when the implementation returned to Rack's native
light lifecycle.

### Draw the specular separately

The specular glint was moved into a source-over detail pass after the blend
reset. This restored the visual flavor, but a smaller single line or glint could
still be seen through menus.

This approach was removed. The specular is again part of the normal Aperture
light pass.

### Suppress only the glint beneath menus

A point-based menu intersection test attempted to suppress only the glint. This
did not solve the issue because the changing core/bloom could also be seen.

This approach was removed.

### Suppress all Aperture illumination while a menu is open

This eliminated the bleed and confirmed that the artifacts originated in the
dynamic Aperture illumination. Its side effect was that every Aperture LED in
the rack went dark whenever any context menu opened.

This was followed by spatial suppression, which was also later removed.

### Suppress only spatially covered Aperture LEDs

The light and menu bounds were converted to absolute scene coordinates. The
full illuminated pass was omitted only when an LED's bloom bounds intersected
a menu. This avoided blanking every Aperture LED, but it made a visual component
depend on knowledge of unrelated scene-level UI and was judged too fragile and
too difficult to justify as the permanent architecture.

This approach was removed.

### Remove the nested bloom framebuffer

The cached bloom framebuffer was removed from `LeviathanApertureLight`, causing
bloom to be drawn directly alongside the core, crescent, and specular in Rack's
screen-blended layer-1 light pass. Both constant and changing LEDs still bled
through menus.

This experiment failed and was reverted.

## Current implementation

`LeviathanApertureLight` follows the inherited Rack light lifecycle without
overriding `draw()` or `drawLayer()`:

- `drawBackground()` renders the cached static socket and unlit lens;
- Rack invokes `drawLight()` in its screen-blended layer 1;
- `drawLight()` submits the cached bloom, then renders core, crescent, and
  specular;
- there is no menu lookup, coordinate intersection, conditional hiding, or
  explicit composite-mode reset;
- the original bloom framebuffer is retained because removing it did not fix
  the problem.

`SmallGoldApertureLight` also uses the inherited native lifecycle. It never used
the general Aperture bloom framebuffer, and all temporary menu-aware and
separate-specular changes have been removed from it.

## Tradeoffs and limitations

- The runtime menu fill's effective alpha and composite state need to be
  measured rather than inferred from Blendish's compiled-in defaults.
- Aperture draws several overlapping screen-blended primitives, unlike the
  simpler native foreground. This can produce much hotter pixels, but intensity
  alone cannot pass through a truly opaque menu drawn afterward.
- A useful controlled test is to temporarily render an Aperture LED as one
  native-style foreground fill. That separates intensity/overdraw from widget
  placement and submission order.
- `SmallGoldApertureLight` had no nested bloom framebuffer. If it independently
  reproduces the issue, the cause is broader than the removed cache.

## Validation checklist

- Open a module context menu over static bright Aperture LEDs.
- Repeat over rapidly changing Aperture LEDs and look for flickering pixels.
- Move the menu across several LEDs; none should conditionally turn off.
- Check menu edges and rounded corners at several Rack zoom levels.
- Check both standalone colored Aperture LEDs and `SmallGoldApertureButton`.
- Confirm bloom, core, crescent, and specular match the pre-fix visual flavor.
- Confirm that the restored bloom cache has no visual or performance regression.
