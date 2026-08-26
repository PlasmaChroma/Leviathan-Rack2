Dragon King Leviathan, I think the architecture is now quite clear. I’ll use the repository spelling **Sibyl** below.

My working name for the companion is **Moirai**: Sibyl foretells the events; the Moirai spin, measure, and cut their amplitude through time. The terminal **AI** is merely a suspiciously convenient flourish.

# Core recommendation

**Moirai should be a 12 HP, dual-lane, 16-channel polyphonic envelope bank.**

Each lane produces an independent polyphonic envelope output, so one Sibyl poly gate cable can generate, for example:

* Lane A: amplitude envelopes for up to 16 tracks.
* Lane B: filter, timbre, pitch, or modulation envelopes for those same tracks.

That gives a practical maximum of **32 simultaneously running envelope voices**, but the panel only needs two output jacks and one shared set of inputs. One lane would be too austere for the common amplitude-plus-filter case; four lanes would begin turning the compact interface into a small telephone exchange. Two is the balance point.

The module would be:

| Dimension         | Recommendation                                               |
| ----------------- | ------------------------------------------------------------ |
| Width             | 12 HP                                                        |
| Envelope lanes    | A and B                                                      |
| Polyphony         | Up to 16 channels per lane                                   |
| Primary outputs   | A and B, both polyphonic                                     |
| Auxiliary outputs | EOC A and EOC B, both polyphonic                             |
| Triggering        | One shared polyphonic gate input                             |
| Modulation        | Poly velocity and three poly macro inputs                    |
| Timing            | Free-time or clock-synchronized                              |
| Configuration     | Local presets plus Octavia semantic configuration            |
| Visualization     | Small operational display; expanded editor optional          |
| Audio dependency  | None; configured state remains fully local and deterministic |

# What Sibyl and Octavia already establish

The existing architecture separates three concerns remarkably well.

**Configuration is semantic.** Sibyl implements `SibylControl`, while Sibyl itself remains the authority for its JSON schema, validation, revision handling, compilation, and transport rules. Octavia treats the request and response bodies as opaque module-owned documents rather than trying to understand musical composition internally. ([GitHub][1])

**Mutation is transactional and undoable.** Octavia resolves the target module in a Rack-safe queued context, verifies that it implements the semantic capability, captures the old Rack state for edits, calls the module, validates the returned JSON object, and creates one undo entry only after a successful commit. Sibyl’s edit request includes an expected revision, ordered atomic operations, an adoption boundary, and a phase policy. ([GitHub][2])

**Patching remains physical.** Octavia’s MCP server separately resolves input and output ports by ID or case-insensitive name and creates ordinary Rack cables. Sibyl already exposes polyphonic pitch, gate, velocity, and three modulation outputs, plus reconstructed clock, scene, and end-of-cycle triggers. ([GitHub][3])

That same separation should govern Moirai:

> The AI describes and installs the envelope system. Rack cables perform the music. The LLM is never anywhere near the per-sample path.

Moirai should therefore be a **companion by workflow**, not a private Sibyl expander. No adjacency requirement, no hidden bus, and no dependency on Sibyl being present after configuration. It should work equally well with another sequencer, MIDI-to-CV module, keyboard, or arbitrary polyphonic gate source.

# Why Moirai should not simply copy Wyrm’s envelope engine

Wyrm gives us an excellent expressive prototype. Its envelope mode supports up to 16 channels, a drawable contour of 32–256 points, an AR shape plus D1–D10 factory contours, exponential time modulation, and 0–10 V envelope output. A rising edge on its V/Oct/trigger input restarts a fixed-duration one-shot, and the output returns to zero when phase reaches the end. ([GitHub][4])

But a dedicated envelope module needs several things Wyrm intentionally does not provide:

