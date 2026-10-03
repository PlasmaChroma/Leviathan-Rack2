#pragma once

#include "TiamatState.hpp"

namespace tiamat {

enum class Action { ClockSource, Mode, Freeze, Bend, Break, Corrupt, ClockReset };
enum class EventKind { Button, Gate };
struct ControlEvent {
    EventKind kind = EventKind::Button;
    Action action = Action::Freeze;
    unsigned frame = 0;
    bool high = true, rising = false;
};

// Prepared by the eventual host adapter. Fixed capacity, stable within each
// event class by renderer timestamp and arrival order. False means overflow;
// the caller must retain/retry an undelivered command, never silently drop it.
struct BlockEvents {
    std::array<ControlEvent, 64> values {};
    unsigned count = 0;
    bool append(ControlEvent event) noexcept {
        if (count >= values.size() || event.frame >= blockFrames) return false;
        unsigned at = count++;
        while (at && (int(values[at - 1].kind) > int(event.kind)
            || (values[at - 1].kind == event.kind && values[at - 1].frame > event.frame))) {
            values[at] = values[at - 1]; --at;
        }
        values[at] = event;
        return true;
    }
};

// Host-rate Schmitt detector; seed after load/reset/bridge replacement before
// transporting events. Seeding a held-high jack never synthesizes an edge.
class GateDetector {
public:
    void seed(float volts) noexcept { high_ = volts >= 1.f; seeded_ = true; }
    bool process(float volts) noexcept {
        if (!seeded_) { seed(volts); return false; }
        const bool previous = high_;
        if (volts >= 1.f) high_ = true;
        else if (!(volts > .1f)) high_ = false;
        return high_ && !previous;
    }
    bool high() const noexcept { return high_; }
private:
    bool high_ = false, seeded_ = false;
};

class EventState {
public:
    ControlState controls;
    SecondarySettings settings;
    void restoreSecondaryDefaults() noexcept {
        settings = SecondarySettings{};
        controls.mode = Mode::Macro;
        controls.macroBend = controls.macroBreak = false;
        controls.microReverse = controls.microSilence = false;
        controls.buttonFreeze = false;
        momentaryFreezeHeld_ = false;
        controls.effect = Effect::Decimate;
    }
    void normalizeSelection() noexcept {
        const int last = settings.effectSet == EffectSet::AllFive ? 5 : 3;
        if (int(controls.effect) < 1 || int(controls.effect) > last) controls.effect = Effect::Decimate;
    }
    // Returns true for a clock-reset action. It never touches audio or PRNG.
    bool apply(const ControlEvent& event) noexcept {
        if (event.kind == EventKind::Gate) {
            if (event.action == Action::Freeze) freezeLevel_ = event.high;
            if (event.action == Action::Bend) bendLevel_ = event.high;
            if (event.action == Action::Break) breakLevel_ = event.high;
            if (!event.rising) return false;
            if (event.action == Action::Corrupt) {
                if (settings.corruptGate == CorruptGate::ClockReset) return true;
                advanceEffect(); return false;
            }
            if (settings.gates == GateBehavior::Level) return false;
        }
        else {
            if (event.action == Action::Freeze && settings.freezeButton == FreezeButton::Momentary) {
                momentaryFreezeHeld_ = event.high; return false;
            }
            if (!event.high) return false; // direct Rack press, no debounce
        }
        switch (event.action) {
        case Action::ClockSource:
            controls.clockSource = controls.clockSource == ClockSource::Internal ? ClockSource::External : ClockSource::Internal; break;
        case Action::Mode: controls.mode = controls.mode == Mode::Macro ? Mode::Micro : Mode::Macro; break;
        case Action::Freeze: controls.buttonFreeze = !controls.buttonFreeze; break;
        case Action::Bend:
            if (controls.mode == Mode::Macro) controls.macroBend = !controls.macroBend;
            else controls.microReverse = !controls.microReverse;
            break;
        case Action::Break:
            if (controls.mode == Mode::Macro) controls.macroBreak = !controls.macroBreak;
            else controls.microSilence = !controls.microSilence;
            break;
        case Action::Corrupt: advanceEffect(); break;
        case Action::ClockReset: return true;
        }
        return false;
    }
    EffectiveFlags effective() const noexcept {
        EffectiveFlags f;
        const bool level = settings.gates == GateBehavior::Level;
        f.bend = (controls.mode == Mode::Macro ? controls.macroBend : controls.microReverse) || (level && bendLevel_);
        f.brk = (controls.mode == Mode::Macro ? controls.macroBreak : controls.microSilence) || (level && breakLevel_);
        f.freeze = controls.buttonFreeze || (level && freezeLevel_)
            || (settings.freezeButton == FreezeButton::Momentary && momentaryFreezeHeld_);
        return f;
    }
private:
    bool freezeLevel_ = false, bendLevel_ = false, breakLevel_ = false;
    bool momentaryFreezeHeld_ = false;
    void advanceEffect() noexcept {
        normalizeSelection();
        const int last = settings.effectSet == EffectSet::AllFive ? 5 : 3;
        controls.effect = static_cast<Effect>(int(controls.effect) == last ? 1 : int(controls.effect) + 1);
    }
};

} // namespace tiamat
