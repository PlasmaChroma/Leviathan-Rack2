#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

namespace deepcache {

enum class ThemeClassification : unsigned char {
	UNKNOWN = 0,
	INVARIANT = 1,
	SENSITIVE = 2
};

// Optional metadata layered beside the version-1 preview archive. A missing,
// stale, or corrupt sidecar only turns classifications back into UNKNOWN; it
// never invalidates preview pixels.
class ThemeClassifier {
public:
	bool load(const std::string& directory);
	bool save(const std::string& directory) const;

	ThemeClassification get(const std::string& cacheKey,
	                        const std::string& buildFingerprint) const;
	bool set(const std::string& cacheKey, const std::string& buildFingerprint,
	         ThemeClassification classification);
	void clear();
	std::size_t size() const { return records_.size(); }

private:
	struct Record {
		std::string buildFingerprint;
		ThemeClassification classification = ThemeClassification::UNKNOWN;
	};
	std::unordered_map<std::string, Record> records_;
};

}  // namespace deepcache
