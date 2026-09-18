import unittest
from pathlib import Path


ROOT = Path(__file__).parents[1]
OCTAVIA = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
INTERFACE = (ROOT / "src" / "SibylControl.hpp").read_text(encoding="utf-8")
MCP_ADAPTER = (ROOT / "MCP" / "mcp_server" / "Octavia_MCP.py").read_text(encoding="utf-8")


class OctaviaSibylContractTest(unittest.TestCase):
    def test_harmony_context_query_is_forwarded(self):
        self.assertIn('"progression", "effective_context"', MCP_ADAPTER)
        self.assertIn('scene_repeat: Optional[int]', MCP_ADAPTER)
        self.assertIn('beat: Optional[float]', MCP_ADAPTER)
        self.assertGreaterEqual(OCTAVIA.count('"pattern_id", "cursor", "scene_id"'), 2)
        self.assertGreaterEqual(OCTAVIA.count('"sample_beats", "scene_repeat", "beat"'), 2)

    def test_automation_samples_use_typed_query_forwarding(self):
        self.assertIn('"notes", "automation"', MCP_ADAPTER)
        self.assertIn('sample_beats: Optional[list[float]]', MCP_ADAPTER)
        self.assertIn('("selector", "fields", "sample_beats")', MCP_ADAPTER)
        self.assertGreaterEqual(OCTAVIA.count('"sample_beats"'), 2)

    def test_all_semantic_routes_are_present(self):
        for route in ("capabilities", "composition", "validate", "edit", "status", "transport"):
            self.assertIn(f'/sibyl/(\\d+)/{route}', OCTAVIA)

    def test_debug_capture_uses_sibyl_ui_thread_dispatch(self):
        self.assertIn('/debug/capture/(\\d+)', OCTAVIA)
        self.assertIn('SibylControl::Operation::DEBUG_CAPTURE', OCTAVIA)

    def test_schema_is_owned_by_capability_implementation(self):
        self.assertIn("handleSibylRequest", INTERFACE)
        self.assertIn("const std::string& requestJson", INTERFACE)
        self.assertNotIn("patterns", OCTAVIA)
        self.assertNotIn("arrangement", OCTAVIA)

    def test_only_successful_edits_create_one_undo_action(self):
        self.assertIn("job->success && job->operation == OctaviaSemanticControl::Operation::EDIT", OCTAVIA)
        self.assertIn('job->legacySibyl ? "edit Sibyl composition', OCTAVIA)
        self.assertNotIn("OctaviaSemanticControl::Operation::COMMAND &&", OCTAVIA)

    def test_mcp_adapter_preserves_public_snake_case_fields(self):
        self.assertIn('{"expected_revision": params.expected_revision', MCP_ADAPTER)
        self.assertIn('"phase_policy": params.phase_policy', MCP_ADAPTER)
        self.assertIn('payload["apply_at"] = params.apply_at', MCP_ADAPTER)
        self.assertNotIn('payload["expectedRevision"]', MCP_ADAPTER)
        self.assertNotIn('payload["sceneId"]', MCP_ADAPTER)
        self.assertNotIn('payload["phaseMode"]', MCP_ADAPTER)


if __name__ == "__main__":
    unittest.main()
