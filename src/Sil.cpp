#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include "SilMicropeak.hpp"
#include <vector>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
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
		REPAIR_ENABLED_PARAM,
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
		GLUE_COMP_LIGHT,
		SATURATOR_LIGHT,
		STEREO_ENHANCE_LIGHT,
		MICROPEAK_LIGHT,
		MASTERING_ENABLED_LIGHT,
		REPAIR_ENABLED_LIGHT,
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
	bool repairEnabled = true;

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
	float lowBandCorrCoeff = 0.f;
	float lowBandSideAttackCoeff = 0.f;
	float lowBandSideReleaseCoeff = 0.f;
	float mudEnvAttackCoeff = 0.f;
	float mudEnvReleaseCoeff = 0.f;
	float mudAttackCoeff = 0.f;
	float mudReleaseCoeff = 0.f;
	float glueRmsCoeff = 0.f;
	float glueAttackCoeff = 0.f;
	float glueReleaseCoeff = 0.f;
	float stereoEnvAttackCoeff = 0.f;
	float stereoEnvReleaseCoeff = 0.f;
	float stereoMidGainAttackCoeff = 0.f;
	float stereoMidGainReleaseCoeff = 0.f;
	float stereoSideGainAttackCoeff = 0.f;
	float stereoSideGainReleaseCoeff = 0.f;
	float limiterAttackCoeff = 0.f;
	float limiterReleaseCoeff = 0.f;
	float limiterCeiling = 0.f;
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
	struct GlueCompressorState {
		dsp::RCFilter sidechainHp;
		float rmsEnv = 1e-8f;
		float gainReductionDb = 0.f;
		float makeupDb = 0.f;
		float ledAmount = 0.f;

		void reset() {
			rmsEnv = 1e-8f;
			gainReductionDb = 0.f;
			makeupDb = 0.f;
			ledAmount = 0.f;
		}
	} glue;
	struct StereoEnhanceState {
		dsp::RCFilter mid350Hp;
		dsp::RCFilter mid350Lp;
		dsp::RCFilter midBroadHp;
		dsp::RCFilter midBroadLp;
		dsp::RCFilter side6kHp;
		dsp::RCFilter side6kLp;
		dsp::RCFilter sideBroadHp;
		dsp::RCFilter sideBroadLp;
		float mid350Env = 1e-6f;
		float midBroadEnv = 1e-6f;
		float side6kEnv = 1e-6f;
		float sideBroadEnv = 1e-6f;
		float targetMidCutDb = 0.f;
		float smoothedMidCutDb = 0.f;
		float targetSideLiftDb = 0.f;
		float smoothedSideLiftDb = 0.f;
		float midActivation = 0.f;
		float sideActivation = 0.f;
		float ledAmount = 0.f;
		dsp::ClockDivider coeffDivider;
		Biquad midEq;
		Biquad sideEq;
		bool coeffsNeutral = true;
	} stereoEnhance;
	struct SaturatorState {
		static constexpr int HISTORY_BINS = 1000;
		static constexpr int PERCENTILE_BINS = 96;
		float peakBins[HISTORY_BINS] = {};
		uint16_t percentileHist[PERCENTILE_BINS] = {};
		uint8_t binToHist[HISTORY_BINS] = {};
		int writeBin = 0;
		int samplesInBin = 0;
		int samplesPerBin = 441;
		float currentBinPeak = 0.f;
		float drive = 1.f;
		float makeupDb = 0.f;
		float makeupLinear = 1.f;
		float driveNormInv = 1.f;
		float ledAmount = 0.f;
		dsp::ClockDivider updateDivider;

		void reset(float sampleRate) {
			samplesPerBin = std::max(1, int(std::round(sampleRate * 10.f / HISTORY_BINS)));
			std::fill(std::begin(peakBins), std::end(peakBins), 0.f);
			std::fill(std::begin(percentileHist), std::end(percentileHist), uint16_t(0));
			std::fill(std::begin(binToHist), std::end(binToHist), uint8_t(0));
			percentileHist[0] = HISTORY_BINS;
			writeBin = 0;
			samplesInBin = 0;
			currentBinPeak = 0.f;
			drive = 1.f;
			makeupDb = 0.f;
			makeupLinear = 1.f;
			driveNormInv = 1.f;
			ledAmount = 0.f;
			updateDivider.setDivision(512);
		}
	} saturator;

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
	static constexpr float kGlueRatio = 1.5f;
	static constexpr float kGlueAttackSec = 0.030f;
	static constexpr float kGlueReleaseSec = 0.250f;
	static constexpr float kGlueKneeDb = 8.f;
	static constexpr float kGlueThresholdDb = -14.f;
	static constexpr float kGlueMaxGainReductionDb = 3.f;
	static constexpr float kGlueMaxMakeupDb = 1.25f;
	static constexpr float kGlueMakeupFraction = 0.50f;
	static constexpr float kGlueSidechainHpHz = 90.f;
	static constexpr float kStereoMidCenterHz = 350.f;
	static constexpr float kStereoMidQ = 7.3f;
	static constexpr float kStereoMidMaxCutDb = 2.0f;
	static constexpr float kStereoSideCenterHz = 6000.f;
	static constexpr float kStereoSideQ = 0.71f;
	static constexpr float kStereoSideMaxLiftDb = 2.0f;
	static constexpr float kStereoMid350LowHz = 270.f;
	static constexpr float kStereoMid350HighHz = 470.f;
	static constexpr float kStereoMidBroadLowHz = 120.f;
	static constexpr float kStereoMidBroadHighHz = 1400.f;
	static constexpr float kStereoSide6kLowHz = 4200.f;
	static constexpr float kStereoSide6kHighHz = 9500.f;
	static constexpr float kStereoSideBroadLowHz = 1000.f;
	static constexpr float kStereoSideBroadHighHz = 12000.f;
	static constexpr float kStereoMidBandNormDb = 7.5f;
	static constexpr float kStereoSideBandNormDb = 3.0f;
	static constexpr float kStereoMidGateDbFs = -42.f;
	static constexpr float kStereoMidGateKneeDb = 10.f;
	static constexpr float kStereoMidExcessThresholdDb = 1.0f;
	static constexpr float kStereoMidExcessKneeDb = 5.0f;
	static constexpr float kStereoSideGateDbFs = -56.f;
	static constexpr float kStereoSideGateKneeDb = 10.f;
	static constexpr float kStereoSideAlreadyBrightDb = 1.5f;
	static constexpr float kStereoSideBrightKneeDb = 4.0f;
	static constexpr float kStereoEnvAttackSec = 0.040f;
	static constexpr float kStereoEnvReleaseSec = 0.300f;
	static constexpr float kStereoMidGainAttackSec = 0.180f;
	static constexpr float kStereoMidGainReleaseSec = 0.900f;
	static constexpr float kStereoSideGainAttackSec = 0.250f;
	static constexpr float kStereoSideGainReleaseSec = 1.200f;
	static constexpr int kStereoEnhanceCoeffDivision = 32;
	static constexpr float kLimiterCeilingDb = -1.0f;
	static constexpr float kSaturatorTargetPreLimiterDb = -0.75f;
	static constexpr float kSatMaxMakeupDb = 2.0f;
	static constexpr float kSatMaxDrive = 1.35f;
	static constexpr float kSatMinDrive = 1.0f;
	static constexpr float kSatMakeupAttackSec = 1.50f;
	static constexpr float kSatMakeupReleaseSec = 4.00f;
	static constexpr float kSatDriveAttackSec = 2.00f;
	static constexpr float kSatDriveReleaseSec = 5.00f;
	static constexpr float kSatHistoryPercentile = 0.995f;
	static constexpr int kSatUpdateDivision = 512;

	static float toDbSafe(float v) {
		return 20.f * std::log10(std::max(v, 1e-7f));
	}
	static float toDbFsSafe(float volts) {
		return 20.f * std::log10(std::max(volts, 1e-7f) / kAudioFullScaleV);
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
	static float inverseSoftKnee01(float xDb, float thresholdDb, float kneeDb) {
		return 1.f - softKnee01(xDb, thresholdDb, kneeDb);
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

	void updateDynamicsCoefficients(float sampleRate) {
		const float sr = std::max(sampleRate, 1.f);
		lowBandCorrCoeff = std::exp(-1.f / (kLowBandCorrTauSec * sr));
		lowBandSideAttackCoeff = std::exp(-1.f / (kLowBandSideAttackSec * sr));
		lowBandSideReleaseCoeff = std::exp(-1.f / (kLowBandSideReleaseSec * sr));
		mudEnvAttackCoeff = std::exp(-1.f / (kMudEnvAttackSec * sr));
		mudEnvReleaseCoeff = std::exp(-1.f / (kMudEnvReleaseSec * sr));
		mudAttackCoeff = std::exp(-1.f / (kMudAttackSec * sr));
		mudReleaseCoeff = std::exp(-1.f / (kMudReleaseSec * sr));
		glueRmsCoeff = std::exp(-1.f / (0.050f * sr));
		glueAttackCoeff = std::exp(-1.f / (kGlueAttackSec * sr));
		glueReleaseCoeff = std::exp(-1.f / (kGlueReleaseSec * sr));
		stereoEnvAttackCoeff = std::exp(-1.f / (kStereoEnvAttackSec * sr));
		stereoEnvReleaseCoeff = std::exp(-1.f / (kStereoEnvReleaseSec * sr));
		stereoMidGainAttackCoeff = std::exp(-1.f / (kStereoMidGainAttackSec * sr));
		stereoMidGainReleaseCoeff = std::exp(-1.f / (kStereoMidGainReleaseSec * sr));
		stereoSideGainAttackCoeff = std::exp(-1.f / (kStereoSideGainAttackSec * sr));
		stereoSideGainReleaseCoeff = std::exp(-1.f / (kStereoSideGainReleaseSec * sr));
		limiterAttackCoeff = std::exp(-1.f / (0.0005f * sr));
		limiterReleaseCoeff = std::exp(-1.f / (0.080f * sr));
		limiterCeiling = kAudioFullScaleV * std::pow(10.f, kLimiterCeilingDb / 20.f);
	}

	int saturatorPeakToHistIndex(float peak) const {
		const float normalized = clamp(peak / kAudioFullScaleV, 0.f, 2.f);
		const float scaled = normalized * float(SaturatorState::PERCENTILE_BINS - 1);
		return clamp(int(std::round(scaled)), 0, SaturatorState::PERCENTILE_BINS - 1);
	}

	float saturatorHistIndexToPeak(int idx) const {
		const float n = float(clamp(idx, 0, SaturatorState::PERCENTILE_BINS - 1)) /
			float(SaturatorState::PERCENTILE_BINS - 1);
		return n * 2.f * kAudioFullScaleV;
	}

	void saturatorPushPeakBin(float peak) {
		const int oldHist = int(saturator.binToHist[saturator.writeBin]);
		if (oldHist >= 0 && oldHist < SaturatorState::PERCENTILE_BINS && saturator.percentileHist[oldHist] > 0) {
			saturator.percentileHist[oldHist]--;
		}
		const int newHist = saturatorPeakToHistIndex(peak);
		saturator.percentileHist[newHist]++;
		saturator.binToHist[saturator.writeBin] = uint8_t(newHist);
		saturator.peakBins[saturator.writeBin] = peak;
		saturator.writeBin++;
		if (saturator.writeBin >= SaturatorState::HISTORY_BINS) {
			saturator.writeBin = 0;
		}
	}

	float saturatorEstimateRecentPeakPercentile() const {
		const int targetRank = clamp(
			int(std::round(kSatHistoryPercentile * float(SaturatorState::HISTORY_BINS - 1))),
			0,
			SaturatorState::HISTORY_BINS - 1
		);
		int accum = 0;
		for (int i = 0; i < SaturatorState::PERCENTILE_BINS; ++i) {
			accum += int(saturator.percentileHist[i]);
			if (accum > targetRank) {
				return saturatorHistIndexToPeak(i);
			}
		}
		return saturatorHistIndexToPeak(SaturatorState::PERCENTILE_BINS - 1);
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

	void updateGlueCutoff(float sampleRate) {
		const float norm = clamp(kGlueSidechainHpHz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
		glue.sidechainHp.setCutoff(norm);
	}
	void updateStereoEnhanceCutoffs(float sampleRate) {
		const auto norm = [&](float hz) {
			return clamp(hz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
		};
		stereoEnhance.mid350Hp.setCutoff(norm(kStereoMid350LowHz));
		stereoEnhance.mid350Lp.setCutoff(norm(kStereoMid350HighHz));
		stereoEnhance.midBroadHp.setCutoff(norm(kStereoMidBroadLowHz));
		stereoEnhance.midBroadLp.setCutoff(norm(kStereoMidBroadHighHz));
		stereoEnhance.side6kHp.setCutoff(norm(kStereoSide6kLowHz));
		stereoEnhance.side6kLp.setCutoff(norm(kStereoSide6kHighHz));
		stereoEnhance.sideBroadHp.setCutoff(norm(kStereoSideBroadLowHz));
		stereoEnhance.sideBroadLp.setCutoff(norm(kStereoSideBroadHighHz));
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
	void resetStereoEnhanceState() {
		stereoEnhance.mid350Env = 1e-6f;
		stereoEnhance.midBroadEnv = 1e-6f;
		stereoEnhance.side6kEnv = 1e-6f;
		stereoEnhance.sideBroadEnv = 1e-6f;
		stereoEnhance.targetMidCutDb = 0.f;
		stereoEnhance.smoothedMidCutDb = 0.f;
		stereoEnhance.targetSideLiftDb = 0.f;
		stereoEnhance.smoothedSideLiftDb = 0.f;
		stereoEnhance.midActivation = 0.f;
		stereoEnhance.sideActivation = 0.f;
		stereoEnhance.ledAmount = 0.f;
		stereoEnhance.coeffsNeutral = true;
		stereoEnhance.mid350Hp.reset();
		stereoEnhance.mid350Lp.reset();
		stereoEnhance.midBroadHp.reset();
		stereoEnhance.midBroadLp.reset();
		stereoEnhance.side6kHp.reset();
		stereoEnhance.side6kLp.reset();
		stereoEnhance.sideBroadHp.reset();
		stereoEnhance.sideBroadLp.reset();
		stereoEnhance.midEq.reset();
		stereoEnhance.sideEq.reset();
	}

	Sil() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(MASTERING_ENABLED_PARAM, 0.f, 1.f, 1.f, "Mastering", {"Disabled", "Enabled"});
		configSwitch(REPAIR_ENABLED_PARAM, 0.f, 1.f, 1.f, "Repair", {"Disabled", "Enabled"});
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
		stereoEnhance.coeffDivider.setDivision(kStereoEnhanceCoeffDivision);
		saturator.updateDivider.setDivision(kSatUpdateDivision);
		updateLowBandCutoff(APP->engine->getSampleRate());
		updateRemoveMudCutoffs(APP->engine->getSampleRate());
		updateGlueCutoff(APP->engine->getSampleRate());
		updateStereoEnhanceCutoffs(APP->engine->getSampleRate());
		updateDynamicsCoefficients(APP->engine->getSampleRate());
		configureRollingBuffer(APP->engine->getSampleRate());
		saturator.reset(APP->engine->getSampleRate());
		removeMud.peakingL.setPeaking(APP->engine->getSampleRate(), kMudCenterHz, kMudQ, 0.f);
		removeMud.peakingR.setPeaking(APP->engine->getSampleRate(), kMudCenterHz, kMudQ, 0.f);
		stereoEnhance.midEq.setPeaking(APP->engine->getSampleRate(), kStereoMidCenterHz, kStereoMidQ, 0.f);
		stereoEnhance.sideEq.setPeaking(APP->engine->getSampleRate(), kStereoSideCenterHz, kStereoSideQ, 0.f);
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
		updateGlueCutoff(e.sampleRate);
		updateStereoEnhanceCutoffs(e.sampleRate);
		updateDynamicsCoefficients(e.sampleRate);
		configureRollingBuffer(e.sampleRate);
		micropeakCleanupFilter.reset();
		resetRemoveMudState();
		resetStereoEnhanceState();
		stereoEnhance.midEq.setPeaking(e.sampleRate, kStereoMidCenterHz, kStereoMidQ, 0.f);
		stereoEnhance.sideEq.setPeaking(e.sampleRate, kStereoSideCenterHz, kStereoSideQ, 0.f);
		glue.reset();
		saturator.reset(e.sampleRate);
		micropeak.weakStreak = 0;
		micropeak.lastSeverityEma.store(0.f, std::memory_order_relaxed);
		micropeak.detectionConfidence.store(0.f, std::memory_order_relaxed);
	}

	void process(const ProcessArgs& args) override {
		masteringEnabled = params[MASTERING_ENABLED_PARAM].getValue() > 0.5f;
		repairEnabled = params[REPAIR_ENABLED_PARAM].getValue() > 0.5f;

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

		const float corrCoeff = lowBandCorrCoeff;
		const float corrMix = 1.f - corrCoeff;
		lowBandCorrLL = corrCoeff * lowBandCorrLL + corrMix * (lowL * lowL);
		lowBandCorrRR = corrCoeff * lowBandCorrRR + corrMix * (lowR * lowR);
		lowBandCorrLR = corrCoeff * lowBandCorrLR + corrMix * (lowL * lowR);
		const float denom = std::sqrt(std::max(lowBandCorrLL * lowBandCorrRR, 1e-12f));
		const float lowCorr = clamp(lowBandCorrLR / denom, -1.f, 1.f);
		const float targetLowSideGain = (lowCorr >= 0.70f) ? 1.f : ((lowCorr <= 0.f) ? 0.f : (lowCorr / 0.70f));
		const float sideCoeff = (targetLowSideGain < lowBandSideGain) ? lowBandSideAttackCoeff : lowBandSideReleaseCoeff;
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

			auto updateEnv = [&](float& env, float x) {
				const float absX = std::fabs(x);
				const float c = (absX > env) ? mudEnvAttackCoeff : mudEnvReleaseCoeff;
				env = absX + c * (env - absX);
			};
			updateEnv(removeMud.mudEnv, mudBand);
			updateEnv(removeMud.bassEnv, bassBand);
			updateEnv(removeMud.presenceEnv, presenceBand);

			const float refEnv = 0.5f * removeMud.bassEnv + 0.5f * removeMud.presenceEnv;
			const float mudDeltaDb = toDbSafe(removeMud.mudEnv) - toDbSafe(refEnv) - kMudAllowedWarmthDb;
			const float activation = softKnee01(mudDeltaDb, kMudThresholdDb, kMudKneeDb);
			removeMud.targetCutDb = -kMudMaxCutDb * activation;

			const float coeff = (removeMud.targetCutDb < removeMud.smoothedCutDb) ? mudAttackCoeff : mudReleaseCoeff;
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
		if (!repairEnabled) {
			micropeak.holdSamples.store(0, std::memory_order_relaxed);
			micropeak.latchedActive.store(false, std::memory_order_relaxed);
			micropeak.lastEventCount.store(0, std::memory_order_relaxed);
			micropeak.lastSeverity.store(0.f, std::memory_order_relaxed);
			micropeak.lastSeverityEma.store(0.f, std::memory_order_relaxed);
			micropeak.detectionConfidence.store(0.f, std::memory_order_relaxed);
			micropeak.weakStreak = 0;
			micropeakCleanupFilter.reset();
		}
		const bool micropeakActive = repairEnabled ? consumeMicropeakHoldSample() : false;
		const sil_micropeak::StereoSample cleaned = repairEnabled
			? micropeakCleanupFilter.process(
				  sil_micropeak::StereoSample(preMasterL, preMasterR),
				  micropeakActive,
				  kAudioFullScaleV
			  )
			: sil_micropeak::StereoSample(preMasterL, preMasterR);
		float gluedL = cleaned.l;
		float gluedR = cleaned.r;
		float glueLed = 0.f;
		if (masteringEnabled) {
			const float mono = 0.5f * (cleaned.l + cleaned.r);
			glue.sidechainHp.process(mono);
			const float sidechain = glue.sidechainHp.highpass();
			const float rmsTarget = sidechain * sidechain;
			glue.rmsEnv = glueRmsCoeff * glue.rmsEnv + (1.f - glueRmsCoeff) * rmsTarget;
			const float levelDb = 20.f * std::log10(std::sqrt(std::max(glue.rmsEnv, 1e-12f)) / kAudioFullScaleV + 1e-9f);
			const float overDb = levelDb - kGlueThresholdDb;
			const float halfKnee = 0.5f * kGlueKneeDb;
			float targetGrDb = 0.f;
			if (overDb > -halfKnee) {
				const float hardGr = std::max(0.f, overDb - overDb / kGlueRatio);
				if (overDb >= halfKnee) {
					targetGrDb = hardGr;
				}
				else {
					const float t = clamp((overDb + halfKnee) / std::max(kGlueKneeDb, 1e-6f), 0.f, 1.f);
					const float s = t * t * (3.f - 2.f * t);
					targetGrDb = hardGr * s;
				}
			}
			targetGrDb = clamp(targetGrDb, 0.f, kGlueMaxGainReductionDb);
			const float grCoeff = (targetGrDb > glue.gainReductionDb) ? glueAttackCoeff : glueReleaseCoeff;
			glue.gainReductionDb = targetGrDb + grCoeff * (glue.gainReductionDb - targetGrDb);
			glue.makeupDb = clamp(glue.gainReductionDb * kGlueMakeupFraction, 0.f, kGlueMaxMakeupDb);
			const float totalGainDb = -glue.gainReductionDb + glue.makeupDb;
			const float gain = std::pow(10.f, totalGainDb / 20.f);
			gluedL = cleaned.l * gain;
			gluedR = cleaned.r * gain;
			glue.ledAmount = clamp(glue.gainReductionDb / kGlueMaxGainReductionDb, 0.f, 1.f);
			glueLed = glue.ledAmount;
		}
		else {
			glue.reset();
		}
		float enhancedL = gluedL;
		float enhancedR = gluedR;
		float stereoEnhanceLed = 0.f;
		if (masteringEnabled) {
			const float mid = 0.5f * (gluedL + gluedR);
			const float side = 0.5f * (gluedL - gluedR);

			stereoEnhance.mid350Hp.process(mid);
			const float mid350High = stereoEnhance.mid350Hp.highpass();
			stereoEnhance.mid350Lp.process(mid350High);
			const float mid350Band = stereoEnhance.mid350Lp.lowpass();
			stereoEnhance.midBroadHp.process(mid);
			const float midBroadHigh = stereoEnhance.midBroadHp.highpass();
			stereoEnhance.midBroadLp.process(midBroadHigh);
			const float midBroadBand = stereoEnhance.midBroadLp.lowpass();
			stereoEnhance.side6kHp.process(side);
			const float side6kHigh = stereoEnhance.side6kHp.highpass();
			stereoEnhance.side6kLp.process(side6kHigh);
			const float side6kBand = stereoEnhance.side6kLp.lowpass();
			stereoEnhance.sideBroadHp.process(side);
			const float sideBroadHigh = stereoEnhance.sideBroadHp.highpass();
			stereoEnhance.sideBroadLp.process(sideBroadHigh);
			const float sideBroadBand = stereoEnhance.sideBroadLp.lowpass();

			auto updateStereoEnv = [&](float& env, float x) {
				const float absX = std::fabs(x);
				const float c = (absX > env) ? stereoEnvAttackCoeff : stereoEnvReleaseCoeff;
				env = absX + c * (env - absX);
			};
			updateStereoEnv(stereoEnhance.mid350Env, mid350Band);
			updateStereoEnv(stereoEnhance.midBroadEnv, midBroadBand);
			updateStereoEnv(stereoEnhance.side6kEnv, side6kBand);
			updateStereoEnv(stereoEnhance.sideBroadEnv, sideBroadBand);

			const float mid350DbFs = toDbFsSafe(stereoEnhance.mid350Env);
			const float midBroadDbFs = toDbFsSafe(stereoEnhance.midBroadEnv);
			const float midPresenceGate = softKnee01(mid350DbFs, kStereoMidGateDbFs, kStereoMidGateKneeDb);
			const float midExcessDb = (mid350DbFs - midBroadDbFs) + kStereoMidBandNormDb;
			const float midExcess = softKnee01(midExcessDb, kStereoMidExcessThresholdDb, kStereoMidExcessKneeDb);
			const float targetMidActivation = clamp(midPresenceGate * midExcess, 0.f, 1.f);
			stereoEnhance.targetMidCutDb = -kStereoMidMaxCutDb * targetMidActivation;

			const float side6kDbFs = toDbFsSafe(stereoEnhance.side6kEnv);
			const float sideBroadDbFs = toDbFsSafe(stereoEnhance.sideBroadEnv);
			const float sideContentGate = softKnee01(
				std::max(side6kDbFs, sideBroadDbFs),
				kStereoSideGateDbFs,
				kStereoSideGateKneeDb
			);
			const float sideBrightnessDb = (side6kDbFs - sideBroadDbFs) + kStereoSideBandNormDb;
			const float sideNotAlreadyBright = inverseSoftKnee01(
				sideBrightnessDb,
				kStereoSideAlreadyBrightDb,
				kStereoSideBrightKneeDb
			);
			const float targetSideActivation = clamp(sideContentGate * sideNotAlreadyBright, 0.f, 1.f);
			stereoEnhance.targetSideLiftDb = kStereoSideMaxLiftDb * targetSideActivation;

			const float midCoeff =
				(stereoEnhance.targetMidCutDb < stereoEnhance.smoothedMidCutDb)
					? stereoMidGainAttackCoeff
					: stereoMidGainReleaseCoeff;
			stereoEnhance.smoothedMidCutDb =
				stereoEnhance.targetMidCutDb + midCoeff * (stereoEnhance.smoothedMidCutDb - stereoEnhance.targetMidCutDb);

			const float sideCoeff =
				(stereoEnhance.targetSideLiftDb > stereoEnhance.smoothedSideLiftDb)
					? stereoSideGainAttackCoeff
					: stereoSideGainReleaseCoeff;
			stereoEnhance.smoothedSideLiftDb =
				stereoEnhance.targetSideLiftDb + sideCoeff * (stereoEnhance.smoothedSideLiftDb - stereoEnhance.targetSideLiftDb);

			stereoEnhance.midActivation = clamp(
				-stereoEnhance.smoothedMidCutDb / std::max(kStereoMidMaxCutDb, 1e-6f),
				0.f,
				1.f
			);
			stereoEnhance.sideActivation = clamp(
				stereoEnhance.smoothedSideLiftDb / std::max(kStereoSideMaxLiftDb, 1e-6f),
				0.f,
				1.f
			);

			if (stereoEnhance.coeffDivider.process()) {
				stereoEnhance.midEq.setPeaking(
					args.sampleRate,
					kStereoMidCenterHz,
					kStereoMidQ,
					stereoEnhance.smoothedMidCutDb
				);
				stereoEnhance.sideEq.setPeaking(
					args.sampleRate,
					kStereoSideCenterHz,
					kStereoSideQ,
					stereoEnhance.smoothedSideLiftDb
				);
				stereoEnhance.coeffsNeutral =
					std::fabs(stereoEnhance.smoothedMidCutDb) < 1e-5f &&
					std::fabs(stereoEnhance.smoothedSideLiftDb) < 1e-5f;
			}

			const float enhancedMid = stereoEnhance.midEq.process(mid);
			const float enhancedSide = stereoEnhance.sideEq.process(side);
			enhancedL = enhancedMid + enhancedSide;
			enhancedR = enhancedMid - enhancedSide;

			// Meter the equivalent per-channel change rather than raw M/S leg movement,
			// so one-sided or near-mono material does not read artificially hot.
			const float leftDeltaDb = std::fabs(
				0.5f * stereoEnhance.smoothedMidCutDb +
				0.5f * stereoEnhance.smoothedSideLiftDb
			);
			const float rightDeltaDb = std::fabs(
				0.5f * stereoEnhance.smoothedMidCutDb -
				0.5f * stereoEnhance.smoothedSideLiftDb
			);
			const float ledDeadbandDb = 0.5f;
			const auto deadbandNorm = [&](float deltaDb) {
				const float activeDb = std::max(0.f, deltaDb - ledDeadbandDb);
				const float spanDb = std::max(kStereoMidMaxCutDb - ledDeadbandDb, 1e-6f);
				return clamp(activeDb / spanDb, 0.f, 1.f);
			};
			const float leftDeltaNorm = deadbandNorm(leftDeltaDb);
			const float rightDeltaNorm = deadbandNorm(rightDeltaDb);
			stereoEnhance.ledAmount = clamp(0.5f * leftDeltaNorm + 0.5f * rightDeltaNorm, 0.f, 1.f);
			stereoEnhanceLed = stereoEnhance.ledAmount;
		}
		else {
			stereoEnhance.targetMidCutDb = 0.f;
			stereoEnhance.smoothedMidCutDb = 0.f;
			stereoEnhance.targetSideLiftDb = 0.f;
			stereoEnhance.smoothedSideLiftDb = 0.f;
			stereoEnhance.midActivation = 0.f;
			stereoEnhance.sideActivation = 0.f;
			stereoEnhance.ledAmount = 0.f;
			if (!stereoEnhance.coeffsNeutral) {
				stereoEnhance.midEq.setPeaking(args.sampleRate, kStereoMidCenterHz, kStereoMidQ, 0.f);
				stereoEnhance.sideEq.setPeaking(args.sampleRate, kStereoSideCenterHz, kStereoSideQ, 0.f);
				stereoEnhance.coeffsNeutral = true;
			}
		}

		float saturatedL = enhancedL;
		float saturatedR = enhancedR;
		float saturatorLed = 0.f;
		if (masteringEnabled) {
			const float preSatPeak = std::max(std::fabs(enhancedL), std::fabs(enhancedR));
			saturator.currentBinPeak = std::max(saturator.currentBinPeak, preSatPeak);
			saturator.samplesInBin++;
			if (saturator.samplesInBin >= saturator.samplesPerBin) {
				saturatorPushPeakBin(saturator.currentBinPeak);
				saturator.samplesInBin = 0;
				saturator.currentBinPeak = 0.f;
			}

			if (saturator.updateDivider.process()) {
				const float recentLoudPeak = std::max(saturatorEstimateRecentPeakPercentile(), 1e-6f);
				const float recentPeakNorm = clamp(recentLoudPeak / kAudioFullScaleV, 1e-6f, 4.f);
				const float recentPeakDb = 20.f * std::log10(recentPeakNorm);
				const float desiredMakeupDb = clamp(
					kSaturatorTargetPreLimiterDb - recentPeakDb,
					0.f,
					kSatMaxMakeupDb
				);
				const float desiredDrive = clamp(
					1.f + desiredMakeupDb * 0.22f,
					kSatMinDrive,
					kSatMaxDrive
				);
				const float updateSeconds = float(kSatUpdateDivision) / std::max(args.sampleRate, 1.f);
				const float makeupTau = (desiredMakeupDb > saturator.makeupDb) ? kSatMakeupAttackSec : kSatMakeupReleaseSec;
				const float makeupCoeff = std::exp(-updateSeconds / std::max(1e-3f, makeupTau));
				saturator.makeupDb = desiredMakeupDb + makeupCoeff * (saturator.makeupDb - desiredMakeupDb);
				const float driveTau = (desiredDrive > saturator.drive) ? kSatDriveAttackSec : kSatDriveReleaseSec;
				const float driveCoeff = std::exp(-updateSeconds / std::max(1e-3f, driveTau));
				saturator.drive = desiredDrive + driveCoeff * (saturator.drive - desiredDrive);
				saturator.makeupLinear = std::pow(10.f, saturator.makeupDb / 20.f);
				saturator.driveNormInv = 1.f / std::max(std::atan(clamp(saturator.drive, kSatMinDrive, kSatMaxDrive)), 1e-6f);
			}

			const float makeup = saturator.makeupLinear;
			const float drive = clamp(saturator.drive, kSatMinDrive, kSatMaxDrive);
			const float driveNormInv = saturator.driveNormInv;
			auto shape = [&](float x) {
				const float xNorm = clamp((x * makeup) / kAudioFullScaleV, -3.f, 3.f);
				const float shapedNorm = std::atan(drive * xNorm) * driveNormInv;
				return shapedNorm * kAudioFullScaleV;
			};
			saturatedL = shape(enhancedL);
			saturatedR = shape(enhancedR);
			const float makeupActivity = clamp(saturator.makeupDb / kSatMaxMakeupDb, 0.f, 1.f);
			const float driveActivity = clamp(
				(drive - kSatMinDrive) / std::max(kSatMaxDrive - kSatMinDrive, 1e-6f),
				0.f,
				1.f
			);
			const float rawLed = 0.55f * makeupActivity + 0.45f * driveActivity;
			saturator.ledAmount = clamp(rawLed * 1.45f, 0.f, 1.f);
			saturatorLed = saturator.ledAmount;
		}
		else {
			saturator.makeupDb = 0.f;
			saturator.makeupLinear = 1.f;
			saturator.drive = 1.f;
			saturator.driveNormInv = 1.f;
			saturator.ledAmount = 0.f;
		}

		float outL = saturatedL;
		float outR = saturatedR;
		float limiterLed = 0.f;
		if (masteringEnabled) {
			// One-sample lookahead "true-peak as much as possible":
			// cleanup emits the delayed center sample, so the current pre-master
			// sample is available as the next point for limiter detection.
			float peak = std::max(std::fabs(saturatedL), std::fabs(saturatedR));
			if (limiterPrevValid) {
				for (int i = 1; i <= kLimiterOversampleFactor; ++i) {
					const float a = float(i) / float(kLimiterOversampleFactor);
					const float interpL = limiterPrevL + (saturatedL - limiterPrevL) * a;
					const float interpR = limiterPrevR + (saturatedR - limiterPrevR) * a;
					peak = std::max(peak, std::max(std::fabs(interpL), std::fabs(interpR)));
				}
			}
			const float desiredGain = (peak > limiterCeiling && peak > 1e-9f) ? (limiterCeiling / peak) : 1.f;
			const float coeff = (desiredGain < limiterGain) ? limiterAttackCoeff : limiterReleaseCoeff;
			limiterGain = desiredGain + coeff * (limiterGain - desiredGain);
			outL = saturatedL * limiterGain;
			outR = saturatedR * limiterGain;
			limiterPrevL = saturatedL;
			limiterPrevR = saturatedR;
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
		lights[GLUE_COMP_LIGHT].setSmoothBrightness(masteringEnabled ? glueLed : 0.f, args.sampleTime);
		lights[STEREO_ENHANCE_LIGHT].setSmoothBrightness(masteringEnabled ? stereoEnhanceLed : 0.f, args.sampleTime);
		lights[SATURATOR_LIGHT].setSmoothBrightness(masteringEnabled ? saturatorLed : 0.f, args.sampleTime);
		if (repairEnabled) {
			pushMicropeakSample(preMasterL, preMasterR, args.sampleRate);
		}
		pushRollingSample(outL, outR);
		const float candidateSeverity = micropeak.lastSeverity.load(std::memory_order_relaxed);
		const int candidateEvents = micropeak.lastEventCount.load(std::memory_order_relaxed);
		const float candidateLed = clamp(candidateSeverity * 0.6f + float(candidateEvents) * 0.08f, 0.f, 0.35f);
		const float micropeakLed = repairEnabled ? (micropeakActive ? 1.f : candidateLed) : 0.f;
		lights[MICROPEAK_LIGHT].setSmoothBrightness(micropeakLed, args.sampleTime);
		lights[MASTERING_ENABLED_LIGHT].setSmoothBrightness(masteringEnabled ? 0.5f : 0.f, args.sampleTime);
		lights[REPAIR_ENABLED_LIGHT].setSmoothBrightness(repairEnabled ? 0.5f : 0.f, args.sampleTime);

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
		json_object_set_new(rootJ, "repairEnabled", json_boolean(repairEnabled));
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
		json_t* repairEnabledJ = json_object_get(rootJ, "repairEnabled");
		if (repairEnabledJ) {
			repairEnabled = json_is_true(repairEnabledJ);
			params[REPAIR_ENABLED_PARAM].setValue(repairEnabled ? 1.f : 0.f);
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
		if (!module->repairEnabled) {
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
		Vec glueCompLightPos(48.f, 48.4f);
		Vec stereoEnhanceLightPos(48.f, 49.6f);
		Vec saturatorLightPos(48.f, 50.4f);
		Vec micropeakLightPos(48.f, 49.1f);
		Vec masteringButtonPos(48.f, 53.f);
		Vec repairButtonPos(48.f, 56.f);

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
		applyPointOverride("GLUE_COMP_LIGHT", &glueCompLightPos);
		applyPointOverride("STEREO_ENHANCE_LIGHT", &stereoEnhanceLightPos);
		applyPointOverride("SATURATOR_LIGHT", &saturatorLightPos);
		applyPointOverride("MICROPEAK_LIGHT", &micropeakLightPos);
		applyPointOverride("MASTERING_ENABLED_PARAM", &masteringButtonPos);
		applyPointOverride("REPAIR_ENABLED_PARAM", &repairButtonPos);

		addInput(createInputCentered<PJ301MPort>(mm2px(inputLPos), module, Sil::INPUT_L_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(inputRPos), module, Sil::INPUT_R_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputLPos), module, Sil::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(outputRPos), module, Sil::OUTPUT_R_OUTPUT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
			mm2px(masteringButtonPos), module, Sil::MASTERING_ENABLED_PARAM, Sil::MASTERING_ENABLED_LIGHT
		));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
			mm2px(repairButtonPos), module, Sil::REPAIR_ENABLED_PARAM, Sil::REPAIR_ENABLED_LIGHT
		));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(limiterLightPos), module, Sil::LIMITER_ACTIVE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(lowRecoveryLightPos), module, Sil::LOW_RECOVERY_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(removeMudLightPos), module, Sil::REMOVE_MUD_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(glueCompLightPos), module, Sil::GLUE_COMP_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(stereoEnhanceLightPos), module, Sil::STEREO_ENHANCE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(saturatorLightPos), module, Sil::SATURATOR_LIGHT));
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
