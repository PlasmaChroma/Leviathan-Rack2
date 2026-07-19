#include "DeepcacheArchive.hpp"

#include "third_party/qoi.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// wingdi.h defines ERROR as a macro, which collides with DatabaseState::ERROR.
#ifdef ERROR
#undef ERROR
#endif
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

namespace deepcache {
namespace {

const char kIndexMagic[8] = {'L', 'V', 'D', 'C', 'I', 'D', 'X', '1'};
const std::uint32_t kIndexVersion = 1;
const std::size_t kMaxDecodedQueueEntries = 16;
const std::size_t kMaxDecodedQueueBytes = 64u * 1024u * 1024u;
const std::size_t kMaxWriteQueueEntries = 16;
const std::size_t kMaxWriteQueueBytes = 64u * 1024u * 1024u;
const std::size_t kReadChunkBytes = 4u * 1024u * 1024u;
const std::size_t kWriteChunkBytes = 1u * 1024u * 1024u;
const std::uint32_t kMaxIndexEntries = 100000u;
const std::uint32_t kMaxCacheKeyBytes = 4096u;
const std::uint32_t kMaxFingerprintBytes = 1024u;

struct QoiPixel {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
	std::uint8_t a;

	QoiPixel(std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0, std::uint8_t a = 0)
		: r(r), g(g), b(b), a(a) {}
};

bool samePixel(const QoiPixel& a, const QoiPixel& b) {
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

std::size_t qoiHash(const QoiPixel& pixel) {
	return (pixel.r * 3u + pixel.g * 5u + pixel.b * 7u + pixel.a * 11u) & 63u;
}

void appendBigEndian32(std::vector<std::uint8_t>& output, std::uint32_t value) {
	output.push_back(static_cast<std::uint8_t>((value >> 24) & 0xff));
	output.push_back(static_cast<std::uint8_t>((value >> 16) & 0xff));
	output.push_back(static_cast<std::uint8_t>((value >> 8) & 0xff));
	output.push_back(static_cast<std::uint8_t>(value & 0xff));
}

bool encodeQoiCancelable(const std::vector<std::uint8_t>& rgba, int width, int height,
	                     const std::atomic<bool>& stopping, std::vector<std::uint8_t>& output) {
	if (width <= 0 || height <= 0)
		return false;
	const std::size_t pixelCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
	if (pixelCount > std::numeric_limits<std::size_t>::max() / 4u || rgba.size() < pixelCount * 4u)
		return false;
	output.clear();
	output.reserve(pixelCount * 5u + 22u);
	appendBigEndian32(output, 0x716f6966u);
	appendBigEndian32(output, static_cast<std::uint32_t>(width));
	appendBigEndian32(output, static_cast<std::uint32_t>(height));
	output.push_back(4);
	output.push_back(QOI_SRGB);

	QoiPixel index[64] = {};
	QoiPixel previous;
	previous.a = 255;
	int run = 0;
	for (std::size_t i = 0; i < pixelCount; ++i) {
		if ((i & 4095u) == 0 && stopping.load(std::memory_order_relaxed))
			return false;
		const std::size_t offset = i * 4u;
		QoiPixel pixel(rgba[offset], rgba[offset + 1], rgba[offset + 2], rgba[offset + 3]);
		if (samePixel(pixel, previous)) {
			++run;
			if (run == 62 || i + 1 == pixelCount) {
				output.push_back(static_cast<std::uint8_t>(0xc0 | (run - 1)));
				run = 0;
			}
		}
		else {
			if (run > 0) {
				output.push_back(static_cast<std::uint8_t>(0xc0 | (run - 1)));
				run = 0;
			}
			const std::size_t indexPosition = qoiHash(pixel);
			if (samePixel(index[indexPosition], pixel)) {
				output.push_back(static_cast<std::uint8_t>(indexPosition));
			}
			else {
				index[indexPosition] = pixel;
				if (pixel.a == previous.a) {
					const int dr = int(pixel.r) - int(previous.r);
					const int dg = int(pixel.g) - int(previous.g);
					const int db = int(pixel.b) - int(previous.b);
					const int drdg = dr - dg;
					const int dbdg = db - dg;
					if (dr > -3 && dr < 2 && dg > -3 && dg < 2 && db > -3 && db < 2) {
						output.push_back(static_cast<std::uint8_t>(0x40 | ((dr + 2) << 4) |
						                                           ((dg + 2) << 2) | (db + 2)));
					}
					else if (drdg > -9 && drdg < 8 && dg > -33 && dg < 32 && dbdg > -9 && dbdg < 8) {
						output.push_back(static_cast<std::uint8_t>(0x80 | (dg + 32)));
						output.push_back(static_cast<std::uint8_t>(((drdg + 8) << 4) | (dbdg + 8)));
					}
					else {
						output.push_back(0xfe);
						output.push_back(pixel.r);
						output.push_back(pixel.g);
						output.push_back(pixel.b);
					}
				}
				else {
					output.push_back(0xff);
					output.push_back(pixel.r);
					output.push_back(pixel.g);
					output.push_back(pixel.b);
					output.push_back(pixel.a);
				}
			}
		}
		previous = pixel;
	}
	static const std::uint8_t padding[8] = {0, 0, 0, 0, 0, 0, 0, 1};
	output.insert(output.end(), padding, padding + sizeof(padding));
	return !stopping.load(std::memory_order_relaxed);
}

bool writeCancelable(std::ostream& stream, const std::uint8_t* data, std::size_t size,
	                 const std::atomic<bool>& stopping) {
	std::size_t offset = 0;
	while (offset < size) {
		if (stopping.load(std::memory_order_relaxed))
			return false;
		const std::size_t count = std::min(kWriteChunkBytes, size - offset);
		stream.write(reinterpret_cast<const char*>(data + offset), static_cast<std::streamsize>(count));
		if (!stream)
			return false;
		offset += count;
	}
	return true;
}

template <typename T>
bool readValue(std::istream& stream, T& value) {
	return static_cast<bool>(stream.read(reinterpret_cast<char*>(&value), sizeof(value)));
}

template <typename T>
void writeValue(std::ostream& stream, const T& value) {
	stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

bool readString(std::istream& stream, std::string& value, std::uint32_t maxLength) {
	std::uint32_t length = 0;
	if (!readValue(stream, length) || length > maxLength)
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

void DeepcacheArchiveWorker::markUnavailable(int errorCode) {
	if (started_)
		return;
	fatalError_.store(true, std::memory_order_relaxed);
	errorCode_.store(errorCode, std::memory_order_relaxed);
	setState(DatabaseState::ERROR);
}

void DeepcacheArchiveWorker::start(const std::string& directory, std::vector<ArchiveWantedEntry> wanted) {
	if (started_)
		return;
	directory_ = directory;
	packPath_ = directory + "/previews-v1.pack";
	indexPath_ = directory + "/index-v1.bin";
	for (const ArchiveWantedEntry& entry : wanted) {
		if (wanted_.count(entry.cacheKey) != 0) {
			wanted_[entry.cacheKey] = entry.fingerprint;
			continue;
		}
		wanted_[entry.cacheKey] = entry.fingerprint;
		const std::string pluginKey = entry.pluginKey.empty() ? entry.cacheKey : entry.pluginKey;
		wantedPluginByKey_[entry.cacheKey] = pluginKey;
		pluginTargetCounts_[pluginKey]++;
	}
	targetCount_.store(static_cast<int>(wanted_.size()), std::memory_order_relaxed);
	targetPluginCount_.store(static_cast<int>(pluginTargetCounts_.size()), std::memory_order_relaxed);
	setState(DatabaseState::LOADING);
	started_ = true;
	try {
		thread_ = std::thread(&DeepcacheArchiveWorker::run, this);
	}
	catch (...) {
		started_ = false;
		fatalError_.store(true, std::memory_order_relaxed);
		errorCode_.store(4, std::memory_order_relaxed);
		setState(DatabaseState::ERROR);
	}
}

bool DeepcacheArchiveWorker::enqueue(PreviewWrite write) {
	if (!started_ || canceled() || fatalError_.load(std::memory_order_relaxed) ||
	    !write.rgba || write.rgba->empty() ||
	    write.rgba->size() > 128u * 1024u * 1024u)
		return false;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		// Recheck after acquiring the queue mutex so shutdown or a failed lease
		// cannot race an already-entered producer and leave pixels stranded.
		if (canceled() || fatalError_.load(std::memory_order_relaxed))
			return false;
		if (leaseUnavailable_.load(std::memory_order_relaxed)) {
			if (volatileWrites_.size() >= kMaxWriteQueueEntries ||
			    queuedVolatileWriteBytes_ >= kMaxWriteQueueBytes)
				return false;
			queuedVolatileWriteBytes_ += write.rgba->size();
			volatileWrites_.push_back(std::move(write));
		}
		else {
			if (writes_.size() >= kMaxWriteQueueEntries || queuedWriteBytes_ >= kMaxWriteQueueBytes)
				return false;
			queuedWriteBytes_ += write.rgba->size();
			writes_.push_back(std::move(write));
		}
	}
	condition_.notify_one();
	return true;
}

void DeepcacheArchiveWorker::discardPendingWrites() {
	std::lock_guard<std::mutex> lock(mutex_);
	writes_.clear();
	queuedWriteBytes_ = 0;
	volatileWrites_.clear();
	queuedVolatileWriteBytes_ = 0;
}

bool DeepcacheArchiveWorker::canAcceptWrite() const {
	if (!started_ || canceled() || fatalError_.load(std::memory_order_relaxed))
		return true;
	std::lock_guard<std::mutex> lock(mutex_);
	if (leaseUnavailable_.load(std::memory_order_relaxed))
		return volatileWrites_.size() < kMaxWriteQueueEntries &&
		       queuedVolatileWriteBytes_ < kMaxWriteQueueBytes;
	return writes_.size() < kMaxWriteQueueEntries && queuedWriteBytes_ < kMaxWriteQueueBytes;
}

bool DeepcacheArchiveWorker::tryPopDecoded(DecodedPreview& preview) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (decoded_.empty())
			return false;
		const std::size_t byteCount = decoded_.front().rgba.size();
		preview = std::move(decoded_.front());
		decoded_.pop_front();
		decodedBytes_ = byteCount <= decodedBytes_ ? decodedBytes_ - byteCount : 0;
	}
	condition_.notify_one();
	return true;
}

bool DeepcacheArchiveWorker::hasPendingDecoded() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return !decoded_.empty();
}

bool DeepcacheArchiveWorker::requestDecode(const std::string& cacheKey,
	                                        std::uint64_t decodeGeneration) {
	if (!started_ || cacheKey.empty() || canceled() || fatalError_.load(std::memory_order_relaxed))
		return false;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (canceled() || fatalError_.load(std::memory_order_relaxed))
			return false;
		const auto existing = requestedDecodeGeneration_.find(cacheKey);
		if (existing != requestedDecodeGeneration_.end() && existing->second == decodeGeneration)
			return true;
		requestedDecodeGeneration_[cacheKey] = decodeGeneration;
		decodeRequests_.push_back({cacheKey, decodeGeneration});
	}
	condition_.notify_one();
	return true;
}

