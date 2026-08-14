#include "Cantor.hpp"

#include <algorithm>
#include <cmath>

Cantor::Cantor() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configParam(INTENT_PARAM, 0.f, 1.f, 0.72f,
		"Intent", "%", 0.f, 100.f);
	configParam(COHERENCE_PARAM, 0.f, 1.f, 0.78f,
		"Coherence", "%", 0.f, 100.f);
	configParam(INTERPRET_PARAM, 0.f, 1.f, 0.f,
		"Interpret", "%", 0.f, 100.f);
	configParam(FIELD_PARAM, 0.f, 1.f, 0.42f,
		"Harmonic field", "%", 0.f, 100.f);
	configInput(PITCH_INPUT, "Pitch (V/oct)");
	configInput(GATE_INPUT, "Gate / note event");
	configOutput(PITCH_OUTPUT, "Interpreted pitch (V/oct)");

	std::uint32_t initialSeed = random::u32();
	if (initialSeed == 0u) initialSeed = 1u;
	culture.setSeed(initialSeed);
	cultureSeed.store(initialSeed, std::memory_order_relaxed);
	serializedRandomState.store(initialSeed, std::memory_order_relaxed);
	pendingSeed.store(initialSeed, std::memory_order_relaxed);
	pendingRandomState.store(initialSeed, std::memory_order_relaxed);
}

cantor::CultureSettings Cantor::currentSettings() {
	cantor::CultureSettings settings;
	settings.intent = clamp(params[INTENT_PARAM].getValue(), 0.f, 1.f);
	settings.coherence = clamp(params[COHERENCE_PARAM].getValue(), 0.f, 1.f);
	settings.interpret = clamp(params[INTERPRET_PARAM].getValue(), 0.f, 1.f);
	settings.field = clamp(params[FIELD_PARAM].getValue(), 0.f, 1.f);
	return settings;
}

void Cantor::publishDecision(const cantor::CultureDecision& decision) {
	visualRequestedPitch.store(decision.requestedPitch, std::memory_order_relaxed);
	visualSelectedPitch.store(decision.selectedPitch, std::memory_order_relaxed);
	visualDistanceCents.store(decision.distanceCents, std::memory_order_relaxed);
	visualHarmonicCost.store(decision.harmonicCost, std::memory_order_relaxed);
	visualActiveVoices.store(culture.getActiveVoiceCount(), std::memory_order_relaxed);
}

