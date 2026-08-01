# Crownstep Settings Overlay — Codex Implementation Specification

## 1. Objective

Replace Crownstep’s large module-specific Rack context-menu hierarchy with an in-panel **Settings Overlay** opened by a dedicated button on the module face.

The overlay should:

- occupy nearly the full Crownstep widget area;
- present settings in a structured, visually coherent tabbed interface;
- make common options discoverable and easy to cycle;
- prevent interaction with the underlying board while open;
- allow non-destructive settings to update immediately;
- stage destructive game-setup changes until explicitly confirmed;
- include the **New Game** action with a confirmation dialog;
- preserve patch compatibility and Crownstep’s existing DSP/game behavior.

This is a UI/UX restructuring project, not a redesign of Crownstep’s musical or game engines.

---

## 2. Existing Implementation Summary

Crownstep’s module-specific settings currently live in `CrownstepWidget::appendContextMenu()` in:

- `src/CrownstepUI.cpp`

The current menu includes several distinct conceptual groups:

### Game

- Game Mode
  - Checkers
  - Chess
  - Reversi/Othello
- Player
  - Cause
  - Effect
- AI Difficulty
- Highlight Color
- Board Texture

### Quantizer

- Enable Quantization
- Scale
- Key

### Pitch

- Range
- Show Cell Pitch Values
- Bipolar
- Smooth Melody
- Board Layout
- Randomize Layout
- Inverted
- Pitch Source

The panel currently exposes a physical `SmallGoldButton` bound to `NEW_GAME_PARAM`.

Important current semantics:

- changing **Game Mode** from the context menu starts a fresh game;
- changing **Player** also starts a fresh game;
- `startNewGame()` clears the board state, move history, generated sequence history, winner/game-over state, animation state, and related UI state;
- pitch, mapping, appearance, and quantization settings are already serialized;
- pitch interpretation is calculated from move history during playback, so most pitch settings can remain live;
- Crownstep already has focused tests in `tests/crownstep_spec.cpp` and persistence coverage in `tests/crownstep_persistence_spec.cpp`.

---

## 3. UX Direction

## 3.1 Full-Panel Dark-Glass Overlay

Use a nearly opaque panel covering the Crownstep module interior.

Recommended treatment:

- dark neutral background;
- approximately `0.94–0.97` alpha;
- subtle cyan/purple edge illumination;
- opaque enough that text and controls remain legible over every board texture;
- faint underlying board silhouette may remain visible, but should not compete with settings;
- no real-time blur shader.

A true blurred translucent panel is unnecessary and risks rendering cost, platform differences, and reduced legibility. The target should feel like an integrated Crownstep control surface rather than a generic Rack popup.

The overlay should remain inside the module bounds. It should not create a floating native menu or extend over neighboring modules.

---

## 3.2 Four Tabs

Use four compact tabs:

1. **GAME**
2. **PITCH**
3. **MAP**
4. **LOOK**

This separates behavior according to user intent rather than mirroring the old context-menu nesting.

### GAME

- Game Mode
- Player Role
- AI Difficulty
- New Game / Apply & New Game

### PITCH

- Quantize
- Key
- Scale
- Range
- Bipolar
- Smooth Melody

### MAP

- Pitch Source
- Board Layout
- Inverted
- Randomize Layout
- Show Cell Pitch Values

### LOOK

- Highlight Color
- Board Texture

Optional later addition:

- Step Counter Style

`stepCounterStyle` is already part of Crownstep’s state, but it is not necessary to expand this project beyond the existing user-facing settings during the MVP.

---

## 3.3 Compact “Cycle Row” Interaction

Most enum settings should use a consistent horizontal row:

```text
SETTING LABEL        <   CURRENT VALUE   >
```

Behavior:

- clicking the left arrow selects the previous value;
- clicking the right arrow selects the next value;
- values wrap around;
- clicking the value pill may also advance to the next value;
- mouse wheel over the row may cycle values, provided it does not interfere with slider interaction;
- disabled rows use reduced contrast and do not accept pointer events.

