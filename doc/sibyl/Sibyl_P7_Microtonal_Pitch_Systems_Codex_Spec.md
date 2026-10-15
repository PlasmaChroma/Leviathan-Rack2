# Sibyl P7 — Explicit Microtonal Pitch Systems

**Status:** implementation specification, not implemented code.  
**Target:** Leviathan-Rack2, `lumin-render`, extending the implemented P1–P6 composition features.  
**Proposed composition schema:** 4; retain schema-2 and schema-3 import.  
**Primary acceptance tunings:** 38 and 53 equal divisions of the octave (38-EDO and 53-EDO). Both are required throughout P7.  
**Design principle:** a tuning is data; twelve-tone assumptions must not define the new pitch model.

## 1. Objective and scope

Make microtonality a first-class authoring, editing, harmony, voicing, and inspection capability in Sibyl. An agent must be able to request a 38-EDO or 53-EDO pattern, move selected notes by one tuning step, reuse it with scene transposition, define native chords, and inspect the resulting voltages without calculating every note itself. The 53-EDO requirement includes its accurate 31-step approximation of the pure 3/2 fifth, with no special-case playback path.

Do not implement a collection of unrelated `EDO_19`, `EDO_31`, and `EDO_38` branches. Implement one bounded equal-division engine with configurable division count and repeat period. Add presets as ordinary data. Also support unequal periodic cents/ratio tables through the same compiled representation. This makes further tunings inexpensive once the integration is complete.

The deliverable includes:

- Native equal-division and unequal-table tunings, scales, pitch contexts, and microtonal events.
- Unit-safe targeted edits, scene offsets, explicit pitch-preserving conversion and quantization.
- Native chord tones and roles, nearest-tone selection, and bounded microtonal voicing.
- Versioned serialization, semantic queries, adoption behavior, presets, Scala `.scl` import, and regression tests.

Out of scope: automatic adaptive just intonation, live inference from external CV, continuous tuning morphs, MIDI/MPE/MTS or `.kbm` keyboard mapping, new output ports, arbitrary note-name spelling systems, and changing oscillator DSP. Those can build on this model later. Scene/track-dependent tuning inheritance is also deferred: P7 uses explicit, statically resolvable pitch contexts for authored events and progressions. Scenes can select different progressions and apply musical offsets without making every static event scene-dependent.

## 2. Source-grounded integration baseline

The current branch contains stable event IDs, repeat conditions, assignment overrides, automation, harmonic events, and bounded voicing. These are the baseline, not features to reimplement. The following observations come from the source files listed in §22:

| Current location | Integration fact |
|---|---|
| `SibylTypes.hpp` | `PitchType` includes `PITCH_V`, `DEGREE`, `NOTE`, and `HARMONIC`; static pitch compilation still stores a float voltage. [S1] |
| `SibylJSON.cpp` | The codec supports schema 2/3; named scales and `degreeToPitchV()` use semitone interval tables. [S2] |
| `SibylHarmonyTypes.hpp`, `SibylHarmonyJSON.hpp` | Chords have integer semitone roots/intervals; role matching and nearest candidates assume twelve pitch classes. [S3–S4] |
| `SibylHarmony.hpp` | Playback selects precompiled harmonic pitches; preserve that control-side compilation boundary. [S5] |
| `SibylNoteEdit.cpp`, `SibylOverrides.hpp` | Note transposition accepts semitones/degrees; assignment pitch offset is semitone-based. [S6–S7] |
| `SibylVoicing.hpp`, `SibylVoicingEdit.hpp` | Voicing uses integer pitches, modulo-12 coverage, bounded search, and fixed-note materialization. [S8–S9] |
| `SibylHarmonyView.hpp`, `SibylAdoption.cpp` | Derived context reporting and adoption comparisons also need tuning awareness. [S10–S11] |
| Existing harmony/voicing tests and `Makefile` | Extend the regression suites and the exhaustive voicing oracle. [S12–S14] |

This was a focused remote source review, not a checked-out build or proof that all existing tests pass. The large current `Sibyl.cpp` and `SibylEdit.cpp` fetches were not fully readable through the browser; verify their current dispatch/runtime paths locally. The source branch is mutable and no immutable commit was established. Record `git rev-parse HEAD` when implementation begins and treat the checked-out implementation as authoritative where it differs from earlier specifications.

## 3. Non-negotiable compatibility rules

1. **Keep 1 V/oct output.** Tuning changes the voltages authored, not the electrical convention. Rack specifies frequency as `f = f0 * 2^V`, with nominal C4 as the audio oscillator baseline. [S15]
2. **Never redefine a semitone.** Existing `transposeSemitones: 1` remains exactly the legacy 100-cent operation, not one step of the active EDO.
3. **Never silently retune old pitches.** `note`, legacy `degree`/`octave`, raw `pitchV`, and legacy `root`/`intervals` chords retain their existing interpretation. `meta.root`, `meta.rootOctave`, and `meta.scale` continue to govern legacy degree notes only.
4. **Keep authored representation.** A new degree note remains a degree note after transposition; an explicit ratio remains an authored ratio through save/load. Derived floating-point pitches are not substituted into ordinary serialization.
5. **Preserve deterministic scheduling.** Tuning changes do not alter probability seeds, note identities, condition ordinals, microshift timing, ratchet counts, or automation clocks.
6. **Preserve the old arithmetic path when new fields are absent.** Do not require new double-precision compilation to produce bit-identical results to every legacy float expression. Instead retain the legacy path, including its existing operation order, and test it independently.
7. **Never silently quantize.** A 5/4 ratio stays 5/4 unless the user explicitly requests approximation onto a tuning grid.
8. **Treat tuning as composition-owned data.** A saved composition must not depend on a preset name, an external file remaining at a path, or another module's current state.

Do not mechanically replace every `12` in the source. Semitone conversion, conventional note names, and legacy compatibility legitimately use twelve. New native grid arithmetic must not.

## 4. Mathematical contract

### 4.1 Absolute voltage and intervals

All newly compiled pitch calculations use `double`; cast once to the existing float output/storage boundary where practical. An interval represented by a frequency ratio `r > 0` corresponds to:

```text
intervalVolts = log2(r)
intervalCents = 1200 * log2(r)
intervalVolts = intervalCents / 1200
nominalFrequencyHz(V) = 261.6255653005986 * 2^V
```

The frequency value is a nominal display/calibration convention, not a measurement of a downstream oscillator with an arbitrary frequency-knob setting. The formulas follow the Rack voltage convention. [S15]

A pitch context supplies an anchor voltage `A`. An equal division of period ratio `R` into `N` steps has period voltage `P = log2(R)` and native lattice pitch:

```text
L(k) = A + k * P / N
```

For an octave, `R = 2` and `P = 1`. Therefore 38-EDO step `k` is simply `A + k/38`, and one step is approximately 31.578947368421 cents. Positive and negative indices use the same formula.

For 53-EDO, `L(k) = A + k/53`, and one step is approximately 22.641509433962 cents. The nearest grid approximation of a pure fifth is 31 steps: `31/53 V`, or 701.886792452830 cents. Relative to `1200 * log2(3/2)`, its signed error is approximately -0.068208412557 cents (slightly flat). This is an approximation, not an exact ratio: an explicitly authored `3/2` must still compile to `log2(3/2) V`. Reference values here describe ideal pitch compilation; downstream oscillator accuracy is separate.

### 4.2 Unequal periodic tables

