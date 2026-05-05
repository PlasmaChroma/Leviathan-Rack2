#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include "SilMicropeak.hpp"
#include <vector>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <condition_variable>
#include <mutex>
#include <thread>

struct Sil : Module {
	struct Biquad {
		float b0 = 1.f;
		float b1 = 0.f;
		float b2 = 0.f;
		float a1 = 0.f;
		float a2 = 0.f;
		float z1 = 0.f;
		float z2 = 0.f;

		float process(float x) {
			const float y = b0 * x + z1;
			z1 = b1 * x - a1 * y + z2;
			z2 = b2 * x - a2 * y;
			return y;
		}

		void reset() {
			z1 = 0.f;
			z2 = 0.f;
		}

		void setPeaking(float sampleRate, float centerHz, float q, float gainDb) {
			if (sampleRate <= 1.f || centerHz <= 1.f || q <= 1e-4f) {
				b0 = 1.f;
				b1 = b2 = a1 = a2 = 0.f;
				return;
			}
			const float nyquistGuard = 0.48f * sampleRate;
			const float fc = clamp(centerHz, 10.f, nyquistGuard);
			const float A = std::pow(10.f, gainDb / 40.f);
			const float w0 = 2.f * M_PI * fc / sampleRate;
			const float c = std::cos(w0);
			const float s = std::sin(w0);
			const float alpha = s / (2.f * q);

			const float rawB0 = 1.f + alpha * A;
			const float rawB1 = -2.f * c;
			const float rawB2 = 1.f - alpha * A;
			const float rawA0 = 1.f + alpha / A;
			const float rawA1 = -2.f * c;
			const float rawA2 = 1.f - alpha / A;
			const float invA0 = (std::fabs(rawA0) > 1e-9f) ? (1.f / rawA0) : 1.f;

			b0 = rawB0 * invA0;
			b1 = rawB1 * invA0;
			b2 = rawB2 * invA0;
			a1 = rawA1 * invA0;
			a2 = rawA2 * invA0;
		}
	};

	enum ParamId {
		MASTERING_ENABLED_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_L_INPUT,
		INPUT_R_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_L_OUTPUT,
		OUTPUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIMITER_ACTIVE_LIGHT,
		LOW_RECOVERY_LIGHT,
		REMOVE_MUD_LIGHT,
		MICROPEAK_LIGHT,
		MASTERING_ENABLED_LIGHT,
		LIGHTS_LEN
	};

	enum ColorScheme {
		SCHEME_DEFAULT,
		SCHEME_CLASSIC,
		SCHEME_MONOCHROME,
		SCHEME_FIRE,
		SCHEME_LEN
	};

	ColorScheme colorScheme = SCHEME_DEFAULT;
	bool masteringEnabled = true;

	static constexpr int HISTOGRAM_BINS = 1000;
	static constexpr float HISTOGRAM_DURATION = 10.f;

	struct HistogramData {
		float minL[HISTOGRAM_BINS] = {};
		float maxL[HISTOGRAM_BINS] = {};
		float minR[HISTOGRAM_BINS] = {};
		float maxR[HISTOGRAM_BINS] = {};
		int writePtr = 0;

		float currentMinL = 1e10f, currentMaxL = -1e10f;
		float currentMinR = 1e10f, currentMaxR = -1e10f;
		int samplesInCurrentBin = 0;
		int samplesPerBin = 441;

		float smoothedPeak = 5.f;
	} hist;

	static constexpr int SPEC_FREQ_BINS = 128;
	static constexpr int FFT_SIZE = 2048;

	struct SpectrumData {
		float magnitudesL[SPEC_FREQ_BINS] = {};
		float magnitudesR[SPEC_FREQ_BINS] = {};
		
		float bufferL[FFT_SIZE] = {};
		float bufferR[FFT_SIZE] = {};
		int writePtr = 0;

		alignas(16) float window[FFT_SIZE];
		alignas(16) float fftInL[FFT_SIZE];
		alignas(16) float fftInR[FFT_SIZE];
		alignas(16) float fftOutL[FFT_SIZE];
		alignas(16) float fftOutR[FFT_SIZE];

		dsp::RealFFT* fft = nullptr;

		float smoothedPeakDb = 0.f;
	} spec;

	dsp::ClockDivider specDivider;
	float limiterGain = 1.f;
	float limiterPrevL = 0.f;
	float limiterPrevR = 0.f;
	bool limiterPrevValid = false;
	sil_micropeak::CleanupFilter micropeakCleanupFilter;
	std::vector<float> rollingBufferL;
	std::vector<float> rollingBufferR;
	int rollingWriteIndex = 0;
	int rollingBufferLength = 0;
	int rollingFilled = 0;
	dsp::RCFilter lowpassL1;
	dsp::RCFilter lowpassL2;
	dsp::RCFilter lowpassR1;
	dsp::RCFilter lowpassR2;
	float lowBandCorrLL = 1e-6f;
	float lowBandCorrRR = 1e-6f;
	float lowBandCorrLR = 0.f;
	float lowBandSideGain = 1.f;
	struct RemoveMudState {
		dsp::RCFilter mudHp;
		dsp::RCFilter mudLp;
		dsp::RCFilter bassHp;
		dsp::RCFilter bassLp;
		dsp::RCFilter presenceHp;
		dsp::RCFilter presenceLp;
		float mudEnv = 1e-6f;
		float bassEnv = 1e-6f;
		float presenceEnv = 1e-6f;
		float targetCutDb = 0.f;
		float smoothedCutDb = 0.f;
		float ledAmount = 0.f;
		dsp::ClockDivider coeffDivider;
		Biquad peakingL;
		Biquad peakingR;
	} removeMud;

