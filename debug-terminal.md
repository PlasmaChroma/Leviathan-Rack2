# Debug Terminal Development Plan

## Goal

Build a general-purpose external debug terminal that can ingest JSON events from Rack modules over a socket and display live data in a terminal. The first producer target is `TD.Scope`, but the transport and server should be designed to support multiple modules and multiple simultaneous module instances.

## Non-Goals For First Pass

- No browser UI
- No bidirectional control protocol
- No remote/network deployment concerns beyond local development
- No attempt to stream every high-frequency internal signal
- No dependency on a specific shell environment such as WSL or MSYS2

## First-Pass Architecture

### Producer Side

- Rack plugin remains the socket client
- External Python app runs as the socket server
- Plugin sends newline-delimited JSON (`NDJSON`) over TCP
- Producer events are queued from hot paths and flushed from a background sender thread
- `TD.Scope` is the first module instrumented for this path

### Server Side

- Python server binds to configurable host/port
- Accepts multiple simultaneous client connections
- Reads one JSON object per line
- Normalizes/parses incoming events
- Maintains live in-memory state per module instance
- Renders a terminal dashboard that updates continuously

## Why TCP + NDJSON

- Cross-platform on Linux, Windows, macOS, MSYS2, and generally workable in WSL
- Easier framing and debugging than raw binary messages
- Human-readable during development
- Easy to prototype in Python
- Easy to evolve without reworking transport

## Cross-Platform Rules

- Default host should be `127.0.0.1`, not `localhost`
- Host and port must be configurable
- No Unix domain sockets or named pipes in first pass
- No assumptions that Rack and the server are running in the same environment
- Reconnection logic must tolerate the server not being present

## Event Schema

Use a stable envelope so multiple modules can coexist.

### Required Fields

- `plugin`: plugin name, e.g. `Leviathan`
- `module`: module type, e.g. `TDScope`
- `instance`: per-instance identifier for the current Rack session
- `stream`: logical stream name, e.g. `ui`, `draw`, `scope`
- `kind`: event category, e.g. `metric`, `state`, `trace`
- `ts`: timestamp in seconds
- `data`: object payload

### Example

```json
{"plugin":"Leviathan","module":"TDScope","instance":"0x12af80","stream":"ui","kind":"metric","ts":1712345678.12,"data":{"ui_ms":1.42,"rows":154,"density_pct":78,"zoom":0.82,"thickness":1.09}}
```

### First `TD.Scope` Payload

For the first pass, emit one coalesced metric event instead of many tiny events.

```json
{
  "plugin": "Leviathan",
  "module": "TDScope",
  "instance": "<instance-id>",
  "stream": "ui",
  "kind": "metric",
  "ts": 1712345678.12,
  "data": {
    "ui_ms": 1.42,
    "rows": 154,
    "density_pct": 78,
    "zoom": 0.82,
    "thickness": 1.09,
    "misses": 3
  }
}
```

## Terminal UX

The first version should optimize for readability and fast iteration, not beauty.

### Minimum View

- Header with bind address, connected clients, and event throughput
- Table grouped by `module` + `instance`
- Latest values for:
  - `ui_ms`
  - `rows`
  - `density_pct`
  - `zoom`
  - `thickness`
  - `misses`
- Age or freshness indicator for each row

### Interaction Goals

- Live refresh at a modest rate such as 5 to 15 Hz
- Keyboard quit
- Optional filter by module name later
- Optional raw event pane later

### Terminal Library Choices

Evaluate in this order:

1. `rich` + `live`
2. `textual` if richer interaction becomes necessary
3. Plain stdout fallback if library dependency becomes a problem

For the first pass, `rich` is the best balance.

## Python Server Structure

Suggested initial layout:

```text
tools/debug_terminal/
  server.py
  model.py
  render.py
  protocol.py
  README.md
  requirements.txt
```

### Responsibilities

- `server.py`
  - socket accept loop
  - per-client read handling
  - lifecycle and shutdown
