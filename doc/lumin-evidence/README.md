# Preserved Lumin evidence

These JSON files are byte-for-byte copies of selected historical summaries and
the installation record. `manifest.json` records their original local paths,
sizes and SHA-256 hashes. Historical source paths, flags and installed binary
hashes describe the runs at that time, not the current cleaned checkout.

Raw CSVs, logs and images remain under ignored `doc/benchmarks` and `work`.
These summaries preserve the findings and capture hashes for a source commit;
they do not replace the raw inputs for reanalysis. No captures were deleted.

- `reinstall-baseline-summary.json`: existing renderer before the private path.
- `full-stroke-summary.json`: successful isolated stroke optimization.
- `private-nanovg-live-summary.json`: failed live performance integration.
- `historical-installation.json`: package identity and original experiment setup.
- `shader-prototype-summary.json`: failed first shader image/performance screening.

`cleanup-validation.json` is a new cleanup record, not a historical capture copy.
It records the rebuilt baseline, focused checks and preserved experimental failure.

`batch-execution-summary.json` records the subsequent shared-pass experiment:
identical grouped/separate shader images and reduced surface CPU submission cost.
It does not establish a live Rack gain or shader parity with NanoVG.
