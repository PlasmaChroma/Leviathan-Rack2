#include "BifurxWorker.hpp"
#include "BifurxRenderPrep.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace bifurx {

namespace {

std::atomic<int> gBifurxVisualWorkerDefaultMode {VISUAL_WORKER_ON};
std::atomic<bool> gBifurxVisualWorkerSettingsLoaded {false};
std::atomic<bool> gBifurxRenderServiceShuttingDown {false};

void loadBifurxVisualWorkerDefaultModeIfNeeded() {
	if (gBifurxVisualWorkerSettingsLoaded.load(std::memory_order_acquire)) {
		return;
	}
	if (gBifurxVisualWorkerSettingsLoaded.load(std::memory_order_relaxed)) {
		return;
	}
	// Global default is intentionally fixed to ON.
	gBifurxVisualWorkerDefaultMode.store(VISUAL_WORKER_ON, std::memory_order_relaxed);
	gBifurxVisualWorkerSettingsLoaded.store(true, std::memory_order_release);
}

struct DisplaySlot {
	bool active = true;
	bool inFlight = false;
	bool queued = false;
	bool hasPending = false;
	BifurxUiRenderRequest pending;
	std::shared_ptr<const BifurxUiRenderSnapshot> latestSnapshot;
};

} // namespace

struct BifurxUiRenderService::Impl {
	mutable std::mutex mutex;
	std::condition_variable cv;
	std::unordered_map<uint64_t, DisplaySlot> slots;
	std::deque<uint64_t> readyQueue;
	std::thread thread;
	bool running = false;
	bool stopRequested = false;
	uint64_t nextDisplayId = 1;

	void run() {
		for (;;) {
			BifurxUiRenderRequest request;
			bool found = false;
			std::shared_ptr<const BifurxUiRenderSnapshot> previousSnapshot;
			{
				std::unique_lock<std::mutex> lock(mutex);
				cv.wait(lock, [&]() {
					if (stopRequested) {
						return true;
					}
					return !readyQueue.empty();
				});
				if (stopRequested) {
					return;
				}
				while (!readyQueue.empty()) {
					const uint64_t displayId = readyQueue.front();
					readyQueue.pop_front();
					auto it = slots.find(displayId);
					if (it == slots.end()) {
						continue;
					}
					DisplaySlot& slot = it->second;
					slot.queued = false;
					if (!slot.active || !slot.hasPending || slot.inFlight) {
						continue;
					}
					request = slot.pending;
					slot.hasPending = false;
					slot.inFlight = true;
					previousSnapshot = slot.latestSnapshot;
					found = true;
					break;
				}
			}
			if (!found) {
				continue;
			}

			auto snapshot = std::make_shared<BifurxUiRenderSnapshot>();
			if (previousSnapshot && previousSnapshot->previewSeq == request.previewSeq && previousSnapshot->hasCurveTarget) {
				request.skipCurvePrep = true;
				snapshot->cachedAxisSampleRate = previousSnapshot->cachedAxisSampleRate;
				snapshot->hasCurveTarget = true;
				for (int i = 0; i < kCurvePointCount; ++i) {
					snapshot->curveHz[i] = previousSnapshot->curveHz[i];
					snapshot->curveBinPos[i] = previousSnapshot->curveBinPos[i];
					snapshot->curveTargetDb[i] = previousSnapshot->curveTargetDb[i];
				}
			}
			prepareCurveSnapshot(request, snapshot.get());
			snapshot->completedAtSec = system::getTime();

			{
				std::lock_guard<std::mutex> lock(mutex);
				auto it = slots.find(request.displayId);
				if (it == slots.end() || !it->second.active) {
					continue;
				}
				DisplaySlot& slot = it->second;
				slot.latestSnapshot = snapshot;
				slot.inFlight = false;
				if (slot.active && slot.hasPending && !slot.queued) {
					slot.queued = true;
					readyQueue.push_back(request.displayId);
				}
			}
		}
	}
};

BifurxUiRenderService::BifurxUiRenderService() : impl(new Impl()) {}
BifurxUiRenderService::~BifurxUiRenderService() {
	stop();
}

void BifurxUiRenderService::start() {
	std::lock_guard<std::mutex> lock(impl->mutex);
	if (impl->running) {
		return;
	}
	impl->stopRequested = false;
	impl->thread = std::thread([this]() { impl->run(); });
	impl->running = true;
}

void BifurxUiRenderService::stop() {
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		if (!impl->running) {
			return;
		}
		impl->stopRequested = true;
	}
	impl->cv.notify_all();
	if (impl->thread.joinable()) {
		impl->thread.join();
	}
	std::lock_guard<std::mutex> lock(impl->mutex);
	impl->running = false;
	impl->stopRequested = false;
}

uint64_t BifurxUiRenderService::registerDisplay() {
	if (gBifurxRenderServiceShuttingDown.load(std::memory_order_acquire)) {
		return 0;
	}
	std::lock_guard<std::mutex> lock(impl->mutex);
	const uint64_t id = impl->nextDisplayId++;
	impl->slots[id] = DisplaySlot {};
	return id;
}

void BifurxUiRenderService::unregisterDisplay(uint64_t displayId) {
	bool shouldStop = false;
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		auto it = impl->slots.find(displayId);
		if (it != impl->slots.end()) {
			it->second.active = false;
			it->second.queued = false;
			impl->slots.erase(it);
		}
		if (impl->slots.empty() && impl->running) {
			shouldStop = true;
			impl->stopRequested = true;
		}
	}
	if (!shouldStop) {
		return;
	}
	impl->cv.notify_all();
	if (impl->thread.joinable()) {
		impl->thread.join();
	}
	std::lock_guard<std::mutex> relock(impl->mutex);
	impl->running = false;
	impl->stopRequested = false;
}

void BifurxUiRenderService::submitLatest(const BifurxUiRenderRequest& request) {
	if (gBifurxRenderServiceShuttingDown.load(std::memory_order_acquire)) {
		return;
	}
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		auto it = impl->slots.find(request.displayId);
		if (it == impl->slots.end() || !it->second.active) {
			return;
		}
		DisplaySlot& slot = it->second;
		slot.pending = request;
		slot.hasPending = true;
		if (!slot.inFlight && !slot.queued) {
			slot.queued = true;
			impl->readyQueue.push_back(request.displayId);
		}
	}
	impl->cv.notify_one();
}

std::shared_ptr<const BifurxUiRenderSnapshot> BifurxUiRenderService::getLatestSnapshot(uint64_t displayId) const {
	if (gBifurxRenderServiceShuttingDown.load(std::memory_order_acquire)) {
		return nullptr;
	}
	std::lock_guard<std::mutex> lock(impl->mutex);
	auto it = impl->slots.find(displayId);
	if (it == impl->slots.end() || !it->second.active) {
		return nullptr;
	}
	return it->second.latestSnapshot;
}

BifurxUiRenderService& bifurxRenderService() {
	static BifurxUiRenderService service;
	return service;
}

void shutdownBifurxRenderService() {
	gBifurxRenderServiceShuttingDown.store(true, std::memory_order_release);
	bifurxRenderService().stop();
}

void setBifurxVisualWorkerDefaultMode(int mode) {
	(void) mode;
	loadBifurxVisualWorkerDefaultModeIfNeeded();
	gBifurxVisualWorkerDefaultMode.store(VISUAL_WORKER_ON, std::memory_order_relaxed);
}

int getBifurxVisualWorkerDefaultMode() {
	loadBifurxVisualWorkerDefaultModeIfNeeded();
	return gBifurxVisualWorkerDefaultMode.load(std::memory_order_relaxed);
}

} // namespace bifurx
