"""Run with the MCP environment's Python (MCP/requirements.txt installed)."""
import importlib.util
import json
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

from pydantic import ValidationError

SERVER_PATH = Path(__file__).parents[1] / "mcp_server" / "Octavia_MCP.py"
spec = importlib.util.spec_from_file_location("octavia_presence_mcp", SERVER_PATH)
server = importlib.util.module_from_spec(spec)
spec.loader.exec_module(server)


class PresenceToolsTest(unittest.IsolatedAsyncioTestCase):
    async def test_set_renews_timed_lease_and_preserves_response(self):
        result = {"ok": True, "targetState": "working", "remainingMs": 45000}
        with patch.object(server, "_envelope_call", AsyncMock(return_value=result)) as call:
            response = await server.vcv_octavia_set_presence(
                server.PresenceInput(state="working", lease_ms=45000))
            self.assertEqual(json.loads(response), result)
            call.assert_awaited_once_with("presence", "POST", {"state": "working", "leaseMs": 45000})

    async def test_release_and_read_do_not_invent_module_ids(self):
        with patch.object(server, "_envelope_call", AsyncMock(return_value={"ok": True})) as call:
            await server.vcv_octavia_set_presence(server.PresenceInput(state="auto"))
            call.assert_awaited_once_with("presence", "POST", {"state": "auto", "leaseMs": 30000})
            call.reset_mock()
            await server.vcv_octavia_get_presence()
            call.assert_awaited_once_with("presence")

    def test_invalid_leases_and_states_are_rejected_before_http(self):
        for lease in [999, 300001, 1000.5, True, "30000"]:
            with self.subTest(lease=lease), self.assertRaises(ValidationError):
                server.PresenceInput(state="thinking", lease_ms=lease)
        for state in ["busy", "Thinking", ""]:
            with self.subTest(state=state), self.assertRaises(ValidationError):
                server.PresenceInput(state=state)
        with self.assertRaises(ValidationError):
            server.PresenceInput(state="thinking", unknown=True)


if __name__ == "__main__":
    unittest.main()
