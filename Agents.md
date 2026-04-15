# Build Environment Note

This repo is developed primarily for **Windows VCV Rack plugin builds**.

## Toolchain Rule

- If running in **WSL / WSL-like shell** (for example `uname -r` contains `microsoft`, or `WSL_INTEROP` is set):
  - Treat the environment as **non-authoritative for final plugin linking**.
  - You may edit code and run local/unit tests.
  - Do **not** treat `plugin.so` / full plugin link failures as code regressions.
  - Final authoritative plugin build/link is expected to be done by the user in their Windows/MSYS2 toolchain.

- If running on **real Linux** with a matching Rack SDK/toolchain:
  - Full plugin builds are expected to work.
  - You should run and verify full plugin linking as part of validation.

## Practical Expectation

- In WSL context: prefer validating behavior with focused tests (e.g. `build/tests/crownstep_spec`) and source-level checks.
- In real Linux context: include full plugin build verification.

# Release Compatibility Note

- `Integral Flux`, `Proc`, and `Temporal Deck` are released modules.
- Changes to those modules must be made with backward compatibility in mind, especially:
  - patch/state serialization
  - parameter/input/output/light IDs and ordering
  - user-visible behavior that existing patches may rely on
- `Crownstep` and `TD.Scope` are still unreleased, so compatibility constraints there are looser unless explicitly stated otherwise.
