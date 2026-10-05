# Persistent playback, tape recording and moving write fades

The continuation joins playback rendering and actual recording on the same
tape, with persistent input history, filter state, input-resampling phase and
manager updates. Ordinary playback splices execute in one fixture profile.
The intervening output coloration remains deliberately skipped.

Continuation: `CONNECTED_RECORD_OUTPUT_PIPELINE.md` now joins output effects to
the same-tape sequence. The baseline results below retain their original scope;
the newer matrix and native-default long fixture are separate.

## Connected same-tape probe

`probes/probe_record_pipeline.py` executes original playback from `0x47fe4` to
before `0x48454`, or starts at `0x47ed4` to include the ordinary playback boundary
manager. It subsequently executes recording traversal and both manager updates
from `0x484e4` to before `0x48554`. Original input history at `0x38a78` and head
motion at `0x4bde0` run as explicit calls before these slices. No DSP/transport
routine is replaced by a host hook.

**108 persistent sequences, 1,296 blocks, 10,840,068 numerical comparisons and
5,184 schedule/state assertions pass with zero discrepancy.** Numerical fields
include raw head sums, filtered sums, additive mixes, filter histories, tape
contents and the input phase after each recording head. Whole-tape comparisons
include untouched cells and are not millions of independent behaviors. Results
record 2,218 distinct instruction addresses and the archived ELF hash.

Fixtures combine 7/32/128-frame blocks, following-speed recording, held opposite
record speed, two overlapping recording engines, or ordinary playback splices;
feedback 0/0.5/1; transparent, knee and rational write clipping. Each sequence
uses twelve signed/fractional speed changes, including two stalled blocks,
with factors 0.5/1/0.75. Tape and input contain distinguishable finite samples.

| Observed event | Count |
|---|---:|
| Recording-head calls that write storage | 1,458 |
| Recording-head calls with no integer displacement | 162 |
| Blocks with overlapping recording-engine write sets | 72 |
| Blocks where counterfactual write-before-render changes the raw sum | 765 |
| Ordinary splice-profile blocks rendering two playback heads | 117 |

These are fixture event counts, not a distribution of typical user patches.

### Scheduling and ownership contracts

- Playback rendering and filter/gain/mix operate on tape left by the previous
  block. They do not mutate storage. The later recording traversal updates tape
  without changing the already rendered mix. Writing first changes checked
  output in 765 blocks, so the ordering is audibly consequential.
- Recording traversal calls `recordInput` in the refreshed active-list order.
  Each head gathers and scatters before the next head runs. A later overlapping
  head blends against storage left by the earlier head. A global gather of old
  tape for all recording heads would describe a different algorithm.
- Recording heads use the same fixture `InputBuffer`. Its phase at +16 updates
  after every head, including no-displacement calls. Preserve that shared phase
  and call order in a literal compatibility implementation; per-head phases
  would be a deliberate change.
- Both manager updates run after recording. Playback splice heads remain
  available through rendering and write proximity calculations until update.
- Held recording heads select held speed for motion and write sampling, while
  playback filtering uses the supplied following-speed cutoff. This validates
  the connection for fixture states, not every upstream control producer.

The independent reference evaluates rounded cubic reads, fade/proximity gain,
input sampling, sequential feedback gathers, clipping, scatter, phase and
persistent AntiAlias/gain mixing. It reads original moved head/fade states at
render entry. These tests establish arithmetic and stage connection, not an
independent complete model of event activation or transport generation.

The playback-splice profile uses fixture region 3430..3550 with 32-sample fades.
Recording heads move on the same tape but their boundary manager is skipped.
Region values are branch fixtures, not demonstrated production states from a
fresh recording. This adds real recording to selected playback splices without
claiming complete simultaneous record/play boundary recovery.

## Moving channel write envelope

`probes/probe_moving_write_fade.py` executes original `Fade::move` at `0x4bafc`
and complete `recordInput` at `0x47818`. **1,044 cases and 2,138,112 whole-tape
comparisons pass with zero discrepancy**, including 432 stationary-envelope
cases. Coverage totals 550 instruction addresses.

Both envelope endpoints remain inside [0,254). Fixtures cover forward/reverse
recording at speeds ±0.3 and ±2, three block lengths, feedback 0/0.5/1, ascending
and descending fades, and changes below/above the 0.1 threshold. The write
clipper is transparent knee=1, compensation=0 for this isolation.

Let `T` be the reconstructed 256-entry table, `p` an integer envelope coordinate,
`f` its fraction, and `b` requested overdub feedback:

```text
incoming  = T[p] + f * (T[p+1] - T[p])
reverse   = T[255-p] + (1-f) * (T[254-p] - T[255-p])
retention = b + (1-b) * reverse
stored    = clip(inputSample * tapFade * incoming + oldStored * retention)
```

Matching probes preserve float32/FMA evaluation. `tapFade` is unity in this
isolated probe; the earlier transport probe checks additional tap fades.

For `abs(currentEnvelope - previousEnvelope) < float32(0.1)`, all writes use
the **current** integer/fraction envelope coordinate. Otherwise the first write
uses the **previous** coordinate, advancing by
`float32((currentEnvelope - previousEnvelope) / writeCount)` for each storage
write. The denominator is crossed tape cells, not host frames. Recording speed
is not applied again to the envelope.

Incoming gain and reversed retention are not complementary copies of one linear
curve. The 254/255 reversed-table convention matters. At low feedback a fade can
preserve old tape while suppressing new input. Replacing these gains with
`incoming=1-retention` changes checked behavior.

## Remaining integration work

Neither probe executes the entire callback. Output TapeFilter/compander/allpass,
MonoPlate, preview and output clipping at `0x48454..0x484d8` are skipped in the
connected probe. Upstream input coloration, control publication, first recording
and tail scheduling remain separate. Channel write-fade activation, out-of-table
prefix/suffix branches and expiry bookkeeping remain unvalidated; the moving
envelope probe uses explicit original calls and supplied states.

Execution and reference share the reconstructed fade table. No hardware, analog
voltage, OS scheduling, concurrent control or final-module-output agreement is
claimed. No Rack module source was modified.

## Reproduction

Use Linux Python with pinned `requirements-analysis.txt` packages. The leaf
model requires glibc `libm.so.6`.

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_record_pipeline.py
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_moving_write_fade.py
```

Results and fixtures are in `probes/record_pipeline_probe_results.json` and
`probes/moving_write_fade_probe_results.json`. Baseline bundle hashes and
validation totals retain their original scope. Current readiness and next work
are in `SPECIFICATION_READINESS.md`.
