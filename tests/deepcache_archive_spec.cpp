#include "../src/DeepcacheArchive.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace {

int processId() {
#ifdef _WIN32
	return _getpid();
#else
	return static_cast<int>(getpid());
#endif
}

void makeDirectory(const std::string& path) {
#ifdef _WIN32
	_mkdir(path.c_str());
#else
	mkdir(path.c_str(), 0700);
#endif
}

void removeDirectory(const std::string& path) {
	const char* files[] = {"previews-v1.pack", "previews-v1.pack.bak", "previews-v1.pack.tmp",
	                       "index-v1.bin", "index-v1.bin.bak", "index-v1.bin.tmp",
	                       "index-v1.bin.compact", "compaction-v1.pending"};
	for (const char* file : files)
		std::remove((path + "/" + file).c_str());
#ifdef _WIN32
	_rmdir(path.c_str());
#else
	rmdir(path.c_str());
#endif
}

std::vector<std::uint8_t> pixels(int width, int height, int salt) {
	std::vector<std::uint8_t> result(static_cast<std::size_t>(width) * height * 4u);
	for (std::size_t i = 0; i < result.size(); i += 4) {
		result[i + 0] = static_cast<std::uint8_t>((i + salt * 17) & 0xff);
		result[i + 1] = static_cast<std::uint8_t>((i * 3 + salt * 29) & 0xff);
		result[i + 2] = static_cast<std::uint8_t>((i * 7 + salt * 11) & 0xff);
		result[i + 3] = 255;
	}
	return result;
}

template <typename Predicate>
bool waitUntil(Predicate predicate) {
	for (int i = 0; i < 500; ++i) {
		if (predicate())
			return true;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	return false;
}

}  // namespace

int main() {
	const std::string directory = "/tmp/leviathan-deepcache-archive-" + std::to_string(processId());
	removeDirectory(directory);
	makeDirectory(directory);

	const std::vector<std::uint8_t> firstPixels = pixels(13, 9, 1);
	const std::vector<std::uint8_t> secondPixels = pixels(7, 11, 2);
	const std::vector<std::uint8_t> updatedPixels = pixels(13, 9, 3);
	std::uint64_t sizeBeforeUpdate = 0;
	std::uint64_t sizeAfterUpdate = 0;
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}, {"two", "fp-two", "plugin-a"}});
		deepcache::PreviewWrite first;
		first.cacheKey = "one";
		first.fingerprint = "fp-one";
		first.width = 13;
		first.height = 9;
		first.rgba = firstPixels;
		worker.enqueue(std::move(first));
		deepcache::PreviewWrite second;
		second.cacheKey = "two";
		second.fingerprint = "fp-two";
		second.width = 7;
		second.height = 11;
		second.rgba = secondPixels;
		worker.enqueue(std::move(second));
		if (!waitUntil([&]() { return worker.readyCount() == 2; }) ||
		    worker.targetPluginCount() != 1 || worker.readyPluginCount() != 1) {
			std::cerr << "[FAIL] initial append did not commit\n";
			return 1;
		}
		sizeBeforeUpdate = worker.packBytes();
		deepcache::PreviewWrite updated;
		updated.cacheKey = "one";
		updated.fingerprint = "fp-one";
		updated.width = 13;
		updated.height = 9;
		updated.rgba = updatedPixels;
		worker.enqueue(std::move(updated));
		if (!waitUntil([&]() { return worker.packBytes() > sizeBeforeUpdate; })) {
			std::cerr << "[FAIL] incremental replacement was not appended\n";
			return 1;
		}
		sizeAfterUpdate = worker.packBytes();
		worker.shutdown();
	}

	bool decodedLatest = false;
	bool staleRejected = true;
	std::uint64_t compactedSize = sizeAfterUpdate;
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}, {"two", "stale-fingerprint", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		while (worker.tryPopDecoded(preview)) {
			if (preview.cacheKey == "one")
				decodedLatest = preview.rgba == updatedPixels;
			if (preview.cacheKey == "two")
				staleRejected = false;
		}
		staleRejected = staleRejected && worker.readyPluginCount() == 0;
		worker.requestCompaction();
		if (!waitUntil([&]() { return worker.packBytes() < sizeAfterUpdate; })) {
			std::cerr << "[FAIL] compaction did not reclaim the superseded payload\n";
			return 1;
		}
		compactedSize = worker.packBytes();
		worker.shutdown();
	}

	// A shutdown immediately after requesting maintenance must cancel/join
	// safely while leaving the authoritative pair readable.
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		worker.requestCompaction();
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] cache was unreadable after canceled shutdown\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool readable = worker.tryPopDecoded(preview) && preview.rgba == updatedPixels;
		worker.shutdown();
		if (!readable) {
			std::cerr << "[FAIL] latest preview was not preserved after cancellation\n";
			return 1;
		}
	}

	removeDirectory(directory);
	const bool pass = decodedLatest && staleRejected && compactedSize < sizeAfterUpdate;
	std::cout << (pass ? "[PASS]" : "[FAIL]")
	          << " append-only QOI archive reloads, invalidates, compacts, and cancels safely"
	          << " :: append=" << sizeAfterUpdate << " compact=" << compactedSize << "\n";
	return pass ? 0 : 1;
}
