# Operational Review — Umi & Doorstop

> Reviewed 2026-07-23. All line references are against the current `src/` tree.

---

## Module Summaries

**Umi** is a physics-based Galton-board / pachinko module. Balls drop through a peg field,
bounce off pegs and walls, and get captured by sinks that fire gate/trigger outputs.
A fixed-timestep physics engine (`1/240 s`) runs inside `process()` via an accumulator;
rendering snapshots are shipped to the UI through a lock-free SPSC queue.

**Doorstop** is a physical-modeling percussion synthesizer. A nonlinear spring drives
modal resonators, a dispersive waveguide, and shaped impact noise. It features a
"break-in" wear model, multiple sound models (classic modal, coupled body, coil contact,
dispersive spring, probabilistic mix), and a sleep/wake system that skips DSP when idle.

---

## Umi Findings

### U-1 · `nextBallId` Wraparound Resets ID Uniqueness Guarantee — LOW

`Umi.hpp:67` / `UmiEngine.cpp:162-165`

| Category | Severity |
|---|---|
| Correctness | LOW |

`nextBallId` increments per spawn and wraps from 0 → 1. After 4 billion spawns, IDs
repeat. `findOldestSlot()` uses the ID as an age proxy (`ball.id < oldestId`), so
post-wrap the ordering breaks and the "replace oldest" eviction could evict the
wrong ball. In practice this requires ~4 billion drops — unlikely in a session — but
worth noting because the fix is trivial.

**Suggestion:** Either ignore (document), or add a generation counter that resets IDs
on overflow:

```cpp
if (nextBallId == 0u) {
    nextBallId = 1u;
    // Optionally renumber active balls sequentially
}
```

---

### U-2 · `captureCount` Unbounded in `StepEvents` — MEDIUM

`UmiEngine.cpp:266`

| Category | Severity |
|---|---|
| Correctness | MEDIUM |

```cpp
CaptureEvent& event = events.captures[static_cast<std::size_t>(events.captureCount++)];
```

`StepEvents::captures` has `MAX_BALLS` (64) slots and `captureCount` is never bounds-checked
before the increment. If somehow more than 64 captures occurred in a single step (not
possible in the current code since each ball can only be captured once per step, and
`capacity <= MAX_BALLS`), it would overrun. The invariant holds today, but it's fragile if
capacity/step logic changes later.

**Suggestion:** Add a guard:

```cpp
if (events.captureCount >= MAX_BALLS) break;
```

---

### U-3 · SPSC `replaceOldestSetting` Toggle Is Not Atomic (Benign Race) — LOW

`UmiWidget.cpp:509-510`

| Category | Severity |
|---|---|
| Correctness | LOW |

```cpp
umiModule->replaceOldestSetting.store(!umiModule->replaceOldestSetting.load());
```

This is a load-negate-store on an `atomic<bool>`. If the UI thread were interrupted
between the load and store (or if two menu items executed concurrently — unlikely
in VCV), the toggle could be lost. VCV Rack's menu system is single-threaded, so
this is benign in practice, but the pattern is fragile.

**Suggestion:** Use `fetch_xor` or a compare-exchange loop if future-proofing is desired.

---

### U-4 · `makePearlLayout` Ignores Its `seed` Parameter — MEDIUM

`UmiLayout.cpp:25-26`

| Category | Severity |
|---|---|
| Correctness | MEDIUM |

```cpp
Layout makePearlLayout(std::uint32_t seed) {
    (void) seed;
```

The seed is accepted but immediately cast to void. Every call produces the identical
layout regardless of seed. The `UmiPlayfieldWidget::step()` method checks
`snapshot.seed != layoutSeed` and rebuilds the layout on seed change, which currently
does nothing meaningful. The module stores, serializes, and lets the user paste/randomize
seeds, but the board never actually varies.

This is presumably intentional (single fixed layout) or planned-but-not-yet-implemented.
If intentional, removing the seed parameter and the randomize/copy/paste seed UI would
reduce user confusion. If unintentional, the seed should drive peg placement randomization.

---

### U-5 · Physics Accumulator Can Build Up Under Load — LOW

`Umi.cpp:232-248`

| Category | Severity |
|---|---|
| Performance / Robustness | LOW |

The physics loop runs at most 4 steps per audio sample. At 44.1 kHz, that budget
is `4 × (1/240) = 16.7 ms` worth of physics per sample — more than enough. But if
the host temporarily stalls (e.g., disk I/O), `physicsAccumulator` can grow, and
the clamping on line 246-248 discards the excess. This is correctly handled — the
`fmod` catch-up prevents runaway — but it means physics "misses" real-time during
stalls. This is the correct trade-off for a non-audio path.

No action needed; noting as a deliberate design decision.

---

### U-6 · `renderSnapshots` Queue Overflow Silently Drops Frames — LOW

