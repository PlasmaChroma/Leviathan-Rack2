# Write-fade movement, dispatch order and expiry

The original callback fade/record-dispatch/expiry slice passes 2,304 cases and
13,824 assertions. It observes 1,152 record calls, 612 lower expiries and 588
upper expiries; coverage is 465 instruction addresses. Record calls are explicit
inert observers here. This checks scheduling and lifecycle, not tape-write
arithmetic when the fade crosses table bounds.

## Reproduction and boundaries

```sh
python firmware/Lubadh/probes/probe_write_fade_expiry.py
```

Results: `probes/write_fade_expiry_probe_results.json`. The pinned main ELF
SHA256 is `2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
Original instructions execute from 0x48c58 through `Fade::move`, real record
manager `active_list`, per-head dispatch, expiry checks and resets, stopping
before manager updates at 0x48544. The `recordInput` boundary at 0x47818 records
its Tap and the current fade coordinates, then returns without changing state.
Supplied callback registers/stack, active fade and empty or fresh one-head
record pool isolate this graph. No appliance process runs.

## Motion law

Let F be supplied callback frames, A supplied motion factor in S21 and H the
looping-speed field at linked Preset+4. The checked movement amount is:

```
amount = f32(F) if H == 1 else f32(f32(F) * f32(A))
fraction = fmaf(step, amount, fraction)
normalize integer/fraction split coordinate
```

The matrix covers H=0/1, F=1/32/128, A=0/0.25/1/4, steps -2/-0.125/0.125/2,
initial integer positions -1/0/1/253/254/255 and fractions 0/0.75. Expected
movement uses explicit host glibc binary32 fmaf. Upstream publication and meaning
of S21 are not reconstructed by this suite; A is a supplied factor rather than
a measured physical speed. Initial out-of-range positions are branch fixtures,
not claims about user-reachable startup states.

## Recording precedes expiry

After movement, original `active_list` dispatches `recordInput` for each supplied
active record head before checking expiry. The observer verifies it sees the
post-move coordinates and an active channel fade, including out-of-range
coordinates. No-head fixtures dispatch nothing. The original record consumer
has separate crossing/envelope branches; observing its call does not establish
their arithmetic or safe table bounds.

Expiry checks **integer position**, after dispatch:

| Position | Executed behavior |
|---|---|
| Less than 0 | Reset channel fade and reset the entire record-head manager |
| 0 through 254 inclusive | Keep channel fade active and record head |
| Greater than 254 | Reset channel fade; preserve record head |

Fraction does not independently trigger expiry. Position 254 with a positive
fraction remains active; position -1 with fraction near 1 takes lower expiry.
The real fade reset clears active/current coordinate and restores step +1 in
the asserted fields. The lower reset clears the supplied record head; upper
reset leaves it active. Manager update/reclamation after this point is omitted.

## Connected interpretation and remaining work

The gate/state suites publish negative release and positive entry steps. This
callback adds their lifecycle: lower completion clears recording heads; upper
completion removes the envelope while retaining heads. Expiry is after that
block's record dispatch, so implementing the check before writing would change
observable behavior. The consumer arithmetic must still determine what the
crossing block writes.

Join event-produced fades, real recordInput crossing branches and tape writes,
multiple-head histories, record manager update and boundary scheduling before
claiming the complete overdub-envelope contract. Existing moving-write-fade
tests only check in-table endpoints. This suite does not silently extend that
claim to the out-of-table dispatch cases.
