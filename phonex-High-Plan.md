# PHONEX — Spoken-Word Quality High Plan

**Status:** Quality audit complete; execution contract ready  
**Module:** `LEVIATHAN // PHONEX`  
**Date:** 2026-08-31  
**Scope:** Clean-default intelligibility, articulation, pronunciation, and
vintage LPC character

| Phase | State | Next action |
|---|---|---|
| Q0 harness and baseline | Complete | `q0-complete`: 186 deterministic renders plus post-TMS report |
| Q0.5 neutral clean path | Complete | `q05-clean-selected`: neutral WARP, linear safety stage, six-pole reconstruction |
| Q1 corpus | In progress | Q1a `S` accepted; Q1b FIRE/TRUCK pronunciation corrected |
| Q2 transitions | Waiting on Q1 | Replace universal midpoint with selective policy |
| Q3 pronunciation | Waiting on stable speech | Add exact-phone tests before new G2P rules |
| Q4 optional engine work | Deferred | Consider only after measured plateau |

Automation is front-loaded. Human listening is accumulated into scheduled
packets and never blocks independent engineering work.

## Executive conclusion

PHONEX's synthesis engine is stable, technically credible, and already carries
the intended vintage educational-toy identity. Its spoken-word quality is at
the level of a promising instrument prototype, but it has not yet passed the
human audition gate defined in `doc/phonex.md`.

The LPC/TMS architecture is sound, but the current clean runtime path is not yet
a trustworthy neutral reference. In particular, centered WARP still transforms
every reflection coefficient, and the always-active post-reconstruction output
curve changes level relationships and regenerates harmonics after filtering.
Those two boundary conditions must be resolved before corpus tuning can be
interpreted reliably. After that, the principal limitations are the speech
corpus, transitions between authored phone prototypes, and the pronunciation
model feeding the engine.

The recommended direction is therefore:

1. freeze the current sound as a legacy comparison baseline;
2. establish a repeatable quality and blind-listening harness;
3. establish an exact-neutral clean coefficient path and measure output staging
   and reconstruction variants in isolation;
4. retune consonants and excitation balance after TMS quantization;
5. introduce compact, context-aware transitions and timing;
6. improve typed-English pronunciation only after the authored speech bank is
   acoustically strong;
7. consider engine extensions only if corpus and transition work reach a
   measured plateau.

Do not globally suppress unvoiced noise. The previous static-reduction
experiment altered the desired voicing because excitation balance is part of
the phoneme identity. Reconstruction may be changed only if controlled tests
show that the current filter fails the image-rejection contract, and then only
to the lowest-cost design that passes it.

## Current quality assessment

| Subsystem | Current state | Present quality ceiling |
|---|---|---|
| LPC lattice and scheduler | Strong | Not presently limiting |
| TMS chirp and parameter reconstruction | Strong | Not presently limiting |
| Clean coefficient/output path | Centered WARP and output curve are non-neutral | Must be resolved before corpus tuning |
| Host-rate reconstruction filtering | Functional, but image rejection is not isolated | Controlled bake-off required |
| Vowel inventory | Clearly differentiated overall | Several close/confusable pairs |
| Consonant inventory | Functional but generic | Major intelligibility constraint |
| Phone transitions | Rudimentary | Major intelligibility constraint |
| Prosody and contextual timing | Minimal | Deliberately mechanical, but underdeveloped |
| Authored dictionary | Reliable scripts for 115 entries | Useful but small |
| General G2P | Very primitive | Major typed-text constraint |
| Automated verification | Excellent engineering coverage | Does not establish intelligibility |
| Human evaluation | Pending | Release gate remains open |

## What is already working

PHONEX has a strong synthesis foundation:

- public TMS5100 chirp and reconstruction tables;
- voiced chirp and deterministic unvoiced-noise excitation;
- stable order-10 reflection lattice;
- TMS-style eight-period parameter interpolation;
- deterministic 8 kHz and 10 kHz operation;
- two-pole filtered reconstruction with Raw Hold as an intentional alternate;
- finite recovery and bounded output;
- contextual HH aspiration;
- limited final-frame consonant-to-vowel tract shaping;
- deterministic bundled, direct-phoneme, and typed-text compilation;
- no text, dictionary, allocation, locking, or file activity in the audio path.

The architecture should be preserved. The current runtime sound should be
captured as the legacy comparison baseline, but it must not be mistaken for an
exact-neutral coefficient path.

## Evidence from the current renders

