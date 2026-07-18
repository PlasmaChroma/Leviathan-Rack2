#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace deepcache {

enum class DatabaseState {
	EMPTY,
	LOADING,
	READY,
	UPDATING,
	COMPACTING,
	CANCELING,
	ERROR
};

struct ArchiveWantedEntry {
	std::string cacheKey;
	std::string fingerprint;
};

struct DecodedPreview {
	std::string cacheKey;
	std::string fingerprint;
	int width = 0;
	int height = 0;
	std::vector<std::uint8_t> rgba;
};

struct PreviewWrite {
	std::string cacheKey;
	std::string fingerprint;
	int width = 0;
	int height = 0;
	std::vector<std::uint8_t> rgba;
};

// Owns all disk I/O and QOI work. The UI thread only submits copied RGBA data
// and drains decoded results. shutdown() is cooperatively cancelable and joins.
class DeepcacheArchiveWorker {
public:
	DeepcacheArchiveWorker();
	~DeepcacheArchiveWorker();

	DeepcacheArchiveWorker(const DeepcacheArchiveWorker&) = delete;
	DeepcacheArchiveWorker& operator=(const DeepcacheArchiveWorker&) = delete;

	void start(const std::string& directory, std::vector<ArchiveWantedEntry> wanted);
	void enqueue(PreviewWrite write);
	bool tryPopDecoded(DecodedPreview& preview);
	void requestCompaction();
	void shutdown();

	DatabaseState state() const { return static_cast<DatabaseState>(state_.load(std::memory_order_relaxed)); }
	int readyCount() const { return readyCount_.load(std::memory_order_relaxed); }
	int targetCount() const { return targetCount_.load(std::memory_order_relaxed); }
	std::uint64_t packBytes() const { return packBytes_.load(std::memory_order_relaxed); }
	int errorCode() const { return errorCode_.load(std::memory_order_relaxed); }

private:
	struct Entry {
		std::string fingerprint;
		std::uint64_t offset = 0;
		std::uint64_t length = 0;
		std::uint64_t checksum = 0;
		std::uint32_t width = 0;
		std::uint32_t height = 0;
	};

	void run();
	bool loadArchive();
	bool appendPreview(PreviewWrite write);
	bool compactArchive();
	bool loadIndex(const std::string& path);
	bool saveIndexAtomically();
	bool canceled() const { return stopping_.load(std::memory_order_relaxed); }
	void setState(DatabaseState state) { state_.store(static_cast<int>(state), std::memory_order_relaxed); }

	std::string directory_;
	std::string packPath_;
	std::string indexPath_;
	std::unordered_map<std::string, std::string> wanted_;
	std::unordered_map<std::string, Entry> entries_;
	std::unordered_set<std::string> readyKeys_;
	std::vector<std::uint8_t> packedBytes_;

	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::thread thread_;
	std::deque<PreviewWrite> writes_;
	std::deque<DecodedPreview> decoded_;
	bool compactRequested_ = false;
	bool started_ = false;
	std::atomic<bool> stopping_ {false};
	std::atomic<int> state_ {static_cast<int>(DatabaseState::EMPTY)};
	std::atomic<int> readyCount_ {0};
	std::atomic<int> targetCount_ {0};
	std::atomic<std::uint64_t> packBytes_ {0};
	std::atomic<int> errorCode_ {0};
};

std::uint64_t deepcacheChecksum(const std::uint8_t* data, std::size_t size);

}  // namespace deepcache
