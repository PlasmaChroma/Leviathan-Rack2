import json
from p8_test_support import MODERN_CAPS
import os
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server
sys.path.insert(0, str(Path(__file__).parents[2] / "tools"))
import sibyl_composer as composer


class P8EPreparedTransactionsTest(unittest.IsolatedAsyncioTestCase):

    async def test_validate_with_prepare_returns_handle(self):
        mock_val_resp = {
            "ok": True,
            "revision": 3,
            "valid": True,
            "handle": "prep_rev3_1",
            "expiresInSeconds": 60,
            "changes": {"notesInserted": 4}
        }

        with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=MODERN_CAPS)), \
                patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_val_resp)) as mock_call:
            params = server.SibylValidateInput(
                module_id=123456,
                expected_revision=3,
                operations=[{"op": "insert_notes", "pattern_id": "p", "notes": [{"step": 0, "note": "C3"}]}],
                prepare=True,
                ttl_seconds=120
            )
            raw = await server.vcv_sibyl_validate(params)
            res = json.loads(raw)
            self.assertTrue(res["ok"])
            self.assertTrue(res["valid"])
            self.assertEqual(res["handle"], "prep_rev3_1")
            self.assertEqual(res["expiresInSeconds"], 60)

            mock_call.assert_called_once()
            called_endpoint, called_method, called_payload = mock_call.call_args[0]
            self.assertEqual(called_endpoint, "sibyl/123456/validate")
            self.assertEqual(called_method, "POST")
            self.assertTrue(called_payload.get("prepare"))
            self.assertEqual(called_payload.get("ttl_seconds"), 120)

    async def test_edit_with_handle_commits_candidate(self):
        mock_edit_resp = {
            "ok": True,
            "revision": 4,
            "activeRevision": 3,
            "applyAt": "nextBeat",
            "phasePolicy": "preserve",
            "pendingRevision": 4,
            "appliedOperations": 7,
            "changes": {"notesInserted": 4},
            "warnings": []
        }

        with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=MODERN_CAPS)), \
                patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_edit_resp)) as mock_call:
            params = server.SibylEditInput(
                module_id=123456,
                handle="prep_rev3_1",
                response_profile="receipt"
            )
            raw = await server.vcv_sibyl_edit(params)
            res = json.loads(raw)
            self.assertTrue(res["ok"])
            self.assertEqual(res["revision"], 4)
            self.assertEqual(res["appliedOperations"], 7)
            self.assertEqual(res["pendingRevision"], 4)
            self.assertEqual(res["applyAt"], "nextBeat")

            mock_call.assert_called_once()
            called_endpoint, called_method, called_payload = mock_call.call_args[0]
            self.assertEqual(called_endpoint, "sibyl/123456/edit")
            self.assertEqual(called_payload.get("handle"), "prep_rev3_1")
            self.assertNotIn("operations", called_payload)

    async def test_capabilities_advertises_prepared_transactions(self):
        mock_caps = {
            "ok": True,
            "capabilities": {
                "sibyl": {
                    "apiVersion": 1,
                    "schemaVersion": 4,
                    "revision": 5,
                    "preparedTransactions": True,
                    "operations": ["validate", "edit"],
                    "pitchSystems": {"stage": "P7D"},
                    "views": ["summary"],
                    "conditions": {"version": 1},
                    "repeatEvolution": {"version": 1}
                }
            }
        }
        with patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_caps)):
            params = server.SibylCapabilitiesInput(module_id=123456, compact=True)
            res = json.loads(await server.vcv_sibyl_get_capabilities(params))
            self.assertTrue(res["ok"])
            self.assertTrue(res["features"]["preparedTransactions"])


class P8EComposerClientTest(unittest.TestCase):

    def test_composer_client_validate_prepare(self):
        client = composer.SibylClient(base_url="http://mock:1234")
        client._module_id = 999

        mock_resp = {
            "ok": True,
            "valid": True,
            "revision": 7,
            "handle": "prep_rev7_42",
            "expiresInSeconds": 60
        }

        with patch.object(client, "capabilities", return_value=MODERN_CAPS["capabilities"]["sibyl"]), \
                patch("urllib.request.urlopen") as mock_urlopen:
            mock_cm = mock_urlopen.return_value.__enter__.return_value
            mock_cm.read.return_value = json.dumps(mock_resp).encode("utf-8")

            res = client.validate([{"op": "dummy"}], expected_revision=7, prepare=True)
            self.assertTrue(res["valid"])
            self.assertEqual(res["handle"], "prep_rev7_42")

    def test_composer_client_two_phase_commit(self):
        client = composer.SibylClient(base_url="http://mock:1234")
        client._module_id = 999

        mock_val = {"ok": True, "valid": True, "revision": 7, "handle": "prep_rev7_42"}
        mock_edit = {"ok": True, "revision": 8, "activeRevision": 7, "warnings": []}

        with patch.object(client, "validate", return_value=mock_val) as mock_val_fn:
            with patch.object(client, "commit_prepared", return_value=mock_edit) as mock_commit_fn:
                res = client.two_phase_commit([{"op": "dummy"}], expected_revision=7)
                mock_val_fn.assert_called_once_with([{"op": "dummy"}], expected_revision=7, prepare=True)
                mock_commit_fn.assert_called_once_with("prep_rev7_42", expected_revision=7, response_profile="receipt")
                self.assertTrue(res["ok"])
                self.assertEqual(res["revision"], 8)


if __name__ == "__main__":
    unittest.main()
