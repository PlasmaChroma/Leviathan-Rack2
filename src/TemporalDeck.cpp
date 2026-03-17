#include "plugin.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


namespace {

struct TemporalDeckBuffer {
	std::vector<float> left;
	std::vector<float> right;
	int size = 0;
	int writeHead = 0;
	int filled = 0;
	float sampleRate = 44100.f;

	void reset(float sr) {
		sampleRate = sr;
		size = std::max(1, int(std::round(sampleRate * 9.f)));
		left.assign(size, 0.f);
		right.assign(size, 0.f);
		writeHead = 0;
		filled = 0;
	}

	int wrapIndex(int index) const {
		if (size <= 0) {
			return 0;
		}
		index %= size;
		if (index < 0) {
			index += size;
		}
		return index;
	}

	float wrapPosition(float pos) const {
		if (size <= 0) {
			return 0.f;
		}
		pos = std::fmod(pos, float(size));
		if (pos < 0.f) {
			pos += float(size);
		}
		return pos;
	}

	void write(float inL, float inR) {
		if (size <= 0) {
			return;
		}
		left[writeHead] = inL;
		right[writeHead] = inR;
		writeHead = wrapIndex(writeHead + 1);
		filled = std::min(filled + 1, size);
	}

	static float cubicSample(float y0, float y1, float y2, float y3, float t) {
		float a0 = y3 - y2 - y0 + y1;
		float a1 = y0 - y1 - a0;
		float a2 = y2 - y0;
		float a3 = y1;
		return ((a0 * t + a1) * t + a2) * t + a3;
	}

	std::pair<float, float> readCubic(float pos) const {
		if (size <= 0 || filled <= 0) {
			return {0.f, 0.f};
		}
		pos = wrapPosition(pos);
		int i1 = int(std::floor(pos));
		float t = pos - float(i1);
		int i0 = wrapIndex(i1 - 1);
		int i2 = wrapIndex(i1 + 1);
		int i3 = wrapIndex(i1 + 2);
		return {
			cubicSample(left[i0], left[i1], left[i2], left[i3], t),
			cubicSample(right[i0], right[i1], right[i2], right[i3], t)
		};
	}
};


struct TemporalDeckEngine {
	static constexpr float kScratchGateThreshold = 1.f;
	static constexpr float kFreezeGateThreshold = 1.f;
	static constexpr float kSlipReturnTime = 0.12f;
	static constexpr float kSlipEnableReturnThreshold = 64.f;
	static constexpr float kSlipFinalCatchThresholdMs = 120.f;
	static constexpr float kSlipFinalCatchTime = 0.035f;
	static constexpr float kScratchFollowTime = 0.012f;
	static constexpr float kScratchSoftLagStepLimit = 10.0f;
	static constexpr float kMouseScratchTravelScale = 4.0f;
	static constexpr float kWheelScratchTravelScale = 4.5f;
	static constexpr float kInertiaBlend = 0.25f;
	static constexpr float kNominalPlatterRpm = 33.333333f;

	TemporalDeckBuffer buffer;
	float sampleRate = 44100.f;
	float readHead = 0.f;
	float timelineHead = 0.f;
	float platterPhase = 0.f;
	float platterVelocity = 0.f;
	bool freezeState = false;
	bool reverseState = false;
	bool slipState = false;
	bool positionCvOffsetMode = false;
	bool scratchActive = false;
	bool slipReturning = false;
	bool slipFinalCatchActive = false;
	float slipReturnRemaining = 0.f;
	float slipReturnStartLag = 0.f;
	float scratchLagSamples = 0.f;
	float scratchLagTargetSamples = 0.f;
	float wheelDeltaRemaining = 0.f;
	float lastPositionLag = 0.f;
	float lastPlatterLagTarget = 0.f;

	void reset(float sr) {
		sampleRate = sr;
		buffer.reset(sr);
		readHead = 0.f;
		timelineHead = 0.f;
		platterPhase = 0.f;
		platterVelocity = 0.f;
		scratchActive = false;
		slipReturning = false;
		slipFinalCatchActive = false;
		slipReturnRemaining = 0.f;
		slipReturnStartLag = 0.f;
		scratchLagSamples = 0.f;
		scratchLagTargetSamples = 0.f;
		wheelDeltaRemaining = 0.f;
		lastPositionLag = 0.f;
		lastPlatterLagTarget = 0.f;
	}

	float maxLagFromKnob(float knob) const {
		return clamp(knob, 0.f, 1.f) * sampleRate * 8.f;
	}

	float accessibleLag(float knob) const {
		return std::min(maxLagFromKnob(knob), float(buffer.filled));
	}

	float clampLag(float lag, float limit) const {
		return clamp(lag, 0.f, std::max(0.f, limit));
	}

	static float baseSpeedFromKnob(float rateKnob) {
		rateKnob = clamp(rateKnob, 0.f, 1.f);
		if (rateKnob < 0.5f) {
			float t = rateKnob / 0.5f;
			return -2.f + t * 3.f;
		}
		float t = (rateKnob - 0.5f) / 0.5f;
		return 1.f + t;
	}

