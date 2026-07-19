#pragma once

#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace deepcache {

enum class BrowserSortMode {
	UPDATED = 0,
	LAST_USED = 1,
	MOST_USED = 2,
	BRAND = 3,
	MODULE_NAME = 4,
	RANDOM = 5
};

struct BrowserModelRecord {
	std::size_t modelIndex = 0;
	std::string pluginSlug;
	std::string pluginBrand;
	std::string pluginName;
	std::string modelSlug;
	std::string modelName;
	std::string description;
	std::string tagSearchText;
	std::string normalizedPluginBrand;
	std::string normalizedPluginName;
	std::string normalizedModelSlug;
	std::string normalizedModelName;
	std::string normalizedSearchText;
	std::vector<int> tagIds;
	bool hidden = false;
	bool enabled = true;
	bool whitelisted = true;
	bool favorite = false;
	double pluginModifiedTimestamp = 0.0;
	double lastAdded = 0.0;
	int addedCount = 0;
	int pluginModelOrder = 0;
	std::uint64_t randomOrder = 0;
};

struct BrowserFilter {
	std::string search;
	std::string normalizedSearch;
	std::string brand;
	std::set<int> tagIds;
	bool favoritesOnly = false;
};

void normalizeBrowserModelRecord(BrowserModelRecord& record);
void normalizeBrowserFilter(BrowserFilter& filter);

bool browserModelMatches(const BrowserModelRecord& record, const BrowserFilter& filter);

// FramebufferWidget multiplies its render transform by floor(windowPixelRatio).
// Return the transform that produces a canonical 2x physical preview at every
// Rack UI scale.
float previewRenderTransformScale(float windowPixelRatio);

std::vector<std::size_t> sortBrowserModelIndices(const std::vector<BrowserModelRecord>& records,
	                                             BrowserSortMode sortMode,
	                                             const std::string& search);

}  // namespace deepcache
