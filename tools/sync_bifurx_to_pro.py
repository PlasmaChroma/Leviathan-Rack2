#!/usr/bin/env python3
"""Synchronize the standalone Bifurx plugin into ../Leviathan-Pro.

Run once with --init to create the destination's plugin scaffold. Subsequent
runs update only the explicitly managed Bifurx source/resource closure and
leave plugin metadata and other destination-owned files alone.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


OWNER = "Bifurx"
MANIFEST_DIR = ".leviathan-sync"
MANIFEST_NAME = f"{MANIFEST_DIR}/{OWNER.lower()}.json"
LEGACY_MANIFEST_NAME = ".bifurx-sync-manifest.json"
GENERATED_ATLAS_PATH = "src/PanelAnchorAtlas.cpp"

SOURCE_FILES = (
    "src/Bifurx.cpp",
    "src/Bifurx.hpp",
    "src/BifurxGL.cpp",
    "src/BifurxInputStage.hpp",
    "src/BifurxOutputStage.hpp",
    "src/BifurxOversampling.hpp",
    "src/BifurxRenderData.hpp",
    "src/BifurxRenderPrep.cpp",
    "src/BifurxRenderPrep.hpp",
    "src/BifurxTransitionSmoother.hpp",
    "src/BifurxUI.cpp",
    "src/BifurxWorker.cpp",
    "src/BifurxWorker.hpp",
    "src/DebugTerminalTransport.cpp",
    "src/DebugTerminalTransport.hpp",
    "src/GlLifecycleUtils.cpp",
    "src/GlLifecycleUtils.hpp",
    "src/IrisSourceField.hpp",
    "src/IrisWavetable.hpp",
    "src/MathHelpers.cpp",
    "src/MathHelpers.hpp",
    "src/NautiloidColor.hpp",
    "src/NautiloidFractal.hpp",
    "src/NanoSvgRasterizer.cpp",
    "src/NvgGraphicsLifecycle.cpp",
    "src/NvgGraphicsLifecycle.hpp",
    "src/PanelAnchorAtlas.hpp",
    "src/PanelSvgUtils.cpp",
    "src/PanelSvgUtils.hpp",
    "src/UndertowShape.hpp",
    "src/theme/ThemeService.cpp",
    "src/theme/ThemeService.hpp",
    "src/theme/ThemePersistence.cpp",
    "src/theme/ThemePersistence.hpp",
    "src/theme/ThemePresets.cpp",
    "src/theme/ThemePresets.hpp",
    "src/theme/ThemeTypes.hpp",
    "src/theme/ThemeUiPoller.hpp",
    "src/visual/AdaptiveGlSurface.cpp",
    "src/visual/AdaptiveGlSurface.hpp",
    "src/visual/ApertureLight.cpp",
    "src/visual/ApertureLight.hpp",
    "src/visual/ApertureLightTransfer.hpp",
    "src/visual/FractalGlassOverlay.cpp",
    "src/visual/FractalGlassOverlay.hpp",
    "src/visual/HaloKnob2.cpp",
    "src/visual/PlasmaConduit.cpp",
    "src/visual/PlasmaConduit.hpp",
    "src/visual/PlasmaSwitch.cpp",
    "src/visual/PlasmaSwitch.hpp",
    "src/visual/RasterImageAssets.cpp",
    "src/visual/VisualAssets.cpp",
    "src/visual/VisualAssets.hpp",
    "res/FractalParams.json",
    "res/bifurx.labels.svg",
    "res/bifurx.panel.svg",
    "res/bifurx.theme-text.svg",
    "res/bifurx/Bifurx-DB.png",
    "res/bifurx/Bifurx-DT.png",
    "res/bifurx/Bifurx-LB.png",
    "res/bifurx/Bifurx-LT.png",
    "res/icon/Bifurx-CS-96c.png",
    "res/icon/Eclipse2Knob.svg",
    "res/icon/HaloKnob2Back.svg",
    "res/icon/HaloKnobCenter.svg",
    "res/icon/HaloKnobCenterLit.svg",
    "res/icon/Leviathan_Logo_S2.png",
    "res/icon/LuminSliderHandle.svg",
    "res/icon/LuminSliderTicks.svg",
    "res/icon/LuminSliderTrack.svg",
    "res/icon/PerfectWave_Tiny.png",
    "res/icon/cyan_orb.png",
    "res/icon/cyan_underlay.png",
    "res/icon/dual_field_contact_track.svg",
    "res/icon/gear_knob_invert.svg",
    "res/icon/gear_knob_shadow.svg",
    "res/icon/gear_knob_tiny.svg",
    "res/icon/gear_knob_tiny_dark.svg",
    "res/icon/magitek2_input_rackfinal_256.png",
    "res/icon/magitek2_output_rackfinal_256.png",
)

MAKEFILE = r"""# Standalone Bifurx VCV Rack plugin build.
RACK_DIR ?= ../Rack-SDK

