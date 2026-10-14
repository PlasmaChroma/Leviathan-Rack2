# Cantor
## Adaptive & Alternative Pitch Quantizer for VCV Rack

**Working title:** Cantor  
**Project:** Leviathan Rack  
**Status:** Exploratory implementation specification  
**Design maturity:** Architecture-ready; musical behavior intentionally expected to evolve through use  
**Primary concept:** A V/oct pitch processor capable of conventional quantization, non-12-TET tuning systems, rational/harmonic tuning, non-octave pitch spaces, and event-driven adaptive pitch interpretation.

---

# 1. Concept

Cantor is a generalized pitch interpretation module for VCV Rack.

At its simplest, Cantor behaves like a conventional quantizer:

> continuous pitch CV → nearest permitted pitch → V/oct output

However, Cantor is designed around a broader abstraction than a traditional scale quantizer.

Instead of assuming:

- 12 notes per octave,
- equal temperament,
- octave repetition,
- fixed pitch locations,
- or deterministic one-input-to-one-output behavior,

Cantor treats incoming pitch CV as a location or **musical intention within pitch space**.

Depending on the selected tuning system and behavior, Cantor may:

- quantize continuously,
- quantize only on trigger events,
- hold notes after selection,
- use unequal temperament,
- use arbitrary rational frequency relationships,
- operate with periods other than 2:1 octaves,
- evaluate pitches relative to currently active notes,
- choose different valid notes from identical input CV on successive triggers,
- or dynamically retune voices according to harmonic context.

The core design goal is that all output remains ordinary VCV Rack-compatible **1 V/oct CV**.

No custom pitch transport format is required.

---

# 2. Design Philosophy

Cantor should separate three concepts that traditional quantizers often conflate.

## 2.1 Pitch Space

Defines how pitch is represented geometrically.

Examples:

- octave-periodic,
- tritave-periodic,
- arbitrary repeating period,
- potentially non-repeating harmonic space.

---

## 2.2 Tuning System

Defines which pitches are musically meaningful or available.

Examples:

- 12-TET,
- 19-EDO,
- 31-EDO,
- just intonation,
- harmonic-series-derived Dragon tuning,
- imported Scala tuning,
- adaptive rational Culture-inspired tuning.

---

## 2.3 Decision Behavior

Defines when and how a pitch is selected.

Examples:

- nearest pitch continuously,
- nearest pitch on trigger,
- contextual pitch on trigger,
- probabilistic selection among good candidates,
- dynamically retune active voices.

These concepts should remain separated internally even if some presets automatically select sensible combinations.

---

# 3. Fundamental Pitch Representation

Rack pitch remains standard floating-point V/oct throughout Cantor.

For an arbitrary frequency ratio:

\[
r = \frac{f}{f_0}
\]

the corresponding pitch offset is:

\[
V = \log_2(r)
\]

Therefore:

- octave `2:1` = `1.0 V`
- perfect fifth `3:2` ≈ `0.5849625 V`
- just major third `5:4` ≈ `0.3219281 V`
- harmonic seventh `7:4` ≈ `0.8073549 V`

Any arbitrary microtonal pitch can therefore be represented directly using normal Rack pitch CV.

For cents:

\[
V = \frac{c}{1200}
\]

where `c` is pitch displacement in cents.

This allows Cantor to internally reason using whichever representation is most useful while converting to ordinary V/oct at its boundaries.

Possible internal representations include:

- volts,
- cents,
- frequency ratios,
- rational numerator/denominator pairs,
- prime-exponent vectors.

---

# 4. Module Operating Model

Conceptually:

```text
PITCH INPUT
     │
     ▼
Pitch Request
     │
     ▼
Pitch-Space / Tuning Engine
     │
     ▼
Candidate Generator
     │
     ▼
Decision Engine
     │
     ▼
Latch / Continuous Output Logic
     │
     ▼
V/OCT OUTPUT
```

Adaptive modes additionally use:

```text
           Active Voice State
                  │
                  ▼
Pitch Request → Harmonic Context → Candidate Scoring
```

The architecture should avoid implementing each tuning mode as an entirely separate processing path.

---

# 5. Primary Modes

Cantor should initially support three principal behavioral modes.

Names are working names.

## 5.1 FLOW

Traditional continuous quantization.

Input CV is continuously evaluated and output tracks the nearest permitted pitch.

Typical use:

- EDO tuning,
- traditional scales,
- Dragon tuning,
- Scala tuning,
- static rational scales.

Example:

```text
0.393 V input
     ↓
nearest permitted pitch
     ↓
0.3863 V output
```

FLOW is stateless except for hysteresis and ordinary DSP bookkeeping.

---

# 5.2 HOLD

Triggered quantization.

A trigger or rising gate:

1. samples incoming pitch,
2. quantizes it,
3. updates the output,
4. holds that voltage until the next event.

The tuning system itself remains static.

This is useful independently of adaptive behavior.

Example:

```text
Pitch CV slowly moving
      +
Clock trigger
      ↓
new note only on clock
```

