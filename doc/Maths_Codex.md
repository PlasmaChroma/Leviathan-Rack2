Dragon King Leviathan — perfect. Here’s a **Codex-ready implementation markdown** that **only “locks in” what we know** from the spec/manal-derived statements, and **explicitly flags the unknowns** as TODOs (so you can tighten later without rewriting architecture).

---

# Maths (Revision 2013 / “Maths 2”) — VCV Rack 2 Emulation

## Codex-ready Implementation Specification (Known-Behavior First)

### Purpose

Implement a VCV Rack 2 module that emulates the **documented/verified behaviors** of the dual slope generators (CH1/CH4), attenuverting offset channels (CH2/CH3), and SUM/OR bus logic as described in the behavioral spec.

### Non-goals (for this pass)

We will not attempt hardware-indistinguishable “feel” until measurement provides exact constants (time laws, CV scalings, saturation curves, comparator hysteresis).

---

## 1) Versioning & feature set

### Primary target: Revision 2013

* Adds **Cycle Input** (gate-qualified cycling), **INV SUM output**, and **Unity Signal Outputs** for CH1/CH4.
* CH2 input normalizes to **+10 V**, CH3 to **+5 V** when unpatched.

### Optional compatibility mode: Classic (Maths 1)

* No INV SUM output.
* CH2 & CH3 normalize to **+5 V**.
* Bus normalization differs (see §5).

**Implementation approach:** build one module with a compile-time or runtime flag `revision = REV_2013 | REV_CLASSIC`, defaulting to REV_2013.

---

## 2) Voltage conventions & Rack interoperability

### Verified module-side ranges (Revision 2013)

* CH1/CH4 Signal inputs: **±10 V**.
* CH1/CH4 Rise/Fall/Both CV inputs: **±8 V**.
* Variable outputs (1–4): **±10 V**.
* SUM and INV SUM: **±10 V**.
* OR output: **0–10 V** and **does not respond to negative voltages**.
* EOR/EOC: **0 or 10 V** binary levels.

### Trigger/gate detection in Rack

Use Rack’s recommended Schmitt trigger thresholds for trigger inputs unless otherwise specified. ([VCV Rack][1])

* Trigger detect: `dsp::SchmittTrigger::process(x, 0.1f, 1.0f)` (or 2.0f) ([VCV Rack][1])
* Trigger pulse generation (if needed): `dsp::PulseGenerator` with 1 ms pulses ([VCV Rack][1])

**Exception:** Cycle Input has a known HIGH threshold (see §4.4).

---

## 3) Module interface (params/inputs/outputs/lights)

> Names here are **implementation IDs**; UI labeling can match panel art later.

### Par_PARAM` (Rise knob)

* `CH1_FALL_PARAM` (Fall knob)
* `CH1_SHAPE_PARAM` (Vari-Response)
* `CH1_CYCLE_PARAM` (Cycle button latch)
* `CH1_ATTENUVERT_PARAM` (center attenuverter driving Var Out 1)

**Channel 2**

* `CH2_ATTENUVERT_PARAM` (attenuverter / offset scaler)

**Channel 3**