	float computeBaseSpeed(float rateKnob, float rateCv, bool reverse) const {
		float speed = baseSpeedFromKnob(rateKnob);
		speed += clamp(rateCv / 5.f, -1.f, 1.f);
		speed = clamp(speed, -3.f, 3.f);
		if (reverse) {
			speed *= -1.f;
		}
		return speed;
	}

	float lagForPositionCv(float cv, float limit) const {
		return clamp((-cv / 5.f) * limit, 0.f, limit);
	}

	float currentLag() const {
		if (buffer.size <= 0) {
			return 0.f;
		}
		float lag = newestReadablePos() - readHead;
		if (lag < 0.f) {
			lag += float(buffer.size);
		}
		return lag;
	}

	float newestReadablePos() const {
		if (buffer.size <= 0 || buffer.filled <= 0) {
			return 0.f;
		}
		return buffer.wrapPosition(float(buffer.writeHead) - 1.f);
	}

	float platterRadiansPerSample() const {
		return (2.f * float(M_PI) * (kNominalPlatterRpm / 60.f)) / std::max(sampleRate, 1.f);
	}

	float samplesPerPlatterRadian() const {
		return 1.f / std::max(platterRadiansPerSample(), 1e-9f);
	}

	float unwrapReadNearWrite(float readPos) const {
		if (buffer.size <= 0) {
			return readPos;
		}
		float writePos = newestReadablePos();
		float sizeF = float(buffer.size);
		while (readPos > writePos) {
			readPos -= sizeF;
		}
		while (readPos <= writePos - sizeF) {
			readPos += sizeF;
		}
		return readPos;
	}

	struct FrameResult {
		float outL = 0.f;
		float outR = 0.f;
		float lag = 0.f;
		float accessibleLag = 0.f;
		float platterAngle = 0.f;
	};

