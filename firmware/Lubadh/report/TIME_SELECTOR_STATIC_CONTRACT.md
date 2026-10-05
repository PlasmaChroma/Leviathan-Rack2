# Time selector and deferred-control static contract

This continues `TIME_EFFECT_PUBLICATION.md` with static decomposition of the
unexecuted branches. **No new original-byte run or passing result is claimed.**
WSL probe execution remains pending approval-review authentication recovery.
Addresses refer to the archived main ELF, SHA256
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.

Notation: C = Channel, W = C+172032, L = pointer at C+232, P = pointer at L+176,
and M = pointer at C+171868 (the reporting mirror). All table indices below
describe literal instructions, not a recommended native indexing policy.

## Accepted control and dispatch

`setTime` at 0x37a44 saturates its signed argument to v in [0,4095]. The distance
is measured against W+0x4ac, the last accepted value. Distance <=29 with no force
returns. Larger movement in Channel mode C+260 ==0 promotes the call to forced;
larger unforced movement in other modes diverts to 0x37d6c. These gate outcomes
were already executed in the prior Time probe, which stops before the diversion.

Accepted/forced dispatch reads the selector at W+0x4d0. It is distinct from
preset TimePot at P+44. The jump table at 0x37a94 has six entries:

| Selector | Entry | Publication |
|---|---|---|
| 0 | 0x37e3c | Index table W+0x4e8/+0x4ec using cached raw W+0x4a8; store selected integer at W+0x4d8 and M+24. Copy C+650 to C+649; store v at W+0x4b0. |
| 1 | 0x37ea4 | float32(v/4095) times preset MaxDubLevel at P+17004; publish L+164 and M+28; store v at W+0x4b8. No clamp appears in this branch. |
| 2 | 0x37ee0 | Dispatch on preset TimePot at P+44; see below. |
| 3 | 0x37f14 | Store v at W+0x4c4 and M+44. |
| 4 | 0x37f28 | Store v at W+0x4c8 and M+48. |
| 5 | 0x37f3c | Store v at W+0x4cc and M+52. |

All these paths finish by storing v at W+0x4ac. Selectors above 5 reach that
common store without another dispatch publication. Labels for the menu fields
need their complete consumer contracts; the raw stores do not imply audio
semantics by themselves.

## Preset-selected third function

At selector 2, TimePot 0 publishes v at W+0x4b4 and M+32. This is the raw
crossfade-control value; conversion into actual fade spans is downstream.
TimePot 1 stores v at W+0x4c0 and publishes
float32(float32(float32(preset integer P+20) * float32(v)) / 4095) at L+172
and M+36 (0x37f50–0x37f7c). The preset integer is not separately capped in
this branch. The speed-ramp producer's subsequent integer conversions and 2.7
divisor remain distinct from this Time law.

TimePot 2 enters the full tape-effect path at 0x37aac, which is already executed
for all factory presets and clamp fixtures in `TIME_EFFECT_PUBLICATION.md`.
It stores v at W+0x4bc, Time amount at L+168, modulation/effect depths and the
write-clipper/reverb parameters. Other TimePot values skip these publications
and still accept v into W+0x4ac. A specification should preserve separately
stored values for crossfade, slew and tape amount when switching functions.

## Deferred unforced clock/control diversion

At 0x37d6c, only Channel modes 1 and 2 continue. The routine refuses publication
if any of three pointed status words is 3: local W+0x1f8, other-channel
W+0x1f8 (other Channel pointer C+28), or local W+0x18c. Exact loader/menu
interpretation and complete causal status histories require separate evidence.

Otherwise, it uses cached raw W+0x4a8, not the new v, to index the integer vector
at L+112/+116. It sets L+124 to 1, writes the selected integer to L+128, and
sets L+108 to whether that integer is nonzero. It sets C+272 to 1, also sets
other Channel+272 when the application's link byte at application+346812 is
nonzero, copies C+144 to M+56, and writes the selected integer to M+60.
It then accepts v at W+0x4ac. These flags are producer-side publication;
they do not establish eventual consumer timing or feedback effects.

