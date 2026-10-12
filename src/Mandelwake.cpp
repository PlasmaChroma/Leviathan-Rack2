#include "Mandelwake.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

namespace {

constexpr float kSmoothDefault = 0.3419951893f;
constexpr float kRatePitchMinimum = -6.3219280949f;
constexpr float kRatePitchMaximum = 5.6438561898f;
constexpr std::array<double, 4> kPulseWidthsMs {{0.1, 1.0, 5.0, 10.0}};

struct MandelwakeSmoothQuantity final : ParamQuantity {
	float getDisplayValue() override {
		const float value = clamp(getValue(), 0.f, 1.f);
		return 2000.f * value * value * value;
	}

	void setDisplayValue(float displayValue) override {
		const float milliseconds = clamp(displayValue, 0.f, 2000.f);
		setImmediateValue(std::cbrt(milliseconds / 2000.f));
	}

	std::string getDisplayValueString() override {
		return string::f("%.1f ms", getDisplayValue());
	}
};

struct MandelwakeRateQuantity final : ParamQuantity {
	float getDisplayValue() override {
		return 4.f * std::exp2(getValue());
	}

	void setDisplayValue(float displayValue) override {
		const float hz = clamp(displayValue, 0.05f, 200.f);
		setImmediateValue(std::log2(hz / 4.f));
	}

	std::string getDisplayValueString() override {
		const float hz = getDisplayValue();
		if (hz < 1.f) return string::f("%.3f Hz", hz);
		if (hz < 10.f) return string::f("%.2f Hz", hz);
		if (hz < 100.f) return string::f("%.1f Hz", hz);
		return string::f("%.0f Hz", hz);
	}
};

float finiteOrZero(float value) {
	return std::isfinite(value) ? value : 0.f;
}

double clampDouble(double value, double low, double high) {
	return std::max(low, std::min(value, high));
}

std::int64_t roundHalfAwayDouble(double value) {
	if (!std::isfinite(value)) return 0;
	return value >= 0.0
		? static_cast<std::int64_t>(std::floor(value + 0.5))
		: static_cast<std::int64_t>(std::ceil(value - 0.5));
}

mandelwake::OrbitQ28 doubleToQ28(double value) {
	const double scaled = value * static_cast<double>(mandelwake::kScaleQ28);
	const std::int64_t rounded = roundHalfAwayDouble(scaled);
	return static_cast<mandelwake::OrbitQ28>(mandelwake::clamp64(
		rounded,
		std::numeric_limits<mandelwake::OrbitQ28>::min(),
		std::numeric_limits<mandelwake::OrbitQ28>::max()));
}

int roundedInt(float value) {
	return static_cast<int>(roundHalfAwayDouble(finiteOrZero(value)));
}

int inputChannels(Input& input) {
	return input.isConnected() ? input.getChannels() : 0;
}

float inputVoltage(Input& input, int channel, bool broadcastMono = true) {
	const int channels = inputChannels(input);
	if (channels <= 0) return 0.f;
	if (channels == 1 && broadcastMono) return finiteOrZero(input.getVoltage(0));
	if (channel < 0 || channel >= channels) return 0.f;
	return finiteOrZero(input.getVoltage(channel));
}

std::uint64_t freshRackSeed() {
	return (std::uint64_t {random::u32()} << 32) | std::uint64_t {random::u32()};
}

mandelwake::Map mapFromInt(int value) {
	value = clamp(value, 0, 2);
	return static_cast<mandelwake::Map>(value);
}

float q28ToXVoltage(mandelwake::OrbitQ28 value) {
	return clamp(
		2.5f * static_cast<float>(value) / static_cast<float>(mandelwake::kScaleQ28),
		-5.f, 5.f);
}

float q28ToRadiusVoltage(std::uint32_t value) {
	return clamp(
		5.f * static_cast<float>(value) / static_cast<float>(mandelwake::kScaleQ28),
		0.f, 10.f);
}

float q30ToNormalizedPhase(std::int32_t value) {
	return clamp(
		static_cast<float>(value) / static_cast<float>(mandelwake::kOneQ30),
		-1.f, 1.f);
}

} // namespace

