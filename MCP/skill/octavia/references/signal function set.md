# Signal Function Set Module Reference for Octavia Agents

This is the source-backed module reference for the **Signal Function Set** VCV Rack plugin. Use it to choose appropriate Signal Function Set modules, understand the plugin's internal musical ecosystems, preserve important module state, and recognize expander relationships before editing a patch.

The production plugin slug is `SignalFunctionSet`. The current source examined for this reference reports version `2.18.1`. Development builds may intentionally alter plugin metadata so they can coexist with release builds, so the **live Rack library remains authoritative**.

Exact parameter IDs, input/output IDs, module IDs, cable IDs, and runtime values should be discovered through Octavia rather than hard-coded here.

> **Important:** model slug casing is significant. The GSX model slug is `gsx`, not `GSX`.

## Selection map

| Model slug | Role | Use it when | Agent-relevant behavior |
|---|---|---|---|
| `Operator` | Polyphonic six-operator FM voice | You want DX7-style FM synthesis, cartridge voices, or faithful operator-envelope behavior | Loads DX7 `.syx` banks and preserves bank/voice/operator-mute state. It is a stateful instrument, not merely a generic FM oscillator. |
| `gsx` | Granular synthesis oscillator | You want pitched grains, clouds, stereo spread, or Truax-style asynchronous granular textures | Variation moves behavior from more synchronous/pitched toward increasingly asynchronous grain clouds. Dense or stochastic texture is expected behavior. |
| `Overtone` | Additive harmonic oscillator | You want a fundamental plus selectable upper harmonics or CV-controlled harmonic masks | The fundamental remains present even when harmonic toggles are off. Mask behavior can change through module options, so inspect state before assuming how a mask CV is interpreted. |
| `Intone` | CHANT/FOF formant voice and processor | You want vocal/formant synthesis, a formant filter bank, or triggered FOF grains | Excitation patching and operating mode change the role of the module and the meaning of pitch control. Inspect the excitation input and module state before diagnosing pitch or silence. |
| `Phase` | Dual sample looper / phase-music engine | You want two sample loops that drift, rotate, reverse, cue from transients, or record live | Loaded samples, loop regions, transient/cue data, speed, and recording state are musically significant persistent state. A single loaded sample may intentionally feed both sides. |
| `Play` | Polyphonic multisample instrument | You want SFZ/DecentSampler-style playback, round robins, loops, or a sampled instrument created with `Record` | Instrument-file selection is persistent state. Velocity may be broadcast across polyphonic voices. Treat loaded instrument data as part of the patch. |
| `Loom` | Eight-string coupled physical-model instrument | You want strummed, plucked, bowed, hammered, or wind-excited string behavior with shared coupling | The visual/string state, tunings, exciter choices, and coupling are part of the instrument. It can be played from CV/gates, automatic patterns, or direct interaction. |
| `Slide` | Eight-string lap-steel physical model | You want lap-steel gestures, shared-bar glides, slant, rolls, swell, and vibrato | Shared bar position, slant, tunings, pickup behavior, and interaction state are musically important. Do not reduce it conceptually to eight ordinary oscillators. |
| `SlideX` | `Slide` expander | You need individual per-string outputs, gates, velocity, or level control from `Slide` | Place immediately to the right of its `Slide` parent and preserve adjacency when moving modules. |
| `Chime` | Eight-note drone / physical chime instrument | You want slowly evolving struck/bowed chimes, semi-free note motion, or per-tube modulation | Per-note motion intentionally drifts and blooms rather than behaving like a conventional clocked sequence. Provides both musical audio and modulation derived from the eight tubes. |
| `Band` | Four-band harmonic filter bank | You want filters locked to integer harmonics of an incoming or supplied fundamental | Bands are harmonic-relative rather than simply fixed-frequency. The module can estimate source pitch or follow explicit pitch CV, so inspect which authority is active. |
| `Tine` | Pingable resonator | You want metallic/percussive resonances ranging from short decay to near self-oscillation | It normally needs a ping/excitation unless damping is set into sustained behavior. Silence without excitation can be completely normal. |
| `Slice` | Seeded stereo grid-slice effect | You want rhythmic, probabilistic-looking but repeatable transformations at slice boundaries | Not every slice is transformed. The random-looking sequence is seeded and repeatable; unchanged slices and references to earlier slices are intentional behavior, not intermittent failure. |
| `Crystal` | Geometry-based quad echo chamber | You want spatial multi-tap echoes, crystal-material coloration, or four-listener output | Geometry, crystal habit/material, emitter/listener placement, and chamber state are central. Visual camera rotation is not necessarily an acoustic rotation. Treat it as a quad spatial processor, not merely stereo delay. |
| `Arrange` | Song-form arranger / macro sequencer | You want phrase-level bars, key, tempo, and multiple instrument clock buses | Intended to sit above lower-level sequencers. It can coordinate phrase gates, root/scale, BPM, clock/bar/reset/EOC behavior, and per-instrument enable state. |
| `Meter` | Time-signature-aware master clock | You want musical bar timing, swung subdivisions, external PPQN sync, or changing meters | External clocking can become timing authority over BPM. Meter changes may be intentionally queued to the next bar rather than applied immediately. |
| `MeterX` | `Meter` expander | You want 24 PPQN, run state, bar triggers, and longer-form 2/4/8/16/32/64/128-bar events | Place immediately to the right of `Meter`. Parent communication does not require patch cables. Its 24 PPQN output is intentionally unswung. |
| `Beat` | Single-voice drum pattern sequencer | You want one programmed percussion voice with velocity, accent, probability, and multiple patterns | One `Beat` normally represents one drum/instrument voice, not an entire kit. Pattern-grid state is part of the composition and must be preserved. |
| `Fill` | Autonomous eight-channel drum sequencer | You want a full percussion generator with pattern sets, evolving fills, pressure, and per-channel outputs | Requires a clock to advance; stopping the clock freezes its progression. Pattern pressure/fill behavior is intentional stateful automation, not an ordinary random trigger source. |
| `Note` | Monophonic pitched pattern sequencer | You want a programmed melodic line using Signal Function Set's shared root/scale convention | Conceptually the pitched relative of `Beat`: pattern-grid state, probability, velocity, accent, root, and scale matter. Do not assume its pitch output is a generic polyphonic chord source. |
| `Chance` | Seeded generative melodic sequencer | You want repeatable probabilistic walks, rests, holds, leaps, ratchets, and a related second voice | Requires an external clock. Seed/pattern state makes the apparent randomness repeatable, so preserve it before broad edits. |
| `Fugue` | Three-voice counterpoint sequencer | You want several independently moving voices derived from one shared pitch sequence | The three voices share source material but can move with independent clocks and harmonic behavior. Clock normaling can create useful hierarchy or polyrhythm. |
| `FugueX` | `Fugue` expander | You want per-voice range/step/sleep/probability controls, sample-and-hold, per-step triggers, or parent randomization | Place immediately to the right of `Fugue`. Preserve adjacency and inspect the parent before interpreting expander values. |
| `MetaFugue` | Combined `Fugue` + `FugueX` | You want Fugue functionality without a separate expander pair | Prefer this when a single-panel solution is desired. Do not add `FugueX` expecting it to expand `MetaFugue`. |
| `Muse` | Triadex Muse-style deterministic melody generator | You want long deterministic melodies produced by counter taps and shift-register logic rather than step programming | Has no internal clock. Slider/tap configuration is effectively the composition. A second `Muse` can participate in a right-side chained relationship when configured for linking. |
| `Drift` | Four-channel phase-related LFO / chaos modulator | You want free-running or clock-synced related modulation with controllable waveform, spread, and stability | Higher Stability means more stable behavior. A patched clock can become the timing authority over free frequency. |
| `Cycle` | Four-channel bar-synchronous LFO | You want modulation locked to musical bars and subdivisions/multipliers | Control semantics change when Bar and Clock are patched. A Bar input hard-aligns musical phase; Clock can quantize stepped/random behavior. |
| `Gravity` | Multi-mode physics/chaos modulation engine | You want modulation derived from simulated motion rather than ordinary LFO shapes | Mode changes the meaning of the physics controls. X/Y/radius/angle, sector CVs, and boundary events derive from the simulated point; visual state is part of understanding the result. |
| `Swell` | Ping-accumulating envelope | You want successive triggers to build or stack amplitude rather than simply restart a normal AD/AR shape | Each ping adds another contribution with rise and decay. Multiple pings intentionally accumulate; this is not conventional ADSR retrigger behavior. |
| `Vac` | Semi-stable AR envelope | You want an AR envelope whose timing can drift from cycle to cycle | Stage Stability controls variation. Low stability can intentionally produce different timings each cycle; loop and end behavior should be inspected before treating variation as error. |
| `OpEnv` | Standalone DX7 operator envelope | You want the multi-stage envelope from a DX7 voice, including rate/level structure and DX7-style modulation | Loads envelope data from `.syx` voices and preserves bank/voice state. It is not a generic ADSR. Release behavior may depend on module options. |
| `Key` | Polyphonic quantizer and shared scale-bus authority | You want central root/scale routing, polyphonic quantization, subscales, Scala tuning, or non-octave periods | Can carry richer microtonal scale information than the canonical scale-index convention. Treat loaded Scala data and scale-bus state as persistent musical state. |
| `Shift` | Four-output CV shift register / history processor | You want delayed CV history, cascading or parallel shift behavior, dividers, or held/jumbled history | Internal history remains musically meaningful. An unpatched input can still expose previously captured history depending on state; Reset clears that state. |
| `Record` | Automatic multisample recorder / SFZ builder | You want to sample an external or Rack voice across notes/velocities and generate a playable sampled instrument | This module performs a workflow with persistent files and external side effects. Do not start captures, overwrite destinations, or otherwise mutate sample libraries casually. Its natural downstream companion is `Play`. |

