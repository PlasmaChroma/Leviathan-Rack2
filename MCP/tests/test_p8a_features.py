import json
import os
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server


class P8AFeaturesTest(unittest.IsolatedAsyncioTestCase):
    def test_dump_json_compact_default(self):
        sample = {"a": 1, "b": [1, 2, 3], "c": {"d": "test"}}
        res = server._dump_json(sample)
        self.assertNotIn("\n", res)
        self.assertNotIn(" ", res)
        self.assertEqual(json.loads(res), sample)

    def test_dump_json_env_override(self):
        sample = {"a": 1, "b": 2}
        with patch.dict(os.environ, {"OCTAVIA_COMPACT_JSON": "0"}):
            res = server._dump_json(sample)
            self.assertIn("\n", res)
            self.assertIn("  ", res)

    async def test_module_caching_eliminates_repeated_http_calls(self):
        server._invalidate_module_cache()
        mock_mods = [{"id": 1234567890123456, "plugin": "Leviathan", "model": "Sibyl"}]
        
        with patch.object(server, "httpx") as mock_httpx:
            mock_client = AsyncMock()
            mock_resp = AsyncMock()
            mock_resp.status_code = 200
            mock_resp.json = lambda: mock_mods
            mock_client.get.return_value = mock_resp
            mock_httpx.AsyncClient.return_value.__aenter__.return_value = mock_client

            # First normalization call should fetch modules
            res1 = await server._normalize_endpoint("sibyl/1234567890123456/status")
            self.assertIn("1234567890123456", res1)
            self.assertEqual(mock_client.get.call_count, 1)

            # Second normalization call must use in-memory cache
            res2 = await server._normalize_endpoint("sibyl/1234567890123456/composition")
            self.assertIn("1234567890123456", res2)
            self.assertEqual(mock_client.get.call_count, 1)

    async def test_sibyl_edit_response_profile_receipt(self):
        full_edit_response = {
            "ok": True,
            "revision": 10,
            "activeRevision": 9,
            "phasePolicy": "preserve",
            "appliedAt": "nextBeat",
            "changes": {"notesInserted": 4, "notesUpdated": 0, "patternsUpserted": 0},
            "warnings": [],
            "extra_large_metadata": {"foo": "bar" * 100}
        }
        
        with patch.object(server, "_sibyl_call", AsyncMock(return_value=full_edit_response)):
            params = server.SibylEditInput(
                module_id=123456789,
                expected_revision=9,
                operations=[{"op": "update_notes", "pattern_id": "p", "selector": {"all": True}}],
                response_profile="receipt"
            )
            raw_res = await server.vcv_sibyl_edit(params)
            receipt = json.loads(raw_res)
            self.assertTrue(receipt["ok"])
            self.assertEqual(receipt["revision"], 10)
            self.assertEqual(receipt["activeRevision"], 9)
            self.assertEqual(receipt["appliedOperations"], 1)
            self.assertEqual(receipt["changes"], {"notesInserted": 4})
            self.assertNotIn("extra_large_metadata", receipt)

    async def test_sibyl_capabilities_compact_manifest(self):
        full_caps = {
            "ok": True,
            "capabilities": {
                "sibyl": {
                    "apiVersion": 1,
                    "schemaVersion": 4,
                    "revision": 12,
                    "pitchSystems": {"stage": "P7D", "version": 1},
                    "views": ["summary", "notes"],
                    "operations": ["edit", "validate"],
                    "harmony": {"version": 1},
                    "automation": {"version": 1},
                    "voicing": {"version": 1},
                    "conditions": {"version": 1},
                    "repeatEvolution": {"version": 1},
                    "large_unneeded_limits_tree": {"maxEntries": 1000000}
                }
            }
        }
        with patch.object(server, "_sibyl_call", AsyncMock(return_value=full_caps)):
            params = server.SibylCapabilitiesInput(module_id=123, compact=True)
            res = json.loads(await server.vcv_sibyl_get_capabilities(params))
            self.assertTrue(res["ok"])
            self.assertEqual(res["apiVersion"], 1)
            self.assertEqual(res["schemaVersion"], 4)
            self.assertEqual(res["pitchStage"], "P7D")
            self.assertNotIn("large_unneeded_limits_tree", res)


if __name__ == "__main__":
    unittest.main()
