# Speed modulation publication: static contract and pending joined probe

**Validation status: static decomposition only; the new joined probe has not
executed.** Automatic approval review rejected the WSL launch because the app
refresh token was revoked. No result JSON or passing comparison count is claimed.
Previously passing factory/scalar flutter probes retain their existing scope.

## Buffer publication

The archived main disassembly identifies the complete buffer-size path:

- `AudioEngine::set_buffer_size` at 0x36610 stores its frame count at engine+140
  and calls `Channel::setBufferSize` for both decks (application+72 and
  application+173440), forwarding the same unsigned count.
- `Channel::setBufferSize` at 0x3b438 resizes the flutter noise vector at
  Channel+25116 (Flutter+6156) to that count. Comparison occurs at
  0x3b51c–0x3b538; shrink adjusts the end pointer at 0x3b64c; growth calls the
  original float-vector append routine at 0x3b790–0x3b79c.
- The same setter sizes ordinary output/gain and coloration scratch vectors to
  N, three working vectors to 5N, and two input/history vectors to N+4. It also
  publishes N into the two input/history buffer-size fields.

Combined with the already executed scalar flutter recurrence, this implies N
random draws and N updates through each crinkle filter per correctly configured
N-frame speed call. The scalar factor samples the first filtered element; the
remaining elements advance the filter state for the next call. Phase updates
use the call's frame argument, whereas noise iteration uses vector length. A
buffer/callback mismatch would therefore decouple these two clocks. Actual
driver size and mismatch reachability have not been established here.

## Complete speed call

`Channel::processSpeed` at 0x37408:

1. When the signed ramp counter at +736 is positive, subtract one and add the
   increment at +732 to the current value at +728 using binary32 arithmetic.
   This is one increment per invocation, independent of N.
2. Publish that current value to +36 and copy LinkData+0 to +40.
3. Call scalar flutter at 0x502c4 with input 1 and N; publish its factor to +48.
4. Call scalar CapTouch at 0x36d20 with input 1 and N, a contact boolean true
   exactly when Channel+452 is 1 or 2, and the byte at +461 as the trigger.
   Publish its factor to +52.

The static call at 0x47cbc places this inside `AudioEngine::processChannel`.
The full callback, its upstream producers, and downstream motion consumption
are not executed by the new probe. A text search of this disassembly finds no
direct call to either vector overload (flutter 0x50040, touch 0x36ba0); this is
not proof that indirect calls are absent.

## Inline aliases and motion schedule

Previously executed pointer-selection slices in
`BOUNDARY_SOURCE_FINDINGS.md` establish unlinked LinkData = Channel+36 and
linked right LinkData = left Channel+36. This adds four important aliases:

| LinkData offset | Owning Channel offset | Meaning in the speed/render path |
|---|---|---|
| 0 | 36 | Current following speed |
| 4 | 40 | Previous following speed |
| 12 | 48 | Flutter factor |
| 16 | 52 | Touch factor |

The callback saves the selected LinkData factor product before the speed call
at 0x47c8c/0x47c90/0x47cb0. After that call, 0x47cc8–0x47ce0 reloads current
and previous following speeds and the current selected factor product. Thus
unlinked previous/current factor endpoints are naturally supplied by the inline
fields. `processSpeed` copies the previous speed before it overwrites the current
speed. A detached LinkData fixture alone cannot verify this relationship.

The motion loop 0x47e10–0x47e60 visits the playback manager first, then the
recording manager, obtaining each active list and calling original Tap::move
with current following speed, current factor product, and N for each nonzero
head. Held/following selection remains inside Tap::move. Input processing and
history publication precede this loop; boundary handling follows it. The
previous/current render endpoints are described and separately executed in
`RENDER_ITERATION_AND_RECORD_PROXIMITY.md`. Joining these whole paths remains
necessary; a per-head motion result is not a callback-wide ordering check.

For the linked right deck, these selected speed/factor reads come from the left
deck, while right processSpeed still updates its own local ramp and component
fields. The outer process at 0x49244 starts a thread whose 0x49688 worker calls
the left deck, calls the right deck on the calling thread at 0x4939c, and joins
the worker at 0x493b0. Static sequencing does not establish which left-field
updates each right-deck read observes. Do not claim that the linked deck always
sees either the prior or current complete left modulation snapshot. A complete
native specification needs an explicit consistent linked-deck snapshot policy
and acceptance tests; physical scheduling equivalence is currently unproven.

## Literal scalar touch contract

