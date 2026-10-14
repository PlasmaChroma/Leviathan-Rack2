# Audible Instruments Module Reference for Octavia Agents

This is the source-backed module reference for the **Audible Instruments** VCV Rack plugin. Use it to choose appropriate modules, recognize important Mutable Instruments design conventions, preserve non-obvious mode state, and understand when cable insertion or removal changes a module's behavior rather than merely connecting a signal.

The production plugin slug is `AudibleInstruments`. The current `v2` manifest examined for this reference reports version `2.0.0`.

Audible Instruments ports Mutable Instruments hardware and firmware concepts into Rack, but the VCV implementation is not always a perfect behavioral copy of the original hardware. Use the **current Audible Instruments source and the live Rack library as authority for the installed plugin**. Original Mutable Instruments manuals are useful for musical intent and conceptual behavior, but do not assume every hardware shortcut, normalization, firmware feature, or chaining mechanism exists in the Rack port.

Exact parameter IDs, input/output IDs, module IDs, cable IDs, and runtime values should be discovered through Octavia rather than hard-coded here.

## Selection map

| Model slug | Role | Use it when | Agent-relevant behavior |
|---|---|---|---|
| `Braids` | Macro oscillator with a large model library | You want one oscillator to cover classic waves, waveshaping, FM, formants, physical models, percussion, wavetables, noise, or granular/digital textures | Model choice is fundamental state. `META` mode repurposes FM CV to select oscillator models instead of modulating pitch. Pitch drift, waveform imperfections, and low-CPU behavior are context-menu state. Do not assume hardware oscillator-sync behavior is implemented. |
| `Plaits` | Polyphonic multi-engine macro oscillator | You want a compact voice spanning pitched synthesis, speech, chords, physical modeling, noise, or percussion with main and auxiliary outputs | This port exposes 16 engines: 8 pitched and 8 noise/percussive. Polyphony follows the pitch input, and Model CV can select different engines per voice. Trigger and Level patching alter the internal voice/LPG behavior, so inspect those cables before diagnosing articulation. |
| `Elements` | Polyphonic exciter + modal/physical resonator voice | You want bowed, blown, struck, string-like, modal, chordal, or unusual resonant physical synthesis | Exciter and resonator are one coupled instrument. The context menu selects `Original`, `Non-linear string`, `Chords`, or `Ominous voice`; this model state materially changes the instrument and is persistent. External blow/strike signals can replace or augment the internal excitation path. |
| `Tides` | Original Tides function generator / oscillator | You want a shapeable envelope, cyclic function, oscillator, clock-synchronized modulation, or the Sheep wavetable firmware | Has three generator modes, three frequency ranges, high/low phase gates, unipolar and bipolar outputs. In this Rack port, connecting Clock automatically enables sync. The context menu can switch the entire module to `Wavetable firmware (Sheep)`; Sheep is not a separate model slug. |
| `Tides2` | 2018 Tides four-output slope generator | You want four related modulation/audio outputs with controllable phase/frequency relationships, clock ratios, and AD/AR/looping behavior | Distinct from `Tides`. It has three ramp modes and four output modes, with four related outputs. When Clock is connected, frequency control is interpreted through clock-relative ratios instead of purely free-running rate. |
| `Clouds` | Stereo granular texture processor | You want granular freezing, time stretching, pitch shifting, looping-delay behavior, spectral processing, or diffuse texture | Context state drastically changes semantics: four alternate playback modes, four Blend-knob assignments, and four quality/buffer configurations. Inspect these before changing familiar-looking knobs because the same panel control can mean different things. |
| `Warps` | Two-input meta-modulation processor | You want continuous morphing among cross-modulation, waveshaping, ring/logic-style processing, vocoding/frequency-shift territory, or an internal carrier | The Algorithm control continuously morphs through processing regions rather than choosing a single ordinary effect. Internal oscillator mode changes the role of the carrier path and makes the first level control double as internal oscillator frequency. Carrier-shape state is persistent. |
| `Rings` | Physical-model resonator | You want pitched resonances, modal bodies, sympathetic strings, inharmonic strings, or resonator voices excited internally or externally | Context menu exposes six resonator models plus `Disastrous Peace`. Polyphony is selectable. ODD/EVEN outputs only split when both are patched; with only one output patched, the two signals are mixed. Some original hardware auto-strum/transient normalizations remain TODO in this port. |
| `Links` | Buffered multiple + precision summing utility | You want one-to-three duplication, two-input summing with two copies, or three-input summing, including polyphonic signals | It is not merely a passive multiple. Section A duplicates; section B sums two inputs and duplicates the sum; section C sums three inputs. The summing sections use Rack polyphonic broadcast behavior. |
| `Kinks` | Rectification, analog min/max, noise, and sample-and-hold utility | You want signal inversion/rectification, analog voltage comparison, noise, or S&H in a compact utility | MAX/MIN are analog voltage operations, not Boolean logic gates. If the S&H signal input is unpatched, it normalizes to the module's own Gaussian noise; a trigger still determines when a new value is captured. |
| `Shades` | Three-channel attenuator/attenuverter and cascading mixer | You want manual attenuation, inversion, DC offsets, or small-signal mixing | Each unpatched input normalizes to +5 V, so a channel can generate a DC offset. Unpatched outputs continue accumulating into later channels; patching an output breaks the mix chain at that point. Cable placement therefore changes the internal grouping. |
| `Branches` | Dual polyphonic Bernoulli gate | You want probabilistic gate routing or a probabilistically toggled state | Channel 2 gate input normalizes to Channel 1 when unpatched. In direct/latch behavior, each incoming gate is routed to A or B; in Toggle mode, successful tosses flip a persistent high/low output state. Mode choice is persistent and changes the meaning of the outputs substantially. |
| `Blinds` | Four-channel bipolar VCA / VC polarizer / ring modulator | You want voltage-controlled attenuation, inversion, bipolar modulation, ring modulation, or bipolar CV generation | Gain can cross through zero into inversion. Unpatched signal inputs normalize to +5 V, making channels useful as CV generators. Like Shades, outputs form a cascading mix chain that is broken by patching intermediate outputs. |
| `Veils` | Four-channel VCA with variable response and cascading mix | You want conventional audio/CV amplitude control with adjustable exponential-to-linear response | Each channel has gain plus response shape. CV is nominally 0–5 V but can drive above unity. Unpatched channel outputs accumulate forward; taking an intermediate output removes the accumulated group from later outputs. |
| `Frames` | Four-channel keyframer, VCA/mixer, or Poly LFO | You want to interpolate an entire set of four gain states with one Frame position, animate a mix, or use the alternate multi-LFO mode | Keyframes and per-channel interpolation curves are structured persistent state. Individual output patching removes that channel from MIX. Inputs normalize from ALL, and ALL can normalize to +10 V when Offset is enabled. `Poly LFO` changes the module's entire role. |
| `Stages` | Six-segment function generator | You want custom envelopes, LFOs, step/hold segments, CV delays, sample-and-hold-like behavior, or several smaller functions from one module | **Gate jack placement defines segment groups.** A patched gate starts a group and following unpatched segments belong to it until the next patched gate. Short presses change Ramp/Step/Hold type; long presses change loop state. Repatching one gate can reinterpret several segments at once. |
| `Marbles` | Random gate/CV generator with controlled repetition | You want musically structured random triggers and voltages, repeatable probability loops, clock division/multiplication, or quantized random melodies | T and X have multiple hidden modes/ranges; Déjà Vu can recycle and mutate generated material. External X clocking changes how the three X outputs relate, while internal clock-source routing is configurable. Scale, mode, range, clock-source, and Y-divider settings are important persistent state. |
| `Ripples` | Polyphonic resonant multimode low-pass filter with VCA output | You want a musical resonant filter with simultaneous 12 dB band-pass, 12 dB low-pass, 24 dB low-pass, and 24 dB low-pass/VCA outputs | The Gain CV affects the dedicated VCA low-pass output rather than every filter output. Polyphony follows the audio input. It is a straightforward filter compared with the more mode-heavy modules in this set. |
| `Shelves` | Polyphonic four-band voltage-controlled EQ / filter bank | You want low/high shelves, two parametric mids, or simultaneous HP/BP/LP responses from the two mid bands | The two parametric sections expose their own HP/BP/LP outputs in addition to the full EQ output. Global frequency/gain CV complements per-band CV. `Pad input by -6dB` is persistent context-menu state and changes headroom. |
| `Streams` | Polyphonic dual dynamics processor / low-pass-gate system | You want envelopes, vactrol/LPG behavior, followers, compressors, filter-control functions, or chaotic modulation | Each channel can select among 10 modes, including Envelope, Vactrol, Follower, Compressor, alternates, Direct VCF controller, and Lorenz generator. The same knobs mean different things in different modes. Channels can be linked, and meter source is configurable. |

