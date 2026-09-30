# Vessel — implementation draft package

Start with **Vessel_DSP_and_Module_Specification.md**.

The specification defines the proposed physical model, numerical contact loop, Rack interface, stereo and optional binaural behavior, energy display, and implementation/test phases.

## Files

- `Vessel_DSP_and_Module_Specification.md`: main handoff, with evidence/provenance distinctions and open calibration work.
- `vessel_seed_profiles.json`: machine-readable starting bowl and mallet descriptors. Only the explicitly labeled published metal frequencies are measured source data; the rest are provisional.
- `vessel_numerical_checks.py`: standalone NumPy algebra checker, not a real-time synthesizer.
- `numerical_check_results.json`: results of running that checker for this draft.

## Run the checks

```sh
python -m pip install numpy
python vessel_numerical_checks.py
```

The checker writes its result JSON next to the script. Small floating-point differences between platforms are expected; assertions use tolerances.

## Implementation boundary

This package is a specification and numerical design check, not a finished VCV Rack module. It has not been auditioned as an instrument, fitted to crystal recordings, or benchmarked for real-time use. Implement the standalone DSP harness before panel artwork or optional binaural work.
