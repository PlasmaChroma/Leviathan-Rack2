"""Behavioral checks for the lean MCP views and batch lookups."""
import json
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server


class LeanViewsTest(unittest.IsolatedAsyncioTestCase):
    async def test_module_detail_defaults_to_slim_and_can_return_voltages(self):
        with patch.object(server, "_call", AsyncMock(return_value={"id": 7})) as call:
            await server.vcv_get_module(server.GetModuleInput(module_id=7))
            call.assert_awaited_with("modules/7?slim=1")
            await server.vcv_get_module(server.GetModuleInput(module_id=7, include_voltages=True))
            call.assert_awaited_with("modules/7")

    async def test_named_cable_batch_fetches_each_module_once(self):
        details = {
            "modules/1?slim=1": {"outputs": [{"id": 0, "name": "Out"}]},
            "modules/2?slim=1": {"inputs": [{"id": 3, "name": "In"}]},
        }
        calls = []
        async def wire(endpoint, method="GET", data=None):
            calls.append(endpoint)
            return details.get(endpoint, {"ok": True})
        connection = {"output_module_id": 1, "output_port_name": "Out",
                      "input_module_id": 2, "input_port_name": "In"}
        with patch.object(server, "_call", side_effect=wire):
            result = json.loads(await server.vcv_connect_cables(
                server.ConnectCablesInput(connections=[connection, connection])))
        self.assertEqual(result, {"ok": True, "applied": 2})
        self.assertEqual(calls.count("modules/1?slim=1"), 1)
        self.assertEqual(calls.count("modules/2?slim=1"), 1)
        self.assertEqual(calls.count("cables"), 2)

    async def test_failed_cable_keeps_applied_count_and_bounded_candidates(self):
        async def wire(endpoint, method="GET", data=None):
            if endpoint == "modules/1?slim=1":
                return {"outputs": [{"id": i, "name": f"Out{i}"} for i in range(12)]}
            return {"inputs": [{"id": 0, "name": "In"}]}
        good = {"output_module_id": 1, "output_port_name": "Out0",
                "input_module_id": 2, "input_port_name": "In"}
        bad = dict(good, output_port_name="Missing")
        with patch.object(server, "_call", side_effect=wire):
            result = json.loads(await server.vcv_connect_cables(
                server.ConnectCablesInput(connections=[good, bad])))
        self.assertEqual((result["applied"], result["failedIndex"]), (1, 1))
        self.assertIn("12 total", result["error"])
        self.assertNotIn("Out11", result["error"])

    async def test_signal_filter_and_slim_inventory(self):
        with patch.object(server, "_call", AsyncMock(return_value=[{"id": 1}, {"id": 2}])) as call:
            result = json.loads(await server.vcv_get_signal_levels(server.SignalLevelsInput(module_id=2)))
            self.assertEqual(result, [{"id": 2}])
            call.assert_awaited_with("modules/voltages")
        with patch.object(server, "_call", AsyncMock(side_effect=[[], []])) as call:
            self.assertEqual(json.loads(await server.vcv_find_unpatched()), [])
            self.assertEqual(call.await_args_list[0].args, ("modules/slim",))

    async def test_state_is_compacted_without_changing_json_values(self):
        source = '{ "data": { "ratio": "14/8", "present": null, "empty": [], "value": 0.7000000476837158 } }'
        class Response:
            def raise_for_status(self): pass
            def json(self): return json.loads(source)
        class Client:
            async def __aenter__(self): return self
            async def __aexit__(self, *_): pass
            async def get(self, *_args, **_kwargs): return Response()
        with patch.object(server.httpx, "AsyncClient", return_value=Client()):
            compact = await server.vcv_get_module_state(server.ModuleStateInput(module_id=7))
        self.assertEqual(json.loads(compact), json.loads(source))
        self.assertLess(len(compact), len(source))


class LeanContractsTest(unittest.TestCase):
    def test_sibyl_defaults_and_error_bound(self):
        self.assertEqual(server.SibylEditInput(module_id=1).response_profile, "receipt")
        self.assertTrue(server.SibylCapabilitiesInput(module_id=1).compact)
        response = server.httpx.Response(500, text="<html>" + "x" * 500 + "</html>",
                                         headers={"content-type": "text/html"},
                                         request=server.httpx.Request("GET", "http://localhost"))
        message = server._error_message(server.httpx.HTTPStatusError("bad", request=response.request, response=response))
        self.assertLess(len(message), 260)
        self.assertNotIn("<html>", message)

    def test_full_and_compact_registration_are_distinct(self):
        script = """import asyncio, json, sys
from mcp_server import Octavia_MCP as s
s.configure_sibyl_toolset(sys.argv[1])
print(json.dumps([t.name for t in asyncio.run(s.mcp.list_tools())]))
"""
        for mode in ("full", "compact"):
            result = subprocess.run([sys.executable, "-B", "-c", script, mode],
                                    cwd=Path(__file__).parents[1], capture_output=True, text=True, check=True)
            names = json.loads(result.stdout)
            self.assertEqual(len(names), len(set(names)))
            self.assertEqual("vcv_sibyl_request" in names, mode == "compact")
            self.assertEqual("vcv_sibyl_edit" in names, mode == "full")
            self.assertIn("vcv_sibyl_get_capabilities", names)


if __name__ == "__main__":
    unittest.main()
