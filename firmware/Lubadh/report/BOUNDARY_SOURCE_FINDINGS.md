# Boundary producers and linked source selection

The original-byte probe `probes/probe_boundary_sources.py` connects loop-region
calculation to the callback's actual boundary-loading instructions. It passes
54 configurations, each with four consecutive region updates and callback
loads: **2,160 field comparisons**, 648 constructor/reset assertions and seven
link-pointer/source assertions. It visits 184 distinct instruction addresses.
The ELF hash and every fixture are in `boundary_source_probe_results.json`.

## Inline fields remove a presumed publication step

The constructor advances its Channel pointer by 36 at `0x3cd90`, constructs
LinkData there, then stores that address in Channel+0xe8 at `0x3cda4`. The probe
executes the last pointer-store slice; the preceding constructor relationship
is established statically, rather than by executing complete construction.

For an unlinked channel, LinkData is therefore **Channel+0x24**. These are aliases:

| LinkData offset | Channel offset | Callback interpretation |
|---|---|---|
| 0x14 | 0x38 | Selected start |
| 0x18 | 0x3c | Start plus previously stored fade |
| 0x1c | 0x40 | Selected end |
| 0x20 | 0x44 | End plus newly calculated fade |
| 0x24 | 0x48 | Physical reverse threshold |
| 0x28 | 0x4c | Logical period / forward physical seam |
| 0x2c | 0x50 | Extended end / reverse physical destination |
| 0x34 | 0x58 | Stored-length pointer |

There is no additional copy required to publish these particular fields in
the unlinked path. The probe executes full `setLoopingParameters` and then
callback instructions `0x47c3c` through `0x47ca8` on the same object. Periods
49,170, 16,777,219 and 29,500,000, start/length controls 0/2048/4095, and fade
controls 0/4095 cover ordinary and wrapped endpoints. Extent and stored-length
values are initialized recording fixtures, not the result of a recording
session. The physical reverse threshold comes from executed `resetLoop`.

The previous endpoint lag is real on this alias path: a call calculates end
using the old length, and reverse start using the old fade. Four consecutive
calls verify settling; calls three and four agree. This is a per-call result,
not an assertion about elapsed time or IO/audio synchronization.

## Reset and link selection

Executed `resetLoop` sets start and reverse start to 1; end and reverse end to
29,501,997; logical period and extended end to 29,501,998; physical reverse
threshold to **2,459**; maximum fade span to 12,292; selected length and fade
to 1; and the pointed-to stored length to zero. Earlier wrapped tests supplied
small thresholds explicitly. Their threshold 32 is not this reset default.
This probe does not prove the threshold can never change elsewhere.

Application layout places the left Channel at Application+72 and the right
Channel at Application+173440. The link-enable pointer store at `0x27dac`
points right Channel+0xe8 to **left Channel+36**. The unlink slice
`0x27e10..0x27e20` restores **right Channel+36**. The probe executes those stores
and runs the right callback's boundary loads against deliberately distinct
left/right fields, verifying the selected source changes accordingly.

These are direct shared-field reads, not a field-by-field copy into the right
channel. They do not establish every linked control's behavior: link-event
eligibility, surrounding state synchronization and complete `processSpeed`
execution remain open.

## Reproduction and remaining work

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_boundary_sources.py
```

This narrows the earlier reports' “boundary publication” gap. Callback
active-list iteration, new-head participation, simultaneous recording/playback,
nonstationary speed ramps and final output processing remain unvalidated.
The test executes neither hardware initialization nor the complete callback,
and provides no concurrency proof between IO updates and audio reads.

Subsequent [Head Iteration Findings](HEAD_ITERATION_FINDINGS.md) validate
per-manager boundary snapshots and render-entry refreshes, narrowing the
active-list gap. Complete callback movement, recording and audio remain open.
