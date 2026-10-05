# Pending first-record stops and capacity boundary

Original first-record movement and callback stop decisions pass 90 boundary
cases plus six persistent event-to-callback sequences, with 636 assertions and
774 instruction addresses covered. Pending and capacity stop paths include
real Channel event 0, FirstRec completion, full state/region/head consumers.
The slice stops before `recordInput`; it does not verify trailing audio writes.

## Reproduction and slice boundaries

```sh
python firmware/Lubadh/probes/probe_pending_first_record_stop.py
```

Results: `probes/pending_first_record_stop_probe_results.json`. The pinned main
ELF SHA256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
Execution begins at 0x48c04, after input-buffer publication, using supplied
callback registers and stack frame count. It moves the initialized active
first-record Tap with original `Tap::move` at speed +1 and frame count F, then
executes state/stop decisions. Stops at 0x48f1c (continue FirstRec) or 0x48f30
(after stop completion) leave actual input recording outside this test.
Original capacity-handler code at 0x48f50 also executes through completion.

Diagnostic strings/logger and semaphore services use the explicit inert
boundaries in `probe_channel_set_state.py`. A separate supplied Link pointer,
control fields, fresh managers and an embedded FirstRec/vtable fixture are
used. No complete callback, driver, worker or appliance process is launched.

## Move before checking the stop

For the initialized active Tap, each tested callback slice increases integer
counter Channel+744 by F. The checked F values are 1/32/128. The state label
through Channel+632 remains FirstRec (1) until a successful stop call changes
it. The decision uses this **post-move counter**, not its value at block entry.
Fractional/nonunit first-record motion is outside this suite; the original
call supplies +1 explicitly.

Let N be the post-move counter, M preset MinLength and P the pending flag at
Channel+604 (embedded FirstRec state+12). Below the capacity threshold:

```
if P != 0 and N > 4*M:
    Channel.interpretButtonPress(0)
    P = 0
else:
    continue FirstRec
```

This is stricter than the complete direct stop consumer in
`FIRST_RECORD_STOP_FINDINGS.md`, which accepts **N >= 4*M**. A pending request
therefore remains pending when the callback lands exactly on the threshold.
The callback clears its flag after the original event call; the complete stop
consumer also clears it during successful completion in these fixtures.

The direct boundary matrix covers MinLength 128/1,280, both pending flags and
six counter offsets around the threshold for each F. Six persistent sequences
start two blocks below threshold with P=0, execute the real FirstRec state event
7 to set P=1, then advance three original callback slices. The first remains
below, the second equals the threshold and retains FirstRec, and the third
exceeds it and completes to Playback. No new button event is needed for that
retry. At completion, stored length becomes the post-move counter plus 12,292.

## Capacity decision takes priority

Before checking P, the callback compares N unsigned against:

```
limit = 29,502,000 - 12,292 - 2*F
if N >= limit:
    Channel.interpretButtonPress(0)
```

The literal base 29,489,708 appears at 0x48ee4/0x48ee8. Eighteen fixtures check
limit-1/limit/limit+1 across F=1/32/128 and both pending flags. Equality takes
the capacity branch. It invokes the real completion consumer even with P=0.
Below capacity, only P=1 requests completion in these fixtures. The capacity
branch runs before the pending test; its completed state and stored length are
asserted, rather than inferred from observing the branch alone.

These are legitimate positive frame/counter fixtures; unsigned wraparound and
arbitrary invalid frame counts are not covered. The reserved trailing storage
and two-block margin are literal arithmetic, not a proof of all writes staying
within capacity. Actual tail recording and worker/storage synchronization are
still required for that claim.

## Callback consequence

Both successful stop paths rejoin the callback immediately before its original
`recordInput` call at 0x48f48. That is static scheduling evidence plus an executed
control-flow endpoint; the audio call itself is not executed here. In
particular, completion publication does not imply that the remainder of this
block or the 12,292-sample trailing region has already been stored.

Literal acceptance vectors can now distinguish direct equality acceptance,
pending equality deferral and capacity equality completion. Remaining recording
closure requires joining this decision slice to input/history/tape writes,
post-completion tail motion/reset, alias-aware linked fields and whole-callback
event ordering.