Let a tuning contain ordered positions `c[0..N-1]`, in cents within a period `C`, with `c[0] = 0` and every position strictly below `C`. Unison is included; the repeated endpoint is not.

```text
q = floor_div(k, N)
i = floor_mod(k, N)             // 0 <= i < N, including negative k
L(k) = A + q * C/1200 + c[i]/1200
```

Use checked 64-bit intermediates. C++ truncating division is not floor division for negative values. Never wrap a negative index using a negative remainder as an array index.

### 4.3 Scale degrees versus tuning steps

A scale is an ordered subset of tuning positions `s[0..M-1]`, beginning at zero. A native degree `d` resolves to a lattice index:

```text
q = floor_div(d, M)
i = floor_mod(d, M)
k = q * N + s[i]
```

Additional authored `periods: p` adds `p*N` to a lattice index. An omitted scale means the complete lattice, not a guessed seven-note scale. Tuning step 12 and scale degree 12 are different quantities.

### 4.4 General periods

A period is a repetition interval, not necessarily an octave. Support `R = 3` for thirteen equal divisions of a tritave using exactly the same engine:

```text
L(13) - L(0) = log2(3) = approximately 1.584962500721 V
```

`periods: 1` repeats that system's period. Existing `octave: 1` remains a 2/1 octave. Do not use the names interchangeably in JSON, UI, or generated instructions.

## 5. Schema 4: tunings, scales, and contexts

Add an optional composition-level `pitchSystems` object. Its four fields are `tunings`, `scales`, `contexts`, and `defaultContext`. Unreferenced definitions are retained through the codec but do not trigger audible changes.

```json
{
  "schemaVersion": 4,
  "pitchSystems": {
    "tunings": {
      "edo38": {
        "kind": "equal",
        "divisions": 38,
        "period": {"ratio": "2/1"}
      },
      "tritave13": {
        "kind": "equal",
        "divisions": 13,
        "period": {"ratio": "3/1"}
      }
    },
    "scales": {
      "edo38_12tet_major_nearest": {
        "tuning": "edo38",
        "steps": [0, 6, 13, 16, 22, 28, 35]
      },
      "edo38_ji_major_nearest": {
        "tuning": "edo38",
        "steps": [0, 6, 12, 16, 22, 28, 34]
      }
    },
    "contexts": {
      "edo38_C4": {
        "tuning": "edo38",
        "scale": "edo38_ji_major_nearest",
        "anchor": {"note": "C4"}
      },
      "tritave13_C4": {
        "tuning": "tritave13",
        "anchor": {"note": "C4"}
      }
    },
    "defaultContext": "edo38_C4"
  }
}
```

These are proposed fields, not fields supported by the current schema-3 reader.

### 5.1 Tuning definitions

An equal definition has `kind: "equal"`, integer `divisions`, and `period`. A table definition has `kind: "table"`, `period`, and `positions`:

```json
{
  "kind": "table",
  "period": {"ratio": "2/1"},
  "positions": [
    {"ratio": "1/1"},
    {"ratio": "9/8"},
    {"ratio": "5/4"},
    {"ratio": "4/3"},
    {"ratio": "3/2"},
    {"ratio": "5/3"},
    {"ratio": "15/8"}
  ]
}
```

Each `period` or table position contains exactly one of `ratio` or `cents`. Ratios are strings `positive_integer/positive_integer`; reduce by GCD for canonical serialization. Support numerator and denominator through 2,147,483,647, also satisfying the Scala ratio-size expectation. [S16] Avoid integer division. Compute the logarithm in double precision.

Optional `name` and `description` are metadata. An optional `source` object may contain bounded `format`, `name`, and `description` strings recording import provenance; no path is a playback dependency. Metadata is serialized but excluded from audible signatures.

### 5.2 Scales

Each scale references exactly one tuning. `steps` contains unique, strictly ascending integers in `[0,N)`, with zero first. Do not append `N` as an octave endpoint. Scale and context tuning references must agree. Optional display labels may accompany positions, but labels never determine their pitch or chord function.

The two major examples above deliberately differ. One maps conventional 12-TET major targets to the closest grid steps. The other maps the explicit ratio targets `1/1, 9/8, 5/4, 4/3, 3/2, 5/3, 15/8`. Neither is declared the unique or canonical major scale of 38-EDO. Store explicit generated steps and the generation strategy in any provenance report.

The corresponding required 53-EDO examples are `edo53_12tet_major_nearest` with steps `[0,9,18,22,31,40,49]`, and `edo53_ji_major_nearest` with steps `[0,9,17,22,31,39,48]`. These are separate target mappings, not interchangeable definitions of a canonical 53-EDO major scale. Include the following composition-owned definitions in the 53-EDO acceptance fixture:

```json
{
  "pitchSystems": {
    "tunings": {
      "edo53": {"kind":"equal", "divisions":53, "period":{"ratio":"2/1"}}
    },
    "scales": {
      "edo53_12tet_major_nearest": {"tuning":"edo53", "steps":[0,9,18,22,31,40,49]},
      "edo53_ji_major_nearest": {"tuning":"edo53", "steps":[0,9,17,22,31,39,48]}
    },
    "contexts": {
      "edo53_C4": {"tuning":"edo53", "scale":"edo53_ji_major_nearest", "anchor":{"note":"C4"}}
    },
    "defaultContext": "edo53_C4"
  }
}
```

This is a schema-4 composition fragment, not a complete playable composition. With this context, `tuned.step: 31` and `tuned.degree: 4` both resolve to `31/53 V`; degree -1 resolves to `-5/53 V`. A grid-native approximation of a just major triad uses root-relative tone steps `[0,17,31]`, with explicitly authored root, third, and fifth roles. Exact-ratio tones `1/1`, `5/4`, and `3/2` remain a distinct chord definition.

### 5.3 Pitch contexts and inheritance

A context contains a tuning ID, an optional matching scale ID, and an anchor. Exactly one anchor field is allowed:

- `note`: existing conventional scientific note, interpreted by the legacy parser.
- `pitchV`: finite absolute voltage.
- `frequencyHz`: positive finite nominal frequency, converted using the double C4 reference in §4.

The anchor is the pitch of lattice step zero. It is independent of `meta.root` and the frequency of any oscillator connected downstream.

For a `tuned` event, resolve context in this order: explicit `tuned.context`, pattern `pitchContext`, composition `pitchSystems.defaultContext`. Missing or unresolved context is an error. There is no implicit 38-EDO fallback. Legacy pitches do not consult this chain.

A native progression must specify its own `pitchContext`; it does not borrow a routed track's context. A tuned reference inside a harmonic nearest expression follows the event/pattern context chain. Chord roots and native chord intervals use the progression context. Keep these two sources separate when compiling cache keys.

Changing an inherited default intentionally changes native notes that inherit it. Reports must identify those dependencies. This is not a silent conversion of legacy notes. The UI must label it as the **default native pitch context**, not as a master retuning of every signal.

## 6. Native note encoding

Add `PitchType::TUNED` without replacing the other pitch kinds. A note must have exactly one of `pitchV`, `note`, legacy `degree`, `harmonic`, or `tuned`.

`tuned` contains exactly one pitch coordinate: `step`, `degree`, `cents`, or `ratio`. It may also contain `context` and integer `periods`, which defaults to zero.

```json
{"id":"n1","step":0,"tuned":{"step":12,"periods":-1},"gate":0.6}
```

