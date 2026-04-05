# 🌀 Temporal Deck Expansion — Waveform Display Spec (v1)

## 0. Design Intent

The expansion module is a **read-only visual companion** to Temporal Deck.

It provides:

* a waveform-based representation of recent audio history
* real-time tracking of playback/scratch position
* a spatial mapping of “time as distance from NOW”

It must:

* remain performant under continuous live input
* remain responsive during aggressive scratching
* never interfere with audio-thread stability

---

# 1. Architecture Overview

## 1.1 Responsibility Split

### Temporal Deck (Host)

Responsible for:

* maintaining audio buffer
* maintaining preview summary (min/max bins)
* publishing canonical timeline state

### Expansion Module

Responsible for:

* mapping timeline → screen
* rendering waveform + markers
* implementing follow behavior (scratch tracking)

---

## 1.2 Communication Model

* Use **VCV expander double-buffered messaging**
* Host → Expander only (v1)
* Fixed-size POD struct
* No raw pointers to host memory
* No dynamic allocation in audio thread

---

# 2. Expander Protocol

## 2.1 Host → Expander Message

```cpp
namespace temporaldeck_expander {

constexpr uint32_t MAGIC = 0x54445831; // "TDX1"
constexpr uint16_t VERSION = 1;
constexpr uint32_t PREVIEW_BIN_COUNT = 4096;

struct PreviewBin {
    int16_t min;
    int16_t max;
};

struct HostToDisplay {
    uint32_t magic;
    uint16_t version;
    uint16_t size;

    uint64_t publishSeq;
    uint64_t bufferGeneration;

    uint32_t flags;

    float sampleRate;

    // Timeline state
    float lagSamples;
    float accessibleLagSamples;
    float platterAngle;

    // Sample mode
    float samplePlayheadSec;
    float sampleDurationSec;
    float sampleProgress;

    // Buffer info
    uint32_t bufferCapacityFrames;
    uint32_t bufferFilledFrames;

    // Preview
    uint32_t previewWriteIndex;
    uint32_t previewFilledBins;
    uint32_t samplesPerBin;

    PreviewBin preview[PREVIEW_BIN_COUNT];
};
}
```

---

## 2.2 Flags

```cpp
FLAG_SAMPLE_MODE
FLAG_SAMPLE_LOADED
FLAG_SAMPLE_PLAYING
FLAG_SAMPLE_LOOP
FLAG_FREEZE
FLAG_REVERSE
FLAG_SLIP
FLAG_PREVIEW_VALID
FLAG_MONO_BUFFER
```

---

## 2.3 Update Rules (Host)

### Scalars

Update every process block.

### Preview bins

* updated incrementally during audio write
* new bin finalized every `samplesPerBin`
* never recompute full history per frame

### Generation counter

Increment when:

* buffer mode changes
* sample loaded/rebuilt
* buffer cleared/reset
* live → sample conversion occurs

---

# 3. Preview Bin System (Host)

## 3.1 Source signal

```cpp
mono = 0.5f * (left + right);
```

## 3.2 Accumulation

```cpp
current.min = min(current.min, mono);
current.max = max(current.max, mono);

if (++count >= samplesPerBin) {
    bins[writeIndex] = current;
    writeIndex = (writeIndex + 1) % BIN_COUNT;
    reset current;
}
```

## 3.3 Bin sizing

```cpp
samplesPerBin = bufferCapacityFrames / PREVIEW_BIN_COUNT;
```

---

# 4. Expander Rendering Model

## 4.1 Core Concept

The expander renders:

> **A vertical timeline viewport over preview bins centered on read-head time**

Not:

* raw audio
* sample-accurate waveform

### 4.1.1 Orientation (required)

* **Top = backward in time** (older than read head)
* **Bottom = forward in time** (newer / closer to NOW)
* **Center = read-head anchor**

Waveform bars are drawn as **horizontal amplitude bars** per timeline row.
Amplitude is visualized left/right from centerline (or full-width fill by style),
while time advances vertically.

---

## 4.2 Two Update Domains

### A. Content updates (slow)