	static constexpr float kLowBandCutoffHz = 120.f;
	static constexpr float kLowBandCorrTauSec = 0.100f;
	static constexpr float kLowBandSideAttackSec = 0.050f;
	static constexpr float kLowBandSideReleaseSec = 0.250f;
	static constexpr int kLimiterOversampleFactor = 4;
	static constexpr float kAudioFullScaleV = 5.f;
	static constexpr float kRollingBufferSeconds = 10.f;
	static constexpr float kMicropeakHoldSeconds = 2.f;
	static constexpr float kMicropeakStrongTopUpSeconds = 0.70f;
	static constexpr float kMicropeakWeakTopUpSeconds = 0.35f;
	static constexpr float kMicropeakOnHoldFloorSeconds = 1.10f;
	static constexpr float kMicropeakKeepHoldFloorSeconds = 0.55f;
	static constexpr float kMicropeakOnSeverity = 0.55f;
	static constexpr float kMicropeakKeepSeverity = 0.35f;
	static constexpr float kMicropeakPromoteSeverityEma = 0.36f;
	static constexpr float kMicropeakKeepSeverityEma = 0.28f;
	static constexpr int kMicropeakPromoteWeakStreak = 3;
	static constexpr float kMicropeakScoreWindowSeconds = 1.0f;
	static constexpr int kMicropeakScoreOnThreshold = 7;
	static constexpr int kMicropeakScoreKeepThreshold = 4;
	static constexpr int kMicropeakOnEvents = 2;
	static constexpr int kMicropeakKeepEvents = 1;
	static constexpr int kMicropeakChunkSize = 2048;
	static constexpr float kMicropeakConfidenceAttackSeconds = 0.20f;
	static constexpr float kMicropeakConfidenceReleaseSeconds = 2.40f;
	static constexpr float kMudLowHz = 180.f;
	static constexpr float kMudHighHz = 520.f;
	static constexpr float kMudCenterHz = 315.f;
	static constexpr float kMudQ = 0.75f;
	static constexpr float kMudAllowedWarmthDb = 1.5f;
	static constexpr float kMudThresholdDb = 2.0f;
	static constexpr float kMudKneeDb = 4.0f;
	static constexpr float kMudMaxCutDb = 2.5f;
	static constexpr float kMudAttackSec = 0.120f;
	static constexpr float kMudReleaseSec = 0.850f;
	static constexpr float kMudEnvAttackSec = 0.030f;
	static constexpr float kMudEnvReleaseSec = 0.220f;

	static float toDbSafe(float v) {
		return 20.f * std::log10(std::max(v, 1e-7f));
	}

	static float softKnee01(float xDb, float thresholdDb, float kneeDb) {
		const float halfKnee = 0.5f * std::max(0.f, kneeDb);
		if (xDb <= thresholdDb - halfKnee) {
			return 0.f;
		}
		if (xDb >= thresholdDb + halfKnee) {
			return 1.f;
		}
		const float t = (xDb - (thresholdDb - halfKnee)) / std::max(kneeDb, 1e-6f);
		return t * t * (3.f - 2.f * t);
	}

	struct MicropeakWorkerState {
		std::array<float, kMicropeakChunkSize> fillL {};
		std::array<float, kMicropeakChunkSize> fillR {};
		int fillPos = 0;
		std::array<float, kMicropeakChunkSize> pendingL {};
		std::array<float, kMicropeakChunkSize> pendingR {};
		float pendingFullScaleVolts = kAudioFullScaleV;
		float pendingSampleRate = 44100.f;
		bool pending = false;
		bool stop = false;
		std::mutex mutex;
		std::condition_variable cv;
		std::thread thread;
		std::atomic<int> holdSamples {0};
		std::atomic<int> lastEventCount {0};
		std::atomic<float> lastSeverity {0.f};
		std::atomic<float> lastSeverityEma {0.f};
		std::atomic<float> detectionConfidence {0.f};
		int weakStreak = 0;
		std::atomic<bool> latchedActive {false};
	} micropeak;

	void configureRollingBuffer(float sampleRate) {
		const int requestedLength = std::max(1, int(std::round(sampleRate * kRollingBufferSeconds)));
		if (requestedLength == rollingBufferLength) {
			return;
		}
		rollingBufferLength = requestedLength;
		rollingBufferL.assign(size_t(rollingBufferLength), 0.f);
		rollingBufferR.assign(size_t(rollingBufferLength), 0.f);
		rollingWriteIndex = 0;
		rollingFilled = 0;
	}

	void pushRollingSample(float sampleL, float sampleR) {
		if (rollingBufferLength <= 0) {
			return;
		}
		rollingBufferL[size_t(rollingWriteIndex)] = sampleL;
		rollingBufferR[size_t(rollingWriteIndex)] = sampleR;
		rollingWriteIndex++;
		if (rollingWriteIndex >= rollingBufferLength) {
			rollingWriteIndex = 0;
		}
		if (rollingFilled < rollingBufferLength) {
			rollingFilled++;
		}
	}

