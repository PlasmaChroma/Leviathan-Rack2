# Leviathan Module Reference for Octavia Agents

This is the source-backed module reference for the **Leviathan** VCV Rack plugin, with the repository's `expander` branch treated as the documentation baseline. Use it to choose appropriate Leviathan modules, understand module families and adjacency relationships, preserve structured musical state, and recognize when a module is infrastructure, an authoring surface, or a global utility rather than an ordinary signal processor.

The production plugin slug is `Leviathan`. The current `expander` branch manifest examined for this reference reports version `2.9.1`.

Leviathan evolves quickly. Source files, the MCP references, and the manifest may occasionally move at different speeds during development. For an actual editing task, the **live Rack library and the exact running module instance are authoritative**. Exact parameter IDs, input/output IDs, module IDs, cable IDs, and runtime values should be discovered through Octavia rather than hard-coded here.

Several Leviathan modules also contain substantial state that is not adequately represented by knobs alone: samples, images, canvases, waveforms, game histories, seeds, fractal locations, room geometry, authored sequences, envelope banks, browser caches, and user-global theme settings can all be part of the user's intent.

## Selection map

| Model slug | Role | Use it when | Agent-relevant behavior |
|---|---|---|---|
| `IntegralFlux` | Four-channel function/CV processor | You need two full rise/fall function generators plus attenuverting/mixing utilities, slew, envelope following, analog-style logic, or audio-rate function work | CH1 and CH4 are the full slope-generator channels; CH2 and CH3 are simpler attenuverting signal channels. Combined OR/SUM/INV outputs make the module more than “two envelopes.” Shape mode, cycle latches, and DSP options are persistent state. |
| `Proc` | Compact function generator / slew | One generator, slew limiter, envelope follower, or cyclic modulation source is enough | Can run from an incoming signal, a trigger, or its cycle state. It provides end-of-rise/end-of-cycle events and complementary signal outputs. Do not assume conventional ADSR behavior. |
| `TemporalDeck` | Turntable-style sampler/effect | You want scratching, rate changes, freeze, reverse, slip, looping/sample playback, or long-buffer manipulation | Sample/media selection, transport, loop state, freeze/reverse/slip state, interpolation, cartridge character, and platter settings are meaningful persistent state. Prefer Temporal Deck's semantic Octavia tools when available. |
| `TDScope` | Interactive Temporal Deck expander | You want waveform visualization and direct scratch/transport interaction with Temporal Deck | It is **bidirectional**, not a passive display: dragging its waveform can send lag/scratch requests back to the deck. It must sit immediately to the right of a compatible `TemporalDeck`; ordinary patch cables do not establish the relationship. |
| `Undertow` | Mono analog-style oscillator | You want a compact sine/morph/sub oscillator with exponential and linear FM, sync, and controllable edge character | The current source is monophonic. Its Sub Gate input has meaningful normalization: unpatched keeps the sub active, while a patched low gate can silence it and gate edges affect sub state. Do not infer polyphony from other Leviathan oscillators. |
| `Deepcache` | Module-browser infrastructure | You want faster Rack module-browser previews and persistent preview caching | It has no musical signal role. It replaces/owns Rack's browser slot while active, writes cache/settings data to user storage, and explicitly yields when conflicting browser infrastructure such as Stoermelder MB owns that slot. Do not diagnose it as unpatched. |
| `Iris` | Polyphonic image-to-wavetable oscillator | You want image-derived wavetable synthesis, scan modulation, quadrature output, or image/fractal authoring workflows | The source image/table is instrument state. Iris can use its own embedded/file image or receive source material from an adjacent `Chromatide` or hidden `Nautiloid`. Preserve both source mode and source data before broad edits. |
| `Puffy` | Stereo asymmetric waveshaper/dynamics | You want saturation, animated peak shaping, roaming character, or different processing for positive and negative waveform halves | Current source provides seven character families and can unlink positive/negative character choices. Limiter, auto-deflate, roaming, and character-link state matter. Mono/stereo normalization and polarity-specific shaping mean routing and mode should be inspected together. |
| `Crownstep` | Game-driven sequencer | You want musical sequencing to emerge from a playable board game and its move history | Current source supports Checkers, Chess, and Reversi. Board state, game type, players/AI, move history, pitch mapping, sequence range, scale/root, and related options form compositional state. Starting a new game is not a harmless UI reset. |
| `Bifurx` | Dual-stage multimode filter | You want paired low/band/notch/high filtering, drive, live response/spectrum feedback, or dual-peak character filtering | The filter-mode matrix materially changes topology, and includes a `Display Only` mode. Inspect the mode before assuming the module is transforming audio. High resonance/drive can make gain changes part of the effect. |
| `Wyrm` | Drawable wavetable oscillator **or envelope** | You want hand-authored wavetable audio/LFOs, folding/FM/sync, or an arbitrary drawable one-shot envelope | Oscillator/Envelope mode changes the module's role. In Envelope mode the V/Oct jack becomes an envelope trigger, FM/fine affect envelope duration, sync is not used as oscillator sync, and signal outputs become unipolar 0–10 V envelope shapes. Wave/editor state is first-class musical state. |
| `Sil` | Automatic stereo mastering/repair processor | You want late-chain mastering, repair, glue, tonal correction, stereo enhancement, saturation, or peak control | Mastering and Repair are independently switchable. It is an active processor rather than a meter. Place it late in the chain and compare physically monitored input/output before making corrective recommendations. |
| `Umi` | Physics probability sequencer | You want visible pearl physics to generate probabilistic timing, eight sink events, and event-derived CV | Event timing emerges from simulated motion rather than a clock grid. The eight sinks share a polyphonic trigger output, with additional aggregate/event CV outputs. Seed, pearl population, and physics state shape behavior; silence between captures can be intentional. |
| `Doorstop` | Impulse-excited physical-model voice | You want spring-doorstop percussion, resonant impacts, or specimen-like physical timbres | It is struck/triggered rather than continuously oscillating and has no conventional V/Oct pitch input. Sound model, specimen seed, and break-in state are timbre identity. Do not reset or reseed casually. |
| `Chromatide` | Iris bitmap-authoring source | You want to paint the image that becomes Iris wavetable material | It authors a persistent bitmap canvas rather than generating audio. Place immediately to the left of Iris for the source relationship. Clear/paint/undo operations are sound-design edits because they change the oscillator source. |
| `Mandelwake` | Deterministic polyphonic fractal-orbit sequencer | You want fractal-derived polyphonic CV, events, spatial coordinates, phase, or random-like deterministic motion | Seed/orbit/model state determines reproducibility. Outputs include orbit geometry and event information rather than just pitch. Deterministic does not mean static; inspect channel counts and phase-polarity state before interpreting results. |
| `Cantor` | Adaptive rational pitch interpreter | You want incoming pitches interpreted through a context-sensitive rational tuning system rather than ordinary fixed 12-TET quantization | **Gate patching changes the algorithm.** With Gate connected, note-on events select and hold interpreted pitches; without Gate it behaves more like a continuously adaptive quantizer. Polyphonic voices participate in the musical context, and seed/state affects decisions. |
| `Theme` | Global visual/theme utility | You want semantic input/output/accent colors or texture changed across theme-aware Leviathan modules | It has no signal I/O. Changes are written to Leviathan user storage and affect compatible modules globally, not just the current patch. Do not alter Theme merely to “improve” one patch unless global visual change is actually intended. |
| `Octavia` | Live Rack bridge and physical monitor | An agent needs to inspect/edit Rack or analyze physically cabled signals | It is infrastructure with six physical observation inputs: Master L/R and Monitor A-D. It cannot secretly hear arbitrary module outputs. The current observation boundary intentionally reads channel 0 of polyphonic monitor ports. Never delete the active bridge during a task. |
| `OctaviaConsole` | Optional in-Rack agent console | The user explicitly wants prompting/responses inside Rack | It has no normal signal I/O and must sit immediately to the right of `Octavia`. It is an interface to the bridge, not additional authority. Enter Console Mode or enable experimental/background behavior only when the user requests it. |
| `Sibyl` | Machine-first polyphonic sequencer/arranger | An agent should author, revise, arrange, or structurally control music using semantic objects | Tracks, patterns, scenes, macros, clock, transport, observation markers, and per-step modulation are structured state. Prefer the revision-guarded `vcv_sibyl_*` semantic API over raw preset editing. Sibyl is generally the default Leviathan sequencer when AI authors the composition. |
| `Moirai` | Dual-lane 16-channel polyphonic envelope bank | You want authored per-voice envelope shapes, two related envelope lanes, and velocity/modulation-aware dynamics for polyphonic voices | Gate determines active polyphony up to 16 channels. Mono Velocity/M1/M2/M3 inputs broadcast; absent channels use neutral values. Global panel controls transform the authored envelope bank rather than replacing it. Treat the bank as structured state, not a collection of ordinary knob values. |

