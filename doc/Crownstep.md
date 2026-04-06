Dragon King Leviathan—**Crownstep** is *the* name. It encodes both domains cleanly:

* **Crown** → kinging, transformation, escalation
* **Step** → sequencing, time, rhythm

It reads instantly. It remembers itself.

Let’s crystallize this into a **Codex-ready implementation spec**—tight, buildable, and aligned with everything we’ve refined.

---

# 🎛 Crownstep — Checkers-Based Sequencer

## Codex-Ready Design Specification (VCV Rack 2)

---

## 1. Overview

**Crownstep** is a VCV Rack module that transforms a game of checkers into a musical sequence.

* The user plays against an internal AI opponent
* Each move (human or AI) appends one step to a **shared sequence**
* The sequence is played back via standard CV/Gate outputs
* The user can constrain playback to **recent history (N steps)** or the **full game**

---

## 2. Panel Layout

## Structure

```text
[         CHECKERS BOARD (PRIMARY UI)         ]

[----------- CONTROL STRIP -------------------]

[ INPUTS (LEFT)        OUTPUTS (RIGHT)       ]
```

---

## 3. Module Dimensions

* Target width: **18HP (preferred)**
* Max: 20HP
* Panel height: standard Rack height (128.5 mm)

### Board sizing (18HP target)

* Panel width: ~91.4 mm
* Board width: ~85 mm usable
* Square size: ~10.5–11.0 mm

---

## 4. UI Components

### 4.1 Board (Center)

* 8×8 grid (dark squares interactive only)
* Click interaction:

  * click piece → highlight legal moves
  * click destination → execute move

### Visual feedback:

* last move highlight
* capture flash
* king indicator (visual distinction)
* turn indicator (player vs AI)

---

### 4.2 Control Strip (below board)

* **Seq Length knob**

  * Values: Full, 8, 16, 32, 64
* **Root knob**
* **Scale selector** (menu or switch)
* **Run/Stop toggle**
* **New Game button** (prominent)

---

## 5. Inputs

### 5.1 CLOCK IN

* Gate/trigger
* Advances playback

### 5.2 RESET IN

* Trigger
* Resets playback position only

### 5.3 SEQ LEN CV IN

* CV input
* Quantized to:

  * Full, 8, 16, 32, 64

### 5.4 TRANSPOSE CV IN

* CV input
* Applied to pitch output

### 5.5 ROOT CV IN

* CV input
* Offsets root note

---

## 6. Outputs

### 6.1 PITCH OUT

* 1V/oct
* Quantized to selected scale

### 6.2 GATE OUT

* Gate per step

### 6.3 ACCENT OUT

* Gate/CV:

  * 0 → normal move
  * 1 → capture
  * > 1 → multi-capture / king

### 6.4 MOD OUT

* Continuous CV representing move intensity:

  * base move
  * +capture weight
  * +multi-jump weight
  * +kinging bonus

### 6.5 EOC OUT

* Trigger on sequence loop

---

## 7. Core Data Structures

```cpp
struct Step {
    float pitch;
    bool gate;
    float accent;
    float mod;
};
```

```cpp
std::vector<Step> history;
```

---

## 8. Sequence Logic

### 8.1 Append on Move

Each move generates exactly one Step:

```cpp
void appendStep(Move move) {
    Step s;

    s.pitch = mapPitch(move);
    s.gate = true;
    s.accent = computeAccent(move);
    s.mod = computeMod(move);

    history.push_back(s);
}
```

---

### 8.2 Active Window

```cpp
int cap = sequenceCap; // 0 = Full

int activeLength = (cap == 0)
    ? history.size()
    : std::min(cap, (int) history.size());

int startIndex = (cap == 0)
    ? 0
    : std::max(0, (int) history.size() - activeLength);
```

Playback reads:

```cpp
history[startIndex + stepIndex]
```

---

### 8.3 Playback

* Driven by clock (external or internal)
* Step index increments per trigger
* Wraps at `activeLength`

---

## 9. Pitch Mapping

### Base mapping

* Use 32-square indexing (dark squares only)

```cpp
int idx = move.originIndex; // 0–31
int scaleDegree = idx % scaleLength;
int octave = idx / scaleLength;
```

```cpp
pitch = scale[scaleDegree] + octaveOffset + transpose + root;
```

---

### Enhancements

* King:

  * +12 semitones OR alternate mapping mode
* Directional moves:

  * optional inversion logic (future)

---

## 10. Accent Mapping

```cpp
float computeAccent(Move m) {
    if (m.isMultiCapture) return 2.0f;
    if (m.isCapture) return 1.0f;
    if (m.isKing) return 1.5f;
    return 0.0f;
}
```

---

## 11. Mod Mapping

```cpp
float computeMod(Move m) {
    float v = 0.2f;

    if (m.isCapture) v += 0.3f;
    if (m.isMultiCapture) v += 0.3f;
    if (m.isKing) v += 0.4f;

    return clamp(v, 0.0f, 1.0f);
}
```

---

## 12. Game Engine (Checkers)

### Requirements

* legal move generation
* forced capture enforcement
* multi-jump handling
* king promotion
* turn alternation

---

## 13. AI Engine (Minimal)

### Strategy

* shallow minimax (depth 2–4)
* heuristic scoring:

  * piece count
  * king count
  * mobility
  * capture availability

---

### Context Menu

Right-click menu:

* AI Difficulty:

  * Easy
  * Normal
  * Hard

---

## 14. Interaction Flow

1. User presses **New Game**
2. User makes first move
3. Step appended
4. AI responds
5. Step appended
6. Sequence grows
7. Playback runs independently

---

## 15. Edge Cases

### Empty history

* no output (gate low)

### Cap > history

* play full history

### Game over

* sequence remains
* playback continues

---

## 16. Future Extensions (not v1)

* Move Event Out
* Game Over Out
* AI personality modes
* Board-state CV scanning
* CV-controlled moves
* Save/load game sequences

---

## 17. Identity Summary

Crownstep is:

* not random
* not purely algorithmic
* not static

It is:

> **a deterministic system shaped by adversarial play,
> rendered as musical sequence**

---

## Final Note

This spec is intentionally:

* minimal where needed
* extensible where it matters
* aligned with your UI and HP constraints

---

If you want next, I can generate:

* **exact SVG coordinate layout (mm-based)**
* component layer definitions for your helper tool
* or a **starter C++ module skeleton for Rack SDK** ready to drop into your plugin

This one is absolutely worthy of becoming a flagship alongside your other modules.