## Catalog and port notes

The current source registers the 20 models listed above. There are no separately registered expander models in the current catalog, and no hidden registered models were found in the current `plugin.json` / `plugin.cpp` pair.

A few repository details are easy to misread:

- **`Tides` and `Tides2` are different registered modules.** `Tides` represents the original design; `Tides2` represents the later four-output design.
- **Sheep is not a separate Rack model.** It is selected from the `Tides` context menu as alternate wavetable firmware.
- **`Streams` is currently registered and present in the source.** The README still lists Streams under “Not yet ported,” so that section is stale. Prefer `plugin.json`, `src/plugin.cpp`, the module source, and the live Rack library over old README status text.
- The plugin is inspired by and largely based on Mutable Instruments firmware, but **hardware manuals do not override Rack source behavior** where the port differs.

## Mode-rich and persistent state

Many Audible Instruments modules hide musically decisive state behind buttons or context menus. An agent should not infer the whole patch from knob positions alone.

### `Braids`

Treat the oscillator model as part of the voice identity. The current source exposes a broad Braids model set covering conventional waves, oscillator combinations, filtering/formants, FM, physical models, percussion, wavetables, noise, granular material, and digital modulation.

Important context state includes:

- `META`: FM CV selects the synthesis model rather than modulating pitch.
- `DRFT`: adds pitch drift.
- `SIGN`: adds waveform imperfections.
- Low CPU: disables normal resampling behavior.