* Gate-low release branching.
* Sustain stages.
* Loops within a subsection of the envelope.
* Per-segment duration and curvature.
* Retrigger policies such as restart, legato, and from-current.
* Clock-relative durations.
* Stable absolute levels rather than oscillator-style normalization.
* Different programs assigned to different poly channels.

There are also two implementation details that should explicitly change:

1. **No amplitude normalization.** Wyrm normalizes its wavetable around its maximum absolute sample, which is sensible for an oscillator. In an envelope, a programmed sustain level of `0.58` must remain `0.58`.

2. **No periodic interpolation except in cycle mode.** Wyrm’s interpolator wraps the end of the waveform back to its beginning. Moirai should use clamped non-periodic Hermite/Catmull interpolation for ordinary envelopes, retaining the endpoint-range overshoot protection already present in Wyrm. Periodic interpolation would only be enabled for a cycling contour. ([GitHub][5])

# Signal architecture

The intended Sibyl patch is extremely direct:

```text
Sibyl GATE             ─────► Moirai GATE
Sibyl VELOCITY         ─────► Moirai VEL
Sibyl MODULATION 1     ─────► Moirai M1
Sibyl MODULATION 2     ─────► Moirai M2
Sibyl MODULATION 3     ─────► Moirai M3
Sibyl CLOCK            ─────► Moirai CLOCK
Sibyl SCENE, optional  ─────► Moirai RESET

Moirai A               ─────► Poly VCA CV
Moirai B               ─────► Poly filter/modulation CV
Moirai EOC A/B         ─────► Downstream rhythmic or logic patch
```

Sibyl’s gate, velocity, and modulation values are emitted channel-for-channel from its track states. Its composition schema permits each of the three modulation values to range from −10 V to +10 V, making them appropriate for either bipolar modulation or quantized selection duties. ([GitHub][3])

A useful factory mapping would be:

* **Velocity:** envelope peak/depth.
* **M1:** exponential time scale.
* **M2:** curve or temporal skew.
* **M3:** program variant selection, sampled on the next gate edge.

That last mapping is especially powerful. Sibyl could choose a different envelope variant on every note without asking the LLM to perform a real-time edit. One bass step might select a sharp pluck, the next a longer resonant contour, and the next an accented double-decay. The AI constructs the vocabulary; Sibyl speaks it through voltage.

## Polyphonic rules

The gate input determines the output channel count, from 1 through 16.

A one-channel velocity or macro signal broadcasts to all gate channels. A matching polyphonic signal addresses channels individually. Missing polyphonic channels contribute their configured neutral value.

Each channel has two assignments:

```text
Channel 1  → Lane A program / Lane B program
Channel 2  → Lane A program / Lane B program
...
Channel 16 → Lane A program / Lane B program
```

The assignments refer to shared program IDs rather than embedding 32 duplicate envelopes. Thus several channels can share `short_pluck`, while only the pad channel uses `slow_bloom`.

A compact bank could support:

* Up to 32 named programs.
* Up to 256 authored contour points in one program.
* A global maximum of approximately 8,192 points across the bank.
* Named channel labels such as `kick`, `bass`, or `upper_pad`.
* A default program per lane plus sparse channel overrides.

That complexity ceiling also keeps serialized state comfortably below Octavia’s generic 1 MiB module-state safety limit. ([GitHub][2])

# One engine, two authoring languages

I would not create separate “basic” and “advanced” DSP engines. Basic envelopes should simply generate advanced envelope programs.

Moirai would accept two equivalent authoring forms.

## 1. Staged envelopes

This is the semantic form for ADSR, AD, AR, AHR, DADSR, and related conventional envelopes.

