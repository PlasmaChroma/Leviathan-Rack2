# Consequence Engine Spec Review

**Reviewing:** [consequence_engine.md](file:///home/Levi.Kendall/dev/Leviathan-Rack2/doc/consequence_engine.md)
**Verdict:** This is a strong, implementation-ready spec. The issues below are refinements, not rewrites.

---

## Overall Assessment

The spec is unusually well-structured for a module of this complexity. The priority ordering in §1, the MUST/SHOULD/MAY discipline, the explicit non-goals in §28, and the phased milestones are all exactly right. The "Phase 2 proof point" philosophy in §29 is wise — this document reads like it was written by someone who has shipped complex interactive audio modules before.

The refinements below fall into three categories:
1. **Ambiguities** that could lead to divergent interpretations during implementation
2. **Missing contracts** for edge cases the spec doesn't address
3. **Notation/wording** that could confuse an implementer

---

## A. Ambiguities to Resolve

### A1. Clock multiplier notation is inverted (§4.3)

The table reads:

```text
4×    2×    1×    1/2×    1/4×
```

With `2×` glossed as "one autonomous opportunity every two input clocks." In standard musical terminology, that's a **division** (÷2), not a multiplication. The `1/2×` entry meaning "two opportunities per clock" is the actual 2× speed.

> [!IMPORTANT]
> Either invert the labels to match musical convention (`÷4  ÷2  1×  2×  4×`) or redefine the notation explicitly: "`N×` means the autonomous period is `N` times the clock period." Both are defensible, but the current text will confuse anyone who reads `2×` as "twice as fast."

### A2. `drift` field is undefined (§6)

The `ConsequenceEvent` struct includes a `float drift` member, but no section of the spec defines what drift represents, how it's set, or what modifies it. It appears in no behavioral rule.

**Suggestion:** Either define drift (presumably X-axis velocity or pitch drift rate) with a clear role in the lifecycle, or remove it from the representative struct and note it as a possible implementation detail.

### A3. Interaction radius is never specified (§14, §17)

The spec says "only spatial neighbors interact" and provides an 8×8 grid, but never defines the actual interaction distance. Is it "same cell + adjacent cells" with any two Events in that 3×3 region eligible? Or is there a continuous distance threshold within those cells?

**Suggestion:** Specify one of:
- "Events interact when their Euclidean distance in normalized pool space is ≤ `INTERACTION_RADIUS` (recommended 0.15) and they share the same or adjacent grid cells."
- "All active Event pairs within the same or adjacent grid cells are eligible for interaction."

The first gives a tunable knob for future refinement. The second is simpler and fully deterministic from the grid alone.

### A4. What "next ecology tick" means for newborn suppression (§7.3)

> "Newborn Events MUST NOT receive an autonomous action until the next ecology tick."

The ecology runs at 1 kHz (§16.2). If "next ecology tick" means literally the next 1 ms tick, this provides almost no suppression. The intent seems to be that newborns skip the **current batch** of simultaneous actions, preventing cascading births within a single tick. If so, say:

> "Newborn Events created during a given ecology tick MUST NOT receive an autonomous action during that same tick. Their first `nextActionTick` is set to at least the tick following their birth."

### A5. Manual recombination when both parents are held (§8.5)

The spec says dragging a captured Event into collision range of another creates a recombined child. But what if the target Event is also held by another controller (mouse + CV, or two touch channels)? Questions:

- Does recombination still occur?
- Who owns the child — no one (it's immediately autonomous), or the dragging controller?
- §7.2 says held Events can't be killed by destructive interaction. Does the same immunity apply to being a recombination parent (energy debit)?

**Suggestion:** Add a brief clause: "Manual recombination applies regardless of the target's held state. The child is born autonomous (unowned). Energy debit from a held parent is deferred until release, or bounded to prevent the held Event from becoming non-viable on release."

### A6. Voice-stealing oscillation between held Events (§12.1)

Rule 4: "if a held request still has no voice, steal the oldest held voice."

The subsequent prose says: "the older Event remains held and visible but silent until released; it MUST NOT repeatedly fight to reacquire a voice."

This is correct for two Events, but consider a rotation: Event A steals from B, then Event C arrives and steals from A, then B's controller re-triggers (e.g., a gate retrigger). Does B now steal from C? The "MUST NOT repeatedly fight" rule needs a clearer mechanism.

**Suggestion:** "A held Event whose voice was stolen enters a `voiceless-held` state. It does not participate in further voice acquisition until its controller releases and re-triggers, or until a free voice becomes available. The voice allocator checks for voiceless-held Events of the same controller on each voice release and re-assigns before considering new requests."

---

## B. Missing Contracts

### B1. No CV inputs for ecology parameters

The eight ecology knobs (CONSEQUENCE, LIFE, TIME, BRANCH, MUTATE, MEMORY, AFFINITY, DENSITY) have no specified CV input ports. For a 20 HP module, jack space is tight, but this is unusual for a Rack module — most expect at least the core parameters to be externally modulatable.

**Options:**
1. **Explicit v1 non-goal.** Add to §28: "Per-parameter CV modulation inputs." This is the cleanest if panel space won't allow it.
2. **Attenuverter inputs for a subset.** CONSEQUENCE and AFFINITY are the most expressive candidates for external modulation. Two extra jacks is feasible at 20 HP.
3. **Expander module.** Note in §28 that a future expander may provide full CV control.

### B2. Module bypass behavior

Rack's bypass mechanism (right-click → Bypass) is unspecified. For a generative module, the musically correct behavior is probably:

- All output gates go low immediately (no lingering held notes downstream)
- All trigger outputs go low
- Ecology freezes (no aging, no autonomous actions)
- On un-bypass, ecology resumes from frozen state (not a RESET)

If this is left unspecified, the default Rack bypass behavior (pass-through from matching input to output) will produce garbage, since the input and output port topology doesn't match.

**Suggestion:** Add a §11.5 or a note in §22: "On module bypass, all output ports MUST output 0 V. Ecology state freezes. On un-bypass, the ecology resumes without reset."

### B3. Hot sample-rate change

§23 requires determinism for a given sample rate, and §16.2 uses a sample-time accumulator for the 1 kHz ecology. If Rack's sample rate changes mid-session (which it can), the accumulator's rounding will differ from a session that started at the new rate. This breaks the determinism contract for any sequence that spans a rate change.

**Suggestion:** Add to §23: "A sample-rate change mid-session is equivalent to a discontinuity. Determinism is guaranteed only for sequences played entirely at a single sample rate. The ecology accumulator MUST reset cleanly on rate change without losing or doubling events."

### B4. Module initialization vs. first `process()` call

The spec covers RESET, CLEAR, REROLL, and serialization load, but doesn't specify what state the module is in after construction and before the first `process()` call. Rack modules can be added to a running patch.

**Suggestion:** Add to §24: "On construction, the module initializes with the default seed, an empty Event pool, and default parameter values. The first `process()` call begins ecology from tick 0."

### B5. `ENERGY` input interaction with `CONSEQUENCE` at 0

§4.1 says at CONSEQUENCE 0, "a Seed Event retires when its controller releases it and its output gate closes." §9 says an absent ENERGY input maps to 1.0. But what if ENERGY input is 0 V while the gate is high? Does the Event exist but with zero energy? Can a zero-energy held Event still sound?

**Suggestion:** Clarify in §9: "ENERGY 0 V creates an Event with zero initial energy. A held Event with zero energy still sounds while its gate is high. On release, it retires immediately regardless of CONSEQUENCE."

---

## C. Wording / Notation

### C1. CLEAR vs. REROLL distinction (§11.3, §11.4)

The current wording splits the logic across two sections in a way that requires cross-referencing to understand. The core distinction is simple:

- **CLEAR:** Empty the pool. Reset PRNG and IDs from the *current* seed. Same seed → same future.
- **REROLL:** Pick a new seed, then CLEAR. Different seed → different future.

Consider restating §11.3–11.4 with this framing up front, then the behavioral details.

### C2. "Deterministic variation" needs bounds (§4.2, §4.3)

§4.2 says "a starting range of 0.75×–1.25× is recommended" but uses SHOULD-adjacent language ("recommended") for what's actually a critical behavioral bound. If an implementer uses 0.5×–2.0× variation, the LIFE knob becomes much less predictive.

**Suggestion:** Promote to: "Variation MUST be bounded to 0.75×–1.25× of the base value." Or if you want to leave room for tuning: "Variation MUST NOT exceed ±50% of the base value; the initial implementation SHOULD use ±25%."

### C3. §12 output voltage for GEN

```text
GEN     generation 0 → 0 V, generation 15 → 10 V, clamp above 15
```

This is a linear 0–10 V mapping clamped at gen 15, which means gen 1 = 0.667 V, gen 2 = 1.333 V, etc. That's fine, but it means gen 0 (the performed note) and gen 1 (first child) are only 0.667 V apart. For downstream use as a modulation source, a logarithmic or stepped mapping might be more useful.

This is a taste call, not a bug — just flagging that the linear mapping compresses the most musically interesting range (gen 0–3) into the bottom 2.67 V.

### C4. Trigger pulse width (§12)

> "Trigger pulses SHOULD be 1 ms."

Given how precisely the rest of the spec uses MUST/SHOULD, this SHOULD is conspicuous. 1 ms is the Rack ecosystem standard. Is there a reason not to make this a MUST?

### C5. §19 VisualEvent `y` field

The `VisualEvent` struct has a `float y` field, but §5.1 says pitch is stored as raw voltage internally and Y position is derived from the pitch range setting. Should the visual snapshot contain the raw pitch, the normalized Y, or both? If it's normalized Y, what converts it — the DSP side during snapshot publication, or the renderer?

**Suggestion:** Clarify: "The `y` field is normalized pool position `[0, 1]`, converted from pitch voltage during snapshot publication. The renderer does not need access to the pitch range setting."

---

## D. Minor Nits

| Section | Issue |
|---------|-------|
| §5.1 | "Values outside the visible range may exist briefly during mutation but MUST be folded or clamped back into the selected range before output." — Before *pitch output* or before *any ecology processing*? If an Event can briefly exist at an out-of-range pitch, its grid cell assignment in §17 could be wrong. |
| §8.2 | "Resolve overlapping hits by smallest pointer distance, then highest Event ID." — Highest ID means newest. Was this intentional, or should it be lowest (oldest/most established)? |
| §9 | "reducing GATE channel count releases removed channels as if their gates fell." — This is correct but should note that Rack can reduce channel count to 0, which releases all. |
| §15 | `pressure = activePopulation / targetDensity` — Should this use active *living* Events, or include Events that are alive but voiceless? The distinction matters because voiceless Events still branch and interact. |
| §17 | "initially an 8 × 8 grid" — Does this mean 8 columns × 8 rows, or is the grid always square regardless of pool aspect ratio? The pool is not square (it's wider than tall at 300 × ~210 px). A non-square grid (e.g., 10×6) would give more uniform cell sizes. |

---

## E. Structural Suggestions

### E1. Add a port summary table

The spec defines ports across §9, §10, §11, and §12. A consolidated table early in the document (or in §3 alongside the panel diagram) would help:

```text
INPUTS                          OUTPUTS
──────                          ───────
PITCH    poly 1V/oct            PITCH     poly 1V/oct
GATE     poly gate              GATE      poly gate
ENERGY   poly 0–10V             ENERGY    poly 0–10V
X        poly 0–10V             GEN       poly 0–10V
Y        poly 0–10V             BIRTH     mono trigger
TOUCH    poly gate              DEATH     mono trigger
PRESSURE poly 0–10V             INTERACT  mono trigger
CLOCK    mono                   POP       mono 0–10V
RESET    mono trigger
```

This also makes it easy to verify that 20 HP can physically accommodate 9 inputs + 8 outputs + 8 knobs + 2 buttons + the pool area.

### E2. Consider a "Quick Reference" appendix

For implementation, a one-page summary of: all MUST rules, all fixed constants (128 Events, 16 voices, 16 touches, 8×8 grid, 1 kHz ecology, 1 ms triggers), and all context-menu settings would be valuable. The spec is thorough enough that finding a specific constraint requires reading through narrative prose.

---

## F. Codebase Alignment (from existing Leviathan modules)

Reviewed existing patterns across `Umi`, `Mandelwake`, `Cantor`, `Crownstep`, `Wyrm`, `Bulkhead`, `Chromatide`, and others.

### F1. The spec's architecture matches proven patterns — mostly

The good news: §16 (timing domains), §18 (command queue), and §19 (visual snapshots) all describe exactly what Umi and Mandelwake already do. The spec is well-aligned with the codebase's idioms.

However, **`SpscQueue` is currently duplicated** as a local template in both [Mandelwake.hpp](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/Mandelwake.hpp) and [Umi.hpp](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/Umi.hpp). The same is true for PRNG wrappers (Xorshift32 in Umi/Cantor, SplitMix64 in Mandelwake) and physics-tick accumulators.

**Suggestion for §25 (file organization):** Consider whether this module is the right moment to extract shared infrastructure:

```text
src/shared/SpscQueue.hpp          // currently duplicated in Umi.hpp, Mandelwake.hpp
src/shared/DeterministicPrng.hpp  // Xorshift32, SplitMix64, domain-tagged hashing
src/shared/FixedSpatialGrid.hpp   // generalized from Umi's ColliderCell pattern
```

Or, if premature extraction is undesirable, §25 should at least note: "Reuse or adapt the `SpscQueue<T, Capacity>` template from existing modules."

### F2. Spatial grid — Umi precedent suggests higher cell capacity

Umi uses `MAX_COLLIDERS_PER_CELL = 32` with a 10×14 grid (140 cells). The CE spec proposes 8×8 (64 cells) for 128 Events. Worst case, if Events cluster, a single cell could need to hold all 128. The spec should either:

- Specify a `MAX_EVENTS_PER_CELL` and define overflow behavior (e.g., excess Events are excluded from interaction that tick but not killed), or
- Note that the grid is an acceleration structure only — all Events are stored in the flat array, and the grid contains indices. Overflow means missed interactions, not data loss.

### F3. PRNG choice should be specified

The codebase uses two patterns:
- **Xorshift32** for fast per-entity decisions ([UmiEngine.cpp](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/UmiEngine.cpp), [CantorCultureEngine.cpp](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/CantorCultureEngine.cpp))
- **SplitMix64 with domain tags** for reproducible multi-axis hashing ([MandelwakeFixedPoint.hpp](file:///home/Levi.Kendall/dev/Leviathan-Rack2/src/MandelwakeFixedPoint.hpp))

The CE spec's §23 determinism contract is demanding enough that the choice matters. Per-Event Xorshift32 (seeded deterministically from engine seed + Event ID) is probably the right fit — it's what the ecology needs (fast, local, sequential draws). But the spec should state this explicitly so the implementer doesn't accidentally use domain-tagged hashing where sequential consumption order would create fragile determinism.

### F4. Existing clock/reset patterns are straightforward to adopt

The codebase consistently uses `dsp::SchmittTrigger` with `(0.1f, 2.0f)` thresholds and tracks `isConnected()` transitions to reset the trigger on cable changes. The spec's §11.1 clock requirements are compatible with this pattern. No spec change needed — just confirming alignment.

### F5. Serialization pattern is established

The `schemaVersion` + Jansson JSON + `pendingSeed` atomic handoff pattern from Cantor/Mandelwake maps cleanly onto §24. The spec's requirement to push deserialized state through the command queue (not mutate DSP state directly in `dataFromJson`) matches the existing idiom exactly.

### F6. Rendering layers — follow Umi's approach

Umi's layered rendering (cached background framebuffer + dynamic canvas widget consuming snapshots) maps directly onto the CE's needs:
- **Background layer:** Pool border, pitch guides, octave lines (cached in a `FramebufferWidget`, invalidated on pitch-range change)
- **Dynamic layer:** Event discs, trails, ancestry lines, ripples (drawn from the latest `VisualSnapshot`)

The spec's §21 visual tiers fit this model well. §19 should note that the background layer (pitch guides, grid) can be a cached framebuffer independent of the snapshot triple-buffer.

### F7. Port/widget conventions to adopt

The spec should reference existing widget classes in §25 or the panel design:
- `Magitek2InputJack` / `Magitek2OutputJack` for port styling
- `LeviathanHaloKnob2` or `DarkTinyClockworkGearKnob` for parameter knobs
- `SmallGoldApertureButton` for CLEAR/REROLL
- `panel_svg::loadPointFromSvgMm` for SVG-driven layout anchoring

This isn't critical for the design spec, but would reduce implementation ambiguity for Phase 1.

---

> [!TIP]
> The strongest aspects of this spec — the timing architecture separation (§16), the determinism contract (§23), the visual/DSP firewall (§18–19), and the "Phase 2 proof point" philosophy — should be preserved exactly as written. They're the kind of constraints that prevent the hardest classes of bugs in interactive generative modules.
