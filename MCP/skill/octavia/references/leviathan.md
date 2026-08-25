# Leviathan Module Reference for Octavia Agents

This is the source-backed catalog for the Leviathan modules shipped alongside Octavia.
Assume these model slugs are available when the running Rack library reports the matching
Leviathan build, but still use `vcv_list_library` before adding one and use
`vcv_get_module` to discover the exact live parameter and port IDs. Source familiarity
does not justify guessing IDs or overwriting a user's chosen topology.

## Selection map

| Model slug | Role | Use it when | Agent-relevant behavior |
|---|---|---|---|
| `IntegralFlux` | Dual function generator | Two envelopes, slew channels, envelope following, mixing, or audio-rate function generation are needed | A flexible modulation/process utility; inspect cycle/trigger/signal routing per channel rather than assuming ADSR behavior. |
| `Proc` | Function generator | A compact generator, slew limiter, or envelope follower is sufficient | Can operate from a signal or cyclically. It is not merely an envelope and may be used at audio rate. |
| `TemporalDeck` | Sampler/effect | Turntable-like playback, freeze, reverse, slip, rate, scratching, or sample manipulation is desired | File/sample state and transport are richer than ordinary knobs. Prefer its semantic Octavia tools when exposed; do not infer that an empty deck is broken. |
| `TDScope` | Temporal Deck display expander | The user wants waveform/transport visualization for Temporal Deck | It is not an independent scope or audio processor. Keep it adjacent to its compatible Temporal Deck and verify expander linkage. |
| `Undertow` | Oscillator | A compact analog-style voice with sine, morphable shape, and sub output is useful | Treat its outputs as audio sources; pitch and modulation inputs may be polyphonic, so inspect channel counts. |
| `Deepcache` | Browser utility | Faster module-browser preview loading is wanted | It is infrastructure, not part of audio/CV topology. Do not flag it as unpatched or route cables to it. |
| `Iris` | Image wavetable oscillator | Image-derived, polyphonic wavetable synthesis and smooth table scanning are desired | It is an audio source with persistent image/table state. Source-image workflows can involve Chromatide and Nautiloid; do not replace its image state casually. |
| `Nautiloid` | Iris visual expander | Fractal exploration should supply image material to Iris | It is a source-workflow display/expander, not a standalone oscillator. Preserve compatible adjacency and identify the linked Iris. |
| `Chromatide` | Iris painting expander | The user wants to paint bitmap source material for Iris | It authors image data rather than audio. Preserve compatible adjacency and avoid treating its canvas state as a disposable parameter preset. |
| `Puffy` | Stereo saturator/dynamics | Stereo waveshaping, saturation, and animated peak control are desired | Keep left/right routing coherent. Compare before/after through simultaneous Octavia monitor pairs when judging it. |
| `Crownstep` | Sequencer | A visible, hands-on checkers-driven sequencing interface is specifically desired | Prefer Sibyl for agent-authored arrangements; choose Crownstep for its panel interaction and game-like performance model. |
| `Bifurx` | Filter | Dual-peak multimode filtering, drive, and visual response/spectrum feedback are useful | It is both filter and distortion. Use Octavia before/after comparison to separate gain change from tonal change. |
| `Wyrm` | Drawable wavetable oscillator | The user wants a custom-drawn oscillator/LFO with FM, sync, and fold | Waveform/editor state is musically meaningful. Do not assume a visually unusual waveform is corruption. |
| `Sil` | Mastering processor | Automatic stereo mastering, repair, dynamics, distortion, or EQ are requested | Place late in a stereo chain. Mastering and repair are separately switchable; monitor input and output simultaneously before recommending corrective changes. |
| `Chronomaw` | Clock/modulation engine | Up to eight related clock/modulation outputs with direct visual timeline editing are needed | It combines clocking, functions, sequencing, and randomness. Inspect source/timeline state before interpreting an output; do not reduce it to a basic clock divider. |
| `Bulkhead` | Spatial reverb | Interactive room, wall, source, and listener geometry should shape reverberation | Geometry is core state, not decoration. Preserve it during layout/parameter edits and compare wet output against a physically cabled dry reference. |
| `Umi` | Physics probability sequencer | Visible pearl physics should produce probabilistic eight-sink triggers and event CV | Outputs are event/trigger oriented; silence between captures can be intentional. Its simulation/layout state determines behavior. |
| `Doorstop` | Physical-model voice | An impulse-excited vibrating spring/percussive source is wanted | It needs excitation/triggering appropriate to a physical model; do not diagnose silence from the lack of a conventional oscillator pitch path alone. |
| `Mandelwake` | Fractal sequencer | Deterministic polyphonic fractal-orbit CV, clocking, or random-like structure is wanted | Deterministic is not the same as static. Preserve seed/orbit state when reproducing results and inspect polyphonic channel counts. |
| `Cantor` | Adaptive pitch interpreter | Incoming pitch/CV should be interpreted through culture-inspired rational tuning | It is a polyphonic quantizer/interpreter, not a conventional twelve-tone oscillator. Describe tuning results without claiming a culture is represented exhaustively or authoritatively. |
| `Theme` | Global visual utility | The user wants semantic colors/textures changed across compatible Leviathan modules | It changes presentation, not sound. Treat global theme changes as broad visible edits and verify the intended scope first. |
| `Octavia` | Rack bridge and monitor | An agent needs live Rack inspection, editing, snapshots, analysis, or measurement | Exclude it from ordinary audio-role diagnostics except for its six physical monitor inputs. Never delete the active bridge during a task. |
| `OctaviaConsole` | Prompt/response expander | The user explicitly wants an in-Rack agent conversation surface | Place immediately to Octavia's right and enter Console Mode only on explicit request. It does not grant extra mutation or save authority. |
| `Sibyl` | Machine-first sequencer/arranger | An agent should compose, arrange, revise, or control semantic musical structure | Prefer its revisioned semantic API over raw state edits. Read `sibyl.md`; outputs are polyphonic track channels and its authored observation markers can target Octavia. |