```json
{
  "id": "bass_amp",
  "name": "Bass Amp",
  "kind": "staged",
  "mode": "gate",
  "gatePath": [
    {
      "id": "attack",
      "to": 1.0,
      "duration": { "ms": 6.0 },
      "curve": { "type": "exponential", "amount": 0.70 }
    },
    {
      "id": "decay",
      "to": 0.62,
      "duration": { "ms": 95.0 },
      "curve": { "type": "exponential", "amount": -0.20 }
    }
  ],
  "sustain": {
    "mode": "hold"
  },
  "releasePath": [
    {
      "id": "release",
      "to": 0.0,
      "duration": { "ms": 180.0 },
      "curve": { "type": "exponential", "amount": 0.35 }
    }
  ],
  "retrigger": "fromCurrent",
  "velocity": {
    "target": "peak",
    "amount": 0.85
  }
}
```

A gate rising edge starts `gatePath`. If the gate remains high, the voice holds at sustain. A falling edge branches immediately into `releasePath` from the current value, even if the attack has not finished.

## 2. Contour envelopes

This is the Wyrm-like form for arbitrary shapes:

```json
{
  "id": "glass_double_bloom",
  "name": "Glass Double Bloom",
  "kind": "contour",
  "mode": "oneShot",
  "duration": { "beats": 0.75 },
  "points": [
    { "t": 0.00, "v": 0.00 },
    { "t": 0.04, "v": 0.94 },
    { "t": 0.16, "v": 0.31 },
    { "t": 0.34, "v": 0.78 },
    { "t": 0.58, "v": 0.43 },
    { "t": 1.00, "v": 0.00 }
  ],
  "interpolation": "monotoneCubic",
  "retrigger": "restart"
}
```

The authoring language can permit 32–256 points, but the runtime compiler transforms both forms into the same immutable representation.

A later expanded editor can draw contours freely, while the AI can generate them numerically from descriptions such as:

> “A fast glassy strike, a hollow dip, then a softer second bloom that decays over three-quarters of a beat.”

## Supported behavior

The complete program model should include:

* `oneShot`, `gate`, and `cycle` modes.
* Millisecond, second, or beat-based durations.
* Linear, exponential, logarithmic, smoothstep, sigmoid, cubic, hold, and step curves.
* One sustain region.
* Optional counted or while-gate loops.
* Forward, reverse, or ping-pong looping.
* Delay and hold stages.
* Restart, from-current, legato, ignore-while-running, and tempo-aligned retriggering.
* Per-trigger deterministic variation.
* 0–10 V, 0–5 V, and ±5 V lane output modes.
* Configurable end-of-cycle and end-of-loop pulses.

Output polarity and range should be a **lane property**, not a channel-program property. Every channel in one poly cable should obey the same electrical convention.

# Macro modulation

Each program can declare mappings from `VEL`, `M1`, `M2`, and `M3` to semantic targets:

```json
{
  "macroBindings": [
    {
      "source": "m1",
      "target": "timeScale",
      "inputRange": [-10.0, 10.0],
      "outputRange": [0.25, 4.0],
      "curve": "exponential",
      "sampling": "onTrigger"
    },
    {
      "source": "m2",
      "target": "curveBias",
      "inputRange": [-10.0, 10.0],
      "outputRange": [-0.8, 0.8],
      "sampling": "continuous",
      "smoothingMs": 5.0
    },
    {
      "source": "m3",
      "target": "variantSelect",
      "variants": [
        "bass_short",
        "bass_medium",
        "bass_long"
      ],
      "sampling": "onTrigger"
    }
  ]
}
```

The distinction between `onTrigger` and `continuous` is important.

Time, program selection, loop counts, and other structural decisions should normally be captured on a trigger edge. Continuously changing those values halfway through a segment can produce jumps or pathological time warping. Level, offset, and gentle curvature modulation can safely remain continuous with smoothing.

# The panel

I would make the **expanded graphical editor optional**, but not the compact visual itself.

A machine-configured envelope module without any visible contour becomes a small black box full of invisible dragons. A modest display is not decoration here; it is operational feedback.

