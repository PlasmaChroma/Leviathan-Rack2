#pragma once

#include "ChimeraProfile.hpp"

namespace chimera {

// Deterministic 48 kHz control/event skeleton. Reel ownership, recording,
// readers, and audible playback are deliberately added in later phases.
class Core {
public:
    Core() : frame_(0), onsets_(0), invalidAudio_(0), random_(), nextSlot_(0),
             sourcePosition_(0.0), rateMode_(0), fullGene_(true),
             mappedMode_(-1), mappedCoordinate_(0), mappedPitch_(0), mappedRate_(0),
             rateEvaluations_(0) {
        holds_[0] = profile1::FiniteHold(0.f);        // S.O.S.
        holds_[1] = profile1::FiniteHold(0.f);        // Gene
        holds_[2] = profile1::FiniteHold(5.f/6.f);   // Vari-Speed
        holds_[3] = profile1::FiniteHold(1.f/6.f);   // Morph
        holds_[4] = profile1::FiniteHold(0.f);        // Slide
        holds_[5] = profile1::FiniteHold(0.f);        // Organize
        for (int i = 6; i < 15; ++i) holds_[i] = profile1::FiniteHold(0.f);
        sos_.setTau(0.001);
        gene_.setTau(0.002);
        rateCoordinate_.setTau(0.001);
        pitchVolts_.setTau(0.00025);
        morph_.setTau(0.002);
        ratios_[0] = 2.0; ratios_[1] = 3.0; ratios_[2] = 4.0;
    }

    void setRateMode(int mode) { rateMode_ = mode >= 0 && mode <= 2 ? mode : 0; }
    void setChordRatios(double first, double second, double third) {
        ratios_[0] = validRatio(first) ? first : 1.0;
        ratios_[1] = validRatio(second) ? second : 1.0;
        ratios_[2] = validRatio(third) ? third : 1.0;
    }
    void setSeed(std::uint32_t seed) { random_ = profile1::Xorshift32(seed); }
    void setSourcePosition(double coordinate) {
        if (profile1::finite(coordinate)) sourcePosition_ = coordinate;
    }
    double sourcePosition() const { return sourcePosition_; }
    bool fullGene() const { return fullGene_; }
    std::uint64_t invalidControls() const {
        std::uint64_t total = 0;
        for (int i = 0; i < 15; ++i) total += holds_[i].invalidCount;
        return total;
    }
    std::uint64_t invalidAudio() const { return invalidAudio_; }
    std::uint64_t rateEvaluations() const { return rateEvaluations_; }
    profile1::OnsetChoice lastOnset() const { return lastOnset_; }

    CoreOutput step(const CoreInput& input) {
        const ControlFrame& c = input.controls;
        const double kSos = unit(0, c.sos);
        const double kGene = unit(1, c.gene);
        const double kRate = unit(2, c.rate);
        const double kMorph = unit(3, c.morph);
        const double kSlide = unit(4, c.slide);
        const double kOrganize = unit(5, c.organize);
        const double aGene = att(6, c.geneAtt);
        const double aRate = att(7, c.rateAtt);
        const double aSlide = att(8, c.slideAtt);
        const double vSos = voltage(9, c.sosCv);
        const double vGene = voltage(10, c.geneCv);
        const double vRate = voltage(11, c.rateCv);
        const double vMorph = voltage(12, c.morphCv);
        const double vSlide = voltage(13, c.slideCv);
        const double vOrganize = voltage(14, c.organizeCv);

        const double sos = sos_.step(profile1::sos(kSos, c.sosPatched, vSos));
        const double gene = gene_.step(profile1::additive8(kGene, aGene, vGene));
        const double morph = morph_.step(profile1::additive5(kMorph, vMorph));
        const double slide = profile1::additive8(kSlide, aSlide, vSlide);
        const double organize = profile1::additive5(kOrganize, vOrganize);
        const double coordinate = rateMode_ == 0 ?
            rateCoordinate_.step(profile1::clamp(2.0 * kRate - 1.0 + aRate * vRate / 4.0,
                                                 -1.0, 1.0)) :
            rateCoordinate_.step(kRate);
        const double pitch = rateMode_ == 0 ? 0.0 :
            pitchVolts_.step(profile1::clamp(aRate * vRate, -8.0, 8.0));
        if (mappedMode_ != rateMode_ || mappedCoordinate_ != coordinate || mappedPitch_ != pitch) {
            if (mappedMode_ != rateMode_ || mappedCoordinate_ != coordinate)
                mappedBaseRate_ = rateMode_ == 0 ? profile1::classicRateFromCoordinate(coordinate) :
                    rateMode_ == 2 ? profile1::forwardBaseRate(coordinate) : profile1::classicRate(coordinate);
            mappedRate_ = rateMode_ == 0 ? mappedBaseRate_ :
                profile1::clamp(mappedBaseRate_ * std::exp2(pitch), -32.0, 32.0);
            mappedMode_ = rateMode_;
            mappedCoordinate_ = coordinate;
            mappedPitch_ = pitch;
            ++rateEvaluations_;
        }
        fullGene_ = geneMode_.observe(gene);

        if (input.events & kOnsetEvent) {
            lastOnset_ = profile1::chooseOnset(random_, nextSlot_, morph, ratios_);
            nextSlot_ = static_cast<std::uint8_t>((nextSlot_ + 1) % kMusicalVoices);
            ++onsets_;
        }
        const float left = profile1::audio(input.live.l);
        const float right = profile1::audio(input.live.r);
        if (!profile1::finite(input.live.l)) ++invalidAudio_;
        if (!profile1::finite(input.live.r)) ++invalidAudio_;
        CoreOutput output = {StereoFrame{left / 5.f, right / 5.f},
                             static_cast<float>(sos), static_cast<float>(gene),
                             static_cast<float>(mappedRate_), static_cast<float>(morph),
                             static_cast<float>(slide), static_cast<float>(organize),
                             frame_, onsets_, random_.state};
        ++frame_;
        return output;
    }

private:
    static bool validRatio(double r) {
        return profile1::finite(r) && std::fabs(r) >= 0.0625 && std::fabs(r) <= 16.0;
    }
    double unit(int index, float value) {
        return profile1::clamp01(holds_[index].take(value));
    }
    double att(int index, float value) {
        return profile1::clamp(holds_[index].take(value), -1.0, 1.0);
    }
    double voltage(int index, float value) {
        return profile1::clamp(holds_[index].take(value), -24.0, 24.0);
    }

    std::uint64_t frame_, onsets_, invalidAudio_;
    profile1::FiniteHold holds_[15];
    profile1::OnePole sos_, gene_, rateCoordinate_, pitchVolts_, morph_;
    profile1::GeneMode geneMode_;
    profile1::Xorshift32 random_;
    std::uint8_t nextSlot_;
    double sourcePosition_;
    int rateMode_;
    bool fullGene_;
    double ratios_[3];
    profile1::OnsetChoice lastOnset_;
    int mappedMode_;
    double mappedCoordinate_, mappedPitch_, mappedRate_;
    double mappedBaseRate_ = 0;
    std::uint64_t rateEvaluations_;
};

} // namespace chimera