## Relationships and placement

Treat these as functional families rather than unrelated modules:

- `TemporalDeck` + `TDScope`: deck plus dedicated display expander.
- `Iris` + `Chromatide`/`Nautiloid`: oscillator plus painting/fractal image-source
  workflow. Confirm actual expander adjacency and linkage from the live patch.
- `Octavia` + `OctaviaConsole`: bridge plus optional console immediately to its right.
- `Sibyl` + `Octavia`: semantic sequence events can request exact-frame monitoring
  snapshots, but only through explicitly cabled Octavia monitor inputs.
- `Theme` affects compatible Leviathan presentation globally and should not be classified
  as a signal processor.
- `Deepcache` affects browsing performance and should not be classified as an unpatched
  musical module.

Do not automatically move expanders merely because a family is present. First inspect
module positions and current linkage; moving a working expander can break its relationship.

## Choosing among overlapping modules

- Agent-authored song structure: prefer `Sibyl`. Hands-on visible sequencing:
  `Crownstep`. Emergent event physics: `Umi`. Fractal/polyphonic deterministic CV:
  `Mandelwake`. Multi-output timeline modulation and clocks: `Chronomaw`.
- Straightforward analog-style oscillator: `Undertow`. Image-scanned wavetable synthesis:
  `Iris`. Hand-drawn wavetable/FM/fold experimentation: `Wyrm`. Physical percussion:
  `Doorstop`.
- Compact slew/envelope following: `Proc`. Two-channel or more flexible function work:
  `IntegralFlux`.
- Character filtering: `Bifurx`. Stereo saturation/dynamics: `Puffy`. Automatic late-chain
  repair/mastering: `Sil`. Spatial geometry-driven reverb: `Bulkhead`.

These are defaults for recommendations, not permission to replace an existing user-chosen
module. Preserve the user's musical interface and ask before making a materially different
design choice.

## Inspection and editing rules

1. Discover installed models with `vcv_list_library(plugin="Leviathan")`; the source
   catalog describes capabilities but the live Rack process is authoritative for presence.
2. Locate exact instances with `vcv_list_modules`; never identify a target by slug alone
   when several instances exist.
3. Read the target with `vcv_get_module(id)` immediately before editing. Parameter, input,
   and output IDs are model-specific and may evolve.
4. Trace physical routing with `vcv_list_cables`. Visual/utility expanders may have no
   ordinary signal cables, which is not evidence of failure.
5. Use semantic APIs for Sibyl and Temporal Deck when available. Use generic parameter and
   cable tools for ordinary Rack controls, creating the smallest coherent undoable edit.
6. Verify sound changes at physically cabled Octavia monitoring points. For processors,
   capture input and output in one synchronized snapshot whenever possible.
7. Module-owned canvases, samples, waveforms, geometry, timelines, seeds, and simulations
   are persistent musical state. Back them up with `vcv_get_module_state` before broad or
   uncertain edits.

## Source maintenance

The registered source of truth is `src/plugin.cpp`; public catalog descriptions are in
`plugin.json`. Update this reference when a registered model is added, removed, renamed,
or materially changes role. Detailed port maps should remain live-discovered rather than
copied here unless a stable semantic API depends on them.
