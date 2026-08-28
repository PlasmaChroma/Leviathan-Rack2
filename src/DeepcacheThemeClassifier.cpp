#include "DeepcacheThemeClassifier.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <utility>

namespace deepcache {
namespace {

const char kMagic[8] = {'L', 'V', 'D', 'C', 'T', 'H', 'M', '1'};
const std::uint32_t kVersion = 1;
const std::uint32_t kMaxRecords = 100000;
const std::uint32_t kMaxStringBytes = 4096;

template <typename T>
bool readValue(std::istream& stream, T* value) {
	return value && static_cast<bool>(stream.read(reinterpret_cast<char*>(value), sizeof(T)));
}

template <typename T>
bool writeValue(std::ostream& stream, const T& value) {
	return static_cast<bool>(stream.write(reinterpret_cast<const char*>(&value), sizeof(T)));
}

bool readString(std::istream& stream, std::string* value) {
	std::uint32_t size = 0;
	if (!value || !readValue(stream, &size) || size > kMaxStringBytes)
		return false;
	value->resize(size);
	return size == 0 || static_cast<bool>(stream.read(&(*value)[0], size));
}

bool writeString(std::ostream& stream, const std::string& value) {
	if (value.size() > kMaxStringBytes)
		return false;
	const std::uint32_t size = static_cast<std::uint32_t>(value.size());
	return writeValue(stream, size) &&
	       (size == 0 || static_cast<bool>(stream.write(value.data(), size)));
}

std::string sidecarPath(const std::string& directory) {
	return directory + "/theme-classifier-v1.bin";
}

}  // namespace

bool ThemeClassifier::load(const std::string& directory) {
	records_.clear();
	std::ifstream stream(sidecarPath(directory).c_str(), std::ios::binary);
	if (!stream)
		return true;
	char magic[8] = {};
	std::uint32_t version = 0;
	std::uint32_t count = 0;
	if (!stream.read(magic, sizeof(magic)) || std::memcmp(magic, kMagic, sizeof(magic)) != 0 ||
	    !readValue(stream, &version) || version != kVersion ||
	    !readValue(stream, &count) || count > kMaxRecords) {
		records_.clear();
		return false;
	}
	for (std::uint32_t i = 0; i < count; ++i) {
		std::string key;
		Record record;
		unsigned char classification = 0;
		if (!readString(stream, &key) || !readString(stream, &record.buildFingerprint) ||
		    !readValue(stream, &classification) || key.empty() ||
		    (classification != static_cast<unsigned char>(ThemeClassification::INVARIANT) &&
		     classification != static_cast<unsigned char>(ThemeClassification::SENSITIVE))) {
			records_.clear();
			return false;
		}
		record.classification = static_cast<ThemeClassification>(classification);
		records_[std::move(key)] = std::move(record);
	}
	return true;
}

bool ThemeClassifier::save(const std::string& directory) const {
	const std::string path = sidecarPath(directory);
	const std::string temporary = path + ".tmp";
	const std::string backup = path + ".bak";
	std::ofstream stream(temporary.c_str(), std::ios::binary | std::ios::trunc);
	const std::uint32_t count = static_cast<std::uint32_t>(records_.size());
	if (!stream || records_.size() > kMaxRecords ||
	    !stream.write(kMagic, sizeof(kMagic)) || !writeValue(stream, kVersion) ||
	    !writeValue(stream, count)) {
		stream.close();
		std::remove(temporary.c_str());
		return false;
	}
	for (const auto& item : records_) {
		const unsigned char classification = static_cast<unsigned char>(item.second.classification);
		if (!writeString(stream, item.first) || !writeString(stream, item.second.buildFingerprint) ||
		    !writeValue(stream, classification)) {
			stream.close();
			std::remove(temporary.c_str());
			return false;
		}
	}
	stream.flush();
	if (!stream) {
		stream.close();
		std::remove(temporary.c_str());
		return false;
	}
	stream.close();
	std::remove(backup.c_str());
	const bool hadOriginal = std::rename(path.c_str(), backup.c_str()) == 0;
	if (std::rename(temporary.c_str(), path.c_str()) != 0) {
		if (hadOriginal)
			std::rename(backup.c_str(), path.c_str());
		std::remove(temporary.c_str());
		return false;
	}
	if (hadOriginal)
		std::remove(backup.c_str());
	return true;
}

ThemeClassification ThemeClassifier::get(const std::string& cacheKey,
	                                       const std::string& buildFingerprint) const {
	const auto found = records_.find(cacheKey);
	if (found == records_.end() || found->second.buildFingerprint != buildFingerprint)
		return ThemeClassification::UNKNOWN;
	return found->second.classification;
}

bool ThemeClassifier::set(const std::string& cacheKey, const std::string& buildFingerprint,
	                       ThemeClassification classification) {
	if (cacheKey.empty() || buildFingerprint.empty() || classification == ThemeClassification::UNKNOWN)
		return false;
	const auto found = records_.find(cacheKey);
	if (found != records_.end() && found->second.buildFingerprint == buildFingerprint &&
	    found->second.classification == classification)
		return false;
	Record record;
	record.buildFingerprint = buildFingerprint;
	record.classification = classification;
	records_[cacheKey] = std::move(record);
	return true;
}

void ThemeClassifier::clear() {
	records_.clear();
}

}  // namespace deepcache
