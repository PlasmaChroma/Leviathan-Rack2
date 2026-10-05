# Pot-speed events and ADC publication

The complete `Channel::setPotSpeed` routine is independently checked over
persistent sequences in all four table modes and four numeric control states.
The original ADC producer, production smoothing constructors and two-deck
speed-dispatch prefix are also joined and checked. This closes the digital
pot-speed publication contract, while leaving physical CV transfer, gesture
state producers, clock events and audio scheduling open.

## Evidence and reproduction

| Probe | Checked result |
|---|---|
| `probe_pot_speed_events.py` | 384 persistent sequences, 6,528 calls, 71,808 field comparisons; 130 distinct original instruction addresses |
| `probe_adc_speed_publication.py` | 256 asymmetric two-deck sequences, 6,144 ticks, 229,376 comparisons; 270 distinct original instruction addresses |

Both pass exactly. The former uses independent speed tables from
`speed_table_model.py`, already checked against the preset generator; its V/oct
calibration remains a supplied fixture. The latter executes original
`readADCs` and `ADC::interpretVal`, both actual `setPotSpeed` calls in the
`interpretADCs` prefix, and the production ADC constructor slice. Synthetic
12-bit values replace only `LubadhHardware::readMCP` at 0x6c064. No hardware,
SPI, OS service or appliance process is executed. Unknown services still fail
closed. See `SPEED_TABLE_AND_SLEW_FINDINGS.md` for pinned executable hashes.

```sh
python firmware/Lubadh/probes/probe_pot_speed_events.py
python firmware/Lubadh/probes/probe_adc_speed_publication.py
```

Results and representative persistent traces are in
`pot_speed_event_probe_results.json` and `adc_speed_publication_probe_results.json`.
Sequences include endpoint saturation, movement of exactly nine/ten counts,
subthreshold movement accumulated from the last accepted value, midpoint
crossings, reverse direction and unequal deck states. Counts do not establish
whole-module parity.

## Complete pot-speed consumer

`setPotSpeed` begins at main:0x37510. These fields have different roles:

| Object offset | Checked role |
|---|---|
| Channel+172032+0x498 | Latest saturated raw speed value |
| Channel+172032+0x49c | Last accepted raw speed value |
| Channel+696 | Candidate speed; may be quantized/direction-adjusted |
| Channel+44, +728, +732, +736 | Target, current slew state, increment, remaining count |
| Channel+0x104 | Numeric control state read by this routine |
| Channel+0xec | V/oct direction state; this routine tests equality to 1 |
| Channel+0x110 | Pending event word |
| Channel+0x2bc | Deferred speed-change flag |
| Channel+0x10e | Enables the deferred-change consumer |
| Channel+0x1c / +0x20 | Peer Channel / Application pointers |
| Application+344064+0xabc | Link flag used for peer event publication |

The numeric control-state and gate fields are fixture inputs. Their complete
upstream user-facing meanings are not inferred from this consumer alone.

1. Saturate the signed input to 0–4095 and always store the raw value.
2. If its distance from the **last accepted** raw value is at most nine,
   leave the accepted value and candidate unchanged. The deferred consumer
   below still runs. A new raw value within the deadband can therefore affect
   tap-speed direction selection even though it does not change this candidate.
3. Otherwise accept the raw value and look up the preset's table entry.
4. In numeric states 1/2, round the candidate to the nearest signed marker
   unless `SpeedControl==1`. This is deliberately different from state 0:
   Stepped's table is linear and remains unquantized in states 1/2.
   Set the local pending event and deferred flag to 1. If linked, also set the
   peer pending event to 1; this routine does not copy the speed to the peer.
5. In state 0, quantize only Stepped mode. In state 3, retain the raw candidate.
6. In V/oct mode, negate the candidate when direction state equals 1.
   States 0/1/2/3 all pass through the applicable sign branch. Direction value
   2 does not negate here; the earlier tap-speed consumer tests nonzero,
   so these routines differ for that supplied, potentially unreachable value.
7. State 0 immediately sets the target/increment/count using the recovered
   slew law. States 1/2/3 leave that state unchanged unless step 8 applies.