	FrameResult process(
		float dt,
		float inL,
		float inR,
		float bufferKnob,
		float rateKnob,
		float mixKnob,
		float feedbackKnob,
		bool freezeButton,
		bool reverseButton,
		bool slipButton,
		bool freezeGate,
		bool scratchGate,
		bool positionConnected,
		float positionCv,
		float rateCv,
		bool platterTouched,
		float platterLagTarget,
		float platterGestureVelocity,
		float wheelDelta
	) {
		FrameResult result;
		float prevReadHead = readHead;
		freezeState = freezeButton || freezeGate;
		reverseState = reverseButton;
		bool prevSlipState = slipState;
		slipState = slipButton;

		float limit = accessibleLag(bufferKnob);
		float minLag = 0.f;
		float maxLag = std::max(limit, 0.f);
		float baseSpeed = computeBaseSpeed(rateKnob, rateCv, reverseState);
		float speed = baseSpeed;
		bool externalScratch = scratchGate && positionConnected;
		bool manualScratch = platterTouched;
		bool anyScratch = externalScratch || manualScratch;
		bool wasScratchActive = scratchActive;
		bool releasedFromScratch = !anyScratch && wasScratchActive;
		bool slipJustEnabled = slipState && !prevSlipState;

		if (!wasScratchActive && anyScratch) {
			scratchLagSamples = currentLag();
			scratchLagTargetSamples = scratchLagSamples;
			lastPlatterLagTarget = platterLagTarget;
		}
		scratchActive = anyScratch;

		if (!slipState) {
			slipReturning = false;
			slipFinalCatchActive = false;
		}

		if (releasedFromScratch && slipState) {
			slipReturning = true;
			slipFinalCatchActive = false;
			slipReturnRemaining = 0.f;
		}

		if (slipJustEnabled && !anyScratch) {
			if (currentLag() > kSlipEnableReturnThreshold) {
				slipReturning = true;
				slipFinalCatchActive = false;
			}
		}

		if (anyScratch) {
			slipReturning = false;
			slipFinalCatchActive = false;
		}

		// 3. Determine actual playhead (readHead)
		if (freezeState) {
			speed = 0.f;
		}

		float lagNow = currentLag();
		bool reverseAtOldestEdge = !scratchActive && !slipReturning && reverseState && limit > 0.f && lagNow >= (limit - 0.5f);
		if (reverseAtOldestEdge && speed < 0.f) {
			speed = 0.f;
		}

		if (manualScratch) {
			if (!freezeState) {
				// Holding platter still while time moves = increasing lag.
				scratchLagTargetSamples += 1.f;
			}
			// Add any wheel movement accumulation to the "to-be-applied" pool.
			wheelDeltaRemaining += wheelDelta;
			
			// Bleed wheel delta into target over time (~20ms reach).
			float wheelBleedAlpha = 0.002f; 
			float applyNow = wheelDeltaRemaining * wheelBleedAlpha;
			// Ensure small remainders are eventually applied.
			if (std::fabs(applyNow) < 0.1f && std::fabs(wheelDeltaRemaining) > 0.f) {
				applyNow = std::copysign(std::min(0.5f, std::fabs(wheelDeltaRemaining)), wheelDeltaRemaining);
			}
			scratchLagTargetSamples += applyNow;
			wheelDeltaRemaining -= applyNow;
			
			float platterDelta = platterLagTarget - lastPlatterLagTarget;
			scratchLagTargetSamples += platterDelta;
			platterVelocity += (platterGestureVelocity - platterVelocity) * kInertiaBlend;
			scratchLagTargetSamples = clampLag(scratchLagTargetSamples + platterVelocity * dt, limit);
			
			float followProgress = clamp(dt / std::max(kScratchFollowTime, 1e-6f), 0.f, 1.f);
			float shapedFollow = 1.f - std::pow(1.f - followProgress, 2.2f);
			float lagStep = (scratchLagTargetSamples - scratchLagSamples) * shapedFollow;
			lagStep = kScratchSoftLagStepLimit * std::tanh(lagStep / std::max(kScratchSoftLagStepLimit, 1e-6f));
			
			scratchLagSamples += lagStep;
			scratchLagSamples = clampLag(scratchLagSamples, limit);
			lastPlatterLagTarget = platterLagTarget;
			
			readHead = buffer.wrapPosition(newestReadablePos() - scratchLagSamples);
		}
		else if (externalScratch) {
			scratchLagSamples = lagForPositionCv(positionCv, limit);
			scratchLagTargetSamples = scratchLagSamples;
			readHead = buffer.wrapPosition(newestReadablePos() - scratchLagSamples);
		}
		else if (slipReturning) {
			// Return to NOW (lag = 0)
			float currentLagSamples = currentLag();
			float finalCatchThresholdSamples = sampleRate * (kSlipFinalCatchThresholdMs / 1000.f);

			if (!slipFinalCatchActive) {
				// Exponential-like approach to zero lag.
				// We target a specific lag value that decreases over time.
				float alpha = dt / std::max(kSlipReturnTime, 1e-6f);
				float targetLag = currentLagSamples * (1.f - alpha);
				
				// Ensure we actually move towards zero even if alpha is tiny.
				if (targetLag > currentLagSamples - 0.5f) {
					targetLag = currentLagSamples - 0.5f;
				}
				if (targetLag < 0.f) targetLag = 0.f;

				readHead = buffer.wrapPosition(newestReadablePos() - targetLag);
				
				if (targetLag <= finalCatchThresholdSamples) {
					slipFinalCatchActive = true;
					slipReturnRemaining = kSlipFinalCatchTime;
					slipReturnStartLag = targetLag;
				}
			}
			else {
				// Final catch phase: smooth snap to live input.
				slipReturnRemaining = std::max(0.f, slipReturnRemaining - dt);
				float progress = 1.f - clamp(slipReturnRemaining / std::max(kSlipFinalCatchTime, 1e-6f), 0.f, 1.f);
				float shapedProgress = 1.f - std::pow(1.f - progress, 2.5f);
				float targetLag = slipReturnStartLag * (1.f - shapedProgress);
				
				readHead = buffer.wrapPosition(newestReadablePos() - targetLag);
				
				if (slipReturnRemaining <= 0.f || targetLag < 0.5f) {
					readHead = newestReadablePos();
					slipReturning = false;
					slipFinalCatchActive = false;
				}
			}
		}
		else if (positionConnected && !externalScratch) {
			// Absolute Position CV
			float targetLag = lagForPositionCv(positionCv, limit);
			if (positionCvOffsetMode) {
				targetLag = clampLag(currentLag() + targetLag - lastPositionLag, limit);
				lastPositionLag = lagForPositionCv(positionCv, limit);
			}
			else {
				lastPositionLag = targetLag;
			}
			readHead = buffer.wrapPosition(newestReadablePos() - targetLag);
		}
		else {
			// Normal Transport
			float writePos = newestReadablePos();
			float candidate = unwrapReadNearWrite(readHead) + speed;
			candidate = clamp(candidate, writePos - maxLag, writePos - minLag);
			readHead = buffer.wrapPosition(candidate);
		}

		if (anyScratch) {
			slipReturning = false;
			slipFinalCatchActive = false;
			slipReturnRemaining = 0.f;
			slipReturnStartLag = 0.f;
		}

		bool holdAtScratchEdge = manualScratch && limit > 0.f && scratchLagSamples >= (limit - 0.5f);
		bool holdAtReverseEdge = reverseAtOldestEdge;
		bool holdAtBufferEdge = holdAtScratchEdge || holdAtReverseEdge;

		auto wet = buffer.readCubic(readHead);
		float mix = clamp(mixKnob, 0.f, 1.f);
		float outL = inL * (1.f - mix) + wet.first * mix;
		float outR = inR * (1.f - mix) + wet.second * mix;

		if (!freezeState && !holdAtBufferEdge) {
			float feedback = clamp(feedbackKnob, 0.f, 1.f);
			buffer.write(inL + outL * feedback, inR + outR * feedback);
		}

		if (buffer.size > 0) {
			float readDelta = readHead - prevReadHead;
			float halfSize = float(buffer.size) * 0.5f;
			if (readDelta > halfSize) {
				readDelta -= float(buffer.size);
			}
			if (readDelta < -halfSize) {
				readDelta += float(buffer.size);
			}
			platterPhase += readDelta * platterRadiansPerSample();
		}

			result.outL = outL;
			result.outR = outR;
			result.lag = currentLag();
			result.accessibleLag = limit;
			result.platterAngle = platterPhase;
			return result;
		}
	};


struct TemporalDeck;

struct TemporalDeckDisplayWidget : Widget {
	TemporalDeck* module = nullptr;
	Vec centerMm = mm2px(Vec(50.8f, 72.f));
	float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;

