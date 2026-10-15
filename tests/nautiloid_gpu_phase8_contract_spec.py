#!/usr/bin/env python3

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WIDGET = (ROOT / "src" / "NautiloidWidget.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "Nautiloid.hpp").read_text(encoding="utf-8")
MODULE = (ROOT / "src" / "Nautiloid.cpp").read_text(encoding="utf-8")
POLICY = (ROOT / "src" / "NautiloidGpuPrecision.hpp").read_text(encoding="utf-8")


class NautiloidGpuPhase8ContractTest(unittest.TestCase):
    def test_precision_selection_uses_actual_world_pixel_size(self):
        self.assertIn("2.0 * halfSpanX / double(framebufferWidth)", POLICY)
        self.assertIn("2.0 * halfSpanY / double(framebufferHeight)", POLICY)
        self.assertIn("maxErrorInPixels = 0.25", POLICY)
        self.assertIn("requiresDeepPrecision(", WIDGET)

    def test_center_and_span_are_uploaded_as_high_low_pairs(self):
        for uniform in (
            "uCenterHi",
            "uCenterLo",
            "uHalfSpanHi",
            "uHalfSpanLo",
        ):
            self.assertIn(uniform, WIDGET)
        self.assertIn("splitDouble(module->fractalCenterX)", WIDGET)
        self.assertIn("splitDouble(halfSpanX)", WIDGET)

    def test_double_single_math_survives_through_orbit_operations(self):
        for helper in (
            "vec2 dsAdd(",
            "vec2 dsSub(",
            "vec2 dsMul(",
            "vec2 dsDiv(",
            "vec2 dsPixelCoordinate(",
        ):
            self.assertIn(helper, WIDGET)
        deep_start = WIDGET.index("void main() {\n#if NAUTILOID_DEEP_PRECISION")
        fast_start = WIDGET.index("#else", deep_start)
        deep_main = WIDGET[deep_start:fast_start]
        self.assertIn("dsSquare(zr)", deep_main)
        self.assertIn("dsSquare(zi)", deep_main)
        self.assertIn("NAUTILOID_MODE == 11", deep_main)
        self.assertIn("NAUTILOID_MODE == 14", deep_main)

    def test_variants_are_lazy_and_context_validated(self):
        self.assertIn("ShaderVariant fast;", WIDGET)
        self.assertIn("ShaderVariant deep;", WIDGET)
        self.assertIn("deepPrecisionRequested && ensureShaderReady(mode, true)", WIDGET)
        self.assertIn("modeProgram.deep.ready", WIDGET)
        self.assertIn("deepInvalid", WIDGET)

    def test_failed_deep_variant_falls_back_to_fast_shader(self):
        self.assertIn("if (!useDeepPrecision && !ensureShaderReady(mode, false))", WIDGET)
        self.assertIn("useDeepPrecision ? modeProgram.deep : modeProgram.fast", WIDGET)

    def test_precision_activity_is_visible_in_telemetry(self):
        self.assertIn("gpuDeepPrecisionActive", HEADER)
        self.assertIn("gpuDeepPrecisionRenders", HEADER)
        self.assertIn("gpu_deep_precision_active", WIDGET)
        self.assertIn("gpu_deep_precision_renders", WIDGET)

    def test_experiment_is_user_toggleable_and_defaults_off(self):
        self.assertIn("gpuDeepPrecisionEnabled {false}", HEADER)
        self.assertIn("Deep precision at extreme zoom (experimental)", WIDGET)
        self.assertIn("module->gpuDeepPrecisionEnabled.load", WIDGET)
        self.assertIn('jsonBoolOr(root, "gpuDeepPrecisionEnabled", false)', MODULE)


if __name__ == "__main__":
    unittest.main()