	void startMicropeakWorker() {
		if (micropeak.thread.joinable()) {
			return;
		}
		micropeak.stop = false;
		micropeak.thread = std::thread([this]() {
			std::array<float, kMicropeakChunkSize> localL;
			std::array<float, kMicropeakChunkSize> localR;
			std::array<int, 128> scoreHistory {};
			int scoreWrite = 0;
			int scoreCount = 0;
			int scoreSum = 0;
			int scoreWindowSize = 1;
			while (true) {
				float fullScale = kAudioFullScaleV;
				float sampleRate = 44100.f;
				{
					std::unique_lock<std::mutex> lock(micropeak.mutex);
					micropeak.cv.wait(lock, [this]() {
						return micropeak.stop || micropeak.pending;
					});
					if (micropeak.stop) {
						return;
					}
					localL = micropeak.pendingL;
					localR = micropeak.pendingR;
					fullScale = micropeak.pendingFullScaleVolts;
					sampleRate = micropeak.pendingSampleRate;
					micropeak.pending = false;
				}

				const sil_micropeak::StereoResult stereoResult =
					sil_micropeak::analyzeChunkStereo(localL.data(), localR.data(), localL.size(), fullScale);
				const sil_micropeak::Result& leftResult = stereoResult.left;
				const sil_micropeak::Result& rightResult = stereoResult.right;
				micropeak.lastEventCount.store(leftResult.eventCount + rightResult.eventCount, std::memory_order_relaxed);
				const float prevEma = micropeak.lastSeverityEma.load(std::memory_order_relaxed);
				const float strongestSeverity = std::max(leftResult.strongestSeverity, rightResult.strongestSeverity);
				const float severityEma = 0.85f * prevEma + 0.15f * strongestSeverity;
				micropeak.lastSeverity.store(strongestSeverity, std::memory_order_relaxed);
				micropeak.lastSeverityEma.store(severityEma, std::memory_order_relaxed);
				const bool leftDirectOn =
					leftResult.eventCount >= kMicropeakOnEvents || leftResult.strongestSeverity >= kMicropeakOnSeverity;
				const bool rightDirectOn =
					rightResult.eventCount >= kMicropeakOnEvents || rightResult.strongestSeverity >= kMicropeakOnSeverity;
				const bool directOnHit = leftDirectOn || rightDirectOn;
					const bool leftKeep =
						leftResult.eventCount >= kMicropeakKeepEvents || leftResult.strongestSeverity >= kMicropeakKeepSeverity;
					const bool rightKeep =
						rightResult.eventCount >= kMicropeakKeepEvents || rightResult.strongestSeverity >= kMicropeakKeepSeverity;
					const bool keepHit = leftKeep || rightKeep;
					const int requestedWindow = std::max(
						1,
						std::min(
							int(scoreHistory.size()),
							int(std::round((kMicropeakScoreWindowSeconds * sampleRate) / float(kMicropeakChunkSize)))
						)
					);
					if (requestedWindow != scoreWindowSize) {
						scoreWindowSize = requestedWindow;
						scoreWrite = 0;
						scoreCount = 0;
						scoreSum = 0;
						scoreHistory.fill(0);
					}
					int chunkScore = 0;
					if (directOnHit) {
						chunkScore += 2;
					}
					else if (keepHit) {
						chunkScore += 1;
					}
					if (strongestSeverity >= kMicropeakOnSeverity) {
						chunkScore += 1;
					}
					if (scoreCount == scoreWindowSize) {
						scoreSum -= scoreHistory[size_t(scoreWrite)];
					}
					else {
						scoreCount++;
					}
					scoreHistory[size_t(scoreWrite)] = chunkScore;
					scoreSum += chunkScore;
					scoreWrite++;
					if (scoreWrite >= scoreWindowSize) {
						scoreWrite = 0;
					}
				const bool windowOnHit = scoreSum >= kMicropeakScoreOnThreshold;
				const bool windowKeepHit = scoreSum >= kMicropeakScoreKeepThreshold;
				if (keepHit) {
					micropeak.weakStreak = micropeak.weakStreak + 1;
				}
				else {
					micropeak.weakStreak = std::max(0, micropeak.weakStreak - 1);
				}
				const bool promotedOnHit = micropeak.weakStreak >= kMicropeakPromoteWeakStreak &&
					severityEma >= kMicropeakPromoteSeverityEma;
				const bool onHit = directOnHit || promotedOnHit || windowOnHit;
				const bool keepHitEma = keepHit || severityEma >= kMicropeakKeepSeverityEma || windowKeepHit;
				const float directProgress = std::max(
					clamp(float(std::max(leftResult.eventCount, rightResult.eventCount)) / float(kMicropeakOnEvents), 0.f, 1.f),
					clamp(strongestSeverity / kMicropeakOnSeverity, 0.f, 1.f)
				);
				const float promotedProgress = std::min(
					clamp(float(micropeak.weakStreak) / float(std::max(1, kMicropeakPromoteWeakStreak)), 0.f, 1.f),
					clamp(severityEma / kMicropeakPromoteSeverityEma, 0.f, 1.f)
				);
				const float windowProgress = clamp(float(scoreSum) / float(std::max(1, kMicropeakScoreOnThreshold)), 0.f, 1.f);
				bool latched = micropeak.latchedActive.load(std::memory_order_relaxed);
				const int maxHold = std::max(1, int(std::round(sampleRate * kMicropeakHoldSeconds)));
				int hold = std::max(0, micropeak.holdSamples.load(std::memory_order_relaxed));

				if (onHit) {
					latched = true;
					const float strongTopUp = directOnHit ? kMicropeakStrongTopUpSeconds : 0.30f;
					const int add = std::max(1, int(std::round(sampleRate * strongTopUp)));
					const int holdFloor = std::max(1, int(std::round(sampleRate * kMicropeakOnHoldFloorSeconds)));
					hold = std::max(hold, holdFloor);
					hold = std::min(maxHold, hold + add);
					micropeak.holdSamples.store(hold, std::memory_order_relaxed);
				}
				else if (latched && keepHitEma) {
					const int add = std::max(1, int(std::round(sampleRate * kMicropeakWeakTopUpSeconds)));
					const int holdFloor = std::max(1, int(std::round(sampleRate * kMicropeakKeepHoldFloorSeconds)));
					hold = std::max(hold, holdFloor);
					hold = std::min(maxHold, hold + add);
					micropeak.holdSamples.store(hold, std::memory_order_relaxed);
				}
				else if (latched && !keepHitEma && hold <= 0) {
					latched = false;
				}

				micropeak.latchedActive.store(latched, std::memory_order_relaxed);
				const float targetConfidence = latched ? 1.f : std::max(directProgress, std::max(promotedProgress, windowProgress));
				const float previousConfidence = micropeak.detectionConfidence.load(std::memory_order_relaxed);
				const float chunkSeconds = float(kMicropeakChunkSize) / std::max(sampleRate, 1.f);
				const float attackCoeff = std::exp(-chunkSeconds / std::max(1e-3f, kMicropeakConfidenceAttackSeconds));
				const float releaseCoeff = std::exp(-chunkSeconds / std::max(1e-3f, kMicropeakConfidenceReleaseSeconds));
				const float coeff = (targetConfidence >= previousConfidence) ? attackCoeff : releaseCoeff;
				const float confidence = targetConfidence + coeff * (previousConfidence - targetConfidence);
				micropeak.detectionConfidence.store(clamp(confidence, 0.f, 1.f), std::memory_order_relaxed);
			}
		});
	}

	void stopMicropeakWorker() {
		{
			std::lock_guard<std::mutex> lock(micropeak.mutex);
			micropeak.stop = true;
			micropeak.pending = false;
			micropeak.weakStreak = 0;
		}
		micropeak.cv.notify_all();
		if (micropeak.thread.joinable()) {
			micropeak.thread.join();
		}
	}

