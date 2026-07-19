#include "../src/DeepcacheArchive.hpp"

#include <chrono>
#include <cstdio>
#include <fstream>
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
	                       "index-v1.bin.compact", "compaction-v1.pending", "archive-v1.lock"};
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
		result[i + 3] = static_cast<std::uint8_t>(((i / 4u) * 13u + salt * 31u) & 0xff);
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

bool corruptFirstByte(const std::string& path) {
	std::fstream stream(path.c_str(), std::ios::binary | std::ios::in | std::ios::out);
	if (!stream)
		return false;
	char byte = 0;
	if (!stream.read(&byte, 1))
		return false;
	byte ^= 0x5a;
	stream.seekp(0);
	stream.write(&byte, 1);
	stream.flush();
	return static_cast<bool>(stream);
}

bool copyFile(const std::string& source, const std::string& destination) {
	std::ifstream input(source.c_str(), std::ios::binary);
	std::ofstream output(destination.c_str(), std::ios::binary | std::ios::trunc);
	output << input.rdbuf();
	output.flush();
	return static_cast<bool>(input) && static_cast<bool>(output);
}

bool writePendingMarker(const std::string& directory) {
	std::ofstream marker((directory + "/compaction-v1.pending").c_str(),
	                     std::ios::binary | std::ios::trunc);
	marker << "pending";
	marker.flush();
	return static_cast<bool>(marker);
}

