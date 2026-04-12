# Leviathan Tape Deck Delay Research and Codex Implementation Spec

## Concept clarity and product framing

Dragon King Leviathan, the concept you described is clear enough to target a new module: a delay whose *primary mental model is a physical tape-echo transport*, with interaction primitives that feel like manipulating real parts (heads, tape path, motor speed, tape condition), and a second layer where the user can “redesign” the internals without leaving the panel. This matches VCV’s own panel guidance to “design panels as if you are designing hardware,” which is an unusually direct endorsement of your skeuomorphic-first instinct. citeturn13view0

What is *not yet fully specified* (and therefore what needs to be nailed down for a Codex-ready build) is not the “what,” but the *boundary between physical metaphor and modular utility*: i.e., which parts are purely visual affordances vs. which are true controllable signal-processing degrees of freedom, and how many of those degrees of freedom you want exposed as Rack parameters/CV vs. “bench-only” internal edits saved as custom state. VCV Rack’s API strongly supports both approaches (parameters/ports/lights for normal modulation, plus custom data serialization for extra state), but your scope decisions will determine how shippable the first version is. citeturn12view2turn1view1

There is also a clear market-positioning “gap” you can occupy *even though tape-y modules exist*: the library already includes (examples) **Ahornberg Tape Recorder** (a “micro cassette recorder”), **Ahornberg Tape Inspector** (visualizes audio on tape), **Path Set IceTray** (speed shifter + tape delay behavior with selective memory), and **AmbushedCat Tape Machine** (tape coloration including saturation, wow & flutter, bias shaping). These demonstrate demand for tape workflows and tape coloration, but none of their descriptions suggest a “hardware redesign bench” UI whose primary interaction is *physical layout editing of a tape machine* (your differentiator). citeturn11view0turn11view2turn11view3turn11view1

A practical framing that keeps the creative promise while making implementation tractable:

- **Core promise (MVP)**: “A tape deck delay where delay time is literally ‘head spacing ÷ tape speed,’ and you can *see* and *move* the heads.”  
- **Extended promise (vNext)**: “You can rewire the internal feedback/eq/saturation order, and swap ‘parts’ (tape type, bias, head condition), with the panel showing the resulting machine.”

This maps cleanly to known tape-echo physics (heads + transport) and to VCV’s customization/serialization facilities. citeturn4view2turn12view2

## What real tape delays do that users actually hear

Tape delay is not “a delay with noise”—it’s a specific physical method: an audio signal is recorded to magnetic tape via a **record head**, later read by one or more **playback heads**, and cleared by an **erase head** so the loop can repeat. Delay time is determined by the physical spacing between heads and the tape’s speed; multiple playback heads create multi-tap echoes, and feedback is created by routing some playback signal back into the record path so it’s re-recorded repeatedly. citeturn4view2turn4view1

Because the same signal is repeatedly re-recorded, tape echoes characteristically **lose high-frequency content and accumulate distortion** with each repeat; this is explicitly described in BOSS’s Space Echo discussion (each re-record “loses a little high end, gets more distorted, and accumulates more…degradation”). citeturn4view2

Delay-time control in tape systems historically comes from two physically different “families” (important for your UI):

- **Length-type behavior**: change the *distance* between write and read points (e.g., physically sliding a playback head).  
- **Speed-type behavior**: change the *speed* the medium traverses the heads (varispeed motor control).  

A major DAFx research paper emphasizes this distinction, noting that these two types produce meaningfully different pitch-change behaviors when delay time is manipulated—one reason tape-delay “scrub” feels the way it does under feedback. citeturn6view1

Your “reposition heads physically” instinct is therefore not only intuitive—it corresponds to a documented, musically salient class of tape-delay behavior.

The mechanical reality also suggests excellent UI metaphors. For example, the **Roland RE-201/101 service notes** explicitly depict a head block with erase + record + multiple playback heads and provide mechanical alignment diagrams—useful references for how a “real” head cluster is laid out and how a tape path is physically constrained. citeturn9view4turn9view1

Wow & flutter, likewise, is not a single knob in the real world: it’s speed instability from transport mechanics and media wear. BOSS describes causes such as pinch-roller deformation and friction/dirty heads contributing to uneven tape speed and modulation. citeturn4view2  
In measurement/standards terms, “wow” is commonly associated with slower speed variation and “flutter” with faster components; some references use a ~4 Hz boundary, while academic treatments describe perceptually relevant modulation ranges (e.g., wow on the order of ~0.5–6 Hz and flutter up to ~100 Hz). citeturn5search0turn5search22