The outer `step` is time. The inner `tuned.step` is pitch. For the example context, the voltage before other offsets is `-1 + 12/38`.

```json
{"id":"n2","step":1,"tuned":{"degree":2},"transposeCents":-2.5}
```

This selects degree two of the context's scale, then applies explicit detuning. It is not the second EDO step.

```json
{"id":"n3","step":2,"tuned":{"ratio":"5/4"}}
```

This outputs anchor plus `log2(5/4)`, not the nearest 38-EDO approximation. `tuned.cents` is likewise an exact interval from the anchor, not implicit grid quantization.

For all four modes, authored `periods` adds the context period. The contextual anchor and authored offsets are retained on save. Existing gate, velocity, probability, tie, glide, ratchet, condition, observation, and evolution fields remain available unchanged.

### 6.1 Grid identity

Compilation must retain whether a pitch has an explicit lattice identity. `tuned.step` and `tuned.degree` do. Ratio/cents pitches do not acquire one merely because floating-point comparison finds them close to a grid pitch.

For lattice pitches, retain the source lattice index independently of continuous detuning. A one-step edit should move the lattice index while preserving a -2.5-cent expressive detune. Do not quantize the detuned final voltage to rediscover its source step.

### 6.2 Bounds and final-domain validation

Native `step`, `degree`, and `periods` accept signed 32-bit integers. Use checked 64-bit intermediates and reject unrepresentable arithmetic. A syntactically legal large integer does not authorize a voltage outside the existing `[-10,+10] V` composition domain.

Validate all reachable resolved event pitches after event and assignment transformations. Do not hard-clip an out-of-range score into a different melody. Legacy checks retain their current behavior. New continuous values must be finite; NaN and infinity are never valid authored or compiled values.

## 7. Transposition and targeted editing

Keep the current `transpose_notes` operation and its revisioned transaction behavior. Existing `semitones` and `degrees` requests keep their meanings. Add an alternative `interval` object; exactly one of `semitones`, `degrees`, or `interval` is required.

```json
{
  "op": "transpose_notes",
  "pattern_id": "bass38",
  "selector": {"order": "stepDesc", "limit": 4},
  "expect_count": 4,
  "interval": {"steps": -1}
}
```

An `interval` contains exactly one of `steps`, `periods`, `cents`, or `ratio`:

| Request | Persistent representation / behavior |
|---|---|
| Existing `semitones` | Add to existing `transposeSemitones`; unchanged limits and semantics. |
| Existing `degrees` | Add to legacy `degree`, or to `tuned.degree`; other event types reject. |
| `interval.steps` | Add to new signed-integer `transposeSteps`; requires a native lattice identity in every reachable context. |
| `interval.periods` | Add to new signed-integer `transposePeriods`; use the pitch domain's repeat period. |
| `interval.cents` | Add to new double `transposeCents`. |
| `interval.ratio` | Convert to cents and add to `transposeCents`; the report retains the requested ratio and resolved cents. |

`transposeSteps`, `transposePeriods`, and `transposeCents` default to zero. Store them only when authored; preserve authored presence in partial edits. Bound the integer fields to signed 32-bit and `transposeCents` to ±24,000 cents; effective-pitch bounds still apply. Converting a ratio transposition into an accumulated cents offset is intentional; a `tuned.ratio` pitch itself remains symbolic.

### 7.1 Allowed pitch domains

Native grid notes support step shifts. Native harmonic tones support them when their selected chord tone has a lattice identity. Native nearest-tone expressions require every reachable selected result to have a compatible lattice identity. Off-grid ratio/cents harmonic tones reject step shifts rather than silently quantizing.

For consistency, step shifts of raw `pitchV`, named notes, legacy degrees, and legacy harmony reject with `unsupported_pitch_transform`. An agent must explicitly convert them to a native context first. Do not interpret a tuning-step request differently based on whether a legacy absolute pitch happens to lie near a grid point.

Period shifts are universal: legacy pitch domains use the octave; native events use their context period; native harmonic events use their selected progression's period. Cent and semitone shifts are universal absolute intervals.

### 7.2 Transform order

For the new native path:

1. Resolve the authored base and any harmonic tone selection, without event/assignment offsets.
2. Apply the sum of event and assignment `transposeSteps` to the stored native lattice coordinate.
3. Apply authored pitch `periods`, harmonic `periods`/legacy harmonic `octave` as applicable, and the sum of event/assignment `transposePeriods`.
4. Apply existing event semitones and event cents, then existing assignment semitones and assignment cents.
5. Validate the final voltage and retain it for runtime/inspection.

Equivalent period shifts can be combined internally, but no field is applied twice. Harmonic nearest selection happens before transposition, preserving the distinction between selecting the nearest tone and moving the selected result. The legacy path with no new fields preserves its old arithmetic order.

On an unequal table, moving one step is coordinate-dependent: it cannot be implemented by adding `period/N`. If a scene changes the step offset, precompute the resulting note pitches control-side or use an equivalent immutable pitch program with bounded lookup. Do not call an unequal-table shift a constant voltage offset.

### 7.3 Partial updates, copies, and selectors

Extend all note field allowlists, pitch exclusivity checks, unset rules, JSON views, and round-trip paths. Setting a new `tuned` object replaces the entire prior pitch representation atomically. Replacing it with another representation removes obsolete pitch-specific fields, but unrelated condition/expression fields and the stable note ID remain.

Offsets are preserved unless explicitly unset; if the new pitch kind cannot support retained step offsets, reject and name the field requiring removal. Never silently discard a meaningful offset.

Rotations move time only. Duplications preserve pitch definitions and offsets, and retain current ID/collision behavior. For cross-pattern duplication of native notes, materialize the **source effective context ID into `tuned.context`** by default, so copying into a differently tuned pattern does not unexpectedly retune the notes. Add explicit `pitch_context_policy: "destination"` to opt into destination inheritance. For harmonic notes whose nearest reference has a tuned coordinate, apply the same rule to that reference. Harmonic tone selection still follows destination harmony, as before; report that dependency.

P7 need not invent pitch-range selectors. Existing ID/time selections suffice. Newly exposed pitch-kind/context predicates, if added, must be structural and documented rather than undocumented comparisons of changing harmonic pitches.

## 8. Scene-specific microtonal overrides

Extend assignment overrides with `transposeSteps`, `transposePeriods`, and `transposeCents`. Keep them typed; do not round all new values into the current generic float override array. Use a separate pitch-offset structure or an equivalently safe typed representation. Existing velocity, gate, probability, and MOD behavior is unchanged.

```json
{
  "pattern": "bass38",
  "phaseMode": "continue",
  "overrides": {
    "transposePeriods": 1,
    "transposeSteps": 1,
    "transposeCents": -1.5,
    "velocityScale": 1.1
  }
}
```

Validate native-step eligibility against every affected event and reachable chord, even if probability is zero or a condition often suppresses the event. Unsupported raw/legacy pitches in a mixed pattern cause an actionable validation error; never ignore an override for selected notes without saying so.

A pattern routed into two scenes may resolve to two compiled offset programs, but its authored notes are not copied. Do not introduce a new tuning inheritance hierarchy on scene assignments in P7. To compose an intentional tuning change, use explicitly bound native patterns or progressions; targeted duplication and context edits already make this manageable.

## 9. Explicit conversion and quantization

Add `retune_notes` for static notes only. It uses existing selectors, `expect_count`, bounded reports, and transaction rules.

