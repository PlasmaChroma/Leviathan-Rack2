# Channel state publication and connected gate round trips

Complete original `Channel::setState` passes 882 fixtures. A further 36 gate
round trips connect the real Channel wrapper, virtual state consumer, full
`setState`, original head managers and write fade. The combined suite passes
9,036 counted assertions and covers 807 original instruction addresses. This
supersedes the recorded `setState` boundary for these specific gate histories;
FirstRec stopping, arbitrary head-pool histories and callback audio remain open.

## Reproduction and explicit services

```sh
python firmware/Lubadh/probes/probe_channel_set_state.py
```

Results are in `probes/channel_set_state_probe_results.json`. The pinned main
ELF SHA256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
The entire function at 0x3a018 executes, including its nested original tap
activation/reset, allocation and fade operations. Supplied fixtures include
link/preset pointers, embedded state/vtables and worker/status pointers.

Diagnostic string helpers at 0x2ebc8/0x2e8ec/0x36fa8/0x36ef8 supply valid empty
SSO strings. Diagnostic append at verified PLT 0x15e88 and logger at 0x6ff20
are inert. Original string lifetime/control instructions execute around these
services. Their symbol identities are asserted; diagnostic text and allocation
semantics are not fidelity claims. `Semaphore::post` at 0x70820 is recorded and
returns without a worker or operating-system call. No appliance process runs.

## Common state and arm publication

At entry, when Link+132 equals 2, `setState` forces Channel+636 to 1 and stores
the old +636 value XOR 1 at +637. Other supplied mode values retain both flags.
The matrix checks mode 0/1/2 with old flag 0/1. The state pointer at Channel+632
then selects the following embedded object:

| Requested label | State | Channel offset | Status byte through Channel+171868 |
|---|---|---|---|
| 0 | EmptyDeck | +580 | 1 |
| 1 | FirstRec | +592 | 1 |
| 2 | OverdubRec | +608 | 0 |
| 3 | Playback | +620 | 0 |

The status byte is a checked publication, not a recovered physical indicator
name. FirstRec entry activates its separate Tap at Channel+740 at position 0
and sets step +1. Playback entry changes the state/status without resetting the
existing supplied playback head. The simple-state matrix has 18 fixtures;
it does not cover active FirstRec re-entry or all preexisting object histories.

## Entering overdub

The 864 overdub fixtures cover live speed -1/0/+1, mode 0/1/2, arm flag 0/1,
variable/fixed recording, one existing playback head or none, held recording
flag 0/1, active/inactive write fade and durations 128/1,000/4,096.

Original playback-manager `any_active` and `active` determine the record head
position. With an existing head, activation copies its integer position
(3,456 in these fixtures). With none, variable reverse uses Link+32 (9,000),
while forward/zero or fixed recording uses Link+20 (1,000). It activates a new
record-manager head at Channel+3688 with Link+100 duration and direction equal
to live speed >=0. Fixed recording additionally sets the returned head's stored
step to +1. The fresh tap already has stored step +1, so this matrix cannot
prove that distinction from the final step alone; the static fixed branch is
at 0x3a3f8 and executes in the fixed fixtures.

If the write fade at Channel+6504 is inactive, entry activates it at index 0,
zeroing current/previous fractions. An active fade retains both coordinates.
Both paths publish positive `f32(256 / f32(signed Link[100]))` step.

When Link+152 equals 1, entry sets Channel+124 to 1 and stores a write anchor at
Channel+120. The anchor is activated position +1 for variable reverse and -1
for every other tested direction/record mode. When that flag is zero, the
supplied +120/+124 fields remain unchanged. This checks publication; the
downstream held-boundary/write consumer is a separate integration.

Record pools start empty and can allocate successfully. Failure/exhaustion,
multiple existing playback engines and manager stealing histories remain open
for the complete transition graph, despite separate allocation probes.

## Empty state and asynchronous erase

The complete nested `AudioData::erase` runs through its original publication
and bookkeeping, with only diagnostics and semaphore post supplied. It sets
the worker request byte through AudioData+8 and posts AudioData+40, then clears
the pointed length at Channel+88. `setState(EmptyDeck)` resets the separate Tap
and both head managers; the fixture checks every active slot flag clears.

This is an asynchronous request contract. The background worker and completion
ordering are not executed, and this test makes no claim that tape storage is
already zero after the request. Additional erase metadata writes are visible
in the executed function but are not all independently asserted in this suite.

## Connected gate feedback

Thirty-six fresh fixtures begin in OverdubRec with an existing playback head.
Channel event 5 reaches original OverdubRec event 7 and complete `setState(3)`;
the state becomes Playback, gate flag +651 clears and release-fade step becomes
negative. An inactive fade starts at 254; an active fade retains its supplied
coordinates. Channel event 4 then reaches real Playback event 6 and complete
`setState(2)`, activates the record head at the existing playback position,
sets the gate flag and makes fade step positive while retaining its position.

Thus a gate close/open without intervening motion reverses the envelope rather
than restarting it. These cases replace the state-request-only evidence in
`RECORD_GATE_EVENT_FINDINGS.md` for this round trip. They do not execute the
jack producer, intervening fade movement, FirstRec stopping, linked feedback,
boundary callbacks or tape writes. Those contracts must still be joined before
claiming a complete record/gate parity specification.