Finally, if you want the “redesign the hardware” layer to feel *real* rather than arbitrary, magnetic tape bias is a strong candidate knob: classic tape engineering references explain that **too much bias reduces high-frequency response** while **too little bias increases distortion** (among other effects), and that changing bias shifts the HF response tradeoffs. citeturn6view2turn6view3

image_group{"layout":"carousel","aspect_ratio":"16:9","query":["tape echo mechanism record playback erase heads diagram","Roland RE-201 Space Echo inside tape path","tape delay playback heads close up","Echoplex tape echo sliding head mechanism"],"num_per_query":1}

## VCV Rack implementation constraints and affordances that shape the design

A Rack module is built around the canonical components: **params, inputs, outputs, lights**, with DSP running in `process()` every audio frame, and UI provided by a `ModuleWidget` plus any custom widgets you add. The official tutorial’s “DSP kernel” section is the cleanest grounding: you read params/inputs and write outputs/lights in `process()`, with `process()` being called at audio rate. citeturn1view1

Signal levels are standardized: oscillators/audio signals are typically **±5 V** (10 Vpp), and many CV conventions are 0–10 V unipolar or ±5 V bipolar. This matters for saturation thresholds, meters, and for CV scaling decisions. citeturn15search0

For panels, VCV’s workflow and limitations directly influence a skeuomorphic, animated design:

- Panels are made in **Inkscape** and must use **mm** units; width is in multiples of **5.08 mm per HP**, and the guide explicitly recommends designing as hardware with human spacing. citeturn13view0  
- Rack’s SVG rendering has limitations (e.g., text must be converted to paths), so “fine print” engineering labels and etched calibration marks should be designed with that pipeline in mind. citeturn13view0  
- `helper.py createmodule` can generate C++ scaffolding from the SVG components layer, which is still a helpful bootstrap even if you later hand-author complex widgets. citeturn1view1turn13view0turn3search21

For animation-heavy “tape parts moving” UI, the Plugin API Guide is crucial:

- Rack draws widgets every screen frame; complex vector drawing can become expensive, so caching static artwork via `FramebufferWidget` is recommended, marking it dirty only when needed. citeturn2view1  
- Rack supports a **self-illuminating draw layer** (layer 1) that remains visible when room brightness is turned down—perfect for glow traces, VU illumination, head “LEDs,” and moving tape highlights. citeturn2view1  
- Custom widgets can use `Widget::step()`, `draw()`, and `drawLayer()`, and can implement drag events (`onDragStart`, `onDragMove`, etc.)—exactly what you need for draggable head widgets. citeturn14view0turn2view1

Polyphony is another design axis you should decide up front. Rack supports polyphonic cables up to **16 channels**, and modules must explicitly implement per-channel processing to support it; the manual and API guide both show the expected pattern (loop channels, use `getPolyVoltage`, set output channels). citeturn13view1turn2view0

Finally, state persistence is directly relevant to your “bench edits” concept. Rack automatically saves parameter values, but any additional state (e.g., custom head positions, wiring topology, selected tape formulation) must be serialized via `dataToJson()`/`dataFromJson()`. For large data (>~100 kB), Rack provides per-module patch storage directories, with warnings about not blocking the audio thread by doing file I/O inside `process()`. citeturn12view2

## UI/UX blueprint for a physical-first tape deck module

A workable UI architecture is to treat the panel as two layers of truth:

**Performance layer (always visible, always modulatable)**  
This is the “musician-facing” surface: time/speed, feedback, mix, tone, wow/flutter amount, drive, plus the patch jacks.

**Bench layer (user can toggle; saved as custom state)**  
This is the “designer-facing” surface: head positions, tape path geometry constraints, bias, tape type, head wear/alignment, internal feedback routing options.

This split matches VCV Rack’s affordances: panel controls are natural params/ports; bench edits are naturally stored via JSON serialization and surfaced via custom widgets and/or context menus. citeturn12view2turn12view3

A recommended panel layout (intended for a first implementation that still feels lavish):

- **Top half**: animated tape transport viewport  
  - Tape loop path (moving texture / scrolling tick marks)  
  - Record head (fixed), N playback heads (draggable in bench mode), erase head (optional visual)  
  - A “tape bin” or “spool” visualization whose motion communicates tape speed  
  - A VU meter or level lamp that responds to record level  

