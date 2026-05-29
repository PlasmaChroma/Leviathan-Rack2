# SDT-SL Conductor v0

`sdt_sl_conductor_v0.py` is a small SDT-SL control runtime that sends 8 OSC lanes to VCV Rack (typically through trowaSoft `cvOSCcv`).

It is a CV conductor, not a full SDT language generator.

## What It Does

- Runs a selected SDT-SL program.
- Emits 8 lanes as OSC floats.
- Supports probe/calibration modes.
- Supports numbered or named OSC addresses.
- Supports human monitor output and JSON monitor output.
- Sends zero values on shutdown so Rack is not left hot.

## cvOSCcv Routing Assumptions

Common setup:

- Rack module output/send port: `7000`
- Rack module input/listen port: `7001`
- Python sends to `127.0.0.1:7001`

Port convention:

- The VCV module input/listen port is where Python sends.
- If Rack says input is `7001`, run Python with `--port 7001`.

## Install (Windows PowerShell)

```powershell
py -m pip install python-osc
```

## Quick Start (Windows PowerShell)

```powershell
py SDT\sdt_sl_conductor_v0.py --program alethe --port 7001 --bpm 60
```

If `python` is your launcher:

```powershell
python SDT\sdt_sl_conductor_v0.py --program alethe --port 7001 --bpm 60
```

## Lane Map

| Lane | Key | Kind | Meaning |
|---:|---|---|---|
| 1 | `pressure` | `continuous` | intensity and accumulated force |
| 2 | `breath` | `continuous` | phrase envelope and breath arc |
| 3 | `identity` | `continuous` | stable motif anchor |
| 4 | `constraint` | `continuous` | restraint and narrowing |
| 5 | `drift` | `continuous` | slippage and instability |
| 6 | `resolution` | `continuous` | permission to settle/release |
| 7 | `threshold` | `gate` | boundary crossing trigger |
| 8 | `phrase` | `gate` | phrase/section trigger |

Detailed lane info:

```powershell
py SDT\sdt_sl_conductor_v0.py --list-lanes
```

## Program List

Show all programs:

```powershell
py SDT\sdt_sl_conductor_v0.py --list-programs
```

Current set includes:

- Musical: `alethe`, `no_fin`, `threshold`, `edict`
- Probe: `probe_ramp`, `probe_gates`, `probe_all`

## Addressing Modes

Default addressing is numbered:

- `/ch/1` .. `/ch/8`

Print active addresses:

```powershell
py SDT\sdt_sl_conductor_v0.py --print-addresses
```

Named mode example:

```powershell
py SDT\sdt_sl_conductor_v0.py --address-mode named --print-addresses
```

Namespace example:

```powershell
py SDT\sdt_sl_conductor_v0.py --namespace sdt --print-addresses
```

Examples:

- Numbered with namespace: `/sdt/ch/1`
- Named with namespace: `/sdt/pressure`

## Probe Workflow

1. Run probe mode:
```powershell
py SDT\sdt_sl_conductor_v0.py --program probe_all --port 7001 --bpm 60
```
2. Patch lane 1 to scope.
3. Verify stable voltage and gate pulses on lanes 7/8.
4. Switch to `probe_ramp` to verify lane mapping one lane at a time.
5. Return to musical program after patch validation.

## Useful Commands

```powershell
py SDT\sdt_sl_conductor_v0.py --dry-run --program alethe --bpm 60
py SDT\sdt_sl_conductor_v0.py --dry-run --program probe_all --bpm 60
py SDT\sdt_sl_conductor_v0.py --json-monitor --program alethe --port 7001
py SDT\sdt_sl_conductor_v0.py --program alethe --port 7001 --namespace sdt --address-mode named
```

## Troubleshooting

No movement in Rack:

- Check Rack listen/input port and match `--port`.
- Confirm IP is `127.0.0.1`.
- Use `--print-addresses` and verify cvOSCcv mapping.
- Run `--program probe_all` to remove musical ambiguity.

Wrong port:

- If Rack listens on `7001`, Python must send to `7001`.

Namespace mismatch:

- If Rack expects `/ch/N`, avoid namespace and use numbered mode.
- If using namespace in Rack, set matching `--namespace`.

Values too small on scope:

- Increase `--scale` (default is `10.0`).
- Verify downstream module expects unipolar 0..10V.

Rack not receiving OSC:

- Confirm `python-osc` is installed.
- Confirm firewall/network policy allows local UDP loopback.
- Test with `--dry-run` first, then send live.
