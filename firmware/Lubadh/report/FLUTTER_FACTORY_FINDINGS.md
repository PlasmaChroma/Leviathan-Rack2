# Factory flutter initialization and uniform endpoints

Complete original TapeFlutter construction now connects to the independent
persistent scalar model. Three seeded factory sequences / 384 process calls /
16,128 random draws pass 260,352 recurrence comparisons with zero discrepancy.
Constructor fields, all sine-table entries, seeded engine words and filter
coefficients are separately checked. Five forced uniform endpoint cases also
pass, including both examples that take the upper-rounding fallback branch.

## Reproduction and boundaries

```sh
python firmware/Lubadh/probes/probe_flutter_state.py --factory
python firmware/Lubadh/probes/probe_flutter_endpoint.py
```

Results: `probes/factory_flutter_state_probe_results.json` and
`probes/flutter_endpoint_probe_results.json`. The first stores exact initialized
parameters and seeded output summaries. Coverage is 533 instruction addresses
for construction/process and 427 for the forced endpoints. Main ELF SHA256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.

Original constructor 0x4f1c0, Biquad constructors/coefficient consumers, seed
expansion, table-fill loops, MT generation and scalar processing execute. Memory
services use the standard inert harness. random_device initialization is inert
and its getval service returns the explicit seed 5489/1337/0xdeadbeef; no entropy
device opens. sinf/tanf/expf are explicit host glibc services. Every service's PLT
identity is asserted. Constructor calls one entropy initializer and one getval,
255 sinf, two tanf and two expf services.

This verifies original initialization arithmetic around those boundaries, not
physical entropy quality or appliance-libm bit identity. The independent table
and coefficient formulas share the declared primitive libm boundary; their
expected results are not read back from initialized original memory.

## Verified defaults

- Frequencies are binary32 0.6 and 5; both integer phase indices start at zero.
  Fractions are 0 and binary32 0.17 respectively.
- Both depth amounts start at zero. The noise-vector header is empty; process
  fixture sizing is still explicit and its production publication remains open.
- The 256-entry table starts with zero, followed by sinf of binary32
  `(i/256) * binary32(2*pi)` for i=1..255. Scalar processing retains modulo-255
  indexing, as verified in the earlier recurrence suite.
- The constructor first creates a default MT state, then replaces it with the
  standard 624-word seed expansion from the supplied random_device result.
  Final engine index is 624 and uniform bounds are -1/+1.
- Both Biquad state vectors are zero. Filter modes are 0 and 1.

## Literal filter coefficient laws

Let f0/f1 be literal binary32 values from 0x38aa9a72/0x382a9a72. Original
coefficient initialization for these modes reduces to:

```
q0 = expf(f32(f0 * (-2*pi)))
denominator0 = [1, -q0, 0]
numerator0   = [f32(1-q0), 0, 0]

q1 = expf(f32((0.5-f1) * (-2*pi)))
denominator1 = [1, q1, 0]
numerator1   = [f32(1-q1), 0, 0]
```

With the declared host math boundary, q0=0.9994890093803406 and
q1=0.04322496056556702. The second mode's literal numerator has no DC-cancelling
zero. Its recurrence must not be substituted with a conventional high-pass
library filter based solely on the mode's name. Persistent factory-filter state
and generated modulation output match the independent equations exactly through
depth changes and release. This supersedes the earlier supplied-test-coefficient
gap for these constructor modes.

## Forced upper rounding

The endpoint suite supplies inverse-tempered MT words so the original operator()
returns 0, 0xffffff00, 0xffffff7f, 0xffffff80 and 0xffffffff. This is a deliberate
state fixture, not a claim that a selected seed often generates those values.
Words at/above 0xffffff80 round to 2^32 when converted to binary32; scaling gives
1 and takes the original fallback at 0x50474. It interpolates using 0x3f7fffff,
the binary32 predecessor of one. Both fallback cases produce raw noise
0.49999988079071045, exactly matching the neighboring ordinary path. Zero gives
raw -1.5. Factory filtering and the returned factor are also checked, rather
than observing only the branch.

## Remaining integration

Actual noise-vector sizing, Time/preset depth publication, processSpeed/touch and
head motion, vector-overload behavior and Rack rate/cadence adaptation remain
open. Host libm is an explicit portability boundary. Physical entropy may be
replaced by a documented native seed policy for reproducible patches, but such
a policy is a Rack design decision rather than recovered hardware behavior.
