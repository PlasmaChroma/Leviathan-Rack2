# Integral Flux — Physical FG Limits + Digital 2x Rate Mode

Version: 1.1
Default behavior: Physical mode
Switch mechanism: Module context menu
Applies to: CH1 and CH4 outer-channel function-generator behavior
Non-goals: Do not change knob/CV scaling laws, shape curves, or slew-limiter speed limits.

## 1. Summary
Add a user-selectable rate mode for the outer function-generator (FG) engine:

- Physical (default)
  - CYCLE/free-run ceiling: about 1 kHz
  - TRIG acceptance ceiling: about 2 kHz
- Digital 2x
  - CYCLE/free-run ceiling: about 2 kHz
  - TRIG acceptance ceiling: about 4 kHz

These ceilings apply only to FG behavior. Slew limiting is explicitly exempt.

## 2. Definitions in Current Code Terms
- FG mode: the channel is running the FG state machine (`ch.phase != OUTER_IDLE`) or a trigger/cycle action is starting that state machine.
- Slew mode: `ch.phase == OUTER_IDLE` and signal input is connected, so output follows `processUnifiedShapedSlew()`.

## 3. Rate Mode State, Defaults, Persistence
Add:

```cpp
enum class RateMode : int {
    Physical = 0,
    Digital2x = 1
};

RateMode rateMode = RateMode::Physical;
```

Persistence:

```cpp
json_object_set_new(rootJ, "rateMode", json_integer((int) rateMode));

if (json_t* j = json_object_get(rootJ, "rateMode")) {
    int v = (int) json_integer_value(j);
    rateMode = (v == (int) RateMode::Digital2x) ? RateMode::Digital2x : RateMode::Physical;
} else {
    rateMode = RateMode::Physical;
}
```

## 4. Frequency Ceilings
Constants:

```cpp
static constexpr float PHYS_CYCLE_MAX_HZ = 1000.f;
static constexpr float PHYS_TRIG_MAX_HZ  = 2000.f;
```

Effective values by mode:

```cpp
float mult = (rateMode == RateMode::Digital2x) ? 2.f : 1.f;
float effCycleMaxHz = PHYS_CYCLE_MAX_HZ * mult;
float effTrigMaxHz  = PHYS_TRIG_MAX_HZ  * mult;
```

## 5. FG Ceiling #1: CYCLE/Free-Run Loop Speed
### 5.1 When to apply
Apply only when all are true:
- Channel is in FG mode.
- Cycle is enabled (`cycleOn == true`).
- Times being used are FG times (`activeRiseTime`, `activeFallTime`) for the phase integrator.

Do not apply in slew mode.

### 5.2 Enforcement
After FG times are resolved for the current sample/tick:

```cpp
float tMinLoop = 1.f / std::max(effCycleMaxHz, 1.f);
float tLoop = T_rise + T_fall;

if (tLoop < tMinLoop) {
    float scale = tMinLoop / std::max(tLoop, 1e-9f);
    T_rise *= scale;
    T_fall *= scale;
}
```

Required property: preserve rise/fall ratio while clamping loop speed.

## 6. FG Ceiling #2: TRIG Acceptance Rearm
### 6.1 Per-channel state
Add to `OuterChannelState`:

```cpp
float trigRearmSec = 0.f;
```

Update each sample:

```cpp
trigRearmSec = std::max(0.f, trigRearmSec - dt);
```

### 6.2 Acceptance rule
On trigger rising edge:
- If `trigRearmSec > 0`, ignore trigger.
- Otherwise accept trigger and set:

```cpp
float rearm = 1.f / std::max(effTrigMaxHz, 1.f);
trigRearmSec = rearm;
```

Notes:
- This caps accepted trigger rate.
- Applies to FG trigger handling regardless of cycle latch state.
- It does not alter slew tracking logic.

## 7. Integration Points (Current File)
Target file: `src/IntegralFlux.cpp`

- State definitions:
  - `IntegralFlux` class members
  - `OuterChannelState`
- JSON save/load:
  - `dataToJson()`
  - `dataFromJson()`
- Trigger acceptance and FG execution:
  - `processOuterChannel(...)` near trigger edge handling and phase updates
- Context menu:
  - `IntegralFluxWidget::appendContextMenu(...)`

## 8. Context Menu UX
Add `Rate Mode` section with mutually exclusive options:
- `Physical (1x)`
- `Digital (2x)`

Default must be `Physical (1x)` for new modules and missing/invalid JSON.

## 9. Validation Plan
### 9.1 Physical mode
Setup:
- Cycle enabled
- Rise/Fall fastest
- Positive BOTH CV at fast end

Expected:
- Free-run plateaus near 1 kHz
- Additional speeding control does not exceed that plateau

Trigger test:
- Feed trigger above 2 kHz

Expected:
- Accepted retriggers plateau near 2 kHz

### 9.2 Digital 2x mode
Expected:
- Free-run plateau near 2 kHz
- Accepted retriggers plateau near 4 kHz

### 9.3 Slew mode
Setup:
- Signal input patched
- Extreme fast settings/CV

Expected:
- Slew behavior may exceed FG ceilings, subject to existing slew/time hard limits.

## 10. Guardrails
- Do not change knob-to-time mapping.
- Do not change CV exponential law.
- Do not change log/lin/exp curve-shape implementation.
- Do not apply the new ceilings to slew mode.
