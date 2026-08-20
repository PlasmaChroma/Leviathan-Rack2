# Leviathan — Consequence Engine

## Implementation Specification v0.2

**Status:** Refined v1 design contract  
**Module slug:** `ConsequenceEngine`  
**Width:** 20 HP  
**Module class:** Interactive generative pitch/gate event ecology

The Consequence Engine is a playable generative instrument in which a performed
note can persist as a process. That process may re-emit, mutate, branch, interact
with neighboring processes, recombine, and eventually die.

The central pool is the primary instrument surface, not a decorative display. It
must be equally playable from:

- direct mouse gestures;
- conventional Rack pitch/gate CV;
- polyphonic X/Y/touch CV acting as virtual fingers.

This document defines a bounded v1. It intentionally excludes a full fluid
simulation, a scale editor, and live-ecology patch persistence.

### Requirement language

`MUST` and `MUST NOT` define v1 requirements. `SHOULD` identifies the preferred
implementation when no repository constraint prevents it. `MAY` identifies
optional polish. Unqualified statements are design requirements.

---

## 1. Product goals

In descending order:

1. Musical playability
2. Clear cause and effect
3. Low input-to-output latency
4. Deterministic and understandable emergence
5. DSP efficiency
6. Rendering efficiency
7. Visual spectacle

The module MUST remain musically complete and understandable with optional
visual effects disabled. DSP behavior MUST NOT depend on the renderer, visual
quality, UI frame rate, or whether the module widget is visible.

The target experience is **controlled emergence**: the player may be surprised
by what happens, but should usually understand enough of why it happened to
intervene again.

---

## 2. Core model and terminology

A performed note creates an internal persistent `Event`:

```text
performed note
    ↓
persistent Event
    ↓
re-emission / mutation / branching
    ↓
interaction / recombination
    ↓
descendants and further consequences
```

The ecology follows three conceptual phases:

```text
SEPARATE  → Events differentiate into descendants.
COMPARE   → Nearby Events perceive and influence one another.
RECOMBINE → Interactions can create new musical state.
```

These are architectural ideas, not panel modes.

Keep the following concepts distinct:

- **Event:** a persistent ecological process. An Event can live without
  currently sounding.
- **Emission:** a request by an Event to produce pitch/gate/energy/generation
  output.
- **Voice:** one of 16 output slots temporarily assigned to an Emission or a
  held performance.
- **Controller:** a mouse pointer, virtual touch channel, or pitch/gate input
  channel that may hold an Event.
- **Seed Event:** a generation-0 Event created by the player or external CV.
- **Descendant:** an Event created by branching or recombination.

Living Events MUST NOT map directly to Rack polyphonic channels. Event identity
and output voice identity are independent.

---

## 3. Panel and interaction hierarchy

Rack uses 15 px per HP, so a 20 HP module is approximately 300 × 380 px. The
pool SHOULD occupy 55–65% of the usable panel height.

```text
┌─────────────────────────────┐
│      CONSEQUENCE ENGINE     │
│                             │
│          THE POOL           │
│    ○             ◌          │
│         ○────○              │
│  ·                 ●        │
│             ◎               │
│                             │
├─────────────────────────────┤
│ CONSEQ  LIFE   TIME  BRANCH │
│ MUTATE  MEMORY AFFIN DENSITY│
├─────────────────────────────┤
│         CLEAR   REROLL      │
├─────────────────────────────┤
│ INPUTS              OUTPUTS │
└─────────────────────────────┘
```

Exact art and spacing may evolve, but:

- the pool MUST be large enough for deliberate mouse performance;
- knobs and jacks MUST NOT overlap the pool;
- inputs and outputs MUST be visually distinct;
- performance controls MUST be distinguishable from ecology controls;
- core operation MUST NOT depend on hidden gestures.

---

## 4. Primary parameters

Implement eight ecology controls. Parameter values below describe the semantic
value exposed to the player; an internal normalized value may be used where
appropriate.

### 4.1 CONSEQUENCE

**Range:** 0–1  
**Default:** 0.35

Master amount of autonomous future behavior.

At 0:

- mouse, touch, and pitch/gate gestures sound normally;
- held Seed Events remain visible and playable;
- no autonomous action, mutation, branching, or recombination occurs;
- a Seed Event retires when its controller releases it and its output gate
  closes.

As CONSEQUENCE rises:

- released Events retain more energy;
- autonomous re-emission becomes more likely;
- branching and interaction outcomes gain influence;
- the ecology is increasingly likely to outlive the original gesture.