Mandelwake::Mandelwake()
	: engine(freshRackSeed()) {
	publishedBaseSeed.store(engine.baseSeed(), std::memory_order_relaxed);
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(MAP_PARAM, 0.f, 2.f, 0.f, "Map", {"Mandelbrot", "Julia", "Burning Ship"});
	getParamQuantity(MAP_PARAM)->snapEnabled = true;
	configParam(CENTER_X_PARAM, -2.25f, 1.f, -0.75f, "Center X");
	configParam(CENTER_Y_PARAM, -1.5f, 1.5f, 0.f, "Center Y");
	configParam(ZOOM_PARAM, 0.f, 12.f, 2.f, "Zoom", " oct");
	configParam(ITERATIONS_PARAM, 1.f, 32.f, 4.f, "Iterations");
	getParamQuantity(ITERATIONS_PARAM)->snapEnabled = true;
	configParam(MUTATION_PARAM, 0.f, 1.f, 0.24f, "Mutation", "%", 0.f, 100.f);
	configParam<MandelwakeSmoothQuantity>(SMOOTH_PARAM, 0.f, 1.f, kSmoothDefault, "Smooth");
	configParam<MandelwakeRateQuantity>(
		RATE_PARAM, kRatePitchMinimum, kRatePitchMaximum, 0.f, "Rate");
	configParam(DENSITY_PARAM, 0.f, 1.f, 0.5f, "Density", "%", 0.f, 100.f);
	configParam(X_AMOUNT_PARAM, -1.f, 1.f, 0.f, "X CV amount", "%", 0.f, 100.f);
	configParam(Y_AMOUNT_PARAM, -1.f, 1.f, 0.f, "Y CV amount", "%", 0.f, 100.f);
	configParam(ZOOM_AMOUNT_PARAM, -1.f, 1.f, 0.f, "Zoom CV amount", "%", 0.f, 100.f);
	configParam(MUTATE_AMOUNT_PARAM, -1.f, 1.f, 0.f, "Mutation CV amount", "%", 0.f, 100.f);
	configButton(RESEED_PARAM, "Reseed");
	configSwitch(SEED_LOCK_PARAM, 0.f, 1.f, 1.f, "Seed lock", {"Unlocked", "Locked"});
	if (ParamQuantity* quantity = getParamQuantity(RESEED_PARAM)) quantity->randomizeEnabled = false;
	if (ParamQuantity* quantity = getParamQuantity(SEED_LOCK_PARAM)) quantity->randomizeEnabled = false;

	configInput(CLOCK_INPUT, "Clock");
	configInput(RESET_INPUT, "Reset");
	configInput(X_INPUT, "X CV");
	configInput(Y_INPUT, "Y CV");
	configInput(ZOOM_INPUT, "Zoom CV");
	configInput(MUTATE_INPUT, "Mutation CV");
	configInput(SMOOTH_INPUT, "Smooth CV");
	configInput(RATE_INPUT, "Rate CV");

	configOutput(X_OUTPUT, "X");
	configOutput(Y_OUTPUT, "Y");
	configOutput(RADIUS_OUTPUT, "Radius");
	configOutput(PHASE_OUTPUT, "Phase");
	configOutput(GATE_OUTPUT, "Gate");
	configOutput(ESCAPE_OUTPUT, "Escape");
	configOutput(STEP_OUTPUT, "Step");

	cachedMap = 0;
	resetRuntimeAll(true);
	refreshPulseLength();
	publishVisualSnapshot();
}

Mandelwake::~Mandelwake() {
	teardownTimer.begin(id);
}

void Mandelwake::resetRuntimeChannel(int channelIndex, bool resetInternalPhase) {
	channelIndex = clamp(channelIndex, 0, mandelwake::kMaxChannels - 1);
	ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
	const mandelwake::ChannelState& state = engine.channel(channelIndex);
	const std::uint32_t radius = mandelwake::radiusQ28(state.xQ28, state.yQ28);
	const std::int32_t phase = mandelwake::phaseQ30(state.xQ28, state.yQ28);
	channel.targetX = q28ToXVoltage(state.xQ28);
	channel.targetY = q28ToXVoltage(state.yQ28);
	channel.targetRadius = q28ToRadiusVoltage(radius);
	channel.targetPhase = q30ToNormalizedPhase(phase);
	channel.currentX = channel.targetX;
	channel.currentY = channel.targetY;
	channel.currentRadius = channel.targetRadius;
	channel.currentPhase = channel.targetPhase;
	channel.gateSamples = 0;
	channel.escapeSamples = 0;
	channel.stepSamples = 0;
	channel.lastMutation = 0.f;
	channel.cachedSmoothIndex = -1;
	if (resetInternalPhase) channel.internalPhase = 0;
}

void Mandelwake::resetRuntimeAll(bool resetInternalPhase) {
	for (int channel = 0; channel < mandelwake::kMaxChannels; ++channel) {
		resetRuntimeChannel(channel, resetInternalPhase);
	}
}