## Registered hidden models

These models are registered by the current source but marked hidden in the current manifest. Agents should **recognize, inspect, and preserve them in existing patches**, but should not normally choose them as default additions unless the live Rack library exposes them and the user's intent specifically calls for them.

| Model slug | Hidden role | Agent handling |
|---|---|---|
| `Nautiloid` | Fractal exploration / Iris source | Can sit immediately to the left of `Iris` and publish generated fractal image material into its source workflow. Fractal mode, location, zoom, color, and source state are musically significant. Its own CV inputs navigate the fractal; it is not a standalone audio oscillator. |
| `Chronomaw` | Eight-output clock/modulation engine | Each output can have its own waveform/modifier, multiplier, width, level, offset, phase, swing, skew, rotation, probability, inversion, and random seed. Banks and timeline state are composition. Preserve existing instances, but do not surface it as a normal public recommendation merely because it is registered. |
| `Bulkhead` | Geometry-driven stereo reverb | Room bounds, walls, listener/source geometry, decay/diffusion/absorption/motion, and dry-geometry behavior are sonic state. Preserve geometry when encountered. Do not replace it with an ordinary reverb just because it is hidden in the current catalog. |

## Relationships and placement

Leviathan includes several different kinds of module relationships. Distinguish **physical adjacency**, **ordinary patch-cable relationships**, and **semantic/infrastructure relationships**.

