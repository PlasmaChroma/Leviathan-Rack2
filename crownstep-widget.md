# Crownstep Widget Spec

## Widget name

**CrownRibbonWidget**

## Purpose

Replace the current plain `currentStep / totalSteps` counter with a composite widget that:

* shows current playback position
* shows active loop length
* shows total move history
* shows whether playback is using **FULL** history or a **RECENT N** tail
* allows the user to change the recent-history cap directly from the widget

This widget should be both a **display** and a **control**.

## Core UX goal

At a glance, the user should understand:

* “Where am I now?”
* “How long is the currently audible loop?”
* “How much total game history exists?”
* “Am I hearing the full game or only the recent tail?”

Numbers remain available, but the primary readout should be **graphical first**.

---

## Recommended Rack implementation shape

### Primary class

Use an interactive widget derived from `rack::widget::OpaqueWidget`, since this widget should intentionally own hover and left-click interaction in its screen area instead of letting those events pass through. Rack documents that `OpaqueWidget` stops propagation of recursive position events, while still letting children consume first, and also consumes hover and left-click button events. ([VCV Rack][2])

```cpp
struct CrownRibbonWidget : rack::widget::OpaqueWidget
```

### Optional cached sublayer

Inside it, optionally add a `rack::widget::FramebufferWidget` child for static or rarely-changing visuals such as:

* bezel / frame
* background glass plate
* inactive grid / segment guides
* static labels

Rack documents that `FramebufferWidget` caches its children’s draw result and re-renders when dirty, which is useful for reducing redraw cost when most of the widget is stable. ([VCV Rack][3])

Suggested split:

* **static framebuffer child**: housing, base strip, decorative trim
* **dynamic direct draw**: current marker, active window highlight, flashes, text

---

# Functional behavior

## Inputs from module state

The widget reads these values from the module or module widget:

```cpp
struct CrownRibbonState {
	int historySize;        // total number of moves in current game
	int activeStart;        // index in history where current playback window begins
	int activeLength;       // number of steps currently in active playback window
	int playbackIndex;      // 0-based local step inside active window
	int capMode;            // 0=FULL, else discrete recent cap like 8/16/32/64
	bool running;           // playback running
	float eventFlash;       // generic recent-event flash decay
	float captureFlash;     // stronger flash for capture
	float kingFlash;        // special flash for kinging event
};
```

### Derived values

```cpp
bool fullMode = (capMode == 0);
int currentStepDisplay = (activeLength > 0) ? (playbackIndex + 1) : 0;
int activeLengthDisplay = activeLength;
int totalHistoryDisplay = historySize;
```

---

# Display modes

The widget uses **semantic zoom**. It draws differently depending on active sequence size.

## Mode A: Discrete mode

Trigger when:

```cpp
activeLength <= 16
```

### Visual behavior

* draw one visible tile per actual step
* each step is individually represented
* current step tile glows brightly
* accented steps can show stronger fill or a top pip
* king-derived steps can show a tiny crown notch or brighter edge
* if in RECENT mode, older full-history area remains visible as a dim background ribbon behind or above the active tiles

### Use case

This mode feels like a literal sequencer and is the most intuitive for short loops.

---

## Mode B: Compressed mode

Trigger when:

```cpp
17 <= activeLength && activeLength <= 48
```

### Visual behavior

* step display remains segmented, but thinner
* if space is insufficient, group multiple steps into a visual segment
* current playback position is shown by a precise vertical marker or crown pip
* accent density in a segment can brighten that segment

### Use case

Still step-oriented, but scalable.

---

## Mode C: Bucketed mode

Trigger when:

```cpp
activeLength > 48
```

### Visual behavior

* draw a fixed number of buckets, e.g. 24 or 32
* map active sequence into those buckets
* brightness or fill height indicates density / event richness
* current playback position is shown as a continuous-position marker over the bucket field
* the total history strip remains visible, with the active RECENT tail highlighted

### Use case

Abstract but readable for long sequences.

---

# Visual design specification

## Overall housing

Shape:

* wide rounded rectangle
* centered in bottom-middle panel space
* should read as a “special instrument display,” not a generic 7-segment readout

Suggested zones inside the widget:

* top-left: mode badge
* center: graphical ribbon
* bottom-center or overlay: numeric text
* optional top-right: tiny cap indicator arrows or glyphs

---

## Main graphical layer: Crown Ribbon

This is the dominant element.

### Always draw these three conceptual layers

#### 1. Full history base strip

Represents all recorded game moves.

* dim, low-contrast
* visible whenever `historySize > 0`
* should not dominate

#### 2. Active window highlight

Represents the region currently being looped.

* if FULL mode: highlight spans all existing history
* if RECENT mode: highlight only the tail region `[activeStart, activeStart + activeLength)`

This should feel like a **window** or **gate** applied to memory.