- **Bottom half**: “digital patch bay” + “quick controls”  
  - Audio in/out (mono + optional stereo normalization)  
  - CV inputs for the essential performance parameters  
  - A small, high-contrast cluster of knobs/switches for stage use  
  - A bench toggle button (or a context-menu option if you want a cleaner face)  

Design fundamentals to keep it actually usable:

- Text and labels must remain readable at 100% scale; VCV explicitly recommends matching the density/text sizes of stock modules and spacing controls so thumbs have room—critical if you’re drawing lots of small “hardware parts.” citeturn13view0  
- Use the self-illuminating layer for the parts you want to remain visible in dark-room mode (e.g., tape position indicator and head-read LEDs). citeturn2view1  
- Use framebuffer caching so the “panel art” and non-animated parts don’t get re-rasterized every frame. citeturn2view1  
- Implement head dragging through widget drag events; the Widget API provides the hooks you need. citeturn14view0

One important practical caution: VCV’s panel guide explicitly warns against using other people’s IP without permission. If your UI is “based on” recognizable hardware, avoid copying logos, exact faceplate layouts, trademarks, or trade dress. You can still evoke the *mechanics* (heads, tape path, mode selector logic) with an original visual language consistent with your Leviathan identity. citeturn13view0

## DSP mapping from physical metaphor to controllable sound

A tape-deck delay module lives or dies by how it behaves when time is changed, especially under feedback. The DAFx-18 paper is directly “load-bearing” here: it distinguishes **length-type** delays (move read head) from **speed-type** delays (change medium speed), and explains why speed-type behavior gives more consistent pitch control when manipulating delay time, while typical digital variable-read delays are length-type in spirit. citeturn6view1

That paper effectively validates a two-control schema you can embody visually:

- **Move head** (bench): changes head spacing → length-type behavior (classic “scrub”)  
- **Varispeed** (performance + CV): changes tape speed → speed-type behavior (classic motor ramp “spiral”)

You can ship an MVP that is musically convincing without solving every tape-physics nuance if you implement the following “character primitives,” each supported by reputable descriptions of tape echo behavior:

**Repeat darkening + cumulative distortion**  
Tape echo repeats lose high end and accumulate distortion because they are re-recorded; BOSS explicitly describes this accumulation, and it’s a signature trait. citeturn4view2

**Saturation/compression from magnetic limits**  
Tape saturates because the medium can only store so much magnetic charge; high recording levels squash peaks and add harmonics. citeturn4view2

**Wow & flutter from transport instability + wear**  
Transport components (pinch roller dents, friction, wear) cause speed modulation, and standards literature characterizes the wow/flutter modulation ranges that are perceptually meaningful. citeturn4view2turn5search0turn5search22

**Bias as a “hardware redesign” parameter with real sonic consequences**  
Engineering notes describe bias tradeoffs: too much bias reduces HF response; too little increases distortion, and bias changes shift the response. This makes “BIAS” a perfect bench trimmer because it’s both authentic and musically legible. citeturn6view2turn6view3

**Avoiding harsh artifacts during modulation**  
Rack users explicitly discuss the desire for “wow/flutter-like” time modulation without harsh breakup, and developers discuss anti-click strategies such as short crossfades. These are cautionary signals: your module should prioritize smoothing, interpolation quality, and/or crossfade techniques when delay time is modulated or head positions jump. citeturn0search1turn0search20

On implementation mechanics inside Rack, you have a few tool-level options:

- Rack provides a `dsp::SampleRateConverter` with a tunable quality setting (0–10), which can be used for resampling-style behaviors if you choose to structure varispeed as sample-rate conversion rather than pure variable-delay reads. citeturn15search9  
- Rack parameters can enable per-sample smoothing via `ParamQuantity::smoothEnabled`, which can help for knob-driven changes, though CV-rate modulation still requires DSP-level care. citeturn15search22

For CV scaling, you can either define your own standard or align with user expectations. VCV’s own Delay module uses 1V/oct scaling for time CV (each additional volt halves time) when the attenuator is at 100%, and documents the behavior clearly; copying this convention (or offering it as a mode) makes your module immediately “Rack-native.” citeturn1view2

## Codex implementation spec in Markdown

This section is written as a build-target spec for a Rack v2 plugin module. All names are suggestions; adjust to match your Leviathan plugin’s established naming scheme.

