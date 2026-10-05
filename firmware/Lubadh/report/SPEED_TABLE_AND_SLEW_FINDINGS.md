# Speed tables and speed-state consumers

The four speed modes now have independently checked table contracts. Quantizer,
slew, snap and tap-speed consumers also execute original application bytes.
This closes table arithmetic, rather than the upstream ADC/CV, gesture, link or
modulation graph. Hardware calibration values and the physical voltage adapter
remain separate requirements.

## Provenance and checked scope

The callable generator is `fillSpeedTable` at `preset_load:0x157ac` (2624 bytes).
In `lubadh_main` the same work is inlined into preset-construction lambdas.
The preset executable SHA-256 is
`3ab7566b2a0d5a641454cb3928acfb658cb3e000c5b0505d2569cc2c78086e1b`.
The main executable SHA-256 is
`2e5827862cf947f61899facf558e39965373c324d401e095622ed3c578d15cd4`.

`preset_byte_probe.py` loads inert ELF sections into Unicorn, derives its PLT
names from relocation/symbol tables and permits only verified allocation and
memory services. Unknown imports, syscalls and unbounded execution fail closed.
No appliance program, loader, worker, startup constructor or script is launched.
The V/oct arithmetic slice adds an explicit host glibc `powf` service; agreement
does not establish bit identity with the absent ARM libm implementation.

| Probe | Result | Scope |
|---|---|---|
| `probe_speed_tables.py` | 13 tables, 53,248 values, zero error | All ten factory presets, Smooth, overlapping notches and unsorted duplicate markers; 770 instruction addresses |
| `probe_v_oct_curve.py` | Five calibration fixtures, 20,480 values, zero error | Original calibrated curve arithmetic; 48 instruction addresses; file parser bypassed |
| `probe_speed_consumers.py` | 3,149 comparisons pass | 89 quantizer, 2,412 slew, 72 snap and 576 tap-speed checks; 122 instruction addresses |

Full tables and fixtures are saved in the three corresponding `*results.json`
files. Counts describe those fixtures, not an overall parity percentage.

## Table construction

Preset byte 64 selects the mode; bytes 68–547 contain 120 float marker slots;
the 4096-float output table begins at byte 548. The generator mirrors markers
about zero, sorts and normalizes them. Zero-filled trailing slots may retain
duplicate zero values in the rewritten marker area; table notch construction
deduplicates the relevant values independently.

| Mode | Table behavior |
|---|---|
| 0 Notched | Signed piecewise linear curve with fixed plateaus around each marker |
| 1 Stepped | Linear signed table; quantization occurs later in `roundSpeed` |
| 2 Smooth | The same linear signed table as Stepped |
| 3 V/oct | Copies the supplied calibrated positive-speed curve |

For Notched, each signed marker `m` maps to center
`trunc(f32(f32(m + 4) * 0.125) * 4095)`; its initial plateau is
`[clamp(center - 50), clamp(center + 50))`, with bounds clamped to 0–4095.
The boundary sentinels are `(-4, 0, 0, 0)` and `(4, 4096, 4096, 4096)`.
Adjacent overlapping plateaus shrink toward their centers using half of the
integer center distance, truncated. The interval between plateaus interpolates
with binary32 division and fused multiply/add. The interval endpoints and
plateaus are half-open. See `speed_table_model.py` for the explicit rounding
order and integer bounds; it reproduces every checked table value exactly.

Default positive markers are 0, 0.5, 1, 2 and 4. Their signed centers include
0→2047, ±0.5→1791/2303, ±1→1535/2559, ±2→1023/3071 and ±4→0/4095.
The fixed 100-count notch width matters: closely spaced custom markers reduce
one another's plateaus rather than retaining overlapping flat spans.

The linear table starts at -4 and repeatedly adds binary32 step
`0x3b000801`. Repeated rounding produces final value `3.999999761581421`,
not an exact +4. The central entries at 2047/2048 are approximately
`-0.0009770388715` / `0.0009765631985`; there is no exact zero entry.
Do not replace this with a direct index formula when comparing literal vectors.

## Calibrated V/oct law