FLAGS +=
CFLAGS +=
CXXFLAGS +=
LDFLAGS +=

SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard src/visual/*.cpp)
SOURCES += $(wildcard src/theme/*.cpp)

DISTRIBUTABLES += res
DISTRIBUTABLES += eula.md

include $(RACK_DIR)/plugin.mk

# Rack SDK 2.5 adds this Clang-only option globally. Avoid a GCC note for every
# translation unit while retaining the SDK's remaining flags.
FLAGS := $(filter-out -Wno-vla-extension,$(FLAGS))

CXX_MACHINE := $(shell $(CXX) -dumpmachine 2>/dev/null)
ifneq (,$(findstring mingw,$(CXX_MACHINE)))
LDFLAGS += -lws2_32
LDFLAGS += -lopengl32
endif
"""

PLUGIN_JSON = r"""{
  "slug": "Leviathan-Pro",
  "name": "Leviathan Pro",
  "version": "2.0.0",
  "license": "proprietary",
  "brand": "Leviathan",
  "author": "Levi Kendall",
  "authorEmail": "levi.kendall@gmail.com",
  "authorUrl": "",
  "pluginUrl": "",
  "manualUrl": "",
  "sourceUrl": "https://github.com/PlasmaChroma/Leviathan-Pro",
  "donateUrl": "",
  "changelogUrl": "",
  "modules": [
    {
      "slug": "Bifurx",
      "name": "Bifurx",
      "description": "Dual-peak multimode filter with live response and spectrum preview.",
      "tags": [
        "Filter",
        "Distortion",
        "Visual"
      ]
    }
  ]
}
"""

GITIGNORE = r"""/build/
/dist/
/plugin.dll
/plugin.dylib
/plugin.so
"""

SYNCING_MD = r"""# Leviathan Pro source synchronization

This repository contains the standalone Leviathan Pro VCV Rack plugin. Bifurx
is its first synchronized module; its implementation and direct shared
dependencies are managed from the sibling `Leviathan-Rack2` repository.

From `Leviathan-Rack2`, synchronize with:

```sh
python3 tools/sync_bifurx_to_pro.py
```

Run with `--check` to report drift without writing. Files listed in
`.leviathan-sync/bifurx.json` are source-managed and should be edited in
`Leviathan-Rack2`. Per-module ownership prevents one synchronizer from deleting
a shared file still claimed by another module. `src/PanelAnchorAtlas.cpp` is
regenerated from the union of panel SVGs in this repository.

The `Makefile`, `plugin.json`, `src/plugin.cpp`, `src/plugin.hpp`, this file,
and other Pro-only files remain owned by this repository after initial setup.
"""

PLUGIN_HPP = r"""#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

#include <rack.hpp>
#include <chrono>
#include <string>

using namespace rack;

extern Plugin* pluginInstance;
extern Model* modelBifurx;

std::string leviathanPluginUserRootPath();

struct BefacoTinyKnobWhite : BefacoTinyKnob {};

bool isDragonKingDebugEnabled();
bool isClockworkDragDebugLoggingEnabled();
bool isExtraGlValidationEnabled();
bool isDragonKingUserFractalParamsEnabled();
void refreshDragonKingDebugEnabled();

struct ModuleTeardownTimer {
	const char* moduleName = nullptr;
	int moduleId = -1;
	bool active = false;
	std::chrono::steady_clock::time_point startedAt;

	explicit ModuleTeardownTimer(const char* moduleName);
	~ModuleTeardownTimer();
	void begin(int moduleId);
};

struct PreviewBuildLogTimer {
	const char* moduleName = nullptr;
	const rack::Module* module = nullptr;
	std::chrono::steady_clock::time_point startedAt;
	double panelDoneMs = -1.0;
	double anchorsDoneMs = -1.0;
	const char* atlasStatus = "n/a";

	PreviewBuildLogTimer(const char* moduleName, const rack::Module* module)
		: moduleName(moduleName), module(module), startedAt(std::chrono::steady_clock::now()) {}

	~PreviewBuildLogTimer() {
		if (module != nullptr || !isDragonKingDebugEnabled()) return;
		const auto endedAt = std::chrono::steady_clock::now();
		const double elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(endedAt - startedAt).count() * 1e-3;
		const char* name = moduleName ? moduleName : "unknown";
		if (panelDoneMs >= 0.0 && anchorsDoneMs >= panelDoneMs) {
			INFO("Preview build [%s]: total=%.3f ms panel=%.3f ms anchors=%.3f ms rest=%.3f ms atlas=%s",
				name, elapsedMs, panelDoneMs, anchorsDoneMs - panelDoneMs,
				elapsedMs - anchorsDoneMs, atlasStatus);
		}
		else if (panelDoneMs >= 0.0) {
			INFO("Preview build [%s]: total=%.3f ms panel=%.3f ms rest=%.3f ms atlas=%s",
				name, elapsedMs, panelDoneMs, elapsedMs - panelDoneMs, atlasStatus);
		}
		else {
			INFO("Preview build [%s]: total=%.3f ms atlas=%s", name, elapsedMs, atlasStatus);
		}
	}

	void markPanelDone() {
		if (module == nullptr && isDragonKingDebugEnabled())
			panelDoneMs = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - startedAt).count() * 1e-3;
	}
	void markAnchorsDone() {
		if (module == nullptr && isDragonKingDebugEnabled())
			anchorsDoneMs = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - startedAt).count() * 1e-3;
	}
	void setAtlasStatus(const char* status) {
		if (status && status[0] != '\0') atlasStatus = status;
	}
};
"""

PLUGIN_CPP = r"""#include "plugin.hpp"
#include "BifurxWorker.hpp"
#include "theme/ThemePersistence.hpp"
#include "visual/VisualAssets.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>

Plugin* pluginInstance = nullptr;

namespace {
std::atomic<bool> dragonKingDebugEnabled {false};
std::atomic<bool> clockworkDragDebugLoggingEnabled {false};
std::atomic<bool> extraGlValidationEnabled {false};
std::atomic<bool> userFractalParamsEnabled {false};
std::mutex moduleTeardownLogMutex;
}

std::string leviathanPluginUserRootPath() {
	const std::string slug = (pluginInstance && !pluginInstance->slug.empty())
		? pluginInstance->slug
		: "Leviathan-Pro";
	return system::join(asset::user(), slug);
}

void refreshDragonKingDebugEnabled() {
	dragonKingDebugEnabled.store(false, std::memory_order_relaxed);
	clockworkDragDebugLoggingEnabled.store(false, std::memory_order_relaxed);
	extraGlValidationEnabled.store(false, std::memory_order_relaxed);
	userFractalParamsEnabled.store(false, std::memory_order_relaxed);
	if (!pluginInstance) return;

	const std::string flagPath = system::join(leviathanPluginUserRootPath(), "dragonking.txt");
	std::ifstream flagFile(flagPath);
	if (!flagFile.good()) return;
	flagFile.close();

	json_error_t error;
	json_t* root = json_load_file(flagPath.c_str(), 0, &error);
	if (!root) {
		dragonKingDebugEnabled.store(true, std::memory_order_relaxed);
		return;
	}
	if (json_is_object(root)) {
		json_t* debugJ = json_object_get(root, "debug");
		json_t* clockworkJ = json_object_get(root, "clockworkDragLogging");
		json_t* extraGlJ = json_object_get(root, "extraGlValidation");
		json_t* userFractalJ = json_object_get(root, "UserFractalParams");
		if (!extraGlJ) extraGlJ = json_object_get(root, "ExtraGlValidation");
		dragonKingDebugEnabled.store(debugJ == nullptr || json_is_true(debugJ), std::memory_order_relaxed);
		clockworkDragDebugLoggingEnabled.store(json_boolean_value(clockworkJ), std::memory_order_relaxed);
		extraGlValidationEnabled.store(json_boolean_value(extraGlJ), std::memory_order_relaxed);
		userFractalParamsEnabled.store(json_boolean_value(userFractalJ), std::memory_order_relaxed);
	}
	json_decref(root);
}

bool isDragonKingDebugEnabled() {
	return dragonKingDebugEnabled.load(std::memory_order_relaxed);
}

bool isClockworkDragDebugLoggingEnabled() {
	return clockworkDragDebugLoggingEnabled.load(std::memory_order_relaxed);
}

bool isExtraGlValidationEnabled() {
	return extraGlValidationEnabled.load(std::memory_order_relaxed);
}

bool isDragonKingUserFractalParamsEnabled() {
	return userFractalParamsEnabled.load(std::memory_order_relaxed);
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {}

void ModuleTeardownTimer::begin(int id) {
	if (!isDragonKingDebugEnabled()) return;
	moduleId = id;
	active = true;
	startedAt = std::chrono::steady_clock::now();
	INFO("Leviathan Pro: module teardown begin: %s id=%d", moduleName ? moduleName : "unknown", moduleId);
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
	if (!active || !isDragonKingDebugEnabled()) return;
	const auto endedAt = std::chrono::steady_clock::now();
	const double elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(endedAt - startedAt).count() * 1e-3;
	const std::string dir = leviathanPluginUserRootPath();
	system::createDirectories(dir);
	const std::string path = system::join(dir, "module_teardown.csv");
	std::lock_guard<std::mutex> lock(moduleTeardownLogMutex);
	std::ifstream existing(path);
	const bool writeHeader = !existing.good();
	existing.close();
	std::ofstream out(path, std::ios::app);
	if (!out.is_open()) return;
	if (writeHeader) out << "unix_time_sec,module_name,module_id,total_ms\n";
	out << std::time(nullptr) << ',' << (moduleName ? moduleName : "unknown") << ','
		<< moduleId << ',' << std::fixed << std::setprecision(3) << elapsedMs << '\n';
}

void init(Plugin* p) {
	pluginInstance = p;
	refreshDragonKingDebugEnabled();
	visual_assets::loadSettings();
	leviathan::theme::persistence::initializeFromUserStorage();
	p->addModel(modelBifurx);
}

void destroy() {
	visual_assets::saveSettings();
	bifurx::shutdownBifurxRenderService();
}
"""

SCAFFOLD_FILES = {
    ".gitignore": GITIGNORE,
    "Makefile": MAKEFILE,
    "SYNCING.md": SYNCING_MD,
    "plugin.json": PLUGIN_JSON,
    "src/plugin.cpp": PLUGIN_CPP,
    "src/plugin.hpp": PLUGIN_HPP,
}

# These were managed by the first version of this synchronizer. Preserve them
# while migrating ownership to Leviathan-Pro and omit them from future manifests.
RELEASED_MANAGED_PATHS = {
    "src/plugin.cpp",
    "src/plugin.hpp",
    GENERATED_ATLAS_PATH,
}


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def safe_path(root: Path, relative: str) -> Path:
    candidate = root / relative
    resolved_parent = candidate.parent.resolve()
    if resolved_parent != root and root not in resolved_parent.parents:
        raise RuntimeError(f"managed path escapes destination: {relative}")
    return candidate


def generated_atlas(
    repo_root: Path,
    destination: Path,
    expected: dict[str, bytes],
    removed: set[str],
) -> bytes:
    """Generate the shared atlas from the destination resource union.

    A temporary overlay models this sync before writes, so --check and
    --dry-run remain non-mutating.
    """
    with tempfile.TemporaryDirectory(prefix="bifurx-atlas-") as temp_name:
        temp_root = Path(temp_name)
        destination_res = destination / "res"
        if destination_res.is_dir():
            for source in destination_res.glob("*.svg"):
                target = temp_root / "res" / source.name
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(source, target)
        for relative, data in expected.items():
            if relative.startswith("res/") and relative.endswith(".svg"):
                target = temp_root / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(data)
        for relative in removed:
            if relative.startswith("res/") and relative.endswith(".svg"):
                target = temp_root / relative
                if target.is_file():
                    target.unlink()
        subprocess.run(
            [
                sys.executable,
                str(repo_root / "tools/generate_panel_anchor_atlas.py"),
                "--repo-root",
                str(temp_root),
            ],
            check=True,
            stdout=subprocess.DEVNULL,
        )
        return (temp_root / GENERATED_ATLAS_PATH).read_bytes()


def expected_files(repo_root: Path) -> dict[str, bytes]:
    missing = [relative for relative in SOURCE_FILES if not (repo_root / relative).is_file()]
    if missing:
        raise RuntimeError("missing source files:\n  " + "\n  ".join(missing))
    return {relative: (repo_root / relative).read_bytes() for relative in SOURCE_FILES}


def read_manifest(path: Path) -> dict[str, str]:
    if not path.is_file():
        return {}
    data = json.loads(path.read_text(encoding="utf-8"))
    entries: dict[str, str] = {}
    for entry in data.get("files", []):
        if not isinstance(entry, dict) or "path" not in entry:
            continue
        relative = str(entry["path"])
        entries[relative] = str(entry.get("source", relative))
    return entries


def read_previous_manifest(destination: Path) -> tuple[dict[str, str], Path | None]:
    current = destination / MANIFEST_NAME
    if current.is_file():
        return read_manifest(current), current
    legacy = destination / LEGACY_MANIFEST_NAME
    if legacy.is_file():
        return read_manifest(legacy), legacy
    return {}, None


def read_other_ownership(destination: Path, current_path: Path | None) -> dict[str, set[str]]:
    ownership: dict[str, set[str]] = {}
    manifest_root = destination / MANIFEST_DIR
    if not manifest_root.is_dir():
        return ownership
    for path in sorted(manifest_root.glob("*.json")):
        if current_path is not None and path.resolve() == current_path.resolve():
            continue
        for relative, source in read_manifest(path).items():
            ownership.setdefault(relative, set()).add(source)
    return ownership


def write_atomic(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + ".bifurx-sync-tmp")
    temporary.write_bytes(data)
    os.replace(temporary, path)


def initialize(destination: Path, dry_run: bool) -> list[str]:
    changes: list[str] = []
    for relative, text in SCAFFOLD_FILES.items():
        path = safe_path(destination, relative)
        if path.exists():
            continue
        changes.append(f"create {relative}")
        if not dry_run:
            write_atomic(path, text.encode())
    return changes


def synchronize(repo_root: Path, destination: Path, init: bool, dry_run: bool) -> tuple[list[str], bool]:
    if not destination.is_dir() or not (destination / ".git").exists():
        raise RuntimeError(f"destination is not an existing Git repository: {destination}")
    changes: list[str] = []
    if init:
        changes.extend(initialize(destination, dry_run))
    elif not (destination / "Makefile").is_file() or not (destination / "plugin.json").is_file():
        raise RuntimeError("destination plugin is not initialized; run once with --init")

    expected = expected_files(repo_root)
    previous, previous_manifest_path = read_previous_manifest(destination)
    other_ownership = read_other_ownership(destination, previous_manifest_path)
    for relative in expected:
        conflicting_sources = {
            source for source in other_ownership.get(relative, set())
            if source != relative
        }
        if conflicting_sources:
            sources = ", ".join(sorted(conflicting_sources))
            raise RuntimeError(
                f"ownership conflict for {relative}: {OWNER} uses {relative}, "
                f"another module uses {sources}"
            )

    stale_candidates = set(previous) - set(expected) - RELEASED_MANAGED_PATHS
    stale = sorted(
        relative for relative in stale_candidates
        if relative not in other_ownership
    )
    for relative in stale:
        path = safe_path(destination, relative)
        if path.is_file() or path.is_symlink():
            changes.append(f"remove {relative}")
            if not dry_run:
                path.unlink()

    for relative, data in sorted(expected.items()):
        path = safe_path(destination, relative)
        current = path.read_bytes() if path.is_file() else None
        if current == data:
            continue
        changes.append(("update " if current is not None else "create ") + relative)
        if not dry_run:
            write_atomic(path, data)

    atlas_data = generated_atlas(repo_root, destination, expected, set(stale))
    atlas_path = safe_path(destination, GENERATED_ATLAS_PATH)
    current_atlas = atlas_path.read_bytes() if atlas_path.is_file() else None
    if current_atlas != atlas_data:
        changes.append(("update " if current_atlas is not None else "create ") + GENERATED_ATLAS_PATH)
        if not dry_run:
            write_atomic(atlas_path, atlas_data)

    manifest = {
        "format": 2,
        "owner": OWNER,
        "sourceRepository": repo_root.name,
        "files": [
            {"path": relative, "source": relative, "sha256": sha256(data)}
            for relative, data in sorted(expected.items())
        ],
    }
    manifest_bytes = (json.dumps(manifest, indent=2) + "\n").encode()
    manifest_path = destination / MANIFEST_NAME
    current_manifest = manifest_path.read_bytes() if manifest_path.is_file() else None
    if current_manifest != manifest_bytes:
        changes.append(("update " if current_manifest is not None else "create ") + MANIFEST_NAME)
        if not dry_run:
            write_atomic(manifest_path, manifest_bytes)
    legacy_manifest = destination / LEGACY_MANIFEST_NAME
    if legacy_manifest.is_file() and legacy_manifest != manifest_path:
        changes.append(f"remove {LEGACY_MANIFEST_NAME}")
        if not dry_run:
            legacy_manifest.unlink()
    return changes, bool(stale)


def main() -> int:
    script_path = Path(__file__).resolve()
    repo_root = script_path.parent.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--dest",
        type=Path,
        default=repo_root.parent / "Leviathan-Pro",
        help="destination Git repository (default: ../Leviathan-Pro)",
    )
    parser.add_argument("--init", action="store_true", help="create one-time plugin scaffold files when absent")
    parser.add_argument("--dry-run", action="store_true", help="report changes without writing")
    parser.add_argument("--check", action="store_true", help="report drift without writing and exit nonzero if found")
    args = parser.parse_args()
    destination = args.dest.resolve()
    dry_run = args.dry_run or args.check
    try:
        changes, _ = synchronize(repo_root, destination, args.init, dry_run)
    except (OSError, RuntimeError, subprocess.CalledProcessError, json.JSONDecodeError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    if changes:
        for change in changes:
            print(change)
    else:
        print("Bifurx destination is already synchronized.")
    if dry_run:
        print(f"Would synchronize {len(changes)} change(s) into {destination}")
    else:
        print(f"Synchronized {len(changes)} change(s) into {destination}")
    return 1 if args.check and changes else 0


if __name__ == "__main__":
    raise SystemExit(main())
