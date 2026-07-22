# Leviathan Rack Module Specification: **Mawfall**

## 1. Concept Summary

**Mawfall** is a pachinko-inspired kinetic gate generator for the Leviathan VCV Rack plugin suite.

The module simulates small balls falling through a semi-realistic 2D physics board containing pegs, deflectors, guides, walls, and “sink” locations. When a ball enters a sink, pocket, lane, vortex, or other capture region, the module emits a gate or trigger pulse from the associated output.

At its core, Mawfall is a **probability sequencer with visible physics**. Instead of selecting outputs with a conventional random distribution, the distribution emerges from board geometry, gravity, bounce, tilt, ball velocity, and CV modulation.

The user experiences it as a living kinetic object: balls are dropped into a board, bounce through a Leviathan-themed field of teeth, reefs, conduits, and gravity wells, then disappear into output holes that produce musical events.

---

## 2. Primary Design Goals

### 2.1 Musical Goals

Mawfall should be useful as:

* A generative rhythm source.
* A probability-based gate router.
* A semi-chaotic trigger sequencer.
* A visual performance module.
* A source of related but non-repeating musical structures.
* A CV-controllable “randomness with memory” system.

The module should feel less like a dice roll and more like a physical ritual: each ball has a path, collisions matter, and small parameter changes alter the probability field.

### 2.2 User Experience Goals

The user should immediately understand:

1. Press or clock **DROP** to release balls.
2. Balls fall through the board.
3. Each sink corresponds to a gate output.
4. Tilt, chaos, gravity, and board layout reshape the results.
5. More balls create denser rhythmic activity.
6. Different layouts create different probability personalities.

The UI should make the relationship between visual motion and output pulses obvious.

### 2.3 Engineering Goals

The first implementation should avoid heavy external dependencies.

Preferred approach:

* Implement a lightweight custom 2D physics system.
* Use deterministic fixed-step simulation.
* Store board layouts as static data structures.
* Use NanoVG for rendering.
* Keep CPU usage bounded with a maximum ball count.
* Ensure patch persistence is stable.
* Keep the system deterministic when using the same seed and input stream.

---

## 3. Working Module Name

Primary name:

# **Mawfall**

Subtitle:

**Kinetic Gate Cascade**

Alternate names, if desired:

* **Pachyderm Reef** — probably too playful.
* **Gravemaw** — darker, more Leviathan.
* **Abyssal Cascade** — elegant and descriptive.
* **Teethfall** — simple, aggressive.
* **Probability Maw** — literal.
* **Koralith** — abstract, reef-like.
* **Dropforge** — strong, machine-like.

Recommended final name: **Mawfall**.

It sounds physical, monstrous, and musical. It also avoids being too literally “pachinko,” which helps the module feel like Leviathan canon rather than a novelty.

---

## 4. Module Size and Panel Layout

Recommended width: **18 HP**

Possible width range:

* Minimum viable: 16 HP
* Comfortable visual board: 18 HP
* Luxury version: 20 HP

Recommended: **18 HP**, because the board visualization is the soul of the module.

### 4.1 High-Level Layout

Panel sections from top to bottom:

1. **Header**

   * Module name: MAWFALL
   * Small subtitle: Kinetic Gate Cascade

2. **Drop / Clock / Ball Controls**

   * Manual DROP button
   * DROP input
   * RATE / AUTO control
   * DENSITY control
   * BALLS indicator

3. **Physics Controls**

   * GRAVITY
   * TILT
   * BOUNCE
   * CHAOS
   * DRAG

4. **Board Display**

   * Large central NanoVG physics board
   * Balls visibly falling
   * Pegs / teeth / conduits
   * Highlighted sink regions
   * Output flashes when triggered

5. **Layout / Probability Controls**

   * LAYOUT selector
   * SEED button / parameter
   * MIRROR / FLIP option
   * RESET / CLEAR balls

6. **Outputs**

   * 8 individual sink gate outputs
   * 1 ANY output
   * 1 LEFT output
   * 1 RIGHT output
   * 1 VELOCITY CV output
   * 1 COUNT / ACTIVITY CV output

---

## 5. Controls

### 5.1 Manual Drop Button

**DROP**

Momentary button.

Behavior:

* Pressing DROP spawns one or more balls at the top of the board.
* The number of balls spawned is controlled by DENSITY.
* If the max active ball count is reached, additional drops are ignored or replace the oldest ball depending on context menu setting.

Visual:

* Button glows briefly when a ball is spawned.
* Spawn area flashes subtly.

---

### 5.2 Drop Input

**DROP IN**

CV / gate input.

Behavior:

* Rising edge spawns balls.
* Edge threshold: approximately 1.0V.
* Schmitt trigger should be used.
* Repeated pulses generate repeated drops.

Recommended implementation:

```cpp
dsp::SchmittTrigger dropTrigger;
```

---

### 5.3 Auto Rate

**RATE**

Controls internal ball dropping clock.

Range:

* Fully CCW: off
* Low: very sparse drops
* High: rapid drops

Suggested range:

* Off below 0.02 normalized
* 0.05 Hz to 20 Hz above threshold

Behavior:

* If RATE is above off threshold, module self-generates drops.
* External DROP input can still add additional drops.
* RATE should support CV modulation through an associated input if panel space allows.

