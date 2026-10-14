# Hydra

Hydra is a granular texture processor derived from **Beads** by Emilie Gillet
(Mutable Instruments), used under the MIT license. Upstream source:
`../eurorack/beads` (DSP in `dsp/`, tables in `resources.cc`).

## Layout (`src/hydra/`)

- `Hydra.cpp` — Rack module + widget (params, CV, buttons, lights, menu, debug telemetry).
- `HydraEngine.{hpp,cpp}` — Rack-independent engine. Wraps Beads `Processor` and
  `Synth`, applies the hardware pot laws, runs the DSP in 4 kHz blocks at the
  media rate (48k / 32k / 24k) and resamples to/from the host rate.
- `beads/` — vendored Beads DSP (`dsp/`) and generated tables (`resources.cc`).
- `stmlib/` — vendored subset of Mutable Instruments `stmlib` (MIT). `dsp.h`
  was patched so `Sqrt`/`Clip16` use portable C on non-ARM targets.
- Tests: `tests/hydra_engine_spec.cpp` (in `test-fast`).
- Panel: `res/hydra.svg` is the master (dev panel); regenerate runtime assets
  with `python3 tools/split_svg_labels.py res/hydra.svg --overwrite` and
  `make generate-panel-anchor-atlas`.

## Status / known gaps

- Dev panel only; final UI layout is undecided.
- Host <-> media resampling is linear with a 2-pole anti-alias/image filter
  when the media rate is below the host rate. Replace with a polyphase SRC if
  the 24 kHz / 32 kHz qualities need to be cleaner.
- Input gain control, normalization detection and freeze-buffer backup from the
  hardware are not implemented (fixed 5 V = full scale, Rack patched-state
  checks, no backup).
- Wavetable synth mode triggers after 10 s with both inputs unpatched (menu
  toggle); hardware also resets its gain/limiter state on entry.
- Module is unreleased (`hidden: true` in `plugin.json`); IDs may still change.
