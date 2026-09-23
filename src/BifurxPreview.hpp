#pragma once

#include "BifurxDsp.hpp"

namespace bifurx {

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b);
BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state);
std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz);
float previewModelResponseDb(const BifurxPreviewModel& model, float hz);
#if defined(BIFURX_DISPLAY_TEST_HOOKS)
std::complex<float> linearBoundaryResponseForTest(int boundary, double omega);
#endif

constexpr float kPreviewProbeLevelKnob = 0.5f;

// Authored module-browser scene. These values intentionally affect only the
// no-engine preview; normal module defaults and existing patches remain unchanged.
constexpr int kBrowserPreviewMode = 1;
constexpr float kBrowserPreviewLevel = 0.5f;
constexpr float kBrowserPreviewFrequency = 0.57605934143066406f;
constexpr float kBrowserPreviewResonance = 0.61867547035217285f;
constexpr float kBrowserPreviewBalance = 0.41204833984375f;
constexpr float kBrowserPreviewSpan = 0.75362539291381836f;
constexpr float kBrowserPreviewFmAmount = 0.0040000001899898052f;
constexpr float kBrowserPreviewSpanAttenuator = 0.f;
constexpr float kBrowserPreviewTito = 0.5f;

float previewProbeStimulusSample(const BifurxPreviewState& state, int sampleIndex);

struct BifurxProbeEngineState {
	TptSvf svfA;
	TptSvf svfB;
};

SvfOutputs processProbeStage(
	BifurxProbeEngineState& state,
	int stageIndex,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm,
	bool highResonanceSelfOscEnabled
);

void simulatePreviewProbeResponse(
	const BifurxPreviewState& state,
	float* inputBuffer,
	float* outputBuffer,
	int sampleCount
);

} // namespace bifurx