8. Independently of raw acceptance, if +0x10e and +0x2bc are both nonzero,
   slew to the stored candidate and clear +0x2bc. Pending event words remain.
   This does not perform a second quantization or sign inversion.

The consumer never advances current speed itself. Peer/current/direction state
is preserved except for the explicitly listed local and peer publications.
The reference model obtains no state from executed output to determine expected
values. It carries its own state through each sequence.

## ADC filtering and publication

`Application::readADCs(int)` at 0x28190 always reads two speed ADCs first,
then reads one of six other ADCs selected by slot 0–5. Slots outside that
range skip the third read. All eight input paths invert 12-bit hardware counts
as `4095 - MCPsample` before filtering.

`ADC::interpretVal` at 0x6c104 computes:

```
retained = f32(f32(previousInteger) * f32(1 - alpha))
newInteger = trunc(fmaf(f32(4095 - sample), alpha, retained))
```

It stores and returns that integer; fractional smoothing history is discarded
at every read. This is not a float-state one-pole filter followed by occasional
quantization. The recurrence and published values match exactly across all
tested endpoints, sweeps and unequal deck stimuli.

The constructor slice 0x3d730–0x3d77c, ending before 0x3d780, creates four
ADCs per Channel. The speed ADC uses binary32 `0x3f666666` (approximately
0.9). The other three use 0.5. All start with current/last-change integers
zero and threshold 2. The joined production fixtures execute those constructors
for both decks and independently assert all sixteen fields per deck.
Additional .1/.5/1 alpha fixtures broaden recurrence coverage. The threshold
belongs to `ADC::hasChanged`; the speed routine separately implements its
nine-count accepted-value deadband. `hasChanged` itself is not invoked here.

Application fields below are relative to its base. Channel A begins at +72;
Channel B at +173440. The offsets are valid for this archived executable only.

| Input | Pin field | ADC object | Published integer | Read slot |
|---|---|---|---|---|
| A speed | 173256 | 173376 | 173280 | Every pass |
| B speed | 346624 | 346744 | 346648 | Every pass |
| A control 1 | 173257 | 173392 | 173288 | 0 |
| B control 1 | 346625 | 346760 | 346656 | 1 |
| A control 2 | 173259 | 173408 | 173292 | 2 |
| B control 2 | 346627 | 346776 | 346660 | 3 |
| A control 3 | 173258 | 173424 | 173296 | 4 |
| B control 3 | 346626 | 346792 | 346664 | 5 |

The speed publication addresses coincide with each Channel's latest raw
speed field. `interpretADCs` at 0x2830c invokes A's `setPotSpeed` and then B's,
using those published values. The joined probe stops at 0x28340 before loop
and Time consumers; it does not replace those consumers with stubs and then
claim a full callback. Production state matrices use A Notched and B V/oct,
with all combinations of numeric state 0–3, linked/unlinked and deferred gate.
Thus peer event effects are checked alongside independent candidates and targets.

## Native specification consequences and remaining work

The digital recurrence, deadband, marker decisions and deferred publication
can now be expressed as requirements with deterministic vectors. Keep raw,
accepted, candidate, target and current speeds distinct. A native improvement
that removes ADC noise filtering should retain a defined control law and test
the resulting midpoint/deadband behavior. Preserving the firmware recurrence
requires an explicit control-tick cadence; host UI frame rate cannot define it.

No separate external speed-CV ADC appears in this routine. That establishes
the digital read structure, not the physical circuit's pot/CV combination or
voltage law. Still unresolved: actual pin assignments/front-end transfer,
control-state and deferred-gate producers, runtime I/O cadence, full linked
event consumption, clock/tap gestures, flutter/touch and audio motion scheduling.
The native Rack adapter needs deliberate voltage/timing decisions or hardware
measurements before claiming those aspects at parity.

The subsequent `TIME_SELECTOR_STATIC_CONTRACT.md` derives a production slow-ADC
domain invariant from this checked alpha=.5/integer recurrence: valid reads
starting from constructor zero publish at most 4094. Independent numerical
checks cover all reachable cached integers and nine Time-indexed runtime arrays.
This narrows the direct-4095 indexing question; alternate cache writers and a
complete readADCs-to-Time/region event history remain outside these probes.
