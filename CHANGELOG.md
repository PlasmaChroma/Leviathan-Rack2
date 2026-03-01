# Changelog

All notable changes to this project are documented in this file.

## [v2.1.3]

- Implemented CV tuning on BOTH input
- Implemented speed_max to try to match HW on generator portions

## [v2.1.2]

- Allow for Signal IN injection with Loop ON combination on signal generators to try to mimic HW behavior.

## [v2.1.1]

- Replaced hard CV clamping on CH1/CH4 Rise, Fall, and BOTH timing CV paths with smooth soft saturation before octave mapping to reduce rail-edge discontinuities.
- Updated slew warp behavior to use segment phase normalization instead of output magnitude, improving symmetry and reducing offset-dependent curvature changes.
- Added per-channel slew segment tracking (`start`, `target`, `direction`) plus cached inverse span for efficient phase computation in the slew hot path.
- Improved preview waveform accuracy at extreme curve/time asymmetry using a LUT + midpoint integration renderer, eliminating right-edge top flattening artifacts in the visualizer.

## [v2.1.0] - 2026-02-25

Tag message: New version created with visualizer, significant CV/knob tuning, and minor performance tweaks.

- CH1 & CH4 now include a visual representation of function generation output
- Graph is "representative" in nature, always displays one full cycle of generation
- Also includes a frequency readout below the graph

## [v2.0.0] - 2026-02-24

Tag message: Initial Release to VCV Rack.

- Initial public VCV Rack release for Integral Flux module.
- Core channel slew behavior, attenuators/attenuverters, cycle logic, UI panel work.
- GPLv3 license file.