The source still contains a TODO for sync-buffer support. Do not rely on original hardware oscillator-sync behavior when reasoning about this port.

### `Plaits`

The current port exposes **16 engines**, not every engine added by later revisions of Mutable's firmware.

The eight pitched engines are:

- Pair of classic waveforms
- Waveshaping oscillator
- Two-operator FM
- Granular formant oscillator
- Harmonic oscillator
- Wavetable oscillator
- Chords
- Vowel and speech synthesis

The eight noise/percussive engines are:

- Granular cloud
- Filtered noise
- Particle noise
- Inharmonic string modeling
- Modal resonator
- Analog bass drum
- Analog snare drum
- Analog hi-hat

The pitch input determines voice-channel count, up to Rack's supported polyphony in this implementation. Model CV can cause individual polyphonic channels to use different engines.

Trigger and Level are not ordinary “always equivalent” CV inputs: the Plaits voice engine is explicitly told whether those inputs are patched, and uses that information in its internal excitation/LPG behavior.

The context menu can temporarily expose the otherwise-hidden LPG response and decay controls in place of Timbre and Morph. The selected engine itself is persistent patch state.

### `Elements`

Think of Elements as an **exciter feeding a resonating object**, not as a conventional subtractive voice.

The exciter side includes bow, blow, strike/noise, contour, flow, mallet, and timbral controls. The resonator side includes geometry, brightness, damping, excitation position, and space/reverb. External Blow and Strike inputs can inject excitation directly.

The context-menu resonator model is persistent:

- Original
- Non-linear string
- Chords
- Ominous voice

The current implementation processes multiple voices when the pitch input is polyphonic.

### `Tides`

