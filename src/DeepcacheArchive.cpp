#include "DeepcacheArchive.hpp"

#include "third_party/qoi.h"

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>

namespace deepcache {
namespace {

const char kIndexMagic[8] = {'L', 'V', 'D', 'C', 'I', 'D', 'X', '1'};
const std::uint32_t kIndexVersion = 1;

template <typename T>
bool readValue(std::istream& stream, T& value) {
	return static_cast<bool>(stream.read(reinterpret_cast<char*>(&value), sizeof(value)));
}

template <typename T>
void writeValue(std::ostream& stream, const T& value) {
	stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

bool readString(std::istream& stream, std::string& value) {
	std::uint32_t length = 0;
	if (!readValue(stream, length) || length > 1024u * 1024u)
		return false;
	value.resize(length);
	return length == 0 || static_cast<bool>(stream.read(&value[0], length));
}

void writeString(std::ostream& stream, const std::string& value) {
	const std::uint32_t length = static_cast<std::uint32_t>(value.size());
	writeValue(stream, length);
	if (length)
		stream.write(value.data(), length);
}

bool replaceFile(const std::string& temporary, const std::string& destination) {
	const std::string backup = destination + ".bak";
	std::remove(backup.c_str());
	std::rename(destination.c_str(), backup.c_str());
	if (std::rename(temporary.c_str(), destination.c_str()) == 0) {
		std::remove(backup.c_str());
		return true;
	}
	std::rename(backup.c_str(), destination.c_str());
	return false;
}

bool decodeQoi(const std::uint8_t* data, std::size_t size, DecodedPreview& output) {
	if (!data || size < 14 || size > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		return false;
	const std::uint32_t headerWidth = (std::uint32_t(data[4]) << 24) | (std::uint32_t(data[5]) << 16) |
	                                  (std::uint32_t(data[6]) << 8) | std::uint32_t(data[7]);
	const std::uint32_t headerHeight = (std::uint32_t(data[8]) << 24) | (std::uint32_t(data[9]) << 16) |
	                                   (std::uint32_t(data[10]) << 8) | std::uint32_t(data[11]);
	if (headerWidth == 0 || headerHeight == 0 || headerWidth > 8192 || headerHeight > 8192 ||
	    std::uint64_t(headerWidth) * headerHeight * 4ull > 128ull * 1024ull * 1024ull)
		return false;
	qoi_desc desc = {};
	void* decoded = qoi_decode(data, static_cast<int>(size), &desc, 4);
	if (!decoded || desc.width == 0 || desc.height == 0 || desc.width > 32768 || desc.height > 32768) {
		std::free(decoded);
		return false;
	}
	const std::size_t byteCount = static_cast<std::size_t>(desc.width) * desc.height * 4u;
	output.width = static_cast<int>(desc.width);
	output.height = static_cast<int>(desc.height);
	output.rgba.assign(static_cast<std::uint8_t*>(decoded), static_cast<std::uint8_t*>(decoded) + byteCount);
	std::free(decoded);
	return true;
}

}  // namespace

std::uint64_t deepcacheChecksum(const std::uint8_t* data, std::size_t size) {
	std::uint64_t hash = 1469598103934665603ull;
	for (std::size_t i = 0; i < size; ++i) {
		hash ^= data[i];
		hash *= 1099511628211ull;
	}
	return hash;
}

DeepcacheArchiveWorker::DeepcacheArchiveWorker() = default;

DeepcacheArchiveWorker::~DeepcacheArchiveWorker() {
	shutdown();
}

void DeepcacheArchiveWorker::start(const std::string& directory, std::vector<ArchiveWantedEntry> wanted) {
	if (started_)
		return;
	directory_ = directory;
	packPath_ = directory + "/previews-v1.pack";
	indexPath_ = directory + "/index-v1.bin";
	for (const ArchiveWantedEntry& entry : wanted)
		wanted_[entry.cacheKey] = entry.fingerprint;
	targetCount_.store(static_cast<int>(wanted_.size()), std::memory_order_relaxed);
	setState(DatabaseState::LOADING);
	started_ = true;
	try {
		thread_ = std::thread(&DeepcacheArchiveWorker::run, this);
	}
	catch (...) {
		started_ = false;
		errorCode_.store(4, std::memory_order_relaxed);
		setState(DatabaseState::ERROR);
	}
}

void DeepcacheArchiveWorker::enqueue(PreviewWrite write) {
	if (!started_ || canceled() || write.rgba.empty())
		return;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		writes_.push_back(std::move(write));
	}
	condition_.notify_one();
}

bool DeepcacheArchiveWorker::tryPopDecoded(DecodedPreview& preview) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (decoded_.empty())
		return false;
	preview = std::move(decoded_.front());
	decoded_.pop_front();
	return true;
}

void DeepcacheArchiveWorker::requestCompaction() {
	if (!started_ || canceled())
		return;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		compactRequested_ = true;
	}
	condition_.notify_one();
}