void Mandelwake::onReset() {
	freeRunWhenUnclocked.store(true, std::memory_order_relaxed);
	restartInternalPhaseOnReset.store(true, std::memory_order_relaxed);
	phaseUnipolar.store(false, std::memory_order_relaxed);
	pulseWidthIndex.store(1, std::memory_order_relaxed);
	displayQuality.store(DISPLAY_NORMAL, std::memory_order_relaxed);
	selectedDisplayChannel.store(0, std::memory_order_relaxed);
	compatibilityWarning.store(false, std::memory_order_relaxed);
	loadedAlgorithmVersion.store(1, std::memory_order_relaxed);
	cachedMap = clamp(roundedInt(params[MAP_PARAM].getValue()), 0, 2);
	if (!engine.setMap(mapFromInt(cachedMap))) engine.resetAll();
	resetRuntimeAll(true);
	refreshPulseLength();
	visualPublishCountdown = 0;
}

void Mandelwake::onRandomize() {
	const bool locked = params[SEED_LOCK_PARAM].getValue() >= 0.5f;
	const std::uint64_t seed = locked ? engine.baseSeed() : freshRackSeed();
	setBaseSeedAndReset(seed);
}

void Mandelwake::setBaseSeedAndReset(std::uint64_t seed) {
	engine.setBaseSeed(seed);
	publishedBaseSeed.store(seed, std::memory_order_relaxed);
	const int requestedMap = clamp(roundedInt(params[MAP_PARAM].getValue()), 0, 2);
	if (!engine.setMap(mapFromInt(requestedMap))) engine.resetAll();
	cachedMap = requestedMap;
	resetRuntimeAll(restartInternalPhaseOnReset.load(std::memory_order_relaxed));
	visualPublishCountdown = 0;
}

void Mandelwake::refreshPulseLength() {
	const int index = clamp(pulseWidthIndex.load(std::memory_order_relaxed), 0, 3);
	cachedPulseWidthIndex = index;
	const double samples = static_cast<double>(sampleRate) * kPulseWidthsMs[static_cast<std::size_t>(index)] / 1000.0;
	pulseLengthSamples = static_cast<std::uint32_t>(std::max<std::int64_t>(
		1, roundHalfAwayDouble(samples)));
}

void Mandelwake::refreshInternalIncrement(int channelIndex, int rateIndex) {
	ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
	if (channel.cachedRateIndex == rateIndex && channel.internalIncrement != 0) return;
	channel.cachedRateIndex = rateIndex;
	const long double hz = static_cast<long double>(mandelwake::rateMicroHz(rateIndex)) * 1.0e-6L;
	const long double scaled = std::ldexp(hz / static_cast<long double>(sampleRate), 64);
	channel.internalIncrement = static_cast<std::uint64_t>(std::floor(scaled + 0.5L));
	if (channel.internalIncrement == 0) channel.internalIncrement = 1;
}

