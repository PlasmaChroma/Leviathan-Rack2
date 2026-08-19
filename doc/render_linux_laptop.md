# Wyrm rendering results: Linux laptop

## Combined waveform and body pass

Captured: 2026-08-18

Commit under test: `6becab4` (`waveform fill and analytical body rendering in
one fullscreen shader -- needs a log analysis`)

Capture file:
`~/.local/share/Rack2/Leviathan/Wyrm/wyrm_draw_1_20260818_185811_0.csv`

The commit evaluates the waveform fill and analytical body in one fullscreen
SHDR fragment pass. For combined samples, `gpu_body_us` measures the complete
pass and `gpu_wave_us` is zero. Results below compare that combined time with the
same-size squared-distance baseline sum of `gpu_wave_us + gpu_body_us`.

### Capture validation

The capture used Rack zoom 2.378 and contains all four requested SHDR states at
the expected framebuffer sizes. Analysis retained rows with:

- `gpu_sample_valid=1`
- `gpu_sample_combined=1`
- `gpu_sample_mode=2`
- positive `gpu_sample_slither`
- the expected framebuffer width and height

GPU sample sequences were deduplicated. There was one repeated sequence row.
The retained sample counts were 432--494 per state.

### GPU results

| State | Samples | Baseline median | Combined median | Median change | Baseline p95 | Combined p95 | p95 change |
|---|---:|---:|---:|---:|---:|---:|---:|
| Collapsed oscillator, 489x445 | 432 | 146.54 us | 209.01 us | 42.6% slower | 353.89 us | 333.44 us | 5.8% faster |
| Collapsed envelope, 489x445 | 483 | 207.60 us | 255.68 us | 23.2% slower | 392.03 us | 370.26 us | 5.6% faster |
| Expanded oscillator, 977x509 | 494 | 283.75 us | 372.40 us | 31.2% slower | 557.46 us | 561.20 us | 0.7% slower |
| Expanded envelope, 977x509 | 439 | 348.67 us | 396.51 us | 13.7% slower | 554.01 us | 594.17 us | 7.2% slower |

The median combined-pass GPU time regressed in every state. The two collapsed
states had modestly better p95 values, but those tail improvements do not offset
the consistent 13.7--42.6% median regressions. The expanded states also had
slightly worse p95 values.

### Conclusion

The combined fullscreen shader does not produce a repeatable end-to-end GPU
benefit on this Linux laptop. It fails the acceptance criterion in
`wyrm_render.md` and should not be retained on performance grounds. Restore the
separate analytical-waveform and analytical-body passes while retaining the
squared-distance body optimization and GPU CSV trace.

Visual equivalence still needs an explicit comparison if the combined shader is
kept temporarily for further investigation, but a visual pass cannot reverse
the timing conclusion above.
