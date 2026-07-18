#include "DeepcachePlanner.hpp"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <tuple>
#include <unordered_set>

namespace deepcache {
namespace {

std::string lowercase(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return value;
}

bool isDescriptorValid(const ModelDescriptor& descriptor) {
	return !descriptor.pluginSlug.empty() && !descriptor.modelSlug.empty();
}

std::string modelIdentity(const ModelDescriptor& descriptor) {
	return descriptor.pluginSlug + "\n" + descriptor.modelSlug;
}

int requestPriority(const ModelDescriptor& descriptor, const PreviewPlanInput& input) {
	if (input.visibleModelIndices.count(descriptor.modelIndex))
		return 0;
	if (descriptor.favorite)
		return 1;
	if (input.recentlyRequestedModelIndices.count(descriptor.modelIndex))
		return 2;
	return 3;
}

bool descriptorIsInScope(const ModelDescriptor& descriptor, const PreviewPlanInput& input) {
	switch (input.scope) {
		case CacheScope::FAVORITES:
			return descriptor.favorite;
		case CacheScope::VISIBLE_SEARCH_RESULTS:
			return input.visibleModelIndices.count(descriptor.modelIndex) != 0;
		case CacheScope::ALL:
			return true;
	}
	return false;
}

}  // namespace

bool isValidTransition(CacheState from, CacheState to) {
	if (from == to)
		return true;
	if (to == CacheState::STOPPING)
		return true;

	switch (from) {
		case CacheState::DISABLED:
			return to == CacheState::IDLE;
		case CacheState::IDLE:
			return to == CacheState::PLANNING || to == CacheState::CLEARING || to == CacheState::ERROR;
		case CacheState::PLANNING:
			return to == CacheState::WARMING || to == CacheState::READY || to == CacheState::IDLE ||
			       to == CacheState::CLEARING || to == CacheState::ERROR;
		case CacheState::WARMING:
			return to == CacheState::PAUSED || to == CacheState::READY || to == CacheState::IDLE ||
			       to == CacheState::CLEARING || to == CacheState::ERROR;
		case CacheState::PAUSED:
			return to == CacheState::WARMING || to == CacheState::IDLE || to == CacheState::CLEARING ||
			       to == CacheState::ERROR;
		case CacheState::READY:
			return to == CacheState::PLANNING || to == CacheState::WARMING ||
			       to == CacheState::CLEARING || to == CacheState::ERROR;
		case CacheState::CLEARING:
			return to == CacheState::IDLE || to == CacheState::ERROR;
		case CacheState::ERROR:
			return to == CacheState::IDLE || to == CacheState::PLANNING || to == CacheState::CLEARING;
		case CacheState::STOPPING:
			return false;
	}
	return false;
}

std::string makePreviewCacheKey(const ModelDescriptor& descriptor) {
	// Human-readable for the memory backend and deliberately versioned so a future
	// persistent backend can extend the same identity without pointer-derived data.
	return "deepcache-raster-v1/" + descriptor.pluginSlug + "/" + descriptor.pluginVersion + "/" +
	       descriptor.modelSlug;
}

PreviewPlanResult planPreviewRequests(const std::vector<ModelDescriptor>& descriptors,
	                                  const PreviewPlanInput& input) {
	struct PlannedDescriptor {
		const ModelDescriptor* descriptor = nullptr;
		int priority = 0;
		std::string normalizedBrand;
		std::string normalizedDisplayName;
		std::string normalizedPluginSlug;
		std::string normalizedModelSlug;
		PlannedDescriptor(const ModelDescriptor* descriptor, int priority)
			: descriptor(descriptor),
			  priority(priority),
			  normalizedBrand(lowercase(descriptor->brand)),
			  normalizedDisplayName(lowercase(descriptor->displayName)),
			  normalizedPluginSlug(lowercase(descriptor->pluginSlug)),
			  normalizedModelSlug(lowercase(descriptor->modelSlug)) {
		}
	};

	PreviewPlanResult result;
	std::vector<PlannedDescriptor> planned;
	planned.reserve(descriptors.size());
	std::unordered_set<std::string> identities;
	identities.reserve(descriptors.size());

	for (const ModelDescriptor& descriptor : descriptors) {
		if (!isDescriptorValid(descriptor)) {
			result.invalidCount++;
			continue;
		}

		if (!identities.insert(modelIdentity(descriptor)).second) {
			result.duplicateCount++;
			continue;
		}

		if (!descriptorIsInScope(descriptor, input))
			continue;

		planned.push_back({&descriptor, requestPriority(descriptor, input)});
	}

	std::stable_sort(planned.begin(), planned.end(), [](const PlannedDescriptor& a, const PlannedDescriptor& b) {
		return std::tie(a.priority, a.normalizedBrand, a.normalizedDisplayName,
		                a.normalizedPluginSlug, a.normalizedModelSlug, a.descriptor->modelIndex) <
		       std::tie(b.priority, b.normalizedBrand, b.normalizedDisplayName,
		                b.normalizedPluginSlug, b.normalizedModelSlug, b.descriptor->modelIndex);
	});

	result.requests.reserve(planned.size());
	for (const PlannedDescriptor& item : planned) {
		const ModelDescriptor& descriptor = *item.descriptor;
		result.requests.push_back({descriptor.modelIndex, input.generation, item.priority,
		                           makePreviewCacheKey(descriptor)});
	}
	return result;
}

bool promotePreviewRequest(std::vector<PreviewBuildRequest>& requests,
	                       std::size_t modelIndex,
	                       std::uint64_t generation) {
	auto it = std::find_if(requests.begin(), requests.end(), [&](const PreviewBuildRequest& request) {
		return request.modelIndex == modelIndex && request.generation == generation;
	});
	if (it == requests.end())
		return false;
	if (it == requests.begin()) {
		it->priority = -1;
		return true;
	}

	PreviewBuildRequest promoted = std::move(*it);
	promoted.priority = -1;
	requests.erase(it);
	requests.insert(requests.begin(), std::move(promoted));
	return true;
}

PreviewPlannerWorker::PreviewPlannerWorker()
	: thread_(&PreviewPlannerWorker::run, this) {
}

PreviewPlannerWorker::~PreviewPlannerWorker() {
	shutdown();
}

void PreviewPlannerWorker::submit(std::vector<ModelDescriptor> descriptors, PreviewPlanInput input) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (stopping_)
		return;
	jobSerial_++;
	activeGeneration_ = input.generation;
	readyGeneration_ = 0;
	failedGeneration_ = 0;
	plannedCount_ = 0;
	output_.clear();
	pendingDescriptors_ = std::move(descriptors);
	pendingInput_ = std::move(input);
	hasJob_ = true;
	condition_.notify_all();
}