	void pushMicropeakSample(float sampleL, float sampleR, float sampleRate) {
		micropeak.fillL[size_t(micropeak.fillPos)] = sampleL;
		micropeak.fillR[size_t(micropeak.fillPos)] = sampleR;
		micropeak.fillPos++;
		if (micropeak.fillPos < kMicropeakChunkSize) {
			return;
		}
		micropeak.fillPos = 0;
		{
			std::unique_lock<std::mutex> lock(micropeak.mutex, std::try_to_lock);
			if (!lock.owns_lock()) {
				return;
			}
			if (!micropeak.pending) {
				micropeak.pendingL = micropeak.fillL;
				micropeak.pendingR = micropeak.fillR;
				micropeak.pendingFullScaleVolts = kAudioFullScaleV;
				micropeak.pendingSampleRate = std::max(sampleRate, 1.f);
				micropeak.pending = true;
			}
		}
		micropeak.cv.notify_one();
	}

	bool consumeMicropeakHoldSample() {
		int remaining = micropeak.holdSamples.load(std::memory_order_relaxed);
		while (remaining > 0) {
			if (micropeak.holdSamples.compare_exchange_weak(remaining, remaining - 1, std::memory_order_relaxed)) {
				return true;
			}
		}
		return false;
	}

	void updateLowBandCutoff(float sampleRate) {
		const float cutoffNorm = clamp(kLowBandCutoffHz / sampleRate, 1e-5f, 0.49f);
		lowpassL1.setCutoff(cutoffNorm);
		lowpassL2.setCutoff(cutoffNorm);
		lowpassR1.setCutoff(cutoffNorm);
		lowpassR2.setCutoff(cutoffNorm);
	}

	void updateRemoveMudCutoffs(float sampleRate) {
		const auto norm = [&](float hz) {
			return clamp(hz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
		};
		removeMud.mudHp.setCutoff(norm(kMudLowHz));
		removeMud.mudLp.setCutoff(norm(kMudHighHz));
		removeMud.bassHp.setCutoff(norm(80.f));
		removeMud.bassLp.setCutoff(norm(160.f));
		removeMud.presenceHp.setCutoff(norm(700.f));
		removeMud.presenceLp.setCutoff(norm(3000.f));
	}

	void resetRemoveMudState() {
		removeMud.mudEnv = 1e-6f;
		removeMud.bassEnv = 1e-6f;
		removeMud.presenceEnv = 1e-6f;
		removeMud.targetCutDb = 0.f;
		removeMud.smoothedCutDb = 0.f;
		removeMud.ledAmount = 0.f;
		removeMud.peakingL.reset();
		removeMud.peakingR.reset();
	}

	Sil() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(MASTERING_ENABLED_PARAM, 0.f, 1.f, 1.f, "Mastering", {"Disabled", "Enabled"});
		configInput(INPUT_L_INPUT, "Left");
		configInput(INPUT_R_INPUT, "Right");
		configOutput(OUTPUT_L_OUTPUT, "Left");
		configOutput(OUTPUT_R_OUTPUT, "Right");

		hist.samplesPerBin = (int)(APP->engine->getSampleRate() * HISTOGRAM_DURATION / HISTOGRAM_BINS);
		
		spec.fft = new dsp::RealFFT(FFT_SIZE);
		for (int i = 0; i < FFT_SIZE; i++) {
			spec.window[i] = 0.5f - 0.5f * std::cos(2.f * M_PI * i / (FFT_SIZE - 1));
		}

		specDivider.setDivision(2048);
		removeMud.coeffDivider.setDivision(32);
		updateLowBandCutoff(APP->engine->getSampleRate());
		updateRemoveMudCutoffs(APP->engine->getSampleRate());
		configureRollingBuffer(APP->engine->getSampleRate());
		removeMud.peakingL.setPeaking(APP->engine->getSampleRate(), kMudCenterHz, kMudQ, 0.f);
		removeMud.peakingR.setPeaking(APP->engine->getSampleRate(), kMudCenterHz, kMudQ, 0.f);
		startMicropeakWorker();
	}

	~Sil() {
		stopMicropeakWorker();
		delete spec.fft;
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		hist.samplesPerBin = (int)(e.sampleRate * HISTOGRAM_DURATION / HISTOGRAM_BINS);
		if (hist.samplesPerBin < 1) hist.samplesPerBin = 1;
		updateLowBandCutoff(e.sampleRate);
		updateRemoveMudCutoffs(e.sampleRate);
		configureRollingBuffer(e.sampleRate);
		micropeakCleanupFilter.reset();
		resetRemoveMudState();
		micropeak.weakStreak = 0;
		micropeak.lastSeverityEma.store(0.f, std::memory_order_relaxed);
		micropeak.detectionConfidence.store(0.f, std::memory_order_relaxed);
	}