```json
{
  "op": "retune_notes",
  "pattern_id": "melody",
  "selector": {"all": true},
  "target_context": "edo38_C4",
  "mode": "nearest",
  "target": "tuning",
  "tie_break": "lower",
  "max_error_cents": 20.0
}
```

`target` is `tuning` (all positions) or `scale` (the selected subset). `mode` is:

- **`nearest`:** resolve each source pitch including event offsets, choose the closest target position in cents, and store `tuned.step` or `tuned.degree` with an explicit target context. Clear old pitch offsets that were folded into the source. This intentionally changes the pitch.
- **`preserve`:** choose a target position but store the exact residual in `transposeCents`, preserving the source voltage to the new-path tolerance. This changes representation, not the intended pitch. Report residuals; it is not an on-grid result unless residual is zero.
- **`reinterpret`:** only for native step/degree events. Keep their coordinate, authored periods, and offsets while replacing the context reference. `target` must match the coordinate kind (`tuning` for step, `scale` for degree); no implicit conversion between them. Resulting pitch changes are reported.

For nearest/preserve, quantize in cents/log-frequency distance, not linear-Hz distance. Enumerate neighboring repeat periods correctly at the seam. A tie defaults to the lower absolute pitch; the caller may explicitly request higher. `max_error_cents`, when supplied, measures source-to-selected-grid distance, including in preserve mode; crossing it aborts the transaction.

Do not resolve a harmonic event to one arbitrary chord and call it retuned. Harmonic events reject here with `dynamic_pitch_requires_harmony_edit`. Author/retune the progression explicitly, or materialize it using the voicing helper first.

Scene offsets are not baked into a shared pattern by `retune_notes`. Reports state that source pitches are event-level, before scene overrides. This prevents one scene's octave shift from being applied twice later.

## 10. Native harmony

Keep legacy chord objects `{root: "C3", intervals: [0,4,7]}` exactly as they are. They remain octave-periodic semitone chords, regardless of `pitchSystems.defaultContext`.

Add a native progression form with an explicit `pitchContext`. Within one progression, require either all legacy chords or all native chords in P7. A native chord has `rootPitch` and `tones`, and must not also have legacy `root`/`intervals`.

```json
{
  "pitchContext": "edo38_C4",
  "lengthBeats": 8,
  "chords": [
    {
      "id": "tonic",
      "beat": 0,
      "rootPitch": {"tuned": {"step": 0, "periods": -1}},
      "tones": [
        {"id": "r", "interval": {"steps": 0}, "roles": ["root"]},
        {"id": "t", "interval": {"steps": 12}, "roles": ["third"]},
        {"id": "f", "interval": {"steps": 22}, "roles": ["fifth"]}
      ]
    },
    {
      "id": "fifth_degree",
      "beat": 4,
      "rootPitch": {"tuned": {"step": 22, "periods": -1}},
      "tones": [
        {"id": "r", "interval": {"steps": 0}, "roles": ["root"]},
        {"id": "t", "interval": {"steps": 12}, "roles": ["third"]},
        {"id": "f", "interval": {"steps": 22}, "roles": ["fifth"]}
      ]
    }
  ]
}
```

### 10.1 Root and tone grammar

`rootPitch` contains exactly one static `tuned`, `note`, or `pitchV`; do not admit harmonic recursion or legacy degree roots in this new grammar. A tuned root inherits the progression's required context; an explicit context, if supplied, must match it. A named/raw root is absolute and has no native lattice identity.

Each tone has a stable ID, exactly one interval, and zero or more explicit role IDs. Intervals contain exactly one of `steps`, `cents`, or `ratio`. Step intervals require a lattice root and advance from that root's coordinate; for unequal tables compute `L(rootIndex + n) - L(rootIndex)`, not the distance from global step zero.

Tones describe one period of chord classes. Require 1–8 tones, ordered by resolved root-relative interval, with the first at zero and each interval in `[0, contextPeriod)`. No duplicate classes within 1e-7 cents. If two functions share a pitch, attach two role labels to one tone instead of defining duplicate tones. Compound registers belong in harmonic event period/octave fields or voicing choices, not duplicate chord classes.

Roles are explicit, case-sensitive IDs. The zero tone must carry `root`; no other tone may carry it. Each role is associated with at most one tone per chord. Custom roles such as `neutralThird` or `color` are valid. The labels do not imply a fixed number of cents. Do not infer `third` by checking whether an interval modulo twelve is three or four.

Ratio/cents tones remain off-grid unless explicitly approximated by an authoring helper. A progression can therefore express exact just ratios while using a context to define its anchor and repeat period. This is a deliberate hybrid, not a claim that every note lies on its EDO grid.

### 10.2 Harmonic event selection

Extend `harmonic.kind: "tone"` to support exactly one of `index`, `role`, or `toneId`. Existing role names remain valid for legacy chords. New role names and tone IDs require matching native chords in all reachable contexts.

A tone expression can specify `periods` or existing `octave`, not both. `periods` follows the chord domain; `octave` always means 2/1. The default is no register shift.

For `nearest`, accept the new `tuned` static reference and range endpoints in addition to existing forms. Existing string endpoints remain conventional absolute notes; object endpoints use exactly one static pitch representation. Never round a new reference or range endpoint to an integer semitone.

For a tone with base voltage `B` and repeat interval `P`, bounded register `[lo,hi]` yields candidates:

```text
B + j*P, for ceil((lo-B)/P) <= j <= floor((hi-B)/P)
```

Generate only these candidates. Choose minimum absolute cent distance to the reference, with explicit lower/higher tie-breaking and stable tone-ID tie resolution. Endpoint tolerance must be consistent with the numerical policy; do not include notes genuinely outside the requested register.

### 10.3 Compile keys and dependencies

Current authored-expression interning is insufficient if identical tuned JSON inherits different contexts. Extend interning/compiled keys with the resolved reference context, tuning/scale/anchor signatures, chord definition, period, explicit role/tone mapping, and all pitch-affecting offsets needed for that entry.

Two chords with identical pitch values but different role labels are not equivalent for role-based expressions. Two events with identical authored `{"tuned":{"degree":2}}` but different pattern contexts are not equivalent. Never cache by the human-readable tuning name or authored JSON string alone.

Continue compiling only reachable harmonic combinations, not the Cartesian product of every tuning, event, scene, and chord. Missing bindings or roles remain structured validation errors. Unbound harmonic patterns retain the existing warning behavior, with eligibility/range checks performed once assigned.

## 11. Microtonal voicing

Extend `voice_progression`; keep the current control-side, bounded, deterministic design. This is the largest integration change after the codec. Do not reuse integer-semitone pitch-class masks for new harmony. [S8]

### 11.1 Candidate identity and coverage

A candidate stores precise voltage plus chord-tone identity and register-period displacement. Track root/third/other coverage using tone indices or IDs. With at most eight tones, an eight-bit tone-coverage mask is enough regardless of whether the tuning has 7, 38, or 1024 positions.

Enumerate each chord tone's period repetitions that fit each voice's register. Do not scan every semitone or every tuning step between the bounds. Fine EDOs therefore do not multiply candidate counts merely because N is larger when the chord still has three tones.

Keep current coverage priorities: require root; require the unique explicit third when at least two voices exist; optimize remaining tone coverage. A chord with no third role does not invent one. All actual-pitch no-crossing constraints use precise values, not rounded display labels.

### 11.2 Cost units and overflow