void DeepcacheArchiveWorker::discardPendingDecodes() {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		decodeRequests_.clear();
		requestedDecodeGeneration_.clear();
		decoded_.clear();
		decodedBytes_ = 0;
	}
	condition_.notify_all();
}

bool DeepcacheArchiveWorker::tryPopCommitted(std::string& cacheKey) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (committed_.empty())
		return false;
	cacheKey = std::move(committed_.front());
	committed_.pop_front();
	return true;
}

void DeepcacheArchiveWorker::markReady(const std::string& cacheKey) {
	if (!readyKeys_.insert(cacheKey).second)
		return;
	readyCount_.store(static_cast<int>(readyKeys_.size()), std::memory_order_relaxed);
	const auto plugin = wantedPluginByKey_.find(cacheKey);
	if (plugin == wantedPluginByKey_.end())
		return;
	const int ready = ++pluginReadyCounts_[plugin->second];
	const auto target = pluginTargetCounts_.find(plugin->second);
	if (target != pluginTargetCounts_.end() && ready >= target->second) {
		readyPlugins_.insert(plugin->second);
		readyPluginCount_.store(static_cast<int>(readyPlugins_.size()), std::memory_order_relaxed);
	}
}

void DeepcacheArchiveWorker::requestCompaction() {
	if (!started_ || canceled() || fatalError_.load(std::memory_order_relaxed) ||
	    leaseUnavailable_.load(std::memory_order_relaxed))
		return;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		compactRequested_ = true;
	}
	condition_.notify_one();
}

