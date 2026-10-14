#include "HydraEngine.hpp"

#include "beads/dsp/processor.h"
#include "beads/dsp/synth.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace hydra {

namespace {

constexpr int kBlockRate = 4000;       // control/process blocks per second
constexpr int kMaxBlock = 12;          // 48 kHz / 4000
constexpr int kTransitionBlocks = kBlockRate / 4;
constexpr int kFifoSize = 128;         // media-rate output frames (power of 2)
constexpr float kTwoPi = 6.28318530718f;

inline float clampf(float v, float lo, float hi) {
	return v < lo ? lo : (v > hi ? hi : v);
}

// Attenuverter pot law (centre detent with a dead zone).
inline float attenuverterLaw(float raw) {
	const float v = raw - 0.5f;
	return (2.0f / 0.47f) * v * std::max(std::fabs(v) - 0.03f, 0.0f);
}

// Pitch pot law: virtual notches at selected semitone intervals.
inline float detuneLaw(float raw) {
	static const float intervals[8] = {0.f, 1.f, 3.f, 4.f, 7.f, 12.f, 24.f, 24.f};
	float detune = 2.0f * raw - 1.0f;
	const float sign = detune < 0.0f ? -1.0f : 1.0f;
	detune = clampf(detune * sign * 6.0f, 0.0f, 5.999f);
	const int integral = int(detune);
	float fractional = detune - float(integral);
	fractional = std::max((fractional - 0.33333333f) * 1.5f, 0.0f);
	const float t = fractional * fractional * (3.0f - 2.0f * fractional);
	const float a = intervals[integral];
	const float b = intervals[integral + 1];
	return (a + (b - a) * t) * sign;
}

} // namespace

struct Engine::Impl {
	std::vector<uint8_t> delayBuffer1 = std::vector<uint8_t>(512 * 1024);
	std::vector<uint8_t> delayBuffer2 = std::vector<uint8_t>(256 * 1024);
	std::vector<uint16_t> reverbBuffer = std::vector<uint16_t>(32768);
	std::unique_ptr<beads::Processor> processor;
	std::unique_ptr<beads::Synth> synth;

	Controls controls;
	float hostRate = 48000.f;

	// Active configuration.
	int activeQuality = -1;
	bool activeMono = false;
	bool activeSynth = false;
	float mediaRate = 48000.f;
	int blockSize = 12;
	float ratio = 1.f; // media samples per host sample

	// Input resampler (host -> media).
	float inPhase = 0.f;
	float prevIn[2] = {0.f, 0.f};
	float inLp[2][2] = {{0.f, 0.f}, {0.f, 0.f}};
	float lpCoeff = 1.f;
	bool lpActive = false;

	// Block accumulation.
	beads::FloatFrame inFrames[kMaxBlock];
	beads::FloatFrame outFrames[kMaxBlock];
	bool freezeFlags[kMaxBlock];
	stmlib::GateFlags seedFlags[kMaxBlock];
	stmlib::GateFlags previousSeed = stmlib::GATE_FLAG_LOW;
	int blockFill = 0;
	bool lastFreeze = false;
	bool lastSeedGate = false;
	int muteBlocks = 0;

	// Output FIFO (media rate) and reader (media -> host).
	float fifo[kFifoSize][2];
	uint32_t fifoWrite = 0;
	uint32_t fifoRead = 0;
	float outFrac = 0.f;
	float outLp[2][2] = {{0.f, 0.f}, {0.f, 0.f}};

	Impl()
		: processor(new beads::Processor())
		, synth(new beads::Synth()) {
		uint8_t* buffers[3] = {
			delayBuffer1.data(),
			delayBuffer1.data() + 256 * 1024,
			delayBuffer2.data()
		};
		processor->Init(buffers, reverbBuffer.data());
		synth->Init(reverbBuffer.data());
		for (auto& f : fifo) {
			f[0] = f[1] = 0.f;
		}
	}

	void resetStreams() {
		inPhase = 0.f;
		prevIn[0] = prevIn[1] = 0.f;
		for (int c = 0; c < 2; ++c) {
			inLp[c][0] = inLp[c][1] = 0.f;
			outLp[c][0] = outLp[c][1] = 0.f;
		}
		blockFill = 0;
		previousSeed = stmlib::GATE_FLAG_LOW;
		outFrac = 0.f;
		fifoRead = 0;
		fifoWrite = 0;
		for (auto& f : fifo) {
			f[0] = f[1] = 0.f;
		}
		// One block of latency headroom so the reader never starves.
		fifoWrite = uint32_t(2 * kMaxBlock);
	}

