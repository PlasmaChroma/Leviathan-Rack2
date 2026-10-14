# Native Codex handoff: Bifurx oversampling listening test

## Request

Use the Octavia skill and live Rack tools to set up and run a controlled Bifurx
listening comparison for me. Build a small reversible test harness, capture evidence,
and leave an easy listening comparison ready. This is patch/test work, not a request
to redesign DSP, change production defaults, sync Pro, or publish anything.

The main decision is whether the anti-aliasing benefit of Bifurx's boundary
oversampling is worth its coloration and added delay, especially when the processed
signal is mixed with a direct dry path. Do not choose a winner from alias numbers alone.

## Context established so far

- The SVF pair runs at host sample rate. Separate input LEVEL and output limiter
  boundaries each perform upsampling and downsampling when oversampling is enabled.
- The old boundary chain uses Rack 2x resamplers with quality 8 (16-tap filters,
  cutoff 0.9). It significantly darkens the high end.
- The current candidate uses quality 32 (64-tap filters, cutoff 1.0). It preserves
  substantially more treble, but its clean boundary-pair impulse peak is at 62 samples
  versus 14 for legacy. At 48 kHz that is about 1.292 ms versus 0.292 ms. These are
  boundary measurements, not a promise that a nonlinear full-module impulse has that peak.
- In High + High, the user heard the highs return when **Legacy dark boundary was
  unchecked**. They also heard brightness return with oversampling entirely off.
- Raising LEVEL with oversampling off produced some harshness. Subsequent on/off
  listening was inconclusive: harmonics, aliasing, loudness, and filtering are confounded.
- No sample rate has yet been confirmed in this conversation. Read it from Rack.
- Prior records document live sine-based alias improvements, but no explicit user
  listening approval of the original always-on chain was found.
- The spectrum's recent max-hold/uniform-bin-sum changes were reverted in the main
  checkout. Interpolation, tapered weighting, and spatial smoothing are restored.
  Treat that display as a smoothed visual estimate, not a calibrated tone-level meter.
  The implementation document's tone-calibration paragraph is now stale.
- The main checkout differs from ../Leviathan-Pro. Do not sync them for this test.

Useful sources, relative to the Leviathan repository:

- `.agents/skills/octavia/SKILL.md` and `references/monitoring.md` beside it.
- `src/BifurxOversampling.hpp`, `src/Bifurx.cpp`, `src/BifurxInputStage.hpp`,
  `src/BifurxOutputStage.hpp`, and the Bifurx context menu in `src/BifurxUI.cpp`.
- `doc/bifurx-sol-h.md`, especially follow-ups 9 and 12: original alias measurements.
- `doc/bifurx-ast-prem.md`, PREM-01 and PREM-09: passband and analyzer findings.
- `doc/bifurx-premium-implementation.md`: candidate FIR and transition details.

## First: discover and preserve

1. Follow the Octavia skill. Start with `vcv_get_status`. If unavailable, stop and
   ask me to START Octavia; do not repeatedly retry. This does not require the
   in-Rack Octavia Console to be armed.
2. List modules and cables. Identify the existing hi-hat source, Bifurx, Octavia,
   monitoring/output route, sample rate, and current settings. Trace actual cables.
3. Read full state for modules you will edit and retain backups plus cable records
   in a dedicated test-results folder. Record plugin/model slugs and versions.
   Inspect the loaded module's available settings; do not assume the installed binary
   matches the latest source or that the Pro module has the new FIR option.
4. Prefer added test copies beside the existing patch over changing its musical
   source. Preserve the existing sequence. Use one source output fanned to every
   branch so random/noise-based hats are identical across candidates.
5. Resolve every module, parameter, and port ID from live discovery. Use supported
   state/parameter tools; do not guess undocumented HTTP routes or raw memory access.
   Context-menu settings may be serialized data rather than ordinary parameters.
   Merge only the intended fields into a fresh full state when that is the supported
   editing mechanism, then verify. Do not replace state with an incomplete preset.
