#!/usr/bin/env python3

import re
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
OBSERVATION = (ROOT / "src" / "OctaviaObservation.hpp").read_text(encoding="utf-8")
RECORDING = (ROOT / "src" / "OctaviaRecording.hpp").read_text(encoding="utf-8")
MCP_SOURCE = (ROOT / "MCP" / "mcp_server" / "Octavia_MCP.py").read_text(encoding="utf-8")
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
        required.update({
            "CONTROL_A_OUTPUT", "CONTROL_B_OUTPUT",
            "CONTROL_A_LABEL", "CONTROL_B_LABEL",
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

    def test_control_outputs_are_append_only_polyphonic_ports(self):
        self.assertIn("CONTROL_A_OUTPUT == 0", SOURCE)
        self.assertIn("CONTROL_B_OUTPUT == 1", SOURCE)
        self.assertIn("OUTPUTS_LEN == 2", SOURCE)
        self.assertIn('configOutput(CONTROL_A_OUTPUT, "Control A (16-channel polyphonic)")', SOURCE)
        self.assertIn('configOutput(CONTROL_B_OUTPUT, "Control B (16-channel polyphonic)")', SOURCE)
        self.assertIn("ControlOutputFrame controlOutput", SOURCE)
        self.assertIn("setChannels(channels)", SOURCE)
        self.assertIn('jStr("controls")', SOURCE)
        self.assertIn("controlOutputConnected[port]", SOURCE)

    def test_all_ports_and_lights_are_configured_and_instantiated(self):
        for name in "ABCD":
            self.assertIn(f'configInput(MONITOR_{name}_INPUT, "Monitor {name}")', SOURCE)
            self.assertIn(f'configLight(MONITOR_{name}_LIGHT, "Monitor {name} attention")', SOURCE)
        self.assertIn('configInput(MASTER_L_INPUT, "Master L")', SOURCE)
        self.assertIn('configInput(MASTER_R_INPUT, "Master R")', SOURCE)
        self.assertIn("Octavia::MONITOR_A_INPUT + monitor", SOURCE)
        self.assertIn("Octavia::MONITOR_A_LIGHT + monitor", SOURCE)

    def test_server_auto_start_uses_ui_lifecycle_and_atomic_single_attempt(self):
        self.assertIn("if (module) module->startServer();", SOURCE)
        self.assertIn("serverRunning.compare_exchange_strong", SOURCE)

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
        self.assertNotIn("analyzeLatestMaster", SOURCE)
        self.assertNotIn('svr.Get(R"(/audio/(\\d+))"', SOURCE)
        self.assertNotIn('svr.Get(R"(/audio/(\\d+)/analyze)"', SOURCE)
        self.assertNotIn('svr.Get("/audio/loudness"', SOURCE)
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
        self.assertNotIn("masterMeasurement", SOURCE)

    def test_all_named_inputs_share_the_modern_analysis_surface(self):
        self.assertIn('MonitorName = Literal["masterL", "masterR", "A", "B", "C", "D"]', MCP_SOURCE)
        self.assertIn('name="vcv_octavia_analyze_snapshot"', MCP_SOURCE)
        self.assertIn('name="vcv_octavia_start_analysis_capture"', MCP_SOURCE)
        self.assertNotIn('name="vcv_analyze_audio"', MCP_SOURCE)
        self.assertNotIn('name="vcv_reset_loudness"', MCP_SOURCE)

    def test_bounded_recording_stays_on_physical_observation_ports(self):
        self.assertIn('#include "OctaviaRecording.hpp"', SOURCE)
        self.assertIn("recordingEngine.process", SOURCE)
        self.assertIn('svr.Post("/audio/recording"', SOURCE)
        self.assertIn('/audio/recording/(\\d+)', SOURCE)
        self.assertIn("RECORDING_MIN_SECONDS = 0.1", RECORDING)
        self.assertIn("RECORDING_MAX_SECONDS = 30.0", RECORDING)
        self.assertIn("preallocated planar storage", RECORDING)

    def test_recording_is_exposed_as_start_and_poll_mcp_tools(self):
        self.assertIn('name="vcv_octavia_start_recording"', MCP_SOURCE)
        self.assertIn('name="vcv_octavia_get_recording"', MCP_SOURCE)
        self.assertIn('ge=0.1, le=30.0', MCP_SOURCE)
        self.assertIn('_envelope_call("audio/recording", "POST", payload)', MCP_SOURCE)
        self.assertIn("class ControlProgramInput", MCP_SOURCE)
        self.assertIn("class ControlEventInput", MCP_SOURCE)
        self.assertIn("control: Optional[ControlProgramInput]", MCP_SOURCE)

    def test_analysis_capture_is_ephemeral_by_default(self):
        self.assertIn('svr.Post("/audio/capture"', SOURCE)
        self.assertIn('/audio/capture/(\\d+)', SOURCE)
        self.assertIn("CaptureDisposition::AnalyzeAndRecord", SOURCE)
        self.assertIn("CaptureDisposition::Analyze", SOURCE)
        self.assertIn('name="vcv_octavia_start_analysis_capture"', MCP_SOURCE)
        self.assertIn('name="vcv_octavia_get_analysis_capture"', MCP_SOURCE)
        self.assertIn("save: bool = False", MCP_SOURCE)
        self.assertIn('_envelope_call("audio/capture", "POST", payload)', MCP_SOURCE)


if __name__ == "__main__":
    unittest.main()