mandelwake::StepInputs Mandelwake::buildStepInputs(int channelIndex) {
	const float xAmount = clamp(finiteOrZero(params[X_AMOUNT_PARAM].getValue()), -1.f, 1.f);
	const float yAmount = clamp(finiteOrZero(params[Y_AMOUNT_PARAM].getValue()), -1.f, 1.f);
	const float zoomAmount = clamp(finiteOrZero(params[ZOOM_AMOUNT_PARAM].getValue()), -1.f, 1.f);
	const float mutateAmount = clamp(finiteOrZero(params[MUTATE_AMOUNT_PARAM].getValue()), -1.f, 1.f);

	const double xNorm = clampDouble(static_cast<double>(inputVoltage(inputs[X_INPUT], channelIndex)) / 5.0, -1.0, 1.0) * xAmount;
	const double yNorm = clampDouble(static_cast<double>(inputVoltage(inputs[Y_INPUT], channelIndex)) / 5.0, -1.0, 1.0) * yAmount;
	const double zoomOctaves = clampDouble(
		static_cast<double>(finiteOrZero(params[ZOOM_PARAM].getValue()))
			+ clampDouble(static_cast<double>(inputVoltage(inputs[ZOOM_INPUT], channelIndex)), -5.0, 5.0) * zoomAmount,
		0.0, 12.0);
	const int zoomIndex = clamp(
		static_cast<int>(roundHalfAwayDouble(zoomOctaves * 256.0)), 0,
		mandelwake::kZoomTableSize - 1);
	const mandelwake::OrbitQ28 zoomQ28 = mandelwake::zoomScaleQ28(zoomIndex);

	const std::uint32_t channelValue = static_cast<std::uint32_t>(channelIndex);
	const mandelwake::OrbitQ28 seedXQ28 = mandelwake::signedUnitQ30ToQ28(
		mandelwake::signedUnitQ30(mandelwake::orbitHash(
			engine.baseSeed(), mandelwake::kDomainViewportX, channelValue, 0u, 0u, 0u)));
	const mandelwake::OrbitQ28 seedYQ28 = mandelwake::signedUnitQ30ToQ28(
		mandelwake::signedUnitQ30(mandelwake::orbitHash(
			engine.baseSeed(), mandelwake::kDomainViewportY, channelValue, 0u, 0u, 0u)));
	const std::int64_t viewportXQ28 = mandelwake::clamp64(
		std::int64_t {seedXQ28} + doubleToQ28(xNorm),
		-2 * mandelwake::kScaleQ28, 2 * mandelwake::kScaleQ28);
	const std::int64_t viewportYQ28 = mandelwake::clamp64(
		std::int64_t {seedYQ28} + doubleToQ28(yNorm),
		-2 * mandelwake::kScaleQ28, 2 * mandelwake::kScaleQ28);
	const std::int64_t xOffsetQ28 = mandelwake::multiplyQ28(
		zoomQ28, mandelwake::multiplyQ28(doubleToQ28(1.625), viewportXQ28));
	const std::int64_t yOffsetQ28 = mandelwake::multiplyQ28(
		zoomQ28, mandelwake::multiplyQ28(doubleToQ28(1.5), viewportYQ28));
	const std::int64_t centerXQ28 = doubleToQ28(clampDouble(
		static_cast<double>(finiteOrZero(params[CENTER_X_PARAM].getValue())), -2.25, 1.0));
	const std::int64_t centerYQ28 = doubleToQ28(clampDouble(
		static_cast<double>(finiteOrZero(params[CENTER_Y_PARAM].getValue())), -1.5, 1.5));

	const double mutation = clampDouble(
		static_cast<double>(finiteOrZero(params[MUTATION_PARAM].getValue()))
			+ clampDouble(static_cast<double>(inputVoltage(inputs[MUTATE_INPUT], channelIndex)) / 5.0, -1.0, 1.0) * mutateAmount,
		0.0, 1.0);
	runtime[static_cast<std::size_t>(channelIndex)].lastMutation = static_cast<float>(mutation);
	const std::int64_t mutationQ28 = doubleToQ28(mutation);
	const std::int64_t mutationDepthQ28 = mandelwake::divideRoundHalfAway(
		mandelwake::multiplyQ28(zoomQ28, mutationQ28), 4);

	mandelwake::StepInputs stepInputs;
	stepInputs.cXQ28 = static_cast<mandelwake::OrbitQ28>(mandelwake::clamp64(
		centerXQ28 + xOffsetQ28, -(std::int64_t {9} << 26), std::int64_t {1} << 28));
	stepInputs.cYQ28 = static_cast<mandelwake::OrbitQ28>(mandelwake::clamp64(
		centerYQ28 + yOffsetQ28, -(std::int64_t {3} << 27), std::int64_t {3} << 27));
	stepInputs.mutationDepthQ28 = static_cast<mandelwake::OrbitQ28>(mutationDepthQ28);
	stepInputs.densityQ16 = static_cast<std::uint32_t>(mandelwake::clamp64(
		roundHalfAwayDouble(clampDouble(
			static_cast<double>(finiteOrZero(params[DENSITY_PARAM].getValue())), 0.0, 1.0) * 65536.0),
		0, 65536));
	stepInputs.iterations = static_cast<std::uint8_t>(clamp(
		roundedInt(params[ITERATIONS_PARAM].getValue()), 1, 32));
	return stepInputs;
}

void Mandelwake::applyStepOutputs(int channelIndex, const mandelwake::StepOutputs& stepOutputs) {
	ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
	channel.targetX = q28ToXVoltage(stepOutputs.xQ28);
	channel.targetY = q28ToXVoltage(stepOutputs.yQ28);
	channel.targetRadius = q28ToRadiusVoltage(stepOutputs.radiusQ28);
	channel.targetPhase = q30ToNormalizedPhase(stepOutputs.phaseQ30);
	channel.stepSamples = pulseLengthSamples;
	if (stepOutputs.escaped) channel.escapeSamples = pulseLengthSamples;
	if (stepOutputs.gate) channel.gateSamples = pulseLengthSamples;
}

