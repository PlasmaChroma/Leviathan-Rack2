# Sibyl – Software Specification

## 1. Overview & Philosophy
**Sibyl** is a headless, machine-first polyphonic sequencer and arranger module for VCV Rack. Designed to pair with the Octavia MCP bridge, Sibyl delegates the tedious aspects of step-sequencing and composition to an AI agent, while maintaining sub-sample accurate, jitter-free DSP audio-rate clocking and CV generation on the Rack side.

**Core Directives:**
* **No Manual Step Entry:** The panel has zero knobs or buttons for inputting notes. The sole interface for composition is JSON via `vcv_set_module_state`.
* **Token-Efficient Architecture:** The JSON schema separates reusable *Patterns* from the linear *Arrangement*, making it extremely efficient for LLMs to compose, repeat, and vary structures without writing massive, redundant step arrays.
* **Polyphonic Density:** Sibyl utilizes VCV Rack's polyphonic cables to transmit up to 16 independent tracks of V/Oct, Gate, Velocity, and Mod out of just four physical ports, keeping the HP footprint tiny while outputting a massive amount of data.

---

## 2. Hardware Interface (The Panel)

Sibyl acts as the "Ghost in the Machine." Its panel is austere, focusing purely on clock sync, macro modulation, and signal output.

### 2.1 Inputs
* **CLOCK IN**: Advances the playhead. If unpatched, Sibyl uses its internal JSON-defined BPM.
* **RUN IN**: Gate high = playing, low = paused.
* **RESET IN**: Trigger to snap the playhead back to the start of the Arrangement.
* **SCENE TRIG IN**: Trigger to advance to the next scene/section in the arrangement early.
* **SCENE CV IN**: 0-10V input to directly address and select a specific scene/section.
* **MACRO 1-4 IN**: 4 assignable CV inputs. The JSON state can map these to any track properties (e.g., global probability, swing, ratchet density).

### 2.2 Outputs (Polyphonic 1-16 Channels)
* **V/OCT OUT**: Polyphonic pitch output.
* **GATE OUT**: Polyphonic gate/trigger output.
* **VELOCITY OUT**: Polyphonic velocity (0-10V).
* **MOD OUT**: Polyphonic assignable continuous modulation (0-10V or -5V to +5V).
* **CLOCK OUT**: Passes the internal or external clock for syncing other modules.
* **EOC / SCENE OUT**: Fires a trigger at the End of a Cycle, or when transitioning to a new scene.

### 2.3 Visual Display
* A central OLED or crisp LED matrix. It does not allow editing. It simply displays:
  * The current `meta.title` or `meta.prompt`.
  * The active Scene Name (e.g., "Chorus B").
  * A scrolling multi-track lane or matrix showing playheads and active gates.

---

## 3. The JSON State Model (The Machine Interface)

Sibyl's state relies on a hierarchy designed for LLMs: **Meta -> Patterns -> Arrangement**. 

By storing the *Prompt* and *Scale/Key* inside the module state, an AI can query Sibyl months later, read the `meta` block, and completely understand the compositional intent before making changes.

### 3.1 JSON Schema Structure

```json
{
  "meta": {
    "title": "Abyssal Techno",
    "prompt": "Deep 130BPM techno. Track 1 is a sparse sub bass, Track 2 is a generative polymetric FM lead in F Dorian.",
    "bpm": 130.0,
    "root": "F",
    "scale": "dorian",
    "swing": 0.12
  },
  "patterns": {
    "bass_verse": {
      "length": 16,
      "resolution": "1/16",
      "steps": [
        { "step": 0, "pitch": -2.0, "gate": 0.8, "vel": 1.0 },
        { "step": 7, "pitch": -2.0, "gate": 0.2, "vel": 0.6, "prob": 0.8 },
        { "step": 14, "pitch": -1.833, "gate": 0.5, "vel": 0.9, "slide": 100 }
      ]
    },
    "lead_poly": {
      "length": 7,
      "resolution": "1/8",
      "steps": [
        { "step": 0, "pitch": 0.0, "gate": 0.5, "vel": 0.7, "ratchet": 3 },
        { "step": 3, "pitch": 0.25, "gate": 0.1, "vel": 0.5 }
      ]
    }
  },
  "arrangement": [
    {
      "scene": "Intro",
      "repeats": 4,
      "tracks": {
        "0": "bass_verse",
        "1": null
      }
    },
    {
      "scene": "Verse A",
      "repeats": 8,
      "tracks": {
        "0": "bass_verse",
        "1": "lead_poly"
      }
    }
  ],
  "macros": {
    "1": { "target": "global_probability", "amount": 1.0 },
    "2": { "target": "track_1_swing", "amount": 0.5 }
  }
}
```

### 3.2 Schema Breakdown
1. **Sparse Step Arrays:** Note that `steps` in a pattern only define *active* events (`"step": 7`). The LLM does not need to write 16 objects full of zeros for empty rests. This drastically reduces token consumption.
2. **Polymeter & Phase:** Patterns define their own `length` and `resolution` (e.g., 7 steps of 1/8th notes). When mapped to a track in the arrangement, they loop independently, allowing instant generative polymeters.
3. **Arrangement & Cycles:** The `arrangement` array defines the song structure. A scene plays for the duration of its longest looping track multiplied by `repeats`. Once finished, Sibyl automatically fires the **SCENE OUT** trigger and advances to the next scene. 
4. **Scale Awareness (Optional but recommended):** If `pitch` is a float, it is direct V/Oct. However, the C++ DSP could optionally interpret integers as *Scale Degrees* based on the `meta.scale` and `meta.root`, allowing the AI to sequence `{"pitch": 3}` and have Sibyl automatically calculate the correct microtonal or chromatic V/Oct float internally.

---

## 4. Playback Logic & Sync Engine

### 4.1 Clocking Mode
* **Internal Mode (Unpatched CLK IN):** Sibyl uses a high-precision internal accumulator based on `meta.bpm`. It calculates time deltas per audio frame (e.g., `sampleTime = 1.0 / engineGetSampleRate()`).
* **External Mode (Patched CLK IN):** Sibyl tracks the delta time between incoming clock triggers, applies a PLL (Phase-Locked Loop) to smooth jitter, and derives internal sub-step ticks for ratchets and microtiming.

### 4.2 Interpolation & Slew
The `slide` property (in ms or percentage of step) tells Sibyl's DSP engine to apply a one-pole lowpass filter or linear interpolation to the `V/OCT` and `MOD` outputs, giving the AI the ability to program buttery 303-style glides without relying on external slew limiters.

### 4.3 Probability & Generative Evaluation
At the exact start of a step trigger, Sibyl's DSP evaluates:
`if (random::uniform() <= step.prob + macroOffset)`
If false, the gate is skipped. This logic must execute on the audio thread to ensure sample-accurate random masking.

---

## 5. Agent Interaction Workflow (via Octavia MCP)

When the user asks the AI to *"write a bassline"*, the workflow is:
1. Agent calls `vcv_get_module_state(sibyl_id)`.
2. Agent reads the `meta` context and current `arrangement`.
3. Agent synthesizes a new `pattern` JSON block (e.g., `"bass_variation_1"`) and updates the `arrangement` block.
4. Agent calls `vcv_set_module_state(sibyl_id, new_json)`.
5. Sibyl's `dataFromJson()` locks its internal playback mutex, updates the pattern memory and arrangement queue, and unlocks. The new sequence begins seamlessly on the next clock tick.