CONSEQUENCE is a master depth, not a replacement for LIFE, TIME, BRANCH,
MUTATE, MEMORY, or AFFINITY. It SHOULD scale their autonomous effects while
preserving the recognizable function of each control.

### 4.2 LIFE

**Range:** 0.5–60 s, logarithmic  
**Default:** 8 s

LIFE sets expected Event lifetime, not output-gate duration. At birth, each
Event receives a deterministic variation around the base value. Variation MUST
be bounded so the knob remains predictive; a starting range of 0.75×–1.25× is
recommended.

A held Seed Event does not expire. Its age may advance, but lifetime countdown
and energy loss pause until release.

### 4.3 TIME

**Free-running range:** 50 ms–4 s, logarithmic  
**Default:** approximately 500 ms

TIME controls the base interval between autonomous actions. Individual Events
receive bounded deterministic variation around this value.

When CLOCK is connected, TIME selects one of five clock-period multipliers:

```text
4×    2×    1×    1/2×    1/4×
```

For example, `2×` means one autonomous opportunity every two input clocks;
`1/2×` means two equally spaced opportunities per measured clock period. The
five choices occupy equal regions of the TIME knob. The tooltip MUST show the
active ratio while CLOCK is connected.

### 4.4 BRANCH

**Range:** 0–1  
**Default:** 0.25

BRANCH sets the probability that a successful autonomous action creates a
descendant in addition to, or instead of, re-emitting the parent. It MUST be
continuously suppressed by population pressure. At 0, autonomous branching is
disabled; deliberate manual recombination remains available.

### 4.5 MUTATE

**Range:** 0–1  
**Default:** 0.2

MUTATE controls the magnitude of possible musical change. Mutation may affect:

- pitch;
- future action interval;
- X position and volatility;
- lifetime and energy;
- local activity characteristics.

At 0, descendants remain musically equivalent to their parent except where two
parents are explicitly recombined. Increasing MUTATE progressively permits
larger deviation.

### 4.6 MEMORY

**Range:** 0–1  
**Default:** 0.7

MEMORY controls how faithfully descendants inherit non-pitch characteristics.
High MEMORY creates recognizable genealogies; low MEMORY allows descendants to
develop independent timing, energy, lifetime, and volatility.

MUTATE controls **how much change is possible**. MEMORY controls **how strongly
ancestry constrains the new state**. They MUST NOT be aliases for one
coefficient.

### 4.7 AFFINITY

**Range:** -1 to +1, bipolar  
**Default:** 0

- Negative values favor repulsion, divergence, and conservative destructive
  interaction.
- Zero keeps Events mostly independent.
- Positive values favor attraction, convergence, and recombination.

AFFINITY affects nearby Events only. It MUST NOT produce global all-to-all
coupling.

### 4.8 DENSITY

**Semantic range:** 4–128 active Events  
**Default:** 32 Events

DENSITY is the target carrying capacity, not merely a hard limit.

As population approaches the target:

- autonomous branch probability falls smoothly;
- weak and old Events lose energy faster.

Above the target, attrition increases. The fixed absolute capacity is 128
Events. Player-created Events take priority over autonomous births.

---

## 5. Pool topology

Pool coordinates are normalized to `[0, 1]` internally.

### 5.1 Y axis: pitch

Top is higher pitch; bottom is lower pitch. Pitch is stored internally as raw
1 V/oct voltage.

Pitch range is selected in the context menu:

```text
Pitch Range
  2 octaves
  4 octaves (default)
  8 octaves
  10 octaves
```

Each range is centered on 0 V. The default therefore maps bottom to -2 V and
top to +2 V. Values outside the visible range may exist briefly during mutation
but MUST be folded or clamped back into the selected range before output. Use
one consistent policy; folding is preferred because it preserves motion better
than hard clipping.

### 5.2 X axis: volatility

Left means stable, conservative, and low evolutionary pressure. Right means
volatile, fertile, and strongly affected by mutation and branching.

X does not directly change pitch. It locally scales autonomous probabilities
and mutation magnitude. At identical global settings, an Event on the left
SHOULD preserve itself longer than the same Event on the right.

X is canonical and MUST NOT be freely assignable in v1.

### 5.3 Pitch modes

Context-menu setting:

```text
Pitch Mutation
  Chromatic (default)
  Continuous
```

Chromatic mutation uses a weighted relative interval vocabulary:

```text
±1, ±2, ±3, ±5, ±7, ±12 semitones
```

Smaller intervals SHOULD be more likely; increasing MUTATE progressively admits
larger intervals. Continuous mode operates directly in volts for microtonal use
or external quantization.

Mouse and virtual-touch placement use a separate setting:

```text
Gesture Pitch Snap
  Semitone (default)
  Continuous
```

Snapping affects the resulting pitch, not visual pointer movement. An internal
scale system is out of scope for v1.

---

## 6. Event state

All audio-thread and ecology data MUST use fixed-capacity storage.

```cpp
static constexpr int MAX_EVENTS = 128;
static constexpr int MAX_TOUCHES = 16;
static constexpr int MAX_OUTPUT_VOICES = 16;
```

Representative state:

```cpp
struct ConsequenceEvent {
    uint32_t id;
    uint32_t parentA;
    uint32_t parentB;
    uint32_t rngState;

    uint64_t birthTick;
    uint64_t nextActionTick;

    uint16_t generation;
    uint16_t controllerRef;

    float x;
    float pitch;
    float energy;
    float age;
    float lifetime;
    float actionInterval;
    float drift;
    float volatility;

    bool active;
    bool manuallyHeld;
};
```

Exact layout may differ, but equivalent information must exist. `0` is reserved
as the null ancestry ID. Event IDs MUST be monotonically assigned within a run;
wraparound may skip `0`.

No allocation, container growth, or reclamation requiring a destructor is
permitted on the audio thread.

---

## 7. Event lifecycle

### 7.1 Birth

An Event may originate from:

- mouse gesture;
- virtual touch;
- PITCH/GATE input;
- autonomous branching;
- automatic or manual recombination.

Each birth receives an ID, ancestry, generation, pitch, X coordinate, energy,
lifetime, deterministic PRNG state, and next-action tick.

Generation rules:

```text
player/CV Seed Event = 0
branch child         = parent generation + 1
recombined child     = max(parent generations) + 1
```

Generation arithmetic MUST saturate at the internal integer maximum.

Player/CV births emit immediately. Autonomous births emit immediately unless no
voice can be allocated under Section 12.

### 7.2 Held state

While held by a controller:

- the Event follows that controller's pitch/X updates;
- its held output gate remains high if it owns a voice;
- autonomous scheduling, aging toward death, and energy loss pause;
- interaction may be visualized, but automatic destructive outcomes MUST NOT
  kill it.

On release, the held gate closes and the Event enters autonomous ecology. At
CONSEQUENCE 0 it retires immediately after gate closure.

### 7.3 Autonomous action

When `nextActionTick` is reached:

1. evaluate whether the Event emits;
2. evaluate bounded mutation of its state;
3. evaluate branching after population suppression;
4. evaluate eligible local interaction;
5. schedule the next action strictly in the future.

An action opportunity does not guarantee every outcome. The parent may
re-emit, change silently, create a child, interact, or do a bounded combination
of these. One Event MUST create at most one autonomous child per ecology tick.

If several Events act on the same tick, process them by ascending Event ID.
Newborn Events MUST NOT receive an autonomous action until the next ecology
tick. These rules prevent container order from changing the result.

### 7.4 Death

An Event dies when:

- its lifetime expires;
- its energy reaches the retirement threshold;
- deterministic population pressure evicts it;
- a destructive interaction removes it;
- CLEAR or RESET removes it.

Natural, pressure, and interaction deaths fire DEATH. CLEAR, RESET, module
destruction, and patch load MUST NOT emit a burst of DEATH triggers.

Death releases any assigned voice and invalidates controller capture. Ancestry
IDs remain historical references and MUST NOT be interpreted as live pointers.

---

## 8. Mouse performance

The mouse is a first-class controller.

### 8.1 Press empty pool

Create and immediately emit a generation-0 Event at the pointer position. Y
sets pitch; X sets initial volatility. The Event remains captured until release.

### 8.2 Press an existing Event

Capture and excite it:

- replenish a bounded amount of energy;
- retrigger or acquire its held voice;
- pause its decay while held.

The hit radius SHOULD grow modestly for small or low-energy Events so selection
does not require pixel-perfect aiming. Resolve overlapping hits by smallest
pointer distance, then highest Event ID.

### 8.3 Drag a captured Event

Vertical movement changes pitch; horizontal movement changes X/volatility.
Pitch output MUST track the gesture at audio rate or through sufficiently smooth
interpolation to feel immediate. Ecology updates may remain decimated.

