"""Behavioral tests for vcv_sibyl_bootstrap."""
import json
import sys
import unittest
from pathlib import Path
from unittest.mock import AsyncMock, patch

sys.path.insert(0, str(Path(__file__).parents[1]))
from mcp_server import Octavia_MCP as server


class SibylBootstrapTest(unittest.IsolatedAsyncioTestCase):
    def setUp(self):
        server._invalidate_module_cache()

    async def test_bridge_connection_failure_fails_immediately_without_retry(self):
        call_count = 0
        async def mock_call(endpoint, method="GET", data=None):
            nonlocal call_count
            call_count += 1
            raise OSError("Connection refused")

        with patch.object(server, "_call", side_effect=mock_call):
            res_str = await server.vcv_sibyl_bootstrap()
            res = json.loads(res_str)

        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "bridge_unavailable")
        self.assertEqual(call_count, 1)

    async def test_omitted_module_id_resolves_singleton_sibyl(self):
        modules = [
            {"id": 10, "plugin": "Fundamental", "model": "VCO"},
            {"id": 42, "plugin": "Leviathan", "model": "Sibyl"},
            {"id": 99, "plugin": "Leviathan", "model": "Proc"},
        ]
        manifest = {"ok": True, "instance": "inst1", "revision": 5, "fingerprint": "fp1",
                    "apiVersion": 1, "schemaVersion": 4, "pitchStage": "P7D",
                    "features": {"harmony": 1, "voicing": 1}}
        arrangement = {
            "ok": True, "revision": 5, "view": "arrangement", "total": 1,
            "scenes": [{"id": "s1", "lengthBeats": 16.0}],
            "patterns": {"p1": {"length": 16, "resolution": "1/16", "eventCount": 4, "referenceCount": 1}}
        }
        status = {
            "ok": True, "revision": 5, "activeRevision": 5, "pendingRevision": None,
            "running": True, "sceneId": "s1", "sceneRepeat": 0, "warnings": []
        }

        async def mock_call(endpoint, method="GET", data=None):
            if endpoint == "status":
                return {"running": True}
            if endpoint == "patch":
                return {"path": "/path/to/test.vcv"}
            raise ValueError(f"Unexpected _call: {endpoint}")

        async def mock_sibyl(endpoint, method="GET", data=None):
            if "capabilities" in endpoint:
                return manifest
            if "composition" in endpoint:
                return arrangement
            if "status" in endpoint:
                return status
            raise ValueError(f"Unexpected _sibyl_call: {endpoint}")

        with patch.object(server, "_call", side_effect=mock_call), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", side_effect=mock_sibyl):
            res_str = await server.vcv_sibyl_bootstrap()
            res = json.loads(res_str)

        self.assertTrue(res["ok"])
        self.assertEqual(res["module_id"], 42)
        self.assertTrue(res["bridge"]["running"])
        self.assertEqual(res["bridge"]["patch"]["path"], "/path/to/test.vcv")
        self.assertEqual(res["consistency"]["acceptedRevision"], 5)
        self.assertTrue(res["consistency"]["consistent"])
        self.assertEqual(res["runtime"]["activeRevision"], 5)
        self.assertEqual(res["runtime"]["sceneId"], "s1")
        self.assertIn("p1", res["arrangement"]["patterns"])
        # Invariant: No raw event arrays in arrangement patterns
        self.assertNotIn("steps", res["arrangement"]["patterns"]["p1"])
        self.assertNotIn("notes", res["arrangement"]["patterns"]["p1"])

    async def test_omitted_module_id_absent_fails_cleanly(self):
        modules = [
            {"id": 10, "plugin": "Fundamental", "model": "VCO"},
            {"id": 99, "plugin": "Leviathan", "model": "Proc"},
        ]
        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)):
            res = json.loads(await server.vcv_sibyl_bootstrap())
        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "sibyl_not_found")
        self.assertEqual(res["total"], 0)

    async def test_omitted_module_id_ambiguous_fails_with_candidates(self):
        modules = [
            {"id": 101, "plugin": "Leviathan", "model": "Sibyl"},
            {"id": 102, "plugin": "Leviathan", "model": "Sibyl"},
            {"id": 103, "plugin": "Leviathan", "model": "Sibyl"},
        ]
        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)):
            res = json.loads(await server.vcv_sibyl_bootstrap())
        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "ambiguous_sibyl")
        self.assertEqual(res["total"], 3)
        self.assertEqual(res["ids"], [101, 102, 103])
        self.assertEqual(res["omitted"], 0)

    async def test_explicit_module_id_validation_does_not_redirect(self):
        modules = [
            {"id": 10, "plugin": "Fundamental", "model": "VCO"},
            {"id": 42, "plugin": "Leviathan", "model": "Sibyl"},
            {"id": 99, "plugin": "Leviathan", "model": "Proc"},
        ]
        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)):
            # 1. Non-existent ID
            res1 = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=999)))
            self.assertFalse(res1["ok"])
            self.assertEqual(res1["error"], "module_not_found")

            # 2. Existing ID that is not a Sibyl module
            res2 = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=99)))
            self.assertFalse(res2["ok"])
            self.assertEqual(res2["error"], "module_is_not_sibyl")
            self.assertEqual(res2["model"], "Proc")

    async def test_inconsistent_revision_detection(self):
        modules = [{"id": 42, "plugin": "Leviathan", "model": "Sibyl"}]
        manifest_pre = {"ok": True, "instance": "i1", "revision": 5}
        manifest_post = {"ok": True, "instance": "i1", "revision": 6}  # Changed during read
        arrangement = {"ok": True, "revision": 5, "view": "arrangement", "scenes": [], "patterns": {}}
        status = {"ok": True, "revision": 5, "activeRevision": 5, "running": True}

        call_idx = 0
        async def mock_sibyl(endpoint, method="GET", data=None):
            nonlocal call_idx
            if "capabilities" in endpoint:
                call_idx += 1
                return manifest_pre if call_idx == 1 else manifest_post
            if "composition" in endpoint:
                return arrangement
            if "status" in endpoint:
                return status
            raise ValueError(endpoint)

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", side_effect=mock_sibyl):
            res = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42)))

        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "inconsistent_read")
        self.assertEqual(res["detail"], "revision_mismatch")
        self.assertIn("observed_revisions", res)
        self.assertEqual(res["observed_revisions"]["capabilities"], 5)
        self.assertEqual(res["observed_revisions"]["post_capabilities"], 6)

    async def test_instance_change_detection(self):
        modules = [{"id": 42, "plugin": "Leviathan", "model": "Sibyl"}]
        manifest_pre = {"ok": True, "instance": "i1", "revision": 5}
        manifest_post = {"ok": True, "instance": "i2", "revision": 5}  # Instance replaced

        call_idx = 0
        async def mock_sibyl(endpoint, method="GET", data=None):
            nonlocal call_idx
            if "capabilities" in endpoint:
                call_idx += 1
                return manifest_pre if call_idx == 1 else manifest_post
            if "composition" in endpoint:
                return {"ok": True, "revision": 5, "scenes": [], "patterns": {}}
            if "status" in endpoint:
                return {"ok": True, "revision": 5, "running": True}
            raise ValueError(endpoint)

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", side_effect=mock_sibyl):
            res = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42)))

        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "inconsistent_read")
        self.assertEqual(res["detail"], "instance_changed")

    async def test_pending_adoption_is_accepted_as_consistent(self):
        # Accepted revision is 10 across composition and status, but activeRevision is 9
        modules = [{"id": 42, "plugin": "Leviathan", "model": "Sibyl"}]
        manifest = {"ok": True, "instance": "i1", "revision": 10}
        arrangement = {"ok": True, "revision": 10, "view": "arrangement", "scenes": [], "patterns": {}}
        status = {
            "ok": True, "revision": 10, "activeRevision": 9, "pendingRevision": 10,
            "applyAt": "nextScene", "phasePolicy": "preserve", "running": True
        }

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", AsyncMock(side_effect=lambda ep, **kw: manifest if "capabilities" in ep else (arrangement if "composition" in ep else status))):
            res = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42)))

        self.assertTrue(res["ok"])
        self.assertEqual(res["consistency"]["acceptedRevision"], 10)
        self.assertTrue(res["consistency"]["consistent"])
        self.assertEqual(res["runtime"]["activeRevision"], 9)
        self.assertEqual(res["runtime"]["pendingRevision"], 10)
        self.assertEqual(res["runtime"]["applyAt"], "nextScene")

    async def test_legacy_nested_capabilities_compatibility(self):
        modules = [{"id": 42, "plugin": "Leviathan", "model": "Sibyl"}]
        legacy_caps = {
            "ok": True,
            "capabilities": {
                "sibyl": {
                    "apiVersion": 1,
                    "schemaVersion": 3,
                    "revision": 8,
                    "harmony": {"version": 1},
                    "voicing": {"version": 1}
                }
            }
        }
        arrangement = {"ok": True, "revision": 8, "scenes": [], "patterns": {}}
        status = {"ok": True, "revision": 8, "running": False}

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", AsyncMock(side_effect=lambda ep, **kw: legacy_caps if "capabilities" in ep else (arrangement if "composition" in ep else status))):
            res = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42)))

        self.assertTrue(res["ok"])
        self.assertEqual(res["consistency"]["acceptedRevision"], 8)
        self.assertEqual(res["capabilities"]["schemaVersion"], 3)
        self.assertEqual(res["capabilities"]["features"]["harmony"], 1)

    async def test_component_read_failures_identify_failed_phase(self):
        modules = [{"id": 42, "plugin": "Leviathan", "model": "Sibyl"}]
        manifest = {"ok": True, "revision": 5}

        # 1. Arrangement failure
        async def mock_arr_fail(ep, **kw):
            if "capabilities" in ep: return manifest
            if "composition" in ep: return {"ok": False, "error": "corrupt_data"}
            return {"ok": True, "revision": 5}

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", side_effect=mock_arr_fail):
            res = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42)))
        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "component_read_failed")
        self.assertEqual(res["phase"], "arrangement")

        # 2. Runtime status failure
        async def mock_status_fail(ep, **kw):
            if "capabilities" in ep: return manifest
            if "composition" in ep: return {"ok": True, "revision": 5, "scenes": []}
            return {"ok": False, "error": "status_timeout"}

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", side_effect=mock_status_fail):
            res = json.loads(await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42)))
        self.assertFalse(res["ok"])
        self.assertEqual(res["error"], "component_read_failed")
        self.assertEqual(res["phase"], "runtime")

    async def test_page_size_parameter_forwarding(self):
        modules = [{"id": 42, "plugin": "Leviathan", "model": "Sibyl"}]
        manifest = {"ok": True, "revision": 5}
        captured_url = None

        async def mock_wire(ep, **kw):
            nonlocal captured_url
            if "capabilities" in ep: return manifest
            if "composition" in ep:
                captured_url = ep
                return {"ok": True, "revision": 5, "scenes": [], "patterns": {}}
            return {"ok": True, "revision": 5}

        with patch.object(server, "_call", AsyncMock(return_value={"running": True})), \
             patch.object(server, "_get_cached_modules", AsyncMock(return_value=modules)), \
             patch.object(server, "_sibyl_call", side_effect=mock_wire):
            await server.vcv_sibyl_bootstrap(server.SibylBootstrapInput(module_id=42, page_size=16))

        self.assertIn("page_size=16", captured_url)


if __name__ == "__main__":
    unittest.main()
