# Chimera implementation status

Updated 2026-09-23. Checkout: branch `expander`, commit `ee5f8ff21d457b73b3752b303dd79bd22da56010`. Existing dirty and untracked work was preserved. See `INTEGRATION_BASELINE.md` for source-backed dependency and lifecycle findings.

## Gates

| Phase | State | Evidence / next work |
|---|---|---|
| 0 — integration baseline | **Complete** | Native `plugin.dll` builds; Rack-independent and Rack-linked probes pass. A headless Rack patch-manager fixture archives and reopens coherent assets for Save and alternate-path Save, exercises autosave, asset failure, bypass/stopped save, shutdown, and API-equivalent duplicate paths. Frozen IDs, latency, event-history and memory bounds, and lifecycle map are recorded. Full `test-fast` has an unrelated Sibyl fixture failure. |
| 1 — types/math and deterministic core harness | **Complete (isolated scope)** | Rack-independent C++11 schema, profile-1 transfers, reader/window primitives, 64-bit core frame count, double source coordinate, explicit input/events/output harness, allocation trap and frozen-vector runner pass. Isolated CTL-003..011/013 and DSP-001/002/005/006/011/013 are covered; no Rack model or audio engine is registered yet. |
| 2–10 | Not started | Next: paged Reel ownership, snapshot leases and worker foundation. Phase 7 still requires a real Chimera module and Rack GUI end-to-end persistence/duplication test. |

## Executed commands and outcomes

- Native MINGW64 `make -j10 plugin.dll`: **PASS**, linked `plugin.dll` after an incremental object rebuild. A sandboxed first attempt failed before `make` with Windows CreateFileMapping error 5; the approved native retry succeeded.
- Native MINGW64 `make test-chimera-phase0`: **PASS**. Compiles and runs `tests/chimera_phase0_smoke.cpp` without Rack headers.
- Native MINGW64 `make test-chimera-rack-contract RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: **PASS**. A stack Rack Engine calls the installed runtime's `prepareSaveModule()` and `prepareSave()` hooks. Plain `dataToJson()` serialization does not call `onSave`, so the probe reports unsaved live state and retains the previous durable revision. This probes the hook contract, not the GUI duplicate or patch archive writer. The test uses internal SDK headers solely to instantiate an Engine outside Rack's application host.
- Native MINGW64 `make test-chimera-rack-save RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: **PASS**. `tests/chimera_rack_patch_save_spec.cpp` drives the installed Rack patch manager and extracts the resulting `.vcv` archives. It checks autosave without save preparation both before the first bundle and after a newer live edit; a save with matching manifest/audio; a failure before any complete bundle (JSON reports missing audio); alternate-path save failure after new audio is written but before its manifest (archive still references the old complete bundle and reports `saveFailure`); bypassed and stopped saves; and shutdown save preparation. It models the exact source-backed single-module and selection-duplicate API sequences: the former prepares and copies patch storage, while the latter only serializes JSON and needs a plugin-owned immutable cache reference for unsaved audio. The latter two are headless contract prototypes, not GUI actions.
- Native MINGW64 `make test-chimera-phase0`: **PASS** after expansion. `tests/chimera_phase0_capacity_spec.cpp` stresses all 13 jacks changing every 768 kHz host frame across the measured 640-frame input delay. Enqueue-before-consume occupancy peaks at 8,333 of 16,384 records; one 16-frame core burst is 208 records, below the 256 cap. Two 133,632,000-byte active-plus-reserve stores total 267,264,000 bytes, within 256 MiB; a third leased retired store would exceed it and must block preparation.
- Native MINGW64 `make -j10 plugin.dll`: **PASS**, up to date at the final Phase 0 checkpoint.
- Native MINGW64 compile/run of `tests/chimera_speex_link_probe.cpp` with SDK `-lRack` and Rack Pro runtime first on PATH: **PASS**. Quality-5 input latency: 40 host frames at 8/44.1 kHz, 80 at 96 kHz, 160 at 192 kHz, 640 at 768 kHz. This measures library delay, not full Chimera adapter latency.
- Frozen ID fixture compared to specification section 3 with Python: **PASS** (12 parameters, 13 inputs, 4 outputs, 9 lights). It is not yet compiled against a real module enum.
- Native MINGW64 `make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"`: **FAIL** at existing `sibyl_module_spec` after many preceding tests passed. The isolated native `.exe` reports two fixture failures: “P5 complete companion expressive-composition fixture compiles” and “P6 combined source compiles.” Both read absent `doc/Sibyl_v3_Example_Composition.json`; that file is absent in this checkout. No Chimera source was involved. The full suite stops there, so later tests were not run.
- `doc/Morphagene-Codex/validate_package.py`: **PASS** after the Phase 0 spec clarification, with unchanged source brief and 138 acceptance cases. This is documentation validation, not module validation.
- Native MINGW64 `make test-chimera-phase1 test-chimera-phase1-fast`: **PASS**. Both C++11 reference-safe and release-equivalent fast-math builds pass the control/core edge tests and all 224 JSON vector evaluations; frozen parameter/input/output/light names and ordering match `tests/chimera_ids_v1.json`. The 10,000-frame callback loop makes no C++ `new`/`new[]` calls and reuses the mapped rate when its smoothed inputs are unchanged. A comprehensive audio-callback allocation/interposition test remains for the full engine.
- Ubuntu WSL `g++ -std=c++11 -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Isrc tests/chimera_phase1_spec.cpp -o build/tests/chimera_phase1_sanitize_wsl && ASAN_OPTIONS=detect_leaks=1 build/tests/chimera_phase1_sanitize_wsl`: **PASS**. MINGW64's GCC 16.1.0 could not link its sanitizer target because `-lasan` and `-lubsan` are absent; the WSL run supplies the bounds/undefined-behavior check for the Rack-independent layer. This is not a substitute for the native Windows plugin build.