### Module overview

**Module name**: Leviathan Tape Deck Delay  
**Primary tags**: Delay, Visual, (Polyphonic if implemented)  
**Core user story**: “Patch audio in, get tape-style echoes out. Change time by moving a head or changing tape speed. See the mechanism. Modulate it like an instrument.”

**Non-goals for MVP**  
- Not a full tape recorder/sampler (avoid scope collision with recorder modules already in the ecosystem). citeturn11view0turn11view2  
- Not a perfect RE-201 clone UI (avoid IP/trade dress issues). citeturn13view0

### Panel and UI construction requirements

**Panel workflow**  
- Panel drawn in Inkscape using mm units; height 128.5 mm; width is an integer HP multiple (5.08 mm/HP). citeturn13view0  
- SVG `components` layer used for params/ports/lights/custom widget placeholders; `helper.py createmodule` used to scaffold `src/LeviathanTapeDeckDelay.cpp` from `res/LeviathanTapeDeckDelay.svg`. citeturn13view0turn1view1turn3search21  
- SVG text converted to paths. citeturn13view0

**Custom widgets**  
- `TapeTransportWidget` is a custom `Widget` responsible for:
  - Animated tape motion (based on tape speed) using `step()` and `draw()`/`drawLayer()`. citeturn14view0turn2view1  
  - Self-illuminating highlights (head-read LEDs, tape position marker) drawn in layer 1. citeturn2view1  
  - Draggable `HeadWidget` children implementing `onDragStart/onDragMove/onDragEnd` for bench-mode head positioning. citeturn14view0  
- Static background art (panel plate, printed labels, screws) placed under a `FramebufferWidget` to reduce redraw cost; mark dirty only if the bench overlay toggles. citeturn2view1

**Bench mode**  
- Bench mode can be toggled by:
  - A front-panel button **and** mirrored in the module context menu (for accessibility / automation of clean panels). citeturn12view3  
- Bench mode reveals:
  - Head position handles  
  - Bias trimmer  
  - Tape type selector  
  - A minimal “internal routing” selector (MVP: choose pre/post filtering in feedback loop)

### DSP and signal flow

**Signal flow (conceptual)**  
Input → Record preamp (drive) → Tape record nonlinearity → Tape medium (delay line) → Playback head taps (multi-tap mixer) → Post playback coloration (age/rolloff/noise) → Wet output  
Feedback: selectable tap point → feedback tone/filters → feedback gain → summed back into record preamp.

Tape echoes are created by feeding playback output back to the input/record path; each re-record degrades (HF loss, distortion), matching real tape echo descriptions. citeturn4view2turn4view1

**Delay time model**  
- Delay time per head: `t_delay = distance(record_head, playback_head) / tape_speed`. This is the canonical tape-echo relationship. citeturn4view2turn4view1turn4view0  
- Provide two “time manipulation” modalities:
  - **Head move**: changes distance (length-type)  
  - **Varispeed**: changes tape_speed (speed-type)  

The distinction is musically meaningful and documented; implement both, even if the MVP approximates speed-type behavior with a simpler method initially. citeturn6view1turn4view2

**Character model (MVP)**  
- **Drive / Saturation**: soft clip or waveshaper representing magnetic saturation. citeturn4view2  
- **Repeat rolloff**: lowpass (and optionally slight high-shelf loss) applied in the feedback path so each repeat gets darker. citeturn4view2turn4view0  
- **Wow**: low-frequency speed modulation (slow). citeturn5search0turn5search22  
- **Flutter**: higher-frequency, lower-depth modulation component (faster). citeturn5search0turn5search22  
- **Tape age**: increases noise, increases rolloff, increases modulation depth (and optionally dropout probability), consistent with the “wear” framing of tape loop devices. citeturn4view2  
- **Bias (bench)**: adjusts a paired response:
  - Underbias: brighter but more distortion  
  - Overbias: duller but smoother/less distortion  
  This directionality is explicitly stated in tape engineering notes. citeturn6view2turn6view3

**Anti-artifact requirements**  
- Smooth all time changes (speed and head moves) with a minimum-slew and/or short crossfade for discontinuous jumps. The desire for non-harsh wow/flutter modulation and developer discussion of anti-click crossfades justify prioritizing this. citeturn0search1turn0search20

### Parameters, ports, and displays

To minimize panel clutter while keeping the “physical first” promise, this spec defines a compact, performance-oriented set of params/CV, and places deeper edits in bench mode.