```text
┌────────────── MOIRAI  [AI] ──────────────┐
│ A   CH 03   bass_amp          REV 12     │
│                                          │
│       ╭────╮                              │
│   ───╯    ╰──────────────                 │
│              ●                           │
│  01 02 [03] 04 05 06 ... 16              │
│                                          │
│ [A/B]   [CHANNEL / PRESET ENCODER]       │
│       TIME       CURVE       LEVEL        │
│                                          │
│ GATE VEL  M1  M2  M3  CLOCK RESET       │
│       A   EOC-A       B   EOC-B          │
└──────────────────────────────────────────┘
```

The display would show:

* The selected lane and channel.
* Program name and active revision.
* The selected contour.
* A moving playhead for the inspected voice.
* A 16-channel activity strip.
* Pending revision or validation status.
* Dynamic names for the three macro mappings.
* Clock-relative duration when applicable.

Lane A and B can appear simultaneously as two luminous threads, with the selected lane brighter. The visual can remain lightweight NanoVG; this module does not need to inherit Wyrm’s substantial GLSL and sand-rendering machinery.

## Fixed panel controls

The three knobs should have stable, automation-safe meanings:

* **TIME:** global logarithmic duration scale.
* **CURVE:** global curvature/temporal-skew bias.
* **LEVEL:** global output depth.

They should modify the compiled program non-destructively rather than rewriting its authored points.

The encoder selects the inspected channel. Pressing it opens the factory preset list. Holding it toggles between the selected channel and `ALL`.

A small A/B button selects the edited lane. A manual trigger action can be bound to encoder double-click or a small dedicated button.

## Factory presets

The readily accessible set should include:

* AD / Percussive.
* AR.
* ADSR.
* AHR.
* DADSR.
* Gate / Trapezoid.
* Pluck.
* Pad.
* Swell.
* Duck.
* Cyclic triangle.
* Cyclic sine.
* Wyrm AR and D1–D10 contours.
* Custom.

Choosing a preset creates or updates an ordinary program through the same semantic edit machinery. There is no hidden preset-only state.

# Octavia integration

## Do not clone `SibylControl` into another one-off island

This is the point where I would extract the reusable semantic-control layer.

The existing `SibylControl` header itself says that the C++ RTTI adapter is optional and not intended as the public cross-plugin ABI; the JSON protocol is the real module-owned interface. That makes the extraction fairly natural. ([GitHub][1])

Something like:

```cpp
struct OctaviaSemanticControl {
    enum class Operation {
        CAPABILITIES,
        GET_DOCUMENT,
        VALIDATE,
        EDIT,
        GET_STATUS,
        COMMAND
    };

    virtual ~OctaviaSemanticControl() = default;

    virtual const char* semanticCapabilityId() const noexcept = 0;

    virtual bool handleSemanticRequest(
        Operation operation,
        const std::string& requestJson,
        std::string& responseJson,
        std::string& error
    ) = 0;
};
```

Sibyl would eventually advertise something like:

```text
leviathan.sibyl.composition
```

Moirai would advertise:

```text
leviathan.moirai.envelope-bank
```

Octavia could expose generic routes:

```text
/semantic/{moduleId}/capabilities
/semantic/{moduleId}/document
/semantic/{moduleId}/validate
/semantic/{moduleId}/edit
/semantic/{moduleId}/status
/semantic/{moduleId}/command
```

The existing `/sibyl/...` routes and `vcv_sibyl_*` tools can remain as compatibility aliases.

At the MCP layer, I would still provide strongly named wrappers:

```text
vcv_moirai_get_capabilities
vcv_moirai_get_bank
vcv_moirai_get_program
vcv_moirai_validate
vcv_moirai_edit
vcv_moirai_get_status
vcv_moirai_command
```

That gives the LLM clear tool descriptions and input validation without duplicating all of Octavia’s internal queue, dispatch, response-validation, and undo machinery.

## Semantic operations

A Moirai edit transaction should support operations such as:

```text
replace_bank
upsert_program
delete_program
clone_program
apply_preset
assign_program
set_channel_label
set_lane_defaults
set_macro_binding
set_output_mode
set_clock
```

