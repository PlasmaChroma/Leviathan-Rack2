# PHONEX — Codex Implementation Specification

**Status:** Implementation-ready specification  
**Module:** `LEVIATHAN // PHONEX`  
**Target:** VCV Rack 2, Windows-first Leviathan build  
**Form factor:** 14 HP (`71.12 mm × 128.5 mm`)  
**Architecture:** Monophonic, order-10 LPC toy-speech synthesizer

**Primary sonic target:** the intelligible, nasal, quantized, slightly uncanny character of late-1970s/early-1980s educational speech synthesizers, especially Speak & Spell-era TI speech hardware, without reproducing proprietary speech ROM contents or attempting bit-exact hardware emulation.

---

## 0. Codex execution contract

This is an implementation specification, not a design brainstorm.

Before editing, inspect the current Leviathan repository and reuse its current patterns for module registration, Rack configuration, JSON persistence, `PanelSvgUtils`, `visual_assets::SplitPanelRenderer`, themed components, `ui::TextField`, standalone tests, Rack tests, and panel-contract tests.

The implementation rules are:

1. Decisions marked **LOCKED** are contracts. Do not silently redesign them.
2. Implement phases in order.
3. Do not make a later subsystem compensate for a failing earlier subsystem.
4. No string parsing, dictionary lookup, pronunciation work, file I/O, allocation, or locks may occur in `Module::process()`.
5. Do not download or depend on a Speak & Spell ROM, pronunciation corpus, TTS engine, neural model, or network service.
6. Do not modify unrelated Leviathan modules except where shared registration/build files require it.
7. Begin with straightforward scalar DSP and establish tests before optimizing.
8. When a repository helper named below has changed, use the current repository equivalent rather than creating a duplicate framework.
9. Generated corpus data must be deterministic.
10. The final authoritative build is the native Windows/MSYS2 `plugin.dll` build.

### 0.1 Explicit v1 non-goals

Do **not** implement these unless the specification is later extended:

- bit-exact TMS5100/TMS5200/TMS5220 emulation;
- dumped Speak & Spell ROM support;
- a physical alphabet keyboard;
- polyphonic speech;
- multilingual pronunciation;
- neural TTS;
- arbitrary runtime speech-database loading;
- phoneme-editor UI;
- multiline text editing;
- host-sample-rate LPC synthesis;
- cubic interpolation;
- mathematically exact formant-frequency shifting.

PHONEX v1 intentionally runs its speech engine at a vintage-like **8 kHz or 10 kHz internal clock**.

---

# 1. Product definition

PHONEX is a voltage-controlled LPC speech instrument behaving like a fictional circuit-bent educational speech toy rebuilt as a VCV Rack module.

It has three primary modes of interaction:

### Speech toy

Turn the `WORD` encoder to select recognizable bundled letters, numbers, words, and phrases. Trigger or press the encoder to speak them.

### Text speaker

Type English text into `UTTERANCE` and press Enter. PHONEX deterministically converts it into its internal phoneme representation and then into LPC frames.

### Circuit-bending instrument

Independently manipulate:

- speech pitch;
- speech transport speed;
- phrase position;
- LPC coefficient warp;
- voiced/unvoiced excitation;
- external excitation;
- chip-clock starvation;
- deterministic frame/data corruption.

At clean defaults, common words should usually be identifiable without reading the display.

Naturalistic TTS is **not** the goal. A conspicuously synthetic, buzzy, nasal, low-rate voice is desirable.

---

# 2. Clean-room requirement — LOCKED

PHONEX may use publicly documented LPC techniques and TI-era architectural ideas, but:

- no original commercial speech ROM is required;
- no dumped ROM is distributed;
- no dumped ROM is transformed into generated PHONEX corpus data;
- no build step depends on proprietary audio or binary assets;
- bundled speech data is Leviathan-authored or clearly redistributable;
- editable corpus sources carry provenance information.

The term `ROM` in PHONEX source code means immutable generated PHONEX speech data. It never means an original Speak & Spell ROM.

---

# 3. User-facing speech sources — LOCKED

The real-time DSP engine consumes a generic immutable `LpcSequence`.

It must not branch according to whether the sequence originated from:

1. the bundled PHONEX word bank;
2. typed text;
3. direct phoneme script.

Source-specific processing finishes before the audio engine receives the sequence.

---

# 4. Text entry — LOCKED

The panel contains a normal Rack `ui::TextField` named `UTTERANCE`.

Rules:

- Maximum submitted source: **256 bytes**.
- v1 pronunciation supports ASCII English letters, digits, apostrophes, hyphens, basic punctuation, whitespace, and explicit phoneme escapes.
- Unsupported Unicode is treated as a token boundary and produces a nonfatal status indication.
- Enter compiles the field, publishes the new sequence, selects `Text`, resets speech state, and starts playback.
- A failed compilation leaves the previous valid sequence active.
- Turning `WORD` selects `Bundled`.
- Pressing `WORD` retriggers the active source without changing it.
- Submitted text and active source persist in patch JSON.
- No multiline editor is required for v1.

Compilation may occur synchronously from the UI/control thread.

It must never occur from `Module::process()`.

---

# 5. Direct phoneme syntax — LOCKED

Bracketed text specifies phonemes directly:

```text
hello [L IY V AY AH TH AH N]
```

The v1 inventory is:

```text
AA AE AH AO AW AY
B CH D DH
EH ER EY
F G HH
IH IY
JH K L M N NG
OW OY
P R S SH T TH
UH UW
V W Y Z ZH
SIL
```

Vowels optionally accept stress suffixes:

```text
AH0
IY1
ER2
```

Stress levels:

- `0` = unstressed;
- `1` = primary;
- `2` = secondary.

An unknown token inside a direct phoneme escape rejects the submitted utterance with:

```text
BAD PHONE
```

Do not silently reinterpret malformed direct phoneme input.

---

# 6. Bundled word bank — LOCKED

`WORD` is a snapped integer from `0..63`.

The indices are a release compatibility contract.

