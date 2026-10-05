# Persistent connected output coloration

`probe_output_color_pipeline.py` executes the continuous original callback slice
`0x48454..0x484d8`: TapeFilter → signed-square wear → TapeAllpass diffuser →
MonoPlate → disabled FilePreview → output SoftClipper. Independent state models
in `output_color_model.py` and `plate_model.py` process the same supplied mixed
audio vector. States persist across blocks and live amount changes.

The three sequences pass **480 blocks, 38,196 samples and 200,580 numeric
comparisons**, with zero maximum error. Execution covers 1,632 distinct
instruction addresses, including initialized effects and reverb publication.
Output vectors, controls, ELF hash and coverage are stored in
`probes/output_color_pipeline_probe_results.json`.

| Fixture | Samples | Block size | Output clipping |
|---|---:|---|---|
| clean | 8,858 | 7 / 32 / 128 cycle | knee .3, compensation 0 |
| colored_knee | 20,480 | 128 | knee .3, compensation .2 |
| colored_rational | 8,858 | 7 / 32 / 128 cycle | rational branch, compensation .2 |

The colored fixtures alternate age/wear/hysteresis controls every 20 blocks and
change reverb amount every 40 blocks (.125, .5, .75, 1), using the original
reverb publication slice. Input includes sine bursts, impulses and intervening
silence. The clean fixture sets all effect amounts to zero; the plate's input
filter and final saturation still operate, as described in the plate report.

Every sample is compared at all five stage boundaries. Each block also checks
the ten TapeFilter histories, eight plate filter histories and two plate phases;
all four diffuser write indices are asserted separately. Thus agreement of final
output alone cannot hide an opposing mismatch in intermediate stages.

## Recovered connection and state rules

TapeFilter uses five one-poles. Two high-pass branches receive the same input;
their .6-weighted sum feeds two low-pass branches. A third low-pass receives
.2 times input. Their outputs combine as .6*lowA + .6*lowB + lowC, then blend
with dry by age. Some .6/.2 operations execute at double precision before
float32 storage; the reference retains those boundaries. Coefficients are
read from the executed constructor rather than claimed as a recovered
sample-rate-aware cutoff law.

Wear blends input with its signed square. The diffuser has lengths
68/159/251/375 and persistent signed writes:

```
writes: x; x-d0; x+d0-d1; x+d0+d1-d2
wet = .175*(x+d0+d1+d2+d3)
output = x + hysteresis*(wet-x)
```

Each d is the old current cell, with the reference preserving float32 order.
These are the actual signed diffuser equations, not conventional allpass
substitutions. The subsequent plate contract is in `PLATE_PROCESS_FINDINGS.md`.
Both rational and knee output clipper branches run in connected sequences;
`output_color_model.py` records their rounded equations.

## Scope and reproduction

```sh
python3 firmware/Lubadh/probes/probe_output_color_pipeline.py
```

Requirements are the same offline Unicorn/glibc setup as the plate probes.
No appliance executable or system script is launched. Only the explicit finite
floor service and existing memory services supplement original instructions.

Input is a supplied mix vector, **not a tape-rendered vector**. The earlier
persistent playback/record probes and this coloration sequence are not yet one
whole callback test. FilePreview is disabled on both decks; active preview and
file I/O are outside this result. Age/wear/hysteresis and clipper parameters are
explicit fixtures, while reverb publication uses original instructions. Full
input coloration, event/preset producers, routing and same-tape recording remain
integration work. No analog, hardware timing or arbitrary Rack-rate equivalence
is claimed.
