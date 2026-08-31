#include "PhonexEngine.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace phonex {

void ChirpGenerator::reset() {
	periodPhase_ = 0;
}

float ChirpGenerator::next(float pitchPeriodTicks) {
	// A zero period is malformed/inaudible input, and must not create a divide
	// or wrap hazard. Voiced corpus frames use positive periods.
	if (!(pitchPeriodTicks > 0.f) || !std::isfinite(pitchPeriodTicks))
		return 0.f;
	const std::size_t chirpIndex = static_cast<std::size_t>(periodPhase_);
	const float output = chirpIndex < kChirp.size() ? kChirp[chirpIndex] : 0.f;
	periodPhase_ += 1.f;
	if (periodPhase_ >= pitchPeriodTicks)
		periodPhase_ = std::fmod(periodPhase_, pitchPeriodTicks);
	return output;
}

void NoiseGenerator::reset() {
	state_ = kLfsrReset;
}

float NoiseGenerator::next() {
	const std::uint32_t old = state_;
	const float output = (old & 1u) ? 1.f : -1.f;
	state_ = ((old >> 1) | kLfsrTopBit)
		^ (((old & 1u) - 1u) & kLfsrXorMask);
	state_ &= kLfsrMask;
	return output;
}

void LatticeFilter::reset() {
	state_.fill(0.f);
}

float LatticeFilter::process(
	float excitation,
	const std::array<float, kLpcOrder>& reflection) {
	std::array<float, kLpcOrder + 1> u{};
	u[kLpcOrder] = excitation;
	for (int i = kLpcOrder - 1; i >= 0; --i) {
		const float k = std::max(-0.995f, std::min(0.995f, reflection[i]));
		u[i] = u[i + 1] - k * state_[i];
	}
	for (int i = kLpcOrder - 1; i >= 1; --i) {
		const float k = std::max(-0.995f, std::min(0.995f, reflection[i - 1]));
		state_[i] = state_[i - 1] + k * u[i - 1];
	}
	state_[0] = u[0];
	return u[0];
}

namespace {

float clampf(float value, float low, float high) {
	return std::max(low, std::min(high, value));
}

float quietSpeed(float speed) {
	return std::abs(speed) < 0.025f ? 0.f : clampf(speed, -4.f, 4.f);
}

} // namespace

void Engine::setSequence(const LpcSequence* sequence) {
	sequence_ = sequence && sequence->valid() ? sequence : nullptr;
	retrigger(1.f);
}

void Engine::setInternalRate(float rateHz) {
	internalRate_ = rateHz < 9000.f ? 8000.f : 10000.f;
}

void Engine::clearSynthesis() {
	lattice_.reset();
	chirp_.reset();
	noise_.reset();
	internalPhase_ = 0.0;
	heldSample_ = 0.f;
	filteredSample_ = 0.f;
}

void Engine::retrigger(float speed) {
	clearSynthesis();
	position_ = sequence_ && sequence_->frameCount > 0 && speed < 0.f
		? static_cast<float>(sequence_->frameCount - 1) : 0.f;
	observedFrame_ = static_cast<std::uint16_t>(position_);
	eoxArmed_ = true;
	frameChanged_ = false;
	eoxEvent_ = false;
	framePulseRemaining_ = 0;
	eoxPulseRemaining_ = 0;
}

LpcFrame Engine::interpolatedFrame() const {
	LpcFrame result;
	if (!sequence_ || sequence_->frameCount == 0)
		return result;
	const float last = static_cast<float>(sequence_->frameCount - 1);
	const float bounded = clampf(position_, 0.f, last);
	const std::uint16_t i0 = static_cast<std::uint16_t>(std::floor(bounded));
	const std::uint16_t i1 = std::min<std::uint16_t>(i0 + 1, sequence_->frameCount - 1);
	const LpcFrame& a = sequence_->frames[i0];
	const LpcFrame& b = sequence_->frames[i1];
	if (a.excitation != b.excitation || i0 == i1)
		return a;
	const float mix = bounded - static_cast<float>(i0);
	result.energy = a.energy + (b.energy - a.energy) * mix;
	result.pitchPeriod10k = a.pitchPeriod10k + (b.pitchPeriod10k - a.pitchPeriod10k) * mix;
	for (int i = 0; i < kLpcOrder; ++i)
		result.reflection[i] = a.reflection[i] + (b.reflection[i] - a.reflection[i]) * mix;
	result.excitation = a.excitation;
	return result;
}