The old solver's semitone integer distances and hard-coded leap ceiling are inappropriate for arbitrary tunings. Preserve the legacy solver path for legacy requests. For native requests, score distance in **integer milli-cents** (0.001 cent), with precise voltages retained separately:

```text
scoreTick = nearest_integer(1200 * pitchV * 1000, ties toward lower pitch)
```

This is a deterministic optimization resolution, not output-pitch quantization. Distinct precise candidates that share a scoring tick remain distinct, with a stable structural tie-break. Use the existing lexicographic objectives: missing coverage, maximum leap, total movement, squared movement, initial register displacement, then stable full-path ordering.

Use signed 64-bit checked accumulators. Across the existing ±10 V pitch domain, a leap is at most 24,000,000 milli-cent ticks. With at most eight voices and 128 chords, the worst squared-movement sum is `8 * 127 * 24,000,000^2 = 585,216,000,000,000,000`, below signed 64-bit maximum. Do not increase to micro-cent integer scoring without changing this overflow analysis.

Derive the leap search bounds from candidate extrema; do not retain the fixed 240-semitone ceiling. All search passes share the transaction budget. Preserve existing bounds unless separately benchmarked: 200,000 partial visits, 2,048 complete candidates per layer, and 2,000,000 transitions. Exhaustion returns `capacity_exceeded`; it never silently returns a truncated optimum. [S8–S9]

### 11.3 Materialization

Default native voicing output is **fixed absolute `pitchV` notes**, preserving the existing materialization promise: later progression/tuning changes do not rewrite an already materialized voicing. Never serialize a microtone into a rounded `C#4`-style note name.

A later live-harmonic materialization mode is outside P7. Provenance is returned in the operation report: source context/tuning, progression/chord/tone IDs, repeat displacement, nominal cents/volts, scoring resolution, and a new algorithm version such as `voice_progression_micro_v1`. Report native leap/movement metrics with units in their names. Retain existing legacy report fields for legacy requests.

All existing destination track, collision, grid, stable-ID, shared-reference, preview, and rollback rules still apply. Microtonality does not change timing-grid validation.

## 12. Presets and authoring assistance

Support every integer division count from 1 through 1024. The catalog is a discovery aid, not an allowlist. Ship ordinary equal-definition presets for at least:

```text
5, 7, 12, 17, 19, 22, 24, 31, 38, 41, 48, 53, 72 EDO
13 equal divisions of 3/1
```

Adding another EDO should require at most one catalog entry and its tests, not changes to the resolver. Include the explicit seven-position just-ratio table from §5 as an unequal-table demonstration. Do not describe all historical or cultural tuning traditions as fixed EDOs or claim that one generated subset represents a whole tradition.

Provide a read-only mapping helper accepting explicit target cents or ratios and a target tuning. Return selected steps, exact target values, realized cents, signed errors, and collisions. For an equal period `R`, the ideal step for ratio `r` is `N * log2(r)/log2(R)`; choose a nearest integer using the specified tie rule.

Scale-generation collisions are errors by default. Do not silently merge two intended scale degrees that map to the same lattice step. Offer returned diagnostics rather than silently repairing the musical intent. Explicit table/step definitions are always available to the agent.

The helper may return suggested `upsert_scale` or `upsert_progression` operations, but it does not execute them. A preset generator should insert only used definitions into the composition, not serialize the complete factory catalog into every patch.

## 13. Scala `.scl` interoperability

Implement bounded control-side import of `.scl` text and normalized export. The official format distinguishes decimal cents from integer/ratio tokens, has implicit unison, and uses a note-count line. Keyboard mapping is a separate format. [S16]

`import_tuning_scl` is an edit operation with `id`, `text`, and optional metadata name. It produces a table definition; absolute anchoring remains a separately authored context. Never open arbitrary filesystem paths from an agent-provided request.

The parser must handle comments beginning with `!`, LF/CRLF, whitespace, an empty description line, decimal cents, slash ratios, bare integer ratios, and trailing pitch-line annotations. Import Latin-1 text via an explicit conversion if a file picker supplies bytes; semantic JSON input is UTF-8 text. Ratios must support the size in §5.

For Sibyl's periodic-table interpretation, the final pitch entry is the positive repeat period. Insert implicit unison, retain all earlier entries as internal positions, and exclude the terminal period from those positions. Thus an N-entry `.scl` with the endpoint becomes N internal positions, not N+1. A singleton period creates a one-position tuning.

Distinguish malformed Scala syntax from a syntactically valid scale outside the P7 periodic-table contract. The official format permits cases such as negative cents or zero stated notes; P7 can reject non-increasing, duplicate, nonpositive-period, or unsupported-size scales with `unsupported_tuning_shape`, without mislabeling them invalid Scala files. Do not sort or fold unusual files silently.

Exports contain the description, correct count, internal positions after unison, and the terminal period. Preserve ratios where authored; use enough decimal precision to retain the numerical tolerance for cents/equal-division exports. An equal grid exported as decimal Scala data may reimport as a table: audible equivalence is required, identical representation is not.

Do not claim `.kbm` or MIDI reference-note support. The explicit pitch context already supplies anchor and scale-degree semantics for Sibyl's CV-oriented authoring.

## 14. Codec migration and versioning

Accept input schemas 2, 3, and 4 after this phase. Keep the current validation of envelope/composition version agreement. New pitch-system fields require schema 4; a schema-3 request containing them must return `schema_version_required`, not ignore them as unknown decoration.

Normalize accepted compositions to an internal schema-4-capable model. The ordinary full writer emits schema 4. Pure legacy content still uses the legacy runtime path. Do not retroactively convert every old degree into `tuned.degree` or inject a non-default context.

For compatibility export to schema 3, allow it only when the document contains no schema-4-only fields or when the caller explicitly requests and accepts a static materialization procedure. P7 need not implement such lossy export; rejecting unsupported downgrade is preferable to discarding tuning data.

Audit every serialization path, not only the full composition: patch persistence, full/pattern/note/progression views, preview candidates, undo/redo snapshots, edit round-trips, and any generated fixture/preset paths. Preserve authored ratios, IDs, omitted-vs-explicit override fields, contexts, labels, and import metadata.

Metadata-only edits can change document revision without changing any sounding pitch. Stable audible signatures must distinguish data identity from effective pitch dependencies. New feature support must be discoverable through the existing capabilities/schema query, including limits and exact units.

## 15. Semantic operations and queries

Extend existing Sibyl semantic dispatch; do not add an independent microtonal server or parallel undo mechanism.

| Operation | Required payload / semantics |
|---|---|
| `upsert_tuning` | `id`, `tuning`; whole definition replacement, final dependency validation. |
| `delete_tuning` | `id`; reject live references unless removed in the same final transaction. |
| `upsert_pitch_scale` / `delete_pitch_scale` | `id`, `scale` for upsert; tuning-linked ordered subset. |
| `upsert_pitch_context` / `delete_pitch_context` | `id`, `context` for upsert; dependency-safe. |
| `set_default_pitch_context` | `context_id`: ID or null; null removes default inheritance. |
| `set_pattern_pitch_context` | `pattern_id`, `context_id`: ID or null. |
| `import_tuning_scl` | `id`, bounded `text`, optional `name`; explicit imported data. |
| `transpose_notes` | Existing forms plus typed `interval`, as §7. |
| `retune_notes` | Static selection, target context and explicit mode, as §9. |
| Existing note/scene/progression edits | Extend strict allowlists and preserve new fields. |
| `voice_progression` | Native progression and precise static register endpoints. |

