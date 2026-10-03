#pragma once

#include "TiamatState.hpp"

namespace tiamat {

constexpr std::array<float, 7> octaveTargets {{.125f, .25f, .5f, 1.f, 2.f, 4.f, 8.f}};
constexpr std::array<float, 9> clockRatios {{.0625f, .125f, .25f, .5f, 1.f, 2.f, 3.f, 4.f, 8.f}};
constexpr std::array<float, 10> bendRates {{1.f, -1.f, 2.f, .5f, -2.f, -.5f, .25f, 1.5f, -.25f, -1.5f}};
constexpr std::array<float, 5> breakSilences {{0.f, .25f, .5f, .75f, .9f}};

float finiteOrZero(float x) noexcept;
float normalized(float x) noexcept;
float snapOctave(float magnitude, bool secondPass, bool* snapped = nullptr) noexcept;
float microRate(float knob, float volts, bool reverse, bool* octave = nullptr) noexcept;
float internalFrequency(float time) noexcept;
float externalRatio(float time) noexcept;
float bufferFrequencyTarget(double requestedSeconds) noexcept;

struct MappedControls {
    float time = .5f, repeats = 0.f, mixTarget = 1.f, corrupt = 0.f;
    float baseRate = 1.f, macroBend = 0.f, macroBreak = 0.f;
    float traverse = 0.f, manualSilence = 0.f;
    float windowSquared = .02f, crossfeed = 0.f;
    unsigned repeatsExponent = 0;
    bool octave = false;
};

// Call once for each buffered 96-frame block, using that block's snapshot.
// Transcendentals are control-rate only; no audio or host dependency.
class ControlMapper {
public:
    MappedControls map(const PrimaryControls&, const ControlVoltages&,
        const SecondarySettings&, Mode, EffectiveFlags) noexcept;
    void reset() noexcept { previousRepeats_ = 0.f; }
private:
    float previousRepeats_ = 0.f;
};

struct ToneCoefficients { float feedforward, feedback; };
// Setup-only extraction of output Tone initialization, not the Vinyl ATone HP.
ToneCoefficients outputToneCoefficients() noexcept;

} // namespace tiamat