Optional input:

**RATE CV**

* Bipolar or unipolar CV modulation.
* 1V/oct behavior is not needed.
* Use attenuverter if space allows.

---

### 5.4 Density

**DENSITY**

Controls how many balls are spawned per drop event.

Range:

* 1 to 8 balls per drop

Behavior:

* Integer quantized internally.
* At low values, one ball per clock.
* At high values, small bursts of balls appear.
* Spawn positions should be slightly distributed to avoid perfect overlap.

CV behavior:

* Optional **DENSITY CV** input.
* 0V = no modulation.
* 10V = maximum positive modulation.

---

### 5.5 Gravity

**GRAVITY**

Controls downward acceleration.

Range:

* Very low gravity: slow, floaty, almost lunar.
* Normal gravity: pachinko-like.
* High gravity: aggressive, fast, percussive.

Suggested internal range:

```cpp
gravity = rescale(param, 0.f, 1.f, 200.f, 2200.f);
```

Units are arbitrary pixels-per-second squared in board space.

Musical effect:

* Lower gravity increases delay between drop and output.
* Higher gravity creates tighter rhythmic response.

---

### 5.6 Tilt

**TILT**

Applies horizontal gravity bias.

Range:

* -1.0 to +1.0

Behavior:

* Negative tilt pulls balls left.
* Positive tilt pulls balls right.
* Center is vertical gravity.

Suggested force:

```cpp
accel.x += tilt * gravity * 0.45f;
```

CV behavior:

* Strong candidate for CV input.
* Slow LFO into TILT should make the module breathe and redistribute probability over time.

---

### 5.7 Bounce

**BOUNCE**

Controls collision restitution.

Range:

* Low: balls lose energy quickly, paths become shorter and heavier.
* High: balls ricochet more dramatically.

Suggested range:

```cpp
restitution = rescale(param, 0.f, 1.f, 0.15f, 0.92f);
```

Musical effect:

* Low bounce: more stable, predictable sink distribution.
* High bounce: more chaotic and longer-lived trajectories.

---

### 5.8 Drag

**DRAG**

Controls velocity damping.

Range:

* Low drag: fast balls, long travel.
* High drag: thick liquid / viscous falling.

Suggested model:

```cpp
velocity *= expf(-drag * dt);
```

Suggested drag range:

```cpp
drag = rescale(param, 0.f, 1.f, 0.0f, 4.0f);
```

---

### 5.9 Chaos

**CHAOS**

Adds subtle randomness to ball motion and collisions.

Range:

* 0: deterministic physics.
* High: micro turbulence, unstable paths, more random results.

Important:

CHAOS should not be pure teleporting randomness. It should feel like air currents, surface imperfections, magnetic shimmer, or microscopic uncertainty.

Suggested behavior:

* Add tiny random acceleration per physics tick.
* Add slight random perturbation to collision normals.
* Add slight random variation to spawned ball velocity.

Suggested internal scale:

```cpp
chaosAccel = randomBipolar() * chaosAmount * 80.f;
```

Determinism note:

Randomness should be generated from the module’s local seeded RNG, not global random state.

---

### 5.10 Layout Selector

**LAYOUT**

Selects the board geometry.

Initial implementation must include at least one complete layout.

Recommended initial layouts:

1. **Maw**

   * Default layout.
   * Balanced pachinko board.
   * Eight bottom sinks.
   * Teeth-like peg pattern.
   * Slightly wider central path.

2. **Reef**

   * Denser pegs.
   * More lateral scattering.
   * Longer ball lifetime.
   * Good for generative textures.

3. **Vortex**

   * Curved guide rails.
   * Central capture pockets.
   * More mid-board triggers.

4. **Trench**

   * Steep vertical lanes.
   * More predictable.
   * Useful for clock division and controlled probability.

Initial shipping requirement:

* Implement **Maw**.
* Architect layout data so more layouts can be added without rewriting physics.

---

### 5.11 Seed

**SEED**

Controls deterministic random variation.

UI options:

* Small button: reseed
* Context menu: copy seed / paste seed / randomize seed
* Optional small numeric display is nice but not required

Behavior:

* Seed affects:

  * Spawn jitter
  * Chaos field
  * Optional peg micro-variation
  * Ball coloration, if applicable

Patch persistence:

* Current seed must be saved to JSON.
* Reopening the patch should preserve the behavior.

---

### 5.12 Clear / Reset

**CLEAR**

Button.

Behavior:

* Removes all active balls.
* Does not reset the board layout or seed.
* Should not emit gates.

Optional long-press or context menu:

**RESET BOARD**

* Clears balls.
* Resets internal timing.
* Resets layout transient state.

---

## 6. Inputs

Recommended input set:

| Input      | Description                     |
| ---------- | ------------------------------- |
| DROP       | Rising edge spawns balls        |
| RATE CV    | Modulates internal drop rate    |
| DENSITY CV | Modulates balls per drop        |
| GRAVITY CV | Modulates gravity               |
| TILT CV    | Modulates horizontal bias       |
| BOUNCE CV  | Modulates restitution           |
| CHAOS CV   | Modulates turbulence            |
| CLEAR      | Rising edge clears active balls |

If panel space becomes tight, prioritize:

1. DROP
2. TILT CV
3. GRAVITY CV
4. CHAOS CV
5. CLEAR

RATE CV, DENSITY CV, and BOUNCE CV are useful but secondary.

---

## 7. Outputs

### 7.1 Individual Sink Outputs

Primary outputs:

| Output | Description                            |
| ------ | -------------------------------------- |
| 1      | Gate pulse when sink 1 captures a ball |
| 2      | Gate pulse when sink 2 captures a ball |
| 3      | Gate pulse when sink 3 captures a ball |
| 4      | Gate pulse when sink 4 captures a ball |
| 5      | Gate pulse when sink 5 captures a ball |
| 6      | Gate pulse when sink 6 captures a ball |
| 7      | Gate pulse when sink 7 captures a ball |
| 8      | Gate pulse when sink 8 captures a ball |

Default sink arrangement:

* Sinks 1–8 are left-to-right along the bottom.
* Each sink should be visibly labeled or visually connected to its output jack.
* When a sink fires, both the sink and output jack should flash.

Gate behavior:

* Output pulse level: **10V**
* Default pulse length: **10 ms**
* Context menu options:

  * 1 ms
  * 5 ms
  * 10 ms
  * 20 ms
  * 50 ms
  * Gate while occupied, if using persistent pockets later

Recommended implementation:

```cpp
dsp::PulseGenerator sinkPulses[8];
```

---

### 7.2 ANY Output

**ANY**

Emits a pulse whenever any sink captures a ball.

Use cases:

* Master event stream
* Clocking envelopes
* Driving counters
* Rhythm extraction

---

### 7.3 LEFT and RIGHT Outputs

**LEFT**

Pulses when sinks 1–4 trigger.

**RIGHT**

Pulses when sinks 5–8 trigger.

Use cases:

* Stereo or split rhythmic behavior.
* Probability balance monitoring.
* Drum routing.

---

### 7.4 VELOCITY CV Output

**VEL**

Outputs a CV corresponding to the captured ball’s final speed.

Behavior:

* When a ball enters a sink, output a sampled voltage based on impact velocity.
* Holds until next capture event.
* Suggested range: 0V to 10V.

Mapping:

```cpp
velocityCv = clamp(ball.speed / maxExpectedSpeed, 0.f, 1.f) * 10.f;
```

Musical use:

* Accent strength.
* Envelope decay modulation.
* Filter cutoff modulation.
* Sample velocity.

---

### 7.5 POSITION CV Output

Optional but recommended if space allows.

**POS**

Outputs sink position as stepped CV.

Behavior:

* Sink 1 = 0V
* Sink 8 = 10V
* Intermediate sinks evenly distributed

Mapping:

```cpp
posCv = sinkIndex / 7.f * 10.f;
```

Musical use:

* Pitch sequencing.
* Sample selection.
* Wavefolder symmetry.
* Pan position.

---

### 7.6 ACTIVITY CV Output

Optional.

**ACT**

Outputs smoothed recent activity.

Behavior:

* Increases when balls are active or sinks are firing.
* Decays slowly.
* Range 0V to 10V.

Suggested formula:

```cpp
activityTarget = activeBallCount / maxBalls;
activityCv += (activityTarget - activityCv) * smoothing;
```

Musical use:

* Patch intensity modulation.
* Reverb send swelling.
* Probability feedback.

---

## 8. Board Physics

### 8.1 Coordinate System

Use an internal board coordinate system independent of panel pixels.

Recommended logical board dimensions:

```cpp
BOARD_W = 1000.f;
BOARD_H = 1600.f;
```

Rendering maps this board rectangle into the module display area.

This makes layout data resolution-independent.

---

### 8.2 Ball Representation

```cpp
struct Ball {
    Vec pos;
    Vec vel;
    float radius;
    float age;
    float mass;
    bool active;
    int id;
};
```

Recommended defaults:

```cpp
radius = 18.f;
mass = 1.f;
maxBalls = 64;
```

Context menu option:

* Max balls: 16 / 32 / 64 / 96

Default:

* 64

---

### 8.3 Peg Representation

Pegs should initially be circular colliders.

```cpp
struct Peg {
    Vec pos;
    float radius;
    int visualType;
};
```

Recommended peg radius:

```cpp
peg.radius = 20.f to 28.f;
```

Visual types can later distinguish:

* Metal peg
* Tooth peg
* Coral node
* Obsidian rivet
* Lit conduit node

The physics engine should not care about visual type.

---

### 8.4 Wall / Rail Representation

Use line segment colliders.

```cpp
struct Segment {
    Vec a;
    Vec b;
    float radius;
    int material;
};
```

Segment radius allows rails to behave like capsules rather than infinitely thin lines.

Use for:

* Board boundaries
* Angled deflectors
* Curved guide approximations
* Funnel walls
* Sink dividers

Curved rails can be approximated as multiple short segments.

---

### 8.5 Sink Representation

```cpp
struct Sink {
    Vec pos;
    float radius;
    int outputIndex;
    int type;
};
```

Sink behavior:

* If ball center enters sink radius, ball is captured.
* Capture emits associated gate.
* Ball is removed from active simulation.
* Visual effect is triggered.

Sink types for future expansion:

```cpp
enum SinkType {
    BottomLane,
    MidBoardPocket,
    Vortex,
    ReturnLane,
    BonusGate
};
```