void Cantor::process(const ProcessArgs& args) {
	(void) args;
	if (cultureStatePending.exchange(false, std::memory_order_acq_rel)) {
		const std::uint32_t loadedSeed =
			pendingSeed.load(std::memory_order_relaxed);
		culture.setSeed(loadedSeed);
		culture.setRandomState(
			pendingRandomState.load(std::memory_order_relaxed));
		cultureSeed.store(loadedSeed, std::memory_order_relaxed);
		serializedRandomState.store(
			culture.getRandomState(), std::memory_order_relaxed);
	}

	const int pitchChannels = std::max(inputs[PITCH_INPUT].getChannels(), 1);
	const int gateChannels = inputs[GATE_INPUT].getChannels();
	const bool gateConnected = inputs[GATE_INPUT].isConnected();
	const int channelCount = clamp(
		gateConnected ? std::max(pitchChannels, std::max(gateChannels, 1))
			: pitchChannels,
		1,
		cantor::kMaximumVoices);
	outputs[PITCH_OUTPUT].setChannels(channelCount);
	const cantor::CultureSettings settings = currentSettings();

	if (gateConnected) {
		if (!gateWasConnected) {
			gateHigh.fill(false);
			for (int voice = 0; voice < cantor::kMaximumVoices; ++voice) {
				culture.noteOff(voice);
			}
		}
		for (int voice = 0; voice < channelCount; ++voice) {
			const int pitchChannel = pitchChannels == 1 ? 0 : voice;
			const float requestedPitch = pitchChannel < pitchChannels
				? inputs[PITCH_INPUT].getVoltage(pitchChannel) : 0.f;
			float gateVoltage = 0.f;
			if (gateChannels == 1) gateVoltage = inputs[GATE_INPUT].getVoltage(0);
			else if (voice < gateChannels) {
				gateVoltage = inputs[GATE_INPUT].getVoltage(voice);
			}
			if (!std::isfinite(gateVoltage)) gateVoltage = 0.f;
			if (!gateHigh[size_t(voice)] && gateVoltage >= 1.f) {
				gateHigh[size_t(voice)] = true;
				const cantor::CultureDecision decision =
					culture.noteOn(voice, requestedPitch, settings);
				heldOutputs[size_t(voice)] = decision.selectedPitch;
				serializedRandomState.store(
					culture.getRandomState(), std::memory_order_relaxed);
				publishDecision(decision);
			}
			else if (gateHigh[size_t(voice)] && gateVoltage <= 0.1f) {
				gateHigh[size_t(voice)] = false;
				culture.noteOff(voice);
				visualActiveVoices.store(
					culture.getActiveVoiceCount(), std::memory_order_relaxed);
			}
			outputs[PITCH_OUTPUT].setVoltage(heldOutputs[size_t(voice)], voice);
		}
		for (int voice = channelCount; voice < cantor::kMaximumVoices; ++voice) {
			if (gateHigh[size_t(voice)] || culture.isVoiceActive(voice)) {
				gateHigh[size_t(voice)] = false;
				culture.noteOff(voice);
			}
		}
	}
	else {
		if (gateWasConnected) {
			for (int voice = 0; voice < cantor::kMaximumVoices; ++voice) {
				gateHigh[size_t(voice)] = false;
				culture.noteOff(voice);
				staticInitialized[size_t(voice)] = false;
			}
			staticDivider = 0u;
		}
		staticDivider = (staticDivider + 1u) & 31u;
		for (int voice = 0; voice < channelCount; ++voice) {
			const int pitchChannel = pitchChannels == 1 ? 0 : voice;
			const float request = inputs[PITCH_INPUT].getVoltage(pitchChannel);
			const bool changed = !staticInitialized[size_t(voice)]
				|| !std::isfinite(staticRequests[size_t(voice)])
				|| !std::isfinite(request)
				|| std::fabs(request - staticRequests[size_t(voice)])
					>= (1.f / 1200.f);
			if (!staticInitialized[size_t(voice)]
				|| (staticDivider == 0u && changed)) {
				const cantor::CultureDecision decision =
					culture.quantizeStatic(request, settings);
				heldOutputs[size_t(voice)] = decision.selectedPitch;
				staticRequests[size_t(voice)] = request;
				staticInitialized[size_t(voice)] = true;
				publishDecision(decision);
			}
			outputs[PITCH_OUTPUT].setVoltage(heldOutputs[size_t(voice)], voice);
		}
		visualActiveVoices.store(0, std::memory_order_relaxed);
	}
	gateWasConnected = gateConnected;
}

void Cantor::onReset(const ResetEvent& event) {
	(void) event;
	culture.reset();
	gateHigh.fill(false);
	heldOutputs.fill(0.f);
	staticRequests.fill(0.f);
	staticInitialized.fill(false);
	gateWasConnected = false;
	staticDivider = 0u;
	serializedRandomState.store(culture.getRandomState(), std::memory_order_relaxed);
	visualRequestedPitch.store(0.f, std::memory_order_relaxed);
	visualSelectedPitch.store(0.f, std::memory_order_relaxed);
	visualDistanceCents.store(0.f, std::memory_order_relaxed);
	visualHarmonicCost.store(0.f, std::memory_order_relaxed);
	visualActiveVoices.store(0, std::memory_order_relaxed);
}

json_t* Cantor::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "schema", json_integer(1));
	json_object_set_new(root, "cultureSeed", json_integer(
		cultureSeed.load(std::memory_order_relaxed)));
	json_object_set_new(root, "cultureRandomState", json_integer(
		serializedRandomState.load(std::memory_order_relaxed)));
	return root;
}

void Cantor::dataFromJson(json_t* root) {
	if (!root) return;
	std::uint32_t loadedSeed = cultureSeed.load(std::memory_order_relaxed);
	if (json_t* seedJ = json_object_get(root, "cultureSeed")) {
		if (json_is_integer(seedJ)) {
			loadedSeed = std::uint32_t(json_integer_value(seedJ));
			if (loadedSeed == 0u) loadedSeed = 1u;
		}
	}
	std::uint32_t loadedState = loadedSeed;
	if (json_t* stateJ = json_object_get(root, "cultureRandomState")) {
		if (json_is_integer(stateJ)) {
			loadedState = std::uint32_t(json_integer_value(stateJ));
			if (loadedState == 0u) loadedState = loadedSeed;
		}
	}
	pendingSeed.store(loadedSeed, std::memory_order_relaxed);
	pendingRandomState.store(loadedState, std::memory_order_relaxed);
	cultureStatePending.store(true, std::memory_order_release);
}