A fresh diagnostic set was rendered at 48 kHz to a temporary directory outside
the repository. The seven clean-default evaluation phrases showed:

- peaks between approximately `-4.7` and `-5.3 dBFS`;
- RMS levels between approximately `-13.9` and `-16.4 dBFS`;
- energy above 6 kHz approximately 17–29 dB below total energy;
- no clipping;
- frame-boundary sample steps below the render-wide 99.9th-percentile sample
  step.

These results do not support a numerical click, unstable lattice event, or
zero-order-hold reconstruction failure at ordinary frame boundaries. The
reported static-switching sensation is more plausibly the perceptual result of
full-duty unvoiced noise beginning and ending as a block.

The current filtered reconstruction is doing useful work. These whole-word
measurements rule out a frame-boundary click diagnosis, but they do not isolate
sample-rate images from legitimate fricative energy or harmonics regenerated by
the final output curve. A zero-coefficient controlled reconstruction test is
therefore required before retaining or replacing the filter. No global
noise-gain pass is justified.

### Vowel separation

Controlled `[B vowel T]` renders showed meaningful early-spectrum separation:

| Phone | Approximate early spectral centroid |
|---|---:|
| `AA` | 986 Hz |
| `EH` | 1596 Hz |
| `IH` | 2160 Hz |
| `IY` | 2850 Hz |

The vowel bank is therefore not collapsed. It has a usable acoustic foundation.

The closest measured early-spectrum pairs included:

| Pair | Relative early-spectrum distance | Interpretation |
|---|---:|---|
| `EY / IH` | 1.89 dB RMS | Close onset; EY trajectory must carry distinction |
| `AO / OW` | 1.96 dB RMS | Expected shared onset; OW trajectory must carry distinction |
| `UH / UW` | 3.36 dB RMS | Likely confusion candidate |
| `AE / EH` | 3.65 dB RMS | Likely confusion candidate |
| `AY / OY` | 4.40 dB RMS | Dynamic trajectory is important |
| `AA / AH` | 5.11 dB RMS | Distinct but worth listening evaluation |

Static spectral distance is not itself an intelligibility score, especially
for diphthongs. It identifies where controlled listening should begin.

### Consonant spectral ordering

Controlled vowel–fricative–vowel renders exposed a more direct corpus problem:

- `F` carried approximately 24.7% of its energy above 3 kHz;
- `S` carried approximately 4.8% of its energy above 3 kHz.

This is effectively backwards for the intended fricative classes. `S` should
normally be the sharper, brighter sibilant. The result demonstrates that the
currently authored reflection arrays are not reliably producing their intended
spectra after TMS coefficient quantization and synthesis.

This should be corrected in the corpus generator and source data, not with a
global output equalizer.

## Current intelligibility constraints

### 1. Consonants are generic and share too much material

Many consonants use hand-shaped reflection arrays rather than targets verified
against their realized post-quantization spectrum. Several phones deliberately
share a tract shape:

- `F` and the `P` release;
- `S` and the `T` release;
- `SH` and `CH`;
- `TH` and `DH`;
- voiced fricatives and their unvoiced partners.

Shared material can work in a compressed LPC system, but timing, energy, and
excitation must then provide stronger distinctions than they currently do.

TMS unvoiced frames retain only K1 through K4. K5 through K10 from an authored
unvoiced reflection array are discarded during reconstruction. Unvoiced phone
design must therefore be evaluated according to the four coefficients and
energy that actually reach the engine.

### 2. Most continuous phones are static

Most vowels, liquids, nasals, and continuous consonants have a single anchor
held for their complete duration. Diphthongs, stops, affricates, and the revised
voiced fricatives carry more internal structure, but the general corpus remains
an isolated-phone prototype bank.

This creates:

- abrupt changes of phoneme identity;
- held-resonator vowels;
- noise blocks that sound switched rather than articulated;
- little onset or release detail;
- minimal influence from neighboring phones.

### 3. Coarticulation is narrow

The current sequence compiler:

- inserts a single midpoint frame only when adjacent phones share excitation;
- switches voiced/unvoiced boundaries without interpolation;
- shapes HH toward the following voiced tract;
- shapes the final frame of an unvoiced consonant toward a following voiced
  tract.

These recent changes are useful, but they are not yet a general transition
model. Stops need deliberate closure, release, aspiration, and following-vowel
behavior. Nasals, liquids, and glides need context-dependent entry and exit.
Vowels should react to neighboring consonant place rather than always entering
at a context-free target.

### 4. Duration and prosody are mostly fixed