void Engine::updateTransport(const EngineControls& controls, bool triggerRise) {
	frameChanged_ = false;
	eoxEvent_ = false;
	if (!sequence_ || sequence_->frameCount == 0) {
		position_ = 0.f;
		observedFrame_ = 0;
		return;
	}
	const float last = static_cast<float>(sequence_->frameCount - 1);
	const float speed = quietSpeed(controls.speed);
	if (triggerMode_ == TriggerMode::AdvanceOneFrame) {
		if (triggerRise) {
			position_ = clampf(position_ + (speed < 0.f ? -1.f : 1.f), 0.f, last);
		}
	}
	else if (controls.scrubConnected) {
		position_ = clampf((controls.scrubVoltage + 5.f) * 0.1f, 0.f, 1.f) * last;
	}
	else if (controls.hostSampleRate > 0.f && std::isfinite(controls.hostSampleRate)) {
		position_ = clampf(position_ + speed * 50.f / controls.hostSampleRate, 0.f, last);
	}
	const std::uint16_t nextFrame = static_cast<std::uint16_t>(std::floor(position_));
	frameChanged_ = nextFrame != observedFrame_;
	observedFrame_ = nextFrame;
	const bool atEnd = speed < 0.f ? position_ <= 0.f : position_ >= last;
	if (!atEnd)
		eoxArmed_ = true;
	else if (eoxArmed_) {
		eoxEvent_ = true;
		eoxArmed_ = false;
	}
}

float Engine::synthesizeTick(const LpcFrame& frame, const EngineControls& controls) {
	float automatic = 0.f;
	const float pitchScale = std::exp2(clampf(
		controls.pitchOctaves + controls.voctAttenuverter * controls.voct, -8.f, 8.f));
	if (frame.excitation == Excitation::Voiced)
		automatic = chirp_.next(frame.pitchPeriod10k / pitchScale);
	else if (frame.excitation == Excitation::Unvoiced)
		automatic = noise_.next();
	const float blend = clampf(controls.exciteBlend, 0.f, 1.f);
	float carrier = automatic;
	if (blend > 0.f) {
		float forced = 0.f;
		if (controls.forcedExcitation == ForcedExcitation::Voiced) {
			forced = frame.excitation == Excitation::Voiced
				? automatic : chirp_.next(frame.pitchPeriod10k / pitchScale);
		}
		else {
			forced = frame.excitation == Excitation::Unvoiced ? automatic : noise_.next();
		}
		carrier += (forced - automatic) * blend;
	}
	if (controls.externalConnected)
		carrier = std::isfinite(controls.externalExcitation)
			? clampf(controls.externalExcitation / 5.f, -2.f, 2.f)
			: std::numeric_limits<float>::quiet_NaN();
	return lattice_.process(carrier * clampf(frame.energy, 0.f, 1.f), frame.reflection);
}

EngineOutput Engine::process(const EngineControls& controls) {
	const bool triggerRise = controls.triggerGate && !triggerHigh_;
	const bool wordPushRise = controls.wordPush && !wordPushHigh_;
	triggerHigh_ = controls.triggerGate;
	wordPushHigh_ = controls.wordPush;
	if (wordPushRise || (triggerRise && triggerMode_ == TriggerMode::RetriggerPhrase))
		retrigger(controls.speed);
	updateTransport(controls, triggerRise);
	if (frameChanged_ || eoxEvent_) {
		const std::uint32_t pulseSamples = controls.hostSampleRate > 0.f
			? std::max<std::uint32_t>(1u, static_cast<std::uint32_t>(
				std::ceil(static_cast<double>(controls.hostSampleRate) / 1000.0)))
			: 1u;
		if (frameChanged_)
			framePulseRemaining_ = pulseSamples;
		if (eoxEvent_)
			eoxPulseRemaining_ = pulseSamples;
	}
	const LpcFrame frame = interpolatedFrame();
	const float hostRate = controls.hostSampleRate;
	if (hostRate > 0.f && std::isfinite(hostRate)) {
		internalPhase_ += static_cast<double>(internalRate_) / static_cast<double>(hostRate);
		while (internalPhase_ >= 1.0 - 1e-12) {
			heldSample_ = synthesizeTick(frame, controls);
			internalPhase_ -= 1.0;
			if (internalPhase_ < 0.0)
				internalPhase_ = 0.0;
			++internalTicks_;
		}
	}
	float reconstructed = heldSample_;
	if (reconstructionMode_ == ReconstructionMode::Filtered && hostRate > 0.f) {
		const float cutoff = std::min(0.45f * internalRate_, 0.45f * hostRate);
		const float alpha = cutoff / (cutoff + hostRate * 0.1591549431f);
		filteredSample_ += alpha * (heldSample_ - filteredSample_);
		reconstructed = filteredSample_;
	}
	float voltage = reconstructed * 4.f;
	voltage = 5.f * voltage / (5.f + std::abs(voltage));
	if (!std::isfinite(voltage)) {
		clearSynthesis();
		voltage = 0.f;
	}
	EngineOutput output;
	output.audio = voltage;
	output.position = position_;
	output.frameIndex = observedFrame_;
	output.framePulse = framePulseRemaining_ > 0;
	output.eoxPulse = eoxPulseRemaining_ > 0;
	if (framePulseRemaining_ > 0)
		--framePulseRemaining_;
	if (eoxPulseRemaining_ > 0)
		--eoxPulseRemaining_;
	output.voiced = frame.excitation == Excitation::Voiced;
	return output;
}

} // namespace phonex
