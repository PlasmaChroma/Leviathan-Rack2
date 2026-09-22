#include "BifurxWorker.hpp"
#include "BifurxRenderPrep.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <deque>
#include <limits>
#include <mutex>
#include <system_error>
#include <thread>
#include <unordered_map>
#include <utility>

namespace bifurx {
namespace {

std::atomic<int> gBifurxVisualWorkerDefaultMode {VISUAL_WORKER_ON};
std::atomic<bool> gBifurxVisualWorkerSettingsLoaded {false};

void loadBifurxVisualWorkerDefaultModeIfNeeded() {
	if (gBifurxVisualWorkerSettingsLoaded.load(std::memory_order_acquire)) return;
	gBifurxVisualWorkerDefaultMode.store(VISUAL_WORKER_ON, std::memory_order_relaxed);
	gBifurxVisualWorkerSettingsLoaded.store(true, std::memory_order_release);
}

enum class SlotPhase { Idle, Queued, InFlight };
enum class RunState { Stopped, Running, Stopping };

struct DisplaySlot {
	SlotPhase phase = SlotPhase::Idle;
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
	RunState runState = RunState::Stopped;
	bool permanentlyClosed = false;
	uint64_t completedStops = 0;
	uint64_t nextDisplayId = 1;
#if defined(BIFURX_WORKER_TEST_HOOKS)
	uint64_t claimedId = 0;
	uint64_t claimedRequestSeq = 0;
	TestHook afterClaim;
	TestHook beforePublish;
	TestHook beforeAdmission;
	bool failNextStart = false;
	size_t published = 0;
	size_t discarded = 0;
	size_t stopWaiters = 0;
#endif

