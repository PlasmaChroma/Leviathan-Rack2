import json
import math
import sys
import unittest
from pathlib import Path

# Add tools directory to sys.path
sys.path.insert(0, str(Path(__file__).parents[2] / "tools"))
import sibyl_composer as sc


class SibylComposerTest(unittest.TestCase):
    def test_bjorklund_euclidean(self):
        # E(5, 8) -> standard Bjorklund: [1, 0, 1, 1, 0, 1, 0, 1]
        e5_8 = sc.bjorklund(5, 8)
        self.assertEqual(len(e5_8), 8)
        self.assertEqual(sum(e5_8), 5)
        self.assertEqual(e5_8, [True, False, True, True, False, True, False, True])

        # E(3, 8) -> standard tresillo [x, ., ., x, ., ., x, .]
        e3_8 = sc.bjorklund(3, 8)
        self.assertEqual(len(e3_8), 8)
        self.assertEqual(sum(e3_8), 3)

        # Rotation test
        e5_8_rot = sc.bjorklund(5, 8, rotation=2)
        self.assertEqual(len(e5_8_rot), 8)
        self.assertEqual(sum(e5_8_rot), 5)
        self.assertEqual(e5_8_rot, [False, True, True, False, True, True, False, True])

    def test_edo_tuning(self):
        # 12-EDO
        edo12 = sc.EdoTuning(12)
        self.assertEqual(edo12.step_from_interval("P1"), 0)
        self.assertEqual(edo12.step_from_interval("M3"), 4)
        self.assertEqual(edo12.step_from_interval("P5"), 7)
        self.assertEqual(edo12.step_from_interval("P8"), 12)
        self.assertEqual(edo12.chord_steps("maj"), [0, 4, 7])
        self.assertEqual(edo12.chord_steps("min", root_step=2), [2, 5, 9])

        # 38-EDO
        edo38 = sc.EdoTuning(38)
        # P5 (700 cents) in 38-EDO: round(38 * 700 / 1200) = round(22.166) = 22
        self.assertEqual(edo38.step_from_interval("P5"), 22)
        # Harmonic 7th (~968.8 cents) in 38-EDO: round(38 * 968.83 / 1200) = round(30.68) = 31
        self.assertEqual(edo38.step_from_interval("harmonic_seventh"), 31)

        # 53-EDO
        edo53 = sc.EdoTuning(53)
        # P5 (700 cents) in 53-EDO: round(53 * 700 / 1200) = round(30.916) = 31
        self.assertEqual(edo53.step_from_interval("P5"), 31)
        # M3 in 53-EDO: round(53 * 400 / 1200) = round(17.66) = 18
        self.assertEqual(edo53.step_from_interval("M3"), 18)

    def test_pattern_builder_euclidean_and_batch(self):
        pb = sc.PatternBuilder(length=16)
        pb.euclidean(pulses=5, steps=16, note="C2", velocity=0.85, gate=0.5,
                     accent_interval=2, accent_velocity=0.95, ratchets=2)

        self.assertEqual(len(pb.events), 5)
        # Verify accent velocity and ratchet applied on accents
        accented = [ev for ev in pb.events.values() if ev.get("velocity") == 0.95]
        self.assertTrue(len(accented) >= 2)
        self.assertTrue(all(ev.get("ratchets") == 2 for ev in accented))

        # Test conversion to P8B columnar batch
        batch_op = pb.to_columnar_batch("kick_pat")
        self.assertEqual(batch_op["op"], "insert_note_batch")
        self.assertEqual(batch_op["pattern_id"], "kick_pat")
        self.assertEqual(batch_op["encoding"], "columns_v1")
        self.assertEqual(len(batch_op["rows"]), 5)
        # Invariant note="C2" and gate=0.5 must be factored into defaults!
        self.assertIn("defaults", batch_op)
        self.assertEqual(batch_op["defaults"]["note"], "C2")
        self.assertEqual(batch_op["defaults"]["gate"], 0.5)
        self.assertNotIn("note", batch_op["columns"])

    def test_pattern_builder_arpeggio(self):
        pb = sc.PatternBuilder(length=8)
        pb.arpeggiate(["C3", "E3", "G3"], step_interval=2, contour="up", gate=0.8, velocity=0.7)
        self.assertEqual(len(pb.events), 4)  # steps 0, 2, 4, 6
        self.assertEqual(pb.events[0]["note"], "C3")
        self.assertEqual(pb.events[2]["note"], "E3")
        self.assertEqual(pb.events[4]["note"], "G3")
        self.assertEqual(pb.events[6]["note"], "C3")

    def test_progression_builder(self):
        pb = sc.ProgressionBuilder(pitch_context="arch_38", length_beats=16)
        pb.chord(beat=0, chord_id="I", root_step=0, intervals=[0, 10, 22, 31])
        pb.chord(beat=4, chord_id="IV", root_step=16, intervals=[0, 10, 22, 31])
        pb.chord(beat=8, chord_id="V", root_step=22, intervals=[0, 10, 22, 31])
        pb.chord(beat=12, chord_id="I", root_step=0, intervals=[0, 10, 22, 31])

        op = pb.to_upsert_op("blues_prog")
        self.assertEqual(op["op"], "upsert_progression")
        self.assertEqual(op["id"], "blues_prog")
        self.assertEqual(op["progression"]["pitchContext"], "arch_38")
        self.assertEqual(len(op["progression"]["chords"]), 4)

    def test_automation_builder(self):
        ab = sc.AutomationBuilder(track_id="v0", lane="mod")
        ab.envelope(start_beat=0, end_beat=16, start_v=-3.0, peak_v=2.0, end_v=-1.0)
        op = ab.to_upsert_op("filter_env")
        self.assertEqual(op["op"], "upsert_automation")
        self.assertEqual(len(op["automation"]["points"]), 3)
        self.assertEqual(op["automation"]["points"][0]["value"], -3.0)
        self.assertEqual(op["automation"]["points"][1]["value"], 2.0)
        self.assertEqual(op["automation"]["points"][2]["value"], -1.0)

    def test_arrangement_builder_and_p8c_clone(self):
        arr = sc.ArrangementBuilder()
        arr.add_track("v0", channel=0)
        arr.add_track("v1", channel=1)
        arr.add_scene("intro", length_beats=16, repeats=1, tracks={"v0": "kick"})
        arr.clone_scene(source_id="intro", id="verse", track_overrides={"v1": "lead"}, repeats=2)

        ops = arr.operations
        self.assertEqual(len(ops), 4)
        self.assertEqual(ops[0]["op"], "upsert_track")
        self.assertEqual(ops[2]["op"], "upsert_scene")
        self.assertEqual(ops[3]["op"], "clone_scene")
        self.assertEqual(ops[3]["source_id"], "intro")
        self.assertEqual(ops[3]["id"], "verse")
        self.assertEqual(ops[3]["patterns"], "share")
        self.assertEqual(ops[3]["track_overrides"], {"v1": "lead"})

    def test_score_orchestrator(self):
        score = sc.SibylScore(title="Ambient Study", bpm=110)
        kick_pb = sc.PatternBuilder(length=16).euclidean(4, 16, note="C2")
        score.add_pattern("kick_pat", kick_pb, use_columnar_batch=True)

        ops = score.compile()
        self.assertEqual(ops[0], {"op": "set_meta", "path": "title", "value": "Ambient Study"})
        self.assertEqual(ops[1], {"op": "set_meta", "path": "bpm", "value": 110})
        self.assertEqual(ops[2]["op"], "upsert_pattern")
        self.assertEqual(ops[3]["op"], "insert_note_batch")
        self.assertEqual(ops[3]["pattern_id"], "kick_pat")


if __name__ == "__main__":
    unittest.main()