---

# 5.3 MIND

Adaptive event-driven quantization.

A trigger represents a request for Cantor to interpret the current input pitch.

On each note event Cantor:

1. samples incoming CV,
2. examines active harmonic context,
3. generates nearby candidate pitches,
4. evaluates each candidate,
5. selects a pitch,
6. latches that pitch.

Critically:

> Identical input CV does not necessarily imply identical output on successive triggers.

The incoming voltage represents **pitch intention**, not an absolute command.

MIND is initially the principal implementation of the Culture-inspired tuning concept.

---

# 6. Optional Future Behavioral Mode: FLUID

FLUID is deliberately not required for the first implementation.

Unlike MIND, where assigned notes remain fixed while held, FLUID may allow currently active notes to be reconsidered when harmonic context changes.

Example:

```text
Voice A = existing note
Voice B = existing note

Voice C enters

       ↓

Cantor discovers a lower-complexity rational relationship

       ↓

A and/or B subtly retune
```

This effectively enables dynamic just intonation.

Changes should generally be slewed rather than stepped.

FLUID could become exceptionally musical, but its behavior is sufficiently unusual that it should not be assumed to belong in the initial release.

---

# 7. Gate Behavior

Cantor should support a dedicated **GATE / TRIG** input.

The port should support polyphony.

## No gate connected

Cantor behaves continuously where appropriate.

FLOW operates normally.

MIND may either:

- fall back to a static version of its pitch lattice, or
- continuously evaluate context.

The initial implementation should favor predictable behavior over attempting full adaptive semantics without note events.

---

## Gate connected

A rising edge represents a note-selection event.

For HOLD and MIND:

```text
RISING EDGE
    ↓
sample pitch
    ↓
select output pitch
    ↓
latch
```

A falling edge marks the associated voice inactive for harmonic-context purposes.

The output CV itself does not necessarily need to reset on gate-off.

---

# 8. Polyphony

Polyphonic operation should be considered fundamental rather than an afterthought.

PITCH and GATE should support standard Rack polyphony.

Each channel represents a possible voice.

Example:

```text
PITCH
ch1 ──────────┐
ch2 ──────────┤
ch3 ──────────┤
ch4 ──────────┘

GATE
ch1 ──────────┐
ch2 ──────────┤
ch3 ──────────┤
ch4 ──────────┘
```

In ordinary tuning systems channels operate independently.

In MIND mode, active channels collectively form a **harmonic context**.

Therefore:

```text
voice 1 pitch ─┐
voice 2 pitch ─┼── Harmonic Context
voice 3 pitch ─┘

new voice 4 request
        ↓
candidate selection informed by voices 1–3
```

A held voice should normally retain its previously assigned pitch.

New notes adapt to the existing harmony rather than rewriting history.

This principle should be considered the default MIND behavior:

> **Existing notes are sovereign. New notes negotiate with them.**

---

# 9. Same Input, Different Result

MIND mode explicitly permits the following patch:

```text
constant voltage
      │
      ▼
    CANTOR
      ▲
      │
    CLOCK
```

Successive clock events may produce different notes.

The amount of variation should be controllable.

At minimum, Cantor should distinguish:

## Deterministic Adaptive

Same input + identical harmonic state = same result.

Changes occur only because context changes.

---

## Interpretive Adaptive

Several similarly good candidates may exist.

Repeated triggers may select different candidates according to weighted probability.

Example:

```text
Input: constant 0.400 V

Trigger 1 → 5/4 region
Trigger 2 → alternate nearby rational pitch
Trigger 3 → 5/4
Trigger 4 → another harmonically valid interpretation
```

This turns stationary or slowly moving control voltages into musical gestures without reducing the engine to random pitch generation.

---

# 10. Candidate Scoring

The exact MIND scoring function is expected to evolve substantially through listening tests.

A useful initial conceptual model is:

\[
S(p)=
w_DD(p,V_{request})
+
w_HH(p,C)
+
w_MM(p,\text{history})
+
w_RR(p)
\]

where:

- `D` = distance from requested pitch,
- `H` = harmonic complexity/coherence relative to active voices,
- `M` = melodic/history preference,
- `R` = controlled stochastic variation.

Lower score may represent better suitability.

---

# 11. Input Distance

The distance term protects the performer's intention.

Basic implementation:

\[
D = |p - V_{request}|
\]

possibly measured in cents.

The system should strongly prefer pitches near the requested CV unless intentionally configured otherwise.

This prevents MIND from behaving like an unrelated random sequencer.

---

# 12. Harmonic Coherence

The harmonic term evaluates how simply a candidate relates to currently sounding notes.

For each active pitch difference:

\[
\Delta V = p - p_i
\]

convert to a frequency ratio:

\[
r = 2^{\Delta V}
\]

Then determine whether `r` is close to a low-complexity rational approximation.

Candidate rational relationships may be limited to a configured prime basis, initially perhaps:

\[
2,3,5,7,11
\]

representing an 11-limit harmonic universe.

A rational ratio:

\[
r = 2^a3^b5^c7^d11^e
\]

can be assigned a complexity metric such as:

\[
C =
w_3|b|+
w_5|c|+
w_7|d|+
w_{11}|e|
\]

The octave exponent may receive little or no penalty.

This is only a starting heuristic.

Listening tests should drive the final behavior.

---

# 13. Melodic Memory

MIND should optionally consider recent output history.

Useful behaviors may include:

- discourage immediate note repetition,
- encourage small melodic movement,
- prefer return to a recent tonal anchor,
- occasionally revisit previously selected notes,
- penalize excessively large leaps,
- encourage contour matching.

A minimal history buffer could contain approximately 8–32 recent decisions per voice or globally.

This should remain computationally trivial.

---

# 14. Interpretive Variation

A control tentatively named **INTERPRET** should regulate how literally MIND obeys the incoming CV.

At minimum:

### 0%

Input dominates.

The nearest strongly harmonic candidate almost always wins.

### Middle

Several nearby candidates compete.

Context and history become more influential.

### 100%

The input defines a broader target region rather than a precise pitch.

Cantor is allowed considerably more freedom.

The term should not simply add random voltage.

Variation should operate through candidate selection.

---

# 15. Tuning Engines

The following engines represent the intended initial architecture.

Not every engine must ship in the first implementation.

---

# 16. EDO Engine

Equal Division of the Octave.

Parameters:

- number of divisions `N`,
- active degree mask,
- root offset.

Pitch step:

\[
\Delta V = \frac{1}{N}
\]

Potential useful presets:

- 5
- 7
- 12
- 19
- 22
- 24
- 31
- 53

Arbitrary values should ideally be supported within a reasonable range.

Likely range:

**1–128 divisions**

Potentially higher later.

---

# 17. Conventional Scale Layer

For EDO systems, Cantor may optionally offer a degree mask.

For 12-TET this supports ordinary scales:

- chromatic,
- major,
- natural minor,
- harmonic minor,
- melodic minor,
- pentatonic,
- whole tone,
- diminished,
- etc.

However, Cantor should not make conventional scale vocabulary the conceptual center of the module.

---

# 18. Rational / Just Engine

Allows pitches to be specified using rational ratios.

Example:

```text
1/1
9/8
5/4
4/3
3/2
5/3
15/8
2/1
```

Internally convert ratios using:

\[
V=\log_2(r)
\]

The scale can then repeat at a selected period.

Potential source formats:

- built-in presets,
- user-defined ratio list,
- Scala import.

---

# 19. WYRM — Dragon Harmonic Tuning

Working tuning name: **WYRM**

The canonical Dragon tuning should initially derive from harmonics 8 through 16.

Ratios relative to harmonic 8:

| Degree | Ratio | Approx. cents |
|---|---:|---:|
| VIII | 1/1 | 0 |
| IX | 9/8 | 203.91 |
| X | 5/4 | 386.31 |
| XI | 11/8 | 551.32 |
| XII | 3/2 | 701.96 |
| XIII | 13/8 | 840.53 |
| XIV | 7/4 | 968.83 |
| XV | 15/8 | 1088.27 |
| XVI | 2/1 | 1200 |

This tuning should sound naturally harmonic while introducing intervals poorly represented in 12-TET.

Particularly important degrees:

- `11/8`
- `13/8`
- `7/4`

These provide much of the tuning's non-human character.

WYRM can operate entirely in FLOW mode.

No state or adaptive logic is required.

---

# 20. Dragon Degree Naming

The panel/display should avoid forcing Western note names onto WYRM.

Possible labels:

```text
VIII
IX
X
XI
XII
XIII
XIV
XV
XVI
```

This reinforces the harmonic-series origin.

Alternative evocative labels can be explored later, but harmonic numbers are musically meaningful and implementation-neutral.

---

# 21. Dragon Subharmonic Variant

A future or secondary Dragon mode may derive intervals from reciprocal/subharmonic relationships.

Working conceptual polarity:

```text
STONE ←────────→ FIRE
```

Where:

**FIRE**

harmonic-series-derived relationships.

**STONE**

subharmonic / undertone-derived relationships.

A morph between these spaces may eventually be interesting, although the mathematical definition of such interpolation requires experimentation.

This should not block the initial WYRM implementation.

---

# 22. ELDER — Non-Octave Dragon Tuning

Working name: **ELDER**

A more alien Dragon system may use the frequency ratio:

\[
3:1
\]

as the fundamental repeating period.

Its V/oct period is:

\[
P = \log_2(3)
\]

approximately:

```text
1.5849625 V
```

This creates a tritave-periodic pitch universe.

Quantization remains completely compatible with ordinary V/oct oscillators.

General periodic quantization becomes:

\[
V_{out}=kP+S_i
\]

where:

- `P` is tuning period in volts,
- `k` is integer period index,
- `S_i` is a valid degree within the period.

ELDER could eventually use:

- Bohlen–Pierce-derived pitches,
- a custom Dragon tritave scale,
- or both.

Exact musical construction is intentionally deferred.