	void draw(const DrawArgs& args) override;
};


struct TemporalDeckPlatterWidget : OpaqueWidget {
	TemporalDeck* module = nullptr;
	Vec centerPx = mm2px(Vec(50.8f, 72.f));
	float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;
	float deadZonePx = 0.f;
	bool dragging = false;
	Vec onButtonPos;
	float lastAngle = 0.f;
	float localLagSamples = 0.f;

	Vec localCenter() const {
		return centerPx.minus(box.pos);
	}

	bool isWithinPlatter(Vec panelPos) const {
		Vec local = panelPos.minus(localCenter());
		float radius = local.norm();
		return radius <= platterRadiusPx;
	}

	void updateScratchFromLocal(Vec local, Vec mouseDelta);

	void draw(const DrawArgs& args) override;
	void onButton(const event::Button& e) override;
	void onHoverScroll(const event::HoverScroll& e) override;
	void onDragMove(const event::DragMove& e) override;
	void onDragStart(const event::DragStart& e) override;
	void onDragEnd(const event::DragEnd& e) override;
};

struct DeckRateQuantity : ParamQuantity {
	std::string getDisplayValueString() override {
		return string::f("%.2fx", TemporalDeckEngine::baseSpeedFromKnob(getValue()));
	}
};


struct TemporalDeck : Module {
	static constexpr float kUiPublishRateHz = 120.f;
	static constexpr float kUiPublishIntervalSec = 1.f / kUiPublishRateHz;

	enum ParamId {
		BUFFER_PARAM,
		RATE_PARAM,
		MIX_PARAM,
		FEEDBACK_PARAM,
		FREEZE_PARAM,
		REVERSE_PARAM,
		SLIP_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		POSITION_CV_INPUT,
		RATE_CV_INPUT,
		INPUT_L_INPUT,
		INPUT_R_INPUT,
		SCRATCH_GATE_INPUT,
		FREEZE_GATE_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_L_OUTPUT,
		OUTPUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		FREEZE_LIGHT,
		REVERSE_LIGHT,
		SLIP_LIGHT,
		LIGHTS_LEN
	};

	TemporalDeckEngine engine;
	dsp::SchmittTrigger freezeTrigger;
	dsp::SchmittTrigger reverseTrigger;
	dsp::SchmittTrigger slipTrigger;
	float cachedSampleRate = 0.f;
	bool freezeLatched = false;
	bool reverseLatched = false;
	bool slipLatched = false;
	std::atomic<bool> platterTouched {false};
	std::atomic<float> platterLagTarget {0.f};
	std::atomic<float> platterGestureVelocity {0.f};
	std::atomic<float> platterWheelDelta {0.f};
	std::atomic<int> platterScratchHoldSamples {0};
	std::atomic<float> uiLagSamples {0.f};
	std::atomic<float> uiAccessibleLagSamples {0.f};
	std::atomic<float> uiSampleRate {44100.f};
	std::atomic<float> uiPlatterAngle {0.f};
	float uiPublishTimerSec = 0.f;
	bool positionCvOffsetMode = false;

	TemporalDeck() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 8.f);
		configParam<DeckRateQuantity>(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
		configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix");
		configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
		configButton(FREEZE_PARAM, "Freeze");
		configButton(REVERSE_PARAM, "Reverse");
		configButton(SLIP_PARAM, "Slip");
		configInput(POSITION_CV_INPUT, "Position CV");
		configInput(RATE_CV_INPUT, "Rate CV");
		configInput(INPUT_L_INPUT, "Left audio");
		configInput(INPUT_R_INPUT, "Right audio");
		configInput(SCRATCH_GATE_INPUT, "Scratch gate");
		configInput(FREEZE_GATE_INPUT, "Freeze gate");
		configOutput(OUTPUT_L_OUTPUT, "Left audio");
		configOutput(OUTPUT_R_OUTPUT, "Right audio");
		onSampleRateChange();
	}

	void onSampleRateChange() override {
		cachedSampleRate = APP->engine->getSampleRate();
		engine.positionCvOffsetMode = positionCvOffsetMode;
		engine.reset(cachedSampleRate);
		uiSampleRate.store(cachedSampleRate);
		uiLagSamples.store(0.f);
		uiAccessibleLagSamples.store(0.f);
		uiPlatterAngle.store(0.f);
		uiPublishTimerSec = 0.f;
		platterScratchHoldSamples.store(0);
	}

	json_t* dataToJson() override {
		json_t* root = json_object();
		json_object_set_new(root, "freezeLatched", json_boolean(freezeLatched));
		json_object_set_new(root, "reverseLatched", json_boolean(reverseLatched));
		json_object_set_new(root, "slipLatched", json_boolean(slipLatched));
		json_object_set_new(root, "positionCvOffsetMode", json_boolean(positionCvOffsetMode));
		return root;
	}

