# Parameter bible — supplied Lúbadh 2.1.0 update

31 flattened fields; exact vendor spellings preserved. Default = 01-Tape-Looper. Numeric ranges here are documented preset ranges, not measured voltage limits.

## `RecordSpeed`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x47c08, 0x47818`

Do not implement all presets as fixed-speed loop recording.

Values / documented range: `{"0": "Variable, signed recording speed follows transport", "1": "Fixed +1 record speed; independent playback pitch"}`

## `PlaybackSpeed.Looping`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x3a6ac, 0x4eea8`

Per-tap speed, independent of one-shot setting.

Values / documented range: `{"0": "Follow live speed", "1": "Hold speed at tap activation"}`

## `PlaybackSpeed.Oneshot`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x4eea8`

Separate preset field.

Values / documented range: `{"0": "Follow live speed", "1": "Hold speed at tap activation"}`

## `MultiTap`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x4eea8, 0x4e358`

Four engines, five transition Tap slots per engine; not twenty ordinary voices.

Values / documented range: `{"0": "Single musical playback head with transition crossfades", "1": "Multiple simultaneously active logical engines"}`

## `SpeedControl`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x37510; fillSpeedTable symbols`

Exact notched/stepped table boundaries remain unprobed.

Values / documented range: `{"0": "Notched", "1": "Stepped", "2": "Smooth", "3": "V/oct"}`

## `SpeedMarkers`

**Default:** `[0, 0.5, 1.0, 2.0, 4.0]` · **Unit:** array  
**Evidence:** `factory preset comments; fillSpeedTable`

Preserve order/data. Listed defaults are magnitude markers; signed mapping requires fillSpeedTable.

## `TimePot`

**Default:** `2` · **Unit:** enum  
**Evidence:** `0x37a44`

Selects the user-defined third Time function, not all Time UI states.

Values / documented range: `{"0": "Crossfade duration", "1": "Speed slew time", "2": "Tape emulation amount"}`

## `RecordJack`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x284b0, 0x3eed8`

Clock is a configurable RECORD input role; do not unconditionally clock RETRIG.

Values / documented range: `{"0": "Latching record", "1": "Gated record", "2": "External clock"}`

## `ExternalClock.Average`

**Default:** `4` · **Unit:** interval count  
**Evidence:** `factory comments; 0x3eed8`

Average inter-pulse durations; exact outlier/first-pulse handling unverified.

## `ExternalClock.Resolution`

**Default:** `64` · **Unit:** pulses/full loop  
**Evidence:** `factory comments; 0x3eed8`

Zero selects currently selected Clock Division. Not necessarily MIDI PPQ.

## `ExternalClock.Timeout`

**Default:** `0` · **Unit:** seconds  
**Evidence:** `factory comments; 0x3eed8`

Zero disables timeout. Actual timeout interaction remains to be tested.

## `ClockDivisions`

**Default:** `0` · **Unit:** enum  
**Evidence:** `0x3b320`

See clock_division_sets.json.

Values / documented range: `{"0": "All", "1": "Even", "2": "Odd", "3": "Powers of two"}`

## `Quantisation`

**Default:** `3` · **Unit:** enum  
**Evidence:** `0x37a44, 0x376ac, factory comments`

Comment also says 0 disables: contradictory. Enable state is separate; do not conflate it with list selector.

Values / documented range: `{"0": "All division set", "1": "Even division set", "2": "Odd division set", "3": "Powers-of-two division set"}`

## `EraseRecord`

**Default:** `0` · **Unit:** enum  
**Evidence:** `factory comments; 0x28734`

Changes Erase+Record gesture.

Values / documented range: `{"0": "Punch-in overwrite", "1": "Tap tempo"}`

## `MinLength`

**Default:** `1280` · **Unit:** samples  
**Evidence:** `0x376ac`

Default minimum selected region, not proven universal first-record stopping threshold. Seconds=samples/rate.

## `CrossfadeDuration`

**Default:** `250` · **Unit:** milliseconds  
**Evidence:** `0x376ac`

Actual selected fade spans have a 128-sample floor on inspected path; zero is not evidence of discontinuous hard cuts.

Values / documented range: `[0, 250]`

## `RetrigDelay`

**Default:** `0` · **Unit:** milliseconds  
**Evidence:** `0x49244, factory comments`

Delays jack retriggers; block countdown, not proven universal button delay.

## `MaxDubLevel`

**Default:** `0.9` · **Unit:** normalized gain  
**Evidence:** `0x37a44, 0x47818, 0x387fc`

Maximum available dub level; transition envelope can vary actual old-buffer coefficient.

Values / documented range: `[0, 1]`

## `WowFlutterDepth`

**Default:** `0.5` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x502c4`

Effective amount = clamp(Time tape amount * preset depth). Full modulation not numerically probed.

Values / documented range: `[0, 1]`

## `CrinkleDepth`

**Default:** `0.2` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x502c4`

Effective amount = clamp(Time tape amount * preset depth). RNG/filtered irregularity.

Values / documented range: `[0, 1]`

## `TapeAge`

**Default:** `0.3` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x4f524`

Mix amount for active static five-one-pole TapeFilter, not standalone TapeAge class.

Values / documented range: `[0, 1]`

## `Hysterisis`

**Default:** `0.2` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x4fce8`

Original misspelling preserved. Mix amount for 68/159/251/375-sample signed delay diffuser.

Values / documented range: `[0, 1]`

## `Wear`

**Default:** `0.125` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x4fc60`

Mix x toward x*abs(x); applies in input and playback chains.

Values / documented range: `[0, 1]`

## `Reverb`

**Default:** `0.6` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x496c0`

u=clamp(Time tape amount*preset); normalized dry/wet; decay=min(2u,0.9). Full plate not ported.

Values / documented range: `[0, 1]`

## `Knee`

**Default:** `0` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x4ffe0, 0x4fea8`

Not multiplied by Tape Amount. Knee <= double 0.01 selects rational mode.

Values / documented range: `[0, 1]`

## `Compensation`

**Default:** `0.2` · **Unit:** normalized  
**Evidence:** `0x37a44, 0x4ffe0, 0x4fea8`

Multiplied by Tape Amount; pre-gain in rational mode, post-gain in knee mode.

Values / documented range: `[0, 1]`

## `LowCutFreq`

**Default:** `20` · **Unit:** Hz  
**Evidence:** `preset comments; 0x4f474`

Input resonant biquad; independent of TapeAge static bank. Coefficient law not fully verified.

## `LowCutQ`

**Default:** `0.2` · **Unit:** normalized  
**Evidence:** `preset comments; 0x4f474`

Vendor comments call it 0..1; exact Q transform is not established.

## `HighCutFreq`

**Default:** `15000` · **Unit:** Hz  
**Evidence:** `preset comments; 0x4f474`

Input one-pole lowpass, independent of TapeAge wet bank.

## `SpeedSlewTime`

**Default:** `25` · **Unit:** milliseconds  
**Evidence:** `0x37510, 0x37408`

Observed finite block ramp with nominal 2.7 divisor and integer conversions; not an exponential time constant.

## `CapacativeTouchMode`

**Default:** `0` · **Unit:** enum  
**Evidence:** `factory comments; 0x27c74`

Original misspelling preserved. Physical touch enable/calibration is separate.

Values / documented range: `{"0": "Off", "1": "Stall", "2": "Dip"}`
