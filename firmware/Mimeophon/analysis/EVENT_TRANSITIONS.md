# MP86 expiry requests, head selection and reverse windows

Continuation, 2026-10-04. Same MP86 binary and hash as
`MODULATION_SCHEDULER.md`. Reference: `reconstruction/event_components.hpp`.
This pass joins the decrement, expiry arbitration and RNG stages, then
separately reconstructs request consumption and selected-head reverse motion.
It does not implement the entire physical-input or clock acquisition logic.

## 1. State correspondence and ordering

| State | A: heads 1/3 | B: heads 0/2 |
|---|---|---|
| Delay counter | `0x20003868` | `0x20002aac` |
| Transition status | `0x20002aa4` | `0x20002ae4` |
| Last consumed control token | `0x20001b28` | `0x20001b44` |
| Stored Hold offset | `0x20002aa8` | `0x20004c70` |
| Delay target at expiry | `0x20002b44` | `0x20001ad0` |
| Timer reset on expiry | `0x20004e80` | `0x200055d4` |
| Transient restart flag | register r6 | register r5 |
| Halo speed perturbed | 969-sample walker | 803-sample walker |

The A/B names retain the modulation pass's ordering. They are not the order
of head pairs in memory: the even pair is B. Both pairs share:

- `0x20004d78`: clock-mode state; expiry requests require **exactly 1**.
- `0x200037f4`: reverse/Flip-like flag by DSP behavior.
- `0x20004f94`: Hold-like flag by cursor/recording behavior.
- `0x20003890`: current integer control token. Its writer at
  `0x080261a2..0x080261c6` truncates a control integer multiplied by 13/4096.
  This pass keeps the address-backed name instead of assuming it is a clock
  edge counter or the eight-zone index.
- `0x200038a0`: Hold anchor/offset seed, populated through the zone-dependent
  table selection ending at `0x08026304`.

The audited sample prefix is `0x08023f64..0x08023fa6`. It decrements both
counters, handles A expiry, then B expiry. Its two transient restart flags
default to zero each frame. Each expiry refills its counter and resets its
timer to 720 before testing request conditions. **It always perturbs its
Halo walker**, including while a head fade is active or clock mode is zero.

The timer decrement sites are in the DSP call prefix at
`0x080239d8..0x080239fc`, once per four-frame call. Thus 720 decrement ticks
correspond nominally to 60 ms at 48 kHz, subject to other writers and refreshes.
This is a cadence statement, not proof of a physical gate pulse width.

## 2. Complete decision table for the inspected expiry paths

Apply this separately to each expired pair, in A/B order:

| Condition, in priority order | Status/result | Other writes |
|---|---|---|
| Existing status != 0 | Retain status; restart=0 | Retain token and Hold offset |
| Clock-mode word != 1 | Retain idle status; restart=0 | Retain token and Hold offset |
| Saved token != current token | Request (1); restart=0 | Copy current token |
| Hold snapshot == 1 | Request (1); restart=0 | Copy Hold anchor into pair offset |
| Flip word == 1 | Request (1); restart=1 | Retain token and Hold offset |
| Otherwise | Idle (0); restart=0 | Retain token and Hold offset |

The common counter, timer and random writes still happen in every row.
Evidence: `0x08025a9e..0x08025baa`, `0x08025dda..0x08025e0e`,
`0x08025fca..0x08025fde`, `0x080263e0..0x08026412`,
`0x080266e2..0x08026702`, `0x080267ae..0x080267b2`, and the two RNG store
tails at `0x08026862/0x0802686a`.

Consequences:

- Status 1 (pending) and status 2 (fading) both suppress these expiry requests.
  A token change during a fade does not update the saved token here. A later
  idle expiry can therefore detect the then-current token; this is not a FIFO
  of every intervening control change.
- A token change takes priority over Hold and Flip. A simultaneous token
  change does not also initialize the Hold offset or emit a restart flag.
- With an unchanged token, Hold takes priority over Flip. Hold+Flip does not
  emit this path's transient restart flag.
- Exact comparisons matter. A nonzero clock word other than 1 does not enable
  these requests, even though the subsequent code at `0x08023fa6` branches on
  any nonzero word. The helper deliberately preserves raw words.

### Hold has both cached and fresh reads

