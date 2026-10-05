# Same-tape playback, complete output effects and recording

`probe_record_pipeline.py --connected --cycles 2` connects the previous
persistent record fixtures to TapeFilter, wear, diffuser, MonoPlate, disabled
preview and final clipping. The same original CPU, tape allocation, input phase,
effect histories and managers persist across blocks. This supersedes the output
coloration omission in `PERSISTENT_RECORD_PIPELINE.md` for this suite.

Continuation: `CONNECTED_INPUT_OUTPUT_RECORD_PIPELINE.md` adds the connected
input filter/coloration/AntiAlias/history path with separate results. The
baseline matrix and scope below remain unchanged.

**108 sequences, 2,592 blocks, 144,288 output samples, 22,453,416 numeric
comparisons and 10,368 schedule/state assertions pass with zero discrepancy in
every measured field.** Coverage totals 3,735 original instruction addresses.
Results and per-block fixtures are in
`probes/connected_record_pipeline_probe_results.json`.

The matrix retains 7/32/128-frame blocks; following, held-opposed, overlapping
record engines and ordinary playback-splice profiles; feedback 0/.5/1; and
transparent/knee/rational clipper fixtures. Each sequence runs two rounds of
twelve signed/fractional speed steps with persistent tape writes and effect
state. Age/wear/hysteresis change every four blocks; reverb amount advances
through .125/.5/.75/1 every three blocks using original publication instructions.
Other effect and clipper settings remain explicit fixtures.

| Observed event | Count |
|---|---:|
| Recording-head calls writing storage | 2,916 |
| Recording-head calls with no integer displacement | 324 |
| Overlapping record-write blocks | 72 |
| Blocks whose counterfactual write-before-render changes raw playback | 1,431 |
| Splice-profile blocks rendering multiple playback heads | 270 |

## Connection and comparison boundary

The original playback/boundary/render/filter/gain/mix slice stops at `0x48454`
for observation. The reference computes coloration from its predicted pre-write
mix and predicts tape writes. No original tape/effect memory is changed at that
observation boundary. Execution resumes at `0x48454` and runs continuously
through output effects, ordinary write traversal and both manager updates to
before `0x48554`.

Comparisons include raw sums, AntiAlias output/state, additive mix, all five
effect stage outputs, ten TapeFilter histories, eight plate filter histories,
two plate phases, each record-head input phase and every cell of the 8,192-sample
fixture tape. Diffuser indices and traversal/output preservation are separately
asserted. The count includes unchanged tape cells; it is not a fidelity score.

Playback uses storage left by the previous block. Output coloration processes
that vector before recording changes storage. Each record head still gathers
and scatters sequentially against tape left by its predecessor, sharing the
input-resampling phase. Recording does not alter the colored output vector.
This checks that coloration and its temporary storage preserve those scheduling
and state contracts.

## Actual output defaults and longer persistence

The matrix's output clipper follows its explicit branch fixture.
`--native-output-clipper` instead executes original constructor publication at
`0x3d7e4..0x3d844`: knee=.5, compensation=.75, gain=1.25, knee mode. See
`TIME_EFFECT_PUBLICATION.md` for its checked producer.

A separate following-head sequence passes **192 blocks, 24,576 samples,
1,774,272 numeric comparisons and 768 assertions**, again with zero discrepancy.
It covers repeated wraps of every plate buffer at initialized modulation rates;
2,972 instruction addresses execute. Results are in
`probes/connected_native_output_record_pipeline_probe_results.json`.

Following heads keep coordinates inside the small allocation. An extended
held-forward recording head eventually exceeded it; the bounds guard rejected
that fixture. Recording seam handling remains required and is not simulated by
wrapping indices in this arithmetic reference. This longer fixture has one
recording head and no splice profile; overlapping heads and splices are covered
by the matrix rather than claimed for this sequence.

## Reproduction and remaining gaps

```sh
python3 firmware/Lubadh/probes/probe_record_pipeline.py --connected --cycles 2
python3 firmware/Lubadh/probes/probe_record_pipeline.py --connected --native-output-clipper --quick --quick-profile following --cycles 16
```

Use the existing offline Unicorn/glibc setup. The plate explicitly supplies the
finite floor service; other DSP executes original instructions. No appliance
program is launched.

This is connected callback slices, not the entire Channel callback. Input
history and motion still run as explicit original calls. Input is supplied audio
rather than the full input coloration/routing path. Head arithmetic reads
original moved head/fade state; region locals are fixtures and recording boundary
generation is omitted. Channel write fade, first-record tails, active preview,
complete control/preset producers, linked routing, sample-rate adaptation and
hardware/analog equivalence remain open. `TIME_EFFECT_PUBLICATION.md` checks
selected scalar producers separately, without closing those event graphs.