void Mandelwake::updateSmoothing(int channelIndex) {
	ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
	const double smoothInput = clampDouble(
		static_cast<double>(inputVoltage(inputs[SMOOTH_INPUT], channelIndex)) / 10.0,
		0.0, 1.0);
	const double normalized = clampDouble(
		static_cast<double>(finiteOrZero(params[SMOOTH_PARAM].getValue())) + smoothInput,
		0.0, 1.0);
	const int smoothIndex = clamp(
		static_cast<int>(roundHalfAwayDouble(normalized * 1024.0)), 0, 1024);
	if (channel.cachedSmoothIndex != smoothIndex) {
		channel.cachedSmoothIndex = smoothIndex;
		if (smoothIndex == 0) {
			channel.smoothCoefficient = 1.f;
		}
		else {
			const double quantized = static_cast<double>(smoothIndex) / 1024.0;
			const double milliseconds = 2000.0 * quantized * quantized * quantized;
			channel.smoothCoefficient = static_cast<float>(
				1.0 - std::exp(-1.0 / (0.001 * milliseconds * sampleRate)));
		}
	}

	const float coefficient = channel.smoothCoefficient;
	channel.currentX += coefficient * (channel.targetX - channel.currentX);
	channel.currentY += coefficient * (channel.targetY - channel.currentY);
	channel.currentRadius += coefficient * (channel.targetRadius - channel.currentRadius);
	float phaseDelta = channel.targetPhase - channel.currentPhase;
	if (phaseDelta > 1.f) phaseDelta -= 2.f;
	else if (phaseDelta < -1.f) phaseDelta += 2.f;
	channel.currentPhase += coefficient * phaseDelta;
	if (channel.currentPhase >= 1.f) channel.currentPhase -= 2.f;
	else if (channel.currentPhase < -1.f) channel.currentPhase += 2.f;
}

