# Verification record

Prepared 2026-10-03. This record concerns the generated kit, not the complete Leviathan plugin.

## Executed successfully

### SDK-free C++ core

The same `tests/body_map_core_test.cpp` was compiled and executed with strict warnings in C++11 and C++17 modes:

```sh
g++ -std=c++11 -O2 -Wall -Wextra -Werror -pedantic -Isrc \
    tests/body_map_core_test.cpp -o build/body_map_core_cpp11
./build/body_map_core_cpp11

g++ -std=c++17 -O2 -Wall -Wextra -Werror -pedantic -Isrc \
    tests/body_map_core_test.cpp -o build/body_map_core_test
./build/body_map_core_test
```

Both completed with:

```text
PASS: 100001 frequency sweep samples; boundaries, invalid values, symbolic supports, modes, frame-rate independence, jump/disconnect fades, and aspect fit.
```

Coverage includes 100,001 logarithmically sampled frequencies in the supported range, report weights remaining bounded/normalized, no more than two steady report masks, boundary continuity and exact shared-boundary weights, endpoints, NaN/infinity and out-of-range values, each symbolic center and points outside its compact support, modes, frame-rate-independent easing, discontinuous jumps, disconnect fade-out, and source-canvas aspect fitting.

The core also passed an AddressSanitizer/UndefinedBehaviorSanitizer run:

```sh
g++ -std=c++11 -O1 -g -fno-omit-frame-pointer \
    -fsanitize=address,undefined -Isrc \
    tests/body_map_core_test.cpp -o build/body_map_core_sanitized
./build/body_map_core_sanitized
```

No sanitizer findings were reported by that test. This does not cover Rack module events, expander lifecycle, image ownership, or host graphics operations.

### Asset validation

```sh
python3 tests/body_map_assets_test.py
```

Output:

```text
PASS: 14 aligned RGBA glow masks; negative-space/background transparency; outline black-matte reconstruction error 0.4471/255.
```

The assets test checks native/reference dimensions, the aligned half-resolution masks, RGBA encoding, representative outside/underarm transparency, and reconstruction of the supplied outline over black. The reported reconstruction error is a maximum channel error below one 8-bit level, not an in-Rack filtering measurement.

### Interactive preview

```sh
python3 tests/preview_test.py
```

Playwright launched local Chromium, loaded the self-contained HTML through `page.set_content`, and checked report mode at 174 Hz, the exact 300 Hz boundary, the 528 Hz symbolic point, disconnect fade-out, and report mode at 45 Hz with a light surround. No JavaScript exceptions were reported. Screenshot inspections showed the outline above the glow, no solid background rectangle around the figure, and no obvious glow bleed across the underarm holes at the inspected sizes.

An additional 390-pixel-wide mobile viewport check passed with no horizontal document overflow. The light-surround test waited for the CSS transition and checked the computed background color.

Output:

```text
PASS: local Chromium load, report/symbolic selection, exact band boundary, disconnect fade, light surround, no JavaScript exceptions.
```

The browser test depends on Playwright and an available Chromium executable. It uses no external service or network asset. It is an optional authoring check, not a plugin runtime dependency.

### Patch syntax/context

`git apply --check` succeeded against a local reconstruction of the publicly fetched `VTune.hpp`, `VTune.cpp`, and `VTuneWidget.cpp`. This verifies that the patch matches that source fixture. It does not establish a current upstream commit, a clone-wide clean patch application, or compatibility with uncommitted/newer local changes.

## Explicitly not executed

- A full Rack SDK/plugin compile, link, load, or `make dist`.
- Compilation of the Rack-dependent widget/module integration translation units against the user's exact SDK/tree.
- In-Rack/VST panel placement, zoom, brightness, framebuffer, or context recreation testing.
- Runtime JSON lifecycle and neighboring-module removal/bypass tests in the Rack engine.
- CPU/GPU profiling or measured draw-time/latency benchmarking.
- Independent scientific verification of `Sound+Body.md` or any body-response model.

These are local integration/acceptance checks, not passed tests. See `INTEGRATION.md` for the concrete procedure.

## Reproducibility and interpretation

The exact uploaded PNG and Markdown are in `reference/`. `definition.json` records source-line associations and explicitly labels authored color, geometry, and interpolation decisions. `asset_manifest.json` records generated asset hashes. The delivered PNGs and generated header remove any need for Python in a normal plugin build.

The preview and C++ core implement matching formulas and use the same generated asset layers. They are separate implementations of the scalar equations, not a claim that a browser canvas produces pixel-identical NanoVG output. Host color/filtering/context differences remain part of local visual validation.
