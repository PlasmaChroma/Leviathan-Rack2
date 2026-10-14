#pragma once

#include "MoiraiTypes.hpp"

#include <array>
#include <atomic>
#include <memory>
#include <vector>

namespace moirai {

struct EngineInputs {
	int channels = 1;
	std::array<float, kMaxChannels> gate {};
	std::array<float, kMaxChannels> velocity {};
	std::array<float, kMaxChannels> m1 {};
	std::array<float, kMaxChannels> m2 {};
	std::array<float, kMaxChannels> m3 {};
	float sampleTime = 1.f / 48000.f;
	float bpm = 120.f;
	float panelTimeScale = 1.f;
	float panelCurveBias = 0.f;
	float panelLevel = 1.f;
	bool clockEdge = false;
	std::array<uint16_t, kLaneCount> triggerMask {};
};

struct AdoptionRequest {
	const CompiledBank* bank = nullptr;
	ApplyAt applyAt = ApplyAt::IMMEDIATE;
	ActiveVoicePolicy activeVoicePolicy = ActiveVoicePolicy::FINISH_CURRENT;
};

struct EngineOutputs {
	int channels = 1;
	std::array<std::array<float, kMaxChannels>, kLaneCount> envelope {};
	std::array<std::array<bool, kMaxChannels>, kLaneCount> eoc {};
};

struct EnvelopeVoice {
	const CompiledBank* bank = nullptr;
	const CompiledProgram* program = nullptr;
	uint32_t triggerCount = 0;
	int segment = 0;
	int loopIteration = 0;
	float segmentPhase = 0.f;
	float segmentStart = 0.f;
	float value = 0.f;
	float latchedTimeScale = 1.f;
	float latchedCurveBias = 0.f;
	float latchedLevelScale = 1.f;
	float latchedLevelOffset = 0.f;
	bool gateHigh = false;
	bool running = false;
	bool releasing = false;
};

class Engine {
public:
	~Engine();
	void setBank(const CompiledBank* bank) noexcept;
	void installBank(const CompiledBankPtr& bank);
	void acceptBank(const CompiledBankPtr& bank, ApplyAt applyAt,
		ActiveVoicePolicy activeVoicePolicy);
	void reclaimGenerations();
	const CompiledBank* bank() const noexcept { return m_bank; }
	int activeRevision() const noexcept;
	int pendingRevision() const noexcept;
	std::size_t ownedGenerationCount() const noexcept { return m_bankOwners.size(); }
	void reset() noexcept;
	void process(const EngineInputs& inputs, EngineOutputs& outputs) noexcept;
	const EnvelopeVoice& voice(int lane, int channel) const noexcept {
		return m_voices[lane * kMaxChannels + channel];
	}

private:
	const CompiledBank* m_bank = nullptr;
	std::vector<CompiledBankPtr> m_bankOwners;
	std::vector<std::unique_ptr<const AdoptionRequest>> m_adoptionOwners;
	std::atomic<const CompiledBank*> m_activeBank {nullptr};
	std::atomic<const CompiledBank*> m_activeHazard {nullptr};
	std::atomic<const AdoptionRequest*> m_pendingAdoption {nullptr};
	std::atomic<const AdoptionRequest*> m_adoptionHazard {nullptr};
	std::array<EnvelopeVoice, kLaneCount * kMaxChannels> m_voices {};
	std::array<bool, kMaxChannels> m_gateHigh {};

	const CompiledProgram* assignedProgram(const CompiledBank* bank,
		int lane, int channel) const noexcept;
	void triggerVoice(EnvelopeVoice& voice, const CompiledProgram* program,
		int lane, int channel, const EngineInputs& inputs) noexcept;
	void releaseVoice(EnvelopeVoice& voice) noexcept;
	bool advanceVoice(EnvelopeVoice& voice, float sampleTime, float bpm,
		bool& loopCompleted) noexcept;
	float outputValue(const EnvelopeVoice& voice, int lane, float panelLevel) const noexcept;
	void stopVoice(EnvelopeVoice& voice) noexcept;
	bool allVoicesIdle() const noexcept;
	void adoptPending(const AdoptionRequest& request,
		const EngineInputs& inputs) noexcept;
	template <typename T>
	const T* acquirePublished(const std::atomic<const T*>& source,
		std::atomic<const T*>& hazard) noexcept;
};

} // namespace moirai