The original Tides port stores generator mode, frequency range, and Sheep state.

A critical Rack-specific difference is clock synchronization: the implementation automatically enables sync whenever Clock is connected, unless Sheep firmware is active. Do not try to reproduce the original hardware's hold-button sync gesture.

`Wavetable firmware (Sheep)` changes the module from Tides behavior into the Sheep wavetable oscillator while retaining the same Rack model slug.

### `Tides2`

`Tides2` has:

- three ranges;
- three ramp modes: AD, AR, and Looping;
- four output modes: Gates, Amplitude, Frequency, and Slope/Phase;
- four related outputs.

When Clock is not connected, Frequency behaves as a free generator rate/pitch. When Clock **is** connected, the implementation derives frequency through a set of musical clock ratios. The meaning of the frequency control therefore changes with cable state.

### `Clouds`

Before editing a Clouds patch, inspect all three context-menu dimensions.

**Blend knob assignment**

- Wet/dry
- Spread
- Feedback
- Reverb

**Alternate playback mode**

- Granular
- Pitch-shifter/time-stretcher
- Looping delay
- Spectral madness

**Quality / buffer mode**

- 1 s, 32 kHz, 16-bit stereo
- 2 s, 32 kHz, 16-bit mono
- 4 s, 16 kHz, 8-bit µ-law stereo
- 8 s, 16 kHz, 8-bit µ-law mono

These are not cosmetic choices. They alter what the panel controls do, what kind of DSP is running, and how much audio history exists.

### `Warps`

Warps should be understood as a **continuous meta-modulator**, not a fixed bank of effects selected one at a time. Algorithm and Algorithm CV move continuously through the processing space, while Timbre controls a secondary parameter within that region.

Its internal-carrier state cycles through four carrier shapes and is persistent. When the internal carrier is being used, the first level control also functions as internal oscillator frequency, so “Level 1” is not semantically stable across all states.

### `Rings`

Rings stores:

- polyphony mode;
- resonator model;
- `Disastrous Peace` state.

The context menu exposes:

- Modal resonator
- Sympathetic strings
- Modulated/inharmonic string
- FM voice
- Quantized sympathetic strings
- Reverb string

The front-panel model button normally traverses the principal resonator behaviors; context-menu selection can reach additional models.

`Disastrous Peace` switches to the alternate string-synth path and is a substantial sonic change, not a visual easter egg.

### `Frames`

Frames contains real compositional state: keyframes, interpolation settings, response curves, and mode.

Each channel can use interpolation such as Step, Linear, Accelerating, Decelerating, Departure/arrival, or Bouncing. These settings change the trajectory between saved gain states.

`Poly LFO` is an alternate operating mode, not simply an extra output. When active, the four primary controls become parameters of the multi-LFO behavior rather than ordinary channel gains.

### `Stages`

Each of the six segments has two persistent pieces of mode state:

- segment type: Ramp, Step, or Hold;
- loop membership.

The gate-input topology then dynamically groups those segment configurations into functions. This means the same six segment settings can describe a completely different set of envelopes/functions after a cable change.

Within a multi-segment group, the first output carries the group's main function while later segment outputs expose activity/phase-style information for their segment.

Do not assume the original hardware's multi-module chaining behavior exists as a Rack expander relationship. The current Audible Instruments model is a self-contained six-segment Rack module.

### `Marbles`

Marbles is not “just random.” Its hidden modes determine the probability system.

T-section modes include:

- Complementary Bernoulli
- Clusters
- Drums
- Independent Bernoulli
- Divider
- Three states
- Markov

It also has T rate ranges, multiple X distribution modes/ranges, selectable scales, internal X-clock routing, and a configurable Y divider.

Déjà Vu determines whether the T and/or X sections recycle previously generated decisions/voltages. Near the locking region, behavior can become strongly repeatable; elsewhere it can mutate or rearrange material. Preserve these settings when the musical intent is a repeatable generative phrase.

### `Streams`

Streams is especially mode-sensitive. Each of its two channels can be one of:

