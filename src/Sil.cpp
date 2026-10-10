#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "SilLimiterPeakWindow.hpp"
#include "SilLevelMeter.hpp"
#include "SilSpectrumSnapshot.hpp"
#include "SpscLatestSnapshot.hpp"
#include "DebugTerminalMetrics.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include <vector>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>

namespace {

std::atomic<uint32_t> gSilDebugInstanceCounter {1u};

} // namespace

struct Sil : Module {
	ModuleTeardownTimer teardownTimer {"Sil"};
	debug_terminal::BaselineModuleMetrics debugMetrics;
	std::atomic<uint64_t> perfLatestProcessNs {0u};
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
	float masteringMix = 1.f;

	static constexpr int HISTOGRAM_BINS = 1000;
	static constexpr float HISTOGRAM_DURATION = 10.f;

	struct HistogramData {
		// Audio-owned accumulation is published through atomics only when a bin
		// completes. UI widgets copy these into a local immutable draw snapshot.
		std::atomic<float> displayMinL[HISTOGRAM_BINS] {};
		std::atomic<float> displayMaxL[HISTOGRAM_BINS] {};
		std::atomic<float> displayMinR[HISTOGRAM_BINS] {};
		std::atomic<float> displayMaxR[HISTOGRAM_BINS] {};
		std::atomic<int> displayWritePtr {0};
		int writePtr = 0;

		float currentMinL = 1e10f, currentMaxL = -1e10f;
		float currentMinR = 1e10f, currentMaxR = -1e10f;
		int samplesInCurrentBin = 0;
		int samplesPerBin = 441;

		float smoothedPeak = 5.f;
	} hist;

	static constexpr int SPEC_FREQ_BINS = 128;
	static constexpr int FFT_SIZE = sil::SpectrumSnapshot::kFftSize;

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
	using SpectrumSnapshot = sil::SpectrumSnapshot;
	snapshot_transport::SpscLatestSnapshot<SpectrumSnapshot> specSnapshots;
	std::atomic<uint32_t> specSnapshotSeq {0u};
	uint32_t specUiLastSeq = 0u;
	struct SpectrumBinMapEntry {
		int idx = 0;
		float frac = 0.f;
	};
	SpectrumBinMapEntry specBinMap[SPEC_FREQ_BINS];

	sil::StereoLevelMeter levelMeter;
	dsp::ClockDivider specDivider;
	dsp::ClockDivider lightDivider;
	float limiterGain = 1.f;
	std::vector<float> limiterDelayL;
	std::vector<float> limiterDelayR;
	int limiterDelayWrite = 0;
	int limiterLatencySamples = 1;
	int limiterLookaheadSamples = 1;
	std::vector<float> bypassDelayL;
	std::vector<float> bypassDelayR;
	int bypassDelayWrite = 0;
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
	static constexpr int kLimiterTruePeakOversample = 4;
	static constexpr int kLimiterTruePeakQuality = 8;
	dsp::Upsampler<kLimiterTruePeakOversample, kLimiterTruePeakQuality> limiterTruePeakUpsamplerL;
	dsp::Upsampler<kLimiterTruePeakOversample, kLimiterTruePeakQuality> limiterTruePeakUpsamplerR;
	sil::LimiterPeakWindow limiterPeakWindow;
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

	static constexpr float kLowBandCutoffHz = 120.f;
	static constexpr float kLowBandCorrTauSec = 0.100f;
	static constexpr float kLowBandSideAttackSec = 0.050f;
	static constexpr float kLowBandSideReleaseSec = 0.250f;
	static constexpr float kAudioFullScaleV = 5.f;
	static constexpr float kRollingBufferSeconds = 10.f;
	static constexpr float kAdaptiveSilenceVolts = 0.0012559432f;
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
	static constexpr float kLimiterLookaheadSeconds = 0.0005f;
	static constexpr float kMasteringCrossfadeSeconds = 0.010f;
	static constexpr int kMaxLimiterLookaheadSamples = sil::LimiterPeakWindow::kMaximumLookaheadSamples;
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

	void updateSpectrumUpdateDivider(float sampleRate) {
		const float sr = std::max(sampleRate, 1.f);
		static constexpr float kSpectrumUpdateHz = 24.f;
		const int division = std::max(1, int(std::round(sr / kSpectrumUpdateHz)));
		specDivider.setDivision(division);
	}

	void updateSpectrumDisplayFromLatestSnapshot() {
		const uint32_t latestSeq = specSnapshotSeq.load(std::memory_order_acquire);
		if (latestSeq == specUiLastSeq) {
			return;
		}

		// Only the left spectrum widget consumes, on Rack's UI thread. Its
		// owned slot remains stable throughout windowing and FFT processing.
		const SpectrumSnapshot& snapshot = specSnapshots.readLatest();
		if (snapshot.sequence == specUiLastSeq) return;

		for (int i = 0; i < FFT_SIZE; ++i) {
			spec.fftInL[i] = snapshot.mid[i] * spec.window[i];
			spec.fftInR[i] = snapshot.side[i] * spec.window[i];
		}

		spec.fft->rfft(spec.fftInL, spec.fftOutL);
		spec.fft->rfft(spec.fftInR, spec.fftOutR);

		auto getMagnitudePow = [&](float* fftOut, int bin) {
			if (bin <= 0) return fftOut[0] * fftOut[0];
			if (bin >= FFT_SIZE / 2) return fftOut[1] * fftOut[1];
			const float re = fftOut[2 * bin];
			const float im = fftOut[2 * bin + 1];
			return re * re + im * im;
		};

		float maxPow = 0.f;
		static constexpr float kSpecNormPowInv = 1.f / (10240.f * 10240.f);
		for (int i = 0; i < SPEC_FREQ_BINS; i++) {
			const int binIdx = specBinMap[i].idx;
			const float frac = specBinMap[i].frac;
			float powL = 0.f;
			float powR = 0.f;
			if (binIdx < FFT_SIZE / 2) {
				powL = (1.f - frac) * getMagnitudePow(spec.fftOutL, binIdx) + frac * getMagnitudePow(spec.fftOutL, binIdx + 1);
				powR = (1.f - frac) * getMagnitudePow(spec.fftOutR, binIdx) + frac * getMagnitudePow(spec.fftOutR, binIdx + 1);
			}
			else {
				powL = getMagnitudePow(spec.fftOutL, FFT_SIZE / 2);
				powR = getMagnitudePow(spec.fftOutR, FFT_SIZE / 2);
			}

			powL *= kSpecNormPowInv;
			powR *= kSpecNormPowInv;
			spec.magnitudesL[i] = spec.magnitudesL[i] * 0.3f + powL * 0.7f;
			spec.magnitudesR[i] = spec.magnitudesR[i] * 0.3f + powR * 0.7f;
			maxPow = std::max(maxPow, std::max(spec.magnitudesL[i], spec.magnitudesR[i]));
		}

		const float instantPeakDb = 10.f * std::log10(maxPow + 1e-12f);
		if (instantPeakDb > spec.smoothedPeakDb) {
			spec.smoothedPeakDb = spec.smoothedPeakDb * 0.1f + instantPeakDb * 0.9f;
		}
		else {
			spec.smoothedPeakDb = spec.smoothedPeakDb * 0.995f + instantPeakDb * 0.005f;
		}
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

		specUiLastSeq = snapshot.sequence;
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
		limiterCeiling = kAudioFullScaleV * std::pow(10.f, kLimiterCeilingDb / 20.f);
		limiterMetricAttackCoeff = std::exp(-1.f / (kLimiterMetricAttackSec * sr));
		limiterMetricReleaseCoeff = std::exp(-1.f / (kLimiterMetricReleaseSec * sr));
		limiterMetricGrCoeff = std::exp(-1.f / (kLimiterMetricGrSec * sr));
	}

	int limiterLookaheadSamplesForRate(float sampleRate) const {
		int lookahead = std::max(1, int(std::round(std::max(sampleRate, 1.f) * kLimiterLookaheadSeconds)));
		return clamp(lookahead, 1, kMaxLimiterLookaheadSamples);
	}