Initial version only requires BottomLane.

---

### 8.6 Collision Model

Physics should be semi-realistic, not perfect.

Use simple impulse-style circle collision against:

* Circular pegs
* Capsule rails
* Board boundaries

#### Ball vs Peg

For each active ball and peg:

1. Compute delta from peg center to ball center.
2. If distance < ball radius + peg radius, resolve overlap.
3. Push ball outward along normal.
4. Reflect velocity along normal using restitution.
5. Apply tangential friction.

Pseudo-code:

```cpp
Vec delta = ball.pos - peg.pos;
float dist = length(delta);
float minDist = ball.radius + peg.radius;

if (dist < minDist && dist > 0.f) {
    Vec n = delta / dist;
    float penetration = minDist - dist;

    ball.pos += n * penetration;

    float vn = dot(ball.vel, n);

    if (vn < 0.f) {
        Vec normalVel = n * vn;
        Vec tangentVel = ball.vel - normalVel;

        ball.vel = tangentVel * friction - normalVel * restitution;
    }
}
```

Suggested parameters:

```cpp
friction = 0.985f;
restitution = controlled by BOUNCE;
```

#### Ball vs Segment

Treat each segment as a capsule:

1. Find closest point on segment.
2. Treat closest point as circle collision.
3. Resolve like peg collision.

Pseudo-code:

```cpp
Vec closest = closestPointOnSegment(ball.pos, seg.a, seg.b);
float combinedRadius = ball.radius + seg.radius;
Vec delta = ball.pos - closest;
```

---

### 8.7 Fixed-Step Simulation

Use fixed physics timestep independent of audio block size.

Recommended timestep:

```cpp
PHYSICS_DT = 1.f / 240.f;
```

Alternative if CPU becomes an issue:

```cpp
PHYSICS_DT = 1.f / 120.f;
```

Implementation:

* Accumulate sample time in `process()`.
* Step physics while accumulator exceeds fixed timestep.
* Cap maximum steps per audio frame to prevent spiral-of-death.

Pseudo-code:

```cpp
physicsAccumulator += args.sampleTime;

int steps = 0;
while (physicsAccumulator >= PHYSICS_DT && steps < MAX_STEPS_PER_FRAME) {
    stepPhysics(PHYSICS_DT);
    physicsAccumulator -= PHYSICS_DT;
    steps++;
}
```

Recommended cap:

```cpp
MAX_STEPS_PER_FRAME = 4;
```

If falling behind:

* Drop excess accumulator.
* The visual may skip slightly, but audio remains stable.

---

### 8.8 Ball-Ball Collision

Initial version:

* Do **not** implement ball-ball collision.

Rationale:

* Pachinko feel mostly comes from peg and rail collisions.
* Ball-ball collision significantly increases complexity and CPU cost.
* Multiple balls can visually overlap slightly without harming musical utility.

Optional future mode:

* Enable simplified ball-ball collision via context menu.
* Default off.

---

### 8.9 Ball Lifetime

Each ball should have a maximum lifetime.

Recommended:

```cpp
maxBallAge = 12.f seconds;
```

If a ball exceeds max age:

* Fade out visually.
* Remove from active list.
* Do not trigger sink output.

Reason:

* Prevents stuck balls.
* Keeps CPU bounded.
* Handles pathological board states.

---

### 8.10 Stuck Detection

A ball should be considered stuck if:

* Its speed remains below threshold for several seconds.
* It is not in a sink.
* It is resting on a rail or peg due to numerical settling.

Suggested behavior:

* Add a tiny nudge.
* If still stuck after repeated nudges, remove it.

Pseudo-code:

```cpp
if (length(ball.vel) < 5.f) {
    ball.sleepTimer += dt;
} else {
    ball.sleepTimer = 0.f;
}

if (ball.sleepTimer > 2.f) {
    ball.vel += randomUnitVector() * 80.f;
}

if (ball.sleepTimer > 5.f) {
    removeBall(ball);
}
```

---

## 9. Default Board Layout: Maw

The first required board layout is **Maw**.

### 9.1 Visual Theme

Maw should look like balls are falling through the inside of a Leviathan artifact:

* Dark metal / obsidian background.
* Pegs shaped like rounded teeth or mineral nodes.
* Bottom sinks resemble feeding mouths, vents, or glowing sockets.
* Subtle conduits connect sinks to output jacks.
* The center has a slightly ritualistic symmetry.
* The board should feel engineered, ancient, and alive.

Avoid:

* Literal casino styling.
* Bright carnival colors.
* Overly toy-like pachinko references.

---

### 9.2 Maw Geometry

Logical board dimensions:

```cpp
1000 x 1600
```

Spawn region:

```cpp
x = 500 ± 120
y = 80
```

Board boundaries:

* Left wall
* Right wall
* Top soft boundary
* Bottom sink area

Peg field:

* Staggered triangular grid.
* Slightly denser near center.
* Enough openings for multiple paths.
* Avoid perfectly uniform sterile layout; tiny deterministic offsets from seed can make it feel organic.

Recommended peg rows:

```text
Row 1: 4 pegs
Row 2: 5 pegs
Row 3: 6 pegs
Row 4: 7 pegs
Row 5: 6 pegs
Row 6: 7 pegs
Row 7: 8 pegs
Row 8: 7 pegs
Row 9: 8 pegs
Row 10: 7 pegs
Row 11: 6 pegs
```

