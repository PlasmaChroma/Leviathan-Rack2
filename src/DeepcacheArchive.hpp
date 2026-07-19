#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <memory>
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
	BUSY,
	READ_ONLY,
	ERROR
};

struct ArchiveWantedEntry {
	std::string cacheKey;
	std::string fingerprint;
	std::string pluginKey;
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
	std::shared_ptr<const std::vector<std::uint8_t>> rgba;
};

// Owns all disk I/O and QOI work. The UI thread submits immutable shared RGBA
// data and drains decoded results. One worker holds the archive write lease; a
// contender loads a validated read-only snapshot and keeps new work in memory.
// shutdown() cancels queued work, interrupts encode/read/write loops at small
// boundaries, and joins.
class DeepcacheArchiveWorker {
public:
	DeepcacheArchiveWorker();
	~DeepcacheArchiveWorker();

	DeepcacheArchiveWorker(const DeepcacheArchiveWorker&) = delete;
	DeepcacheArchiveWorker& operator=(const DeepcacheArchiveWorker&) = delete;

	void start(const std::string& directory, std::vector<ArchiveWantedEntry> wanted);
	void markUnavailable(int errorCode);
	bool enqueue(PreviewWrite write);
	void discardPendingWrites();
	bool canAcceptWrite() const;
	bool tryPopDecoded(DecodedPreview& preview);
	bool hasPendingDecoded() const;
	void requestCompaction();
	void shutdown();

	DatabaseState state() const { return static_cast<DatabaseState>(state_.load(std::memory_order_relaxed)); }
	int readyCount() const { return readyCount_.load(std::memory_order_relaxed); }
	int targetCount() const { return targetCount_.load(std::memory_order_relaxed); }
	int readyPluginCount() const { return readyPluginCount_.load(std::memory_order_relaxed); }
	int targetPluginCount() const { return targetPluginCount_.load(std::memory_order_relaxed); }
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
	void runOwned();
	bool acquireLease();
	void releaseLease();
	bool loadArchive(bool allowRecovery);
	bool appendPreview(PreviewWrite write);
	bool compactArchive();
	bool readPackFile(const std::string& path, std::vector<std::uint8_t>& bytes);
	bool loadIndex(const std::string& path);
	bool saveIndexAtomically();
	bool canceled() const { return stopping_.load(std::memory_order_relaxed); }
	void setState(DatabaseState state) { state_.store(static_cast<int>(state), std::memory_order_relaxed); }
	void markReady(const std::string& cacheKey);

	std::string directory_;
	std::string packPath_;
	std::string indexPath_;
	std::unordered_map<std::string, std::string> wanted_;
	std::unordered_map<std::string, std::string> wantedPluginByKey_;
	std::unordered_map<std::string, int> pluginTargetCounts_;
	std::unordered_map<std::string, int> pluginReadyCounts_;
	std::unordered_map<std::string, Entry> entries_;
	std::unordered_set<std::string> readyKeys_;
	std::unordered_set<std::string> readyPlugins_;
	std::vector<std::uint8_t> packedBytes_;

	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::thread thread_;
	std::deque<PreviewWrite> writes_;
	std::size_t queuedWriteBytes_ = 0;
	std::deque<DecodedPreview> decoded_;
	std::size_t decodedBytes_ = 0;
	bool compactRequested_ = false;
	bool started_ = false;
	std::atomic<bool> stopping_ {false};
	std::atomic<bool> fatalError_ {false};
	std::atomic<bool> leaseUnavailable_ {false};
	std::atomic<int> state_ {static_cast<int>(DatabaseState::EMPTY)};
	std::atomic<int> readyCount_ {0};
	std::atomic<int> targetCount_ {0};
	std::atomic<int> readyPluginCount_ {0};
	std::atomic<int> targetPluginCount_ {0};
	std::atomic<std::uint64_t> packBytes_ {0};
	std::atomic<int> errorCode_ {0};
	std::uintptr_t leaseHandle_ = 0;
};

std::uint64_t deepcacheChecksum(const std::uint8_t* data, std::size_t size);

}  // namespace deepcache