#### 3. Current playback marker

Represents current local playback position inside the active region.

* brightest element in the widget
* use a small crown-shaped pip, or a vertical luminous cursor with a tiny crown cap
* pulse slightly on step advance when running

---

## Numeric layer

Numbers are secondary, but still present for precision.

### Required text

Primary text:

* `09 / 16`
* means current local playback step / active loop length

Secondary text:

* in RECENT mode: `of 37`
* in FULL mode: `FULL 37`

### Badge text

Mode badge should show:

* `FULL`
* or `R16`, `R32`, `R64`

This keeps the cap readable without adding a separate knob label.

---

# Interaction model

Rack widgets expose `onButton()`, `onHover()`, `onDoubleClick()`, `onHoverScroll()`, drag events, and more, so this widget can support wheel changes, click-zones, and drag gestures in a single custom control. ([VCV Rack][1])

## Primary control target

The widget directly controls the sequence cap mode.

Allowed cap values:

```cpp
static constexpr int CAP_VALUES[] = {0, 8, 16, 32, 64};
```

Where:

* `0` means FULL

## Interaction behaviors

### Mouse wheel

* wheel up: next larger cap
* wheel down: next smaller cap
* wrap optional, but default should be clamped, not wrapped

Implementation point:

```cpp
void onHoverScroll(const HoverScrollEvent& e) override;
```

Rack provides `HoverScrollEvent` for scroll wheel movement while hovering. ([VCV Rack][4])

### Click left/right zones

Divide widget into thirds or edge-zones.

* left zone click: smaller cap
* right zone click: larger cap
* center zone click: no-op, or toggle FULL / last recent value

Implementation point:

```cpp
void onButton(const ButtonEvent& e) override;
```

Rack widgets receive button press/release events through `onButton()`. ([VCV Rack][1])

### Optional horizontal drag

User can drag horizontally across the ribbon to choose cap size.
Recommended only if Codex judges it stable and pleasant.

If implemented:

* snap to nearest allowed preset
* show live ghost highlight during drag
* commit on release

Implementation points:

```cpp
void onDragStart(const DragStartEvent& e) override;
void onDragMove(const DragMoveEvent& e) override;
void onDragEnd(const DragEndEvent& e) override;
```

Rack documents drag lifecycle events on widgets. ([VCV Rack][1])

### Double-click

Suggested behavior:

* double-click toggles between `FULL` and the most recently used recent cap

Implementation point:

```cpp
void onDoubleClick(const DoubleClickEvent& e) override;
```

Rack supports double-click events on widgets. ([VCV Rack][1])

---

# Rendering rules in detail

## General geometry

Let:

```cpp
float w = box.size.x;
float h = box.size.y;
```

Suggested layout:

* outer padding: `4–6 px`
* badge zone height: `10–12 px`
* main ribbon zone height: `12–18 px`
* text zone height: `10–12 px`

The exact values can be refined visually, but keep the ribbon dominant.

---

## Mode A rendering: Discrete

For short sequences, render literal step cells.

```cpp
int n = activeLength;
float gap = 1.5f;
float cellW = (usableWidth - gap * (n - 1)) / n;
```

### Cell styling

Each cell can encode:

* normal step: base glow
* accent step: brighter lower fill or edge
* king step: special highlight color or top cap
* current step: full glow + crown marker above

If per-step musical metadata is easy to read from the module, use it. Otherwise, in v1, only current-step highlighting is required.

---

## Mode B rendering: Compressed

For mid-size sequences:

* target a maximum visible segment count, such as 24 or 32
* compute grouping size:

```cpp
groupSize = ceil(activeLength / float(maxSegments));
```

Each segment represents a small run of steps.
Segment brightness can reflect:

* whether any steps in that run are accented
* whether current playback lies in that run

---

## Mode C rendering: Bucketed

For long sequences:

* fixed bucket count, e.g. 24
* map active window into buckets by normalized position
* fill bucket brightness or fill height by local density

### Bucket brightness inputs

At minimum:

* 1.0 if bucket contains any active steps
* plus small gain if bucket contains accented steps
* plus current-step overlay if playback marker falls inside

If rich metadata is unavailable at first, simple occupancy is fine.

---

# History-vs-window mapping

This is the conceptual core.

## Full history strip

Always normalize against `historySize`.

### Mapping helper

```cpp
float historyNorm(int historyIndex) {
	if (historySize <= 1) return 0.f;
	return historyIndex / float(historySize - 1);
}
```

## Active window

```cpp
int windowStart = activeStart;
int windowEnd   = activeStart + activeLength;
```

Draw a highlighted frame or brighter fill over that region.

## Current playback position

```cpp
int absolutePlaybackIndex = activeStart + playbackIndex;
float x = lerp(stripLeft, stripRight, historyNorm(absolutePlaybackIndex));
```

This is important because it visually explains that the active loop is a window into a larger game history.

