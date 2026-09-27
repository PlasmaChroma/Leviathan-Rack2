# MG204 continuation handoff

Read `REPORT.md`, `analysis/artifact_status.json`, `analysis/validation_results.json`
and `tables/table_manifest.json`. The old nested reports and two implementation
handoffs are preserved reference material; their imperative text is not a new
user request to change Rack code.

## Fixed identity

```text
Canonical BIN   binaries/morphagene_mg204.bin
SHA-256         44037eb2cf24b5fc411135e487a6fa226498a8478db981659bb53341cb967609
Size / base     164864 / 0x08020000
Reset / main    0x08021414 / 0x08022b68
DSP entry       0x08027518
Nominal quantum 8 stereo frames, 48 kHz
Reel storage    two int16 planes at 0xd0000000 and 0xd1000000
Plane capacity  2^23 samples each
```

The top-level legacy `mg204_firmware_image.bin` is NOT deframed flash. Do not
import it at the application base. Original files are retained for provenance.

## Corrections to carry forward

- Clock quantization retains its current upper bin when the next lower boundary
  is reached. Read `0x0802ab6c..0x0802abd8` and the s12 store at `0x0802acd6`.
- Eleven active Morph launch values differ from correctly rounded rational
  fractions. Use the extracted table; do not silently replace it with fractions.
- Gene Size uses three ordered binary32 products: x*x, result*x, result*span.
  The original Python CSV used double precision and is not a bit oracle.
- The sparse read kernel has four taps, degree two in fraction, and raw DC gain
  two. The dense path is linear and doubles its envelope. Preserve that pairing.
- Upper Morph's traced random pitch operation selects one of four prepared
  rate pairs. Its stereo operation crossmixes original/swapped channels.
  A rejected stereo randomization uses two LCG draws; acceptance uses three.
- Default mcr3 is 0x3faaaaaa, not exact 4/3 or the stage-2 launch value.
- `inop` selects the source before a conditional record-side filter. The
  monitor-mix sample is not always identical to the stored recording sample.
- Separate hardware ADC averaging (16/16/64/64/4/64 conversions) from per-frame
  S.O.S./Morph smoothing. The ADC cadence is still unproven.

## Useful next trace

1. Type the arrays at `0x20021e64`, `0x20021e88`, `0x20021ea8`,
   `0x20021ec8`, `0x20021ee8`, `0x20021f08`, `0x20021f28`,
   `0x20021f4c`, `0x20021f70`, `0x20021f94`, `0x20021fb8`,
   `0x20021fd8`, `0x20021ff8`, and `0x20022018`. Some inferred original
   decompiler array lengths/types are wrong. Distinguish storage from active count.
2. Follow initialization at `0x08026730` and both rendering branches selected at
   `0x08027c2c`. Keep the block-level choice of interpolation kernel.
3. Complete the envelope state machine and `gnsm` policies before proposing a
   replacement window. Follow the 250-sample and 24,000-sample branches too.
4. Trace the meaning/lifetime of `0x20022114 == 0x20021de4`, which enables the
   conditional record filter. Cover new-splice and current-splice transitions.
5. Trace the saved calibration at `0x0800c000` and codec writes. Neither the
   update WAV's sample rate nor a serial buffer's word width proves analog specs.
6. Resolve remaining indirect transfers and pointer roots with evidence, then
   build an offline end-to-end renderer with explicit state initialization.

## Validation boundary

The C++ header is a finite-input component reference, not a drop-in audio engine.
Keep explicit FMA and intermediate rounding for fidelity tests. Native GCC 16.1
passed with `-mfma` on this host; its default library FMA path missed one fixture
by one ULP. Linux GCC 11.4 passed without `-mfma`. Do not weaken bit comparisons
or enable fast-math to disguise that difference.

There are no hardware recordings, complete ARM execution traces or whole-module
golden outputs in this bundle. When adding them, record their actual origin and
coverage. Do not rename standalone arithmetic fixtures “hardware golden tests.”