* `CH3_ATTENUVERT_PARAM` (atte

**Channel 4**

* `CH4_RISE_PARAM`
* `CH4_FALL_:contentReference[oaicite:24]{index=24} `CH4_CYCLE_PARAM`
* `CH4_ATTENUVERT_PARAM`

### Inputs

**Channel 1**

* `Cd)
* `CH1_TRIGGER_IN` (gate/pulse trigger)
* `CH1_RISE_CV_IN` (linear)
* `CH1_FALL_CV_
* `CH1_BOTH_CV_IN` (bipolary) 3 only, gate-qualified) hannel 2**
* `CH2_SIGNAL_IN` (normalized reference when unpatc

**Channel 3**

* `CH3_SIGNAL_IN` (normalized

**Channel 4**

* `CH4_SIGNAL_IN`
* `CH4_TRIGGE:contentReference[oaicite:40]{index=40}CH4_FALL_CV_IN`
* `CH4_BOTH_CV_IN`
* `CH4_CYCLE_IN` (REV_2013 only) ## Outputs
* `CH1_UNITY_OUT` (not affected by attenuverter; 0–8 V when cycling; otherwise follows input amplitude)  ed CH1; ±10; bus-normalled unless patched)
* `CH2_VAR_OUT`
* `CH3_VAR_OUT`
* `CH4_UNITY_:contentReference[oaicite:46]{index=46}UT` (sum of attenuverted variable outs)
  -; -SUM)
* `OR_OUT` (max selector over positives only)    event, 0/10, default low idle) _OUT` (CH4 end-of-cycle event, 0/10, default high

### Lights (Revision 2 and state LEDs for SUM and EOR/EOC exist as a feature update.

brightness to the associated latch/state described in DSP sections.

---

## 4) DSP architecture

### 4.1 Polyphony

No polyphony support on this module -- all inputs and outputs are monophonic

### 4.2 Common helper conventions (deterministic even if later tuned)

* Use sample time `dt = args.sampleTime`
* Use clamps to respect output ranges:

  * `clamp10(x) = clamp(x, -10, +10)`
  * `clampPos10(x) = clamp(x, 0, 10)` (OR output)
* **TODO/UNKNOWN:** soft saturation vs hard clamp (hardware may clip non-ideally).

### 4.3 CH2 & CH3 attenuverting offset/mix channels (Verified + minimal deterministic choices)

**Verified behaviors**

* Inputs normalize to reference voltages when unpatched for offset generatio CH3 = +5 V

  * Rev 2013: CH2 = +10 V, CH3 = +5 V
* Attenuverter: NOON = 0, CW = max positive, CCW = max negative/inverted; described as having a “small amount of gain.” Codex-ready implementation**
* `in = inpu:contentReference[oaicite:69]{index=69}oltage(c) : normRef`
* `gain = mapAttenuverter(param)` such that noon=0, ends near ±1
* `out = clamp10(in * gain)`
* **TO of gain” exact max (1.2? 2.0?) requires measurement.

### 4.4 CH1 & CH4 slope core (Verified behavior contract)

#### 4.4.1 Dual role: processing + function generation

* Signal inputs are direct-coupled and used for lag/portamento/ASR processing.
* Trigger input causes a transient described as **0 → 10 → 0** with **NO sustain**.
* Trigger is **retriggerable during FALLING** but **not during RISING**. #### 4.4.2 Rise/Fall knobs and CV semantics
* Rise knob CW increases rise time; Fall kn
* Rise CV and Fall CV are **linear**: positiveative decreases time (faster), relative to knob setting.
* Both CV is **bipolar exponeotal time (faster), negative increases total time (slower), and is inverse polarity vs Rise/Fall CV.

# family

* LOG: rate of change decreases as voltage increases
* LINEAR: constant rate
* EXPO: rate of change increases as voltage increases
  …and includes eve
* Response curve adjustment also affects Rise/Fall times.

#### 4.4.4 Cycle behavior (Revision 2013)

* Cycle can be engaged by Cycle button or Cycle input; both late).
* Cycle Input is gate-+2.5 V**.

---

## 5) Normalization, bus injection, and mix logic (Verified)

### Revision 2013 bus rules

* Va to SUM and OR buses. If a Variable Out jack is unpatched, it cohannel is removed from both busses (“break-normal”).
* Unity Signal Outs (CH1/CH4) are **not** normalled into SUM/OR, and patching them does not remove anything.

### Classic bus rules (optional mode)

* All four Si to SUM/OR, and patching removes them.
* CH1/CH4 provide a separate “Signal OUT he buses.

### SUM / INV SUM / OR math

Let `v1..v4` be the **Variable Out signals** for cha*NOT** patched; otherwise that `vi = 0` (per break-normal).  + v2 + v3 + v4)`

* `INV_SUM = clamp10(-SUM)` (REV_2013 only)
* `OR = clampPos10(max(0, v1, v2, v3, v4))` (do

**TODO/U OR non-idealities are not specified; default t

---

## 6) Outer-channel state machine (Codex-ready vides a deterministic model that satisfies all verified constraints. Unknown analog “feel” constants are markedte (CH1 and CH4 separately)

```text
enum Phase { IDLE, RISING, FALLING }

struct SlopeVoice {
  Phase phase;
  float y;              // core slope output (Unity domain)
  bool transientMode;   // true when triggered by TRIG input
  bool cycleLatched;    // from cycle button (toggle)
  // optional: last input for slew mode
}
```

### 6.2 Inputs per voice

For each voice `c`, read:

* `xSignal` from SIGNAL_IN (or 0 if unpatched)
* `trig` from TRIGGER_IN (Schmitt trigger)
* `riseCV`, `fallCV`, `bothCV` from respective CV inputs
* `cycleGate` from CYCLE_IN (REV_2013 only; treat as HIGH if ≥ 2.5 V)

### 6.3 Cycle enabled

`cycleEnabled = cycleLatched || cycleGateHigh`

**TODO/UNKNOWN:** behavior when cycle gate drops mid-cycle (stop immediately vs end-of-cycle) requires measurement; choose

**Default (placeholder):**

* If `cycleEnabled` becomes false, finish current segment then go IDLE.

### 6.4 Trigger acceptance rule (Verified)

* Trigger-generated transient is **0→10→0, no sus
* Retrigger ignored during RISING, accepted during FALLING.

**Codex-ready behavior:**

* On trigger event:

  * If `phase != RISING`: set `phase=RISING`, set `tratop = +10 V, target bottom = 0 V
  * If `phase == RISING`: ignoetrigger “restart semantics” (restart from current y vs hard reset to 0) is not specified; choose restart-from-current for smoothness unless measurement contradicts.

### 6.5 Slew/lag mode (Verified)

* Maths cannot increase rate of an external voltage; it can only slow it or allow it through.
* Gate applied to Signal Input yi sustain while gate is high, then fall to 0.