As with Sibyl, operations should be applied in order to a private copy, followed by one complete validation and compilation pass. The accepted state is untouched if any operation fails. Sibyl’s current edit system already follows this private-copy-and-compile-once pattern. ([GitHub][6])

An edit request might look like:

```json
{
  "expected_revision": 11,
  "apply_at": "nextTrigger",
  "active_voice_policy": "finishCurrent",
  "operations": [
    {
      "op": "upsert_program",
      "id": "bass_amp",
      "program": {
        "kind": "staged",
        "mode": "gate",
        "gatePath": [
          {
            "to": 1.0,
            "duration": { "ms": 6.0 },
            "curve": {
              "type": "exponential",
              "amount": 0.7
            }
          },
          {
            "to": 0.62,
            "duration": { "ms": 95.0 },
            "curve": {
              "type": "exponential",
              "amount": -0.2
            }
          }
        ],
        "sustain": {
          "mode": "hold"
        },
        "releasePath": [
          {
            "to": 0.0,
            "duration": { "ms": 180.0 },
            "curve": {
              "type": "exponential",
              "amount": 0.35
            }
          }
        ]
      }
    },
    {
      "op": "assign_program",
      "lane": "A",
      "channels": [0],
      "program_id": "bass_amp"
    }
  ]
}
```

A successful request produces one undo entry. Cable creation remains a separate Octavia patching action, exactly as it is now.

## Read views

A full bank containing freeform contours can be unnecessarily expensive in tokens, so the read API should provide:

* `summary`: program names, modes, durations, assignments, and macro targets.
* `program`: one complete program.
* `channel`: both assignments and labels for one channel.
* `lane`: lane defaults and channel overrides.
* `full`: authoritative complete bank.

The summary should include computed characteristics such as peak, sustain level, nominal gate-path duration, release duration, loop state, output range, and point count. This allows an agent to reason about the bank without repeatedly retrieving hundreds of contour points.

# Revision adoption and real-time safety

Moirai should mirror Sibyl’s distinction between **accepted revision**, **active revision**, and **pending revision**, but the most useful adoption boundary is different. Sibyl currently maintains accepted and active composition pointers plus pending adoption state, and exposes those distinctions through status. ([GitHub][3])

For envelopes, I would support:

| `apply_at`    | Behavior                                    |
| ------------- | ------------------------------------------- |
| `immediate`   | Adopt now according to active-voice policy  |
| `nextTrigger` | New triggers use the new revision           |
| `allIdle`     | Swap after every active voice has completed |
| `nextClock`   | Adopt on the next configured clock boundary |

The default should be:

```text
apply_at = nextTrigger
active_voice_policy = finishCurrent
```

That means an agent can reconfigure the bass envelope while a long pad release is sounding. Existing voices finish from the immutable program revision with which they started; new notes use the new revision.

Each active voice therefore retains a pointer to its compiled bank generation. The bank object carries an atomic active-voice reference count:

* Increment only when a voice begins using the generation.
* Decrement when the voice becomes idle.
* Reclaim old generations from the UI/control side only when they are no longer accepted, active, pending, or voice-referenced.

There should be no locks, allocation, JSON work, container mutation, or reference-counted smart-pointer traffic on every sample.

Each runtime voice needs only compact fixed state:

```cpp
struct EnvelopeVoice {
    const CompiledBank* bank = nullptr;
    const CompiledProgram* program = nullptr;

    int stageIndex = 0;
    float stagePhase = 0.f;
    float stageStart = 0.f;
    float value = 0.f;

    bool gateHigh = false;
    bool running = false;
    bool releasing = false;
};
```

With two lanes and sixteen channels, evaluating 32 voices is modest work: a gate edge detector, stage advancement, one curve evaluation, and output scaling per active voice.

# Clock behavior

