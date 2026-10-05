# GPIO edges and two-deck jack event routing

The complete original `Application::interpretJacks` matches an independent
ordered-event model over 20,224 cases and 94,464 recorded event calls. Of those
cases, 4,096 join the original six-input GPIO publication slice and complete
`Button::interpretGPIO` to the dispatcher. Coverage is 205 original instruction
addresses. This verifies producer routing with inert event consumers, not the
connected recording/clock state machine.

## Provenance and reproduction

`probes/probe_jack_event_routing.py` uses the pinned main ELF with SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
It executes the complete dispatcher at 0x284b0. Its boundary at
`Channel::interpretButtonPress` (0x3f1c4) records deck/event and returns without
mutating state. This deliberate boundary makes the producer independently
testable; consumer side effects could alter later decisions in real execution.

```sh
python firmware/Lubadh/probes/probe_jack_event_routing.py
```

Results and simultaneous-edge traces are in
`probes/jack_event_routing_probe_results.json`. The direct-routing matrix covers
every combination of eight edge flags, every valid RecordJack role pair and
both link states (4,608 cases). A further 11,520 cases vary both decks' supplied
one-shot mode, one-shot flag and stop flag at representative edge combinations.
The joined GPIO matrix supplies every previous/current combination of six
binary inputs, with asymmetric roles (A clock, B latching record).

## GPIO publication

The callback slice starts at 0x295cc and stops before 0x29628. It calls original
`Button::interpretGPIO` (0x36640) for three jack objects per deck. Only
`LubadhHardware::readPin` at 0x6c0c4 is replaced with a synthetic binary service.
Pin identifiers are supplied fixtures, not recovered electrical pin numbers.
Decks begin at Application+72 and Application+173440; work is Channel+172032.

| Read order within each deck | Pin field relative to work | Button object relative to Channel | Dispatcher edge fields |
|---|---|---|---|
| First | +0x485 | +0x1b0 | +0x1bd rising (event 2 path) |
| Second | +0x489 | +0x1a0 | +0x1ad rising (event 1/23 path) |
| Third | +0x487 | +0x190 | +0x19d rising / +0x19e falling (RecordJack role) |

All three A inputs are read before all three B inputs. The complete Button
consumer stores the current binary level at object+4 and object+8, publishes
rise at +13 only for 0-to-1 and fall at +14 only for 1-to-0, and clears both
edge flags on repeated levels. There is no debounce/time threshold in this
consumer. Analog thresholds, hardware polarity and pulse capture between
callback reads remain unmeasured. The dispatcher does not clear its edge fields;
the next GPIO publication replaces them.

## Ordered dispatcher contract

The dispatcher visits A then B. Within a deck it handles RecordJack rise,
RecordJack fall, the +0x1ad rise, then the +0x1bd rise. Consumer calls retain
their exact order, including duplicates when simultaneous linked inputs cause
the same destination/event to be dispatched more than once.

For ordinary local-then-peer forwarding below, the peer receives a call only
when the supplied link flag at Application+344064+0xabc is enabled.

| Input / condition | Checked emitted event order |
|---|---|
| RecordJack rise, role 0 (latching record) | Event 0 local, then peer if linked |
| RecordJack rise, role 1 (gated record) | Event 4 local, then peer if linked |
| RecordJack rise, role 2 (clock), unlinked | Event 11 local; if Channel+172 is set, clear it and then event 0 local |
| RecordJack rise, role 2, linked A | Event 11 B then A; if A+172 is set, clear both +172 flags and then event 0 A then B |
| RecordJack rise, role 2, linked B | Event 0 B then A; no event 11 and no producer one-shot clearing |
| RecordJack fall, unlinked | Event 12 local |
| RecordJack fall, linked A | Event 12 B then A |
| RecordJack fall, linked B | No event 12 |
| RecordJack fall, Channel+651 set | Additionally event 5 local, then peer if linked, after the event 12 decisions |
| +0x1ad rise | Event 23 when local linked Preset+8 equals 1, otherwise event 1; local then peer if linked |
| +0x1bd rise | Event 2 local, then peer if linked |

The preset role is read through Channel+232 then Link+176 then Preset+24.
Preset+8 is a separate supplied mode field; the fixture labels it one-shot
mode, consistent with the preset layout, and does not use it as RecordJack
role. Do not combine these fields in a Rack adapter. Event 11's static consumer
branch sets Channel+173 then calls `tempoClock`; event 23 statically calls
`killTaps(true)`. Those consumers do not execute in this routing suite.

The one-shot flag at Channel+172 is independently modeled because linked A
can clear B's flag before B is processed. Other state changes are suppressed
by the inert boundary. The literal A-before-B traversal alone is therefore
not proof that every real simultaneous event has these same downstream effects.

## Callback ordering and remaining connection

Static `runIO` (0x29594) reads buttons, publishes jack GPIO levels, reads ADCs,
applies both speed-pot calls, both looping-parameter calls and both Time calls,
then calls `interpretJacks`, `interpretButtons`, and finally `interpretLink`.
This ordering is visible in the original listing; the complete `runIO` graph
is not executed here. A simultaneous link-switch change may consequently use
the previous link state during jack/button routing. Its complete publication
and connected behavior need verification before specifying that transition.

Specification closure still requires real consumer feedback during these
ordered calls, gate/arm/state transitions, link/unlink histories, button chord
priority, and connected audio/output-pulse scheduling. The binary input
recurrence can already supply deterministic acceptance vectors for a native
adapter; voltage thresholds and improved sample-accurate event capture must be
explicit native policies rather than claimed hardware measurements.
