# Notes

# Compiling
You probably won't be able to compile this code, because the build tool is the 64-bit version of msys and it's not going to be setup to run in a WSL environment

# Hardware observations (Make Noise FUNCTION, CH1)

## Cycle OFF, input patched
- SW vs hardware slew behavior was initially very different at high frequency.
- Updated SW slew model now tracks hardware trend better (stable low/mid frequency amplitude, drop near 1kHz).

## Cycle ON + input patched (quirky analog behavior)
- Behavior is nontrivial and frequency-dependent on hardware.
- At some input frequencies, enabling Cycle appears to halve observed frequency (divide-by-2 lock).
- Example observed: around 266Hz input, Cycle ON can produce ~133Hz output; around 270Hz, output can stay near input frequency.
- This behavior appears consistent/repeatable in regions, not random.
- Likely analog coupling/locking behavior; not yet modeled intentionally in SW.

## Harmonic/timbre observations at 266Hz triangle input (hardware)
- LOG:
  - Fundamental: -15dB
  - H2: -36dB
  - H3: -46dB
  - H4: -48dB
  - H5: -55dB
- LINEAR:
  - H2: -33dB
  - H3: -62dB
  - H4: -43dB
  - H5: -68dB
- EXP:
  - H2: -33dB
  - H3: -37dB
  - H4: -40dB
  - H5: -41dB

## Interpretation target for future SW tuning
- LOG and especially LINEAR can suppress odd harmonics vs even.
- EXP brings odd harmonics up significantly (closer to even-harmonic levels).
- Future tuning should use harmonic-ratio targets (e.g. H3/H2, H5/H4), not only Vpp.

## End-of-session note (subjective)
- Current SW shaping does not yet match hardware harmonic trend reliably.
- However, current behavior is musically usable as an "odd harmonic destroyer / restorer" style effect.
- As odd harmonics come up toward EXP, perceived timbre becomes noticeably harsher.
- This is potentially useful in its own right, but likely needs an intentional target curve decision:
  - either match hardware measurements more strictly
  - or preserve/design a deliberate musical response that differs from hardware
