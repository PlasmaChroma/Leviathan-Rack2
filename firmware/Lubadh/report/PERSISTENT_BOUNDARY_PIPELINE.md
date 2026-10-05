# Persistent boundary-generated playback and reclamation

`probes/probe_boundary_pipeline.py` joins the original per-manager boundary
iteration to the already recovered render/filter/gain path. Its continuous
callback slice starts at `0x47ed4` and stops before `0x48454`. Newly allocated
splice heads therefore flow directly through refreshed-list rendering, output
AntiAlias, engine-count gain and additive mixing on the same emulated objects.

After rendering, the original final manager-update slice at `0x48544..0x48550`
executes separately, stopping before the callback epilogue. This deliberately
skips the intervening output coloration and recording operations, whose objects
are not initialized by this probe. Static evidence places both manager updates
after that intervening work in the full callback.

**80 persistent sequences, 960 blocks and 202,080 numerical comparisons pass
with zero discrepancy** in raw sums, filtered sums, additive mixes, filter
histories and gain state. **3,040 additional assertions** check original-head
boundary traversal, refreshed render traversal, unchanged old-head coordinates,
and first-block allocation/slot-count predictions. Coverage totals 2,169
distinct instruction addresses. All fixtures and the ELF hash are in
`probes/boundary_pipeline_probe_results.json`.

## Coverage and actual lifecycle observations

Each sequence retains the same managers, heads, output-filter histories and
gain state for 12 blocks. Test combinations include 7/128-frame blocks,
one/four musical engines, one/five initially occupied slots per engine, and
replacement permission enabled/disabled. Following speed stays at +1 or -1,
factor at 0.75, and initial fractional position at 0.375.

The five boundary layouts cover ordinary forward/reverse, physical forward/
reverse and a reduced reverse-start layout that triggers both physical and
selected-loop seams in one iteration. The ordinary selected-loop fade is 32
samples; physical seam code uses its original fixed duration 2,458. Small
physical coordinates are explicit branch fixtures, not demonstrated normal
recording configurations or reset defaults.

Across these sequences, execution observes 600 direct allocation attempts,
54 blocks with newly rendered heads, 74 blocks with reclaimed heads, and 284
blocks empty after update. These are exercised-event counts, not independent
comparisons or a general distribution of module behavior.

An ordinary forward fixture with one engine/slot, replacement permitted and
128-frame blocks illustrates the persistent lifecycle:

| Block | Heads before boundary | Allocation attempts | Heads rendered | Survivors after update |
|---|---|---|---|---|
| 0 | 1 | 1 | 2 | 2 |
| 1 | 2 | 0 | 2 | 1 |
| 2–4 | 1 | 0 | 1 | 1 |
| 5 | 1 | 1 | 2 | 1 |
| 6–8 | 1 | 0 | 1 | 1 |
| 9 | 1 | 1 | 2 | 1 |

The checked ordinary reverse fixture has the same head-count progression.
This confirms repeated splices after incoming fades clear and outgoing heads
are reclaimed. A departing head can still render in a block whose later update
removes it. Removing it before rendering would change the checked schedule.

Physical-seam fixtures retain overlaps across more blocks because their fade
span differs. The reduced reverse double-seam fixture starts with one head,
attempts two replacements and renders three heads in block zero; subsequent
updates and transitions change that population. Full initial engines still
attempt replacements but produce no additional slot, preserving the previously
recovered allocation-exhaustion behavior.

## Independent model and its limits

The reference independently evaluates fade-vector arithmetic, speed/proximity
gain, rounded cubic reads, fused head summation, persistent AntiAlias recurrence
and gain/mix state. It reads the original **post-boundary head and fade states**
at render entry. Consequently zero discrepancy establishes the downstream
arithmetic and stage connection for these states; it does not constitute an
independent end-to-end proof of fade-trigger or replacement-position equations.
Those laws have separate boundary probes.

The probe verifies the boundary snapshot contains exactly the heads active
before that pass, and that render traversal follows the refreshed list.
Original heads' position/fraction fields remain unchanged during boundary
handling. First-block counts are independently predicted from occupancy,
permission and seam family. Original `Tap::move` calls run explicitly before
the continuous slice rather than through the full callback's movement loop.

The recording manager is empty. No tape writes, input history, monitoring
producer, full speed/control producer, hardware or concurrency behavior is
executed. Both execution and reference use the reconstructed fade table.

## Reproduction and next work

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_boundary_pipeline.py
```

The next major integration gap is actual recording: join InputBuffer history,
resampling, feedback gathering, clipping and scatter to playback on the same
tape, respecting the full callback's read-before-write order. Then carry the
output through coloration and establish the remaining control/event and
hardware contracts. This is a substantially more connected playback model,
but remains short of a complete recreation specification.
