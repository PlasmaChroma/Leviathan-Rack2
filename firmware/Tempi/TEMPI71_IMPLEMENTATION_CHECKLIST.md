# TEMPI 71 behavioral implementation checklist

Use this alongside `TEMPI71_BEHAVIORAL_ENGINEERING_SPEC.md`.

## Core timing
- [ ] One shared Leading Tempo reference.
- [ ] Six channels derived from that reference.
- [ ] Signed ratio code retained internally.
- [ ] `r > 0`: `h = trunc(4H/(r+4))`.
- [ ] `r = 0`: `h = H`.
- [ ] `r < 0`: `h = trunc(H*(4-r)/4)`.
- [ ] Clamp recovered half-period to `[200, 0xFFFFFF]` if reproducing hardware arithmetic domain.
- [ ] Rational alignment factor and six-way LCM preserved.
- [ ] Divider vs multiplier phase-reference asymmetry preserved.

## Channel behavior
- [ ] 50% clock output mode.
- [ ] Nominal 10 ms trigger output mode.
- [ ] Mute suppresses output without destroying internal timing.
- [ ] LED/internal timing remains active while muted.

## State model
- [ ] 64 States in 4 Banks of 16.
- [ ] Each State stores 6 ratios, 6 phases, enable mask, MOD mask.
- [ ] Editable RAM state is separate from persistent state.
- [ ] Explicit Store / Recall / Revert semantics.
- [ ] Factory states reproduce recovered values.

## State selection
- [ ] Absolute State CV/panel base selection.
- [ ] Relative State Gate stepping modulo 16.
- [ ] New absolute base resets relative stepping.
- [ ] Programming pages defer/ignore State changes appropriately.
- [ ] Tap Tempo disabled repurposes State control/CV to Leading Tempo.

## MOD
- [ ] Per-State MOD membership.
- [ ] Shift OFF / CW / CCW / Random.
- [ ] Shift excludes muted channels.
- [ ] Shift is transient and resets on State change.
- [ ] Run/Stop normal.
- [ ] Run/Stop All.
- [ ] Alternate Run/Stop.
- [ ] Momentary MOD gate behavior.
- [ ] Toggled MOD rising-edge behavior.
- [ ] Run can start channels immediately off-grid.
- [ ] Runtime phase displacement is stored separately from programmed phase.
- [ ] Combined Shift + Run/Stop reassigns State Gate to Shift.

## Editing
- [ ] Machine coarse multiplier/divisor 1..32.
- [ ] Fine ratio quarter-step increments/decrements.
- [ ] Machine phase page.
- [ ] Human Programming abstraction.
- [ ] Human Resolution 100/50/25%.
- [ ] Copy/Paste.
- [ ] Deterministic Mutate with recovered LCG if fidelity required.

## Select Bus
- [ ] Receive-only behavior.
- [ ] `C0 ss` State Select.
- [ ] `F4 ss` save/copy path.
- [ ] `F4 40` Save All.
- [ ] `F0 00 02 2D 00 ss F7` Mesh Off.
- [ ] `F0 00 02 2D 01 ss F7` Mesh On.
- [ ] `F0 00 02 2D 02 F7` Default.
- [ ] `F0 00 02 2D 03 F7` Revert.
- [ ] Do not use `F8` as TEMPI scheduling clock through this parser.

## Fidelity tests
- [ ] Ratio fixtures from Section 26 pass exactly.
- [ ] Divider phase fixture produces 4000, not 12000, for `H=8000, r=-8, p=1`.
- [ ] Factory State 15 uses `ratio=-2` on all channels and phase `[0,1,2,3,4,5]`.
- [ ] State 16 produces divisions 2, 2.25, 2.5, 2.75, 3, 3.25.
- [ ] Mute/unmute retains underlying phase.
- [ ] Shift state is discarded by State change.
- [ ] Run/Stop produces transient phase displacement rather than simple gate masking.
- [ ] State CV overrides prior Gate stepping.