	void process(const ProcessArgs& args) override {
		masteringEnabled = params[MASTERING_ENABLED_PARAM].getValue() > 0.5f;

		const float inL = inputs[INPUT_L_INPUT].getVoltage();
		const float inR = inputs[INPUT_R_INPUT].getVoltage();

		// Low-band mono recovery below 120 Hz:
		// preserve coherent bass stereo, progressively collapse risky low side content.
		lowpassL1.process(inL);
		float lowL = lowpassL1.lowpass();
		lowpassL2.process(lowL);
		lowL = lowpassL2.lowpass();

		lowpassR1.process(inR);
		float lowR = lowpassR1.lowpass();
		lowpassR2.process(lowR);
		lowR = lowpassR2.lowpass();

		const float highL = inL - lowL;
		const float highR = inR - lowR;
		const float lowMid = 0.5f * (lowL + lowR);
		const float lowSide = 0.5f * (lowL - lowR);

		const float corrCoeff = std::exp(-1.f / (kLowBandCorrTauSec * args.sampleRate));
		const float corrMix = 1.f - corrCoeff;
		lowBandCorrLL = corrCoeff * lowBandCorrLL + corrMix * (lowL * lowL);
		lowBandCorrRR = corrCoeff * lowBandCorrRR + corrMix * (lowR * lowR);
		lowBandCorrLR = corrCoeff * lowBandCorrLR + corrMix * (lowL * lowR);
		const float denom = std::sqrt(std::max(lowBandCorrLL * lowBandCorrRR, 1e-12f));
		const float lowCorr = clamp(lowBandCorrLR / denom, -1.f, 1.f);
		const float targetLowSideGain = (lowCorr >= 0.70f) ? 1.f : ((lowCorr <= 0.f) ? 0.f : (lowCorr / 0.70f));
		const float sideAttackCoeff = std::exp(-1.f / (kLowBandSideAttackSec * args.sampleRate));
		const float sideReleaseCoeff = std::exp(-1.f / (kLowBandSideReleaseSec * args.sampleRate));
		const float sideCoeff = (targetLowSideGain < lowBandSideGain) ? sideAttackCoeff : sideReleaseCoeff;
		lowBandSideGain = targetLowSideGain + sideCoeff * (lowBandSideGain - targetLowSideGain);
		const float lowRecoveryAmount = clamp(1.f - lowBandSideGain, 0.f, 1.f);

		const float recoveredLowL = lowMid + lowSide * lowBandSideGain;
		const float recoveredLowR = lowMid - lowSide * lowBandSideGain;
		const float recoveredL = highL + recoveredLowL;
		const float recoveredR = highR + recoveredLowR;
		float removeMudLed = 0.f;
		float mudCleanL = recoveredL;
		float mudCleanR = recoveredR;
		if (masteringEnabled) {
			const float mono = 0.5f * (recoveredL + recoveredR);
			removeMud.mudHp.process(mono);
			const float mudHigh = removeMud.mudHp.highpass();
			removeMud.mudLp.process(mudHigh);
			const float mudBand = removeMud.mudLp.lowpass();
			removeMud.bassHp.process(mono);
			const float bassHigh = removeMud.bassHp.highpass();
			removeMud.bassLp.process(bassHigh);
			const float bassBand = removeMud.bassLp.lowpass();
			removeMud.presenceHp.process(mono);
			const float presenceHigh = removeMud.presenceHp.highpass();
			removeMud.presenceLp.process(presenceHigh);
			const float presenceBand = removeMud.presenceLp.lowpass();

			const float envAttack = std::exp(-1.f / (kMudEnvAttackSec * args.sampleRate));
			const float envRelease = std::exp(-1.f / (kMudEnvReleaseSec * args.sampleRate));
			auto updateEnv = [&](float& env, float x) {
				const float absX = std::fabs(x);
				const float c = (absX > env) ? envAttack : envRelease;
				env = absX + c * (env - absX);
			};
			updateEnv(removeMud.mudEnv, mudBand);
			updateEnv(removeMud.bassEnv, bassBand);
			updateEnv(removeMud.presenceEnv, presenceBand);

			const float refEnv = 0.5f * removeMud.bassEnv + 0.5f * removeMud.presenceEnv;
			const float mudDeltaDb = toDbSafe(removeMud.mudEnv) - toDbSafe(refEnv) - kMudAllowedWarmthDb;
			const float activation = softKnee01(mudDeltaDb, kMudThresholdDb, kMudKneeDb);
			removeMud.targetCutDb = -kMudMaxCutDb * activation;

			const float attackCoeff = std::exp(-1.f / (kMudAttackSec * args.sampleRate));
			const float releaseCoeff = std::exp(-1.f / (kMudReleaseSec * args.sampleRate));
			const float coeff = (removeMud.targetCutDb < removeMud.smoothedCutDb) ? attackCoeff : releaseCoeff;
			removeMud.smoothedCutDb = removeMud.targetCutDb + coeff * (removeMud.smoothedCutDb - removeMud.targetCutDb);
			removeMud.ledAmount = clamp((-removeMud.smoothedCutDb) / kMudMaxCutDb, 0.f, 1.f);

			if (removeMud.coeffDivider.process()) {
				removeMud.peakingL.setPeaking(args.sampleRate, kMudCenterHz, kMudQ, removeMud.smoothedCutDb);
				removeMud.peakingR.setPeaking(args.sampleRate, kMudCenterHz, kMudQ, removeMud.smoothedCutDb);
			}
			mudCleanL = removeMud.peakingL.process(recoveredL);
			mudCleanR = removeMud.peakingR.process(recoveredR);
			removeMudLed = removeMud.ledAmount;
		}
		else {
			removeMud.targetCutDb = 0.f;
			removeMud.smoothedCutDb = 0.f;
			removeMud.ledAmount = 0.f;
		}

		const float preMasterL = masteringEnabled ? mudCleanL : inL;
		const float preMasterR = masteringEnabled ? mudCleanR : inR;
		const bool micropeakActive = consumeMicropeakHoldSample();
		const sil_micropeak::StereoSample cleaned = micropeakCleanupFilter.process(
			sil_micropeak::StereoSample(preMasterL, preMasterR),
			micropeakActive,
			kAudioFullScaleV
		);

		float outL = cleaned.l;
		float outR = cleaned.r;
		float limiterLed = 0.f;
		if (masteringEnabled) {
			const float limiterCeiling = kAudioFullScaleV * std::pow(10.f, -1.f / 20.f);
			// One-sample lookahead "true-peak as much as possible":
			// cleanup emits the delayed center sample, so the current pre-master
			// sample is available as the next point for limiter detection.
			float peak = std::max(std::fabs(cleaned.l), std::fabs(cleaned.r));
			if (limiterPrevValid) {
				for (int i = 1; i <= kLimiterOversampleFactor; ++i) {
					const float a = float(i) / float(kLimiterOversampleFactor);
					const float interpL = limiterPrevL + (cleaned.l - limiterPrevL) * a;
					const float interpR = limiterPrevR + (cleaned.r - limiterPrevR) * a;
					peak = std::max(peak, std::max(std::fabs(interpL), std::fabs(interpR)));
				}
			}
			for (int i = 1; i <= kLimiterOversampleFactor; ++i) {
				const float a = float(i) / float(kLimiterOversampleFactor);
				const float interpL = cleaned.l + (preMasterL - cleaned.l) * a;
				const float interpR = cleaned.r + (preMasterR - cleaned.r) * a;
				peak = std::max(peak, std::max(std::fabs(interpL), std::fabs(interpR)));
			}
			const float desiredGain = (peak > limiterCeiling && peak > 1e-9f) ? (limiterCeiling / peak) : 1.f;
			const float attackCoeff = std::exp(-1.f / (0.0005f * args.sampleRate));
			const float releaseCoeff = std::exp(-1.f / (0.080f * args.sampleRate));
			const float coeff = (desiredGain < limiterGain) ? attackCoeff : releaseCoeff;
			limiterGain = desiredGain + coeff * (limiterGain - desiredGain);
			outL = cleaned.l * limiterGain;
			outR = cleaned.r * limiterGain;
			limiterPrevL = cleaned.l;
			limiterPrevR = cleaned.r;
			limiterPrevValid = true;
			const float grDb = -20.f * std::log10(std::max(limiterGain, 1e-6f));
			limiterLed = clamp(grDb / 6.f, 0.f, 1.f);
		}
		else {
			limiterGain = 1.f;
			limiterPrevValid = false;
		}

		outputs[OUTPUT_L_OUTPUT].setChannels(1);
		outputs[OUTPUT_R_OUTPUT].setChannels(1);
		outputs[OUTPUT_L_OUTPUT].setVoltage(outL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(outR);
		lights[LIMITER_ACTIVE_LIGHT].setSmoothBrightness(limiterLed, args.sampleTime);
		lights[LOW_RECOVERY_LIGHT].setSmoothBrightness(masteringEnabled ? lowRecoveryAmount : 0.f, args.sampleTime);
		lights[REMOVE_MUD_LIGHT].setSmoothBrightness(masteringEnabled ? removeMudLed : 0.f, args.sampleTime);
		pushMicropeakSample(preMasterL, preMasterR, args.sampleRate);
		pushRollingSample(outL, outR);
		const float candidateSeverity = micropeak.lastSeverity.load(std::memory_order_relaxed);
		const int candidateEvents = micropeak.lastEventCount.load(std::memory_order_relaxed);
		const float candidateLed = clamp(candidateSeverity * 0.6f + float(candidateEvents) * 0.08f, 0.f, 0.35f);
		const float micropeakLed = micropeakActive ? 1.f : candidateLed;
		lights[MICROPEAK_LIGHT].setSmoothBrightness(micropeakLed, args.sampleTime);
		lights[MASTERING_ENABLED_LIGHT].setSmoothBrightness(masteringEnabled ? 1.f : 0.f, args.sampleTime);

		// Update histogram (Waveform)
		hist.currentMinL = std::min(hist.currentMinL, outL);
		hist.currentMaxL = std::max(hist.currentMaxL, outL);
		hist.currentMinR = std::min(hist.currentMinR, outR);
		hist.currentMaxR = std::max(hist.currentMaxR, outR);
		hist.samplesInCurrentBin++;

		if (hist.samplesInCurrentBin >= hist.samplesPerBin) {
			hist.minL[hist.writePtr] = hist.currentMinL;
			hist.maxL[hist.writePtr] = hist.currentMaxL;
			hist.minR[hist.writePtr] = hist.currentMinR;
			hist.maxR[hist.writePtr] = hist.currentMaxR;
			hist.writePtr = (hist.writePtr + 1) % HISTOGRAM_BINS;

			float instantPeak = std::max({std::abs(hist.currentMinL), std::abs(hist.currentMaxL), std::abs(hist.currentMinR), std::abs(hist.currentMaxR)});
			if (instantPeak > hist.smoothedPeak) 
				hist.smoothedPeak = hist.smoothedPeak * 0.9f + instantPeak * 0.1f;
			else
				hist.smoothedPeak = hist.smoothedPeak * 0.999f + instantPeak * 0.001f;
			
			hist.smoothedPeak = clamp(hist.smoothedPeak, 0.5f, 12.f);

			hist.currentMinL = 1e10f; hist.currentMaxL = -1e10f;
			hist.currentMinR = 1e10f; hist.currentMaxR = -1e10f;
			hist.samplesInCurrentBin = 0;
		}

		// Update Spectrum Ring Buffer
		spec.bufferL[spec.writePtr] = outL;
		spec.bufferR[spec.writePtr] = outR;
		spec.writePtr = (spec.writePtr + 1) % FFT_SIZE;

		if (specDivider.process()) {
			for (int i = 0; i < FFT_SIZE; i++) {
				int idx = (spec.writePtr + i) % FFT_SIZE;
				spec.fftInL[i] = spec.bufferL[idx] * spec.window[i];
				spec.fftInR[i] = spec.bufferR[idx] * spec.window[i];
			}

			spec.fft->rfft(spec.fftInL, spec.fftOutL);
			spec.fft->rfft(spec.fftInR, spec.fftOutR);

			auto getMagnitude = [&](float* fftOut, int bin) {
				if (bin <= 0) return std::abs(fftOut[0]);
				if (bin >= FFT_SIZE / 2) return std::abs(fftOut[1]);
				float re = fftOut[2 * bin];
				float im = fftOut[2 * bin + 1];
				return std::sqrt(re * re + im * im);
			};

			float sampleRate = args.sampleRate;
			float maxMag = 0.f;
			for (int i = 0; i < SPEC_FREQ_BINS; i++) {
				float f01 = (float)i / (SPEC_FREQ_BINS - 1);
				float hz = 20.f * std::pow(1000.f, f01);
				float bin = hz / (sampleRate / FFT_SIZE);
				
				int binIdx = (int)bin;
				float magL, magR;
				if (binIdx < FFT_SIZE / 2 - 1) {
					float frac = bin - binIdx;
					magL = (1.f - frac) * getMagnitude(spec.fftOutL, binIdx) + frac * getMagnitude(spec.fftOutL, binIdx + 1);
					magR = (1.f - frac) * getMagnitude(spec.fftOutR, binIdx) + frac * getMagnitude(spec.fftOutR, binIdx + 1);
				} else {
					magL = getMagnitude(spec.fftOutL, FFT_SIZE / 2);
					magR = getMagnitude(spec.fftOutR, FFT_SIZE / 2);
				}

				magL /= 10240.f;
				magR /= 10240.f;

				spec.magnitudesL[i] = spec.magnitudesL[i] * 0.3f + magL * 0.7f;
				spec.magnitudesR[i] = spec.magnitudesR[i] * 0.3f + magR * 0.7f;
				maxMag = std::max({maxMag, spec.magnitudesL[i], spec.magnitudesR[i]});
			}

			float instantPeakDb = 20.f * std::log10(maxMag + 1e-6f);
			if (instantPeakDb > spec.smoothedPeakDb)
				spec.smoothedPeakDb = spec.smoothedPeakDb * 0.1f + instantPeakDb * 0.9f;
			else
				spec.smoothedPeakDb = spec.smoothedPeakDb * 0.995f + instantPeakDb * 0.005f;
			
			spec.smoothedPeakDb = clamp(spec.smoothedPeakDb, -100.f, 20.f);
		}
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "colorScheme", json_integer(colorScheme));
		json_object_set_new(rootJ, "masteringEnabled", json_boolean(masteringEnabled));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* colorSchemeJ = json_object_get(rootJ, "colorScheme");
		if (colorSchemeJ) colorScheme = (ColorScheme)clamp(int(json_integer_value(colorSchemeJ)), 0, SCHEME_LEN - 1);
		json_t* masteringEnabledJ = json_object_get(rootJ, "masteringEnabled");
		if (masteringEnabledJ) {
			masteringEnabled = json_is_true(masteringEnabledJ);
			params[MASTERING_ENABLED_PARAM].setValue(masteringEnabled ? 1.f : 0.f);
		}
	}
};

