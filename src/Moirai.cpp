#include "Moirai.hpp"

#include <algorithm>
#include <cmath>

using namespace rack;

Moirai::Moirai() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configParam(TIME_PARAM, -4.f, 4.f, 0.f, "Time scale", " oct", 2.f);
	configParam(CURVE_PARAM, -1.f, 1.f, 0.f, "Curve bias");
	configParam(LEVEL_PARAM, 0.f, 1.f, 1.f, "Level", "%", 0.f, 100.f);
	configButton(LANE_PARAM, "Select inspected lane");
	configParam(CHANNEL_PARAM, 0.f, 15.f, 0.f, "Inspected channel", "", 0.f, 1.f, 1.f);
	getParamQuantity(CHANNEL_PARAM)->snapEnabled = true;
	configButton(MANUAL_TRIGGER_PARAM, "Manually trigger selected channel");
	configInput(GATE_INPUT, "Polyphonic gate");
	configInput(VELOCITY_INPUT, "Polyphonic velocity");
	configInput(M1_INPUT, "Polyphonic modulation 1");
	configInput(M2_INPUT, "Polyphonic modulation 2");
	configInput(M3_INPUT, "Polyphonic modulation 3");
	configInput(CLOCK_INPUT, "Clock");
	configInput(RESET_INPUT, "Reset");
	configOutput(A_OUTPUT, "Lane A envelope (polyphonic)");
	configOutput(EOC_A_OUTPUT, "Lane A end of cycle (polyphonic)");
	configOutput(B_OUTPUT, "Lane B envelope (polyphonic)");
	configOutput(EOC_B_OUTPUT, "Lane B end of cycle (polyphonic)");

	authoredBank = moirai::makeInitialBank();
	const moirai::CompileResult compiled = moirai::compileBank(authoredBank);
	compiledBank = compiled.bank;
	envelopeEngine.setBank(compiledBank.get());
	for (auto& mask : telemetryActiveMask) mask.store(0u, std::memory_order_relaxed);
	for (auto& value : telemetrySelectedValue) value.store(0.f, std::memory_order_relaxed);
}

float Moirai::resolvedInputVoltage(InputId inputId, int channel, float neutral) noexcept {
	Input& input = inputs[inputId];
	if (!input.isConnected() || input.getChannels() <= 0) return neutral;
	if (input.getChannels() == 1) return input.getVoltage(0);
	return channel < input.getChannels() ? input.getVoltage(channel) : neutral;
}

void Moirai::resetRuntime() noexcept {
	envelopeEngine.reset();
	for (auto& lane : eocPulses)
		for (dsp::PulseGenerator& pulse : lane) pulse.reset();
	manualChannelFloor = 1;
	clockElapsed = 0.0;
	seenClockEdge = false;
	estimatedBpm = compiledBank ? compiledBank->clock.fallbackBpm : 120.f;
}

void Moirai::onReset(const ResetEvent& e) {
	Module::onReset(e);
	selectedLane.store(0, std::memory_order_relaxed);
	resetRuntime();
}

