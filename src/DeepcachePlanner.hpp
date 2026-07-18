#pragma once

#include <cstddef>
#include <cstdint>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace deepcache {

enum class CacheState {
	DISABLED,
	IDLE,
	PLANNING,
	WARMING,
	PAUSED,
	READY,
	CLEARING,
	ERROR,
	STOPPING
};

enum class PreviewEntryState {
	EMPTY,
	QUEUED,
	CONSTRUCTING,
	RESIDENT,
	FRAMEBUFFER_READY,
	FAILED
};

enum class CacheScope {
	FAVORITES,
	VISIBLE_SEARCH_RESULTS,
	ALL
};

struct ModelDescriptor {
	std::size_t modelIndex = 0;
	std::string pluginSlug;
	std::string pluginVersion;
	std::string modelSlug;
	std::string brand;
	std::string displayName;
	std::string artifactFingerprint;
	bool favorite = false;
	bool hidden = false;

	ModelDescriptor() = default;
	ModelDescriptor(std::size_t modelIndex,
	                std::string pluginSlug,
	                std::string pluginVersion,
	                std::string modelSlug,
	                std::string brand,
	                std::string displayName,
	                bool favorite,
	                bool hidden,
	                std::string artifactFingerprint = std::string())
		: modelIndex(modelIndex),
		  pluginSlug(std::move(pluginSlug)),
		  pluginVersion(std::move(pluginVersion)),
		  modelSlug(std::move(modelSlug)),
		  brand(std::move(brand)),
		  displayName(std::move(displayName)),
		  artifactFingerprint(std::move(artifactFingerprint)),
		  favorite(favorite),
		  hidden(hidden) {
	}
};

struct PreviewBuildRequest {
	std::size_t modelIndex = 0;
	std::uint64_t generation = 0;
	int priority = 0;
	std::string cacheKey;

	PreviewBuildRequest() = default;
	PreviewBuildRequest(std::size_t modelIndex, std::uint64_t generation, int priority, std::string cacheKey)
		: modelIndex(modelIndex),
		  generation(generation),
		  priority(priority),
		  cacheKey(std::move(cacheKey)) {
	}
};

struct PreviewPlanInput {
	std::uint64_t generation = 0;
	CacheScope scope = CacheScope::ALL;
	std::unordered_set<std::size_t> visibleModelIndices;
	std::unordered_set<std::size_t> recentlyRequestedModelIndices;
};

struct PreviewPlanResult {
	std::vector<PreviewBuildRequest> requests;
	std::size_t invalidCount = 0;
	std::size_t duplicateCount = 0;
};

bool isValidTransition(CacheState from, CacheState to);

std::string makePreviewCacheKey(const ModelDescriptor& descriptor);

PreviewPlanResult planPreviewRequests(const std::vector<ModelDescriptor>& descriptors,
	                                  const PreviewPlanInput& input);

bool promotePreviewRequest(std::vector<PreviewBuildRequest>& requests,
	                       std::size_t modelIndex,
	                       std::uint64_t generation);

class PreviewPlannerWorker {
public:
	PreviewPlannerWorker();
	~PreviewPlannerWorker();

	PreviewPlannerWorker(const PreviewPlannerWorker&) = delete;
	PreviewPlannerWorker& operator=(const PreviewPlannerWorker&) = delete;

	void submit(std::vector<ModelDescriptor> descriptors, PreviewPlanInput input);
	void cancel(std::uint64_t generation);
	void pause();
	void resume();
	bool promote(std::size_t modelIndex, std::uint64_t generation);
	bool tryPop(PreviewBuildRequest& request);
	bool isPlanReady(std::uint64_t generation) const;
	std::size_t plannedRequestCount(std::uint64_t generation) const;
	std::size_t pendingRequestCount(std::uint64_t generation) const;
	void shutdown();

private:
	void run();

	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::thread thread_;
	bool stopping_ = false;
	bool paused_ = false;
	bool hasJob_ = false;
	std::uint64_t jobSerial_ = 0;
	std::uint64_t activeGeneration_ = 0;
	std::uint64_t readyGeneration_ = 0;
	std::size_t plannedCount_ = 0;
	std::vector<ModelDescriptor> pendingDescriptors_;
	PreviewPlanInput pendingInput_;
	std::deque<PreviewBuildRequest> output_;
};

class PreviewCacheBackend {
public:
	virtual ~PreviewCacheBackend() = default;
	virtual bool contains(const std::string& key) const = 0;
	virtual void store(const std::string& key) = 0;
	virtual void invalidate(const std::string& key) = 0;
	virtual void clear() = 0;
};

class MemoryPreviewCacheBackend : public PreviewCacheBackend {
public:
	bool contains(const std::string& key) const override;
	void store(const std::string& key) override;
	void invalidate(const std::string& key) override;
	void clear() override;
	std::size_t size() const;

private:
	std::unordered_set<std::string> residentKeys_;
};

}  // namespace deepcache