	void dataFromJson(json_t* root) override {
		if (!root) {
			return;
		}
		json_t* freezeJ = json_object_get(root, "freezeLatched");
		json_t* reverseJ = json_object_get(root, "reverseLatched");
		json_t* slipJ = json_object_get(root, "slipLatched");
		json_t* offsetJ = json_object_get(root, "positionCvOffsetMode");
		if (freezeJ) {
			freezeLatched = json_boolean_value(freezeJ);
		}
		if (reverseJ) {
			reverseLatched = json_boolean_value(reverseJ);
		}
		if (slipJ) {
			slipLatched = json_boolean_value(slipJ);
		}
		if (offsetJ) {
			positionCvOffsetMode = json_boolean_value(offsetJ);
			engine.positionCvOffsetMode = positionCvOffsetMode;
		}
	}

	void process(const ProcessArgs& args) override {
		if (args.sampleRate != cachedSampleRate) {
			onSampleRateChange();
		}

		if (freezeTrigger.process(params[FREEZE_PARAM].getValue())) {
			bool next = !freezeLatched;
			freezeLatched = next;
			if (next) {
				reverseLatched = false;
				slipLatched = false;
			}
		}
		if (reverseTrigger.process(params[REVERSE_PARAM].getValue())) {
			bool next = !reverseLatched;
			reverseLatched = next;
			if (next) {
				freezeLatched = false;
				slipLatched = false;
			}
		}
		if (slipTrigger.process(params[SLIP_PARAM].getValue())) {
			bool next = !slipLatched;
			slipLatched = next;
			if (next) {
				freezeLatched = false;
				reverseLatched = false;
			}
		}

		float inL = inputs[INPUT_L_INPUT].getVoltage();
		float inR = inputs[INPUT_R_INPUT].isConnected() ? inputs[INPUT_R_INPUT].getVoltage() : inL;
		float positionCv = inputs[POSITION_CV_INPUT].getVoltage();
		float rateCv = inputs[RATE_CV_INPUT].getVoltage();

		engine.positionCvOffsetMode = positionCvOffsetMode;
		int scratchHold = platterScratchHoldSamples.load();
		bool wheelScratchHeld = scratchHold > 0;
		if (wheelScratchHeld) {
			platterScratchHoldSamples.store(std::max(0, scratchHold - 1));
		}
		float wheelDelta = platterWheelDelta.exchange(0.f);

		auto frame = engine.process(
			args.sampleTime,
			inL,
			inR,
			params[BUFFER_PARAM].getValue(),
			params[RATE_PARAM].getValue(),
			params[MIX_PARAM].getValue(),
			params[FEEDBACK_PARAM].getValue(),
			freezeLatched,
			reverseLatched,
			slipLatched,
			inputs[FREEZE_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kFreezeGateThreshold,
			inputs[SCRATCH_GATE_INPUT].getVoltage() >= TemporalDeckEngine::kScratchGateThreshold,
			inputs[POSITION_CV_INPUT].isConnected(),
			positionCv,
			rateCv,
			platterTouched.load() || wheelScratchHeld,
			platterLagTarget.load(),
			platterGestureVelocity.load(),
			wheelDelta
		);

		outputs[OUTPUT_L_OUTPUT].setVoltage(frame.outL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(frame.outR);
		lights[FREEZE_LIGHT].setBrightness(freezeLatched ? 1.f : 0.f);
		lights[REVERSE_LIGHT].setBrightness(reverseLatched ? 1.f : 0.f);
		lights[SLIP_LIGHT].setBrightness(slipLatched ? 1.f : 0.f);
		// Keep platter rotation responsive during scratch gestures.
		uiPlatterAngle.store(frame.platterAngle);

		uiPublishTimerSec += args.sampleTime;
		if (uiPublishTimerSec >= kUiPublishIntervalSec) {
			uiPublishTimerSec = std::fmod(uiPublishTimerSec, kUiPublishIntervalSec);
			uiLagSamples.store(frame.lag);
			uiAccessibleLagSamples.store(frame.accessibleLag);
			uiSampleRate.store(args.sampleRate);
		}
	}

	void setPlatterScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples = 0) {
		platterTouched.store(touched);
		platterLagTarget.store(lagSamples);
		platterGestureVelocity.store(velocitySamples);
		platterScratchHoldSamples.store(std::max(0, holdSamples));
		// Reset wheel delta when starting a new gesture or manual touch.
		if (touched || holdSamples == 0) {
			platterWheelDelta.store(0.f);
		}
	}

	void addPlatterWheelDelta(float delta, int holdSamples) {
		float current = platterWheelDelta.load();
		platterWheelDelta.store(current + delta);
		platterScratchHoldSamples.store(std::max(0, holdSamples));
	}
};


static bool loadSvgCircleMm(const std::string& svgPath, const std::string& circleId, Vec* outCenterMm, float* outRadiusMm) {
	std::ifstream svgFile(svgPath);
	if (!svgFile.good()) {
		return false;
	}
	std::ostringstream svgBuffer;
	svgBuffer << svgFile.rdbuf();
	const std::string svgText = svgBuffer.str();

	const std::regex tagRe("<circle\\b[^>]*\\bid\\s*=\\s*\"" + circleId + "\"[^>]*/?>", std::regex::icase);
	std::smatch tagMatch;
	if (!std::regex_search(svgText, tagMatch, tagRe) || tagMatch.empty()) {
		return false;
	}

	const std::string tag = tagMatch.str(0);
	auto parseAttr = [&](const char* attr, float* out) {
		const std::regex attrRe(std::string("\\b") + attr + "\\s*=\\s*\"([^\"]+)\"", std::regex::icase);
		std::smatch attrMatch;
		if (!std::regex_search(tag, attrMatch, attrRe)) {
			return false;
		}
		*out = std::stof(attrMatch.str(1));
		return true;
	};

	float cxMm = 0.f;
	float cyMm = 0.f;
	float radiusMm = 0.f;
	if (!parseAttr("cx", &cxMm) || !parseAttr("cy", &cyMm) || !parseAttr("r", &radiusMm)) {
		return false;
	}

	*outCenterMm = Vec(cxMm, cyMm);
	*outRadiusMm = radiusMm;
	return true;
}

static bool loadPlatterAnchor(Vec& centerPx, float& radiusPx) {
	Vec centerMm;
	float radiusMm = 0.f;
	if (!loadSvgCircleMm(asset::plugin(pluginInstance, "res/deck.svg"), "PLATTER_AREA", &centerMm, &radiusMm)) {
		return false;
	}
	centerPx = mm2px(centerMm);
	radiusPx = mm2px(Vec(radiusMm, 0.f)).x;
	return true;
}

static bool isLeftMouseDown() {
	return APP && APP->window && APP->window->win
		&& glfwGetMouseButton(APP->window->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}


void TemporalDeckDisplayWidget::draw(const DrawArgs& args) {
	if (!module) {
		return;
	}
	float accessibleLag = std::max(1.f, module->uiAccessibleLagSamples.load());
	float lag = clamp(module->uiLagSamples.load(), 0.f, accessibleLag);
	float maxLag = std::max(1.f, module->uiSampleRate.load() * 8.f);
	float lagRatio = clamp(lag / maxLag, 0.f, 1.f);
	float limitRatio = clamp(accessibleLag / maxLag, 0.f, 1.f);

	nvgSave(args.vg);
	nvgLineCap(args.vg, NVG_ROUND);

	float endAngle = 0.f;
	float lagAngle = endAngle - M_PI * lagRatio;
	float limitAngle = endAngle - M_PI * limitRatio;
	float arcRadius = platterRadiusPx + mm2px(Vec(3.5f, 0.f)).x;

	if (lagRatio > 0.f) {
		nvgBeginPath(args.vg);
		nvgArc(args.vg, centerMm.x, centerMm.y, arcRadius, lagAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(255, 228, 92, 16));
		nvgStrokeWidth(args.vg, mm2px(Vec(8.6f, 0.f)).x);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, centerMm.x, centerMm.y, arcRadius, lagAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(255, 224, 86, 34));
		nvgStrokeWidth(args.vg, mm2px(Vec(6.4f, 0.f)).x);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, centerMm.x, centerMm.y, arcRadius, lagAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(255, 218, 70, 78));
		nvgStrokeWidth(args.vg, mm2px(Vec(4.2f, 0.f)).x);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, centerMm.x, centerMm.y, arcRadius, lagAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(255, 214, 52, 242));
		nvgStrokeWidth(args.vg, mm2px(Vec(1.8f, 0.f)).x);
		nvgStroke(args.vg);
	}

	Vec dotPos = centerMm.plus(Vec(std::cos(limitAngle), std::sin(limitAngle)).mult(arcRadius));
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, dotPos.x, dotPos.y, mm2px(Vec(1.15f, 0.f)).x);
	nvgFillColor(args.vg, nvgRGBA(255, 220, 64, 245));
	nvgFill(args.vg);

	if (APP && APP->window && APP->window->uiFont) {
		float lagMs = 1000.f * lag / std::max(module->uiSampleRate.load(), 1.f);
		char text[32];
		std::snprintf(text, sizeof(text), "%.0f ms", lagMs);
		Vec textPos = centerMm.plus(Vec(arcRadius + mm2px(Vec(5.0f, 0.f)).x, -arcRadius * 0.86f));

		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, mm2px(Vec(3.4f, 0.f)).x);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

		nvgFontBlur(args.vg, 1.6f);
		nvgFillColor(args.vg, nvgRGBA(255, 214, 52, 72));
		nvgText(args.vg, textPos.x, textPos.y, text, nullptr);

		nvgFontBlur(args.vg, 0.f);
		nvgFillColor(args.vg, nvgRGBA(255, 238, 160, 230));
		nvgText(args.vg, textPos.x, textPos.y, text, nullptr);
	}
	nvgRestore(args.vg);
}


