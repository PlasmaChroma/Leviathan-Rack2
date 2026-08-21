import unittest
from pathlib import Path


ROOT = Path(__file__).parents[1]
OCTAVIA = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
INTERFACE = (ROOT / "src" / "SibylControl.hpp").read_text(encoding="utf-8")


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


if __name__ == "__main__":
    unittest.main()
