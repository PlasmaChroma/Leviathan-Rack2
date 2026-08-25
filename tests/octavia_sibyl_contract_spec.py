import unittest
from pathlib import Path


ROOT = Path(__file__).parents[1]
OCTAVIA = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
INTERFACE = (ROOT / "src" / "SibylControl.hpp").read_text(encoding="utf-8")
MCP_ADAPTER = (ROOT / "MCP" / "mcp_server" / "Octavia_MCP.py").read_text(encoding="utf-8")


class OctaviaSibylContractTest(unittest.TestCase):
    def test_all_semantic_routes_are_present(self):
        for route in ("capabilities", "composition", "validate", "edit", "status", "transport"):
            self.assertIn(f'/sibyl/(\\d+)/{route}', OCTAVIA)

    def test_schema_is_owned_by_capability_implementation(self):
        self.assertIn("handleSibylRequest", INTERFACE)
        self.assertIn("const std::string& requestJson", INTERFACE)
        self.assertNotIn("patterns", OCTAVIA)
        self.assertNotIn("arrangement", OCTAVIA)

    def test_only_successful_edits_create_one_undo_action(self):
        self.assertIn("job->success && job->operation == SibylControl::Operation::EDIT", OCTAVIA)
        self.assertIn('undo.label="edit Sibyl composition', OCTAVIA)
        self.assertNotIn("SibylControl::Operation::TRANSPORT &&", OCTAVIA)

    def test_mcp_adapter_preserves_public_snake_case_fields(self):
        self.assertIn('{"expected_revision": params.expected_revision', MCP_ADAPTER)
        self.assertIn('"phase_policy": params.phase_policy', MCP_ADAPTER)
        self.assertIn('payload["apply_at"] = params.apply_at', MCP_ADAPTER)
        self.assertNotIn('payload["expectedRevision"]', MCP_ADAPTER)
        self.assertNotIn('payload["sceneId"]', MCP_ADAPTER)
        self.assertNotIn('payload["phaseMode"]', MCP_ADAPTER)


if __name__ == "__main__":
    unittest.main()