Expiry uses `[sp+16]`. That value is loaded from the Hold word before the
sample loop at `0x08023e5a..0x08023e64`, then refreshed later in each frame at
`0x08024b30..0x08024b4a` during Color/recording processing. Other operations
reread the live word, including `0x08024276..0x0802427c` before Hold-specific
retargeting. A harness should expose this snapshot separately from the live
Hold input; using one freshly sampled boolean throughout the frame can alter
event ordering. Physical input update/interrupt timing remains unresolved.

## 3. Consuming a request toggles selection without resetting gains

At `0x08024288..0x08024298`, request status 1 is consumed for B first, then A.
Out-of-line blocks are `0x0802599e..0x080259e6`, `0x08025eda..0x08025ee2`,
and `0x08025eec..0x08025ef6`.

For each pair `(first,second)`:

```text
if status == 1:
    if first.selected == 1:
        second.delay = current_pair_target
        first.selected = 0
        second.selected = 1
    else:
        first.delay = current_pair_target
        first.selected = 1
        second.selected = 0
    status = 2
```

The target is sampled here, after intervening control and Hold-specific code;
it need not equal the target used earlier to refill the expiry counter.
The stores copy the target's raw float word. The consumer does **not** reset
crossfade gains, moving-delay coordinates, interpolation state, or buffers.
Status values other than 1 leave both heads alone in this stage.

This matters for interrupted fades: a newly requested head change continues
from the existing gains. Restarting every transition at gains 1/0 would create
a different envelope and potentially a discontinuity. Existing gain helpers
describe the later render-before-gain-update order; this pass does not replace
the downstream status clearing/reassertion logic.

## 4. Reverse requests use a four-sample window and can interrupt

Only when the live Hold word is not exactly 1 and Flip is exactly 1 does the
branch at `0x080242e6..0x080242f6` select this reverse stage. It processes the
selected even head (B), then selected odd head (A). Earlier code has already
advanced the nonselected heads' moving coordinates by 2; the helper here only
models the selected-head stage.

Evidence: `0x08025c80..0x08025d40`, `0x08025e1e..0x08025e3a`, and
`0x08025ea8..0x08025eba`. For each selected head, using finite float32 values:

```text
use_moving = 1
moving = max(moving + 2, 2)
twice = delay + delay
if moving > twice + 16:
    moving = 2
else if moving > twice + 12:
    status = 1
else if this_frame_restart:
    moving = 2
```

The request interval is **(2*delay+12, 2*delay+16]**, after advancing by 2.
Equality at the lower bound does not request; equality at the upper bound
does. Crossing above the upper bound resets position without writing status.
The transient restart flag is tested only at or below the lower threshold;
it does not override the request-window branch.

Unlike expiry arbitration, the window's status=1 store has no idle-status
guard and can overwrite status 2. It also executes **after** request
consumption, so its request is left for a subsequent consumer rather than
causing a second immediate head toggle. On that later toggle the old gains
survive. Which requests survive all intervening render/control writes still
requires a full-frame trace; the local overwrite and ordering are established.

## 5. Reference boundary and validation

`tools/test_rack_components.py` compiles `tests/test_events.cpp` natively.
Three new byte-checked instruction translations cover the joined expiry
prefix, request consumer and selected-head reverse stage. Tests compare the
**entire fixture memory map**, so extra writes as well as missing writes fail,
and compare both transient restart registers. A Cartesian matrix covers idle,
pending and fading statuses, malformed/nonbinary words, token changes,
simultaneous expiries, unity and nonunity ratios, all head selections, and
adjacent float values at reverse-window boundaries. A separate regression
checks gain preservation when a reverse request interrupts a fade.

The generated slices include the actual random updates, testing their order
inside expiry rather than supplying independent triggers. Test counts,
compiler and source hashes are in `continuation_validation.json`; addresses
and instruction counts are in `color_trace_audit.json`. This remains a bounded
host translation, not hardware or full CPU equivalence. Tests use finite,
representable delay sums and supplied boundary state.

The subsequent [Hold/input pass](HOLD_AND_INPUTS.md) closes Hold transport and
retargeting, validates raw Hold polling and clock qualification, and traces
the TIM2 callback and Flip writer. Outstanding work is now more specific:

1. Finish the accepted-clock handler, period selection and mode-exit dependencies.
2. Connect the UI's TIM2 configuration to its clock tree and finish the
   secondary input/mode-save path; confirm physical input correspondence.
3. Join these stages with nonselected-head updates, rendering and final
   status writes to test complete interrupted transitions over multiple frames.
4. Establish full startup state and peripheral assumptions before claiming
   an end-to-end engine or mapping the remaining words to Rack ports.
