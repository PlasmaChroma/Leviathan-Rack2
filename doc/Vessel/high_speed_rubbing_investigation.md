# High-speed rubbing investigation — 2026-10-02

## Finding

The current high-speed failure is primarily a friction/contact-model operating-range problem, not a numerical energy cap. The existing engine can sustain high-speed singing with different fixed physical parameters, but the tested changes also alter low-speed startup. No production DSP or profiles were changed in this investigation.

The reference friction law is

`F(s) = N [muK + (muS-muK) exp(-(s/vc)^2)] tanh(s/epsilon)`.

At startup, slip is approximately hand speed `U = 2*pi*R*rps`. A small bowl velocity changes force by approximately `-F'(U)*v`. Negative slope supplies the feedback needed to overcome modal damping. At high slip, the Gaussian term becomes negligible; nearly constant tangential force produces mostly a slowly moving deflection instead of reinforcing audio-frequency vibration. Increasing hand power then mostly increases sliding loss.

For metal/suede at 2.5 N, the force slope changes from -3.357 N/(m/s) at 0.4 rev/s to -0.435 at 0.8 and -5.22e-9 at 2.0. At 2 rev/s the last-second energy is 3.13e-7 J but kinetic energy is only 7.19e-11 J: the apparent floor is overwhelmingly deformation, not ringing.

## Measurements

The native C++ probe runs the actual reference engine with energy auditing enabled. Default pitch is 261.625565 Hz. Unless noted, runs last 12 seconds at 192 kHz internal rate, start from rest, and retain the engine's normal startup smoothing. Onset means first crossing of 0.001 J; it is an engineering comparison threshold, not an audibility threshold. Late values average the final second. `-1` in CSV means no crossing within the run, not a proof that it can never occur.

Metal bowl, suede mallet, original curve:

| Speed (rev/s) | Pressure (N) | Onset (s) | Late energy (J) |
|---|---:|---:|---:|
| 0.4 | 2.5 | 1.880 | 0.01629 |
| 0.8 | 2.5 | none in 60 s | 3.63e-7 |
| 0.8 | 7.5 | 8.752 | 0.04319 |
| 0.8 | 15 | 3.162 | 0.06165 |
| 2.0 | 2.5 | none in 60 s | 3.13e-7 |
| 2.0 | 15 | none in 12 s | 1.13e-5 |

All four mallets on both bowl profiles fail to cross the threshold at 2 rev/s and default pressure within 12 seconds. An eight-second low-speed pre-excitation followed by twelve seconds at 2 rev/s also decays, with late energy 2.23e-5 J.

### Controls and alternatives

- **Numerical resolution:** 96, 192 and 384 kHz agree on the failure. Suede 0.4 rev/s onset differs by about 34 microseconds between the endpoint rates. The 60-second 0.8 run shows some slow growth, but remains far below useful resonance.
- **Constant inward load:** the existing prescribed-radial-load experiment improves default onset from 1.880 to 1.414 seconds. At 2 rev/s, its 1.62e-5 J total energy is mostly deformation (3.37e-9 J kinetic). Turning on this force is not a sufficient remedy.
- **Broader Gaussian:** doubling suede `vc` from 0.2 to 0.4 m/s restores 0.8 rev/s onset at 2.5 N (6.517 seconds), but slows 0.4 onset to 4.105 seconds. Quadrupling it alone weakens low-speed onset further.
- **Joint calibration:** `vc=0.8 m/s`, 7.5 N gives 2 rev/s onset in 5.010 seconds and late energy 0.2267 J. At 15 N onset is 1.985 seconds and energy 0.2872 J. However, 0.2 rev/s then starts poorly: no crossing in 12 seconds at 7.5 N, and 5.335 seconds at 15 N. These are sensitivity experiments, not recommended material parameters.
- **Exponential weakening:** an isolated build substitutes `exp(-abs(s)/vc)`, retaining Vessel's tanh smoothing and other mechanics. At 0.8 rev/s/7.5 N, suede onset improves from 8.752 to 5.048 seconds. At 0.4 rev/s/2.5 N it worsens to 4.212 seconds; 2 rev/s still does not start within twelve seconds. A curve-shape substitution alone is not sufficient.
- **Compliant normal follower, local screen only:** a separate continuous-time Jacobian includes mallet mass, a normal spring/damper, handle spring/damper and `F_t = mu F_n`. Three contact stiffness/damping combinations are screened at 24 frozen angles, with and without the velocity-weakening slope. All tested 2 rev/s cases have negative local growth. Compliance must not be promised as an automatic cure for a flat friction curve. This is not a moving-contact simulation: it cannot establish nonlinear startup, intermittent contact, trajectory work or saturation.

