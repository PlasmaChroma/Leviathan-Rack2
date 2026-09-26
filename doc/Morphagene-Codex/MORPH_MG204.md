# MG204 Morph refinement

`src/ChimeraMorph.hpp` holds the recovered 22-stage table. The filtered control
is quantized with `floor(control * 4096)`, capped at 4095, and the stage is
`min(21, int((adc / 4096.f) * 33.99f))`. This replaces the old continuous
density curve; existing knob/CV routing and saved parameter values are retained.

Representative finite-Gene settings:

| ADC range | Approximate knob range | Launch interval / Gene duration |
| --- | --- | --- |
| 0–120 | 0–2.95% | 2 (one Gene, then an equal gap) |
| 121–241 | 2.95–5.91% | 3/2 |
| 242–361 | 5.91–8.84% | 4/3 |
| 362–482 | 8.84–11.79% | 1 (contiguous) |
| 1085–1205 | 26.49–29.44% | 1/2 (two Genes) |
| 1808–1928 | 44.14–47.09% | 1/3 (three Genes) |
| 2531–4095 | 61.79–100% | 1/4 (four Genes) |

The existing default knob position remains 1/6; under the recovered mapping
this is stage 5, a 3/4 launch interval. Contiguous playback is now around 10%.

The unclocked finite-Gene scheduler adds the rational denominator each output
sample and subtracts `duration * numerator` on launch. Remainders survive
launches; a fixed setting has no floating-point timing drift. Control changes
rescale fractional progress without resetting the four active voice slots.
Their age, captured region, window, pan, and signed pitch ratio remain intact.
Finite-Gene duration and cadence use output samples, including at Stop. Source
motion separately uses the live Vari-Speed rate times each voice's ratio.

Gene Size's existing whole-splice bypass (ADC <= 199) remains a traversal mode:
it completes after source travel across the splice and holds at Stop. It is
not converted into a finite output-time Gene by this Morph refinement. Its
existing floating-point traversal scheduler remains separate from the recovered
finite-Gene scheduler. This preserves the preceding Gene Size implementation.

Continuous Morph is separately updated at 48 kHz using the recovered
`value += (adc / 4096.f - value) * 0.01f`. This follows the existing filtered
knob/CV input. Recovered thresholds are 0.5 for pan and 0.6 for pitch.
The **provisional** pan depth is `(value - .5) / .5`, clamped to [0,1], with
the existing uniform seeded pan draw and equal-power balance. The **provisional**
pitch depth is `(value - .6) / .4`, clamped to [0,1], used as the probability
of selecting a secondary slot's configured ratio. Each onset consumes two
PRNG draws and latches its choices. These distributions and the preserved
window/declick law are not claimed to be bit-exact firmware behavior.

Default ratios are 1, 2, 3/2, and 4/3. Existing `mcr1`–`mcr3` parser/UI options
still replace the last three, including negative ratios and magnitude limits
1/16 through 16. Saved explicit ratios remain authoritative.

Clock Gene Shift/Stretch trajectory, edge, timeout and transition handling are
preserved. `stretchCandidate()` exposes the recovered `launchFactor <= .5`
boundary (stage 9 onward) for hybrid `ckop`; the clock path can override the
rational free-running scheduler. The full clock state machine remains
**provisional**, not a claim of recovered MG204 equivalence.

`test-chimera-morph` checks every ADC bin, million-sample timing runs for all
stages, smoothing, thresholds, seeded ratios, finite DSP cadence across speeds,
duration scaling, live voice preservation and an audio allocation trap.
`test-chimera-phase4` includes it plus the existing grain and Gene Size suites.
