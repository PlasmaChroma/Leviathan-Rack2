# TemporalDeck Mouse Scratching Research and Codex-Ready Coding Recommendations

Dragon King Leviathan, this document focuses on making the **manual mouse scratch** on TemporalDeck’s virtual platter feel *intentionally playable* to turntablists—tight, reversible, and predictable—while keeping CPU use controlled and offering optional “pay-for-quality” modes via the context menu. fileciteturn0file0 fileciteturn0file1

## What you have now and why it’s hard

TemporalDeck’s design is already aligned with the right conceptual model for a live-buffer “deck”: a continuously-writing circular buffer with a read head whose lag (read-behind-write) is manipulable via transport controls, CV, mouse wheel, and **mouse platter drag**. fileciteturn0file0

The core difficulty is that **turntablism-grade “scratch feel” is not primarily about visuals or even interpolation order**; it’s about *gesture-to-audio mapping being stable* under four realities:

First, GUI drag events are not “audio rate.” In VCV Rack, `DragMoveEvent` occurs **once per frame** and provides `mouseDelta` “since the last frame.” citeturn0search1turn0search14 This means the gesture stream is fundamentally tied to Rack’s effective frame cadence, monitor refresh, and the Rack “Frame rate” setting (which can intentionally lower redraw frequency). citeturn0search3

Second, turntablist scratches commonly involve **fast back-and-forth micro-motions** (baby, scribble, tear, etc.), with repeated direction changes that must “bite” cleanly, not smear into latency or coast. citeturn1search0turn1search9

Third, users will expect that “how it feels” does **not change** just because they changed GPU load, limited Rack’s frame rate, or moved between 60 Hz and 144 Hz displays. Any velocity-/inertia-dependent logic that is implicitly frame-rate dependent will feel inconsistent. citeturn0search3turn0search1

Fourth, if you want the “quality” path to sound better under violent time-warping, you are (DSP-wise) doing *sample-rate conversion* under rapidly changing playback speed (tempo/pitch) rather than steady-rate playback, which is exactly why DJ software treats scratching resampling as a special case. citeturn2search5turn0search4

Your spec and current code already show strong structure: separate scratch models (Legacy vs Hybrid), optional higher-quality interpolation for scratch, and lots of motion-aware cleanup/tone logic. fileciteturn0file0 fileciteturn0file1 The “win” is to make the **mouse gesture layer** (and the motion estimation it feeds) frame-rate invariant and turntablist-friendly.

## VCV Rack UI mechanics that matter for scratching

Three Rack-level facts shape the best approach.

Drag move cadence: `DragMoveEvent` provides a `mouseDelta` that is explicitly “since the last frame,” and the event “occurs every frame on the dragged Widget.” citeturn0search1turn0search14 Designing the scratch engine as if drag events were “timed” by audio is the trap; you must either (a) compute time deltas (dt) explicitly or (b) avoid any logic that depends on an assumed GUI dt.

Reliable timing source: Rack provides `rack::system::getTime()` which returns a monotonic “seconds since application launch” and is intended to be fine-grained and fast for timing/benchmarking. citeturn2search0turn2search10 This is suitable for measuring dt between successive drag updates if you decide to do dt-based velocity estimation at the UI layer.

Cursor lock as a usability multiplier: Rack exposes `Window::cursorLock()` / `cursorUnlock()` and a user setting `settings::allowCursorLock` described as allowing Rack “to hide and lock the cursor position when dragging knobs etc.” citeturn1search4turn1search1 For turntable-like gestures, cursor lock can meaningfully improve feel because it prevents “hitting the screen edge” during aggressive strokes—especially on small laptop screens—without requiring your own pointer-warp hacks.

## Turntablist-relevant feel cues to design for

A “good” mouse scratch in a live-buffer deck usually needs four psycho-motor properties.

Direction changes must be crisp. Turntablism training systems describe the “phantom click” at direction change: the record is momentarily motionless, producing a micro-gap that contributes to the articulation of a baby scratch. citeturn1search9 Your DSP cleanup already acknowledges direction flips as special; the remaining improvement is ensuring the *gesture* itself doesn’t create a mushy, delayed reversal.

Stationary hold must be intentional. When the user stops movement while holding the platter, the audio should stop moving quickly, not coast for tens of milliseconds (unless you intentionally model slip). Many scratch fundamentals are literally “push–pull” movements across a small region (baby, scribble), and those techniques rely on predictable stopping points. citeturn1search0turn1search9

