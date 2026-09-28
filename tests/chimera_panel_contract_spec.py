#!/usr/bin/env python3
"""Production Chimera panel/asset geometry contract."""

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ET.parse(ROOT / "res" / "Chimera.svg").getroot()
PANEL = ET.parse(ROOT / "res" / "Chimera.panel.svg").getroot()
LABELS = ET.parse(ROOT / "res" / "Chimera.labels.svg").getroot()


def keyed(root):
    return {element.attrib["id"]: element.attrib
            for element in root.iter() if "id" in element.attrib}


def element(root, element_id):
    return next(item for item in root.iter()
                if item.attrib.get("id") == element_id)


class ChimeraPanelContractTest(unittest.TestCase):
    def test_28_hp_and_runtime_anchors(self):
        self.assertEqual(MASTER.attrib["width"], "142.24mm")
        self.assertEqual(MASTER.attrib["height"], "128.5mm")
        required = {
            "DISPLAY_ORIGIN", "DISPLAY_END", "REEL_LEFT", "REEL_RIGHT",
            "VU_LEFT", "VU_RIGHT",
            "SOS_PARAM", "GENE_SIZE_PARAM",
            "VARISPEED_PARAM", "MORPH_PARAM", "SLIDE_PARAM", "ORGANIZE_PARAM",
            "GENE_ATT_PARAM", "VARISPEED_ATT_PARAM", "SLIDE_ATT_PARAM",
            "APPEND_PARAM", "REC_PARAM", "SPLICE_PARAM", "SHIFT_PARAM", "UNSPLICE_BUTTON", "SOS_CV_INPUT",
            "GENE_SIZE_CV_INPUT", "VARISPEED_CV_INPUT", "MORPH_CV_INPUT",
            "SLIDE_CV_INPUT", "ORGANIZE_CV_INPUT", "CLOCK_INPUT",
            "PLAY_INPUT", "REC_INPUT", "SPLICE_INPUT", "SHIFT_INPUT",
            "AUDIO_L_INPUT", "AUDIO_R_INPUT", "AUDIO_L_OUTPUT",
            "AUDIO_R_OUTPUT", "CV_OUTPUT", "EOSG_OUTPUT", "REC_LIGHT",
            "REC_ARMED_LIGHT", "PLAY_LIGHT", "PENDING_LIGHT",
            "CLOCK_LIGHT", "PM_LIGHT", "IO_BUSY_LIGHT", "CLIP_LIGHT",
            "ERROR_LIGHT",
        }
        self.assertTrue(required <= keyed(MASTER).keys())
        self.assertTrue(required <= keyed(PANEL).keys())

    def test_master_anchors_are_visible_guides_and_runtime_anchors_are_hidden(self):
        master_group = element(MASTER, "component_anchors")
        panel_group = element(PANEL, "component_anchors")
        self.assertIn("display:inline", master_group.attrib.get("style", ""))
        self.assertIn("display:none", panel_group.attrib.get("style", ""))
        anchors = keyed(MASTER)
        self.assertAlmostEqual(float(anchors["SOS_PARAM"]["r"]), 5.67, places=5)
        self.assertAlmostEqual(float(anchors["GENE_ATT_PARAM"]["r"]), 4.0)
        self.assertAlmostEqual(float(anchors["REC_PARAM"]["r"]), 3.0)
        self.assertAlmostEqual(float(anchors["APPEND_PARAM"]["r"]), 3.0)
        self.assertAlmostEqual(float(anchors["CLOCK_INPUT"]["r"]), 4.08)
        self.assertAlmostEqual(float(anchors["REC_LIGHT"]["r"]), 2.33, places=5)

    def test_reel_anchors_are_symmetric_and_above_display(self):
        anchors = keyed(MASTER)
        self.assertTrue(element(MASTER, "REEL_LEFT").tag.endswith("circle"))
        self.assertTrue(element(MASTER, "REEL_RIGHT").tag.endswith("circle"))
        self.assertGreater(float(anchors["REEL_LEFT"]["r"]), 8.0)
        self.assertAlmostEqual(float(anchors["REEL_LEFT"]["r"]),
                               float(anchors["REEL_RIGHT"]["r"]), delta=0.01)
        self.assertAlmostEqual(float(anchors["REEL_LEFT"]["cy"]),
                               float(anchors["REEL_RIGHT"]["cy"]), delta=0.01)
        self.assertAlmostEqual(
            float(anchors["REEL_LEFT"]["cx"]) +
            float(anchors["REEL_RIGHT"]["cx"]), 142.24, places=1)
        self.assertLess(
            float(anchors["REEL_LEFT"]["cy"]) +
            float(anchors["REEL_LEFT"]["r"]),
            float(anchors["DISPLAY_ORIGIN"]["cy"]) + 0.5)

    def test_vu_anchors_match_reels_and_sit_inside_them(self):
        anchors = keyed(MASTER)
        left_vu = anchors["VU_LEFT"]
        right_vu = anchors["VU_RIGHT"]
        left_reel = anchors["REEL_LEFT"]
        right_reel = anchors["REEL_RIGHT"]
        self.assertTrue(element(MASTER, "VU_LEFT").tag.endswith("circle"))
        self.assertTrue(element(MASTER, "VU_RIGHT").tag.endswith("circle"))
        self.assertAlmostEqual(float(left_vu["cy"]), float(right_vu["cy"]), places=5)
        self.assertAlmostEqual(float(left_vu["r"]), float(left_reel["r"]), delta=0.01)
        self.assertAlmostEqual(float(right_vu["r"]), float(right_reel["r"]), delta=0.01)
        self.assertGreater(float(left_vu["cx"]), float(left_reel["cx"]))
        self.assertLess(float(right_vu["cx"]), float(right_reel["cx"]))
        self.assertAlmostEqual(float(left_vu["cx"]) + float(right_vu["cx"]),
                               142.24, places=1)

    def test_display_and_control_rows_do_not_overlap(self):
        anchors = keyed(MASTER)
        self.assertAlmostEqual(float(anchors["DISPLAY_ORIGIN"]["cx"]), 3.5)
        self.assertAlmostEqual(float(anchors["DISPLAY_END"]["cx"]), 138.5)
        self.assertAlmostEqual(float(keyed(PANEL)["DISPLAY_ORIGIN"]["cx"]), 3.5)
        self.assertAlmostEqual(float(keyed(PANEL)["DISPLAY_END"]["cx"]), 138.5)
        bezel = element(MASTER, "rect5")
        self.assertAlmostEqual(float(bezel.attrib["x"]), 2.9)
        self.assertAlmostEqual(float(bezel.attrib["width"]), 136.44)
        display_end = float(anchors["DISPLAY_END"]["cy"])
        lights = float(anchors["REC_LIGHT"]["cy"])
        first_knobs = float(anchors["SOS_PARAM"]["cy"])
        second_knobs = float(anchors["MORPH_PARAM"]["cy"])
        first_cv = float(anchors["SOS_CV_INPUT"]["cy"])
        second_cv = float(anchors["MORPH_CV_INPUT"]["cy"])
        buttons = float(anchors["REC_PARAM"]["cy"])
        jacks = float(anchors["CLOCK_INPUT"]["cy"])
        self.assertGreaterEqual(display_end - float(anchors["DISPLAY_ORIGIN"]["cy"]), 25)
        self.assertTrue(display_end < lights < first_knobs < first_cv <
                        second_knobs < second_cv < buttons < jacks)
        self.assertGreaterEqual(lights - display_end, 3)
        self.assertGreaterEqual(first_cv - first_knobs, 4)
        self.assertGreaterEqual(second_knobs - first_cv, 10)
        self.assertGreaterEqual(buttons - second_cv, 9)
        self.assertGreaterEqual(jacks - buttons, 9)
        self.assertEqual(float(anchors["UNSPLICE_BUTTON"]["cy"]), buttons)

    def test_append_button_sits_left_of_record(self):
        anchors = keyed(MASTER)
        self.assertEqual(float(anchors["APPEND_PARAM"]["cx"]), 48)
        self.assertEqual(float(anchors["REC_PARAM"]["cx"]), 60)
        self.assertEqual(float(keyed(PANEL)["APPEND_PARAM"]["cx"]), 48)
        label = element(MASTER, "append_label")
        self.assertEqual((float(label.attrib["x"]), label.text), (48, "REC+"))

    def test_output_order_and_labels(self):
        anchors = keyed(MASTER)
        self.assertEqual(float(anchors["SHIFT_INPUT"]["cx"]), 84)
        for jack_id, x in (
                ("EOSG_OUTPUT", 96), ("CV_OUTPUT", 108),
                ("AUDIO_L_OUTPUT", 120), ("AUDIO_R_OUTPUT", 132)):
            self.assertEqual(float(anchors[jack_id]["cx"]), x)
            self.assertEqual(float(keyed(PANEL)[jack_id]["cx"]), x)
        for label_id, x, caption in (
                ("text43", 96, "EOSG"), ("text42", 108, "CV OUT"),
                ("text40", 120, "OUT L"), ("text41", 132, "OUT R")):
            label = element(MASTER, label_id)
            self.assertEqual(float(label.attrib["x"]), x)
            self.assertEqual(label.text, caption)

    def test_generated_labels_have_no_stale_developer_warning(self):
        master_text = " ".join((element.text or "") for element in MASTER.iter()
                               if element.tag.endswith("text"))
        self.assertNotIn("AUDIO NOT SAVED", master_text)
        self.assertFalse(any(element.tag.endswith("text") for element in PANEL.iter()))
        self.assertTrue(any(element.tag.endswith("path") for element in LABELS.iter()))


if __name__ == "__main__":
    unittest.main()