Phone duration currently changes little or not at all with:

- stress;
- word position;
- phrase position;
- surrounding consonants;
- the voicing of a following consonant.

Primary stress raises energy and pitch but does not lengthen the stressed
vowel. Sentence prosody is a small final voiced-frame pitch contour. This is
enough for synthetic cadence, but not enough to preserve word identity reliably
in more complex phrases.

Free-running transport mutes when it reaches the final source-frame index. This
satisfies the requirement that audio stop at EOX, but it means the last source
frame is not held for a complete 20 ms interval. Final consonants and nasals can
therefore be shortened. The preferred solution is to author terminal release
and silence inside the sequence while preserving exact silence after EOX.

### 5. General G2P is substantially weaker than authored vocabulary

The dictionary currently contains 115 authored pronunciations. These entries,
the 64 bundled entries, numbers, and direct-phoneme scripts are the strongest
linguistic paths.

Unknown ordinary words pass through a small ordered letter-rule engine. It does
not yet handle many common English behaviors:

- final silent E;
- contextual vowel pronunciation;
- final Y as `IY`;
- voiced `TH` in common function words;
- syllable stress discovery;
- schwa reduction;
- contractions;
- syllabic `-es` and `-ed` endings.

For example, an unknown `MAKE` is approximately reduced to `M AE K EH`, and a
word containing an apostrophe falls back to spelling letters. General typed
English should therefore not be evaluated as though it has the same maturity as
the authored bank.

Current G2P tests generally prove that a rule produces a deterministic nonempty
sequence. They do not prove that the resulting phone sequence is correct.

## Interpretation of the GEM report

`phonex-gem-report.md` remains useful as an architectural inventory, but it is
not a reliable spoken-quality verdict.

The report:

- makes perceptual claims such as "unmistakable" without recording a blind
  listening procedure;
- contains stale implementation counts;
- rates the pronunciation system near-perfect despite the deliberately small
  G2P implementation;
- declares release readiness while the master specification explicitly leaves
  human intelligibility pending.

Automated tests establish safety, determinism, correctness of contracts, and
audibility. They do not establish word recognition.

## Interpretation of the design review

`phonex-DR.md` reinforces the overall direction, especially the need for a
deterministic headless harness, post-quantization corpus analysis, pronunciation
tests, and selective rather than universal transition smoothing. Its most useful
claims were checked against the current source before being adopted here.

The source audit confirmed:

- centered WARP is not an identity operation because it applies the saturation
  map even when its control value is zero;
- the final output curve is always active, substantially boosts small signals,
  compresses frame-energy differences, and adds harmonics after reconstruction;
- forcing voiced excitation on an originally unvoiced frame can request a
  zero-pitch chirp and collapse toward silence;
- direct external excitation is sampled at the internal LPC rate without an
  anti-aliasing option;
- the compiler midpoint plus runtime TMS interpolation can smooth some attacks
  twice;
- the current two-pole reconstruction filter is useful, but the available
  whole-word measurements do not prove adequate image rejection.

The review's stale plugin-registration and test-availability statements are not
part of this plan. Its historical citations are treated as research leads, not
as implementation facts. PHONEX keeps TMS quantization and does not oversample
the entire lattice.

## Locked clean-default contracts

These contracts remove ambiguity from the implementation passes:

1. At `FORMANT = 0`, `WARP = 0`, `BEND = 0`, and clean GLITCH, the lattice must
   receive the exact reconstructed, quantized TMS reflection coefficients.
2. The center of bipolar WARP is exact identity. Any coefficient saturation or
   stability mapping belongs only to nonzero WARP, BEND, or GLITCH behavior.
3. The normal internal-speech path must not depend on always-active coloration
   after its final reconstruction filter. A final safety limiter may remain only
   if it is effectively transparent on clean corpus renders.
4. Clean reconstruction uses the lowest-cost filter that passes the controlled
   image-rejection contract. Raw Hold remains an intentional alternate mode.
5. Unvoiced noise is tuned per phone and per transition. There is no global
   static suppressor, gate, or blanket excitation reduction.
6. Exact silence after EOX is preserved. Audible final release is authored
   before EOX rather than created by holding output beyond the utterance.
7. Clean-path work must preserve deterministic output, TMS table quantization,
   and the low-rate fictional-chip identity.

Before changing any of these paths, render and retain a tagged legacy baseline.
PHONEX is unreleased, so an ongoing legacy-voicing compatibility mode is not
required; the legacy render is an A/B reference and rollback artifact.