struct SilColors {
	NVGcolor low;
	NVGcolor high;
	NVGcolor bg;
	NVGcolor divider;

	static SilColors get(Sil::ColorScheme scheme) {
		switch (scheme) {
			case Sil::SCHEME_CLASSIC:
				return {nvgRGBA(0x00, 0xff, 0x00, 0xff), nvgRGBA(0xff, 0x00, 0x00, 0xff), nvgRGBA(0, 0, 0, 255), nvgRGBA(0x00, 0xff, 0x00, 0x40)};
			case Sil::SCHEME_MONOCHROME:
				return {nvgRGBA(0x40, 0x40, 0x40, 0xff), nvgRGBA(0xff, 0xff, 0xff, 0xff), nvgRGBA(0, 0, 0, 255), nvgRGBA(0xff, 0xff, 0xff, 0x40)};
			case Sil::SCHEME_FIRE:
				return {nvgRGBA(0x80, 0x00, 0x00, 0xff), nvgRGBA(0xff, 0xff, 0x00, 0xff), nvgRGBA(0, 0, 0, 255), nvgRGBA(0xff, 0x80, 0x00, 0x40)};
			case Sil::SCHEME_DEFAULT:
			default:
				return {nvgRGBA(0x7a, 0x5c, 0xff, 0xff), nvgRGBA(0x1c, 0xcc, 0xd9, 0xff), nvgRGBA(0, 0, 0, 255), nvgRGBA(0x1c, 0xca, 0xd8, 0x40)};
		}
	}
};