| Index | Entry | Index | Entry |
|---:|---|---:|---|
| 0–25 | `A` through `Z` | 32 | `SIX` |
| 26 | `ZERO` | 33 | `SEVEN` |
| 27 | `ONE` | 34 | `EIGHT` |
| 28 | `TWO` | 35 | `NINE` |
| 29 | `THREE` | 36 | `HELLO` |
| 30 | `FOUR` | 37 | `READY` |
| 31 | `FIVE` | 38 | `SPEAK` |
| 39 | `SPELL` | 52 | `VOICE` |
| 40 | `AGAIN` | 53 | `SYNTH` |
| 41 | `CORRECT` | 54 | `GLITCH` |
| 42 | `WRONG` | 55 | `BEND` |
| 43 | `YES` | 56 | `ERROR` |
| 44 | `NO` | 57 | `WARNING` |
| 45 | `COMPUTER` | 58 | `START` |
| 46 | `MACHINE` | 59 | `STOP` |
| 47 | `ROBOT` | 60 | `ENTER` |
| 48 | `VOLTAGE` | 61 | `LISTEN` |
| 49 | `CIRCUIT` | 62 | `LEVIATHAN` |
| 50 | `SIGNAL` | 63 | `PHONEX` |
| 51 | `PITCH` | | |

Default is:

```text
36 — HELLO
```

Every bundled entry is stored as an authored phoneme script.

Bundled phrases must therefore work before general text G2P exists.

---

# 7. Rack module contract — LOCKED

Freeze enum ordering before the first public PHONEX release.

Future IDs are appended.

## 7.1 Parameters

| ID | Parameter | Range | Default | Meaning |
|---|---|---:|---:|---|
| 0 | `PITCH` | -2..+2 oct | 0 | speech pitch transpose |
| 1 | `VOCT_ATTEN` | -1..+1 | +1 | V/oct attenuverter |
| 2 | `SPEED` | -4..+4 | +1 | source-frame transport rate |
| 3 | `WARP` | -1..+1 | 0 | LPC coefficient/vocal-tract warp |
| 4 | `EXCITE_BLEND` | 0..1 | 0 | automatic → forced excitation |
| 5 | `BEND` | 0..1 | 0 | clock starvation / instability |
| 6 | `GLITCH` | integer 0..15 | 0 | corruption recipe |
| 7 | `WORD` | integer 0..63 | 36 | bundled speech selection |
| 8 | `WORD_PUSH` | momentary | 0 | retrigger |

`SPEED` values with:

```text
abs(speed) < 0.025
```

are exactly zero.

## 7.2 Inputs

```text
VOCT
TRIG_GATE
SCRUB_CV
WARP_CV
BEND_CV
EXT_EXCITE
```

Mappings:

```text
VOCT:
    standard 1 V/oct
    multiplied by VOCT_ATTEN

SCRUB_CV:
    -5 V = start
    +5 V = end

WARP_CV:
    +/-5 V = +/-1 warp

BEND_CV:
    +/-5 V = +/-1 additive bend

EXT_EXCITE:
    external speech excitation carrier
```

## 7.3 Outputs

```text
AUDIO
FRAME_CLK
EOX
```

`FRAME_CLK` and `EOX` emit 1 ms pulses.

## 7.4 Lights

```text
VOICED
FRAME
EOX
BEND
```

`BEND` brightness follows effective bend amount.

---

# 8. Fixed source-frame cadence — LOCKED

Every `LpcFrame` represents exactly:

```text
20 ms
```

of source speech at `SPEED = 1`.

Therefore:

```text
50 source frames / second
2048 frames = 40.96 seconds maximum sequence duration
```

Internal synthesis clock does **not** control phrase duration.

This distinction is fundamental:

```text
SPEED    -> speech time / frame transport
PITCH    -> excitation pitch
8/10 kHz -> low-level speech-chip clock/timbre
```

PHONEX can therefore slow a word down without automatically lowering its pitch.

---

# 9. Core data structures — LOCKED

```cpp
enum class PhonexExcitation : uint8_t {
    Silence = 0,
    Unvoiced,
    Voiced,
};

struct LpcFrame {
    float energy = 0.f;
    float pitchPeriod10k = 0.f;
    std::array<float, 10> reflection{};
    PhonexExcitation excitation = PhonexExcitation::Silence;
};
```

Contracts:

```text
energy:
    normalized 0..1

pitchPeriod10k:
    pitch period measured in 10 kHz reference-clock ticks

reflection:
    K1..K10

clean coefficient limit:
    abs(K) <= 0.985
```

`pitchPeriod10k` is used only for voiced frames.

Silence has zero energy.

Typical voiced pitch periods should stay approximately within:

```text
20..200 reference ticks
```

although the DSP must remain safe outside the ordinary corpus range.

---

# 10. Internal speech clock — LOCKED

Available context-menu modes:

```text
10 kHz — default
8 kHz
```

There is deliberately **no host-rate mode in v1**.

The source corpus is authored for the 10 kHz reference clock.

The same coefficients and pitch periods are used at 8 kHz.

Thus 8 kHz intentionally behaves as an underclock:

```text
8 kHz base pitch = 0.8 * 10 kHz base pitch
```

and the digital lattice resonances shift downward as well.

Source-frame timing remains 20 ms, so changing the speech-chip clock does not change the word's overall temporal cadence.

This provides a useful vintage underclock coloration while preserving independent `SPEED`.

Host-rate LPC synthesis is deferred because blindly processing the same digital LPC coefficients at 44.1/48/96 kHz would radically move their pole frequencies. A future high-rate mode would require an explicit pole-remapping design.

---

# 11. Sequence capacity — LOCKED

```cpp
static constexpr int PHONEX_MAX_FRAMES = 2048;

struct LpcSequence {
    std::array<LpcFrame, PHONEX_MAX_FRAMES> frames{};
    uint16_t frameCount = 0;
    uint16_t phraseId = 0xffff;
    uint32_t generation = 0;
};
```

Optional fixed-size phoneme-boundary metadata may be added.

Do not use a dynamically growing vector in the published audio sequence.

If compilation needs more than 2048 frames:

```text
TEXT TOO LONG
```

Reject the new utterance.

Do not truncate it.

Keep the previous valid sequence active.

---

# 12. Lock-free publication — LOCKED

Use two fixed-capacity `LpcSequence` slots.

Control/UI side:

```text
1. select inactive slot
2. completely construct sequence
3. assign generation
4. release-publish index/generation atomically
```

Audio side:

```text
1. check published generation
2. acquire newly published slot
3. use immutable sequence
```

The audio callback:

- never mutates a published source sequence;
- never destroys the final heap reference to sequence data;
- never locks a mutex;
- never allocates sequence storage.

Because both slots have fixed value storage, ordinary heap-lifetime transfer is unnecessary.

---

# 13. Transport behavior — LOCKED

Two persisted `TRIG_GATE` modes exist.

## 13.1 Retrigger Phrase — default

### SCRUB_CV disconnected

`SPEED` controls source transport:

```text
+1 = normal forward
+4 = 4x forward
 0 = frozen
-1 = normal reverse
-4 = 4x reverse
```

Transport speed never directly transposes speech.

### SCRUB_CV connected

Patching `SCRUB_CV` switches into absolute voltage scrub.

```text
normalized = clamp((voltage + 5) / 10, 0, 1)

position =
    normalized * (frameCount - 1)
```

While scrubbing:

- `SPEED` does not move the playhead;
- pitch synthesis continues normally;
- continuous CV moves through interpolated LPC frames;
- `FRAME_CLK` occurs when the observed integer frame changes;
- large position jumps emit at most one frame pulse.

### Retrigger

A retrigger from `TRIG_GATE` or `WORD_PUSH` resets:

- lattice state;
- voiced chirp phase;
- LFSR state;
- bend/starvation PRNG;
- reconstruction state as required;
- interpolation state;
- EOX arming.

For free-running transport:

```text
speed >= 0 -> start at first frame
speed <  0 -> start at last frame
```

In voltage-scrub mode, synthesis state resets but the next process step follows the patched CV position.

---

## 13.2 Advance One Frame

Each rising edge on `TRIG_GATE` moves exactly one frame.

```text
SPEED < 0  -> reverse
SPEED >= 0 -> forward
```

The magnitude of `SPEED` is ignored.

`SCRUB_CV` is ignored.

`WORD_PUSH` remains a complete utterance retrigger.

---

# 14. Frame interpolation — LOCKED

Transport position is floating-point frame position:

```text
i0   = floor(position)
i1   = min(i0 + 1, frameCount - 1)
frac = position - i0
```

When:

```text
frame[i0].excitation == frame[i1].excitation
```

linearly interpolate:

- energy;
- pitch period;
- K1..K10.

When excitation kind differs, suppress interpolation and hold `frame[i0]` until the next frame boundary.

The sequence compiler must insert explicit phonetic transition frames where smooth transitions are wanted.

Because interpolation depends only on position rather than direction, reverse speech traverses the same parameter path backwards.

---

# 15. Frame and EOX events — LOCKED

`FRAME_CLK`:

```text
1 ms pulse whenever observed integer frame index changes
```

A large scrub jump emits only one pulse.

Do not queue pulses for every skipped frame.

`EOX`:

```text
forward -> pulse on first arrival at last frame
reverse -> pulse on first arrival at first frame
```

EOX rearms only after:

- retrigger; or
- moving away from that boundary.

---

# 16. Typed-text pronunciation

Processing pipeline:

```text
input
 -> size validation
 -> normalization
 -> number expansion
 -> tokenization
 -> direct phoneme escapes
 -> dictionary lookup
 -> deterministic G2P
 -> letter-spelling fallback
 -> stress / timing / prosody
 -> phoneme transitions
 -> LPC sequence
 -> mailbox publication
```

Identical:

```text
text + corpus version + compiler settings
```

must create identical LPC frames.

---

# 17. Number support

v1 supports:

```text
0..9999
```

as ordinary English number words.

Larger integer strings are spoken digit-by-digit.

Example:

```text
12583
```

may become:

```text
ONE TWO FIVE EIGHT THREE
```

for values outside the supported normal number expansion range.

Also support:

```text
-12.4
```

as:

```text
MINUS TWELVE POINT FOUR
```

No general date, currency, measurement-unit, or mathematical expression parser is required.

---

# 18. Pronunciation dictionary

Editable source:

```text
tools/phonex_rom/pronunciations.tsv
```

Format:

```text
WORD<TAB>PHONEME PHONEME PHONEME...
```

Include at minimum:

- every bundled word;
- A–Z letter names;
- number vocabulary;
- irregular words needed by tests;
- `LEVIATHAN`;
- `PHONEX`.

The generated plugin must not parse this TSV at runtime.

---

# 19. Deterministic G2P fallback

Implement a compact ordered rule engine.

This is not intended to be a complete English linguistic system.

At minimum support common patterns including:

```text
CH
SH
TH
PH
NG
QU
CK
WH

EE
EA
AI
AY
OA

OI
OY
OW
OU

ER
IR
UR
```

Also implement:

- soft `C` before `E/I/Y`;
- soft `G` before `E/I/Y`;
- reasonable final plural `S`;
- common `ED` behavior where practical.

Dictionary entries take precedence over rules.

## 19.1 Unknown-word behavior — LOCKED

If the deterministic rule engine cannot confidently resolve an ordinary word, do **not** fail.

Spell it using the built-in A–Z pronunciations.

For example, an unknown token may become:

```text
X Y L O P H O N E
```

rather than disappearing or throwing an error.

This fallback is intentionally reminiscent of the educational-toy identity.

Malformed explicit `[PHONEMES]` remain errors because they are user-authored low-level instructions.

---

# 20. Phone timing

Source frames are fixed at 20 ms.

Seed durations:

| Phone class | Frames | Approx. time |
|---|---:|---:|
| stressed vowel/diphthong | 6 | 120 ms |
| unstressed vowel | 4 | 80 ms |
| nasal/liquid/approximant | 4 | 80 ms |
| fricative | 4 | 80 ms |
| affricate | 4 | 80 ms |
| stop closure + burst | 3–4 | 60–80 ms |
| word gap | 2 | 40 ms |
| comma/semicolon | 6 | 120 ms |
| sentence ending | 12 | 240 ms |

