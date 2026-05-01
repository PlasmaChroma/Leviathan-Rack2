# Wyrm Translation Unit Split Plan

## Goal

Split `src/Wyrm.cpp` into smaller translation units without changing behavior. The first pass should be mechanical: move existing code into clearer ownership boundaries, keep names and logic stable, then verify with the normal build and tests.

## Proposed Files

- `src/Wyrm.hpp`
  - Shared enums, constants, helper declarations, and core type declarations.
  - `WyrmShapeId`, `WyrmRockMouseMode`, `WyrmRock`.
  - `Wyrm` module declaration, including param/input/output/light IDs and public state used by widgets.
  - `WyrmFreqQuantity` declaration if it remains referenced from widget construction.

- `src/Wyrm.cpp`
  - `Wyrm` constructor, DSP processing, serialization, waveform reset/base-shape behavior, and non-UI state helpers.
  - `WyrmFreqQuantity` implementation if kept as a module-level quantity.
  - Model registration if the Rack plugin pattern allows keeping it here.

- `src/WyrmWaveEditor.cpp`
  - `WyrmWaveEditor` implementation.
  - Wave editor drawing, point hit testing, click/drag behavior, slither compensation, rock rendering, rock hover/drag, and rock collision/push math.
  - Keep rock/slither math here initially because it is currently editor-coupled. Extract later only if tests or reuse justify it.

- `src/WyrmWidget.cpp`
  - `WyrmWidget` implementation.
  - Panel construction, SVG anchor lookup, knob/button placement, context menu population, and icon button widgets.
  - Menu item structs can live here unless they become reused elsewhere.

## Refactor Order

1. Add `src/Wyrm.hpp` and include it from `src/Wyrm.cpp`.
2. Move declarations first, leaving implementations in `src/Wyrm.cpp`.
3. Split widget/menu/icon code into `src/WyrmWidget.cpp`.
4. Split wave editor code into `src/WyrmWaveEditor.cpp`.
5. Update the build file only after each new `.cpp` compiles locally.
6. Run `make -j4`.
7. Run `make test`.
8. Do a quick manual smoke pass in Rack for:
   - waveform editing
   - lock/unlock
   - reset
   - slither and speed
   - rocks menu, hover, drag, lift/drag modes

## Boundaries To Preserve

- Do not redesign slither or rock collision during the split.
- Do not rename params, JSON keys, or SVG anchor IDs.
- Keep serialization behavior byte-for-byte compatible where practical.
- Keep `Wyrm` state ownership unchanged; widgets should still read/write the same module fields.
- Avoid introducing new abstractions unless required to break circular dependencies.

## Risks

- Rack widget types often need complete declarations at construction sites, so forward declarations may not be enough for some nested widget/menu structs.
- Anonymous namespace helpers in `Wyrm.cpp` may need to become `static` functions in the owning `.cpp` or declarations in `Wyrm.hpp`.
- Menu items and editor code may currently depend on implementation details of `Wyrm`; prefer exposing small public helpers only if direct state access becomes too messy.
- Build scripts may already glob `src/*.cpp`; if not, the new files need to be added explicitly.

## Suggested Commit Shape

One commit for the mechanical split only. Follow-up commits can then clean up ownership, extract rock/slither helpers, or add focused tests once the code is easier to navigate.