void TemporalDeckPlatterWidget::draw(const DrawArgs& args) {
	nvgSave(args.vg);
	float rotation = module ? module->uiPlatterAngle.load() : 0.f;
	Vec center = localCenter();

	NVGcolor outerDark = nvgRGB(20, 22, 26);
	NVGpaint vinylGrad = nvgRadialGradient(
		args.vg,
		center.x - platterRadiusPx * 0.18f,
		center.y - platterRadiusPx * 0.22f,
		platterRadiusPx * 0.15f,
		platterRadiusPx * 1.05f,
		nvgRGBA(52, 56, 64, 220),
		outerDark
	);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
	nvgFillPaint(args.vg, vinylGrad);
	nvgFill(args.vg);

	// Optimization: Skip complex grooves if platter is too small to see them clearly
	if (platterRadiusPx > 10.f) {
		nvgSave(args.vg);
		nvgTranslate(args.vg, center.x, center.y);
		nvgRotate(args.vg, rotation * 0.92f);
		
		for (int i = 0; i < 16; ++i) {
			float grooveRadius = platterRadiusPx * (0.24f + 0.047f * i);
			float alpha = (i % 2 == 0) ? 34.f : 18.f;
			float wobbleAmp = 0.55f + 0.05f * float(i % 4);
			float wobblePhase = 0.47f * float(i) + 0.061f * float(i * i);
			float wobbleFreq = 3.1f + 0.23f * float((i * 2 + 1) % 5);
			float ringRotation = 0.19f * float(i) + 0.043f * float(i * i);
			
			nvgBeginPath(args.vg);
			constexpr int kSteps = 64; // Reduced from 96 for performance
			for (int step = 0; step <= kSteps; ++step) {
				float t = 2.f * float(M_PI) * float(step) / float(kSteps) + ringRotation;
				// Simplified wobble: removed expensive pow and copysign
				float wobble = std::sin(t * wobbleFreq + wobblePhase);
				float radius = grooveRadius + wobbleAmp * wobble;
				float x = std::cos(t) * radius;
				float y = std::sin(t) * radius;
				if (step == 0) nvgMoveTo(args.vg, x, y);
				else nvgLineTo(args.vg, x, y);
			}
			nvgStrokeColor(args.vg, nvgRGBA(210, 218, 228, (unsigned char) alpha));
			nvgStrokeWidth(args.vg, 0.7f);
			nvgStroke(args.vg);
		}
		nvgRestore(args.vg);
	}

	float labelRadius = platterRadiusPx * 0.33f;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, labelRadius);
	nvgFillColor(args.vg, nvgRGB(138, 86, 34));
	nvgFill(args.vg);

	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgRotate(args.vg, rotation);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, 0.f, 0.f, labelRadius * 0.74f);
	nvgFillColor(args.vg, nvgRGB(196, 155, 87));
	nvgFill(args.vg);

	for (int i = 0; i < 3; ++i) {
		float angle = 2.f * float(M_PI) * float(i) / 3.f;
		Vec a(std::cos(angle) * labelRadius * 0.22f, std::sin(angle) * labelRadius * 0.22f);
		Vec b(std::cos(angle) * labelRadius * 0.62f, std::sin(angle) * labelRadius * 0.62f);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, a.x, a.y);
		nvgLineTo(args.vg, b.x, b.y);
		nvgStrokeColor(args.vg, nvgRGBA(90, 52, 19, 170));
		nvgStrokeWidth(args.vg, 1.2f);
		nvgStroke(args.vg);
	}

	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, -labelRadius * 0.42f, -labelRadius * 0.055f, labelRadius * 0.84f, labelRadius * 0.11f, 1.2f);
	nvgFillColor(args.vg, nvgRGBA(120, 72, 28, 120));
	nvgFill(args.vg);

	nvgRestore(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, labelRadius * 0.12f);
	nvgFillColor(args.vg, nvgRGB(222, 228, 235));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, platterRadiusPx);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 32));
	nvgStrokeWidth(args.vg, 1.1f);
	nvgStroke(args.vg);

	nvgRestore(args.vg);
	Widget::draw(args);
}