### 8.4 Influence-field drag

`Shift`-dragging through the pool without first capturing an Event creates a
transient influence field. Nearby Events receive a bounded impulse based on
pointer velocity, distance, and direction. The field may alter X, pitch drift,
or interaction tendency but MUST NOT require fluid simulation.

An unmodified press in empty space always creates and captures an Event, as
specified in Section 8.1. The modifier avoids delaying the note onset while the
UI waits to distinguish a click from a drag. The pool tooltip and module manual
MUST advertise `Shift`-drag; influence-field control is never required for core
operation.

### 8.5 Manual recombination

Dragging a captured Event within the collision radius of another Event and
holding it there for a short dwell (recommended 100 ms) creates one recombined
child. The pair then enters a cooldown until separated beyond the collision
radius, preventing repeated births from a single hold.

Manual recombination does not require positive AFFINITY and MUST provide strong
visual feedback. Both parents normally survive.

---

## 9. Conventional CV input

Inputs:

```text
PITCH   standard 1 V/oct, polyphonic
GATE    polyphonic gate
ENERGY  optional 0–10 V, polyphonic
```

A GATE rising edge creates a generation-0 Event for that channel. While high,
the Event follows PITCH and owns a held gate where voice capacity permits. A
falling edge releases it into the ecology.

Channel handling follows these rules:

- process at most 16 GATE channels;
- absent PITCH reads as 0 V;
- monophonic PITCH or ENERGY broadcasts to all GATE channels;
- a polyphonic source uses the matching channel, clamping to its last available
  channel when it has fewer channels than GATE;
- absent ENERGY maps to 1.0 internal energy;
- ENERGY is clamped from 0–10 V into 0–1;
- reducing GATE channel count releases removed channels as if their gates fell.

Pitch and energy updates MUST NOT create additional Events while the gate
remains high.

---

## 10. Virtual touch CV

Inputs:

```text
X         0–10 V → left to right
Y         0–10 V → bottom to top
TOUCH     gate
PRESSURE  0–10 V → interaction strength
```

All support up to 16 polyphonic touch channels. TOUCH determines active channel
count. X, Y, and PRESSURE use the same mono-broadcast and last-channel clamping
rules as Section 9. If PRESSURE is absent, use full performance strength.

On TOUCH rising:

- capture and excite the nearest Event within the hit radius; otherwise
- create a generation-0 Event at X/Y.

While TOUCH is high, X/Y drags the captured Event. On falling edge or channel
removal, release it. Gesture Pitch Snap applies to Y-derived pitch.

Mouse, touch, and PITCH/GATE controllers may coexist. An Event may be owned by
only one controller at a time. A controller that targets an already-held Event
MUST create a new Event instead of stealing ownership.

---

## 11. Clock, reset, and panel buttons

### 11.1 CLOCK

CLOCK synchronizes autonomous actions only. It MUST NOT quantize mouse response,
input gate edges, held pitch motion, or immediate Seed Event emission.

After two valid rising edges, measure the input period and apply the TIME ratio
from Section 4.3. Before the second edge, use the current free-running TIME
interval. Reject implausibly short periods below 1 ms. If the clock stops, retain
the last valid period while the jack remains connected.

Clock subdivision opportunities are scheduled from the measured period, not by
running ecology logic on the cable edge alone. RESET re-establishes clock phase.

### 11.2 RESET input

On a rising edge:

- clear all Events without DEATH triggers;
- close all voice gates;
- reset Event IDs, ecology tick, clock phase, and deterministic PRNG state to
  the current seed;
- clear pending UI commands and trigger pulses.

### 11.3 CLEAR button

CLEAR performs the same runtime clearing behavior as RESET but does not alter
the current seed. The next Event ID and engine PRNG sequence SHOULD restart from
that seed so CLEAR provides a reproducible empty state.

### 11.4 REROLL button

REROLL creates a new seed, serializes it as current state, and then performs a
CLEAR. Seed generation occurs in response to the button/UI action and MUST NOT
call ambient or system randomness from the audio processing loop.

---

## 12. Output and voice allocation

Polyphonic outputs:

```text
PITCH   emitted Event pitch, 1 V/oct
GATE    10 V while active
ENERGY  Event energy mapped to 0–10 V
GEN     generation 0 → 0 V, generation 15 → 10 V, clamp above 15
```