- `protocol.py`
  - parse NDJSON lines
  - validate event envelope
  - normalize missing/invalid fields
- `model.py`
  - maintain latest state by module instance
  - track last update time
  - compute simple counters and freshness
- `render.py`
  - terminal dashboard rendering
  - row sorting/grouping

## Rack Plugin Integration Plan

### Phase 1: Transport Scaffold

- Add a small debug transport component in the plugin
- Add configuration for host/port/enable flag
- Add a background sender thread
- Add a thread-safe queue or ring buffer for debug events

### Phase 2: `TD.Scope` Producer

- Emit one throttled metric snapshot event from the UI side
- Start with existing debug values already computed:
  - UI draw time
  - row count
  - density percent
  - zoom
  - thickness
  - miss count
- Send at a throttled cadence rather than every draw call if needed

### Phase 3: Multi-Module Generalization

- Move event construction behind a small helper API
- Reuse the same transport from `TemporalDeck`
- Add stream names and instance tagging consistently

## Producer-Side Safety Requirements

- No blocking socket I/O from `process()`
- No direct socket writes from `draw()`
- UI and audio paths may only enqueue or overwrite lightweight metric snapshots
- Sender thread owns connect/reconnect/write behavior
- Queue overflow must fail soft:
  - drop oldest, or
  - coalesce latest metric snapshot

For `TD.Scope`, coalescing latest metrics is better than unbounded queue growth.

## Instance Identity

Need one identifier per live module instance.

First pass options:

- pointer-derived session-local ID
- incrementing runtime ID assigned at module construction

The ID only needs to be stable during one Rack run for now.

## Connection Lifecycle

### Expected Behavior

- If the Python server is not running, Rack should continue normally
- Sender thread retries connection periodically
- If the server disconnects, sender reconnects automatically
- Failed sends do not surface UI errors unless debug logging is enabled

### Suggested Defaults

- Host: `127.0.0.1`
- Port: `8765`
- Reconnect interval: 1 second

## Throttling Strategy

For the first pass, do not stream on every UI redraw if that creates noise.

Suggested initial policy:

- Maintain latest `TD.Scope` metric snapshot
- Sender thread publishes at 5 to 10 Hz
- If values change rapidly, only the latest snapshot is sent

This is enough for a readable terminal and avoids oversampling the transport.

## Development Phases

### Phase 0: Protocol and Tool Skeleton

- Finalize event envelope
- Create Python server skeleton
- Add simple terminal print mode before full dashboard

### Phase 1: Live Terminal Dashboard

- Add `rich`-based live table
- Accept multiple clients
- Track latest event per module instance
- Render `TD.Scope` metrics live

### Phase 2: Rack Transport

- Add plugin-side debug transport scaffold
- Add config toggles
- Add reconnection behavior
- Add `TD.Scope` metric publisher

### Phase 3: Hardening

- Handle malformed JSON safely
- Handle disconnects and stale rows
- Add simple logging for troubleshooting
- Add rate counters

### Phase 4: Generalization

- Add `TemporalDeck`
- Add optional stream filtering
- Add log-to-file mode

## Immediate Deliverables

1. Create `tools/debug_terminal/` Python app scaffold
2. Implement TCP server accepting NDJSON
3. Implement terminal live table for latest `TD.Scope` metrics
4. Add plugin-side debug transport skeleton
5. Hook `TD.Scope` metrics into the transport

## Open Questions

- Should host/port live in environment variables, a config file, or a Rack debug menu?
- Should the sender queue coalesce only by stream, or globally per module instance?
- Do we want the Python tool to show raw events from the start, or only structured summary rows?
- Should `TemporalDeck` and `TD.Scope` share one transport singleton inside the plugin, or should each module own its own sender state?

## Recommended Decisions For Now

- Use Python + `rich`
- Use TCP + NDJSON
- Use `127.0.0.1:8765`
- Make Rack the client and Python the server
- Use one coalesced `TD.Scope` metrics stream first
- Design the protocol for many modules from day one