This is preferable to nested dropdowns within the already narrow 18HP panel.

Boolean settings should use a similarly styled toggle row:

```text
QUANTIZE                         [ ON ]
```

The Range control should remain a horizontal slider with a numeric semitone display.

---

## 4. State-Application Model

Settings must be divided into two classes.

## 4.1 Live Settings

These apply immediately when adjusted:

- AI Difficulty
- Quantization Enabled
- Key
- Scale
- Pitch Range
- Bipolar
- Smooth Melody
- Pitch Source
- Board Layout
- Board Layout Inverted
- Randomize Board Layout
- Show Cell Pitch Values
- Highlight Color
- Board Texture

Live changes should continue to serialize through the existing module fields.

Pitch-affecting changes should call:

```cpp
module->refreshHeldPitchForCurrentStep();
```

This ensures the currently held playback pitch reflects the new interpretation without waiting for another move or playback transition.

At minimum, call the refresh helper after changes to:

- quantization enabled;
- key;
- scale;
- range;
- bipolar;
- melodic/smooth mode;
- pitch source;
- board layout;
- inverted state;
- random seed/layout randomization.

Calling it after every pitch-tab or map-tab mutation is acceptable and simpler than trying to optimize individual cases.

---

## 4.2 Staged Game-Setup Settings

The following settings are destructive under current Crownstep semantics:

- Game Mode
- Player Role

Do **not** apply these directly while the user is browsing the overlay.

The overlay maintains:

```cpp
int pendingGameMode;
int pendingPlayerMode;
```

When the overlay opens, copy the module’s current values into these fields.

Changing either control updates only the pending value.

When pending values differ from the active module state:

- show a small `NEW GAME REQUIRED` indicator;
- change the primary action label from `NEW GAME` to `APPLY & NEW GAME`.

Closing the overlay without confirmation discards the pending game setup. All live-applied settings remain applied.

This gives the overlay a clean rule:

> Exploratory adjustments are safe; changing the rules or sides only takes effect through an explicit new-game action.

---

## 5. New Game Confirmation

Pressing `NEW GAME` or `APPLY & NEW GAME` opens an in-overlay modal confirmation layer.

Suggested copy:

```text
START A NEW GAME?

The current board and generated move history
will be cleared. Your settings will be kept.

[CANCEL]                  [START NEW GAME]
```

When staged settings differ, optionally add:

```text
Checkers → Chess
Cause → Effect
```

Only include lines that actually changed.

### Confirmation Semantics

On confirmation:

1. cancel any pending AI work;
2. apply the pending game mode without independently starting a game;
3. apply the pending player mode;
4. call `startNewGame()` exactly once;
5. resynchronize pending values from the active module;
6. close the confirmation modal;
7. either:
   - keep the settings overlay open so the result can be reviewed, or
   - close the full overlay and return to the board.

Recommended MVP behavior: **close the full overlay after a confirmed new game**, because the action represents completion and immediately reveals the new board.

Add a module-level helper to centralize the operation:

```cpp
void Crownstep::applyGameSetupAndStartNewGame(
    int requestedGameMode,
    int requestedPlayerMode
);
```

Suggested implementation:

```cpp
void Crownstep::applyGameSetupAndStartNewGame(
    int requestedGameMode,
    int requestedPlayerMode
) {
    cancelAiTurnWork();

    requestedGameMode = clamp(
        requestedGameMode,
        0,
        GAME_MODE_COUNT - 1
    );
    requestedPlayerMode = clamp(
        requestedPlayerMode,
        0,
        PLAYER_MODE_COUNT - 1
    );

    // Preserve same-mode state such as a user-selected Chess texture.
    if (requestedGameMode != gameMode) {
        setGameMode(requestedGameMode, false);
    }
    playerMode = requestedPlayerMode;
    startNewGame();
}
```

Use the project’s existing clamp conventions and enum names rather than copying this pseudocode literally if they differ.

Do not call the existing context-menu pathways in sequence, because both currently have fresh-game behavior and could cause redundant resets.

---