	void updateLimiterLatencyNoAlloc(float sampleRate) {
		const int requestedLimiterLookahead = limiterLookaheadSamplesForRate(sampleRate);
		if (requestedLimiterLookahead == limiterLookaheadSamples) {
			return;
		}
		limiterLookaheadSamples = requestedLimiterLookahead;
		limiterLatencySamples = limiterLookaheadSamples;
		std::fill(bypassDelayL.begin(), bypassDelayL.end(), 0.f);
		std::fill(bypassDelayR.begin(), bypassDelayR.end(), 0.f);
		bypassDelayWrite = 0;
		std::fill(limiterDelayL.begin(), limiterDelayL.end(), 0.f);
		std::fill(limiterDelayR.begin(), limiterDelayR.end(), 0.f);
		limiterDelayWrite = 0;
		limiterGain = 1.f;
		limiterPrevSatL1 = 0.f;
		limiterPrevSatL2 = 0.f;
		limiterPrevSatR1 = 0.f;
		limiterPrevSatR2 = 0.f;
		limiterTruePeakUpsamplerL.reset();
		limiterTruePeakUpsamplerR.reset();
		limiterPeakWindow.clear();
	}

		void initializeLimiterFastPathStorage() {
			const int delayBufferLength = kMaxLimiterLookaheadSamples;
			limiterLookaheadSamples = 1;
			limiterLatencySamples = limiterLookaheadSamples;
			limiterDelayL.assign(size_t(delayBufferLength), 0.f);
			limiterDelayR.assign(size_t(delayBufferLength), 0.f);
		bypassDelayL.assign(size_t(delayBufferLength), 0.f);
		bypassDelayR.assign(size_t(delayBufferLength), 0.f);
		bypassDelayWrite = 0;
		limiterDelayWrite = 0;
		limiterGain = 1.f;
		limiterPrevSatL1 = 0.f;
		limiterPrevSatL2 = 0.f;
		limiterPrevSatR1 = 0.f;
			limiterPrevSatR2 = 0.f;
			limiterTruePeakUpsamplerL.reset();
			limiterTruePeakUpsamplerR.reset();
			limiterPeakWindow.clear();
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
		if (oldValid && oldHist < SaturatorState::PERCENTILE_BINS && saturator.percentileHist[oldHist] > 0) {
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
		debugMetrics.assignInstanceId(gSilDebugInstanceCounter);
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(MASTERING_ENABLED_PARAM, 0.f, 1.f, 1.f, "Mastering", {"Disabled", "Enabled"});
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

		// Keep spectrum visually responsive at a stable update rate across sample rates.
		updateSpectrumUpdateDivider(initialSampleRate);
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
		updateLimiterLatencyNoAlloc(initialSampleRate);
		levelMeter.configure(initialSampleRate);
	}

	~Sil() {
		teardownTimer.begin(id);
		delete spec.fft;
	}

	static std::string userDebugRootPath() {
		return system::join(asset::user(), "Leviathan/Sil");
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
		updateSpectrumUpdateDivider(e.sampleRate);
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
		updateLimiterLatencyNoAlloc(e.sampleRate);
		levelMeter.configure(e.sampleRate);
	}

	void process(const ProcessArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto processStart = debug_terminal::debugTimerStart(measurePerf);
		masteringEnabled = params[MASTERING_ENABLED_PARAM].getValue() > 0.5f;

		const bool hasL = inputs[INPUT_L_INPUT].isConnected();
		const bool hasR = inputs[INPUT_R_INPUT].isConnected();
		const float rawL = inputs[INPUT_L_INPUT].getVoltage();
		const float rawR = inputs[INPUT_R_INPUT].getVoltage();
		const float inL = hasL ? rawL : rawR;
		const float inR = hasR ? rawR : rawL;
		const int bypassDelayLen = std::max(1, limiterLookaheadSamples);
		const float delayedInL = bypassDelayL[size_t(bypassDelayWrite)];
		const float delayedInR = bypassDelayR[size_t(bypassDelayWrite)];
		bypassDelayL[size_t(bypassDelayWrite)] = inL;
		bypassDelayR[size_t(bypassDelayWrite)] = inR;
		bypassDelayWrite = (bypassDelayWrite + 1) % bypassDelayLen;

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
		float impactAirL;
		float impactAirR;
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
		float mudCleanL;
		float mudCleanR;
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
			const float causalPeakL = intersamplePeak4xCausal(limiterPrevSatL2, limiterPrevSatL1, saturatedL);
			const float causalPeakR = intersamplePeak4xCausal(limiterPrevSatR2, limiterPrevSatR1, saturatedR);
			limiterPrevSatL2 = limiterPrevSatL1;
			limiterPrevSatL1 = saturatedL;
			limiterPrevSatR2 = limiterPrevSatR1;
			limiterPrevSatR1 = saturatedR;
			float truePeakSamplesL[kLimiterTruePeakOversample];
			float truePeakSamplesR[kLimiterTruePeakOversample];
			limiterTruePeakUpsamplerL.process(saturatedL, truePeakSamplesL);
			limiterTruePeakUpsamplerR.process(saturatedR, truePeakSamplesR);
			float detectorPeakL = causalPeakL;
			float detectorPeakR = causalPeakR;
				for (int i = 0; i < kLimiterTruePeakOversample; ++i) {
					detectorPeakL = std::max(detectorPeakL, std::fabs(truePeakSamplesL[i]));
					detectorPeakR = std::max(detectorPeakR, std::fabs(truePeakSamplesR[i]));
				}
				const float detectorPeakNow = std::max(detectorPeakL, detectorPeakR);
				const float detectorPeak = limiterPeakWindow.push(detectorPeakNow, limiterLookaheadSamples);

			const int delayLen = std::max(1, limiterLookaheadSamples);
			const float delayedL = limiterDelayL[size_t(limiterDelayWrite)];
			const float delayedR = limiterDelayR[size_t(limiterDelayWrite)];
			limiterDelayL[size_t(limiterDelayWrite)] = saturatedL;
			limiterDelayR[size_t(limiterDelayWrite)] = saturatedR;
			limiterDelayWrite = (limiterDelayWrite + 1) % delayLen;

			const float desiredGain = (detectorPeak > limiterCeiling && detectorPeak > 1e-9f)
				? limiterCeiling / detectorPeak : 1.f;
			const float gainCoeff = desiredGain < limiterGain ? limiterAttackCoeff : limiterReleaseCoeff;
			limiterGain = desiredGain + gainCoeff * (limiterGain - desiredGain);
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
		const float bypassL = delayedInL;
		const float bypassR = delayedInR;
		const float targetMasteringMix = masteringEnabled ? 1.f : 0.f;
		const float mixStep = args.sampleTime / std::max(kMasteringCrossfadeSeconds, 1e-6f);
		if (masteringMix < targetMasteringMix) {
			masteringMix = std::min(targetMasteringMix, masteringMix + mixStep);
		}
		else if (masteringMix > targetMasteringMix) {
			masteringMix = std::max(targetMasteringMix, masteringMix - mixStep);
		}
		const float audibleL = bypassL + masteringMix * (outL - bypassL);
		const float audibleR = bypassR + masteringMix * (outR - bypassR);

		levelMeter.process(audibleL, audibleR, hasL || hasR);
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
			lights[MASTERING_ENABLED_LIGHT].setSmoothBrightness(masteringEnabled ? 0.5f : 0.f, lightDt);
		}

		// Update histogram (Waveform)
		hist.currentMinL = std::min(hist.currentMinL, audibleL);
		hist.currentMaxL = std::max(hist.currentMaxL, audibleL);
		hist.currentMinR = std::min(hist.currentMinR, audibleR);
		hist.currentMaxR = std::max(hist.currentMaxR, audibleR);
		hist.samplesInCurrentBin++;

		if (hist.samplesInCurrentBin >= hist.samplesPerBin) {
			hist.displayMinL[hist.writePtr].store(hist.currentMinL, std::memory_order_relaxed);
			hist.displayMaxL[hist.writePtr].store(hist.currentMaxL, std::memory_order_relaxed);
			hist.displayMinR[hist.writePtr].store(hist.currentMinR, std::memory_order_relaxed);
			hist.displayMaxR[hist.writePtr].store(hist.currentMaxR, std::memory_order_relaxed);
			hist.writePtr = (hist.writePtr + 1) % HISTOGRAM_BINS;
			hist.displayWritePtr.store(hist.writePtr, std::memory_order_release);

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
			const uint32_t sequence = specSnapshotSeq.load(std::memory_order_relaxed) + 1u;
			specSnapshots.publishWith([&](SpectrumSnapshot& snapshot) {
				snapshot.capture(spec.bufferL, spec.bufferR, spec.writePtr, sequence);
			});
			// Keep the existing framebuffer invalidation and logging signal.
			specSnapshotSeq.store(sequence, std::memory_order_release);
		}

		if (measurePerf) {
			const uint64_t elapsedNs = debug_terminal::elapsedNsSince(processStart);
			perfLatestProcessNs.store(elapsedNs, std::memory_order_relaxed);
			debugMetrics.recordProcess(elapsedNs);
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

struct SilRenderDebugMetrics {
	enum Slot {
		HISTOGRAM,
		SPECTRUM_LEFT,
		SPECTRUM_RIGHT,
		STAGE_HISTORY,
		SLOT_COUNT
	};
	enum DirtyReason : uint32_t {
		DIRTY_DATA = 1u << 0,
		DIRTY_PALETTE = 1u << 1,
		DIRTY_INITIAL = 1u << 2,
		DIRTY_EXTERNAL = 1u << 3
	};
	enum CacheSlot {
		CACHE_HISTOGRAM,
		CACHE_SPECTRUM_LEFT_GRID,
		CACHE_SPECTRUM_LEFT_DATA,
		CACHE_SPECTRUM_RIGHT_GRID,
		CACHE_SPECTRUM_RIGHT_DATA,
		CACHE_STAGE_HISTORY,
		CACHE_SLOT_COUNT
	};

	std::array<float, SLOT_COUNT> frameDrawUs {};
	std::array<uint32_t, SLOT_COUNT> frameDrawn {};
	std::array<uint32_t, SLOT_COUNT> frameDirtyReasons {};
	std::array<uint32_t, SLOT_COUNT> pendingDirtyReasons {};
	std::array<uint64_t, SLOT_COUNT> totalDraws {};
	std::array<float, CACHE_SLOT_COUNT> frameCacheUs {};
	std::array<uint32_t, CACHE_SLOT_COUNT> frameCacheDirty {};
	std::array<float, CACHE_SLOT_COUNT> frameCacheWidth {};
	std::array<float, CACHE_SLOT_COUNT> frameCacheHeight {};

	void beginFrame() {
		frameDrawUs.fill(0.f);
		frameDrawn.fill(0u);
		frameDirtyReasons.fill(0u);
		frameCacheUs.fill(0.f);
		frameCacheDirty.fill(0u);
	}

	void markDirty(Slot slot, uint32_t reasons) {
		pendingDirtyReasons[size_t(slot)] |= reasons;
	}

	void recordDraw(Slot slot, float elapsedUs) {
		const size_t index = size_t(slot);
		frameDrawUs[index] += elapsedUs;
		frameDrawn[index] = 1u;
		frameDirtyReasons[index] |= pendingDirtyReasons[index]
			? pendingDirtyReasons[index] : uint32_t(DIRTY_EXTERNAL);
		pendingDirtyReasons[index] = 0u;
		++totalDraws[index];
	}

	void recordCacheDraw(CacheSlot slot, float elapsedUs, bool wasDirty, Vec framebufferSize) {
		const size_t index = size_t(slot);
		frameCacheUs[index] += elapsedUs;
		frameCacheDirty[index] |= wasDirty ? 1u : 0u;
		frameCacheWidth[index] = framebufferSize.x;
		frameCacheHeight[index] = framebufferSize.y;
	}
};

struct SilTimedFramebufferWidget : widget::FramebufferWidget {
	SilRenderDebugMetrics* renderDebugMetrics = nullptr;
	SilRenderDebugMetrics::CacheSlot debugSlot = SilRenderDebugMetrics::CACHE_HISTOGRAM;

	void draw(const DrawArgs& args) override {
		const bool measurePerf = renderDebugMetrics && isDragonKingDebugEnabled();
		const bool cacheWasDirty = dirty;
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		widget::FramebufferWidget::draw(args);
		if (measurePerf) {
			renderDebugMetrics->recordCacheDraw(
				debugSlot,
				debug_terminal::elapsedUsSince(drawStart),
				cacheWasDirty,
				getFramebufferSize());
		}
	}
};

struct SilUiRefreshCoordinator {
	bool histogramRefreshedThisStep = false;
};

struct HistogramWidget : TransparentWidget {
	Sil* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	SilRenderDebugMetrics* renderDebugMetrics = nullptr;
	SilUiRefreshCoordinator* refreshCoordinator = nullptr;
	int lastWritePtr = -1;
	int lastColorScheme = -1;
	double lastRefreshSec = -1.0;
	std::array<float, Sil::HISTOGRAM_BINS> minL {};
	std::array<float, Sil::HISTOGRAM_BINS> maxL {};
	std::array<float, Sil::HISTOGRAM_BINS> minR {};
	std::array<float, Sil::HISTOGRAM_BINS> maxR {};
	NVGcontext* rasterImageVg = nullptr;
	int rasterImage = -1;
	int rasterWidth = 0;
	int rasterHeight = 0;
	int rasterWritePtr = -1;
	int rasterColorScheme = -1;
	std::vector<unsigned char> rasterPixels;

	~HistogramWidget() override {
		nvg_gfx_lifecycle::resetOwnedNvgImage(
			rasterImageVg, rasterImage, rasterWidth, rasterHeight, nullptr, false);
	}

	void step() override {
		TransparentWidget::step();
		if (!module || !framebuffer) return;
		const int writePtr = module->hist.displayWritePtr.load(std::memory_order_acquire);
		const int colorScheme = int(module->colorScheme);
		const double nowSec = system::getTime();
		const bool paletteChanged = colorScheme != lastColorScheme;
		const bool refreshDue = lastRefreshSec < 0.0 || nowSec - lastRefreshSec >= (1.0 / 30.0);
		if (paletteChanged || (writePtr != lastWritePtr && refreshDue)) {
			if (refreshCoordinator) refreshCoordinator->histogramRefreshedThisStep = true;
			uint32_t dirtyReasons = 0u;
			if (lastRefreshSec < 0.0) dirtyReasons |= SilRenderDebugMetrics::DIRTY_INITIAL;
			if (paletteChanged) dirtyReasons |= SilRenderDebugMetrics::DIRTY_PALETTE;
			if (writePtr != lastWritePtr) dirtyReasons |= SilRenderDebugMetrics::DIRTY_DATA;
			if (renderDebugMetrics) renderDebugMetrics->markDirty(SilRenderDebugMetrics::HISTOGRAM, dirtyReasons);
			for (int i = 0; i < Sil::HISTOGRAM_BINS; ++i) {
				minL[size_t(i)] = module->hist.displayMinL[i].load(std::memory_order_relaxed);
				maxL[size_t(i)] = module->hist.displayMaxL[i].load(std::memory_order_relaxed);
				minR[size_t(i)] = module->hist.displayMinR[i].load(std::memory_order_relaxed);
				maxR[size_t(i)] = module->hist.displayMaxR[i].load(std::memory_order_relaxed);
			}
			lastWritePtr = writePtr;
			lastColorScheme = colorScheme;
			lastRefreshSec = nowSec;
			framebuffer->setDirty();
		}
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = renderDebugMetrics && isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		SilColors colors = SilColors::get(module ? module->colorScheme : Sil::SCHEME_DEFAULT);
		if (module) {
			const int targetWidth = Sil::HISTOGRAM_BINS;
			const int targetHeight = std::max(2, int(std::ceil(box.size.y)));
			if (rasterImageVg != args.vg) {
				nvg_gfx_lifecycle::resetOwnedNvgImage(
					rasterImageVg, rasterImage, rasterWidth, rasterHeight, args.vg, false);
				rasterWritePtr = -1;
			}
			if (rasterImage >= 0 && !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
				args.vg, rasterImage, rasterWidth, rasterHeight)) {
				nvg_gfx_lifecycle::resetOwnedNvgImage(
					rasterImageVg, rasterImage, rasterWidth, rasterHeight, args.vg, true);
				rasterWritePtr = -1;
			}
			if (rasterImage < 0 || rasterWidth != targetWidth || rasterHeight != targetHeight) {
				nvg_gfx_lifecycle::resetOwnedNvgImage(
					rasterImageVg, rasterImage, rasterWidth, rasterHeight, args.vg, true);
				rasterWidth = targetWidth;
				rasterHeight = targetHeight;
				rasterPixels.assign(size_t(rasterWidth * rasterHeight * 4), 0u);
				rasterImage = nvgCreateImageRGBA(
					args.vg, rasterWidth, rasterHeight, NVG_IMAGE_PREMULTIPLIED, rasterPixels.data());
				rasterImageVg = args.vg;
				rasterWritePtr = -1;
			}

			if (rasterImage >= 0) {
				const bool paletteChanged = rasterColorScheme != int(module->colorScheme);
				int advance = rasterWritePtr < 0 ? rasterWidth
					: (lastWritePtr - rasterWritePtr + rasterWidth) % rasterWidth;
				const bool fullRebuild = rasterWritePtr < 0 || paletteChanged || advance <= 0 || advance >= rasterWidth;
				if (fullRebuild) {
					advance = rasterWidth;
					std::fill(rasterPixels.begin(), rasterPixels.end(), 0u);
				}
				else {
					const size_t rowBytes = size_t(rasterWidth) * 4u;
					const size_t keptBytes = size_t(rasterWidth - advance) * 4u;
					for (int y = 0; y < rasterHeight; ++y) {
						unsigned char* row = rasterPixels.data() + size_t(y) * rowBytes;
						std::memmove(row, row + size_t(advance) * 4u, keptBytes);
						std::memset(row + keptBytes, 0, size_t(advance) * 4u);
					}
				}

				auto writePixel = [&](int x, int y, NVGcolor color) {
					if (x < 0 || x >= rasterWidth || y < 0 || y >= rasterHeight) return;
					unsigned char* pixel = rasterPixels.data() + (size_t(y) * size_t(rasterWidth) + size_t(x)) * 4u;
					pixel[0] = uint8_t(clamp(color.r * color.a, 0.f, 1.f) * 255.f);
					pixel[1] = uint8_t(clamp(color.g * color.a, 0.f, 1.f) * 255.f);
					pixel[2] = uint8_t(clamp(color.b * color.a, 0.f, 1.f) * 255.f);
					pixel[3] = 255u;
				};
				auto drawSpan = [&](int x, const float* minBuf, const float* maxBuf, float centerY, int index) {
					const float valMin = clamp(minBuf[index] / Sil::kAudioFullScaleV, -1.f, 1.f);
					const float valMax = clamp(maxBuf[index] / Sil::kAudioFullScaleV, -1.f, 1.f);
					const float amp = std::max(std::abs(valMin), std::abs(valMax));
					const int bucket = clamp(int(amp * 16.f), 0, 15);
					const float tint = (float(bucket) + 0.5f) / 16.f;
					const NVGcolor color = nvgLerpRGBA(colors.low, colors.high, tint);
					const float halfH = float(rasterHeight) * 0.25f;
					int y0 = int(std::round(centerY - valMin * halfH));
					int y1 = int(std::round(centerY - valMax * halfH));
					if (y0 > y1) std::swap(y0, y1);
					for (int y = y0; y <= y1; ++y) writePixel(x, y, color);
				};
				const int firstX = rasterWidth - advance;
				for (int x = firstX; x < rasterWidth; ++x) {
					for (int y = 0; y < rasterHeight; ++y) {
						writePixel(x, y, nvgRGBA(0, 0, 0, 255));
					}
					const int index = (lastWritePtr + x) % rasterWidth;
					drawSpan(x, minL.data(), maxL.data(), float(rasterHeight) * 0.25f, index);
					drawSpan(x, minR.data(), maxR.data(), float(rasterHeight) * 0.75f, index);
					writePixel(x, rasterHeight / 2, colors.divider);
				}
				nvgUpdateImage(args.vg, rasterImage, rasterPixels.data());
				nvgBeginPath(args.vg);
				nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
				nvgFillPaint(args.vg, nvgImagePattern(
					args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, rasterImage, 1.f));
				nvgFill(args.vg);
				rasterWritePtr = lastWritePtr;
				rasterColorScheme = int(module->colorScheme);
				if (measurePerf) {
					renderDebugMetrics->recordDraw(
						SilRenderDebugMetrics::HISTOGRAM,
						debug_terminal::elapsedUsSince(drawStart));
				}
				return;
			}
		}

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, colors.bg);
		nvgFill(args.vg);

		float midY = box.size.y / 2.f;
		float halfH = box.size.y / 4.f;

		auto drawChannel = [&](const float* minBuf, const float* maxBuf, float centerY) {
			// Quantize the amplitude tint into a small palette so all lines sharing
			// a tint can be emitted as one NanoVG path instead of 1000 strokes.
			static constexpr int kColorBuckets = 16;
			for (int bucket = 0; bucket < kColorBuckets; ++bucket) {
				nvgBeginPath(args.vg);
				for (int i = 0; i < Sil::HISTOGRAM_BINS; ++i) {
					const int idx = (lastWritePtr + i) % Sil::HISTOGRAM_BINS;
					const float valMin = clamp(minBuf[idx] / Sil::kAudioFullScaleV, -1.f, 1.f);
					const float valMax = clamp(maxBuf[idx] / Sil::kAudioFullScaleV, -1.f, 1.f);
					const float amp = std::max(std::abs(valMin), std::abs(valMax));
					const int ampBucket = clamp(int(amp * float(kColorBuckets)), 0, kColorBuckets - 1);
					if (ampBucket != bucket) continue;
					const float x = float(i) / float(Sil::HISTOGRAM_BINS - 1) * box.size.x;
					nvgMoveTo(args.vg, x, centerY - valMin * halfH);
					nvgLineTo(args.vg, x, centerY - valMax * halfH);
				}
				const float tint = (float(bucket) + 0.5f) / float(kColorBuckets);
				nvgStrokeColor(args.vg, nvgLerpRGBA(colors.low, colors.high, tint));
				nvgStrokeWidth(args.vg, 1.f);
				nvgStroke(args.vg);
			}
		};

		if (module) {
			drawChannel(minL.data(), maxL.data(), midY * 0.5f);
			drawChannel(minR.data(), maxR.data(), midY * 1.5f);
		}
		else {
			for (float centerY : {midY * 0.5f, midY * 1.5f}) {
				nvgBeginPath(args.vg);
				for (int i = 0; i < 48; ++i) {
					const float t = float(i) / 47.f;
					const float envelope = std::sin(float(M_PI) * t);
					const float y = centerY - std::sin(10.f * float(M_PI) * t) * envelope * halfH * 0.34f;
					const float x = t * box.size.x;
					if (i == 0) nvgMoveTo(args.vg, x, y);
					else nvgLineTo(args.vg, x, y);
				}
				nvgStrokeColor(args.vg, nvgLerpRGBA(colors.low, colors.high, 0.45f));
				nvgStrokeWidth(args.vg, 1.f);
				nvgStroke(args.vg);
			}
		}

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0, midY);
		nvgLineTo(args.vg, box.size.x, midY);
		nvgStrokeColor(args.vg, colors.divider);
		nvgStrokeWidth(args.vg, 0.5f);
		nvgStroke(args.vg);
		if (measurePerf) {
			renderDebugMetrics->recordDraw(
				SilRenderDebugMetrics::HISTOGRAM,
				debug_terminal::elapsedUsSince(drawStart));
		}
	}
};