void TemporalDeckPlatterWidget::updateScratchFromLocal(Vec local, Vec mouseDelta) {
	if (!module || !dragging) {
		return;
	}
	float radius = local.norm();
	if (radius < deadZonePx * 0.25f) {
		return;
	}
	float angle = std::atan2(local.y, local.x);
	float deltaAngle = angle - lastAngle;
	if (deltaAngle > M_PI) {
		deltaAngle -= 2.f * M_PI;
	}
	if (deltaAngle < -M_PI) {
		deltaAngle += 2.f * M_PI;
	}
	float effectiveRadius = std::max(radius, deadZonePx);
	float weight = clamp(effectiveRadius / platterRadiusPx, 0.3f, 1.f);
	float samplesPerRadian = 60.f * module->uiSampleRate.load()
		/ (2.f * float(M_PI) * TemporalDeckEngine::kNominalPlatterRpm)
		* TemporalDeckEngine::kMouseScratchTravelScale;
	float lagDelta = deltaAngle * samplesPerRadian * weight;
	localLagSamples = clamp(localLagSamples - lagDelta, 0.f, module->uiAccessibleLagSamples.load());
	float velocity = (std::fabs(mouseDelta.x) + std::fabs(mouseDelta.y)) * module->uiSampleRate.load() * 0.0005f;
	if (deltaAngle < 0.f) {
		velocity *= -1.f;
	}
	module->setPlatterScratch(true, localLagSamples, velocity);
	lastAngle = angle;
}

