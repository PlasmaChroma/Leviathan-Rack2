import json
from p8_test_support import MODERN_CAPS
import os
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server


class P8CStructuralTest(unittest.IsolatedAsyncioTestCase):
    async def test_update_pattern_forwarding(self):
        mock_response = {
            "ok": True,
            "revision": 15,
            "activeRevision": 14,
            "changes": [],
            "warnings": []
        }

        op = {
            "op": "update_pattern",
            "id": "lead",
            "set": {"length": 32, "evolution": {"probability": 0.2}}
        }

        with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=MODERN_CAPS)), \
                patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_response)) as mock_call:
            params = server.SibylEditInput(
                module_id=123456789,
                expected_revision=14,
                operations=[op],
                response_profile="receipt"
            )
            raw_res = await server.vcv_sibyl_edit(params)
            receipt = json.loads(raw_res)

            mock_call.assert_called_once()
            _, method, payload = mock_call.call_args[0]
            self.assertEqual(method, "POST")
            self.assertEqual(payload["operations"][0]["op"], "update_pattern")
            self.assertEqual(payload["operations"][0]["set"]["length"], 32)
            self.assertTrue(receipt["ok"])
            self.assertEqual(receipt["appliedOperations"], 1)

    async def test_clone_pattern_forwarding(self):
        mock_response = {
            "ok": True,
            "revision": 16,
            "activeRevision": 15,
            "changes": [],
            "warnings": []
        }

        op = {
            "op": "clone_pattern",
            "source_id": "verse_lead",
            "id": "chorus_lead",
            "overrides": {"length": 32}
        }

        with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=MODERN_CAPS)), \
                patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_response)) as mock_call:
            params = server.SibylEditInput(
                module_id=123456789,
                expected_revision=15,
                operations=[op],
                response_profile="receipt"
            )
            raw_res = await server.vcv_sibyl_edit(params)
            receipt = json.loads(raw_res)

            mock_call.assert_called_once()
            _, method, payload = mock_call.call_args[0]
            self.assertEqual(payload["operations"][0]["op"], "clone_pattern")
            self.assertEqual(payload["operations"][0]["source_id"], "verse_lead")
            self.assertEqual(payload["operations"][0]["id"], "chorus_lead")
            self.assertTrue(receipt["ok"])

    async def test_clone_scene_forwarding(self):
        mock_response = {
            "ok": True,
            "revision": 17,
            "activeRevision": 16,
            "changes": [],
            "warnings": []
        }

        op = {
            "op": "clone_scene",
            "source_id": "verse",
            "id": "verse_2",
            "patterns": "share",
            "track_overrides": {"v1": "lead_var", "v3": None},
            "position": "after_source",
            "repeats": 2
        }

        with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=MODERN_CAPS)), \
                patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_response)) as mock_call:
            params = server.SibylEditInput(
                module_id=123456789,
                expected_revision=16,
                operations=[op],
                response_profile="receipt"
            )
            raw_res = await server.vcv_sibyl_edit(params)
            receipt = json.loads(raw_res)

            mock_call.assert_called_once()
            _, method, payload = mock_call.call_args[0]
            self.assertEqual(payload["operations"][0]["op"], "clone_scene")
            self.assertEqual(payload["operations"][0]["patterns"], "share")
            self.assertIsNone(payload["operations"][0]["track_overrides"]["v3"])
            self.assertTrue(receipt["ok"])


if __name__ == "__main__":
    unittest.main()