struct SpectrumGridWidget : TransparentWidget {
	SilRenderDebugMetrics* renderDebugMetrics = nullptr;
	SilRenderDebugMetrics::Slot debugSlot = SilRenderDebugMetrics::SPECTRUM_LEFT;

	void draw(const DrawArgs& args) override {
		const bool measurePerf = renderDebugMetrics && isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGB(0, 0, 0));
		nvgFill(args.vg);

		auto getX = [&](float hz) {
			return (std::log10(hz / 20.f) / 3.f) * box.size.x;
		};
		for (int majorPass = 0; majorPass < 2; ++majorPass) {
			nvgBeginPath(args.vg);
			for (float decade = 10.f; decade <= 10000.f; decade *= 10.f) {
				for (int i = 1; i <= 9; ++i) {
					if ((i == 1) != (majorPass != 0)) continue;
					const float frequency = decade * float(i);
					if (frequency < 20.f) continue;
					if (frequency > 20000.f) break;
					const float x = getX(frequency);
					if (x < 0.f || x > box.size.x) continue;
					nvgMoveTo(args.vg, x, 0.f);
					nvgLineTo(args.vg, x, box.size.y);
				}
			}
			if (majorPass == 0) {
				const float x20k = getX(20000.f);
				if (x20k >= 0.f && x20k <= box.size.x) {
					nvgMoveTo(args.vg, x20k, 0.f);
					nvgLineTo(args.vg, x20k, box.size.y);
				}
			}
			nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, majorPass ? 34 : 16));
			nvgStrokeWidth(args.vg, majorPass ? 1.f : 0.7f);
			nvgStroke(args.vg);
		}
		if (measurePerf) {
			renderDebugMetrics->recordDraw(debugSlot, debug_terminal::elapsedUsSince(drawStart));
		}
	}
};