void TemporalDeckPlatterWidget::onButton(const event::Button& e) {
	onButtonPos = e.pos;
	if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS && isWithinPlatter(e.pos)) {
		e.consume(this);
		return;
	}
	Widget::onButton(e);
}

void TemporalDeckPlatterWidget::onHoverScroll(const event::HoverScroll& e) {
	if (!module || !isWithinPlatter(e.pos)) {
		OpaqueWidget::onHoverScroll(e);
		return;
	}

	float scroll = -e.scrollDelta.y;
	if (std::fabs(scroll) < 1e-4f) {
		OpaqueWidget::onHoverScroll(e);
		return;
	}

	float maxLag = module->uiAccessibleLagSamples.load();
	if (maxLag <= 0.f) {
		e.consume(this);
		return;
	}

	float sampleRate = module->uiSampleRate.load();
	float samplesPerNotch = sampleRate * 0.008f * TemporalDeckEngine::kWheelScratchTravelScale;
	float lagDelta = scroll * samplesPerNotch;
	float holdSeconds = module->slipLatched ? 0.09f : 0.02f;
	int holdSamples = std::max(1, int(std::round(sampleRate * holdSeconds)));
	
	module->addPlatterWheelDelta(lagDelta, holdSamples);
	e.consume(this);
}

void TemporalDeckPlatterWidget::onDragStart(const event::DragStart& e) {
	if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || !isWithinPlatter(onButtonPos)) {
		return;
	}
	Vec local = onButtonPos.minus(localCenter());
	dragging = true;
	lastAngle = std::atan2(local.y, local.x);
	localLagSamples = module->uiLagSamples.load();
	module->setPlatterScratch(true, localLagSamples, 0.f);
	e.consume(this);
}

void TemporalDeckPlatterWidget::onDragMove(const event::DragMove& e) {
	if (!dragging || e.button != GLFW_MOUSE_BUTTON_LEFT) {
		return;
	}
	if (!isLeftMouseDown()) {
		dragging = false;
		if (module) {
			module->setPlatterScratch(false, localLagSamples, 0.f);
		}
		return;
	}
	Vec local = APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos).minus(localCenter());
	updateScratchFromLocal(local, e.mouseDelta);
	e.consume(this);
}

void TemporalDeckPlatterWidget::onDragEnd(const event::DragEnd& e) {
	if (dragging && e.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (isLeftMouseDown()) {
			e.consume(this);
			return;
		}
		dragging = false;
		if (module) {
			module->setPlatterScratch(false, localLagSamples, 0.f);
		}
		e.consume(this);
	}
}


struct TemporalDeckWidget : ModuleWidget {
	TemporalDeckWidget(TemporalDeck* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/deck.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.0, 18.0)), module, TemporalDeck::BUFFER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.0, 18.0)), module, TemporalDeck::RATE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(62.0, 18.0)), module, TemporalDeck::MIX_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(84.0, 18.0)), module, TemporalDeck::FEEDBACK_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(32.0, 100.0)), module, TemporalDeck::FREEZE_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(50.8, 100.0)), module, TemporalDeck::REVERSE_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(69.0, 100.0)), module, TemporalDeck::SLIP_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.0, 72.0)), module, TemporalDeck::POSITION_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(89.0, 72.0)), module, TemporalDeck::RATE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.0, 118.0)), module, TemporalDeck::INPUT_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.0, 118.0)), module, TemporalDeck::INPUT_R_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.0, 118.0)), module, TemporalDeck::SCRATCH_GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.0, 118.0)), module, TemporalDeck::FREEZE_GATE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(78.0, 118.0)), module, TemporalDeck::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(94.0, 118.0)), module, TemporalDeck::OUTPUT_R_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(32.0, 94.2)), module, TemporalDeck::FREEZE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(50.8, 94.2)), module, TemporalDeck::REVERSE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(69.0, 94.2)), module, TemporalDeck::SLIP_LIGHT));

		Vec platterCenter = mm2px(Vec(50.8f, 72.f));
		float platterRadius = mm2px(Vec(29.5f, 0.f)).x;
		loadPlatterAnchor(platterCenter, platterRadius);

		auto display = new TemporalDeckDisplayWidget();
		display->module = module;
		display->centerMm = platterCenter;
		display->platterRadiusPx = platterRadius;
		display->box.size = box.size;
		addChild(display);

		auto platter = new TemporalDeckPlatterWidget();
		platter->module = module;
		platter->centerPx = platterCenter;
		platter->platterRadiusPx = platterRadius;
		platter->deadZonePx = platterRadius * 0.08f;
		platter->box.pos = platterCenter.minus(Vec(platterRadius, platterRadius));
		platter->box.size = Vec(platterRadius * 2.f, platterRadius * 2.f);
		addChild(platter);
	}

	void appendContextMenu(Menu* menu) override {
		TemporalDeck* module = dynamic_cast<TemporalDeck*>(this->module);
		assert(menu);
		menu->addChild(new MenuSeparator());
		menu->addChild(createBoolPtrMenuItem("Position CV offset mode", "", module ? &module->positionCvOffsetMode : nullptr));
	}
};

} // namespace

Model* modelTemporalDeck = createModel<TemporalDeck, TemporalDeckWidget>("TemporalDeck");
