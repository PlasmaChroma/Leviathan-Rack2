Here is a comprehensive module specification designed to act as an ironclad blueprint for an AI agent (or senior developer) to implement a fully functional, real-time Doom engine port inside a VCV Rack module.

---

Specification: Doom Engine Integration Module (id: "ChronoDoom") 

## 1. Executive Summary & Design Scope

* **Objective:** Embed a functional, headless port of **Chocolate Doom** into a VCV Rack module v2+ using the native C++ SDK.

* **Asset Policy:** The module binary will contain zero copyrighted assets. It will operate purely on a "Bring Your Own WAD" (BYOWAD) infrastructure. The user must supply their own WAD file via a file browser before the engine can start (see §4.4).

* **Core Architectural Goal:** Isolate the deterministic 35 Hz Doom ticker execution entirely from the high-frequency real-time audio thread to prevent any UI freeze or audio drops.

### 1.1 Engine Selection: Chocolate Doom

**Chocolate Doom** (https://github.com/chocolate-doom/chocolate-doom) is the chosen engine fork for the following reasons:

* **Faithful to original:** It reproduces the original Doom engine behavior exactly — no renderer extensions, no gameplay modifications. This means a smaller, simpler codebase to strip and embed.
* **Clean C codebase:** Well-structured, portable C code with clear separation between engine logic and platform I/O layers (`i_video.c`, `i_sound.c`, `i_system.c`). These are the only files that need replacement with Rack-specific shims.
* **Active maintenance:** Long-standing project with stable releases and well-documented internals.
* **SDL2 dependency removal:** Chocolate Doom uses SDL2 for video/audio/input. Our integration replaces all SDL2 calls with Rack equivalents, so SDL2 is **not** a dependency of the built plugin.

### 1.2 Licensing

* **Chocolate Doom** is licensed under **GPL-2.0-or-later**.
* **Leviathan** is licensed under **GPL-3.0**.
* GPL-3 is compatible with GPL-2+ code. The combined work is distributed under GPL-3, which is already the Leviathan license. No license change is required.
* The `src/doom/` directory must retain all original copyright headers and include Chocolate Doom's `COPYING` file.


---

## 2. Threading & Ring-Buffer Topology

To prevent breaching VCV Rack's microsecond audio execution budget ($< 2.0\,\mu\text{s}$ per sample) or causing UI lag, execution must be decoupled across three distinct threads:

```
+-----------------------------------+
|  UI Thread (draw() via NanoVG)    | <--- Reads 320x200 32-bit RGBA Framebuffer
+-----------------------------------+
                  ^
                  | Non-blocking Texture Update
+-----------------------------------+
|  Doom Worker Thread (35Hz Ticker) | 
+-----------------------------------+
                  |
                  | Thread-Safe RingBuffer Populated
                  v
+-----------------------------------+
|  Audio Thread (process() @ 48kHz) | ---> Pushes Low-Latency Stereo Audio out to Rack
+-----------------------------------+

```

### 2.1 Audio Thread Safeties

* 
**Strict Constraints:** The `process(const ProcessArgs& args)` loop must perform zero memory allocations (`malloc`/`new`), zero disk operations, and zero thread-blocking mutex acquisitions.


* 
**Inter-Thread Conduit:** Use `rack::dsp::RingBuffer<Frame, 2048>` to push interleaved stereo floating-point audio frames from the Doom sound mixer out to the real-time audio pipeline safely.


### 2.2 Thread Lifecycle

The Doom worker thread must be managed carefully to avoid dangling threads or race conditions on module removal:

* **Spawn:** The worker thread is created only after a valid WAD has been loaded (see §4.4). It runs a loop that calls `D_RunFrame()` (or equivalent single-tick entry point) at 35 Hz, sleeping between ticks.
* **Shutdown Signal:** Use a `std::atomic<bool> running{false}` flag. When set to `false`, the worker thread exits its loop cleanly on the next iteration.
* **Module `onRemove()` / Destructor:** Set `running = false`, then `thread.join()` to block until the worker has exited. This ensures no dangling thread accesses freed module memory.
* **WAD Swap (§4.4.5):** Set `running = false`, join the thread, tear down engine state, re-initialize with the new WAD, then spawn a fresh worker thread.
* **Bypass/Disable:** When the module is bypassed, the worker thread should continue running (to keep the game state alive) but the audio ring buffer output should be silenced in `process()`.

### 2.3 Audio Resampling

Doom's internal sound mixer runs at a fixed sample rate (typically **11025 Hz** in Chocolate Doom). Rack's audio thread runs at the user's configured rate (commonly 44100, 48000, or 96000 Hz). The ring buffer alone does not bridge this gap.

* **Resampling Strategy:** Use simple linear interpolation to upsample from Doom's mixer rate to `args.sampleRate` in `process()`. This is cheap enough for real-time and sufficient quality for Doom's 8-bit audio.
* **Rate Tracking:** In `process()`, consume samples from the ring buffer at a rate of `doomSampleRate / args.sampleRate` samples per output sample. Maintain a fractional accumulator to handle non-integer ratios.
* **Underrun Handling:** If the ring buffer is empty (engine hasn't ticked yet, or is stalled), output silence — never block.
* **Overrun Handling:** If the ring buffer is nearly full, the worker thread should drop the oldest audio rather than stalling the tick loop.


---

## 3. Graphical Pipeline & Interface Rendering

The module's UI layout must map Doom's legacy display constraints to NanoVG's hardware-accelerated vector rendering.

* 
**Framebuffer Management:** * Doom output resolution is fixed at $320 \times 200$ pixels (8-bit indexed color converted to 32-bit RGBA texture arrays).


* Within the `draw(const DrawArgs& args)` method of your module widget, the data must be bound using NanoVG image handlers. **Important:** Follow the `NvgGraphicsLifecycle.hpp` patterns established in the Leviathan codebase — NanoVG image handles are context-owned and must be invalidated/recreated on context change (common in DAW window close/reopen):


```cpp
// Member variables
int doomImage = -1;
int doomImageW = 0, doomImageH = 0;
NVGcontext* ownerVg = nullptr;

void draw(const DrawArgs& args) override {
    using namespace nvg_gfx_lifecycle;

    // Detect OpenGL context change — invalidate the old handle
    if (clearCacheOnContextSwitch(args.vg, ownerVg, nullptr)) {
        doomImage = -1;  // old handle is now invalid, do not delete
    }

    // (Re)create the texture if needed
    if (doomImage < 0 || !ownedNvgImageSizeMatches(args.vg, doomImage, 320, 200)) {
        resetOwnedNvgImage(ownerVg, doomImage, doomImageW, doomImageH, args.vg, true);
        doomImage = nvgCreateImageRGBA(args.vg, 320, 200, NVG_IMAGE_NEAREST, doom_rgba_buffer);
        ownerVg = args.vg;
    }

    // Update texture data only if the engine has ticked a new frame
    if (dirtyFrame.exchange(false) && doomImage >= 0) {
        nvgUpdateImage(args.vg, doomImage, doom_rgba_buffer);
    }

    // Blit the viewport
    if (doomImage >= 0) {
        NVGpaint imgPaint = nvgImagePattern(args.vg, x, y, w, h, 0.0f, doomImage, 1.0f);
        nvgBeginPath(args.vg);
        nvgRect(args.vg, x, y, w, h);
        nvgFillPaint(args.vg, imgPaint);
        nvgFill(args.vg);
    }
}
```




* 
**UI Frame Divider:** Because drawing is expensive, do not force an OpenGL re-render if the Doom engine hasn't ticked a new frame. Check against a "dirty frame" atomic flag updated by the 35 Hz worker thread before executing raw GPU blits.


### 3.1 Panel Size & Viewport Layout

* **Vertical Fill Target:** The Doom viewport should consume the entire usable vertical range of the Rack panel (`RACK_GRID_HEIGHT` = 380 mm). The viewport rectangle should span from just below the top screws to just above the bottom screws.

* **Aspect Ratio Correction:** Doom's raw framebuffer is $320 \times 200$ (8:5), but the original game was rendered on 4:3 CRT monitors with non-square pixels. The viewport must apply **4:3 pixel-aspect-ratio correction** (i.e., stretch the 320×200 image to a 4:3 rectangle) to match the original visual intent.

* **Width Derivation:** Given the full vertical extent $H$ of the panel, the viewport width is $W = H \times \frac{4}{3}$. At $H \approx 380\,\text{mm}$, this gives $W \approx 507\,\text{mm}$ ($\approx 33.8\,\text{HP}$).

* **Jack Margins:** The total module width must be wider than the viewport to leave room on the **left and right sides** for columns of CV input/output jacks (see §4). Target roughly $2$–$3\,\text{HP}$ of margin on each side for jack placement, making the total module width approximately **38–40 HP**.

* **Layout Summary:**
```
  |<-- ~2-3 HP -->|<-------- ~34 HP Doom Viewport -------->|<-- ~2-3 HP -->|
  |   CV Jacks    |   4:3 corrected 320×200 framebuffer    |   CV Jacks    |
  |   (Inputs)    |                                         |   (Outputs)   |
```


### 3.2 Keyboard Input Capture

Rack modules do not receive keyboard input by default. ChronoDoom needs a robust "hover-to-capture" strategy that feels natural without hijacking the rest of the Rack UI.

#### 3.2.1 Focus Acquisition

* **Hover-to-Focus:** When the mouse cursor enters the Doom viewport rectangle, the module widget should call `APP->event->setSelectedWidget(this)` to claim keyboard focus. This makes the widget the receiver of `onSelectKey` events.

* **Visual Focus Indicator:** When the viewport has keyboard focus, render a subtle border glow or corner brackets so the user knows their keystrokes are being captured. This is critical UX — without it, users will accidentally type into Doom when they meant to interact with Rack.

* **Focus Release:** When the mouse cursor **leaves** the viewport rectangle, call `APP->event->setSelectedWidget(nullptr)` to release focus back to Rack. This must also happen if the user right-clicks (to open the Rack context menu) or if the module is bypassed/disabled.

#### 3.2.2 Key Event Routing

Override `onSelectKey(const SelectKeyEvent& e)` on the viewport widget to intercept keyboard input:

| Key(s) | Doom Action | Notes |
| --- | --- | --- |
| `W` / `↑` | `cmd.forwardmove` (+) | Forward |
| `S` / `↓` | `cmd.forwardmove` (−) | Backward |
| `A` | `cmd.sidemove` (−) | Strafe left |
| `D` | `cmd.sidemove` (+) | Strafe right |
| `←` / `→` | `cmd.angleturn` | Turn left / right |
| `Space` / `Ctrl` | `BT_ATTACK` flag | Fire weapon |
| `Shift` | Run modifier | Doubles move speed |
| `1`–`7` | Weapon select | Direct weapon slot |
| `E` / `Enter` | `BT_USE` flag | Open doors / switches |

* **Consume vs. Pass-through:** The handler must call `e.consume(this)` for game-bound keys to prevent them from bubbling up to Rack. **Do not consume** modifier combos that Rack uses globally (e.g., `Ctrl+Z`, `Ctrl+S`, `Ctrl+C/V`). A simple rule: if `e.mods & RACK_MOD_MASK` contains `Ctrl` or `Alt`, pass the event through unless it is a Doom-specific binding.

* **Key State Tracking:** Maintain a small `std::bitset` or `bool[]` of currently-pressed game keys, updated on both key-down and key-up events. The 35 Hz ticker thread reads this snapshot atomically each tick to build the `ticcmd_t` for that frame.

#### 3.2.3 Mouse Look (Stretch Goal)

* **Optional Mouse Capture:** If the user clicks inside the viewport, enter a "mouse-captured" mode where relative mouse motion maps to `cmd.angleturn` (horizontal look). Display a small "Press ESC to release" overlay.

* **Implementation:** Use `APP->event->setSelectedWidget(this)` combined with `onDragMove` or `onHover` delta tracking. On `ESC` keypress, release mouse capture and restore normal Rack cursor behavior.

* This is a stretch goal — CV-driven turning (§4) is the primary interface; mouse look is a convenience for manual play.

#### 3.2.4 Interaction with CV Inputs

Keyboard input and CV inputs target the same `ticcmd_t` fields. When both are active simultaneously, they should be **additive** (clamped to the engine's max values). This lets a user play manually while a sequencer modulates weapon selection or strafing via CV.


---

## 4. Voltage I/O Matrix (CV Mappings)

### 4.1 CV Inputs (Scaled from 0.0V–10.0V to Engine Coordinates)

| Hardware Port | Target Subsystem | Action |
| --- | --- | --- |
| **X-MOVE IN** | Player Input Struct | Maps $-5\text{V}$ to $+5\text{V}$ directly to `cmd.sidemove` (Strafe). |
| **Y-MOVE IN** | Player Input Struct | Maps $-5\text{V}$ to $+5\text{V}$ directly to `cmd.forwardmove` (Walk/Run). |
| **FIRE GATE** | Weapon Firing Logic | Rising edge triggers `ATTACK` bit flag state in player ticcmds. |
| **WEAPON CV** | Inventory Controller | Stepped voltage quantizes to inventory arrays ($0\text{V} = \text{Pistol}$, $5\text{V} = \text{Plasma}$, etc.). |

### 4.2 CV Outputs (0.0V–10.0V Signals Generated by Game State)

* **HEALTH OUT ($0\text{V}$ to $10\text{V}$):** Tracks player health dynamically (e.g., $100\% \text{ health} = 10\text{V}$). Useful for modulating external CV filters based on your survival state.
* **FRAG TRIG (10ms $+10\text{V}$ Pulse):** Fired on a callback hook whenever `P_DamageMobj` results in an enemy death event. Hook this to a VCV drum module or envelope generator for rhythmic reinforcement of carnage.

### 4.3 Audio Outputs

The Doom engine's sound output (SFX + music) is routed to a dedicated pair of **stereo audio jacks** on the module panel:

| Hardware Port | Signal | Notes |
| --- | --- | --- |
| **AUDIO L** | Left channel audio | Resampled from Doom's mixer rate to Rack's sample rate (see §2.3) |
| **AUDIO R** | Right channel audio | Resampled from Doom's mixer rate to Rack's sample rate (see §2.3) |

* **Signal Level:** Output at Rack audio standard ($\pm 5\text{V}$ peak). Scale the Doom mixer's 16-bit integer output to $[-5\text{V}, +5\text{V}]$ floating point.
* **Source:** These jacks carry the combined output of Doom's sound effects and music mixer. The ring buffer pipeline described in §2.1 feeds directly into these jacks via the `process()` loop.
* **When no WAD is loaded:** Output $0\text{V}$ (silence).
* **When module is bypassed:** Output $0\text{V}$ (silence), but the engine continues running internally (see §2.2).
* **Placement:** The L/R audio jacks should be placed in the right-side jack margin alongside the CV outputs (see §3.1 layout).

### 4.4 WAD Initialization & File Browser

The module ships with **zero game assets**. Before the Doom engine can start, the user must point the module at a valid WAD file.

#### 4.4.1 Uninitialized State

When no WAD path is configured (fresh module instantiation), the viewport should render an **idle splash screen** instead of a black rectangle. Display a message such as:

```
   ╔══════════════════════════════╗
   ║   CHRONODOOM — NO WAD LOADED ║
   ║                              ║
   ║   Right-click → Load WAD…    ║
   ╚══════════════════════════════╝
```

The engine ticker thread must **not** be spawned until a valid WAD is loaded.

#### 4.4.2 File Browser (osdialog)

WAD selection uses the same `osdialog` library already used by other Leviathan modules (Iris, Temporal Deck). The browser is triggered from the **right-click context menu** on the module widget:

```cpp
menu->addChild(createMenuItem("Load WAD…", "", [=]() {
    osdialog_filters* filters = osdialog_filters_parse("Doom WAD:wad,WAD");
    char* pathC = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
    osdialog_filters_free(filters);
    if (pathC) {
        std::string path(pathC);
        std::free(pathC);
        module->loadWad(path);  // validate + boot engine
    }
}));
```

#### 4.4.3 Validation

Before booting the engine, the module must validate the selected file:

1. **File exists** and is readable.
2. **WAD header check:** First 4 bytes are `IWAD` or `PWAD`.
3. **Minimum viable lumps:** Verify essential lumps like `PLAYPAL`, `E1M1` (or `MAP01`), etc. exist in the directory.

If validation fails, show an error via `osdialog_message(OSDIALOG_ERROR, ...)` and remain in the uninitialized state.

#### 4.4.4 Path Persistence (Plugin-Level Config)

Once the user has pointed ChronoDoom at a valid WAD, that path should be saved as **plugin-level configuration** — not per-patch. This means every new ChronoDoom instance immediately knows where the WAD is, and the user only has to browse once.

Storage follows the existing Leviathan settings pattern (see `visual_assets::saveSettings()` / `loadSettings()`):

* **Location:** `asset::user() / "Leviathan" / "chronodoom.json"`
* **Contents:**
```json
{
  "wadPath": "/home/user/games/doom/doom2.wad"
}
```
* **Write on change:** Whenever the user selects a new WAD via the file browser, validate it, then immediately write `chronodoom.json`.
* **Read on construction:** Every ChronoDoom module reads this config in its constructor. If the file exists and the path is still valid, boot the engine automatically — no user interaction needed.
* **Stale path handling:** If the saved path no longer exists (file moved/deleted), fall back to the uninitialized splash screen and log a warning via `WARN()`. Do **not** show a blocking dialog on startup.

> **Per-patch override (optional, future):** A module could additionally store a `"wadPath"` in `dataToJson()` to allow per-patch WAD overrides (e.g., a total conversion WAD for a specific project). If present, the per-patch path takes priority over the global config. This is a stretch goal and not required for the initial implementation.

#### 4.4.5 WAD Swapping

The user may load a different WAD at any time via the context menu. When this happens:

1. Shut down the running Doom ticker thread cleanly.
2. Tear down the old engine state.
3. Re-initialize with the new WAD.

This allows switching between `doom1.wad` (shareware), `doom.wad`, `doom2.wad`, or total conversion WADs without removing and re-adding the module.

---

## 5. Implementation Roadmap for Agentic Coding

### 5.1 Source Tree Layout

The Doom engine source must be isolated from the existing Leviathan module code to keep the repository clean and the build boundary clear.

```
src/
├── ChronoDoom.cpp          # Module class (process(), dataToJson, etc.)
├── ChronoDoomWidget.cpp    # ModuleWidget, viewport, input handling
├── doom/                   # ← All engine-derived code lives here
│   ├── d_main.c / .h       # Stripped headless Doom entry point
│   ├── g_game.c / .h       # Game tick loop
│   ├── p_*.c / .h          # Play simulation (map, mobj, etc.)
│   ├── r_*.c / .h          # Software renderer (column/span drawers)
│   ├── w_wad.c / .h        # WAD loader
│   ├── s_sound.c / .h      # Sound engine (adapted for ring-buffer output)
│   ├── i_video_rack.c      # Rack-specific video shim (framebuffer handoff)
│   ├── i_sound_rack.c      # Rack-specific audio shim (ring-buffer push)
│   └── ...                 # Other engine modules as needed
├── IntegralFlux.cpp
├── Proc.cpp
├── ...                     # Other existing Leviathan modules
```

* **`src/doom/`** contains the ported/stripped Chocolate Doom engine source. This is a modified fork — platform-specific I/O layers (`i_video`, `i_sound`, `i_system`) are replaced with thin Rack-specific shims that hand off framebuffer data and audio samples to the module. Must retain all original copyright headers and Chocolate Doom's `COPYING` file.

* **`src/ChronoDoom*.cpp`** contains the VCV Rack module and widget code. This is "our" code that speaks the Rack API and calls into `src/doom/` as a library.

* **Build integration:** The current `Makefile` uses `$(wildcard src/**/*.cpp)` which picks up C++ files in subdirectories, but does **not** compile `.c` files under `src/`. Add the Doom engine C sources explicitly:

```makefile
# Add to existing SOURCES line:
SOURCES += $(wildcard src/doom/*.c)
```

Keep the engine sources as C (not C++) to minimize porting friction from the upstream Doom codebase. Use `extern "C"` wrappers in the ChronoDoom C++ files when calling into the engine.

### 5.2 Module Registration

Follow the existing Leviathan pattern for registering the new module:

1. **`plugin.json`:** Add a new entry to the `"modules"` array:
```json
{
  "slug": "ChronoDoom",
  "name": "ChronoDoom",
  "description": "Embedded Doom engine with CV input/output",
  "tags": ["Visual", "External"],
  "hidden": true
}
```

2. **`src/plugin.hpp`:** Add the extern declaration (append at end of the list):
```cpp
extern Model* modelChronoDoom;
```

3. **`src/plugin.cpp` `init()`:** Add the registration (append at end):
```cpp
p->addModel(modelChronoDoom);
```

3. **`src/ChronoDoom.cpp`:** Define the model:
```cpp
Model* modelChronoDoom = createModel<ChronoDoomModule, ChronoDoomWidget>("ChronoDoom");
```

### 5.3 Implementation Phases

1. **Phase 1: Skeleton Module**
    * Create `ChronoDoom.cpp` and `ChronoDoomWidget.cpp` with an empty module (no params/inputs/outputs yet).
    * Register in `plugin.json` and `plugin.cpp`.
    * Render a static splash screen ("NO WAD LOADED") in the viewport area.
    * Implement the right-click context menu with "Load WAD…" file browser.
    * Implement `chronodoom.json` config read/write.
    * **Milestone:** Module appears in Rack, shows splash screen, file browser opens and persists a path.

2. **Phase 2: Headless Engine Compilation**
    * Clone Chocolate Doom source into `src/doom/`.
    * Strip SDL2 dependencies: replace `i_video.c`, `i_sound.c`, `i_system.c`, `i_input.c` with stub/shim implementations.
    * Add `$(wildcard src/doom/*.c)` to the Makefile.
    * Get the engine compiling and linking into the plugin without errors.
    * **Milestone:** Plugin builds with Doom engine code linked in (engine not yet running).

3. **Phase 3: Engine Boot & Framebuffer**
    * Implement `loadWad()` — validate WAD, call `D_DoomMain()` (or equivalent) with the WAD path.
    * Spawn the 35 Hz worker thread. Run the tick loop.
    * Implement `PLAYPAL` → RGBA palette conversion in `i_video_rack.c`. Write the 320×200 framebuffer each tick.
    * Blit the framebuffer to the NanoVG viewport using the context-safe pattern (§3).
    * **Milestone:** Doom title screen renders in the module viewport.

4. **Phase 4: Keyboard Input**
    * Implement hover-to-focus keyboard capture (§3.2).
    * Wire key events through to the engine's `ticcmd_t` builder.
    * **Milestone:** Player can navigate menus and walk around E1M1 using WASD.

5. **Phase 5: Audio Pipeline**
    * Implement `i_sound_rack.c` — adapt Doom's sound mixer to push samples into the ring buffer.
    * Implement linear-interpolation resampling in `process()` (§2.3).
    * Wire up stereo output jacks.
    * **Milestone:** Doom sound effects and music play through Rack's audio output.

6. **Phase 6: CV Integration**
    * Add input/output ports and params as specified in §4.
    * Wire CV inputs to `ticcmd_t` fields (additive with keyboard, §3.2.4).
    * Wire game state to CV outputs (health, frag trigger).
    * **Milestone:** Full playable module with CV control.