New definition edits must validate the final candidate graph so a single transaction can introduce related definitions or replace/delete their references atomically. Read-dependent operations such as conversion/voicing evaluate the working state at their position in the operation sequence; prerequisites must therefore precede them. Later failure rolls back the entire candidate.

Use existing QUERY routing for proposed `tuning_catalog`, `pitch_systems`, `pitch_context`, and `map_intervals` views. Extend `notes`, `progression`, and `effective_context` views with optional native pitch details. A `pitch_context` request identifies `id`; a `map_intervals` request identifies `context_id`, an `intervals` array of cents/ratio objects, and an optional `tie_break`.

Expose context/tuning/scale IDs, unit, period, native step and degree where meaningful, authored pitch, selected voltage, final voltage, nominal Hz, nearest conventional note plus signed cent offset, and quantization residual where requested. For native chords expose tone IDs and role mappings; do not populate a misleading twelve-entry `pitchClasses` array. Legacy derived fields remain valid only for legacy chords.

Paged or potentially large responses are opt-in and bounded. Preserve revision-bound cursors. A preview of edits runs through the existing VALIDATE path, consumes no accepted revision or note IDs, produces no adoption request, and creates no undo entry. Accepted EDIT is still the single commit/undo point.

## 16. Compilation and real-time architecture

Introduce small shared, dependency-light pitch math/types components. A reasonable division is:

```text
SibylTuningTypes.hpp       authored tuning/scale/context, interval tags
SibylTuningMath.hpp        checked floor arithmetic, ratios, lookups, nearest math
SibylTuningJSON.hpp/.cpp   codec, normalization, references, Scala adapters
SibylPitchCompiler.hpp/.cpp contextual resolution and immutable pitch programs
SibylTuningEdit.hpp/.cpp   definition operations and retune helpers
SibylTuningView.hpp        bounded introspection and mapping reports
```

Use the repository's preferred header/source conventions rather than forcing a mechanical file split. Do not introduce Rack dependencies into pure math tests. Do not reuse Cantor's stateful/adaptive decision engine as Sibyl's deterministic authoring resolver; sharing stateless interval utilities is a separate, test-backed refactor. Cantor's exposed engine includes note-on voice state and contextual selection, which is a different responsibility. [S17]

A conceptual compiled pitch result should retain:

```cpp
struct ResolvedPitch {
    double volts;
    double periodVolts;
    int contextIndex;       // immutable compiled context, or legacy sentinel
    bool hasLatticeIndex;
    int64_t latticeIndex;   // meaningful only when hasLatticeIndex
    double residualCents;  // expressive offset separate from grid identity
};
```

This is a specification sketch, not drop-in code. Separate authored objects from compiled objects; do not serialize derived indices or pointer addresses.

Compile ratios, periods, scale lookups, role matching, nearest-tone candidates, and unequal-grid scene shifts off the audio thread. The runtime should select an immutable precomputed voltage/program at the existing note-on boundary and feed it into existing glide/output behavior.

Avoid dense scene-by-event-by-tuning allocation. Intern immutable contexts and offset programs; share identical reachable combinations. A static event can still use `compiledPitchV` when one value suffices. A new contextual program must distinguish event/assignment offsets from the existing `contextualPitch()`/override paths so transposition is never applied twice.

Prohibited in `process()`: JSON, strings/maps built on demand, allocation, blocking locks, file access, ratio parsing, `log2`, candidate enumeration, and re-solving harmony or voicing. Event-time bounded array lookup is acceptable. Do not move work into a per-sample callback merely because the math is individually inexpensive.

Retain the existing harmonic pitch-entry and memory safeguards; expand accounting to include tuning tables, contexts, per-route programs, authored metadata retained in compiled snapshots, and their vector capacities. Default P7 compiled pitch/harmony/automation budget is 32 MiB, with at most 1,048,576 contextual pitch entries. Refuse a candidate before committing it when it cannot fit. Control-side solver scratch memory also needs a bounded preflight, independent of the snapshot budget.

## 17. Adoption, sounding notes, and timing

Build control-side dependency signatures from each pitch consumer to its tuning, scale, anchor, chord tone/role definitions, and pitch offsets. An unused tuning rename must not restart every channel. A changed scale used only by `tuned.degree` should not change a `tuned.step` event that uses the same tuning and anchor.

Differentiate document-only changes from audible pitch dependencies. If a relevant new definition changes selected/future note behavior, mark only affected channels for the existing adoption policy. Even where current numeric pitches coincide, changes to harmonic role/selection dependencies must not leave stale compiled programs.

For P7, retain the existing safe adoption behavior: at the accepted quantized adoption boundary, affected channels close gates/cancel glides according to existing channel adoption rules; phase follows `preserve`, `restartChanged`, or `restartAll`. [S11] Unaffected channels and automation lanes continue. Do not introduce live sustained-note retuning or retrigger every note solely to demonstrate the new tuning.

An ordinary chord/scene transition changes new notes at their normal onset. A held/tied note follows the current scheduling semantics; P7 adds no independent asynchronous retune. Test this in the actual module rather than assuming how ties interact with chord changes.

Tuning selection and harmonic lookup use the same effective onset/negative-microshift context as the existing harmony implementation. Conditions and failed probability tests do not advance or rewind pitch contexts. Scalar tuning definitions are not automation lanes. MOD1–3 curves continue independently of note activity and remain unrelated to a tuning-definition edit unless their own definitions changed.

## 18. Limits, numerical rules, and failure handling

Initial explicit authoring limits:

| Resource | Limit |
|---|---:|
| Equal divisions / positions per tuning | 1–1024 |
| Tuning definitions | 128 |
| Scale definitions / pitch contexts | 256 each |
| Total stored table positions | 65,536 |
| Scale positions | 1–1024, no more than its tuning |
| Period size | 1–4800 cents, finite and strictly positive |
| New native chord tones | 1–8 |
| Native ratio numerator/denominator | 1–2,147,483,647 |
| Source `.scl` text | 256 KiB after decoding |
| Definition ID / role ID / tone ID | Existing 64-byte ID rules |
| Definition name / description | 256 / 2048 UTF-8 bytes |
| Final compiled pitch | Existing -10 to +10 V |

The period lower bound is an engineering capacity choice, not a musical definition. A definition can be valid while an extremely dense requested voicing exceeds candidate limits; report the actual capacity failure.

Use double arithmetic for new paths with test tolerance `1e-7 cents` for mathematical identities at modest indices. Final float voltage tests use an absolute tolerance of `1e-6 V` over the bounded output domain. Where compound arithmetic requires a slightly larger justified tolerance, document it explicitly rather than hiding mistakes with an oversized epsilon. Canonical JSON must retain enough significant digits for double round-tripping.

Validate table/scale ordering before allocation-heavy work. Normalize ratios with exact integer GCD; do not decide authored equality by display rounding. Audible metadata signatures must be stable across key order and ratio reduction. Detect multiplication/addition overflow before computing it. Do not calculate invalid log arguments and hope to catch NaN afterward.

Add structured codes including `invalid_tuning`, `invalid_pitch_scale`, `unresolved_pitch_context`, `unsupported_pitch_transform`, `unsupported_tuning_shape`, `quantization_error_exceeded`, `scale_mapping_collision`, and `dynamic_pitch_requires_harmony_edit`; retain existing shared codes for range, schema, object-in-use, revision, and capacity errors. Every error includes a precise JSON path, relevant IDs, and expected unit where useful. Failure leaves accepted state, revision, note IDs, pending adoption, and undo history unchanged.