Both this branch and selector 0 form index by multiplying the binary32 cached
amount (cached raw /4095) by the full unsigned vector length, then converting
to unsigned integer and reading begin[index]. There is no local clamp to
length-1. At a supplied cached raw of 4095 and a normally representable length,
the mathematical endpoint selects index length. Whether production values,
vector storage, a sentinel, or earlier scheduling makes that case legitimate
remains unverified. Native bounds and endpoint behavior must be deliberate and
tested; do not claim a hardware failure from this static observation.

## Archived table bytes and copied extent

`tools/extract_time_indexed_tables.ps1` now reads the archived ELF through its
file-backed PT_LOAD segments, checks the exact SHA256, and extracts the data
into `evidence/time_indexed_table_bytes.json`. This is executed data extraction,
not ARM execution. The clock setter at 0x3b320 passes the following complete
arrays to the original vector assignment at Channel+173288 (W+0x4e8):

| Mode | Source address | Copied entries |
|---|---|---|
| All | 0x72f4c | 0,1,2,3,4,5,6,7,8,9,10,11,12,16,24,32,64 |
| Even | 0x72f90 | 0,2,4,6,8,10,12,16,24,32,64 |
| Odd | 0x72fbc | 0,1,3,5,7,9,11 |
| Powers of two | 0x72fd8 | 0,1,2,4,8,16,32,64 |

The ranges passed to vector assignment contain respectively 17,11,7,8 words.
The constructor at 0x3be94–0x3bf20 allocates exactly 28 bytes for L+112's default
quantisation vector, copies seven words from 0x72ff8, and sets both end and
capacity to begin+28. The extracted words are 0,2,4,8,16,32,64. This proves no
eighth allocated sentinel in that constructor path. Alternate preset lists and
later vector reuse still need their complete publication histories checked.

The leading zero is an actual copied element, not a trailing bounds sentinel.
Earlier `tables/clock_division_sets.json` listed documented musical values
without zero. It now retains those separately and adds `runtime_clock` and
`runtime_constructor_quantisation` from this byte evidence. Treating the old
documented lists as complete runtime tables would change the low-end bins and
their widths. The zero value's causal behavior must be checked in each consumer;
do not assume it universally means clock-off or quantisation-off.

The raw endpoint question therefore persists and is stronger on the default
quantisation allocation: no extra constructor storage makes index==7 valid.
The Time division path is still not executed with a production ADC history.

## Production ADC domain narrows the endpoint question

`POT_SPEED_EVENT_PUBLICATION.md` and the passing
`adc_speed_publication_probe_results.json` already execute the ADC recurrence,
the production constructor slice and actual readADCs publication. The three
non-speed ADCs per deck have alpha=0.5 and integer state initially zero. The
Time ADC is the third slow channel, published at Channel+173224 (W+0x4a8).
Its sampled input is inverted as 4095 minus the supplied MCP value.

For valid MCP inputs in [0,4095], let x be that inverted input. The exact
production recurrence specializes to:

```text
current_next = floor((current + x) / 2)
```

Every intermediate here is an integer or half-integer well within exact
binary32 representation, so the original multiply/FMA/conversion boundaries
do not alter this formula. Starting at zero, induction gives current in
[0,4094]: the largest next value from current<=4094 and x<=4095 is
floor(8189/2)=4094. Repeated full scale approaches the fixed point 4094; it
does not produce 4095. This proof assumes unchanged constructor alpha/state and
valid reads; it does not prove that other code cannot overwrite the cache.

`tools/verify_time_control_domain.ps1` checks the two recurrence bounds for all
4,095 allowed current states (8,190 endpoint checks), and the literal binary32
table-index expression for every cached raw 0..4094 across the nine extracted
arrays (36,855 index checks). It also records a 16-read full-scale trace, per-bin
sample counts, and the direct 4095 diagnostic. All checks pass in
`evidence/time_control_domain_checks.json` with status
`PASS_NUMERICAL_DOMAIN_ONLY`. This is an independent numerical check anchored
to prior ARM evidence, not a new ARM run or complete readADCs-to-Time test.

