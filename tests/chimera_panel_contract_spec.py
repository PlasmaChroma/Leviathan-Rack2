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


class ChimeraPanelContractTest(unittest.TestCase):
    def test_28_hp_and_runtime_anchors(self):
        self.assertEqual(MASTER.attrib["width"], "142.24mm")
        self.assertEqual(MASTER.attrib["height"], "128.5mm")
        required = {
            "DISPLAY_ORIGIN", "DISPLAY_END", "SOS_PARAM", "GENE_SIZE_PARAM",
            "VARISPEED_PARAM", "MORPH_PARAM", "SLIDE_PARAM", "ORGANIZE_PARAM",
            "GENE_ATT_PARAM", "VARISPEED_ATT_PARAM", "SLIDE_ATT_PARAM",
            "REC_PARAM", "SPLICE_PARAM", "SHIFT_PARAM", "UNSPLICE_BUTTON", "SOS_CV_INPUT",
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

    def test_display_and_control_rows_do_not_overlap(self):
        anchors = keyed(MASTER)
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
        self.assertGreaterEqual(first_cv - first_knobs, 12)
        self.assertGreaterEqual(second_knobs - first_cv, 10)
        self.assertGreaterEqual(buttons - second_cv, 9)
        self.assertGreaterEqual(jacks - buttons, 9)
        self.assertEqual(float(anchors["UNSPLICE_BUTTON"]["cy"]), buttons)

    def test_generated_labels_have_no_stale_developer_warning(self):
        master_text = " ".join((element.text or "") for element in MASTER.iter()
                               if element.tag.endswith("text"))
        self.assertNotIn("AUDIO NOT SAVED", master_text)
        self.assertFalse(any(element.tag.endswith("text") for element in PANEL.iter()))
        self.assertTrue(any(element.tag.endswith("path") for element in LABELS.iter()))


if __name__ == "__main__":
    unittest.main()