bool DeepcacheArchiveWorker::requestReset() {
	if (!started_ || canceled() || leaseUnavailable_.load(std::memory_order_relaxed))
		return false;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		// Accept a reset while initial lease acquisition is still in progress. If
		// this worker becomes a read-only contender, run() clears the request
		// without touching the shared archive.
		if (canceled() || leaseUnavailable_.load(std::memory_order_relaxed) ||
		    (!ownsLease_.load(std::memory_order_relaxed) && state() != DatabaseState::LOADING))
			return false;
		writes_.clear();
		queuedWriteBytes_ = 0;
		volatileWrites_.clear();
		queuedVolatileWriteBytes_ = 0;
		decoded_.clear();
		decodedBytes_ = 0;
		decodeRequests_.clear();
		requestedDecodeGeneration_.clear();
		committed_.clear();
		compactRequested_ = false;
		resetRequested_ = true;
		// Publish LOADING before releasing the mutex. The worker can already be
		// between operations, so publishing afterward could overwrite its fast
		// EMPTY completion and strand the visible state at LOADING.
		setState(DatabaseState::LOADING);
	}
	condition_.notify_all();
	return true;
}

void DeepcacheArchiveWorker::shutdown() {
	if (!started_)
		return;
	setState(DatabaseState::CANCELING);
	stopping_.store(true, std::memory_order_relaxed);
	{
		std::lock_guard<std::mutex> lock(mutex_);
		writes_.clear();
		queuedWriteBytes_ = 0;
		volatileWrites_.clear();
		queuedVolatileWriteBytes_ = 0;
		decoded_.clear();
		decodedBytes_ = 0;
		decodeRequests_.clear();
		requestedDecodeGeneration_.clear();
		committed_.clear();
		compactRequested_ = false;
		resetRequested_ = false;
	}
	condition_.notify_all();
	if (thread_.joinable())
		thread_.join();
	started_ = false;
}