For this production recurrence, all nine copied-table indices remain in range
and both the first and last bins are reachable. Direct raw 4095 still produces
index==count in the literal expression. Thus the previous report's endpoint
question is narrowed: it is not evidence of a normal valid-read ADC fault;
altered state, direct publication, restoration and alternate writers still need
audit. A Rack adapter that supplies unsmoothed integer full scale can reach a
different domain and must explicitly clamp its lookup or preserve the ADC law.

## Production input and cached-state provenance

The hardware-to-Time bound is now supported by additional static and archived
data evidence. `tools/verify_time_input_bytes.ps1` produces
`evidence/time_input_byte_checks.json` with status
`PASS_DATA_AND_NUMERICAL_ONLY`. It verifies the main ELF hash, reads the
file-backed constructor/initHardware literals and independently checks all
65,536 pairs of response bytes. No ARM instruction, SPI operation or hardware
initialization executes in this check.

`Application` constructs A with channel ID 0 at APP+72 (0x2a018–0x2a01c), and
B with ID 1 at APP+173440 (0x2a020–0x2a034). The Channel constructor selects
the ID-specific 16-byte pin block and copies it to C+173184, or W+1152:
ID 0 uses the 0x3dd60 literal (0x3da8c–0x3daac); ID 1 uses 0x3dd50
(0x3da0c–0x3da28). `initHardware` at 0x37084 installs identical byte blocks
from 0x37108 and 0x370f8. The first four bytes are:

| Deck / ID | Speed | First control | Time | Second control |
|---|---:|---:|---:|---:|
| A / 0 | 5 | 4 | 7 | 6 |
| B / 1 | 0 | 1 | 2 | 3 |

The control labels here describe `readADCs` destinations, not physical circuit
provenance. Slow slot 4 reads A's third byte at APP+173258 (0x282e4), slot 5
reads B's third byte at APP+346626 (0x2821c). Both invoke Time's ADC and store
the result at APP+173296/+346664, respectively. Relative to each Channel,
these are C+173224, the same W+0x4a8 cached Time field used by the selector.
Earlier passing ADC probes supplied synthetic pin fields; these literal checks
close production pin provenance statically, without upgrading those probe
fixtures to a complete production hardware run.

`readMCP` rejects unsigned pins >7 at 0x6c064–0x6c068, returning 65535 at
0x6c0bc. For valid pins, it calls SPI with three command bytes, then decodes
`((response[1]<<8)&0xf00)|response[2]` at 0x6c0a0–0x6c0b0. Because both
responses are byte loads, this enforces 0..4095 even for arbitrary returned
bytes. `readADCs` inverts the result with 4095 minus the read value. Thus both
installed Time pins satisfy the 12-bit input assumption of the alpha=.5 domain
proof. An invalid pin instead gives -61440 after inversion and is outside that
proof; no such pin is installed by these ID 0/1 paths.

Cached Time starts at zero: at 0x3d7f4 r3 becomes W+1184; d18 loads eight zero
bytes from 0x3d8c0, then 0x3d814 stores them at r3+8, covering W+1192
(0x4a8) and W+1196. This must not be confused with the 0x3d748 zero store
to W+0x4f0, which is a different Channel field. Time ADC construction at
0x3d774–0x3d77c supplies alpha=.5 to `ADC::ADC`, whose constructor initializes
current and prior integer states to zero. The independent check verifies the
cached zero literal; the previously executed ADC probe verifies ADC behavior.

This audit resolves the constructor, normal read publication, valid-pin decode,
and hardware-map reinitialization slices. Direct-offset text searches are not
a complete alias/writer analysis: alternate state mutation, restoration,
bulk copies and direct setter callers remain open. The normal domain conclusion
therefore retains its explicit unchanged-ADC-state assumption.

## Preset quantisation arrays and zero consumer boundary