struct SpectrumWidget : TransparentWidget {
	Sil* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	SilRenderDebugMetrics* renderDebugMetrics = nullptr;
	bool isRightChannel = false;
	uint32_t lastSnapshotSeq = uint32_t(-1);
	int lastColorScheme = -1;

	void step() override {
		TransparentWidget::step();
		if (!module || !framebuffer) return;
		const uint32_t snapshotSeq = module->specSnapshotSeq.load(std::memory_order_acquire);
		const int colorScheme = int(module->colorScheme);
		if (snapshotSeq != lastSnapshotSeq || colorScheme != lastColorScheme) {
			uint32_t dirtyReasons = 0u;
			if (lastSnapshotSeq == uint32_t(-1)) dirtyReasons |= SilRenderDebugMetrics::DIRTY_INITIAL;
			if (snapshotSeq != lastSnapshotSeq) dirtyReasons |= SilRenderDebugMetrics::DIRTY_DATA;
			if (colorScheme != lastColorScheme) dirtyReasons |= SilRenderDebugMetrics::DIRTY_PALETTE;
			if (renderDebugMetrics) {
				renderDebugMetrics->markDirty(
					isRightChannel ? SilRenderDebugMetrics::SPECTRUM_RIGHT : SilRenderDebugMetrics::SPECTRUM_LEFT,
					dirtyReasons);
			}
			lastSnapshotSeq = snapshotSeq;
			lastColorScheme = colorScheme;
			framebuffer->setDirty();
		}
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = renderDebugMetrics && isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		// Keep the FFT demand-driven by visible rendering. The step method only
		// invalidates the cache, so an offscreen Sil does not perform UI FFT work.
		if (module && !isRightChannel) module->updateSpectrumDisplayFromLatestSnapshot();
		SilColors colors = SilColors::get(module ? module->colorScheme : Sil::SCHEME_DEFAULT);
		const NVGcolor channelLow = colors.low;
		const NVGcolor channelHigh = colors.high;

		float barW = box.size.x / Sil::SPEC_FREQ_BINS;

		static constexpr int kColorBuckets = 16;
		for (int bucket = 0; bucket < kColorBuckets; ++bucket) {
			nvgBeginPath(args.vg);
			for (int i = 0; i < Sil::SPEC_FREQ_BINS; ++i) {
				float norm = 0.f;
				if (module) {
					norm = isRightChannel ? module->spec.displayNormR[i] : module->spec.displayNormL[i];
				}
				else {
					const float previewX = float(i) / float(Sil::SPEC_FREQ_BINS - 1);
					const float previewShape = std::sin(float(M_PI) * previewX);
					const float previewRipple = 0.08f * std::sin(
						(isRightChannel ? 13.f : 11.f) * float(M_PI) * previewX);
					norm = clamp(0.08f + 0.48f * previewShape + previewRipple, 0.f, 1.f);
				}
				if (norm <= 0.01f) continue;
				const int normBucket = clamp(int(norm * float(kColorBuckets)), 0, kColorBuckets - 1);
				if (normBucket != bucket) continue;
				const float barH = norm * box.size.y;
				const float x = float(i) * barW;
				nvgRect(args.vg, x, box.size.y - barH, barW - 0.5f, barH);
			}
			const float tint = (float(bucket) + 0.5f) / float(kColorBuckets);
			nvgFillColor(args.vg, nvgLerpRGBA(channelLow, channelHigh, tint));
			nvgFill(args.vg);
		}
		if (measurePerf) {
			renderDebugMetrics->recordDraw(
				isRightChannel ? SilRenderDebugMetrics::SPECTRUM_RIGHT : SilRenderDebugMetrics::SPECTRUM_LEFT,
				debug_terminal::elapsedUsSince(drawStart));
		}
	}
};