void Moirai::process(const ProcessArgs& args) {
	if (!compiledBank) return;
	if (laneButtonTrigger.process(params[LANE_PARAM].getValue(), 0.f, 1.f))
		selectedLane.store(1 - selectedLane.load(std::memory_order_relaxed), std::memory_order_relaxed);
	const int channelSelection = clamp(static_cast<int>(std::round(params[CHANNEL_PARAM].getValue())), 0, 15);
	selectedChannel.store(channelSelection, std::memory_order_relaxed);
	const bool manualTrigger = manualButtonTrigger.process(
		params[MANUAL_TRIGGER_PARAM].getValue(), 0.f, 1.f);
	if (manualTrigger) manualChannelFloor = std::max(manualChannelFloor, channelSelection + 1);

	const bool reset = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f);
	clockElapsed += args.sampleTime;
	const bool clockEdge = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 1.f);
	if (clockEdge) {
		if (seenClockEdge && clockElapsed > 0.0001) {
			const float observed = 60.f / static_cast<float>(clockElapsed * compiledBank->clock.externalPpqn);
			if (observed >= 20.f && observed <= 400.f)
				estimatedBpm += 0.2f * (observed - estimatedBpm);
		}
		seenClockEdge = true;
		clockElapsed = 0.0;
	}
	if (!inputs[CLOCK_INPUT].isConnected()) estimatedBpm = compiledBank->clock.fallbackBpm;
	else if (clockElapsed * 1000.0 > compiledBank->clock.lossTimeoutMs &&
			compiledBank->clock.onClockLoss == moirai::ClockLossPolicy::FALLBACK)
		estimatedBpm = compiledBank->clock.fallbackBpm;

	if (reset) {
		resetRuntime();
	}

	const int gateChannels = inputs[GATE_INPUT].isConnected()
		? clamp(inputs[GATE_INPUT].getChannels(), 1, 16) : 1;
	int channels = std::max(gateChannels, manualChannelFloor);
	moirai::EngineInputs engineInputs;
	engineInputs.channels = channels;
	engineInputs.sampleTime = args.sampleTime;
	engineInputs.bpm = estimatedBpm;
	engineInputs.panelTimeScale = dsp::exp2_taylor5(params[TIME_PARAM].getValue());
	engineInputs.panelCurveBias = params[CURVE_PARAM].getValue();
	engineInputs.panelLevel = params[LEVEL_PARAM].getValue();
	for (int channel = 0; channel < channels; ++channel) {
		engineInputs.gate[channel] = reset ? 0.f : resolvedInputVoltage(GATE_INPUT, channel, 0.f);
		if (manualTrigger && channel == channelSelection) engineInputs.gate[channel] = 10.f;
		engineInputs.velocity[channel] = resolvedInputVoltage(VELOCITY_INPUT, channel, 10.f);
		engineInputs.m1[channel] = resolvedInputVoltage(M1_INPUT, channel, 0.f);
		engineInputs.m2[channel] = resolvedInputVoltage(M2_INPUT, channel, 0.f);
		engineInputs.m3[channel] = resolvedInputVoltage(M3_INPUT, channel, 0.f);
	}
	moirai::EngineOutputs engineOutputs;
	envelopeEngine.process(engineInputs, engineOutputs);
	outputs[A_OUTPUT].setChannels(channels);
	outputs[EOC_A_OUTPUT].setChannels(channels);
	outputs[B_OUTPUT].setChannels(channels);
	outputs[EOC_B_OUTPUT].setChannels(channels);
	for (int channel = 0; channel < channels; ++channel) {
		outputs[A_OUTPUT].setVoltage(engineOutputs.envelope[0][channel], channel);
		outputs[B_OUTPUT].setVoltage(engineOutputs.envelope[1][channel], channel);
		for (int lane = 0; lane < 2; ++lane)
			if (engineOutputs.eoc[lane][channel]) eocPulses[lane][channel].trigger(0.001f);
		outputs[EOC_A_OUTPUT].setVoltage(eocPulses[0][channel].process(args.sampleTime) ? 10.f : 0.f, channel);
		outputs[EOC_B_OUTPUT].setVoltage(eocPulses[1][channel].process(args.sampleTime) ? 10.f : 0.f, channel);
	}
	if (manualChannelFloor > gateChannels) {
		const bool laneAIdle = !envelopeEngine.voice(0, manualChannelFloor - 1).running;
		const bool laneBIdle = !envelopeEngine.voice(1, manualChannelFloor - 1).running;
		if (laneAIdle && laneBIdle) manualChannelFloor = gateChannels;
	}

	const int lane = selectedLane.load(std::memory_order_relaxed);
	lights[LANE_A_LIGHT].setBrightness(lane == 0 ? 1.f : 0.08f);
	lights[LANE_B_LIGHT].setBrightness(lane == 1 ? 1.f : 0.08f);
	if (--telemetryCountdown <= 0) {
		telemetryCountdown = std::max(1, static_cast<int>(args.sampleRate / 60.f));
		telemetryChannels.store(channels, std::memory_order_relaxed);
		telemetryBpm.store(estimatedBpm, std::memory_order_relaxed);
		telemetryExternalClock.store(inputs[CLOCK_INPUT].isConnected(), std::memory_order_relaxed);
		for (int telemetryLane = 0; telemetryLane < 2; ++telemetryLane) {
			uint16_t activeMask = 0u;
			for (int channel = 0; channel < channels; ++channel)
				if (envelopeEngine.voice(telemetryLane, channel).running) activeMask |= uint16_t(1u << channel);
			telemetryActiveMask[telemetryLane].store(activeMask, std::memory_order_relaxed);
			telemetrySelectedValue[telemetryLane].store(
				envelopeEngine.voice(telemetryLane, channelSelection).value, std::memory_order_relaxed);
		}
	}
}

json_t* Moirai::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "selectedLane", json_integer(selectedLane.load(std::memory_order_relaxed)));
	return root;
}

void Moirai::dataFromJson(json_t* rootJ) {
	json_t* laneJ = json_object_get(rootJ, "selectedLane");
	if (json_is_integer(laneJ)) selectedLane.store(clamp(int(json_integer_value(laneJ)), 0, 1));
	resetRuntime();
}
