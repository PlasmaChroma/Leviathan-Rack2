import unittest
from pathlib import Path


ROOT = Path(__file__).parents[1]
OCTAVIA = (ROOT / "src" / "Octavia.cpp").read_text(encoding="utf-8")
INTERFACE = (ROOT / "src" / "OctaviaSemanticControl.hpp").read_text(encoding="utf-8")
SIBYL = (ROOT / "src" / "SibylControl.hpp").read_text(encoding="utf-8")
PHONEX = (ROOT / "src" / "PhonexSemantic.cpp").read_text(encoding="utf-8")
REFERENCE = (ROOT / "MCP" / "skill" / "octavia" / "references" / "semantic.md").read_text(encoding="utf-8")


class OctaviaSemanticContractTest(unittest.TestCase):
    def test_generic_interface_owns_no_module_schema(self):
        self.assertIn("semanticCapabilityId", INTERFACE)
        self.assertIn("handleSemanticRequest", INTERFACE)
        self.assertNotIn("composition", INTERFACE.lower())
        self.assertNotIn("envelope", INTERFACE.lower())

    def test_all_generic_routes_are_present(self):
        for route in ("capabilities", "document", "validate", "edit", "status", "command"):
            self.assertIn(f'/semantic/(\\d+)/{route}', OCTAVIA)

    def test_dispatch_contract_is_bounded_and_edit_only_undoable(self):
        self.assertIn("kMaxSemanticRequestBytes", OCTAVIA)
        self.assertIn("waitDone(job, 5000)", OCTAVIA)
        self.assertIn("semantic capability response id mismatch", OCTAVIA)
        self.assertIn(
            "job->operation == OctaviaSemanticControl::Operation::EDIT",
            OCTAVIA,
        )
        self.assertNotIn(
            "job->operation == OctaviaSemanticControl::Operation::COMMAND &&",
            OCTAVIA,
        )

    def test_sibyl_is_a_compatibility_adapter(self):
        self.assertIn("struct SibylControl : OctaviaSemanticControl", SIBYL)
        self.assertIn('return "leviathan.sibyl.composition"', SIBYL)
        self.assertIn("Operation::GET_COMPOSITION", SIBYL)
        self.assertIn("Operation::TRANSPORT", SIBYL)

    def test_phonex_capability_is_machine_self_describing(self):
        self.assertIn('"requestSchemas"', PHONEX)
        self.assertIn('"https://json-schema.org/draft/2020-12/schema"', PHONEX)
        self.assertIn('"oneOf"', PHONEX)
        self.assertIn('"examples"', PHONEX)
        self.assertNotIn('"editRequest"', PHONEX)

    def test_skill_treats_live_schemas_as_authoritative(self):
        self.assertIn("requestSchemas", REFERENCE)
        self.assertIn("live capability response is the source of truth", REFERENCE)
        self.assertNotIn("## Phonex word-bank capability", REFERENCE)


if __name__ == "__main__":
    unittest.main()