struct SilStageHistoryWidget : TransparentWidget {
	Sil* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	SilRenderDebugMetrics* renderDebugMetrics = nullptr;
	SilUiRefreshCoordinator* refreshCoordinator = nullptr;
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
	std::array<float, kCount> current {};
	int writeIndex = 0;
	float histogramStartX = 0.f;
	double lastSampleSec = -1.0;
	bool staggerDeferred = false;
	NVGcontext* rasterImageVg = nullptr;
	int rasterImage = -1;
	int rasterWidth = 0;
	int rasterHeight = 0;
	int rasterWriteIndex = -1;
	std::vector<unsigned char> rasterPixels;

	~SilStageHistoryWidget() override {
		nvg_gfx_lifecycle::resetOwnedNvgImage(
			rasterImageVg, rasterImage, rasterWidth, rasterHeight, nullptr, false);
	}

	void step() override {
		TransparentWidget::step();
		if (!module || !framebuffer) return;
		const double nowSec = system::getTime();
		if (lastSampleSec >= 0.0 && nowSec - lastSampleSec < (1.0 / 30.0)) return;
		// Histogram and stage history are the two expensive steady-state caches.
		// When both become due together, move history to the following UI frame.
		// The one-deferral limit prevents starvation at unusually low UI rates.
		if (refreshCoordinator && refreshCoordinator->histogramRefreshedThisStep && !staggerDeferred) {
			staggerDeferred = true;
			return;
		}
		staggerDeferred = false;
		if (renderDebugMetrics) {
			renderDebugMetrics->markDirty(
				SilRenderDebugMetrics::STAGE_HISTORY,
				(lastSampleSec < 0.0 ? SilRenderDebugMetrics::DIRTY_INITIAL : 0u)
					| SilRenderDebugMetrics::DIRTY_DATA);
		}
		lastSampleSec = nowSec;
		for (int i = 0; i < kCount; ++i) {
			current[i] = clamp(module->lights[lightIds[i]].getBrightness(), 0.f, 1.f);
			histories[i][size_t(writeIndex)] = current[i];
		}
		writeIndex = (writeIndex + 1) % kHistBins;
		framebuffer->setDirty();
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = renderDebugMetrics && isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		if (!module || !APP || !APP->window || !APP->window->uiFont) {
			return;
		}

		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 9.0f);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

		char label[16];
		for (int i = 0; i < kCount; ++i) {
			const float b = current[i];
			const int pct = int(std::round(b * 100.f));
			std::snprintf(label, sizeof(label), "%3d%%", pct);

			const Vec p = textPositions[i];
			nvgFillColor(args.vg, nvgRGBA(8, 8, 8, 210));
			nvgText(args.vg, p.x + 0.45f, p.y + 0.45f, label, nullptr);
			nvgFillColor(args.vg, nvgRGBA(245, 245, 245, 255));
			nvgText(args.vg, p.x, p.y, label, nullptr);
		}

		const float x0 = (histogramStartX > 0.f) ? histogramStartX : 0.50f * box.size.x;
		const float rightPad = 6.f;
		const float histW = std::max(24.f, box.size.x - x0 - rightPad);
		const float histH = 8.f;
		const float barW = histW / float(kHistBins);
		(void) barW;

