# Private contour NanoVG

Based on VCVRack/nanovg commit `0bebdb314aff9cfa28fde4744bcb037a2b3fd756`,
the NanoVG dependency of Rack 2.6.6. Upstream source notices are retained.
The GL backend and public header match that SDK byte-for-byte.

Local changes:

- `nanovg.inc` is an explicitly altered copy of `nanovg.c`. It keeps the
  original round-join implementation as a fallback and adds conservative endpoint
  reuse for two-sample arcs. This was first validated in the offline full-stroke
  experiment documented in `doc/flux-full-stroke-candidate.md`.
- `rename.h` prefixes public NanoVG and Fontstash names with `luminPrivate_`.
  `declarations.inc` contains renamed public declarations using Rack's shared POD
  types. No installed Rack function or callback is replaced.
- Fontstash finishes its font backend before freeing the context, fixing the
  original teardown ordering warning. STB functions have private linkage.
- The owning translation unit redirects GL object deletion to the plugin's
  existing context-lease retirement service. Image decoding is disabled.

This code is now used only by explicitly built offline experiments. The failed
Integral Flux live integration was removed from the normal plugin build.
Private rendering occurs inside `AdaptiveGlSurface`'s state guard and managed
framebuffers. It is not a general replacement for Rack's NanoVG API.