struct HistogramWidget : TransparentWidget {
	Sil* module;

	void draw(const DrawArgs& args) override {
		if (!module) return;
		SilColors colors = SilColors::get(module->colorScheme);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, colors.bg);
		nvgFill(args.vg);

		float midY = box.size.y / 2.f;
		float halfH = box.size.y / 4.f;

		auto drawChannel = [&](const float* minBuf, const float* maxBuf, float centerY) {
			for (int i = 0; i < Sil::HISTOGRAM_BINS; i++) {
				int idx = (module->hist.writePtr + i) % Sil::HISTOGRAM_BINS;
				float x = (float)i / (Sil::HISTOGRAM_BINS - 1) * box.size.x;
				float valMin = clamp(minBuf[idx] / Sil::kAudioFullScaleV, -1.f, 1.f);
				float valMax = clamp(maxBuf[idx] / Sil::kAudioFullScaleV, -1.f, 1.f);
				float yMin = centerY - valMin * halfH;
				float yMax = centerY - valMax * halfH;
				float amp = std::max(std::abs(valMin), std::abs(valMax));
				NVGcolor color = nvgLerpRGBA(colors.low, colors.high, amp);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, yMin);
				nvgLineTo(args.vg, x, yMax);
				nvgStrokeColor(args.vg, color);
				nvgStrokeWidth(args.vg, 1.0f);
				nvgStroke(args.vg);
			}
		};

		drawChannel(module->hist.minL, module->hist.maxL, midY * 0.5f);
		drawChannel(module->hist.minR, module->hist.maxR, midY * 1.5f);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0, midY);
		nvgLineTo(args.vg, box.size.x, midY);
		nvgStrokeColor(args.vg, colors.divider);
		nvgStrokeWidth(args.vg, 0.5f);
		nvgStroke(args.vg);
	}
};

struct SpectrumWidget : TransparentWidget {
	Sil* module;
	bool isRightChannel = false;

	void draw(const DrawArgs& args) override {
		if (!module) return;
		SilColors colors = SilColors::get(module->colorScheme);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, colors.bg);
		nvgFill(args.vg);

		// Draw background grid
		float ceilingDb = module->spec.smoothedPeakDb + 6.f;
		float floorDb = ceilingDb - 70.f;

		auto getX = [&](float hz) {
			float f01 = std::log10(hz / 20.f) / 3.f; // log10(20000/20) = 3
			return f01 * box.size.x;
		};

		// Vertical frequency lines: 10 per decade
		for (float decade = 10.f; decade <= 10000.f; decade *= 10.f) {
			for (int i = 1; i <= 9; i++) {
				float f = decade * i;
				if (f < 20.f) continue;
				if (f > 20000.f) break;
				
				float x = getX(f);
				if (x < 0 || x > box.size.x) continue;
				
				bool isDecade = (i == 1);

				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, 0);
				nvgLineTo(args.vg, x, box.size.y);
				nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, isDecade ? 34 : 16));
				nvgStrokeWidth(args.vg, isDecade ? 1.0f : 0.7f);
				nvgStroke(args.vg);
			}
		}
		// 20kHz line
		{
			float x = getX(20000.f);
			if (x >= 0 && x <= box.size.x) {
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, x, 0);
				nvgLineTo(args.vg, x, box.size.y);
				nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 16));
				nvgStrokeWidth(args.vg, 0.7f);
				nvgStroke(args.vg);
			}
		}

		const float* magnitudes = isRightChannel ? module->spec.magnitudesR : module->spec.magnitudesL;
		float barW = box.size.x / Sil::SPEC_FREQ_BINS;

		for (int i = 0; i < Sil::SPEC_FREQ_BINS; i++) {
			float mag = magnitudes[i];
			float db = 20.f * std::log10(mag + 1e-6f);
			float norm = clamp((db - floorDb) / (ceilingDb - floorDb), 0.f, 1.f);
			if (norm <= 0.01f) continue;

			float barH = norm * box.size.y;
			float x = (float)i * barW;
			NVGcolor color = nvgLerpRGBA(colors.low, colors.high, norm);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, x, box.size.y - barH, barW - 0.5f, barH);
			nvgFillColor(args.vg, color);
			nvgFill(args.vg);
		}
	}
};