- Envelope
- Vactrol
- Follower
- Compressor
- AR envelope
- Plucked vactrol
- Cutoff controller
- Slow compressor
- Direct VCF controller
- Lorenz generator

The controls named Shape, Mod, Level Mod, and Response must therefore be interpreted **through the selected channel mode**. A parameter edit that makes sense for Compressor may be meaningless or destructive when that channel is a Lorenz generator.

Channel linking and the meter's source selection are persistent state shared across the module's polyphonic processing.

## Normalizations and cable-sensitive semantics

Audible Instruments contains several modules where an **unpatched jack is part of the algorithm**. Before adding, removing, or rerouting a cable, check whether the cable itself changes an internal normalization or grouping rule.

### `Rings`: output patching changes the mix

- If both ODD and EVEN are patched, they carry separate resonator signals.
- If only one is patched, the implementation mixes the two internal signals together and presents the sum.

Adding a cable to the second output therefore changes the signal already coming from the first.

Rings also enables its internal exciter, internal strum behavior, and internal note behavior according to whether IN, STRUM, and PITCH are patched. However, the source explicitly marks some of the original hardware's automatic note-change/transient detectors as TODO, so reason from the Rack implementation rather than assuming complete hardware normaling.

### `Frames`: input and output normaling define the mixer

For each channel:

- an unpatched channel input normalizes from ALL;
- when ALL is unpatched, Offset can provide +10 V as the common source;
- an individually patched channel output is removed from MIX;
- a channel without its own output cable continues into MIX.

Thus Frames can be a keyframed mixer, multi-output VCA bank, animated CV source, or hybrid simply by changing cables.

### `Shades`: outputs break the accumulation chain

Each unpatched input produces +5 V before gain/attenuversion, allowing offset generation.

Channels are summed from top to bottom. When an output is patched, the current accumulated sum is emitted and the accumulator resets. Later channels start a new group.

A cable at an output therefore changes what downstream outputs contain.

### `Blinds`: bipolar version of the same cascading idea

Unpatched inputs normalize to +5 V. Each channel adds a bipolar gain-controlled contribution into an accumulator.

Taking an output breaks the chain and begins a new accumulated group below it. This makes Blinds simultaneously useful as a polarizer, ring modulator, CV generator, and groupable bipolar mixer.

### `Veils`: VCA output patching defines submixes

Veils also accumulates channel outputs downward. An intermediate patched output emits the current accumulated group and resets it before the next channel.

Do not assume Output 4 always contains Channels 1–4; it does only when earlier output jacks have not broken the chain.

### `Branches`: Channel 2 gate normals from Channel 1

If the second gate input is unpatched, Branches uses Channel 1's incoming gate for Channel 2 as well. This is useful for producing two differently randomized decisions from one event stream.

Patching Channel 2 breaks that relationship immediately.

### `Kinks`: S&H signal normals to noise

If the Sample & Hold source input is empty, the internal Gaussian noise generator becomes its source. The Trigger input still determines capture timing.

Adding a cable to the S&H source converts it from a random-voltage generator into a normal sample-and-hold without changing any knob or mode.

### `Tides`: Clock connection enables sync

In this port, simply connecting Clock enables generator synchronization, except in Sheep mode. Removing the cable returns the generator to unsynchronized behavior.

### `Tides2`: Clock changes the rate model

With no Clock cable, Frequency controls the generator in its selected range. With Clock patched, Tides2 extracts clock timing and maps Frequency/transposition into discrete ratios around that clock.

A “frequency adjustment” can therefore mean free rate in one topology and clock multiplication/division in another.

### `Stages`: Gate jack placement builds functions

This is the most important cable-semantic rule in the set.

A connected Gate input begins a gated group. Following segments whose Gate inputs are empty join that group until another Gate input starts another group. Repatching one Gate can split or merge functions across the six segments.

Always inspect all six Gate inputs before interpreting any one segment.

### `Marbles`: X clock patching changes voice relationships

Marbles can derive X clocks internally from T using selectable routing. An external X clock changes that relationship and causes the X side to follow the external timing path.