void DeepcacheArchiveWorker::shutdown() {
	if (!started_)
		return;
	setState(DatabaseState::CANCELING);
	stopping_.store(true, std::memory_order_relaxed);
	condition_.notify_all();
	if (thread_.joinable())
		thread_.join();
	started_ = false;
}

void DeepcacheArchiveWorker::run() {
	if (!loadArchive() && !canceled()) {
		errorCode_.store(1, std::memory_order_relaxed);
		setState(DatabaseState::ERROR);
	}
	while (!canceled()) {
		PreviewWrite write;
		bool compact = false;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [&]() { return canceled() || !writes_.empty() || compactRequested_; });
			if (canceled())
				break;
			if (!writes_.empty()) {
				write = std::move(writes_.front());
				writes_.pop_front();
			}
			else {
				compact = compactRequested_;
				compactRequested_ = false;
			}
		}
		const bool ok = compact ? compactArchive() : appendPreview(std::move(write));
		if (!ok && !canceled()) {
			errorCode_.store(compact ? 3 : 2, std::memory_order_relaxed);
			setState(DatabaseState::ERROR);
		}
		else if (!canceled()) {
			std::lock_guard<std::mutex> lock(mutex_);
			if (!compact && writes_.empty() && !compactRequested_) {
				std::uint64_t liveBytes = 0;
				for (const auto& entry : entries_)
					liveBytes += entry.second.length;
				const std::uint64_t packBytes = packBytes_.load(std::memory_order_relaxed);
				const std::uint64_t deadBytes = packBytes > liveBytes ? packBytes - liveBytes : 0;
				if (deadBytes >= 64ull * 1024ull * 1024ull && deadBytes * 4ull >= packBytes) {
					compactRequested_ = true;
					condition_.notify_one();
				}
			}
			if (writes_.empty() && !compactRequested_)
				setState(entries_.empty() ? DatabaseState::EMPTY : DatabaseState::READY);
		}
	}
}

bool DeepcacheArchiveWorker::loadIndex(const std::string& path) {
	std::ifstream stream(path.c_str(), std::ios::binary);
	if (!stream)
		return false;
	char magic[8] = {};
	std::uint32_t version = 0;
	std::uint32_t count = 0;
	if (!stream.read(magic, sizeof(magic)) || std::memcmp(magic, kIndexMagic, sizeof(magic)) != 0 ||
	    !readValue(stream, version) || version != kIndexVersion || !readValue(stream, count) || count > 1000000u)
		return false;
	std::unordered_map<std::string, Entry> loaded;
	for (std::uint32_t i = 0; i < count; ++i) {
		std::string key;
		Entry entry;
		if (!readString(stream, key) || !readString(stream, entry.fingerprint) ||
		    !readValue(stream, entry.offset) || !readValue(stream, entry.length) ||
		    !readValue(stream, entry.checksum) || !readValue(stream, entry.width) ||
		    !readValue(stream, entry.height))
			return false;
		loaded[key] = std::move(entry);
	}
	entries_.swap(loaded);
	return true;
}

