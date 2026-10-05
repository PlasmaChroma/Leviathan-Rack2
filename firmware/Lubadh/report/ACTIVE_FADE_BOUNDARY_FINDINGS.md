# Active fade retriggering and independent boundary audio

The decomposition now checks already-active fade retriggering and joins the
independent fade-state model to ordinary boundary allocation, rendering and
manager cleanup. These are original-byte tests with explicit inert service and
callback-local fixtures, not appliance execution.

## Already-active triggering

```sh
python firmware/Lubadh/probes/probe_active_fade_retrigger.py
```

All 720 cases pass, checking 95,040 Tap bytes; coverage is 441 instruction
addresses. Results: `probes/active_fade_retrigger_probe_results.json`.
The complete trigger_fade at 0x4e46c runs with existing active phases
-1/0/31/254/255, prior steps -2/0/+2, durations 32/128/1000, all four fade kinds
and both boolean combinations. The supplied target is outside the current
integer interval.

**An already-active selected fade is reinitialized.** The branch at 0x4e7c0
constructs a diagnostic and calls the logger once, then branches back to 0x4e4ac
and executes the same trigger initialization as an inactive fade. It does not
preserve phase, return early or ignore the request. Every selected fade field
is independently predicted from the recovered trigger equations; all other Tap
bytes are checked unchanged. Existing phase and step do not affect that
noncrossing reinitialization.

Diagnostic numeric-string and replacement services return valid empty SSO
objects. Diagnostic append and logger calls are inert, while surrounding original
string movement/destruction instructions execute. Their symbol/PLT identities
are asserted by the harness. The logged text and appliance logger timing are not
tested. Zero duration and repeated crossing targets remain outside this suite.

## Boundary-produced fades through audio and cleanup

```sh
python firmware/Lubadh/probes/probe_boundary_fade_model.py
```

All 160 cases / 240 head renders pass with 21,120 comparisons. Maximum raw
gain/mix discrepancy is 2.510718088988284e-8; coverage is 1,469 instruction
addresses. Results: `probes/boundary_fade_model_probe_results.json`.

Original boundary iteration from 0x47f44 to before 0x47f10 executes real fade
triggering, replacement allocation, transfer and movement. Its call list and
replacement coordinates are compared to the independent ordinary-region model.
The expected calls and expected source/replacement head coordinates then drive
the independent trigger equations. All four fade states on each active head are
compared before original raw rendering from 0x48048 to before 0x48398. Independent
fade states, speed gain, head coordinates and cubic tape samples predict each
head's gain and accumulated mix. Expected rendering inputs are never refreshed
from executed fade endpoints or head positions.

Complete TapManager::update then executes. The reference predicts below-zero
whole-head reset and above-254 individual fade reset and checks all fade fields
plus head active flags after update. This joins state generation, audible splice
and cleanup in one test rather than treating original state as a render oracle.

Fixtures use ordinary region 100..500 with reverse boundaries 132/532, effective
speeds -1/-0.125/0/+0.125/+1, following or held speed (opposed live speed when
held), fractions 0/0.375, 7/32 frames, 32/128 fade durations and replacement
enabled/disabled. A held zero fixture now correctly preserves zero in the shared
boundary helper; its former `held or 1` expression replaced zero with one. This
is an offline fixture correction, not a firmware or module behavior change.
The existing splice-render regression was rerun: 128 cases / 9,672 comparisons
pass with maximum discrepancy 2.8711464317154878e-8.

The original main ELF remains pinned by the shared harness to SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
Link/stack publication, full callback scheduling, persistent boundary reentry,
physical/wrapped seams, saturation, simultaneous fade kinds, recording-boundary
interaction and whole output coloration remain separate integration work.
Stationary out-of-table states omitted in the earlier supplied-trigger suite
are not resolved by these ordinary boundary fixtures; their production
reachability and Rack policy still require evidence.
