# Connected input coloration, tape processing and final output

`probe_record_pipeline.py --connected --input-chain --native-output-clipper`
joins the previously checked input and output DSP to actual tape recording on
the same allocation. Its independent input filter/coloration/AntiAlias/history
state feeds the tape-write reference; independently predicted tape rendering
feeds the output effects. Original tape, history, phase and effect state persist
across blocks.

## Joined matrix

The matrix passes **108 sequences, 2,592 blocks, 144,288 samples, 23,257,800
numeric comparisons and 10,368 schedule/state assertions**, with zero maximum
error in all measured fields. Coverage is 4,080 original instruction addresses.
Results are in `probes/connected_input_output_record_pipeline_probe_results.json`.

Fixtures combine 7/32/128-frame blocks, following/held-opposed/overlapping record
heads and ordinary playback splices, feedback 0/.5/1 and three write-clipper
branches. The final output clipper uses its actual constructor defaults for all
cases. Twenty-four blocks per sequence exercise the first eight factory filter
settings; the longer fixture below cycles through all ten. Neither executes
complete factory-preset loading or all associated control fields.

Observed events include 2,916 writing head calls, 324 no-displacement calls,
72 overlapping-write blocks and 270 splice-profile multihead playback blocks.
Counterfactual write-before-render changes raw output in 1,429 blocks. These
counts describe these fixtures, not ordinary patch frequency or overall parity.

## Longer persistent fixture

A following-head sequence passes **192 blocks, 24,576 samples, 1,903,296 numeric
comparisons and 768 schedule/state assertions**, with zero maximum discrepancy
in every measured field. It executes 3,293 original instruction addresses. The
result is `probes/connected_input_output_long_record_pipeline_probe_results.json`.

This fixture cycles through all ten factory filter settings, preserving filter
histories on original coefficient publication. Both input cutoff-dispatch
branches execute: following-speed cutoff and fixed 20000. This flag is supplied
as a fixture; it does not establish complete fixed-record-speed head activation.
Age/wear/hysteresis and reverb amounts change while tape and effects persist.
The actual constructor output clipper uses knee=.5, compensation=.75.

The longer sequence covers repeated wraps of every plate buffer at initialized
modulation rates. It has one recording head and no playback-splice profile;
those cases require the separate matrix rather than being inferred from length.
All tape coordinates remain inside the 8,192-sample fixture allocation.

## Original slices and independently checked connections

Input execution starts at `0x47cc0`, including the Channel LinkData load. It runs
continuously through cutoff publication, input filter, age, wear, diffuser,
disabled monitoring branches, input AntiAlias and `InputBuffer::input`, stopping
at `0x47df4`. Preset filter publication executes `0x3e1e4..0x3e2a4` separately.
The actual callback InputBuffer is used at **Channel+172004**.

The reference builds the four history samples plus independently predicted
processed input. It compares the original buffer exactly before tape recording
uses it. It does not copy original processed input into its recording reference.
Input and output AntiAlias histories are separate, and their cutoff publications
are checked as well as the sample recurrence.

Original explicit head-motion calls then precede playback/boundary/render/
AntiAlias/gain/mix execution. At `0x48454`, an observation stop allows reference
comparison and write prediction without changing original tape/effect memory.
Execution resumes through output coloration, disabled preview, final clipping,
sequential record-head writes and both manager updates to before `0x48554`.

Comparisons cover all five input stage vectors, input biquad/low-pass/age/
AntiAlias histories and coefficients; raw playback, output AntiAlias, additive
mix and all five output stages; output filter/plate state; record sampling phase
after each head; and every tape cell. Whole-tape comparisons include unchanged
cells and should not be counted as independent scenarios or an overall fidelity
score. Head/fade arithmetic still reads original moved state, so upstream event
and transport generation are not an independent whole-instrument model.

The connection proves that post-color, post-AntiAlias input reaches actual tape
writes and later playback, while current-block output is rendered before those
writes. `MONITOR_MIX_FINDINGS.md` separately checks the disabled branch's active
copy/fade behavior; monitoring is not enabled in this connected suite.

## Reproduction and remaining work

```sh
python3 firmware/Lubadh/probes/probe_record_pipeline.py --connected --input-chain --native-output-clipper --cycles 2
python3 firmware/Lubadh/probes/probe_record_pipeline.py --connected --input-chain --native-output-clipper --quick --quick-profile following --cycles 16
```

Use the existing offline Unicorn/glibc environment. The harness explicitly
permits only the established memory services plus verified host `tanf` and
finite `floorf` imports. This is not original ARM-libm or physical hardware
equivalence, and no appliance executable or system script is launched.

This joins callback slices, not the complete callback. Monitoring/routing flags
are disabled, motion calls remain explicit, recording boundary generation is
omitted, and playback-splice region locals are fixtures. Moving channel write
fade activation/expiry, first-record tails, active preview, full preset/control
events, linked decks, voltage/rate adaptation and analog behavior remain open.
Passing these DSP connections does not close those specification requirements.
