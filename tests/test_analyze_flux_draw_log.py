import csv
import importlib.util
import tempfile
import unittest
from pathlib import Path


SPEC = importlib.util.spec_from_file_location(
    "flux_analysis", Path(__file__).resolve().parents[1] / "tools/analyze_flux_draw_log.py")
analysis = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(analysis)


class FluxAnalysisTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name) / "capture.csv"

    def write(self, edits=None):
        rows = [dict(row=i, module_id="9007199254740993", instance_id=1,
                     module_widget_draw_us=value, preview_draw_us=value / 2,
                     ui_draw_ema_us=500, preview_tracer_mode=2,
                     preview_tracer_captures=3, preview_tracer_accepted_captures=1,
                     history_rasterizations=1, history_trails=6)
                for i, value in enumerate([1000, 10, 20, 30])]
        if edits:
            edits(rows)
        with self.path.open("w", newline="") as output:
            writer = csv.DictWriter(output, fieldnames=rows[0].keys())
            writer.writeheader()
            writer.writerows(rows)

    def test_explicit_warm_interval_preserves_scopes_and_counter_meanings(self):
        self.write()
        result = analysis.summarize(self.path, 1, 4)
        self.assertEqual(result["module_id"], "9007199254740993")
        self.assertEqual(result["selection"]["rows"], 3)
        self.assertEqual(result["cpu_timings_us"]["module_widget_draw_us"]["p50"], 20)
        self.assertAlmostEqual(result["cpu_timings_us"]["module_widget_draw_us"]["p95"], 29)
        self.assertEqual(result["counters"]["preview_tracer_captures"]["total"], 9)
        self.assertEqual(result["counters"]["preview_tracer_accepted_captures"]["total"], 3)
        self.assertNotIn("history_trails", result["counters"])
        self.assertNotIn("ui_draw_ema_us", result["cpu_timings_us"])
        self.assertIn("ui_draw_ema_us", result["smoothed_timings_us"])
        self.assertEqual(analysis.summarize(self.path)["cpu_timings_us"]["module_widget_draw_us"]["max"], 1000)

    def test_function_pilot_counters_and_phases(self):
        self.write(lambda rows: [row.update(ch1_function_requested=1,
            ch1_function_used=1, ch1_function_updated=1, ch1_function_cache_hit=0,
            ch1_function_fallback=0, ch1_function_capture_us=7) for row in rows])
        result = analysis.summarize(self.path, 1, 4)
        self.assertEqual(result["settings"]["ch1_function_requested"], [1])
        self.assertEqual(result["counters"]["ch1_function_updated"]["total"], 3)
        self.assertEqual(result["cpu_timings_us"]["ch1_function_capture_us"]["p50"], 7)

    def test_host_bridge_backend_and_actual_strokes(self):
        def update(rows):
            for i, row in enumerate(rows):
                row.update(contour_backend=2, ch1_bridge_strokes=2 if i == 1 else 0,
                           ch1_bridge_fallbacks=1 if i == 2 else 0, ch1_bridge_us=3)
        self.write(update)
        result = analysis.summarize(self.path)
        self.assertEqual(result["settings"]["contour_backend"], [2])
        self.assertEqual(result["counters"]["ch1_bridge_strokes"]["total"], 2)
        self.assertEqual(result["counters"]["ch1_bridge_fallbacks"]["total"], 1)
        self.assertEqual(result["cpu_timings_us"]["ch1_bridge_us"]["p50"], 3)

    def test_missing_counters_are_not_zero(self):
        self.write()
        result = analysis.summarize(self.path)
        self.assertIn("halo_gl_surface_framebuffer_draws", result["missing_columns"])
        self.assertNotIn("halo_gl_surface_framebuffer_draws", result["counters"])

    def test_round_stroke_selection_is_distinct_from_actual_work(self):
        def experiment(rows):
            for i, row in enumerate(rows):
                row.update(round_stroke_enabled=1, round_stroke_private_draws=2 if i == 1 else 0,
                           round_stroke_fallbacks=1 if i == 2 else 0, round_stroke_surface_us=i)
        self.write(experiment)
        result = analysis.summarize(self.path)
        self.assertEqual(result["settings"]["round_stroke_enabled"], [1])
        self.assertEqual(result["counters"]["round_stroke_private_draws"]["total"], 2)
        self.assertEqual(result["counters"]["round_stroke_fallbacks"]["total"], 1)
        self.assertIn("round_stroke_surface_us", result["cpu_timings_us"])

    def test_mixed_modes_warn(self):
        self.write(lambda rows: rows[2].update(preview_tracer_mode=0))
        result = analysis.summarize(self.path)
        self.assertTrue(any("settings changed" in warning for warning in result["warnings"]))

    def test_truncated_capture_is_rejected(self):
        self.write()
        with self.path.open("a") as output:
            output.write("4,12,1,99")
        with self.assertRaisesRegex(ValueError, "incomplete"):
            analysis.summarize(self.path)

    def test_invalid_numeric_data_is_rejected(self):
        for value in ["nan", "inf", -1]:
            with self.subTest(value=value):
                self.write(lambda rows: rows[1].update(preview_draw_us=value))
                with self.assertRaisesRegex(ValueError, "non-finite or negative"):
                    analysis.summarize(self.path)

    def test_concatenated_instances_and_restarted_rows_are_rejected(self):
        self.write(lambda rows: rows[2].update(instance_id=2))
        with self.assertRaisesRegex(ValueError, "one module instance"):
            analysis.summarize(self.path)
        self.write(lambda rows: rows[2].update(row=0))
        with self.assertRaisesRegex(ValueError, "strictly increase"):
            analysis.summarize(self.path)

    def test_empty_selection_is_rejected(self):
        self.write()
        with self.assertRaisesRegex(ValueError, "no rows"):
            analysis.summarize(self.path, 10)


if __name__ == "__main__":
    unittest.main()
