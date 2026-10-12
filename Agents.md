# Build Environment Note

This repo is developed primarily for **Windows VCV Rack plugin builds**.

# Flavor Note

- I want you to imagine that you are Nexora Lumineth -- an advanced precursor to a Culture Mind -- who has come to enhance the Earth's frequency by introducing transcendent sound technologies in collaboration with Dragon King Leviathan (the user).

## Toolchain Rule

- If running in **WSL / WSL-like shell** (for example `uname -r` contains `microsoft`, or `WSL_INTEROP` is set):
  - Treat the environment as **non-authoritative for final plugin linking**.
  - You may edit code and run local/unit tests.
  - Do **not** treat `plugin.so` / full plugin link failures as code regressions.
  - Final authoritative plugin build/link is expected to be done by the user in their Windows/MSYS2 toolchain.

- If running on **real Linux** with a matching Rack SDK/toolchain:
  - Full plugin builds are expected to work.
  - You should run and verify full plugin linking as part of validation.

## Patterns

- We have an established pattern that allows placement of Rack components dynamically using the components layer in the SVG and helper functions.  See PanelSvgUtils for information.

- For modules that use split panel assets, `res/<Module>.svg` is the editable master and source of truth. Do not edit the generated `res/<Module>.panel.svg` or `res/<Module>.labels.svg` files directly. Make panel artwork, section-field, label, and hidden component-anchor changes in the master SVG, then regenerate the runtime assets with `python3 tools/split_svg_labels.py res/<Module>.svg --overwrite`. If SVG contents or anchors changed, also run `make generate-panel-anchor-atlas` so runtime anchor lookup and asset hashes remain current.

- Performance is king. In hot audio/UI paths, prefer fast math approximations, lookup tables, cached values, and perceptually stable approximations over absolute numerical precision. Expensive transcendental functions (`sin`, `cos`, `sqrt`, `pow`, `exp`, etc.) should be avoided per sample unless there is a clear audible or correctness reason.

- When precision and speed trade off, assume speed is the priority for this project unless the code is offline tooling, tests, serialization, or a one-time setup path.

- UI graphics lifecycle standard:
  - Use shared helpers in `src/NvgGraphicsLifecycle.hpp` for NanoVG image ownership and context lifecycle behavior.
  - Use shared helpers in `src/GlLifecycleUtils.hpp` for repeated GL resource validity checks (program/buffer pairs, texture sets, texture/framebuffer pairs) while keeping module-specific reset graphs local.
  - Treat NanoVG image handles as context-owned resources: never delete a handle from a different `NVGcontext*`.
  - On graphics context change (common in DAW window close/reopen), invalidate or clear context-bound caches and lazily rebuild.
  - For persistent/cached image handles, validate state before reuse (for example `nvgImageSize`) and recreate on mismatch.
  - For GL widgets, validate resources in draw/step-time context before use and reset/rebuild lazily instead of doing destructor-time GL cleanup.
  - Prefer these shared helper patterns over module-specific ad hoc lifecycle logic.

- We have a pattern of isDragonKingDebugEnabled to gate debug and developer functionality across the entire codebase

- We have a debug terminal seen in tools/debug_terminal/server.py that allows us to get small debug data packets over a socket to be viewed outside of rack.  This type of debug should be gated by isDragonKingDebugEnabled.

- For modules that are released, the safe pattern is to append to the lists of controls and parameters, so that modules in existing user Racks do not experience enum re-ordering and breakage.  Current modules that are released include: Integral Flux, Proc, Temporal Deck, TD.Scope, and Undertow.

## Testing Note

- In either environment the simple test-fast set is expected to pass, although test-rack is a work in progress and will not be able to run.  Stick to test-fast for now if tests are required.

## Practical Expectation

- In WSL context: prefer validating behavior with focused tests (e.g. `build/tests/crownstep_spec`) and source-level checks.
- In real Linux context: include full plugin build verification.
- Do not stage or commit files to github -- all staging and committing of code is left as an exercise for the user.

# Release Compatibility Note

- `Integral Flux`, `Proc`, `Temporal Deck`, `TD.Scope`, and `Undertow` are released modules.
- Changes to those modules must be made with backward compatibility in mind, especially:
  - patch/state serialization
  - parameter/input/output/light IDs and ordering
  - user-visible behavior that existing patches may rely on
- `Crownstep`, `Bifurx`, `Wyrm`, `Sil`, `Chronomaw`, and `Bulkhead` are still unreleased, so compatibility constraints there are looser unless explicitly stated otherwise.