`genVpOctTable` reads channel-specific `zeroV`, `oneV`, `threeV`, `fiveV`
fields. On the successful static path only `zeroV` and `threeV` affect this
curve. Literal constants at 0x16680/0x16684 are 819 and 2457. Define
`D = f32(f32(f32(threeV - zeroV) * 819) / 2457)` and `K = trunc(D)`.
The original arithmetic from 0x162b0 through 0x16368 is:

```
i < K: speed[i] = f32(f32(i * 0.25) / D)
i >= K: speed[i] = clamp(f32(powf(2, f32(f32(i / D) - 1)) * 0.25), 0, 4)
```

Thus it moves continuously from stopped toward quarter speed, then follows an
exponential doubling law, saturating at four times speed. It is positive-only;
the direction state is applied downstream. The index is not directly reduced
by `zeroV` in this routine; the calibration difference determines its scale.

For supplied fixture `zeroV=0, threeV=2457`, D=819: index 819 is quarter
speed, 1638 half speed, 2457 unit speed, 3276 double speed and 4095 four times
speed. This is a fixture, not an appliance calibration measurement. The other
four supplied calibration pairs check fractional D and shifted saturation.
Actual calibration file contents, error/fallback branches and upstream ADC/CV
transforms remain unresolved. A native Rack law should explicitly define its
voltage scaling and stopped region instead of borrowing arbitrary ADC counts.

## Quantization and speed state

`roundSpeed` at main:0x3748c uses the sorted signed marker vector at Link+180.
For adjacent markers a/b containing x, the threshold is
`f32(f32(a + b) * 0.5)`. Inputs at or above that threshold select b; ties
therefore select the higher signed marker. Inputs outside the vector's range
clamp to its first/last value. Empty/invalid vectors and NaN are not checked.

`setSlewSpeed` at 0x372d4 stores target at Channel+44 and reads current at +728.
For nonnegative valid preset `SpeedSlewTime` t:

```
N = trunc(f32(f32(f32(trunc(f32(t))) / f32(2.7)) + 1))
increment = f32(f32(target - current) / f32(N))
remaining = N
```

`processSpeed` starts at 0x37408. Its checked prefix ends before 0x37444,
where flutter processing begins. The prefix copies Link's current speed to
Channel+40 and, if remaining>0, adds the increment to current and decrements
remaining once. It advances once per invocation, independent of the frames
argument: fixtures of 1, 16 and 128 frames produce the same state sequence.
It does not clamp to target when the count expires. Repeated binary32 addition
can leave a small final error. Flutter/touch and later speed publication are
outside this probe; the prefix alone does not set the final playback speed.

`snapSpeedTo` at 0x3731c immediately stores the requested value at +44, +728
and +36, clears increment and remaining, and, only in mode 3, sets direction
state +236 to 1 for a negative value and 0 otherwise. Other modes retain that
direction field.

`setTapTempoSpeed` at 0x37364 clamps the supplied magnitude to binary32 0.01–4,
then applies direction and the same slew law. For mode 3 it negates when the
direction field is nonzero. For other modes it negates when the stored raw
pot value is below 2048. This consumer does not establish clock interval,
tap-timeout, retrigger or linked forwarding behavior.

## Specification implications and remaining work

These equations and exported vectors are sufficient to specify speed-table
generation and the checked consumers. A faithful Rack implementation should
precompute tables outside the audio hot path. Any improved direct-index table,
sample-rate-aware slew or exact final-target snap must be a deliberate native
decision with separate acceptance vectors. The original callback cadence must
be recovered before assigning a duration in seconds to the slew count.

`POT_SPEED_EVENT_PUBLICATION.md` subsequently checks the complete pot deadband,
alternate numeric speed states, production ADC recurrence and asymmetric
two-deck speed dispatch. Physical CV mapping, direction-changing gestures,
clock/tap producers, preset-update/reset
graph, linked asymmetry, flutter/touch, and connection into transport scheduling.
Factory tables passing does not close those contracts.

## Reproduction

From the repository root with Unicorn and Capstone installed in the offline
Linux research Python environment:

```sh
python firmware/Lubadh/probes/probe_v_oct_curve.py
python firmware/Lubadh/probes/probe_speed_tables.py
python firmware/Lubadh/probes/probe_speed_consumers.py
```

Only research artifacts changed. No Rack module source changed, so a native
plugin build is not a validation of this decomposition work.