**Performance params (front panel, modulatable)**  
- **SPEED** (tape speed / varispeed): continuous  
- **FDBK** (feedback gain): 0…>1 (allow controlled runaway with limiter/clamp)  
- **MIX** (dry/wet for MIX output)  
- **TONE** (tilt filter or DJ-style filter on wet/feedback path)  
- **WOW/FLUTTER AMT** (macro; bench splits into separate depths)  
- **DRIVE** (record preamp level into saturation)

**Bench params (front panel but visually “internal,” modulatable optional)**  
- **BIAS** (trimmer)  
- **TAPE TYPE** (switch: e.g., Clean / Standard / Hot)  
- **HEAD COUNT / MODE** (switches whether playback head 2/3 are active, or selectable “mode” patterns)

**Audio and CV ports (front panel)**  
- Audio in: IN L, IN R (R normalled from L if unpatched)  
- Audio out: OUT L, OUT R  
- Optional: HEAD 1/2/3 outs (provides multi-tap patchability; can be omitted in MVP if panel space is tight)  
- CV inputs (MVP): SPEED CV, FDBK CV, MIX CV, WOW/FLUTTER CV, DRIVE CV  
- Gate inputs (optional but powerful): FREEZE (holds tape / stops erase), RESET (re-centers tape position / clears buffer)

Voltage conventions should follow Rack standards (audio typically ±5 V; CV commonly 0–10 V or ±5 V). citeturn15search0

**Displays/lights (visual feedback)**  
- VU meter (record level)  
- Tape motion indicator (position + speed)  
- Head read LEDs (blink/brighten when their tap is active)  
- Optional dropout indicator when tape age produces dropouts

Use the self-illuminating layer for these so they remain visible in dark-room mode. citeturn2view1

### Polyphony behavior

**Option A (recommended for MVP)**: polyphonic-by-channel, shared transport modulation  
- For a polyphonic input of N channels, maintain N independent delay buffers (or N delay taps) but use the same wow/flutter/speed modulation signals across all channels so it feels like a single motor driving multiple tracks. Rack supports up to 16 channels; implement channel loops accordingly. citeturn13view1turn2view0

**Option B (alternate via context menu)**: sum-to-mono into one tape path, then output mono/stereo  
- This mimics a single tape machine more literally, but is less “Rack modular.” If offered, make it a context-menu mode. Context menus are explicitly supported for non-panel settings. citeturn12view3

### State serialization

Because bench edits are not all simple params, store them explicitly:

- `dataToJson()/dataFromJson()` stores:
  - Bench mode toggle state  
  - Head positions (normalized 0–1 along allowed rail)  
  - Tape type selection  
  - Bias value  
  - Any internal routing choice  

Rack requires custom serialization for non-parameter state and provides the exact API for it. citeturn12view2

If you later add very long “tape” memory or recording-like features that exceed typical JSON sizes, migrate large buffers to patch storage per the guide (and do not read/write files inside `process()`). citeturn12view2

### Bypass behavior

Implement bypass routes using `configBypass(inputId, outputId)` so the module can be bypassed cleanly (dry routing), consistent with Rack’s bypass conventions and API. citeturn15search2turn12view3

### Acceptance criteria

A Codex implementation should be considered “MVP complete” when all items below pass:

- **Physical time correctness**: moving a playback head farther from the record head increases delay time; increasing SPEED decreases delay time, consistent with “spacing ÷ speed.” citeturn4view2turn4view0  
- **Tape-like time modulation behavior**: changing SPEED while feedback is active produces a pitch-ramping echo behavior characteristic of tape-style modulation (no zippering; no explosive digital crackle under moderate modulation). citeturn6view1turn0search1  
- **Repeat character**: with feedback >0, repeats progressively darken and become more distorted than the initial echo (user can reduce this with controls). citeturn4view2  
- **Wow/flutter plausibility**: wow produces slow wobble; flutter produces faster shimmer; combined macro control is musical across typical ranges. citeturn5search0turn5search22  
- **Visual coherence**: tape animation speed visibly corresponds to SPEED; head positions visibly correspond to resulting delay time; self-illuminating indicators remain visible in low room brightness. citeturn2view1turn14view0  
- **Rack correctness**: meets voltage standards (no unexpected clipping at nominal levels), supports polyphony per chosen option, and bench state persists via JSON on save/load. citeturn15search0turn13view1turn12view2