### True adjacency / expander relationships

- **`TemporalDeck` → `TDScope`**: `TDScope` belongs immediately to the right of `TemporalDeck`. The relationship is bidirectional: the scope receives deck state and can send interactive scratch/lag requests back. No ordinary cable can substitute for adjacency.
- **`Chromatide` → `Iris`**: `Chromatide` belongs immediately to the left of Iris when its canvas is being used as the oscillator's source.
- **`Nautiloid` → `Iris`**: hidden `Nautiloid` can occupy the same immediate-left source position and provide fractal material instead of a painted bitmap.
- **`Octavia` → `OctaviaConsole`**: Console belongs immediately to Octavia's right. It is a UI/bridge relationship, not a signal path.

Only one module can occupy a given immediate-left/right relationship. For example, Iris cannot simultaneously have both Chromatide and Nautiloid directly to its left. Inspect the active source mode and adjacency before moving anything.

### Patch-cable relationships

- **`Sibyl` → `Moirai`**: this is deliberately ordinary Rack polyphony, not a hidden expander bus. Sibyl's Gate, Velocity, and modulation streams map naturally into Moirai's Gate, Velocity, M1, M2, and M3 inputs. Moirai then provides dual envelope lanes for downstream voice shaping.
- **`Sibyl` → voices / processors**: Sibyl's polyphonic pitch/gate/velocity/modulation outputs are intended to patch into normal Rack modules. Preserve the user's chosen voice architecture; Sibyl does not require Leviathan-only destinations.
- **`Octavia` monitoring**: Master L/R and Monitor A-D only observe signals that are physically cabled to them. Semantic knowledge of a patch does not bypass this requirement.

### Semantic / external relationships

- **`Sibyl` ↔ `Octavia`**: Sibyl can author observation markers that request exact-frame captures from named Octavia monitor inputs. The timing request is semantic, but the measured audio/CV must still arrive through physical monitor cables.
- **`TemporalDeck` ↔ Octavia tools**: the MCP bridge exposes dedicated Temporal Deck operations. Prefer those for transport/sample operations over reverse-engineering raw parameters when the semantic tool exists.
- **`Theme` → Leviathan UI**: Theme affects compatible Leviathan modules globally through shared user settings. It is not a cable or expander relationship.
- **`Deepcache` → Rack browser**: Deepcache takes ownership of Rack's browser infrastructure while active. It is not part of patch signal topology and may deliberately stand down when another browser replacement owns the browser slot.

After moving a module that participates in an adjacency relationship, verify the relationship explicitly. A visually nearby module is not necessarily connected, and a correct cable graph does not prove a correct expander/source graph.

## State categories agents must distinguish

A useful way to reason about Leviathan is to separate state into three categories.