## Changed in this phase

- Added `INTEGRATION_BASELINE.md`, this status file, `tests/chimera_ids_v1.json`, `tests/chimera_phase0_smoke.cpp`, `tests/chimera_phase0_capacity_spec.cpp`, `tests/chimera_speex_link_probe.cpp`, `tests/chimera_rack_save_contract_spec.cpp`, and `tests/chimera_rack_patch_save_spec.cpp`.
- Added `make test-chimera-phase0`, `make test-chimera-rack-contract`, and `make test-chimera-rack-save` targets.
- Clarified the spec's 256-event bound by encoding a jack's connection and gate edge together; recorded a 16,384-entry proposed delayed event history based on the measured maximum Speex input delay.

## Handoff and limits

Phase 0 establishes the host contract and a small transactional asset prototype. It does not implement Chimera's production snapshot, workers, patch assets, panel, or audio engine. Once those exist, Phase 7 must exercise real GUI Save/Save As and both Duplicate commands with the installed Rack application, plus patch reload on another machine or an isolated user directory. The headless fixture cannot prove those complete user flows. It also cannot prove that Rack cancels a failed save: the observed behavior is that Rack archives the module's safe fallback JSON. The module must show the failure and keep a coherent prior bundle. An orphan audio file can enter the archive after a manifest-stage failure; it is unreferenced and needs later worker-side pruning.

The unrelated Sibyl fixture failure remains a separate baseline issue. At the Phase 0 checkpoint, no Chimera audio engine, panel or production patch asset implementation existed; Phase 1 adds the isolated control/math core below.

## Phase 1 implementation

- Added `src/ChimeraTypes.hpp` with fixed capacities, all frozen Rack enum IDs, stereo/region PODs, and explicit control/event/output frames. Updated the Phase 0 smoke target to compile these real types.
- Added `src/ChimeraProfile.hpp` with bit-stable finite checks, transfer laws, one-pole and linear fades, Gene hysteresis, finite-duration rounding, cosine-edge windows, region-wrapped cubic interpolation, Morph density, xorshift32 onset choices and stereo balance. The functions express the frozen software profile; they are not hardware measurements.
- Added `src/ChimeraCore.hpp`, a deterministic control/event skeleton. It owns a 64-bit frame counter, double source coordinate, sanitization state, smoothers, seeded onset state and a cached nonlinear rate mapping. Its output currently exposes sanitized monitoring and control state; Reel playback/recording do not exist yet.
- Added `tests/chimera_phase1_spec.cpp`, `tests/chimera_profile_probe.cpp`, and `tools/chimera_phase1_vectors.py`; added Makefile targets for reference, fast-math and sanitizer runs. The sanitizer target is usable in WSL/Linux; the installed MINGW64 toolchain lacks its sanitizer link libraries.

The Phase 1 exit gate is met for its isolated DSP/control scope. CTL-001/002 require a registered Rack module and widget later; DSP playback, storage and transport acceptance cases begin in later phases. No complete Chimera Rack module exists yet.
