import ast
import unittest
from pathlib import Path


SERVER_PATH = Path(__file__).parents[1] / "mcp_server" / "Octavia_MCP.py"


class ServerContractTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.source = SERVER_PATH.read_text(encoding="utf-8")
        cls.tree = ast.parse(cls.source, filename=str(SERVER_PATH))

    def test_tool_names_are_unique(self):
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

        self.assertGreater(len(names), 20)
        self.assertEqual(len(names), len(set(names)), f"duplicate MCP tool names: {names}")

    def test_bridge_configuration_is_supported(self):
        self.assertIn('os.environ.get("OCTAVIA_PORT"', self.source)
        self.assertIn('os.environ.get("OCTAVIA_TOKEN"', self.source)
        self.assertIn('"X-Octavia-Token"', self.source)

    def test_tool_errors_are_raised(self):
        error_helper = next(
            node for node in self.tree.body
            if isinstance(node, ast.FunctionDef) and node.name == "_err"
        )
        self.assertTrue(any(isinstance(node, ast.Raise) for node in ast.walk(error_helper)))


if __name__ == "__main__":
    unittest.main()
