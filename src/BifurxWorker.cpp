#include "BifurxWorker.hpp"
#include "BifurxRenderPrep.hpp"

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace bifurx {

namespace {

std::atomic<int> gBifurxVisualWorkerDefaultMode {VISUAL_WORKER_OFF};
std::atomic<bool> gBifurxVisualWorkerSettingsLoaded {false};
std::atomic<bool> gBifurxRenderServiceShuttingDown {false};
std::mutex gBifurxVisualWorkerSettingsMutex;

std::string bifurxVisualWorkerSettingsPath() {
	return system::join(asset::user(), "Leviathan/Bifurx/visual_worker_settings.json");
}

void saveBifurxVisualWorkerDefaultModeUnlocked(int mode) {
	system::createDirectories(system::join(asset::user(), "Leviathan/Bifurx"));
	json_t* root = json_object();
	json_object_set_new(root, "visualWorkerDefaultMode", json_integer(mode));
	json_dump_file(root, bifurxVisualWorkerSettingsPath().c_str(), JSON_INDENT(2));
	json_decref(root);
}

void loadBifurxVisualWorkerDefaultModeIfNeeded() {
	if (gBifurxVisualWorkerSettingsLoaded.load(std::memory_order_acquire)) {
		return;
	}
	std::lock_guard<std::mutex> lock(gBifurxVisualWorkerSettingsMutex);
	if (gBifurxVisualWorkerSettingsLoaded.load(std::memory_order_relaxed)) {
		return;
	}
	const std::string settingsPath = bifurxVisualWorkerSettingsPath();
	json_error_t error {};
	json_t* root = json_load_file(settingsPath.c_str(), 0, &error);
	if (root && json_is_object(root)) {
		json_t* modeJ = json_object_get(root, "visualWorkerDefaultMode");
		if (modeJ && json_is_integer(modeJ)) {
			const int loadedMode = int(json_integer_value(modeJ));
			gBifurxVisualWorkerDefaultMode.store(
				clamp(loadedMode, VISUAL_WORKER_OFF, VISUAL_WORKER_ON),
				std::memory_order_relaxed
			);
		}
	}
	if (root) {
		json_decref(root);
	}
	gBifurxVisualWorkerSettingsLoaded.store(true, std::memory_order_release);
}

struct DisplaySlot {
	bool active = true;
	bool inFlight = false;
	bool hasPending = false;
	BifurxUiRenderRequest pending;
	std::shared_ptr<const BifurxUiRenderSnapshot> latestSnapshot;
};

} // namespace

struct BifurxUiRenderService::Impl {
	mutable std::mutex mutex;
	std::condition_variable cv;
	std::unordered_map<uint64_t, DisplaySlot> slots;
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
					for (auto& kv : slots) {
						DisplaySlot& slot = kv.second;
						if (slot.active && slot.hasPending && !slot.inFlight) {
							return true;
						}
					}
					return false;
				});
				if (stopRequested) {
					return;
				}
				for (auto& kv : slots) {
					DisplaySlot& slot = kv.second;
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
		it->second.pending = request;
		it->second.hasPending = true;
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
	loadBifurxVisualWorkerDefaultModeIfNeeded();
	const int clamped = clamp(mode, VISUAL_WORKER_OFF, VISUAL_WORKER_ON);
	gBifurxVisualWorkerDefaultMode.store(clamped, std::memory_order_relaxed);
	std::lock_guard<std::mutex> lock(gBifurxVisualWorkerSettingsMutex);
	saveBifurxVisualWorkerDefaultModeUnlocked(clamped);
}

int getBifurxVisualWorkerDefaultMode() {
	loadBifurxVisualWorkerDefaultModeIfNeeded();
	return gBifurxVisualWorkerDefaultMode.load(std::memory_order_relaxed);
}

} // namespace bifurx