struct MicropeakDebugReadoutWidget : TransparentWidget {
	Sil* module = nullptr;

	void draw(const DrawArgs& args) override {
		if (!module || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		const float confidence = clamp(module->micropeak.detectionConfidence.load(std::memory_order_relaxed), 0.f, 1.f);
		const int percent = int(std::round(confidence * 100.f));
		char label[64];
		std::snprintf(label, sizeof(label), "Micropeak Repair: %d%%", percent);

		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 10.0f);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGBA(8, 8, 8, 210));
		nvgText(args.vg, 0.45f, box.size.y * 0.5f + 0.45f, label, nullptr);
		nvgFillColor(args.vg, nvgRGBA(240, 240, 240, 255));
		nvgText(args.vg, 0.f, box.size.y * 0.5f, label, nullptr);
	}
};

struct BananutBlack : app::SvgPort {
	BananutBlack() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg")));
	}
};

struct SilWidget : ModuleWidget {
	SilWidget(Sil* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/sil.svg");
		setPanel(createPanel(asset::plugin(pluginInstance, "res/sil.svg")));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		math::Rect histRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "HISTOGRAM", &histRect)) {
			histRect = histRect.grow(Vec(-0.2f, -0.2f));
			HistogramWidget* hw = createWidget<HistogramWidget>(mm2px(histRect.pos));
			hw->box.size = mm2px(histRect.size);
			hw->module = module;
			addChild(hw);
		}

		math::Rect specLRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_LEFT", &specLRect)) {
			specLRect = specLRect.grow(Vec(-0.2f, -0.2f));
			SpectrumWidget* sw = createWidget<SpectrumWidget>(mm2px(specLRect.pos));
			sw->box.size = mm2px(specLRect.size);
			sw->module = module;
			sw->isRightChannel = false;
			addChild(sw);
		}

		math::Rect specRRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_RIGHT", &specRRect)) {
			specRRect = specRRect.grow(Vec(-0.2f, -0.2f));
			SpectrumWidget* sw = createWidget<SpectrumWidget>(mm2px(specRRect.pos));
			sw->box.size = mm2px(specRRect.size);
			sw->module = module;
			sw->isRightChannel = true;
			addChild(sw);
		}

		Vec inputLPos(26.f, 118.f);
		Vec inputRPos(42.f, 118.f);
		Vec outputLPos(58.f, 118.f);
		Vec outputRPos(74.f, 118.f);
		Vec limiterLightPos(48.f, 42.f);
		Vec lowRecoveryLightPos(48.f, 46.f);
		Vec removeMudLightPos(48.f, 47.6f);
		Vec micropeakLightPos(48.f, 49.1f);
		Vec masteringButtonPos(48.f, 53.f);

		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};

		applyPointOverride("INPUT_L_INPUT", &inputLPos);
		applyPointOverride("INPUT_R_INPUT", &inputRPos);
		applyPointOverride("OUTPUT_L_OUTPUT", &outputLPos);
		applyPointOverride("OUTPUT_R_OUTPUT", &outputRPos);
		applyPointOverride("LIMITER_ACTIVE_LIGHT", &limiterLightPos);
		applyPointOverride("LOW_RECOVERY_LIGHT", &lowRecoveryLightPos);
		applyPointOverride("REMOVE_MUD_LIGHT", &removeMudLightPos);
		applyPointOverride("MICROPEAK_LIGHT", &micropeakLightPos);
		applyPointOverride("MASTERING_ENABLED_PARAM", &masteringButtonPos);

		addInput(createInputCentered<PJ301MPort>(mm2px(inputLPos), module, Sil::INPUT_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(inputRPos), module, Sil::INPUT_R_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputLPos), module, Sil::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputRPos), module, Sil::OUTPUT_R_OUTPUT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
			mm2px(masteringButtonPos), module, Sil::MASTERING_ENABLED_PARAM, Sil::MASTERING_ENABLED_LIGHT
		));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(limiterLightPos), module, Sil::LIMITER_ACTIVE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(lowRecoveryLightPos), module, Sil::LOW_RECOVERY_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(removeMudLightPos), module, Sil::REMOVE_MUD_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(micropeakLightPos), module, Sil::MICROPEAK_LIGHT));

		MicropeakDebugReadoutWidget* micropeakReadout =
			createWidget<MicropeakDebugReadoutWidget>(mm2px(micropeakLightPos.plus(Vec(2.8f, 0.f))));
		micropeakReadout->box.size = mm2px(Vec(28.f, 3.8f));
		micropeakReadout->module = module;
		addChild(micropeakReadout);
	}

	void appendContextMenu(Menu* menu) override {
		Sil* sil = dynamic_cast<Sil*>(module);
		if (!sil) return;

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Visuals"));
		menu->addChild(createSubmenuItem("Color Scheme", "",
			[=](Menu* submenu) {
				auto addSchemeItem = [=](Sil::ColorScheme scheme, std::string label) {
					submenu->addChild(createCheckMenuItem(label, "",
						[=]() { return sil->colorScheme == scheme; },
						[=]() { sil->colorScheme = scheme; }
					));
				};
				addSchemeItem(Sil::SCHEME_DEFAULT, "Default (Cyan/Purple)");
				addSchemeItem(Sil::SCHEME_CLASSIC, "Classic (Green/Red)");
				addSchemeItem(Sil::SCHEME_MONOCHROME, "Monochrome (White/Gray)");
				addSchemeItem(Sil::SCHEME_FIRE, "Fire (Yellow/Red)");
			}
		));
	}
};

Model* modelSil = createModel<Sil, SilWidget>("Sil");
