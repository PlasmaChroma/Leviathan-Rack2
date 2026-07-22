#include "Umi.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float PULSE_LENGTHS[] = {0.001f, 0.005f, 0.010f, 0.020f, 0.050f};
constexpr int PULSE_LENGTH_COUNT = int(sizeof(PULSE_LENGTHS) / sizeof(PULSE_LENGTHS[0]));

float normalizedCv(Input& input) {
	return input.isConnected() ? input.getVoltage(0) / 5.f : 0.f;
}

int clampMaxBalls(int value) {
	if (value <= 16) return 16;
	if (value <= 32) return 32;
	return 64;
}

} // namespace

Umi::Umi() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configButton(DROP_PARAM, "Drop pearls");
	configParam(RATE_PARAM, 0.f, 1.f, 0.f, "Automatic drop rate");
	configSwitch(DENSITY_PARAM, 1.f, 8.f, 1.f, "Pearls per drop", {"1", "2", "3", "4", "5", "6", "7", "8"});
	getParamQuantity(DENSITY_PARAM)->snapEnabled = true;
	configParam(GRAVITY_PARAM, 0.f, 1.f, 0.45f, "Gravity");
	configParam(TILT_PARAM, -1.f, 1.f, 0.f, "Tilt");
	configParam(BOUNCE_PARAM, 0.f, 1.f, 0.55f, "Bounce");
	configParam(DRAG_PARAM, 0.f, 1.f, 0.30f, "Drag");
	configParam(CHAOS_PARAM, 0.f, 1.f, 0.08f, "Chaos");
	configButton(CLEAR_PARAM, "Clear pearls");

	configInput(DROP_INPUT, "Drop trigger");
	configInput(GRAVITY_CV_INPUT, "Gravity CV");
	configInput(TILT_CV_INPUT, "Tilt CV");
	configInput(BOUNCE_CV_INPUT, "Bounce CV");
	configInput(CHAOS_CV_INPUT, "Chaos CV");
	configInput(CLEAR_INPUT, "Clear trigger");

	configOutput(GATES_OUTPUT, "Sink gates 1-8");
	configOutput(ANY_OUTPUT, "Any sink trigger");
	configOutput(LEFT_OUTPUT, "Left sinks trigger");
	configOutput(RIGHT_OUTPUT, "Right sinks trigger");
	configOutput(VEL_OUTPUT, "Capture velocity CV");
	configOutput(POS_OUTPUT, "Capture position CV");
	configOutput(ACT_OUTPUT, "Activity CV");

	controlDivider.setDivision(32);
	engine.setCapacity(32);
	updateCachedControls();
	publishSnapshot();
}

Umi::~Umi() {
	teardownTimer.begin(id);
}

void Umi::onReset() {
	maxBallsSetting.store(32, std::memory_order_relaxed);
	pulseLengthIndex.store(2, std::memory_order_relaxed);
	replaceOldestSetting.store(false, std::memory_order_relaxed);
	appliedMaxBalls = 32;
	appliedReplaceOldest = false;
	engine.setCapacity(32);
	engine.setReplaceOldest(false);
	resetBoard(1u);
}

float Umi::currentPulseLengthSeconds() const {
	const int index = clamp(pulseLengthIndex.load(std::memory_order_relaxed), 0, PULSE_LENGTH_COUNT - 1);
	return PULSE_LENGTHS[index];
}

void Umi::updateCachedControls() {
	const float rate = clamp(params[RATE_PARAM].getValue(), 0.f, 1.f);
	if (rate < 0.02f) {
		cachedRateHz = 0.f;
	}
	else {
		const float normalized = (rate - 0.02f) / 0.98f;
		cachedRateHz = 0.05f * std::exp2(normalized * std::log2(400.f));
	}
	cachedDensity = clamp(int(std::lround(params[DENSITY_PARAM].getValue())), 1, 8);

	const float gravityNorm = clamp(params[GRAVITY_PARAM].getValue() + normalizedCv(inputs[GRAVITY_CV_INPUT]), 0.f, 1.f);
	const float bounceNorm = clamp(params[BOUNCE_PARAM].getValue() + normalizedCv(inputs[BOUNCE_CV_INPUT]), 0.f, 1.f);
	physicsParams.gravity = 200.f + gravityNorm * 2000.f;
	physicsParams.tilt = clamp(params[TILT_PARAM].getValue() + normalizedCv(inputs[TILT_CV_INPUT]), -1.f, 1.f);
	physicsParams.restitution = 0.15f + bounceNorm * 0.77f;
	physicsParams.drag = clamp(params[DRAG_PARAM].getValue(), 0.f, 1.f) * 4.f;
	physicsParams.chaos = clamp(params[CHAOS_PARAM].getValue() + normalizedCv(inputs[CHAOS_CV_INPUT]), 0.f, 1.f);
}