void DeepcacheArchiveWorker::run() {
	if (!acquireLease()) {
		if (errorCode_.load(std::memory_order_relaxed) != 0) {
			fatalError_.store(true, std::memory_order_relaxed);
			{
				std::lock_guard<std::mutex> lock(mutex_);
				writes_.clear();
				queuedWriteBytes_ = 0;
				volatileWrites_.clear();
				queuedVolatileWriteBytes_ = 0;
				compactRequested_ = false;
			}
			setState(DatabaseState::ERROR);
			return;
		}
		leaseUnavailable_.store(true, std::memory_order_relaxed);
		{
			std::lock_guard<std::mutex> lock(mutex_);
			writes_.clear();
			queuedWriteBytes_ = 0;
			volatileWrites_.clear();
			queuedVolatileWriteBytes_ = 0;
			decodeRequests_.clear();
			requestedDecodeGeneration_.clear();
			committed_.clear();
			compactRequested_ = false;
			resetRequested_ = false;
		}
		// A lease contender must never repair, append, or compact the shared
		// archive, but it can safely consume a committed pack/index snapshot.
		// Brief retries cover the small rename window during owner compaction.
		bool loadedReadOnly = false;
		for (int attempt = 0; attempt < 20 && !canceled(); ++attempt) {
			if (loadArchive(false)) {
				loadedReadOnly = true;
				break;
			}
			std::unique_lock<std::mutex> retryLock(mutex_);
			condition_.wait_for(retryLock, std::chrono::milliseconds(100), [&]() { return canceled(); });
		}
		setState(loadedReadOnly && !entries_.empty() ? DatabaseState::READ_ONLY : DatabaseState::BUSY);
		while (!canceled()) {
			DecodeRequest request;
			PreviewWrite volatileWrite;
			bool encodeVolatile = false;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.wait(lock, [&]() {
					return canceled() || !volatileWrites_.empty() || !decodeRequests_.empty();
				});
				if (canceled())
					break;
				if (!volatileWrites_.empty()) {
					const std::size_t byteCount = volatileWrites_.front().rgba ? volatileWrites_.front().rgba->size() : 0;
					volatileWrite = std::move(volatileWrites_.front());
					volatileWrites_.pop_front();
					queuedVolatileWriteBytes_ = byteCount <= queuedVolatileWriteBytes_
					                              ? queuedVolatileWriteBytes_ - byteCount : 0;
					encodeVolatile = true;
				}
				else {
					request = std::move(decodeRequests_.front());
					decodeRequests_.pop_front();
					const auto latest = requestedDecodeGeneration_.find(request.cacheKey);
					if (latest == requestedDecodeGeneration_.end() || latest->second != request.generation)
						continue;
					requestedDecodeGeneration_.erase(latest);
				}
			}
			condition_.notify_all();
			if (encodeVolatile)
				storeVolatilePreview(std::move(volatileWrite));
			else
				decodeEntry(request);
		}
		return;
	}
	ownsLease_.store(true, std::memory_order_relaxed);
	runOwned();
	ownsLease_.store(false, std::memory_order_relaxed);
	releaseLease();
}

void DeepcacheArchiveWorker::runOwned() {
	bool needsLoad = true;
	while (!canceled()) {
		try {
			if (needsLoad) {
				bool reset = false;
				{
					std::lock_guard<std::mutex> lock(mutex_);
					reset = resetRequested_;
					resetRequested_ = false;
				}
				if (reset && !resetArchive()) {
					fatalError_.store(true, std::memory_order_relaxed);
					errorCode_.store(8, std::memory_order_relaxed);
					setState(DatabaseState::ERROR);
				}
				else if (!loadArchive(true)) {
					std::lock_guard<std::mutex> lock(mutex_);
					if (resetRequested_) {
						setState(DatabaseState::LOADING);
						needsLoad = true;
						continue;
					}
					if (!canceled()) {
						fatalError_.store(true, std::memory_order_relaxed);
						errorCode_.store(1, std::memory_order_relaxed);
						setState(DatabaseState::ERROR);
					}
				}
				else {
					std::lock_guard<std::mutex> lock(mutex_);
					if (resetRequested_) {
						setState(DatabaseState::LOADING);
						needsLoad = true;
						continue;
					}
					fatalError_.store(false, std::memory_order_relaxed);
					errorCode_.store(0, std::memory_order_relaxed);
					if (!canceled() && shouldCompactArchive())
						compactRequested_ = true;
				}
				needsLoad = false;
			}

			if (fatalError_.load(std::memory_order_relaxed)) {
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.wait(lock, [&]() { return canceled() || resetRequested_; });
				if (canceled())
					break;
				needsLoad = true;
				continue;
			}

			PreviewWrite write;
			DecodeRequest decodeRequest;
			bool decode = false;
			bool skip = false;
			bool compact = false;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.wait(lock, [&]() {
					return canceled() || resetRequested_ || !writes_.empty() ||
					       !decodeRequests_.empty() || compactRequested_;
				});
				if (canceled())
					break;
				if (resetRequested_) {
					needsLoad = true;
					continue;
				}
				if (!writes_.empty()) {
					const std::size_t byteCount = writes_.front().rgba ? writes_.front().rgba->size() : 0;
					write = std::move(writes_.front());
					writes_.pop_front();
					queuedWriteBytes_ = byteCount <= queuedWriteBytes_ ? queuedWriteBytes_ - byteCount : 0;
				}
				else if (!decodeRequests_.empty()) {
					decodeRequest = std::move(decodeRequests_.front());
					decodeRequests_.pop_front();
					const auto latest = requestedDecodeGeneration_.find(decodeRequest.cacheKey);
					if (latest != requestedDecodeGeneration_.end() && latest->second == decodeRequest.generation) {
						requestedDecodeGeneration_.erase(latest);
						decode = true;
					}
					else {
						skip = true;
					}
				}
				else {
					compact = compactRequested_;
					compactRequested_ = false;
				}
			}
			if (skip)
				continue;
			const bool ok = compact ? compactArchive() :
			                decode ? decodeEntry(decodeRequest) : appendPreview(std::move(write));
			if (!ok && !canceled()) {
				std::lock_guard<std::mutex> lock(mutex_);
				if (resetRequested_) {
					setState(DatabaseState::LOADING);
					needsLoad = true;
				}
				else {
					fatalError_.store(true, std::memory_order_relaxed);
					errorCode_.store(compact ? 3 : decode ? 9 : 2, std::memory_order_relaxed);
					setState(DatabaseState::ERROR);
				}
			}
			else if (!canceled()) {
				std::lock_guard<std::mutex> lock(mutex_);
				if (!resetRequested_ && !compact && !decode && writes_.empty() &&
				    decodeRequests_.empty() && !compactRequested_ && shouldCompactArchive()) {
					compactRequested_ = true;
					condition_.notify_one();
				}
				if (!resetRequested_ && writes_.empty() && decodeRequests_.empty() && !compactRequested_)
					setState(entries_.empty() ? DatabaseState::EMPTY : DatabaseState::READY);
			}
		}
		catch (...) {
			if (!canceled()) {
				std::lock_guard<std::mutex> lock(mutex_);
				if (resetRequested_) {
					setState(DatabaseState::LOADING);
					needsLoad = true;
				}
				else {
					fatalError_.store(true, std::memory_order_relaxed);
					errorCode_.store(5, std::memory_order_relaxed);
					setState(DatabaseState::ERROR);
					needsLoad = false;
				}
			}
		}
	}
}