The four outputs share 16 stable voice slots. While any of these outputs is
connected, expose 16 channels to avoid downstream channel reshuffling. Inactive
slots output 0 V for GATE, ENERGY, and GEN. PITCH may retain its last value, but
must be finite.

Monophonic outputs:

```text
BIRTH     pulse on every accepted Event birth
DEATH     pulse on natural/pressure/interaction death
INTERACT  pulse on recombination or destructive interaction
POP       active population / target DENSITY, mapped to 0–10 V and clamped
```

Trigger pulses SHOULD be 1 ms. Multiple occurrences during one pulse extend or
retrigger that pulse; the monophonic outputs do not encode event counts.

### 12.1 Voice rules

A voice has one of three states: free, autonomous, or held.

Allocation order for a new emission:

1. use the lowest-index free voice;
2. for a held request, steal the oldest autonomous voice;
3. for an autonomous request, steal the oldest autonomous voice only if the new
   Event has higher deterministic priority;
4. if a held request still has no voice, steal the oldest held voice;
5. otherwise drop the autonomous Emission, not the Event.

Ties resolve by voice index. New manual performance therefore remains audible,
while autonomous saturation cannot cut off a held gesture. If one held gesture
steals another's voice, the older Event remains held and visible but silent
until released; it MUST NOT repeatedly fight to reacquire a voice.

Recommended autonomous priority, from highest to lowest:

```text
new birth → higher energy → lower generation → newer emission request → Event ID
```

Implement this as a fixed tuple or score with explicit tie-breaking so results
remain deterministic.

### 12.2 Gate duration

Held controller gates remain active until release or voice steal. Autonomous
gates last approximately 25% of that Event's current action interval, clamped to
10–500 ms. Retriggering an Event already assigned to an autonomous voice MUST
produce a new rising edge; insert at least a 1 ms low interval when necessary.

---

## 13. Mutation, branching, and inheritance

All stochastic decisions derive from Event-local or engine-local deterministic
PRNG state. Each decision site SHOULD consume a documented, fixed number of
draws so a renderer or harmless refactor cannot perturb musical results.

### 13.1 Mutation

Mutation magnitude is shaped by:

```text
global MUTATE × local X/volatility × autonomous CONSEQUENCE
```

Clamp all resulting state to valid bounds. Mutation MUST NOT create NaN or
infinite state. Chromatic pitch mutation selects from the weighted intervals in
Section 5.3; continuous mutation operates in volts.

### 13.2 Branching

Branch probability is shaped by BRANCH, CONSEQUENCE, local volatility, Event
energy, and population pressure. A child inherits parent state before bounded
mutation. Birth energy MUST be drawn from a bounded budget so repeated branching
does not create energy from nothing; splitting some parent energy is preferred.

### 13.3 Memory

For an inheritable value `v`, a useful conceptual model is:

```text
child = lerp(independentSample, parentValue, MEMORY) + boundedMutation
```

This is guidance, not a mandated formula. Pitch retains its own mutation rules.
The implementation MUST make high and low MEMORY perceptibly different even
when MUTATE is unchanged.

---

## 14. Interaction and recombination

Only spatial neighbors interact. Positive AFFINITY encourages attraction and
shared descendants. Negative AFFINITY encourages repulsion and occasional
destructive collision.

A recombined child inherits approximately:

```text
pitch       energy-weighted midpoint or one parent's pitch, then mutation
x           energy-weighted parent average
energy      bounded contribution from both parents
lifetime    parent-derived according to MEMORY
generation  max(parent generations) + 1
ancestry    both parent IDs
```

Both parents normally survive. Recombination MUST debit bounded energy from the
parents or otherwise account for population growth. High population pressure
may deterministically retire one weak parent.

Negative-AFFINITY annihilation should be uncommon near the default setting. A
destructive collision retires the weaker Event; ties resolve by older birth
tick, then lower Event ID.

Each unordered Event pair needs a short interaction cooldown or separation
latch. A persistent overlap MUST NOT generate a child or INTERACT pulse every
ecology tick.

---

## 15. Population regulation

Let:

```text
pressure = activePopulation / targetDensity
```

Use a smooth, monotonic suppression curve for autonomous branching as pressure
approaches 1. Above 1, accelerate energy loss according to weakness and age.

At the 128-Event absolute limit:

- reject autonomous births unless the candidate outranks a deterministic
  eviction victim;
- always admit a player/CV birth by evicting the lowest-priority unheld Event;
- if every Event is held, admit no additional Event and keep the controller
  responsive without corrupting an existing hold.

