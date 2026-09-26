# Chimera Gene Size: MG204 mapping

This replaces the earlier profile-1 Gene Size curve and endpoint hysteresis.
The mapping follows the MG204 behavior supplied in the implementation request.
CV routing and smoothing, interpolation, voice scheduling, envelopes, and panel
controls are unchanged.

The combined control is rounded to a 12-bit ADC code without inversion.
Codes 0–199 select **whole-splice traversal**, a semantic mode whose source
boundaries are the complete selected splice. This mode bypasses folding and
ordinary clock subdivision quantization. Its elapsed traversal time depends on
playback speed, as before.

For codes 200–4095, the source splice length is repeatedly halved until it is at
most 576,000 samples. The lookup index is `1073 - (code >> 2)`. The table stores
`exp2(index / 341 - 3)`; multiplying the folded length by that value three times
gives the duration in **output samples**, with an eight-sample floor. There is
no eight-second maximum. Four consecutive ADC codes share a lookup entry.
The table is initialized during grain-engine construction, outside processing.

The pure helper returns the fractional firmware duration. The existing engine
rounds it to its integer output-frame lifetime when scheduling a voice. An
ordinary voice advances its source position by its signed playback increment
for that many output frames, so source distance is `abs(increment) * duration`.
There is no second division by Vari-Speed. Source reads wrap within the complete
splice, retaining the existing reader and envelope behavior.

The existing external-clock estimator supplies validity: a connected playback
clock with a measured period and no timeout enables ordinary-duration
quantization. Its candidates use the original, unfolded splice length and the
`1, 1/2, 1/3, 1/4, 1/6, ...` family, with the firmware two-thirds float threshold.
Clock Shift/Stretch source-origin movement remains part of the existing engine.

Mapping caches include the combined ADC value, selected splice length, and
clock validity. Already sounding voices keep their captured envelopes and
lifetimes; subsequent onsets use the new duration.

`make test-chimera-gene-size` covers the curve, endpoint, 12-second discontinuity,
four-code plateaus, clock family and threshold, speed/direction invariance, and
splice-length changes. `test-chimera-phase4` includes this target and the existing
grain regression suite. The phase-1 reference generator now emits this mapping.