	void run() {
		for (;;) {
			BifurxUiRenderRequest request;
			std::shared_ptr<const BifurxUiRenderSnapshot> previousSnapshot;
#if defined(BIFURX_WORKER_TEST_HOOKS)
			TestHook afterClaimHook;
			TestHook beforePublishHook;
#endif
			// Claim under the service mutex. No slot reference escapes this scope.
			{
				std::unique_lock<std::mutex> lock(mutex);
				cv.wait(lock, [&]() { return runState != RunState::Running || !readyQueue.empty(); });
				if (runState != RunState::Running) return;
				const uint64_t displayId = readyQueue.front();
				readyQueue.pop_front();
				auto it = slots.find(displayId);
				if (it == slots.end()) continue;
				DisplaySlot& slot = it->second;
				assert(slot.phase == SlotPhase::Queued && slot.hasPending);
				if (slot.phase != SlotPhase::Queued || !slot.hasPending) continue;
				request = std::move(slot.pending);
				slot.pending = BifurxUiRenderRequest {};
				slot.hasPending = false;
				slot.phase = SlotPhase::InFlight;
				previousSnapshot = slot.latestSnapshot;
#if defined(BIFURX_WORKER_TEST_HOOKS)
				claimedId = request.displayId;
				claimedRequestSeq = request.requestSeq;
				afterClaimHook = afterClaim;
				beforePublishHook = beforePublish;
#endif
			}
#if defined(BIFURX_WORKER_TEST_HOOKS)
			if (afterClaimHook) afterClaimHook(request.displayId, request.requestSeq);
#endif

			// Prepare CPU targets without the service mutex.
			auto snapshot = std::make_shared<BifurxUiRenderSnapshot>();
			const bool sameRate = previousSnapshot
				&& previousSnapshot->cachedAxisSampleRate == request.previewState.sampleRate;
			if (sameRate) {
				snapshot->cachedAxisSampleRate = previousSnapshot->cachedAxisSampleRate;
				std::copy_n(previousSnapshot->curveHz, kCurvePointCount, snapshot->curveHz);
				std::copy_n(previousSnapshot->curveBinPos, kCurvePointCount, snapshot->curveBinPos);
			}
			request.skipCurvePrep = false;
			if (sameRate && previousSnapshot->previewSeq == request.previewSeq && previousSnapshot->hasCurveTarget) {
				request.skipCurvePrep = true;
				snapshot->hasCurveTarget = true;
				std::copy_n(previousSnapshot->curveTargetDb, kCurvePointCount, snapshot->curveTargetDb);
			}
			if (!request.payload && sameRate && previousSnapshot->hasOverlayTarget) {
				request.analysisSeq = previousSnapshot->analysisSeq;
				snapshot->hasOverlayTarget = true;
				snapshot->displayTopTargetDbfs = previousSnapshot->displayTopTargetDbfs;
				std::copy_n(previousSnapshot->overlayTargetModuleDb, kCurvePointCount, snapshot->overlayTargetModuleDb);
				std::copy_n(previousSnapshot->overlayTargetOutputDbfs, kCurvePointCount, snapshot->overlayTargetOutputDbfs);
			}
			prepareCurveSnapshot(request, snapshot.get());
			snapshot->completedAtSec = system::getTime();
#if defined(BIFURX_WORKER_TEST_HOOKS)
			if (beforePublishHook) beforePublishHook(request.displayId, request.requestSeq);
#endif

			// Publish only if the same registration is still live and running.
			std::shared_ptr<const BifurxUiRenderSnapshot> retiredSnapshot;
			bool requeued = false;
			{
				std::lock_guard<std::mutex> lock(mutex);
				auto it = slots.find(request.displayId);
				if (runState == RunState::Running && !permanentlyClosed && it != slots.end()
					&& it->second.phase == SlotPhase::InFlight) {
					DisplaySlot& slot = it->second;
					retiredSnapshot = std::move(slot.latestSnapshot);
					slot.latestSnapshot = std::move(snapshot);
					if (slot.hasPending) {
						slot.phase = SlotPhase::Queued;
						readyQueue.push_back(request.displayId);
						requeued = true;
					}
					else slot.phase = SlotPhase::Idle;
#if defined(BIFURX_WORKER_TEST_HOOKS)
					++published;
#endif
				}
#if defined(BIFURX_WORKER_TEST_HOOKS)
				else ++discarded;
				claimedId = 0;
				claimedRequestSeq = 0;
#endif
			}
			if (requeued) cv.notify_one();
			// Request, predecessor and replaced snapshot retire after unlock.
		}
	}
};

BifurxUiRenderService::BifurxUiRenderService() : impl(new Impl()) {}
BifurxUiRenderService::~BifurxUiRenderService() { stop(); }

bool BifurxUiRenderService::start() {
	std::lock_guard<std::mutex> lock(impl->mutex);
	if (impl->permanentlyClosed || impl->runState == RunState::Stopping) return false;
	if (impl->runState == RunState::Running) return true;
#if defined(BIFURX_WORKER_TEST_HOOKS)
	if (impl->failNextStart) { impl->failNextStart = false; return false; }
#endif
	try {
		impl->runState = RunState::Running;
		impl->thread = std::thread([this]() { impl->run(); });
		return true;
	}
	catch (const std::system_error&) {
		impl->runState = RunState::Stopped;
		return false;
	}
}

void BifurxUiRenderService::stop() {
	std::thread threadToJoin;
	std::unordered_map<uint64_t, DisplaySlot> retiredSlots;
	std::deque<uint64_t> retiredQueue;
	{
		std::unique_lock<std::mutex> lock(impl->mutex);
		if (impl->runState == RunState::Stopping) {
			const uint64_t completedBefore = impl->completedStops;
#if defined(BIFURX_WORKER_TEST_HOOKS)
			++impl->stopWaiters;
#endif
			impl->cv.wait(lock, [&]() { return impl->completedStops != completedBefore; });
#if defined(BIFURX_WORKER_TEST_HOOKS)
			--impl->stopWaiters;
#endif
			return;
		}
		if (impl->runState == RunState::Stopped) return;
		assert(impl->thread.get_id() != std::this_thread::get_id());
		impl->runState = RunState::Stopping;
		threadToJoin = std::move(impl->thread);
		retiredSlots.swap(impl->slots);
		retiredQueue.swap(impl->readyQueue);
	}
	impl->cv.notify_all();
	if (threadToJoin.joinable()) threadToJoin.join();
	retiredQueue.clear();
	retiredSlots.clear();
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		impl->runState = RunState::Stopped;
		++impl->completedStops;
	}
	impl->cv.notify_all();
}

void BifurxUiRenderService::shutdown() {
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		impl->permanentlyClosed = true;
	}
	stop();
}

uint64_t BifurxUiRenderService::registerDisplay() {
#if defined(BIFURX_WORKER_TEST_HOOKS)
	TestHook beforeAdmission;
	{ std::lock_guard<std::mutex> lock(impl->mutex); beforeAdmission = impl->beforeAdmission; }
	if (beforeAdmission) beforeAdmission(0, 0);
#endif
	std::lock_guard<std::mutex> lock(impl->mutex);
	if (impl->runState != RunState::Running || impl->permanentlyClosed
		|| impl->nextDisplayId == 0) return 0;
	const uint64_t id = impl->nextDisplayId;
	impl->slots.emplace(id, DisplaySlot {});
	impl->nextDisplayId = id == std::numeric_limits<uint64_t>::max() ? 0 : id + 1;
	return id;
}

void BifurxUiRenderService::unregisterDisplay(uint64_t displayId) {
	DisplaySlot retiredSlot;
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		auto it = impl->slots.find(displayId);
		if (it == impl->slots.end()) return;
		if (it->second.phase == SlotPhase::Queued) {
			auto queued = std::find(impl->readyQueue.begin(), impl->readyQueue.end(), displayId);
			assert(queued != impl->readyQueue.end());
			impl->readyQueue.erase(queued);
		}
		retiredSlot = std::move(it->second);
		impl->slots.erase(it);
	}
}

