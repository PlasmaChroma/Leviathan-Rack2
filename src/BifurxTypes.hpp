#pragma once

#include <complex>
#include <cstdint>

namespace bifurx {

// Constants
constexpr float kDefaultPanelWidthMm = 71.12f;
constexpr float kDefaultPanelHeightMm = 128.5f;
constexpr float kPi = 3.14159265358979323846f;
constexpr float kLog2e = 1.4426950408889634f;
constexpr float kFreqMinHz = 4.f;
constexpr float kFreqMaxHz = 28000.f;
constexpr float kFreqLog2Span = 12.7731392f; // log2(28000 / 4)
constexpr float kSvfDampingMin = 0.02f;
constexpr float kSvfDampingMax = 2.2f;
constexpr int kCurvePointCount = 513;
constexpr int kFftSize = 4096;
constexpr int kFftBinCount = kFftSize / 2 + 1;
constexpr int kFftHopSize = kFftSize / 2;
constexpr int kSnapshotSlotCount = 3;
constexpr int kAnalysisFrameSlotCount = 4;
constexpr int kPreviewPublishFastDivision = 128;
constexpr int kPreviewPublishSlowDivision = 256;
constexpr int kPerfMeasureDivision = 17;
constexpr float kLlTelemetryTauSeconds = 0.05f;
constexpr float kPreviewInstantSettleMotionOctThreshold = 2e-5f;
constexpr int kPreviewInstantSettleHoldSamples = 96;
constexpr int kBifurxModeCount = 11;
constexpr int kBifurxDisplayOnlyMode = 10;
constexpr int kBifurxUiModeCount = kBifurxModeCount;
constexpr int kBifurxModeParamIndex = 0;
extern const char* const kBifurxModeLabels[kBifurxModeCount];

inline bool isBifurxDisplayOnlyMode(int mode) {
	return mode == kBifurxDisplayOnlyMode;
}

constexpr float kResponseMinDb = -48.f;
constexpr float kResponseMaxDb = 48.f;
constexpr float kOverlayDbfsFloor = -96.f;
constexpr float kOverlayDbfsCeiling = 6.f;
constexpr float kOverlaySubsonicCutHz = 10.f;
constexpr float kOverlaySubsonicFadeHz = 30.f;
constexpr float kTitoCoeffRelativeUpdateThreshold = 2.5e-4f;
constexpr float kTitoCoeffAbsoluteUpdateThresholdHz = 0.002f;
constexpr float kDisplayDbfsSpan = 48.f;
constexpr float kDisplayTopDbfsFloor = -36.f;
constexpr float kDisplayTopDbfsCeiling = 0.f;
constexpr float kDisplayTopDynamicCeilingDbfs = kOverlayDbfsCeiling;
constexpr float kDisplayPeakHeadroomDb = 0.6f;
constexpr float kCurveVisualSlewDbPerSec = 170.f;
constexpr float kPeakMarkerFillRadius = 2.2f;
constexpr float kPeakMarkerOutlineExtraRadius = 0.4f;
constexpr float kPeakMarkerOutlineStrokeWidth = 0.8f;
constexpr float kPeakMarkerEdgePadding = 0.4f;
constexpr float kPeakMarkerBottomLanePadding = 0.f;

struct DisplayBiquad {
	double b0 = 0.f;
	double b1 = 0.f;
	double b2 = 0.f;
	double a1 = 0.f;
	double a2 = 0.f;

	std::complex<double> response(double omega) const;
	std::complex<double> response(std::complex<double> z1, std::complex<double> z2) const;
};

struct BifurxPreviewState {
	float sampleRate = 44100.f;
	float freqA = 440.f;
	float freqB = 440.f;
	float qA = 1.f;
	float qB = 1.f;
	float balance = 0.f;
	float balanceTarget = 0.f;
	float resoNorm = 0.f;
	float spanParamNorm = 0.5f;
	float spanCvNorm = 0.f;
	float spanAtten = 0.f;
	float spanNorm = 0.5f;
	float spanOct = 0.f;
	float freqParamNorm = 0.5f;
	float voctCv = 0.f;
	int mode = 0;
	int boundary = 2;
};

struct BifurxLlTelemetryState {
	bool active = false;
	float excitationRms = 0.f;
	float stageALpRms = 0.f;
	float stageBLpRms = 0.f;
	float outputRms = 0.f;
	float stageBLpOverALpDb = 0.f;
	float outputOverInputDb = 0.f;
};

struct BifurxPreviewModel {
	DisplayBiquad lowA;
	DisplayBiquad bandA;
	DisplayBiquad highA;
	DisplayBiquad notchA;
	DisplayBiquad lowB;
	DisplayBiquad bandB;
	DisplayBiquad highB;
	DisplayBiquad notchB;
	float markerFreqA = 440.f;
	float markerFreqB = 440.f;
	float sampleRate = 44100.f;
	float qA = 1.f;
	float qB = 1.f;
	float resoNorm = 0.f;
	float wA = 1.f;
	float wB = 1.f;
	int mode = 0;
	int boundary = 2;
};

struct BifurxAnalysisFrame {
	alignas(16) float rawInput[kFftSize] = {};
	alignas(16) float output[kFftSize] = {};
};

} // namespace bifurx