Victim priority, lowest first, SHOULD consider held state, energy, age, and
generation, with Event ID as the final tie-breaker. Capacity decisions MUST NOT
depend on memory address or array traversal accidents.

An accepted birth fires BIRTH even if its immediate Emission is dropped. A
rejected birth fires neither BIRTH nor DEATH.

---

## 16. Timing architecture

Use three independent timing domains.

### 16.1 Audio rate

Rack `process()` handles:

- port sampling and channel-count changes;
- gate, clock, and reset edge detection;
- held pitch tracking;
- output voice gates and voltages;
- trigger pulses;
- an accumulator that schedules ecology ticks.

Do not run pair searches or rendering work at audio rate.

### 16.2 Ecology rate

Run ecology at a fixed nominal 1 kHz. Use a sample-time accumulator so behavior
remains correct at all Rack sample rates. A bounded loop may process accumulated
ticks, but cap catch-up work per audio sample and preserve the unprocessed
remainder rather than allowing an unbounded stall.

Ecology handles:

- age, energy, and lifetime;
- autonomous scheduling;
- mutation and branching;
- influence fields and spatial interaction;
- population regulation;
- render-snapshot publication at a lower divisor.

Use integer ecology ticks for ordering and scheduled action deadlines where
practical. Musical determinism is required for identical seed, parameter
automation, sample rate, input samples, and clock edges. Cross-sample-rate
bit-identical output is not a v1 requirement.

### 16.3 Rendering rate

Rendering runs independently at Rack's UI rate, typically 30–120 FPS. Missing a
visual frame MUST have no musical consequence.

---

## 17. Spatial acceleration

Do not compare all Events with every other Event at every ecology tick. Use a
fixed spatial partition, initially an 8 × 8 grid.

Each active Event belongs to one cell. Check interactions against the same and
adjacent cells only. Process each unordered pair once, in deterministic Event-ID
order. Grid storage MUST be fixed-capacity and allocation-free.

If profiling later shows a 1 kHz pair pass is unnecessary, interaction scans may
run at a lower fixed divisor without changing gesture or gate response.

---

## 18. UI/audio thread boundary

The DSP engine owns authoritative Event and voice state. The widget MUST NOT
mutate it directly, retain raw Event pointers, or acquire an audio-thread mutex.

Publish UI gestures through a fixed-capacity single-producer/single-consumer
command queue. Commands should include normalized position, gesture/controller
ID, phase (`begin`, `move`, `end`), and a monotonically increasing sequence
number. Coalescing consecutive `move` commands is allowed; `begin` and `end`
commands MUST NOT be lost.

If the queue is full:

- replace/coalesce an older move for the same controller when possible;
- preserve release commands;
- otherwise fail safely by synthesizing release after a bounded timeout.

Atomic latest-pointer state may supplement the queue for smooth tracking, but is
not sufficient by itself because a press and release can occur between audio
polls.

---

## 19. Visual snapshot boundary

The renderer MUST NOT traverse mutable DSP structures. Publish compact immutable
snapshots containing only visual state.

```cpp
struct VisualEvent {
    uint32_t id;
    uint32_t parentA;
    uint32_t parentB;
    float x;
    float y;
    float energy;
    float life;
    uint16_t generation;
    uint8_t flags;
};
```

Use triple buffering, a sequence-lock copy, or another scheme that proves the UI
cannot read a buffer while DSP overwrites it. A naive two-buffer pointer swap is
insufficient if the UI may retain the old read buffer across two publications.

Snapshots SHOULD include a monotonically increasing revision and enough recent
birth/death/interaction markers for the UI to animate an event once without
feeding anything back into DSP. UI interpolation between stable snapshots is
encouraged.

---

## 20. Visual grammar

Essential mappings:

```text
Y position      → pitch
X position      → volatility
disc size       → energy
opacity         → remaining life
outline/ring    → selected or held
trail           → recent movement
thin line       → surviving ancestry
outward ripple  → birth or excitation
joining ripple  → recombination
collapse        → death
```

Essential state MUST NOT be encoded by color alone. Held/selected state, energy,
and life require shape, size, opacity, or line-style cues.

Provide subtle octave guides. Semitone guides MAY appear at higher detail. The
pool must not read as a dense piano roll. Hovered or dragged Events may show a
compact pitch readout such as `C4 / 0.000 V`; persistent labels are discouraged.

---

## 21. Visual detail and fallback

Persist a context-menu `Visual Detail` setting.

### Minimal