Approximate peg y positions:

```text
220
340
460
580
700
820
940
1060
1180
1300
1400
```

Bottom sink area:

* 8 sinks across bottom.
* Each sink has a funnel divider.
* Sinks should be equally spaced horizontally.

Sink centers:

```cpp
for i in 0..7:
    x = 90 + i * 117
    y = 1510
    radius = 46
```

Add divider rails between sinks:

* Short upward triangular walls.
* Prevent ambiguous sink capture.
* Guide final trajectories.

---

### 9.3 Mid-Board Optional Features

Initial version may include one or two special collision features that do not trigger outputs:

* Central curved tooth rail.
* Two side bumpers.
* A subtle “jaw” funnel near the lower third.

Do not overcomplicate first version.

The first layout should be readable and musically useful before becoming visually elaborate.

---

## 10. Future Layouts

The layout system should support future boards through data definitions.

### 10.1 Reef

Dense chaotic board.

Characteristics:

* Many small pegs.
* Longer ball lifetime.
* Highly scattered outputs.
* Good for generative percussion.

### 10.2 Vortex

Board with curved rails and central pockets.

Characteristics:

* Balls spiral around one or more zones.
* Mid-board pockets can trigger outputs before bottom sinks.
* Useful for delayed, swirling rhythms.

### 10.3 Trench

More deterministic board.

Characteristics:

* Strong vertical lanes.
* Fewer pegs.
* Clear left / center / right routing.
* Good for controlled probability sequencing.

### 10.4 Teeth

Aggressive board.

Characteristics:

* Large triangular or rounded tooth deflectors.
* Fast deflections.
* High rhythmic instability.
* Very Leviathan.

---

## 11. Layout Data Architecture

Define board layouts in C++ as data structures.

```cpp
struct BoardLayout {
    std::string name;
    std::vector<Peg> pegs;
    std::vector<Segment> rails;
    std::vector<Sink> sinks;
    Vec spawnCenter;
    Vec spawnSpread;
};
```

Recommended:

* Keep default layouts compiled into the plugin.
* Do not introduce external JSON layout files for first implementation.
* Later, optional user layout files could be considered.

Function:

```cpp
BoardLayout makeMawLayout(uint32_t seed);
```

The seed can slightly perturb peg positions while preserving musical validity.

Important:

* Seed variation should never create impossible or broken geometry.
* Perturbations should be small.

Suggested max perturbation:

```cpp
±8 logical units
```

---

## 12. Timing and Gate Behavior

### 12.1 Gate Generation

When a ball enters a sink:

1. Determine sink output index.
2. Fire that sink’s pulse generator.
3. Fire ANY pulse generator.
4. Fire LEFT or RIGHT pulse generator if applicable.
5. Update VEL CV.
6. Update POS CV.
7. Trigger visual flash.
8. Remove ball.

Pseudo-code:

```cpp
void captureBall(Ball& ball, Sink& sink) {
    int i = sink.outputIndex;

    sinkPulses[i].trigger(pulseLength);
    anyPulse.trigger(pulseLength);

    if (i < 4)
        leftPulse.trigger(pulseLength);
    else
        rightPulse.trigger(pulseLength);

    velocityCv = computeVelocityCv(ball);
    positionCv = computePositionCv(i);

    sinkFlashes[i] = 1.f;

    removeBall(ball);
}
```

---

### 12.2 Pulse Length

Default:

```cpp
10 ms
```

Context menu:

* 1 ms
* 5 ms
* 10 ms
* 20 ms
* 50 ms

Optional parameter:

* If panel space allows, add a small **LEN** trimpot.
* Otherwise, use context menu.

Recommendation:

Use context menu for first version.

---

### 12.3 Polyphony Option

Possible future feature:

* Individual outputs are monophonic gates.
* ANY output could optionally be polyphonic, with one channel per simultaneous capture.

Initial version:

* Monophonic outputs only.
* Multiple captures in same sample block retrigger pulse generators.
* If pulses overlap, output remains high.

---

## 13. Visual Rendering

### 13.1 Display Style

The central board should be the main performance visual.

Render layers:

1. Board background.
2. Static rails / pegs / sinks.
3. Sink glow state.
4. Balls.
5. Ball trails, optional.
6. Output flash overlay.
7. Debug overlay, if enabled.

### 13.2 Balls

Visual behavior:

* Balls are small glowing metal or pearl-like objects.
* Color may subtly vary by age, speed, or seed.
* Fast balls can have short motion trails.
* Captured balls should disappear into sinks with a flash or small implosion.

Avoid excessive glow if it harms readability.

### 13.3 Pegs

Pegs should read as physical obstacles.

Possible visual forms:

* Rounded teeth.
* Obsidian rivets.
* Coral nodules.
* Metallic pins embedded in dark substrate.

Physics colliders may be circular even if visual peg art is more ornate.

### 13.4 Sinks

Sinks should be obvious capture zones.

Each sink should:

* Have a visible opening.
* Flash when triggered.
* Map clearly to output 1–8.

Possible visual language:

* Eight glowing sockets.
* Eight small maws.
* Eight lower vents.
* Eight abyssal apertures.