### 1. Patch-local musical state

This state belongs to the composition or sound design and should normally travel with the patch or module preset.

Examples include:

- Temporal Deck media, loops, transport modes, platter/sample configuration;
- Iris source images and generated wavetable state;
- Chromatide canvas pixels;
- Nautiloid fractal location/source state;
- Crownstep game, board, move history, pitch mapping, and sequence range;
- Wyrm wave/envelope points, rocks/deformation, editor mode, and oscillator/envelope role;
- Mandelwake seeds/orbits;
- Doorstop specimen/model/break-in state;
- Umi physics population and seed;
- Chronomaw output/bank/timeline settings;
- Bulkhead room/listener/source geometry;
- Sibyl tracks, patterns, scenes, macros, transport, and observation markers;
- Moirai authored envelope bank.

When this kind of state is changed, the sound or composition may be altered even if no ordinary parameter value appears dramatically different. Before uncertain edits, capture module state with `vcv_get_module_state`.

### 2. Adjacency and protocol state

This state exists because two modules occupy a specific relationship rather than because a cable connects them.

Key examples are:

- Temporal Deck + TD.Scope;
- Chromatide + Iris;
- Nautiloid + Iris;
- Octavia + Octavia Console.

Moving one member can silently break a working relationship. Treat module layout as functionally significant for these families.

### 3. Global or external state

This state can extend beyond a patch and may not be covered cleanly by Rack undo.

Examples include:

- Theme settings written into Leviathan user storage;
- Deepcache preview caches and browser settings;
- sample/media files referenced or manipulated by Temporal Deck;
- the running Octavia bridge/server relationship with an external agent.

Do not assume a normal Rack undo will reverse filesystem or user-global effects. Avoid broad changes to global/external state unless the user actually asked for them.

## Mode-rich and persistent behavior

### `IntegralFlux`: not merely a dual envelope

Integral Flux is best thought of as a four-channel control-voltage processor organized around two full outer function generators.

- CH1 and CH4 can generate triggered/cyclic rise-fall functions or slew/follow incoming signals.
- Both outer channels expose independent rise, fall, curve, loop, CV, and end-event behavior.
- Their shape mode switches between `Shark Fin` and `Mirror` curve behavior.
- CH2 and CH3 provide simpler attenuverting signal channels.
- Variable channel outputs feed combined `OR`, `SUM`, and `INV` logic/mix outputs.
- CH1 and CH4 also expose unity outputs and end events.

Cycle latches and DSP/render options persist. If an Integral Flux appears to be “doing too much” for an envelope, inspect the middle channels and combined outputs before simplifying it.

### `TemporalDeck` + `TDScope`: transport is structured state

Temporal Deck's turntable metaphor corresponds to real persistent transport/media choices: freeze, reverse, slip, rate behavior, interpolation, gate modes, loop/sample state, cartridge character, platter appearance, and sample path can all matter.

TD.Scope adds more than visualization. Its waveform surface is interactive: dragging can create a lag/scratch request that the adjacent Temporal Deck consumes. A user may therefore be “playing” the scope as part of the instrument.

For Octavia-driven transport or sample control, prefer the dedicated Temporal Deck semantic tools when they are exposed. They represent intent more accurately than raw parameter mutation.

### `Iris` + image-source modules: the picture is the oscillator

Iris converts image/source data into a polyphonic wavetable voice. Its source can come from:

- an image/file or embedded image state owned by Iris;
- a directly adjacent Chromatide canvas;
- a directly adjacent hidden Nautiloid fractal source.

The source kind is part of the patch state. Restoring an Iris preset while changing its adjacent source, or moving an expander without checking source mode, can produce a technically valid patch whose instrument identity has changed.

Chromatide has painting controls but no ordinary signal-processing role. Clearing its canvas is equivalent to replacing source material, not “resetting a visual.”

Nautiloid similarly treats fractal location, zoom, mode, and color as source-generation state. Its Zoom/X/Y CV inputs can animate exploration, but its value to Iris comes through adjacency/source publication rather than an audio cable.

### `Puffy`: positive and negative waveform identity can diverge

Puffy is a stereo waveshaper/dynamics processor with seven current character families:

- Bloom
- Spine
- Frenzy
- Riptide
- Void
- Swarm
- Teeth

The positive and negative waveform halves can use linked or independent character selection. This means an asymmetric transfer function can be intentional.