		const int targetWidth = kHistBins;
		const int targetHeight = std::max(2, int(std::ceil(box.size.y)));
		if (rasterImageVg != args.vg) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				rasterImageVg, rasterImage, rasterWidth, rasterHeight, args.vg, false);
			rasterWriteIndex = -1;
		}
		if (rasterImage >= 0 && !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
			args.vg, rasterImage, rasterWidth, rasterHeight)) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				rasterImageVg, rasterImage, rasterWidth, rasterHeight, args.vg, true);
			rasterWriteIndex = -1;
		}
		if (rasterImage < 0 || rasterWidth != targetWidth || rasterHeight != targetHeight) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				rasterImageVg, rasterImage, rasterWidth, rasterHeight, args.vg, true);
			rasterWidth = targetWidth;
			rasterHeight = targetHeight;
			rasterPixels.assign(size_t(rasterWidth * rasterHeight * 4), 0u);
			rasterImage = nvgCreateImageRGBA(
				args.vg, rasterWidth, rasterHeight, NVG_IMAGE_PREMULTIPLIED, rasterPixels.data());
			rasterImageVg = args.vg;
			rasterWriteIndex = -1;
		}

		if (rasterImage >= 0) {
			int advance = rasterWriteIndex < 0 ? rasterWidth
				: (writeIndex - rasterWriteIndex + rasterWidth) % rasterWidth;
			if (rasterWriteIndex < 0 || advance <= 0 || advance >= rasterWidth) {
				advance = rasterWidth;
				std::fill(rasterPixels.begin(), rasterPixels.end(), 0u);
			}
			else {
				const size_t rowBytes = size_t(rasterWidth) * 4u;
				const size_t keptBytes = size_t(rasterWidth - advance) * 4u;
				for (int y = 0; y < rasterHeight; ++y) {
					unsigned char* row = rasterPixels.data() + size_t(y) * rowBytes;
					std::memmove(row, row + size_t(advance) * 4u, keptBytes);
					std::memset(row + keptBytes, 0, size_t(advance) * 4u);
				}
			}
			auto setPixel = [&](int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
				if (x < 0 || x >= rasterWidth || y < 0 || y >= rasterHeight) return;
				unsigned char* p = rasterPixels.data() + (size_t(y) * size_t(rasterWidth) + size_t(x)) * 4u;
				p[0] = uint8_t((uint16_t(r) * a) / 255u);
				p[1] = uint8_t((uint16_t(g) * a) / 255u);
				p[2] = uint8_t((uint16_t(b) * a) / 255u);
				p[3] = a;
			};
			for (int x = rasterWidth - advance; x < rasterWidth; ++x) {
				const int idx = (writeIndex + x) % rasterWidth;
				for (int i = 0; i < kCount; ++i) {
					const int top = int(std::round(textPositions[i].y - 0.5f * histH));
					const int bottom = int(std::round(textPositions[i].y + 0.5f * histH));
					for (int y = top; y <= bottom; ++y) setPixel(x, y, 0, 0, 0, 80);
					const float v = clamp(histories[i][size_t(idx)], 0.f, 1.f);
					const int barTop = int(std::round(float(bottom) - v * histH));
					for (int y = barTop; y <= bottom; ++y) setPixel(x, y, 245, 245, 245, 210);
				}
			}
			nvgUpdateImage(args.vg, rasterImage, rasterPixels.data());
			nvgBeginPath(args.vg);
			nvgRect(args.vg, x0, 0.f, histW, box.size.y);
			nvgFillPaint(args.vg, nvgImagePattern(
				args.vg, x0, 0.f, histW, box.size.y, 0.f, rasterImage, 1.f));
			nvgFill(args.vg);
			rasterWriteIndex = writeIndex;
		}
		if (measurePerf) {
			renderDebugMetrics->recordDraw(
				SilRenderDebugMetrics::STAGE_HISTORY,
				debug_terminal::elapsedUsSince(drawStart));
		}
	}
};

struct SilHorizontalMeter : TransparentWidget {
    Sil* module = nullptr;
    bool dbfs = false;
    bool connected = false;
    float levels[2] = {};
    void step() override {
        TransparentWidget::step();
        connected = module && module->levelMeter.connected.load(std::memory_order_relaxed);
        for (int j=0;j<2;++j) {
            float target=0.f;
            if(connected) {
                const float measure=dbfs ? module->levelMeter.peaks[j].load(std::memory_order_relaxed)
                    : module->levelMeter.power[j].load(std::memory_order_relaxed);
                const float db=dbfs ? 20.f*std::log10(measure+1e-12f)
                    : -.691f+10.f*std::log10(measure+1e-12f);
                target=clamp((db+60.f)/60.f,0.f,1.f);
            }
            levels[j]+=(target-levels[j])*(target>levels[j] ? .42f : .075f);
        }
    }
    void draw(const DrawArgs& args) override {
        const SilColors colors = SilColors::get(module ? module->colorScheme : Sil::SCHEME_DEFAULT);
        const float labelWidth=mm2px(22.f);
        const float x=labelWidth, w=box.size.x-x, h=box.size.y;
        nvgBeginPath(args.vg); nvgRect(args.vg,x,0.f,w,h);
        nvgFillColor(args.vg,colors.bg); nvgFill(args.vg);
        nvgStrokeWidth(args.vg,1.f); nvgStrokeColor(args.vg,colors.divider); nvgStroke(args.vg);
        const float inset=1.25f, gap=1.f, lane=(h-2.f*inset-gap)*.5f;
        for(int j=0;j<2;++j) {
            const float y=inset+j*(lane+gap);
            nvgBeginPath(args.vg); nvgRect(args.vg,x+inset,y,(w-2.f*inset)*levels[j],lane);
            nvgFillPaint(args.vg,nvgLinearGradient(args.vg,x+inset,y,x+w-inset,y,
                colors.low,colors.high)); nvgFill(args.vg);
        }
        if(APP && APP->window && APP->window->uiFont) {
            char text[32];
            if(connected) std::snprintf(text,sizeof(text),"%s  %.1f",dbfs ? "dBFS" : "LUFS",std::max(levels[0],levels[1])*60.f-60.f);
            else std::snprintf(text,sizeof(text),"%s  --",dbfs ? "dBFS" : "LUFS");
            nvgFontFaceId(args.vg,APP->window->uiFont->handle); nvgFontSize(args.vg,9.5f);
            nvgTextAlign(args.vg,NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE); nvgFillColor(args.vg,color::WHITE);
            nvgText(args.vg,0.f,h*.5f,text,nullptr);
        }
    }
};

struct SilWidget : ModuleWidget {
	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
	debug_terminal::UiCycleTimingAccumulator drawLayerTiming;
	SilRenderDebugMetrics renderDebugMetrics;
	SilUiRefreshCoordinator refreshCoordinator;
	std::ofstream timingLogFile;
	std::string timingLogPath;
	bool timingLogActive = false;
	uint64_t timingLogRow = 0u;
	double timingLogStartedAtSec = 0.0;
	double timingLogPreviousFrameSec = 0.0;
	float latestStepUs = 0.f;
	uint32_t timingLogPreviousSnapshotSeq = uint32_t(-1);

	~SilWidget() override {
		stopTimingLog();
	}

	void startTimingLog(Sil* sil) {
		if (!sil || timingLogActive || !isDragonKingDebugEnabled()) {
			return;
		}
		system::createDirectories(Sil::userDebugRootPath());
		static uint32_t openSequence = 0u;
		timingLogPath = system::join(
			Sil::userDebugRootPath(),
			"timings_" + std::to_string(sil->debugMetrics.instanceId) + "_"
				+ std::to_string(std::time(nullptr)) + "_"
				+ std::to_string(openSequence++) + ".csv");
		timingLogFile.open(timingLogPath.c_str(), std::ios::out | std::ios::trunc);
		if (!timingLogFile.is_open()) {
			WARN("Sil failed to open timing CSV: %s", timingLogPath.c_str());
			timingLogPath.clear();
			return;
		}
		timingLogFile << std::fixed << std::setprecision(3);
		timingLogFile
			<< "row,elapsed_sec,module_id,instance_id,process_us,step_us,draw_us,"
			<< "frame_interval_ms,histogram_draw_us,spectrum_left_draw_us,spectrum_right_draw_us,"
			<< "stage_history_draw_us,other_draw_us,"
			<< "histogram_fb_us,spectrum_left_grid_fb_us,spectrum_left_data_fb_us,"
			<< "spectrum_right_grid_fb_us,spectrum_right_data_fb_us,stage_history_fb_us,"
			<< "histogram_fb_dirty,spectrum_left_grid_fb_dirty,spectrum_left_data_fb_dirty,"
			<< "spectrum_right_grid_fb_dirty,spectrum_right_data_fb_dirty,stage_history_fb_dirty,"
			<< "histogram_fb_width,histogram_fb_height,spectrum_left_data_fb_width,spectrum_left_data_fb_height,"
			<< "spectrum_right_data_fb_width,spectrum_right_data_fb_height,stage_history_fb_width,stage_history_fb_height,"
			<< "histogram_drawn,spectrum_left_drawn,spectrum_right_drawn,stage_history_drawn,"
			<< "histogram_dirty_reasons,spectrum_left_dirty_reasons,spectrum_right_dirty_reasons,stage_history_dirty_reasons,"
			<< "histogram_total_draws,spectrum_left_total_draws,spectrum_right_total_draws,stage_history_total_draws,"
			<< "spectrum_snapshot_seq,spectrum_snapshot_changed,histogram_write_ptr,rack_zoom,pixel_ratio\n";
		timingLogActive = true;
		timingLogRow = 0u;
		timingLogStartedAtSec = system::getTime();
		timingLogPreviousFrameSec = 0.0;
		timingLogPreviousSnapshotSeq = uint32_t(-1);
		INFO("Sil started timing CSV: %s", timingLogPath.c_str());
	}

