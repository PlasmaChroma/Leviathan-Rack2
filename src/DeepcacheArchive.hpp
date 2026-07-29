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
#include <utility>
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
	// Zero identifies the initial archive load. Later values identify the
	// graphics-context generation that requested an indexed QOI re-decode.
	std::uint64_t decodeGeneration = 0;
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
	// Re-decodes an already validated entry from its bounded on-disk pack span.
	// The archive worker owns the I/O and read-only workers may use it safely.
	bool requestDecode(const std::string& cacheKey, std::uint64_t decodeGeneration);
	void discardPendingDecodes();
	// Reports a successful append/index commit so the UI can release the source
	// RGBA knowing that a recoverable compressed representation now exists.
	bool tryPopCommitted(std::string& cacheKey);
	void requestCompaction();
	// Discards the persistent archive on the worker thread while it owns the
	// write lease. The worker remains alive and accepts fresh preview writes
	// after the reset completes.
	bool requestReset();
	void shutdown();

	DatabaseState state() const { return static_cast<DatabaseState>(state_.load(std::memory_order_relaxed)); }
	int readyCount() const { return readyCount_.load(std::memory_order_relaxed); }
	int targetCount() const { return targetCount_.load(std::memory_order_relaxed); }
	int readyPluginCount() const { return readyPluginCount_.load(std::memory_order_relaxed); }
	int targetPluginCount() const { return targetPluginCount_.load(std::memory_order_relaxed); }
	std::uint64_t packBytes() const { return packBytes_.load(std::memory_order_relaxed); }
	std::uint64_t hotCompressedBytes() const {
		return volatileBytes_.load(std::memory_order_relaxed);
	}
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
	struct DecodeRequest {
		std::string cacheKey;
		std::uint64_t generation = 0;

		DecodeRequest() = default;
		DecodeRequest(std::string cacheKey, std::uint64_t generation)
			: cacheKey(std::move(cacheKey)), generation(generation) {}
	};
	struct VolatileEntry {
		std::string fingerprint;
		int width = 0;
		int height = 0;
		std::vector<std::uint8_t> qoi;
	};

	void run();
	void runOwned();
	bool acquireLease();
	void releaseLease();
	bool loadArchive(bool allowRecovery);
	bool decodeEntry(const DecodeRequest& request);
	bool pushDecoded(DecodedPreview preview);
	bool storeVolatilePreview(PreviewWrite write);
	bool appendPreview(PreviewWrite write);
	bool compactArchive();
	bool resetArchive();
	bool shouldCompactArchive() const;
	bool getPackFileSize(std::uint64_t& size) const;
	bool readPackRange(std::uint64_t offset, std::uint64_t length,
	                   std::vector<std::uint8_t>& bytes) const;
	bool loadIndex(const std::string& path);
	bool saveIndexAtomically();
	bool canceled() const { return stopping_.load(std::memory_order_relaxed); }
	bool resetPending() const;
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

	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::thread thread_;
	std::deque<PreviewWrite> writes_;
	std::size_t queuedWriteBytes_ = 0;
	std::deque<PreviewWrite> volatileWrites_;
	std::size_t queuedVolatileWriteBytes_ = 0;
	std::deque<DecodedPreview> decoded_;
	std::size_t decodedBytes_ = 0;
	std::deque<DecodeRequest> decodeRequests_;
	std::unordered_map<std::string, std::uint64_t> requestedDecodeGeneration_;
	std::deque<std::string> committed_;
	std::unordered_map<std::string, VolatileEntry> volatileEntries_;
	bool compactRequested_ = false;
	bool resetRequested_ = false;
	bool started_ = false;
	std::atomic<bool> stopping_ {false};
	std::atomic<bool> fatalError_ {false};
	std::atomic<bool> leaseUnavailable_ {false};
	std::atomic<bool> ownsLease_ {false};
	std::atomic<int> state_ {static_cast<int>(DatabaseState::EMPTY)};
	std::atomic<int> readyCount_ {0};
	std::atomic<int> targetCount_ {0};
	std::atomic<int> readyPluginCount_ {0};
	std::atomic<int> targetPluginCount_ {0};
	std::atomic<std::uint64_t> packBytes_ {0};
	std::atomic<std::uint64_t> volatileBytes_ {0};
	std::atomic<int> errorCode_ {0};
	std::uintptr_t leaseHandle_ = 0;
};

std::uint64_t deepcacheChecksum(const std::uint8_t* data, std::size_t size);

}  // namespace deepcache