Roaming, auto-deflate, limiter mode, sensitivity, puff amount, and related state can animate the processor over time. Before trying to “match” left/right or positive/negative behavior, inspect whether the user deliberately unlinked it.

### `Crownstep`: game history is sequence history

Crownstep currently supports three games:

- Checkers
- Chess
- Reversi

Moves become musical sequence material. The module also tracks game-specific state, human/AI sides, AI difficulty, pitch interpretation, board-value mapping, scale/root, range trimming, playhead, move history, and display/game options.

This makes operations such as **New Game**, changing game mode, clearing history, or randomizing board-value layout musically substantial. They can invalidate the sequence the user built through play.

Use Crownstep when the game itself is the interface/compositional constraint. Prefer Sibyl when the goal is direct agent-authored musical structure rather than game-mediated composition.

### `Bifurx`: inspect filter topology before editing frequency

Bifurx has a mode matrix combining low-pass, band-pass, notch, and high-pass behavior across its two stages/peaks, plus a `Display Only` mode.

Because mode determines what “frequency,” span, balance, resonance, and drive are shaping, the active mode should be read before tonal diagnosis. In Display Only mode, visual analysis can remain active without the expected filter transformation.

When evaluating a change, compare input/output level as well as spectrum; resonance or drive can make “brighter” or “better” judgments collapse into simple loudness differences.

### `Wyrm`: oscillator and envelope are two different instruments

Wyrm's drawable table editor now serves two roles.

**Oscillator mode**

- V/Oct is pitch.
- FM modifies pitch.
- Sync performs oscillator synchronization.
- Audio/LFO range and continuous/octave stepping affect oscillator behavior.
- Main and folded waveform outputs are bipolar audio/CV-like signals.

**Envelope mode**

- the V/Oct jack becomes the envelope trigger input;
- the table is traversed once per trigger rather than looped as an oscillator;
- frequency/fine/FM control traversal duration rather than musical pitch in the ordinary oscillator sense;
- oscillator sync behavior is disabled;
- the raw and folded outputs are emitted as unipolar 0–10 V envelope shapes;
- the cycle stops after one traversal.

Switching between Oscillator and Envelope is therefore not a cosmetic mode change. It can reinterpret an existing cable and may regenerate or adapt editor state. Inspect mode before touching pitch/trigger routing.

Wave points, editor resolution, rocks/deformation, selected shape, and render/editor options are persistent. An unusual table may be deliberately hand-authored.

### `Sil`: automatic does not mean context-free

Sil exposes a compact panel while internally applying a chain of mastering/repair stages. The current source tracks stages including limiting, low recovery, impact/air, mud removal, mid enhancement, glue compression, stereo enhancement, saturation, micropeak handling, mastering, and repair.

Mastering and Repair can be enabled independently. An agent should not decide that Sil is “helping” merely because it is active. Use Octavia to compare a physically cabled pre-Sil reference and post-Sil output in the same capture whenever possible.

### `Umi`: probability emerges from physics

Umi simulates pearls falling through a physical board. Drop events can be manual, automatic, or CV-triggered; gravity, tilt, bounce, drag, chaos, and pearl population shape the trajectories.

The eight capture sinks appear as channels of a polyphonic trigger output, alongside aggregate triggers and event-derived capture velocity/position/activity outputs.

This is not a step sequencer. Uneven gaps, clustering, and silence can be the point. `Clear` destroys the current simulated population, and changing seed or replacement behavior changes the statistical trajectory.

### `Doorstop`: specimen identity matters

Doorstop is an impulse-excited spring physical model. A trigger/manual strike excites the resonator, with strike velocity controlling the event. It does not expose a conventional V/Oct pitch path.

Its sound-model family, specimen seed, and break-in state can make two otherwise similar instances sound intentionally different. Reseeding is closer to changing the physical specimen than adjusting a normal tone knob.

Silence while unstruck is expected.

### `Mandelwake`: deterministic chaos-like control

Mandelwake produces polyphonic control from fractal-orbit calculations. Depending on the active fractal/model state, outputs describe positions and derived geometry such as X, Y, radius, phase, plus gate/escape/step events.

The seed, model, center/zoom, iterations, mutation, density/rate, free-run policy, reset behavior, and phase polarity are part of repeatability.

Do not call an output “random” merely because it is complex. If the seed and state are preserved, the structure is intended to be reproducible.