---

# 23. MIND — Culture-Inspired Adaptive Tuning

Working tuning/behavior name: **MIND**

MIND is not simply a fixed scale.

It represents an adaptive harmonic field.

A candidate pitch is judged by its relationship to:

- incoming pitch request,
- currently sounding voices,
- recent melodic history,
- configured harmonic complexity,
- optional interpretive variation.

MIND should initially operate primarily in triggered mode.

---

# 24. MIND Harmonic Universe

A practical initial harmonic vocabulary may use ratios constructed from:

```text
2
3
5
7
11
```

This allows:

- perfect fifths,
- pure thirds,
- septimal intervals,
- undecimal intervals,
- many pitches uncommon to conventional Western systems.

The engine should not evaluate an unlimited number of arbitrary rational numbers in the audio thread.

Instead, generate or precompute a bounded candidate table.

Possible approach:

At initialization, generate all prime-vector combinations within configured exponent bounds and a defined pitch range.

Example:

```text
3 exponent:  -4 .. +4
5 exponent:  -3 .. +3
7 exponent:  -2 .. +2
11 exponent: -2 .. +2
```

Normalize candidates into the desired period and calculate:

- volts,
- cents,
- ratio representation,
- harmonic complexity score.

Prune excessively complex or near-duplicate entries.

The exact candidate limits should be tuned for musical usefulness rather than mathematical completeness.

---

# 25. MIND Pitch Selection

On a rising trigger:

```text
requestedPitch = current input voltage
context        = active held pitches
candidates     = tuningEngine.getCandidates(requestedPitch)

for candidate:
    score candidate

selected = chooseCandidate()

output[channel] = selected.volts
active[channel] = true
```

On gate fall:

```text
active[channel] = false
```

Held pitch may remain electrically present at the output but no longer contributes to harmonic context.

---

# 26. Anchor / Root Handling

Adaptive tuning raises an important unresolved issue:

> Relative to what does the harmonic field exist?

Cantor should support an explicit anchor concept.

Potential behaviors:

### Fixed Root

User specifies a pitch center.

All rational relationships ultimately derive from it.

### First Voice

First active note becomes the temporary harmonic anchor.

### Lowest Voice

Lowest currently active pitch acts as anchor.

### Contextual

No single privileged root exists; relationships are evaluated pairwise.

The initial MIND implementation should probably favor **pairwise context with an optional fixed root**.

This avoids requiring Western-style root harmony while still giving users control when desired.

---

# 27. Root CV

A dedicated root CV input is optional and should be evaluated against panel-space cost.

Potentially useful:

```text
ROOT
```

Accept ordinary V/oct.

Applications:

- transpose tuning system,
- establish adaptive harmonic anchor,
- modulate the entire pitch lattice.

If panel space becomes constrained, root may instead be implemented through:

- transpose knob,
- context menu,
- or secondary mode.

---

# 28. Hysteresis

FLOW mode should include pitch-boundary hysteresis.

Without hysteresis, noisy or slowly moving CV near a quantization boundary may cause rapid note chatter.

Implementation:

- retain current degree,
- require input to move slightly beyond the exact midpoint before changing.

Hysteresis should be defined in cents or a fraction of local degree spacing.

It must work for irregular tunings where neighboring pitch distances differ.

---

# 29. Output Slew

Cantor should optionally support a small amount of pitch slew/glide.

Use cases:

- prevent clicks or abrupt retuning,
- enable expressive adaptive behavior,
- support future FLUID mode.

However, default conventional quantizer output should remain immediate.

Possible control:

**GLIDE**

Range conceptually:

```text
0 ms → several seconds
```

Exact range can be tuned during implementation.

Slew should occur after pitch decision rather than altering the quantization calculation itself.

---

# 30. Candidate Selection Controls

A useful initial control vocabulary may be:

## INTENT

How strongly requested input pitch dominates candidate selection.

High INTENT:

> stay close to input.

Low INTENT:

> harmonic context can pull farther away.

---

## COHERENCE

How strongly MIND prefers low-complexity harmonic relationships with active notes.

Potentially INTENT and COHERENCE could become opposite ends of one bipolar control:

```text
INTENT ←────────→ COHERENCE
```

This may produce a cleaner panel.

---

## INTERPRET

Controls stochastic / alternative interpretation among acceptable candidates.

At zero:

deterministic.

At high values:

more varied selection.

---

## MEMORY

Potential future control governing how much recent melodic history influences decisions.

This may initially belong in a secondary settings view rather than the main panel.

---

# 31. Degree Density / Complexity

MIND may require a way to control the density of available rational pitches.

Working control concepts:

- COMPLEXITY
- DEPTH
- FIELD
- RESOLUTION

Lower values:

- simple ratios only,
- sparse harmonic field.

Higher values:

- more complex ratios,
- denser pitch space,
- more unusual harmonic options.

This parameter may prove musically more important than exposing formal prime limits.

Users should not need number-theory knowledge to operate Cantor.

---

# 32. Initial I/O Proposal

