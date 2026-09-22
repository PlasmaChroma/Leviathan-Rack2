#pragma once

#include "BifurxRenderData.hpp"

#include <memory>
#if defined(BIFURX_WORKER_TEST_HOOKS)
#include <functional>
#include <vector>
#endif

namespace bifurx {

enum VisualWorkerMode {
	VISUAL_WORKER_INHERIT = -1,
	VISUAL_WORKER_OFF = 0,
	VISUAL_WORKER_AUTO = 1,
	VISUAL_WORKER_ON = 2
};

// One shared CPU preparation thread. A registration owns at most one claimed
// request and one replaceable pending request; the UI owns displayed state.
// stop() joins and may restart, while shutdown() closes admission permanently.
class BifurxUiRenderService {
public:
	BifurxUiRenderService();
	~BifurxUiRenderService();

	uint64_t registerDisplay();
	void unregisterDisplay(uint64_t displayId);
	bool submitLatest(BifurxUiRenderRequest request);
	std::shared_ptr<const BifurxUiRenderSnapshot> getLatestSnapshot(uint64_t displayId) const;

	bool start();
	void stop();
	void shutdown();
	BifurxUiRenderService(const BifurxUiRenderService&) = delete;
	BifurxUiRenderService& operator=(const BifurxUiRenderService&) = delete;

#if defined(BIFURX_WORKER_TEST_HOOKS)
	struct TestState {
		int runState = 0;
		bool permanentlyClosed = false;
		size_t displayCount = 0;
		std::vector<uint64_t> queuedIds;
		uint64_t claimedId = 0;
		uint64_t claimedRequestSeq = 0;
		uint64_t pendingRequestSeq = 0;
		uint32_t pendingAnalysisSeq = 0;
		bool pendingHasPayload = false;
		size_t published = 0;
		size_t discarded = 0;
		size_t stopWaiters = 0;
	};
	using TestHook = std::function<void(uint64_t, uint64_t)>;
	void setTestHooks(TestHook afterClaim, TestHook beforePublish);
	void setAdmissionTestHook(TestHook beforeAdmission);
	TestState testState(uint64_t displayId = 0) const;
	void setNextDisplayIdForTest(uint64_t nextId);
	void failNextStartForTest();
#endif

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

BifurxUiRenderService& bifurxRenderService();
void shutdownBifurxRenderService();
void setBifurxVisualWorkerDefaultMode(int mode);
int getBifurxVisualWorkerDefaultMode();

} // namespace bifurx