Responsiveness vs looseness should be tunable (even implicitly). DJ scratching implementations frequently model a slipmat “looseness” with a tracking filter rather than raw position chasing. citeturn1search2turn2search7 Mixxx explicitly documents using an **alpha–beta filter** (a simple constant-velocity tracker related to a simplified Kalman approach) to tune responsiveness/looseness for scratching. citeturn1search2turn2search7 This is directly relevant: it’s cheap, stable, and robust to irregular update intervals.

Sound quality under time-warping is a separate knob from feel. When you time-warp aggressively, you need a resampling strategy. DJ software projects explicitly add higher-quality resamplers (e.g., sinc-based options) for scratching because fast-changing tempo/pitch stresses simpler interpolators. citeturn2search5turn0search4 The key is: treat that as an **optional quality tier** (context menu), not a requirement for great feel.

## Recommended scratch architecture changes

This section is written in “Codex-friendly” terms: concrete changes, minimal philosophy, and explicit tradeoffs. It assumes you keep your current ScratchModel split (Legacy vs Hybrid), but it intentionally tightens the gesture estimation pipeline feeding both.

### Make mouse-derived velocity frame-rate invariant

Your current platter drag computes lag deltas from `mouseDelta` (good—position is naturally frame-rate independent because deltas accumulate), but any *velocity* derived from raw per-frame `mouseDelta` without using dt will implicitly vary with frame rate. This is especially risky because Rack’s drag events are frame-based. citeturn0search1turn0search3

Recommendation: compute gesture velocity from the **actual lag delta** divided by an explicit dt measured with `rack::system::getTime()`. citeturn2search0

Implementation sketch (UI thread, `TemporalDeckPlatterWidget::updateScratchFromLocal`), conceptually:

```cpp
// State on widget:
double lastMoveTime = NAN;   // seconds from rack::system::getTime()
float  velLp = 0.f;          // optional smoothed velocity

void updateScratchFromLocal(Vec mouseDelta) {
    // ...existing tangentialPx, deltaAngle...

    float lagDeltaSamples = deltaAngle * samplesPerRadian; // same as today
    localLagSamples = clamp(localLagSamples - lagDeltaSamples, 0.f, accessibleLag);

    double now = rack::system::getTime();
    double dt = std::isnan(lastMoveTime) ? (1.0 / 60.0) : (now - lastMoveTime);
    lastMoveTime = now;

    // Clamp dt to avoid spikes when OS stalls or when first move happens.
    dt = clamp(dt, 1.0/240.0, 1.0/20.0);

    // Define "gesture velocity" in samples/sec; positive means towards NOW,
    // matching your sign conventions (because localLag -= lagDelta).
    float vSamplesPerSec = lagDeltaSamples / (float)dt;

    // Optional: light 1-pole smoothing to reduce hand jitter without adding "rubber band".
    float alpha = 1.f - std::exp(-2.f * float(M_PI) * 30.f * (float)dt); // ~30 Hz
    velLp += (vSamplesPerSec - velLp) * alpha;

    module->setPlatterScratch(true, localLagSamples, velLp);
}
```

Why this matters: it makes the **feel consistent** across 60/120/144 Hz and across Rack’s frame-rate limiter, because your velocity estimate is now grounded in time, not frames. citeturn0search1turn0search3turn2search0

### Replace fixed “motion fresh” with adaptive or decay-based bridging

Your code uses a fixed “motionFreshSamples = sampleRate * 0.02” (20 ms) to bridge between sparse UI drag updates. That is a reasonable defense against stepping when dragging at ~60 Hz, but it also risks a subtle problem: after fast motion stops, the engine may still treat you as “moving” for up to that window (or apply continuation), which can blur direction-change articulation. This is exactly the kind of blur turntablists notice. citeturn1search9turn0search1

Two viable improvements (they can be combined):

Adaptive window: set motionFreshSamples based on measured UI dt (the same dt you computed above):

```cpp
int motionFreshSamples = (int)std::round(module->uiSampleRate.load() * (float)(dt * 1.25));
motionFreshSamples = clamp(motionFreshSamples, 1, (int)(module->uiSampleRate.load() * 0.03f));
module->setPlatterMotionFreshSamples(motionFreshSamples);
```

