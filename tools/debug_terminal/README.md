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
applies to all timing ranges, including module Process, Step, and Draw, in both
the Rich and plain terminal views. Keyboard controls work in native Windows and
POSIX terminals. The default is Range; the selection lasts for this terminal session.

The plugin sends measured arithmetic means alongside its existing range strings:
`"draw_us":"1.00-10.00","draw_us_avg":4.0`. Means use the recorded timing samples
since the previous telemetry submission (normally one second), rather than the
midpoint of the extrema. Empty intervals send `null`; Average mode displays `-`
for unavailable means, including packets from older plugins. Existing scalar
metrics keep their original meaning and display. Restart the terminal and load
the rebuilt plugin to use the new fields.

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
