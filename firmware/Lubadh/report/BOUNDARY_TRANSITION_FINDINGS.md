# Lúbadh ordinary-region head transitions

The boundary investigation now executes an original single-head iteration inside `AudioEngine::processChannel`, including its nested allocation, fade, transfer and motion routines. This connects previously tested primitives to the callback's ordinary selected-region transition policy. It does not yet render audio or establish wrapped-region and physical-seam behavior.

## Execution scope

`probes/probe_boundary_transitions.py` enters at `0x47f44` and stops before `0x47f10`, the next-head iteration boundary. The callback prologue, upstream control processing and stack-local production are not executed. Channel, TapManager, LinkData-derived locals and VFP registers are explicit fixtures.

The original TapManager constructor and activation routines initialize the source head. Original `Tap::move` advances it before the boundary iteration. The boundary slice then executes its own calls to:

- `Tap::is_fading`, `0x4bf6c`;
- `Tap::trigger_fade`, `0x4e46c`;
- `TapManager::activate`, `0x4e460`;
- `Tap::transfer_data(source, excluded_fade_type)`, `0x4bcf8`;
- `Tap::move`, `0x4bde0`.

Observers record the original allocation and fade-call arguments without replacing their execution. An independently expressed model compares branch outcomes, call traces, active slots, held state and replacement coordinates. The source coordinate pair is also checked byte-for-byte to ensure the transition does not move the outgoing head.

This remains an object/callback-slice comparison in the bounded offline byte harness. It does not execute appliance processes or substitute for hardware measurements. Fade-state phase values and rendered fade samples are outside the comparisons here; the observed call sequence alone does not establish an audible splice waveform.

## Direction and endpoint conventions

The branch uses the source head's held speed when enabled, otherwise the supplied following speed. Zero speed takes the nonnegative branch. The displacement separately includes the supplied factor and frame count.

For an ordinary region with start `A` and end `B`, `A < B`, forward playback accepts integer head positions **`A <= p <= B`**. It initiates a selected-region transition when `p < A` or `p > B`. This differs from the `(A, B]` cursor check in `setLoopingParameters`; those two checks must not be merged.

Reverse playback compares against separate locals `RA` and `RB`, originating from LinkData offsets 0x18 and 0x20. It accepts **`RA <= p < RB`**, transitioning when `p < RA` or `p >= RB`. The fixtures use `RA = 132`, `RB = 532`, alongside `A = 100`, `B = 500`. They deliberately establish the separate-field contract without executing the upstream publication of those fields.

The boundary predicates inspect integer coordinates. Fractions influence prior motion and the resulting integer crossing, but are not included directly in those endpoint comparisons.

## Outgoing and incoming heads are separate

If the outgoing head already has its type-1 fade active, the selected-region transition does not repeat. Otherwise it starts a type-1 fade at `B` for forward playback or `RA` for reverse playback, with the configured fade duration. The source head retains its current and previous coordinates; the code does not apply an in-place modulo wrap.

When the supplied replacement-permission local is false, processing stops after the outgoing fade. When it is true, the callback attempts an additional slot in the **same logical engine**, retaining the source's engine ID.

For an integer crossing from previous position `q` to current position `p`, the allocation origin is:

```text
forward:
    distance = B-q if q <= B <= p, otherwise 0
    incoming_origin = A-distance
reverse:
    distance = RA-q if p <= RA <= q, otherwise 0
    incoming_origin = RB-distance
```

If that integer crossing occurred, the newly activated head receives a full block of original `Tap::move` using the source's held/following speed and the same factor/frame count. If it did not, it stays at the allocation origin. Thus an already-outside source and a source crossing during the current block do not produce the same incoming motion history.

The incoming head then starts its type-1 fade at `A` for forward playback or `RB` for reverse playback. The source fade call has its first bool argument false; the incoming call has it true. Their final direction bool is true for forward, false for reverse. These are recorded arguments, not a newly inferred replacement fade equation.

## Transfer excludes the splice fade and retains held speed

`Tap::transfer_data` copies the held flag/speed and all four 24-byte fade states, then restores the destination's specified excluded fade state. The boundary path excludes type 1, preserving the incoming slot's fresh state for its own splice fade. Other source fades can therefore accompany a replacement instead of being implicitly reset; this probe does not yet compare their complete phase evolution.

The routine does not copy the source's position or its fraction. A newly activated head begins with fraction zero. After an integer crossing, its new fraction comes from the full-block displacement from that zero origin; it does not automatically equal the source fraction.

For a fractional previous source position, this can differ from simply translating the complete floating coordinate by `B-A`. This is an observed coordinate rule to preserve in a fidelity model. Whether to change it in an improved instrument requires a deliberate splice design and audible comparison, not silently normalizing it away.

One checked fixture starts the source at `496 + 0.375` and advances it by `7 * 0.75 = 5.25` tape samples. It reaches `501 + 0.625`. The incoming origin is 96, and the full-block move reaches `101 + 0.25`, rather than a translated `101 + 0.625`. Both coordinates are explicit emulator results; no audible consequence is claimed yet.

## Exhaustion leaves an outgoing fade without a replacement

The boundary path uses direct `TapManager::activate`, whose full-engine result is null. If all five slots in that engine are active, the outgoing type-1 fade is still initiated but no incoming head is created or moved. It does not use `activate_new`'s nonnull fallback behavior recovered in the allocation report.

This demonstrates the caller's response to direct slot exhaustion within the tested ordinary-region branch. It does not establish the audibility or duration of a missing replacement in the complete callback, how fast other fades reclaim slots, or which user actions can produce the same saturation timing.

## Validation and remaining work

The script systematically varies following-speed sign/magnitude, held-speed overrides, movement factor, frame count, initial position fraction, exact integer endpoints, replacement permission, full-engine saturation and the outgoing fade flag. It stores every configuration and observed result in `probes/boundary_transition_probe_results.json`, together with executed-address coverage and explicit limitations.

The expanded run passes **16,128 configurations** and **135,464 state-field comparisons**, executing 1,006 distinct instruction addresses. The comparison count includes compound call-trace and active-slot fields; it is not a count of independently rendered audio samples. Following speeds are 0, +/-0.5, +/-1 and +/-4; held overrides are absent, -1 or +1; factors are 1 and 0.75; block sizes are 7 and 32; initial fractions are 0 and 0.375.

Run from the repository root with the optional analysis environment described in [Transport Findings](TRANSPORT_FINDINGS.md):

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_boundary_transitions.py
```

The follow-up [wrapped-splice and render findings](WRAPPED_SPLICE_AND_RENDER_FINDINGS.md) now check wrapped selected regions, physical-seam branches and raw multihead rendering under fixtures. Actual LinkData field publication, callback active-list iteration and final output processing remain open. Clock/link event ordering, real tail writes, color processing and the plate network remain separate unfinished fidelity work.
