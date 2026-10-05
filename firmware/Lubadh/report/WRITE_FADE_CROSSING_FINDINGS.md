# Tape writes when the channel fade crosses its bounds

Complete original `recordInput`, preceded by original `Fade::move`, matches an
independent crossing-envelope/tape model in 864 cases. All 1,772,064 comparisons
have zero discrepancy; coverage is 545 instruction addresses. This extends the
earlier in-table write-fade model to bounded moving crossings at both ends.
It does not yet join gate production, callback expiry or arbitrary out-of-range
stationary states to those writes.

## Reproduction and fixtures

```sh
python firmware/Lubadh/probes/probe_write_fade_crossings.py
```

Results and prefix/suffix examples:
`probes/write_fade_crossing_probe_results.json`. The original main ELF is pinned
by the shared byte harness to SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
Original Fade movement at 0x4bafc and complete recordInput at 0x47818 execute
with real resampling, gain vectors, clipping and tape scatter.

Lower cases start at 2/4/16 and move to -0.25/-0.75/-1/-2. Upper cases start at
252/253/254 and move to 255/255.25/256/258. Frames are 32/128, transport speeds
-2/-1/-0.5/+0.5/+1/+2 and feedback 0/0.5/1. Supplied Tap histories, a transparent
original knee=1/compensation=0 clipper, reconstructed fade table and finite tape
isolate this contract. Every interpolated prefix coordinate is asserted inside
the table range. Tests do not silently accept arbitrary invalid memory reads.

Expected state is independently computed from the supplied step using explicit
host glibc binary32 fmaf, rather than taken from executed movement. Tape writes
use the prior cubic resampling reference and binary32 gain/blend equations.
Every tape cell is compared, including untouched cells; counts are not a
whole-instrument fidelity score.

## Prefix length at a crossing

Let P/C be previous/current combined binary32 fade coordinates, I the current
integer coordinate and K the number of tape writes. The nonstationary envelope
path calculates:

```
delta = f32(C - P)
signedStep = f32(delta / f32(K))
magnitudeStep = f32(abs(delta) / f32(K))
outside = f32(-C) if I < 0 else f32(C - 254)
removed = min(trunc(f32(outside / magnitudeStep)) + 1, K)
prefix = K - removed if removed >= 0 else K
```

The upper distance uses literal **254** (0x437e0000), not 255. The extra +1 is
observable in the last interpolated sample. This formula is checked for the
moving, finite crossings above; K=0 and stationary out-of-range paths are
separate. Its selected prefix advances from P by signedStep per tape sample,
normalizing the integer/fraction coordinate as in the in-table model.

## Prefix and suffix gains

For prefix samples, interpolate the incoming and reverse fade-table curves
with the same binary32 equations as `probe_moving_write_fade.py`. Retention is
`fmaf(1-feedback, reverseGain, feedback)`. Then apply input resampling, incoming
gain, old tape retention and original write clipping.

The remaining suffix changes by crossing direction:

| Crossing | Incoming gain | Old tape retention |
|---|---|---|
| Below zero | 0 | 1 |
| Above 254 | 1 | Supplied feedback |

A lower suffix preserves old tape even at feedback zero. An upper suffix uses
ordinary recording/overdub gain and feedback. This explains why both directions
can dispatch recordInput before expiry without simply applying the table beyond
its valid prefix. The tested prefix/suffix partition exactly reproduces the
complete consumer's resulting tape.

## Remaining connection

`WRITE_FADE_EXPIRY_FINDINGS.md` independently checks that dispatch precedes
expiry, lower expiry resets record heads and upper expiry preserves them.
This probe checks the dispatched consumer's crossing-block tape arithmetic.
They are still explicitly sequenced fixtures, not one event-to-audio callback
history. Join real gate/state publication, manager movement/update and these
writes before closing the full overdub-envelope contract. Stationary invalid
coordinates, extreme overshoots, multiple heads, simultaneous boundary splices
and full input/output coloration remain outside this crossing suite.