Decay-based bridging (preferred for turntablism): keep a short window, but decay the last measured velocity towards 0 inside the audio engine when no fresh gesture arrives. This preserves continuity without enforcing a hard “constant velocity” tail. This approach is consistent with how “slipmat looseness” is modeled in practice: you track motion, but the system doesn’t keep pushing when there’s no evidence of continued hand energy. citeturn1search2turn2search7

### Use an alpha–beta tracker for manual scratch (cheap, robust, tunable)

This is the single highest-value “feel” recommendation from existing DJ software practice.

Mixxx documents its scratch control API as using an **alpha–beta filter** (position/velocity tracker) with coefficients that “affect responsiveness and looseness of the imaginary slip mat,” recommending starting values like alpha = 1/8 and beta = alpha/32. citeturn1search2turn2search7

You can apply the same concept to TemporalDeck manual scratching with very low CPU cost:

State (audio thread, per deck/module):
- `lagEst` (estimated scratch lag in samples)
- `lagVel` (estimated lag velocity in samples/sec, or read velocity—choose sign convention and stick to it)
- `tSinceUpdate` (time since last UI measurement)
- `lastRevision` (gesture revision counter)

On each audio sample:
- If a new UI measurement arrives (revision changed), update tracker with dt since last measurement.
- If no measurement arrives and platter is still touched, predict forward using current velocity and apply damping to velocity.

Alpha–beta update in lag-space:

```cpp
// Inputs: lagMeas, dtSec
// Predict
float lagPred = lagEst + (lagVel * dtSec);      // lagVel: samples/sec
float resid   = lagMeas - lagPred;

// Correct
lagEst = lagPred + alpha * resid;
lagVel = lagVel + (beta / dtSec) * resid;
```

Then clamp:
- `lagEst = clamp(lagEst, 0, limit)`
- (optional) clamp `lagVel` to avoid insane spikes.

Then set:
- `readHead = newestPos - lagEst`.

Two tuning notes for turntablism:
- Higher alpha makes the platter feel “tighter” (more immediate lock to hand).
- Higher beta makes velocity snap faster to the new inferred motion (helps chirps/scribbles feel crisp). citeturn1search2turn1search9

Where to integrate this:
- It can replace the Legacy manual chase + step-limits entirely (recommended).
- Or it can serve as the “manual measurement → target/velocity” estimator feeding your existing Hybrid integrator (less invasive).

This will also reduce the amount of hand-written “if slow then smooth, if fast then bite” code you need for basic stability, because the tracker naturally handles irregular update intervals.

### Add optional cursor lock for “infinite platter travel” while dragging

Aggressive scratches with a mouse can hit the mousepad/screen boundary quickly. Rack provides cursor lock/unlock on the window, and a user-facing setting governing whether cursor lock is allowed. citeturn1search4turn1search1

Recommendation:
- In `onDragStart`, if `rack::settings::allowCursorLock` is true, call `APP->window->cursorLock()`.
- In `onDragEnd` (or if drag aborts), call `APP->window->cursorUnlock()`.

This mirrors how Rack can lock the cursor while dragging knobs (the setting exists specifically for that behavior). citeturn1search1turn0search3

Make it a context menu toggle (“Cursor lock on platter drag”) that defaults to “follow Rack’s global allowCursorLock,” because some users (trackpad users especially) dislike cursor locking.

### Make “turntablist articulation” explicit at direction changes

Turntablist pedagogy emphasizes that direction changes create a tiny moment of stillness (“phantom click”) that helps make bursts discrete even with an open fader. citeturn1search9

You already have direction-flip detection and transient shaping in your scratch cleanup path. fileciteturn0file1 The improvement here is a small but meaningful rule:

When the estimated hand velocity crosses zero (sign flip or |v| near 0), apply a **micro-gate / micro-crossfade** window of ~1–3 ms to reduce discontinuity and emphasize articulation (not to “smooth it away,” but to make it *sound intentional*).

This should be *conditional*:
- Enable only in “Legacy” (or “Turntablist”) scratch feel mode.
- Scale amount by speed (fast scratches get more articulation, slow drags get less).

This is very cheap CPU-wise and materially improves that “chirp/scribble” readability.

## DSP quality and CPU strategy

You’ve already recognized the right product shape: a default mode that’s affordable, and a context-menu “high quality” mode that costs more. fileciteturn0file0

The DSP research takeaway from DJ software practice is:

Scratching is effectively short-duration and continually varying sample-rate conversion; a simple interpolator can be “fine,” but higher-quality resamplers materially improve the harshness/aliasing under extreme warp. citeturn2search5turn0search4