void Mandelwake::process(const ProcessArgs& args) {
	bool uiStateChange = false;
	UiCommand command;
	while (uiCommands.pop(&command)) {
		if (command.type == UiCommandType::SetSeed) {
			setBaseSeedAndReset(command.seed);
			uiStateChange = true;
		}
	}
	if (std::isfinite(args.sampleRate) && args.sampleRate > 1.f && args.sampleRate != sampleRate) {
		sampleRate = args.sampleRate;
		refreshPulseLength();
		for (ChannelRuntime& channel : runtime) {
			channel.cachedRateIndex = std::numeric_limits<int>::min();
			channel.cachedSmoothIndex = -1;
		}
	}
	if (cachedPulseWidthIndex != clamp(pulseWidthIndex.load(std::memory_order_relaxed), 0, 3)) {
		refreshPulseLength();
	}

	int nextActiveChannels = 1;
	for (int inputId = 0; inputId < INPUTS_LEN; ++inputId) {
		nextActiveChannels = std::max(nextActiveChannels, inputChannels(inputs[inputId]));
	}
	nextActiveChannels = clamp(nextActiveChannels, 1, mandelwake::kMaxChannels);

	std::array<bool, mandelwake::kMaxChannels> activationSample {};
	for (int channelIndex = 0; channelIndex < mandelwake::kMaxChannels; ++channelIndex) {
		ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
		const bool activeNow = channelIndex < nextActiveChannels;
		activationSample[static_cast<std::size_t>(channelIndex)] = activeNow && !channel.active;
		if (!activeNow && channel.active) {
			channel.gateSamples = 0;
			channel.escapeSamples = 0;
			channel.stepSamples = 0;
		}
		channel.active = activeNow;
	}
	activeChannels = nextActiveChannels;

	const bool clockConnected = inputs[CLOCK_INPUT].isConnected();
	const bool resetConnected = inputs[RESET_INPUT].isConnected();
	const bool clockConnectionChanged = clockConnected != clockWasConnected;
	const bool resetConnectionChanged = resetConnected != resetWasConnected;

	for (int channelIndex = 0; channelIndex < activeChannels; ++channelIndex) {
		ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
		if (activationSample[static_cast<std::size_t>(channelIndex)] || clockConnectionChanged) {
			channel.clockTrigger.reset();
			if (clockConnected) {
				channel.clockTrigger.process(inputVoltage(inputs[CLOCK_INPUT], channelIndex), 0.1f, 2.f);
			}
		}
		if (activationSample[static_cast<std::size_t>(channelIndex)] || resetConnectionChanged) {
			channel.resetTrigger.reset();
			if (resetConnected) {
				channel.resetTrigger.process(inputVoltage(inputs[RESET_INPUT], channelIndex), 0.1f, 2.f);
			}
		}
	}

	const int requestedMap = clamp(roundedInt(params[MAP_PARAM].getValue()), 0, 2);
	const bool reseedEdge = reseedTrigger.process(
		params[RESEED_PARAM].getValue() >= 0.5f ? 1.f : 0.f, 0.1f, 0.5f);
	bool globalStateChange = uiStateChange;
	if (reseedEdge) {
		engine.reseed(mapFromInt(requestedMap));
		publishedBaseSeed.store(engine.baseSeed(), std::memory_order_relaxed);
		cachedMap = requestedMap;
		resetRuntimeAll(restartInternalPhaseOnReset.load(std::memory_order_relaxed));
		globalStateChange = true;
	}
	else if (requestedMap != cachedMap) {
		engine.setMap(mapFromInt(requestedMap));
		cachedMap = requestedMap;
		resetRuntimeAll(false);
		globalStateChange = true;
	}

	std::array<bool, mandelwake::kMaxChannels> resetThisSample {};
	if (!globalStateChange && resetConnected && !resetConnectionChanged) {
		const int resetChannels = inputChannels(inputs[RESET_INPUT]);
		if (resetChannels == 1) {
			if (runtime[0].resetTrigger.process(inputVoltage(inputs[RESET_INPUT], 0), 0.1f, 2.f)) {
				for (int channelIndex = 0; channelIndex < mandelwake::kMaxChannels; ++channelIndex) {
					resetThisSample[static_cast<std::size_t>(channelIndex)] = true;
				}
			}
		}
		else {
			for (int channelIndex = 0; channelIndex < std::min(resetChannels, mandelwake::kMaxChannels); ++channelIndex) {
				if (runtime[static_cast<std::size_t>(channelIndex)].resetTrigger.process(
					inputVoltage(inputs[RESET_INPUT], channelIndex, false), 0.1f, 2.f)) {
					resetThisSample[static_cast<std::size_t>(channelIndex)] = true;
				}
			}
		}
	}

	for (int channelIndex = 0; channelIndex < mandelwake::kMaxChannels; ++channelIndex) {
		if (!resetThisSample[static_cast<std::size_t>(channelIndex)]) continue;
		engine.resetChannel(channelIndex);
		resetRuntimeChannel(
			channelIndex, restartInternalPhaseOnReset.load(std::memory_order_relaxed));
	}

	for (int channelIndex = 0; channelIndex < activeChannels; ++channelIndex) {
		ChannelRuntime& channel = runtime[static_cast<std::size_t>(channelIndex)];
		bool stepRequested = false;
		const bool suppress = globalStateChange
			|| resetThisSample[static_cast<std::size_t>(channelIndex)]
			|| activationSample[static_cast<std::size_t>(channelIndex)]
			|| clockConnectionChanged;
		if (!suppress) {
			if (clockConnected) {
				stepRequested = channel.clockTrigger.process(
					inputVoltage(inputs[CLOCK_INPUT], channelIndex), 0.1f, 2.f);
			}
			else if (freeRunWhenUnclocked.load(std::memory_order_relaxed)) {
				const float pitch = clamp(
					finiteOrZero(params[RATE_PARAM].getValue())
						+ inputVoltage(inputs[RATE_INPUT], channelIndex),
					kRatePitchMinimum, kRatePitchMaximum);
				const int rateIndex = clamp(
					roundedInt(pitch * 256.f),
					mandelwake::kRateTableMinIndex,
					mandelwake::kRateTableMaxIndex);
				refreshInternalIncrement(channelIndex, rateIndex);
				const std::uint64_t previous = channel.internalPhase;
				channel.internalPhase += channel.internalIncrement;
				stepRequested = channel.internalPhase < previous;
			}
		}

		if (stepRequested) {
			applyStepOutputs(channelIndex, engine.step(channelIndex, buildStepInputs(channelIndex)));
		}
		updateSmoothing(channelIndex);

		outputs[X_OUTPUT].setVoltage(channel.currentX, channelIndex);
		outputs[Y_OUTPUT].setVoltage(channel.currentY, channelIndex);
		outputs[RADIUS_OUTPUT].setVoltage(channel.currentRadius, channelIndex);
		const bool outputPhaseUnipolar = phaseUnipolar.load(std::memory_order_relaxed);
		const float phaseVoltage = outputPhaseUnipolar
			? 5.f * (channel.currentPhase + 1.f)
			: 5.f * channel.currentPhase;
		outputs[PHASE_OUTPUT].setVoltage(
			outputPhaseUnipolar ? clamp(phaseVoltage, 0.f, 10.f) : clamp(phaseVoltage, -5.f, 5.f),
			channelIndex);
		outputs[GATE_OUTPUT].setVoltage(channel.gateSamples > 0 ? 10.f : 0.f, channelIndex);
		outputs[ESCAPE_OUTPUT].setVoltage(channel.escapeSamples > 0 ? 10.f : 0.f, channelIndex);
		outputs[STEP_OUTPUT].setVoltage(channel.stepSamples > 0 ? 10.f : 0.f, channelIndex);
		if (channel.gateSamples > 0) --channel.gateSamples;
		if (channel.escapeSamples > 0) --channel.escapeSamples;
		if (channel.stepSamples > 0) --channel.stepSamples;
	}

	for (int outputId = 0; outputId < OUTPUTS_LEN; ++outputId) {
		outputs[outputId].setChannels(activeChannels);
	}
	lights[SEED_LOCK_LIGHT].setBrightness(params[SEED_LOCK_PARAM].getValue() >= 0.5f ? 1.f : 0.f);

	if (visualPublishCountdown == 0) {
		publishVisualSnapshot();
		visualPublishCountdown = static_cast<std::uint32_t>(std::max(1.f, sampleRate / 30.f));
	}
	else {
		--visualPublishCountdown;
	}

	clockWasConnected = clockConnected;
	resetWasConnected = resetConnected;
}

