# Persistent render, output AntiAlias and engine-gain integration

`probes/probe_playback_pipeline.py` executes a continuous original callback
slice from `0x47fe4` until just before `0x48454`: playback-list refresh, all
head rendering and recording-head proximity attenuation, output AntiAlias,
engine-count gain update, and additive destination mixing. Those stages are
not separately invoked or replaced inside the slice.

**Four persistent-object sequences, 96 blocks and 12,864 numerical comparisons
pass with zero discrepancy** for raw sums, filtered sums, final additive mixes,
filter histories and gain state. Coverage totals 1,150 distinct instruction
addresses. Full fixtures and the ELF hash are in
`probes/playback_pipeline_probe_results.json`.

## What persists and changes

Each sequence retains the same playback and recording managers, heads, output
AntiAlias object and engine-count gain state across 24 blocks. Original gain
initialization and AntiAlias construction execute once. Original setters change
the filter coefficient between blocks without resetting its histories.

Block sizes cycle through 1, 7, 32 and 128 samples. Following speed cycles
through +0.125, +0.5, +1, -0.125, -0.5 and +0.25. The factor is either fixed
at 0.75 or cycles through 0.5, 1 and 0.75. Previous speed/factor locals come
from the preceding fixture block. These remain explicit producer inputs,
rather than execution of the complete hardware/control speed path.

At blocks 0, 3, 6 and 9, original activation adds a new musical engine. Some
heads follow speed and others hold -0.5. This interrupts gain ramps before
settling at four engines. Block 15 adds one direct transition slot per engine,
raising active heads from four to eight while preserving engine count and
its gain target. No manager rebuild occurs between these events.

The recording manager is either empty or contains two persistent held heads
at -1 and +0.5. They move each block using original `Tap::move`, and their
proximity attenuation executes inside the renderer. They do not write tape
in this probe. Tape contents remain a deterministic initialized waveform.

## Arithmetic fidelity exposed by integration

The initial independent model used the recovered real-valued cubic equation
with rounding at its output and accumulation. Raw discrepancies were small,
but persistent output-filter history accumulated them: one filtered sample
at block 15 exceeded the existing 8e-7 tolerance by reaching approximately
8.34465e-7 error. This was not resolved by raising the tolerance.

The reference now evaluates the same independently expressed cubic law with
the renderer's float32 and fused-operation order (`0x482d4..0x48338`):

```text
delta = float32(c - b)
curvature = fma(-3, delta, float32(d - a))
bend = fma(2, a, d)
bend = fma(-3, b, bend)
bend = fma(fraction, curvature, bend)
weight = float32(-inv6 * float32(1 - fraction))
slope = fma(weight, bend, delta)
sample = fma(fraction, slope, b)
sum = fma(head_gain, sample, sum)
```

Here `inv6` is the recovered float32 literal 0.16666670143604279. Every `fma`
above produces a float32 result. With this evaluation order, raw sums and the
subsequent stateful stages match exactly in the checked fixtures. Earlier
real-valued cubic models remain useful equation comparisons, but their small
tolerated differences should not be assumed to stay equally small through
arbitrarily long stateful chains.

The AntiAlias reference carries its two pairs of previous-input/previous-low
state through every block. Coefficients come from the recovered speed/cutoff
law and the original setter; the probe supplies that cutoff rather than
executing upstream dispatch. The gain model likewise carries current gain,
increment, remaining calls and cached engine count. It compares all four fields
after each continuous slice.

## Reproduction and limits

```sh
/tmp/lubadh-research-env/bin/python firmware/Lubadh/probes/probe_playback_pipeline.py
```

The reference reads original head/fade states after setup movement, so this
does not independently validate motion and fade-trigger equations end to end.
Allocation and movement are explicit setup calls; selected boundary handling,
natural fade expiry/manager update and tape resampling/scatter are not joined
to this slice. Recording-head participation does not establish simultaneous
record/playback fidelity without actual writes.

The destination begins with supplied nonzero content; its real monitoring
origin remains open. Execution stops before output TapeFilter, compander,
allpass, plate, preview and clipping. Reconstructed fade-table bytes are used
by both execution and reference. These results establish a persistent playback
subset, not the complete callback, original hardware bit identity or a finished
recreation. Next integration should join boundary-generated heads and actual
tape writes, then continue into the output coloration chain.

Subsequent [Persistent Boundary Pipeline](PERSISTENT_BOUNDARY_PIPELINE.md)
joins original boundary generation and subsequent manager updates to this
playback subset. Actual tape writes and coloration remain separate.