### `Cantor`: Gate presence changes interpretation

Cantor has two importantly different operating regimes.

**Gate connected**

On a rising gate, Cantor performs a note-on interpretation, chooses a rationally related result within its current context, and holds that decision for the event. Falling gates release voices. This is the intended behavior for explicitly articulated musical notes.

**Gate unconnected**

Cantor continually observes pitch movement and behaves more like an adaptive quantizer/interpreter, recomputing as incoming pitch changes.

Therefore, adding or removing the Gate cable changes the musical algorithm, not merely articulation. In polyphonic use, currently active voices contribute to context, so editing one voice can affect how another voice is interpreted.

Its culture/seed/random state is part of repeatability. Do not describe it as a fixed scale quantizer or assume a stable 12-tone mapping.

### `Sibyl`: composition is semantic structured data

Sibyl is intentionally machine-first. Its musical objects include:

- tracks;
- patterns;
- pattern steps;
- scenes;
- macros;
- transport/clock state;
- exact-frame observation markers.

Steps can carry pitch/gate/velocity plus three modulation lanes, and gate duration, ties, ratchets, and scene structure have semantic meanings beyond raw voltages.

Use the dedicated revisioned semantic API. Read current state/revision, make the smallest atomic semantic change, and handle revision conflicts rather than writing raw preset JSON.

Sibyl should normally be preferred when the user asks the agent to **compose or arrange**. It should not automatically replace a user-chosen visual/manual sequencer whose interaction model is part of the request.

### `Moirai`: authored envelopes plus live transforms

Moirai is a dual-lane envelope bank for up to 16 polyphonic channels. Its Gate input sets active channel count. Velocity and M1/M2/M3 inputs use useful normaling semantics:

- an unconnected modulation input uses a neutral value;
- a monophonic input broadcasts to every active voice;
- missing channels in a polyphonic modulation cable fall back to neutral.

Its Clock input can inform tempo-relative authored envelope timing; without a usable external clock it can fall back according to the bank's timing policy.

The visible global Time Scale, Curve Bias, and Level controls transform an **authored envelope bank**. They are not substitutes for authoring the actual per-lane/per-channel shapes.

The current Octavia skill documents dedicated semantic APIs for Sibyl and Temporal Deck, but not a dedicated Moirai semantic editor. Until such an interface is explicitly exposed, treat Moirai's authored bank as structured preset state and avoid hand-editing its JSON merely because generic module-state tools can see it.

### `Theme`: a patch module that edits user-global state

Theme has no audio/CV parameters or ports. Its UI controls semantic Input, Output, and Accent colors plus texture settings shared by theme-aware Leviathan modules.

Applying a preset, swapping colors, resetting, or committing an edit writes the result into Leviathan's user storage. The visible Theme module therefore acts as an editor for global preferences rather than a patch-local color processor.

Do not alter it as part of an ordinary layout cleanup. A user who asks to recolor one screenshot has not necessarily asked to change every compatible Leviathan module across Rack.

### `Deepcache`: infrastructure can own UI resources

Deepcache has no musical patch I/O. It installs a replacement browser surface and manages pre-rendered previews/caches. It also stores its own plugin settings under Leviathan user storage.

Only one browser implementation can own Rack's browser slot at a time. The source explicitly recognizes Stoermelder MB and enters standby when MB owns the browser, activating again when that conflict disappears.

Do not try to “fix” a dormant Deepcache by moving cables or adding duplicates. Diagnose it as Rack UI infrastructure.

## Cable- and topology-sensitive semantics

A few Leviathan modules deserve special caution because cable presence or topology changes their interpretation.

### `Undertow`: Sub Gate normalization

With no Sub Gate cable, Undertow treats the gate as active and produces its sub-octave behavior normally. A patched low gate can suppress the sub path, while gate edges affect the sub-divider state.

Adding a cable therefore changes more than simple amplitude routing.

### `Puffy`: mono/stereo routing and asymmetric shaping

Puffy is stereo-aware and can normalize a single-sided input into the stereo processing path. Combined with independent positive/negative character state, apparently “duplicate” routing can still be musically deliberate.

Inspect both L/R cabling and character-link state before changing the topology.

### `Cantor`: Gate changes the algorithm

A Gate cable changes Cantor from continuously adaptive pitch interpretation into event-held note interpretation. Never add or remove it as a generic “better quantizer” improvement without considering the intended musical behavior.

