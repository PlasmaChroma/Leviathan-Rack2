#include "plugin.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <regex>
#include <string>
#include <utility>
#include <vector>


struct TemporalDeckBuffer {
	std::vector<float> left;
	std::vector<float> right;
	int size = 0;
	int writeHead = 0;
	int filled = 0;
	float sampleRate = 44100.f;

	void reset(float sr) {
		sampleRate = sr;
		size = std::max(1, int(std::round(sampleRate * 8.f)));
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

	float zeroCrossCatch(float targetPos, float minLag, float maxLag) const {
		if (size <= 0 || filled <= 0) {
			return wrapPosition(targetPos);
		}
		float bestPos = wrapPosition(targetPos);
		float bestScore = std::numeric_limits<float>::infinity();
		for (int offset = -24; offset <= 24; ++offset) {
			float candidate = wrapPosition(targetPos + float(offset));
			float lag = float(writeHead) - candidate;
			if (lag < 0.f) {
				lag += float(size);
			}
			if (lag < minLag || lag > maxLag) {
				continue;
			}
			auto sample = readCubic(candidate);
			float score = std::fabs(sample.first) + std::fabs(sample.second);
			if (score < bestScore) {
				bestScore = score;
				bestPos = candidate;
			}
		}
		return bestPos;
	}
};


struct TemporalDeckEngine {
	static constexpr float kScratchGateThreshold = 1.f;
	static constexpr float kFreezeGateThreshold = 1.f;
	static constexpr float kSlipReturnTime = 0.12f;
	static constexpr float kInertiaBlend = 0.25f;

	TemporalDeckBuffer buffer;
	float sampleRate = 44100.f;
	float readHead = 0.f;
	float timelineHead = 0.f;
	float platterVelocity = 0.f;
	bool freezeState = false;
	bool reverseState = false;
	bool slipState = false;
	bool positionCvOffsetMode = false;
	bool scratchActive = false;
	bool slipReturning = false;
	float scratchLagSamples = 0.f;
	float lastPositionLag = 0.f;