	void applyRates() {
		mediaRate = activeSynth ? 48000.f : processor->sample_rate();
		blockSize = int(mediaRate / float(kBlockRate) + 0.5f);
		blockSize = std::max(1, std::min(blockSize, kMaxBlock));
		ratio = mediaRate / hostRate;
		lpActive = ratio < 1.f;
		const float fc = 0.4f * mediaRate;
		lpCoeff = 1.f - std::exp(-kTwoPi * fc / hostRate);
	}

	void configure(int quality, bool mono, bool synthMode) {
		activeQuality = quality;
		activeMono = mono;
		activeSynth = synthMode;
		// Hardware media index: 0 dirty tape .. 3 bright digital.
		const int media = 3 - quality;
		if (synthMode) {
			synth->Init(reverbBuffer.data());
		} else {
			processor->Configure(beads::Processor::Media(media), mono);
		}
		applyRates();
		resetStreams();
		muteBlocks = kTransitionBlocks;
	}

	void buildParameters(beads::Parameters* p, beads::ModulatedParameter* d,
	                     beads::Processor::MixParameters* m) const {
		const Controls& c = controls;
		p->time.base = c.time;
		p->time.attenuverter = attenuverterLaw(c.timeAmount);
		p->time.has_cv = c.timeCvPatched;
		p->time.cv = c.timeCvPatched ? clampf(c.timeCv * 0.2f, -1.6f, 1.6f) : 0.f;

		p->size.base = c.size;
		p->size.attenuverter = attenuverterLaw(c.sizeAmount);
		p->size.has_cv = c.sizeCvPatched;
		p->size.cv = c.sizeCvPatched ? clampf(c.sizeCv * 0.2f, -1.6f, 1.6f) : 0.f;

		p->shape.base = c.shape;
		p->shape.attenuverter = attenuverterLaw(c.shapeAmount);
		p->shape.has_cv = c.shapeCvPatched;
		p->shape.cv = c.shapeCvPatched ? clampf(c.shapeCv * 0.2f, -1.6f, 1.6f) : 0.f;

		// Pitch works in semitones: 1 V/oct CV, +-24 st range from the pot.
		p->pitch.base = detuneLaw(c.pitch);
		p->pitch.attenuverter = attenuverterLaw(c.pitchAmount);
		p->pitch.has_cv = c.pitchCvPatched;
		p->pitch.cv = c.pitchCvPatched ? clampf(c.pitchCv * 12.f, -60.f, 60.f) : 0.f;
		p->monophonic_mode = false;

		d->base = c.density;
		d->cv = c.densityCvPatched ? clampf(c.densityCv * 0.2f, -1.f, 1.f) : 0.f;

		const float assign = clampf(c.assignCv * 0.2f, -1.f, 1.f) - 0.002f;
		const float base[3] = {c.feedback, c.dryWet, c.reverb};
		float mix[3];
		for (int i = 0; i < 3; ++i) {
			mix[i] = clampf(base[i] * 1.005f + assign * c.assignAmount[i], 0.f, 1.f);
		}
		m->feedback = mix[0];
		m->dry_wet = mix[1];
		m->reverb_amount = mix[2];
		m->output_gate = c.gateOutput;
	}

	void pushOutputBlock(const beads::FloatFrame* frames, int count) {
		for (int i = 0; i < count; ++i) {
			const uint32_t w = fifoWrite & (kFifoSize - 1);
			fifo[w][0] = clampf(frames[i].l, -1.f, 0.999999f);
			fifo[w][1] = clampf(frames[i].r, -1.f, 0.999999f);
			++fifoWrite;
		}
	}

	void runBlock() {
		const Controls& c = controls;
		const bool wantSynth = c.synthMode;
		const bool freezeNow = lastFreeze;
		if (activeQuality < 0 ||
		    (!freezeNow && (c.quality != activeQuality || c.mono != activeMono || wantSynth != activeSynth))) {
			configure(c.quality, c.mono, wantSynth);
		}

		if (muteBlocks > 0) {
			--muteBlocks;
			for (int i = 0; i < blockSize; ++i) {
				outFrames[i].l = outFrames[i].r = 0.f;
			}
			pushOutputBlock(outFrames, blockSize);
			return;
		}

		beads::Parameters p;
		beads::ModulatedParameter d;
		beads::Processor::MixParameters m;
		buildParameters(&p, &d, &m);

		beads::SeedButtonState seedButton = beads::SEED_BUTTON_STATE_OFF;
		if (c.seedState == SEED_LATCHED) {
			seedButton = beads::SEED_BUTTON_STATE_LOCKED;
		} else if (c.seedState == SEED_ON) {
			seedButton = beads::SEED_BUTTON_STATE_ON;
		}
		const stmlib::GateFlags* seedInput = c.seedInputPatched ? seedFlags : NULL;

		if (activeSynth) {
			synth->Render(seedButton, seedInput, freezeFlags,
			              beads::Synth::Resolution(3 - activeQuality),
			              d, p, m, outFrames, size_t(blockSize));
		} else {
			processor->Process(seedButton, seedInput, freezeFlags, d, p, m,
			                   inFrames, outFrames, size_t(blockSize));
		}
		pushOutputBlock(outFrames, blockSize);
	}

