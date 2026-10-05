# Persistent scalar flutter/crinkle state

Complete original scalar TapeFlutter::process at 0x502c4 matches independent
phase, MT19937, biquad and returned-factor models over 24 sequences / 3,072 calls.
All 129,024 random draws and 2,082,816 comparisons pass with zero discrepancy;
coverage is 279 instruction addresses. These results close the scalar recurrence
gap for explicit initialized fixtures, not the factory constructor or full
modulation-to-head scheduling.

## Reproduction and fixtures

```sh
python firmware/Lubadh/probes/probe_flutter_state.py
```

Results: `probes/flutter_state_probe_results.json`, including per-sequence seeded
raw-noise statistics and factor ranges. Main ELF SHA256 remains pinned by the
shared harness to
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.
No appliance process or entropy device executes. Complete original MT19937
operator() and both Biquad::process calls run inside the scalar process.

Seeds are 5489/1337/0xdeadbeef; independent standard integer seed expansion
initializes the 624-word engine. Every call compares all 624 words and its index,
covering refills as well as tempering. Initial oscillator coordinates are zero
or index 254/fraction 0.875. Supplied frequencies are 0.6/5 and stress values
500/1500. Frame arguments cycle through 1/7/32/128/1024, while the separately
supplied noise vector has 1/7/32/128 entries. This deliberately distinguishes the
two scheduling inputs.

The sine table is supplied from host reconstruction, not recovered original
libm output. Filter coefficients are supplied transparent or stable stateful
test coefficients, not asserted factory coefficients. Their original binary32
recurrences and every persistent state entry are independently checked. Factory
constructor/entropy/table/filter initialization remains a separate requirement.

## Phase and periodic component

For each oscillator with frequency f, integer index i and fraction p:

```
amount = f32(f32(f / 49170.25390625) * f32(frames))
p = fmaf(amount, 255, p)
while p >= 1:
    p = f32(p - 1)
    i += 1
i %= 255
wave = fmaf(p, f32(table[i+1]-table[i]), table[i])
periodic = fmaf(depth, wave0, f32(f32(depth*0.5) * wave1))
```

The 256-entry table is interpolated with modulo-255 index progression. The
stress frequencies exercise multi-index/multiple-period movement. The float
input argument supplied to the scalar overload is not used to scale the returned
factor in this path; varying it from -10 through 100 leaves the independent
prediction valid. The unsigned frame argument advances periodic phases.

## Stochastic component and filters

The noise vector length determines how many random draws and filter samples
execute; it is not replaced with the unsigned frame argument. For the supplied
distribution bounds -1/+1, the scalar path converts each unsigned random word
to binary32, scales by 2^-32, interpolates between the bounds, then subtracts
0.5. Its raw range is therefore approximately -1.5..+0.5, with a negative mean.
This is a pre-filter property; it is not a claim about factory filtered output
or audible DC. The upper rounded endpoint has a dedicated fallback branch,
which requires a separate forced-endpoint fixture if the seeded result reports
zero such cases.

Both filters process the entire generated vector in sequence using their
stored denominator/numerator coefficients and Direct Form II state. The final
factor consumes only the first filtered entry. Later entries still advance the
filter state used on subsequent calls. Changing vector length consequently
changes future irregularity even when the periodic frame argument is unchanged.

## Factor and release

```
combined = fmaf(crinkleDepth, filteredNoise[0], periodic)
factor = fmaf(f32(f32(combined+2.5)/5), 0x3daaab00_as_float,
             0x3f755550_as_float)
```

Depth controls cycle through zero, quarter/full and release to zero. With both
depths zero the returned factor is exactly 1 in these binary32 fixtures, while
phases, RNG and filters continue evolving. Release is multiplication at output,
not a reset or suspension of the generator. Enabling depth later consumes the
then-current generator/filter state. The saved seeded summaries describe the
supplied test coefficients and cadence only.

## Remaining specification work

Execute the constructor with an explicit entropy boundary; recover and verify
factory sine generation and filter coefficients; force the rare uniform endpoint;
join actual Time/preset publication, processSpeed/touch and head movement; resolve
the vector overload's separate contract and choose Rack cadence/rate adaptation.
The reconstructed table and stable test filters must not silently become parity
implementation defaults.

Subsequent `FLUTTER_FACTORY_FINDINGS.md` verifies the complete constructor with
explicit entropy/libm services and joins its independently predicted table,
coefficients and seed state to persistent processing. It also forces the uniform
endpoint branch omitted by the ordinary seeded runs above.
