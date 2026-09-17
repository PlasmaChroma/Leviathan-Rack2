# Octavia panel presence

The panel uses `res/icon/Octavia_6_States_256.png`: three columns, two rows,
256-pixel square cells, indexed left-to-right and top-to-bottom.

| Index | State | Automatic trigger | Agent meaning |
| --- | --- | --- | --- |
| 0 | Idle | Listening, no recent activity; browser preview | Ready / finished |
| 1 | Inspecting | GET or HEAD request | Examining the patch or its signals |
| 2 | Thinking | `/audio/analyze` or `/audio/compare` | Planning or preparing future actions |
| 3 | Working | Other write requests | Actively editing Rack |
| 4 | Error | HTTP status >= 400 or listener startup failure | Task encountered a problem |
| 5 | Sleeping | Bridge stopped | Inactive |

Automatic Thinking reflects local audio analysis. It does not detect remote
model reasoning. Agents can explicitly label their planning with the override.

## Timing and precedence

- Every visual transition is a 350 ms smooth crossfade. Retargeting during a fade
  starts from the current blend, without snapping or losing shared-image brightness.
- Automatic states have a minimum 2-second dwell, including Idle and HTTP errors.
- Request arrival and completion publish 2.5-second activity pulses; HTTP errors
  publish 4-second pulses. At each allowed change, current priority is Error,
  Thinking, Working, Inspecting, Idle. Expired pulses are not queued for replay.
- Status checks, presence calls, and Console housekeeping do not create normal
  activity pulses. Their HTTP errors still count as errors.
- An MCP override takes priority over all automatic HTTP activity. Explicit state
  changes bypass the automatic dwell but still crossfade.
- On expiry or explicit release, the icon crossfades to the current automatic
  state. Automatic tracking continues underneath an override.
- Stopping the server or failing startup clears the lease and bypasses the dwell:
  stopped shows Sleeping; startup failure shows Error until retry/stop.
- Starting or stopping clears prior pulses and overrides.

These are recent-activity indicators, not continuous job-progress indicators.
The state, hold timers and leases are transient: no serialization or undo entry.

## MCP control

`vcv_octavia_get_presence()` returns the target state, settled automatic state,
override (or null), remaining lease milliseconds, and timing constants.
`targetState` is the crossfade destination, not a claim that the fade is complete.

`vcv_octavia_set_presence({state, lease_ms})` sets a deliberate task state:

```json
{"state":"thinking","lease_ms":60000}
```

Use **thinking** while planning/preparing, **working** while applying edits, and
**inspecting** while examining the patch/signals. Hold the state across incidental
requests. Set it again to renew or change it; release with `{"state":"auto"}`
when the task ends. `idle`, `error`, and `sleeping` are also available.

The default lease is 30 seconds; accepted leases are integer milliseconds from
1,000 through 300,000 (5 minutes). Use a lease long enough for a planned async
operation and renew before expiry when needed. Disconnects cannot latch the icon
forever. The latest valid setter wins; this initial API has no client ownership
or lease token. Do not have multiple agents compete for the same panel presence.

The Rack plugin must be rebuilt/reloaded and the MCP server restarted/reconnected
before these new tools can be used. Older running bridges have no `/presence` route.

## HTTP contract

Both routes belong to the serving Octavia instance and use the existing optional
`X-Octavia-Token` authentication. No module ID is needed.

- `GET /presence`: read status.
- `POST /presence`: `{"state":"working","leaseMs":30000}`; renew by repeating.
- `POST /presence`: `{"state":"auto"}`; release.

States are lowercase. Unknown fields/states, duplicate JSON keys, noninteger or
out-of-range leases, and malformed bodies are rejected with 400. Bodies over
1,024 bytes are rejected with 413. Rejected writes preserve the existing lease.

## Read/write brightness overlay

The artwork follows the brighter RD/WR LED with a subtle additive brightness
pulse, up to 18% additional RGB at full activity. It uses the existing LED
brightness and 400 ms linear decay, including while an explicit presence override
is latched. Each new request refreshes its RD or WR light to full brightness.
The overlay reuses the currently crossfading cells, adds no tint, and preserves
image alpha. Simultaneous reads and writes do not double the boost. At rest the
artwork returns to normal brightness and remains cached.

## Rendering and validation

All cells share the existing raster mipmap cache and its NanoVG lifecycle
helpers. The widget retains no raw context-owned image handles. Weighted atlas
cells are added into the dedicated transparent status framebuffer. Only fades and active RD/WR brightness changes
invalidate it repeatedly; the finished image remains cached. State selection and
blending run on the UI thread, with no extra audio-thread work.

`tests/octavia_presence_spec.cpp` tests dwell, priority, lease renewal/expiry,
release, stop, concurrent pulses, and continuous interrupted fades.
`tests/octavia_presence_routes_spec.cpp` tests the real HTTP handlers on an
isolated ephemeral localhost port, including failed-write preservation.
Both run in `test-fast`. `MCP/tests/test_presence_tools.py` checks MCP argument
validation and route payloads using the MCP Python environment.