## Registered hidden models

These models are registered by the source and described in the plugin manifest, but the current manifest marks them hidden. Agents should be able to **recognize and inspect them in existing patches**, but should not normally recommend or add them as public-facing choices unless the live Rack library exposes them and the user's intent specifically calls for them.

| Model slug | Hidden role | Agent handling |
|---|---|---|
| `Wave` | Parametric wavetable voice with macro/snapshot behavior | Preserve if encountered. Treat snapshot/morph state as significant; do not substitute for a public wavetable module without user intent. |
| `Ratio` | Quad ratio/VCA-style utility | Preserve and inspect live semantics if present. Do not assume it is part of the normal public workflow. |
| `OpMorph` | Hidden `Operator` expander / DX7-routing morph utility | If encountered beside `Operator`, preserve the relationship and inspect both modules before changes. Do not normally add it just because it is registered. |
| `Kit` | Struck-membrane percussion model | Preserve if encountered. Treat its physical-model voice type as patch intent rather than replacing it with `Beat` or `Fill`, which are sequencers. |
| `Trace` | Four-channel drawn/paper-loop CV source | Preserve drawing/loop state. It may behave as controller, LFO, sequencer, or sample-and-hold depending on configuration. |

## Relationships and placement

- **`Meter` → `MeterX`**: `MeterX` is a true expander and belongs immediately to the right of `Meter`. Communication is through Rack expander messaging, not ordinary patch cables.
- **`Fugue` → `FugueX`**: `FugueX` expands a `Fugue` placed immediately to its left. `MetaFugue` is the merged alternative and does not need `FugueX`.
- **`Slide` → `SlideX`**: `SlideX` expands `Slide` with per-string I/O and belongs directly to the right of the parent.
- **`Muse` → `Muse`**: two Muse modules can form a right-side linked relationship when that mode is enabled. Do not assume two adjacent instances are independent without inspecting their state.
- **`Operator` ↔ `OpEnv`**: these are not an expander pair, but both understand DX7 cartridge/voice concepts. They may intentionally be configured from related `.syx` material.
- **`Operator` → `OpMorph`**: `OpMorph` is a registered hidden expander for `Operator`. Preserve the relationship if an existing patch contains it; do not make it a default recommendation.
- **`Record` → `Play`**: this is a workflow relationship rather than an expander relationship. `Record` can create multisample material that `Play` subsequently performs.
- **Timing ecosystem**: `Meter` supplies musical clock/bar structure; `Arrange` coordinates higher-level phrase, meter, BPM, root/scale, and instrument buses; `Beat`, `Note`, `Chance`, `Fugue`, `Fill`, and `Muse` consume clocks in different ways; `Cycle` can lock modulation to the same bar/clock grid.
- **Scale ecosystem**: `Key` is the strongest central quantization/scale-bus utility. `Arrange`, `Note`, `Chance`, `Fugue`, `Muse`, `Chime`, `Loom`, and `Slide` participate in Signal Function Set's shared root/scale vocabulary to varying degrees.
- **Physical-instrument family**: `Loom`, `Slide`, and `Chime` combine sound generation with visual/playable state. Their interaction state can be just as important as cable topology.