### 13.5 Output Jack Feedback

When a sink fires:

* Associated output jack ring flashes.
* Sink region flashes.
* A faint conduit pulse can travel visually from sink to output jack.

This would strongly reinforce cause and effect.

### 13.6 Performance Considerations

Avoid expensive per-frame NanoVG operations.

Recommended:

* Precompute static geometry.
* Draw static layout normally but simply.
* Avoid hundreds of complex paths.
* Use simple circles, strokes, and cached positions.
* Keep trails short and optional.
* Use frame-rate independent flash decay.

Optional optimization:

* Render static board to a framebuffer or cached SVG-like layer later.
* Initial NanoVG direct rendering should be fine with modest geometry.

---

## 14. User Interaction with the Display

### 14.1 Click to Drop

Clicking near the top of the board should spawn a ball at that horizontal position.

Behavior:

* Mouse down in upper spawn zone drops a ball.
* If clicked elsewhere, optional behavior:

  * Drop from clicked x at top.
  * Or ignore.

Recommended:

* Any click in the board drops a ball from that x coordinate at the top.

This makes the module feel tactile.

### 14.2 Drag Tilt

Optional but desirable:

* Dragging horizontally on the board temporarily influences tilt.
* Releasing mouse returns control to knob value.

This may be too much for first version, but it would be delightful.

### 14.3 Debug Overlay

Context menu option:

**Show Physics Debug**

Displays:

* Collider circles.
* Rail segments.
* Sink radii.
* Active ball count.
* Physics step rate.

Useful during development.

Default off.

---

## 15. Context Menu

Recommended context menu items:

```text
Pulse Length
  1 ms
  5 ms
  10 ms
  20 ms
  50 ms

Max Balls
  16
  32
  64
  96

Ball Replacement
  Ignore new balls when full
  Replace oldest ball when full

Layout
  Maw
  Reef
  Vortex
  Trench

Randomize Seed
Copy Seed
Paste Seed

Reset Board
Clear Balls

Show Physics Debug
```

Initial implementation may include layout menu with only Maw enabled, but architecture should support future layouts.

---

## 16. Patch Persistence

Persist the following:

* Layout index
* Seed
* Max balls
* Pulse length
* Ball replacement mode
* Debug overlay enabled / disabled
* Optional: current active balls

Recommendation:

Do **not** persist active balls in first version.

Reason:

* Patch reopen should start clean.
* Persisting dynamic physics state adds complexity.
* The musical patch depends on module settings, not transient ball positions.

JSON example:

```cpp
json_t* dataToJson() override {
    json_t* rootJ = json_object();

    json_object_set_new(rootJ, "layout", json_integer(layoutIndex));
    json_object_set_new(rootJ, "seed", json_integer(seed));
    json_object_set_new(rootJ, "maxBalls", json_integer(maxBalls));
    json_object_set_new(rootJ, "pulseLengthMs", json_real(pulseLengthMs));
    json_object_set_new(rootJ, "replaceOldest", json_boolean(replaceOldest));
    json_object_set_new(rootJ, "debug", json_boolean(debugDraw));

    return rootJ;
}
```

---

## 17. Parameters, Inputs, Outputs, Lights

### 17.1 Params

```cpp
enum ParamId {
    DROP_PARAM,
    RATE_PARAM,
    DENSITY_PARAM,
    GRAVITY_PARAM,
    TILT_PARAM,
    BOUNCE_PARAM,
    DRAG_PARAM,
    CHAOS_PARAM,
    LAYOUT_PARAM,
    SEED_PARAM,
    CLEAR_PARAM,
    PARAMS_LEN
};
```

Notes:

* `LAYOUT_PARAM` can be a snap knob or switch.
* `SEED_PARAM` may be a button rather than a knob.
* If layout is only changed by context menu initially, omit `LAYOUT_PARAM`.

### 17.2 Inputs

```cpp
enum InputId {
    DROP_INPUT,
    RATE_CV_INPUT,
    DENSITY_CV_INPUT,
    GRAVITY_CV_INPUT,
    TILT_CV_INPUT,
    BOUNCE_CV_INPUT,
    CHAOS_CV_INPUT,
    CLEAR_INPUT,
    INPUTS_LEN
};
```

### 17.3 Outputs

```cpp
enum OutputId {
    SINK1_OUTPUT,
    SINK2_OUTPUT,
    SINK3_OUTPUT,
    SINK4_OUTPUT,
    SINK5_OUTPUT,
    SINK6_OUTPUT,
    SINK7_OUTPUT,
    SINK8_OUTPUT,
    ANY_OUTPUT,
    LEFT_OUTPUT,
    RIGHT_OUTPUT,
    VEL_OUTPUT,
    POS_OUTPUT,
    ACT_OUTPUT,
    OUTPUTS_LEN
};
```

### 17.4 Lights

```cpp
enum LightId {
    DROP_LIGHT,
    SINK1_LIGHT,
    SINK2_LIGHT,
    SINK3_LIGHT,
    SINK4_LIGHT,
    SINK5_LIGHT,
    SINK6_LIGHT,
    SINK7_LIGHT,
    SINK8_LIGHT,
    ANY_LIGHT,
    LIGHTS_LEN
};
```

---

