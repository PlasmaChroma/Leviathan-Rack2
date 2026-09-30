# MG204 Gene, Morph and splice envelope trace

Traced 2026-09-29 from the existing `disassembly/dsp_core.asm` against the
canonical application identified in `CODEX_HANDOFF.md`. This is static
instruction evidence and a component transcription, not hardware listening,
an ARM execution trace, or an end-to-end golden renderer.

## Envelope configuration: recovered conditions

The setup at `0x08028ad4..0x08028ae8` starts from half the effective Gene
duration N. The duration has already passed its eight-sample floor. The
configuration is shared by the two render branches.

| Quantity | RAM | Meaning established by producer/consumer |
| --- | --- | --- |
| N | `0x20021f90` | effective Gene duration/chunk quantity; whole-splice mode needs separate interpretation |
| 1/N | `0x20021e84` | reciprocal duration |
| E | `0x20021e0c` | envelope edge span, initially N/2 |
| d | `0x20021e08` | envelope increment, initially 2*(1/N) |
| D | `0x2002209c` | extracted Morph density entry |
| F | `0x20021f48` | extracted Morph launch factor |
| gnsm | options base `0x200012a8` +40 | Gene Smooth option |

The finite positive configuration reduces to:

```text
E = min(N/2, 24000)
d = 2*(1/N), or firmware 0x382ec33e when the cap is taken
if E > 250:
    if D < 1 or gnsm == 0: E = 250; d = firmware 0x3b83126f
if F == 1: E = 250; d = firmware 0x3b83126f
```

Evidence: cap at `0x08028942..0x08028964`; D/gnsm decisions at
`0x08028966..0x0802897c`; F equality decision at
`0x08027b7a..0x08027b86` and `0x08028980..0x08028990`; short constants
stored at `0x08027b88..0x08027b92`.

Thus classic mode usually chooses 250 output frames (5.208333 ms at 48 kHz).
Smooth mode with overlapping launches retains half-duration edges, up to
24000 frames (500 ms). F==1 forces 250 even with smoothing enabled. For
short Genes with E<=250 and F!=1, the half-duration edge survives even in
classic mode. Do not replace this selector with a universal 250-frame rule.
The extracted D and F tables must be read independently; do not manufacture
D by taking 1/F.

`reconstruction/audited_components.hpp::geneEnvelopeConfig` transcribes this
configuration. `tests/test_envelope_config.cpp` checks branch examples and
constant bits; expected results are static-transcription fixtures, not an
independent firmware execution oracle.

## How the configuration reaches the reader

The sparse branch loads d at `0x08028154..0x0802815c` and E at
`0x08028260..0x08028278`. A voice envelope is stored in the array rooted at
`0x20021e64`; its output-frame age is rooted at `0x20021ec8`. The launch path
initializes age and envelope, captures splice bounds into arrays rooted at
`0x20021fd8` and `0x20021ff8`, and records splice identity at
`0x20021fb8` (`0x0802834c..0x080283da`).

The per-frame attack branch adds d (`0x08028d44`) and the release branch
subtracts d (`0x080284fe`). Age increments once per output frame at
`0x08028576..0x08028592`. The envelope multiplies the interpolated samples
before stereo crossmix (`0x08028c36..0x08028c52`). These are linear envelope
increments; Chimera's cosine edge is not the recovered curve.

The mirrored dense branch loads the same d and E at
`0x080299bc..0x080299c4` and `0x08029a28` onward, and uses corresponding
attack/release operations at `0x0802a060..0x0802a074` and
`0x08029d24..0x08029d2e`. Dense interpolation doubles envelope scaling.
The complete plateau/retirement/relaunch policy, including short Genes,
still requires executable state fixtures; these observations alone are not
a complete drop-in window formula.

## Splice boundaries have their own attenuation

In the sparse reader, `0x08028524..0x0802855c` multiplies the envelope by:

```text
if address <= begin + 63: envelope *= (address - begin)/64
if address >= end - 63:   envelope *= (end - address)/64
```

