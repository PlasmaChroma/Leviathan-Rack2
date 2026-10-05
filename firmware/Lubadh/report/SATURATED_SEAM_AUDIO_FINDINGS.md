# Occupied-slot seam allocation and audio

Physical, wrapped selected-region and overlapping seam transitions now pass
independent state/audio checks with four and five occupied slots in one Tap
engine. All 1,440 cases / 7,080 head renders pass, with 1,135,160 comparisons and
maximum gain/mix discrepancy 1.1074456107706965e-7. The suite checks 600 successful
allocations, 696 failed allocation attempts and 192 heads with simultaneous
fade kinds. Coverage is 1,643 instruction addresses. These counts establish the
bounded occupied-slot contract below, not full factory parity.

## Reproduction and scope

```sh
python firmware/Lubadh/probes/probe_boundary_fade_model.py --seams --saturation
```

Results: `probes/saturated_seam_fade_model_probe_results.json`. The shared byte
harness pins original main ELF SHA256 to
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
No appliance process executes.

The complete original single-source boundary iteration executes real fade
triggering and same-engine replacement allocation, then independently predicted
head/fade states drive gain and raw mix comparisons. Complete manager update
executes after rendering and all active flags/fade fields are compared. The
model does not use original output head positions or fade endpoints to generate
reference audio.

Fixtures extend the available-slot seam families to occupancy four/five, with
the same effective speeds -1/-0.125/0/+0.125/+1, following/held choices, fractions
0/0.375, frames 7/32/128, selected duration 32/128 and replacement permissions.
Other occupied slots are initialized as stationary unfaded heads at positions
10001 onward. They are explicitly rendered alongside the boundary source and
any successful replacement. Their following live speed, rather than the source's
possibly held/opposed speed, determines raw sampling direction and speed gain.
The deterministic tape grows to 16,384 cells to include those coordinates.

## Allocation failure leaves the outgoing transition in place

The source fade is triggered before replacement allocation. At five occupied
slots, same-engine activation returns null; the callback skips transfer, incoming
fade triggering and replacement rendering. The outgoing source fade remains
active and is included in the verified audio. Allocation failure does not cancel
the seam or silently steal an existing slot in this boundary path.

At four occupied slots, the first permitted replacement can fill the last slot.
In overlapping reverse seam cases, physical allocation precedes selected-region
allocation. If the physical replacement takes the last slot, a subsequent
selected replacement attempt fails. Both source fade kinds remain present; the
physical replacement retains the source fade state inherited at its transfer
time. Later source fade changes are not retroactively copied into it. This
ordering and its raw gain/mix consequences are checked against the independently
generated ordered transition list.

Unrelated occupied heads preserve their inactive fade states and remain active
through manager update. Source/incoming fades still obey their normal lower
whole-head reset and upper individual-fade reset policies. There is no replacement
audio for a failed attempt, but preexisting head audio continues to accumulate.

## Remaining integration

After adding occupied-slot support, ordinary mode (160 cases / 240 renders) and
available-slot seam mode (720 cases / 1,368 renders) were rerun successfully.
Their maximum discrepancies remain 2.510718088988284e-8 and
3.0401959949521995e-8 respectively.

This covers one old source's boundary transition in one engine, with explicit
render order. It does not move or boundary-check the other old heads before
rendering, execute the callback-wide active-list iteration, or model four-engine
age/stealing behavior from retrigger bursts. Existing allocation/head-iteration
suites check related pieces separately. Persistent saturation/reentry, multiple
old-head transitions, simultaneous recording-manager boundaries, upstream field
publication, full output coloration and callback scheduling remain open.
