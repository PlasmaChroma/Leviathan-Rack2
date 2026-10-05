# First-record stopping and connected completion publication

Complete original `FirstRecMode::interpretButtonPress` passes 960 fixtures,
including 432 completions and 144 too-short stop requests. The suite checks
8,688 assertions and covers 588 instruction addresses. Unlike the earlier
tail-bookkeeping slice, it executes eligibility, full `setState`, original
region calculation and playback-head activation/fade consumers together.
Counter growth and actual trailing audio writes remain outside the suite.

## Reproduction and scope

```sh
python firmware/Lubadh/probes/probe_first_record_stop.py
```

Results are in `probes/first_record_stop_probe_results.json`. It uses the pinned
main ELF SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
The complete method at 0x3a6ac and its original DSP/state dependencies execute.
The explicit diagnostic and semaphore services are inherited from
`probe_channel_set_state.py`; diagnostic append/move imports are verified
against the archived PLT map and supply empty valid strings. No worker,
hardware process or operating-system service runs.

Fixtures supply MinLength 128/1,280 and first-record counters at four times that
value minus 1, equal, plus 1, or plus 20,000. They cover state events -1/0/1/6/7,
Link+132 modes 0/1/2, old arm flag 0/1, linked live speed -1/+1 and Link+140
values 0/1. State event numbers are consumed directly; physical button/jack
producers are not called here. Control fields and a fresh head pool are supplied.

## Stop eligibility

State events 0, 1 and 7 are stop requests. Other supplied events leave the
fixture in FirstRec. Let N be the signed first-record integer counter at
Channel+744 and M the signed preset MinLength at Preset+48. The checked law is:

```
if N < 4*M:
    FirstRecState[12] = 1
    retain FirstRec and existing length state
else:
    FirstRecState[12] = 0
    setState(Playback)
    publish completion
```

Equality is accepted. The multiplication is an original integer left shift
followed by signed comparison; the positive fixture values avoid overflow.
This is an event-consumer threshold in samples, not a measured time threshold.
The too-short flag's downstream callback handling remains to be traced.
Do not interpret a rejected request as automatic audio stopping.

## Connected completion

Accepted requests execute real `setState(3)`, selecting Playback at Channel+620,
before original completion bookkeeping and forced `setLoopingParameters`.
The fixture independently checks the region through the existing region model,
with supplied controls, no quantization and division count 4. Completion first
publishes:

| Field | Checked value |
|---|---|
| Channel+76 logical period | N |
| Channel+80 end marker | N + 2,458 |
| Channel+132 maximum fade span | min(N - 1, 12,292) |
| Length pointed to by Channel+88 | N + 12,292 |

Forced region setup uses the newly published period/end marker/fade limit, the
supplied linked span and the preceding selected region length/fade. Its exact
field output matches `probe_loop_regions.model`, including those old-field
dependencies. This connects bookkeeping to region publication rather than
assuming the selected region immediately equals N.

Completion resets target speed Channel+44, current Channel+728 and pre-speed
Channel+36 to +1, clears the speed increment and remaining count at Channel+736.
The suite directly asserts target,
current, increment/count and V/oct direction Channel+236=0; all fixtures use
SpeedControl=3. Pre-speed and other stores are executed but not independently
asserted here. Avoid expanding the assertion count into a claim about every
field written by the function.

When supplied Link+140 is zero, it activates a playback head at Link+20 for
positive linked speed or Link+32 for negative linked speed. The original call
passes direction **true** in both cases and the supplied fade duration 128.
The further original playback-step/fade operations execute. Their complete
numerical state is not independently asserted in this suite. When Link+140
equals 1, this activation is skipped. At the end, status-object+4 receives the
stored N+12,292 length.

The Link pointer is deliberately separate from the current Channel's embedded
fields. Therefore linked live speed can remain negative after this Channel's
speed reset. This fixture exposes a branch; it does not establish that every
unlinked completion can take it. Alias-aware link publication and two-deck
completion histories remain integration requirements.

## Specification consequence and remaining work

The previous description of MinLength as only a selected-region floor is now
incomplete: this original event path also uses four times MinLength as its
first-record stop threshold. That law must appear in literal acceptance
vectors. A native design can use a different explicit minimum recording
duration, but should document the changed behavior and units.

Completion still needs callback counter growth, pending-short-stop handling,
tail write scheduling, full-buffer limits, linked alias histories and physical
gesture priorities. Publishing N+12,292 storage length does not prove those
12,292 trailing samples have been written. Import/export tail policies remain
separate contracts.