bool DeepcacheArchiveWorker::resetArchive() {
	const std::string paths[] = {
		packPath_, packPath_ + ".bak", packPath_ + ".tmp",
		indexPath_, indexPath_ + ".bak", indexPath_ + ".tmp", indexPath_ + ".compact",
		indexPath_ + ".compact.tmp",
		directory_ + "/compaction-v1.pending"
	};
	for (const std::string& path : paths) {
		errno = 0;
		if (std::remove(path.c_str()) != 0 && errno != ENOENT)
			return false;
	}
	entries_.clear();
	readyKeys_.clear();
	readyPlugins_.clear();
	pluginReadyCounts_.clear();
	packedBytes_.clear();
	volatileEntries_.clear();
	packBytes_.store(0, std::memory_order_relaxed);
	volatileBytes_.store(0, std::memory_order_relaxed);
	readyCount_.store(0, std::memory_order_relaxed);
	readyPluginCount_.store(0, std::memory_order_relaxed);
	{
		std::lock_guard<std::mutex> lock(mutex_);
		decoded_.clear();
		decodedBytes_ = 0;
		decodeRequests_.clear();
		requestedDecodeGeneration_.clear();
		committed_.clear();
	}
	setState(DatabaseState::EMPTY);
	return true;
}

bool DeepcacheArchiveWorker::resetPending() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return resetRequested_;
}

bool DeepcacheArchiveWorker::shouldCompactArchive() const {
	std::uint64_t liveBytes = 0;
	for (const auto& entry : entries_)
		liveBytes += entry.second.length;
	const std::uint64_t packBytes = packBytes_.load(std::memory_order_relaxed);
	const std::uint64_t deadBytes = packBytes > liveBytes ? packBytes - liveBytes : 0;
	return deadBytes >= 64ull * 1024ull * 1024ull && deadBytes >= packBytes / 4ull;
}

bool DeepcacheArchiveWorker::acquireLease() {
	const std::string leasePath = directory_ + "/archive-v1.lock";
#ifdef _WIN32
	const int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, leasePath.c_str(), -1, nullptr, 0);
	if (wideLength <= 0) {
		errorCode_.store(6, std::memory_order_relaxed);
		return false;
	}
	std::vector<wchar_t> widePath(static_cast<std::size_t>(wideLength));
	if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, leasePath.c_str(), -1, widePath.data(), wideLength)) {
		errorCode_.store(6, std::memory_order_relaxed);
		return false;
	}
	HANDLE handle = CreateFileW(widePath.data(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS,
	                            FILE_ATTRIBUTE_NORMAL, nullptr);
	if (handle == INVALID_HANDLE_VALUE) {
		const DWORD error = GetLastError();
		if (error != ERROR_SHARING_VIOLATION && error != ERROR_LOCK_VIOLATION)
			errorCode_.store(6, std::memory_order_relaxed);
		return false;
	}
	leaseHandle_ = reinterpret_cast<std::uintptr_t>(handle);
#else
	const int descriptor = ::open(leasePath.c_str(), O_CREAT | O_RDWR, 0600);
	if (descriptor < 0) {
		errorCode_.store(6, std::memory_order_relaxed);
		return false;
	}
	if (::flock(descriptor, LOCK_EX | LOCK_NB) != 0) {
		const int lockError = errno;
		::close(descriptor);
		if (lockError != EWOULDBLOCK && lockError != EAGAIN)
			errorCode_.store(6, std::memory_order_relaxed);
		return false;
	}
	leaseHandle_ = static_cast<std::uintptr_t>(descriptor + 1);
#endif
	return true;
}

void DeepcacheArchiveWorker::releaseLease() {
	if (leaseHandle_ == 0)
		return;
#ifdef _WIN32
	CloseHandle(reinterpret_cast<HANDLE>(leaseHandle_));
#else
	const int descriptor = static_cast<int>(leaseHandle_ - 1);
	::flock(descriptor, LOCK_UN);
	::close(descriptor);
#endif
	leaseHandle_ = 0;
}

