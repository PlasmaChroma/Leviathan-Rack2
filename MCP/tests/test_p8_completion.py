import copy
import json
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server
from tools import sibyl_composer as sc
from p8_test_support import MODERN_CAPS


class P8NegotiationTest(unittest.IsolatedAsyncioTestCase):
    async def test_contract_cache_tracks_instance_hash_and_current_revision(self):
        server._SIBYL_CONTRACTS.clear()
        identity = ["instance1", "hash1", 1]
        calls = []
        async def wire(endpoint):
            calls.append(endpoint)
            if "format=manifest" in endpoint:
                return {"ok": True, "instance": identity[0], "fingerprint": identity[1], "revision": identity[2]}
            full = copy.deepcopy(MODERN_CAPS)
            full.update(instance=identity[0], fingerprint=identity[1])
            return full
        with patch.object(server, "_sibyl_call", side_effect=wire):
            await server._get_sibyl_contract(1)
            identity[2] = 2
            second = await server._get_sibyl_contract(1)
            self.assertEqual(second["capabilities"]["sibyl"]["revision"], 2)
            self.assertEqual(len(calls), 3)  # Two manifests, one detailed contract.
            identity[0] = "instance2"
            await server._get_sibyl_contract(1)
            self.assertEqual(len(calls), 5)
            identity[1] = "hash2"
            await server._get_sibyl_contract(1)
            self.assertEqual(len(calls), 7)
        with patch.object(server, "_sibyl_call", AsyncMock(side_effect=OSError("disconnected"))):
            with self.assertRaises(OSError):
                await server._get_sibyl_contract(1)
            self.assertFalse(server._SIBYL_CONTRACTS)

    async def test_native_receipt_negotiation_and_legacy_fallback(self):
        operation = {"op": "insert_note_batch", "pattern_id": "p", "encoding": "columns_v1",
                     "columns": ["degree"], "rows": [[0], [4]],
                     "onsets": {"start": 0, "spacing": 2, "count": 4}, "overrides": {"3": {"gate": 0}}}
        params = server.SibylEditInput(module_id=1, expected_revision=7, operations=[operation], response_profile="receipt")
        for modern in (True, False):
            caps = MODERN_CAPS if modern else {"ok": True, "capabilities": {"sibyl": {"editOperations": ["insert_notes"]}}}
            with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=caps)), \
                    patch.object(server, "_sibyl_call", AsyncMock(return_value={"ok": True, "revision": 8})) as call:
                await server.vcv_sibyl_edit(params)
                payload = call.call_args.args[2]
                self.assertEqual("response_profile" in payload, modern)
                self.assertEqual(payload["operations"][0]["op"], "insert_note_batch" if modern else "insert_notes")
                if not modern:
                    self.assertEqual(payload["operations"][0]["notes"][-1], {"step": 6, "degree": 4, "gate": 0})
        params.operations[0]["rows"] = [[0, 1]]
        with patch.object(server, "_get_sibyl_contract", AsyncMock(return_value=MODERN_CAPS)), \
                patch.object(server, "_sibyl_call", AsyncMock()) as call:
            with self.assertRaisesRegex(RuntimeError, "rectangular"):
                await server.vcv_sibyl_edit(params)
            call.assert_not_called()

    async def test_generic_dispatch_uses_typed_contract(self):
        with patch.object(server, "_sibyl_call", AsyncMock(return_value={"ok": True})) as call:
            await server.vcv_sibyl_request(server.SibylRequestInput(module_id=1, action="status"))
            call.assert_awaited_once_with("sibyl/1/status")
            call.reset_mock()
            with self.assertRaisesRegex(RuntimeError, "module_id"):
                await server.vcv_sibyl_request(server.SibylRequestInput(module_id=1, action="edit", parameters={"module_id": 2}))
            call.assert_not_called()

    def test_compact_toolset_keeps_discovery_and_dispatch(self):
        code = """import asyncio, json
from mcp_server import Octavia_MCP as s
s.configure_sibyl_toolset('compact')
print(json.dumps([t.name for t in asyncio.run(s.mcp.list_tools())]))
"""
        result = subprocess.run([sys.executable, "-B", "-c", code], cwd=Path(__file__).parents[1], capture_output=True, text=True, check=True)
        names = json.loads(result.stdout)
        self.assertIn("vcv_sibyl_request", names)
        self.assertIn("vcv_sibyl_get_capabilities", names)
        self.assertIn("vcv_sibyl_resolve", names)
        self.assertNotIn("vcv_sibyl_edit", names)


