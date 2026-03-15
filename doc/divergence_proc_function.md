# Proc vs FUNCTION Divergence Notes

This note compares the current `Proc` implementation against the local Make Noise FUNCTION manual in [doc/FUNCTIONmanual.pdf](/mnt/c/msys64/home/Plasm/Leviathan/doc/FUNCTIONmanual.pdf).

Reviewed manual sections:
- page 5: panel controls, trigger behavior, CV descriptions
- page 6: hang, LEDs, EOR/EOC, output descriptions
- page 7: overview and stated module intent
- pages 10, 14, 15: patch behaviors and mode descriptions

## Summary

`Proc` is close to FUNCTION in broad feature set:
- signal input
- trigger input
- hang/halt input
- cycle control
- rise / fall / both CV inputs
- variable response control
- EOR and EOC outputs
- non-inverted and inverted outputs
- 0-8 V free-running function output

It is not yet a strict behavioral match. The main divergences are in the active DSP behavior during triggered/cycling operation and in the Rise/Fall CV law.

## Divergences

### High: Signal input still influences function-generator mode

Manual basis:
- page 5 says Trigger Input produces a `0V to 8V function` regardless of activity at Signal Input
- page 14 describes triggered transient behavior as a self-contained rise-to-8V then fall-to-0V event

Current `Proc` behavior:
- when the channel is actively rising or falling, a patched `Signal Input` is still mapped into the internal FG domain
- that input then perturbs the running FG state through `injectAlpha`

Relevant code:
- [src/Proc.cpp:675](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:675)
- [src/Proc.cpp:687](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:687)

Impact:
- triggered or cycling behavior is not isolated from Signal Input the way the manual describes
- this is the largest functional mismatch if the goal is a faithful FUNCTION clone

### Medium: Rise/Fall CV law is log-time based, not obviously linear

Manual basis:
- page 5 describes Rise CV and Fall CV as `Linear control signal input`
- positive CV increases stage time, negative CV decreases it

Current `Proc` behavior:
- Rise/Fall CV is first soft-clamped
- it is then converted into octave scaling in log-time space before being applied to stage time

Relevant code:
- [src/Proc.cpp:493](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:493)
- [src/Proc.cpp:496](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:496)

Impact:
- polarity matches the manual
- exact response law does not literally match the manual wording
- this may or may not be acceptable depending on whether the goal is panel-faithful or behavior-faithful emulation

### Low: Panel/UI semantics are close, but not a literal match

Manual basis:
- page 6 refers to `Hang Input`
- page 6 describes an activity LED and a combined EOR/EOC LED state description

Current `Proc` behavior:
- uses `Halt CV` naming in code
- exposes separate EOR and EOC lights
- also exposes separate output lights for non-inverted and inverted outputs

Relevant code:
- [src/Proc.cpp:778](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:778)
- [src/Proc.cpp:1166](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:1166)

Impact:
- this is mostly naming and panel-language divergence
- core capability is present, but the visual semantics are not a strict copy of the manual

## Areas That Match Well

### Core control and output set

`Proc` includes the expected FUNCTION-style core interface:
- signal
- trigger
- hang/halt
- rise CV
- both CV
- fall CV
- cycle
- vari-response
- EOR
- EOC
- non-inverted output
- inverted output

Relevant code:
- [src/Proc.cpp:17](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:17)
- [src/Proc.cpp:774](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:774)

### Free-running function range

Manual basis:
- pages 5, 6, and 14 describe the generated function as `0V to 8V`

Current `Proc` behavior:
- free-running FG range uses `FG_V_MAX = 8.f`

Relevant code:
- [src/Proc.cpp:152](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:152)
- [src/Proc.cpp:700](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:700)

### Hang/Halt behavior

Manual basis:
- page 6 says Hang stops the circuit `dead in its tracks`

Current `Proc` behavior:
- if halt is high, new trigger/cycle starts are blocked
- the routine returns early before further integration/slew progress

Relevant code:
- [src/Proc.cpp:527](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:527)
- [src/Proc.cpp:660](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:660)

### Retrigger during falling

Manual basis:
- page 14 states the transient is retriggerable during the falling portion

Current `Proc` behavior:
- triggers are accepted when not already in `OUTER_RISE`
- this allows retrigger in `OUTER_FALL` and from idle

Relevant code:
- [src/Proc.cpp:534](/mnt/c/msys64/home/Plasm/Leviathan/src/Proc.cpp:534)

## Suggested Next Fixes

If the target is a more faithful FUNCTION implementation, the next changes should be:

1. Remove or redesign Signal Input injection during active triggered/cycling function-generator mode.
2. Decide whether Rise/Fall CV should be recalibrated away from the current log-time law.
3. Decide whether panel semantics should move closer to the manual wording and LED behavior.
