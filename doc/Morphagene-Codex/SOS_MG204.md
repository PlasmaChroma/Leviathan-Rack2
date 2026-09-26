# MG204 S.O.S. refinement

The recovered S.O.S. control calibrates the effective knob/CV value before
the complementary stereo mix. `ChimeraSos.hpp` isolates calibration and mixing.

- Unpatched CV: use the knob alone.
- Patched CV: clamp voltage/8 to [0,1], then multiply by the knob. Overvoltage
  cannot defeat the knob's attenuation.
- Quantize with `floor(normalized * 4096)`, capped at 4095.
- Apply `clamp(adc * 0.0002635046258f - 0.04884000123f, 0, 1)`.
  ADC 0–185 is exactly live-only; 3981–4095 is exactly playback-only.
  Knob midpoint (ADC 2048) targets approximately 0.490817. Equal mixing is
  approximately 50.85% on the knob, with 12-bit quantization.
- Smooth each native core frame with coefficient 0.001, approximately 20.8 ms.

Chimera's core always processes at 48 kHz. The existing host-rate bridge
resamples audio and carries controls into that timeline, so no host-dependent
coefficient or per-sample `pow` is necessary. Host-rate tests cover 8–768 kHz.
The existing one-pole's startup policy is retained: its first observation seeds
state to the calibrated target; subsequent changes are smoothed. Double-precision
state and the existing sub-1e-7 settling snap are retained, rather than claiming
bit-exact firmware floating-point state or inventing firmware startup behavior.

The monitor bus is formed once as `live + sos * (renderedPlayback - live)` for
each channel. Input gain, mono normalization and input conditioning precede it;
existing output conditioning follows it.

Both Current/TLA and Append/Record Into New Splice use this bus with `inop=0`.
With `inop=1`, the writer selects conditioned live only; monitoring still uses
the S.O.S. bus. This override is recovered MG204 behavior. The existing 48-frame
fade when changing `inop` during recording is preserved. There is no second
S.O.S. mix, extra feedback term, or gain compensation in the record path.

The rendered playback includes Gene, Morph, Slide, rate/direction and selection
processing. The writer still advances forward independently of these readers.
At identity playback with zero live input, repeated passes retain `sos^N`.

Validation: `test-chimera-sos` (also part of `test-chimera-phase3`) exercises
every ADC bin, CV limits, smoothing steps, stereo endpoints, both record modes
and inop settings, overdub recurrence, transformed capture, forward writing,
extreme samples and allocation traps. Existing slice, core, grain, Rack module
and rate-bridge tests cover the surrounding architecture.