## 6. Overlay Lifecycle

## 6.1 Opening

A UI-only settings button opens the overlay.

On open:

```cpp
void CrownstepSettingsOverlay::open() {
    activeTab = GAME; // or preserve for current Rack session
    confirmationOpen = false;
    syncPendingGameSetup();
    visible = true;
}
```

The active tab may be remembered for the current Rack session, but it should not be serialized into the patch.

Do not persist whether the overlay was open.

---

## 6.2 Closing

The close button:

- closes the confirmation modal first if one is open;
- otherwise hides the full overlay;
- discards staged game-mode/player-role changes;
- preserves all live settings already applied.

Recommended hierarchy:

- `X` close button in the top-right;
- `Esc` cancels the confirmation modal first;
- a second `Esc` closes the settings overlay.

Do not close the overlay when the user clicks the dark backdrop; the overlay already occupies the entire panel and accidental closure would be easy.

---

## 6.3 Underlying Module Behavior

While the overlay is visible:

- the engine continues processing;
- playback continues;
- clocks and CV continue to work;
- AI servicing continues;
- only direct panel interaction is intercepted.

The overlay must block:

- board clicks;
- knobs, switches, and jacks beneath it;
- wheel events;
- drag events;
- context interaction intended for underlying custom widgets.

Use `OpaqueWidget` or equivalent event consumption rather than relying only on visual coverage.

Rack cable rendering may remain visible above or around the module according to normal Rack behavior; this project should not attempt to suppress global cable rendering.

---

## 7. Settings Button and Existing New Game Param

## 7.1 Replace the Visible New Game Button

Remove the visible `SmallGoldButton` for `NEW_GAME_PARAM` from the Crownstep panel UI.

Place the settings button in that region, or at a new nearby anchor, so the panel does not need additional vertical space.

Recommended icon:

- compact gear;
- three horizontal sliders;
- Crownstep-specific crown/control glyph.

The button should be a UI widget, not a parameter.

---

## 7.2 Preserve `NEW_GAME_PARAM`

Do not remove, reorder, or repurpose `NEW_GAME_PARAM`.

Keep:

- the enum entry;
- `configButton()`/parameter configuration;
- the process-side Schmitt-trigger handling.

Reasons:

- patch and parameter-index compatibility;
- potential automation or remote-control compatibility;
- reduced implementation risk.

The parameter can remain “headless”: supported by the module but no longer represented by a faceplate ParamWidget.

Do not use `NEW_GAME_PARAM` as the settings button. The new settings button should only control local UI visibility.

---

## 7.3 SVG Anchor

Add a dedicated panel anchor:

```text
SETTINGS_BUTTON
```

Use the project’s existing `PanelSvgUtils`/components-layer lookup.

For transitional robustness, the implementation may:

1. search for `SETTINGS_BUTTON`;
2. fall back to the old `NEW_GAME_PARAM` anchor if not found;
3. fall back to a hardcoded safe position only in development/preview.

Update the panel icon/label accordingly.

---

## 8. Context Menu After Migration

Keep the standard Rack context menu.

Remove the large Crownstep-specific submenu tree after the overlay reaches feature parity.

Add one module-specific item:

```text
Open Crownstep Settings…
```

Selecting it calls `CrownstepWidget::openSettings()`.

This provides:

- discoverability for users accustomed to right-clicking;
- accessibility when the panel button is obscured;
- a fallback entry point without duplicating the settings implementation.

Do not keep two independent implementations of the settings controls.

---

## 9. Detailed Tab Specification

## 9.1 GAME Tab

### Game Mode

Cycle values using the existing mode names:

- Checkers
- Chess
- Reversi/Othello, matching current UI terminology

Writes to `pendingGameMode`.

### Player Role

Cycle values:

- Cause
- Effect

Writes to `pendingPlayerMode`.

Add concise helper text beneath the value where space permits:

- Cause: `You move first`
- Effect: `AI moves first`

Use the precise actual behavior implied by Crownstep’s current side mapping. If a game mode can alter that meaning, use neutral labels instead:

- Cause: `Initial side`
- Effect: `Following side`

### AI Difficulty

Cycle through the existing five named difficulty levels.

Applies immediately. It affects future AI searches; it should not restart the current game.

### Primary Action

Normal state:

```text
NEW GAME
```

Pending setup state:

```text
APPLY & NEW GAME
```

Both routes open the confirmation modal.

---

## 9.2 PITCH Tab

### Quantize

Boolean toggle.

When off:

- Key and Scale remain visible;
- they may be dimmed but should retain their configured values;
- editing them while quantization is off is optional.

Recommended: allow editing while off, but reduce their value-pill brightness slightly. This lets users prepare settings before enabling quantization.

### Key

Cycle the existing 12 keys.

### Scale

Cycle the existing 13 scales.

### Range

Use the current `RANGE_PARAM`.

Show the current value as semitones:

```text
RANGE                     24 ST
```

The control must use normal Rack parameter APIs so automation, undo history, and serialization continue to work.

Use the existing parameter quantity rather than assigning the module field directly.

### Bipolar

Boolean toggle.

### Smooth Melody

Boolean toggle mapped to the existing melodic-bias/smoothing field.

Add a short one-line description only if layout permits:

```text
Reduces abrupt melodic jumps
```

---

## 9.3 MAP Tab

### Pitch Source

Cycle:

- Origin Square
- Destination Square
- Blend

Use existing names from Crownstep’s core definitions.

### Board Layout

Cycle the existing layout modes:

- Center-Out
- Linear Horizontal
- Linear Vertical
- Linear Diagonal
- Serpentine Horizontal
- Serpentine Vertical
- Serpentine Diagonal
- Random

Use the exact existing strings if they differ.

### Inverted

Boolean toggle.

### Randomize Layout

Action button.

Behavior:

- update the existing board-layout random seed;
- enable the action only while the persistent Random layout mode is selected;
- preserve Random as the selected layout mode;
- immediately refresh the held pitch.

Suggested label:

```text
RESHUFFLE MAP
```

This is clearer than presenting “Randomize” as if it were a persistent enum value.

### Show Cell Pitch Values

Boolean toggle.

This belongs in MAP rather than LOOK because it is primarily an aid to understanding the board-to-pitch mapping.

---

## 9.4 LOOK Tab

### Highlight Color

Cycle:

- Purple
- Cyan
- Green
- Off

### Board Texture

Cycle current texture options.

When the active or pending game mode is Reversi/Othello and the mode enforces a fixed visual treatment:

- disable the texture row;
- display `FIXED FOR REVERSI`, or the currently enforced texture;
- do not silently accept a texture choice that cannot be shown.

When applying a staged game mode, preserve existing `setGameMode()` behavior, including any game-specific texture default such as Chess selecting Wood.

Do not add texture-reapplication logic unless a product decision explicitly changes the existing semantics.

---

## 10. Visual Layout

Crownstep is approximately 18HP, so the overlay should be dense but not cramped.

Suggested structure:

```text
┌──────────────────────────────────┐
│ CROWNSTEP SETTINGS            [X]│
│                                  │
│ [GAME] [PITCH] [MAP] [LOOK]      │
│ ──────────────────────────────── │
│                                  │
│ GAME MODE       <  CHECKERS  >   │
│ PLAYER ROLE     <   CAUSE    >   │
│ AI DIFFICULTY   < HURT ME... >   │
│                                  │
│        [ NEW GAME ]              │
│                                  │
│ Destructive changes require      │
│ starting a new game.             │
└──────────────────────────────────┘
```

### Recommended Sizing

Use actual widget dimensions at runtime rather than assuming a fixed pixel resolution.

Approximate logical layout:

- outer inset: 6–8 px;
- header height: 24–28 px;
- tabs: 22–26 px;
- row height: 30–36 px;
- row gap: 3–5 px;
- bottom action region: 42–54 px;
- corner radius: 4–7 px.

The overlay should scale correctly with Rack zoom.

---

## 11. Suggested Class Structure

Create dedicated files:

- `src/CrownstepSettingsOverlay.hpp`
- `src/CrownstepSettingsOverlay.cpp`

Avoid adding another large block to the already substantial `CrownstepUI.cpp`.

### Public Overlay Interface

```cpp
struct CrownstepSettingsOverlay : OpaqueWidget {
    enum Tab {
        TAB_GAME,
        TAB_PITCH,
        TAB_MAP,
        TAB_LOOK,
        TAB_COUNT
    };

    Crownstep* module = nullptr;
    Tab activeTab = TAB_GAME;

    int pendingGameMode = GAME_MODE_CHECKERS;
    int pendingPlayerMode = PLAYER_INIT;
    bool confirmationOpen = false;

    explicit CrownstepSettingsOverlay(Crownstep* module);

    void open();
    void close();

    void syncPendingGameSetup();
    bool hasPendingGameSetup() const;

    void requestNewGame();
    void confirmNewGame();
    void cancelConfirmation();

    void draw(const DrawArgs& args) override;
    void step() override;
    void onButton(const ButtonEvent& e) override;
    void onHoverScroll(const HoverScrollEvent& e) override;
    void onSelectKey(const SelectKeyEvent& e) override;
};
```

Adjust event APIs to the Rack SDK version used by the repository.

### Reusable Internal Widgets

Suggested components:

```cpp
struct CrownstepSettingsOpenButton;
struct CrownstepSettingsCloseButton;
struct CrownstepSettingsTabButton;
struct CrownstepSettingsCycleRow;
struct CrownstepSettingsToggleRow;
struct CrownstepSettingsRangeRow;
struct CrownstepSettingsActionButton;
struct CrownstepSettingsConfirmDialog;
```

These may remain in an anonymous namespace in the `.cpp` unless another module is expected to reuse them.

Prefer callback-based UI controls or small typed bindings rather than one monolithic coordinate-checking `onButton()` function.

Example cycle-row API:

```cpp
struct CrownstepSettingsCycleRow : OpaqueWidget {
    std::string label;
    std::function<int()> getValue;
    std::function<void(int)> setValue;
    std::function<std::string(int)> getValueName;
    int valueCount = 0;
    std::function<bool()> isEnabled;
};
```

Wrap safely when cycling:

```cpp
int wrapIndex(int value, int count) {
    if (count <= 0)
        return 0;
    return (value % count + count) % count;
}
```

---

## 12. CrownstepWidget Integration

Add members to `CrownstepWidget`:

```cpp
CrownstepSettingsOverlay* settingsOverlay = nullptr;
CrownstepSettingsOpenButton* settingsButton = nullptr;

void openSettings();
void closeSettings();
```

During construction:

1. build the existing panel and normal controls;
2. add the settings button at `SETTINGS_BUTTON`;
3. construct the overlay with the module pointer;
4. size it to the full module box;
5. set `visible = false`;
6. add it last so it is above Crownstep’s other child widgets.

Example:

```cpp
settingsOverlay = new CrownstepSettingsOverlay(module);
settingsOverlay->box.pos = Vec(0.f, 0.f);
settingsOverlay->box.size = box.size;
settingsOverlay->visible = false;
addChild(settingsOverlay);
```

If the widget size is not finalized at construction, synchronize the overlay box in `step()` only when dimensions change, or after panel initialization. Avoid unnecessary per-frame layout rebuilding.

`CrownstepWidget::step()` must continue calling the existing AI UI-thread service function regardless of overlay visibility.

---

## 13. Module Preview and Null Safety

Rack module-browser previews may construct `CrownstepWidget` with `module == nullptr`.

Requirements:

- no dereference of `module` during overlay construction;
- settings button may open a read-only preview overlay or remain disabled;
- rows should display defaults or `—`;
- all callbacks must guard `module`;
- confirmation actions must be disabled;
- no crash in the module browser.

Recommended: leave the button visually present but disabled in preview mode.

---

## 14. Rendering and Performance

Use NanoVG primitives:

