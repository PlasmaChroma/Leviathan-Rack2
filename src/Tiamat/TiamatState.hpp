#pragma once

#include <array>
#include <cstdint>

namespace tiamat {

constexpr float rendererRate = 48000.f;
constexpr float coefficientRate = 96028.f;
constexpr unsigned blockFrames = 96;
constexpr unsigned planeFrames = 3601050;
constexpr unsigned maxCaptureFrames = planeFrames / 2;
constexpr unsigned schemaVersion = 1;
constexpr unsigned algorithmVersion = 1;

enum class Mode { Macro, Micro };
enum class ClockSource { Internal, External };
enum class GateBehavior { Latching, Level };
enum class FreezeButton { Latching, Momentary };
enum class CorruptGate { Advance, ClockReset };
enum class EffectSet { AllFive, OriginalThree };
enum class Effect { Retained = 0, Decimate, Dropout, Destroy, DjFilter, Vinyl };

struct PrimaryControls {
    float time = .5f, repeats = 0.f, mix = 1.f;
    float bend = .5f, brk = 0.f, corrupt = 0.f;
};

// Volts, not the hardware's electrically inverted ADC readings.
struct ControlVoltages {
    float time = 0.f, repeats = 0.f, mix = 0.f;
    float bend = 0.f, brk = 0.f, corrupt = 0.f;
};

struct SecondarySettings {
    float window = .1414213562373095f;
    float separation = 1.f;
    float bendDepth = 1.f, breakDepth = 1.f, corruptDepth = 1.f;
    bool unique = true;
    GateBehavior gates = GateBehavior::Latching;
    FreezeButton freezeButton = FreezeButton::Latching;
    CorruptGate corruptGate = CorruptGate::Advance;
    EffectSet effectSet = EffectSet::AllFive;
};

struct ControlState {
    Mode mode = Mode::Macro;
    ClockSource clockSource = ClockSource::Internal;
    bool macroBend = false, macroBreak = false;
    bool microReverse = false, microSilence = false;
    bool buttonFreeze = false;
    Effect effect = Effect::Decimate;
    std::uint64_t seed = 1;
};

// Resolved after button and gate event handling; not persisted.
struct EffectiveFlags {
    bool bend = false, brk = false, freeze = false;
};

struct ClockState {
    std::array<double, 3> intervals {{.5, .5, .5}};
    double phase = 0., lastEdge = 0., nextBoundary = 0.;
    unsigned intervalCursor = 0, dividerCount = 0;
    bool hasEdge = false, lost = false, capacityLimited = false;
};

struct TransitionState {
    bool freezeRequested = false, freezeActive = false;
    std::array<bool, 2> pendingAcceptance {{false, false}};
    bool clockPending = false;
    unsigned guard = 0;
};

struct ReaderState {
    float position = 0.f, rate = 1.f, previousSample = 0.f;
    unsigned anchor = 0, sliceStart = 0, sliceFrames = 1;
};
struct WriterState {
    unsigned captureFrames = 1, writeBank = 0, writePosition = 0;
    unsigned historyPosition = 0;
};
struct MacroState {
    float rate = 1.f, slew = 1.f, silence = 0.f;
    float randomPosition = 0.f;
    unsigned extraExponent = 0;
};
struct CorruptRoutingState {
    Effect primary = Effect::Decimate, retained = Effect::Decimate;
    float amount = 0.f, retainedAmount = 0.f;
    bool retainedEnabled = false, dropoutOpen = true;
};
struct OutputState {
    float mix = 0.f;
    std::array<float, 2> toneHistory {{0.f, 0.f}};
};

// Small value snapshot boundary for a future UI; never carries sample planes.
struct Status {
    Mode mode = Mode::Macro;
    bool freezeRequested = false, freezeActive = false;
    bool clockLost = false, capacityLimited = false;
    Effect effect = Effect::Decimate;
    std::array<unsigned, 2> captureFrames {{0, 0}}, sliceFrames {{0, 0}};
    std::array<float, 2> rates {{1.f, 1.f}};
};

} // namespace tiamat
