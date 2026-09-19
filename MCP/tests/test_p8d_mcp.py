import json
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server


class P8DMacroTest(unittest.IsolatedAsyncioTestCase):
    async def test_compose_euclidean_macro(self):
        mock_response = {
            "ok": True,
            "revision": 20,
            "activeRevision": 19,
            "changes": [{"inserted": 5, "updated": 0, "deleted": 0, "clamped": 0}],
            "warnings": []
        }

        with patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_response)) as mock_call:
            params = server.SibylComposeEuclideanInput(
                module_id=123456789,
                pattern_id="euclid_kick",
                pulses=5,
                steps=16,
                note="C2",
                accent_interval=2,
                ratchets=2,
                expected_revision=19
            )
            raw_res = await server.vcv_sibyl_compose_euclidean(params)
            receipt = json.loads(raw_res)

            # Verify _sibyl_call was invoked with 2 operations (upsert_pattern scaffold + insert_note_batch)
            mock_call.assert_called_once()
            _, method, payload = mock_call.call_args[0]
            self.assertEqual(method, "POST")
            self.assertEqual(len(payload["operations"]), 2)
            self.assertEqual(payload["operations"][0]["op"], "upsert_pattern")
            self.assertEqual(payload["operations"][1]["op"], "insert_note_batch")
            self.assertEqual(payload["operations"][1]["pattern_id"], "euclid_kick")
            self.assertEqual(len(payload["operations"][1]["rows"]), 5)

            # Invariant note="C2" should be factored into defaults
            self.assertEqual(payload["operations"][1]["defaults"]["note"], "C2")

            # Receipt check
            self.assertTrue(receipt["ok"])
            self.assertEqual(receipt["revision"], 20)
            self.assertEqual(receipt["appliedOperations"], 2)

    async def test_compose_progression_macro(self):
        mock_response = {
            "ok": True,
            "revision": 21,
            "activeRevision": 20,
            "changes": [],
            "warnings": []
        }

        with patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_response)) as mock_call:
            params = server.SibylComposeProgressionInput(
                module_id=123456789,
                progression_id="changes_38",
                pitch_context="arch_38",
                divisions=38,
                expected_revision=20,
                chords=[
                    {"beat": 0, "id": "I", "root_step": 0, "chord_type": "septimal_dom7"},
                    {"beat": 4, "id": "IV", "root_step": 16, "chord_type": "maj7"}
                ]
            )
            raw_res = await server.vcv_sibyl_compose_progression(params)
            receipt = json.loads(raw_res)

            mock_call.assert_called_once()
            _, method, payload = mock_call.call_args[0]
            self.assertEqual(method, "POST")
            self.assertEqual(len(payload["operations"]), 1)
            op = payload["operations"][0]
            self.assertEqual(op["op"], "upsert_progression")
            self.assertEqual(op["id"], "changes_38")
            self.assertEqual(op["progression"]["pitchContext"], "arch_38")
            self.assertEqual(len(op["progression"]["chords"]), 2)
            # Chord 0: root_step 0, septimal_dom7 intervals resolved to [0, 13, 22, 31]
            c0 = op["progression"]["chords"][0]
            self.assertEqual(c0["rootPitch"], {"tuned": {"step": 0}})
            self.assertIn(22, c0["intervals"])  # P5 in 38-EDO is 22
            self.assertIn(31, c0["intervals"])  # septimal 7th in 38-EDO is 31

            self.assertTrue(receipt["ok"])


if __name__ == "__main__":
    unittest.main()