bool DeepcacheArchiveWorker::readPackFile(const std::string& path, std::vector<std::uint8_t>& bytes) {
	std::ifstream input(path.c_str(), std::ios::binary | std::ios::ate);
	if (!input)
		return false;
	const std::streamoff fileSize = input.tellg();
	if (fileSize < 0 || static_cast<std::uint64_t>(fileSize) > std::numeric_limits<std::size_t>::max())
		return false;
	if (resetPending())
		return false;
	std::vector<std::uint8_t> loaded(static_cast<std::size_t>(fileSize));
	input.seekg(0);
	std::size_t offset = 0;
	while (offset < loaded.size()) {
		if (canceled() || resetPending())
			return false;
		const std::size_t count = std::min(kReadChunkBytes, loaded.size() - offset);
		if (!input.read(reinterpret_cast<char*>(loaded.data() + offset), static_cast<std::streamsize>(count)))
			return false;
		offset += count;
	}
	bytes.swap(loaded);
	return true;
}

bool DeepcacheArchiveWorker::loadIndex(const std::string& path) {
	std::ifstream stream(path.c_str(), std::ios::binary);
	if (!stream)
		return false;
	char magic[8] = {};
	std::uint32_t version = 0;
	std::uint32_t count = 0;
	if (!stream.read(magic, sizeof(magic)) || std::memcmp(magic, kIndexMagic, sizeof(magic)) != 0 ||
	    !readValue(stream, version) || version != kIndexVersion || !readValue(stream, count) || count > kMaxIndexEntries)
		return false;
	std::unordered_map<std::string, Entry> loaded;
	for (std::uint32_t i = 0; i < count; ++i) {
		std::string key;
		Entry entry;
		if (!readString(stream, key, kMaxCacheKeyBytes) ||
		    !readString(stream, entry.fingerprint, kMaxFingerprintBytes) ||
		    !readValue(stream, entry.offset) || !readValue(stream, entry.length) ||
		    !readValue(stream, entry.checksum) || !readValue(stream, entry.width) ||
		    !readValue(stream, entry.height))
			return false;
		loaded[key] = std::move(entry);
	}
	entries_.swap(loaded);
	return true;
}

bool DeepcacheArchiveWorker::pushDecoded(DecodedPreview preview) {
	const std::size_t byteCount = preview.rgba.size();
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [&]() {
		return canceled() || resetRequested_ ||
		       (decoded_.size() < kMaxDecodedQueueEntries &&
		        (decoded_.empty() || decodedBytes_ + byteCount <= kMaxDecodedQueueBytes));
	});
	if (canceled() || resetRequested_)
		return false;
	decodedBytes_ += byteCount;
	decoded_.push_back(std::move(preview));
	return true;
}

bool DeepcacheArchiveWorker::storeVolatilePreview(PreviewWrite write) {
	if (canceled())
		return true;
	const auto wanted = wanted_.find(write.cacheKey);
	const std::uint64_t byteCount = write.width > 0 && write.height > 0
		? static_cast<std::uint64_t>(write.width) * static_cast<std::uint64_t>(write.height) * 4ull
		: 0;
	if (wanted == wanted_.end() || write.width <= 0 || write.height <= 0 ||
	    write.width > 8192 || write.height > 8192 || byteCount > 128ull * 1024ull * 1024ull ||
	    !write.rgba || byteCount != write.rgba->size())
		return false;
	std::vector<std::uint8_t> encoded;
	if (!encodeQoiCancelable(*write.rgba, write.width, write.height, stopping_, encoded))
		return canceled();
	if (encoded.empty() || canceled())
		return canceled();
	wanted->second = write.fingerprint;
	std::uint64_t totalBytes = volatileBytes_.load(std::memory_order_relaxed);
	const auto previous = volatileEntries_.find(write.cacheKey);
	if (previous != volatileEntries_.end() && previous->second.qoi.size() <= totalBytes)
		totalBytes -= previous->second.qoi.size();
	VolatileEntry entry;
	entry.fingerprint = std::move(write.fingerprint);
	entry.width = write.width;
	entry.height = write.height;
	entry.qoi = std::move(encoded);
	totalBytes += entry.qoi.size();
	volatileEntries_[write.cacheKey] = std::move(entry);
	volatileBytes_.store(totalBytes, std::memory_order_relaxed);
	{
		std::lock_guard<std::mutex> lock(mutex_);
		committed_.push_back(write.cacheKey);
	}
	return true;
}

