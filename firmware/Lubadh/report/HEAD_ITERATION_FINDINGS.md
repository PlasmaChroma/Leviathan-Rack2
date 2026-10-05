# Callback head snapshots and render-list refresh

`probes/probe_head_iteration.py` executes the original callback's complete
boundary iteration for one TapManager, starting at `0x47ed4` and stopping
before `0x47fd4`. It then executes the callback's render-entry list refresh
at `0x47fe4..0x47ff0`. Nested allocation, fade, transfer and movement routines
execute as original bytes. **144 configurations pass 1,584 assertions**, with
1,230 distinct instruction addresses covered. Results and all fixtures are
in `probes/head_iteration_probe_results.json`.

## Snapshot behavior

`TapManager::active_list` returns the manager-owned 20-pointer scratch buffer
at Manager+2736. It copies five-entry slot-age lists from engines in their
manager age order, compacts nonzero pointers to the front and fills the rest
with zeros. It preserves both source engine and slot-age arrays.

With allocation-produced age lists, the resulting traversal is newest engine
first, then newest head first within each engine. It is not physical slot
order. The probe explicitly checks that order before invoking the callback.

The callback takes this snapshot once for the manager's boundary pass. The
probe observes the head pointer at `0x47f44` for every nonzero entry. Exactly
the original heads are visited, in snapshot order. Direct allocations modify
the engine's source slot-age list but leave the 80-byte scratch snapshot
unchanged. Newly allocated heads are consequently **not boundary-checked
again in that pass**.

The render-entry call to `active_list` refreshes the same scratch buffer.
It includes all new heads, places them before the older heads in their engine,
and preserves the manager engine ordering in these fixtures. This establishes
their eligibility in the refreshed render list; this probe does not execute
audio rendering or compare the resulting samples. Earlier raw splice tests
iterated physical slots explicitly, so their summation order should not be
treated as the callback's traversal order.

## Tested occupancy and transitions

The fixtures use one through four occupied engines, each with one through
five contiguous slots, plus four uneven four-engine occupancy patterns.
Original constructors and activation routines produce every initial age list.
Each original head is moved by the original `Tap::move` before the boundary
loop; movement is explicit setup rather than the complete callback's earlier
movement phase.

Ordinary forward and reverse regions test replacement permission both enabled
and disabled. Each outside original head attempts one direct replacement when
permitted. Incoming heads consume free slots in traversal order. Once an engine
fills, subsequent attempts return no new head; the original snapshot still
finishes normally.

A reduced reverse-start wrapped fixture tests two transitions in each original
head iteration: physical reverse seam followed by selected-loop seam. With
replacement permission enabled there are two allocation attempts per old head;
with permission disabled the physical transition still attempts allocation.
New heads remain absent from the ongoing boundary snapshot in both cases,
including when the first transition consumes the final slot. These small
coordinates are explicit branch fixtures, not a demonstrated normal recording
configuration or reset defaults.

For initial occupancy `n`, the checked final slot count in each engine is:

```text
ordinary:          min(5, n * (1 + replacement_permission))
reverse double:    min(5, n * (2 + replacement_permission))
```

These formulas describe the tested outside-head fixtures, not arbitrary heads
with pre-existing fades or heads inside their selected region.

## Reproduction and remaining integration gaps

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_head_iteration.py
```

Still open: full callback movement and update order, deactivation and sparse
age-list integration, simultaneous record/playback managers, audio rendering
in actual age order, overlap suppression, nonstationary speed ramps and final
output normalization/coloration. The complete appliance and hardware events
are not executed. This evidence supplies a recreation scheduling contract for
the checked boundary phase, rather than complete instrument fidelity.

Subsequent [Render Iteration and Record Proximity](RENDER_ITERATION_AND_RECORD_PROXIMITY.md)
executes raw audio rendering in actual age order, speed ramps and recording-head
attenuation. Complete boundary/write/render integration remains open.