These may be tuned before public release.

After release, timing changes should be treated as corpus/version compatibility changes because they alter the audible result of existing text.

---

# 21. Toy prosody

Keep prosody deliberately simple.

Primary stress:

```text
energy approximately +10%
pitch approximately +1 semitone
```

Secondary stress:

```text
energy approximately +5%
```

Question marks raise pitch over the final voiced portion.

Normal sentence endings gently lower pitch over the final voiced portion.

Punctuation creates the specified silence.

Do not build a statistical or neural prosody model.

---

# 22. Clean-room phoneme corpus — LOCKED strategy

The first functional corpus is **synthetic**.

Do not block the implementation waiting for recorded speech.

Later versions may analyze Leviathan-owned recordings into LPC data, but direct synthetic speech must work first.

Editable inputs:

```text
tools/phonex_rom/phonemes.json
tools/phonex_rom/pronunciations.tsv
tools/phonex_rom/bundled_phrases.tsv
tools/phonex_rom/PROVENANCE.md
```

Generated data:

```text
src/PhonexRomData.inc
```

---

# 23. Phoneme prototype representation

Every phoneme resolves to one or more 10 kHz reference `LpcFrame` prototypes.

Editable corpus definitions may provide either:

1. explicit K1..K10 reflection coefficients; or
2. five synthetic formant frequency/bandwidth pairs.

For formant-defined frames, the generator:

```text
formant frequencies/bandwidths
 -> five conjugate pole pairs
 -> five second-order denominator sections
 -> order-10 LPC denominator
 -> step-down recursion
 -> K1..K10
```

Reject nonfinite or unstable results.

Generated coefficients must be deterministic.

Diphthongs use at least two anchor prototypes.

---

# 24. Initial vowel anchors

Use these as starting—not sacred—10 kHz synthetic formant targets.

| Phone | F1 | F2 | F3 |
|---|---:|---:|---:|
| `IY` | 270 | 2290 | 3010 |
| `IH` | 390 | 1990 | 2550 |
| `EH` | 530 | 1840 | 2480 |
| `AE` | 660 | 1720 | 2410 |
| `AA` | 730 | 1090 | 2440 |
| `AO` | 570 | 840 | 2410 |
| `AH` | 640 | 1190 | 2390 |
| `UH` | 440 | 1020 | 2240 |
| `UW` | 300 | 870 | 2240 |
| `ER` | 490 | 1350 | 1690 |

Initial shared higher formants may use approximately:

```text
F4 = 3500 Hz
F5 = 4500 Hz
```

Seed bandwidths may begin near:

```text
B1 = 60 Hz
B2 = 90 Hz
B3 = 150 Hz
B4 = 250 Hz
B5 = 300 Hz
```

These values exist to get PHONEX speaking, not to establish a claim of perfect human vocal modeling.

Diphthongs can initially morph:

```text
EY : EH -> IY
AY : AA -> IY
AW : AA -> UW
OW : AO -> UW
OY : AO -> IY
```

Human tuning may refine them before release.

---

# 25. Consonant synthesis strategy

A perfect acoustic model is unnecessary.

Use deliberately simple classes.

### Vowels/diphthongs

Voiced formant prototypes.

### Nasals/liquids/approximants

Voiced resonant prototypes.

### Fricatives

Unvoiced excitation through class-specific LPC spectral shaping.

Useful broad classes include:

```text
S / Z       -> high-frequency emphasis
SH / ZH     -> somewhat lower fricative emphasis
F / V       -> broad weaker high-frequency emphasis
TH / DH     -> diffuse mid/high-frequency emphasis
```

### Affricates

```text
short closure/burst
 -> fricative section
```

### Stops

```text
closure/silence
 -> short unvoiced burst
 -> transition toward following phone
```

### HH

Lightly shaped unvoiced excitation biased toward the following vowel.

### SIL

Zero energy.

The milestone is recognizable artificial speech, not linguistically immaculate synthesis.

---

# 26. Corpus generator validation

Command:

```sh
python3 tools/generate_phonex_rom.py --check
```

Validate:

- all 40 symbols including `SIL`;
- all bundled phrase scripts;
- all dictionary pronunciations;
- finite coefficients;
- clean coefficient stability;
- valid voiced pitch periods;
- correct excitation types;
- exactly 64 bundled entries;
- exact frozen bundled ordering;
- required provenance;
- deterministic generated output.

Running generation twice from identical source files must yield byte-identical generated data.

---

# 27. Internal synthesis scheduler

At host rate `Fs`:

```text
nominalInternalRate = 8000 or 10000

internalPhase += effectiveInternalRate / Fs
```

While:

```text
internalPhase >= 1
```

run an internal synthesis tick and subtract one.

Source-frame transport is based on host elapsed time and `SPEED`.

It is independent of the number of internal synthesis ticks.

---

# 28. Pitch — LOCKED

For voiced frames:

```text
pitchOctaves =
    PITCH
    + VOCT_ATTEN * VOCT

pitchScale =
    2 ^ pitchOctaves

periodTicks =
    pitchPeriod10k / pitchScale
```

Actual frequency is approximately:

```text
effectiveInternalRate / periodTicks
```

Thus:

- `PITCH` changes voice pitch;
- V/oct changes voice pitch;
- `SPEED` does not;
- 8 kHz underclock lowers pitch;
- `BEND` clock starvation can lower/disturb pitch.

Cache exponent conversion at control/internal-tick rate.

Use a fast path such as:

```cpp
rack::dsp::exp2_taylor5()
```

Do not call `pow()` every host sample.

---

# 29. Voiced excitation — LOCKED

Use this PHONEX-authored clean-room chirp:

```cpp
static constexpr float PHONEX_CHIRP[16] = {
     0.00f,  0.38f,  0.82f,  1.00f,
     0.68f,  0.32f,  0.04f, -0.22f,
    -0.40f, -0.34f, -0.26f, -0.18f,
    -0.11f, -0.06f, -0.02f,  0.00f,
};
```