Minimum useful ports:

### Inputs

**PITCH**  
Polyphonic V/oct pitch input.

**GATE**  
Polyphonic note-event input.

Potential additions:

**ROOT**  
Pitch anchor / transpose.

**MODE CV** or parameter CV inputs are optional and may not justify panel cost initially.

---

### Outputs

**PITCH**  
Polyphonic quantized/interpreted V/oct output.

Potentially:

**CHANGE / TRIG**  
Pulse whenever Cantor selects a new note.

This could be useful for downstream envelopes when Cantor is driven by clocks or continuous CV.

Not required for the first prototype.

---

# 33. Gate Channel Matching

When both PITCH and GATE are polyphonic:

channel `n` of GATE controls channel `n` of PITCH.

If PITCH has more channels than GATE:

possible fallback behavior:

- monophonic gate broadcasts to all pitch channels,
- otherwise missing gates are considered low.

Standard Rack conventions should guide implementation.

This should be tested against expected polyphonic workflow.

---

# 34. Voice State

Each polyphonic channel should maintain lightweight state.

Example:

```cpp
struct VoiceState {
    float heldPitch;
    float targetPitch;
    bool active;
    bool previousGate;
    int lastDegree;
    uint64_t noteAge;
};
```

MIND may also require:

```cpp
struct HarmonicHistoryEntry {
    float pitch;
    int voice;
    uint64_t sequence;
};
```

Avoid dynamic memory allocation from the audio thread.

All buffers should have fixed maximum sizes.

---

# 35. Suggested Internal Architecture

A clean internal separation might resemble:

```cpp
class TuningSystem {
public:
    CandidateSet getCandidates(
        float requestedPitch,
        const TuningContext& context
    );
};
```

Implementations:

```text
EDOTuning
RationalTuning
DragonTuning
TritaveTuning
ScalaTuning
MindTuning
```

Decision layer:

```cpp
class DecisionStrategy {
public:
    float select(
        float requestedPitch,
        CandidateSet candidates,
        const HarmonicContext& context
    );
};
```

Possible strategies:

```text
NearestDecision
LatchedNearestDecision
AdaptiveDecision
```

The Rack `Module::process()` implementation should primarily orchestrate these systems rather than contain tuning-specific logic.

---

# 36. Candidate Representation

Example:

```cpp
struct PitchCandidate {
    float volts;
    float distanceCents;
    float harmonicCost;
    float melodicCost;
    float totalScore;

    int degree;
    int ratioNumerator;
    int ratioDenominator;
};
```

Not all fields are needed for every engine.

For high-performance fixed tunings, simpler specialized lookup structures may be used internally.

---

# 37. Performance Requirements

Cantor should be inexpensive relative to audio-generating DSP modules.

FLOW modes should be extremely cheap.

For fixed scales:

- no heap allocation,
- precomputed pitch tables,
- nearest-degree lookup,
- optional binary search,
- minimal transcendental math per sample.

MIND is more computationally complex, but adaptive evaluation only needs to occur on **note events**, not every sample.

This is an important optimization.

At normal operation:

```text
no trigger → simply output held voltage
```

Therefore even comparatively sophisticated harmonic scoring should remain inexpensive.

---

# 38. Precomputation

Precompute wherever possible:

- EDO degree positions,
- rational pitch offsets,
- ratio complexity,
- normalized pitch positions,
- Dragon scale offsets,
- Scala degree positions.

Avoid repeated `log2()` calculations in FLOW processing.

MIND may use precomputed interval tables or cached rational approximation data.

---

# 39. Randomness

Interpretive MIND behavior requires controlled stochastic selection.

Requirements:

- deterministic when INTERPRET = 0,
- stable per patch if desired,
- no expensive random generator,
- optional stored RNG seed.

A small fast PRNG such as PCG or xoshiro-family implementation is sufficient.

Patch serialization should optionally preserve seed/state to improve reproducibility.

---

# 40. Selection Probability

Avoid simple uniform random choice.

Possible method:

Given candidate score `S`:

\[
P_i \propto e^{-S_i/T}
\]

where `T` is derived from INTERPRET.

At:

```text
T → 0
```

the best candidate almost always wins.

As temperature rises:

other good candidates become increasingly likely.

Bad candidates should remain unlikely.

An approximation may be used if exponential evaluation becomes undesirable, although note-event frequency makes this optimization largely unnecessary.

---

# 41. Display Concept

Cantor likely benefits substantially from a small display because tuning information cannot always be represented meaningfully using ordinary note names.

Potential display elements:

```text
MIND
5/4
+386.3¢
```

or:

```text
WYRM
XIII
13/8
```

or:

```text
31 EDO
17 / 31
```

Possible hierarchy:

**top:** tuning system  
**center:** selected degree / ratio  
**bottom:** cents or contextual information

The precise visual design should follow later.

---

# 42. Visual Representation of Pitch Space

A more ambitious display could show the current pitch field.

For example:

```text
|  •    • •      ●     •   •  |
```

where:

- points = nearby candidate pitches,
- central marker = input intention,
- highlighted point = selected output.

In MIND mode the field could visibly reconfigure as harmonic context changes.

This could make otherwise abstract behavior understandable.

The display should remain lightweight and avoid expensive continuously animated rendering unless it clearly improves UX.

---

# 43. Mode Feedback

Because behavior changes significantly between FLOW, HOLD and MIND, the current behavior must be visually obvious.

Users should never wonder why output stopped continuously tracking input.

Possible indication:

```text
FLOW
HOLD
MIND
```

with mode-specific graphics or illumination.

---

# 44. Panel-Level Control Proposal

A preliminary control hierarchy:

```text
          CANTOR

        [ DISPLAY ]

        TUNING
          ◉

   INTENT ↔ COHERENCE
          ◉

       INTERPRET
          ◉

         GLIDE
          ◉

   [ FLOW | HOLD | MIND ]

      PITCH     GATE
        ○         ○

          PITCH
            ○
```

This is intentionally conceptual.

Actual width and panel composition should be determined after prototyping.

---

# 45. Tuning Selection UX

The module may quickly accumulate many tuning systems.

A giant knob with dozens of entries is unlikely to scale well.

Possible hierarchy:

```text
FAMILY
  EDO
  RATIO
  WYRM
  ELDER
  MIND
  SCALA
```

Selecting EDO exposes division count.

Selecting WYRM exposes Dragon-specific parameters.

Selecting MIND exposes adaptive field controls.

The module could use:

- compact display-driven encoder interface,
- custom overlay,
- or a mixture of physical controls and context settings.

This should evolve with actual panel design.

---

# 46. Parameter Relevance

Not every control needs identical meaning in every tuning mode.

Examples:

**INTERPRET**

meaningful primarily in MIND.

**DIVISIONS**

meaningful only in EDO.

**FIELD / COMPLEXITY**

meaningful primarily in MIND.

Cantor should not force artificial parameter equivalence merely for implementation symmetry.

Controls may:

- become inactive,
- change displayed labels,
- or assume mode-specific meaning.

However, behavior should remain understandable.

---

# 47. Scala Support

Scala `.scl` import would significantly expand Cantor's practical usefulness.

Expected behavior:

1. user loads `.scl`,
2. parse tuning,
3. convert entries to normalized V/oct offsets,
4. store tuning data in patch or retain path with embedded fallback,
5. quantize using ordinary FLOW/HOLD behavior.

Scala support should be architecturally anticipated even if not part of MVP.

A later `.kbm` mapping layer may also be considered but is not initially necessary.

---

# 48. Patch Serialization

Store at minimum:

- selected tuning engine,
- behavior mode,
- relevant parameters,
- active degree masks,
- imported tuning data,
- RNG seed/state if appropriate,
- custom ratio definitions,
- mode-specific settings.

Held notes do not necessarily need to survive patch reload unless Rack convention suggests otherwise.

Imported tunings should preferably remain functional if the original external file is later unavailable.

Embedding parsed tuning definitions may therefore be preferable to depending solely on file paths.

---

# 49. CV Range

Cantor should operate over ordinary Rack pitch ranges without artificial limitation.

Practical safe range might be:

```text
approximately -10 V to +10 V
```

or whatever range emerges naturally from Rack expectations.

Very extreme CV values should not cause:

- overflow,
- NaN,
- runaway lookup loops,
- invalid logarithms.

Quantization works on pitch offsets and therefore does not require frequency-domain conversion for most tuning engines.

---

# 50. Floating-Point Precision

32-bit float pitch representation is more than sufficient for normal musical tuning resolution.

A cent corresponds to approximately:

```text
0.000833333 V
```

Cantor should nevertheless use double precision during:

- ratio conversion,
- tuning generation,
- Scala parsing,
- rational candidate precomputation,

where convenient.

Runtime outputs may remain ordinary Rack `float`.

---

# 51. Musical Safety Bounds

MIND should have configurable or implicit bounds preventing absurd interpretation.

Potential constraints:

- maximum deviation from requested pitch,
- maximum interval complexity,
- candidate count cap,
- optional maximum leap from previous output.

A default maximum interpretation distance somewhere around one to several semitones may be musically sensible, but this should be determined experimentally.

At extreme settings the user may intentionally allow much larger deviations.

---

# 52. Candidate Window

MIND should not search the entire pitch universe.

Given requested pitch `V`:

```text
search candidates within:
V - W
through
V + W
```

where `W` depends on INTENT / INTERPRET.

Candidate window can expand as interpretation becomes freer.

This both reduces processing and preserves relationship between control input and output.

---

# 53. Repetition Behavior

MIND should probably include some mild default resistance to immediate repetition when multiple equally strong candidates exist.

However:

- repeated notes must remain possible,
- a strongly preferred pitch should not be rejected merely to create variation,
- INTERPRET = 0 should remain deterministic.

Potential history penalty:

```text
same as last output       → moderate penalty
same as output 2 ago      → small penalty
older note                → negligible
```

Exact values should be tuned by ear.