The byte extractor also checks the arrays selected by preset Quantisation
(P+17000) in the jump table at 0x3e2d4. Unlike the constructor's default,
preset selector 0 installs All:

| Preset selector | Entry | Source / copied count | Runtime values |
|---|---|---|---|
| 0 | 0x3e6e8 | 0x7305c /16 | 0,2,3,4,5,6,7,8,9,10,11,12,16,24,32,64 |
| 1 | 0x3e644 | 0x72f90 /11 | 0,2,4,6,8,10,12,16,24,32,64 |
| 2 | 0x3e5a0 | 0x7309c /6 | 0,3,5,7,9,11 |
| 3 | 0x3e398 | 0x72ff8 /7 | 0,2,4,8,16,32,64 |

These branches allocate a temporary vector of precisely count*4 bytes and copy
it into the selected LinkData vector at +112. The table bytes and static copy
extents are checked; vector assignment and complete preset transition are not
new execution evidence. The published artifact now has a separate
`runtime_start_length_quantisation` mapping for these four complete arrays.

The deferred Time producer writes selected D at L+128 and sets the start
quantisation flag L+108 to (D!=0). It does not write the independent length
quantisation flag C+144; it only reports that byte at M+56. Consequently zero
provably disables the producer's start flag, but this is not proof that it
disables all region quantisation, clocking, or every consumer.

`setLoopingParameters` computes signed (period-1)/D before testing the start
flag (0x376d4–0x376f4). Its length branch separately checks C+144 at 0x377dc.
The existing region probe excludes zero divisors and zero grids. Those cases
must be verified in the original ARM execution rather than inferred by
translating an unchecked integer division directly into C++.

For the native implementation, the proposed explicit safety contract is:
table lookup clamps the chosen index to [0,count-1] with an empty-table fallback
defined as no quantisation; region quantisation performs division only when
D>0 and its computed grid>0. With no valid grid, preserve the raw scaled
start/length and apply normal region bounds, retaining stored enable flags for
the next valid division. This is a deliberate bounds policy, pending integration
with the complete region/event specification; it is not a claim that firmware
behaves identically for invalid grids. Acceptance must cover raw 0/4094/4095,
all four arrays, empty arrays, D=0, D>period-1, and both length-enable values.

## Selector lifecycle and touch capability call

The ordinary cycle in `interpretButtonPress` at 0x3f67c–0x3f6ac increments the
selector modulo 3 and sets C+645/+648 reporting flags. Its downstream branches
inspect the preset third function for selector 2. The complete gesture priority
is not inferred from this slice.

`PresetLoader::init` saves the prior selector at loader+360 and publishes
selector 5 at 0x3911c–0x39128; `PresetLoader::exit` restores the saved selector
at 0x38edc. File loader paths also publish/save selectors (0x39438,
0x39528–0x39540, 0x395c8/0x395dc). Thus selectors 3–5 are reachable through
asset/menu state and are not extra preset TimePot enum values. Their complete
save/restore and cancellation graphs remain open.

One further call site resolves the earlier touch-disable invocation question:
`processPresetUpdate` calls `Application::checkCapTouch` at 0x3e2a4 after input
filter configuration. That routine's pin-30 disable behavior is described in
`SPEED_MODULATION_JOIN_FINDINGS.md`. Full preset execution and physical pin
meaning remain unverified.

## Required closure checks

- Execute all six setter branches with asymmetric stored/cached/new raw values;
  verify unchanged fields as well as each publication.
- Execute the deferred branch with each status guard and both linked modes;
  join its flags to actual consumers and pointer aliases.
- Test cached-raw endpoints and table allocation/sentinel provenance. The
  production alpha=.5 recurrence domain is now proved and numerically checked;
  audit alternate writers/restoration/direct publication. Choose
  explicit safe native table indexing and record any behavioral deviation.
- Run cycle/menu/load/cancel histories carrying separate crossfade, slew, tape,
  dub and division selections through mode restoration.
- Join effect publication to the prepared persistent speed/touch probe, then
  to motion and rendering; retain the actual IO/audio scheduling boundary.