Triggered by:

* new bins
* generation change

Action:

* rebuild visible waveform slice

---

### B. Position updates (fast)

Triggered by:

* lag changes (scratch/playback)

Action:

* update viewport mapping
* update markers
* redraw

---

# 5. View Modes

## 5.1 Overview Mode (default)

* fixed window anchored to recent history
* read-head remains at center indicator
* waveform rows scroll relative to read-head lag evolution

---

## 5.2 Scratch-Follow Mode (required)

Activated when:

* active platter gesture
* OR high delta in lag

Behavior:

* viewport centers (or biases) around current read position
* waveform scrolls under marker

---

## 5.3 Transition Behavior

After scratch ends:

Option A (recommended):

* hold view briefly (~300–500ms)
* smoothly ease back to overview

---

# 6. View Mapping

## 6.1 Time Window Around Read Head

Target window:

* `900 ms` before center (older / upward)
* `900 ms` after center (forward / downward)

### Live-mode forward clamp

In live mode, forward time from the read head is limited by distance to NOW:

```cpp
availableForwardMs = (lagSamples / sampleRate) * 1000.0f;
forwardWindowMs = min(900.0f, availableForwardMs);
```

Backward window remains target `900 ms` unless bounded by available history.

If forward window is clamped, lower rows with no timeline coverage render as
empty/quiet background (not wrapped).

---

## 6.2 Convert lag → normalized/bin position

```cpp
normalized = lagSamples / accessibleLagSamples;
```

Clamp:

```cpp
normalized ∈ [0, 1]
```

---

## 6.3 Convert to preview bin index

```cpp
binPos = normalized * previewFilledBins;
```

Adjust for circular buffer:

```cpp
startIndex = (previewWriteIndex - previewFilledBins + binPos) mod BIN_COUNT;
```

---

## 6.4 Screen mapping (vertical timeline)

For each pixel row:

```cpp
binsPerPixel = visibleBins / widgetHeight

for each row:
    gather bins in range
    min = min(all mins)
    max = max(all maxes)
    draw horizontal amplitude bar
```

---

# 7. Rendering Strategy

## 7.1 Waveform

* horizontal min/max bars OR filled horizontal band
* mono only (v1)

## 7.2 Markers

* read/playhead marker (primary): **bright purple horizontal center line**
* optional:

  * NOW marker
  * accessible limit marker

## 7.3 Caching

Use `FramebufferWidget`:

* only redraw when dirty
* separate:

  * waveform body cache
  * marker overlay

---

# 8. Redraw Policy

## 8.1 Trigger full redraw when:

* new bins enter visible region
* generation changes
* view mode changes
* viewport shifts more than ~1 pixel

## 8.2 Marker-only redraw (optional optimization)

* when only lag changes

---

# 9. Performance Targets

Must ensure:

* no scanning raw buffer in UI
* no per-frame allocation
* bounded cost ~O(widget height)
* stable under:

  * continuous live input
  * rapid scratch motion

---

# 10. Threading Rules

* expander reads only `consumerMessage`
* no direct access to host memory
* no locks in audio thread
* no pointer sharing to vectors/buffers

---

# 11. First-Pass Constraints

## Included

* mono waveform
* fixed bin resolution
* scratch-follow mode
* overview mode
* read marker

## Excluded

* stereo display
* zoom controls
* spectral view
* smoothing/interpolation
* host control from expander

---

# 12. Acceptance Criteria

1. Waveform displays recent buffer content
2. Live input updates appear smoothly
3. Scratching causes view to follow position
4. No UI stutter during aggressive gestures
5. No audio performance degradation
6. No unsafe memory sharing

---

# 🔥 Final Insight

The key idea that makes this whole system work is:

> **The expander is not rendering audio — it is rendering time.**

Temporal Deck owns:

* *what exists*

The expander interprets:

* *where we are inside it*

---

If you want next, I can:

* write **exact C++ integration points** for the expander (Module + Widget)
* or sketch the **preview-bin integration into your existing engine loop** so Codex can implement it cleanly without regressions