void Umi::applySettings() {
	const int requestedMax = clampMaxBalls(maxBallsSetting.load(std::memory_order_relaxed));
	if (requestedMax != appliedMaxBalls) {
		engine.setCapacity(requestedMax);
		appliedMaxBalls = requestedMax;
	}
	const bool requestedReplace = replaceOldestSetting.load(std::memory_order_relaxed);
	if (requestedReplace != appliedReplaceOldest) {
		engine.setReplaceOldest(requestedReplace);
		appliedReplaceOldest = requestedReplace;
	}
}

void Umi::spawnDrop(float normalizedX) {
	if (engine.spawnBurst(cachedDensity, normalizedX) > 0) {
		dropSerial++;
		dropFlash = 1.f;
	}
}

void Umi::clearBoard(bool resetTiming) {
	engine.clear();
	activity = 0.f;
	if (resetTiming) {
		physicsAccumulator = 0.f;
		autoDropPhase = 0.f;
	}
	clearFlash = 1.f;
	publishSnapshot();
}

void Umi::resetBoard(std::uint32_t newSeed) {
	engine.reset(newSeed ? newSeed : 1u);
	engine.setCapacity(appliedMaxBalls);
	engine.setReplaceOldest(appliedReplaceOldest);
	publishedSeed.store(engine.getSeed(), std::memory_order_relaxed);
	physicsAccumulator = 0.f;
	autoDropPhase = 0.f;
	velocityCv = 0.f;
	positionCv = 0.f;
	activity = 0.f;
	dropSerial = 0;
	captureSerial.fill(0u);
	for (dsp::PulseGenerator& pulse : sinkPulses) pulse.reset();
	anyPulse.reset();
	leftPulse.reset();
	rightPulse.reset();
	publishSnapshot();
}

void Umi::handleCapture(const umi::CaptureEvent& event) {
	const int sinkIndex = clamp(int(event.sinkIndex), 0, umi::SINK_COUNT - 1);
	const float pulseLength = currentPulseLengthSeconds();
	sinkPulses[static_cast<std::size_t>(sinkIndex)].trigger(pulseLength);
	anyPulse.trigger(pulseLength);
	if (sinkIndex < 4) leftPulse.trigger(pulseLength);
	else rightPulse.trigger(pulseLength);
	velocityCv = clamp(event.speed / 2400.f, 0.f, 1.f) * 10.f;
	positionCv = float(sinkIndex) * (10.f / 7.f);
	activity = clamp(activity + 0.2f, 0.f, 1.f);
	captureSerial[static_cast<std::size_t>(sinkIndex)]++;
}

void Umi::publishSnapshot() {
	RenderSnapshot snapshot;
	snapshot.captureSerial = captureSerial;
	snapshot.dropSerial = dropSerial;
	snapshot.seed = engine.getSeed();
	snapshot.activity = activity;
	for (const umi::Ball& ball : engine.getBalls()) {
		if (!ball.active || snapshot.ballCount >= umi::MAX_BALLS) continue;
		BallRenderState& out = snapshot.balls[static_cast<std::size_t>(snapshot.ballCount++)];
		out.pos = ball.pos;
		out.vel = ball.vel;
		out.radius = ball.radius;
		out.age = ball.age;
		out.id = ball.id;
	}
	renderSnapshots.push(snapshot);
	publishedSeed.store(snapshot.seed, std::memory_order_relaxed);
}

bool Umi::consumeLatestSnapshot(RenderSnapshot* snapshot) {
	if (!snapshot) return false;
	RenderSnapshot next;
	bool consumed = false;
	while (renderSnapshots.pop(&next)) {
		*snapshot = next;
		consumed = true;
	}
	return consumed;
}

