#pragma once

#include "BifurxRenderData.hpp"

#include <memory>

namespace bifurx {

enum VisualWorkerMode {
	VISUAL_WORKER_INHERIT = -1,
	VISUAL_WORKER_OFF = 0,
	VISUAL_WORKER_AUTO = 1,
	VISUAL_WORKER_ON = 2
};

class BifurxUiRenderService {
public:
	BifurxUiRenderService();
	~BifurxUiRenderService();

	uint64_t registerDisplay();
	void unregisterDisplay(uint64_t displayId);
	void submitLatest(const BifurxUiRenderRequest& request);
	std::shared_ptr<const BifurxUiRenderSnapshot> getLatestSnapshot(uint64_t displayId) const;

	void start();
	void stop();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

BifurxUiRenderService& bifurxRenderService();
void shutdownBifurxRenderService();
void setBifurxVisualWorkerDefaultMode(int mode);
int getBifurxVisualWorkerDefaultMode();

} // namespace bifurx
