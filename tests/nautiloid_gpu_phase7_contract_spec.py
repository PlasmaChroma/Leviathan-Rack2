#!/usr/bin/env python3

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WIDGET = (ROOT / "src" / "NautiloidWidget.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "Nautiloid.hpp").read_text(encoding="utf-8")
SURFACE = (ROOT / "src" / "visual" / "AdaptiveGlSurface.cpp").read_text(encoding="utf-8")
SURFACE_HEADER = (ROOT / "src" / "visual" / "AdaptiveGlSurface.hpp").read_text(encoding="utf-8")


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin)
    return text[begin:finish]


class NautiloidGpuPhase7ContractTest(unittest.TestCase):
    def test_nautiloid_uses_shared_adaptive_surface(self):
        preview = section(WIDGET, "struct NautiloidGlPreview", "struct NautiloidDisplay")
        self.assertIn('#include "visual/AdaptiveGlSurface.hpp"', WIDGET)
        self.assertIn("visual_assets::AdaptiveGlSurface adaptiveSurface;", preview)
        self.assertIn("adaptiveSurface.renderIfNeeded(", preview)
        self.assertIn("adaptiveSurface.draw(args, box.size)", preview)

    def test_surface_has_quantized_density_policy_and_nautiloid_specific_capacity(self):
        self.assertIn("float minDensity = 0.25f;", SURFACE_HEADER)
        self.assertIn("float maxDensity = 2.f;", SURFACE_HEADER)
        self.assertIn("int sizeQuantum = 16;", SURFACE_HEADER)
        self.assertIn("logicalSize.x * maxDensity", SURFACE)
        self.assertIn("((requested + quantum - 1) / quantum) * quantum", SURFACE)
        self.assertIn("policy.maxDensity = 4.f;", WIDGET)

    def test_only_active_prefix_is_rendered_and_sampled(self):
        render = section(WIDGET, "void renderGlContent(", "void drawFramebuffer() override")
        self.assertIn("glViewport(0, viewportY, activeWidth, activeHeight);", render)
        self.assertIn("glScissor(0, viewportY, activeWidth, activeHeight);", render)
        self.assertIn("capacityHeight - activeHeight", SURFACE)
        self.assertIn("frontActiveWidth", SURFACE)

    def test_context_lifecycle_does_not_reuse_surface_names(self):
        preview = section(WIDGET, "struct NautiloidGlPreview", "struct NautiloidDisplay")
        self.assertIn("adaptiveSurface.reset(false);", preview)
        self.assertIn("adaptiveSurface.reset(ownerVg != nullptr && ownerVg == e.vg);", preview)

    def test_fractal_zoom_and_rack_density_remain_separate(self):
        render = section(WIDGET, "void renderGlContent(", "void drawFramebuffer() override")
        adaptive = section(WIDGET, "void renderAdaptiveSurfaceIfNeeded()", "void draw(const DrawArgs& args)")
        self.assertIn("module->fractalZoom", render)
        self.assertNotIn("rackZoom", render)
        self.assertIn("rackScroll->getZoom()", adaptive)
        self.assertNotIn("fractalZoom", adaptive)

    def test_gpu_surface_telemetry_is_published(self):
        for field in (
            "gpuSurfaceRenders",
            "gpuSurfaceRenderUs",
            "gpuSurfaceDensity",
            "gpuSurfaceActiveWidth",
            "gpuSurfaceActiveHeight",
            "gpuSurfaceCapacityWidth",
            "gpuSurfaceCapacityHeight",
        ):
            self.assertIn(field, HEADER)
            self.assertIn(field, WIDGET)
        self.assertIn("gpuSurfaceRenders != lastLoggedGpuSurfaceRenders", WIDGET)


if __name__ == "__main__":
    unittest.main()