## Improvement program

### Q0 — Establish a repeatable quality harness

Create a permanent quality toolchain before another subjective DSP change.

Add a single documented build target, provisionally
`make phonex-quality-audit`. All generated audio and reports must go under:

```text
build/phonex-quality/<tag>/
```

Do not write audition WAVs into the repository root.

The first immutable tag is `legacy-baseline`. The harness must render:

- the seven release-gate utterances;
- all 64 bundled entries;
- every vowel in one or more common consonant contexts;
- every consonant between shared vowels;
- stop, fricative, nasal, and liquid contrast sets;
- a short authored typed-text suite;
- a separate unknown-word G2P suite;
- alternate-rate and circuit-bent evaluation variants.

It must also render controlled synthetic probes that do not depend on the
corpus:

- zero-reflection-coefficient voiced and unvoiced excitation;
- fixed-energy/fixed-pitch impulses through each reconstruction candidate;
- fixed coefficients across the complete output-level range;
- clean WARP-center coefficient identity;
- forced-voiced excitation applied to originally unvoiced frames.

Each render set should include a machine-readable manifest containing:

- version/tag;
- source text or direct-phone script;
- compiled frame count;
- expected duration;
- source and realized excitation classes;
- RMS and peak;
- band energies and spectral centroid;
- post-quantization coefficient or formant diagnostics;
- passband ripple, first-image rejection, DC, limiter duty, and output-energy
  correlation for the controlled probes;
- focused engine timing for performance comparisons in a separate benchmark
  report.

The harness should also create randomized identifiers for blind listening so
the listener does not see the expected word before answering.

The manifest and acoustic metric reports must be machine-readable TSV or JSON.
Given the same source, sample rate, seed, and build, rerunning a tag in a
comparison location must produce byte-identical audio and acoustic metrics.
Wall-clock benchmark results are explicitly excluded from byte identity. A tag
is never silently overwritten; use a new tag for every implementation variant.

Q0 exits only when all 64 bundled entries and all controlled probes render,
determinism is tested, the legacy baseline is frozen, and no audition WAVs are
created outside `build/phonex-quality/`.

### Q0.5 — Establish the neutral clean path

Perform this pass before changing the corpus. Change and measure one variable at
a time, retaining an independently tagged render for every variant.

#### 1. Make centered WARP an exact identity

At clean controls, compare the coefficient array entering the lattice with the
TMS-reconstructed array. The maximum absolute error attributable to the control
path must be at most `1e-7`, with exact identity preferred. Implement the
nonzero-WARP transform using cached or inexpensive math suitable for an audio
hot path; do not add per-sample transcendental work merely to preserve the old
curve.

This change is locked and does not require a preference audition.

#### 2. Select the clean output stage

Compare the current output curve with a calibrated linear output and a linear
output followed by a rare safety limiter. Select the simplest candidate that:

- remains finite and bounded for the complete test corpus;
- produces normal clean peaks in the approximate `3.0–4.5 V` range;
- activates its limiter on fewer than `0.1%` of clean samples;
- does not reduce correlation between encoded frame energy and output RMS;
- introduces no avoidable DC or post-filter harmonic coloration.

If candidates are objectively tied, prefer the linear, lowest-cost path and
retain the alternatives for human audition.

#### 3. Measure reconstruction rather than assuming its order

Compare the existing two-pole reconstruction with four-pole and six-pole
minimum-phase candidates using the zero-coefficient probes. Choose the
lowest-order candidate that achieves:

- first-image rejection greater than `45 dB` for the controlled 1 kHz probe;
- passband ripple below `0.25 dB` through `0.38 ×` the internal frame-sample
  rate;
- finite, deterministic output at 48, 96, and 192 kHz host rates.

Profile each candidate at those host rates. Reject a higher-order filter that
costs more than `10%` additional PHONEX process time at 192 kHz unless the lower
orders fail the acoustic contract. This is a deliberately bounded
reconstruction decision, not permission for a general voice-filter redesign.

#### 4. Correct forced-voiced pitch continuity

When forced voiced excitation encounters an originally unvoiced frame, use the
most recent valid voiced pitch and a deterministic startup fallback equivalent
to approximately 125 Hz. The mode must produce bounded periodic excitation
rather than silence. This is isolated from the clean default and may land in
the same pass.

Q0.5 exits when the neutral-coefficient invariant is tested, one output stage
and one reconstruction design have been selected by the rules above, focused
performance results are recorded, all engineering tests pass, and the
legacy/current audition packet is available. External-excitation anti-aliasing
is measured in Q0 but remains an optional later control mode unless its result
reveals a clean-path defect.

