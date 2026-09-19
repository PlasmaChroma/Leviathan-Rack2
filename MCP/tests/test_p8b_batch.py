import json
import os
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server


class P8BBatchTest(unittest.IsolatedAsyncioTestCase):
    async def test_insert_note_batch_forwarding(self):
        mock_response = {
            "ok": True,
            "revision": 11,
            "activeRevision": 10,
            "phasePolicy": "preserve",
            "appliedAt": "immediate",
            "changes": [
                {
                    "operationIndex": 0,
                    "patternId": "lead",
                    "matched": 0,
                    "updated": 0,
                    "inserted": 4,
                    "deleted": 0,
                    "clamped": 0,
                    "noteIds": ["n1", "n2", "n3", "n4"],
                    "displacedIds": [],
                    "idsTruncated": False,
                    "noteIdsTotal": 4,
                    "displacedIdsTotal": 0
                }
            ],
            "warnings": []
        }

        batch_op = {
            "op": "insert_note_batch",
            "pattern_id": "lead",
            "encoding": "columns_v1",
            "columns": ["step", "degree", "gate", "velocity"],
            "rows": [
                [0, 0, 0.75, 0.8],
                [2, 2, 0.5, 0.7],
                [4, 4, 1.0, 0.9],
                [6, 5, 0.8, 0.85]
            ],
            "defaults": {
                "probability": 1.0
            },
            "collision": "replace"
        }

        with patch.object(server, "_sibyl_call", AsyncMock(return_value=mock_response)) as mock_call:
            params = server.SibylEditInput(
                module_id=123456789,
                expected_revision=10,
                operations=[batch_op],
                response_profile="receipt"
            )
            raw_res = await server.vcv_sibyl_edit(params)
            receipt = json.loads(raw_res)

            # Verify _sibyl_call was invoked with the exact columnar batch payload
            mock_call.assert_called_once()
            called_endpoint, called_method, called_payload = mock_call.call_args[0]
            self.assertEqual(called_endpoint, "sibyl/123456789/edit")
            self.assertEqual(called_method, "POST")
            self.assertEqual(called_payload["operations"][0]["op"], "insert_note_batch")
            self.assertEqual(called_payload["operations"][0]["encoding"], "columns_v1")
            self.assertEqual(len(called_payload["operations"][0]["rows"]), 4)

            # Verify receipt profile properly summarizes batch insertion
            self.assertTrue(receipt["ok"])
            self.assertEqual(receipt["revision"], 11)
            self.assertEqual(receipt["appliedOperations"], 1)
            self.assertEqual(receipt["changes"], {"inserted": 4})


if __name__ == "__main__":
    unittest.main()