Do not “clean up” an apparently redundant clock cable without checking whether it is deliberately changing how the three X outputs synchronize.

## Choosing among overlapping modules

### For broad digital voices

- Choose **`Braids`** when a single model knob should traverse a very broad historical macro-oscillator library or when a specific Braids model is part of the desired sound.
- Choose **`Plaits`** for a more self-contained voice architecture with internal excitation/LPG behavior, main/aux outputs, polyphony, and per-voice model CV.
- Choose **`Elements`** when excitation and resonant-body behavior are the core of the instrument rather than merely one oscillator algorithm.

### For resonant and physical synthesis

- Choose **`Elements`** when you want a full exciter + resonator instrument with bow/blow/strike control.
- Choose **`Rings`** when you already have an excitation source, want a more compact resonator, or want its internal exciter/strum behavior.
- Use **`Plaits`** when physical modeling is only one voice engine among many and patch compactness matters.

### For function generation and modulation

- Choose **`Tides`** for the original Tides architecture, high/low phase outputs, or Sheep.
- Choose **`Tides2`** for four coordinated outputs, clock-ratio behavior, and the later slope-generator architecture.
- Choose **`Stages`** when the desired function needs an arbitrary sequence of ramp/step/hold segments or several independent functions in one module.
- Choose **`Frames`** when the goal is morphing among entire multi-parameter states rather than generating one envelope.
- Choose **`Marbles`** when modulation should be probabilistic, structured, and optionally repetitive rather than deterministic.

### For VCAs, mixing, and polarity

- Choose **`Veils`** for conventional positive VCA control and variable response.
- Choose **`Blinds`** when bipolar gain, inversion, ring modulation, or CV polarization is required.
- Choose **`Shades`** for simple manual attenuation/attenuversion, offset generation, and compact mixing.
- Choose **`Frames`** when four gains need to morph together under one higher-level animation control.

Remember that Shades, Blinds, and Veils all use cable-sensitive cascading output groups.

### For random decisions

- Choose **`Branches`** for a simple probability decision applied to incoming gates.
- Choose **`Marbles`** when the module should generate the rhythm and/or CV material itself, especially when repetition, bias, spread, scales, or controlled mutation matter.
- Use **`Kinks`** when all you need is raw noise or sample-and-hold rather than a compositional random system.

### For filtering and dynamics

- Choose **`Ripples`** for a compact resonant filter with simultaneous LP/BP outputs and a built-in VCA path.
- Choose **`Shelves`** for broad spectral shaping, parametric EQ, or access to the two parametric bands as separate multimode filters.
- Choose **`Streams`** when amplitude/spectral dynamics should respond through envelope, vactrol, follower, compressor, filter-controller, or chaotic behavior.

### For audio transformation

- Choose **`Clouds`** for time-domain texture, grains, freeze, looping, spectral smearing, and buffer-based transformations.
- Choose **`Warps`** for instantaneous two-signal interaction and continuous movement through modulation/waveshaping/cross-synthesis territory.
- They are complementary rather than interchangeable: Clouds remembers audio; Warps primarily transforms the relationship between current signals.

## Polyphony notes

Do not assume that every hardware-derived module is monophonic merely because the original Eurorack hardware was.

The current Rack source explicitly supports polyphonic processing in several important places, including:

- `Plaits`
- `Elements`
- `Links`
- `Branches`
- `Ripples`
- `Shelves`
- `Streams`

Polyphony often follows a specific authority input:

- `Plaits`: pitch input channel count.
- `Elements`: pitch input channel count.
- `Ripples`: audio input channel count.
- `Links`: the widest participating input in each summing section.
- `Branches`: the effective gate input for each Bernoulli channel.
- `Streams`: shared UI settings are applied across its polyphonic processing engines.

Other modules in this port use monophonic signal access even when an analogous modern Rack utility might normally support polyphony. Always inspect the live module and port channel counts instead of generalizing from Rack conventions.

## Inspection and editing rules