class P8ToolkitCompletionTest(unittest.TestCase):
    def test_non_octave_and_seeded_walk(self):
        tuning = sc.EdoTuning(13, "3/1")
        self.assertEqual(tuning.step_from_ratio(3, 1), 13)
        self.assertEqual(tuning.to_operations("t", "c")[0]["tuning"]["period"], {"ratio": "3/1"})
        before = sc.random.getstate()
        a = sc.PatternBuilder(pitch_context="c").arpeggiate([0, 4, 7], contour="random_walk", seed=314)
        b = sc.PatternBuilder(pitch_context="c").arpeggiate([0, 4, 7], contour="random_walk", seed=314)
        self.assertEqual(a.events, b.events)
        self.assertEqual(before, sc.random.getstate())
        with self.assertRaises(ValueError):
            sc.PatternBuilder().arpeggiate([0, 1], contour="random_walk")

    def test_columnar_missing_null_and_exact_ratios(self):
        reply = {"encoding": "columns_v1", "columns": ["step", "note", "tuned"],
                 "rows": [[0, None, {"ratio": "14/8"}], [1, None, None]], "missing": {"0": [1], "1": [2]}}
        notes = sc.decode_note_columns(reply)
        self.assertNotIn("note", notes[0])
        self.assertIsNone(notes[1]["note"])
        self.assertEqual(notes[0]["tuned"]["ratio"], "14/8")

    def test_sparse_overrides_and_nested_defaults_do_not_alias(self):
        batch = {"op": "insert_note_batch", "pattern_id": "p", "encoding": "columns_v1",
                 "columns": ["tuned.step"], "rows": [[0], [17]], "defaults": {"tuned": {"context": "c"}},
                 "onsets": {"start": 0, "spacing": 2, "count": 4},
                 "overrides": {"3": {"tuned": {"context": "c", "ratio": "14/8"}, "gate": 0}}}
        original = copy.deepcopy(batch)
        notes = sc.expand_note_batch(batch)["notes"]
        self.assertEqual([note["step"] for note in notes], [0, 2, 4, 6])
        self.assertEqual(notes[0]["tuned"]["step"], 0)
        self.assertEqual(notes[1]["tuned"]["step"], 17)
        self.assertEqual(notes[3]["tuned"]["ratio"], "14/8")
        self.assertEqual(batch, original)

    def test_client_cache_and_prewrite_profile_validation(self):
        client = sc.SibylClient(module_id=1)
        full = copy.deepcopy(MODERN_CAPS)
        full.update(instance="one", fingerprint="hash")
        manifest = {"ok": True, "instance": "one", "fingerprint": "hash", "revision": 9}
        with patch.object(client, "_request", side_effect=[manifest, full, manifest]) as call:
            self.assertEqual(client.capabilities()["revision"], 9)
            self.assertEqual(client.capabilities()["revision"], 9)
            self.assertEqual(call.call_count, 3)
        with patch.object(client, "_request") as call:
            with self.assertRaises(ValueError):
                client.edit([], response_profile="invalid")
            call.assert_not_called()

    def test_scene_automation_and_voicing(self):
        automation = sc.AutomationBuilder("v", scope="scene", scene_id="s").envelope(0, 4, 0, 5, 0)
        self.assertEqual(automation.to_upsert_op("a")["automation"]["scope"], {"scene": "s"})
        voicing = sc.VoicingBuilder("h", "s").spread({"v": "p"}, -1, 1).to_operation()
        self.assertEqual(voicing["voices"][0]["min"], {"pitchV": -1})
        with self.assertRaises(ValueError):
            sc.AutomationBuilder("v", scope="pattern")


if __name__ == "__main__":
    unittest.main()
