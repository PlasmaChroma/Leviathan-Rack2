# Intensity feedback prototype — 2026-10-02

The timed gestures created a weak initial phase followed by resonance onset that
could still feel abrupt. The new virtual player chooses startup speed from the
mallet's useful friction region, advances toward sustained speed as mechanical
energy develops, and relaxes grip when measured energy rises too quickly.
It only requests speed and pressure from the existing engine. It is a provisional
performance policy, not a physically derived controller or a change to friction.

## Comparison

Default pitch/decay, full intensity, pure wood, audited 192 kHz engine:

| Measurement | Timed metal | Feedback metal | Timed crystal | Feedback crystal |
|---|---:|---:|---:|---:|
| First 10 microjoules (100 ms trace resolution) | 6.7 s | 1.4 s | 4.7 s | 0.5 s |
| First 1 millijoule | 9.76 s | 3.08 s | 6.52 s | 2.32 s |
| First 10 millijoules (100 ms trace resolution) | 10.3 s | 4.5 s | 6.9 s | 3.8 s |

The historical timed comparison uses a 0.5 s speed smoothstep and 5 s pressure
smoothstep. Feedback starts developing energy earlier and spreads the strong
swell over several seconds. This does not establish subjective naturalness.
At 20 seconds, final-second mean wood energies were approximately 16.1 mJ
(metal) and 18.7 mJ (crystal); energy varies over the rubbing orbit, so these are
not fixed plateaus or targets. The meter still uses its original 20 mJ reference.

The 100 ms traces record mechanical energy, mean hand speed, mean normal load,
contact power into the bowl `F*(U-slip)`, sliding loss `F*slip`, and RMS slip.
They distinguish transfer into vibration from energy lost in sliding. Pressure
feedback follows filtered mechanical energy/growth, not instantaneous contact
power; the latter is recorded for diagnosis and oscillates with the contact.

## Checks and limits

- All eight bowl/mallet combinations at half and full intensity completed
  20-second runs without solver faults, speed caps, or energy-audit failures.
- Pure wood onset at 96 and 384 kHz agreed within 0.03 ms. Final-second means
  for these 12-second runs differed by less than 1.3%. This focused check does
  not establish complete rate/alias convergence for all presets.
- Eleven native Rack adapter groups passed, including synthetic quiet/ringing
  state response at 44.1/48/96 kHz, pressure relaxation during fast growth,
  release/zero, CV/pad priority, restoration of independent controls, resets,
  quality changes, and absence of C++ allocations in audio callbacks.
- Native Windows `plugin.dll` linked successfully. No installation or live Rack
  listening was performed for this iteration.
- Half-intensity metal/felt remained very quiet through 20 seconds. Material
  response and mode selection remain nonlinear: more intensity is not guaranteed
  to produce more late energy for every material, tuning, or decay setting.
- The 2 mJ readiness scale, starting pressure, and growth allowance are proposed
  controller choices, not calibrated measurements of a human player's behavior.
  There is no direct energy injection or gain envelope. A shared mean energy
  drives both bowls; binaural behavior still needs auditioning.

## Reproduction

Build `tools/vessel/probe_rub_startup.cpp` with `VesselEngine.cpp`, `ModalBank.cpp`,
`FrictionContact.cpp`, `StrikeContact.cpp`, and `ContactSolver.cpp` using `-Isrc`.

```
probe_rub_startup intensity-player intensity-player-trace.csv
probe_rub_startup intensity-player-rates
probe_rub_startup intensity-ramp intensity-timer-trace.csv
```

The summary is emitted to stdout; the optional second argument receives the
contact trace. For feedback runs, summary speed/pressure columns are nominal
ceilings and the slope column is evaluated there, not at the varying live
controls. Use the traces for actual hand speed/load. CSV evidence is stored in
`experiments/2026-10-02-rub-startup/` under the corresponding names.