### Tiered scratch resampling options

Recommended context-menu options:

- **Fast**: Linear (only during scratch)  
- **Balanced**: Cubic (default scratch path unless user chooses otherwise)  
- **HQ**: 6-point Lagrange (your current “high quality scratch interpolation”) fileciteturn0file1  
- **Ultra (optional)**: Windowed-sinc / polyphase FIR, 3 quality levels (e.g., 8/16/32 taps)

The reason “Ultra” is justified: Mixxx specifically added a **sinc-based resampler with three quality settings** for scratching, after reports that linear scratch resampling sounded suboptimal. citeturn2search5turn0search4 That’s strong evidence that offering sinc as an opt-in “audiophile scratch” mode is a reasonable product decision.

### Dynamic activation to protect CPU

Even if the user enables HQ/Ultra scratch interpolation, only engage it when it matters:

- If `anyScratch` (manual or external) is active: allow HQ path.
- Additionally, gate by read-head speed/warp severity: if `abs(readDeltaForTone - 1.0) < eps` (close to normal playback), drop to cubic (or even linear) because the audible difference will be negligible but CPU cost persists.

This kind of gating is common-sense for Rack patches where multiple modules are running simultaneously and users expect CPU-aware behavior. Rack even provides per-module performance meters in the menu system, reflecting the culture of CPU mindfulness. citeturn0search3

## Codex-ready change list

This section is intentionally direct and implementation-scoped.

### UI layer changes

Modify `TemporalDeckPlatterWidget::updateScratchFromLocal`:

- Add widget fields:
  - `double lastMoveTimeSec`
  - `float velFiltered`
- Compute `dtSec` using `rack::system::getTime()` deltas. citeturn2search0turn2search10
- Compute `vSamplesPerSec = lagDeltaSamples / dtSec` (frame-rate invariant).
- Smooth velocity lightly (optional) with one-pole using dtSec.
- Set motionFreshSamples adaptively from dtSec (or reduce it and rely on engine damping).

Add optional cursor-lock:

- On drag start: if `rack::settings::allowCursorLock` and user enabled “Cursor lock on platter drag”, call `APP->window->cursorLock()`. citeturn1search1turn1search4
- On drag end/abort: unlock.

### Engine layer changes

For manualTouchScratch, especially Legacy mode:

- Implement an alpha–beta tracker operating on lag. Base parameter defaults on Mixxx’s documented starting suggestions (alpha ~0.125; beta ~alpha/32), then tune for your module. citeturn1search2turn2search7
- Replace the fixed “predict target drift” logic with:
  - if no new measurement: `lagVel *= exp(-dt * dampHz)` (fast decay, e.g., 20–40 Hz), and `lagEst += lagVel * dt`.
- Snap-to-hold:
  - if platter touched AND `abs(lagVel) < deadband` AND no new measurement in ~1 frame, set `lagVel = 0`.

Direction-change articulation:

- Detect sign flips in `lagVel` or inferred read velocity.
- Apply a short (1–3 ms) micro-crossfade/micro-gate envelope to mimic the “phantom click” articulation at reversals. citeturn1search9

### Optional HQ scratch resampler (context menu)

- Keep your existing interpolation options and toggles as “Fast / Balanced / HQ.” fileciteturn0file1
- Add “Ultra (sinc)” as a context menu option only if profiling shows acceptable CPU at typical patch scale.
- Provide 3 quality levels (tap counts) analogous to Mixxx’s “three quality settings” concept. citeturn2search5turn0search4

## Validation protocol oriented to turntablism

To evaluate “feel,” test with gesture patterns that reflect actual scratch technique families:

- Baby / scribble / tear: fast back-and-forth across a small displacement; reversals must sound discrete and the deck must not “coast wrong.” citeturn1search0turn1search9
- Slow drags: slow reverse/forward movements must not fight the user or creep, and must not glitch at low speed.
- Frame-rate sensitivity test: run Rack at different frame-rate limits and on different monitors; the mapping should be perceptually stable because DragMove is frame-based. citeturn0search1turn0search3

For sound quality validation:

- Stress test rapid speed changes (fast spin → stop → reverse) and compare Linear vs Cubic vs Lagrange6 vs optional Sinc.
- Confirm that HQ modes only engage under scratch/warp conditions to protect CPU.

All of the above stays consistent with TemporalDeck’s intended identity: a live-buffer deck whose platter interaction is a playable performance surface, not merely a scrub knob. fileciteturn0file0