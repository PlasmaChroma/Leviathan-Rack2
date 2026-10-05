# Gentle high-energy damping trial — 2026-10-04

Vessel now adds a small amount of viscous modal loss above 40 mJ per bowl.
Below that knee it restores the exact original coefficients. Sustain, seed
profiles, friction, intensity feedback, the 20 mJ meter scale, and output gain
are unchanged. This is a provisional musical policy, not a calibrated material
law, clipping guarantee, or hard energy ceiling.

## Policy and implementation

For energy E in joules, let z = max(0, (E - 0.04)/0.04). The added damping
coefficient is `0.5*z*z/(1+z*z)` per second, with z bounded before squaring.
The curve has zero slope at the knee and approaches 0.5/s. At 60 mJ the added
coefficient is 0.1/s; at 80 mJ it is 0.25/s. Near the fundamental, the extra
energy-envelope loss rate is approximately twice that coefficient.

The existing approximately 1 kHz Vessel control update evaluates each active
bowl independently. `ModalBank::setAdditionalDamping()` changes only cached
sigma, inverse denominator, and force admittance. No state scaling, stiffness
change, transcendental calculation, allocation, or per-sample loop operation
is added. Reconfiguration preserves the extra loss; engine reset clears it.
The coefficient accessor continues to describe the base linear specimen.

Both the fused free tail and collocated contact step use these cached values.
The existing modal-loss ledger accounts for the additional dissipation.
Additional positive damping reduces contact admittance, so the base friction
uniqueness certificate remains conservative. The standalone engine remains
the linear reference unless its caller invokes `updateHighEnergyDamping()`;
Vessel opts in at its control boundary.

## What the joules mean

There is no calibrated physical maximum in the current profiles. Apart from
published metal frequencies, the seed parameters (including modal masses and
decay times) are provisional. A meter reaching 100% does not mean a bowl is
physically full.

As an illustration within this model, `E = 0.5*m*(2*pi*f*A)^2` gives roughly
0.41 mm peak displacement for a single 261.6 Hz metal fundamental holding
20 mJ (modal mass 0.09 kg), or 0.50 mm for the crystal seed (0.06 kg).
40 mJ increases these by sqrt(2). These are model-derived displacements, not
measurements or validated limits; redistribution among modes and changes in
pitch alter the interpretation. Physical calibration requires a real specimen's
modal masses, vibration amplitudes, decay envelopes and playing conditions.

The [singing-bowl modeling paper](https://documentacion.sea-acustica.es/storage/publicaciones/revista_VOL35-12_05_01.pdf)
also discusses nonlinear excitation and intermittent contact. It does not
establish the 40 mJ threshold chosen here. See the separate high-speed rubbing
investigation for why maximum hand speed does not necessarily maximize ringing.

## Native A/B measurements

The accompanying `experiments/2026-10-04-energy-damping.csv` was generated with
the Windows MINGW64 build of `tools/vessel/energy_damping_probe.cpp`. Both seeds
and all four mallets were exercised at 48 kHz host rate, Balanced quality,
default pitch, zero binaural separation and maximum Sustain (4x). Rubbing and
10 Hz repeated strikes last 12 seconds; full-intensity feedback lasts 20 seconds.
Every case includes another five seconds after release. Single strikes use
full velocity with the production Felt velocity scale. Clipping counts observe
either channel at maximum output level (16 times velocity), before the clamp.

- All eight single strikes and all eight default rubbing cases stayed below
  40 mJ and matched the reference metrics exactly. The largest peak among
  those cases was 29.45 mJ.
- Metal/suede, 0.6 rev/s and 15 N: peak 118.29 -> 111.79 mJ; five seconds
  after release 34.82 -> 25.05 mJ. This preserves most of the driven bloom.
- Metal/wood, 10 Hz strikes: peak 304.27 -> 113.70 mJ; five-second tail
  54.74 -> 19.79 mJ. The repeated-excitation stress case changes substantially.
- Full-intensity feedback: four combinations stayed below the knee and matched
  exactly. Peak changes for the other four were under 1.5%. Driven feedback is
  not monotonic: metal/suede's end-drive energy rose from 25.70 to 27.75 mJ
  even though its peak fell slightly. The player/contact loop can compensate
  for added damping; this is not an absolute energy regulator.
- All 96 A/B trajectories completed without an engine fault. Strong drive can
  still exceed the output rails; this adjustment does not replace output protection.

Build and reproduce from MINGW64:

```sh
g++ -std=c++11 -O2 -Wall -Wextra -fno-fast-math -Isrc \
  tools/vessel/energy_damping_probe.cpp src/vessel/*.cpp \
  -o build/tools/vessel_energy_damping_probe.exe
./build/tools/vessel_energy_damping_probe.exe > build/vessel-energy-probe.csv
```

Passing any argument restricts the probe to full-intensity feedback.
Regression coverage in `vessel_engine_spec` checks sub-knee equality, positive
loss and energy accounting, unchanged state during damping updates, reduced
admittance, retune/reset behavior and fast/full-step agreement at 96/192 kHz.
Listening in Rack remains necessary to judge the musical result.

Validation on 2026-10-04: native MINGW64 `make -j10 test-fast plugin.dll
RACK_APP_RUNTIME_DIR="/c/Program Files/VCV/Rack2Pro"` completed successfully.
The rebuilt plugin was not installed into Rack. No files were staged or committed.