At the beginning of every pitch period:

```text
chirpIndex = 0
```

The first 16 internal ticks read the table.

The remaining ticks of the pitch period output zero.

If extreme pitch modulation creates a period shorter than 16 ticks, the next period restart truncates the table.

This chirp becomes a release compatibility fixture.

It is intentionally not a copied TI chirp table.

---

# 30. Unvoiced excitation — LOCKED

Use a 17-bit maximal-length LFSR based on taps 17 and 14, giving a period of 131071 states. The 17/14 tap choice is a documented maximal-length configuration.

Freeze:

```cpp
static constexpr uint32_t LFSR_MASK     = 0x1ffffu;
static constexpr uint32_t LFSR_TOP_BIT  = 0x10000u;
static constexpr uint32_t LFSR_XOR_MASK = 0x12000u;
static constexpr uint32_t LFSR_RESET    = 0x1ace1u;

float nextNoise() {
    const float out =
        (lfsrState & 1u) ? 1.f : -1.f;

    lfsrState =
        ((lfsrState >> 1) | LFSR_TOP_BIT)
        ^ (((lfsrState & 1u) - 1u) & LFSR_XOR_MASK);

    lfsrState &= LFSR_MASK;
    return out;
}
```

Tests freeze:

- first 64 output signs;
- full 131071-state period;
- retrigger reset.

Do not later change the sequence without a compatibility mode.

---

# 31. Excitation selection

Automatic excitation:

```text
Silence  -> 0
Unvoiced -> LFSR
Voiced   -> chirp
```

Forced-excitation context setting:

```text
Voiced — default
Unvoiced
```

`EXCITE_BLEND`:

```text
0 -> automatic
1 -> forced

internal =
    lerp(automatic, forced, EXCITE_BLEND)
```

At exactly zero, the clean path must remain equivalent to automatic excitation.

---

# 32. External excitation

If `EXT_EXCITE` is patched:

```text
externalNormalized =
    clamp(inputVoltage / 5, -2, +2)
```

It completely replaces the internal excitation carrier.

However:

```text
excitationAfterEnergy =
    externalNormalized * frameEnergy
```

Therefore external excitation replaces the **carrier**, not the speech envelope.

Sample the external port on internal synthesis ticks.

---

# 33. LPC lattice — LOCKED

Use this sign/state convention:

```cpp
float processLattice(
    float excitation,
    const std::array<float, 10>& k
) {
    float u[11]{};
    u[10] = excitation;

    for (int i = 9; i >= 0; --i) {
        u[i] =
            u[i + 1]
            - k[i] * latticeState[i];
    }

    for (int i = 9; i >= 1; --i) {
        latticeState[i] =
            latticeState[i - 1]
            + k[i - 1] * u[i - 1];
    }

    latticeState[0] = u[0];
    return u[0];
}
```

Clean coefficients are clamped to:

```text
[-0.995, +0.995]
```

before lattice processing.

Standalone tests compare against an independently written double-precision reference using the same mathematical convention.

---

# 34. WARP — LOCKED

`WARP` is a **stylized LPC coefficient warp**.

It is not advertised internally or externally as mathematically exact formant translation.

Effective amount:

```text
warp =
    clamp(
        WARP + WARP_CV / 5,
        -1,
        +1
    )
```

Smooth at control rate.

Coefficient transformation:

```text
kWarped =
    tanh(
        k * (1 + 0.80 * warp)
    )
```

Do not execute ten standard-library `tanh()` calls every host sample.

Recompute only when:

- interpolated speech coefficients materially change;
- smoothed warp materially changes.

A fast approximation or lookup table is acceptable.

Clean warped coefficients are clamped to the normal stability limit before `BEND` instability is applied.

---

# 35. Reconstruction modes

Persisted modes:

```text
Raw hold — default
Filtered
```

### Raw hold

Hold the latest internal synthesis result until another internal synthesis tick occurs.

This intentionally exposes low-rate reconstruction images and grit.

### Filtered

Begin with the same held signal and apply a cheap host-rate low-pass reconstruction stage.

Approximate cutoff:

```text
min(
    0.45 * nominalInternalRate,
    0.45 * hostRate
)
```

A stable first-order or comparably cheap filter is sufficient.

This filter is not a historical hardware-emulation contract.

---

# 36. Output stage

After reconstruction:

```text
fixed calibrated gain
 -> bounded cheap soft clip
 -> Rack voltage
```

Target clean speech peaks:

```text
approximately +/-3 to +/-4 V
```

Extreme behavior remains approximately bounded by:

```text
+/-5 V
```

If DSP produces NaN or infinity:

```text
output = 0
clear lattice state
clear reconstruction state
resume safely
```

One invalid calculation must never poison later samples.

---

# 37. BEND / STARVE — LOCKED

Effective bend:

```text
b =
    clamp(
        BEND + BEND_CV / 5,
        0,
        1
    )
```

At:

```text
b = 0
```

the bend code must have **no sonic effect**.

Derive:

```text
slow =
    b * b

clockMult =
    1.0
    - 0.55 * slow

jitterAmt =
    0.12
    * clamp(
        (b - 0.25) / 0.75,
        0,
        1
      )

skipProb =
    0.30
    * clamp(
        (b - 0.45) / 0.55,
        0,
        1
      )^2

leakAmt =
    0.04
    * clamp(
        (b - 0.55) / 0.45,
        0,
        1
      )

overdrive =
    1.0
    + 0.10
    * clamp(
        (b - 0.70) / 0.30,
        0,
        1
      )
```

---

# 38. Bend clock behavior

Internal tick rate:

```text
effectiveInternalRate =
    nominalInternalRate
    * clockMult
    * jitterScale
```

After each attempted internal tick, choose a new:

```text
jitterScale =
    1
    + jitterAmt * randomBipolar
```

Hold it until the next attempted tick.

This makes BEND audibly resemble unstable/underpowered speech-chip clocking.

---

# 39. Bend PRNG — LOCKED

