#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace bifurx {

class BifurxUiRenderService;
struct BifurxUiRenderPayload;
struct BifurxUiRenderRequest;
struct BifurxUiRenderSnapshot;
BifurxUiRenderService& bifurxRenderService();

// UI-thread owner of one render-service registration and its reusable requests.
// Display state and backend resources remain with the spectrum controller.
class BifurxRenderClient {
public:
	static constexpr size_t kAnalysisFramePoolSize = 3;
	explicit BifurxRenderClient(BifurxUiRenderService& service = bifurxRenderService()) : service(service) {}
	~BifurxRenderClient() { release(); }
	BifurxRenderClient(const BifurxRenderClient&) = delete;
	BifurxRenderClient& operator=(const BifurxRenderClient&) = delete;

	bool ensureRegistered();
	void release();
	std::shared_ptr<BifurxUiRenderPayload> tryAcquirePayload();
	bool submit(BifurxUiRenderRequest request,
		const std::chrono::steady_clock::time_point* timingStart = nullptr);
	std::shared_ptr<const BifurxUiRenderSnapshot> pollLatest() const;

	uint64_t displayId() const { return currentDisplayId; }
	uint64_t requestSeq() const { return lastRequestSeq; }
	uint64_t lastAppliedRequestSeq() const { return appliedRequestSeq; }
	uint32_t lastSubmittedPreviewSeq() const { return submittedPreviewSeq; }
	uint32_t lastSubmittedAnalysisSeq() const { return submittedAnalysisSeq; }
	uint32_t lastAppliedPreviewSeq() const { return appliedPreviewSeq; }
	uint32_t lastAppliedAnalysisSeq() const { return appliedAnalysisSeq; }
	const std::shared_ptr<const BifurxUiRenderSnapshot>& snapshot() const { return latestSnapshot; }
	float lastSubmitUs() const { return submitUs; }
	void acknowledge(std::shared_ptr<const BifurxUiRenderSnapshot> snapshot);
	void acknowledgeAnalysis(uint32_t seq) { appliedAnalysisSeq = seq; }
	size_t allocatedPayloadSlots() const;

private:
	BifurxUiRenderService& service;
	uint64_t currentDisplayId = 0;
	uint64_t lastRequestSeq = 0;
	uint64_t appliedRequestSeq = 0;
	uint32_t submittedPreviewSeq = 0;
	uint32_t appliedPreviewSeq = 0;
	uint32_t submittedAnalysisSeq = 0;
	uint32_t appliedAnalysisSeq = 0;
	std::shared_ptr<const BifurxUiRenderSnapshot> latestSnapshot;
	std::array<std::shared_ptr<BifurxUiRenderPayload>, kAnalysisFramePoolSize> payloadPool {};
	size_t poolCursor = 0;
	float submitUs = 0.f;
};

} // namespace bifurx
