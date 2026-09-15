#!/usr/bin/env python3
"""Exercise Premium snapshot isolation and dependency validation without DRM."""
from pathlib import Path
import json
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import prepare_bifurx_premium as prepare
import sync_bifurx_to_pro as sync


class PremiumStagingSpec(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="premium-staging-spec-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / "main"
        self.pro = Path(self.temp.name) / "pro"
        self.drm = self.pro / "DRM"
        self.sdk = Path(self.temp.name) / "sdk"
        self.output = self.root / "build" / "premium-validation"
        for path, text in {
            self.root / "src/Bifurx.cpp": "current main source",
            self.root / "plugin.dll": "normal binary",
            self.pro / "src/Bifurx.cpp": "older Pro source",
            self.pro / "src/plugin.cpp": "Pro bootstrap",
            self.pro / "src/plugin.hpp": "Pro declarations",
            self.pro / "Makefile": "FLAGS += -DLEVIATHAN_PRO_DRM=1\n",
            self.pro / "eula.md": "Pro license",
            self.pro / "plugin.json": json.dumps({"slug": "Leviathan-Pro", "modules": [{"slug": "Bifurx"}]}),
            self.drm / "drm.hpp": "external vendored header",
            self.sdk / "plugin.mk": "SDK makefile",
        }.items():
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
        for context in (
            patch.object(prepare, "ROOT", self.root), patch.object(prepare, "OUTPUT", self.output),
            patch.object(sync, "SOURCE_FILES", ("src/Bifurx.cpp",)),
            patch.object(prepare, "generated_atlas", return_value=b"generated atlas"),
        ):
            context.start()
            self.addCleanup(context.stop)

    def stage(self):
        return prepare.stage(self.pro, self.drm, self.sdk)

    def test_current_sources_with_pro_identity_and_external_header(self):
        self.stage()
        self.assertEqual((self.output / "src/Bifurx.cpp").read_text(), "current main source")
        self.assertEqual((self.output / "src/plugin.cpp").read_text(), "Pro bootstrap")
        self.assertEqual((self.pro / "src/Bifurx.cpp").read_text(), "older Pro source")
        self.assertEqual((self.root / "plugin.dll").read_text(), "normal binary")
        self.assertFalse((self.output / "DRM").exists())

    def test_missing_header_fails_before_touching_snapshot(self):
        self.stage()
        before = (self.output / "src/Bifurx.cpp").stat().st_mtime_ns
        (self.drm / "drm.hpp").unlink()
        with self.assertRaisesRegex(RuntimeError, "Missing Premium build dependency"):
            self.stage()
        self.assertEqual((self.output / "src/Bifurx.cpp").stat().st_mtime_ns, before)

    def test_unchanged_snapshot_retains_objects_and_timestamps(self):
        self.stage()
        obj = self.output / "build/src/Bifurx.cpp.o"
        obj.parent.mkdir(parents=True)
        obj.write_bytes(b"incremental object")
        source = self.output / "src/Bifurx.cpp"
        stamp = source.stat().st_mtime_ns
        self.stage()
        self.assertEqual(source.stat().st_mtime_ns, stamp)
        self.assertEqual(obj.read_bytes(), b"incremental object")
        config = (self.output / ".premium-config.json").read_bytes()
        (self.drm / "drm.hpp").write_text("new upstream version")
        self.stage()
        self.assertNotEqual(config, (self.output / ".premium-config.json").read_bytes())

    def test_wrong_plugin_identity_is_rejected(self):
        (self.pro / "plugin.json").write_text('{"slug":"Leviathan","modules":[]}')
        with self.assertRaisesRegex(RuntimeError, "manifest"):
            self.stage()
        self.assertFalse(self.output.exists())

    def test_stale_manifest_cannot_remove_files_outside_snapshot(self):
        self.stage()
        (self.output / ".premium-files.json").write_text('["../../plugin.dll"]')
        with self.assertRaisesRegex(RuntimeError, "Unsafe Premium staging path"):
            self.stage()
        self.assertEqual((self.root / "plugin.dll").read_text(), "normal binary")


if __name__ == "__main__":
    unittest.main()