bool DeepcacheArchiveWorker::loadArchive() {
	const std::string compactMarker = directory_ + "/compaction-v1.pending";
	std::ifstream marker(compactMarker.c_str(), std::ios::binary);
	if (marker) {
		marker.close();
		const std::string packBackup = packPath_ + ".bak";
		const std::string indexBackup = indexPath_ + ".bak";
		std::ifstream oldPack(packBackup.c_str(), std::ios::binary);
		if (oldPack) {
			oldPack.close();
			std::remove(packPath_.c_str());
			std::rename(packBackup.c_str(), packPath_.c_str());
		}
		std::ifstream oldIndex(indexBackup.c_str(), std::ios::binary);
		if (oldIndex) {
			oldIndex.close();
			std::remove(indexPath_.c_str());
			std::rename(indexBackup.c_str(), indexPath_.c_str());
		}
		std::remove(compactMarker.c_str());
		std::remove((packPath_ + ".tmp").c_str());
		std::remove((indexPath_ + ".compact").c_str());
	}
	std::ifstream pack(packPath_.c_str(), std::ios::binary | std::ios::ate);
	if (!pack) {
		setState(DatabaseState::EMPTY);
		return true;
	}
	const std::streamoff size = pack.tellg();
	if (size < 0)
		return false;
	packedBytes_.resize(static_cast<std::size_t>(size));
	pack.seekg(0);
	if (size > 0 && !pack.read(reinterpret_cast<char*>(packedBytes_.data()), size))
		return false;
	packBytes_.store(packedBytes_.size(), std::memory_order_relaxed);
	if (!loadIndex(indexPath_) && !loadIndex(indexPath_ + ".bak")) {
		entries_.clear();
		setState(DatabaseState::EMPTY);
		return true;
	}
	for (auto it = entries_.begin(); it != entries_.end();) {
		if (wanted_.count(it->first) == 0)
			it = entries_.erase(it);
		else
			++it;
	}

	int ready = 0;
	for (const auto& wanted : wanted_) {
		if (canceled())
			return true;
		const auto found = entries_.find(wanted.first);
		if (found == entries_.end() || found->second.fingerprint != wanted.second)
			continue;
		const Entry& entry = found->second;
		if (entry.offset > packedBytes_.size() || entry.length > packedBytes_.size() - entry.offset)
			continue;
		const std::uint8_t* payload = packedBytes_.data() + entry.offset;
		if (deepcacheChecksum(payload, static_cast<std::size_t>(entry.length)) != entry.checksum)
			continue;
		DecodedPreview preview;
		preview.cacheKey = wanted.first;
		preview.fingerprint = wanted.second;
		if (!decodeQoi(payload, static_cast<std::size_t>(entry.length), preview) ||
		    preview.width != static_cast<int>(entry.width) || preview.height != static_cast<int>(entry.height))
			continue;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			decoded_.push_back(std::move(preview));
		}
		readyKeys_.insert(wanted.first);
		readyCount_.store(++ready, std::memory_order_relaxed);
	}
	setState(entries_.empty() ? DatabaseState::EMPTY : DatabaseState::READY);
	return true;
}

bool DeepcacheArchiveWorker::saveIndexAtomically() {
	const std::string temporary = indexPath_ + ".tmp";
	std::ofstream stream(temporary.c_str(), std::ios::binary | std::ios::trunc);
	if (!stream)
		return false;
	stream.write(kIndexMagic, sizeof(kIndexMagic));
	writeValue(stream, kIndexVersion);
	const std::uint32_t count = static_cast<std::uint32_t>(entries_.size());
	writeValue(stream, count);
	for (const auto& item : entries_) {
		writeString(stream, item.first);
		writeString(stream, item.second.fingerprint);
		writeValue(stream, item.second.offset);
		writeValue(stream, item.second.length);
		writeValue(stream, item.second.checksum);
		writeValue(stream, item.second.width);
		writeValue(stream, item.second.height);
	}
	stream.flush();
	if (!stream)
		return false;
	stream.close();
	return replaceFile(temporary, indexPath_);
}

bool DeepcacheArchiveWorker::appendPreview(PreviewWrite write) {
	if (canceled())
		return true;
	setState(DatabaseState::UPDATING);
	qoi_desc desc = {};
	desc.width = static_cast<unsigned int>(write.width);
	desc.height = static_cast<unsigned int>(write.height);
	desc.channels = 4;
	desc.colorspace = QOI_SRGB;
	int encodedLength = 0;
	void* encoded = qoi_encode(write.rgba.data(), &desc, &encodedLength);
	if (!encoded || encodedLength <= 0) {
		std::free(encoded);
		return false;
	}
	if (canceled()) {
		std::free(encoded);
		return true;
	}
	std::ofstream pack(packPath_.c_str(), std::ios::binary | std::ios::app);
	if (!pack) {
		std::free(encoded);
		return false;
	}
	const std::uint64_t offset = packBytes_.load(std::memory_order_relaxed);
	pack.write(static_cast<const char*>(encoded), encodedLength);
	pack.flush();
	if (!pack) {
		std::free(encoded);
		return false;
	}
	Entry entry;
	entry.fingerprint = std::move(write.fingerprint);
	entry.offset = offset;
	entry.length = static_cast<std::uint64_t>(encodedLength);
	entry.checksum = deepcacheChecksum(static_cast<const std::uint8_t*>(encoded), encodedLength);
	entry.width = static_cast<std::uint32_t>(write.width);
	entry.height = static_cast<std::uint32_t>(write.height);
	const std::uint8_t* encodedBytes = static_cast<const std::uint8_t*>(encoded);
	packedBytes_.insert(packedBytes_.end(), encodedBytes, encodedBytes + encodedLength);
	std::free(encoded);
	entries_[write.cacheKey] = std::move(entry);
	const auto wanted = wanted_.find(write.cacheKey);
	if (wanted != wanted_.end() && wanted->second == entries_[write.cacheKey].fingerprint) {
		readyKeys_.insert(write.cacheKey);
		readyCount_.store(static_cast<int>(readyKeys_.size()), std::memory_order_relaxed);
	}
	packBytes_.store(offset + static_cast<std::uint64_t>(encodedLength), std::memory_order_relaxed);
	if (canceled())
		return true; // Appended bytes are harmless until the index commits.
	return saveIndexAtomically();
}

