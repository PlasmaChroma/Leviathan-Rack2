# Alternate tint notes

- The temporary extreme glass-rect diagnostic tint on TemporalDeck is visually interesting enough to revisit as an intentional alternate panel treatment.
- Worth exploring: stronger colored wash, brighter beveled border, and more obvious glass separation on smaller grouped regions like TemporalDeck outputs.
- Keep this distinct from the final diagnostic overlay; the production version should be tuned rather than magenta/white debug-heavy.

## Current diagnostic overlay

Applied at the end of `drawGlassRectPiece()`:

```cpp
nvgFillColor(args.vg, nvgRGBA(255, 0, 255, 42));
nvgStrokeWidth(args.vg, 2.2f);
nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 220));
```

So the visible diagnostic colors are:

- fill: magenta `rgba(255, 0, 255, 42)`
- border: white `rgba(255, 255, 255, 220)`
- border width: `2.2 px`

## Size-aware glass shift used before the diagnostic overlay

```cpp
smallBoost = clamp((90.f - min(w, h)) / 55.f, 0.f, 1.f);

glowAlpha       = 0.105f + smallBoost * 0.08f;
baseWashAlpha   = 0.055f + smallBoost * 0.07f;
topWhiteAlpha   = round(20.f + smallBoost * 16.f);
sheenAlpha      = round(10.f + smallBoost * 16.f);
strokeWhiteAlpha = round(24.f + smallBoost * 24.f);
topLineAlpha    = round(34.f + smallBoost * 34.f);
edgeAlphaBoost  = 1.f + smallBoost * 0.7f;
```

Palette references:

- violet edge: `#7a5cff`, alpha `0.22 * edgeAlphaBoost`
- cyan edge: `#1cccd9`, alpha `0.17 * edgeAlphaBoost`
- top fill white: `rgba(255, 255, 255, topWhiteAlpha)`
- sheen white: `rgba(255, 255, 255, sheenAlpha)`
