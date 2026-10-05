# Independent Tap fade state and rendering

Complete original inactive `trigger_fade`, `Tap::move`, `get_xfade` and
`Tap::update` match an independent state model in 960 trigger fixtures and
6,720 motion/update blocks. The 4,772 supported gain renders and 484,080 state/gain
comparisons have zero discrepancy. Coverage is 651 instruction addresses.
There are 316 whole-head lower resets and 372 individual upper fade resets.

## Reproduction and scope

```sh
python firmware/Lubadh/probes/probe_tap_fade_state.py
```

Results: `probes/tap_fade_state_probe_results.json`. Shared byte-harness identity:
main ELF SHA256 `2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
The binary is inert evidence; no appliance process executes. Trigger at 0x4e46c,
motion at 0x4bde0, gain at 0x4bf7c and cleanup at 0x4bfa0 execute completely.

Fixtures cover all four fade kinds, both interval directions, integer/fractional
head coordinates, five targets before/at/within/at/after the ten-sample integer
interval, both trigger booleans and durations 32/128/1000. Each fixture then
uses seven 32-frame blocks with stopped, forward, reversed, half-speed and
double-speed motion. The independent state drives the existing interpolation
reference through a model-only memory view. No original fade state is used as
expected rendering input.

## Trigger contract for an inactive selected fade

Let P/C be previous/current head integer positions, pf/cf their fractions,
X the target integer, D the positive supplied duration, R the first boolean and
F the second. The tests treat these booleans as explicit inputs, rather than
claiming every combination is produced by every user event.

```
crossing = min(P,C) <= X <= max(P,C)  # fractions do not affect this test
step = f32(256 / f32(D))
if R != F: step = -step
offset = trunc(f32(f32(X-P) * step))

if R:
    start = -offset if crossing else 0
else:
    start = 255-offset if crossing else 255
```

The selected fade activates with integer start and fraction zero. For R=true,
the final signed step is stored immediately. When crossing, movement is applied
by the combined binary32 head-coordinate difference:
`f32(f32(f32(C)+cf) - f32(f32(P)+pf))`, with binary32 FMA and unit normalization.
Otherwise current and previous stay at the initialized start.

For R=false, initialization temporarily uses step +1, moves by the literal
0xb58637bd (approximately -1e-6), then moves by zero before assigning the final
step. The zero movement copies the biased coordinate to the previous fields.
When crossing, the combined head displacement is applied afterward. This bias
keeps the nominal 255 endpoint just below it; it is observable in exact state
and gain results. It must not be replaced by a coarse integer-only phase model.

## Movement and cleanup

Tap::move only advances active head fades. It uses signed effective head motion,
not its magnitude: reversing the head can reverse progression of a previously
triggered fade. Each movement publishes the prior fade coordinate, then uses
binary32 FMA and repeated binary32 unit normalization. Stopped active fades
publish identical endpoints and render through the stationary path.

Tap::update checks active fade integer coordinates, independent of their
fractions. A coordinate below zero resets the entire head and all four fades;
a coordinate above 254 resets that individual fade while retaining the head.
Integers zero through 254 stay active. The post-update reference state is carried
into later blocks; it is not refreshed from the binary.

## Explicit omissions and next integration

The suite omits gain rendering in 240 blocks where an active fade is stationary
and its integer is outside 0..255. It still executes and checks movement and
cleanup in those blocks. The existing interpolation reference explicitly rejects
those table indices; arbitrary adjacent mapped memory is not accepted as a gain
model. These are supplied trigger/direction combinations, not yet evidence that
production scheduling reaches the same states. Reachability, original stationary
render behavior and the Rack policy for such states remain open.

Already-active triggering takes a diagnostic branch not covered here. Zero
duration, extreme coordinates, simultaneous fades, manager allocation and full
event/audio scheduling are also outside this suite. The fresh-head gate histories
now independently check their inactive fades; this suite supplies the active-fade
state model needed to extend them to reuse, boundaries and multiple heads.

Subsequent `ACTIVE_FADE_BOUNDARY_FINDINGS.md` checks the complete already-active
branch and joins the independent model to ordinary boundary allocation, raw
audio and manager update. Those later results supersede the corresponding
integration gaps above, while retaining their stated scheduling limits.