`Umi.cpp:183`

| Category | Severity |
|---|---|
| Robustness | LOW |

`renderSnapshots` is an `SpscQueue<RenderSnapshot, 3>`. When full, `push()` returns
`false` and the snapshot is silently dropped. `consumeLatestSnapshot()` drains to the
most recent. This is correct for a display pipeline — dropped intermediate frames are
harmless. No action needed.

---

### U-7 · `RenderSnapshot` Is Large (~2.6 KB) and Copied by Value — LOW

`Umi.hpp:70-78`

| Category | Severity |
|---|---|
| Performance | LOW |

`RenderSnapshot` contains `std::array<BallRenderState, 64>` (each entry is 24 bytes) =
~1.5 KB, plus other fields totaling ~2.6 KB. It's value-copied through the SPSC queue
and again in `consumeLatestSnapshot()`. At the publish rate (~60 Hz from the divider)
this is fine, but it's worth knowing this exists if the struct grows.

---

## Doorstop Findings

### D-1 · Symplectic Euler Integrator Can Become Unstable at Low Sample Rates — HIGH

`DoorstopEngine.cpp:382-393`

| Category | Severity |
|---|---|
| Correctness | HIGH |

The spring integrator uses symplectic (semi-implicit) Euler:

```cpp
springVelocity += acceleration * sampleTime;
displacement += springVelocity * sampleTime;
```

For a nonlinear stiffened spring, the stability condition is approximately
`ω × dt < 2`, where `ω` is the effective angular frequency. The base spring
(`baseFrequencyHz = 16 Hz` → `ω ≈ 100`) is fine even at low sample rates. But
modes go up to `maximumModeOmega = 2π × min(12000, 0.2 × sampleRate)`. At 44.1 kHz,
the highest mode `ω = 2π × 8820 ≈ 55,400`, giving `ωdt = 55400/44100 ≈ 1.26` — within
the stability limit but with noticeable frequency warping. The mode frequency clamping
at `0.2 × sampleRate` (line 93) is the key defense here, keeping `ωdt ≤ 1.26`.

This is borderline. At exactly `1000 Hz` (the minimum accepted sample rate per line 54),
`maximumModeOmega = 2π × 200`, giving `ωdt = 2π×200/1000 ≈ 1.26` — the same ratio due
to the `0.20f * sampleRate` scaling. **The ratio is always 1.26**, which is stable for
a linear oscillator but may cause issues with the nonlinear stiffness
(`nonlinearStiffness = 1.4`) at high amplitudes. The displacement clamping on lines
385-392 is the last-resort safety net.

**Suggestion:** The current design is workable. For extra margin, consider tightening the
mode frequency cap to `0.15 × sampleRate` or adding a single sub-step for the spring
integration. The nonlinear `x³` term could be computed at half-step displacement for
better energy conservation.

---

### D-2 · `allFinite()` Does Not Check the Waveguide Delay Line — MEDIUM

`DoorstopEngine.cpp:607-639`

| Category | Severity |
|---|---|
| Robustness | MEDIUM |

`allFinite()` checks ~21 scalar state variables and all modal bank entries, but does
**not** scan the `waveguideDelay[8192]` buffer. If a NaN entered the waveguide delay
line but the currently-read scalar outputs happened to still be finite (possible in
the first few samples of corruption), `allFinite()` would return `true` and the
contamination would persist, spreading on each read.

**Suggestion:** Check the most recently written sample:

```cpp
if (!std::isfinite(waveguideDelay[waveguideWriteIndex > 0
        ? waveguideWriteIndex - 1 : MAX_WAVEGUIDE_DELAY - 1])) {
    return false;
}
```

Or check the read position. Scanning the full 8192 buffer every sample would be
wasteful.

---

### D-3 · `process()` Ignores Its `requestedSampleTime` Parameter — LOW

`DoorstopEngine.cpp:669-670`

| Category | Severity |
|---|---|
| Correctness | LOW |

```cpp
Frame Engine::process(float requestedSampleTime) {
    (void) requestedSampleTime;
```

The engine uses its internally cached `sampleTime` (set via `setSampleRate()`). The
caller in `Doorstop::process()` passes `args.sampleTime`. As long as
`onSampleRateChange()` is called before `process()` resumes (which VCV Rack guarantees),
these will agree. However, if VCV Rack ever provides per-sample time-stretching (unlikely
but possible), this would diverge.

**Suggestion:** Either use the parameter or remove it from the signature to avoid
confusion.

---

### D-4 · Probabilistic Model Selection Is Uniform Over 4 Models — No Issue

`DoorstopEngine.cpp:266`

```cpp
return static_cast<SoundModel>(nextModelRandom() % 4u);
```

