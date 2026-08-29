#include "DeepcacheBrowserLogic.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <tuple>

namespace deepcache {
namespace {

std::string lowercase(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return value;
}

double finiteOrNegativeInfinity(double value) {
	return std::isfinite(value) ? value : -INFINITY;
}

int searchRank(const BrowserModelRecord& record, const std::string& lowercaseSearch) {
	if (lowercaseSearch.empty())
		return 0;
	if (record.normalizedModelName == lowercaseSearch || record.normalizedModelSlug == lowercaseSearch)
		return 0;
	if (record.normalizedModelName.find(lowercaseSearch) == 0 || record.normalizedModelSlug.find(lowercaseSearch) == 0)
		return 1;
	if (record.normalizedModelName.find(lowercaseSearch) != std::string::npos ||
	    record.normalizedModelSlug.find(lowercaseSearch) != std::string::npos)
		return 2;
	if (record.normalizedPluginBrand.find(lowercaseSearch) == 0 ||
	    record.normalizedPluginName.find(lowercaseSearch) == 0)
		return 3;
	return 4;
}

}  // namespace

void normalizeBrowserModelRecord(BrowserModelRecord& record) {
	record.normalizedPluginBrand = lowercase(record.pluginBrand);
	record.normalizedPluginName = lowercase(record.pluginName);
	record.normalizedModelSlug = lowercase(record.modelSlug);
	record.normalizedModelName = lowercase(record.modelName);
	record.normalizedSearchText = lowercase(record.pluginBrand + " " + record.pluginName + " " +
	                                       record.pluginSlug + " " + record.modelName + " " +
	                                       record.modelSlug + " " + record.description + " " +
	                                       record.tagSearchText);
}

void normalizeBrowserFilter(BrowserFilter& filter) {
	filter.normalizedSearch = lowercase(filter.search);
}

bool browserModelIsDisplayEligible(const BrowserModelRecord& record) {
	return !record.hidden && record.enabled && record.whitelisted;
}

bool browserModelMatches(const BrowserModelRecord& record, const BrowserFilter& filter) {
	if (!filter.unhide && !browserModelIsDisplayEligible(record))
		return false;
	if (filter.favoritesOnly && !record.favorite)
		return false;
	if (!filter.brand.empty() && record.pluginBrand != filter.brand)
		return false;
	for (int tagId : filter.tagIds) {
		if (std::find(record.tagIds.begin(), record.tagIds.end(), tagId) == record.tagIds.end())
			return false;
	}

	if (filter.normalizedSearch.empty())
		return true;
	return record.normalizedSearchText.find(filter.normalizedSearch) != std::string::npos;
}

float previewRenderTransformScale(float windowPixelRatio, float canonicalScale) {
	if (!std::isfinite(windowPixelRatio) || windowPixelRatio <= 0.f)
		windowPixelRatio = 1.f;
	canonicalScale = std::isfinite(canonicalScale) && canonicalScale >= 1.5f ? 2.f : 1.f;
	const float framebufferPixelRatio = std::max(1.f, std::floor(windowPixelRatio));
	return canonicalScale / framebufferPixelRatio;
}

std::vector<std::size_t> sortBrowserModelIndices(const std::vector<BrowserModelRecord>& records,
	                                             BrowserSortMode sortMode,
	                                             const std::string& search) {
	std::vector<const BrowserModelRecord*> sorted;
	sorted.reserve(records.size());
	for (const BrowserModelRecord& record : records)
		sorted.push_back(&record);

	const std::string query = lowercase(search);
	std::stable_sort(sorted.begin(), sorted.end(), [&](const BrowserModelRecord* a, const BrowserModelRecord* b) {
		if (!query.empty()) {
			return std::make_tuple(searchRank(*a, query), a->normalizedModelName, a->normalizedPluginBrand, a->modelIndex) <
			       std::make_tuple(searchRank(*b, query), b->normalizedModelName, b->normalizedPluginBrand, b->modelIndex);
		}
		switch (sortMode) {
			case BrowserSortMode::UPDATED:
				return std::make_tuple(-finiteOrNegativeInfinity(a->pluginModifiedTimestamp), a->normalizedPluginBrand,
				                       a->normalizedPluginName, a->pluginModelOrder, a->modelIndex) <
				       std::make_tuple(-finiteOrNegativeInfinity(b->pluginModifiedTimestamp), b->normalizedPluginBrand,
				                       b->normalizedPluginName, b->pluginModelOrder, b->modelIndex);
			case BrowserSortMode::LAST_USED:
				return std::make_tuple(-finiteOrNegativeInfinity(a->lastAdded),
				                       -finiteOrNegativeInfinity(a->pluginModifiedTimestamp), a->normalizedPluginBrand,
				                       a->normalizedPluginName, a->pluginModelOrder, a->modelIndex) <
				       std::make_tuple(-finiteOrNegativeInfinity(b->lastAdded),
				                       -finiteOrNegativeInfinity(b->pluginModifiedTimestamp), b->normalizedPluginBrand,
				                       b->normalizedPluginName, b->pluginModelOrder, b->modelIndex);
			case BrowserSortMode::MOST_USED:
				return std::make_tuple(-a->addedCount, -finiteOrNegativeInfinity(a->lastAdded),
				                       -finiteOrNegativeInfinity(a->pluginModifiedTimestamp), a->normalizedPluginBrand,
				                       a->normalizedPluginName, a->pluginModelOrder, a->modelIndex) <
				       std::make_tuple(-b->addedCount, -finiteOrNegativeInfinity(b->lastAdded),
				                       -finiteOrNegativeInfinity(b->pluginModifiedTimestamp), b->normalizedPluginBrand,
				                       b->normalizedPluginName, b->pluginModelOrder, b->modelIndex);
			case BrowserSortMode::BRAND:
				return std::make_tuple(a->normalizedPluginBrand, a->normalizedPluginName,
				                       a->pluginModelOrder, a->modelIndex) <
				       std::make_tuple(b->normalizedPluginBrand, b->normalizedPluginName,
				                       b->pluginModelOrder, b->modelIndex);
			case BrowserSortMode::MODULE_NAME:
				return std::make_tuple(a->normalizedModelName, a->normalizedPluginBrand, a->modelIndex) <
				       std::make_tuple(b->normalizedModelName, b->normalizedPluginBrand, b->modelIndex);
			case BrowserSortMode::RANDOM:
				return std::make_tuple(a->randomOrder, a->modelIndex) <
				       std::make_tuple(b->randomOrder, b->modelIndex);
		}
		return a->modelIndex < b->modelIndex;
	});

	std::vector<std::size_t> indices;
	indices.reserve(sorted.size());
	for (const BrowserModelRecord* record : sorted)
		indices.push_back(record->modelIndex);
	return indices;
}

}  // namespace deepcache