The exact address here is the reader's integer source address after masking
and wrapping; the floating fractional coordinate must not be substituted
without tracing its conversion. These are 64 SOURCE-sample ramps, not an
extra 250-output-frame crossfade. Their time depends on playback increment:
64 samples at unity rate span approximately 1.333 ms; at 2x, approximately
0.667 ms. The end-bound convention is preserved as the instructions use it.
The mirrored boundary ramp appears at `0x08029d48` onward. Boundary flags,
special envelope states and full-splice mode affect the preceding decisions.

## Organize and overlapping voices

`omod` is read from options +12 at `0x080280c0..0x080280ca`.
When enabled, a changed requested splice (`0x20021cbc`) is copied to
`0x20021de4` at `0x0802949c..0x080294aa` and enters the launch path at
`0x0802902a`. That path advances the rotating voice slot and marks the next
slot for launch with sentinel -111111 (`0x08029030..0x08029062`). It does
not clear the entire voice-envelope array there. The dense branch mirrors
this at `0x0802a7a0..0x0802a7ae` and `0x0802a34c` onward.

Existing voices have captured their own splice bounds and splice IDs at
launch. Therefore a splice selection must be modeled together with the
rotating voices and their envelopes. A single global old-reader/new-reader
crossfade is not a demonstrated firmware architecture. Normal deferred
selection also interacts with launch cadence and transport flags; the exact
commit/EOSG event ordering is not yet fully reconstructed.

Morph changes launch timing through F and the cycle table, changes envelope
configuration through D/F, and changes shared gain through envelope sum.
Gain lookup uses `clamp(trunc((sum-1)*6)+3,0,33)` and smooths its target
with 0.01/0.99 each output frame (`0x08027ade..0x08027b16`,
`0x08028dca..0x08028df8`; dense mirror `0x0802a098..0x0802a0c6`).
Consequently overlap loudness cannot be reproduced by dividing by the sum
of voice weights alone.

## Implications for Chimera parity

The implemented 250-frame forced-transition crossfade is a useful adaptation,
with reported audible improvement, but it is not a recovered standalone
MG204 Organize crossfade. Smooth Gene's current max(96,20% of N) cosine
window does not implement the recovered half-duration/250/cap selection.
The current 96-frame classic edge likewise differs from this selector.
Changing only 96 to 250 would miss Morph's conditions, linear envelopes,
source-boundary ramps, captured splice bounds, and gain-table smoothing.

Remaining work for a parity implementation: finish an executable per-voice
state model for both renderer branches; trace normal versus immediate splice
commit and EOSG ordering; exercise whole-splice, reverse, short Gene, clock,
retrigger, and repeated-selection cases. Preserve exact table values and
instruction rounding. No hardware equivalence claim is justified until an
independent execution or recording oracle covers those paths.

## Chimera audition implementation

The context-menu option `Recovered firmware envelopes (experimental)` is
optional in Chimera; the previous engine is the default. Saved explicit choices
are retained.
Finite Genes use the recovered E/d selector, linear attack/hold/release,
64-source-sample boundary ramps, the extracted gain table and sparse/dense
reader arithmetic. Splice changes rotate the launch slot while other voices
retain their source regions. Immediate Organize no longer disables those
envelopes. These paths allocate nothing per sample.

This is a partial reconstruction for audition, not bit-exact MG204 playback.
The existing four-slot scheduler, onset choices and selection/EOSG logic are
retained. Envelope configuration is latched at launch; the firmware updates
shared configuration and per-voice thresholds more intricately. Reader branch
selection is held for eight frames as in the firmware callback. Whole-splice
mode retains the previous playback implementation,
including its added transition fades. Short unity Genes, control changes during
a Gene, clocks and precise release/launch ordering need an execution oracle.
Optional bandlimited playback is an independent quality extension in both
engines. With it off, recovered finite playback uses the firmware reader.
Native component checks validate selected
rules and regression behavior, not hardware equivalence.

Audition feedback reported increased level and noise/clicking. The initial
implementation missed the launch's outgoing release-threshold write at
0x08029030..0x08029036. The corrected path sets the previous launched voice's
release threshold to its age, preserves the earlier natural-release deadline,
and retires on envelope exhaustion rather than truncating at the original
duration. The inclusive age comparison is tested. The selector now reads the
occupied-density ROM table independently from launch rate. Raw reader gain is
two; complete hardware/Chimera level calibration remains unresolved. These
changes are not evidence that the reported noise is fully resolved.