When moving a parent or expander, re-check adjacency and linkage afterward. A correct cable graph does not prove a correct expander graph.

## Shared musical conventions

### Canonical root and scale

Signal Function Set reuses a canonical root/scale convention across many musical modules. This allows an arrangement or scale authority to coordinate multiple sequencers and instruments without reproducing the same musical settings everywhere.

`Key` extends this idea further: its scale output can carry a richer polyphonic representation for Scala and non-octave scales. A normal module reading a single scale voltage may only consume the canonical channel, while another `Key` can preserve the richer scale definition.

Therefore:

- Do not assume every `SCALE` connection is just a generic continuous CV.
- Do not replace a shared scale-bus cable with a manually chosen local scale unless that change is intentional.
- Do not assume 12-TET when `Key` is using Scala or a non-octave period.
- When debugging “wrong notes,” inspect root, scale, scale-bus topology, and loaded Scala state before changing pitch sources.

### Clocks, bars, and timing authority

Several Signal Function Set controls change meaning depending on whether a timing input is connected.

Examples include:

- `Meter`: external clocking can override its free BPM authority.
- `Cycle`: a Bar input turns its frequency control into a musically related multiplier/divider regime; Clock can further quantize certain shapes.
- `Chance` and `Muse`: no external clock means no advancing melody.
- `Fill`: stopping the clock intentionally stops progression.
- `Beat` and `Note`: Bar/Reset relationships are part of pattern synchronization, not redundant clock inputs.

