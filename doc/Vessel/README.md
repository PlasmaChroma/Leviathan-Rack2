# Vessel — implementation draft 0.2 package

Start with **Vessel_DSP_and_Module_Specification.md**.

The specification defines the proposed physical model, numerical contact loop, Rack interface, stereo and optional binaural behavior, energy display, and implementation/test phases.

The first C++ implementation milestone is now available: modal mechanics, finite-mass strikes, energy accounting, and an offline renderer. See `Implementation_Status.md` for files, native validation, listening commands, and the next friction/Rack gate.

## Files

- `Vessel_DSP_and_Module_Specification.md`: main handoff, with evidence/provenance distinctions and open calibration work.
- `vessel_seed_profiles.json`: machine-readable starting bowl and mallet descriptors. Only the explicitly labeled published metal frequencies are measured source data; the rest are provisional.
- `vessel_numerical_checks.py`: standalone NumPy algebra checker, not a real-time synthesizer.
- `numerical_check_results.json`: results of running that checker for this draft.
- `Physical_Model_Review.md`: in-depth review, corrections incorporated into draft 0.2, and unresolved physical validation gates.
- `vessel_review_checks.py`: additional mechanical-response, local-onset, and passive-trajectory diagnostics.
- `review_check_results.json`: reproducible review diagnostic results; not a singing/audio certification.
- `Implementation_Status.md`: current C++ implementation progress, validation evidence, and render commands.

## Run the checks

```sh
python -m pip install numpy
python vessel_numerical_checks.py
python vessel_review_checks.py
```

The checker writes its result JSON next to the script. Small floating-point differences between platforms are expected; assertions use tolerances.

## Implementation boundary

The standalone strike engine and render harness are implemented. Complete the moving friction/coupled-contact validation before the Rack panel or optional binaural work. Bowl/mallet calibration and production real-time/aliasing measurements remain open; the crystal seed is still synthetic.