6. Reversible harness edits and bounded recordings are requested. Do not overwrite
   the existing patch, delete modules, change source code, reinstall plugins, or
   change the host sample rate without the relevant additional authorization.
   If the loaded build lacks a required control, explain that concrete blocker.

## Candidate definitions — avoid toggle confusion

| Branch | Selective 2x nonlinear oversampling | Legacy dark boundary FIR |
|---|---|---|
| Host-rate | Off | Irrelevant; set off for clarity |
| New FIR | On | Off |
| Legacy FIR | On | On |

Serialized fields in current source are `nonlinearOversamplingEnabled` and
`legacyBoundaryResampling`. Verify names against the loaded module/state.

**Dragon King debug must stay enabled for the host-rate branch.** Outside debug,
production currently forces oversampling on regardless of the stored off setting.
Do not claim bypass is effective merely because JSON says false. Verify debug state
and, if feasible, a clean timing/transfer capture. Turn expensive debug recorders off;
debug enablement does not require performance/curve logging.

Display Only bypasses both boundary chains and is not an oversampling comparison mode.
Use it only as an optional routing reference. Do not confuse bypass with normal H+H.

## Physical routing

Use the actual hi-hat source, fanned simultaneously to three identically configured
Bifurx copies. Only the boundary settings above differ.

- Octavia A: dry source immediately before the branch split.
- Octavia B: host-rate Bifurx output.
- Octavia C: new-FIR Bifurx output.
- Octavia D: legacy-FIR Bifurx output.

Physically cable every observation. Discover monitor status and verify all four
connections and sample rate before capture. Preserve Master L/R unless explicitly
needed. Do not sum all candidate outputs into the speakers. Use a downstream selector
or muted mixer at a comfortable listening level, with only one candidate audible.
Avoid using Bifurx LEVEL to match playback loudness: it changes the treatment itself.

Legacy is mainly a diagnostic reference, not the preferred replacement. The main
listening decision is host-rate versus new FIR.

## Short test sequence

Begin at the user's current sample rate. Keep High + High, center frequency around
20–40 Hz, minimum SPAN and RES, neutral BAL, centered TITO, self-oscillation off,
and no parameter CV. Verify actual displayed frequencies and parameter mappings.

Use 6–10 second simultaneous A/B/C/D recordings containing several hat hits. Let all
branches settle before capture. Change settings between takes, never during a take;
FIR switching deliberately fades and primes state, so switch transients are not the
steady-state sound under evaluation.

1. **Clean:** LEVEL 50%, Soft Limiting off.
2. **Driven:** same settings, LEVEL at the user's harshness-producing position,
   Soft Limiting off. Record the exact value; if unknown, try 80%, then 100% only
   if necessary and adjust downstream listening gain.
3. **Limiter:** LEVEL 50%, Soft Limiting on, common input gain sufficient to make
   limiting engage. Keep the source feed identical for all candidates. If the limiter
   never engages, report that instead of calling this a limiter comparison.
4. **Combined:** the same driven LEVEL with Soft Limiting on, if the preceding
   comparisons leave a meaningful question or this matches the user's normal use.

The input stage is exactly undriven at LEVEL 50%; drive onset is above about 62% in
current code. The output knee begins at 4 V and approaches 5 V. The SVF's own character
processing can still be nonlinear: these conditions isolate boundary settings, not
all possible distortion sources.

Prioritize this small matrix. Do not automatically expand to every mode/sample rate.
Once H+H is understood, offer a brief resonant Band + Band or Low + Low check if needed.

## Capture and analysis

Read the monitoring reference before using captures. Prefer one bounded archival
recording with A/B/C/D together for each condition. Octavia's archival recording route
exports the identical engine-frame interval for all channels, with a JSON sidecar.
Use returned paths; do not invent filenames or claim to hear unrecorded outputs.
Only one bounded recording/capture can be active at a time.

