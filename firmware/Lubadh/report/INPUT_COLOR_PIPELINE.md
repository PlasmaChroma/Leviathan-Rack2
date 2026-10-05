# Input coloration, filter coefficients and persistent preset changes

`probe_input_color_pipeline.py` executes original TapeInFilter/TapeFilter/
TapeAllpass construction, the high-pass biquad setter, the preset filter
publication slice `0x3e1e4..0x3e2a4`, and the connected input coloration slice
`0x47d44..0x47d80`. Independent rounded recurrences are in
`input_color_model.py` and `output_color_model.py`.

Continuation: `CONNECTED_INPUT_OUTPUT_RECORD_PIPELINE.md` now joins these
stages to input AntiAlias/history, tape recording/playback and final output.
The counts and scope below retain this isolated probe's boundaries.

The probe passes **50 normalized coefficient cases, 36 preset updates, 288
blocks and 32,508 samples**, with **134,904 numerical comparisons and zero
maximum error**. It executes 688 original instruction addresses. Results,
coefficient vectors, trajectories and the archived ELF hash are in
`probes/input_color_pipeline_probe_results.json`.

The publication fixtures cycle through all ten factory presets plus low/high
clamp fixtures three times, preserving filter histories throughout. Blocks use
7/32/128 samples and contain a sine plus DC offset. Each preset interval starts
with two blocks at zero age/wear/hysteresis, then exercises nonzero coloration.
The test compares every sample after each of the four connected stages, all
three biquad history cells, both input low-pass histories and all ten TapeFilter
histories. Diffuser indices and history preservation at coefficient publication
are separately asserted.

## Recovered input filtering law

TapeInFilter at Channel+6592 runs a **high-pass biquad** and then a **one-pole
low-pass**. These stages always precede the input TapeFilter at +6648, wear at
+6748 and diffuser at +6752. Effect amounts of zero do not bypass TapeInFilter.

The preset-update slice clamps LowCutFreq and HighCutFreq to 20..20000 Hz and
LowCutQ to float32(.2)..float32(.8). With firmware rate R=49170.25390625:

```
f = float32(LowCutFreq/R)
K = tanf(float32(pi*double(f)))
n = 1 / (1 + K/Q + K*K)
b = (n, -2*n, n)
a = (1, 2*(K*K-1)*n, (1-K/Q+K*K)*n)
lowPassCoefficient = float32(1 / (pi*double(float32(HighCutFreq/R))))
```

The reference retains the original float32/FMA and double-precision boundaries;
the equations above express topology rather than prescribe arbitrary evaluation
order. This is a preset-update law, not a recovered Rack-rate conversion policy.
Constructor defaults use an embedded normalized frequency and Q=.2; complete
constructor versus preset-update event behavior remains a separate state graph.

The high-pass setter's normalized-frequency edge paths use K=0 for f<=0 and
K=8.742277657347586e-8 for f>=1. Quality clamps at .2/.8. These unusual setter
edges are checked as branch fixtures; normal preset publication confines f to
20/R..20000/R, below the tan pole at .5. Do not expose unchecked normalized
setter behavior as a native user control range.

The biquad is direct form II. With previous internal values v1/v2:

```
v0 = x - a1*v1 - a2*v2
y = b0*v0 + b1*v1 + b2*v2
v2 = v1; v1 = v0
```

Its three-cell history stores v0 in both first and second cells at return. The
following one-pole uses coefficient c and histories px/py:

```
low = (y + px - py*(1-c)) / (1+c)
px = y; py = low
```

Coefficient updates preserve these histories; this probe checks that property
through each preset slice. Full preset update may have additional side effects
and is not covered by that narrow preservation claim.

## Connection, services and remaining scope

The callback slice connects TapeInFilter → TapeFilter → signed-square wear →
signed-delay diffuser. The last three stages use the independently expressed
equations already checked in `OUTPUT_COLOR_PIPELINE.md`; input and output own
separate persistent state. `TIME_EFFECT_PUBLICATION.md` checks their shared
amount publication separately. Here the age/wear/hysteresis values are explicit
fixtures rather than complete Time/event producer sequences.

Original instructions perform coefficients, filter state and stage processing.
Only this probe additionally allows `tanf` at `0x159d8`, checked against the
archived import map. Its finite float32 argument/result uses host glibc tanf.
The reference uses the same declared service. This proves the surrounding
arithmetic and connection under that service, not original ARM-libm bit identity
or physical analog equivalence. No appliance program or script is launched.

```sh
python3 firmware/Lubadh/probes/probe_input_color_pipeline.py
```

Use the existing offline Unicorn/glibc environment. Input routing/gating,
AntiAlias, input-history publication, recording and transport are not yet joined
to this slice. Full preset update, reset, linked routing, control event timing
and Rack adaptation remain necessary before claiming a complete specification.
