"""Materialize the pre-lazy renderer from its immutable Git blob for A/B testing."""
from pathlib import Path
import subprocess

blob = "2d1930c16b7db916ccc53bd05cec91430b7d5457"
source = subprocess.check_output(["git", "show", blob], text=True)
source = source.replace('"../GlLifecycleUtils.hpp"', '"GlLifecycleUtils.hpp"')
source = source.replace("namespace visual_assets", "namespace baseline_phosphor")
out = Path("build/tools/PhosphorPreviewBaseline.hpp")
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(source)