bool BifurxUiRenderService::submitLatest(BifurxUiRenderRequest request) {
#if defined(BIFURX_WORKER_TEST_HOOKS)
	TestHook beforeAdmission;
	{ std::lock_guard<std::mutex> lock(impl->mutex); beforeAdmission = impl->beforeAdmission; }
	if (beforeAdmission) beforeAdmission(request.displayId, request.requestSeq);
#endif
	BifurxUiRenderRequest retiredRequest;
	{
		std::lock_guard<std::mutex> lock(impl->mutex);
		if (impl->runState != RunState::Running || impl->permanentlyClosed) return false;
		auto it = impl->slots.find(request.displayId);
		if (it == impl->slots.end()) return false;
		DisplaySlot& slot = it->second;
		if (!request.payload && slot.hasPending && slot.pending.payload
			&& slot.pending.previewState.sampleRate == request.previewState.sampleRate) {
			request.payload = slot.pending.payload;
			request.analysisSeq = slot.pending.analysisSeq;
		}
		if (slot.hasPending) retiredRequest = std::move(slot.pending);
		slot.pending = std::move(request);
		slot.hasPending = true;
		if (slot.phase == SlotPhase::Idle) {
			slot.phase = SlotPhase::Queued;
			impl->readyQueue.push_back(slot.pending.displayId);
		}
	}
	impl->cv.notify_one();
	return true;
}

std::shared_ptr<const BifurxUiRenderSnapshot> BifurxUiRenderService::getLatestSnapshot(uint64_t displayId) const {
	std::lock_guard<std::mutex> lock(impl->mutex);
	if (impl->runState != RunState::Running || impl->permanentlyClosed) return nullptr;
	auto it = impl->slots.find(displayId);
	return it == impl->slots.end() ? nullptr : it->second.latestSnapshot;
}

#if defined(BIFURX_WORKER_TEST_HOOKS)
void BifurxUiRenderService::setTestHooks(TestHook afterClaim, TestHook beforePublish) {
	std::lock_guard<std::mutex> lock(impl->mutex);
	impl->afterClaim = std::move(afterClaim);
	impl->beforePublish = std::move(beforePublish);
}

void BifurxUiRenderService::setAdmissionTestHook(TestHook beforeAdmission) {
	std::lock_guard<std::mutex> lock(impl->mutex);
	impl->beforeAdmission = std::move(beforeAdmission);
}

BifurxUiRenderService::TestState BifurxUiRenderService::testState(uint64_t displayId) const {
	std::lock_guard<std::mutex> lock(impl->mutex);
	TestState state;
	state.runState = int(impl->runState);
	state.permanentlyClosed = impl->permanentlyClosed;
	state.displayCount = impl->slots.size();
	state.queuedIds.assign(impl->readyQueue.begin(), impl->readyQueue.end());
	state.published = impl->published;
	state.discarded = impl->discarded;
	state.stopWaiters = impl->stopWaiters;
	state.claimedId = impl->claimedId;
	state.claimedRequestSeq = impl->claimedRequestSeq;
	auto it = impl->slots.find(displayId);
	if (it != impl->slots.end()) {
		state.pendingRequestSeq = it->second.hasPending ? it->second.pending.requestSeq : 0;
		state.pendingAnalysisSeq = it->second.hasPending ? it->second.pending.analysisSeq : 0;
		state.pendingHasPayload = it->second.hasPending && bool(it->second.pending.payload);
	}
	return state;
}

void BifurxUiRenderService::setNextDisplayIdForTest(uint64_t nextId) {
	std::lock_guard<std::mutex> lock(impl->mutex);
	assert(impl->runState == RunState::Stopped);
	impl->nextDisplayId = nextId;
}

void BifurxUiRenderService::failNextStartForTest() {
	std::lock_guard<std::mutex> lock(impl->mutex);
	assert(impl->runState == RunState::Stopped);
	impl->failNextStart = true;
}
#endif

BifurxUiRenderService& bifurxRenderService() {
	static BifurxUiRenderService service;
	return service;
}

void shutdownBifurxRenderService() { bifurxRenderService().shutdown(); }

void setBifurxVisualWorkerDefaultMode(int mode) {
	loadBifurxVisualWorkerDefaultModeIfNeeded();
	gBifurxVisualWorkerDefaultMode.store(
		std::max(int(VISUAL_WORKER_OFF), std::min(mode, int(VISUAL_WORKER_ON))),
		std::memory_order_relaxed
	);
}

int getBifurxVisualWorkerDefaultMode() {
	loadBifurxVisualWorkerDefaultModeIfNeeded();
	return gBifurxVisualWorkerDefaultMode.load(std::memory_order_relaxed);
}

} // namespace bifurx