void Mandelwake::publishVisualSnapshot() {
	const int selected = clamp(
		selectedDisplayChannel.load(std::memory_order_relaxed), 0,
		mandelwake::kMaxChannels - 1);
	mandelwake::VisualSnapshot snapshot;
	const mandelwake::ChannelState& state = engine.channel(selected);
	snapshot.pointCount = state.historyCount;
	for (int index = 0; index < state.historyCount; ++index) {
		snapshot.points[static_cast<std::size_t>(index)] =
			engine.historyPointOldestFirst(selected, index);
	}
	snapshot.selectedChannel = static_cast<std::uint8_t>(selected);
	snapshot.map = static_cast<std::uint8_t>(engine.map());
	snapshot.escapeSerial = state.escapeSerial;
	snapshot.resetSerial = state.resetSerial;
	snapshot.reseedSerial = engine.reseedSerial();
	snapshot.mapSerial = engine.mapSerial();
	snapshot.current = mandelwake::HistoryPoint(state.xQ28, state.yQ28);
	snapshot.lastPreEscape = state.lastPreEscape;
	snapshot.lastReentry = state.lastReentry;
	snapshot.mutation = runtime[static_cast<std::size_t>(selected)].lastMutation;
	snapshot.seedLocked = params[SEED_LOCK_PARAM].getValue() >= 0.5f;
	snapshot.channelActive = selected < activeChannels;
	snapshot.compatibilityWarning = compatibilityWarning.load(std::memory_order_relaxed);
	visualSnapshots.push(snapshot);
}

bool Mandelwake::consumeLatestVisualSnapshot(mandelwake::VisualSnapshot* snapshot) {
	if (!snapshot) return false;
	bool consumed = false;
	mandelwake::VisualSnapshot latest;
	while (visualSnapshots.pop(&latest)) consumed = true;
	if (consumed) *snapshot = latest;
	return consumed;
}