	void reset(float sr) {
		sampleRate = sr;
		buffer.reset(sr);
		readHead = 0.f;
		timelineHead = 0.f;
		platterVelocity = 0.f;
		scratchActive = false;
		slipReturning = false;
		scratchLagSamples = 0.f;
		lastPositionLag = 0.f;
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

	float computeBaseSpeed(float rateKnob, float rateCv, bool reverse) const {
		float speed = (clamp(rateKnob, 0.f, 1.f) - 0.5f) * 4.f;
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
		float lag = float(buffer.writeHead) - readHead;
		if (lag < 0.f) {
			lag += float(buffer.size);
		}
		return lag;
	}

	struct FrameResult {
		float outL = 0.f;
		float outR = 0.f;
		float lag = 0.f;
		float accessibleLag = 0.f;
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
		float platterGestureVelocity
	) {
		FrameResult result;
		freezeState = freezeButton || freezeGate;
		reverseState = reverseButton;
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

		if (slipState && !wasScratchActive && anyScratch) {
			timelineHead = readHead;
		}
		if (!wasScratchActive && anyScratch) {
			scratchLagSamples = currentLag();
		}
		scratchActive = anyScratch;

		if (freezeState) {
			speed = 0.f;
		}

		if (!scratchActive) {
			readHead = buffer.wrapPosition(readHead + speed);
			float lag = float(buffer.writeHead) - readHead;
			if (lag < 0.f) {
				lag += float(buffer.size);
			}
			if (lag < minLag) {
				readHead = float(buffer.writeHead) - minLag;
			}
			if (lag > maxLag) {
				readHead = float(buffer.writeHead) - maxLag;
			}
			readHead = buffer.wrapPosition(readHead);
			if (slipState) {
				timelineHead = readHead;
			}
		}

		if (positionConnected && !externalScratch && !manualScratch) {
			float targetLag = lagForPositionCv(positionCv, limit);
			if (positionCvOffsetMode) {
				targetLag = clampLag(currentLag() + targetLag - lastPositionLag, limit);
				lastPositionLag = lagForPositionCv(positionCv, limit);
			}
			else {
				lastPositionLag = targetLag;
			}
			float targetReadHead = buffer.wrapPosition(float(buffer.writeHead) - targetLag);
			targetReadHead = buffer.zeroCrossCatch(targetReadHead, minLag, maxLag);
			readHead = targetReadHead;
			if (slipState) {
				timelineHead = buffer.wrapPosition(timelineHead + speed);
			}
		}

		if (manualScratch) {
			platterVelocity += (platterGestureVelocity - platterVelocity) * kInertiaBlend;
			scratchLagSamples = clampLag(platterLagTarget + platterVelocity * dt, limit);
			readHead = buffer.wrapPosition(float(buffer.writeHead) - scratchLagSamples);
			if (slipState) {
				timelineHead = buffer.wrapPosition(timelineHead + speed);
			}
		}
		else if (externalScratch) {
			scratchLagSamples = lagForPositionCv(positionCv, limit);
			readHead = buffer.wrapPosition(float(buffer.writeHead) - scratchLagSamples);
			if (slipState) {
				timelineHead = buffer.wrapPosition(timelineHead + speed);
			}
		}
		else if (slipState && slipReturning) {
			float returnMix = clamp(dt / kSlipReturnTime, 0.f, 1.f);
			float target = buffer.zeroCrossCatch(timelineHead, minLag, maxLag);
			float delta = target - readHead;
			if (delta > float(buffer.size) * 0.5f) {
				delta -= float(buffer.size);
			}
			if (delta < -float(buffer.size) * 0.5f) {
				delta += float(buffer.size);
			}
			readHead = buffer.wrapPosition(readHead + delta * returnMix);
			if (std::fabs(delta) < 1.f) {
				readHead = target;
				slipReturning = false;
			}
		}

		if (!anyScratch && slipState && !slipReturning && wasScratchActive) {
			slipReturning = true;
		}
		if (anyScratch) {
			slipReturning = false;
		}

		auto wet = buffer.readCubic(readHead);
		float mix = clamp(mixKnob, 0.f, 1.f);
		float outL = inL * (1.f - mix) + wet.first * mix;
		float outR = inR * (1.f - mix) + wet.second * mix;

		if (!freezeState) {
			float feedback = clamp(feedbackKnob, 0.f, 1.f);
			buffer.write(inL + outL * feedback, inR + outR * feedback);
		}

		result.outL = outL;
		result.outR = outR;
		result.lag = currentLag();
		result.accessibleLag = limit;
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


struct TemporalDeckPlatterWidget : Widget {
	TemporalDeck* module = nullptr;
	Vec centerMm = mm2px(Vec(50.8f, 72.f));
	float platterRadiusPx = mm2px(Vec(29.5f, 0.f)).x;
	float deadZonePx = 0.f;
	bool dragging = false;
	float lastAngle = 0.f;
	float localLagSamples = 0.f;

	bool isWithinActivePlatter(Vec panelPos) const {
		Vec local = panelPos.minus(centerMm);
		float radius = local.norm();
		return radius >= deadZonePx && radius <= platterRadiusPx;
	}

	void onButton(const event::Button& e) override;
	void onDragStart(const event::DragStart& e) override;
	void onDragMove(const event::DragMove& e) override;
	void onDragEnd(const event::DragEnd& e) override;
};


struct TemporalDeck : Module {
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
	std::atomic<float> uiLagSamples {0.f};
	std::atomic<float> uiAccessibleLagSamples {0.f};
	std::atomic<float> uiSampleRate {44100.f};
	bool positionCvOffsetMode = false;

	TemporalDeck() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BUFFER_PARAM, 0.f, 1.f, 1.f, "Buffer", " s", 0.f, 8.f);
		configParam(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
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
			freezeLatched = !freezeLatched;
		}
		if (reverseTrigger.process(params[REVERSE_PARAM].getValue())) {
			reverseLatched = !reverseLatched;
		}
		if (slipTrigger.process(params[SLIP_PARAM].getValue())) {
			slipLatched = !slipLatched;
		}

		float inL = inputs[INPUT_L_INPUT].getVoltage();
		float inR = inputs[INPUT_R_INPUT].isConnected() ? inputs[INPUT_R_INPUT].getVoltage() : inL;
		float positionCv = inputs[POSITION_CV_INPUT].getVoltage();
		float rateCv = inputs[RATE_CV_INPUT].getVoltage();

		engine.positionCvOffsetMode = positionCvOffsetMode;
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
			platterTouched.load(),
			platterLagTarget.load(),
			platterGestureVelocity.load()
		);

		outputs[OUTPUT_L_OUTPUT].setVoltage(frame.outL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(frame.outR);
		lights[FREEZE_LIGHT].setBrightness(freezeLatched ? 1.f : 0.f);
		lights[REVERSE_LIGHT].setBrightness(reverseLatched ? 1.f : 0.f);
		lights[SLIP_LIGHT].setBrightness(slipLatched ? 1.f : 0.f);
		uiLagSamples.store(frame.lag);
		uiAccessibleLagSamples.store(frame.accessibleLag);
		uiSampleRate.store(args.sampleRate);
	}

	void setPlatterScratch(bool touched, float lagSamples, float velocitySamples) {
		platterTouched.store(touched);
		platterLagTarget.store(lagSamples);
		platterGestureVelocity.store(velocitySamples);
	}
};


static bool loadPlatterAnchor(Vec& centerPx, float& radiusPx) {
	std::ifstream svg(asset::plugin(pluginInstance, "res/deck.svg"));
	if (!svg.is_open()) {
		return false;
	}
	std::string text((std::istreambuf_iterator<char>(svg)), std::istreambuf_iterator<char>());
	std::regex re("id=\"PLATTER_AREA\"[\\s\\S]*?cx=\"([^\"]+)\"[\\s\\S]*?cy=\"([^\"]+)\"[\\s\\S]*?r=\"([^\"]+)\"");
	std::smatch match;
	if (!std::regex_search(text, match, re) || match.size() != 4) {
		return false;
	}
	float cx = std::stof(match[1].str());
	float cy = std::stof(match[2].str());
	float r = std::stof(match[3].str());
	centerPx = mm2px(Vec(cx, cy));
	radiusPx = mm2px(Vec(r, 0.f)).x;
	return true;
}


void TemporalDeckDisplayWidget::draw(const DrawArgs& args) {
	if (!module) {
		return;
	}
	float lag = module->uiLagSamples.load();
	float accessibleLag = std::max(1.f, module->uiAccessibleLagSamples.load());
	float maxLag = std::max(1.f, module->uiSampleRate.load() * 8.f);
	float lagRatio = clamp(lag / maxLag, 0.f, 1.f);
	float limitRatio = clamp(accessibleLag / maxLag, 0.f, 1.f);

	nvgSave(args.vg);
	nvgLineCap(args.vg, NVG_ROUND);

	float startAngle = 0.f;
	float lagAngle = -M_PI * lagRatio;
	float limitAngle = -M_PI * limitRatio;
	float arcRadius = platterRadiusPx + mm2px(Vec(3.5f, 0.f)).x;

	nvgBeginPath(args.vg);
	nvgArc(args.vg, centerMm.x, centerMm.y, arcRadius, startAngle, lagAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(244, 210, 75, 220));
	nvgStrokeWidth(args.vg, mm2px(Vec(1.8f, 0.f)).x);
	nvgStroke(args.vg);

	Vec dotPos = centerMm.plus(Vec(std::cos(limitAngle), std::sin(limitAngle)).mult(arcRadius));
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, dotPos.x, dotPos.y, mm2px(Vec(1.15f, 0.f)).x);
	nvgFillColor(args.vg, nvgRGBA(244, 210, 75, 240));
	nvgFill(args.vg);
	nvgRestore(args.vg);
}


void TemporalDeckPlatterWidget::onButton(const event::Button& e) {
	if (e.button == GLFW_MOUSE_BUTTON_LEFT && isWithinActivePlatter(e.pos)) {
		e.consume(this);
	}
	Widget::onButton(e);
}

void TemporalDeckPlatterWidget::onDragStart(const event::DragStart& e) {
	if (!module) {
		return;
	}
	Vec panelPos = APP->scene->rack->getMousePos().minus(getAbsoluteOffset(Vec()));
	if (!isWithinActivePlatter(panelPos)) {
		return;
	}
	Vec local = panelPos.minus(centerMm);
	dragging = true;
	lastAngle = std::atan2(local.y, local.x);
	localLagSamples = module->uiLagSamples.load();
	module->setPlatterScratch(true, localLagSamples, 0.f);
	e.consume(this);
}

void TemporalDeckPlatterWidget::onDragMove(const event::DragMove& e) {
	if (!module || !dragging) {
		return;
	}
	Vec panelPos = APP->scene->rack->getMousePos().minus(getAbsoluteOffset(Vec()));
	Vec local = panelPos.minus(centerMm);
	float radius = local.norm();
	if (radius < deadZonePx) {
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
	float weight = clamp(radius / platterRadiusPx, 0.3f, 1.f);
	float lagDelta = deltaAngle * (module->uiSampleRate.load() / float(M_PI)) * weight;
	localLagSamples = clamp(localLagSamples - lagDelta, 0.f, module->uiAccessibleLagSamples.load());
	float velocity = (std::fabs(e.mouseDelta.x) + std::fabs(e.mouseDelta.y)) * module->uiSampleRate.load() * 0.0005f;
	if (deltaAngle < 0.f) {
		velocity *= -1.f;
	}
	module->setPlatterScratch(true, localLagSamples, velocity);
	lastAngle = angle;
	e.consume(this);
}

void TemporalDeckPlatterWidget::onDragEnd(const event::DragEnd& e) {
	if (!module) {
		return;
	}
	dragging = false;
	module->setPlatterScratch(false, localLagSamples, 0.f);
	e.consume(this);
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
		platter->centerMm = platterCenter;
		platter->platterRadiusPx = platterRadius;
		platter->deadZonePx = platterRadius * 0.2f;
		platter->box.size = box.size;
		addChild(platter);
	}

	void appendContextMenu(Menu* menu) override {
		TemporalDeck* module = dynamic_cast<TemporalDeck*>(this->module);
		assert(menu);
		menu->addChild(new MenuSeparator());
		menu->addChild(createBoolPtrMenuItem("Position CV offset mode", "", module ? &module->positionCvOffsetMode : nullptr));
	}
};


Model* modelTemporalDeck = createModel<TemporalDeck, TemporalDeckWidget>("TemporalDeck");
