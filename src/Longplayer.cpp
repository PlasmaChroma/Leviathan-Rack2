#include "Longplayer.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace {

float playbackRateFromKnob(float knob) {
	knob = clamp(knob, 0.f, 1.f);
	if (knob < 0.5f) {
		return 0.5f + knob;
	}
	return knob * 2.f;
}

struct LongplayerRateQuantity : ParamQuantity {
	static float valueForRate(float rate) {
		rate = clamp(rate, 0.5f, 2.f);
		return rate <= 1.f ? rate - 0.5f : rate * 0.5f;
	}

	float getDisplayValue() override {
		return playbackRateFromKnob(getValue());
	}

	void setDisplayValue(float displayValue) override {
		setImmediateValue(valueForRate(displayValue));
	}

	std::string getDisplayValueString() override {
		return string::f("%.2fx", playbackRateFromKnob(getValue()));
	}
};

std::string lowercaseExtension(const std::string& path) {
	std::string extension = system::getExtension(path);
	std::transform(
		extension.begin(), extension.end(), extension.begin(),
		[](unsigned char value) { return char(std::tolower(value)); });
	return extension;
}

} // namespace

Longplayer::Longplayer() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(PLAY_PARAM, 0.f, 1.f, 0.f, "Transport", {"Paused", "Playing"});
	configSwitch(LOOP_PARAM, 0.f, 1.f, 0.f, "Loop", {"Off", "On"});
	configParam<LongplayerRateQuantity>(RATE_PARAM, 0.f, 1.f, 0.5f, "Rate");
	configInput(TRIGGER_INPUT, "Restart and play");
	configOutput(LEFT_OUTPUT, "Left audio");
	configOutput(RIGHT_OUTPUT, "Right audio");
	transitionLength = std::max(1, int(44100.f * 0.005f));
}

Longplayer::~Longplayer() {
	teardownTimer.begin(id);
}

void Longplayer::beginTransition() {
	transitionLeft = lastLeft;
	transitionRight = lastRight;
	transitionSamples = transitionLength;
}

void Longplayer::process(const ProcessArgs& args) {
	const std::uint64_t generation = stream.generation();
	if (generation != observedGeneration) {
		observedGeneration = generation;
		playhead = 0.0;
		beginTransition();
	}

	if (trigger.process(inputs[TRIGGER_INPUT].getVoltage())) {
		playhead = 0.0;
		params[PLAY_PARAM].setValue(1.f);
		beginTransition();
	}

	const std::uint32_t seekRevision =
		requestedSeekRevision.load(std::memory_order_acquire);
	const std::uint64_t totalFrames = stream.totalFrames();
	if (seekRevision != appliedSeekRevision && totalFrames > 0u) {
		appliedSeekRevision = seekRevision;
		const float normalized = clamp(
			requestedSeek.load(std::memory_order_relaxed), 0.f, 1.f);
		playhead = double(totalFrames - 1u) * normalized;
		beginTransition();
	}

	const bool playing = params[PLAY_PARAM].getValue() > 0.5f;
	const bool loop = params[LOOP_PARAM].getValue() > 0.5f;
	if (playing && !wasPlaying) {
		transitionLeft = 0.f;
		transitionRight = 0.f;
		transitionSamples = transitionLength;
	}
	wasPlaying = playing;

	float outputLeft = lastLeft;
	float outputRight = lastRight;
	bool buffering = false;
	if (playing && stream.ready() && totalFrames > 0u) {
		const std::uint64_t frame0 = std::min(
			std::uint64_t(playhead), totalFrames - 1u);
		std::uint64_t frame1 = frame0 + 1u;
		if (frame1 >= totalFrames) {
			frame1 = loop ? 0u : frame0;
		}
		float left0 = 0.f;
		float right0 = 0.f;
		float left1 = 0.f;
		float right1 = 0.f;
		const bool available = stream.readFrame(frame0, &left0, &right0)
			&& stream.readFrame(frame1, &left1, &right1);
		if (available) {
			const float fraction = float(playhead - double(frame0));
			const float sampleLeft = (left0 + (left1 - left0) * fraction) * 5.f;
			const float sampleRight = (right0 + (right1 - right0) * fraction) * 5.f;
			if (transitionSamples > 0) {
				const float blend = 1.f
					- float(transitionSamples) / float(transitionLength);
				outputLeft = transitionLeft
					+ (sampleLeft - transitionLeft) * blend;
				outputRight = transitionRight
					+ (sampleRight - transitionRight) * blend;
				--transitionSamples;
			}
			else {
				outputLeft = sampleLeft;
				outputRight = sampleRight;
			}

			const double sourceRate = double(stream.sampleRate());
			const float playbackRate = playbackRateFromKnob(
				params[RATE_PARAM].getValue());
			playhead += sourceRate * args.sampleTime * playbackRate;
			if (playhead >= double(totalFrames)) {
				if (loop) {
					playhead = std::fmod(playhead, double(totalFrames));
				}
				else {
					playhead = double(totalFrames - 1u);
					params[PLAY_PARAM].setValue(0.f);
				}
			}
		}
		else {
			buffering = true;
			const float decay = std::max(0.f, 1.f - args.sampleTime * 300.f);
			outputLeft *= decay;
			outputRight *= decay;
		}
	}
	else if (!playing) {
		const float decay = std::max(0.f, 1.f - args.sampleTime * 300.f);
		outputLeft *= decay;
		outputRight *= decay;
	}

	lastLeft = outputLeft;
	lastRight = outputRight;
	outputs[LEFT_OUTPUT].setChannels(1);
	outputs[RIGHT_OUTPUT].setChannels(1);
	outputs[LEFT_OUTPUT].setVoltage(outputLeft);
	outputs[RIGHT_OUTPUT].setVoltage(outputRight);
	lights[PLAY_LIGHT].setBrightness(playing ? 1.f : 0.f);

	if (++publishDivider >= 128u) {
		publishDivider = 0u;
		stream.setDesiredFrame(
			totalFrames > 0u
				? std::min(std::uint64_t(playhead), totalFrames - 1u)
				: 0u,
			loop);
		uiProgress.store(
			totalFrames > 1u
				? float(playhead / double(totalFrames - 1u))
				: 0.f,
			std::memory_order_relaxed);
		uiBuffering.store(buffering, std::memory_order_relaxed);
	}
}