## 18. DSP / Process Behavior

### 18.1 Main Process Loop

Pseudo-code:

```cpp
void Mawfall::process(const ProcessArgs& args) {
    updateParamsAndCv(args);

    handleDropInput(args);
    handleInternalClock(args);
    handleClearInput(args);

    runPhysics(args);

    updatePulseOutputs(args);
    updateCvOutputs(args);
    updateLights(args);
}
```

---

### 18.2 CV Modulation

Use helper function:

```cpp
float readModulatedParam(
    ParamId param,
    InputId input,
    float minValue,
    float maxValue,
    float cvScale = 1.f
);
```

General behavior:

* Knob sets base value.
* CV adds modulation.
* Clamp final result.
* Avoid abrupt extreme behavior where possible.

Example:

```cpp
float tiltBase = params[TILT_PARAM].getValue(); // -1 to 1
float tiltCv = inputs[TILT_CV_INPUT].isConnected()
    ? inputs[TILT_CV_INPUT].getVoltage() / 5.f
    : 0.f;

tilt = clamp(tiltBase + tiltCv, -1.f, 1.f);
```

---

## 19. Randomness and Determinism

Use local RNG state.

Recommended:

```cpp
struct XorShift32 {
    uint32_t state;
    uint32_t next();
    float uniform();
    float bipolar();
};
```

Why:

* Fast.
* Deterministic.
* No dependency on global Rack random state.
* Patch behavior can be reproduced.

Seed use cases:

* Spawn jitter.
* Chaos.
* Peg micro-offsets.
* Optional visual variation.

When seed changes:

* Rebuild layout.
* Clear active balls.
* Reset RNG state.

---

## 20. Safety and Stability

### 20.1 CPU Limits

Hard limits:

* Max active balls.
* Max physics steps per audio frame.
* Max peg count per layout.
* Max segment count per layout.

Recommended first-version limits:

```cpp
MAX_BALLS = 64;
MAX_PEGS = 128;
MAX_SEGMENTS = 96;
MAX_SINKS = 16;
```

Initial active sinks:

```cpp
8
```

### 20.2 NaN / Invalid State Protection

Every physics step should guard against invalid values.

If ball position or velocity becomes NaN:

* Remove ball.
* Do not trigger output.

Pseudo-code:

```cpp
if (!std::isfinite(ball.pos.x) || !std::isfinite(ball.pos.y)) {
    removeBall(ball);
}
```

### 20.3 Boundary Escape

If a ball leaves the board bounds by a large margin:

* Remove it.

Suggested:

```cpp
if (ball.pos.y > BOARD_H + 300.f ||
    ball.pos.x < -300.f ||
    ball.pos.x > BOARD_W + 300.f) {
    removeBall(ball);
}
```

---

## 21. Suggested File Structure

Assuming existing Leviathan plugin structure:

```text
src/
  Mawfall.cpp
  Mawfall.hpp              optional, if project uses headers
  MawfallPhysics.hpp       optional helper
  MawfallLayouts.hpp       board layout definitions
  plugin.cpp

res/
  Mawfall.svg              panel
```

If the plugin currently keeps each module mostly self-contained, `Mawfall.cpp` can contain:

* Module class
* Widget class
* Physics structs
* Layout factory functions

However, for clarity, separating layout and physics helpers is recommended.

---

## 22. Class Structure

### 22.1 Module Class

```cpp
struct Mawfall : Module {
    enum ParamId { ... };
    enum InputId { ... };
    enum OutputId { ... };
    enum LightId { ... };

    BoardLayout layout;
    std::vector<Ball> balls;

    dsp::SchmittTrigger dropTrigger;
    dsp::SchmittTrigger clearTrigger;
    dsp::PulseGenerator sinkPulses[8];
    dsp::PulseGenerator anyPulse;
    dsp::PulseGenerator leftPulse;
    dsp::PulseGenerator rightPulse;

    float physicsAccumulator = 0.f;
    float autoDropPhase = 0.f;

    uint32_t seed = 1;
    XorShift32 rng;

    int layoutIndex = 0;
    int maxBalls = 64;
    bool replaceOldest = true;
    bool debugDraw = false;

    float pulseLength = 0.010f;

    float velocityCv = 0.f;
    float positionCv = 0.f;
    float activityCv = 0.f;

    Mawfall();
    void process(const ProcessArgs& args) override;

    void spawnBall(float xNorm = 0.5f);
    void clearBalls();
    void stepPhysics(float dt);
    void captureBall(size_t ballIndex, int sinkIndex);
    void rebuildLayout();
};
```

---

### 22.2 Widget Class

```cpp
struct MawfallWidget : ModuleWidget {
    MawfallDisplay* display;

    MawfallWidget(Mawfall* module);
    void appendContextMenu(Menu* menu) override;
};
```

---

### 22.3 Display Widget

```cpp
struct MawfallDisplay : Widget {
    Mawfall* module;

    void draw(const DrawArgs& args) override;
    void onButton(const event::Button& e) override;

    Vec boardToScreen(Vec p);
    Vec screenToBoard(Vec p);
};
```

Display should read module state but not modify physics except for explicit UI events like click-to-drop.

---

## 23. Visual Mapping

Given display bounds:

```cpp
Rect displayBox;
```

Map board coordinate to local display coordinate:

