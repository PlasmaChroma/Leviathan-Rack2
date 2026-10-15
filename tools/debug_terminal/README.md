# Debug Terminal

Minimal TCP/NDJSON debug server for Leviathan module telemetry.

## Run

```bash
python3 tools/debug_terminal/server.py
```

Optional arguments:

```bash
python3 tools/debug_terminal/server.py --host 127.0.0.1 --port 8765 --refresh-hz 8
```

## Protocol

Press **A** to toggle the displayed **Range / Average** timing control. This
applies to all timing ranges, including module Process, Step, Draw, and DrawLayer, in both
the Rich and plain terminal views. Keyboard controls work in native Windows and
POSIX terminals. The default is Range; the selection lasts for this terminal session.

The plugin sends measured arithmetic means alongside its existing range strings:
`"draw_us":"1.00-10.00","draw_us_avg":4.0`. Means use the recorded timing samples
since the previous telemetry submission (normally one second), rather than the
midpoint of the extrema. Empty intervals send `null`; Average mode displays `-`
for unavailable means, including packets from older plugins. Existing scalar
metrics keep their original meaning and display. Restart the terminal and load
the rebuilt plugin to use the new fields.

The basic columns are `Process`, `Step`, `Draw`, and `DrawLayer`, in that order.
`Draw` measures normal module `draw()` calls. `DrawLayer` measures the sum of
module `drawLayer()` calls between UI steps, including shadow/light passes and
any additional calls. Detached layer widgets contribute to their owning module.
Child calls already included by a parent timer are not counted again.

Process timing is collected on every normal process invocation while Dragon King
debug mode is enabled; no periodic sampling divider is used. Disabling debug
mode disables collection. Background-worker work remains outside these basic
callback timings.

Wyrm and Puffy sum normal Step and Draw work per UI cycle as well. Wyrm has one
editor, moved between docked and detached parents; the main panel still runs in
both modes. Puffy's roaming movement and rendering contribute to its owner's
Step and Draw. Contributions are summed before calculating min/max/mean, so
opposing spikes in separate widgets cannot manufacture a larger maximum.
These totals are finalized at the next module step, including cycles that switch
editor/avatar ownership. The existing audio min/max/mean snapshot behavior is
unchanged.

A cycle with layer durations 2, 5, and 3 microseconds contributes one sample of
10 microseconds. A second cycle totaling 4 gives `4.00-10.00` in Range mode and
`7.00` in Average mode. Reporting midway through a cycle leaves it pending until
the next step. Cycles with no layer calls contribute no sample; disabled debug
mode clears pending layer data. A measured zero remains a valid sample.
The terminal labels this column `DL (us)`. The wire fields are `draw_layer_us`
and `draw_layer_us_avg`.

GL work executed inside `step()` remains in Step. These are CPU elapsed times,
not asynchronous GPU completion times. Timing bookkeeping/reporting may sit
outside the measured region. This separation replaces Chimera's former combined
draw/layer number and Integral Flux's former addition of GL-in-step time to Draw.

Each line must be one JSON object.

Example:

```json
{"plugin":"Leviathan","module":"TDScope","instance":"0x12af80","stream":"ui","kind":"metric","ts":1712345678.12,"data":{"process_us":0.0,"step_us":93.4,"draw_us":1420.7,"rows":154,"density_pct":78,"zoom":0.82,"thickness":1.09,"publish_seq":12345,"draw_seq":12340,"draw_calls":912}}
```

## Notes

- Binds to `127.0.0.1` by default.
- Accepts multiple simultaneous clients.
- Uses `rich` for the live table when installed.
- Falls back to periodic plain-text rendering when `rich` is unavailable.