- dark pool background;
- Event discs;
- selection/hold rings;
- basic ancestry lines;
- touch indicators;
- essential pitch guides.

No shader effects. Minimal MUST remain fully playable.

### Standard (default)

Adds short trails, simple birth/death ripples, restrained interaction animation,
and modest depth.

### Rich

May add subtle surface motion, wakes, longer trails, and interaction distortion.

### Maximum

May add higher-cost particles, liquid response, and longer visual history.

Visual tiers MUST NOT change hit testing, Event behavior, timing, randomness, or
instrument semantics.

The module MUST have a NanoVG/simple-rendering fallback. If optional OpenGL or
shader work fails, is unsupported, or is too expensive, the instrument remains
usable without reloading the patch.

---

## 22. Performance requirements

### Audio thread

- zero heap allocation after construction/reset preparation;
- no mutex acquisition;
- no filesystem access;
- no rendering calls;
- no unbounded loops;
- no ambient random-number calls.

### Ecology

- fixed arrays and queues;
- lightweight deterministic PRNG;
- spatial bucketing;
- decimated expensive work;
- finite values and bounded state transitions.

Avoid expensive transcendental math where a perceptually equivalent bounded
approximation is available.

### Renderer

Minimal mode MUST remain responsive on weak integrated GPUs. Standard is the
normal default. Optional effects disappear before pointer latency or Rack UI
responsiveness degrades.

Performance verification SHOULD include 128 active Events, 16 virtual touches,
16 held CV gates, connected polyphonic outputs, and Rich rendering. Audio-thread
profiling must be performed in a release build.

---

## 23. Determinism contract

Given identical:

```text
seed
parameter and setting state
sample rate
sample-aligned input/CV sequence
clock and reset sequence
```

the Event evolution and emitted musical sequence MUST reproduce.

Requirements:

- use local deterministic PRNGs;
- seed each Event deterministically from engine seed, Event ID, and ancestry;
- define ordering for simultaneous actions, pairs, eviction, and voice stealing;
- do not consume musical PRNG state for visuals;
- RESET and CLEAR return the current seed to the documented empty initial state;
- REROLL changes that seed.

Floating-point bit identity across architectures is not required, but the design
SHOULD avoid chaotic dependence on unordered iteration and tiny rounding noise.

---

## 24. Serialization

Persist a schema version and:

- current seed;
- Visual Detail;
- pitch range;
- Gesture Pitch Snap;
- Pitch Mutation mode;
- future non-parameter settings.

Rack parameters persist normally. JSON loading MUST validate enums and numeric
ranges and fall back to documented defaults for absent or invalid values.

V1 MUST NOT serialize the live Event population, voice allocator, controller
captures, pending triggers, or clock phase. A loaded patch starts with an empty
pool using the saved seed. Identical subsequent input may then reconstruct the
same behavior.

---

## 25. Suggested file organization

Fit the module into current Leviathan conventions without premature
fragmentation:

```text
src/ConsequenceEngine.hpp
src/ConsequenceEngine.cpp
src/ConsequenceEngineWidget.cpp
src/ConsequenceEngineRenderer.hpp
src/ConsequenceEngineRenderer.cpp
```

If ecology logic becomes large enough to test independently:

```text
src/ConsequenceEngineDSP.hpp
src/ConsequenceEvent.hpp
src/ConsequenceVisualState.hpp
```

Keep deterministic ecology logic separable from Rack UI and drawing code. Add
the model declaration/registration, `plugin.json` entry, panel SVG, and build
source list according to repository conventions.

---

## 26. Implementation phases

### Phase 1 — Skeleton

Implement:

- model registration and 20 HP panel;
- parameters and ports;
- fixed Event pool and voice allocator;
- polyphonic PITCH/GATE input;
- PITCH/GATE output;
- deterministic seed;
- CLEAR, RESET, and REROLL;
- simple circle renderer.

Milestone: **A note can enter, become an Event, re-emit, and die.**

### Phase 2 — Playable pool

Implement:

- mouse birth, capture, excite, hold, and drag;
- drag-field threshold and influence;
- virtual X/Y/TOUCH/PRESSURE control;
- command queue and stable render snapshots;
- low-latency held pitch/gate behavior.

Milestone: **The pool is enjoyable as a manual instrument before advanced
ecology exists.**

Do not postpone this milestone in favor of recursion or visual effects.

### Phase 3 — Consequence ecology

Implement:

- LIFE, TIME, BRANCH, MUTATE, MEMORY, and CONSEQUENCE coupling;
- ancestry and generation;
- autonomous descendants;
- population pressure;
- ENERGY, GEN, BIRTH, DEATH, and POP outputs.

Milestone: **A performed phrase develops recognizable but increasingly
independent descendants.**

### Phase 4 — Compare/recombine

Implement:

- fixed spatial grid;
- AFFINITY attraction and repulsion;
- automatic recombination;
- manual drag-to-recombine and pair cooldown;
- conservative destructive interaction;
- INTERACT output.

Milestone: **Events visibly and musically affect one another.**

### Phase 5 — Rendering tiers

Implement Minimal and Standard first, then Rich where practical. Add visual
setting persistence, snapshot interpolation, and fallback verification. Maximum
is optional polish and MUST NOT delay DSP completion.

### Phase 6 — Clocking and hardening

Implement and refine:

- external clock measurement and TIME ratios;
- reset phase behavior;
- context-menu settings and tooltips;
- serialization validation;
- deterministic edge cases;
- load, sample-rate-change, and module-removal behavior;
- presets and default state.

---

## 27. Verification and acceptance criteria

### 27.1 Basic performance

- Clicking the pool immediately emits an audible pitch.
- Vertical drag changes pitch with playable latency.
- Horizontal drag changes subsequent behavior without directly changing pitch.
- Hold sustains the gate; release closes it and permits autonomous continuation.
- CONSEQUENCE 0 behaves as a direct instrument with no autonomous persistence.

### 27.2 CV performance

- Polyphonic PITCH/GATE creates, tracks, and releases up to 16 Seed Events.
- ENERGY mono-broadcast and polyphonic channel matching follow Section 9.
- Up to 16 X/Y/TOUCH channels operate the pool.
- Channel-count reduction releases removed controllers.
- Mouse, conventional CV, and virtual touch coexist without stealing Event
  ownership.

### 27.3 Consequences and interaction

- Events can re-emit, mutate, branch, recombine, and die.
- Generation and ancestry are correct and saturate safely.
- Positive AFFINITY can attract and recombine neighbors.
- Negative AFFINITY can repel and conservatively destroy a weaker Event.
- Manual recombination creates one child per collision dwell, not a stream.
- Population never exceeds 128 and player births outrank autonomous births.

### 27.4 Output behavior

- PITCH/GATE/ENERGY/GEN use the same stable voice slot.
- Held gestures outrank autonomous emissions.
- Voice stealing and tie-breaking are deterministic.
- Retriggers create a valid low-to-high gate transition.
- Trigger outputs do not burst on CLEAR, RESET, load, or module destruction.
- POP is finite, clamped, and reflects population relative to DENSITY.

### 27.5 Architecture and performance

- Audio processing performs no allocation, locking, file access, or rendering.
- UI cannot lose a normal begin/end gesture pair or mutate authoritative state.
- Renderer cannot observe a partially written snapshot.
- Visual frame drops and detail changes do not affect musical output.
- A stress patch at absolute population and polyphony remains responsive.

### 27.6 Determinism

- RESET followed by an identical input sequence reproduces Event evolution.
- CLEAR returns to the same current-seed empty state.
- REROLL creates a different seed and ecology.
- Rendering detail and hidden-widget state do not change the musical sequence.

---

## 28. Explicit v1 non-goals

- full fluid dynamics or audio-rate physical simulation;
- elaborate particle simulation as a functional dependency;
- arbitrary scales or tuning editors;
- neural or AI models;
- sample generation;
- more than 128 living Events or 16 output voices;
- serialization of a live ecology;
- networked Consequence Engines;
- user-programmable transformation graphs;
- assignable meanings for the X axis;
- exhaustive Maximum-tier visual polish.

These are possible future directions, not requirements for validating the
instrument.

---

## 29. Guiding implementation rule

When an engineering choice remains ambiguous, prefer the choice that makes this
loop clearer and faster:

```text
PLAYER ACTS
     ↓
POOL VISIBLY RESPONDS
     ↓
MUSIC RESPONDS
     ↓
EVENT DEVELOPS
     ↓
PLAYER UNDERSTANDS ENOUGH
TO INTERVENE AGAIN
```

Phase 2 is the proof point. The 20 HP pool must feel good to click, hold, drag,
and control with CV before recursion or liquid rendering is allowed to carry the
concept. If that substrate is satisfying, later ecology and visual work amplify
the instrument instead of compensating for it.