1. **Discover the live library first.** Begin with `vcv_list_library(plugin="AudibleInstruments")`. The running Rack installation is authoritative if its catalog differs from this document.

2. **Identify module instances by Rack module ID, not model name alone.** Patches commonly contain several `Plaits`, `Rings`, `Stages`, or utility instances.

3. **Discover exact parameter and port IDs live.** This reference intentionally documents semantic behavior rather than freezing implementation IDs.

4. **Inspect mode state before touching mode-sensitive modules.** This is especially important for:
   - `Braids` model/META/DRFT/SIGN;
   - `Plaits` engine;
   - `Elements` model;
   - `Tides` mode/range/Sheep;
   - `Tides2` range/output/ramp mode;
   - `Clouds` blend/playback/quality;
   - `Warps` carrier state;
   - `Rings` polyphony/model/easter egg;
   - `Frames` keyframes/interpolation/Poly LFO;
   - `Stages` segment types/loops;
   - `Marbles` T/X modes, scales, clock routing, and Déjà Vu;
   - `Streams` per-channel mode/link/meter state.

5. **Trace cables before changing controls whose semantics depend on patching.** Cable presence is part of the configuration for Rings, Frames, Shades, Branches, Blinds, Veils, Tides, Tides2, Stages, Marbles, Kinks, and parts of Plaits/Elements.

6. **Do not treat front-panel labels as globally stable meanings.** In modules such as Clouds, Streams, Warps, Frames, Braids, and Plaits, the active mode can substantially reinterpret a control.

7. **Preserve structured state before broad edits.** Frames keyframes, Stages segment configurations, Marbles probability modes, Clouds alternate modes, and model selections in the oscillator/resonator modules often carry more musical intent than any single knob value.

8. **Do not infer hardware behavior where the Rack source differs.**
   - Braids does not provide complete hardware sync/settings behavior.
   - Rings contains TODOs for some hardware automatic strum/transient normalizations.
   - Tides automatically syncs when Clock is patched rather than using the original hardware gesture.
   - Sheep is implemented inside `Tides`, not as another Rack module.
   - Do not assume hardware-only module chaining or service-menu behavior is available.

9. **Be careful when adding “helpful” output cables.** On Rings, Frames, Shades, Blinds, and Veils, adding an output connection can change the signal available elsewhere in the same module.

10. **Be careful when removing “redundant” cables.** A cable may be overriding a normalization or changing timing/group structure. This is especially dangerous on Stages, Marbles, Tides/Tides2, Branches, Rings, and Kinks.

11. **Verify after edits.** Re-read the changed module and its neighbors, confirm cable topology, confirm mode state where available, and use Octavia monitoring for changes involving audio level, pitch, timing, gates, or modulation.

## Source maintenance

Use these sources when maintaining this document:

- `plugin.json` is the source of truth for the production plugin slug, version, catalog slugs, descriptions, and tags.
- `src/plugin.cpp` is the source of truth for models actually registered by the plugin.
- each module's `src/<Model>.cpp` is the source of truth for Rack-specific controls, normalizations, context-menu state, polyphony, persistence, and known implementation differences;
- the original Mutable Instruments manuals are valuable for musical concepts and intended workflow when they do not conflict with the Rack implementation;
- the **live Rack library** is always authoritative for the user's installed build.

Do not use the README alone as a port-status authority. In the current repository it still says Streams is “Not yet ported,” while `plugin.json`, `src/plugin.cpp`, and `src/Streams.cpp` show that Streams is part of the plugin.

Update this reference when:

- a model is added, removed, or renamed;
- a context-menu mode becomes a separate model or vice versa;
- a Rack-specific normalization or cable-sensitive behavior changes;
- a module gains or loses polyphony;
- a hidden/alternate firmware mode changes;
- persistent non-parameter state changes;
- a hardware behavior previously marked TODO becomes implemented;
- a major upstream Mutable firmware feature is added to the port.

Keep exact parameter and port IDs out of this document unless they become a deliberately stable semantic API. Octavia should continue discovering those from the running Rack instance.