The original constructor 0x36b58 sets factor 1, state 0, minimum binary32 0.3,
and four increments with bits b7a7c5ac, 3951b717, b8d1b717, 38d1b717.
The mode is read via Channel+232 -> LinkData+176 -> preset+60.

- Other modes return the input unchanged, preserving touch state.
- Mode 1 advances the factor with one fused multiply-add of N and the first
  increment while contact is held, or the second otherwise. It clamps to [0,1].
  The >=1 branch stores factor 1 and returns the original input directly;
  interior values multiply the input by the factor. The joined caller's input
  1 makes the output equal the clamped factor in all branches.
- Mode 2 state 0 with a trigger sets state 1 and delta 0 for this call. State 1
  uses N times the third increment; if the factor was already <=minimum on
  entry, it changes to state 2. State 2 uses N times the fourth increment; if
  the factor was already >=1 on entry, it changes to state 0. After the state
  decision, add the delta and clamp to [minimum,1]. Transitions therefore use
  the prior factor, not the newly advanced factor. State 0 without a trigger
  retains the unscaled N delta, which normally clamps the result to 1.

These are literal scalar branches from static instructions. The behavioral
names, reachability of unusual states, and producer/gesture priority require
separate evidence. Do not silently replace this with a generic envelope.

## Contact and depth producers

Correction to the first version of this report: Channel+452 is the state of
the Button constructed at Channel+448 (0x3cdf4/0x3cdf8), not recording state.
Channel+461 is that Button's +13 rising-edge flag. `readButtons` supplies its
fourth input to that object at 0x28418–0x28428; `runIO` calls `readButtons` at
0x295c8, before the outer callback launches the deck-processing worker.
The actual GPIO pin is read from the application pin table; this decomposition
does not substitute a guessed pin number or physical capacitance law.

Original `Button::interpretGPIO` at 0x36640 sets current/previous state to 1 for
high and 0 for low, publishes rising=high && previous==0 at +13 and
falling=low && previous==1 at +14. Repeated high clears the rising edge. Hence
Stall's boolean is contact-held in ordinary Button states, and Dip's trigger
is the latest sampled rising edge. The scalar routine accepts state 2 as held
as well, but that state is not produced by this ordinary button routine.
Do not label its fixture-only acceptance as a normal physical transition.

`Application::checkCapTouch` at 0x27c74 is a separate capability/disable path:
if `readPin(30)` returns zero it returns; otherwise it logs and writes mode 0
into both deck preset objects at +60 (0x27cd0–0x27ce4). It is not the contact
edge producer and does not implement a pressure-to-envelope curve. The
physical meaning of pin 30 and this path's invocation conditions remain open.

Depth publication already has executed evidence in
`TIME_EFFECT_PUBLICATION.md`: forced setTime in selector 2 / preset TimePot=2
publishes amount = float32(saturated raw / 4095), then
depth = clamp(float32(amount * preset depth),0,1) to Flutter+24/+28.
The static preset-update path also writes these destinations at
0x3df5c–0x3dfa0 using the selected LinkData Time amount. This latter whole
transition has not executed. Neither path resets the Flutter oscillator, RNG
or filter state at these stores; this local observation is not a claim about
the entire preset-reset graph. Existing passing depth and scalar DSP probes
are separate runs, so their joined control history remains pending.

## Pending reproducible validation

```sh
python firmware/Lubadh/probes/probe_speed_modulation_join.py
```

The prepared probe executes the original Flutter, CapTouch and Button constructors,
Button.interpretGPIO, the complete
Channel buffer setter (including original vector growth), and the complete
speed call with an independently carried factory flutter/MT/filter/phase/ramp
and scalar touch model. It checks resize preservation/zero initialization,
all listed vector lengths, held/released contact, original button-edge histories, all three
touch modes, positive frame sizes 1/7/32/128, and three explicit seeds, with both
detached LinkData and the actual unlinked Channel+36 alias. It is
configured for 384 persistent calls per seed/mode so mode 2 can traverse its
downward and upward state cycle. Its assertions, heap budget and expected
transition coverage remain unverified until execution succeeds.

The Channel shell and upstream preset/depth/link/contact-pin/ramp publication
are supplied fixture boundaries. No full Channel construction, physical entropy,
appliance libm, zero-frame behavior, driver scheduling, vector overload, or head
motion is claimed. Upon a passing run, the probe writes
`probes/speed_modulation_join_probe_results.json`; that file does not currently
exist as evidence from this work.