## 19. File-level implementation checklist

| File / area | Required work |
|---|---|
| `SibylTypes.hpp` | New authored pitch kind, definitions/references, typed offsets, compiled dependency/program handles. |
| `SibylJSON.cpp/.hpp` | Schema 4, strict feature gates, pitch exclusivity, normalization, full/pattern serialization, legacy path retention. |
| `SibylNoteEdit.cpp/.hpp` | New fields, typed transpose, conversion, cross-context copy safety, note views. |
| `SibylOverrides.hpp`, assignment edit helper | Typed microtonal offsets; partial edits and presence preservation. |
| `SibylHarmonyTypes.hpp` | Native roots/tones/roles, precise references, domain/period information. |
| `SibylHarmonyJSON.hpp` | New grammar and compile keys; no integer-semitone coercion in native resolution. |
| `SibylHarmony.hpp` | Bounded lookup integration; eliminate double application of offsets. |
| `SibylHarmonyView.hpp` | Accurate native context/role/cent reporting; units and schema version. |
| `SibylVoicing.hpp` | Native candidates, tone coverage, cent-based scoring, overflow/budget accounting. |
| `SibylVoicingEdit.hpp` | Native register grammar, context dependencies in isolated compilation, fixed microtonal materialization. |
| `SibylAdoption.cpp/.hpp` | Dependency-aware change detection; preserve nonpitch lanes and legacy actions. |
| `SibylEdit.cpp`, `Sibyl.cpp`, control/query dispatch | Inspect current local entry points; integrate operations, preview, capabilities, persistence, and runtime programs. |
| Tests / `Makefile` | New pure tuning/codec tests and expanded real-module, oracle, contract, and benchmark coverage. |
| Agent skill/docs / panel pitch display | Teach units, explicit contexts, ratios versus quantization, native labels, and revised capability limits. |

Do not declare completion after only making `tuned.step` play the right voltage. Codec, query, copy/edit, harmony, voicing, and adoption are equally part of the contract.

## 20. Acceptance tests

Each case is a requirement to implement in the repository, not a claim that the current branch already passes it. Existing P1–P6 tests remain required. The supplied Python reference checks validate only mathematical examples and fixture consistency, not the C++ codec or module.

### Arithmetic and representations

| ID | Required result |
|---|---|
| M01 | Every N from 1–1024 satisfies `L(k+N)-L(k)=1 V` for representative signed indices in an octave system. |
| M02 | 38-EDO steps 0, 1, 12, 19, 22, 38 resolve to 0, 1/38, 12/38, 1/2, 22/38, 1 V at C4. |
| M03 | Negative index -1 resolves one lattice step below anchor; no negative array indexing. |
| M04 | Native degree wrap and negative degree wrap use scale cardinality, not N. |
| M05 | Thirteen divisions of 3/1 repeat after `log2(3)` V, not 1 V. |
| M06 | Exact `5/4` differs from the nearest 38-EDO approximation; no hidden quantization. |
| M07 | Unequal-table one-step shifts vary with starting index. |
| M08 | Context anchors from C4, 0 V, and nominal C4 Hz agree within tolerance. |
| M09 | A frequency anchor of 432 Hz changes only native consumers of that context; legacy A4 remains its old pitch. |
| M10 | Native cents and ratio pitches with negative periods resolve correctly. |
| M11 | Grid detuning survives native-step transposition without snapping. |
| M12 | Float-boundary accuracy remains within the declared tolerance over ±10 V. |
| M13 | 53-EDO steps -54, -53, -1, 0, 1, 17, 31, 52, 53, 54 resolve to their signed index divided by 53 V at C4. The 31-step fifth has the signed 3/2 error specified in §4.1; an exact 3/2 pitch remains distinct. |
| M14 | Mapping the two target sets in §5.2 into 53-EDO reproduces both stated scale arrays. JI-major degrees -1, 4, 7 resolve to -5/53, 31/53, 1 V respectively. |

### Codec and migration

| ID | Required result |
|---|---|
| C01 | Existing schema-2/3 golden fixtures retain their legacy voltage/timing behavior. |
| C02 | Schema-4 definitions/events round-trip without losing context, ratio, roles, offsets, or metadata. |
| C03 | Schema-3 documents using new fields fail with `schema_version_required`. |
| C04 | Older schema-3-only builds reject schema 4 rather than playing silently mistuned data. |
| C05 | Full, pattern, note, progression, patch persistence, and undo paths preserve native data. |
| C06 | Conflicting pitch representations, nonfinite values, missing contexts, and mismatched scale/tuning IDs reject. |
| C07 | Unused definitions survive round-trip but do not produce audible adoption changes. |
| C08 | Ratio reduction and object key order do not alter audible results. |
| C09 | Duplicate/unsorted positions and an included terminal endpoint reject with precise paths. |
| C10 | Out-of-domain final pitch after all offsets rejects, including an otherwise valid authored coordinate. |

### Editing and scene behavior

| ID | Required result |
|---|---|
| E01 | One semitone is 100 cents in a 38-EDO composition; one native step is 1200/38 cents. |
| E02 | “Last four notes down one step” changes exactly the selected IDs and no time/expression fields. |
| E03 | Degree transposition preserves `tuned.degree`; it does not become a grid-step edit. |
| E04 | Step shifts of unsupported raw/legacy/off-grid harmonic notes reject atomically. |
| E05 | Native period transposition follows 3/1 in a tritave context; octave fields remain 2/1. |
| E06 | Event and scene step/period/cent offsets apply exactly once and in documented order. |
| E07 | Mixed-pattern invalid scene shifts reject rather than skipping unsupported notes. |
| E08 | Partial velocity/condition edits do not remove tuning data. |
| E09 | Rotation changes temporal steps only; duplication preserves pitch fields and fresh IDs. |
| E10 | Cross-pattern duplication pins source context by default; explicit destination policy retunes intentionally. |
| E11 | `retune_notes.nearest` reports signed errors and obeys max error. |
| E12 | `retune_notes.preserve` retains absolute pitch with an explicit residual. |
| E13 | `reinterpret` preserves coordinates/offsets and reports pitch change; incompatible kinds reject. |
| E14 | Harmonic conversion cannot silently freeze an arbitrary chord. |
| E15 | Scene offsets are not baked into static conversion. |
| E16 | Later-operation failure restores all definitions, notes, revisions, and IDs. |
| E17 | In 53-EDO, a native step is 1200/53 cents, a semitone remains 100 cents, and a period is 1 V. Exercise targeted edits, scene offsets, cross-context copying, all retune modes, and rollback using the 53-EDO fixture. |

### Harmony and voicing

