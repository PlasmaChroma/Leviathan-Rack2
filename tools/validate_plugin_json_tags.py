#!/usr/bin/env python3
"""Validate VCV Rack module tags in plugin.json.

Allowed tags are copied from VCVRack/Rack v2 src/tag.cpp:
https://raw.githubusercontent.com/VCVRack/Rack/v2/src/tag.cpp
"""

from __future__ import annotations

import argparse
import difflib
import json
import sys
from pathlib import Path
from typing import Any


TAG_ALIASES = (
    ("Arpeggiator",),
    ("Attenuator",),
    ("Blank",),
    ("Chorus",),
    ("Clock generator", "Clock"),
    ("Clock modulator",),
    ("Compressor",),
    ("Controller",),
    ("Delay",),
    ("Digital",),
    ("Distortion",),
    ("Drum", "Drums", "Percussion"),
    ("Dual",),
    ("Dynamics",),
    ("Effect",),
    ("Envelope follower",),
    ("Envelope generator",),
    ("Equalizer", "EQ"),
    ("Expander",),
    ("External",),
    ("Filter", "VCF", "Voltage controlled filter"),
    ("Flanger",),
    ("Function generator",),
    ("Granular",),
    ("Hardware clone", "Hardware"),
    ("Limiter",),
    ("Logic",),
    ("Low-frequency oscillator", "LFO", "Low frequency oscillator"),
    ("Low-pass gate", "Low pass gate", "Lowpass gate"),
    ("MIDI",),
    ("Mixer",),
    ("Multiple",),
    ("Noise",),
    ("Oscillator", "VCO", "Voltage controlled oscillator"),
    ("Panning", "Pan"),
    ("Phaser",),
    ("Physical modeling",),
    ("Polyphonic", "Poly"),
    ("Quad",),
    ("Quantizer",),
    ("Random",),
    ("Recording",),
    ("Reverb",),
    ("Ring modulator",),
    ("Sample and hold", "S&H", "Sample & hold"),
    ("Sampler",),
    ("Sequencer",),
    ("Slew limiter",),
    ("Speech",),
    ("Switch",),
    ("Synth voice",),
    ("Tuner",),
    ("Utility",),
    ("Visual",),
    ("Vocoder",),
    (
        "Voltage-controlled amplifier",
        "Amplifier",
        "VCA",
        "Voltage controlled amplifier",
    ),
    ("Waveshaper",),
)

VALID_TAGS = {alias.lower(): alias for aliases in TAG_ALIASES for alias in aliases}
KNOWN_TAGS = [alias for aliases in TAG_ALIASES for alias in aliases]


def tag_suggestion(tag: str) -> str:
    matches = difflib.get_close_matches(tag, KNOWN_TAGS, n=1, cutoff=0.6)
    return f" Did you mean {matches[0]!r}?" if matches else ""


def load_manifest(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        print(f"[FAIL] Missing manifest: {path}", file=sys.stderr)
    except json.JSONDecodeError as exc:
        print(f"[FAIL] Invalid JSON in {path}: {exc}", file=sys.stderr)
    return None


def validate_tags(manifest: Any, path: Path) -> list[str]:
    errors: list[str] = []
    if not isinstance(manifest, dict):
        return [f"{path}: root value must be a JSON object"]

    modules = manifest.get("modules", [])
    if not isinstance(modules, list):
        return [f"{path}: modules must be an array"]

    for index, module in enumerate(modules):
        if not isinstance(module, dict):
            errors.append(f"{path}: modules[{index}] must be an object")
            continue

        module_name = module.get("slug") or module.get("name") or f"modules[{index}]"
        tags = module.get("tags", [])
        if not isinstance(tags, list):
            errors.append(f"{path}: {module_name}: tags must be an array")
            continue

        for tag_index, tag in enumerate(tags):
            if not isinstance(tag, str):
                errors.append(
                    f"{path}: {module_name}: tags[{tag_index}] must be a string"
                )
                continue

            if tag.lower() not in VALID_TAGS:
                errors.append(
                    f"{path}: {module_name}: invalid tag {tag!r}."
                    f"{tag_suggestion(tag)}"
                )

    return errors


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate plugin.json module tags against Rack v2 tag aliases."
    )
    parser.add_argument(
        "manifest",
        nargs="?",
        default="plugin.json",
        type=Path,
        help="Path to plugin.json (default: plugin.json)",
    )
    args = parser.parse_args()

    manifest = load_manifest(args.manifest)
    if manifest is None:
        return 1

    errors = validate_tags(manifest, args.manifest)
    if errors:
        print("[FAIL] Invalid plugin.json tags found:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return 1

    print(f"[PASS] {args.manifest}: all module tags are valid Rack v2 tags")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