void Longplayer::onReset() {
	params[PLAY_PARAM].setValue(0.f);
	params[LOOP_PARAM].setValue(0.f);
	params[RATE_PARAM].setValue(0.5f);
	playhead = 0.0;
	lastLeft = 0.f;
	lastRight = 0.f;
	wasPlaying = false;
	transitionSamples = 0;
	seekNormalized(0.f);
}

void Longplayer::onSampleRateChange(const SampleRateChangeEvent& event) {
	transitionLength = std::max(1, int(event.sampleRate * 0.005f));
}

json_t* Longplayer::dataToJson() {
	json_t* root = json_object();
	const std::string path = stream.path();
	if (!path.empty()) {
		json_object_set_new(root, "path", json_string(path.c_str()));
	}
	return root;
}

void Longplayer::dataFromJson(json_t* root) {
	if (!root) return;
	json_t* pathValue = json_object_get(root, "path");
	if (json_is_string(pathValue)) {
		const char* path = json_string_value(pathValue);
		if (path && path[0] != '\0') {
			stream.requestLoad(path);
		}
	}
}

bool Longplayer::loadFile(const std::string& path, std::string* error) {
	const std::string extension = lowercaseExtension(path);
	if (extension != ".wav" && extension != ".wave"
		&& extension != ".flac" && extension != ".mp3") {
		if (error) *error = "Choose a WAV, FLAC, or MP3 file";
		return false;
	}
	if (!system::isFile(path)) {
		if (error) *error = "The selected file does not exist";
		return false;
	}
	params[PLAY_PARAM].setValue(0.f);
	stream.requestLoad(path);
	return true;
}

void Longplayer::clearFile() {
	params[PLAY_PARAM].setValue(0.f);
	stream.clear();
}

void Longplayer::seekNormalized(float normalized) {
	requestedSeek.store(clamp(normalized, 0.f, 1.f), std::memory_order_relaxed);
	requestedSeekRevision.fetch_add(1u, std::memory_order_release);
}

float Longplayer::progress() const {
	return clamp(uiProgress.load(std::memory_order_relaxed), 0.f, 1.f);
}

double Longplayer::durationSeconds() const {
	const std::uint32_t rate = stream.sampleRate();
	return rate > 0u ? double(stream.totalFrames()) / rate : 0.0;
}

double Longplayer::playheadSeconds() const {
	return durationSeconds() * progress();
}

bool Longplayer::isLoading() const { return stream.loading(); }
bool Longplayer::isBuffering() const { return uiBuffering.load(std::memory_order_relaxed); }
bool Longplayer::hasFile() const { return stream.ready(); }
std::string Longplayer::filePath() const { return stream.path(); }
std::string Longplayer::displayName() const { return stream.displayName(); }
std::string Longplayer::loadError() const { return stream.error(); }