---

# 54. Harmonic Context Weighting

Not all active voices necessarily deserve identical harmonic importance.

Possible future weighting:

- oldest note stronger,
- lowest note stronger,
- root channel stronger,
- higher gate velocity impossible under ordinary gate CV unless inferred separately,
- dedicated anchor voice.

Initial implementation should use equal weighting unless testing suggests otherwise.

Simplicity is preferable until the musical need becomes clear.

---

# 55. Chord Construction Behavior

One particularly important MIND use case:

```text
Voice 1 request → 1/1
Voice 2 request → 5/4
Voice 3 request → 3/2
Voice 4 request → 7/4
```

The module should naturally tend toward coherent low-complexity structures without requiring predefined chord names.

This is a core philosophical distinction:

Cantor MIND does not need to know:

```text
"dominant seventh"
```

It can instead discover:

```text
1 : 5/4 : 3/2 : 7/4
```

through relational coherence.

---

# 56. Monophonic MIND

MIND must remain interesting even with one voice.

Without simultaneous harmonic context, candidate scoring can rely on:

- fixed or inferred anchor,
- melodic history,
- input distance,
- interpretive variation.

Therefore a stationary input plus clock can still produce a coherent sequence.

This is desirable and should be treated as an intentional use case rather than an accidental side effect.

---

# 57. Potential Tonal-Center Memory

A future enhancement could allow MIND to infer a slowly changing tonal center from recent notes.

Example:

```text
short-term active voices
       +
long-term pitch history
       ↓
estimated harmonic center
```

This could allow melodies to establish tonal gravity without explicit root CV.

This feature is attractive but risks overcomplicating initial behavior.

Defer unless simple adaptive implementation feels directionless without it.

---

# 58. Dragon vs MIND Architectural Relationship

These two flagship tunings intentionally exercise different portions of Cantor.

## WYRM

Tests:

- arbitrary rational tuning,
- irregular degree spacing,
- non-Western note labeling,
- conventional FLOW quantization.

## MIND

Tests:

- event-driven decision making,
- polyphonic context,
- adaptive candidates,
- history,
- stochastic interpretation.

Their behavioral difference is not a design flaw.

It is evidence that Cantor's abstraction is broader than a traditional quantizer.

---

# 59. User Mental Model

Cantor should gradually reveal complexity rather than require understanding it immediately.

At the simplest level:

> Put pitch in. Choose tuning. Get quantized pitch out.

Second level:

> Add a gate and notes become clocked/held.

Third level:

> Select MIND and incoming pitch becomes an intention interpreted in harmonic context.

This progression should drive the UI.

---

# 60. Suggested MVP

The first playable implementation should deliberately avoid attempting every idea in this specification.

### MVP Stage 1

Implement:

- monophonic/polyphonic PITCH input,
- polyphonic output,
- FLOW behavior,
- HOLD behavior,
- GATE input,
- 12-TET,
- arbitrary EDO,
- WYRM harmonic tuning,
- basic display,
- root/transposition,
- hysteresis.

This validates the fundamental generalized quantizer.

---

# 61. MVP Stage 2 — MIND Prototype

Add:

- active voice tracking,
- rational candidate table,
- triggered adaptive selection,
- input-distance scoring,
- harmonic-complexity scoring,
- deterministic adaptive mode,
- INTERPRET stochasticity,
- minimal note history.

At this stage the goal is not algorithmic perfection.

The goal is answering:

> Does this actually feel musically intelligent?

---

# 62. Stage 3 — Musical Refinement

Tune:

- candidate density,
- ratio limits,
- harmonic weighting,
- repetition penalties,
- interpretation range,
- polyphonic interaction,
- anchor behavior,
- UI terminology.

This stage should be driven heavily by patching and listening.

It is expected that significant behavioral revisions may occur here.

---

# 63. Stage 4 — Expanded Pitch Worlds

Possible additions:

- ELDER Dragon tuning,
- subharmonic Dragon mode,
- 19/22/31/53 EDO presets,
- just-intonation presets,
- Scala import,
- custom ratio editor,
- alternative Culture pitch fields.

---

# 64. Stage 5 — Experimental Behavior

Candidates:

- FLUID dynamic retuning,
- adaptive tonal-center inference,
- pitch-field visualization,
- harmonic morphing,
- voltage-controlled tuning changes,
- field mutation,
- alternate candidate-selection personalities.

These should not burden the first architecture but should remain possible.

---

# 65. Unit Testing

Much of Cantor can be tested deterministically.

Required tests should include:

### EDO

For N divisions:

```text
output always lies on k/N voltage boundary
```

### Rational

Known ratios produce correct `log2(r)` voltages.

### WYRM

Verify exact stored harmonic ratios.

### Tritave

Verify periodicity at:

```text
log2(3)
```

### Trigger Behavior

Output does not change without a valid note event in HOLD/MIND.

### Gate State

Falling gate correctly removes voice from harmonic context.

### Determinism

MIND with INTERPRET = 0 returns identical results for identical state.

### Polyphony

