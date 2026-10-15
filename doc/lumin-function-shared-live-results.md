# Shared-boundary live results — 10 September 2026

The installed stripped DLL matches the packaged candidate (SHA-256 in the evidence
JSON). The 19:11:13 capture provides the pilot-off baseline; the 19:11:59 capture
contains the new pilot-on test. Both use the same module ID, render mode 0,
tracer enabled/mode 2, Lumen adapter enabled and no forced Halo NanoVG mode.
Both captures have zero pilot fallbacks. Shared phase costs appear under CH1,
consistent with the new module-level accounting; CH4 zero phase time is expected.

Exclude the first 120 rows of each file to remove initial startup. Compare exact
point-rebuild counts, rather than all active frames pooled together:

| Cohort | Baseline n | Shader n | Baseline module Draw median | Shader module Draw median | Baseline preview median | Shader preview median |
|---|---:|---:|---:|---:|---:|---:|
| One preview rebuilt | 361 | 230 | 340.4 us | 573.3 us | 72.7 us | 325.4 us |
| Both previews rebuilt | 380 | 357 | 376.1 us | 675.0 us | 98.3 us | 398.1 us |

For both-preview updates, contour medians are 37.55 us baseline and 327.1 us
shader. History medians are 23.4/27.2 us, with 12 trails and median submitted
point counts 378/382. This supports a substantially better workload comparison
than the first live capture, but separate runs are not an identical replay of
parameters, zoom or host load. CSV does not establish all those conditions.
The module median difference is 298.9 us (about 79% higher for the shader cohort).
This is an observed cohort difference, not a causal universal slowdown factor.

In the shader dual-rebuild cohort, per-row summed batch phase medians are:

| CPU phase | Median |
|---|---:|
| Capture | 21.7 us |
| Shader baseline setup | 7.9 us |
| Ensure/bind/clear target | 114.0 us |
| Shader callback | 43.7 us |
| Restore | 122.0 us |
| Capture plus restore, summed per row | 146.2 us |

Medians are not additive. CPU scopes can absorb deferred driver work; callback
is not GPU execution time. The remaining cost cannot be attributed exclusively
to capture/restore. Target setup is now a major measured scope too.

The earlier separate-boundary live dual-update cohort had contour median
482.65 us and state median 427.5 us. The new corresponding values are 327.1
and 146.2 us. This suggests improvement versus the previous candidate, but the
captures are separate runs and this is not a controlled before/after speedup.
The shared boundary has not made the custom path competitive with NanoVG.

Next experiment: retain the same execution path under representative Rack load
and compare no-op/clear-only/shader submissions, with total timing and delayed
GPU queries. Distinguish unavoidable target/composition work from state operations
and waits that merely move between scopes. Deep research is investigating better
host integration routes in parallel. Do not promote the candidate as a production
performance win or compensate by further reducing visual quality.

Durable metrics and CSV hashes: `lumin-evidence/function-shared-live-results.json`.
Raw copied CSVs: local ignored `doc/benchmarks/lumin-shared-live-20260910/`.
No runtime source, installation or settings changed during this analysis.
