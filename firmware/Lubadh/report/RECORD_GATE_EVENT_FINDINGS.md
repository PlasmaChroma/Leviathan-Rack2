# Record gate wrappers and overdub write-fade release

Complete original Channel gate events and three real state-method consumers
pass 216 cases and 16,416 comparisons, including 2,304 event-produced write-fade
moves. Coverage is 128 instruction addresses. This closes gate wrapper
publication and the overdub release fade law with a recorded `setState`
boundary. It does not close complete state transitions or gate-driven audio.

## Reproduction and boundaries

```sh
python firmware/Lubadh/probes/probe_record_gate_events.py
```

Results: `probes/record_gate_event_probe_results.json`. The pinned original main
ELF SHA256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
Complete `Channel::interpretButtonPress` (0x3f1c4) runs with event 4/5. Its
virtual call reaches original EmptyDeck (0x3a470), OverdubRec (0x3a4ec) or
Playback (0x3a5a0) methods through supplied state/vtable fixtures. Only
`Channel::setState` at 0x3a018 is intercepted to record requested labels and
return without changing state. No appliance process or logger runs.

The matrix covers state labels 0/2/3, gate rise/fall events, linked mode field
values 0/1/2, supplied arm flag 0/1, active/inactive write fade and fade durations
128/1,000/4,096. FirstRec label 1 is omitted because its stop path requires the
larger first-record transition/bookkeeping graph. Each case begins from a fresh
fixture; intercepted requests are not treated as completed transitions.

## Channel wrapper and state requests

| Channel event | Original publication and forwarding |
|---|---|
| 4 | Set Channel+651 gate to 1 and +645 indicator-dirty flag to 1; forward state event 6 |
| 5 | Clear Channel+651 gate to 0 and set +645 to 1; forward state event 7 |

The dispatcher in `JACK_EVENT_ROUTING_FINDINGS.md` emits event 4 for a RecordJack
role-1 rise. A falling RecordJack edge can emit event 5 when +651 is set. The
real wrapper now verifies the flag the producer checks; the complete producer
and state-transition feedback are still separate suites.

| Current state | State event 6 | State event 7 |
|---|---|---|
| EmptyDeck (0) | Request FirstRec (1) | Ignore |
| Playback (3) | Request OverdubRec (2) | Ignore |
| OverdubRec (2) | Ignore | Request Playback (3), publish release fade, clear Channel+124 |

On these paths the supplied arm flags +636/+637 remain unchanged by the
executed wrappers/methods. `setState` itself can modify these and many other
fields, so that observation must not be extended to a complete transition.
Mode at Link+132 does not alter these gate-specific branches in the matrix.

## Event-produced write fade

OverdubRec event 7 requests Playback first, then reads the write-fade object at
Channel+6504. If inactive, original `Fade::activate(254)` (0x4ba80) makes it
active, sets integer current/previous positions to 254, fractions to zero and
temporarily sets step to 1. If already active, its current and previous
coordinates are preserved. Both paths then execute original
`Fade::set_step` (0x4baf4):

```
step = -f32(f32(256) / f32(signed Link[100]))
Channel[124] = 0
```

Link+100 supplies the duration here. The 256 numerator and 254 start position
are distinct constants; an adapter should not silently replace one with the
other. Durations are valid positive supplied fixtures, not a recovered universal
range or sample-rate policy. The code requests `setState` before fade setup;
because that dependency is intercepted, this suite does not prove that the
real transition leaves all fixture state untouched before fade publication.

Each overdub-release case carries an independently computed fade state through
64 original `Fade::move` calls (0x4bafc), with frame counts 1/7/32/128 repeated
16 times. The reference uses explicit host glibc binary32 fused multiply-add:

```
previousCoordinate = currentCoordinate
fraction = fmaf(step, f32(frames), fraction)
while fraction >= 1: fraction = f32(fraction - 1); integer += 1
while fraction < 0: fraction = f32(fraction + 1); integer -= 1
```

Comparisons are exact for the exercised fixtures. Moves include negative
coordinates. This complete movement consumer does not clamp to zero, clear the
active flag or end recording. Therefore an implementation cannot infer expiry
from `Fade::move` alone: outer callback expiry and out-of-table record-envelope
branches still require connection. The existing `moving_write_fade` probe
checks tape-write arithmetic only while supplied endpoints remain in-table.

## Remaining contract

Execute the real `setState` graph, FirstRec stop behavior, linked producer
feedback and callback write-fade expiry/tape writes before claiming a complete
gate/arm/record contract. This result supplies concrete release-fade vectors
and transition requests for that integration, not an end-to-end parity claim.
