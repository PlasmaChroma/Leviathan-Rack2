# Connected first-record completion, trailing writes and reset

Eight persistent sequences pass 5,296 original callback slices and 99,476
sample writes, with exact agreement against an independent tape/history model.
Coverage is 1,138 original instruction addresses. The 106,172,300 comparisons
include all 20,000 tape cells after every block, including untouched cells;
that count describes coverage, not whole-instrument fidelity.

## Reproduction and scope

```sh
python firmware/Lubadh/probes/probe_first_record_tail_writes.py
```

Results: `probes/first_record_tail_write_probe_results.json`. The pinned main
ELF SHA256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
The original callback slice executes from 0x48c04 through movement, pending
stop/completion, tail recording or Tap reset, ending before 0x47dc0. Complete
`recordInput` at 0x47818 executes on the callback's own control-flow path.
Original `InputBuffer::input` at 0x38a78 publishes history separately before
each slice. State/region/head consumers execute with inherited explicit inert
diagnostic and semaphore boundaries. No appliance process runs.

Fixtures use MinLength 128/1,280, frames 7/31/32/128 and fixed +1 first-record
motion. The initial counter is two blocks below four times MinLength. A real
FirstRec state event 7 sets the pending flag before the first block. The
separate Link/control fixture and fresh managers match the preceding stop
probes. The tape contains 20,000 supplied zero cells. Samples before the
supplied initial counter are deliberately absent. Zero feedback, original
transparent knee=1/compensation=0 write clipping and the reconstructed fade
table isolate scheduling; full record coloration is not modeled.

## Completion block still records

The first block remains below four times MinLength, the second equals it and
the third exceeds it. The third executes real event-0 completion, becomes
Playback, publishes stored length N+12,292 and still calls `recordInput` with
the first-record Tap in that same slice. Each unit-speed write includes the
previous integer position and excludes the newly reached one.

The reference independently carries four-sample input history, tape, counter,
state and stored length. Checks cover every emitted index, input phase, Tap
active/counter fields, state pointer, record-call arguments and every tape cell
after each block. Expected state is not read from the executed post-move Tap.
Input repeats 97 distinguishable binary32 values; at integer phase the expected
write is the corresponding history-buffer sample. Output rendering is absent.

## Tail decision and reset

After completion the first-record Tap continues to move at +1. The callback
compares its post-move counter against stored length supplied in the callback
register:

```
moveFirstRecordTap(+1, frames)
if postMoveCounter <= storedLength:
    recordInput(firstRecordTap, factor=1, feedback=0)
else:
    reset(firstRecordTap)
    skip this block's recordInput
```

Equality records. Strictly exceeding length executes original Tap reset at
0x4bc50, clearing active flag and counter and bypassing that block's recording.
The trace ends there; it does not force later slices after the outer callback
would stop selecting the inactive Tap. Stored length remains N+12,292.

There is no partial-block write to fill the remainder. For these constant-frame
fixtures, the unwritten suffix is `12,292 modulo frames`:

| Frames | Unwritten cells within published stored extent |
|---|---|
| 7 | 0; exact end equality is written |
| 31 | 16 |
| 32 | 4 |
| 128 | 4 |

The suffix remains zero because fixtures start with zero tape. Content in reused
or asynchronously erased storage is unestablished. Published storage extent
and samples actually written consequently need distinct specification fields.

## Remaining closure

First-record tail motion, connected writes and reset now have executable
acceptance vectors. Still join full input effects/AntiAlias and output rendering,
startup recording from zero, changing block sizes, linked aliases/events,
background erase histories and capacity-near storage. This suite stays far from
physical capacity and does not establish import/export tail policy. A native
adapter that fills the partial final block deliberately changes this literal
behavior and should document the improvement.