## Playback bus integration, 2026-09-29

Experimental finite playback now applies the enable gain with
`g = .999*g + .001*enabled`, then hard-clamps playback to [-1,+1], then
forms the existing calibrated/smoothed complementary S.O.S. mix. Evidence:
0x08028e28..0x08028e32 (enable gain), 0x08028e42..0x08028e76 (clamp),
0x08028e7a..0x08028e90 (S.O.S.). Dense clipping mirrors at 0x0802a110 onward.
The same pre-output-conditioning bus feeds the default recording source.
This is hard saturation, not a soft limiter or adaptive compressor.
Chimera's normalized unit maps nominally to 5 Rack volts; that convention
does not establish per-device hardware output calibration.

`chimera_slice_spec.cpp` checks both clipping polarities with loud reel data,
and checks an opposing live signal at half S.O.S. to distinguish clipping
playback before mixing from clipping the completed mix. Conditioning is
disabled in that fixture so a DC blocker cannot obscure the bus equation.
Baseline behavior is checked separately in the same fixture. No audible
equivalence claim follows from these component checks.

Remaining pipeline differences to resolve as a system:

- Live input: firmware's 1.5 scale/clamp and per-device/codec calibration;
  Chimera still uses its existing input gain and conditioning.
- Voice state: shared duration/envelope changes, slot reuse and exact
  deferred-selection/clock/EOSG ordering.
- Recording: conditional 0.7/0.997 filter, integer conversion and resampling;
  float reel storage currently differs from native signed-halfword writes.
- Output: signed integer conversion, codec settings and analog AC-coupling
  response. Existing output DC conditioning remains an approximation.
- Transport: firmware's enable predicate and startup state need exhaustive
  tracing; the recovered enable coefficient is applied to Chimera's existing
  predicate. Its explicit Play-stop tail still uses the earlier adaptation.
- Whole-splice mode remains on the baseline path. Optional bandlimiting is
  independent of the experimental toggle (see below).

Continue using the A/B toggle for investigation. Neither leveling the modes
by ear nor adding an assumed analog limiter establishes firmware parity.

## Independent bandlimiting extension

The existing bandlimited playback option now applies to experimental finite
voices. This is a Chimera quality extension, not recovered MG204 behavior.
With filtering off, or at absolute effective speed <=1, the recovered reader
is retained. Above unity the existing windowed-sinc reader is blended in,
with its normalized output multiplied by TWO to preserve recovered reader
DC gain before envelopes, overlap gain and clipping. The switch uses the
existing 240-frame quality blend and a continuous near-unity speed blend.
Effective speed includes voice pitch, Slide movement and position modulation.

Integration fixtures check constant-source gain, unchanged unity playback,
unchanged voice counts and rejection of a 0.3125-cycle/source-sample tone
that folds into the output band at 2x. The native reader benchmark measured
roughly 0.98% of one CPU core per stereo voice at 2x on this machine, versus
0.095% for the unity cubic path; higher speed can cost more. These are short
reader-only timing estimates, not whole-module or live-patch CPU measurements.

The quality selection now offers Full (radius 8*speed) and Balanced (radius
4*speed), with the same .94 normalized cutoff and a separately prepared
Blackman-windowed sinc table. Balanced halves the direct tap span and trades
stop-band rejection for CPU cost. Existing patches default to Full when the
new `balancedBandlimiting` JSON boolean is absent. Both engines use the same
choice. Quality changes blend over 240 core frames; both readers run during
that brief transition, only one afterward.

Native benchmark on this machine: about .96% of one core per stereo reader
at 2x Full versus .48% Balanced; at 8x about 3.54% versus 1.77%. A 15kHz
source at 2x measured RMS .000789 Full and .06325 Balanced for a unit-amplitude
sine (roughly 59dB and 21dB rejection relative to unfiltered sine RMS).
Timing estimates vary with load and exclude the rest of the module. Tests
also cover Balanced's live-overwrite block-cache accuracy and constant gain,
and integration into experimental voice rendering.
