#!/usr/bin/env python3

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src" / "Nautiloid.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "Nautiloid.hpp").read_text(encoding="utf-8")


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin)
    return text[begin:finish]


class NautiloidCachePhase6ContractTest(unittest.TestCase):
    def test_current_cache_uses_immutable_shared_tile_generations(self):
        self.assertIn("struct DisplayCacheGeneration", HEADER)
        self.assertIn("shared_ptr<const DisplayCacheTileFrame>", HEADER)
        self.assertIn("DisplayCacheGenerationPtr displayCacheGeneration", HEADER)
        self.assertIn("displayCacheGenerationWithTile", SOURCE)

    def test_per_tile_full_presentation_copy_is_gone(self):
        cache_worker = section(SOURCE, "void Nautiloid::cacheWorkerLoop()", "void Nautiloid::irisWorkerLoop()")
        self.assertNotIn("displayPresentationCache", cache_worker)
        self.assertNotIn("rgb8 = displayTileCache.stitchedRgb8", cache_worker)
        self.assertIn("retainedDisplayCacheGeneration = displayCacheGeneration", cache_worker)

    def test_reprojection_reads_snapshots_outside_cache_lock(self):
        reprojection = section(
            SOURCE,
            "bool Nautiloid::publishDisplayReprojection(",
            "bool Nautiloid::publishDisplayCacheComposite(",
        )
        snapshot = reprojection.index("currentCache = displayCacheGeneration")
        output_loop = reprojection.index("for (int y = 0; y < reprojected.height; ++y)")
        self.assertLess(snapshot, output_loop)
        self.assertIn("bilinearDisplayCacheGenerationRgb(*currentCache", reprojection)
        self.assertNotIn("bilinearDisplayTileCacheRgb(displayTileCache", reprojection)

    def test_interactive_pan_prefers_cache_crop_and_defers_speculative_ahead_work(self):
        worker = section(SOURCE, "void Nautiloid::reprojectionWorkerLoop()", "void Nautiloid::workerLoop()")
        self.assertIn("publishDisplayCacheComposite(request, true)", worker)
        self.assertIn("publishDisplayReprojection(request)", worker)
        cache_worker = section(SOURCE, "void Nautiloid::cacheWorkerLoop()", "void Nautiloid::irisWorkerLoop()")
        self.assertIn("!request.zoomInteractionActive", cache_worker)

    def test_partial_composites_use_time_policy_and_final_publish(self):
        cache_worker = section(SOURCE, "void Nautiloid::cacheWorkerLoop()", "void Nautiloid::irisWorkerLoop()")
        self.assertIn("CompositePublishPolicy compositePublishPolicy", cache_worker)
        self.assertIn("shouldPublishPartial", cache_worker)
        self.assertIn("publishDisplayCacheComposite(request, true);", cache_worker)


if __name__ == "__main__":
    unittest.main()