Before modifying a timing parameter, trace the incoming timing cables and determine which module currently owns musical time.

## Choosing among overlapping modules

### For sound generation

- Choose **`Operator`** for structured six-operator FM and DX7 voice compatibility.
- Choose **`gsx`** for granular material that can range from tonal grains to clouds.
- Choose **`Overtone`** for explicit additive harmonic construction.
- Choose **`Intone`** for vocal/formant synthesis or FOF-style excitation.
- Choose **`Phase`** when recorded/sample material and loop drift are the source.
- Choose **`Play`** for a conventional multisample instrument.
- Choose **`Loom`**, **`Slide`**, or **`Chime`** when physical-model behavior and interactive state are part of the musical idea.

### For sequence generation

- Choose **`Beat`** for one explicitly programmed percussion voice.
- Choose **`Fill`** for autonomous multi-channel drum performance.
- Choose **`Note`** for an explicitly programmed monophonic melody.
- Choose **`Chance`** for seeded, repeatable generative melodic movement.
- Choose **`Fugue`** for three related counterpoint voices.
- Choose **`MetaFugue`** when you want Fugue plus expander capability in one panel.
- Choose **`Muse`** for deterministic algorithmic melody derived from Triadex-style logic.
- Choose **`Arrange`** above these when the task is song form, phrase changes, key/tempo coordination, or multiple instrument clock buses rather than note generation itself.

### For modulation

- Choose **`Drift`** for direct LFO/chaos modulation that can be free-running or clock-related.
- Choose **`Cycle`** when bar-synchronous musical modulation is the priority.
- Choose **`Gravity`** when the desired CV should emerge from a physical/chaotic simulation rather than a conventional waveform.

### For envelopes

- Choose **`Swell`** for additive ping accumulation and blooming amplitude.
- Choose **`Vac`** for a conventional-ish AR shape whose timing can intentionally wander.
- Choose **`OpEnv`** for DX7-style multi-stage operator envelopes.

### For resonant/effect processing

- Choose **`Band`** to isolate or reshape harmonics relative to a fundamental.
- Choose **`Tine`** for pinged metallic resonance.
- Choose **`Slice`** for seeded grid-synchronous slicing/transformation.
- Choose **`Crystal`** for spatial, geometry-derived quad echoes.