void PreviewPlannerWorker::cancel(std::uint64_t generation) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (generation != activeGeneration_)
		return;
	jobSerial_++;
	hasJob_ = false;
	activeGeneration_ = 0;
	readyGeneration_ = 0;
	failedGeneration_ = 0;
	plannedCount_ = 0;
	pendingDescriptors_.clear();
	output_.clear();
	condition_.notify_all();
}

void PreviewPlannerWorker::pause() {
	std::lock_guard<std::mutex> lock(mutex_);
	paused_ = true;
}

void PreviewPlannerWorker::resume() {
	std::lock_guard<std::mutex> lock(mutex_);
	paused_ = false;
	condition_.notify_all();
}

bool PreviewPlannerWorker::promote(std::size_t modelIndex, std::uint64_t generation) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (generation != activeGeneration_)
		return false;
	auto it = std::find_if(output_.begin(), output_.end(), [&](const PreviewBuildRequest& request) {
		return request.modelIndex == modelIndex && request.generation == generation;
	});
	if (it == output_.end())
		return false;
	PreviewBuildRequest promoted = std::move(*it);
	promoted.priority = -1;
	output_.erase(it);
	output_.push_front(std::move(promoted));
	return true;
}

bool PreviewPlannerWorker::tryPop(PreviewBuildRequest& request) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (paused_ || output_.empty())
		return false;
	request = std::move(output_.front());
	output_.pop_front();
	return true;
}

bool PreviewPlannerWorker::isPlanReady(std::uint64_t generation) const {
	std::lock_guard<std::mutex> lock(mutex_);
	return readyGeneration_ == generation;
}

bool PreviewPlannerWorker::hasPlanFailed(std::uint64_t generation) const {
	std::lock_guard<std::mutex> lock(mutex_);
	return failedGeneration_ == generation;
}

std::size_t PreviewPlannerWorker::plannedRequestCount(std::uint64_t generation) const {
	std::lock_guard<std::mutex> lock(mutex_);
	return readyGeneration_ == generation ? plannedCount_ : 0;
}

std::size_t PreviewPlannerWorker::pendingRequestCount(std::uint64_t generation) const {
	std::lock_guard<std::mutex> lock(mutex_);
	if (generation != activeGeneration_)
		return 0;
	return static_cast<std::size_t>(std::count_if(output_.begin(), output_.end(), [&](const PreviewBuildRequest& request) {
		return request.generation == generation;
	}));
}

void PreviewPlannerWorker::shutdown() {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (stopping_)
			return;
		stopping_ = true;
		jobSerial_++;
		hasJob_ = false;
		output_.clear();
		condition_.notify_all();
	}
	if (thread_.joinable())
		thread_.join();
}

void PreviewPlannerWorker::run() {
	for (;;) {
		std::vector<ModelDescriptor> descriptors;
		PreviewPlanInput input;
		std::uint64_t serial = 0;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [&]() { return stopping_ || (hasJob_ && !paused_); });
			if (stopping_)
				return;
			serial = jobSerial_;
			descriptors = std::move(pendingDescriptors_);
			input = std::move(pendingInput_);
			hasJob_ = false;
		}

		PreviewPlanResult result;
		bool failed = false;
		try {
			result = planPreviewRequests(descriptors, input);
		}
		catch (...) {
			failed = true;
		}

		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait(lock, [&]() { return stopping_ || !paused_ || serial != jobSerial_; });
		if (stopping_)
			return;
		if (serial != jobSerial_ || input.generation != activeGeneration_)
			continue;
		if (failed) {
			output_.clear();
			plannedCount_ = 0;
			readyGeneration_ = 0;
			failedGeneration_ = input.generation;
			continue;
		}
		output_.insert(output_.end(), std::make_move_iterator(result.requests.begin()),
		               std::make_move_iterator(result.requests.end()));
		plannedCount_ = output_.size();
		readyGeneration_ = input.generation;
	}
}

bool MemoryPreviewCacheBackend::contains(const std::string& key) const {
	return residentKeys_.count(key) != 0;
}

void MemoryPreviewCacheBackend::store(const std::string& key) {
	residentKeys_.insert(key);
}

void MemoryPreviewCacheBackend::invalidate(const std::string& key) {
	residentKeys_.erase(key);
}

void MemoryPreviewCacheBackend::clear() {
	residentKeys_.clear();
}

std::size_t MemoryPreviewCacheBackend::size() const {
	return residentKeys_.size();
}

}  // namespace deepcache
