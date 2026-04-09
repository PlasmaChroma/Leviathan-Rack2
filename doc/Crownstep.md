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
* **Gate Width knob**

  * Fraction of clock cycle used for gate high time
  * Default: 0.50 (50%)
* **Run/Stop toggle**
* **New Game button** (prominent)

---

## 5. Inputs

### 5.1 CLOCK IN

* Gate/trigger
* Advances playback
* **Required in v1** (no internal clock for v1)

### 5.2 RESET IN

* Trigger
* Resets playback position only

### 5.3 SEQ LEN CV IN

* **Deferred for v1**
* v1 uses the Seq Length knob only, discretized to:

  * Full, 8, 16, 32, 64

### 5.4 TRANSPOSE CV IN

* CV input
* 1V/oct additive pitch offset
* Applied directly to pitch output path (after step pitch mapping)

### 5.5 ROOT CV IN

* CV input
* Semitone-domain root offset input
* `-10V -> -10 semitones`, `0V -> 0`, `+10V -> +10 semitones`
* Combined with Root knob, then wrapped to the 12-note key domain used by the selected scale

### 5.6 CV Range Policy (v1)

* Default assumption for CV input handling is Rack’s common useful range: **-10V to +10V**
* Inputs should be clamped to their effective working range before mapping
* Some controls may intentionally use narrower musical ranges (for finer control):

  * e.g. transpose/root style controls may map a reduced sub-range rather than the full +/-10V span
* Any narrowed mapping should be explicitly documented in implementation constants/comments

---

## 6. Outputs

### 6.1 PITCH OUT

* 1V/oct
* Quantized to selected scale

### 6.2 GATE OUT

* Gate per step
* Gate high duration is derived from:

  * `gateHighSeconds = gateWidthFraction * previousClockPeriodSeconds`
* `gateWidthFraction` comes from the Gate Width parameter (default 0.50)
* Startup behavior (before a valid period is known):

  * on the first received clock edge, gate goes high and remains high
  * gate is closed only when the next clock edge arrives
  * fractional gate-width timing begins once a prior clock period has been measured
* If a new clock edge arrives before the scheduled gate-off time:

  * current gate pulse is terminated immediately
  * next step gate is retriggered on the new clock edge
  * effective pulse width follows the newest measured period

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
* Default output range target in v1: **0V to +10V** from normalized move intensity

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

* Driven by **external clock input only** in v1
* Step index increments per trigger
* Wraps at `activeLength`
* Gate timing model:

  * measure `previousClockPeriodSeconds` from successive clock edges
  * if no valid previous period exists yet, hold gate high until next clock edge
  * on each clock edge, emit gate high and schedule gate low at
    `now + gateWidthFraction * previousClockPeriodSeconds`
  * if clock retriggers early, cancel pending gate-off and start the new pulse immediately

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

Where `transpose` and `root` are both driven by 1V/oct style offsets (knob + CV),
with root constrained to the module's semitone/scale-root domain before final quantization.

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
* v1 AI turn response timing is **immediate / event-driven** after the user move commits
* no rigid timing budget or delayed-response behavior is required in v1

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
4. AI responds immediately (event-driven; no fixed delay requirement)
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