bool DeepcacheArchiveWorker::decodeEntry(const DecodeRequest& request) {
	DecodedPreview preview;
	preview.cacheKey = request.cacheKey;
	preview.decodeGeneration = request.generation;
	const auto wanted = wanted_.find(request.cacheKey);
	if (wanted != wanted_.end())
		preview.fingerprint = wanted->second;
	const auto volatileFound = volatileEntries_.find(request.cacheKey);
	if (wanted != wanted_.end() && volatileFound != volatileEntries_.end() &&
	    volatileFound->second.fingerprint == wanted->second) {
		const VolatileEntry& entry = volatileFound->second;
		if (!decodeQoi(entry.qoi.data(), entry.qoi.size(), preview) ||
		    preview.width != entry.width || preview.height != entry.height) {
			preview.width = 0;
			preview.height = 0;
			preview.rgba.clear();
		}
		return pushDecoded(std::move(preview));
	}
	const auto found = entries_.find(request.cacheKey);
	if (wanted == wanted_.end() || found == entries_.end())
		return pushDecoded(std::move(preview));
	const Entry& entry = found->second;
	if (entry.fingerprint != wanted->second || entry.offset > packedBytes_.size() ||
	    entry.length > packedBytes_.size() - entry.offset)
		return pushDecoded(std::move(preview));
	const std::uint8_t* payload = packedBytes_.data() + entry.offset;
	if (deepcacheChecksum(payload, static_cast<std::size_t>(entry.length)) != entry.checksum ||
	    !decodeQoi(payload, static_cast<std::size_t>(entry.length), preview) ||
	    preview.width != static_cast<int>(entry.width) || preview.height != static_cast<int>(entry.height)) {
		preview.width = 0;
		preview.height = 0;
		preview.rgba.clear();
	}
	return pushDecoded(std::move(preview));
}

bool DeepcacheArchiveWorker::loadArchive(bool allowRecovery) {
	const std::string compactMarker = directory_ + "/compaction-v1.pending";
	std::ifstream marker(compactMarker.c_str(), std::ios::binary);
	if (marker) {
		marker.close();
		if (!allowRecovery)
			return false;
		const std::string packBackup = packPath_ + ".bak";
		const std::string indexBackup = indexPath_ + ".bak";
		std::ifstream oldPack(packBackup.c_str(), std::ios::binary);
		if (oldPack) {
			oldPack.close();
			std::remove(packPath_.c_str());
			if (std::rename(packBackup.c_str(), packPath_.c_str()) != 0)
				return false;
		}
		std::ifstream oldIndex(indexBackup.c_str(), std::ios::binary);
		if (oldIndex) {
			oldIndex.close();
			std::remove(indexPath_.c_str());
			if (std::rename(indexBackup.c_str(), indexPath_.c_str()) != 0)
				return false;
		}
		if (std::remove(compactMarker.c_str()) != 0)
			return false;
		std::remove((packPath_ + ".tmp").c_str());
		std::remove((indexPath_ + ".compact").c_str());
	}
	std::ifstream packExists(packPath_.c_str(), std::ios::binary);
	if (!packExists) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (resetRequested_)
			return false;
		setState(DatabaseState::EMPTY);
		return true;
	}
	packExists.close();
	if (!readPackFile(packPath_, packedBytes_))
		return false;
	packBytes_.store(packedBytes_.size(), std::memory_order_relaxed);
	if (!loadIndex(indexPath_) && !loadIndex(indexPath_ + ".bak")) {
		entries_.clear();
		std::lock_guard<std::mutex> lock(mutex_);
		if (resetRequested_)
			return false;
		setState(DatabaseState::EMPTY);
		return true;
	}
	for (auto it = entries_.begin(); it != entries_.end();) {
		if (wanted_.count(it->first) == 0)
			it = entries_.erase(it);
		else
			++it;
	}

	std::vector<std::string> invalidEntries;
	for (const auto& wanted : wanted_) {
		if (canceled() || resetPending())
			return false;
		const auto found = entries_.find(wanted.first);
		if (found == entries_.end())
			continue;
		if (found->second.fingerprint != wanted.second) {
			invalidEntries.push_back(wanted.first);
			continue;
		}
		const Entry& entry = found->second;
		if (entry.offset > packedBytes_.size() || entry.length > packedBytes_.size() - entry.offset) {
			invalidEntries.push_back(wanted.first);
			continue;
		}
		const std::uint8_t* payload = packedBytes_.data() + entry.offset;
		if (deepcacheChecksum(payload, static_cast<std::size_t>(entry.length)) != entry.checksum) {
			invalidEntries.push_back(wanted.first);
			continue;
		}
		DecodedPreview preview;
		preview.cacheKey = wanted.first;
		preview.fingerprint = wanted.second;
		if (!decodeQoi(payload, static_cast<std::size_t>(entry.length), preview) ||
		    preview.width != static_cast<int>(entry.width) || preview.height != static_cast<int>(entry.height)) {
			invalidEntries.push_back(wanted.first);
			continue;
		}
		if (!pushDecoded(std::move(preview)))
			return false;
		markReady(wanted.first);
	}
	for (const std::string& cacheKey : invalidEntries)
		entries_.erase(cacheKey);
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (resetRequested_)
			return false;
		setState(entries_.empty() ? DatabaseState::EMPTY : DatabaseState::READY);
	}
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
	auto wanted = wanted_.find(write.cacheKey);
	if (wanted == wanted_.end())
		return true;
	// Live panel-theme changes update the fingerprint for a known key. Writes
	// are serialized here, so the newest completed render is authoritative.
	wanted->second = write.fingerprint;
	const std::uint64_t byteCount = write.width > 0 && write.height > 0
		? static_cast<std::uint64_t>(write.width) * static_cast<std::uint64_t>(write.height) * 4ull
		: 0;
	if (write.width <= 0 || write.height <= 0 || write.width > 8192 || write.height > 8192 ||
	    byteCount > 128ull * 1024ull * 1024ull || !write.rgba || byteCount != write.rgba->size())
		return false;
	setState(DatabaseState::UPDATING);
	std::vector<std::uint8_t> encoded;
	if (!encodeQoiCancelable(*write.rgba, write.width, write.height, stopping_, encoded))
		return canceled();
	if (encoded.empty())
		return false;
	if (canceled())
		return true;
	std::ofstream pack(packPath_.c_str(), std::ios::binary | std::ios::app);
	if (!pack)
		return false;
	const std::uint64_t offset = packBytes_.load(std::memory_order_relaxed);
	if (!writeCancelable(pack, encoded.data(), encoded.size(), stopping_))
		return canceled();
	pack.flush();
	if (!pack)
		return false;
	Entry entry;
	entry.fingerprint = std::move(write.fingerprint);
	entry.offset = offset;
	entry.length = static_cast<std::uint64_t>(encoded.size());
	entry.checksum = deepcacheChecksum(encoded.data(), encoded.size());
	entry.width = static_cast<std::uint32_t>(write.width);
	entry.height = static_cast<std::uint32_t>(write.height);
	packedBytes_.insert(packedBytes_.end(), encoded.begin(), encoded.end());
	entries_[write.cacheKey] = std::move(entry);
	packBytes_.store(offset + static_cast<std::uint64_t>(encoded.size()), std::memory_order_relaxed);
	if (canceled())
		return true; // Appended bytes are harmless until the index commits.
	if (!saveIndexAtomically())
		return false;
	if (resetPending())
		return true;
	markReady(write.cacheKey);
	{
		std::lock_guard<std::mutex> lock(mutex_);
		committed_.push_back(write.cacheKey);
	}
	return true;
}

