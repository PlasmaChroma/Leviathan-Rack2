#!/usr/bin/env python3

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ET.parse(ROOT / "res" / "Phonex.svg").getroot()
PANEL = ET.parse(ROOT / "res" / "Phonex.panel.svg").getroot()
LABELS = ET.parse(ROOT / "res" / "Phonex.labels.svg").getroot()
WIDGET = (ROOT / "src" / "PhonexWidget.cpp").read_text(encoding="utf-8")


def ids(root):
    return {element.attrib.get("id") for element in root.iter() if element.attrib.get("id")}


class PhonexPanelContractTest(unittest.TestCase):
    def test_master_is_exactly_fourteen_hp(self):
        self.assertEqual(MASTER.attrib["width"], "71.12mm")
        self.assertEqual(MASTER.attrib["height"], "128.5mm")
        self.assertEqual(MASTER.attrib["viewBox"], "0 0 7112 12850")

    def test_master_and_runtime_panel_contain_all_anchors(self):
        params = {
            "PITCH_PARAM", "FORMANT_PARAM", "SPEED_PARAM", "WARP_PARAM",
            "EXCITE_BLEND_PARAM", "BEND_PARAM", "GLITCH_PARAM", "WORD_PARAM",
            "WORD_PUSH_PARAM", "BANK_PARAM",
        }
        inputs = {
            "VOCT_INPUT", "TRIG_GATE_INPUT", "SCRUB_CV_INPUT", "WARP_CV_INPUT",
            "BEND_CV_INPUT", "EXT_EXCITE_INPUT", "WORD_CV_INPUT",
        }
        outputs = {"AUDIO_OUTPUT", "FRAME_CLK_OUTPUT", "EOX_OUTPUT"}
        lights = {"VOICED_LIGHT", "FRAME_LIGHT", "EOX_LIGHT", "BEND_LIGHT"}
        regions = {"UTTERANCE_FIELD", "PHRASE_DISPLAY", "WORD_SELECTOR"}
        required = params | inputs | outputs | lights | regions
        self.assertEqual(len(params), 10)
        self.assertEqual(len(inputs), 7)
        self.assertEqual(len(outputs), 3)
        self.assertEqual(len(lights), 4)
        self.assertTrue(required <= ids(MASTER))
        self.assertTrue(required <= ids(PANEL))

    def test_voiced_light_sits_beside_speak_button(self):
        anchors = {
            element.attrib["id"]: element.attrib
            for element in MASTER.iter()
            if element.attrib.get("id") in {"VOICED_LIGHT", "WORD_PUSH_PARAM"}
        }
        self.assertEqual(anchors["VOICED_LIGHT"]["cy"], anchors["WORD_PUSH_PARAM"]["cy"])
        self.assertEqual(anchors["VOICED_LIGHT"]["cx"], "6760")

    def test_bank_switch_has_a_dedicated_block_beside_word_selector(self):
        anchors = {
            element.attrib["id"]: element.attrib
            for element in MASTER.iter()
            if element.attrib.get("id") in {"WORD_SELECTOR", "BANK_PARAM"}
        }
        selector_right = int(anchors["WORD_SELECTOR"]["x"]) + int(
            anchors["WORD_SELECTOR"]["width"]
        )
        self.assertLess(selector_right, int(anchors["BANK_PARAM"]["cx"]))
        labels = {(element.text or "").strip()
                  for element in MASTER.iter() if element.tag.endswith("text")}
        self.assertTrue({"STOCK", "USER"} <= labels)

    def test_utterance_and_primary_control_row_use_full_panel_width(self):
        anchors = {
            element.attrib["id"]: element.attrib
            for element in MASTER.iter()
            if element.attrib.get("id") in {
                "UTTERANCE_FIELD", "PITCH_PARAM", "SPEED_PARAM",
                "WARP_PARAM", "WORD_PUSH_PARAM",
            }
        }
        self.assertEqual(anchors["UTTERANCE_FIELD"]["width"], "6392")
        self.assertEqual(
            [anchors[name]["cx"] for name in (
                "PITCH_PARAM", "SPEED_PARAM", "WARP_PARAM", "WORD_PUSH_PARAM"
            )],
            ["900", "2750", "4600", "6350"],
        )
        self.assertEqual({
            anchors[name]["cy"] for name in (
                "PITCH_PARAM", "SPEED_PARAM", "WARP_PARAM", "WORD_PUSH_PARAM"
            )
        }, {"4100"})

    def test_knob_and_jack_labels_are_below_their_controls(self):
        anchor_ids = {
            "PITCH": "PITCH_PARAM", "SPEED": "SPEED_PARAM", "WARP": "WARP_PARAM",
            "FORMANT": "FORMANT_PARAM", "EXCITE": "EXCITE_BLEND_PARAM",
            "BEND": "BEND_PARAM", "GLITCH": "GLITCH_PARAM",
            "SPEAK": "WORD_PUSH_PARAM", "V/OCT": "VOCT_INPUT",
            "WORD CV": "WORD_CV_INPUT",
            "TRIG/GATE": "TRIG_GATE_INPUT", "SCRUB": "SCRUB_CV_INPUT",
            "WARP CV": "WARP_CV_INPUT", "BEND CV": "BEND_CV_INPUT",
            "EXT EXCITE": "EXT_EXCITE_INPUT", "AUDIO": "AUDIO_OUTPUT",
            "FRAME CLK": "FRAME_CLK_OUTPUT", "EOX": "EOX_OUTPUT",
        }
        anchors = {element.attrib.get("id"): float(element.attrib["cy"])
                   for element in MASTER.iter() if element.attrib.get("id") in anchor_ids.values()}
        labels = {(element.text or "").strip(): float(element.attrib["y"])
                  for element in MASTER.iter() if element.tag.endswith("text")}
        for label, anchor_id in anchor_ids.items():
            self.assertGreater(labels[label], anchors[anchor_id], label)

    def test_split_labels_are_generated_and_panel_is_text_free(self):
        panel_text = [e for e in PANEL.iter() if e.tag.endswith("text")]
        self.assertFalse(panel_text)
        self.assertIn("labels", ids(LABELS))

    def test_widget_uses_split_assets_and_dynamic_anchor_helpers(self):
        self.assertIn('SplitPanelRenderer splitPanel(this, "res/Phonex.panel.svg")', WIDGET)
        self.assertIn('splitPanel.addLabels("res/Phonex.labels.svg")', WIDGET)
        self.assertIn("loadPointFromSvgMm(panelPath, id", WIDGET)
        self.assertIn("loadRectFromSvgMm(panelPath, id", WIDGET)

    def test_knob_polarity_matches_parameter_domains(self):
        self.assertEqual(WIDGET.count("createParamCentered<BipolarDarkTinyClockworkGearKnob>"), 1)
        self.assertEqual(WIDGET.count("createParamCentered<DarkTinyClockworkGearKnob>"), 3)
        self.assertEqual(WIDGET.count("addMainKnob("), 3)
        self.assertEqual(WIDGET.count("setProgressRingBipolar(true)"), 1)
        for parameter in ("PITCH_PARAM", "SPEED_PARAM", "WARP_PARAM"):
            line = next(line for line in WIDGET.splitlines()
                        if f'addMainKnob(Phonex::{parameter}' in line)
            self.assertTrue(line.rstrip().endswith("true);"))
        self.assertIn('createParam<PhonexWordBar>', WIDGET)
        self.assertIn('createParamCentered<PlasmaSwitch>', WIDGET)
        self.assertIn('point("BANK_PARAM"', WIDGET)
        self.assertIn('std::lround(normalized * 63.f)', WIDGET)
        self.assertNotIn("BefacoTinyKnobWhite", WIDGET)

    def test_text_submission_and_preview_are_safe(self):
        self.assertIn("module->submitText(getText())", WIDGET)
        self.assertIn("if (module)", WIDGET)
        self.assertIn("PhonexStatusDisplay(module)", WIDGET)
        self.assertNotIn("BUNDLED  10KHZ", WIDGET)
        self.assertNotIn('secondary += module->internalRate', WIDGET)
        self.assertIn('secondary = "UNICODE BOUNDARY"', WIDGET)


if __name__ == "__main__":
    unittest.main()