| ID | Required result |
|---|---|
| H01 | Native 38-EDO root/third/fifth select 0/12/22 steps, not 0/4/7 semitones. |
| H02 | Explicit role changes invalidate relevant compile keys even with identical tone voltages. |
| H03 | Identical inherited tuned-reference JSON in different pattern contexts resolves independently. |
| H04 | Unequal root-relative step chords use the root coordinate, not a zero-root interval table. |
| H05 | Exact ratio chord tones remain exact and can coexist with a native grid context. |
| H06 | Native nearest selection uses precise cents and handles negative registers and period seams. |
| H07 | Missing/ambiguous roles and mixed legacy/native progression forms reject. |
| H08 | Native tone periods and legacy octaves differ correctly in a non-octave progression. |
| H09 | Candidate generation uses chord-tone repetitions, not a 12-class mask or N-step scan. |
| H10 | Native voice-leading cost uses milli-cents while retaining unquantized output voltages. |
| H11 | Native squared-cost stress cases remain inside checked 64-bit bounds. |
| H12 | Native solver matches an independent exhaustive oracle on small randomized cases. |
| H13 | Existing legacy oracle fixtures retain their existing solution and report behavior. |
| H14 | Budgets are cumulative across all solver passes and operations; no partial optimum on exhaustion. |
| H15 | Materialized native voicings remain fixed after changing source tuning/progression. |
| H16 | Native voicing cannot round microtones into conventional note strings. |
| H17 | Native isolated progression compilation includes its referenced tuning/context/scale closure. |
| H18 | Effective-context output agrees with actual final note-on voltage before oscillator offsets. |
| H19 | Native 53-EDO root/third/fifth select 0/17/31 steps. Precise nearest selection and the independent exhaustive voicing oracle cover this progression across negative registers and octave seams; materialized output preserves microtonal voltages. |

### Scala, runtime, and integration

| ID | Required result |
|---|---|
| I01 | Scala 38-entry octave import produces 38 positions with implicit unison and no duplicate endpoint. |
| I02 | Scala ratio/integer/cents tokens, comments, annotations, empty description, and CRLF parse correctly. |
| I03 | Non-octave Scala period is retained; no forced 1200-cent normalization. |
| I04 | Malformed Scala and valid-but-unsupported shape have distinct errors. |
| I05 | Import size/ratio/position limits reject before unbounded allocation. |
| I06 | Export/reimport preserves audible tuning within tolerance without depending on original file paths. |
| I07 | Unrelated metadata or unused-tuning edits leave gates, glides, phase, and automation intact. |
| I08 | Relevant tuning edits adopt only affected channels at the requested boundary. |
| I09 | Negative microshift and scene-repeat/chord boundaries use existing onset semantics. |
| I10 | Probability failures/conditions do not affect tuning lookup state or sweep automation. |
| I11 | No allocations, locks, parsing, logarithms, or candidate solving occur in the audio thread. |
| I12 | Snapshots fit the combined budget; failures leave accepted and pending state intact. |
| I13 | Repeated native insert/edit/save/load does not accumulate pitch drift. |
| I14 | Bounded capabilities/query/preview/undo contract tests include all new fields and units. |
| I15 | A nominal oscillator/measurement patch confirms both 38-EDO and 53-EDO intervals without a downstream 12-TET quantizer. For 53-EDO, verify one step, the 31-step fifth, and the 53-step octave against compiled/output voltages; report oscillator measurements separately from ideal tuning error. |
| I16 | Benchmarks compare matched legacy and native workloads; report compiler/configuration and failures honestly. |
| I17 | Preset discovery exposes 53-EDO; generated definitions are composition-owned. Scala export/reimport yields 53 positions with implicit unison and the octave endpoint excluded. The 53-EDO fixture survives codec/patch round-trips, preview, undo, and targeted adoption without pitch drift. |

## 21. Implementation milestones and Codex handoff

**P7A — Core pitch model and codec.** Add generic equal divisions, general periods, unequal tables, contexts, scales, tagged notes, static pitch resolution, schema migration, and pure math tests. Native 38-EDO and 53-EDO sequences must already play without going through harmony. Keep the legacy path untouched.

**P7B — Safe composing operations.** Add native targeted transposition, static conversion/quantization, typed scene offsets, copy-context safety, definition edits, preview, and query/capability support. Validate unequal-step semantics and adoption dependencies.

**P7C — Harmony and voicing.** Add explicit native chord roots/tones/roles, precise nearest queries, context-sensitive compile keys, candidate identity, cent-based bounded optimization, and fixed microtonal materialization. Extend the exhaustive oracle and real-module integration tests.

**P7D — Interchange and hardening.** Add preset discovery, mapping helper, Scala import/export, agent documentation/UI labels, persistence/undo tests, capacity accounting, and matched runtime benchmarks. All existing expressive-composition suites remain required.

The submilestones are implementation order, not excuses to call partially integrated microtonality complete. Each should land with tests. Defer no compatibility fix merely because an example produces the right pitch.

Carry both acceptance tunings through P7B–P7D: 53-EDO is required for composing operations, harmony/voicing, interchange, and live integration, not merely a catalog entry. Retain the 38-EDO fixtures and the generic 1–1024 division contract; adding 53-EDO does not narrow either requirement.

### Initial Codex instruction

```text
Implement Sibyl P7 from this specification on the current lumin-render checkout.
First record the commit and inspect the implemented P1–P6 code and test contracts.
Start with P7A; preserve legacy 12-TET arithmetic and scheduling behavior.
Use one generic tuning model, not an enum branch for every EDO.
Keep note timing step separate from tuned pitch step; never redefine semitones.
Implement checked negative-index math, explicit context resolution, and schema-4
feature gates before integrating new pitches into playback. Add and run focused
math/codec tests, then the existing legacy golden tests. Explain any discrepancy
between the local tree and this specification before choosing a compatible fix.
Do not claim P7 complete until edits, harmony, voicing, queries, persistence,
adoption, Scala interchange, and acceptance coverage have been addressed.
```

## 22. Sources and validation provenance

Repository references below were inspected on the mutable `lumin-render` branch during this response. They identify integration points; they do not certify a build. All new schemas, limits beyond existing contracts, algorithms, and requirements above are design proposals.

- **[S1]** `SibylTypes.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/lumin-render/src/SibylTypes.hpp
- **[S2]** `SibylJSON.cpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/lumin-render/src/SibylJSON.cpp
- **[S3]** `SibylHarmonyTypes.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/lumin-render/src/SibylHarmonyTypes.hpp
- **[S4]** `SibylHarmonyJSON.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylHarmonyJSON.hpp
- **[S5]** `SibylHarmony.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylHarmony.hpp
- **[S6]** `SibylNoteEdit.cpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylNoteEdit.cpp
- **[S7]** `SibylOverrides.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/lumin-render/src/SibylOverrides.hpp
- **[S8]** `SibylVoicing.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylVoicing.hpp
- **[S9]** `SibylVoicingEdit.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylVoicingEdit.hpp
- **[S10]** `SibylHarmonyView.hpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/SibylHarmonyView.hpp
- **[S11]** `SibylAdoption.cpp`: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/refs/heads/lumin-render/src/SibylAdoption.cpp
- **[S12]** Harmony cases: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/tests/sibyl_harmony_cases.hpp
- **[S13]** Voicing cases: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/tests/sibyl_voicing_cases.hpp
- **[S14]** Test integration: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/Makefile
- **[S15]** VCV Rack voltage standard: https://vcvrack.com/manual/VoltageStandards
- **[S16]** Official Scala scale format: https://www.huygens-fokker.org/scala/scl_format.html
- **[S17]** Cantor engine interface: https://raw.githubusercontent.com/PlasmaChroma/Leviathan-Rack2/lumin-render/src/CantorCultureEngine.hpp

Companion artifacts contain proposed schema-4 examples, computed tuning vectors, and an independent Python reference check. Those checks do not execute Sibyl, validate the C++ parser, prove real-time safety, or replace the acceptance suite above. No code changes or repository writes were performed as part of producing this specification.