json_t* Mandelwake::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "schemaVersion", json_integer(1));
	json_object_set_new(rootJ, "algorithmVersion", json_integer(
		loadedAlgorithmVersion.load(std::memory_order_relaxed)));
	const std::uint64_t seed = engine.baseSeed();
	json_object_set_new(rootJ, "baseSeedHi", json_integer(static_cast<json_int_t>(seed >> 32)));
	json_object_set_new(rootJ, "baseSeedLo", json_integer(static_cast<json_int_t>(seed & UINT64_C(0xFFFFFFFF))));
	json_object_set_new(rootJ, "freeRunWhenUnclocked", json_boolean(
		freeRunWhenUnclocked.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "restartInternalPhaseOnReset", json_boolean(
		restartInternalPhaseOnReset.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "phasePolarity", json_integer(
		phaseUnipolar.load(std::memory_order_relaxed) ? 1 : 0));
	const int pulseIndex = clamp(pulseWidthIndex.load(std::memory_order_relaxed), 0, 3);
	json_object_set_new(rootJ, "pulseWidthMs", json_real(kPulseWidthsMs[static_cast<std::size_t>(pulseIndex)]));
	json_object_set_new(rootJ, "displayQuality", json_integer(clamp(
		displayQuality.load(std::memory_order_relaxed), 0, DISPLAY_QUALITY_COUNT - 1)));
	json_object_set_new(rootJ, "selectedDisplayChannel", json_integer(clamp(
		selectedDisplayChannel.load(std::memory_order_relaxed), 0, mandelwake::kMaxChannels - 1)));
	return rootJ;
}

void Mandelwake::dataFromJson(json_t* rootJ) {
	std::uint64_t loadedSeed = engine.baseSeed();
	int algorithmVersion = 1;
	bool freeRun = true;
	bool restartPhase = true;
	bool unipolar = false;
	int loadedPulseIndex = 1;
	int quality = DISPLAY_NORMAL;
	int selected = 0;

	if (rootJ) {
		json_t* algorithmJ = json_object_get(rootJ, "algorithmVersion");
		if (json_is_integer(algorithmJ)) {
			algorithmVersion = std::max(1, static_cast<int>(json_integer_value(algorithmJ)));
		}
		json_t* hiJ = json_object_get(rootJ, "baseSeedHi");
		json_t* loJ = json_object_get(rootJ, "baseSeedLo");
		if (json_is_integer(hiJ) && json_is_integer(loJ)) {
			const json_int_t hi = json_integer_value(hiJ);
			const json_int_t lo = json_integer_value(loJ);
			if (hi >= 0 && lo >= 0
				&& static_cast<std::uint64_t>(hi) <= UINT64_C(0xFFFFFFFF)
				&& static_cast<std::uint64_t>(lo) <= UINT64_C(0xFFFFFFFF)) {
				loadedSeed = (static_cast<std::uint64_t>(hi) << 32)
					| static_cast<std::uint64_t>(lo);
			}
		}
		json_t* freeRunJ = json_object_get(rootJ, "freeRunWhenUnclocked");
		if (json_is_boolean(freeRunJ)) freeRun = json_boolean_value(freeRunJ);
		json_t* restartJ = json_object_get(rootJ, "restartInternalPhaseOnReset");
		if (json_is_boolean(restartJ)) restartPhase = json_boolean_value(restartJ);
		json_t* phaseJ = json_object_get(rootJ, "phasePolarity");
		if (json_is_integer(phaseJ)) unipolar = json_integer_value(phaseJ) == 1;
		json_t* pulseJ = json_object_get(rootJ, "pulseWidthMs");
		if (json_is_number(pulseJ)) {
			const double requested = json_number_value(pulseJ);
			double bestDistance = std::numeric_limits<double>::infinity();
			for (int index = 0; index < 4; ++index) {
				const double distance = std::fabs(requested - kPulseWidthsMs[static_cast<std::size_t>(index)]);
				if (distance < bestDistance) {
					bestDistance = distance;
					loadedPulseIndex = index;
				}
			}
		}
		json_t* qualityJ = json_object_get(rootJ, "displayQuality");
		if (json_is_integer(qualityJ)) {
			quality = clamp(static_cast<int>(json_integer_value(qualityJ)), 0, DISPLAY_QUALITY_COUNT - 1);
		}
		json_t* selectedJ = json_object_get(rootJ, "selectedDisplayChannel");
		if (json_is_integer(selectedJ)) {
			selected = clamp(static_cast<int>(json_integer_value(selectedJ)), 0, mandelwake::kMaxChannels - 1);
		}
	}

	loadedAlgorithmVersion.store(algorithmVersion, std::memory_order_relaxed);
	compatibilityWarning.store(algorithmVersion != 1, std::memory_order_relaxed);
	freeRunWhenUnclocked.store(freeRun, std::memory_order_relaxed);
	restartInternalPhaseOnReset.store(restartPhase, std::memory_order_relaxed);
	phaseUnipolar.store(unipolar, std::memory_order_relaxed);
	pulseWidthIndex.store(loadedPulseIndex, std::memory_order_relaxed);
	displayQuality.store(quality, std::memory_order_relaxed);
	selectedDisplayChannel.store(selected, std::memory_order_relaxed);
	setBaseSeedAndReset(loadedSeed);
	refreshPulseLength();
	reseedTrigger.reset();
	reseedTrigger.process(params[RESEED_PARAM].getValue() >= 0.5f ? 1.f : 0.f, 0.1f, 0.5f);
	clockWasConnected = inputs[CLOCK_INPUT].isConnected();
	resetWasConnected = inputs[RESET_INPUT].isConnected();
	visualPublishCountdown = 0;
}

Model* modelMandelwake = createModel<Mandelwake, MandelwakeWidget>("Mandelwake");
