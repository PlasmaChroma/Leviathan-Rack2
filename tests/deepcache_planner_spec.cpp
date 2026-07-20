#include "../src/DeepcachePlanner.hpp"
#include "../src/DeepcacheBrowserLogic.hpp"

#include <iostream>
#include <chrono>
#include <cmath>
#include <limits>
#include <string>
#include <thread>
#include <vector>

namespace {

using deepcache::CacheState;
using deepcache::ModelDescriptor;
using deepcache::PreviewPlanInput;

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

std::vector<ModelDescriptor> descriptors() {
	return {
		{0, "alpha", "2.0", "zeta", "A Brand", "Zeta", false, false},
		{1, "beta", "1.0", "favorite", "Z Brand", "Favorite", true, false},
		{2, "alpha", "2.0", "visible", "B Brand", "Visible", false, false},
		{3, "gamma", "3.1", "recent", "C Brand", "Recent", false, false},
	};
}

TestResult testPriorityOrdering() {
	PreviewPlanInput input;
	input.generation = 42;
	input.visibleModelIndices.insert(2);
	input.recentlyRequestedModelIndices.insert(3);
	auto result = deepcache::planPreviewRequests(descriptors(), input);
	const bool pass = result.requests.size() == 4 &&
	                  result.requests[0].modelIndex == 2 && result.requests[0].priority == 0 &&
	                  result.requests[1].modelIndex == 1 && result.requests[1].priority == 1 &&
	                  result.requests[2].modelIndex == 3 && result.requests[2].priority == 2 &&
	                  result.requests[3].modelIndex == 0 && result.requests[3].priority == 3;
	return {"visible, favorite, recent, remaining priority", pass,
	        "planned=" + std::to_string(result.requests.size())};
}

TestResult testDeduplicationAndInvalidEntries() {
	auto models = descriptors();
	models.push_back(models[0]);
	models.push_back({99, "", "1", "invalid", "", "", false, false});
	PreviewPlanInput input;
	auto result = deepcache::planPreviewRequests(models, input);
	const bool pass = result.requests.size() == 4 && result.duplicateCount == 1 && result.invalidCount == 1;
	return {"invalid and duplicate descriptors are rejected", pass,
	        "requests=" + std::to_string(result.requests.size()) +
	            " duplicates=" + std::to_string(result.duplicateCount) +
	            " invalid=" + std::to_string(result.invalidCount)};
}

TestResult testGenerationAndPromotion() {
	PreviewPlanInput input;
	input.generation = 77;
	auto result = deepcache::planPreviewRequests(descriptors(), input);
	const bool promoted = deepcache::promotePreviewRequest(result.requests, 3, 77);
	const bool staleRejected = !deepcache::promotePreviewRequest(result.requests, 1, 76);
	const bool pass = promoted && staleRejected && result.requests.front().modelIndex == 3 &&
	                  result.requests.front().generation == 77 && result.requests.front().priority == -1;
	return {"promotion is generation-scoped", pass,
	        "front=" + std::to_string(result.requests.front().modelIndex)};
}

TestResult testStableCacheKey() {
	auto models = descriptors();
	const std::string key = deepcache::makePreviewCacheKey(models[0]);
	models[0].modelIndex = 12345;
	const bool pass = key == deepcache::makePreviewCacheKey(models[0]) &&
	                  key == "deepcache-raster-v3-canonical-2x/alpha/2.0/zeta";
	return {"cache key excludes runtime model index", pass, key};
}

TestResult testCanonicalPreviewRenderScale() {
	const float oneAt100 = deepcache::previewRenderTransformScale(1.f);
	const float oneAt150 = deepcache::previewRenderTransformScale(1.5f);
	const float oneAt200 = deepcache::previewRenderTransformScale(2.f);
	const float oneAt300 = deepcache::previewRenderTransformScale(3.f);
	const float twoAt100 = deepcache::previewRenderTransformScale(1.f, 2.f);
	const float twoAt150 = deepcache::previewRenderTransformScale(1.5f, 2.f);
	const float twoAt200 = deepcache::previewRenderTransformScale(2.f, 2.f);
	const float twoAt300 = deepcache::previewRenderTransformScale(3.f, 2.f);
	const float invalid = deepcache::previewRenderTransformScale(
		std::numeric_limits<float>::quiet_NaN());
	const bool onePass = std::abs(oneAt100 - 1.f) < 1e-6f && std::abs(oneAt150 - 1.f) < 1e-6f &&
	                     std::abs(oneAt200 - 0.5f) < 1e-6f && std::abs(oneAt300 - (1.f / 3.f)) < 1e-6f;
	const bool twoPass = std::abs(twoAt100 - 2.f) < 1e-6f && std::abs(twoAt150 - 2.f) < 1e-6f &&
	                     std::abs(twoAt200 - 1.f) < 1e-6f && std::abs(twoAt300 - (2.f / 3.f)) < 1e-6f;
	const bool pass = onePass && twoPass && std::abs(invalid - 1.f) < 1e-6f;
	return {"1x and 2x preview scales cancel Rack framebuffer pixel ratio", pass,
	        "1x=" + std::to_string(oneAt100) + "/" + std::to_string(oneAt150) + "/" +
	            std::to_string(oneAt200) + "/" + std::to_string(oneAt300) + ", 2x=" +
	            std::to_string(twoAt100) + "/" + std::to_string(twoAt150) + "/" +
	            std::to_string(twoAt200) + "/" + std::to_string(twoAt300)};
}

TestResult testStateTransitions() {
	const bool happyPath = deepcache::isValidTransition(CacheState::IDLE, CacheState::PLANNING) &&
	                       deepcache::isValidTransition(CacheState::PLANNING, CacheState::WARMING) &&
	                       deepcache::isValidTransition(CacheState::WARMING, CacheState::READY);
	const bool pausePath = deepcache::isValidTransition(CacheState::WARMING, CacheState::PAUSED) &&
	                       deepcache::isValidTransition(CacheState::PAUSED, CacheState::WARMING);
	const bool contextRewarm = deepcache::isValidTransition(CacheState::READY, CacheState::WARMING);
	const bool invalid = !deepcache::isValidTransition(CacheState::IDLE, CacheState::READY) &&
	                     !deepcache::isValidTransition(CacheState::STOPPING, CacheState::IDLE);
	return {"cache state machine accepts only explicit transitions", happyPath && pausePath && contextRewarm && invalid,
	        "happy/pause/invalid paths checked"};
}

bool waitForPlan(deepcache::PreviewPlannerWorker& worker, std::uint64_t generation) {
	const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
	while (std::chrono::steady_clock::now() < deadline) {
		if (worker.isPlanReady(generation))
			return true;
		std::this_thread::yield();
	}
	return false;
}

TestResult testWorkerPublicationAndCancellation() {
	deepcache::PreviewPlannerWorker worker;
	const bool zeroIsIdle = !worker.isPlanReady(0) && !worker.hasPlanFailed(0);
	PreviewPlanInput first;
	first.generation = 100;
	worker.submit(descriptors(), first);
	const bool firstReady = waitForPlan(worker, 100);
	const bool promoted = worker.promote(3, 100);
	deepcache::PreviewBuildRequest request;
	const bool popped = worker.tryPop(request);

	PreviewPlanInput second;
	second.generation = 101;
	worker.submit(descriptors(), second);
	worker.cancel(101);
	const bool staleAbsent = !worker.tryPop(request) && !worker.isPlanReady(101);
	worker.shutdown();
	const bool pass = zeroIsIdle && firstReady && promoted && popped && request.modelIndex == 3 && staleAbsent;
	return {"worker publishes, promotes, cancels, and joins", pass,
	        "zeroIdle=" + std::to_string(zeroIsIdle) + " firstReady=" + std::to_string(firstReady) +
	            " staleAbsent=" + std::to_string(staleAbsent)};
}

TestResult testWorkerBulkPromotion() {
	deepcache::PreviewPlannerWorker worker;
	PreviewPlanInput input;
	input.generation = 150;
	worker.submit(descriptors(), input);
	const bool ready = waitForPlan(worker, 150);
	const std::size_t promoted = worker.promote({0, 3}, 150);
	const std::size_t pendingBeforePop = worker.pendingRequestCount(150);
	std::vector<std::size_t> order;
	deepcache::PreviewBuildRequest request;
	while (worker.tryPop(request))
		order.push_back(request.modelIndex);
	const bool pass = ready && promoted == 2 && pendingBeforePop == 4 &&
	                  order == std::vector<std::size_t>({0, 3, 1, 2}) &&
	                  worker.pendingRequestCount(150) == 0;
	return {"worker promotes a visible set in one stable bulk operation", pass,
	        "promoted=" + std::to_string(promoted) + " pending=" + std::to_string(pendingBeforePop)};
}

TestResult testMemoryBackendLifecycle() {
	deepcache::MemoryPreviewCacheBackend backend;
	backend.store("one");
	backend.store("one");
	backend.store("two");
	const bool stored = backend.contains("one") && backend.contains("two") && backend.size() == 2;
	backend.invalidate("one");
	const bool invalidated = !backend.contains("one") && backend.size() == 1;
	backend.clear();
	const bool cleared = backend.size() == 0;
	return {"memory backend records and releases cache keys", stored && invalidated && cleared,
	        "stored=" + std::to_string(stored) + " cleared=" + std::to_string(cleared)};
}

TestResult testWorkerPauseAndReplacement() {
	deepcache::PreviewPlannerWorker worker;
	worker.pause();
	PreviewPlanInput pausedInput;
	pausedInput.generation = 200;
	worker.submit(descriptors(), pausedInput);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	const bool remainedPaused = !worker.isPlanReady(200);
	worker.resume();
	const bool resumed = waitForPlan(worker, 200);

	PreviewPlanInput replacement;
	replacement.generation = 201;
	worker.submit(descriptors(), replacement);
	const bool replacementReady = waitForPlan(worker, 201);
	deepcache::PreviewBuildRequest request;
	std::vector<std::size_t> replacementOrder;
	bool onlyReplacementGeneration = true;
	while (worker.tryPop(request)) {
		onlyReplacementGeneration = onlyReplacementGeneration && request.generation == 201;
		replacementOrder.push_back(request.modelIndex);
	}
	const bool onlyReplacement = onlyReplacementGeneration && replacementOrder.size() == 4 &&
	                             replacementOrder.front() == 1;
	return {"worker pause and replacement discard stale work", remainedPaused && resumed && replacementReady && onlyReplacement,
	        "paused=" + std::to_string(remainedPaused) + " replacement=" + std::to_string(onlyReplacement)};
}

std::vector<deepcache::BrowserModelRecord> browserRecords() {
	std::vector<deepcache::BrowserModelRecord> records(4);
	records[0].modelIndex = 0;
	records[0].pluginSlug = "alpha";
	records[0].pluginBrand = "Alpha Brand";
	records[0].pluginName = "Alpha Plugin";
	records[0].modelSlug = "bass-voice";
	records[0].modelName = "Bass Voice";
	records[0].description = "A deep oscillator";
	records[0].tagSearchText = "Oscillator VCO";
	records[0].tagIds = {1, 2};
	records[0].favorite = true;
	records[0].pluginModifiedTimestamp = 20.0;
	records[0].lastAdded = 5.0;
	records[0].addedCount = 4;
	records[0].pluginModelOrder = 1;
	records[0].randomOrder = 40;

	records[1].modelIndex = 1;
	records[1].pluginSlug = "alpha";
	records[1].pluginBrand = "Alpha Brand";
	records[1].pluginName = "Alpha Plugin";
	records[1].modelSlug = "filter";
	records[1].modelName = "Filter";
	records[1].tagIds = {2, 3};
	records[1].pluginModifiedTimestamp = 20.0;
	records[1].lastAdded = 10.0;
	records[1].addedCount = 2;
	records[1].pluginModelOrder = 0;
	records[1].randomOrder = 10;

	records[2].modelIndex = 2;
	records[2].pluginSlug = "zeta";
	records[2].pluginBrand = "Zeta Brand";
	records[2].pluginName = "Zeta Plugin";
	records[2].modelSlug = "clock";
	records[2].modelName = "Clock";
	records[2].tagIds = {4};
	records[2].pluginModifiedTimestamp = 30.0;
	records[2].lastAdded = 1.0;
	records[2].addedCount = 8;
	records[2].randomOrder = 30;

	records[3].modelIndex = 3;
	records[3].pluginBrand = "Hidden Brand";
	records[3].modelName = "Hidden";
	records[3].hidden = true;
	records[3].favorite = true;
	records[3].tagIds = {1, 2};
	records[3].randomOrder = 99;
	for (auto& record : records)
		deepcache::normalizeBrowserModelRecord(record);
	return records;
}

TestResult testBrowserFilterParity() {
	const auto records = browserRecords();
	deepcache::BrowserFilter filter;
	filter.brand = "Alpha Brand";
	filter.tagIds = {1, 2};
	filter.favoritesOnly = true;
	deepcache::normalizeBrowserFilter(filter);
	const bool combined = deepcache::browserModelMatches(records[0], filter) &&
	                      !deepcache::browserModelMatches(records[1], filter) &&
	                      !deepcache::browserModelMatches(records[3], filter);
	filter = deepcache::BrowserFilter();
	filter.search = "vco";
	deepcache::normalizeBrowserFilter(filter);
	const bool tagAliasSearch = deepcache::browserModelMatches(records[0], filter) &&
	                            !deepcache::browserModelMatches(records[1], filter);
	filter.search = "deep oscillator";
	deepcache::normalizeBrowserFilter(filter);
	const bool descriptionSearch = deepcache::browserModelMatches(records[0], filter);
	return {"browser combines brand, tags, favorites, hidden, and search fields",
	        combined && tagAliasSearch && descriptionSearch,
	        "combined=" + std::to_string(combined) + " tagAlias=" + std::to_string(tagAliasSearch)};
}

TestResult testBrowserSortParity() {
	const auto records = browserRecords();
	const auto updated = deepcache::sortBrowserModelIndices(records, deepcache::BrowserSortMode::UPDATED, "");
	const auto lastUsed = deepcache::sortBrowserModelIndices(records, deepcache::BrowserSortMode::LAST_USED, "");
	const auto mostUsed = deepcache::sortBrowserModelIndices(records, deepcache::BrowserSortMode::MOST_USED, "");
	const auto brand = deepcache::sortBrowserModelIndices(records, deepcache::BrowserSortMode::BRAND, "");
	const auto name = deepcache::sortBrowserModelIndices(records, deepcache::BrowserSortMode::MODULE_NAME, "");
	const auto random = deepcache::sortBrowserModelIndices(records, deepcache::BrowserSortMode::RANDOM, "");
	const bool pass = updated[0] == 2 && lastUsed[0] == 1 && mostUsed[0] == 2 &&
	                  brand[0] == 1 && name[0] == 0 && random[0] == 1;
	return {"browser implements all stock sort modes", pass,
	        "first=" + std::to_string(updated[0]) + "/" + std::to_string(lastUsed[0]) + "/" +
	            std::to_string(mostUsed[0]) + "/" + std::to_string(brand[0]) + "/" +
	            std::to_string(name[0]) + "/" + std::to_string(random[0])};
}

}  // namespace

int main() {
	const std::vector<TestResult> tests = {
		testPriorityOrdering(),
		testDeduplicationAndInvalidEntries(),
		testGenerationAndPromotion(),
		testStableCacheKey(),
		testCanonicalPreviewRenderScale(),
		testStateTransitions(),
		testWorkerPublicationAndCancellation(),
		testWorkerBulkPromotion(),
		testMemoryBackendLifecycle(),
		testWorkerPauseAndReplacement(),
		testBrowserFilterParity(),
		testBrowserSortParity(),
	};

	std::size_t failed = 0;
	std::cout << "Deepcache Planner Spec\n";
	std::cout << "--------------------------------\n";
	for (const TestResult& test : tests) {
		std::cout << (test.pass ? "[PASS] " : "[FAIL] ") << test.name << " :: " << test.detail << "\n";
		if (!test.pass)
			failed++;
	}
	std::cout << "--------------------------------\n";
	std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
