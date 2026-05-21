# CODEX OSC Lane Contract

## Scope

This is the narrow contract for code that emits or receives SDT-SL control data over OSC.

Use this when modifying:

- `SDT/sdt_sl_conductor_v0.py`
- future Python conductor versions
- future Rack-side SDT receiver modules
- test harnesses that simulate the conductor or receiver

## Transport

Default sender:

```text
host: 127.0.0.1
port: 7001
protocol: OSC over UDP
namespace: blank
```

Default addresses:

```text
/ch/1
/ch/2
/ch/3
/ch/4
/ch/5
/ch/6
/ch/7
/ch/8
```

Each message should carry one float.

## Value Range

Use Rack-friendly volts by default:

```text
continuous lanes: 0.0..10.0
gate lanes:       0.0 or 10.0
```

The conductor may compute internally in normalized `0.0..1.0`, but transport output should be multiplied by `--scale`, default `10.0`.

If a specific OSC target expects normalized values, use `--scale 1.0` rather than changing program semantics.

## Timing

Default update rate: `30 Hz`.

The sender should avoid bursty sleeps and should keep CPU cost low. Exact sample accuracy is not required because this bridge is for CV control and live experimentation, not audio-rate modulation.

Gate pulses must last long enough to be observed by Rack at the configured update rate. A practical minimum is one update frame; two frames are safer for visual/debugging tests.

## Lane Semantics

### `PRESSURE`

Range: `0.0..10.0`

Controls density, force, ritual weight, and accumulated intensity.

Implementation tendency:

- smooth ramps
- slow accumulation
- occasional plateaus
- avoid high-rate noise

### `BREATH`

Range: `0.0..10.0`

Controls phrase envelope and exhale/inhale motion.

Implementation tendency:

- smooth curves
- triangle or sine-like phrase arcs
- slow attack and release

### `IDENTITY`

Range: `0.0..10.0`

Controls stable motif anchor, pitch center, cutoff center, loop memory, or recognizable presence.

Implementation tendency:

- stable baseline
- small periodic variation
- avoid random wandering unless the program explicitly mutates identity

### `CONSTRAINT`

Range: `0.0..10.0`

Controls withholding, clamp, law, blocked motion, or restricted movement.

Implementation tendency:

- higher value means more restriction
- may inversely shape modulation depth, slew openness, probability, or filter width

### `DRIFT`

Range: `0.0..10.0` for v0.

Controls wobble, detune, slippage, mutation, and instability.

Implementation tendency:

- low-frequency motion
- optional stepped perturbation
- avoid audio-rate randomness in the conductor

Later versions may add bipolar `-5.0..5.0` drift mode, but v0 should stay unipolar.

### `RESOLUTION`

Range: `0.0..10.0`

Controls permission to settle, open, release, or resolve.

Implementation tendency:

- high value allows cadence, tonal settling, wet/dry opening, or finalization
- low value denies closure
- `no_fin` should never reach full release

### `THRESHOLD`

Range: `0.0` normally, `10.0` during pulses.

Controls boundary-crossing events.

Implementation tendency:

- use as gate or trigger source
- keep short and deliberate
- excellent for validating Rack wiring

### `PHRASE`

Range: `0.0` normally, `10.0` during pulses.

Controls phrase, section, or ritual block changes.

Implementation tendency:

- lower frequency than threshold in most programs
- useful for resets, sample-and-hold clocks, sequencer advance, or scene changes

## Program Profiles

### `alethe`

Expected monitor profile:

- pressure: low to mid
- breath: smooth and obvious
- identity: stable
- constraint: low
- drift: low
- resolution: high
- threshold/phrase: gentle periodic gates

### `no_fin`

Expected monitor profile:

- pressure: rising or held high
- breath: slow
- identity: persistent
- constraint: high
- drift: mid and increasing
- resolution: low, with false-cadence bumps only
- threshold/phrase: sparse

### `threshold`

Expected monitor profile:

- mostly quiet or restrained
- threshold pulses are obvious
- phrase gates mark larger cycle boundaries
- useful for testing `/ch/7` and `/ch/8`

### `edict`

Expected monitor profile:

- pressure: monolithic
- breath: minimal or slow
- identity: fixed
- constraint: high
- drift: low
- resolution: controlled, usually low-to-mid
- gates: sparse, hard, deliberate

## Dry Run Output

Dry-run mode should print the same values that would be sent over OSC.

Preferred monitor order:

```text
PRESSURE BREATH IDENTITY CONSTRAINT DRIFT RESOLUTION THRESHOLD PHRASE
```

Print once per second, not every update frame.

## Shutdown

On normal exit, Ctrl+C, SIGTERM, or handled exception, send a zero state to all eight channels.

The zero state must use the configured host and port, not hard-coded fallback values.