**Recorded WAV values are raw Rack volts, not normalized PCM audio.** Preserve the
original files. Produce separate listening files with one documented common volts-to-
normalized-audio scale and sufficient headroom before any candidate-specific matching.
Never send an unscaled raw-voltage WAV straight to normal playback.

For each take retain:

- Raw WAV, sidecar, engine frames, channel mapping, sample rate, exact settings.
- RMS, peak, DC, crest, output-limit observations, and level-normalized band differences.
- Clean branch timing/transfer estimates where supported. Keep SVF phase distinct from
  extra boundary delay. Noise/hats alone do not identify individual alias products.

Create two distinct listening comparisons:

### 1. Sound quality, timing aligned

Extract B and C from the same recording. Use clean-condition timing measurements to
remove their relative bulk delay, crop to a common valid interval, and document any
fractional-delay method. Do not warp signals or align each hit independently. Fixed
alignment cannot remove frequency-dependent phase or nonlinear differences.

Use constant downstream gain per candidate, measured over the same active passage,
for level matching. No compression, limiting, EQ, per-hit gain, or automatic peak
normalization. Report the matching gains and remaining RMS/loudness difference. Keep
unmatched versions available because gain/tonal changes are themselves useful evidence.

Make anonymous, randomized X/Y files or a verified in-Rack listening selector and
retain the mapping separately. Let the user judge before revealing it. Ask whether one
has objectionable metallic/fizzy residue, loses brightness, or changes attack. A short
informal preference trial is not statistical proof of audibility or an ABX result.

### 2. Actual parallel behavior, timing preserved

Use the original simultaneous channels without time alignment. Make fixed-ratio mixes
of A+B and A+C, initially equal dry/wet coefficients with common headroom. Preserve
actual branch gains and timing in the primary comparison. If level-matched playback
versions help, apply gain after forming each complete mix and retain the originals.

This is where added delay, cancellation, and transient changes must remain audible.
Do not delay the dry path to conceal the cost being evaluated. An additional explicitly
labeled delay-compensated comparison may illustrate a workaround, but must not replace
this test. The SVF already has phase response; no version promises phase-neutral mixing.

Use supported installed playback tools/modules. If external file rendering/playback
is unavailable, leave the physical selector and dry/wet mixer ready and explain which
comparisons could not be produced. Never fabricate recordings or listening results.

## Optional focused measurement

Only if useful after the hat comparison, add a bounded clean impulse/low-level tone
measurement and a driven sine test. Use Octavia Control outputs only for temporary,
frame-synchronized diagnostic stimuli, not musical sequencing. Follow their supported
control-program schema and verify physical cables. Use an actual source module when
an audio stimulus is not directly supported by Control outputs.

For alias measurements, choose identifiable folded products and distinguish them from
legitimate harmonics and broadband source energy. Include clean passband and timing
results beside alias reduction: a darker output alone is not evidence of a better design.
Do not use the smoothed Bifurx spectrum as the measurement oracle.

## Deliverables and stopping point

- Leave a clearly labeled, quiet, usable listening harness in Rack, with one audible
  route at a time. Tell the user exactly which selector/mixer to operate.
- Save a concise results Markdown beside the captures: settings, routing IDs, files,
  level matching, timing, measurements, user observations, and limitations.
- Separate measured facts, the user's stated preference, and your recommendation.
  An inconclusive listening result is valid. Do not repeatedly toggle until a verdict
  is forced, or claim smaller alias numbers settle the latency tradeoff.
- Report any original controls/cables changed and how to restore them. Ask before
  saving over the original patch or deleting helper modules; those Octavia actions
  are not undoable. Do not silently disable debug while host-rate listening is active.
- End with a decision-ready comparison of host-rate versus current FIR. A future IIR,
  ADAA, or DSP redesign is outside this task; no implementation decision is pre-approved.
