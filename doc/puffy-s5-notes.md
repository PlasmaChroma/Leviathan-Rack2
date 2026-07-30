# Puffy v1 — Review Notes

> Scope: this is a critique pass, not a rewrite. The spec is already
> implementation-grade; the findings below are the places where the contract
> is ambiguous, internally in tension, or likely to bite an implementer who
> follows it literally. Line refs are to `puffy.md` as uploaded.

## 1. DSP / signal-flow findings

### 1.1 `DEFLATE` placement changes what "12 dB of attenuation" means (§3.3, §5, §6)

The signal order is `Auto Deflate -> manual DEFLATE -> limiter`, and the
limiter ceiling (`5.0 V` / `4.456 V`) is fixed regardless of `DEFLATE`. That's
a legitimate design — it makes `DEFLATE` a pre-limiter headroom control, so
turning it up means the limiter has to work *less hard*, not just that the
output gets quieter by a fixed amount.

But §1.1 states the user can "reduce the processed output by up to 12 dB
with `DEFLATE`," which reads like a literal output trim. It only behaves that
way below limiter engagement. Once the limiter is active, raising `DEFLATE`
reduces gain reduction proportionally, so the *net* output change is less
than the nominal dB value — the two controls interact nonlinearly.

This isn't a bug, but it's the kind of thing that will generate confused bug
reports ("DEFLATE isn't doing what it says"). Recommend either:
- adding one sentence to §3.3 making the pre-limiter interaction explicit, or
- adding it as a named acceptance test (see §4.2 below) so the behavior is
  pinned down rather than discovered later.

### 1.2 `MASTER` mode runs a second, independent oversampler (§6.2)

The detector always upsamples at a fixed 4x (`dsp::Upsampler<4,8>`)
regardless of the user's main oversampling selection (2x/4x/8x, §5.2). At
`8x` + `MASTER`, you're running two live FIR oversample instances
concurrently — the character path's 8x and the detector's fixed 4x. That's
real, not huge, but real CPU on top of what the realtime budget already has
to absorb.

§11.6's perf test says "8x must remain at least 5x realtime" but doesn't
pin down that this must be measured *with `MASTER` also engaged*, which is
the actual worst case the module can be put into. Worth naming that specific
combination as the acceptance target rather than leaving it implied.

### 1.3 Ambiguous whether there's one `FRENZY` detector instance or two (§5.5 vs §7.2)

§5.5 defines the `fast`/`slowSq`/`transient` detector that drives `FRENZY`'s
own drive/bias math. §7.2 separately says `transientActivity` in the visual
snapshot is "computed from the `FRENZY` detector formula even when another
character is selected" — i.e., it's always live, not just when `FRENZY` is
the active character.

The spec doesn't explicitly say these are *the same object*. As written, an
implementer could reasonably build two separate detector instances (one
audio-facing, one visual-facing) that share a formula but drift in state.
Worth one explicit line: there is exactly one live detector; `FRENZY`'s DSP
reads it when active, the visual snapshot reads it always.

### 1.4 `FRENZY`'s negative-side gain isn't normalized, and nothing tests it (§5.5, §11.1)

`positiveNorm` only normalizes the positive branch of the asymmetric curve;
the negative side isn't separately unity-anchored. That's presumably
intentional — it's the source of the asymmetry — but it means the negative
peak at `x = -1`, high amount, high `fast`/`transient` isn't guaranteed to
land anywhere near unity before the `±1.25` clamp catches it. §11.1 tests
that `FRENZY` is "asymmetrical and input-reactive" but doesn't pin a bound on
*how much* negative-side gain is expected. Suggest adding an explicit bound
(e.g. negative excursion at `x=-1`, `a=1`, worst-case `fast`/`transient`
stays under some stated multiple of unity) so a future bias-constant tweak
doesn't silently change headroom demands on the limiter.

## 2. Architecture findings

### 2.1 `PuffyWidget.cpp` bundles panel, controls, menu, and fish widget (§8)

Given what turned up on the Integral Flux review — a hand-rolled cross-widget
mediator and duplicated drop-shadow framebuffer-cache machinery spread across
five knob types — putting "panel, controls, menu, fish widget" into a single
translation unit is the same shape of risk starting fresh. The appendix
(§A.7) already names a target boundary for the *future* renderer split
(`PuffyFishWidget.hpp`, `PuffyFishNanoVG.cpp`). Cheapest fix: adopt that
boundary in v1 instead of retrofitting it later — split the fish widget out
of `PuffyWidget.cpp` now, even while it's still NanoVG-only.

### 2.2 Framebuffer caching should be shared, not re-derived

§7.1 mentions caching static viewport decoration in a `FramebufferWidget`.
If there's already a shared caching helper from the knob-cache cleanup on
Integral Flux, Puffy should use it rather than growing its own bespoke
cache-invalidation logic for the fish viewport — that's exactly the kind of
repeated-per-module machinery that review flagged.

## 3. Testing gaps

### 3.1 Concurrent transitions aren't composed (§5.2, §6.3, §9)

Character crossfade (10 ms, parallel-run old/new) and structural transitions
(oversampling or limiter-mode change: 5 ms fade-down → reset → 5 ms fade-up)
are each specified in isolation. Nothing states what happens if a user
triggers both near-simultaneously — e.g. flips the character switch right as
they change oversampling from the context menu. Recommend an explicit rule
(e.g. "a structural transition in progress defers any character-crossfade
request until it completes") and add it to §11.5's acceptance list. Without
it, this is exactly the kind of interaction that only shows up as an
intermittent click during manual QA.

### 3.2 Pin down the `DEFLATE`/limiter interaction from §1.1 as a test

Add an acceptance criterion that measures actual output-level change from
`DEFLATE` both below and above limiter engagement, so the nonlinear
interaction is a documented, verified property instead of emergent behavior.

## 4. Non-issues worth a one-line confirmation

- §6.2's 4096-sample fixed capacity is ~21 ms of headroom at 192 kHz against
  a 2 ms requirement — looks like deliberate over-provisioning, not a stale
  number, but worth a comment in the code so a future reader doesn't "right
  size" it down.
- Panel region math (§3.1) stacks cleanly: 7–18 / 20–68 / 70–99 / 101–125 mm,
  no overlaps, fits inside the 128.5 mm panel height. No action needed.

## Summary

The core DSP contract, threading boundary, and persistence rules are solid
and internally consistent. The findings above are concentrated in three
places: what `DEFLATE` actually guarantees once the limiter is engaged,
whether the `FRENZY` detector is one instance or two, and whether the widget
file boundaries you already designed for the future 3D renderer should just
be adopted now instead of retrofitted post-v1.