## Physical interpretation and next implementation

The original research uses an exponential weakening envelope, radial mallet dynamics and contact compliance. Its stated playing-speed survey is 0.1–0.5 m/s; on Vessel's metal radius that is approximately 0.21–1.05 rev/s. Vessel's top speed is 0.955 m/s. The paper reports pressure-dependent startup and contact interruption. Its precursor also reports that increasing speed can lengthen startup even when eventual vibration amplitude increases. These are model/experimental context, not universal guarantees for every bowl.

Sources:

- Inácio, Henrique & Antunes, [The physics of tibetan singing bowls (2004), equations 11–14 and numerical simulations](https://documentacion.sea-acustica.es/storage/publicaciones/revista_VOL35-12_05_01.pdf).
- [2003 precursor, discussion of speed, force and transient duration](https://documentacion.sea-acustica.es/storage/publicaciones/Bilbao03_ams004.pdf).

The next faithful prototype should jointly investigate measured/plausible friction parameters and compliant normal contact, rather than replacing the energy meter, adding excitation, or dynamically widening friction according to speed. Keep the original model as the comparison reference. Use a separate contact-model version as specified in section 12 of the main specification.

The prototype needs a unilateral contact potential, mallet/handle dynamics, friction bounded by the solved normal force, zero friction during separation, and an explicit energy ledger including prescribed-orbit geometry work. Validate onset and sustained behavior across speed/pressure/material, low-energy initial states and pre-excited states; then verify rate convergence, zero-speed dissipation, release, and existing strike behavior. A fixed-angle eigenvalue result does not replace these checks. Listening comparisons remain necessary before production adoption.

No candidate in this investigation both resolves the full speed range and demonstrates preservation of the existing low-speed sound. The evidence supports a calibration/contact investigation, not a guaranteed monotonic speed-to-startup relationship.

## Reproduction and validation

Data: [experiments/2026-10-02-rub-startup](experiments/2026-10-02-rub-startup).

Run in native MINGW64 from repository root (preserve the existing build cache):

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Isrc tools/vessel/probe_rub_startup.cpp \
  src/vessel/VesselEngine.cpp src/vessel/ModalBank.cpp src/vessel/FrictionContact.cpp \
  src/vessel/StrikeContact.cpp src/vessel/ContactSolver.cpp -o test-results/probe_rub_startup.exe
for group in baseline contact convergence joint; do
  test-results/probe_rub_startup.exe "$group" > "test-results/rub-$group.csv"
done
python tools/vessel/probe_exponential_friction.py test-results/ExponentialFrictionContact.cpp
g++ -std=c++17 -O2 -Wall -Wextra -Isrc -Isrc/vessel tools/vessel/probe_rub_startup.cpp \
  src/vessel/VesselEngine.cpp src/vessel/ModalBank.cpp test-results/ExponentialFrictionContact.cpp \
  src/vessel/StrikeContact.cpp src/vessel/ContactSolver.cpp -o test-results/probe_rub_exponential.exe
test-results/probe_rub_exponential.exe law > test-results/rub-exponential.csv
# Requires NumPy; the Codex bundled Python was used for this separate screen.
python tools/vessel/probe_rub_linearization.py > test-results/rub-linearization.csv
make -j10 plugin.dll
```

103 nonlinear runs completed with zero solver faults, caps or nonfinite resets. Every run checks its work/energy balance against 1e-7 J. Both friction-law builds passed finite-difference derivative, dissipation-sign and negative-slope-bound checks on the sampled slips. The local linear screen evaluated 576 matrices. The native plugin build was up to date. No Rack listening validation or production DSP change was performed; the full fast suite was not rerun for these standalone experiments.