### `Moirai`: polyphonic broadcast and timing authority

A mono Velocity or modulation input broadcasts to all active voices; a disconnected input uses neutral state. Replacing a mono cable with a poly cable can therefore create voice-specific articulation without touching any visible global control.

Likewise, connecting Clock can make authored envelopes follow the bank's external timing assumptions. Trace timing before adjusting Time Scale to compensate for something that is actually a clock-source issue.

### `Wyrm`: mode reinterprets the V/Oct jack

The cable itself can remain unchanged while Oscillator/Envelope mode changes the jack from musical pitch to trigger. An agent looking only at cable endpoints can therefore misunderstand the patch unless it also reads Wyrm mode.

### Adjacency families

TD.Scope, Chromatide/Nautiloid, and Octavia Console can appear “unpatched” because their important relationship is physical adjacency. Do not add fake patch cables to make the topology look complete.

## Choosing among overlapping modules

### Oscillators and voices

- Choose **`Undertow`** for a straightforward mono analog-style oscillator with sine, morph, FM, sync, and sub.
- Choose **`Iris`** when an image or generative visual field should literally become the wavetable source, especially with Chromatide/Nautiloid authoring.
- Choose **`Wyrm`** when the user wants to draw/edit the waveform directly, deform it dynamically, fold it, or reuse the same authored shape as an envelope.
- Choose **`Doorstop`** when the desired sound is a struck physical object rather than a continuously pitched oscillator.

### Envelopes and function generation

- Choose **`Proc`** for one compact function/slew/envelope-following task.
- Choose **`IntegralFlux`** when two full rise/fall generators plus attenuverting, mixing, OR/SUM/INV logic, and more complex CV processing are useful.
- Choose **`Wyrm` Envelope mode** when the envelope shape itself should be freely drawn as a table and played as a one-shot traversal.
- Choose **`Moirai`** when many polyphonic voices need authored, dual-lane envelopes with velocity/modulation-aware variation.

### Sequencing and generative control

- Choose **`Sibyl`** for agent-authored tracks, patterns, scenes, arrangement, and semantic revision.
- Choose **`Crownstep`** when a playable board game and move history are the desired sequencing interface.
- Choose **`Umi`** for emergent asynchronous event timing from visible physics.
- Choose **`Mandelwake`** for deterministic fractal-orbit polyphonic CV and event structure.
- Choose hidden **`Chronomaw`** only when the live/user context specifically calls for its eight-output timeline-oriented clock/modulation workflow.

### Pitch interpretation

- Choose **`Cantor`** when an existing pitch stream should be adaptively interpreted into rational/contextual relationships.
- Choose **`Sibyl`** when the task is to author the notes themselves.
- Do not treat Cantor as a replacement for Sibyl or vice versa: one interprets pitch; the other composes structured sequence data.

### Audio processing

- Choose **`Bifurx`** for resonant dual-stage spectral shaping/filtering.
- Choose **`Puffy`** for stereo asymmetric waveshaping, saturation, and animated dynamics.
- Choose **`Sil`** for automatic late-chain mastering/repair.
- Choose hidden **`Bulkhead`** for geometry-driven spatial reverb when an existing/live-visible patch specifically uses it.
- Choose **`TemporalDeck`** when audio transformation depends on stored/buffered time, playback rate, freeze, reverse, slip, or scratching rather than a stateless effect.

### Non-musical infrastructure

- **`Theme`** edits global visual preferences.
- **`Deepcache`** edits browser behavior/caching.
- **`Octavia`** provides live control and physical observation.
- **`OctaviaConsole`** provides an optional in-Rack conversation surface.

Do not classify these as failed musical modules because they have no useful audio output or are intentionally unpatched.

## Octavia-specific inspection and editing rules

1. **Start from the live process.** Use `vcv_get_status`, then `vcv_list_library(plugin="Leviathan")` when module availability matters. The current source reference explains semantics; the running Rack process decides what is actually installed.

2. **Resolve exact instances and IDs live.** Use `vcv_list_modules`, `vcv_get_module`, and `vcv_list_cables`. Never guess a parameter, port, module, or model ID from this document or from source familiarity.

3. **Treat layout as a hint except where adjacency is explicitly semantic.** Normal Rack routing is established by cables. TD.Scope, Iris source modules, and Octavia Console are the major Leviathan exceptions where immediate left/right placement itself carries protocol meaning.

