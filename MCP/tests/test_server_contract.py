import ast
import unittest
from pathlib import Path


SERVER_PATH = Path(__file__).parents[1] / "mcp_server" / "Octavia_MCP.py"


class ServerContractTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = SERVER_PATH.read_text(encoding="utf-8")
        cls.tree = ast.parse(cls.source, filename=str(SERVER_PATH))

    def test_tool_names_are_unique_and_registered(self):
        names = []
        for node in self.tree.body:
            if not isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
                continue
            for decorator in node.decorator_list:
                if not isinstance(decorator, ast.Call):
                    continue
                if not (isinstance(decorator.func, ast.Attribute)
                        and isinstance(decorator.func.value, ast.Name)
                        and decorator.func.value.id == "mcp"
                        and decorator.func.attr == "tool"):
                    continue
                explicit_name = next(
                    (keyword.value.value for keyword in decorator.keywords
                     if keyword.arg == "name" and isinstance(keyword.value, ast.Constant)),
                    None,
                )
                names.append(explicit_name or node.name)

        self.assertGreaterEqual(len(names), 15)
        self.assertEqual(len(names), len(set(names)), f"duplicate MCP tool names: {names}")
        expected_tools = {
            "vcv_get_status", "vcv_get_perf", "vcv_get_debug_metrics", "vcv_debug_capture", "vcv_list_modules", "vcv_get_module",
            "vcv_list_library", "vcv_list_cables", "vcv_get_signal_levels",
            "vcv_find_unpatched", "vcv_analyze_audio", "vcv_reset_loudness", "vcv_temporal_deck_transport", "vcv_add_module", "vcv_delete_module",
            "vcv_update_module", "vcv_layout_modules", "vcv_set_parameters", "vcv_connect_cables",
            "vcv_disconnect_cable", "vcv_get_module_state", "vcv_set_module_state",
            "vcv_undo", "vcv_save_patch", "vcv_sibyl_get_capabilities",
            "vcv_sibyl_get_composition", "vcv_sibyl_validate", "vcv_sibyl_edit",
            "vcv_sibyl_get_status", "vcv_sibyl_transport",
            "vcv_octavia_console_status", "vcv_octavia_console_wait",
            "vcv_octavia_console_respond"
        }
        self.assertEqual(set(names), expected_tools)

    def test_bridge_configuration_is_supported(self):
        self.assertIn('os.environ.get("OCTAVIA_PORT"', self.source)
        self.assertIn('os.environ.get("OCTAVIA_TOKEN"', self.source)
        self.assertIn('"X-Octavia-Token"', self.source)

    def test_debug_metrics_are_module_scoped_and_brief(self):
        self.assertIn('await _call(f"debug/metrics/{params.module_id}")', self.source)
        self.assertIn("It does not start or return detailed", self.source)

    def test_debug_capture_is_bounded_and_extensible(self):
        self.assertIn('name="vcv_debug_capture"', self.source)
        self.assertIn('f"debug/capture/{params.module_id}"', self.source)
        self.assertIn("ge=1.0, le=10.0", self.source)
        self.assertIn("stable extensible diagnostics surface", self.source)

    def test_tool_errors_are_raised(self):
        error_helper = next(
            node for node in self.tree.body
            if isinstance(node, ast.FunctionDef) and node.name == "_err"
        )
        self.assertTrue(any(isinstance(node, ast.Raise) for node in ast.walk(error_helper)))

    def test_library_search_is_bounded_and_cable_failures_identify_the_connection(self):
        self.assertIn("if not params.plugin and not params.q:", self.source)
        self.assertIn('"failedIndex": failed_index', self.source)

    def test_layout_contract_is_row_aware_and_atomic(self):
        self.assertIn("row: Optional[int]", self.source)
        self.assertIn('name="vcv_layout_modules"', self.source)
        self.assertIn('await _call("modules/layout"', self.source)

    def test_sibyl_tools_use_semantic_routes_and_revision_guard(self):
        self.assertIn('await _sibyl_call(f"sibyl/{params.module_id}/composition?', self.source)
        self.assertIn('await _sibyl_call(f"sibyl/{params.module_id}/validate", "POST"', self.source)
        self.assertIn('await _sibyl_call(f"sibyl/{params.module_id}/edit", "POST"', self.source)
        self.assertIn('"expected_revision": params.expected_revision', self.source)
        self.assertIn('"phase_policy": params.phase_policy', self.source)
        self.assertIn('Literal["preserve", "restartChanged", "restartAll"]', self.source)
        self.assertIn('await _sibyl_call(f"sibyl/{params.module_id}/status")', self.source)
        self.assertIn('await _sibyl_call(f"sibyl/{params.module_id}/transport", "POST"', self.source)
        self.assertIn('Literal["scene", "arrangement", "patterns", "randomness"]', self.source)
        self.assertIn('payload = params.model_dump(exclude={"module_id"}, exclude_none=True)', self.source)


if __name__ == "__main__":
    unittest.main()