- rectangles;
- rounded rectangles;
- simple strokes;
- subtle linear/radial gradients;
- cached fonts;
- existing Leviathan color constants where available.

Do not use:

- live Gaussian blur;
- repeated SVG/image loading in `draw()`;
- dynamically allocated strings or widgets every frame;
- framebuffer recreation every frame;
- any audio-thread UI work.

Rows should be constructed once and rebound/synchronized through getters.

The overlay itself is UI-thread-only.

---

## 15. Undo and Parameter Semantics

For `ROOT_PARAM`, `SCALE_PARAM`, and `RANGE_PARAM`, use Rack’s parameter/quantity APIs and history gestures so host automation, undo, and normal parameter behavior remain intact.

For non-param state fields, match the existing context-menu semantics. Full Rack undo support for every menu state is not required for this MVP unless the current code already provides it.

Do not introduce new Param/Input/Output/Light IDs.

Do not reorder existing enum IDs.

Do not reinterpret `NEW_GAME_PARAM` as a settings parameter.

---

## 16. Serialization

No new serialization should be needed for settings already stored by Crownstep.

Do not serialize:

- overlay visibility;
- active settings tab;
- pending game mode;
- pending player role;
- confirmation-dialog state.

Pending values are transient UI state.

Confirm that every live setting exposed in the overlay maps to an existing serialized field or param.

---

## 17. Testing Strategy

## 17.1 Unit Tests

Extend `tests/crownstep_spec.cpp`.

Add tests for the new module helper:

### Apply Setup and Start New Game

Arrange:

- create a Crownstep module;
- establish a non-empty move history;
- modify board/game state;
- request a different game mode and player mode.

Assert:

- requested game mode is active;
- requested player mode is active;
- board is initialized for the new mode;
- move history is empty;
- generated history/sequence is empty as expected;
- game-over/winner state is reset;
- the module is ready for the correct next side;
- no duplicated reset side effects are visible.

### Same Setup, New Game

Call the helper with current game mode/player role.

Assert that it still resets the game correctly.

### Invalid Values

Pass out-of-range values.

Assert safe clamping and no crash.

---

## 17.2 Persistence Tests

Extend `tests/crownstep_persistence_spec.cpp` only where needed.

Confirm round-trip persistence for settings surfaced by the overlay, especially:

- game mode;
- player mode;
- AI difficulty;
- quantization;
- key;
- scale;
- pitch source;
- board layout;
- inverted;
- random seed;
- bipolar;
- smooth melody;
- highlight;
- board texture;
- pitch-value overlay.

The overlay’s transient state must not appear in JSON.

---

## 17.3 UI-State Testing

Consider extracting staged setup into a tiny testable non-Rack helper:

```cpp
struct CrownstepSettingsDraft {
    int gameMode;
    int playerMode;

    void syncFrom(const Crownstep& module);
    bool differsFrom(const Crownstep& module) const;
};
```

This is optional. Do not over-engineer UI-state tests if the Rack test harness does not conveniently instantiate widgets.

---

## 17.4 Manual Rack Checklist

Verify on the repository’s authoritative Windows Rack build:

- settings button opens overlay;
- overlay exactly fits Crownstep at multiple Rack zoom levels;
- no board/control click-through;
- tabs switch cleanly;
- cycle rows wrap in both directions;
- text does not clip at longest values;
- Range automation and display remain correct;
- pitch settings update currently held playback pitch;
- live changes survive patch save/reload;
- Game Mode and Player Role do not change before confirmation;
- closing discards staged setup;
- New Game confirmation Cancel changes nothing;
- confirmation applies staged setup;
- current board and move sequence are cleared once;
- AI behavior resumes correctly;
- texture row behavior is correct for Reversi/Othello;
- right-click `Open Crownstep Settings…` works;
- module-browser preview does not crash;
- cables and playback continue normally while overlay is open.

Run the project’s focused Crownstep test target and `test-fast` according to repository guidance.

---

## 18. Implementation Phases

### Phase 1 — State and Shell

- add `applyGameSetupAndStartNewGame()`;
- add overlay files;
- add settings button;
- render overlay shell, header, tabs, and close button;
- verify event blocking and preview safety.

