#!/usr/bin/env python3

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ET.parse(ROOT / "res" / "Moirai.svg").getroot()
PANEL = ET.parse(ROOT / "res" / "Moirai.panel.svg").getroot()
WIDGET = (ROOT / "src" / "MoiraiWidget.cpp").read_text(encoding="utf-8")


def ids(root):
    return {element.attrib.get("id") for element in root.iter() if element.attrib.get("id")}


class MoiraiPanelContractTest(unittest.TestCase):
    def test_master_and_generated_panel_have_every_runtime_anchor(self):
        required = {
            "MOIRAI_DISPLAY", "LANE_PARAM", "CHANNEL_PARAM", "MANUAL_TRIGGER_PARAM",
            "TIME_PARAM", "CURVE_PARAM", "LEVEL_PARAM", "LANE_A_LIGHT", "LANE_B_LIGHT",
            "GATE_INPUT", "VELOCITY_INPUT", "M1_INPUT", "M2_INPUT", "M3_INPUT",
            "CLOCK_INPUT", "RESET_INPUT", "A_OUTPUT", "EOC_A_OUTPUT", "B_OUTPUT", "EOC_B_OUTPUT",
        }
        self.assertTrue(required <= ids(MASTER))
        self.assertTrue(required <= ids(PANEL))

    def test_widget_uses_svg_anchors_without_coordinate_fallbacks(self):
        self.assertIn('loadPointFromSvgMm(panelPath, id, &point)', WIDGET)
        self.assertIn('loadRectFromSvgMm(panelPath, "MOIRAI_DISPLAY", &displayMm)', WIDGET)
        self.assertNotIn("fallbackMm", WIDGET)
        self.assertNotIn("inputX[", WIDGET)
        self.assertNotIn("outputX[", WIDGET)

    def test_display_uses_coherent_snapshot_and_runtime_program_identity(self):
        self.assertIn("module->readTelemetry()", WIDGET)
        self.assertIn("selected.assignments[channel]", WIDGET)
        self.assertNotIn('nvgText(args.vg, 7.f, 32.f, "factory_adsr"', WIDGET)

    def test_factory_menu_uses_semantic_edit_and_one_history_action(self):
        self.assertIn('"apply_preset"', WIDGET)
        self.assertIn('"assign_program"', WIDGET)
        self.assertIn("OctaviaSemanticControl::Operation::EDIT", WIDGET)
        self.assertIn('action->name = "apply Moirai factory preset"', WIDGET)


if __name__ == "__main__":
    unittest.main()