bool DeepcacheArchiveWorker::compactArchive() {
	if (canceled() || packedBytes_.empty())
		return true;
	setState(DatabaseState::COMPACTING);
	if (entries_.empty()) {
		std::ofstream pack(packPath_.c_str(), std::ios::binary | std::ios::trunc);
		if (!pack)
			return false;
		pack.flush();
		if (!pack)
			return false;
		pack.close();
		packedBytes_.clear();
		packBytes_.store(0, std::memory_order_relaxed);
		return saveIndexAtomically();
	}
	const std::string temporaryPack = packPath_ + ".tmp";
	std::ofstream output(temporaryPack.c_str(), std::ios::binary | std::ios::trunc);
	if (!output)
		return false;
	std::unordered_map<std::string, Entry> compacted;
	std::vector<std::uint8_t> compactedBytes;
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
		if (!writeCancelable(output, packedBytes_.data() + source.offset,
		                     static_cast<std::size_t>(source.length), stopping_)) {
			output.close();
			std::remove(temporaryPack.c_str());
			return canceled();
		}
		compactedBytes.insert(compactedBytes.end(), packedBytes_.begin() + source.offset,
		                      packedBytes_.begin() + source.offset + source.length);
		Entry destination = source;
		destination.offset = offset;
		offset += source.length;
		compacted[item.first] = std::move(destination);
	}
	output.flush();
	if (!output) {
		output.close();
		std::remove(temporaryPack.c_str());
		return false;
	}
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
		if (!marker) {
			entries_ = std::move(previous);
			std::remove(temporaryPack.c_str());
			std::remove(stagedIndex.c_str());
			return false;
		}
		marker << "pending";
		marker.flush();
		if (!marker) {
			marker.close();
			entries_ = std::move(previous);
			std::remove(markerPath.c_str());
			std::remove(temporaryPack.c_str());
			std::remove(stagedIndex.c_str());
			return false;
		}
	}
	const std::string packBackup = packPath_ + ".bak";
	const std::string indexBackup = indexPath_ + ".bak";
	std::remove(packBackup.c_str());
	std::remove(indexBackup.c_str());
	bool packBackedUp = false;
	bool packInstalled = false;
	bool indexBackedUp = false;
	bool indexInstalled = false;
	packBackedUp = std::rename(packPath_.c_str(), packBackup.c_str()) == 0;
	if (packBackedUp)
		packInstalled = std::rename(temporaryPack.c_str(), packPath_.c_str()) == 0;
	if (packInstalled)
		indexBackedUp = std::rename(indexPath_.c_str(), indexBackup.c_str()) == 0;
	if (indexBackedUp)
		indexInstalled = std::rename(stagedIndex.c_str(), indexPath_.c_str()) == 0;
	if (!indexInstalled) {
		if (packBackedUp) {
			if (packInstalled)
				std::remove(packPath_.c_str());
			std::rename(packBackup.c_str(), packPath_.c_str());
		}
		if (indexBackedUp) {
			std::rename(indexBackup.c_str(), indexPath_.c_str());
		}
		// Keep the marker until a fully successful commit. If either best-effort
		// restore above failed, the next launch can finish recovery from whatever
		// backup still exists.
		entries_ = std::move(previous);
		return false;
	}
	std::remove(markerPath.c_str());
	std::remove(packBackup.c_str());
	std::remove(indexBackup.c_str());
	packedBytes_.swap(compactedBytes);
	packBytes_.store(packedBytes_.size(), std::memory_order_relaxed);
	return true;
}

}  // namespace deepcache