### Q1 — Retune the corpus after TMS quantization

This is the highest-priority quality pass and the expected largest return.

Add post-quantization analysis to the corpus tooling. Authored formant or
reflection targets must be judged according to the TMS-reconstructed frames
that the engine actually consumes.

Work in this order:

1. Correct the realized spectral ordering of `S`, `SH`, `F`, and `TH`.
2. Calibrate phone-specific unvoiced energy instead of applying a global noise
   reduction.
3. Separate stop releases from the corresponding continuous fricative body.
4. Improve `B`, `D`, `G`, `JH`, and voiced-fricative onset structure.
5. Add onset, steady-state, and release anchors to important continuous phones.
6. Evaluate and tune `UH/UW`, `AE/EH`, `IH/EY`, and other measured close vowel
   pairs.
7. Preserve a coherent voice profile across pitch, bandwidths, F4/F5, and
   relative energy.

Every corpus edit must regenerate deterministic data and produce before/after
audition sets.

The first measurable corpus targets are:

- `S` has a higher spectral centroid and clearly more energy above 3 kHz than
  `SH` in shared contexts;
- `F` and `TH` remain lower-energy, diffuse fricatives rather than becoming
  substitute sibilants;
- stop bursts are shorter and independently shaped from continuous fricatives;
- improvements do not collapse the existing vowel separation measurements;
- no phone is repaired by changing a global clean-path gain or filter.

Q1 exits when the fricative and stop contrast sets pass their automated
spectral ordering checks, all 64 entries have updated renders, deterministic
corpus generation passes, and a randomized listening packet is ready. Human
recognition remains the authority when a spectral proxy and perception
disagree.

### Q2 — Add compact contextual transitions and timing

Do not build a huge triphone database. Introduce a small rule-driven transition
layer based on phone classes and articulatory place.

Useful phone classes include:

- vowel/diphthong;
- stop;
- fricative/affricate;
- nasal;
- liquid;
- glide;
- silence.

Useful place classes include:

- labial;
- dental;
- alveolar;
- palatal;
- velar.

The compiler should be able to author or derive:

- consonant onset toward the following vowel;
- vowel release toward the following consonant;
- stop closure, burst, aspiration, and voicing onset;
- nasal/liquid entry and exit;
- context-sensitive energy ramps;
- primary-stress duration changes;
- phrase-final lengthening;
- explicit terminal release and silence frames.

The current universal same-excitation midpoint frame should become selective.
Direct A-to-B transitions are the default candidate for plosives and
fricatives, while midpoint assistance remains available for vowels, sonorants,
and cases where it measurably improves continuity. This avoids combining a
compiler midpoint with runtime TMS interpolation when that double smoothing
smears an attack.

Terminal release must be explicit in the compiled sequence, followed by an
explicit silence frame and exact silence after EOX.

Q2 exits when the transition policy is covered by exact frame-sequence tests,
terminal phones receive their authored duration, the controlled contrast suite
shows no new discontinuity outliers, and before/after randomized renders exist.

### Q3 — Improve typed-English pronunciation

Do this after bundled and direct-phoneme quality are strong enough to evaluate
the linguistic front end independently.

First add exact expected-phone tests for every G2P rule. Compilation success is
not an adequate pronunciation assertion.

Then add:

- common function words;
- final silent-E rules;
- final-Y and context-sensitive Y rules;
- common vowel teams and context-sensitive `OW/OU` behavior;
- voiced and unvoiced `TH` distinctions;
- contractions and apostrophes;
- schwa in common unstressed positions;
- better plural and past-tense endings;
- basic stress assignment;
- a larger clean-room high-value dictionary.

Bundled-word recognition, authored dictionary recognition, and unknown-word
G2P recognition must remain separate quality scores.

Q3 exits when every new rule has an exact expected-phone test, the existing
dictionary and bundled-bank sequences remain deterministic, and the typed-text
quality packet reports dictionary and unknown-word results separately.

### Q4 — Consider excitation or engine extensions only after a plateau

If Q1 through Q3 reach a measured plateau, evaluate more invasive options one
at a time:

- per-frame excitation calibration separate from acoustic frame energy;
- alternating unvoiced/voiced microframes for voiced fricatives;
- optional mixed excitation;
- optional vintage fixed-point or saturation behavior;
- closer interpolation and end-of-stream comparison against the local
  `../ti_lpc` reference;
