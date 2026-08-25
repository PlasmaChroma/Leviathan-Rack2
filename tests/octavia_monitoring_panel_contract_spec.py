#!/usr/bin/env python3

import re
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
PANEL_PATH = ROOT / "res" / "Octavia.svg"
PANEL = ET.parse(PANEL_PATH).getroot()


def element_by_id(wanted: str) -> ET.Element:
    return next(element for element in PANEL.iter() if element.attrib.get("id") == wanted)


class OctaviaMonitoringPanelContractTest(unittest.TestCase):
    def test_panel_uses_the_ten_hp_phase_one_width(self):
        self.assertEqual(PANEL.attrib["width"], "50.8mm")
        self.assertEqual(PANEL.attrib["viewBox"], "0 0 50.8 128.5")

    def test_master_and_monitor_semantic_anchors_exist(self):
        required = {
            "TITLE_LABEL", "OCTOPUS_STATUS", "READ_ACTIVITY_LIGHT",
            "WRITE_ACTIVITY_LIGHT", "START_PARAM", "LOUDNESS_METERS",
            "MASTER_L_INPUT", "MASTER_R_INPUT", "MASTER_L_LABEL", "MASTER_R_LABEL",
        }
        for name in "ABCD":
            required.update({
                f"MONITOR_{name}_INPUT", f"MONITOR_{name}_LIGHT", f"MONITOR_{name}_LABEL"
            })
        for anchor in required:
            self.assertEqual(element_by_id(anchor).attrib["id"], anchor)

    def test_input_ids_preserve_master_and_append_monitors(self):
        self.assertRegex(SOURCE, r"MASTER_L_INPUT\s*=\s*0")
        self.assertRegex(SOURCE, r"MASTER_R_INPUT\s*=\s*1")
        self.assertIn("MONITOR_A_INPUT == 2", SOURCE)
        self.assertIn("MONITOR_D_INPUT == 5", SOURCE)
        self.assertIn("INPUTS_LEN == 6", SOURCE)

    def test_light_ids_preserve_legacy_prefix_and_append_monitors(self):
        legacy = re.search(
            r"STATUS_R_LIGHT\s*,\s*STATUS_G_LIGHT\s*,\s*STATUS_B_LIGHT\s*,"
            r"\s*READ_ACTIVITY_LIGHT\s*,\s*WRITE_ACTIVITY_LIGHT", SOURCE
        )
        self.assertIsNotNone(legacy)
        self.assertIn("MONITOR_A_LIGHT == 5", SOURCE)
        self.assertIn("MONITOR_D_LIGHT == 8", SOURCE)
        self.assertIn("LIGHTS_LEN == 9", SOURCE)

    def test_all_ports_and_lights_are_configured_and_instantiated(self):
        for name in "ABCD":
            self.assertIn(f'configInput(MONITOR_{name}_INPUT, "Monitor {name}")', SOURCE)
            self.assertIn(f'configLight(MONITOR_{name}_LIGHT, "Monitor {name} attention")', SOURCE)
        self.assertIn('configInput(MASTER_L_INPUT, "Master L")', SOURCE)
        self.assertIn('configInput(MASTER_R_INPUT, "Master R")', SOURCE)
        self.assertIn("Octavia::MONITOR_A_INPUT + monitor", SOURCE)
        self.assertIn("Octavia::MONITOR_A_LIGHT + monitor", SOURCE)

    def test_phase_one_led_baseline_is_off_or_dim_connected(self):
        self.assertIn("inputs[MONITOR_A_INPUT + monitor].isConnected()", SOURCE)
        self.assertIn("setBrightness(connected ? 0.12f : 0.f)", SOURCE)

    def test_widened_panel_has_both_right_edge_screws(self):
        self.assertEqual(SOURCE.count("box.size.x - RACK_GRID_WIDTH"), 2)


if __name__ == "__main__":
    unittest.main()
