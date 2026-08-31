#!/usr/bin/env python3

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
HELPER = (ROOT / "src" / "NvgGraphicsLifecycle.cpp").read_text(encoding="utf-8")
IRIS_WIDGET = (ROOT / "src" / "IrisWidget.cpp").read_text(encoding="utf-8")
NAUT_WIDGET = (ROOT / "src" / "NautiloidWidget.cpp").read_text(encoding="utf-8")
IRIS_HEADER = (ROOT / "src" / "Iris.hpp").read_text(encoding="utf-8")
NAUT_HEADER = (ROOT / "src" / "Nautiloid.hpp").read_text(encoding="utf-8")


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin)
    return text[begin:finish]


class IrisNautiloidNvgPhase5ContractTest(unittest.TestCase):
    def test_shared_helper_updates_stable_images_and_recreates_when_needed(self):
        body = section(
            HELPER,
            "bool updateOwnedNvgImageRgba(",
            "bool clearCacheOnContextSwitch(",
        )
        self.assertIn("ownerVg != currentVg", body)
        self.assertIn("ownedNvgImageSizeMatches", body)
        self.assertIn("nvgUpdateImage", body)
        self.assertIn("nvgCreateImageRGBA", body)

    def test_all_three_phase5_displays_use_the_shared_update_path(self):
        iris_display = section(IRIS_WIDGET, "struct IrisDisplay", "struct IrisScanLineOverlay")
        naut_display = section(NAUT_WIDGET, "struct NautiloidDisplay", "struct NautiloidZoomSpeedQuantity")
        mini_display = section(NAUT_WIDGET, "struct NautiloidIrisMiniDisplay", "struct NautiloidTileCacheGrid")
        for body in (iris_display, naut_display, mini_display):
            self.assertIn("updateOwnedNvgImageRgba", body)
            self.assertNotIn("nvgCreateImageRGBA", body)

    def test_raster_displays_are_not_wrapped_in_outer_framebuffers(self):
        self.assertNotIn("displayFb", IRIS_WIDGET)
        self.assertNotIn("displayFb", NAUT_WIDGET)

    def test_widgets_acquire_immutable_presentation_snapshots(self):
        self.assertIn("previewPixelsSnapshot", IRIS_HEADER)
        self.assertIn("sourceFieldSnapshot", IRIS_HEADER)
        self.assertIn("previewSourceSnapshot", NAUT_HEADER)
        self.assertIn("module->previewPixelsSnapshot", IRIS_WIDGET)
        self.assertIn("module->previewSourceSnapshot", NAUT_WIDGET)
        self.assertIn("module->irisExpanderOwnedSourceSnapshot", NAUT_WIDGET)


if __name__ == "__main__":
    unittest.main()