- optional anti-aliased external excitation alongside the existing raw mode;
- alternate authored voice profiles.

The local `ti_lpc` project is GPL-licensed. It is useful as a black-box
engineering and behavioral reference. Its implementation or vocabulary data
must not be copied or bundled without an explicit licensing decision.

Avoid introducing a high-rate naturalistic synthesis mode merely to improve a
few words. PHONEX's low-rate, quantized artificial voice is part of the product
identity.

## Execution contract

This section is the handoff contract for an implementation agent. It may execute
Q0 through Q3 in order without asking for routine design choices, provided it
obeys the locked contracts, phase exits, and stop rules below.

### Automation-first dependency order

The executable queue is:

```text
Q0 harness
  -> legacy baseline + deterministic probes + metric comparator
  -> Q0.5 coefficient identity
  -> output-stage bake-off
  -> reconstruction bake-off
  -> forced-voiced correction
  -> Q1 corpus metric loop
  -> Q2 exact transition loop
  -> Q3 exact pronunciation loop
  -> consolidated human audition packets
```

Once Q0 exists, every later phase must extend the same runner rather than create
one-off scripts or root-directory WAVs. Machine gates run first and eliminate
failing variants automatically. Passing variants are retained by tag. Listening
packets are generated in the background of the engineering sequence and queued
for the next available listener; they do not stop unrelated automated work.

Q3 test infrastructure and rule characterization may begin while Q1/Q2 renders
await listening, but changes to pronunciation outputs should be scored only
against the stable post-Q2 voice. Q4 remains strictly downstream of the Q1-Q3
plateau.

### Work order and expected ownership

| Phase | Primary implementation surfaces | Required proof |
|---|---|---|
| Q0 | `tools/phonex_render.cpp`, new quality-analysis tooling, `Makefile`, engine tests | Deterministic tagged baseline, all-bank manifest, controlled probes |
| Q0.5 | `src/PhonexEngine.*`, focused engine tests | Neutral coefficients, selected output/filter path, timing report |
| Q1 | `tools/phonex_rom/`, `tools/generate_phonex_rom.py`, generated ROM, corpus tests | Post-TMS targets, contrast metrics, before/after bank |
| Q2 | `src/PhonexSequenceCompiler.*`, compiler tests | Exact transition scripts, terminal release, contrast renders |
| Q3 | `src/PhonexPronunciation.*`, pronunciation fixtures/tests | Exact expected-phone cases and separate G2P score |

Generated source remains generated: edit the ROM inputs and generator, then
regenerate `src/PhonexRomData.inc`; do not hand-edit the generated include.
Build and audition artifacts remain under `build/phonex-quality/` and are not
source-controlled deliverables.

### Per-change loop

For each independently meaningful change:

1. choose one hypothesis and name a new render tag;
2. run the focused pre-change test or render;
3. implement only that variable;
4. run focused tests and regenerate data if applicable;
5. render the same corpus/probes and compare machine metrics;
6. retain the variant if it meets its phase contract, otherwise revert only
   that isolated experiment;
7. record the result and decision in this document before moving on.

Do not combine coefficient neutrality, output staging, reconstruction order,
and corpus tuning into one unauditable sound change.

Maintain a compact decision log here as phases execute:

| Tag/change | Hypothesis | Automated result | Listening result | Decision |
|---|---|---|---|---|
| `legacy-baseline` / `q0-complete` | Preserve the pre-neutral-path voice for comparison | 186 renders, all 64 entries, isolated/context phones, probes, blind packet, post-TMS frames; deterministic rerenders passed | Deferred | Frozen; Q0 complete |
| `q05-warp-neutral` | Centered WARP must be exact identity | Exact coefficient test passed; 180/186 renders changed, up to 4.46 dB RMS and 604 Hz centroid | Deferred | Selected by locked invariant |
| forced-voiced fallback | Zero-pitch unvoiced frames need a deterministic pitch | Probe changed from silence to -25.36 dBFS; focused test passed | Not required | Selected; recent voiced pitch with approximately 125 Hz startup fallback |
| `q05-output-linear` / `q05-output-limited` | Remove the always-active legacy compressor | Initial 2.5x calibration clipped or limited too often | Deferred | Rejected; recalibrate |
| `q05-output-linear-unity` | Unity linear staging should restore dynamics with headroom | No clean clipping; core peaks 2.08–4.00 V; energy/RMS correlation 0.99997 | Deferred | Safe but slightly conservative |
| `q05-output-limited-1p1` | 1.1x linear staging plus a rare safety knee should use headroom transparently | All 186 renders unclipped; clean limiter duty 0%; core peaks 2.29–4.40 V; energy/RMS correlation improved from 0.99704 to 0.99997 | Deferred | Selected as runtime default |
| `q05-filter49-2pole` | Two poles may be sufficient with cutoff near internal Nyquist | 1.277 dB passband ripple; 30.67 dB first-image rejection | Not required | Rejected |
| `q05-filter49-4pole` | Four poles may meet both reconstruction targets | 0.480 dB passband ripple; 43.28 dB first-image rejection | Not required | Rejected |
| `q05-filter49-6pole` / `q05-clean-selected` | Six poles should meet both targets at acceptable cost | 0.170 dB ripple; 55.51 dB rejection; 192 kHz median 49.97 ns/sample versus 46.19 for two poles (+8.2%); 196 deterministic renders passed | Deferred | Selected as runtime default; Q0.5 complete |
| Q0.5 Windows validation | Confirm the selected engine under the authoritative toolchain | Native `test-fast` passed; the PHONEX binary was then force-rebuilt to avoid a stale `.exe` and passed 109748 checks; incremental MINGW64 `plugin.dll` built successfully | Rack audition deferred | Engineering gate passed; proceed to Q1 |
| `q1a-s-candidate1` | A moderate post-TMS `/s/` target should separate `S` from `SH` without a level boost | `S` centroid 1.94→3.07 kHz and 3–6 kHz energy 1.23%→64.55%; `SH` remained 1.94 kHz/15.05%; isolated `S` remained slightly quieter than `SH`; 26/196 renders changed; deterministic, bounded, unclipped; native 109748-check test and `plugin.dll` build passed | Listening packet ready | Selected as incremental Q1a corpus change |
| `q1b-fire-truck` | FIRE sounding like FUR is a pronunciation-path defect | Fallback was `F ER EH`; authored result is exactly `F AY1 ER SIL T R AH1 K`; deterministic typed packet and 109749-check native Windows test passed; authoritative `plugin.dll` built | Listening sample ready | Selected; retain as exact dictionary pronunciations |

### Autonomous decision rules

The implementation agent may proceed without a listening response when:

- a locked invariant is being corrected;
- one candidate uniquely passes the stated objective thresholds;
- candidates tie and the simplest, lowest-CPU, exact-neutral candidate can be
  selected while preserving tagged alternatives;
- corpus ordering or exact phone-sequence tests give a clear pass/fail result.

Human listening is required to declare the recognition gates passed and to
resolve a perceptual preference that objective measures cannot separate. A
human checkpoint does not prevent continued work on independent tests,
pronunciation rules, or tooling. The agent should present one compact randomized
audition packet after Q1 and another after Q2 rather than interrupting for every
phone adjustment.

### Stop and rollback rules

Stop the affected experiment, preserve its report, and return to its last
passing tagged state if it:

- violates a locked clean-default contract;
- creates non-finite output, clipping, unexpected DC, or a boundary-step
  regression;
- breaks determinism, audio-thread safety, ROM generation, or existing patch
  behavior outside PHONEX;
- reduces a previously passing phonetic contrast without a documented listening
  benefit;
- exceeds the reconstruction CPU allowance without being required to satisfy
  image rejection;
- requires global noise suppression to repair one phone.

Three consecutive failed approaches to the same blocking defect, or a true tie
that changes the module's voice materially, is the point to request user
direction. Ordinary failed variants are not blockers.

### Validation commands

The implementation should add `make phonex-quality-audit`; until it exists,
`build/tools/phonex_render` is the baseline renderer. Routine focused checks are:

```sh
python3 tools/generate_phonex_rom.py --check
python3 tests/phonex_panel_contract_spec.py
make build/tests/phonex_engine_spec
make build/tests/phonex_module_spec
make phonex-quality-audit
```

Run the authoritative native Windows fast suite before a phase is declared
complete:

```sh
/mnt/c/msys64/usr/bin/bash.exe -lc \
  'export MSYSTEM=MINGW64; \
   export PATH=/mingw64/bin:/usr/bin; \
   hash -r; \
   cd /home/Plasm/Leviathan && \
   make -j10 test-fast \
     RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"'
```

After Q0.5 and at the final handoff, also run the authoritative incremental
`make -j10 plugin.dll` bridge command documented in
`doc/windows_build_from_wsl.md`. Do not run `install`, stage files, or commit.

