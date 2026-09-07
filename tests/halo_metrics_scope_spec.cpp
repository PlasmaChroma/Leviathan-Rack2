#include "../src/visual/HaloKnob2Metrics.hpp"
#include <cassert>
#include <cstdio>
int main() {
    using namespace visual_assets;
    HaloKnob2DrawMetrics a, b;
    {
        ScopedHaloKnob2Metrics scope(a);
        haloKnob2DrawMetrics().stepSurfaceNs += 120;
        ++haloKnob2DrawMetrics().glSurfaceFramebufferDraws;
        {
            ScopedHaloKnob2Metrics nested(b);
            haloKnob2DrawMetrics().stepSurfaceNs += 700;
        }
        haloKnob2DrawMetrics().nanoVgSurfaceDrawNs += 30;
    }
    { ScopedHaloKnob2Metrics idle(b); b = {}; }
    assert(a.stepSurfaceNs == 120 && a.glSurfaceFramebufferDraws == 1 && a.nanoVgSurfaceDrawNs == 30);
    HaloKnob2DrawMetrics frame = a; a = {};
    { ScopedHaloKnob2Metrics draw(frame); haloKnob2DrawMetrics().nanoVgSurfaceDrawNs += 20; }
    assert(frame.nanoVgSurfaceDrawNs == 50 && frame.stepSurfaceNs == 120);
    assert(a.stepSurfaceNs == 0 && b.stepSurfaceNs == 0);
    assert(&haloKnob2DrawMetrics() != &a && &haloKnob2DrawMetrics() != &frame);
    std::puts("Halo metrics scopes: owner isolation, nested scopes, step/draw accumulation passed");
}