bool DeepcacheArchiveWorker::compactArchive() {
	if (entries_.empty() || canceled())
		return true;
	setState(DatabaseState::COMPACTING);
	const std::string temporaryPack = packPath_ + ".tmp";
	std::ofstream output(temporaryPack.c_str(), std::ios::binary | std::ios::trunc);
	if (!output)
		return false;
	std::unordered_map<std::string, Entry> compacted;
	std::uint64_t offset = 0;
	for (const auto& item : entries_) {
		if (canceled()) {
			output.close();
			std::remove(temporaryPack.c_str());
			return true;
		}
		const Entry& source = item.second;
		if (source.offset > packedBytes_.size() || source.length > packedBytes_.size() - source.offset)
			continue;
		output.write(reinterpret_cast<const char*>(packedBytes_.data() + source.offset), source.length);
		if (!output)
			return false;
		Entry destination = source;
		destination.offset = offset;
		offset += source.length;
		compacted[item.first] = std::move(destination);
	}
	output.flush();
	if (!output)
		return false;
	output.close();
	if (canceled()) {
		std::remove(temporaryPack.c_str());
		return true;
	}
	// Save the compacted index first as a temporary, but commit the pack before
	// publishing that index. The old index backup remains recoverable.
	auto previous = std::move(entries_);
	entries_ = compacted;
	const std::string stagedIndex = indexPath_ + ".compact";
	const std::string normalIndex = indexPath_;
	indexPath_ = stagedIndex;
	const bool staged = saveIndexAtomically();
	indexPath_ = normalIndex;
	if (!staged || canceled()) {
		entries_ = std::move(previous);
		std::remove(temporaryPack.c_str());
		std::remove(stagedIndex.c_str());
		return staged;
	}
	const std::string markerPath = directory_ + "/compaction-v1.pending";
	{
		std::ofstream marker(markerPath.c_str(), std::ios::binary | std::ios::trunc);
		if (!marker)
			return false;
		marker << "pending";
		marker.flush();
		if (!marker)
			return false;
	}
	const std::string packBackup = packPath_ + ".bak";
	const std::string indexBackup = indexPath_ + ".bak";
	std::remove(packBackup.c_str());
	std::remove(indexBackup.c_str());
	if (std::rename(packPath_.c_str(), packBackup.c_str()) != 0 ||
	    std::rename(temporaryPack.c_str(), packPath_.c_str()) != 0 ||
	    std::rename(indexPath_.c_str(), indexBackup.c_str()) != 0 ||
	    std::rename(stagedIndex.c_str(), indexPath_.c_str()) != 0) {
		std::remove(packPath_.c_str());
		std::rename(packBackup.c_str(), packPath_.c_str());
		std::remove(indexPath_.c_str());
		std::rename(indexBackup.c_str(), indexPath_.c_str());
		std::remove(markerPath.c_str());
		entries_ = std::move(previous);
		return false;
	}
	std::remove(markerPath.c_str());
	std::remove(packBackup.c_str());
	std::remove(indexBackup.c_str());
	std::ifstream input(packPath_.c_str(), std::ios::binary | std::ios::ate);
	if (!input)
		return false;
	packedBytes_.resize(static_cast<std::size_t>(input.tellg()));
	input.seekg(0);
	if (!packedBytes_.empty())
		input.read(reinterpret_cast<char*>(packedBytes_.data()), packedBytes_.size());
	packBytes_.store(packedBytes_.size(), std::memory_order_relaxed);
	return static_cast<bool>(input) || packedBytes_.empty();
}

}  // namespace deepcache