**Codex-ready behavior:**

* If not in `transientMode`

  * treat as asymmetric slew limiter tracking `xSignal` with Rise limiting upward movement and Fall limiting ame shape family as Vari-Response (timing coupling noted)

### 6.6 Rise/Fall time computation (Known semantics + TODO constants)

**Known facts**

* Rise/Fall CV are linear time controls; Both CV is bipolar exponential and inverse polarity. ready placeholder model (must be marked TODO(measure))**

1. Start with knob-derived base times:

* `riseBaseSec = mapRiseKnob(params[RISE])`
* `fallBaseSec = mapFallKnob(params[FALL])`
  ve offset in a “time control domain”:
* `riseCtrl = riseBaseCtrl + kLinear * riseCV`
* `fallCtrl = fallBaseCtrl + kLinear * fallCV`

3. Convert ctrl back to seconds:

* `riseSec = ctrlToSec(riseCtrl)`
* `fallSec = ctrlToSec(fallCtrl)`

4. Apply Both CV as exponential rate scaling:

* `scale = exp(-kBoth * bothCV)` (positive bothCV speeds up)
* `riseSec *= scale; fallSec *= scale`

**TODO/UNKNOWN:** `mapRiseKnob`, `mapFallKnob`, `kLinear`, `kBoth`, and whether ctrl domain is log-time require measurement.  e mapping (Known family + TODO curve constants)
**Known facts**

* LOG/LIN/EXPO behavior and “everything in-between.”
* Shape affects tim

**Codex-ready placeholder mapping**

* Let `shape` be [-1..+1] mapped from knob, where 0 = linear tick.
* Use a power curve parameteart, slow-end (LOG-like for rising `p > 1` => slow-start, fast-end (EXPO-like)
* Apply the curve to normalized segment progress.

**TODO/UNKNOWN:** exact mapping to “Hyper-Exponential” range and the time-coupling factor.

---

## 7) Outputs & attenuverters (Verified routing)

### 7.1 Unity outputs (CH1 & CH4)

* Unity Signal Output is tapped from core, not affected by attenuverter.
  n cycling; otherwise follows input amplitude.**

**TODO/UNKNOWN:** triggered transient peak on Unity Out (8 V vs 10 V) is explicitly m

### 7.2 Variable outputs (CH1–CH4)

* Center attenuverted/generated signal and drives Variable Out and bus injection.
* Variable Out

**Codex-ready behavior:**

* `var = clamp10(unity * attenGain)` where `attenGain` is bipolar around 0 at noon.

---

## 8) EOR/EOC o default interpretation)

### 8.1  events

* EOR (CH1): 0/10 V, defaults low when idle; “event” is reaching highest voltage.
* EOC (CH4): 0/10 V, defaults high when idle; “event” is reaching lowest voltage.

### 8.2 Codex-ready default (phase-gate model)

Because patch guidance treats E with width controlled by Rise/Fall, implement them as **segment-phase gates** (not 1

**Default mapping:**

* `EOR = 10V if (phase == FALLING) else 0V`
* `EOC = 10V if (phase != FALLING) else 0V`

**TODO/UNKNOWN:** coses; measurement plan calls this out.

---

## 9) Explicit TODO / UNKNOWN list (lehtening)

These are **intentionally not locked** in this pass (Codex must leave clear markers):

1. Rise/Fall knob law (
2. Both CV exponential scaling constant (V/oct? V/decade?)
3. Vari-Response mapping and time-coupling constant
4. Analog saturation/csoft clip)
5. Trigger input threut threshold is known; Trigger isn’t)  “restart semantics” during FALLING (reset-to-0 vs restart-from-c
6. Cycle Input run/stop edge case when gate drops mid-cycle amplitude specifics in triggered/cycle modes (8 V vs 10 V) ality (default to ideal)

---

or Codex (Rack SDK idioms)

* Use `dsp::SchmittTrigger` for trigger detectifor any 1 ms trigger outputs you choose to add or debu([VCV Rack][1])
* Standard processing primitives: `params[]`, `inputs[]`, `outputs[]`, `lights[]`. ([VCV Rack][3])

---

### What you can do next (optional, but extremely high leverage)

If you paste this markdown into Codex, it should generate a full module skeleton and DSP loop with **all verified behaviors implemented** and **every ambiguity clearly TODO-tagged**.

If you want, I can also produce a **second markdown file** that is *pure TODO calibration scaffolding* (constants, test harness patch suggestions, expected plots) so you can tighten the feel later without touching DSP structure.

[1]: https://vcvrack.com/manual/VoltageStandards?utm_source=chatgpt.com "Voltage Standards - VCV Rack Manual"
[2]: https://vcvrack.com/manual/PluginGuide?utm_source=chatgpt.com "Plugin API Guide - VCV Rack Manual"
[3]: https://vcvrack.com/manual/PluginDevelopmentTutorial?utm_source=chatgpt.com "Plugin Development Tutorial - VCV Rack Manual"