Use module-owned xorshift32:

```cpp
uint32_t xorshift32(uint32_t& x) {
    if (x == 0)
        x = 0x6d2b79f5u;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return x;
}
```

The serialized PHONEX seed initializes bend PRNG state on retrigger.

Never call Rack global randomness from the audio callback.

---

# 40. Starved synthesis ticks

For every scheduled internal tick, compare a PRNG sample with:

```text
skipProb
```

When skipped:

- do not advance chirp;
- do not advance LFSR;
- do not process lattice;
- hold the previous synthesis sample;
- leak lattice state:

```text
state *= 1 - leakAmt
```

When not skipped, process normally.

---

# 41. Controlled LPC instability

After ordinary WARP:

```text
kBent =
    kWarped * overdrive
```

Bent coefficient clamp:

```text
[-1.08, +1.08]
```

Only BEND may intentionally exceed unity.

Finite detection and bounded output remain mandatory.

---

# 42. GLITCH / MANGLE — LOCKED

`GLITCH` is integer:

```text
0..15
```

The source sequence remains immutable.

Corrupt a scratch-selected frame.

Corruption depends only on:

- serialized module seed;
- source frame index;
- selected glitch level.

This ensures repeatable reverse playback and scrubbing.

---

# 43. Stateless glitch hash — LOCKED

```cpp
uint32_t phonexFrameHash(
    uint32_t seed,
    uint32_t frameIndex,
    uint32_t level
) {
    uint32_t x =
        seed
        ^ (frameIndex * 0x9e3779b9u)
        ^ (level * 0x85ebca6bu);

    if (x == 0)
        x = 0x27d4eb2du;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return x;
}
```

Frame index remapping happens first.

Clamp remapped indices safely to:

```text
0 .. frameCount - 1
```

Then copy the selected source frame and apply field corruption.

---

# 44. Exact glitch recipes — LOCKED

| Level | Name | Recipe |
|---:|---|---|
| 0 | `CLEAN` | exact unmodified path |
| 1 | `HOLD-4` | every fourth frame selects previous frame |
| 2 | `SKIP-4` | every fourth frame selects next frame |
| 3 | `ADDR-X1` | `index XOR 1` |
| 4 | `ADDR-X2` | `index XOR 2` |
| 5 | `OFFSET-2` | hash selects `index - 2` or `index + 2` |
| 6 | `REV-4` | reverse frame order inside each group of four |
| 7 | `ENERGY-2BIT` | `round(energy * 3) / 3` |
| 8 | `PITCH-8` | voiced pitch period rounded to nearest 8 ticks |
| 9 | `PITCH-FOLD` | hash selects pitch period ×0.5 or ×2 |
| 10 | `K-SIGN` | negate K1, K4, K7, K10 |
| 11 | `K-ROTATE` | rotate K array right two positions |
| 12 | `K-HOLES` | zero K2, K4, K6, K8, K10 |
| 13 | `K-4BIT` | `round(K * 8) / 8`, then clean clamp |
| 14 | `VOICE-FLIP` | on odd frames swap Voiced ↔ Unvoiced; Silence unchanged |
| 15 | `BUS-SCRAMBLE` | index XOR 3 + 2-bit energy + 8-tick pitch + per-K hash sign mask |

Levels are **not cumulative**.

`GLITCH = 12` means recipe 12 only.

At level zero there must not be an extra quantization/copy/conversion stage that changes clean output.

---

# 45. Source layout

Suggested structure:

| File | Responsibility |
|---|---|
| `src/PhonexTypes.hpp` | frames, sequences, enums, capacities |
| `src/PhonexRom.hpp/.cpp` | generated phrase/phoneme lookup |
| `src/PhonexRomData.inc` | generated immutable corpus |
| `src/PhonexPronunciation.hpp/.cpp` | normalization, dictionary, G2P, phoneme parsing |
| `src/PhonexSequenceCompiler.hpp/.cpp` | phoneme timing, transitions, prosody |
| `src/PhonexSequenceMailbox.hpp` | fixed double-buffer publication |
| `src/PhonexEngine.hpp/.cpp` | transport, glitch, excitation, lattice, bend, reconstruction |
| `src/Phonex.hpp/.cpp` | Rack module and persistence |
| `src/PhonexWidget.cpp` | text/display/panel/menus |
| `tools/generate_phonex_rom.py` | corpus generation and validation |
| `tools/phonex_rom/` | editable corpus |
| `res/Phonex.svg` | master panel |
| `res/Phonex.panel.svg` | generated panel |
| `res/Phonex.labels.svg` | generated labels |
| `tests/phonex_engine_spec.cpp` | standalone DSP |
| `tests/phonex_pronunciation_spec.cpp` | pronunciation/compiler |
| `tests/phonex_module_spec.cpp` | Rack module |
| `tests/phonex_panel_contract_spec.py` | panel |

A custom historical-style packed speech bitstream is **not required in v1**.

Do not invent a pseudo-TI bitstream merely so glitch can corrupt it.

The explicit glitch layer already provides deterministic virtual data/bus corruption.

---

# 46. Rack enum ordering — LOCKED

```text
Params:
PITCH
VOCT_ATTEN
SPEED
WARP
EXCITE_BLEND
BEND
GLITCH
WORD
WORD_PUSH

Inputs:
VOCT
TRIG_GATE
SCRUB_CV
WARP_CV
BEND_CV
EXT_EXCITE

Outputs:
AUDIO
FRAME_CLK
EOX

Lights:
VOICED
FRAME
EOX
BEND
```

---

# 47. JSON schema v1 — LOCKED

Persist:

```text
schemaVersion = 1

submitted text
active source
internal rate
reconstruction mode
trigger mode
forced excitation target
32-bit corruption/starvation seed
```

Values:

```text
active source:
    Bundled
    Text

internal rate:
    8000
    10000

reconstruction:
    RawHold
    Filtered

trigger mode:
    RetriggerPhrase
    AdvanceOneFrame

forced excitation:
    Voiced
    Unvoiced
```

Normal Rack parameter values are already serialized by Rack and must not be duplicated.