The `% 4u` selects uniformly from `{Classic, CoupledBody, CoilContact, DispersiveSpring}`,
excluding `ProbabilisticMix` (value 4). This is correct — mixing the mixer into itself
would be recursive. The xorshift32 output `% 4` has zero bias since `2^32 mod 4 == 0`.

---

### D-5 · `recoverFromNonFinite()` Does Not Log — LOW

`DoorstopEngine.cpp:228-230`

| Category | Severity |
|---|---|
| Robustness | LOW |

When NaN recovery fires, it silently resets all motion. Adding a `WARN()` log would
help diagnose instability during development without affecting production performance
(VCV's logging is already conditional).

**Suggestion:**

```cpp
void Engine::recoverFromNonFinite() {
    WARN("Doorstop: non-finite state detected, resetting motion");
    resetMotion();
}
```

---

### D-6 · `DoorstopOverflowWidget` Lifetime Edge Cases — MEDIUM

`DoorstopWidget.cpp:241-252, 305-311, 336-347`

| Category | Severity |
|---|---|
| Robustness | MEDIUM |

The overflow widget is added as a child of `RackWidget` (not `ModuleWidget`) to allow
drawing across panel boundaries. The `shared_ptr<DoorstopOverlayLink>` ensures the link
outlives both the widget and the overlay. The destructor clears `link->owner = nullptr`
and calls `destroyOverflowWidget()`.

**Concern:** If `createOverflowWidget()` fails to find `cableContainer` (line 341),
the overlay is deleted and never retried until the overflow toggle is cycled. This could
happen if the widget tree isn't fully initialized at `step()` time. On the next `step()`
call, `overlayLink->overlay` is null and `createOverflowWidget()` will be retried (line
397-398), so this is self-healing. However, if `cableContainer` is *persistently* null
(broken rack state), it retries every frame — a minor waste.

**Suggestion:** Add a retry cooldown or a "failed" flag that clears on toggle.

---

### D-7 · Spring Geometry Computed Every UI Frame — Performance Observation

`DoorstopWidget.cpp:380-390`

| Category | Severity |
|---|---|
| Performance | LOW |

`buildSpringGeometry()` computes 401 points with `sin()`, `sqrt()`, `atan2()` calls.
This runs every UI frame (≤60 Hz) per spring path, and up to 4 paths when trails are
active. At 60 FPS this is ~96,000 trig evaluations per second. On modern CPUs this is
negligible (~0.1 ms/frame), but if many Doorstop instances are loaded it could add up.

The `springPointTemplates()` function correctly pre-computes and caches the expensive
`sin()` calls in a static array — the per-frame work is mostly multiplies and a `sqrt()`.
This is well-optimized already.

---

## Summary

| ID | Module | Severity | Category | One-liner |
|----|--------|----------|----------|-----------|
| U-1 | Umi | LOW | Correctness | `nextBallId` wrap breaks oldest-eviction ordering |
| U-2 | Umi | MEDIUM | Correctness | `captureCount` increment has no bounds guard |
| U-3 | Umi | LOW | Correctness | Non-atomic read-modify-write on `replaceOldestSetting` |
| U-4 | Umi | MEDIUM | Correctness | `makePearlLayout` ignores `seed` parameter |
| U-5 | Umi | LOW | Robustness | Physics accumulator overflow (correctly handled) |
| U-6 | Umi | LOW | Robustness | SPSC queue silently drops frames (by design) |
| U-7 | Umi | LOW | Performance | `RenderSnapshot` (~2.6 KB) copied by value |
| D-1 | Doorstop | HIGH | Correctness | Symplectic Euler borderline stability for high modes |
| D-2 | Doorstop | MEDIUM | Robustness | `allFinite()` doesn't check waveguide delay buffer |
| D-3 | Doorstop | LOW | Correctness | `process()` ignores its `sampleTime` parameter |
| D-5 | Doorstop | LOW | Robustness | NaN recovery is silent — no diagnostic logging |
| D-6 | Doorstop | MEDIUM | Robustness | Overflow widget lifetime edge cases |
| D-7 | Doorstop | LOW | Performance | Spring geometry recomputed every frame |

### Overall Assessment

Both modules are well-engineered. The code shows strong defensive practices:

- **NaN/Inf guards** throughout Doorstop (`allFinite()`, `recoverFromNonFinite()`,
  `isfinite()` checks on every input)
- **Lock-free audio↔UI communication** via SPSC queues (Umi) and atomic
  release-acquire pairs (Doorstop)
- **Robust serialization** with schema versioning, type checking, and range validation
- **Sleep optimization** in Doorstop that skips DSP entirely when idle
- **Input clamping** on all physics parameters and voltage inputs

The only HIGH-severity finding is the integrator stability margin in Doorstop (D-1),
which is already mitigated by the `0.2 × sampleRate` mode frequency cap and displacement
clamping, but has thin margin under nonlinear excitation at high amplitudes.