### Definition of plan completion

The engineering program is complete when Q0 through Q3 meet their exit
criteria, the engineering and transform gates pass, the authoritative Windows
tests and plugin build pass, artifacts and decisions are documented, and no
known clean-path defect remains. Product/release completion additionally
requires the human core, bundled-bank, contrast, and typed-text listening gates.

## Quality gates

### Engineering gate

Every pass must retain:

- deterministic generated corpus data;
- finite and bounded output;
- zero audio-thread allocation and locking;
- exact-neutral clean-zero behavior for all bending controls;
- no material process-time regression outside an explicitly accepted measured
  reconstruction tradeoff;
- passing engine, module, panel, and aggregate tests;
- authoritative native Windows `plugin.dll` validation.

### Core blind-listening gate

At clean defaults, randomize and audition:

```text
HELLO
SPEAK
ROBOT
COMPUTER
ONE TWO THREE
LEVIATHAN
PHONEX
```

The listener must not see the expected answer while the sample plays. Record
the free-response answer and, separately, a forced choice among the seven
candidates.

The initial acceptance target is:

- all seven recognizable under forced choice;
- at least six of seven recognizable by free response for a listener familiar
  with PHONEX's synthetic voice;
- no result accepted solely because the listener followed a displayed word or
  frame script.

### Bundled-bank gate

Randomize the complete 64-entry bank and record per-entry recognition. Use the
confusion data to prioritize corpus work rather than tuning only favorite
examples.

Letters, digits, and words should be scored separately because their candidate
sets and recognition difficulty differ.

### Phonetic contrast gate

Blind-test controlled contexts for:

- vowel pairs;
- `S/SH/F/TH`;
- `P/T/K`;
- `B/D/G`;
- voiced/unvoiced fricative pairs;
- `M/N/NG`;
- `L/R/W/Y`.

A phone does not pass merely because its isolated waveform differs. Listeners
must distinguish it in a word-like context.

### Typed-text gate

Score separately:

1. authored dictionary words;
2. numbers;
3. direct-phone escapes;
4. unknown words handled by G2P;
5. short multiword sentences.

Do not allow strong dictionary performance to hide weak general G2P.

### Transform robustness gate

Once clean speech passes, repeat representative listening at:

- 8 kHz mode;
- `SPEED = 0.5`;
- `SPEED = 2.0`;
- reverse;
- scrub;
- forced unvoiced excitation;
- external excitation;
- moderate BEND;
- GLITCH 3, 10, 14, and 15.

These modes need not preserve ordinary intelligibility, but their behavior must
remain intentional, finite, and musically useful.

## Immediate next pass

Q0, Q0.5, and the first Q1a `/s/` correction are complete. Continue Q1
incrementally:

1. use `q1a-s-candidate1` as the current corpus baseline while retaining
   `q05-clean-selected` for pre-Q1 comparison;
2. audit reported phrases for dictionary/G2P errors before changing acoustics;
3. review diphthong and rhotic trajectories (`AY/ER`, `EY/IH`, `AO/OW`) in
   controlled contexts;
4. review the remaining close vowel pairs (`UH/UW`, `AE/EH`, `AA/AH`) one pair
   at a time;
5. collect the user's weak isolated-letter list and map each letter to its phone
   sequence before changing shared material;
6. rank any needed `SH/F/TH` or stop-release candidates by isolated spectral
   centroid, 3–6 kHz energy, total energy, and separation;
7. change only one source phone or release anchor at a time in
   `tools/phonex_rom/phonemes.json`,
   regenerate the ROM include, and render a new immutable tag;
8. require `S` to exceed `SH` in both centroid and 3–6 kHz energy while `F` and
   `TH` remain lower-energy and diffuse;
9. reject candidates that materially collapse vowel metrics, introduce clipping
   or discontinuity outliers, or require a global gain/filter adjustment;
10. retain the best machine-passing candidate and then move to stop-release
   separation while the randomized Q1 listening packet accumulates.

No further clean-path reconstruction, output-stage, or global noise change is
authorized during Q1.

## Expected attainable result

With a tuned post-quantization corpus and better context rules, the current
engine should be capable of strong recognition across the bundled vocabulary
and stylized short sentences while retaining a conspicuously synthetic,
nasal, quantized educational-toy voice.

Arbitrary typed English will remain uneven until the G2P and dictionary passes
are completed. Naturalistic TTS is neither expected nor desirable. The target
is a highly intelligible fictional speech chip, not a modern speech service.
