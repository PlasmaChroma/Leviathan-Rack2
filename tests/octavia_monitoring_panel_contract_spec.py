#!/usr/bin/env python3

import re
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
OBSERVATION = (ROOT / "src" / "OctaviaObservation.hpp").read_text(encoding="utf-8")
MEASUREMENT = (ROOT / "src" / "OctaviaMeasurement.hpp").read_text(encoding="utf-8")
MEASUREMENT_IMPL = (ROOT / "src" / "OctaviaMeasurement.cpp").read_text(encoding="utf-8")
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
        self.assertIn("const float idle = connected ? 0.12f : 0.f", SOURCE)
        self.assertIn("std::max(idle, monitorActivityEnvelope[monitor])", SOURCE)

    def test_widened_panel_has_both_right_edge_screws(self):
        self.assertEqual(SOURCE.count("box.size.x - RACK_GRID_WIDTH"), 2)

    def test_phase_two_routes_use_the_shared_observation_substrate(self):
        self.assertIn('svr.Get("/audio/monitors"', SOURCE)
        self.assertIn('svr.Post("/audio/snapshot"', SOURCE)
        self.assertIn('svr.Post("/audio/analyze"', SOURCE)
        self.assertIn('svr.Post("/audio/compare"', SOURCE)
        self.assertIn("AnalysisEngine analysisEngine", SOURCE)
        self.assertIn("levelNormalizedBandsDb", SOURCE)
        self.assertIn('jStr("activeAnalysisUsers")', SOURCE)
        self.assertIn('jStr("snapshotGeneration")', SOURCE)
        self.assertIn("analyzeLatestMaster", SOURCE)
        self.assertEqual(SOURCE.count('svr.Get(R"(/audio/(\\d+))"'), 1)
        self.assertEqual(SOURCE.count('svr.Get(R"(/audio/(\\d+)/analyze)"'), 1)
        self.assertNotIn("/retired/audio/", SOURCE)
        self.assertIn('#include "OctaviaObservationBus.hpp"', SOURCE)
        self.assertIn('svr.Get("/audio/triggered-snapshots"', SOURCE)
        self.assertIn("snapshotPool.createAt(trigger.triggerFrame", SOURCE)
        self.assertIn('/audio/snapshot/(\\d+)', SOURCE)
        self.assertIn("observationHistory.publish", SOURCE)
        self.assertNotIn("AudioRingBuf", SOURCE)
        self.assertNotIn("audioRing[", SOURCE)

    def test_phase_two_history_and_pool_are_bounded(self):
        self.assertIn("OBSERVATION_HISTORY_FRAMES = 262144", OBSERVATION)
        self.assertIn("SNAPSHOT_POOL_LIMIT = 12", OBSERVATION)

    def test_phase_three_continuous_meter_is_slimmed(self):
        self.assertIn("MASTER_METER_BLOCKS = 32", SOURCE)
        for removed in ("resetFlag", "pKSum", "pRawSum", "pClipped", "pSumLR", "pN"):
            self.assertNotIn(removed, SOURCE)
        self.assertIn("masterMeasurement.process", SOURCE)

    def test_phase_three_legacy_routes_arm_triggered_sessions(self):
        self.assertIn("masterMeasurement.arm(0, true", SOURCE)
        self.assertIn("measurement_busy", MEASUREMENT_IMPL)
        self.assertIn("requestedDurationFrames_", MEASUREMENT)


if __name__ == "__main__":
    unittest.main()
