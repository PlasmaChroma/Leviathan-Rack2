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

Each line must be one JSON object.

Example:

```json
{"plugin":"Leviathan","module":"TDScope","instance":"0x12af80","stream":"ui","kind":"metric","ts":1712345678.12,"data":{"ui_ms":1.42,"rows":154,"density_pct":78,"zoom":0.82,"thickness":1.09,"publish_seq":12345,"draw_seq":12340,"draw_calls":912}}
```

## Notes

- Binds to `127.0.0.1` by default.
- Accepts multiple simultaneous clients.
- Uses `rich` for the live table when installed.
- Falls back to periodic plain-text rendering when `rich` is unavailable.