bool writeOversizedKeyIndex(const std::string& path) {
	std::ofstream stream(path.c_str(), std::ios::binary | std::ios::trunc);
	const char magic[8] = {'L', 'V', 'D', 'C', 'I', 'D', 'X', '1'};
	const std::uint32_t version = 1;
	const std::uint32_t count = 1;
	const std::uint32_t oversizedKeyLength = 4097;
	stream.write(magic, sizeof(magic));
	stream.write(reinterpret_cast<const char*>(&version), sizeof(version));
	stream.write(reinterpret_cast<const char*>(&count), sizeof(count));
	stream.write(reinterpret_cast<const char*>(&oversizedKeyLength), sizeof(oversizedKeyLength));
	stream.flush();
	return static_cast<bool>(stream);
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
		first.rgba = std::make_shared<const std::vector<std::uint8_t>>(firstPixels);
		worker.enqueue(std::move(first));
		deepcache::PreviewWrite second;
		second.cacheKey = "two";
		second.fingerprint = "fp-two";
		second.width = 7;
		second.height = 11;
		second.rgba = std::make_shared<const std::vector<std::uint8_t>>(secondPixels);
		worker.enqueue(std::move(second));
		if (!waitUntil([&]() { return worker.readyCount() == 2; }) ||
		    worker.targetPluginCount() != 1 || worker.readyPluginCount() != 1) {
			std::cerr << "[FAIL] initial append did not commit\n";
			return 1;
		}
		deepcache::DeepcacheArchiveWorker contender;
		contender.start(directory, {{"one", "fp-one", "plugin-a"}});
		auto contenderPixels = std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
		deepcache::PreviewWrite raced;
		raced.cacheKey = "one";
		raced.fingerprint = "fp-one";
		raced.width = 13;
		raced.height = 9;
		raced.rgba = contenderPixels;
		contender.enqueue(std::move(raced));
		if (!waitUntil([&]() { return contender.state() == deepcache::DatabaseState::READ_ONLY; })) {
			std::cerr << "[FAIL] second archive worker did not enter read-only state\n";
			return 1;
		}
		deepcache::DecodedPreview contenderPreview;
		if (!contender.tryPopDecoded(contenderPreview) || contenderPreview.cacheKey != "one" ||
		    contenderPreview.rgba != firstPixels || contender.readyCount() != 1 ||
		    contender.readyPluginCount() != 1) {
			std::cerr << "[FAIL] read-only archive worker did not consume the committed snapshot\n";
			return 1;
		}
		if (!waitUntil([&]() { return contenderPixels.use_count() == 1; })) {
			std::cerr << "[FAIL] read-only archive worker retained a raced queued write\n";
			return 1;
		}
		deepcache::PreviewWrite denied;
		denied.cacheKey = "one";
		denied.fingerprint = "fp-one";
		denied.width = 13;
		denied.height = 9;
		denied.rgba = contenderPixels;
		if (contender.enqueue(std::move(denied))) {
			std::cerr << "[FAIL] read-only archive worker accepted a disk write\n";
			return 1;
		}
		contender.shutdown();
		sizeBeforeUpdate = worker.packBytes();
		deepcache::PreviewWrite updated;
		updated.cacheKey = "one";
		updated.fingerprint = "fp-one";
		updated.width = 13;
		updated.height = 9;
		updated.rgba = std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
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

	// A damaged payload must be treated as a cache miss, then replaced by a
	// normal append without exposing corrupt pixels or poisoning the archive.
	if (!corruptFirstByte(directory + "/previews-v1.pack")) {
		std::cerr << "[FAIL] could not prepare corrupt pack test\n";
		return 1;
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] corrupt pack reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		if (worker.tryPopDecoded(preview) || worker.readyCount() != 0 ||
		    worker.state() == deepcache::DatabaseState::ERROR) {
			std::cerr << "[FAIL] corrupt pack payload was not isolated as a cache miss\n";
			return 1;
		}
		deepcache::PreviewWrite repair;
		repair.cacheKey = "one";
		repair.fingerprint = "fp-one";
		repair.width = 13;
		repair.height = 9;
		repair.rgba = std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
		worker.enqueue(std::move(repair));
		if (!waitUntil([&]() { return worker.readyCount() == 1; })) {
			std::cerr << "[FAIL] corrupt pack payload was not repairable\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] repaired pack reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool repaired = worker.tryPopDecoded(preview) && preview.rgba == updatedPixels;
		worker.shutdown();
		if (!repaired) {
			std::cerr << "[FAIL] repaired pack did not persist readable pixels\n";
			return 1;
		}
	}

	// A damaged index must likewise discard metadata, rebuild from a normal
	// write, and produce a valid index for the next process launch.
	if (!corruptFirstByte(directory + "/index-v1.bin")) {
		std::cerr << "[FAIL] could not prepare corrupt index test\n";
		return 1;
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] corrupt index reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		if (worker.tryPopDecoded(preview) || worker.readyCount() != 0 ||
		    worker.state() == deepcache::DatabaseState::ERROR) {
			std::cerr << "[FAIL] corrupt index was not isolated as an empty cache\n";
			return 1;
		}
		deepcache::PreviewWrite repair;
		repair.cacheKey = "one";
		repair.fingerprint = "fp-one";
		repair.width = 13;
		repair.height = 9;
		repair.rgba = std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
		worker.enqueue(std::move(repair));
		if (!waitUntil([&]() { return worker.readyCount() == 1; })) {
			std::cerr << "[FAIL] corrupt index was not repairable\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] repaired index reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool repaired = worker.tryPopDecoded(preview) && preview.rgba == updatedPixels;
		worker.shutdown();
		if (!repaired) {
			std::cerr << "[FAIL] repaired index did not persist readable pixels\n";
			return 1;
		}
	}

	// If a crash occurs after the old pack is backed up but before the old
	// index is backed up, the pack backup and still-current index are a matching
	// pair. Recovery must restore that pair rather than discard the cache.
	if (!copyFile(directory + "/previews-v1.pack", directory + "/previews-v1.pack.bak") ||
	    !corruptFirstByte(directory + "/previews-v1.pack") || !writePendingMarker(directory)) {
		std::cerr << "[FAIL] could not prepare interrupted compaction recovery test\n";
		return 1;
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] interrupted compaction recovery did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool recovered = worker.tryPopDecoded(preview) && preview.rgba == updatedPixels;
		worker.shutdown();
		if (!recovered) {
			std::cerr << "[FAIL] pack-only backup did not recover with the current index\n";
			return 1;
		}
	}

	// Index strings are bounded before allocation. An oversized declared key is
	// isolated as an empty cache instead of allocating attacker-controlled data.
	if (!writeOversizedKeyIndex(directory + "/index-v1.bin")) {
		std::cerr << "[FAIL] could not prepare oversized index-key test\n";
		return 1;
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(directory, {{"one", "fp-one", "plugin-a"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] oversized index-key reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool rejected = !worker.tryPopDecoded(preview) && worker.readyCount() == 0 &&
		                      worker.state() != deepcache::DatabaseState::ERROR;
		worker.shutdown();
		if (!rejected) {
			std::cerr << "[FAIL] oversized index key was not isolated as an empty cache\n";
			return 1;
		}
	}

	// A live panel-theme rebuild changes the fingerprint without changing the
	// model cache key. The replacement must persist under the new fingerprint.
	const std::string themeDirectory = directory + "-theme";
	removeDirectory(themeDirectory);
	makeDirectory(themeDirectory);
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(themeDirectory, {{"theme-model", "light-fingerprint", "plugin-theme"}});
		deepcache::PreviewWrite themed;
		themed.cacheKey = "theme-model";
		themed.fingerprint = "dark-fingerprint";
		themed.width = 13;
		themed.height = 9;
		themed.rgba = std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
		worker.enqueue(std::move(themed));
		if (!waitUntil([&]() { return worker.readyCount() == 1; })) {
			std::cerr << "[FAIL] live theme fingerprint replacement did not commit\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(themeDirectory, {{"theme-model", "dark-fingerprint", "plugin-theme"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] live theme fingerprint replacement did not reload\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool persisted = worker.tryPopDecoded(preview) && preview.rgba == updatedPixels;
		worker.shutdown();
		if (!persisted) {
			std::cerr << "[FAIL] rebuilt theme preview was not persisted under its new fingerprint\n";
			return 1;
		}
	}
	removeDirectory(themeDirectory);

	removeDirectory(directory);
	const bool pass = decodedLatest && staleRejected && compactedSize < sizeAfterUpdate;
	std::cout << (pass ? "[PASS]" : "[FAIL]")
	          << " append-only QOI archive reloads, invalidates, compacts, cancels, and repairs corruption"
	          << " :: append=" << sizeAfterUpdate << " compact=" << compactedSize << "\n";
	return pass ? 0 : 1;
}
