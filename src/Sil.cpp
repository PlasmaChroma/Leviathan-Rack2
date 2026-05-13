#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include "SilRepairBuffer.hpp"
#include "SilRepairKernel.hpp"
#include <vector>
#include <algorithm>
#include <array>
#include <atomic>
#include <deque>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>

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

		void setHighShelf(float sampleRate, float cutoffHz, float q, float gainDb) {
			if (sampleRate <= 1.f || cutoffHz <= 1.f || q <= 1e-4f) {
				b0 = 1.f;
				b1 = b2 = a1 = a2 = 0.f;
				return;
			}
			const float nyquistGuard = 0.48f * sampleRate;
			const float fc = clamp(cutoffHz, 10.f, nyquistGuard);
			const float A = std::pow(10.f, gainDb / 40.f);
			const float w0 = 2.f * M_PI * fc / sampleRate;
			const float c = std::cos(w0);
			const float s = std::sin(w0);
			const float alpha = s / (2.f * q);
			const float twoSqrtAAlpha = 2.f * std::sqrt(A) * alpha;

			const float rawB0 = A * ((A + 1.f) + (A - 1.f) * c + twoSqrtAAlpha);
			const float rawB1 = -2.f * A * ((A - 1.f) + (A + 1.f) * c);
			const float rawB2 = A * ((A + 1.f) + (A - 1.f) * c - twoSqrtAAlpha);
			const float rawA0 = (A + 1.f) - (A - 1.f) * c + twoSqrtAAlpha;
			const float rawA1 = 2.f * ((A - 1.f) - (A + 1.f) * c);
			const float rawA2 = (A + 1.f) - (A - 1.f) * c - twoSqrtAAlpha;
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
		IMPACT_AIR_LIGHT,
		REMOVE_MUD_LIGHT,
		MID_ENHANCE_LIGHT,
		GLUE_COMP_LIGHT,
		STEREO_ENHANCE_LIGHT,
		SATURATOR_LIGHT,
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
		float displayNormL[SPEC_FREQ_BINS] = {};
		float displayNormR[SPEC_FREQ_BINS] = {};
		
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
	struct SpectrumBinMapEntry {
		int idx = 0;
		float frac = 0.f;
	};
	SpectrumBinMapEntry specBinMap[SPEC_FREQ_BINS];

	dsp::ClockDivider specDivider;
	dsp::ClockDivider lightDivider;
	float limiterGain = 1.f;
	std::vector<float> limiterDelayL;
	std::vector<float> limiterDelayR;
	int limiterDelayWrite = 0;
	int limiterLatencySamples = 1;
	std::vector<float> bypassDelayL;
	std::vector<float> bypassDelayR;
	int bypassDelayWrite = 0;
	sil::repair::RepairBuffer repairBuffer;
	sil::repair::CandidateConfig repairCandidateConfig;
	int repairLookaheadSamples = 1;
	float micropeakActivity = 0.f;
	float micropeakActivityAttackCoeff = 0.f;
	float micropeakActivityReleaseCoeff = 0.f;
	int micropeakActivityHoldSamples = 0;
	float micropeakActivityHoldLevel = 0.f;
	std::atomic<uint32_t> micropeakRepairCountL {0};
	std::atomic<uint32_t> micropeakRepairCountR {0};
	std::atomic<uint32_t> micropeakNearMissCount {0};
	std::ofstream micropeakDebugFile;
	std::string micropeakDebugPath;
	uint64_t micropeakDebugSequence = 0;
	double micropeakDebugStartSec = 0.0;
	std::atomic<bool> micropeakDebugActive {false};
	static constexpr uint32_t kMicropeakDebugQueueCapacity = 1024u;
	struct MicropeakDebugEvent {
		char eventType[16];
		float sampleRate = 0.f;
		int repairEnabled = 0;
		int masteringEnabled = 0;
		int lookaheadSamples = 0;
		int candidateL = 0;
		int candidateR = 0;
		float depthL = 0.f;
		float depthR = 0.f;
		float repairedL = 0.f;
		float repairedR = 0.f;
		sil::repair::StereoWindows windows;
		float l_peak = 0.f;
		float l_near = 0.f;
		float l_guard = 0.f;
		float l_localMean = 0.f;
		float l_neighborDrop = 0.f;
		float l_isolation = 0.f;
		float l_neighborRatio = 0.f;
		float l_neighborShare = 0.f;
		int l_passPeak = 0;
		int l_passDrop = 0;
		int l_passRatio = 0;
		int l_passShare = 0;
		int l_passIsolation = 0;
		int l_localProminent = 0;
		float r_peak = 0.f;
		float r_near = 0.f;
		float r_guard = 0.f;
		float r_localMean = 0.f;
		float r_neighborDrop = 0.f;
		float r_isolation = 0.f;
		float r_neighborRatio = 0.f;
		float r_neighborShare = 0.f;
		int r_passPeak = 0;
		int r_passDrop = 0;
		int r_passRatio = 0;
		int r_passShare = 0;
		int r_passIsolation = 0;
		int r_localProminent = 0;
	};
	std::array<MicropeakDebugEvent, kMicropeakDebugQueueCapacity> micropeakDebugQueue;
	std::atomic<uint32_t> micropeakDebugQueueWrite {0u};
	std::atomic<uint32_t> micropeakDebugQueueRead {0u};
	std::atomic<uint32_t> micropeakDebugDropped {0u};
	std::vector<float> rollingBufferL;
	std::vector<float> rollingBufferR;
	int rollingWriteIndex = 0;
	int rollingBufferLength = 0;
	int rollingFilled = 0;
	int rollingAudibleCount = 0;
	double rollingMonoSqSum = 0.0;
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
	float impactAirEnvAttackCoeff = 0.f;
	float impactAirEnvReleaseCoeff = 0.f;
	float impactAirSlowAttackCoeff = 0.f;
	float impactAirSlowReleaseCoeff = 0.f;
	float impactAirGainAttackCoeff = 0.f;
	float impactAirGainReleaseCoeff = 0.f;
	float mudEnvAttackCoeff = 0.f;
	float mudEnvReleaseCoeff = 0.f;
	float mudAttackCoeff = 0.f;
	float mudReleaseCoeff = 0.f;
	float glueRmsCoeff = 0.f;
	float glueAttackCoeff = 0.f;
	float glueReleaseCoeff = 0.f;
	float glueAdaptiveThresholdDb = -14.f;
	dsp::ClockDivider glueThresholdUpdateDivider;
	float midEnhanceEnvAttackCoeff = 0.f;
	float midEnhanceEnvReleaseCoeff = 0.f;
	float midEnhanceGainAttackCoeff = 0.f;
	float midEnhanceGainReleaseCoeff = 0.f;
	float stereoEnvAttackCoeff = 0.f;
	float stereoEnvReleaseCoeff = 0.f;
	float stereoMidGainAttackCoeff = 0.f;
	float stereoMidGainReleaseCoeff = 0.f;
	float stereoSideGainAttackCoeff = 0.f;
	float stereoSideGainReleaseCoeff = 0.f;
	float limiterAttackCoeff = 0.f;
	float limiterReleaseCoeff = 0.f;
	float limiterRepairAttackCoeff = 0.f;
	float limiterRepairReleaseCoeff = 0.f;
	float limiterCeiling = 0.f;
	float limiterMetricAttackCoeff = 0.f;
	float limiterMetricReleaseCoeff = 0.f;
	float limiterMetricGrCoeff = 0.f;
	float limiterTriggerEma = 0.f;
	float limiterRecentGrDb = 0.f;
	float limiterPrevSatL1 = 0.f;
	float limiterPrevSatL2 = 0.f;
	float limiterPrevSatR1 = 0.f;
	float limiterPrevSatR2 = 0.f;
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
	struct ImpactAirState {
		float env = 1e-6f;
		float slowEnv = 1e-6f;
		float targetLiftDb = 0.f;
		float smoothedLiftDb = 0.f;
		float ledAmount = 0.f;
		dsp::ClockDivider coeffDivider;
		Biquad shelfL;
		Biquad shelfR;
	} impactAir;
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
	struct MidrangeEnhanceState {
		dsp::RCFilter lowRefHp;
		dsp::RCFilter lowRefLp;
		dsp::RCFilter coreHp;
		dsp::RCFilter coreLp;
		dsp::RCFilter presenceHp;
		dsp::RCFilter presenceLp;
		float lowRefEnv = 1e-6f;
		float coreEnv = 1e-6f;
		float presenceEnv = 1e-6f;
		float targetLiftDb = 0.f;
		float smoothedLiftDb = 0.f;
		float activation = 0.f;
		float ledAmount = 0.f;
		dsp::ClockDivider coeffDivider;
		Biquad liftL;
		Biquad liftR;
	} midEnhance;
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
		uint8_t binValid[HISTORY_BINS] = {};
		int writeBin = 0;
		int validBinCount = 0;
		int samplesInBin = 0;
		int samplesPerBin = 441;
		float currentBinPeak = 0.f;
		float drive = 1.f;
		float makeupDb = 0.f;
		float makeupLinear = 1.f;
		float driveNormInv = 1.f;
		float tubeBiasOffset = 0.f;
		float ledAmount = 0.f;
		float limiterEngagement = 0.f;
		float limiterRecentGrDb = 0.f;
		dsp::ClockDivider updateDivider;

		void reset(float sampleRate) {
			samplesPerBin = std::max(1, int(std::round(sampleRate * kSatHistorySeconds / HISTORY_BINS)));
			std::fill(std::begin(peakBins), std::end(peakBins), 0.f);
			std::fill(std::begin(percentileHist), std::end(percentileHist), uint16_t(0));
			std::fill(std::begin(binToHist), std::end(binToHist), uint8_t(0));
			std::fill(std::begin(binValid), std::end(binValid), uint8_t(0));
			writeBin = 0;
			validBinCount = 0;
			samplesInBin = 0;
			currentBinPeak = 0.f;
			drive = 1.f;
			makeupDb = 0.f;
			makeupLinear = 1.f;
			driveNormInv = 1.f;
			tubeBiasOffset = 0.f;
			ledAmount = 0.f;
			limiterEngagement = 0.f;
			limiterRecentGrDb = 0.f;
			updateDivider.setDivision(512);
		}
	} saturator;

	struct MicropeakDebugFeatures {
		float peak = 0.f;
		float near = 0.f;
		float guard = 0.f;
		float localMean = 0.f;
		float neighborDrop = 0.f;
		float isolation = 0.f;
		float neighborRatio = 0.f;
		float neighborShare = 0.f;
		bool passPeak = false;
		bool passDrop = false;
		bool passRatio = false;
		bool passShare = false;
		bool passIsolation = false;
		bool localProminent = false;
	};

	static constexpr float kLowBandCutoffHz = 120.f;
	static constexpr float kLowBandCorrTauSec = 0.100f;
	static constexpr float kLowBandSideAttackSec = 0.050f;
	static constexpr float kLowBandSideReleaseSec = 0.250f;
	static constexpr float kAudioFullScaleV = 5.f;
	static constexpr float kRollingBufferSeconds = 10.f;
	static constexpr float kAdaptiveSilenceVolts = 0.0012559432f;
	static constexpr float kMicropeakActivityAttackSec = 0.015f;
	static constexpr float kMicropeakActivityReleaseSec = 0.120f;
	static constexpr float kMicropeakActivityHoldSec = 0.120f;
	static constexpr float kMicropeakActivityHoldBrightness = 0.60f;
	static constexpr float kMicropeakActivityRepeatBoost = 0.20f;
	static constexpr float kMicropeakDebugNearPeakVolts = 1.0f;
	static constexpr float kMicropeakDebugNearDropVolts = 0.20f;
	static constexpr float kMicropeakDebugNearIsolationRatio = 1.8f;
	static constexpr int kMicropeakHistorySamples = 2;
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
	static constexpr float kImpactAirMaxLiftDb = 0.75f;
	static constexpr float kImpactAirShelfHz = 1000.f;
	static constexpr float kImpactAirShelfQ = 0.707f;
	static constexpr float kImpactAirEnvAttackSec = 0.004f;
	static constexpr float kImpactAirEnvReleaseSec = 0.070f;
	static constexpr float kImpactAirSlowAttackSec = 0.120f;
	static constexpr float kImpactAirSlowReleaseSec = 0.450f;
	static constexpr float kImpactAirGainAttackSec = 0.010f;
	static constexpr float kImpactAirGainReleaseSec = 0.090f;
	static constexpr float kImpactAirSlowFloorVolts = 0.015f;
	static constexpr float kImpactAirTransientThresholdDb = 3.5f;
	static constexpr float kImpactAirTransientKneeDb = 3.0f;
	static constexpr int kImpactAirCoeffDivision = 32;
	static constexpr float kGlueRatio = 1.5f;
	static constexpr float kGlueAttackSec = 0.030f;
	static constexpr float kGlueReleaseSec = 0.250f;
	static constexpr float kGlueKneeDb = 8.f;
	static constexpr float kGlueThresholdDb = -14.f;
	static constexpr float kGlueAdaptiveOffsetDb = 6.f;
	static constexpr float kGlueAdaptiveMinThresholdDb = -24.f;
	static constexpr float kGlueAdaptiveMaxThresholdDb = -8.f;
	static constexpr int kGlueThresholdUpdateDivision = 1024;
	static constexpr float kGlueMaxGainReductionDb = 3.f;
	static constexpr float kGlueMaxMakeupDb = 2.0f;
	static constexpr float kGlueMakeupFraction = 0.75f;
	static constexpr float kGlueSidechainHpHz = 90.f;
	static constexpr float kMidEnhanceLowRefLowHz = 140.f;
	static constexpr float kMidEnhanceLowRefHighHz = 560.f;
	static constexpr float kMidEnhanceCoreLowHz = 700.f;
	static constexpr float kMidEnhanceCoreHighHz = 2400.f;
	static constexpr float kMidEnhancePresenceLowHz = 2600.f;
	static constexpr float kMidEnhancePresenceHighHz = 6500.f;
	static constexpr float kMidEnhanceCenterHz = 1450.f;
	static constexpr float kMidEnhanceQ = 0.72f;
	static constexpr float kMidEnhanceMaxLiftDb = 0.85f;
	static constexpr float kMidEnhanceGateDbFs = -50.f;
	static constexpr float kMidEnhanceGateKneeDb = 12.f;
	static constexpr float kMidEnhanceDeficitThresholdDb = 1.15f;
	static constexpr float kMidEnhanceDeficitKneeDb = 4.50f;
	static constexpr float kMidEnhanceRefBiasDb = 0.75f;
	static constexpr float kMidEnhanceRemoveMudAssistDb = 0.35f;
	static constexpr float kMidEnhancePresenceNormDb = 1.75f;
	static constexpr float kMidEnhancePresenceGuardThresholdDb = 2.50f;
	static constexpr float kMidEnhancePresenceGuardKneeDb = 5.00f;
	static constexpr float kMidEnhanceLimiterBackoffStartDb = 0.75f;
	static constexpr float kMidEnhanceLimiterBackoffKneeDb = 1.25f;
	static constexpr float kMidEnhanceEnvAttackSec = 0.050f;
	static constexpr float kMidEnhanceEnvReleaseSec = 0.420f;
	static constexpr float kMidEnhanceGainAttackSec = 0.350f;
	static constexpr float kMidEnhanceGainReleaseSec = 1.250f;
	static constexpr int kMidEnhanceCoeffDivision = 64;
	static constexpr float kMidEnhanceLedDeadbandDb = 0.08f;
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
	static constexpr float kRepairLookaheadSeconds = 0.001f;
	static constexpr int kMaxLimiterLookaheadSamples = 512;
	static constexpr float kLimiterRepairKneeLeadDb = 0.75f;
	static constexpr float kLimiterRepairKneeDepthDb = 0.20f;
	static constexpr float kLimiterMetricAttackSec = 0.020f;
	static constexpr float kLimiterMetricReleaseSec = 0.750f;
	static constexpr float kLimiterMetricGrSec = 0.120f;
	static constexpr float kLimiterTriggerDb = 0.10f;
	static constexpr float kSaturatorTargetPreLimiterDb = -0.50f;
	static constexpr float kSatMaxMakeupDb = 3.25f;
	static constexpr float kSatMaxDrive = 1.60f;
	static constexpr float kSatMinDrive = 1.0f;
	static constexpr float kSatLimiterSeekBoostDb = 0.75f;
	static constexpr float kSatLimiterTargetEngagement = 0.25f;
	static constexpr float kSatLimiterGrBackoffStartDb = 1.25f;
	static constexpr float kSatLimiterGrBackoffKneeDb = 1.5f;
	static constexpr float kSatLimiterGrBackoffDb = 1.5f;
	static constexpr float kSatMakeupAttackSec = 0.90f;
	static constexpr float kSatMakeupReleaseSec = 2.80f;
	static constexpr float kSatDriveAttackSec = 1.20f;
	static constexpr float kSatDriveReleaseSec = 3.50f;
	static constexpr float kSatHistoryPercentile = 0.990f;
	static constexpr float kSatHistorySeconds = 8.f;
	static constexpr int kSatUpdateDivision = 512;
	static constexpr float kSatTubeBias = 0.055f;
	static constexpr float kSatTubeWet = 0.72f;
	static constexpr int kLightDivision = 32;

	static float toDbSafe(float v) {
		return 20.f * std::log10(std::max(v, 1e-7f));
	}
	static float toDbFsSafe(float volts) {
		return 20.f * std::log10(std::max(volts, 1e-7f) / kAudioFullScaleV);
	}
	static float hermiteCausal(float y0, float y1, float m0, float m1, float t) {
		const float t2 = t * t;
		const float t3 = t2 * t;
		const float h00 = 2.f * t3 - 3.f * t2 + 1.f;
		const float h10 = t3 - 2.f * t2 + t;
		const float h01 = -2.f * t3 + 3.f * t2;
		const float h11 = t3 - t2;
		return h00 * y0 + h10 * m0 + h01 * y1 + h11 * m1;
	}
	static float intersamplePeak4xCausal(float prev2, float prev1, float current) {
		const float m0 = prev1 - prev2;
		const float m1 = current - prev1;
		float peak = std::max(std::fabs(prev1), std::fabs(current));
		for (float t : {0.25f, 0.5f, 0.75f}) {
			peak = std::max(peak, std::fabs(hermiteCausal(prev1, current, m0, m1, t)));
		}
		return peak;
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

	// Fast atan approximation with small error; used in hot audio path.
	// Reference form:
	// atan(x) ~= x * (pi/4 + 0.273 * (1 - |x|)) for |x| <= 1,
	// and range-reduced for |x| > 1.
	static float fastAtanApprox(float x) {
		const float ax = std::fabs(x);
		if (ax <= 1.f) {
			return x * (0.78539816339f + 0.273f * (1.f - ax));
		}
		const float inv = 1.f / ax;
		const float t = inv * (0.78539816339f + 0.273f * (1.f - inv));
		return (x >= 0.f) ? (1.57079632679f - t) : (-1.57079632679f + t);
	}

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
		rollingAudibleCount = 0;
		rollingMonoSqSum = 0.0;
	}

	void pushRollingSample(float sampleL, float sampleR) {
		if (rollingBufferLength <= 0) {
			return;
		}
		const int idx = rollingWriteIndex;

		const float oldMono = 0.5f * (rollingBufferL[size_t(idx)] + rollingBufferR[size_t(idx)]);
		const bool oldAudible = std::fabs(oldMono) > kAdaptiveSilenceVolts;
		if (rollingFilled >= rollingBufferLength) {
			if (oldAudible) {
				rollingMonoSqSum -= double(oldMono) * double(oldMono);
				rollingAudibleCount = std::max(0, rollingAudibleCount - 1);
			}
		}

		rollingBufferL[size_t(idx)] = sampleL;
		rollingBufferR[size_t(idx)] = sampleR;

		const float newMono = 0.5f * (sampleL + sampleR);
		const bool newAudible = std::fabs(newMono) > kAdaptiveSilenceVolts;
		if (newAudible) {
			rollingMonoSqSum += double(newMono) * double(newMono);
			rollingAudibleCount++;
		}

		rollingWriteIndex++;
		if (rollingWriteIndex >= rollingBufferLength) {
			rollingWriteIndex = 0;
		}
		if (rollingFilled < rollingBufferLength) {
			rollingFilled++;
		}
	}

	float estimateRollingProgramDbFs() const {
		if (rollingFilled <= 0 || rollingBufferLength <= 0 || rollingAudibleCount <= 0) {
			return -100.f;
		}
		const double meanSq = std::max(0.0, rollingMonoSqSum) / double(rollingAudibleCount);
		const float rmsVolts = std::sqrt(float(meanSq));
		return toDbFsSafe(rmsVolts);
	}

	void updateSpectrumBinMap(float sampleRate) {
		const float sr = std::max(sampleRate, 1.f);
		const float binHz = sr / float(FFT_SIZE);
		for (int i = 0; i < SPEC_FREQ_BINS; ++i) {
			const float f01 = float(i) / float(SPEC_FREQ_BINS - 1);
			const float hz = 20.f * std::pow(1000.f, f01);
			const float bin = hz / binHz;
			const int idx = clamp(int(bin), 0, FFT_SIZE / 2);
			specBinMap[i].idx = idx;
			specBinMap[i].frac = clamp(bin - float(idx), 0.f, 1.f);
		}
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
		impactAirEnvAttackCoeff = std::exp(-1.f / (kImpactAirEnvAttackSec * sr));
		impactAirEnvReleaseCoeff = std::exp(-1.f / (kImpactAirEnvReleaseSec * sr));
		impactAirSlowAttackCoeff = std::exp(-1.f / (kImpactAirSlowAttackSec * sr));
		impactAirSlowReleaseCoeff = std::exp(-1.f / (kImpactAirSlowReleaseSec * sr));
		impactAirGainAttackCoeff = std::exp(-1.f / (kImpactAirGainAttackSec * sr));
		impactAirGainReleaseCoeff = std::exp(-1.f / (kImpactAirGainReleaseSec * sr));
		mudEnvAttackCoeff = std::exp(-1.f / (kMudEnvAttackSec * sr));
		mudEnvReleaseCoeff = std::exp(-1.f / (kMudEnvReleaseSec * sr));
		mudAttackCoeff = std::exp(-1.f / (kMudAttackSec * sr));
		mudReleaseCoeff = std::exp(-1.f / (kMudReleaseSec * sr));
		glueRmsCoeff = std::exp(-1.f / (0.050f * sr));
		glueAttackCoeff = std::exp(-1.f / (kGlueAttackSec * sr));
		glueReleaseCoeff = std::exp(-1.f / (kGlueReleaseSec * sr));
		midEnhanceEnvAttackCoeff = std::exp(-1.f / (kMidEnhanceEnvAttackSec * sr));
		midEnhanceEnvReleaseCoeff = std::exp(-1.f / (kMidEnhanceEnvReleaseSec * sr));
		midEnhanceGainAttackCoeff = std::exp(-1.f / (kMidEnhanceGainAttackSec * sr));
		midEnhanceGainReleaseCoeff = std::exp(-1.f / (kMidEnhanceGainReleaseSec * sr));
		stereoEnvAttackCoeff = std::exp(-1.f / (kStereoEnvAttackSec * sr));
		stereoEnvReleaseCoeff = std::exp(-1.f / (kStereoEnvReleaseSec * sr));
		stereoMidGainAttackCoeff = std::exp(-1.f / (kStereoMidGainAttackSec * sr));
		stereoMidGainReleaseCoeff = std::exp(-1.f / (kStereoMidGainReleaseSec * sr));
		stereoSideGainAttackCoeff = std::exp(-1.f / (kStereoSideGainAttackSec * sr));
		stereoSideGainReleaseCoeff = std::exp(-1.f / (kStereoSideGainReleaseSec * sr));
		limiterAttackCoeff = std::exp(-1.f / (0.0005f * sr));
		limiterReleaseCoeff = std::exp(-1.f / (0.080f * sr));
		limiterRepairAttackCoeff = std::exp(-1.f / (0.00015f * sr));
		limiterRepairReleaseCoeff = std::exp(-1.f / (0.120f * sr));
		limiterCeiling = kAudioFullScaleV * std::pow(10.f, kLimiterCeilingDb / 20.f);
		limiterMetricAttackCoeff = std::exp(-1.f / (kLimiterMetricAttackSec * sr));
		limiterMetricReleaseCoeff = std::exp(-1.f / (kLimiterMetricReleaseSec * sr));
		limiterMetricGrCoeff = std::exp(-1.f / (kLimiterMetricGrSec * sr));
		micropeakActivityAttackCoeff = std::exp(-1.f / (kMicropeakActivityAttackSec * sr));
		micropeakActivityReleaseCoeff = std::exp(-1.f / (kMicropeakActivityReleaseSec * sr));
	}

	int repairLookaheadSamplesForRate(float sampleRate) const {
		int lookahead = std::max(1, int(std::round(std::max(sampleRate, 1.f) * kRepairLookaheadSeconds)));
		return clamp(lookahead, 1, kMaxLimiterLookaheadSamples);
	}

	void initializeRepairPathStorage(float sampleRate) {
		repairBuffer.configure(kMaxLimiterLookaheadSamples, kMicropeakHistorySamples);
		repairLookaheadSamples = 0;
		repairBuffer.setLookaheadSamples(0);
		const int maxBypassBufferLength = (kMaxLimiterLookaheadSamples + 1) + 1;
		bypassDelayL.assign(size_t(maxBypassBufferLength), 0.f);
		bypassDelayR.assign(size_t(maxBypassBufferLength), 0.f);
		bypassDelayWrite = 0;
		micropeakActivity = 0.f;
		micropeakActivityHoldSamples = 0;
		micropeakActivityHoldLevel = 0.f;
		(void)sampleRate;
	}

	void applyRepairLatencyModeNoAlloc(float sampleRate, bool repairMode) {
		int requestedLatency = repairMode ? repairLookaheadSamplesForRate(sampleRate) : 0;
		if (requestedLatency == repairLookaheadSamples) {
			return;
		}
		repairLookaheadSamples = requestedLatency;
		repairBuffer.setLookaheadSamples(repairLookaheadSamples);
		std::fill(bypassDelayL.begin(), bypassDelayL.end(), 0.f);
		std::fill(bypassDelayR.begin(), bypassDelayR.end(), 0.f);
		bypassDelayWrite = 0;
		micropeakActivity = 0.f;
		micropeakActivityHoldSamples = 0;
		micropeakActivityHoldLevel = 0.f;
	}

	bool repairFeatureAvailable() const {
		return isDragonKingDebugEnabled();
	}

	void initializeLimiterFastPathStorage() {
		const int fastLatency = 1;
		const int delayBufferLength = fastLatency + 1;
		limiterLatencySamples = fastLatency;
		limiterDelayL.assign(size_t(delayBufferLength), 0.f);
		limiterDelayR.assign(size_t(delayBufferLength), 0.f);
		limiterDelayWrite = 0;
		limiterGain = 1.f;
		limiterPrevSatL1 = 0.f;
		limiterPrevSatL2 = 0.f;
		limiterPrevSatR1 = 0.f;
		limiterPrevSatR2 = 0.f;
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
		const bool newValid = peak > kAdaptiveSilenceVolts;
		const bool oldValid = saturator.binValid[saturator.writeBin] != 0;
		const int oldHist = int(saturator.binToHist[saturator.writeBin]);
		if (oldValid && oldHist >= 0 && oldHist < SaturatorState::PERCENTILE_BINS && saturator.percentileHist[oldHist] > 0) {
			saturator.percentileHist[oldHist]--;
		}
		if (oldValid && !newValid) {
			saturator.validBinCount = std::max(0, saturator.validBinCount - 1);
		}
		int newHist = 0;
		if (newValid) {
			newHist = saturatorPeakToHistIndex(peak);
			saturator.percentileHist[newHist]++;
			if (!oldValid) {
				saturator.validBinCount = std::min(SaturatorState::HISTORY_BINS, saturator.validBinCount + 1);
			}
		}
		saturator.binValid[saturator.writeBin] = newValid ? uint8_t(1) : uint8_t(0);
		saturator.binToHist[saturator.writeBin] = uint8_t(newHist);
		saturator.peakBins[saturator.writeBin] = peak;
		saturator.writeBin++;
		if (saturator.writeBin >= SaturatorState::HISTORY_BINS) {
			saturator.writeBin = 0;
		}
	}

	float saturatorEstimateRecentPeakPercentile() const {
		if (saturator.validBinCount <= 0) {
			return 1e-6f;
		}
		const int targetRank = clamp(
			int(std::round(kSatHistoryPercentile * float(saturator.validBinCount - 1))),
			0,
			saturator.validBinCount - 1
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
	void updateMidEnhanceCutoffs(float sampleRate) {
		const auto norm = [&](float hz) {
			return clamp(hz / std::max(sampleRate, 1.f), 1e-5f, 0.49f);
		};
		midEnhance.lowRefHp.setCutoff(norm(kMidEnhanceLowRefLowHz));
		midEnhance.lowRefLp.setCutoff(norm(kMidEnhanceLowRefHighHz));
		midEnhance.coreHp.setCutoff(norm(kMidEnhanceCoreLowHz));
		midEnhance.coreLp.setCutoff(norm(kMidEnhanceCoreHighHz));
		midEnhance.presenceHp.setCutoff(norm(kMidEnhancePresenceLowHz));
		midEnhance.presenceLp.setCutoff(norm(kMidEnhancePresenceHighHz));
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
	void resetImpactAirState() {
		impactAir.env = 1e-6f;
		impactAir.slowEnv = 1e-6f;
		impactAir.targetLiftDb = 0.f;
		impactAir.smoothedLiftDb = 0.f;
		impactAir.ledAmount = 0.f;
		impactAir.shelfL.reset();
		impactAir.shelfR.reset();
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
	void resetMidEnhanceState() {
		midEnhance.lowRefEnv = 1e-6f;
		midEnhance.coreEnv = 1e-6f;
		midEnhance.presenceEnv = 1e-6f;
		midEnhance.targetLiftDb = 0.f;
		midEnhance.smoothedLiftDb = 0.f;
		midEnhance.activation = 0.f;
		midEnhance.ledAmount = 0.f;
		midEnhance.lowRefHp.reset();
		midEnhance.lowRefLp.reset();
		midEnhance.coreHp.reset();
		midEnhance.coreLp.reset();
		midEnhance.presenceHp.reset();
		midEnhance.presenceLp.reset();
		midEnhance.liftL.reset();
		midEnhance.liftR.reset();
	}

	Sil() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(MASTERING_ENABLED_PARAM, 0.f, 1.f, 1.f, "Mastering", {"Disabled", "Enabled"});
		configSwitch(REPAIR_ENABLED_PARAM, 0.f, 1.f, 1.f, "Repair", {"Disabled", "Enabled"});
		configInput(INPUT_L_INPUT, "Left");
		configInput(INPUT_R_INPUT, "Right");
		configOutput(OUTPUT_L_OUTPUT, "Left");
		configOutput(OUTPUT_R_OUTPUT, "Right");

		const float initialSampleRate = (APP && APP->engine) ? APP->engine->getSampleRate() : 44100.f;
		hist.samplesPerBin = (int)(initialSampleRate * HISTOGRAM_DURATION / HISTOGRAM_BINS);
		if (hist.samplesPerBin < 1) hist.samplesPerBin = 1;
		
		spec.fft = new dsp::RealFFT(FFT_SIZE);
		for (int i = 0; i < FFT_SIZE; i++) {
			spec.window[i] = 0.5f - 0.5f * std::cos(2.f * M_PI * i / (FFT_SIZE - 1));
		}

		// Keep spectrum visually responsive; this is UI-only work.
		specDivider.setDivision(2048);
		lightDivider.setDivision(kLightDivision);
		glueThresholdUpdateDivider.setDivision(kGlueThresholdUpdateDivision);
		impactAir.coeffDivider.setDivision(kImpactAirCoeffDivision);
		removeMud.coeffDivider.setDivision(32);
		midEnhance.coeffDivider.setDivision(kMidEnhanceCoeffDivision);
		stereoEnhance.coeffDivider.setDivision(kStereoEnhanceCoeffDivision);
		saturator.updateDivider.setDivision(kSatUpdateDivision);
		updateLowBandCutoff(initialSampleRate);
		updateRemoveMudCutoffs(initialSampleRate);
		updateGlueCutoff(initialSampleRate);
		updateMidEnhanceCutoffs(initialSampleRate);
		updateStereoEnhanceCutoffs(initialSampleRate);
		updateDynamicsCoefficients(initialSampleRate);
		updateSpectrumBinMap(initialSampleRate);
		glueAdaptiveThresholdDb = kGlueThresholdDb;
		configureRollingBuffer(initialSampleRate);
		saturator.reset(initialSampleRate);
		impactAir.shelfL.setHighShelf(initialSampleRate, kImpactAirShelfHz, kImpactAirShelfQ, 0.f);
		impactAir.shelfR.setHighShelf(initialSampleRate, kImpactAirShelfHz, kImpactAirShelfQ, 0.f);
		removeMud.peakingL.setPeaking(initialSampleRate, kMudCenterHz, kMudQ, 0.f);
		removeMud.peakingR.setPeaking(initialSampleRate, kMudCenterHz, kMudQ, 0.f);
		midEnhance.liftL.setPeaking(initialSampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
		midEnhance.liftR.setPeaking(initialSampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
		stereoEnhance.midEq.setPeaking(initialSampleRate, kStereoMidCenterHz, kStereoMidQ, 0.f);
		stereoEnhance.sideEq.setPeaking(initialSampleRate, kStereoSideCenterHz, kStereoSideQ, 0.f);
		initializeLimiterFastPathStorage();
		initializeRepairPathStorage(initialSampleRate);
		applyRepairLatencyModeNoAlloc(initialSampleRate, repairEnabled);
	}

	~Sil() {
		stopMicropeakDebugCapture();
		delete spec.fft;
	}

	static std::string userDebugRootPath() {
		return system::join(asset::user(), "Leviathan/Sil");
	}

	bool isMicropeakDebugCaptureActive() const {
		return micropeakDebugActive.load(std::memory_order_relaxed);
	}

	std::string getMicropeakDebugPath() const {
		return micropeakDebugPath;
	}

	void startMicropeakDebugCapture() {
		if (!isDragonKingDebugEnabled()) {
			return;
		}
		if (micropeakDebugActive.load(std::memory_order_relaxed)) {
			return;
		}

		system::createDirectories(userDebugRootPath());
		micropeakDebugPath = system::join(userDebugRootPath(), "micropeak_debug_" + std::to_string(std::time(nullptr)) + ".csv");
		micropeakDebugFile.open(micropeakDebugPath);
		if (!micropeakDebugFile.is_open()) {
			WARN("Sil failed to open micropeak debug CSV: %s", micropeakDebugPath.c_str());
			micropeakDebugPath.clear();
			return;
		}

		micropeakDebugFile << std::setprecision(9);
		micropeakDebugFile
			<< "sequence,time_sec,event_type,sample_rate,repair_enabled,mastering_enabled,lookahead_samples,"
			<< "candidate_l,candidate_r,depth_l,depth_r,repaired_l,repaired_r,"
			<< "l_prev2,l_prev1,l_center,l_next1,l_next2,"
			<< "r_prev2,r_prev1,r_center,r_next1,r_next2,"
			<< "l_peak,l_near,l_guard,l_local_mean,l_neighbor_drop,l_isolation,l_neighbor_ratio,l_neighbor_share,"
			<< "l_pass_peak,l_pass_drop,l_pass_ratio,l_pass_share,l_pass_isolation,l_local_prominent,"
			<< "r_peak,r_near,r_guard,r_local_mean,r_neighbor_drop,r_isolation,r_neighbor_ratio,r_neighbor_share,"
			<< "r_pass_peak,r_pass_drop,r_pass_ratio,r_pass_share,r_pass_isolation,r_local_prominent\n";
		micropeakDebugStartSec = system::getTime();
		micropeakDebugSequence = 0;
		micropeakRepairCountL.store(0u, std::memory_order_relaxed);
		micropeakRepairCountR.store(0u, std::memory_order_relaxed);
		micropeakNearMissCount.store(0u, std::memory_order_relaxed);
		micropeakDebugQueueRead.store(0u, std::memory_order_relaxed);
		micropeakDebugQueueWrite.store(0u, std::memory_order_relaxed);
		micropeakDebugDropped.store(0u, std::memory_order_relaxed);
		micropeakDebugActive.store(true, std::memory_order_relaxed);
		DEBUG("Sil started micropeak debug CSV: %s", micropeakDebugPath.c_str());
	}

	void stopMicropeakDebugCapture() {
		drainMicropeakDebugQueueToCsv();
		if (micropeakDebugFile.is_open()) {
			micropeakDebugFile.close();
		}
		if (micropeakDebugActive.load(std::memory_order_relaxed)) {
			DEBUG("Sil stopped micropeak debug CSV: %s", micropeakDebugPath.c_str());
		}
		micropeakDebugActive.store(false, std::memory_order_relaxed);
	}

	bool popMicropeakDebugEvent(MicropeakDebugEvent* outEvent) {
		if (!outEvent) {
			return false;
		}
		uint32_t read = micropeakDebugQueueRead.load(std::memory_order_relaxed);
		uint32_t write = micropeakDebugQueueWrite.load(std::memory_order_acquire);
		if (read == write) {
			return false;
		}
		*outEvent = micropeakDebugQueue[size_t(read)];
		micropeakDebugQueueRead.store((read + 1u) % kMicropeakDebugQueueCapacity, std::memory_order_release);
		return true;
	}

	uint32_t consumeMicropeakDebugDroppedCount() {
		return micropeakDebugDropped.exchange(0u, std::memory_order_acq_rel);
	}

	void toggleMicropeakDebugCapture() {
		if (!isDragonKingDebugEnabled()) {
			stopMicropeakDebugCapture();
			return;
		}
		if (isMicropeakDebugCaptureActive()) {
			stopMicropeakDebugCapture();
		}
		else {
			startMicropeakDebugCapture();
		}
	}

	void drainMicropeakDebugQueueToCsv() {
		if (!micropeakDebugActive.load(std::memory_order_relaxed) || !micropeakDebugFile.is_open()) {
			return;
		}
		const uint32_t dropped = consumeMicropeakDebugDroppedCount();
		if (dropped > 0u) {
			WARN("Sil micropeak debug dropped %u events (queue full)", dropped);
		}
		MicropeakDebugEvent event;
		while (popMicropeakDebugEvent(&event)) {
			const double timeSec = system::getTime() - micropeakDebugStartSec;
			micropeakDebugFile
				<< micropeakDebugSequence++ << ','
				<< timeSec << ','
				<< event.eventType << ','
				<< event.sampleRate << ','
				<< event.repairEnabled << ','
				<< event.masteringEnabled << ','
				<< event.lookaheadSamples << ','
				<< event.candidateL << ','
				<< event.candidateR << ','
				<< event.depthL << ','
				<< event.depthR << ','
				<< event.repairedL << ','
				<< event.repairedR << ','
				<< event.windows.left.prev2 << ','
				<< event.windows.left.prev1 << ','
				<< event.windows.left.center << ','
				<< event.windows.left.next1 << ','
				<< event.windows.left.next2 << ','
				<< event.windows.right.prev2 << ','
				<< event.windows.right.prev1 << ','
				<< event.windows.right.center << ','
				<< event.windows.right.next1 << ','
				<< event.windows.right.next2 << ','
				<< event.l_peak << ','
				<< event.l_near << ','
				<< event.l_guard << ','
				<< event.l_localMean << ','
				<< event.l_neighborDrop << ','
				<< event.l_isolation << ','
				<< event.l_neighborRatio << ','
				<< event.l_neighborShare << ','
				<< event.l_passPeak << ','
				<< event.l_passDrop << ','
				<< event.l_passRatio << ','
				<< event.l_passShare << ','
				<< event.l_passIsolation << ','
				<< event.l_localProminent << ','
				<< event.r_peak << ','
				<< event.r_near << ','
				<< event.r_guard << ','
				<< event.r_localMean << ','
				<< event.r_neighborDrop << ','
				<< event.r_isolation << ','
				<< event.r_neighborRatio << ','
				<< event.r_neighborShare << ','
				<< event.r_passPeak << ','
				<< event.r_passDrop << ','
				<< event.r_passRatio << ','
				<< event.r_passShare << ','
				<< event.r_passIsolation << ','
				<< event.r_localProminent << '\n';
		}
	}

	MicropeakDebugFeatures measureMicropeakDebugFeatures(const sil::repair::Window5& w) const {
		MicropeakDebugFeatures f;
		const float absPrev2 = std::fabs(w.prev2);
		const float absPrev1 = std::fabs(w.prev1);
		const float absCenter = std::fabs(w.center);
		const float absNext1 = std::fabs(w.next1);
		const float absNext2 = std::fabs(w.next2);
		const float maxNeighbor = std::max(absPrev1, absNext1);
		const float maxSurround = std::max(maxNeighbor, std::max(absPrev2, absNext2));
		const float minPeak = repairCandidateConfig.minPeakFullScale * kAudioFullScaleV;
		const float minDrop = repairCandidateConfig.minNeighborDropFullScale * kAudioFullScaleV;

		f.peak = absCenter;
		f.near = maxNeighbor;
		f.guard = std::max(absPrev2, absNext2);
		f.localMean = 0.25f * (absPrev2 + absPrev1 + absNext1 + absNext2);
		f.neighborDrop = absCenter - maxNeighbor;
		f.isolation = absCenter / std::max(f.localMean, 1e-6f);
		f.neighborRatio = absCenter / std::max(maxNeighbor, 1e-6f);
		f.neighborShare = maxSurround / std::max(absCenter, 1e-6f);
		f.passPeak = f.peak >= minPeak;
		f.passDrop = f.neighborDrop >= minDrop;
		f.passRatio = f.peak >= f.near * repairCandidateConfig.minNeighborRatio;
		f.passShare = f.near <= f.peak * repairCandidateConfig.maxNeighborShare
			&& f.guard <= f.peak * repairCandidateConfig.maxNeighborShare;
		f.passIsolation = f.isolation >= repairCandidateConfig.minIsolationRatio;
		f.localProminent = f.peak > f.near;
		return f;
	}

	bool shouldLogMicropeakNearMiss(const MicropeakDebugFeatures& f, bool candidate) const {
		if (candidate) {
			return false;
		}
		if (!f.localProminent || f.peak < kMicropeakDebugNearPeakVolts) {
			return false;
		}
		return f.neighborDrop >= kMicropeakDebugNearDropVolts
			|| f.isolation >= kMicropeakDebugNearIsolationRatio;
	}

	void enqueueMicropeakDebugEvent(
		float sampleRate,
		const char* eventType,
		const sil::repair::StereoWindows& windows,
		const sil::repair::RepairDecision& repairL,
		const sil::repair::RepairDecision& repairR,
		bool candidateL,
		bool candidateR,
		const MicropeakDebugFeatures& featureL,
		const MicropeakDebugFeatures& featureR
	) {
		if (!isDragonKingDebugEnabled()) {
			return;
		}
		if (!micropeakDebugActive.load(std::memory_order_relaxed)) {
			return;
		}
		MicropeakDebugEvent event;
		std::snprintf(event.eventType, sizeof(event.eventType), "%s", eventType ? eventType : "event");
		event.sampleRate = sampleRate;
		event.repairEnabled = repairEnabled ? 1 : 0;
		event.masteringEnabled = masteringEnabled ? 1 : 0;
		event.lookaheadSamples = repairLookaheadSamples;
		event.candidateL = candidateL ? 1 : 0;
		event.candidateR = candidateR ? 1 : 0;
		event.depthL = repairL.depth;
		event.depthR = repairR.depth;
		event.repairedL = repairL.repaired;
		event.repairedR = repairR.repaired;
		event.windows = windows;
		event.l_peak = featureL.peak;
		event.l_near = featureL.near;
		event.l_guard = featureL.guard;
		event.l_localMean = featureL.localMean;
		event.l_neighborDrop = featureL.neighborDrop;
		event.l_isolation = featureL.isolation;
		event.l_neighborRatio = featureL.neighborRatio;
		event.l_neighborShare = featureL.neighborShare;
		event.l_passPeak = featureL.passPeak ? 1 : 0;
		event.l_passDrop = featureL.passDrop ? 1 : 0;
		event.l_passRatio = featureL.passRatio ? 1 : 0;
		event.l_passShare = featureL.passShare ? 1 : 0;
		event.l_passIsolation = featureL.passIsolation ? 1 : 0;
		event.l_localProminent = featureL.localProminent ? 1 : 0;
		event.r_peak = featureR.peak;
		event.r_near = featureR.near;
		event.r_guard = featureR.guard;
		event.r_localMean = featureR.localMean;
		event.r_neighborDrop = featureR.neighborDrop;
		event.r_isolation = featureR.isolation;
		event.r_neighborRatio = featureR.neighborRatio;
		event.r_neighborShare = featureR.neighborShare;
		event.r_passPeak = featureR.passPeak ? 1 : 0;
		event.r_passDrop = featureR.passDrop ? 1 : 0;
		event.r_passRatio = featureR.passRatio ? 1 : 0;
		event.r_passShare = featureR.passShare ? 1 : 0;
		event.r_passIsolation = featureR.passIsolation ? 1 : 0;
		event.r_localProminent = featureR.localProminent ? 1 : 0;
		uint32_t write = micropeakDebugQueueWrite.load(std::memory_order_relaxed);
		uint32_t read = micropeakDebugQueueRead.load(std::memory_order_acquire);
		uint32_t next = (write + 1u) % kMicropeakDebugQueueCapacity;
		if (next == read) {
			micropeakDebugDropped.fetch_add(1u, std::memory_order_relaxed);
			return;
		}
		micropeakDebugQueue[size_t(write)] = event;
		micropeakDebugQueueWrite.store(next, std::memory_order_release);
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		hist.samplesPerBin = (int)(e.sampleRate * HISTOGRAM_DURATION / HISTOGRAM_BINS);
		if (hist.samplesPerBin < 1) hist.samplesPerBin = 1;
		updateLowBandCutoff(e.sampleRate);
		updateRemoveMudCutoffs(e.sampleRate);
		updateGlueCutoff(e.sampleRate);
		updateMidEnhanceCutoffs(e.sampleRate);
		updateStereoEnhanceCutoffs(e.sampleRate);
		updateDynamicsCoefficients(e.sampleRate);
		updateSpectrumBinMap(e.sampleRate);
		glueAdaptiveThresholdDb = kGlueThresholdDb;
		configureRollingBuffer(e.sampleRate);
		resetRemoveMudState();
		resetImpactAirState();
		resetMidEnhanceState();
		resetStereoEnhanceState();
		impactAir.shelfL.setHighShelf(e.sampleRate, kImpactAirShelfHz, kImpactAirShelfQ, 0.f);
		impactAir.shelfR.setHighShelf(e.sampleRate, kImpactAirShelfHz, kImpactAirShelfQ, 0.f);
		midEnhance.liftL.setPeaking(e.sampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
		midEnhance.liftR.setPeaking(e.sampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, 0.f);
		stereoEnhance.midEq.setPeaking(e.sampleRate, kStereoMidCenterHz, kStereoMidQ, 0.f);
		stereoEnhance.sideEq.setPeaking(e.sampleRate, kStereoSideCenterHz, kStereoSideQ, 0.f);
		glue.reset();
		saturator.reset(e.sampleRate);
		initializeLimiterFastPathStorage();
		initializeRepairPathStorage(e.sampleRate);
		const bool repairActive = repairFeatureAvailable() && repairEnabled;
		applyRepairLatencyModeNoAlloc(e.sampleRate, repairActive);
	}

	void process(const ProcessArgs& args) override {
		masteringEnabled = params[MASTERING_ENABLED_PARAM].getValue() > 0.5f;
		repairEnabled = params[REPAIR_ENABLED_PARAM].getValue() > 0.5f;
		const bool repairActive = repairFeatureAvailable() && repairEnabled;

		const float inL = inputs[INPUT_L_INPUT].getVoltage();
		const float inR = inputs[INPUT_R_INPUT].getVoltage();
		applyRepairLatencyModeNoAlloc(args.sampleRate, repairActive);

		float repairInputL = inL;
		float repairInputR = inR;
		float repairLedTarget = 0.f;
		if (repairActive) {
			repairBuffer.push(inL, inR);
			const sil::repair::StereoWindows windows = repairBuffer.readCurrentWindows();
			const bool candidateL = sil::repair::detectCandidate(windows.left, repairCandidateConfig, kAudioFullScaleV);
			const bool candidateR = sil::repair::detectCandidate(windows.right, repairCandidateConfig, kAudioFullScaleV);
			const sil::repair::RepairDecision repairL = sil::repair::repairCenterLinear(windows.left, candidateL);
			const sil::repair::RepairDecision repairR = sil::repair::repairCenterLinear(windows.right, candidateR);
			const bool debugCaptureActive = isDragonKingDebugEnabled()
				&& micropeakDebugActive.load(std::memory_order_relaxed);
			MicropeakDebugFeatures debugFeatureL;
			MicropeakDebugFeatures debugFeatureR;
			if (debugCaptureActive) {
				debugFeatureL = measureMicropeakDebugFeatures(windows.left);
				debugFeatureR = measureMicropeakDebugFeatures(windows.right);
			}
			const bool repairHit = candidateL || candidateR;
			if (repairHit) {
				if (debugCaptureActive) {
					enqueueMicropeakDebugEvent(args.sampleRate, "repair", windows, repairL, repairR, candidateL, candidateR, debugFeatureL, debugFeatureR);
				}
				if (candidateL) {
					micropeakRepairCountL.fetch_add(1u, std::memory_order_relaxed);
				}
				if (candidateR) {
					micropeakRepairCountR.fetch_add(1u, std::memory_order_relaxed);
				}
				const bool repeatedDuringHold = micropeakActivityHoldSamples > 0;
				micropeakActivityHoldSamples = std::max(1, int(std::round(args.sampleRate * kMicropeakActivityHoldSec)));
				const float depthL = repairL.depth;
				const float depthR = repairR.depth;
				const float eventLevel = clamp(std::max(std::max(depthL, depthR), kMicropeakActivityHoldBrightness), 0.f, 1.f);
				micropeakActivityHoldLevel = repeatedDuringHold
					? clamp(std::max(micropeakActivityHoldLevel, eventLevel) + kMicropeakActivityRepeatBoost, 0.f, 1.f)
					: eventLevel;
				repairLedTarget = micropeakActivityHoldLevel;
				repairInputL = repairL.repaired;
				repairInputR = repairR.repaired;
			}
			else {
				const bool logNearMiss = debugCaptureActive
					&& (shouldLogMicropeakNearMiss(debugFeatureL, candidateL)
						|| shouldLogMicropeakNearMiss(debugFeatureR, candidateR));
				if (logNearMiss) {
					micropeakNearMissCount.fetch_add(1u, std::memory_order_relaxed);
					enqueueMicropeakDebugEvent(args.sampleRate, "near_miss", windows, repairL, repairR, candidateL, candidateR, debugFeatureL, debugFeatureR);
				}
				repairInputL = windows.left.center;
				repairInputR = windows.right.center;
			}
		}
		if (micropeakActivityHoldSamples > 0) {
			micropeakActivityHoldSamples--;
			repairLedTarget = std::max(repairLedTarget, micropeakActivityHoldLevel);
		}
		else {
			micropeakActivityHoldLevel = 0.f;
		}
		const float micropeakCoeff =
			(repairLedTarget > micropeakActivity) ? micropeakActivityAttackCoeff : micropeakActivityReleaseCoeff;
		micropeakActivity = repairLedTarget + micropeakCoeff * (micropeakActivity - repairLedTarget);

		const int bypassDelayLen = std::max(1, repairLookaheadSamples + 2);
		const int bypassReadIdx = (bypassDelayWrite + 1) % bypassDelayLen;
		const float delayedInL = bypassDelayL[size_t(bypassReadIdx)];
		const float delayedInR = bypassDelayR[size_t(bypassReadIdx)];
		bypassDelayL[size_t(bypassDelayWrite)] = inL;
		bypassDelayR[size_t(bypassDelayWrite)] = inR;
		bypassDelayWrite = bypassReadIdx;

		// Low-band mono recovery below 120 Hz:
		// preserve coherent bass stereo, progressively collapse risky low side content.
		lowpassL1.process(repairInputL);
		float lowL = lowpassL1.lowpass();
		lowpassL2.process(lowL);
		lowL = lowpassL2.lowpass();

		lowpassR1.process(repairInputR);
		float lowR = lowpassR1.lowpass();
		lowpassR2.process(lowR);
		lowR = lowpassR2.lowpass();

		const float highL = repairInputL - lowL;
		const float highR = repairInputR - lowR;
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
		float impactAirL = recoveredL;
		float impactAirR = recoveredR;
		float impactAirLed = 0.f;
		{
			const float detector = std::fabs(lowMid);
			const float envCoeff = (detector > impactAir.env) ? impactAirEnvAttackCoeff : impactAirEnvReleaseCoeff;
			impactAir.env = detector + envCoeff * (impactAir.env - detector);
			const float slowCoeff =
				(detector > impactAir.slowEnv) ? impactAirSlowAttackCoeff : impactAirSlowReleaseCoeff;
			impactAir.slowEnv = detector + slowCoeff * (impactAir.slowEnv - detector);

			const float transientDeltaDb = toDbSafe(impactAir.env / std::max(impactAir.slowEnv, kImpactAirSlowFloorVolts));
			const float transientGate = softKnee01(
				transientDeltaDb,
				kImpactAirTransientThresholdDb,
				kImpactAirTransientKneeDb
			);
			impactAir.targetLiftDb = kImpactAirMaxLiftDb * transientGate;
			const float gainCoeff =
				(impactAir.targetLiftDb > impactAir.smoothedLiftDb) ? impactAirGainAttackCoeff : impactAirGainReleaseCoeff;
			impactAir.smoothedLiftDb =
				impactAir.targetLiftDb + gainCoeff * (impactAir.smoothedLiftDb - impactAir.targetLiftDb);
			impactAir.ledAmount = clamp(impactAir.smoothedLiftDb / std::max(kImpactAirMaxLiftDb, 1e-6f), 0.f, 1.f);
			if (impactAir.coeffDivider.process()) {
				impactAir.shelfL.setHighShelf(
					args.sampleRate,
					kImpactAirShelfHz,
					kImpactAirShelfQ,
					impactAir.smoothedLiftDb
				);
				impactAir.shelfR.setHighShelf(
					args.sampleRate,
					kImpactAirShelfHz,
					kImpactAirShelfQ,
					impactAir.smoothedLiftDb
				);
			}
			impactAirL = impactAir.shelfL.process(recoveredL);
			impactAirR = impactAir.shelfR.process(recoveredR);
			impactAirLed = impactAir.ledAmount;
		}
		float removeMudLed = 0.f;
		float mudCleanL = impactAirL;
		float mudCleanR = impactAirR;
		const float mono = 0.5f * (impactAirL + impactAirR);
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
		mudCleanL = removeMud.peakingL.process(impactAirL);
		mudCleanR = removeMud.peakingR.process(impactAirR);
		removeMudLed = removeMud.ledAmount;

		float midEnhancedL = mudCleanL;
		float midEnhancedR = mudCleanR;
		float midEnhanceLed = 0.f;
		{
			const float monoPostMud = 0.5f * (mudCleanL + mudCleanR);
			midEnhance.lowRefHp.process(monoPostMud);
			const float lowRefHigh = midEnhance.lowRefHp.highpass();
			midEnhance.lowRefLp.process(lowRefHigh);
			const float lowRefBand = midEnhance.lowRefLp.lowpass();
			midEnhance.coreHp.process(monoPostMud);
			const float coreHigh = midEnhance.coreHp.highpass();
			midEnhance.coreLp.process(coreHigh);
			const float coreBand = midEnhance.coreLp.lowpass();
			midEnhance.presenceHp.process(monoPostMud);
			const float presenceHighBand = midEnhance.presenceHp.highpass();
			midEnhance.presenceLp.process(presenceHighBand);
			const float presenceBandMid = midEnhance.presenceLp.lowpass();
			auto updateMidEnhanceEnv = [&](float& env, float x) {
				const float absX = std::fabs(x);
				const float c = (absX > env) ? midEnhanceEnvAttackCoeff : midEnhanceEnvReleaseCoeff;
				env = absX + c * (env - absX);
			};
			updateMidEnhanceEnv(midEnhance.lowRefEnv, lowRefBand);
			updateMidEnhanceEnv(midEnhance.coreEnv, coreBand);
			updateMidEnhanceEnv(midEnhance.presenceEnv, presenceBandMid);
			const float refEnvMid = 0.68f * midEnhance.lowRefEnv + 0.32f * midEnhance.presenceEnv;
			const float activityEnv = std::max(midEnhance.coreEnv, refEnvMid);
			const float levelGate = softKnee01(toDbFsSafe(activityEnv), kMidEnhanceGateDbFs, kMidEnhanceGateKneeDb);
			const float thresholdDb = kMidEnhanceDeficitThresholdDb
				- kMidEnhanceRemoveMudAssistDb * clamp(removeMud.ledAmount, 0.f, 1.f);
			const float deficitDb = toDbSafe(refEnvMid / std::max(midEnhance.coreEnv, 1e-7f)) - kMidEnhanceRefBiasDb;
			const float deficit = softKnee01(deficitDb, thresholdDb, kMidEnhanceDeficitKneeDb);
			const float presenceRatioDb = toDbSafe(midEnhance.presenceEnv / std::max(midEnhance.coreEnv, 1e-7f))
				+ kMidEnhancePresenceNormDb;
			const float presenceGuard = inverseSoftKnee01(
				presenceRatioDb,
				kMidEnhancePresenceGuardThresholdDb,
				kMidEnhancePresenceGuardKneeDb
			);
			const float limiterBackoff = inverseSoftKnee01(
				limiterRecentGrDb,
				kMidEnhanceLimiterBackoffStartDb,
				kMidEnhanceLimiterBackoffKneeDb
			);
			midEnhance.activation = clamp(levelGate * deficit * presenceGuard * limiterBackoff, 0.f, 1.f);
			midEnhance.targetLiftDb = kMidEnhanceMaxLiftDb * midEnhance.activation;
			const float liftCoeff = (midEnhance.targetLiftDb > midEnhance.smoothedLiftDb)
				? midEnhanceGainAttackCoeff
				: midEnhanceGainReleaseCoeff;
			midEnhance.smoothedLiftDb =
				midEnhance.targetLiftDb + liftCoeff * (midEnhance.smoothedLiftDb - midEnhance.targetLiftDb);
			if (midEnhance.coeffDivider.process()) {
				midEnhance.liftL.setPeaking(args.sampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, midEnhance.smoothedLiftDb);
				midEnhance.liftR.setPeaking(args.sampleRate, kMidEnhanceCenterHz, kMidEnhanceQ, midEnhance.smoothedLiftDb);
			}
			midEnhancedL = midEnhance.liftL.process(mudCleanL);
			midEnhancedR = midEnhance.liftR.process(mudCleanR);
			const float activeDb = std::max(0.f, midEnhance.smoothedLiftDb - kMidEnhanceLedDeadbandDb);
			const float ledNorm = clamp(
				activeDb / std::max(kMidEnhanceMaxLiftDb - kMidEnhanceLedDeadbandDb, 1e-6f),
				0.f,
				1.f
			);
			midEnhance.ledAmount = std::sqrt(ledNorm);
			midEnhanceLed = midEnhance.ledAmount;
		}

		const float cleanedL = midEnhancedL;
		const float cleanedR = midEnhancedR;
		pushRollingSample(cleanedL, cleanedR);
		float gluedL = cleanedL;
		float gluedR = cleanedR;
		float glueLed = 0.f;
		if (glueThresholdUpdateDivider.process()) {
			const float programDbFs = estimateRollingProgramDbFs();
			const float targetThresholdDb = clamp(
				programDbFs - kGlueAdaptiveOffsetDb,
				kGlueAdaptiveMinThresholdDb,
				kGlueAdaptiveMaxThresholdDb
			);
			glueAdaptiveThresholdDb = targetThresholdDb;
		}
		const float glueMono = 0.5f * (cleanedL + cleanedR);
		glue.sidechainHp.process(glueMono);
		const float sidechain = glue.sidechainHp.highpass();
		const float rmsTarget = sidechain * sidechain;
		glue.rmsEnv = glueRmsCoeff * glue.rmsEnv + (1.f - glueRmsCoeff) * rmsTarget;
		const float levelDb = 20.f * std::log10(std::sqrt(std::max(glue.rmsEnv, 1e-12f)) / kAudioFullScaleV + 1e-9f);
		const float overDb = levelDb - glueAdaptiveThresholdDb;
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
		gluedL = cleanedL * gain;
		gluedR = cleanedR * gain;
		glue.ledAmount = clamp(glue.gainReductionDb / kGlueMaxGainReductionDb, 0.f, 1.f);
		glueLed = glue.ledAmount;
		float enhancedL = gluedL;
		float enhancedR = gluedR;
		float stereoEnhanceLed = 0.f;
		{
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
			const float midPresenceGate = softKnee01(mid350DbFs, kStereoMidGateDbFs, kStereoMidGateKneeDb);
			const float midExcessDb = toDbSafe(stereoEnhance.mid350Env / std::max(stereoEnhance.midBroadEnv, 1e-7f)) + kStereoMidBandNormDb;
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
			const float sideBrightnessDb = toDbSafe(stereoEnhance.side6kEnv / std::max(stereoEnhance.sideBroadEnv, 1e-7f)) + kStereoSideBandNormDb;
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

			// Diagnostic-style meter: take absolute movement per M/S leg first,
			// then combine contributions so M/S sign opposition cannot cancel the readout.
			const float midAbsDb = std::fabs(stereoEnhance.smoothedMidCutDb);
			const float sideAbsDb = std::fabs(stereoEnhance.smoothedSideLiftDb);
			const float leftDeltaDb = 0.5f * midAbsDb + 0.5f * sideAbsDb;
			const float rightDeltaDb = 0.5f * midAbsDb + 0.5f * sideAbsDb;
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

		float saturatedL = enhancedL;
		float saturatedR = enhancedR;
		float saturatorLed = 0.f;
		{
			// Expose recent limiter behavior to saturator adaptation.
			saturator.limiterEngagement = limiterTriggerEma;
			saturator.limiterRecentGrDb = limiterRecentGrDb;

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
				float desiredMakeupDb = clamp(
					kSaturatorTargetPreLimiterDb - recentPeakDb,
					0.f,
					kSatMaxMakeupDb
					);
				const float limiterUnderEngaged = 1.f - clamp(
					saturator.limiterEngagement / std::max(kSatLimiterTargetEngagement, 1e-6f),
					0.f,
					1.f
				);
				const float limiterOverworked = softKnee01(
					saturator.limiterRecentGrDb,
					kSatLimiterGrBackoffStartDb,
					kSatLimiterGrBackoffKneeDb
				);
				desiredMakeupDb = clamp(
					desiredMakeupDb +
						kSatLimiterSeekBoostDb * limiterUnderEngaged -
						kSatLimiterGrBackoffDb * limiterOverworked,
					0.f,
					kSatMaxMakeupDb
				);
				const float desiredDrive = clamp(
					1.f + desiredMakeupDb * 0.18f,
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
				const float clampedDrive = clamp(saturator.drive, kSatMinDrive, kSatMaxDrive);
				saturator.driveNormInv = 1.f / std::max(fastAtanApprox(clampedDrive), 1e-6f);
				saturator.tubeBiasOffset = fastAtanApprox(clampedDrive * kSatTubeBias);
			}

			const float makeup = saturator.makeupLinear;
			const float drive = clamp(saturator.drive, kSatMinDrive, kSatMaxDrive);
			const float driveNormInv = saturator.driveNormInv;
			const bool nearNeutralSat = (drive <= 1.01f && makeup <= 1.01f);
			if (nearNeutralSat) {
				saturatedL = enhancedL;
				saturatedR = enhancedR;
			}
			else {
				auto shape = [&](float x) {
					const float xNorm = clamp((x * makeup) / kAudioFullScaleV, -3.f, 3.f);
					const float biased = xNorm + kSatTubeBias;
					const float shapedNorm = (fastAtanApprox(drive * biased) - saturator.tubeBiasOffset) * driveNormInv;
					const float y = shapedNorm * kAudioFullScaleV;
					return x + kSatTubeWet * (y - x);
				};
				saturatedL = shape(enhancedL);
				saturatedR = shape(enhancedR);
			}
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

		float outL = saturatedL;
		float outR = saturatedR;
		float limiterLed = 0.f;
		{
			const float detectorPeakL = intersamplePeak4xCausal(limiterPrevSatL2, limiterPrevSatL1, saturatedL);
			const float detectorPeakR = intersamplePeak4xCausal(limiterPrevSatR2, limiterPrevSatR1, saturatedR);
			const float detectorPeak = std::max(detectorPeakL, detectorPeakR);
			limiterPrevSatL2 = limiterPrevSatL1;
			limiterPrevSatL1 = saturatedL;
			limiterPrevSatR2 = limiterPrevSatR1;
			limiterPrevSatR1 = saturatedR;

			const int delayLen = std::max(1, int(limiterDelayL.size()));
			const int readIdx = (limiterDelayWrite + 1) % delayLen;
			const float delayedL = limiterDelayL[size_t(readIdx)];
			const float delayedR = limiterDelayR[size_t(readIdx)];
			limiterDelayL[size_t(limiterDelayWrite)] = saturatedL;
			limiterDelayR[size_t(limiterDelayWrite)] = saturatedR;
			limiterDelayWrite = readIdx;

			float desiredGain = 1.f;
			if (repairEnabled) {
				const float kneeStart = limiterCeiling * std::pow(10.f, -kLimiterRepairKneeLeadDb / 20.f);
				if (detectorPeak > limiterCeiling && detectorPeak > 1e-9f) {
					desiredGain = limiterCeiling / detectorPeak;
				}
				else if (detectorPeak > kneeStart) {
					const float t = clamp(
						(detectorPeak - kneeStart) / std::max(limiterCeiling - kneeStart, 1e-9f),
						0.f,
						1.f
					);
					const float kneeDb = -kLimiterRepairKneeDepthDb * (t * t);
					desiredGain = std::pow(10.f, kneeDb / 20.f);
				}
			}
			else {
				desiredGain = (detectorPeak > limiterCeiling && detectorPeak > 1e-9f) ? (limiterCeiling / detectorPeak) : 1.f;
			}
			if (desiredGain < limiterGain) {
				const float attackCoeff = repairEnabled ? limiterRepairAttackCoeff : limiterAttackCoeff;
				limiterGain = desiredGain + attackCoeff * (limiterGain - desiredGain);
			}
			else {
				const float releaseCoeff = repairEnabled ? limiterRepairReleaseCoeff : limiterReleaseCoeff;
				limiterGain = desiredGain + releaseCoeff * (limiterGain - desiredGain);
			}
			outL = delayedL * limiterGain;
			outR = delayedR * limiterGain;
			const float finalPeak = std::max(std::fabs(outL), std::fabs(outR));
			if (finalPeak > limiterCeiling && finalPeak > 1e-9f) {
				const float guardGain = limiterCeiling / finalPeak;
				outL *= guardGain;
				outR *= guardGain;
			}
			const float detectorPeakDbTp = toDbFsSafe(detectorPeak);
			const float limiterDemandDb = std::max(0.f, detectorPeakDbTp - kLimiterCeilingDb);
			const float triggeredNow = (limiterDemandDb >= kLimiterTriggerDb) ? 1.f : 0.f;
			const float trigCoeff =
				(triggeredNow > limiterTriggerEma) ? limiterMetricAttackCoeff : limiterMetricReleaseCoeff;
			limiterTriggerEma = triggeredNow + trigCoeff * (limiterTriggerEma - triggeredNow);
			limiterRecentGrDb = limiterDemandDb + limiterMetricGrCoeff * (limiterRecentGrDb - limiterDemandDb);

			// Hybrid indicator for the -1.0 dBTP safety stage:
			// subtle glow near ceiling, strong brightness when limiting demand is real.
			const float ceilingProximity = softKnee01(detectorPeakDbTp, kLimiterCeilingDb - 3.f, 6.f);
			const float limitingActivity = std::sqrt(clamp(limiterDemandDb / 1.5f, 0.f, 1.f));
			limiterLed = clamp(std::max(0.25f * ceilingProximity, limitingActivity), 0.f, 1.f);
		}
		const float bypassL = repairActive ? delayedInL : inL;
		const float bypassR = repairActive ? delayedInR : inR;
		const float audibleL = masteringEnabled ? outL : bypassL;
		const float audibleR = masteringEnabled ? outR : bypassR;

		outputs[OUTPUT_L_OUTPUT].setChannels(1);
		outputs[OUTPUT_R_OUTPUT].setChannels(1);
		outputs[OUTPUT_L_OUTPUT].setVoltage(audibleL);
		outputs[OUTPUT_R_OUTPUT].setVoltage(audibleR);
		if (lightDivider.process()) {
			const float lightDt = args.sampleTime * float(kLightDivision);
			lights[LIMITER_ACTIVE_LIGHT].setSmoothBrightness(masteringEnabled ? limiterLed : 0.f, lightDt);
			lights[LOW_RECOVERY_LIGHT].setSmoothBrightness(masteringEnabled ? lowRecoveryAmount : 0.f, lightDt);
			lights[IMPACT_AIR_LIGHT].setSmoothBrightness(masteringEnabled ? impactAirLed : 0.f, lightDt);
			lights[REMOVE_MUD_LIGHT].setSmoothBrightness(masteringEnabled ? removeMudLed : 0.f, lightDt);
			lights[MID_ENHANCE_LIGHT].setSmoothBrightness(masteringEnabled ? midEnhanceLed : 0.f, lightDt);
			lights[GLUE_COMP_LIGHT].setSmoothBrightness(masteringEnabled ? glueLed : 0.f, lightDt);
			lights[STEREO_ENHANCE_LIGHT].setSmoothBrightness(masteringEnabled ? stereoEnhanceLed : 0.f, lightDt);
			lights[SATURATOR_LIGHT].setSmoothBrightness(masteringEnabled ? saturatorLed : 0.f, lightDt);
			lights[MICROPEAK_LIGHT].setSmoothBrightness(repairActive ? micropeakActivity : 0.f, lightDt);
			lights[MASTERING_ENABLED_LIGHT].setSmoothBrightness(masteringEnabled ? 0.5f : 0.f, lightDt);
			lights[REPAIR_ENABLED_LIGHT].setSmoothBrightness(repairActive ? 0.5f : 0.f, lightDt);
		}

		// Update histogram (Waveform)
		hist.currentMinL = std::min(hist.currentMinL, audibleL);
		hist.currentMaxL = std::max(hist.currentMaxL, audibleL);
		hist.currentMinR = std::min(hist.currentMinR, audibleR);
		hist.currentMaxR = std::max(hist.currentMaxR, audibleR);
		hist.samplesInCurrentBin++;

		if (hist.samplesInCurrentBin >= hist.samplesPerBin) {
			hist.minL[hist.writePtr] = hist.currentMinL;
			hist.maxL[hist.writePtr] = hist.currentMaxL;
			hist.minR[hist.writePtr] = hist.currentMinR;
			hist.maxR[hist.writePtr] = hist.currentMaxR;
			hist.writePtr = (hist.writePtr + 1) % HISTOGRAM_BINS;

			const float instantPeak = std::max(
				std::max(std::fabs(hist.currentMinL), std::fabs(hist.currentMaxL)),
				std::max(std::fabs(hist.currentMinR), std::fabs(hist.currentMaxR))
			);
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
		// Spectrogram view is Mid (left panel) / Side (right panel), not raw L/R.
		spec.bufferL[spec.writePtr] = 0.5f * (audibleL + audibleR);
		spec.bufferR[spec.writePtr] = 0.5f * (audibleL - audibleR);
		spec.writePtr = (spec.writePtr + 1) % FFT_SIZE;

		if (specDivider.process()) {
			int outIdx = 0;
			for (int idx = spec.writePtr; idx < FFT_SIZE; ++idx, ++outIdx) {
				spec.fftInL[outIdx] = spec.bufferL[idx] * spec.window[outIdx];
				spec.fftInR[outIdx] = spec.bufferR[idx] * spec.window[outIdx];
			}
			for (int idx = 0; idx < spec.writePtr; ++idx, ++outIdx) {
				spec.fftInL[outIdx] = spec.bufferL[idx] * spec.window[outIdx];
				spec.fftInR[outIdx] = spec.bufferR[idx] * spec.window[outIdx];
			}

			spec.fft->rfft(spec.fftInL, spec.fftOutL);
			spec.fft->rfft(spec.fftInR, spec.fftOutR);

			auto getMagnitudePow = [&](float* fftOut, int bin) {
				if (bin <= 0) return fftOut[0] * fftOut[0];
				if (bin >= FFT_SIZE / 2) return fftOut[1] * fftOut[1];
				float re = fftOut[2 * bin];
				float im = fftOut[2 * bin + 1];
				return re * re + im * im;
			};

			float maxPow = 0.f;
			static constexpr float kSpecNormPowInv = 1.f / (10240.f * 10240.f);
			for (int i = 0; i < SPEC_FREQ_BINS; i++) {
				const int binIdx = specBinMap[i].idx;
				const float frac = specBinMap[i].frac;
				float powL, powR;
				if (binIdx < FFT_SIZE / 2) {
					powL = (1.f - frac) * getMagnitudePow(spec.fftOutL, binIdx) + frac * getMagnitudePow(spec.fftOutL, binIdx + 1);
					powR = (1.f - frac) * getMagnitudePow(spec.fftOutR, binIdx) + frac * getMagnitudePow(spec.fftOutR, binIdx + 1);
				} else {
					powL = getMagnitudePow(spec.fftOutL, FFT_SIZE / 2);
					powR = getMagnitudePow(spec.fftOutR, FFT_SIZE / 2);
				}

				powL *= kSpecNormPowInv;
				powR *= kSpecNormPowInv;

				spec.magnitudesL[i] = spec.magnitudesL[i] * 0.3f + powL * 0.7f;
				spec.magnitudesR[i] = spec.magnitudesR[i] * 0.3f + powR * 0.7f;
				maxPow = std::max(maxPow, std::max(spec.magnitudesL[i], spec.magnitudesR[i]));
			}

			float instantPeakDb = 10.f * std::log10(maxPow + 1e-12f);
			if (instantPeakDb > spec.smoothedPeakDb)
				spec.smoothedPeakDb = spec.smoothedPeakDb * 0.1f + instantPeakDb * 0.9f;
			else
				spec.smoothedPeakDb = spec.smoothedPeakDb * 0.995f + instantPeakDb * 0.005f;
			
			spec.smoothedPeakDb = clamp(spec.smoothedPeakDb, -100.f, 20.f);
			const float ceilingDb = spec.smoothedPeakDb + 6.f;
			const float floorDb = ceilingDb - 70.f;
			const float dbSpanInv = 1.f / std::max(ceilingDb - floorDb, 1e-6f);
			for (int i = 0; i < SPEC_FREQ_BINS; ++i) {
				const float dbL = 10.f * std::log10(spec.magnitudesL[i] + 1e-12f);
				const float dbR = 10.f * std::log10(spec.magnitudesR[i] + 1e-12f);
				spec.displayNormL[i] = clamp((dbL - floorDb) * dbSpanInv, 0.f, 1.f);
				spec.displayNormR[i] = clamp((dbR - floorDb) * dbSpanInv, 0.f, 1.f);
			}
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
		auto rgbToHsv = [](const NVGcolor& c, float& h, float& s, float& v) {
			const float r = clamp(c.r, 0.f, 1.f);
			const float g = clamp(c.g, 0.f, 1.f);
			const float b = clamp(c.b, 0.f, 1.f);
			const float mx = std::max(r, std::max(g, b));
			const float mn = std::min(r, std::min(g, b));
			const float d = mx - mn;
			v = mx;
			s = (mx <= 1e-6f) ? 0.f : (d / mx);
			if (d <= 1e-6f) {
				h = 0.f;
				return;
			}
			if (mx == r) {
				h = std::fmod(((g - b) / d), 6.f);
			}
			else if (mx == g) {
				h = ((b - r) / d) + 2.f;
			}
			else {
				h = ((r - g) / d) + 4.f;
			}
			h *= 60.f;
			if (h < 0.f) h += 360.f;
		};
		auto hsvToRgb = [](float h, float s, float v, float a) {
			h = std::fmod(h, 360.f);
			if (h < 0.f) h += 360.f;
			s = clamp(s, 0.f, 1.f);
			v = clamp(v, 0.f, 1.f);
			const float c = v * s;
			const float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
			const float m = v - c;
			float rp = 0.f, gp = 0.f, bp = 0.f;
			if (h < 60.f) {
				rp = c; gp = x; bp = 0.f;
			}
			else if (h < 120.f) {
				rp = x; gp = c; bp = 0.f;
			}
			else if (h < 180.f) {
				rp = 0.f; gp = c; bp = x;
			}
			else if (h < 240.f) {
				rp = 0.f; gp = x; bp = c;
			}
			else if (h < 300.f) {
				rp = x; gp = 0.f; bp = c;
			}
			else {
				rp = c; gp = 0.f; bp = x;
			}
			return nvgRGBAf(rp + m, gp + m, bp + m, clamp(a, 0.f, 1.f));
		};
		auto shiftForSide = [&](const NVGcolor& base) {
			float h = 0.f, s = 0.f, v = 0.f;
			rgbToHsv(base, h, s, v);
			const float shiftedHue = h + 42.f;
			const float shiftedSat = clamp(s * 0.92f + 0.08f, 0.f, 1.f);
			const float shiftedVal = clamp(v * 0.92f + 0.05f, 0.f, 1.f);
			return hsvToRgb(shiftedHue, shiftedSat, shiftedVal, base.a);
		};
		const NVGcolor sideLow = shiftForSide(colors.low);
		const NVGcolor sideHigh = shiftForSide(colors.high);
		const NVGcolor channelLow = isRightChannel ? sideLow : colors.low;
		const NVGcolor channelHigh = isRightChannel ? sideHigh : colors.high;

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, colors.bg);
		nvgFill(args.vg);

		// Draw background grid
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

		const float* norms = isRightChannel ? module->spec.displayNormR : module->spec.displayNormL;
		float barW = box.size.x / Sil::SPEC_FREQ_BINS;

		for (int i = 0; i < Sil::SPEC_FREQ_BINS; i++) {
			const float norm = norms[i];
			if (norm <= 0.01f) continue;

			float barH = norm * box.size.y;
			float x = (float)i * barW;
			NVGcolor color = nvgLerpRGBA(channelLow, channelHigh, norm);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, x, box.size.y - barH, barW - 0.5f, barH);
			nvgFillColor(args.vg, color);
			nvgFill(args.vg);
		}

	}
};

struct ChainLedDebugReadoutWidget : TransparentWidget {
	Sil* module = nullptr;
	static constexpr int kCount = 8;
	static constexpr int kHistBins = 160;
	std::array<int, kCount> lightIds = {
		Sil::LIMITER_ACTIVE_LIGHT,
		Sil::LOW_RECOVERY_LIGHT,
		Sil::IMPACT_AIR_LIGHT,
		Sil::REMOVE_MUD_LIGHT,
		Sil::MID_ENHANCE_LIGHT,
		Sil::GLUE_COMP_LIGHT,
		Sil::STEREO_ENHANCE_LIGHT,
		Sil::SATURATOR_LIGHT
	};
	std::array<Vec, kCount> textPositions;
	std::array<std::array<float, kHistBins>, kCount> histories {};
	int writeIndex = 0;
	float histogramStartX = 0.f;

	void draw(const DrawArgs& args) override {
		if (!module || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}

		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 9.0f);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

		std::array<float, kCount> current {};
		char label[16];
		for (int i = 0; i < kCount; ++i) {
			const float b = clamp(module->lights[lightIds[i]].getBrightness(), 0.f, 1.f);
			current[i] = b;
			const int pct = int(std::round(b * 100.f));
			std::snprintf(label, sizeof(label), "%3d%%", pct);

			const Vec p = textPositions[i];
			nvgFillColor(args.vg, nvgRGBA(8, 8, 8, 210));
			nvgText(args.vg, p.x + 0.45f, p.y + 0.45f, label, nullptr);
			nvgFillColor(args.vg, nvgRGBA(245, 245, 245, 255));
			nvgText(args.vg, p.x, p.y, label, nullptr);
		}

		for (int i = 0; i < kCount; ++i) {
			histories[i][size_t(writeIndex)] = current[i];
		}
		writeIndex = (writeIndex + 1) % kHistBins;

		const float x0 = (histogramStartX > 0.f) ? histogramStartX : 0.50f * box.size.x;
		const float rightPad = 6.f;
		const float histW = std::max(24.f, box.size.x - x0 - rightPad);
		const float histH = 8.f;
		const float barW = histW / float(kHistBins);

		for (int i = 0; i < kCount; ++i) {
			const float centerY = textPositions[i].y;
			const float top = centerY - 0.5f * histH;

			nvgBeginPath(args.vg);
			nvgRect(args.vg, x0, top, histW, histH);
			nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 80));
			nvgFill(args.vg);

			for (int j = 0; j < kHistBins; ++j) {
				const int idx = (writeIndex + j) % kHistBins;
				const float v = clamp(histories[i][size_t(idx)], 0.f, 1.f);
				if (v <= 0.01f) {
					continue;
				}
				const float x = x0 + j * barW;
				const float h = v * histH;
				nvgBeginPath(args.vg);
				nvgRect(args.vg, x, top + (histH - h), std::max(0.8f, barW - 0.25f), h);
				nvgFillColor(args.vg, nvgRGBA(245, 245, 245, 210));
				nvgFill(args.vg);
			}
		}
	}
};

struct MicropeakRepairCountWidget : TransparentWidget {
	Sil* module = nullptr;
	Vec textPosPx;

	void draw(const DrawArgs& args) override {
		if (!module || !isDragonKingDebugEnabled() || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 9.0f);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

		char label[40];
		const uint32_t countL = module->micropeakRepairCountL.load(std::memory_order_relaxed);
		const uint32_t countR = module->micropeakRepairCountR.load(std::memory_order_relaxed);
		const uint32_t nearMissCount = module->micropeakNearMissCount.load(std::memory_order_relaxed);
		std::snprintf(label, sizeof(label), "L: %u R: %u NM: %u", countL, countR, nearMissCount);
		nvgFillColor(args.vg, nvgRGBA(8, 8, 8, 210));
		nvgText(args.vg, textPosPx.x + 0.45f, textPosPx.y + 0.45f, label, nullptr);
		nvgFillColor(args.vg, nvgRGBA(245, 245, 245, 255));
		nvgText(args.vg, textPosPx.x, textPosPx.y, label, nullptr);
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
		float sideSpecLeftX = mm2px(Vec(51.f, 0.f)).x;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_RIGHT", &specRRect)) {
			specRRect = specRRect.grow(Vec(-0.2f, -0.2f));
			sideSpecLeftX = mm2px(specRRect.pos).x;
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
		Vec impactAirLightPos(48.f, 46.8f);
		Vec removeMudLightPos(48.f, 47.6f);
		Vec midEnhanceLightPos(48.f, 48.4f);
		Vec glueCompLightPos(48.f, 49.2f);
		Vec stereoEnhanceLightPos(48.f, 50.0f);
		Vec saturatorLightPos(48.f, 50.8f);
		Vec micropeakLightPos(48.f, 51.6f);
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
		applyPointOverride("IMPACT_AIR_LIGHT", &impactAirLightPos);
		applyPointOverride("REMOVE_MUD_LIGHT", &removeMudLightPos);
		applyPointOverride("MID_ENHANCE_LIGHT", &midEnhanceLightPos);
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
		if (isDragonKingDebugEnabled()) {
			addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
				mm2px(repairButtonPos), module, Sil::REPAIR_ENABLED_PARAM, Sil::REPAIR_ENABLED_LIGHT
			));
		}
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(limiterLightPos), module, Sil::LIMITER_ACTIVE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(lowRecoveryLightPos), module, Sil::LOW_RECOVERY_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(impactAirLightPos), module, Sil::IMPACT_AIR_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(removeMudLightPos), module, Sil::REMOVE_MUD_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(midEnhanceLightPos), module, Sil::MID_ENHANCE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(glueCompLightPos), module, Sil::GLUE_COMP_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(stereoEnhanceLightPos), module, Sil::STEREO_ENHANCE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(saturatorLightPos), module, Sil::SATURATOR_LIGHT));
		if (isDragonKingDebugEnabled()) {
			addChild(createLightCentered<SmallLight<RedLight>>(mm2px(micropeakLightPos), module, Sil::MICROPEAK_LIGHT));
		}

		if (isDragonKingDebugEnabled()) {
			MicropeakRepairCountWidget* micropeakCount = createWidget<MicropeakRepairCountWidget>(Vec(0.f, 0.f));
			micropeakCount->box.size = box.size;
			micropeakCount->module = module;
			micropeakCount->textPosPx = mm2px(Vec(micropeakLightPos.x + 4.4f, micropeakLightPos.y));
			addChild(micropeakCount);
		}

		ChainLedDebugReadoutWidget* chainLedReadout = createWidget<ChainLedDebugReadoutWidget>(Vec(0.f, 0.f));
		chainLedReadout->box.size = box.size;
		chainLedReadout->module = module;
		const float histLeftExtension = 0.125f * box.size.x;
		chainLedReadout->histogramStartX = std::max(0.f, sideSpecLeftX - histLeftExtension);
		const float textOffsetMm = 2.4f;
		chainLedReadout->textPositions = {
			mm2px(Vec(limiterLightPos.x - textOffsetMm, limiterLightPos.y)),
				mm2px(Vec(lowRecoveryLightPos.x - textOffsetMm, lowRecoveryLightPos.y)),
				mm2px(Vec(impactAirLightPos.x - textOffsetMm, impactAirLightPos.y)),
				mm2px(Vec(removeMudLightPos.x - textOffsetMm, removeMudLightPos.y)),
				mm2px(Vec(midEnhanceLightPos.x - textOffsetMm, midEnhanceLightPos.y)),
				mm2px(Vec(glueCompLightPos.x - textOffsetMm, glueCompLightPos.y)),
				mm2px(Vec(stereoEnhanceLightPos.x - textOffsetMm, stereoEnhanceLightPos.y)),
				mm2px(Vec(saturatorLightPos.x - textOffsetMm, saturatorLightPos.y))
		};
		addChild(chainLedReadout);

	}

	void step() override {
		Sil* sil = dynamic_cast<Sil*>(module);
		if (sil) {
			sil->drainMicropeakDebugQueueToCsv();
		}
		ModuleWidget::step();
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
				addSchemeItem(Sil::SCHEME_DEFAULT, "Default (Purple/Cyan)");
				addSchemeItem(Sil::SCHEME_CLASSIC, "Classic (Green/Red)");
				addSchemeItem(Sil::SCHEME_MONOCHROME, "Monochrome (Gray/White)");
				addSchemeItem(Sil::SCHEME_FIRE, "Fire (Red/Yellow)");
			}
		));
		if (isDragonKingDebugEnabled()) {
			const bool debugActive = sil->isMicropeakDebugCaptureActive();
			menu->addChild(new MenuSeparator());
			menu->addChild(createMenuLabel("Debug"));
			menu->addChild(createMenuItem(
				debugActive ? "Close Sil micropeak CSV" : "Begin Sil micropeak CSV",
				debugActive ? "Active" : "",
				[=]() {
					sil->toggleMicropeakDebugCapture();
				}
			));
			if (debugActive) {
				menu->addChild(createMenuLabel(sil->getMicropeakDebugPath()));
			}
		}
	}
};

Model* modelSil = createModel<Sil, SilWidget>("Sil");
