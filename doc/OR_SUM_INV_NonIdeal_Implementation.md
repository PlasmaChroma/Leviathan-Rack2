# Intended Implementation: OR / SUM / INV Non-Ideal Analog Behavior

## Purpose

Define a practical, staged implementation for non-ideal behavior of the Maths mix section:
- SUM output
- INV output
- OR output

This document focuses on behavior that is currently idealized in code and should be upgraded to better match hardware feel.

## Current Behavior (baseline)

Current implementation is mathematically ideal:
- `SUM = clamp(v1 + v2 + v3 + v4, -10V, +10V)`
- `INV = clamp(-SUM, -10V, +10V)`
- `OR = clamp(max(0, v1, v2, v3, v4), 0V, +10V)`

Where each `vi` is included only if its variable output jack is unpatched (break-normal).

## Design Goals

1. Keep routing logic unchanged (break-normal behavior remains exactly the same).
2. Add non-ideal transfer behavior only at output stage.
3. Preserve monophonic, low-CPU implementation.
4. Make calibration constants explicit and easy to tune from measurements.
5. Provide a clean fallback mode that reproduces current ideal behavior.

## Intended Model

## 1) SUM non-ideal saturation

### Rationale

Hardware summing stages are rarely hard-clipped exactly at rails. A soft knee is usually closer to analog behavior.

### Proposed transfer

Let `s_raw = v1 + v2 + v3 + v4`.

Use a symmetric soft saturator around 0V:
- `SUM = sat_sym(s_raw, satV, drive)`

Suggested default function:
- `sat_sym(x, satV, drive) = satV * tanh((drive / satV) * x)`

Suggested starting constants:
- `satV = 10.0`
- `drive = 1.15`

Behavior notes:
- Near 0V: approximately linear.
- Large magnitudes: compresses toward +/-satV rather than hard clipping.

## 2) INV output generation

### Rationale

INV should remain the inversion of SUM stage behavior, not an independently saturated path.

### Proposed transfer

- `INV = -SUM`

Optional measurement-dependent variant:
- If hardware suggests a separate inverter saturation stage, add
  `INV = sat_sym(-SUM, invSatV, invDrive)`
  but default should be direct inversion.

## 3) OR non-ideal rectifier behavior

### Rationale

Ideal max/rectifier behavior is often too perfect compared to analog OR combiners.

### Proposed staged options

### Option A (default first pass): Ideal OR + output soft saturation

- `or_raw = max(0, v1, v2, v3, v4)`
- `OR = sat_pos(or_raw, orSatV, orDrive)`

Where:
- `sat_pos(x, satV, drive) = clamp(satV * tanh((drive / satV) * max(0, x)), 0, satV)`

Suggested starting constants:
- `orSatV = 10.0`
- `orDrive = 1.05`

This keeps channel selection behavior deterministic while adding analog-like top-end compression.

### Option B (later, only if measurement supports): diode-threshold-like OR

Per channel apply a small threshold/drop before max:
- `vi_eff = max(0, vi - vDrop)`
- `or_raw = max(vi_eff...)`
- `OR = sat_pos(or_raw, orSatV, orDrive)`

Suggested starting value:
- `vDrop = 0.05V to 0.25V` (must be measured)

Only enable if hardware measurements show low-level OR suppression consistent with threshold behavior.

## Integration Plan

## Phase 1 (safe)

1. Add helper functions:
- `softSatSym(x, satV, drive)`
- `softSatPos(x, satV, drive)`

2. Replace:
- SUM hard clamp -> `softSatSym`
- INV hard clamp -> direct `-SUM`
- OR hard clamp -> `softSatPos(max(...))`

3. Keep all routing and break-normal logic untouched.

## Phase 2 (measurement-backed)

1. Fit `drive` and `satV` from captured transfer curves.
2. Decide whether OR needs a `vDrop` threshold.
3. If needed, add optional per-revision constants.

## Proposed Constants Block

```cpp
struct MixNonIdealCal {
    bool enabled = true;

    // SUM
    float sumSatV = 10.f;
    float sumDrive = 1.15f;

    // OR
    float orSatV = 10.f;
    float orDrive = 1.05f;
    float orVDrop = 0.f;    // 0 for Phase 1

    // INV
    bool invUseExtraSat = false;
    float invSatV = 10.f;
    float invDrive = 1.0f;
};
```

## Validation Checklist

1. Break-normal unchanged:
- patching OUT 1 removes CH1 contribution from SUM/OR/INV.

2. Low-level linearity:
- small signals should remain nearly unchanged.

3. High-level behavior:
- SUM and OR should approach rail smoothly (not abrupt flat-top).

4. INV symmetry:
- `INV` tracks `-SUM` closely unless explicit separate saturation is enabled.

5. Regression patch test:
- Existing patches should remain stable in behavior except for expected subtle analog-like compression near rails.

## Open Questions / Measurement Needed

1. Does hardware SUM hard-clip or soft-clip near +/-10V?
2. Does OR show any measurable threshold/drop at low levels?
3. Is INV exactly `-SUM` or independently limited at extreme levels?
4. Should non-ideal behavior be always-on or user-switchable (context menu "Ideal Mix" / "Analog Mix")?

## Recommendation

Implement Phase 1 now behind a small boolean flag (`enabled`) defaulting to on, with conservative drive values.
Then fit constants from hardware sweeps and decide if OR threshold behavior is warranted.