Beat-based programs should use the same general external-clock estimator concept already present in Sibyl. The bank stores:

```json
{
  "clock": {
    "externalPpqn": 4,
    "fallbackBpm": 120.0,
    "onClockLoss": "holdTempo"
  }
}
```

Octavia can read Sibyl’s composition clock settings and configure Moirai to match before connecting the reconstructed clock output.

Beat durations should remain musical when tempo changes:

* A stage authored as `0.25 beats` follows tempo continuously.
* A stage authored as `80 ms` remains absolute.
* A program may mix the two if the schema allows it.
* Structural macro values remain trigger-sampled unless explicitly configured otherwise.

# Runtime status

The status API should expose enough information for verification without streaming audio-thread internals:

```json
{
  "revision": 12,
  "activeRevision": 12,
  "pendingRevision": null,
  "channels": 8,
  "clock": {
    "connected": true,
    "estimatedBpm": 127.98,
    "externalPpqn": 4
  },
  "lanes": {
    "A": {
      "activeMask": 5,
      "selectedChannel": 2,
      "program": "bass_amp",
      "stage": "decay",
      "value": 0.71
    },
    "B": {
      "activeMask": 5,
      "selectedChannel": 2,
      "program": "bass_filter",
      "stage": "contour",
      "value": 0.43
    }
  }
}
```

The audio thread can publish a rate-limited immutable telemetry snapshot, perhaps 30–60 times per second. The display and MCP status read that snapshot rather than inspecting mutable voice objects directly.

# A particularly potent later feature

The Octavia observation work already appearing in the branch creates the path toward a genuine closed configuration loop. Sibyl compositions can attach observation instructions to steps, including an Octavia module target, monitor selection, pre/post capture frames, and a label. ([GitHub][7])

That eventually permits a workflow like:

1. The AI creates a bass envelope and patches Moirai A to the VCA.
2. Sibyl triggers a labeled test note.
3. Octavia captures either the envelope CV or resulting audio through one of its monitor inputs.
4. The agent compares the observed result with the requested contour.
5. It performs one revised semantic edit.

That is not required for Moirai v1, but the architecture above makes it possible without changing the envelope DSP.

# Choices I would lock now

I would consider these resolved enough for a formal implementation specification:

1. **Dual-lane, sixteen-channel polyphony in 12 HP.**
2. **Ordinary Rack cables are the only musical coupling to Sibyl.**
3. **Program bank plus per-lane/per-channel assignments.**
4. **Basic presets and freeform contours compile into one engine.**
5. **A small contour/status display is included; a large editor is optional.**
6. **Velocity plus M1–M3 provide per-note control from Sibyl.**
7. **Semantic edits are revisioned, validated, atomic, and undoable.**
8. **Default adoption occurs on the next trigger while existing voices finish.**
9. **Moirai becomes the first user of a generalized Octavia semantic-control layer rather than another bespoke `/module-name/` integration.**
10. **The audio engine is entirely local, deterministic, allocation-free, and independent of whether an LLM remains connected.**

The result would feel less like “an ADSR with an AI API” and more like a **polyphonic envelope fabric**: Sibyl decides *what happens*, while Moirai decides *how each event enters, persists, transforms, and disappears*. This is sufficiently resolved to become a Codex-ready module and protocol specification.

[1]: https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/SibylControl.hpp "https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/SibylControl.hpp"
[2]: https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/Octavia.cpp "https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/Octavia.cpp"
[3]: https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/Sibyl.cpp "https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/Sibyl.cpp"
[4]: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/Wyrm.cpp "https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/Wyrm.cpp"
[5]: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/Wyrm.hpp "https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/Wyrm.hpp"
[6]: https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/SibylEdit.hpp "https://github.com/PlasmaChroma/Leviathan-Rack2/raw/refs/heads/expander/src/SibylEdit.hpp"
[7]: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/SibylJSON.cpp "https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/expander/src/SibylJSON.cpp"
