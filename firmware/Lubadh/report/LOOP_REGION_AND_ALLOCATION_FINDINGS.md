# Lúbadh loop regions and transition-slot allocation

Two further original-byte probes narrow the transport integration gap: the complete `Channel::setLoopingParameters(bool, bool)` routine and the TapManager constructor, allocation, age queries and fade reclamation. These establish per-call behavior under initialized fixtures. The full callback, clock/link event graph and audible loop splices remain unvalidated.

## Loop-region fields and inputs

`probes/probe_loop_regions.py` executes `0x376ac` through its actual return, including `ADC::hasChanged` at `0x6c13c`. The latter compares the integer cached value with the previous value using a strict `abs(delta) > threshold` test, then stores the current value as previous regardless of the result. A small change therefore consumes the previous value; this is not an accumulated deadband measured from the last accepted control update.

The following offsets are the contracts observed here. LinkData fixtures remain independent from Channel fixtures, because the copying/linking rules producing those values are a separate investigation.

| Input | Location |
|---|---|
| Logical recorded period `N` | Channel+0x4c |
| End marker `E` | Channel+0x50 |
| Previous selected length `Lold` | Channel+0x80 |
| Previous fade width `Fold` | Channel+0x88 |
| Maximum fade width | Channel+0x84 |
| Start scaling span `S` | LinkData+0x28 |
| Grid division count `D` | LinkData+0x80 |
| Start quantization flag | LinkData+0x6c |
| Length quantization flag | Channel+0x90 |
| Minimum length | Preset+0x30 |
| Cached start/length/fade controls | Channel+173220 / +173216 / +173236 |
| Start/length ADC objects | Channel+173336 / +173320 |

The cached controls and ADC values can be varied independently in the fixture. This confirms that the former supply the scaling values while the latter gate updates. It is not evidence that normal hardware operation makes them independent.

The two bool arguments are called `modifier` and `force` in the probe according to their effect. They are not yet a complete mapping from physical button gestures. Start and length each update when their ADC reports a change or `force` is true.

## Scaling and quantization

Every arithmetic stage below retains float32 rounding where the original instructions do. Conversion to integer truncates toward zero. With 12-bit control `u`:

```text
scale(u, span) = trunc(float32(float32(float32(u)/4095) * float32(span)))
grid = signed_integer_division(N-1, D)
start = clamp(scale(start_control, S), 1, S-2)
```

When start quantization is enabled and `modifier` is false, start becomes `(start / grid) * grid` using integer division. The initial clamp occurs **before** this floor operation; a quantized start can therefore become zero.

Raw selected length is `scale(length_control, N-1)`. With length quantization enabled and `modifier` false, it becomes `(raw_length / grid + 1) * grid`. This selects the next grid multiple even when raw length is already on a grid boundary. The result then becomes:

```text
new_length = max(max(1, preset_minimum), min(N-1, candidate_length))
```

The minimum is applied after the upper bound. An explicitly supplied minimum larger than `N-1` wins in this routine. That fact does not establish how short native/imported recordings are admitted or how callers handle the resulting selection. Tests include short periods with the factory minimum 1280 without claiming all such fixture combinations are UI-reachable.

The probe covers positive periods/spans and positive nonzero grids. Zero divisors, zero-sized grids, invalid negative spans and malformed presets remain outside these comparisons.

## Endpoints use values captured on entry

This is the most consequential result for moving controls. The routine snapshots `Lold` and `Fold` before updating selected length or fade width. It then calculates:

```text
new_fade = max(128, trunc(float32(min(trunc(Lold/2), maximum_fade)
                                 * float32(fade_control/4095))))
start_plus_old_fade = new_start + Fold
end = new_start + Lold
if end >= E:
    end = end % N                    # signed integer remainder
end_plus_new_fade = end + new_fade
```

These go to Channel offsets 0x88, 0x3c, 0x40 and 0x44 respectively. The selected length at 0x80 may already contain its new value when the old length is used for end. The fade calculation has a hard floor of 128 samples, even when the half-length or maximum fade is smaller. `start_plus_old_fade` is not wrapped in this routine. End wrapping is gated by the separate marker `E`, rather than simply testing `end >= N`; when `E > N`, a coordinate beyond the logical period can remain unwrapped.

A four-call test on the **same emulated object**, with only the first call forced, checks how these fields settle after a length change. Later calls recalculate endpoints even when the ADCs no longer report a change. This establishes successive-call behavior, not how many audio frames elapse between calls or when LinkData is refreshed.

For example, with old length 20,000 and old fade 512, the fixture changes selected length to 12,295 and start to 24,591:

| Call | Selected length | End | Fade width | Start plus fade field |
|---|---|---|---|---|
| First, forced | 12,295 | 44,591 | 5,001 | 25,103 |
| Second, unchanged controls | 12,295 | 36,886 | 3,074 | 29,592 |
| Third and fourth | 12,295 | 36,886 | 3,074 | 27,665 |

That last field needs an additional call to consume the updated fade width. A model that recalculates every endpoint immediately from the new controls would miss this observed sequence. Whether that intermediate state affects audible playback depends on the unresolved caller/LinkData ordering.

## Cursor boundary check is asymmetric

When Channel+0x7c enables its cursor, this routine checks the integer coordinate at Channel+0x78 against LinkData start/end at offsets 0x14/0x1c. The accepted interval is `(start, end]` for an ordinary region, or `position > start OR position <= end` for a wrapped region. Equal boundaries accept every integer in this particular check.

Outside that interval, nonnegative speed snaps the cursor to LinkData+0x1c; negative speed snaps it to LinkData+0x18. Zero speed follows the nonnegative branch. This check concerns that cursor field, not every playback head; the callback's head/fade boundary logic remains separate.

## Four engines, five slots, age-based reuse

`probes/probe_tap_allocation.py` runs the original TapManager constructor at `0x4ca68`. The manager contains four 680-byte engines, each with five 132-byte Tap slots and a five-entry slot-age list. Its engine-age list begins at offset 2720. Engine identity remains stable while slots represent overlapping transitions.

Direct `TapManager::activate(position, engine)` at `0x4e460` uses the first inactive slot within the specified engine. Five calls allocate slots 0..4. A sixth call returns null, leaves positions intact, and retains the previous last-activated pointer.

`activate_new` at `0x4eea8` first prefers a wholly inactive engine in physical engine order. When all four engines have active slots, it selects the oldest engine from the manager age list. It triggers a type-2 kill fade on that engine's active slots that do not already have one, then asks the engine to activate another slot. The selected engine moves to the front of the age list.

In tested bursts with no intervening motion/update, allocations proceed:

```text
engines: 0 1 2 3 0 1 2 3 ...
slots:   0 0 0 0 1 1 1 1 ... 4 4 4 4
```

The first 20 calls populate all slots. Subsequent `activate_new` calls still rotate engine age and may initiate kill fades, but do not activate or reposition another slot. They return the selected engine's previous last-activated Tap pointer. This differs from the direct activation routine's null return. A caller cannot infer successful allocation solely from a nonnull `activate_new` result.

The tested `any_available` at `0x4dc80` reports availability of another logical engine through the manager age list. It becomes false once all four engines are present, even while transition slots remain unused. `num_engines` at `0x4ddd0` counts nonzero entries in that age list; it does not count every simultaneously active transition slot. These distinctions matter for polyphony limits and the previously inspected mixing target.

## Motion controls reclamation

After 20 no-update triggers with 128-sample kill fades, advancing all heads by 256 frames at speed +1 or -1 and running the complete `TapManager::update` at `0x4d138` leaves four active slots: slot 4 in each engine. The manager count remains four. After 25 triggers, the saturation attempts have also applied kill fades to those final slots, so the same motion/update sequence leaves zero active slots and zero listed engines.

With zero following speed, the same 256-frame call does not advance the fades, and all 20 slots remain active after update. Explicit held speed +1 or -1 permits reclamation while supplied following speed is zero. These fixtures set the held override after allocation; they establish motion/update mechanics without reconstructing the hardware action that enables held pitch.

This is not yet a complete retrigger policy: callback timing, caller behavior after saturation, selective kill operations, holes in age lists, held-speed changes during fades and audible splice rendering still need integration fixtures.

## Validation and reproduction

The loop-region probe passes **1,884 independent configurations plus four successive calls**, with **18,880 exact field comparisons** and 177 distinct instruction addresses. Its JSON includes representative fixtures and the settling sequence; the CSV records each compared field. The allocation probe passes **5,454 assertions**, covering four direct-engine saturation tests, six 32-trigger bursts, and ten motion/reclamation cases, with 1,304 distinct instruction addresses. These are emulator/object-contract results, not hardware measurements.

From the repository root, using the optional environment documented in [Transport Findings](TRANSPORT_FINDINGS.md):

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_loop_regions.py
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_tap_allocation.py
```

Results are separate from the archived baseline validation totals and hashes. The probes require the exact archived ELF hash and use the bounded offline byte harness. No appliance process, Rack module or firmware update script is launched.

The follow-up [ordinary-region boundary findings](BOUNDARY_TRANSITION_FINDINGS.md) now execute selected signed callback branches, including replacement allocation, transfer, full-block movement and direct-slot exhaustion. Wrapped-region/physical-seam transitions, tail access and mixed audio through those transitions remain open. The verified per-call region and allocator rules provide stronger fixtures for that work; they do not substitute for it.
