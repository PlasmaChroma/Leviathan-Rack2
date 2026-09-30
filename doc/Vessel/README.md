# Vessel — implementation draft 0.2 package

Start with **Vessel_DSP_and_Module_Specification.md**.

The specification defines the proposed physical model, numerical contact loop, Rack interface, stereo and optional binaural behavior, energy display, and implementation/test phases.

The C++ reference engine includes modal mechanics, finite-mass strikes, moving friction, simultaneous strike/rub solving, full energy accounting, causal host-rate adaptation, a registered Rack prototype with a 16 HP control panel and 0–33 Hz dual-bowl tuning with a crossfade to one simulation at zero, and offline render/characterization tools. See `Implementation_Status.md` for native Windows strike validation, Linux contact/host-rate/module validation, listening commands, and the remaining physical/Rack gates.

## Files

- `Vessel_DSP_and_Module_Specification.md`: main handoff, with evidence/provenance distinctions and open calibration work.
- `vessel_seed_profiles.json`: machine-readable starting bowl and mallet descriptors. Only the explicitly labeled published metal frequencies are measured source data; the rest are provisional.
- `vessel_numerical_checks.py`: standalone NumPy algebra checker, not a real-time synthesizer.
- `numerical_check_results.json`: results of running that checker for this draft.
- `Physical_Model_Review.md`: in-depth review, corrections incorporated into draft 0.2, and unresolved physical validation gates.
- `vessel_review_checks.py`: additional mechanical-response, local-onset, and passive-trajectory diagnostics.
- `review_check_results.json`: reproducible review diagnostic results; not a singing/audio certification.
- `Implementation_Status.md`: current C++ implementation progress, validation evidence, and render commands.
- `friction_characterization_results.csv`: 51 deterministic C++ moving-contact trajectories and diagnostics.
- `friction_characterization_metadata.json`: environment, metric definitions, and remaining characterization gates.

## Run the checks

```sh
python -m pip install numpy
python vessel_numerical_checks.py
python vessel_review_checks.py
```

The checker writes its result JSON next to the script. Small floating-point differences between platforms are expected; assertions use tolerances.

## Implementation boundary

The standalone strike/rub reference, render harness and causal host-rate foundation are implemented; default singing, coupled energy accounting, stopped-contact damping, limited rate/regularization sweeps and output-filter measurements pass. Complete the broader physical convergence and calibration gates before calling the reference DSP complete. The first Rack module/UI is implemented. Live Rack and native Windows integration checks, production performance/nonlinear aliasing measurements, normal-contact fidelity and optional binaural work remain open; the crystal seed is still synthetic.
