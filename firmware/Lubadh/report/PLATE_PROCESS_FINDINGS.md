# Executed MonoPlate process, modulation and control contract

This continues `PLATE_INITIALIZATION_FINDINGS.md`. An independently expressed
state recurrence now matches the complete original `MonoPlate::process(float&)`
at `0x496c0`, including its filters, thirteen delay writes, modulated cubic reads,
cross-feedback, seven output taps and final saturation. This closes the core DSP
reconstruction gap for the tested firmware state; it does not establish complete
instrument or analog equivalence.

## Evidence and checks

| Probe / result JSON in `probes/` | Coverage | Result |
|---|---|---|
| `probe_plate_process.py` / `plate_process_probe_results.json` | Five profiles, 16,384 samples each; default, wet, mixed, allpass and bypass filter fixtures | 2,129,920 numeric comparisons, zero maximum error; 1,065 instruction addresses |
| `probe_plate_controls.py` / `plate_control_probe_results.json` | 60 amount/preset combinations, both 128-entry constructor tables | 496 exact assertions; 438 addresses |
| `probe_plate_state_transitions.py` / `plate_state_transition_probe_results.json` | 24 seeded sequences, 6,144 samples, all four filter modes, live amount updates | 178,176 comparisons, zero error; 960 phase wraps, 360 ring-index wraps; 1,182 addresses |

Each process sample checks output, eight filter history values, two modulation
phases, two feedback values and the latest write in all thirteen buffers. All
thirteen indices are asserted separately. The reference in `plate_model.py`
does not read original intermediate/output state while processing. Coefficients
and modes come from executed initialization or explicit fixtures; both modulation
tables are independently generated and compared directly with constructor memory.

Original instructions execute under Unicorn with the existing bounded memory
services. This probe explicitly permits only the additional `floorf` import at
`0x163a4`, checked against the archived PLT map. A finite float32 floor service
supplies its result; the original filter, interpolation, feedback and clipping
instructions remain active. The archive main ELF SHA-256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.

## Filter and delay topology

The constructor report contains the exact buffer/descriptor layout. Relative to
the plate, filter state is at `0xf74` (input), `0xf64` (after the 980-sample delay),
`0x9ee0` and `0x198b0` (tank damping). Each stores mode, coefficient, previous
input and previous low-pass output. With coefficient c and histories px, py:

```
low = (x + px - py*(1-c)) / (1+c)
high = x - low
mode 0: low; mode 1: high; mode 2: low-high; other: bypass
```

Modes 0–2 update both histories; bypass leaves them unchanged. Evaluation order,
float32 rounding and FMA boundaries are explicit in the reference. Constructor
modes are high-pass input and low-pass for the other three; coefficients are
313.02593994140625, 2.4078917503356934 and 4.471799373626709 for each tank filter.

The filtered input is written to the 980 delay. Its old value passes through the
second filter and four sequential allpasses of lengths 233, 175, 623 and 455,
with coefficients .75, .75, .625 and .625. For each allpass:

```
written = x + coefficient*old_delay
y = old_delay - coefficient*written
```

At each sample, feedback captures the old current cells of the 5156 and 6116
buffers, multiplied by decay. Tank A reads 6933, applies damping, then the
2959/.5 allpass and writes 5156. Tank B reads 7322, applies damping, then the
4376/.5 allpass and writes 6116. The input diffuser output plus feedback from B
feeds the 588 modulated allpass and writes 6933; its counterpart plus feedback
from A feeds the 392 modulated allpass and writes 7322. Reads use old cells;
preserving this scheduling is part of the sound contract.

## Modulated allpasses

Constructor coefficients are float32 .35 for length 588 and .475 for length 392,
both with depth 50. Phase increments are respectively 1.423632238584105e-5 and
1.220256308442913e-5 per processed sample. They are firmware sample-domain
constants, not yet a recovered Rack sample-rate adaptation law.

Both 128-entry tables use x=i/127, rounded as recorded in the reference:

```
x < .75: 1 - 2*abs(x-.25)
x >= .75: 2*(x-.75)
```

Advance and wrap phase before reading. Linearly interpolate at phase*127,
multiply by depth, and subtract length-1 once if the amount exceeds length-1.
Read delay length-minus-amount. The read coordinate uses truncation toward zero,
including a possibly negative fractional remainder, then wrapped four-point
cubic interpolation. Substituting floor-based coordinates changes this contract.
The ordinary allpass write/output equations follow that fractional read.

Seeded tests accelerate phase increments and place all indices near ring ends.
Some deliberately exceed normal modulation depth to exercise the single
subtraction branch. They demonstrate branch arithmetic, not user-reachable
production settings. Default-rate long sequences and connected output tests
separately cover initialized production parameters.

## Output and amount publication

All seven taps are captured before tank writes. Listed as buffer length / read
delay / signed weight:

| Buffer | Delay | Weight |
|---:|---:|---:|
| 6933 | 437 | +1 |
| 6933 | 4889 | +.125 |
| 2959 | 2655 | −float32(.3) |
| 5156 | 3282 | +float32(.2) |
| 7322 | 3272 | −.25 |
| 4376 | 307 | −.8752999901771545 |
| 6116 | 1753 | −.5 |

The evaluation order is tap4889 + tap437 − tap2655 + tap3282 − tap3272 − tap307
− tap1753. Mix dry times **input-filtered** sample plus wet times this sum. For
the mixed value x, apply and clamp:

```
y = clamp(x*(28.274333953857422+x*x)
          /(28.274333953857422+9.42477798461914*x*x), -1, 1)
```

Dry configuration therefore retains the input filter and this nonlinear stage.
The wet fixture first becomes nonzero at sample 1,417. It contains impulses and
an early seeded random burst, so this is a fixture observation, not a pure
impulse-response or latency measurement.

The original reverb publication slice `0x37ce8..0x37d54` reads Time amount and
the preset Reverb multiplier. With u=clamp(amount*preset,0,1):

```
norm = sqrt(u*u + (1-u)*(1-u))
wet = u/norm; dry = (1-u)/norm; decay = min(2*u, float32(.9))
```

It also publishes the unmultiplied Time amount to the mirror field. This checks
the setter's reverb portion, including clamp endpoints; it does not check the
complete Time gesture, CV/ADC producer or linked-deck event semantics.

## Reproduction and remaining limits

Use Python with Unicorn and the existing Linux/glibc probe prerequisites, from
the repository root:

```sh
python3 firmware/Lubadh/probes/probe_plate_controls.py
python3 firmware/Lubadh/probes/probe_plate_process.py
python3 firmware/Lubadh/probes/probe_plate_state_transitions.py
```

`OUTPUT_COLOR_PIPELINE.md` checks the connected downstream callback slice. Whole
channel scheduling, preset/event producers, long decay characterization,
sample-rate conversion and physical timing remain separate requirements. Exact
agreement shares host float/FMA semantics and is not a physical ARM comparison.
Do not convert comparison counts into an overall parity score.
