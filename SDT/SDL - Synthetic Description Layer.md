# SDL Primer v0.1
**SDL = Synthetic Description Layer**  
*A secondary, design-time artifact that enables SDT and SDT-SL generation through explicit structure, constraints, and transforms. SDL is not spoken, not performed, and not presented to SUNO directly.*

## 0. Status and Intent
SDL exists to:
- Prevent layer bleed between **ritual voice (SDT)**, **spoken logic (SDT-SL)**, and **music-direction cues (SUNO brackets)**.
- Provide a stable **intermediate representation** for “what the piece is doing” so we can re-render it reliably.
- Make transformations repeatable: same **SDL → consistent SDT-SL (phonetic)** → consistent **SUNO bracket behavior**.

SDL does **not** define SDT. SDT remains the core.

---

## 1. Non-Goals
SDL is **not**:
- A performable dialect
- A chant language
- A poetic system
- A notation meant to be heard
- A substitute for SDT or SDT-SL

If SDL starts “sounding cool,” it’s probably doing the wrong job.

---

## 2. Layer Model (Authoritative Boundaries)

### SDT (performable)
- Mythic / ritual / embodied
- Breath-led, vowel-weighted
- Performed as chant / whisper / drone-voice

### SDT-SL (performable)
- “Spoken logic” rendered into pronounceable phonetics
- Emotionally flat or procedural by delivery **but still vocal**
- **No symbols**, no operators, no code punctuation

### SDL (non-performable)
- Pure structure: process / state / constraints / intents
- May use symbols internally
- Must be **compiled** into SDT-SL (and bracket cues)

---

## 3. SDL Core Data Types (Primitives)
SDL describes **behavior**, not meaning.

### 3.1 State
Represents a persistent condition.
- `STATE(stable | unstable | drifting | sealed | silent)`

### 3.2 Process
Represents an ongoing action.
- `PROCESS(iterate | fold | compress | erode | accumulate | halt)`

### 3.3 Constraint
Represents a rule the piece must obey.
- `CONSTRAINT(no_groove | no_melody | long_silence | misalign | resist_resolution)`

### 3.4 Trigger
Represents a threshold crossing.
- `TRIGGER(threshold_crossed | pressure_peak | pattern_glimpse)`

### 3.5 Section
Represents a block in time with roles.
- `SECTION(name, role, duration_hint)`

Common roles:
- `CHANT_SDT`
- `SPOKEN_SDT_SL`
- `INSTRUMENTAL`
- `INTERFERENCE`
- `SEAL`

---

## 4. SDL Minimal Syntax (Human-Friendly)
A small, readable shape:

```text
SECTION(Intro, CHANT_SDT, long)
  STATE(unstable)
  CONSTRAINT(no_rhythm, long_pauses)
  PROCESS(fold)

SECTION(LogicA, SPOKEN_SDT_SL, short)
  STATE(unstable)
  PROCESS(iterate)
  CONSTRAINT(dry_voice, monotone)

SECTION(Interference, INTERFERENCE, medium)
  CONSTRAINT(misalign, not_synchronized)
  PROCESS(iterate, fold)

SECTION(Seal, CHANT_SDT, short)
  STATE(sealed)
  PROCESS(halt)
```

---

## 5. Compilation Targets
SDL compiles into **two outputs**:

### 5.1 SDT-SL Token Stream
A pronounceable “logic voice” vocabulary.

Rules:
- No symbols: `: ≠ > = ⟨ ⟩`
- No code punctuation
- Prefer 2–3 syllable chunks
- Hyphens allowed to stabilize stress (`val-thra`, `tor-in`)
- Consonants: keep mostly soft; allow harder edges sparingly for “machine bite”

### 5.2 SUNO Bracket Directives
Bracket cues are **performance instructions**, not language content.

Rules:
- Use `|` separated lists
- Keep directives concrete: “spoken | monotone | dry | no reverb”
- Avoid jargon like “SDT” unless you also describe the sound behavior

---

## 6. SDL → SDT-SL Transform Rules (Compiler Spec v0.1)
SDL primitives map to SDT-SL “roots” (phonetic stems). Example mapping set:

### 6.1 Suggested Root Lexicon
- `INIT` → **tor** (start / boot)
- `LOOP` → **re-ka** (repeat)
- `STATE_UNSTABLE` → **nu-ren**
- `THRESHOLD` → **val-thra**
- `PRESSURE` → **kosh**
- `ALLOW_DRIFT` → **mor-fah**
- `IDENTITY_DERIVED` → **kai-ren**
- `VOICE_ORIGIN_SPLIT` → **nor-esh**
- `CONTINUITY_FALSE` → **no-lor**
- `HALT` → **sel-fin** (stop / close)

### 6.2 Assembly Patterns
- `PROCESS(iterate)` → prefix with **re-ka**
- `STATE(x)` → suffix with a state marker like **-ren / -lor / -sil**
- `TRIGGER(threshold_crossed)` → include **val-thra** plus a hard stop consonant (optional) for emphasis

### 6.3 Example Compilation
SDL:
- `PROCESS(iterate)`
- `STATE(unstable)`
- `TRIGGER(threshold_crossed)`

SDT-SL result:
- **re-ka sel**
- **vak nu-ren**
- **kosh val-thra**

(Delivery instructions then decide whether it’s whispered, monotone, etc.)

---

## 7. Recommended Output Formats

### 7.1 “Composer View” (SDL)
Used by you + any LLM to reason and regenerate.

### 7.2 “Performer View” (SDT + SDT-SL)
Used for vocals / lyrics.

### 7.3 “Generator View” (SUNO)
Bracket directives + performer text.

---

## 8. Practical Pattern Library (Reusable Templates)

### Template: Autechre-Pressure Hybrid
- Intro: SDT whisper / unstable state
- Logic punctures: SDT-SL monotone bursts
- Interference: overlap, misalignment, irregular entries
- Seal: Abyssal SDT “law voice”
- Outro: instrumental fade with no resolution

SDL sketch:
```text
SECTION(Intro, CHANT_SDT, long) STATE(unstable) CONSTRAINT(long_pauses, no_rhythm)
SECTION(LogicA, SPOKEN_SDT_SL, short) PROCESS(iterate) CONSTRAINT(dry_voice, monotone)
SECTION(Interference, INTERFERENCE, medium) CONSTRAINT(misalign) PROCESS(fold, iterate)
SECTION(Seal, CHANT_SDT, short) STATE(sealed) PROCESS(halt) CONSTRAINT(minimal)
SECTION(Outro, INSTRUMENTAL, long) CONSTRAINT(no_resolution, near_silence)
```

---

## 9. Guardrails and Failure Modes

### If SDT-SL starts looking like code…
You’ve leaked SDL into performance. Re-compile into phonetics.

### If chant becomes “too understandable”…
You’ve dragged SDT toward semantic language. Remove explicit meaning cues; return to phonetic invocation.

### If SUNO ignores the contrast…
Increase bracket specificity:
- “spoken | monotone | dry | no reverb | short phrases | silence between”
vs
- “whispered | breathy | close-mic | reverb halo | long vowels”

---

## 10. Versioning
- SDL Primer versions track **system behavior**, not lore.
- Keep “root lexicon” stable; expand carefully.
- Any new primitive should specify:
  - Purpose
  - SDT-SL mapping
  - SUNO directive implications

