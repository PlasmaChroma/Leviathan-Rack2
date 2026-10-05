# Render traversal, speed ramps and recording-head proximity

`probes/probe_render_iteration.py` executes the original render phase from
`0x47fe4` until just before the output AntiAlias call at `0x483a4`. Unlike the
earlier one-head slices, this runs the complete refreshed-list traversal and
the recording-head attenuation loop for every playback head.

**256 fixtures pass 17,472 gain/audio comparisons**, with maximum absolute
error **8.246194682648422e-8** and 1,024 distinct instruction addresses covered.
The independent model expresses fade gains, velocity/position progression,
proximity attenuation, cubic reads and accumulated output. It reads initialized
original head/fade states; fade trigger initialization is not independently
modeled. Every fixture and ELF hash is recorded in
`probes/render_iteration_probe_results.json`.

## Actual render ordering

The callback refreshes its playback manager list and renders heads in that
list's order. The probe observes every `0x48048` entry and verifies the full
pointer sequence against the allocation-produced age list. It compares each
head's final gain vector and the accumulated raw audio output.

This joins the previously verified age-list schedule to rendering. It avoids
the earlier splice probe's explicit physical-slot traversal. The output is
still the raw head sum, before anti-alias output filtering, engine-count gain
and coloration; it is not complete module output.

## Following-speed gain ramps

Held heads use the previously recovered flat gain `clamp(4*abs(held_speed),0,1)`.
Following heads ramp the **magnitude** from the previous following speed to
the current following speed over the frame count. Each sample uses the current
ramp value before incrementing it. Thus the last sample is one increment short
of the specified endpoint.

The signed speed determines the directional cubic neighborhood; the magnitude
gain ramp is separate. In the checked sign-change fixture from +0.125 to
-0.125, its magnitude endpoints are equal, so this gain remains flat. It does
not fade through zero merely because signed speed changes sign. These are
explicit register fixtures, not a demonstration that the physical speed
controls generate each ramp in one callback.

Static upstream instructions support the register interpretation: `s18` is
the product of the LinkData flutter/touch factors loaded before `processSpeed`;
`s17` is their product loaded afterward; `s20/s19` are previous/current following
speed, and `s22/s21` their magnitudes (`0x47c8c..0x47ce0`). The full speed producer
and its stochastic behavior are not executed here.

## Recording-head proximity attenuation

For each playback head, the renderer obtains the recording manager's active
list and processes every nonzero recording head. At each sample, it compares
the playback and recording **velocities**, including factor and held/following
selection. Positions begin at each head's split previous integer/fraction.

For one playback/recording pair, the checked law is:

```text
relative_velocity = abs(playback_velocity - recording_velocity)
if relative_velocity > float32(0.000001):
    attenuation = clamp(abs(playback_position - recording_position)
                        / (relative_velocity * 4096), 0, 1)
    playback_gain *= attenuation
```

At or below that velocity threshold, this pair leaves the gain unchanged.
Consequently **equal-velocity coincident heads bypass this attenuation**.
Differing velocities with coincident positions instead produce zero gain at
that sample. Distance is an absolute linear tape-coordinate difference, not
a wrapped distance around the selected loop. Multiple recording heads multiply
their attenuation contributions. The inspected loop does not multiply these
contributions by a recording head's fade envelope.

For each pair, velocities start with the previous factor multiplied by the
previous following speed, or by the head's held speed. Their per-sample ramp
is `(current_factor * current_selected_speed - initial_velocity) / frames`,
with the original fused arithmetic reproduced by the model. Positions advance
using the current velocity, fractions normalize while retaining split integer
storage, and velocities then increment. Distance calculations convert those
split coordinates to float32 totals. Long-position precision remains a
separate test requirement.

The 4096 factor comes from the callback literal at `0x48020`; the strict
velocity threshold comes from `0x48024`. This is a velocity-dependent proximity
window, not a generic fixed-distance mute around a write head.

## Tested fixtures and remaining gaps

Fixtures include 7/32-frame blocks, one/four playback heads, held and following
heads, stationary and changing factor/speed locals, a signed direction change,
zero/one/two recording heads, equal/opposed velocities, distant positions,
relative velocities on both sides of the threshold, and held recording speed
zero. Record positions and velocities are initialized states; the probe
does not perform tape writes.

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_render_iteration.py
```

Remaining integration work includes joining boundary generation, recording
resampling/scatter and this render phase on the same objects; complete callback
movement/update timing; anti-alias output, engine-count normalization and
coloration; long-coordinate precision; real control-event ramps; and hardware
agreement. The original reconstructed fade table is used by both execution
and the model, so original-libm table bit identity remains unproven.

Subsequent [Engine Count Gain Findings](ENGINE_COUNT_GAIN_FINDINGS.md) validate
the gain state machine and additive mix in a separate original-byte slice.
Connecting these stages and the full output chain remains open.