Clamp malformed persisted state.

Persisted text must be compiled outside the audio callback.

---

# 48. Seed behavior

Default seed:

```cpp
0x50484f4eu
```

ASCII mnemonic:

```text
PHON
```

A context-menu action:

```text
Randomize corruption seed
```

is optional.

If implemented, randomization occurs only on the UI/control thread.

The resulting module-owned seed persists with the patch.

Retrigger never calls external/global randomness.

---

# 49. Panel contract

Master dimensions:

```text
71.12 mm × 128.5 mm
14 HP
```

Visual character:

- matte dark slate;
- strong readable labeling;
- cyan/amber indication;
- monochrome schematic/runic detail;
- electronic educational-toy cues;
- unmistakably Leviathan rather than copied Speak & Spell trade dress.

PHONEX should feel like **a Leviathan speech instrument haunted by an educational speech chip**.

---

# 50. Required panel anchors

The master SVG contains hidden anchors for:

```text
9 params
6 inputs
3 outputs
4 lights

UTTERANCE field rectangle
phrase/status display rectangle
WORD encoder/push target
```

`res/Phonex.svg` is authoritative.

Never manually edit:

```text
res/Phonex.panel.svg
res/Phonex.labels.svg
```

Regenerate using the repository's normal process:

```sh
python3 tools/split_svg_labels.py res/Phonex.svg --overwrite
make generate-panel-anchor-atlas
```

Use anchors rather than duplicated C++ coordinates.

---

# 51. Text/display UI

Follow current Leviathan `ui::TextField` approaches, especially the closest current equivalents of:

```text
OctaviaConsole.cpp
Deepcache.cpp
CrownstepSettingsOverlay.cpp
```

Display at minimum:

### Bundled mode

Show selected bundled entry.

### Text mode

Show compact text/source status.

### Errors

Support at least:

```text
BAD PHONE
TEXT TOO LONG
```

Widget creation must be safe when:

```cpp
module == nullptr
```

for module-browser preview.

---

# 52. Implementation Phase 1 — contracts and fixtures

Implement:

- core frame types;
- fixed sequence;
- procedural voiced/unvoiced/silence sequences;
- corpus-generator skeleton;
- chirp;
- LFSR;
- scalar/double reference lattice.

Exit when:

- generator `--check` runs;
- chirp tests pass;
- LFSR first-sequence and period tests pass;
- lattice impulse reference passes;
- empty/malformed sequence handling is safe.

Do **not** implement G2P yet.

---

# 53. Phase 2 — clean DSP and transport

Implement:

- 20 ms source transport;
- forward;
- reverse;
- freeze;
- voltage scrub;
- interpolation;
- retrigger;
- frame stepping;
- EOX/frame events;
- 8/10 kHz scheduler;
- clean excitation;
- lattice;
- reconstruction;
- output safety.

Exit when procedural LPC sequences produce audible standalone speech-like synthesis and every clean transport/DSP test passes.

BEND and GLITCH are not allowed to be necessary to make this phase work.

---

# 54. Phase 3 — WARP, BEND, GLITCH

Implement the exact contracts above.

Exit when:

- `BEND = 0` is clean invariant;
- `GLITCH = 0` is clean invariant;
- every glitch level has a direct unit test;
- bend repeatability is tested from fixed seed;
- extreme randomized tests remain finite.

---

# 55. Phase 4 — direct phoneme speech

Implement:

- 40-symbol phoneme inventory;
- synthetic prototype data;
- formant-to-LPC generator path;
- consonant classes;
- phone duration expansion;
- transitions;
- direct phoneme parser;
- 64 bundled entries;
- corpus validation/provenance.

Required direct audition phrases:

```text
HELLO
ROBOT
SPEAK
LEVIATHAN
PHONEX
```

Also verify A–Z and ZERO–NINE.

This is the **first real speech milestone**.

Do not wait for arbitrary-text G2P before assessing whether PHONEX actually sounds like a speech synthesizer.

---

# 56. Phase 5 — typed text

Implement:

- normalization;
- number expansion;
- dictionary;
- G2P rules;
- letter spelling;
- stress;
- timing;
- simple prosody;
- fixed-capacity compiler;
- mailbox publication.

Exit when:

- known test words produce expected phonemes;
- unknown normal words produce deterministic speech or spelling;
- malformed phoneme escapes report errors;
- over-capacity text is rejected;
- identical text produces identical sequences;
- audio processing performs zero text work.

---

# 57. Phase 6 — Rack wrapper

Implement:

- module enums;
- exact ranges/defaults;
- ports;
- triggers;
- pulses;
- lights;
- CV scaling;
- source switching;
- context settings;
- JSON;
- registration.

Update:

```text
plugin.hpp
plugin.cpp
plugin.json
```

as required by current repository conventions.

Exit when Rack headless tests pass.

---

# 58. Phase 7 — panel/UI

Implement:

- 14 HP SVG;
- anchors;
- split rendering;
- themed components;
- text field;
- status display;
- WORD encoder push;
- preview-safe behavior.

Exit when:

- panel contract passes;
- generated assets validate;
- typed submission works in Rack;
- module browser preview is safe.

---

# 59. Phase 8 — performance/release hardening

Profile:

- normal playback;
- voltage scrub;
- heavy bend;
- glitch;
- external excitation.

Audit `process()` for:

- allocation;
- locking;
- strings;
- dictionary access;
- file I/O;
- unnecessary transcendental functions.

Generate reference WAVs for human audition.

Run native Windows tests/build.

Dragon King Debug Terminal metrics may be added only if profiling justifies them and existing `Process`, `Step`, and `Draw` metric contracts are preserved.

---

# 60. DSP tests

Required coverage:

