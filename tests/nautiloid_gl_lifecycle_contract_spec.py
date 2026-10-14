#!/usr/bin/env python3

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WIDGET = (ROOT / "src" / "NautiloidWidget.cpp").read_text(encoding="utf-8")
HELPER_HEADER = (ROOT / "src" / "GlLifecycleUtils.hpp").read_text(encoding="utf-8")


class NautiloidGlLifecycleContractTest(unittest.TestCase):
    def test_context_create_forgets_names_and_rearms_lazy_initialization(self):
        context_create = WIDGET.index("void onContextCreate(const ContextCreateEvent& e) override")
        owner_sync = WIDGET.index("void synchronizeOwnerContext()")
        body = WIDGET[context_create:owner_sync]
        self.assertIn("releaseGlResources(false);", body)
        self.assertIn("ownerVg = e.vg;", body)
        self.assertIn("setDirty();", body)

    def test_unexpected_context_switch_never_deletes_old_context_names(self):
        owner_sync = WIDGET.index("void synchronizeOwnerContext()")
        release = WIDGET.index("void releaseGlResources(bool deleteGlObjects)")
        body = WIDGET[owner_sync:release]
        self.assertIn("currentVg == ownerVg", body)
        self.assertIn("releaseGlResources(false);", body)
        self.assertNotIn("releaseGlResources(true);", body)

    def test_destructor_cleanup_remains_non_gl(self):
        destructor = WIDGET.index("~NautiloidGlPreview() override")
        context_destroy = WIDGET.index("void onContextDestroy", destructor)
        body = WIDGET[destructor:context_destroy]
        self.assertIn("releaseGlResources(false);", body)
        self.assertNotIn("glDelete", body)

    def test_ready_programs_use_shared_program_shader_validation(self):
        self.assertIn("isValidProgramShaderSet", HELPER_HEADER)
        self.assertIn("isExtraGlValidationEnabled()", WIDGET)
        self.assertIn("validateModeProgramForCurrentContext(mode);", WIDGET)


if __name__ == "__main__":
    unittest.main()