### For pitch and scale control

- Choose **`Key`** when the patch needs central quantization, shared root/scale, subscales, Scala tuning, or non-octave periods.
- Choose **`Arrange`** when key/scale changes belong to song sections and need to move with phrase structure.
- Do not treat those as interchangeable: `Key` is a quantization/scale authority; `Arrange` is a macro-form authority.

## Inspection and editing rules

1. **Discover the live library first.** For the release build, begin with `vcv_list_library(plugin="SignalFunctionSet")`. If a development build has altered plugin metadata, use a broader live-library query rather than guessing. Live Rack discovery outranks this document.

2. **Preserve exact model slugs.** In particular, use `gsx` with its actual lowercase slug. Do not normalize names from panel labels.

3. **Identify instances by Rack module ID, not by model name alone.** Multiple instances of `Beat`, `Key`, `Fugue`, etc. are common. Use the live module list, then inspect the exact target with `vcv_get_module`.

4. **Discover exact parameter and port IDs live.** This reference intentionally describes semantic roles rather than freezing implementation IDs that can change between plugin builds.

5. **Trace cables before changing controls whose meaning depends on patching.** This is especially important for `Intone`, `Meter`, `Cycle`, `Band`, sequencing clocks/bars/resets, and shared root/scale buses.

6. **Treat non-knob state as first-class patch state.** Sample files, DX7 cartridges and voice selection, pattern grids, seeds, tunings, Scala scales, string/bar geometry, simulation state, loop/cue regions, drawn traces, and shift-register history can all carry the musical intent. Inspect module state before broad edits and preserve a state snapshot when the change is risky.

7. **Respect expander adjacency.** `MeterX`, `FugueX`, and `SlideX` need their intended parent immediately to the left. Hidden `OpMorph` should likewise be preserved with its `Operator` parent when encountered. Moving modules can break functionality without changing any patch cable.

8. **Know the semantic traps before diagnosing a module as broken.**
   - `Intone` changes operating behavior with excitation mode/patching.
   - `Tine` may be silent until pinged or pushed toward self-oscillation.
   - `Chance` and `Muse` need an external clock.
   - `Fill` intentionally stops when its clock stops.
   - `Slice` intentionally leaves many slices unmodified.
   - `Swell` intentionally stacks repeated pings.
   - `Vac` intentionally varies timing when Stability is reduced.
   - `Meter` may defer a meter change until a bar boundary.
   - `Phase` may intentionally run backward, stop, drift, or cascade one sample across two decks.

9. **Treat file operations as external side effects.** `Record` can create sample files and SFZ definitions. `Operator`, `OpEnv`, `Phase`, and `Play` can depend on external files. Do not overwrite sample destinations, change cartridges/instruments, or replace media casually. Rack undo may not restore filesystem changes.

10. **Separate hidden-model recognition from recommendation.** `Wave`, `Ratio`, `OpMorph`, `Kit`, and `Trace` may appear in saved patches or a live library, but their hidden manifest status means they should not silently become default module choices.

11. **Verify after edits.** Re-read the edited module, confirm expander links and cable topology, and use Octavia's monitoring tools when the change affects audio, timing, pitch, gates, or modulation.

## Source maintenance

Use these repository sources when maintaining this document:

- `src/plugin.cpp` is the source of truth for models actually registered by the plugin.
- `plugin.json` is the source of truth for production slug, version, public catalog metadata, model slugs, descriptions, tags, and hidden status.
- `README.md` and module-specific source files provide behavioral semantics, workflow details, persistent-state meaning, and expander relationships.
- The **live Rack library** is always authoritative for the build the user is currently running.

Update this reference when:

- a model is added, removed, renamed, hidden, or made public;
- exact model slug casing changes;
- a module's primary musical role materially changes;
- an expander relationship or placement rule changes;
- the shared root/scale or timing conventions change;
- persistent module state gains a new musically important category;
- file-loading or file-writing behavior changes in a way that affects safe agent operation.

Keep detailed parameter IDs and port IDs out of this document unless they become a deliberately stable semantic API. Octavia should continue discovering those from the running Rack instance.
