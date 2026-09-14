import io
import unittest
from unittest.mock import Mock, patch

import render
from model import DebugState


class TimingDisplayTests(unittest.TestCase):
    def test_average_is_measured_and_legacy_ranges_are_unavailable(self):
        data = {"draw_us": "1.00-10.00", "draw_us_avg": 4.0, "rows": 7}
        self.assertEqual(render._metric_cell(data, "draw_us"), "1.00-10.00")
        self.assertEqual(render._metric_cell(data, "draw_us", True), "4.00")
        self.assertEqual(render._metric_cell(data, "rows", True), "7")
        for missing in (None, float("nan"), float("inf")):
            data["draw_us_avg"] = missing
            self.assertEqual(render._metric_cell(data, "draw_us", True), "-")
        del data["draw_us_avg"]
        self.assertEqual(render._metric_cell(data, "draw_us", True), "-")
        data["draw_us_avg"] = 0.0
        self.assertEqual(render._metric_cell(data, "draw_us", True), "0.00")

    def test_schema_packet_to_both_renderers_and_toggle_back(self):
        state = DebugState()
        event = dict(plugin="Leviathan", module="IntegralFlux", instance="1", stream="ui", ts=0)
        state.ingest_event(dict(event, kind="schema", data={"columns": [{"key": "draw_us", "label": "Draw (us)"}]}))
        state.ingest_event(dict(event, kind="metric", data={"draw_us": "1.00-10.00", "draw_us_avg": 4.0}))
        view = render.ModuleViewState()
        for mode, number in (("Range", "1.00-10.00"), ("Average", "4.00"), ("Range", "1.00-10.00")):
            plain = render.build_plain_text(state.snapshot(), "localhost", 8765, view)
            self.assertIn("Timing = " + mode, plain)
            self.assertIn(number, plain)
            if render.Console is not None:
                output = io.StringIO()
                render.Console(file=output, width=120).print(render.build_table(state.snapshot(), "localhost", 8765, view))
                self.assertIn(number, output.getvalue())
                self.assertIn("Timing = " + mode, output.getvalue())
            view.handle_key(render.KEY_AVERAGE)

    def test_windows_keys(self):
        keyboard = Mock()
        keyboard.kbhit.return_value = True
        with patch.object(render, "msvcrt", keyboard), patch.object(render.sys, "stdin") as stdin:
            stdin.isatty.return_value = True
            for char in ("a", "A"):
                keyboard.getwch.return_value = char
                self.assertEqual(render._read_key_nonblocking(), render.KEY_AVERAGE)
            keyboard.getwch.return_value = "\x1b"
            self.assertIsNone(render._read_key_nonblocking())
            stdin.read.assert_not_called()


if __name__ == "__main__":
    unittest.main()