	void stopTimingLog() {
		if (timingLogFile.is_open()) {
			timingLogFile.flush();
			timingLogFile.close();
		}
		if (timingLogActive) {
			INFO("Sil stopped timing CSV: %s", timingLogPath.c_str());
		}
		timingLogActive = false;
		timingLogRow = 0u;
		timingLogStartedAtSec = 0.0;
		timingLogPreviousFrameSec = 0.0;
		timingLogPreviousSnapshotSeq = uint32_t(-1);
	}

	void writeTimingLogRow(Sil* sil, double nowSec, float drawUs) {
		if (!timingLogActive || !timingLogFile.is_open() || !sil) {
			return;
		}
		const float processUs = float(double(
			sil->perfLatestProcessNs.load(std::memory_order_relaxed)) * 0.001);
		const float componentDrawUs =
			renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::HISTOGRAM]
			+ renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::SPECTRUM_LEFT]
			+ renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::SPECTRUM_RIGHT]
			+ renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::STAGE_HISTORY];
		const float otherDrawUs = std::max(0.f, drawUs - componentDrawUs);
		const double frameIntervalMs = timingLogPreviousFrameSec > 0.0
			? (nowSec - timingLogPreviousFrameSec) * 1000.0 : 0.0;
		timingLogPreviousFrameSec = nowSec;
		const uint32_t snapshotSeq = sil->specSnapshotSeq.load(std::memory_order_acquire);
		const bool snapshotChanged = timingLogPreviousSnapshotSeq != uint32_t(-1)
			&& snapshotSeq != timingLogPreviousSnapshotSeq;
		timingLogPreviousSnapshotSeq = snapshotSeq;
		float rackZoom = 1.f;
		if (APP && APP->scene && APP->scene->rackScroll) {
			rackZoom = APP->scene->rackScroll->getZoom();
		}
		const float pixelRatio = (APP && APP->window) ? APP->window->pixelRatio : 1.f;
		timingLogFile
			<< timingLogRow << ','
			<< (nowSec - timingLogStartedAtSec) << ','
			<< sil->id << ','
			<< sil->debugMetrics.instanceId << ','
			<< processUs << ','
			<< latestStepUs << ','
			<< drawUs << ','
			<< frameIntervalMs << ','
			<< renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::HISTOGRAM] << ','
			<< renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::SPECTRUM_LEFT] << ','
			<< renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::SPECTRUM_RIGHT] << ','
			<< renderDebugMetrics.frameDrawUs[SilRenderDebugMetrics::STAGE_HISTORY] << ','
			<< otherDrawUs << ','
			<< renderDebugMetrics.frameCacheUs[SilRenderDebugMetrics::CACHE_HISTOGRAM] << ','
			<< renderDebugMetrics.frameCacheUs[SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_GRID] << ','
			<< renderDebugMetrics.frameCacheUs[SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_DATA] << ','
			<< renderDebugMetrics.frameCacheUs[SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_GRID] << ','
			<< renderDebugMetrics.frameCacheUs[SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_DATA] << ','
			<< renderDebugMetrics.frameCacheUs[SilRenderDebugMetrics::CACHE_STAGE_HISTORY] << ','
			<< renderDebugMetrics.frameCacheDirty[SilRenderDebugMetrics::CACHE_HISTOGRAM] << ','
			<< renderDebugMetrics.frameCacheDirty[SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_GRID] << ','
			<< renderDebugMetrics.frameCacheDirty[SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_DATA] << ','
			<< renderDebugMetrics.frameCacheDirty[SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_GRID] << ','
			<< renderDebugMetrics.frameCacheDirty[SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_DATA] << ','
			<< renderDebugMetrics.frameCacheDirty[SilRenderDebugMetrics::CACHE_STAGE_HISTORY] << ','
			<< renderDebugMetrics.frameCacheWidth[SilRenderDebugMetrics::CACHE_HISTOGRAM] << ','
			<< renderDebugMetrics.frameCacheHeight[SilRenderDebugMetrics::CACHE_HISTOGRAM] << ','
			<< renderDebugMetrics.frameCacheWidth[SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_DATA] << ','
			<< renderDebugMetrics.frameCacheHeight[SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_DATA] << ','
			<< renderDebugMetrics.frameCacheWidth[SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_DATA] << ','
			<< renderDebugMetrics.frameCacheHeight[SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_DATA] << ','
			<< renderDebugMetrics.frameCacheWidth[SilRenderDebugMetrics::CACHE_STAGE_HISTORY] << ','
			<< renderDebugMetrics.frameCacheHeight[SilRenderDebugMetrics::CACHE_STAGE_HISTORY] << ','
			<< renderDebugMetrics.frameDrawn[SilRenderDebugMetrics::HISTOGRAM] << ','
			<< renderDebugMetrics.frameDrawn[SilRenderDebugMetrics::SPECTRUM_LEFT] << ','
			<< renderDebugMetrics.frameDrawn[SilRenderDebugMetrics::SPECTRUM_RIGHT] << ','
			<< renderDebugMetrics.frameDrawn[SilRenderDebugMetrics::STAGE_HISTORY] << ','
			<< renderDebugMetrics.frameDirtyReasons[SilRenderDebugMetrics::HISTOGRAM] << ','
			<< renderDebugMetrics.frameDirtyReasons[SilRenderDebugMetrics::SPECTRUM_LEFT] << ','
			<< renderDebugMetrics.frameDirtyReasons[SilRenderDebugMetrics::SPECTRUM_RIGHT] << ','
			<< renderDebugMetrics.frameDirtyReasons[SilRenderDebugMetrics::STAGE_HISTORY] << ','
			<< renderDebugMetrics.totalDraws[SilRenderDebugMetrics::HISTOGRAM] << ','
			<< renderDebugMetrics.totalDraws[SilRenderDebugMetrics::SPECTRUM_LEFT] << ','
			<< renderDebugMetrics.totalDraws[SilRenderDebugMetrics::SPECTRUM_RIGHT] << ','
			<< renderDebugMetrics.totalDraws[SilRenderDebugMetrics::STAGE_HISTORY] << ','
			<< snapshotSeq << ','
			<< (snapshotChanged ? 1 : 0) << ','
			<< sil->hist.displayWritePtr.load(std::memory_order_acquire) << ','
			<< rackZoom << ','
			<< pixelRatio << '\n';
		if ((timingLogRow++ & 63u) == 0u) {
			timingLogFile.flush();
		}
	}

	SilWidget(Sil* module) {
		setModule(module);
		PreviewBuildLogTimer previewBuildTimer("Sil", module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/sil.svg");
		visual_assets::SplitPanelRenderer splitPanel(this, "res/sil.panel.svg");
		visual_assets::addFractalGlassOverlay(
			this, splitPanel.panelPath(), splitPanel.panelSurfaceEffectWidget());
		splitPanel.addThemedLabels("res/sil.labels.svg",
			"res/sil.theme-text-input.svg", "res/sil.theme-text-output.svg");
		visual_assets::addPerfectWavePanelBranding(this, panelPath);
		{
			math::Rect logoRectMm(Vec(34.44015f, 119.43102f), Vec(32.71933f, 12.24054f));
			panel_svg::loadRectFromSvgMm(
				panelPath, "BRANDING_LEVIATHAN_LOGO_RASTER", &logoRectMm);
			addChild(visual_assets::createAspectFitRasterImageWidget(
				"res/icon/Leviathan_Logo_S2.png", logoRectMm));
		}
		previewBuildTimer.markPanelDone();
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		auto addCachedDisplay = [&](const math::Rect& rectPx, Widget* content,
			SilRenderDebugMetrics::CacheSlot cacheSlot) {
			SilTimedFramebufferWidget* framebuffer = new SilTimedFramebufferWidget;
			framebuffer->box = rectPx;
			framebuffer->dirtyOnSubpixelChange = false;
			framebuffer->renderDebugMetrics = &renderDebugMetrics;
			framebuffer->debugSlot = cacheSlot;
			content->box.size = rectPx.size;
			framebuffer->addChild(content);
			addChild(framebuffer);
			return framebuffer;
		};

		math::Rect histRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "HISTOGRAM", &histRect)) {
			histRect = histRect.grow(Vec(-0.2f, -0.2f));
			HistogramWidget* hw = new HistogramWidget;
			hw->module = module;
			hw->renderDebugMetrics = &renderDebugMetrics;
			hw->refreshCoordinator = &refreshCoordinator;
			hw->framebuffer = addCachedDisplay(
				math::Rect(mm2px(histRect.pos), mm2px(histRect.size)), hw,
				SilRenderDebugMetrics::CACHE_HISTOGRAM);
		}

		math::Rect specLRect;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_LEFT", &specLRect)) {
			specLRect = specLRect.grow(Vec(-0.2f, -0.2f));
			const math::Rect spectrumRectPx(mm2px(specLRect.pos), mm2px(specLRect.size));
			SpectrumGridWidget* grid = new SpectrumGridWidget;
			grid->renderDebugMetrics = &renderDebugMetrics;
			grid->debugSlot = SilRenderDebugMetrics::SPECTRUM_LEFT;
			addCachedDisplay(spectrumRectPx, grid, SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_GRID);
			SpectrumWidget* sw = new SpectrumWidget;
			sw->module = module;
			sw->isRightChannel = false;
			sw->renderDebugMetrics = &renderDebugMetrics;
			sw->framebuffer = addCachedDisplay(
				spectrumRectPx, sw, SilRenderDebugMetrics::CACHE_SPECTRUM_LEFT_DATA);
		}

		math::Rect specRRect;
		float sideSpecLeftX = mm2px(Vec(51.f, 0.f)).x;
		if (panel_svg::loadRectFromSvgMm(panelPath, "SPECTROGRAM_RIGHT", &specRRect)) {
			specRRect = specRRect.grow(Vec(-0.2f, -0.2f));
			sideSpecLeftX = mm2px(specRRect.pos).x;
			const math::Rect spectrumRectPx(mm2px(specRRect.pos), mm2px(specRRect.size));
			SpectrumGridWidget* grid = new SpectrumGridWidget;
			grid->renderDebugMetrics = &renderDebugMetrics;
			grid->debugSlot = SilRenderDebugMetrics::SPECTRUM_RIGHT;
			addCachedDisplay(spectrumRectPx, grid, SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_GRID);
			SpectrumWidget* sw = new SpectrumWidget;
			sw->module = module;
			sw->isRightChannel = true;
			sw->renderDebugMetrics = &renderDebugMetrics;
			sw->framebuffer = addCachedDisplay(
				spectrumRectPx, sw, SilRenderDebugMetrics::CACHE_SPECTRUM_RIGHT_DATA);
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
		applyPointOverride("IMPACT_AIR_LIGHT", &impactAirLightPos);
		applyPointOverride("REMOVE_MUD_LIGHT", &removeMudLightPos);
		applyPointOverride("MID_ENHANCE_LIGHT", &midEnhanceLightPos);
		applyPointOverride("GLUE_COMP_LIGHT", &glueCompLightPos);
		applyPointOverride("STEREO_ENHANCE_LIGHT", &stereoEnhanceLightPos);
		applyPointOverride("SATURATOR_LIGHT", &saturatorLightPos);
		applyPointOverride("MASTERING_ENABLED_PARAM", &masteringButtonPos);
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();
        for(int i=0;i<2;++i) {
            math::Rect rectMm(Vec(11.7f,88.f+i*6.f),Vec(78.f,4.5f));
            panel_svg::loadRectFromSvgMm(panelPath,i==0 ? "LUFS_METER" : "DBFS_METER",&rectMm);
            auto* meter=new SilHorizontalMeter;
            meter->module=module; meter->dbfs=i==1;
            meter->box.pos=mm2px(rectMm.pos); meter->box.size=mm2px(rectMm.size);
            addChild(meter);
        }


		addInput(createInputCentered<Magitek2InputJack>(mm2px(inputLPos), module, Sil::INPUT_L_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(inputRPos), module, Sil::INPUT_R_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outputLPos), module, Sil::OUTPUT_L_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outputRPos), module, Sil::OUTPUT_R_OUTPUT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(
			mm2px(masteringButtonPos), module, Sil::MASTERING_ENABLED_PARAM, Sil::MASTERING_ENABLED_LIGHT
		));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(limiterLightPos), module, Sil::LIMITER_ACTIVE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(lowRecoveryLightPos), module, Sil::LOW_RECOVERY_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(impactAirLightPos), module, Sil::IMPACT_AIR_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(removeMudLightPos), module, Sil::REMOVE_MUD_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(midEnhanceLightPos), module, Sil::MID_ENHANCE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(glueCompLightPos), module, Sil::GLUE_COMP_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(stereoEnhanceLightPos), module, Sil::STEREO_ENHANCE_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(saturatorLightPos), module, Sil::SATURATOR_LIGHT));

		SilStageHistoryWidget* chainLedReadout = new SilStageHistoryWidget;
		chainLedReadout->module = module;
		chainLedReadout->renderDebugMetrics = &renderDebugMetrics;
		chainLedReadout->refreshCoordinator = &refreshCoordinator;
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
		float historyTop = chainLedReadout->textPositions[0].y;
		float historyBottom = historyTop;
		float historyLabelLeft = chainLedReadout->textPositions[0].x;
		for (const Vec& position : chainLedReadout->textPositions) {
			historyTop = std::min(historyTop, position.y);
			historyBottom = std::max(historyBottom, position.y);
			historyLabelLeft = std::min(historyLabelLeft, position.x);
		}
		const Vec historyOrigin(
			std::max(0.f, historyLabelLeft - 34.f),
			std::max(0.f, historyTop - 6.f));
		const Vec historyEnd(
			std::max(historyOrigin.x + 1.f, box.size.x - 6.f),
			std::min(box.size.y, historyBottom + 6.f));
		for (Vec& position : chainLedReadout->textPositions) position = position.minus(historyOrigin);
		chainLedReadout->histogramStartX -= historyOrigin.x;
		chainLedReadout->framebuffer = addCachedDisplay(
			math::Rect(historyOrigin, historyEnd.minus(historyOrigin)), chainLedReadout,
			SilRenderDebugMetrics::CACHE_STAGE_HISTORY);

	}

	void step() override {
		drawLayerTiming.beginCycle(module && isDragonKingDebugEnabled());
		const bool measurePerf = isDragonKingDebugEnabled();
		if (!measurePerf && timingLogActive) {
			stopTimingLog();
		}
		const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
		refreshCoordinator.histogramRefreshedThisStep = false;
		ModuleWidget::step();
		if (measurePerf) {
			latestStepUs = debug_terminal::elapsedUsSince(stepStart);
			debugWidgetMetrics.recordStep(latestStepUs);
		}
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		const bool measurePerf = drawLayerTiming.enabled;
		const auto start = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::drawLayer(args, layer);
		if (measurePerf) drawLayerTiming.add(debug_terminal::elapsedUsSince(start));
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		if (measurePerf) {
			renderDebugMetrics.beginFrame();
		}
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::draw(args);

		Sil* sil = dynamic_cast<Sil*>(module);
		if (!sil || !measurePerf) {
			return;
		}

		debug_terminal::drawDebugInstanceId(args.vg, box.size, sil->debugMetrics.instanceId);
		const float drawUs = debug_terminal::elapsedUsSince(drawStart);
		debugWidgetMetrics.recordDraw(drawUs);
		const double nowSec = system::getTime();
		writeTimingLogRow(sil, nowSec, drawUs);
		if (debug_terminal::baselineSubmitDue("Sil", sil->debugMetrics.instanceId, nowSec)) {
			debug_terminal::submitBaselineMetrics(
				"Sil",
				sil->debugMetrics.instanceId,
				sil->debugMetrics.consumeProcessRange(),
				debugWidgetMetrics.consumeStepRange(),
				debugWidgetMetrics.consumeDrawRange(), drawLayerTiming.consume());
		}
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
			menu->addChild(new MenuSeparator());
			menu->addChild(createMenuLabel("Debug"));
			menu->addChild(createCheckMenuItem(
				"Timing log (CSV)", "",
				[this]() { return timingLogActive; },
				[this, sil]() {
					if (timingLogActive) {
						stopTimingLog();
					}
					else {
						startTimingLog(sil);
					}
				}));
			if (!timingLogPath.empty()) {
				menu->addChild(createMenuLabel(
					std::string(timingLogActive ? "Writing: " : "Last log: ") + timingLogPath));
			}

		}
	}
};

Model* modelSil = createModel<Sil, SilWidget>("Sil");
