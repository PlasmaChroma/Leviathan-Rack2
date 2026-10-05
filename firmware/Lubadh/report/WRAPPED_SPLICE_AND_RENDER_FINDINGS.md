# Lúbadh wrapped boundaries and splice rendering

The continuation now checks wrapped selected-region transitions, the callback's physical-seam branches, and downstream multihead sample rendering. Two new probes execute original ELF bytes and compare against independently expressed rules. They advance the tape specification without establishing the complete callback or hardware equivalence.

## What executes

`probe_wrapped_boundaries.py` extends the single-head callback slice from `0x47f44` to before `0x47f10`. Original nested fade, same-engine allocation, transfer and movement routines execute. It checks **33,696 configurations**, **330,480 state-field comparisons**, and 1,178 distinct instruction addresses. Comparisons include compound call traces and head lists; these are not rendered audio sample counts.

Fixtures vary three wrapped/equal-boundary layouts, sign and held override, exact boundary coordinates, fractional position, replacement permission, existing fade flags and one/four/five occupied slots. LinkData-derived locals remain independent fixtures. In particular, the physical reverse threshold/destination used here are **32 and 1282**; this probe does not establish those as factory/device coordinates. It also supplies the reduced reverse-start local directly, without executing its upstream marker/modulo calculation.

`probe_splice_render.py` follows a boundary slice with the original per-head rendering slice `0x48048..0x48394`, stopping before the next-head iteration test at `0x48398`. It renders active slots into the same output vector. It checks **128 splice configurations**, **9,672 gain/audio comparisons**, maximum absolute error `2.8711464317154878e-8`, and 1,472 distinct instruction addresses. Tape contents are a deterministic mixed-sinusoid fixture; no device recording is used.

The render reference takes resulting original head/fade states as its input. It independently expresses fade-vector generation, cubic reads, speed gain and summation, but is **not** an independent recovery of `trigger_fade`'s phase initialization. The fade table is the existing host-cosine reconstruction and fused float operations use host `fmaf`. Color processing, recording-head overlap suppression, plate reverb and final engine-count gain are outside this render comparison.

## Selected and physical boundaries are distinct

For a forward head in a wrapped selected region (`start >= end`), the callback initiates a type-1 selected-region transition in the gap:

```text
end <= current_integer < start
```

This differs from the ordinary-region endpoint convention. The old fade begins at `end`, and the incoming fade begins at `start`. If `previous_integer <= end`, the allocation origin subtracts `end - previous_integer` and the replacement receives a full-block move; otherwise it starts at `start` without that move. The selected-region replacement permission local still applies.

For reverse playback, let `RA` be the original reverse-start local, `M` its reduced local, and `RB` reverse end. When `M == RA`, the selected transition gap is:

```text
RB <= current_integer < M
```

When `M != RA`, it is:

```text
current_integer < M OR RB <= current_integer < start
```

The old fade begins at `M`, and the incoming fade at `RB`. In the reduced-coordinate branch, a replacement is moved through the current block only for `current_integer <= M <= previous_integer`. Otherwise it is allocated at `RB` without the move. The unreduced branch tests whether the previous coordinate was at/above `M`; its gap condition already places the current coordinate below `M`.

These comparisons use integer positions and preserve the separate fraction/held-speed contracts recovered earlier. Equal start/end values take the wrapped branches; they do not imply one universal full-loop or zero-loop interpretation.

## Physical-seam fades bypass selected replacement permission

Forward wrapped playback additionally checks `current_integer >= extent`. If its type-0 fade is inactive, it triggers the outgoing type-0 fade at that extent, using **2458 samples** as duration. It attempts a replacement at coordinate 1, adjusted by the previous-to-extent distance when the previous coordinate was at/below the extent. The incoming type-0 fade begins at 1. This replacement is attempted regardless of the selected-region replacement permission local.

Reverse wrapped playback checks `current_integer < physical_reverse_threshold`. If type 0 is inactive, it fades at that threshold and attempts a replacement at the supplied reverse destination. If the previous position was at/above the threshold, it subtracts `threshold - previous_integer` from that destination and advances the new head through the block. The incoming type-0 fade begins at the reverse destination. It also bypasses selected replacement permission.

The separate 2458-sample fade duration is approximately 50 ms at the recurring DSP constant, rather than the 12,292-sample stored-tail extension recovered from first-record completion. Its literal use here is checked; actual clock calibration and physical reverse-field production remain unresolved.

