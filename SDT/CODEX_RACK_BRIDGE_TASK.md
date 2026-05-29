# CODEX Rack Bridge Task Brief

## Purpose

This document aligns Codex agents with the SDT-SL-to-VCV-Rack task.

The immediate goal is not to build audio, lyrics, or a full SDT language engine. The goal is to create reliable code paths that let an external SDT-SL conductor drive VCV Rack control behavior through clear, testable control lanes.

Treat SDT-SL as a procedural control system:

- pressure over time
- breath-shaped motion
- identity persistence
- constraint and withholding
- drift and mutation
- resolution permission or denial
- threshold crossings
- phrase gates

Do not treat SDT-SL as a dictionary, cipher, word translator, or hidden symbolic code.

## Current Bridge Target

The first bridge is an external Python conductor sending OSC into Rack through trowaSoft `cvOSCcv`.

Rack-side assumptions from `SDT/SDT_SL_context.md`:

- cvOSCcv OUT: `7000`
- cvOSCcv IN: `7001`
- Python sends to `127.0.0.1:7001`
- cvOSCcv namespace is blank
- OSC addresses are `/ch/1` through `/ch/8`

The existing draft conductor is `SDT/sdt_sl_conductor_v0.py`.

Important current mismatch: Rack/cvOSCcv is already expected to be at `7000/7001`, but the draft Python script still defaults to port `9000` and sends normalized `0.0..1.0` values. The Codex target contract is `7001` with lane values scaled for Rack CV, normally `0.0..10.0` floats unless a specific backend requires normalized values.

## Lane Contract

The eight lanes are stable for v0 work:

| Lane | OSC Address | Name | Meaning |
|---:|---|---|---|
| 1 | `/ch/1` | `PRESSURE` | intensity, density, accumulated force |
| 2 | `/ch/2` | `BREATH` | body-like phrase envelope and respiratory motion |
| 3 | `/ch/3` | `IDENTITY` | persistent motif anchor or stable control center |
| 4 | `/ch/4` | `CONSTRAINT` | restraint, law, blocked motion, narrowing |
| 5 | `/ch/5` | `DRIFT` | instability, detune, mutation, slippage |
| 6 | `/ch/6` | `RESOLUTION` | permission to settle, release, or open |
| 7 | `/ch/7` | `THRESHOLD` | short event gate for boundary crossings |
| 8 | `/ch/8` | `PHRASE` | short event gate for section or phrase changes |

Continuous lanes should normally emit `0.0..10.0`.

Gate lanes should normally emit `0.0` and pulse to `10.0`. Use short pulse windows that remain visible at the selected update rate.

## Program Targets

The conductor should provide these named programs as stable test surfaces:

- `alethe`: gentle field-state, smooth breath, low-to-mid pressure, low drift, high resolution permission.
- `no_fin`: resolution denied, rising pressure, high constraint, persistent identity, false-cadence bumps but no full release.
- `threshold`: mostly quiet, then clear threshold and phrase crossings for gate testing.
- `edict`: sparse hard events, low drift, high constraint, monolithic pressure, slow deliberate phrase changes.

Program behavior should be understandable from lane monitors and dry-run output without needing Rack open.

## Canon Guardrails

Preserve the SDT-SL concept:

- Keep it operator-driven and non-vocal.
- Avoid spoken SDT roots, phoneme grammar, sung vocabulary, or direct symbols.
- Do not create a word-code.
- Do not make lane names pretend to be semantic translation.

The Rack bridge should convert SDT-SL behavior into voltages. It should not attempt to define SDT canon.

## Repo Constraints

This repository is primarily developed for Windows VCV Rack plugin builds.

In WSL:

- Edit code and docs.
- Run focused local checks and fast tests where useful.
- Do not treat full plugin link failures as regressions.

Released modules require compatibility care:

- Integral Flux
- Proc
- Temporal Deck

Avoid enum reordering, incompatible serialization changes, or user-visible behavior shifts in those modules unless explicitly required.

Crownstep and TD.Scope are unreleased, so compatibility is looser there, but keep changes scoped.

## Good First Coding Task

For the Python conductor:

1. Change defaults to `--host 127.0.0.1`, `--port 7001`, `--bpm 60`, `--hz 30`, `--program alethe`, `--scale 10.0`.
2. Add `--dry-run` and `--list-programs`.
3. Implement `threshold` and `edict`.
4. Send `0.0..scale` values to OSC.
5. Print a compact lane monitor once per second.
6. On Ctrl+C or termination, send zero values to all eight channels using the configured host and port.

Keep this script dependency-light. `python-osc` is acceptable; avoid introducing a larger framework.