### Phase 2 — Controls

- implement reusable cycle/toggle/range/action rows;
- bind all live settings;
- add held-pitch refresh calls;
- implement disabled states.

### Phase 3 — Staging and Confirmation

- implement pending game mode/player role;
- add dirty-state indicator;
- add confirmation modal;
- wire Apply & New Game.

### Phase 4 — Panel and Context Cleanup

- replace visible New Game button artwork with Settings artwork;
- add `SETTINGS_BUTTON` SVG anchor;
- remove module-specific context-menu tree;
- add `Open Crownstep Settings…`.

### Phase 5 — Tests and Polish

- add helper/persistence tests;
- run focused and fast tests;
- verify Windows Rack build;
- refine spacing, text truncation, hover/focus states, and visual hierarchy.

---

## 19. Acceptance Criteria

The implementation is complete when:

1. A dedicated panel button opens Crownstep’s full-panel Settings Overlay.
2. The overlay uses GAME, PITCH, MAP, and LOOK organization.
3. Every existing Crownstep-specific context-menu setting has an overlay equivalent.
4. Underlying panel interactions are blocked while the overlay is visible.
5. Playback, CV processing, and AI servicing continue while it is open.
6. Live settings apply immediately and continue to serialize.
7. Pitch-related changes refresh the currently held playback pitch.
8. Game Mode and Player Role remain staged until a confirmed new game.
9. Closing without confirmation discards staged destructive changes.
10. New Game presents a confirmation dialog explaining that board and move history will be cleared.
11. Confirmation applies staged setup and calls `startNewGame()` exactly once.
12. The existing visible New Game control is replaced by the Settings button.
13. `NEW_GAME_PARAM` remains in place for compatibility.
14. The right-click menu contains a single Crownstep settings entry rather than duplicated nested controls.
15. The overlay is safe with `module == nullptr`.
16. No Param/Input/Output/Light IDs are added, removed, reordered, or repurposed.
17. Focused Crownstep tests and project fast tests pass.
18. The authoritative Windows Rack build succeeds.

---

## 20. Explicit Non-Goals

Do not include the following in this task:

- changes to game rules;
- changes to AI algorithms;
- new pitch-generation algorithms;
- a general-purpose overlay framework for every Leviathan module;
- native operating-system dialogs;
- true background blur;
- serialization of UI navigation state;
- a universal Apply/Cancel transaction for all live settings;
- removing the hidden `NEW_GAME_PARAM`;
- redesigning the entire Crownstep panel.

---

## 21. Codex Execution Notes

Before editing:

1. read `Agents.md`;
2. inspect current Crownstep enum names and helper functions rather than assuming pseudocode identifiers;
3. inspect the components layer in Crownstep’s panel SVG;
4. identify the exact Rack event API version used by neighboring custom widgets;
5. preserve existing formatting conventions.

During implementation:

- make small compile-safe steps;
- avoid broad unrelated refactors;
- do not stage or commit files;
- keep UI code out of the audio path;
- avoid locking from `process()` beyond existing patterns;
- do not duplicate existing setting-name tables;
- reuse Crownstep’s existing enum-to-name functions;
- preserve current Chess/Reversi texture semantics;
- keep `serviceAiTurnFromUiThread()` active while the overlay is open.

Expected changed files:

```text
src/CrownstepUI.cpp
src/CrownstepShared.hpp
src/CrownstepModule.cpp
src/CrownstepSettingsOverlay.hpp       (new)
src/CrownstepSettingsOverlay.cpp       (new)
res/... Crownstep panel/components SVG
tests/crownstep_spec.cpp
tests/crownstep_persistence_spec.cpp   (only if coverage is missing)
Makefile or source list                (only if explicit registration is required)
```

Final Codex report should include:

- files changed;
- settings mapped to each tab;
- explanation of staged versus live behavior;
- confirmation-flow behavior;
- compatibility decisions;
- tests run and results;
- any visual/manual validation still required in Rack.