	inline void pushMediaSample(float l, float r, bool freeze, bool seedGate) {
		const int i = blockFill;
		if (activeMono || controls.mono) {
			r = l;
		}
		inFrames[i].l = l;
		inFrames[i].r = r;
		freezeFlags[i] = freeze;
		previousSeed = stmlib::ExtractGateFlags(previousSeed, seedGate);
		seedFlags[i] = previousSeed;
		if (++blockFill >= blockSize) {
			blockFill = 0;
			runBlock();
		}
	}

	void process(float inL, float inR, bool freeze, bool seedGate, float* outL, float* outR) {
		lastFreeze = freeze;
		lastSeedGate = seedGate;

		if (activeQuality < 0) {
			// First call: configure lazily so the host rate is known.
			configure(controls.quality, controls.mono, controls.synthMode);
		}

		float x[2] = {clampf(inL, -2.f, 2.f), clampf(inR, -2.f, 2.f)};
		if (lpActive) {
			for (int c = 0; c < 2; ++c) {
				inLp[c][0] += lpCoeff * (x[c] - inLp[c][0]);
				inLp[c][1] += lpCoeff * (inLp[c][0] - inLp[c][1]);
				x[c] = inLp[c][1];
			}
		}

		// Linear-interpolated host -> media conversion.
		const float start = inPhase;
		const float end = start + ratio;
		float t = 1.f;
		while (t <= end) {
			const float frac = (t - start) / ratio;
			const float l = prevIn[0] + (x[0] - prevIn[0]) * frac;
			const float r = prevIn[1] + (x[1] - prevIn[1]) * frac;
			pushMediaSample(l, r, freeze, seedGate);
			t += 1.f;
		}
		inPhase = end - std::floor(end);
		prevIn[0] = x[0];
		prevIn[1] = x[1];

		// Linear-interpolated media -> host conversion.
		float y[2] = {0.f, 0.f};
		if (fifoWrite - fifoRead >= 2) {
			const float* a = fifo[fifoRead & (kFifoSize - 1)];
			const float* b = fifo[(fifoRead + 1) & (kFifoSize - 1)];
			y[0] = a[0] + (b[0] - a[0]) * outFrac;
			y[1] = a[1] + (b[1] - a[1]) * outFrac;
			outFrac += ratio;
			while (outFrac >= 1.f) {
				outFrac -= 1.f;
				++fifoRead;
			}
		}
		if (lpActive) {
			for (int c = 0; c < 2; ++c) {
				outLp[c][0] += lpCoeff * (y[c] - outLp[c][0]);
				outLp[c][1] += lpCoeff * (outLp[c][0] - outLp[c][1]);
				y[c] = outLp[c][1];
			}
		}
		*outL = y[0];
		*outR = y[1];
	}
};

Engine::Engine() : impl(new Impl()) {}
Engine::~Engine() {}

void Engine::init(float hostSampleRate) {
	impl->hostRate = hostSampleRate;
	impl->activeQuality = -1;
}

void Engine::setHostSampleRate(float hostSampleRate) {
	if (hostSampleRate != impl->hostRate) {
		impl->hostRate = hostSampleRate;
		impl->activeQuality = -1; // reconfigure on next sample
	}
}

void Engine::setControls(const Controls& controls) {
	impl->controls = controls;
}

void Engine::process(float inL, float inR, bool freeze, bool seedGate, float* outL, float* outR) {
	impl->process(inL, inR, freeze, seedGate, outL, outR);
}

float Engine::freezeReserve() const {
	return impl->activeSynth ? 1.f : impl->processor->overwrite_reserve();
}

float Engine::mediaSampleRate() const {
	return impl->mediaRate;
}

bool Engine::inSynthMode() const {
	return impl->activeSynth;
}

} // namespace hydra
