# Proc Crash-Risk Findings

## Summary

Static review of `src/Proc.cpp` found no current enum/widget count mismatch and no stale references to removed `ATTENUATE_1_PARAM` or `OUT_1_OUTPUT`.

The main remaining crash-risk candidates are schema drift under a stable module slug, unsanitized non-finite CV values reaching the preview path, and exception-prone SVG preview-rect parsing.

## Findings

### 1. Stable `Proc` slug with changing schema

`Proc` has changed params, outputs, and widget layout repeatedly while keeping the same module slug. That makes old `Proc` instances and saved patches a plausible crash source during development.

References:
- `plugin.json:28`
- `src/Proc.cpp:13`
- `src/Proc.cpp:761`

### 2. Non-finite CVs can propagate into timing and preview code

Incoming CVs are not checked with `std::isfinite()` before being used in the timing path. A `NaN` or `Inf` from another module could propagate through `bothTimeScaleFromCv()` and `computeStageTime()`, then into preview state and finally UI draw coordinates.

References:
- `src/Proc.cpp:195`
- `src/Proc.cpp:466`
- `src/Proc.cpp:537`
- `src/Proc.cpp:929`
- `src/Proc.cpp:1006`

### 3. Preview rect parsing can throw during widget construction

`loadPreviewRectMm()` uses `std::regex` and `std::stof()` without exception handling. Since `Proc` depends on parsing `CH1_PREVIEW` from `res/flux.svg`, a malformed preview rect can terminate Rack when the module widget is created.

References:
- `src/Proc.cpp:1045`
- `src/Proc.cpp:1061`
- `src/Proc.cpp:1067`

### 4. UI font dereference is unchecked

The preview draw path assumes `APP->window->uiFont` is valid and dereferences it without a guard. This is less likely than the items above, but it is still an unchecked UI pointer access.

Reference:
- `src/Proc.cpp:1036`

## Not Found

- No current param/input/output/light count mismatch.
- No stale removed-symbol access for `ATTENUATE_1_PARAM` or `OUT_1_OUTPUT`.
- No obvious out-of-bounds access in the current `process()` path.

## Recommended Next Steps

1. Temporarily change the module slug to `ProcDev` while iterating.
2. Add `std::isfinite()` guards around incoming CVs and preview values.
3. Wrap `loadPreviewRectMm()` parsing in a `try/catch` and fall back to hardcoded bounds on failure.
