#include "BifurxRenderClient.hpp"
#include "BifurxWorker.hpp"

#include <chrono>
#include <utility>

namespace bifurx {

bool BifurxRenderClient::ensureRegistered() {
	if (currentDisplayId) return true;
	if (!service.start()) return false;
	currentDisplayId = service.registerDisplay();
	return currentDisplayId != 0;
}

void BifurxRenderClient::release() {
	if (!currentDisplayId && !lastRequestSeq && !appliedRequestSeq &&
		!submittedPreviewSeq && !appliedPreviewSeq && !submittedAnalysisSeq &&
		!appliedAnalysisSeq && !latestSnapshot && !allocatedPayloadSlots() && submitUs == 0.f)
		return;
	const uint64_t oldId = currentDisplayId;
	currentDisplayId = 0;
	if (oldId) service.unregisterDisplay(oldId);
	lastRequestSeq = appliedRequestSeq = 0;
	submittedPreviewSeq = appliedPreviewSeq = 0;
	submittedAnalysisSeq = appliedAnalysisSeq = 0;
	latestSnapshot.reset();
	for (auto& payload : payloadPool) payload.reset();
	poolCursor = 0;
	submitUs = 0.f;
}

std::shared_ptr<BifurxUiRenderPayload> BifurxRenderClient::tryAcquirePayload() {
	for (size_t attempt = 0; attempt < kAnalysisFramePoolSize; ++attempt) {
		const size_t index = (poolCursor + attempt) % kAnalysisFramePoolSize;
		auto& payload = payloadPool[index];
		if (!payload) payload = std::make_shared<BifurxUiRenderPayload>();
		if (payload.use_count() == 1) {
			poolCursor = (index + 1) % kAnalysisFramePoolSize;
			return payload;
		}
	}
	return nullptr;
}

bool BifurxRenderClient::submit(BifurxUiRenderRequest request,
	const std::chrono::steady_clock::time_point* timingStart) {
	if (!currentDisplayId) return false;
	request.displayId = currentDisplayId;
	request.requestSeq = lastRequestSeq + 1;
	const uint32_t previewSeq = request.previewSeq;
	const uint32_t analysisSeq = request.analysisSeq;
	if (!service.submitLatest(std::move(request))) return false;
	++lastRequestSeq;
	submittedPreviewSeq = previewSeq;
	submittedAnalysisSeq = analysisSeq;
	if (timingStart) {
		submitUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - *timingStart).count()) * 1e-3f;
	}
	return true;
}

std::shared_ptr<const BifurxUiRenderSnapshot> BifurxRenderClient::pollLatest() const {
	if (!currentDisplayId || appliedRequestSeq >= lastRequestSeq) return nullptr;
	auto candidate = service.getLatestSnapshot(currentDisplayId);
	if (!candidate || candidate->displayId != currentDisplayId
		|| candidate->requestSeq <= appliedRequestSeq) return nullptr;
	return candidate;
}

void BifurxRenderClient::acknowledge(std::shared_ptr<const BifurxUiRenderSnapshot> snapshot) {
	appliedRequestSeq = snapshot->requestSeq;
	appliedPreviewSeq = snapshot->previewSeq;
	latestSnapshot = std::move(snapshot);
}

size_t BifurxRenderClient::allocatedPayloadSlots() const {
	size_t count = 0;
	for (const auto& payload : payloadPool) count += bool(payload);
	return count;
}

} // namespace bifurx