---

# State update and animation

## Transient flashes

Maintain decaying animation variables in the module or widget:

* `eventFlash`
* `captureFlash`
* `kingFlash`

On step advance or new appended move:

* trigger a small pulse

On capture:

* trigger stronger amber pulse

On king:

* trigger gold pulse or marker bloom

Decay in `step()` or via time-based update.

---

## Dirtying / redraw policy

If using a `FramebufferWidget`, only mark the cached layer dirty when static or semi-static visuals change, such as:

* widget size change
* theme change
* mode threshold crossing
* cap mode change
* history size change if the base strip is cached

Leave the moving playback marker and numeric text in the dynamic layer so they redraw each frame or on playback changes. Rack’s `FramebufferWidget` is explicitly meant for cached child drawing that only re-renders when dirty. ([VCV Rack][3])

---

# Module integration spec

## New module-side state

Add a compact UI-facing state block or accessor:

```cpp
struct CrownRibbonUIState {
	int historySize;
	int activeStart;
	int activeLength;
	int playbackIndex;
	int capMode;
	bool running;
	float eventFlash;
	float captureFlash;
	float kingFlash;
};
```

Provide one method for the widget to pull this:

```cpp
CrownRibbonUIState CrownstepModule::getRibbonUIState() const;
```

## Cap mode writeback

Widget writes cap changes through a module method:

```cpp
void CrownstepModule::setSequenceCapMode(int newCapMode);
```

This should:

* update active window logic
* clamp playback index if needed
* trigger UI refresh

---

# Acceptance criteria

## Functional

* widget always shows meaningful state when history exists
* widget never crashes on empty history
* cap mode changes from mouse wheel and click-zones
* current playback marker tracks sequence playback correctly
* RECENT mode clearly differs visually from FULL mode

## Visual

* short sequences feel literal and step-based
* long sequences remain readable and do not degenerate into tiny unreadable LEDs
* current step is always the strongest visual signal
* the user can understand “current / active / total” without reading documentation

## Performance

* no unnecessary expensive redraw of static bezel/background
* dynamic redraw remains smooth during playback
* bucketed mode does not iterate absurdly over history every frame if history becomes large

---

# Suggested implementation skeleton

```cpp
struct CrownRibbonWidget : rack::widget::OpaqueWidget {
	CrownstepModule* module = nullptr;

	enum class VisualMode {
		DISCRETE,
		COMPRESSED,
		BUCKETED
	};

	int hoverZone = 0;          // -1 left, 0 center, +1 right
	int lastRecentCap = 16;

	CrownRibbonWidget() {
	}

	VisualMode chooseMode(int activeLength) const {
		if (activeLength <= 16) return VisualMode::DISCRETE;
		if (activeLength <= 48) return VisualMode::COMPRESSED;
		return VisualMode::BUCKETED;
	}

	void onHover(const HoverEvent& e) override;
	void onLeave(const LeaveEvent& e) override;
	void onHoverScroll(const HoverScrollEvent& e) override;
	void onButton(const ButtonEvent& e) override;
	void onDoubleClick(const DoubleClickEvent& e) override;
	void draw(const DrawArgs& args) override;

	void drawHousing(const DrawArgs& args);
	void drawBadge(const DrawArgs& args, const CrownRibbonUIState& s);
	void drawNumbers(const DrawArgs& args, const CrownRibbonUIState& s);
	void drawRibbon(const DrawArgs& args, const CrownRibbonUIState& s);
	void drawDiscrete(const DrawArgs& args, const CrownRibbonUIState& s);
	void drawCompressed(const DrawArgs& args, const CrownRibbonUIState& s);
	void drawBucketed(const DrawArgs& args, const CrownRibbonUIState& s);
	void drawCurrentMarker(const DrawArgs& args, const CrownRibbonUIState& s);
};
```

---

# Behavioral copy for Codex

Use this as the design intent paragraph inside the implementation task:

> Build a bottom-center custom widget called `CrownRibbonWidget` for Crownstep. It replaces the plain `current / total` display and also acts as the sequence-cap control. The widget must visually represent total move history, the active playback window, and the current playback position. It must adapt its rendering by sequence magnitude: literal per-step cells for short loops, compressed grouped segments for medium loops, and fixed-count bucketed density display for long loops. It must support wheel-based cap changes and simple click-zone cap changes. The widget should be graphically rich, readable at a glance, and feel like a signature instrument display rather than a utility readout.

---

# My recommendation for naming inside code

Public-facing concept:

* **Crown Ribbon**

Internal class names:

* `CrownRibbonWidget`
* `CrownRibbonUIState`

Alternate names if you prefer slightly more mythic flavor:

* `MemoryGateWidget`
* `RoyalSequenceRibbon`
* `CrownWindowWidget`

`CrownRibbonWidget` is the cleanest.
