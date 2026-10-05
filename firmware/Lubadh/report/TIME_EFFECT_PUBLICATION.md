# Time effect publication and clipping producers

`probe_time_effect_publication.py` executes the complete original
`Channel::setTime(int,bool)` at `0x37a44` with effect selector 2, preset TimePot=2
and forced publication. It compares all ten factory presets and two clamp
fixtures at thirteen raw inputs, including negative and above-4095 values.
Factory scalar fields are injected at recovered offsets; the preset file loader
is not executed.

The result passes **156 publication cases, 42 gating cases and 72 preset-update
clipper cases: 2,879 exact assertions**, covering 685 original instruction
addresses. It also executes the constructor's output clipper initialization.
Fixtures, coverage and archived ELF hash are in
`probes/time_effect_publication_probe_results.json`.

## Checked effect law

The setter saturates its signed integer argument to [0,4095]. In the checked
effect path, Time amount is float32(raw/4095). Each of these amounts is
clamp(Time times preset field,0,1):

| Preset field | Channel-relative destinations | Role |
|---|---|---|
| WowFlutterDepth | 18984 | Flutter amount publication |
| CrinkleDepth | 18988 | Crinkle amount publication |
| TapeAge | 6648, 12804 | Input and output TapeFilter amounts |
| Wear | 6748, 12904 | Input and output signed-square wear |
| Hysterisis | 6752, 12908 | Input and output diffuser amounts |

The misspelled preset key is retained. Publishing modulation depth does not
validate the flutter/crinkle random process itself. Both sides receive the same
amount law, but their processing locations and persistent histories are separate.

Write clipping at Channel+25160 receives knee=clamp(preset.Knee,0,1) and
compensation=clamp(Time times preset.Compensation,0,1). Knee does not scale with
Time. Rational clipping is selected by knee<=double(.01); its gain is
1+compensation. Otherwise gain is
1+compensation*(2/(1+knee)-1), preserving the firmware float32/FMA boundaries.
The rational branch leaves the stored knee word untouched and selects behavior
with its separate mode byte; do not interpret that stale word as active knee.

Reverb publication follows `PLATE_PROCESS_FINDINGS.md`: clamp(Time*Reverb,0,1),
equal-power wet/dry and decay=min(2*u,float32(.9)). Time amount also reaches the
mirror at +40; saturated raw control reaches the relevant last-control fields.

## Gating and a meaningful producer difference

The setter compares saturated raw control with the last accepted raw control.
At distance<=29, unforced calls return before the Channel-mode check; forced
calls can publish. At distance>29, Channel mode 0 promotes the request to forced
publication. For nonzero Channel mode with an unforced request, the checked gate
diverts to `0x37d6c`. The probe stops before that alternate path; its clock/control
behavior remains unrecovered. These are execution conditions, not inferred
names for the Channel mode enum.

The preset-update write-clipper slice `0x3e138..0x3e1a4` applies the same knee
law but caps compensation at **2**, whereas Time effect adjustment caps it at
**1**. Both paths were executed with factory fields and clamp fixtures. This
distinction can matter to unusually large preset values. A literal compatibility
mode must preserve it; normalizing both caps would be a deliberate design change.
Only that preset-update slice is checked here, not its complete reset/event graph.

## Final output defaults

The constructor slice `0x3d7e4..0x3d844` uses the real stack-loaded output clipper
pointer. It calls setParams with knee=.5 and compensation=.75, publishing gain
**1.25** and knee mode at Channel+25172. Time effect adjustment targets the write
clipper rather than this final output clipper. Earlier output-chain tests used
explicit knee/rational branch fixtures; those values were not production defaults.

## Reproduction and limits

```sh
python3 firmware/Lubadh/probes/probe_time_effect_publication.py
```

This uses the existing offline Unicorn/glibc environment. The original appliance
is never launched. Complete preset update, other Time selectors, linked deck
forwarding, ADC/CV event producers, UI gestures and analog calibration remain
separate contracts. These checks advance scalar production into the DSP chain;
they do not close the full control or modulation specification.
