"""Tests for tools/sync_mcp_schemas.py."""
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

_repo_root = Path(__file__).resolve().parents[2]
SYNC_SCRIPT = _repo_root / "tools" / "sync_mcp_schemas.py"


class SyncMcpSchemasTest(unittest.TestCase):
    def setUp(self):
        self.tmp_dir = Path(tempfile.mkdtemp(prefix="test_sync_schemas_"))

    def tearDown(self):
        shutil.rmtree(self.tmp_dir, ignore_errors=True)

    def run_exporter(self, *args, check=True) -> subprocess.CompletedProcess:
        cmd = [sys.executable, "-B", str(SYNC_SCRIPT), *args]
        return subprocess.run(cmd, cwd=str(_repo_root), capture_output=True, text=True, check=check)

    def test_export_tools_returns_deterministic_tools_for_both_modes(self):
        # 1. Full mode in fresh process
        full_dir = self.tmp_dir / "full"
        res_full = self.run_exporter("--toolset", "full", "--output", str(full_dir))
        self.assertEqual(res_full.returncode, 0)
        self.assertTrue((full_dir / "vcv_sibyl_bootstrap.json").is_file())
        self.assertTrue((full_dir / "vcv_sibyl_edit.json").is_file())
        self.assertTrue((full_dir / "vcv_sibyl_get_capabilities.json").is_file())
        self.assertFalse((full_dir / "vcv_sibyl_request.json").exists())

        # 2. Compact mode in fresh process
        compact_dir = self.tmp_dir / "compact"
        res_compact = self.run_exporter("--toolset", "compact", "--output", str(compact_dir))
        self.assertEqual(res_compact.returncode, 0)
        self.assertTrue((compact_dir / "vcv_sibyl_bootstrap.json").is_file())
        self.assertTrue((compact_dir / "vcv_sibyl_request.json").is_file())
        self.assertFalse((compact_dir / "vcv_sibyl_edit.json").exists())
        self.assertTrue((compact_dir / "vcv_sibyl_get_capabilities.json").is_file())

        # Check content determinism
        manifest_full = json.loads((full_dir / "manifest.json").read_text(encoding="utf-8"))
        self.assertEqual(manifest_full["toolset"], "full")
        self.assertGreater(manifest_full["toolCount"], 55)

        manifest_compact = json.loads((compact_dir / "manifest.json").read_text(encoding="utf-8"))
        self.assertEqual(manifest_compact["toolset"], "compact")
        self.assertEqual(manifest_compact["toolCount"], manifest_full["toolCount"] - 6)

    def test_schema_wrapper_and_parameters_preservation(self):
        full_dir = self.tmp_dir / "schema_check"
        self.run_exporter("--toolset", "full", "--output", str(full_dir))

        # Check vcv_sibyl_get_capabilities wrapper format and fields
        caps_obj = json.loads((full_dir / "vcv_sibyl_get_capabilities.json").read_text(encoding="utf-8"))
        self.assertIn("name", caps_obj)
        self.assertIn("description", caps_obj)
        self.assertIn("parameters", caps_obj)
        params = caps_obj["parameters"]
        self.assertIn("$defs", params)
        input_def = params["$defs"]["SibylCapabilitiesInput"]
        self.assertIn("compact", input_def["properties"])
        self.assertIn("topic", input_def["properties"])

        # Check vcv_get_module has include_voltages
        mod_obj = json.loads((full_dir / "vcv_get_module.json").read_text(encoding="utf-8"))
        mod_params = mod_obj["parameters"]["$defs"]["GetModuleInput"]
        self.assertIn("include_voltages", mod_params["properties"])

        # Check vcv_sibyl_bootstrap format
        boot_obj = json.loads((full_dir / "vcv_sibyl_bootstrap.json").read_text(encoding="utf-8"))
        boot_params = boot_obj["parameters"]["$defs"]["SibylBootstrapInput"]
        self.assertIn("module_id", boot_params["properties"])
        self.assertIn("page_size", boot_params["properties"])

    def test_apply_export_and_check_mode(self):
        target = self.tmp_dir / "target"

        # Initially check fails (directory empty/non-existent)
        res_drift = self.run_exporter("--toolset", "full", "--output", str(target), "--check", check=False)
        self.assertNotEqual(res_drift.returncode, 0)
        self.assertIn("FAILED", res_drift.stdout)

        # Apply export
        res_apply = self.run_exporter("--toolset", "full", "--output", str(target))
        self.assertEqual(res_apply.returncode, 0)

        # Check again - should pass with 0 drift
        res_ok = self.run_exporter("--toolset", "full", "--output", str(target), "--check")
        self.assertEqual(res_ok.returncode, 0)
        self.assertIn("OK: All", res_ok.stdout)

        # Modify a file - check should detect drift
        (target / "vcv_sibyl_bootstrap.json").write_text("corrupted", encoding="utf-8")
        res_drift2 = self.run_exporter("--toolset", "full", "--output", str(target), "--check", check=False)
        self.assertNotEqual(res_drift2.returncode, 0)
        self.assertIn("Changed content: vcv_sibyl_bootstrap.json", res_drift2.stdout)

    def test_unrelated_files_survive_publication_and_pruning(self):
        target = self.tmp_dir / "unrelated_test"
        self.run_exporter("--toolset", "full", "--output", str(target))

        # Create an unrelated file
        unrelated_file = target / "custom_config.json"
        unrelated_file.write_text('{"custom": true}', encoding="utf-8")

        # Re-apply export with prune=True
        res = self.run_exporter("--toolset", "full", "--output", str(target), "--prune")
        self.assertEqual(res.returncode, 0)

        # Unrelated file must survive because it was never in manifest!
        self.assertTrue(unrelated_file.is_file())
        self.assertEqual(unrelated_file.read_text(encoding="utf-8"), '{"custom": true}')
        self.assertIn("unmanaged files retained", res.stdout)

    def test_stale_files_pruned_when_switching_toolsets_with_prune(self):
        target = self.tmp_dir / "mode_switch_test"

        # 1. Export in full mode
        self.run_exporter("--toolset", "full", "--output", str(target))
        self.assertTrue((target / "vcv_sibyl_edit.json").is_file())

        # 2. Export in compact mode with prune=True
        res = self.run_exporter("--toolset", "compact", "--output", str(target), "--prune")
        self.assertEqual(res.returncode, 0)

        # vcv_sibyl_edit.json was owned by full manifest, so it should be pruned
        self.assertFalse((target / "vcv_sibyl_edit.json").exists())
        self.assertTrue((target / "vcv_sibyl_request.json").is_file())
        self.assertIn("Pruned stale managed file: vcv_sibyl_edit.json", res.stdout)


if __name__ == "__main__":
    unittest.main()