```cpp
float sx = displayBox.size.x / BOARD_W;
float sy = displayBox.size.y / BOARD_H;
float scale = std::min(sx, sy);

screen.x = displayBox.pos.x + board.x * scale;
screen.y = displayBox.pos.y + board.y * scale;
```

If aspect ratios differ, center the board.

---

## 24. Development Phases

### Phase 1 — Minimal Playable Prototype

Implement:

* Module shell.
* Params and ports.
* Static Maw layout.
* Ball spawning.
* Gravity.
* Peg collision.
* Bottom sink capture.
* 8 gate outputs.
* Basic NanoVG rendering.

Success criteria:

* User can clock DROP input.
* Balls visibly fall.
* Balls bounce off pegs.
* Balls enter one of 8 sinks.
* Corresponding output pulses fire.

---

### Phase 2 — Musical Controls

Implement:

* Gravity knob.
* Tilt knob and CV.
* Bounce knob.
* Drag knob.
* Chaos knob.
* Density knob.
* Internal RATE.
* ANY / LEFT / RIGHT outputs.
* VEL / POS / ACT outputs.

Success criteria:

* Parameter changes audibly alter output distribution and rhythm.
* TILT clearly shifts probability left/right.
* CHAOS increases path variation.
* GRAVITY changes timing.

---

### Phase 3 — Polish and Persistence

Implement:

* JSON persistence.
* Context menu.
* Seed system.
* Max balls setting.
* Pulse length setting.
* Clear/reset.
* Visual flashes.
* Output jack light feedback.

Success criteria:

* Patch save/load works.
* Seed persists.
* No stuck or infinite balls.
* UI feels responsive.

---

### Phase 4 — Additional Layouts

Implement:

* Reef
* Vortex
* Trench

Success criteria:

* Layouts have distinct musical behavior.
* Switching layout clears balls and rebuilds geometry safely.
* Layout selection persists.

---

### Phase 5 — Advanced Interaction

Optional:

* Click-to-drop by x position.
* Drag-to-tilt interaction.
* Ball trails.
* Conduit pulse animation.
* Debug overlay.
* Special mid-board trigger pockets.
* Polyphonic ANY output.
* Ball-ball collisions.

---

## 25. Testing Plan

### 25.1 Unit Tests / Headless Tests

If the Leviathan project has or can add a lightweight test harness, test:

#### Ball Spawn

* Spawning adds active ball.
* Max ball count is respected.
* Replace-oldest mode works.

#### Sink Capture

* Ball placed inside sink triggers expected output.
* Ball is removed after capture.
* ANY output fires.
* LEFT/RIGHT output fires correctly.

#### Physics Stability

* Simulate 10,000 physics steps.
* No NaNs.
* Ball count remains bounded.
* Balls outside board are removed.

#### Determinism

With same seed and same drop sequence:

* Sink event sequence should match.
* Event timing should match within expected tolerance.

#### Layout Validity

For every layout:

* At least 8 sinks exist.
* Peg count below max.
* Segment count below max.
* Spawn point is inside board.
* Sinks are inside board.

---

### 25.2 Manual Rack Tests

Create test patches:

#### Test Patch 1 — Basic Gate Verification

* Clock DROP with 4 PPQN clock.
* Patch outputs 1–8 to envelope generators.
* Verify gates fire visibly and audibly.

#### Test Patch 2 — Tilt Probability

* Send slow LFO to TILT CV.
* Monitor sink distribution.
* Verify left/right bias changes over time.

#### Test Patch 3 — Density Stress

* Increase DENSITY and RATE.
* Confirm no audio glitches.
* Confirm CPU remains reasonable.

#### Test Patch 4 — Seed Recall

* Set seed.
* Save patch.
* Reload patch.
* Confirm same seed and layout are restored.

#### Test Patch 5 — Musical Use

* Outputs 1–8 trigger different drum voices.
* POS CV controls sample selection or oscillator pitch.
* VEL CV controls accent.
* Confirm module feels musically useful.

---

## 26. Acceptance Criteria

Mawfall is ready for initial release when:

1. The module builds cleanly on all supported Rack targets.
2. The default Maw layout is visually clear and musically useful.
3. Balls spawn reliably from manual, external, and internal clock sources.
4. Balls collide convincingly with pegs and rails.
5. Balls are captured by sinks without ambiguity.
6. Sink outputs emit stable 10V pulses.
7. ANY, LEFT, RIGHT, VEL, POS, and ACT outputs work.
8. Parameters and CV inputs visibly and musically affect behavior.
9. CPU usage is bounded under stress.
10. Patch persistence works.
11. No NaN, runaway, or stuck-ball behavior is observed in normal use.
12. The module feels like a Leviathan instrument, not a generic pachinko toy.

---

## 27. Design Philosophy

Mawfall should not merely randomize gates.

It should make probability visible.

The user should feel that each pulse has a physical cause: a drop, a collision, a deflection, a descent, a capture. The module should live somewhere between a pachinko board, a ritual machine, a probability sequencer, and a tiny abyssal physics engine.

The ideal experience:

A clock pulse releases a bead of light.
It falls through teeth and rails.
It ricochets through a dark mechanical reef.
It vanishes into a socket.
A gate fires.
The patch answers.

That is Mawfall.
