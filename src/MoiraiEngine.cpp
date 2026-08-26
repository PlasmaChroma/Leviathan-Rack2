#include "MoiraiEngine.hpp"

#include "MoiraiCurves.hpp"

#include <algorithm>
#include <cmath>

namespace moirai {
namespace {

float sourceVoltage(const EngineInputs& inputs, MacroSource source, int channel) {
	switch (source) {
		case MacroSource::VELOCITY: return inputs.velocity[channel];
		case MacroSource::M1: return inputs.m1[channel];
		case MacroSource::M2: return inputs.m2[channel];
		case MacroSource::M3: return inputs.m3[channel];
	}
	return 0.f;
}

float mappedBinding(const CompiledMacroBinding& binding, float voltage) {
	float normalized = clamp01((voltage - binding.inputMin) * binding.inverseInputSpan);
	if (binding.mapping == MacroMapping::EXPONENTIAL) normalized = fourth(normalized);
	return linearInterpolate(binding.outputMin, binding.outputMax, normalized);
}

uint32_t mixHash(uint32_t value) {
	value ^= value >> 16;
	value *= 0x7feb352du;
	value ^= value >> 15;
	value *= 0x846ca68bu;
	value ^= value >> 16;
	return value;
}

float bipolarHash(uint32_t value) {
	return 2.f * static_cast<float>(mixHash(value) & 0x00ffffffu) / 16777216.f - 1.f;
}

float durationSeconds(const Duration& duration, float bpm, float timeScale) {
	const float base = duration.unit == DurationUnit::SECONDS
		? duration.value : duration.value * 60.f / std::max(20.f, std::min(400.f, bpm));
	return std::max(0.0000001f, base * std::max(0.0001f, timeScale));
}

const std::vector<CompiledStage>& currentPath(const EnvelopeVoice& voice) {
	return voice.releasing ? voice.program->releasePath : voice.program->gatePath;
}

} // namespace

Engine::~Engine() {
	reset();
}

void Engine::setBank(const CompiledBank* bank) noexcept {
	m_bank = bank;
	m_activeBank.store(bank, std::memory_order_release);
}

void Engine::installBank(const CompiledBankPtr& bank) {
	reset();
	m_bankOwners.clear();
	m_adoptionOwners.clear();
	m_pendingAdoption.store(nullptr, std::memory_order_release);
	if (bank) m_bankOwners.push_back(bank);
	setBank(bank.get());
}

void Engine::acceptBank(const CompiledBankPtr& bank, ApplyAt applyAt,
		ActiveVoicePolicy activeVoicePolicy) {
	if (!bank) return;
	m_bankOwners.push_back(bank);
	std::unique_ptr<AdoptionRequest> request(new AdoptionRequest());
	request->bank = bank.get();
	request->applyAt = applyAt;
	request->activeVoicePolicy = activeVoicePolicy;
	const AdoptionRequest* published = request.get();
	m_adoptionOwners.emplace_back(request.release());
	m_pendingAdoption.store(published, std::memory_order_release);
}

int Engine::activeRevision() const noexcept {
	const CompiledBank* bank = m_activeBank.load(std::memory_order_acquire);
	return bank ? bank->revision : -1;
}

int Engine::pendingRevision() const noexcept {
	const AdoptionRequest* request = m_pendingAdoption.load(std::memory_order_acquire);
	return request && request->bank ? request->bank->revision : -1;
}

void Engine::reclaimGenerations() {
	const CompiledBank* active = m_activeBank.load(std::memory_order_acquire);
	const CompiledBank* hazard = m_activeHazard.load(std::memory_order_acquire);
	const AdoptionRequest* pending = m_pendingAdoption.load(std::memory_order_acquire);
	const AdoptionRequest* adoptionHazard = m_adoptionHazard.load(std::memory_order_acquire);
	const CompiledBank* accepted = m_bankOwners.empty() ? nullptr : m_bankOwners.back().get();
	m_bankOwners.erase(std::remove_if(m_bankOwners.begin(), m_bankOwners.end(),
		[&](const CompiledBankPtr& owner) {
			const CompiledBank* ptr = owner.get();
			return ptr != active && ptr != hazard && ptr != accepted
				&& (!pending || pending->bank != ptr)
				&& (!adoptionHazard || adoptionHazard->bank != ptr)
				&& ptr->activeVoiceCount.load(std::memory_order_acquire) == 0;
		}), m_bankOwners.end());
	m_adoptionOwners.erase(std::remove_if(m_adoptionOwners.begin(), m_adoptionOwners.end(),
		[&](const std::unique_ptr<const AdoptionRequest>& owner) {
			return owner.get() != pending && owner.get() != adoptionHazard;
		}), m_adoptionOwners.end());
}

void Engine::reset() noexcept {
	for (EnvelopeVoice& voice : m_voices) {
		stopVoice(voice);
		voice = EnvelopeVoice();
	}
	m_gateHigh.fill(false);
}

const CompiledProgram* Engine::assignedProgram(const CompiledBank* bank,
		int lane, int channel) const noexcept {
	if (!bank || lane < 0 || lane >= kLaneCount || channel < 0 || channel >= kMaxChannels) return nullptr;
	const CompiledLane& compiledLane = bank->lanes[lane];
	int index = compiledLane.assignments[channel];
	if (index < 0) index = compiledLane.defaultProgram;
	if (index < 0 || index >= static_cast<int>(bank->programs.size())) return nullptr;
	return &bank->programs[index];
}

void Engine::triggerVoice(EnvelopeVoice& voice, const CompiledProgram* program,
		int lane, int channel, const EngineInputs& inputs) noexcept {
	if (!program || !m_bank) return;
	if (voice.running) {
		if (program->retrigger == RetriggerPolicy::IGNORE_WHILE_RUNNING ||
				program->retrigger == RetriggerPolicy::LEGATO) return;
		stopVoice(voice);
	}
	const float priorValue = voice.value;
	const uint32_t triggerCount = voice.triggerCount + 1u;
	voice = EnvelopeVoice();
	voice.bank = m_bank;
	voice.program = program;
	voice.triggerCount = triggerCount;
	voice.gateHigh = true;
	voice.running = true;
	voice.bank->activeVoiceCount.fetch_add(1, std::memory_order_acq_rel);
	voice.segmentStart = program->retrigger == RetriggerPolicy::FROM_CURRENT ? priorValue : 0.f;
	voice.value = voice.segmentStart;
	voice.latchedTimeScale = std::max(0.0001f, inputs.panelTimeScale);
	voice.latchedCurveBias = inputs.panelCurveBias;
	voice.latchedLevelScale = 1.f;
	voice.latchedLevelOffset = 0.f;

	const CompiledProgram* selected = program;
	for (const CompiledMacroBinding& binding : program->macroBindings) {
		if (binding.target != MacroTarget::VARIANT_SELECT || binding.variantProgramIndices.empty()) continue;
		const float normalized = clamp01((sourceVoltage(inputs, binding.source, channel)
			- binding.inputMin) * binding.inverseInputSpan);
		const size_t bin = std::min(binding.variantProgramIndices.size() - 1,
			static_cast<size_t>(normalized * binding.variantProgramIndices.size()));
		const int selectedIndex = binding.variantProgramIndices[bin];
		if (selectedIndex >= 0 && selectedIndex < static_cast<int>(m_bank->programs.size()))
			selected = &m_bank->programs[selectedIndex];
		break;
	}
	voice.program = selected;
	for (const CompiledMacroBinding& binding : selected->macroBindings) {
		if (binding.sampling != MacroSampling::ON_TRIGGER || binding.target == MacroTarget::VARIANT_SELECT) continue;
		const float mapped = mappedBinding(binding, sourceVoltage(inputs, binding.source, channel));
		switch (binding.target) {
			case MacroTarget::TIME_SCALE: voice.latchedTimeScale *= mapped; break;
			case MacroTarget::CURVE_BIAS: voice.latchedCurveBias += mapped; break;
			case MacroTarget::LEVEL_SCALE: voice.latchedLevelScale *= mapped; break;
			case MacroTarget::LEVEL_OFFSET: voice.latchedLevelOffset += mapped; break;
			case MacroTarget::VARIANT_SELECT: break;
		}
	}
	const uint32_t identity = static_cast<uint32_t>(m_bank->seed) ^ selected->stableIdHash
		^ (static_cast<uint32_t>(lane) * 0x9e3779b9u)
		^ (static_cast<uint32_t>(channel) * 0x85ebca6bu) ^ triggerCount;
	voice.latchedTimeScale *= std::max(0.01f, 1.f + selected->variation.time * bipolarHash(identity));
	voice.latchedLevelScale *= std::max(0.f, 1.f + selected->variation.level * bipolarHash(identity ^ 0xa511e9b3u));
}

void Engine::releaseVoice(EnvelopeVoice& voice) noexcept {
	if (!voice.running || !voice.program || voice.program->kind != ProgramKind::STAGED ||
			voice.program->mode != ProgramMode::GATE || voice.releasing) return;
	voice.releasing = true;
	voice.segment = 0;
	voice.segmentPhase = 0.f;
	voice.segmentStart = voice.value;
	voice.loopIteration = 0;
	if (voice.program->releasePath.empty()) stopVoice(voice);
}

bool Engine::advanceVoice(EnvelopeVoice& voice, float sampleTime, float bpm,
		bool& loopCompleted) noexcept {
	loopCompleted = false;
	if (!voice.running || !voice.program) return false;
	float remaining = std::max(0.f, sampleTime);
	for (int transition = 0; transition < kMaxTransitionsPerSample && voice.running; ++transition) {
		if (voice.program->kind == ProgramKind::CONTOUR) {
			const float duration = durationSeconds(voice.program->duration, bpm, voice.latchedTimeScale);
			const float toEnd = (1.f - voice.segmentPhase) * duration;
			if (remaining < toEnd) {
				voice.segmentPhase += remaining / duration;
				remaining = 0.f;
				voice.value = evaluateContour(*voice.program, voice.segmentPhase);
				break;
			}
			remaining -= toEnd;
			voice.segmentPhase = 1.f;
			voice.value = evaluateContour(*voice.program, 1.f);
			if (voice.program->mode == ProgramMode::CYCLE) {
				voice.segmentPhase = 0.f;
				voice.value = evaluateContour(*voice.program, 0.f);
				loopCompleted = true;
				if (remaining <= 0.f) break;
				continue;
			}
			stopVoice(voice);
			return true;
		}

		const std::vector<CompiledStage>& path = currentPath(voice);
		if (voice.segment < 0 || voice.segment >= static_cast<int>(path.size())) {
			if (!voice.releasing && voice.program->mode == ProgramMode::GATE && voice.gateHigh) {
				// Sustain at the final gate-path value until gate-low branches to release.
				break;
			}
			if (!voice.releasing && voice.program->mode == ProgramMode::GATE) {
				releaseVoice(voice);
				continue;
			}
			if (!voice.releasing && voice.program->mode == ProgramMode::ONE_SHOT &&
					!voice.program->releasePath.empty()) {
				voice.releasing = true;
				voice.segment = 0;
				voice.segmentPhase = 0.f;
				voice.segmentStart = voice.value;
				continue;
			}
			stopVoice(voice);
			return true;
		}
		const CompiledStage& stage = path[voice.segment];
		const float duration = durationSeconds(stage.duration, bpm, voice.latchedTimeScale);
		const float toEnd = (1.f - voice.segmentPhase) * duration;
		if (remaining < toEnd) {
			voice.segmentPhase += remaining / duration;
			voice.value = evaluateSegment(voice.segmentStart, stage.target, voice.segmentPhase,
				stage.curve.type, stage.curve.amount, voice.latchedCurveBias);
			break;
		}
		remaining -= toEnd;
		voice.value = stage.target;
		voice.segmentPhase = 0.f;
		if (!voice.releasing && voice.segment == voice.program->loopEnd) {
			loopCompleted = true;
			const bool repeat = voice.program->loopMode == LoopMode::WHILE_GATE
				? voice.gateHigh : (voice.program->loopMode == LoopMode::COUNTED &&
					voice.loopIteration + 1 < voice.program->loopCount);
			if (repeat) {
				++voice.loopIteration;
				voice.segment = voice.program->loopStart;
				voice.segmentStart = voice.value;
				if (remaining <= 0.f) break;
				continue;
			}
		}
		++voice.segment;
		voice.segmentStart = voice.value;
	}
	return false;
}

float Engine::outputValue(const EnvelopeVoice& voice, int lane, float panelLevel) const noexcept {
	if (!voice.bank || lane < 0 || lane >= kLaneCount) return 0.f;
	const float normalized = clamp01(voice.value * voice.latchedLevelScale + voice.latchedLevelOffset);
	const float level = std::max(0.f, std::min(1.f, panelLevel));
	switch (voice.bank->lanes[lane].outputMode) {
		case OutputMode::UNIPOLAR_10: return 10.f * normalized * level;
		case OutputMode::UNIPOLAR_5: return 5.f * normalized * level;
		case OutputMode::BIPOLAR_5: return (10.f * normalized - 5.f) * level;
	}
	return 0.f;
}

void Engine::stopVoice(EnvelopeVoice& voice) noexcept {
	if (voice.running && voice.bank)
		voice.bank->activeVoiceCount.fetch_sub(1, std::memory_order_acq_rel);
	voice.running = false;
}

bool Engine::allVoicesIdle() const noexcept {
	for (const EnvelopeVoice& voice : m_voices) if (voice.running) return false;
	return true;
}

template <typename T>
const T* Engine::acquirePublished(const std::atomic<const T*>& source,
		std::atomic<const T*>& hazard) noexcept {
	const T* value = nullptr;
	do {
		value = source.load(std::memory_order_acquire);
		hazard.store(value, std::memory_order_release);
	} while (value != source.load(std::memory_order_acquire));
	return value;
}

void Engine::adoptPending(const AdoptionRequest& request,
		const EngineInputs& inputs) noexcept {
	m_bank = request.bank;
	m_activeBank.store(request.bank, std::memory_order_release);
	if (request.activeVoicePolicy == ActiveVoicePolicy::RESTART_ACTIVE) {
		for (int lane = 0; lane < kLaneCount; ++lane) {
			for (int channel = 0; channel < kMaxChannels; ++channel) {
				EnvelopeVoice& voice = m_voices[lane * kMaxChannels + channel];
				if (!voice.running) continue;
				const float prior = voice.value;
				stopVoice(voice);
				triggerVoice(voice, assignedProgram(request.bank, lane, channel),
					lane, channel, inputs);
				voice.segmentStart = prior;
				voice.value = prior;
				voice.gateHigh = inputs.gate[channel] >= 1.f;
			}
		}
	}
	const AdoptionRequest* expected = &request;
	m_pendingAdoption.compare_exchange_strong(
		expected, nullptr, std::memory_order_acq_rel);
}

void Engine::process(const EngineInputs& inputs, EngineOutputs& outputs) noexcept {
	outputs = EngineOutputs();
	outputs.channels = std::max(1, std::min(kMaxChannels, inputs.channels));
	const CompiledBank* active = acquirePublished(m_activeBank, m_activeHazard);
	if (active) m_bank = active;
	bool anyRising = false;
	for (int channel = 0; channel < outputs.channels; ++channel)
		anyRising = anyRising || (inputs.gate[channel] >= 1.f && !m_gateHigh[channel]);
	const AdoptionRequest* pending = acquirePublished(
		m_pendingAdoption, m_adoptionHazard);
	if (pending) {
		const bool due = pending->applyAt == ApplyAt::IMMEDIATE
			|| (pending->applyAt == ApplyAt::NEXT_TRIGGER && anyRising)
			|| (pending->applyAt == ApplyAt::ALL_IDLE && allVoicesIdle())
			|| (pending->applyAt == ApplyAt::NEXT_CLOCK && inputs.clockEdge);
		if (due) adoptPending(*pending, inputs);
	}
	m_adoptionHazard.store(nullptr, std::memory_order_release);
	m_activeHazard.store(nullptr, std::memory_order_release);
	for (int channel = 0; channel < outputs.channels; ++channel) {
		const bool gateHigh = inputs.gate[channel] >= 1.f;
		const bool rising = gateHigh && !m_gateHigh[channel];
		const bool falling = !gateHigh && m_gateHigh[channel];
		m_gateHigh[channel] = gateHigh;
		for (int lane = 0; lane < kLaneCount; ++lane) {
			EnvelopeVoice& active = m_voices[lane * kMaxChannels + channel];
			if (inputs.triggerMask[lane] & uint16_t(1u << channel)) {
				if (active.running) stopVoice(active);
				triggerVoice(active, assignedProgram(m_bank, lane, channel), lane, channel, inputs);
			}
			active.gateHigh = gateHigh;
			if (rising) triggerVoice(active, assignedProgram(m_bank, lane, channel), lane, channel, inputs);
			active.gateHigh = gateHigh;
			if (falling) releaseVoice(active);
			bool loopCompleted = false;
			const bool programCompleted = advanceVoice(active, inputs.sampleTime, inputs.bpm, loopCompleted);
			const EocSource source = m_bank ? m_bank->lanes[lane].eocSource : EocSource::PROGRAM;
			outputs.eoc[lane][channel] = source == EocSource::PROGRAM ? programCompleted : loopCompleted;
			outputs.envelope[lane][channel] = outputValue(active, lane, inputs.panelLevel);
		}
	}
}

} // namespace moirai