Voice processing remains channel-correct.

### Serialization

Custom tuning state survives patch save/load.

---

# 66. Debugging Instrumentation

During development, MIND should expose detailed debug information in logs or optional debug builds.

For each note event:

```text
requested pitch
active voices
candidate ratios
input distance
harmonic cost
history cost
random contribution
final score
selected candidate
```

This will be extremely valuable when the engine produces a musically surprising decision.

Production builds should avoid excessive logging.

---

# 67. UI Debug Mode

A temporary developer display may show:

```text
REQ  +0.401
OUT  +0.386
5/4
H 0.18
D 0.07
```

This could substantially accelerate tuning of the scoring algorithm.

It does not need to survive as the shipping interface.

---

# 68. Important Open Questions

The following should remain intentionally unresolved until prototype testing.

### MIND root semantics

Is harmony primarily:

- root-relative,
- pairwise,
- lowest-note-relative,
- or dynamically inferred?

### Candidate density

How many rational pitches produce richness without chromatic mud?

### Repetition

How much should Cantor actively generate variation?

### Intent radius

How far may MIND depart from incoming CV before the user feels control has been lost?

### Polyphonic arrival order

Does note-order dependency feel expressive or arbitrary?

### Voice release

How strongly should removing a voice alter interpretation of subsequent notes?

### Culture fixed tuning

Should there also be a static Culture-inspired tuning such as 31-EDO?

### Dragon physiology

Should canonical Dragon tuning remain harmonic 8–16, or eventually become a more elaborate physical/acoustic model?

### FLUID

Does active retuning create wonder, seasickness, or both?

These questions should be answered through musical interaction rather than specification purity.

---

# 69. Non-Goals for Initial Implementation

Cantor is not initially intended to be:

- a full sequencer,
- a MIDI microtuning host,
- an MPE processor,
- an audio-frequency pitch detector,
- an oscillator,
- a chord recognizer,
- a machine-learning model,
- a general-purpose generative composition system.

MIND may produce generative behavior, but generation remains constrained by incoming musical intention and tuning logic.

---

# 70. Core Identity

Cantor should ultimately feel understandable at two levels.

To a conventional Rack user:

> **Cantor is a very capable quantizer supporting unusual tuning systems.**

To a user who explores deeper:

> **Cantor interprets musical intention inside programmable models of pitch.**

That second identity is what distinguishes the module.

Traditional quantization asks:

> Which allowed note is closest to this voltage?

Cantor is designed to ask a broader question:

> Given this requested region of pitch space, this tuning universe, this musical context, and this moment in time — what note should exist now?

---

# 71. Initial Product Definition

For implementation purposes, Cantor can presently be summarized as:

**A polyphonic V/oct quantizer and pitch-interpretation engine supporting fixed, rational, non-octave, and adaptive harmonic tuning systems, with continuous and event-driven operation.**

Its two flagship experimental pitch worlds are:

**WYRM**  
A Dragon-inspired tuning derived from natural harmonic resonances.

**MIND**  
A Culture-inspired adaptive harmonic system in which incoming CV represents pitch intention and note selection may depend on currently active voices and recent musical history.

Both ultimately emit ordinary Rack-compatible V/oct voltage.

That common representation is the foundation allowing these otherwise very different musical ideas to coexist cleanly inside one module.

---

# 72. Guiding Principle for Development

The architecture should be rigorous.

The musical behavior should remain allowed to evolve.

Cantor is unusual enough that some of its most important rules cannot be confidently designed on paper. The specification should therefore protect the separation of:

- pitch representation,
- tuning,
- context,
- decision behavior,
- state,
- and output,

while allowing the actual musical intelligence inside those boundaries to be rewritten repeatedly as the instrument begins to reveal what it wants to be.

---

# 73. Culture-First MVP Implementation

The first playable implementation intentionally begins with the experimental
Culture behavior rather than the fixed-quantizer Stage 1 described above.

Implemented first-pass scope:

- Culture / MIND only,
- polyphonic PITCH and GATE inputs,
- polyphonic held pitch output,
- static rational quantization when GATE is unpatched,
- rising-edge adaptive selection,
- falling-edge removal from harmonic context,
- sovereign held notes,
- a bounded precomputed 11-limit field,
- INTENT, COHERENCE, INTERPRET, and FIELD controls,
- deterministic selection at zero INTERPRET,
- seeded weighted alternatives above zero INTERPRET,
- fixed 0 V anchor when no voices are active,
- pairwise scoring against active voices,
- persisted interpretation seed and random state,
- a compact developer display for request, result, distance, harmonic cost,
  and active voice count.

Deliberately deferred:

- FLOW and HOLD mode selection,
- EDO and conventional scale layers,
- WYRM and ELDER tunings,
- root/transposition control,
- glide,
- melodic-memory controls,
- ratio naming,
- Scala import,
- FLUID retuning,
- final visual design.

The immediate listening question is whether the combination of input-distance,
rational-complexity, and active-voice scoring feels like musical negotiation
rather than correction or randomness.