Physical-seam handling runs before the selected-loop check. One old-head iteration can consequently create **two** incoming heads. In a checked reduced reverse-start fixture, the source at `1 + 0.125` receives both type-0 and type-1 fades, with incoming heads at 1282 and 332. The selected-loop replacement inherits the source's active physical fade as well as starting its own loop fade. This is a fixture-level result, not proof that those exact coordinates occur in a normal patch.

The first replacement can also consume the final available slot, causing a later replacement in the same iteration to fail. Full-engine failures retain outgoing fades and do not steal/reposition active slots. Existing type-0 and type-1 flags independently suppress repeated transitions of their respective kind.

## Fade-vector entry/exit rules

The original `Tap::get_xfade` initializes the output gains to one and multiplies each active fade's contribution. It uses split previous/current fade coordinates and their float32 total difference.

If absolute movement is below float32 0.1, it applies a single table interpolation using the **current** integer/fraction coordinate to the entire block. With larger movement it advances from the previous coordinate by the signed difference divided by frame count.

Table-range entry and exit use integer branches rather than a generic per-sample clamp. A previous integer below zero or above **254** calculates a skipped prefix using `trunc(distance / abs_step) + 1`, bounded by block size. Negative-side skipped samples become zero; upper-side skipped samples retain one. A current integer outside that range similarly defines the processed suffix endpoint. Negative-side trailing samples become zero; upper-side samples retain one. The checked reference reproduces these branches and float32/fused rounding.

When no range-entry adjustment is needed, interpolation begins with the stored previous integer and fraction, rather than splitting their rounded float32 sum again. One fixture exposed why that matters: the summed coordinate rounded to 255 while the separate pair was still integer 254 with a fraction just below one. Preserving the split pair fixed the reference mismatch without changing tolerances.

These rules establish tested fade-vector behavior for the splice fixtures. Arbitrary invalid stationary coordinates, table-memory overrun semantics, expiry scheduling and all fade types' trigger-phase laws remain separate questions.

## Reads, near-stall gain and summation

The renderer starts at the head's previous integer/fraction position and selects the same directional cubic neighborhood recovered earlier: nonnegative speed adds 2 to the anchor, negative speed subtracts 3. It clamps to physical storage bounds rather than wrapping the selected region in the read loop.

The per-sample coordinate increment is calculated from the float32 totals of current and previous positions, divided by frame count. Thus this block renderer has additional float32 coordinate rounding beyond `Tap::move`'s split-coordinate storage. Very long tape positions still need dedicated render tests; small-position results cannot establish one-sample precision across the full capacity.

With held speed, the fade vector receives a flat gain:

```text
speed_gain = clamp(4 * abs(held_speed), 0, 1)
```

The following-speed path generates that gain across a block using separate start/end magnitude locals. The current probe sets those magnitudes equal and confirms the same flat law for following speed. It includes zero, 0.125, 0.5 and 1 in both signs; held overrides can move while following speed is zero. The factor changing head displacement does not enter this particular gain law. A speed ramp with differing start/end magnitude remains open.

Each head contributes `cubic_sample * final_fade_gain` to the shared output through the original fused accumulation. The independent model retains its own cumulative sum across heads. The final module output still requires engine-count normalization and downstream coloration; these raw head sums must not be represented as the complete instrument's output level.

## Reproduction and next integration work

Using the optional environment from [Transport Findings](TRANSPORT_FINDINGS.md), run from the repository root:

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_wrapped_boundaries.py
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_splice_render.py
```

Wrapped results contain representative branch-signature fixtures, executed-address coverage and explicit limitations. The render JSON records every splice/output fixture; its CSV records gain and cumulative audio comparisons after each head.

Next, trace the publication of the actual LinkData boundary fields and execute the callback's active-list iteration, including newly allocated heads and simultaneous recording/playback. Then compare complete fades and nonstationary speed ramps, validate overlap suppression and engine-count gain, and join real tail writing to these transitions. These remaining tasks are necessary before treating the component contracts as a complete tape instrument.

Subsequent [Boundary Source Findings](BOUNDARY_SOURCE_FINDINGS.md) establish the
inline producer/load relationship and selected linked-source pointer stores.
The remaining active-list, recording, ramp and final-output work above is still open.