void Umi::process(const ProcessArgs& args) {
	if (controlDivider.process()) {
		updateCachedControls();
		applySettings();
	}

	UiCommand command;
	for (int i = 0; i < 15 && uiCommands.pop(&command); ++i) {
		switch (command.type) {
			case UiCommandType::DropAtX: spawnDrop(command.value); break;
			case UiCommandType::SetSeed: resetBoard(command.seed); break;
			case UiCommandType::Clear: clearBoard(false); break;
			case UiCommandType::ResetBoard: resetBoard(engine.getSeed()); break;
		}
	}

	if (dropButtonTrigger.process(params[DROP_PARAM].getValue())) spawnDrop();
	if (dropInputTrigger.process(inputs[DROP_INPUT].getVoltage(0), 0.1f, 1.f)) spawnDrop();
	if (clearButtonTrigger.process(params[CLEAR_PARAM].getValue())) clearBoard(false);
	if (clearInputTrigger.process(inputs[CLEAR_INPUT].getVoltage(0), 0.1f, 1.f)) clearBoard(false);

	if (cachedRateHz > 0.f) {
		autoDropPhase += cachedRateHz * args.sampleTime;
		if (autoDropPhase >= 1.f) {
			autoDropPhase -= std::floor(autoDropPhase);
			spawnDrop();
		}
	}
	else {
		autoDropPhase = 0.f;
	}

	physicsAccumulator += args.sampleTime;
	int physicsSteps = 0;
	while (physicsAccumulator >= umi::PHYSICS_DT && physicsSteps < 4) {
		const umi::StepEvents events = engine.step(physicsParams);
		for (int i = 0; i < events.captureCount; ++i) {
			handleCapture(events.captures[static_cast<std::size_t>(i)]);
		}
		const float occupancy = float(engine.getActiveCount()) / float(std::max(1, engine.getCapacity()));
		activity = std::max(occupancy, activity * 0.98816f);
		publishSnapshot();
		physicsAccumulator -= umi::PHYSICS_DT;
		physicsSteps++;
	}
	if (physicsSteps == 4 && physicsAccumulator >= umi::PHYSICS_DT) {
		physicsAccumulator = std::fmod(physicsAccumulator, umi::PHYSICS_DT);
	}

	bool anyHigh = false;
	outputs[GATES_OUTPUT].setChannels(umi::SINK_COUNT);
	for (int i = 0; i < umi::SINK_COUNT; ++i) {
		const bool high = sinkPulses[static_cast<std::size_t>(i)].process(args.sampleTime);
		outputs[GATES_OUTPUT].setVoltage(high ? 10.f : 0.f, i);
		lights[SINK1_LIGHT + i].setBrightnessSmooth(high ? 1.f : 0.f, args.sampleTime);
		anyHigh = anyHigh || high;
	}
	const bool anyPulseHigh = anyPulse.process(args.sampleTime);
	outputs[ANY_OUTPUT].setVoltage(anyPulseHigh ? 10.f : 0.f);
	outputs[LEFT_OUTPUT].setVoltage(leftPulse.process(args.sampleTime) ? 10.f : 0.f);
	outputs[RIGHT_OUTPUT].setVoltage(rightPulse.process(args.sampleTime) ? 10.f : 0.f);
	outputs[VEL_OUTPUT].setVoltage(velocityCv);
	outputs[POS_OUTPUT].setVoltage(positionCv);
	outputs[ACT_OUTPUT].setVoltage(activity * 10.f);

	dropFlash = std::max(0.f, dropFlash - args.sampleTime * 7.f);
	clearFlash = std::max(0.f, clearFlash - args.sampleTime * 7.f);
	lights[DROP_LIGHT].setBrightnessSmooth(dropFlash, args.sampleTime);
	lights[CLEAR_LIGHT].setBrightnessSmooth(clearFlash, args.sampleTime);
	lights[ANY_LIGHT].setBrightnessSmooth((anyPulseHigh || anyHigh) ? 1.f : 0.f, args.sampleTime);
}

json_t* Umi::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "schema", json_integer(1));
	json_object_set_new(rootJ, "seed", json_integer(publishedSeed.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "layout", json_integer(0));
	json_object_set_new(rootJ, "maxBalls", json_integer(maxBallsSetting.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "pulseLengthIndex", json_integer(pulseLengthIndex.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "replaceOldest", json_boolean(replaceOldestSetting.load(std::memory_order_relaxed)));
	return rootJ;
}

void Umi::dataFromJson(json_t* rootJ) {
	if (!rootJ || !json_is_object(rootJ)) return;
	std::uint32_t loadedSeed = 1u;
	if (json_t* valueJ = json_object_get(rootJ, "seed")) {
		if (json_is_integer(valueJ)) loadedSeed = std::uint32_t(json_integer_value(valueJ));
	}
	int loadedMaxBalls = 32;
	if (json_t* valueJ = json_object_get(rootJ, "maxBalls")) {
		if (json_is_integer(valueJ)) loadedMaxBalls = clampMaxBalls(int(json_integer_value(valueJ)));
	}
	int loadedPulseIndex = 2;
	if (json_t* valueJ = json_object_get(rootJ, "pulseLengthIndex")) {
		if (json_is_integer(valueJ)) loadedPulseIndex = clamp(int(json_integer_value(valueJ)), 0, PULSE_LENGTH_COUNT - 1);
	}
	bool loadedReplaceOldest = false;
	if (json_t* valueJ = json_object_get(rootJ, "replaceOldest")) {
		if (json_is_boolean(valueJ)) loadedReplaceOldest = json_boolean_value(valueJ);
	}
	maxBallsSetting.store(loadedMaxBalls, std::memory_order_relaxed);
	pulseLengthIndex.store(loadedPulseIndex, std::memory_order_relaxed);
	replaceOldestSetting.store(loadedReplaceOldest, std::memory_order_relaxed);
	appliedMaxBalls = loadedMaxBalls;
	appliedReplaceOldest = loadedReplaceOldest;
	engine.setCapacity(loadedMaxBalls);
	engine.setReplaceOldest(loadedReplaceOldest);
	resetBoard(loadedSeed);
}
