Yes. The current halo optimization helps, but there are still a few real levers in TD.Scope that should buy back UI time without
  materially trashing the look.

  The biggest one is row density. Right now the display is hard-coded to 2x vertical supersampling in src/TDScope.cpp:17, and rowCount is
  derived directly from that in src/TDScope.cpp:817. That doubles everything downstream: main strokes, connectors, halo opportunities, boost
  strokes, and the transient rebuild work. If you make that adaptive instead of fixed, you’ll get the cleanest win. I’d keep 2.0 only when
  zoomed in or when halo is off, and run more like 1.25 to 1.5 at normal rack zoom with halo on. That should preserve the overall character
  while cutting a lot of stroke count.

  The next likely win is connectors. In src/TDScope.cpp:1484 every valid row pair draws two connector segments, every frame, even when
  adjacent rows barely differ. Visually, those connectors matter most where the shape bends or breaks. You can skip them when prevX0/x0 and
  prevX1/x1 are within a tiny threshold, or decimate them in dense layouts the same way we now decimate some halo rows. That is lower risk
  than touching the main trace.

  After that, there are two smaller-but-safe optimizations:

  - Color lookup caching. gradientColorForIntensity() is doing a full scheme switch and interpolation per row in the hot draw loop at src/
    TDScope.cpp:1228. A 256-entry LUT per color scheme would remove most of that CPU work.
  - Skip the boost pass more aggressively. The extra highlight stroke at src/TDScope.cpp:1473 is another full stroke for bright rows. You
    could keep it only for very strong peaks, or disable it in dense layouts.

  If you want the best payoff order, I’d do it like this:

  1. Adaptive supersample factor.
  2. Connector skip/decimation for near-identical adjacent rows.
  3. Color LUT.
  4. Optional boost-pass gating.

  If you want, I can implement 1 and 2 together next. That’s the pair most likely to noticeably improve “3 scopes with halo” without making
  the scope look cheap.