4. **Use semantic APIs where they exist.**
   - Use `vcv_sibyl_*` for Sibyl composition/arrangement.
   - Use dedicated Temporal Deck operations for transport/sample behavior when exposed.
   - Use generic Rack parameter/cable tools for ordinary controls.
   - Do not hand-edit complex JSON merely because a generic state endpoint makes it possible.

5. **Preserve structured module state before broad or uncertain edits.** Use `vcv_get_module_state` for user-held backup when changing canvases, waves, game histories, seeds, geometry, timelines, authored banks, or other state that cannot be reconstructed from knobs alone.

6. **Respect physical observation boundaries.** Octavia hears only signals physically cabled to Master L/R or Monitor A-D. Do not claim to measure an arbitrary unpatched output. Current monitoring uses channel 0 of a polyphonic monitor connection, so a monitor cable is not automatically a full polyphonic analyzer.

7. **For processor evaluation, compare before and after simultaneously when possible.** This is especially valuable for Puffy, Bifurx, Sil, Temporal Deck effects, and Bulkhead. Level-match mentally or numerically before attributing “better” to a louder signal.

8. **Do not diagnose infrastructure as signal failure.**
   - Deepcache may have no signal cables at all.
   - Theme has no signal role.
   - Octavia Console has no normal signal role.
   - TD.Scope/Chromatide/Nautiloid use adjacency rather than ordinary audio routing.

9. **Separate public selection from hidden-model recognition.** `Nautiloid`, `Chronomaw`, and `Bulkhead` are currently hidden in the manifest. Preserve them in existing patches, inspect them normally, and use them when explicitly appropriate, but do not silently elevate them to default recommendations.

10. **Treat destructive semantic actions as destructive even when they look like buttons.**
    - Crownstep `New Game` can destroy sequence-producing game history.
    - Chromatide `Clear` destroys source-art state.
    - Umi `Clear` destroys the current physics population.
    - Doorstop reseeding changes specimen identity.
    - Theme reset/preset application changes global user settings.
    - replacing Temporal Deck media changes external/sample state.

11. **Treat filesystem and global-setting changes separately from Rack undo.** Theme and Deepcache write user-storage state; Temporal Deck can reference external media. Do not promise that `vcv_undo` can restore effects outside ordinary Rack patch state.

12. **Verify after every coherent edit.** Re-read the target, check cable application counts, re-check adjacency when relevant, and use the cheapest meaningful signal/state verification. If verification fails, stop additional writes rather than compounding uncertainty.

13. **Do not delete or save casually.** The active Octavia bridge must not be deleted during an agent task. Module deletion and patch-file overwrite follow the authorization rules in the main Octavia skill.

## Source maintenance

Use the `expander` branch when maintaining this reference until another branch is explicitly designated as the Leviathan agent-documentation baseline.

Relevant sources have different jobs:

- `plugin.json` — production plugin slug/version, model catalog, descriptions, tags, and hidden status;
- `src/plugin.cpp` — models actually registered by the build;
- individual `src/<Module>.cpp/.hpp` files — current module semantics, persistence, topology, polyphony, normaling, and UI/runtime relationships;
- `MCP/skill/octavia/SKILL.md` — current Octavia authorization, live-discovery, monitoring, and semantic-tool rules;
- `MCP/skill/octavia/references/sibyl.md` and other MCP references — specialized semantic workflows;
- the **live Rack library and live module instance** — final authority for the build currently being controlled.

The manifest's short descriptions are useful catalog metadata but can lag richer source behavior during active development. For example, a short description may still call Crownstep “checkers-driven” after additional games exist, or describe Wyrm principally as an oscillator after Envelope mode has been implemented. Prefer current module source for agent-facing behavioral detail.

Update this reference when:

- a registered model is added, removed, renamed, hidden, or made public;
- a module changes primary role or gains a role-changing mode;
- a new adjacency/expander relationship is introduced;
- a patch cable gains or loses meaning as an operating-mode switch or normalization;
- persistent structured state changes in a way agents must preserve;
- a module gains dedicated Octavia semantic tooling;
- global/user-storage or filesystem side effects change;
- polyphony behavior changes materially;
- source and manifest descriptions diverge in a way that could cause bad agent selection.

Keep exhaustive parameter and port IDs out of this document unless they become a deliberately stable semantic API. Octavia should continue discovering those from the running Rack instance.