- chirp table/reset;
- LFSR initial sequence/reset/period;
- ten-stage lattice versus double reference;
- clean coefficient boundaries;
- bend unstable range;
- finite recovery;
- pitch at 8 kHz;
- pitch at 10 kHz;
- PITCH transpose;
- V/oct attenuation;
- scheduler long-run accuracy;
- fixed 20 ms frame cadence;
- forward transport;
- reverse transport;
- freeze;
- scrub mapping;
- frame-step mode;
- interpolation endpoints;
- hard excitation transitions;
- retrigger state;
- frame pulse behavior;
- EOX behavior;
- external excitation;
- reconstruction modes;
- bend-zero identity;
- seeded bend repeatability;
- all 16 glitch levels;
- glitch-zero identity;
- safe frame remapping;
- randomized finite/bounded-output testing.

---

# 61. Pronunciation tests

Required coverage:

- case normalization;
- whitespace;
- punctuation;
- integers;
- larger digit spelling;
- decimals;
- negative numbers;
- dictionary precedence;
- direct phonemes;
- stress suffixes;
- malformed phonemes;
- digraph rules;
- soft C/G;
- letter fallback;
- timing;
- punctuation silence;
- question contour;
- sentence-ending contour;
- maximum sequence rejection;
- deterministic compiled frames.

---

# 62. Corpus tests

Validate:

```text
40 phoneme symbols
64 bundled entries
frozen entry ordering
valid direct phone scripts
finite frames
stable clean coefficients
valid voiced periods
deterministic generated file
complete provenance
```

---

# 63. Rack tests

Validate:

- exact enum ordering;
- param ranges;
- defaults;
- snapped controls;
- port names;
- light names;
- CV scaling;
- trigger modes;
- 1 ms pulses;
- bundled/text switching;
- encoder retrigger;
- JSON round trip;
- malformed JSON;
- reset;
- sample-rate changes;
- no-text startup.

---

# 64. Panel tests

Validate:

- exactly 14 HP;
- dimensions;
- required anchors;
- generated label split;
- valid text/display rectangles;
- expected param/port/light anchor count;
- null-module preview.

---

# 65. Human audition gate

A speech synthesizer cannot be declared complete solely because its unit tests are green.

At clean defaults, render and audition:

```text
HELLO
SPEAK
ROBOT
COMPUTER
ONE TWO THREE
LEVIATHAN
PHONEX
```

A listener who knows the candidate vocabulary should normally be able to identify the words without following a diagnostic frame display.

Then audition:

```text
8 kHz mode
SPEED = 0.5
SPEED = 2.0
reverse
voltage scrub
forced unvoiced excitation
external excitation
moderate BEND
GLITCH 3
GLITCH 10
GLITCH 14
GLITCH 15
```

Human audition may tune phoneme prototypes, energy, transitions, and prosody before release.

It must not quietly redefine deterministic DSP contracts.

---

# 66. Build/validation

Add PHONEX test executables to the repository's current equivalents of:

```text
TEST_BINS_NON_RACK
TEST_BINS_RACK
```

Final expected validation:

```sh
python3 tools/generate_phonex_rom.py --check
python3 tests/phonex_panel_contract_spec.py
make validate-plugin-json
make -j10 test-fast RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"
make -j10
```

Use native MSYS2 MINGW64 for the final authoritative build.

Success means an actual native:

```text
plugin.dll
```

is produced.

If command names have changed in the repository, use their current equivalents and record the substitutions.

---

# 67. Definition of done

PHONEX v1 is complete when:

- no proprietary speech ROM is required or distributed;
- the bundled bank contains exactly the frozen 64 entries;
- bundled, direct-phoneme, and typed text all reach the same DSP engine;
- direct phoneme speech is functional and recognizable;
- typed text compilation is deterministic;
- ordinary unknown words fall back to letter spelling;
- malformed explicit phonemes produce deterministic diagnostics;
- every source frame represents 20 ms;
- speech transport and excitation pitch are independently controllable;
- 8 kHz and 10 kHz modes survive arbitrary host sample-rate changes;
- no underspecified host-rate LPC mode exists;
- reverse, freeze, scrub, retrigger, frame-step, FRAME_CLK, and EOX obey their contracts;
- chirp behavior is frozen;
- LFSR behavior is frozen;
- lattice sign convention is frozen;
- bend formulas are frozen;
- seed behavior is frozen;
- all 16 glitch recipes are frozen;
- `BEND = 0` preserves clean behavior;
- `GLITCH = 0` preserves clean behavior;
- pathological settings cannot leave the engine poisoned with NaN/infinity;
- output remains bounded;
- external excitation replaces the carrier but retains frame energy;
- `Module::process()` contains no allocation, lock, file I/O, dictionary lookup, text parsing, or pronunciation work;
- corpus generation is deterministic;
- corpus provenance is complete;
- panel generation passes;
- plugin manifest validation passes;
- `test-fast` passes;
- the authoritative Windows `plugin.dll` builds;
- clean speech passes the human audition gate.

---

# 68. Primary technical references

NIST FIPS 137, 2,400 bit/s LPC:

<https://nvlpubs.nist.gov/nistpubs/Legacy/FIPS/fipspub137.pdf>

Texas Instruments TSP50C0x/1x Family Speech Synthesizer Design Manual:

<https://www.ti.com/lit/ml/spss011d/spss011d.pdf>

Texas Instruments MSP50C3x User's Guide:

<https://www.ti.com/lit/ug/spsu006c/spsu006c.pdf>

Speak & Spell parameter-interpolator / learning-aid architecture patent:

<https://patents.searchlight.law/doc/US4189779>

17-bit maximal-length LFSR tap reference:

<https://www.digikey.com/en/articles/use-readily-available-components-generate-binary-sequences-white-noise>

---

# 69. Implementation priority

When tradeoffs occur, prioritize:

1. audio-thread safety;
2. deterministic behavior and patch compatibility;
3. recognizable toy speech at clean defaults;
4. playable transport and CV behavior;
5. expressive but bounded circuit bending;
6. performance;
7. visual polish;
8. pronunciation breadth.

A smaller deterministic PHONEX that convincingly says **HELLO** is preferable to a sprawling pseudo-TTS subsystem that does not yet convincingly sound like a speech chip.

The conceptual test is simple:

> PHONEX should feel like a speech chip that has been given modular-synthesis control surfaces—not a general-purpose text-to-speech engine hidden behind a Rack panel.