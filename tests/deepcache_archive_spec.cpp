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
	                       "index-v1.bin.compact", "index-v1.bin.compact.tmp",
	                       "compaction-v1.pending", "archive-v1.lock"};
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

bool writeSingleEntryIndex(const std::string& path,
	                       const std::string& key,
	                       const std::string& fingerprint) {
	std::ofstream stream(path.c_str(), std::ios::binary | std::ios::trunc);
	const char magic[8] = {'L', 'V', 'D', 'C', 'I', 'D', 'X', '1'};
	const std::uint32_t version = 1;
	const std::uint32_t count = 1;
	const std::uint32_t keyLength = static_cast<std::uint32_t>(key.size());
	const std::uint32_t fingerprintLength =
		static_cast<std::uint32_t>(fingerprint.size());
	const std::uint64_t offset = 0;
	const std::uint64_t length = 22;
	const std::uint64_t checksum = 0;
	const std::uint32_t width = 1;
	const std::uint32_t height = 1;
	stream.write(magic, sizeof(magic));
	stream.write(reinterpret_cast<const char*>(&version), sizeof(version));
	stream.write(reinterpret_cast<const char*>(&count), sizeof(count));
	stream.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
	stream.write(key.data(), key.size());
	stream.write(reinterpret_cast<const char*>(&fingerprintLength),
	             sizeof(fingerprintLength));
	stream.write(fingerprint.data(), fingerprint.size());
	stream.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
	stream.write(reinterpret_cast<const char*>(&length), sizeof(length));
	stream.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
	stream.write(reinterpret_cast<const char*>(&width), sizeof(width));
	stream.write(reinterpret_cast<const char*>(&height), sizeof(height));
	stream.flush();
	return static_cast<bool>(stream);
}

bool createSizedFile(const std::string& path, std::uint64_t size) {
	if (size == 0)
		return false;
	std::ofstream stream(path.c_str(), std::ios::binary | std::ios::trunc);
	stream.seekp(static_cast<std::streamoff>(size - 1));
	stream.put('\0');
	stream.flush();
	return static_cast<bool>(stream);
}

std::uint64_t fileSize(const std::string& path) {
	std::ifstream stream(path.c_str(), std::ios::binary | std::ios::ate);
	if (!stream)
		return 0;
	const std::streamoff size = static_cast<std::streamoff>(stream.tellg());
	return size >= 0 ? static_cast<std::uint64_t>(size) : 0;
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
		bool committedOne = false;
		bool committedTwo = false;
		std::string committedKey;
		if (!waitUntil([&]() {
			while (worker.tryPopCommitted(committedKey)) {
				committedOne = committedOne || committedKey == "one";
				committedTwo = committedTwo || committedKey == "two";
			}
			return committedOne && committedTwo;
		})) {
			std::cerr << "[FAIL] successful archive commits were not acknowledged\n";
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
		if (!contender.requestDecode("one", 77)) {
			std::cerr << "[FAIL] read-only archive worker rejected an in-memory re-decode\n";
			return 1;
		}
		deepcache::DecodedPreview restoredPreview;
		if (!waitUntil([&]() { return contender.tryPopDecoded(restoredPreview); }) ||
		    restoredPreview.cacheKey != "one" || restoredPreview.decodeGeneration != 77 ||
		    restoredPreview.rgba != firstPixels) {
			std::cerr << "[FAIL] hot QOI re-decode did not restore the requested generation\n";
			return 1;
		}
		if (!waitUntil([&]() { return contenderPixels.use_count() == 1; })) {
			std::cerr << "[FAIL] read-only archive worker retained a raced queued write\n";
			return 1;
		}
		const std::uint64_t contenderPackBytes = contender.packBytes();
		deepcache::PreviewWrite memoryOnly;
		memoryOnly.cacheKey = "one";
		memoryOnly.fingerprint = "fp-one";
		memoryOnly.width = 13;
		memoryOnly.height = 9;
		memoryOnly.rgba = contenderPixels;
		if (!contender.enqueue(std::move(memoryOnly))) {
			std::cerr << "[FAIL] read-only archive worker rejected a volatile QOI write\n";
			return 1;
		}
		std::string volatileCommit;
		if (!waitUntil([&]() { return contender.tryPopCommitted(volatileCommit); }) ||
		    volatileCommit != "one" || contender.packBytes() != contenderPackBytes ||
		    contender.hotCompressedBytes() == 0) {
			std::cerr << "[FAIL] read-only QOI write was not kept strictly in memory\n";
			return 1;
		}
		if (!contender.requestDecode("one", 78)) {
			std::cerr << "[FAIL] volatile QOI entry rejected a restore decode\n";
			return 1;
		}
		deepcache::DecodedPreview volatilePreview;
		if (!waitUntil([&]() { return contender.tryPopDecoded(volatilePreview); }) ||
		    volatilePreview.decodeGeneration != 78 || volatilePreview.rgba != updatedPixels) {
			std::cerr << "[FAIL] volatile QOI entry did not restore memory-only pixels\n";
			return 1;
		}
		if (contender.requestReset()) {
			std::cerr << "[FAIL] read-only archive worker accepted a database reset\n";
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
		if (!waitUntil([&]() { return worker.indexDiscoveryComplete(); })) {
			std::cerr << "[FAIL] index discovery barrier was not published\n";
			return 1;
		}
		deepcache::IndexedCandidate candidate;
		if (!worker.tryPopIndexedCandidate(candidate) || candidate.cacheKey != "one" ||
		    candidate.fingerprint != "fp-one" || worker.tryPopIndexedCandidate(candidate)) {
			std::cerr << "[FAIL] metadata discovery did not publish only matching candidates\n";
			return 1;
		}
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
		deepcache::IndexedCandidate corruptCandidate;
		if (!worker.tryPopIndexedCandidate(corruptCandidate) ||
		    corruptCandidate.cacheKey != "one") {
			std::cerr << "[FAIL] corrupt payload was not published as an indexed candidate\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		if (!worker.indexDiscoveryComplete() || worker.hasPendingIndexedCandidates() ||
		    worker.tryPopDecoded(preview) || worker.readyCount() != 0 ||
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

	// A cache backend that failed before startup must apply backpressure. Saying
	// it can accept writes would let the UI retain an entire library of RGBA
	// previews even though none can ever be committed.
	{
		deepcache::DeepcacheArchiveWorker unavailable;
		unavailable.markUnavailable(7);
		if (unavailable.canAcceptWrite()) {
			std::cerr << "[FAIL] unavailable archive accepted raster work\n";
			return 1;
		}
	}

	// Candidate-size admission allows one bounded preview above the aggregate
	// queue threshold when empty, but rejects anything above the single-preview
	// ceiling. The UI retains a bounded retry if admission later changes.
	const std::string admissionDirectory = directory + "-admission";
	removeDirectory(admissionDirectory);
	makeDirectory(admissionDirectory);
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(admissionDirectory, {{"admission-model", "fp-admission", "plugin-admission"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; }) ||
		    !worker.canAcceptWrite(1) ||
		    !worker.canAcceptWrite(64u * 1024u * 1024u + 1u) ||
		    worker.canAcceptWrite(128u * 1024u * 1024u + 1u)) {
			std::cerr << "[FAIL] archive candidate-size admission was inconsistent\n";
			return 1;
		}
		worker.shutdown();
	}
	removeDirectory(admissionDirectory);

	// A sparse pack larger than the former 512 MB ceiling must be handled through
	// its indexed span without allocating or reading the whole file. Its invalid
	// payload is discarded and the owning worker can accept a normal replacement.
	const std::string oversizedPackDirectory = directory + "-oversized-pack";
	removeDirectory(oversizedPackDirectory);
	makeDirectory(oversizedPackDirectory);
	if (!writeSingleEntryIndex(oversizedPackDirectory + "/index-v1.bin",
	                           "oversized-model", "fp-oversized") ||
	    !createSizedFile(oversizedPackDirectory + "/previews-v1.pack",
	                     513ull * 1024ull * 1024ull)) {
		std::cerr << "[FAIL] could not prepare oversized pack test\n";
		return 1;
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(oversizedPackDirectory,
		             {{"oversized-model", "fp-oversized", "plugin-oversized"}});
		if (!waitUntil([&]() {
			return worker.state() == deepcache::DatabaseState::EMPTY &&
			       worker.packBytes() == 0;
		})) {
			std::cerr << "[FAIL] oversized pack was not reset safely\n";
			return 1;
		}
		deepcache::PreviewWrite replacement;
		replacement.cacheKey = "oversized-model";
		replacement.fingerprint = "fp-oversized";
		replacement.width = 13;
		replacement.height = 9;
		replacement.rgba =
			std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
		if (!worker.enqueue(std::move(replacement)) ||
		    !waitUntil([&]() { return worker.readyCount() == 1; })) {
			std::cerr << "[FAIL] archive did not recover after oversized pack reset\n";
			return 1;
		}
		worker.shutdown();
	}
	removeDirectory(oversizedPackDirectory);

	// Extremely wide modules may exceed the former 8192-pixel dimension at
	// 200% cache resolution even though their total bitmap is modest.
	const std::string wideDirectory = directory + "-wide";
	removeDirectory(wideDirectory);
	makeDirectory(wideDirectory);
	const std::vector<std::uint8_t> widePixels = pixels(9000, 1, 41);
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(wideDirectory, {{"wide-model", "fp-wide", "plugin-wide"}});
		deepcache::PreviewWrite wide;
		wide.cacheKey = "wide-model";
		wide.fingerprint = "fp-wide";
		wide.width = 9000;
		wide.height = 1;
		wide.rgba =
			std::make_shared<const std::vector<std::uint8_t>>(widePixels);
		if (!worker.enqueue(std::move(wide)) ||
		    !waitUntil([&]() { return worker.readyCount() == 1; })) {
			std::cerr << "[FAIL] wide preview did not persist\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(wideDirectory, {{"wide-model", "fp-wide", "plugin-wide"}});
		if (!waitUntil([&]() {
			return worker.state() != deepcache::DatabaseState::LOADING;
		})) {
			std::cerr << "[FAIL] wide preview reload did not finish\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool restored = worker.tryPopDecoded(preview) &&
			preview.width == 9000 && preview.height == 1 &&
			preview.rgba == widePixels;
		worker.shutdown();
		if (!restored) {
			std::cerr << "[FAIL] wide preview did not round-trip\n";
			return 1;
		}
	}
	removeDirectory(wideDirectory);

	// Startup reads small append-only holes sequentially. A superseded middle
	// record must be skipped without disturbing either neighboring live QOI.
	const std::string fragmentedDirectory = directory + "-fragmented";
	removeDirectory(fragmentedDirectory);
	makeDirectory(fragmentedDirectory);
	const std::vector<std::uint8_t> gapPixelsA = pixels(13, 9, 51);
	const std::vector<std::uint8_t> gapPixelsB = pixels(13, 9, 52);
	const std::vector<std::uint8_t> gapPixelsB2 = pixels(13, 9, 53);
	const std::vector<std::uint8_t> gapPixelsC = pixels(13, 9, 54);
	const std::vector<deepcache::ArchiveWantedEntry> fragmentedWanted = {
		{"gap-a", "fp-a", "plugin-gap"},
		{"gap-b", "fp-b", "plugin-gap"},
		{"gap-c", "fp-c", "plugin-gap"}
	};
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(fragmentedDirectory, fragmentedWanted);
		const std::vector<std::pair<std::string, const std::vector<std::uint8_t>*>> initial = {
			{"gap-a", &gapPixelsA}, {"gap-b", &gapPixelsB}, {"gap-c", &gapPixelsC}
		};
		for (const auto& item : initial) {
			deepcache::PreviewWrite write;
			write.cacheKey = item.first;
			write.fingerprint = item.first == "gap-a" ? "fp-a" :
			                    item.first == "gap-b" ? "fp-b" : "fp-c";
			write.width = 13;
			write.height = 9;
			write.rgba =
				std::make_shared<const std::vector<std::uint8_t>>(*item.second);
			if (!waitUntil([&]() { return worker.enqueue(write); })) {
				std::cerr << "[FAIL] fragmented archive write queue stalled\n";
				return 1;
			}
		}
		if (!waitUntil([&]() { return worker.readyCount() == 3; })) {
			std::cerr << "[FAIL] fragmented archive was not populated\n";
			return 1;
		}
		std::string committedKey;
		while (worker.tryPopCommitted(committedKey)) {}
		deepcache::PreviewWrite replacement;
		replacement.cacheKey = "gap-b";
		replacement.fingerprint = "fp-b";
		replacement.width = 13;
		replacement.height = 9;
		replacement.rgba =
			std::make_shared<const std::vector<std::uint8_t>>(gapPixelsB2);
		if (!worker.enqueue(std::move(replacement)) ||
		    !waitUntil([&]() {
			    while (worker.tryPopCommitted(committedKey)) {
				    if (committedKey == "gap-b")
					    return true;
			    }
			    return false;
		    })) {
			std::cerr << "[FAIL] fragmented archive replacement was not appended\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(fragmentedDirectory, fragmentedWanted);
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] fragmented archive reload did not finish\n";
			return 1;
		}
		bool restoredA = false;
		bool restoredB = false;
		bool restoredC = false;
		deepcache::DecodedPreview preview;
		while (worker.tryPopDecoded(preview)) {
			restoredA = restoredA || (preview.cacheKey == "gap-a" && preview.rgba == gapPixelsA);
			restoredB = restoredB || (preview.cacheKey == "gap-b" && preview.rgba == gapPixelsB2);
			restoredC = restoredC || (preview.cacheKey == "gap-c" && preview.rgba == gapPixelsC);
		}
		worker.shutdown();
		if (!restoredA || !restoredB || !restoredC) {
			std::cerr << "[FAIL] sequential gap read disturbed a live preview\n";
			return 1;
		}
	}
	removeDirectory(fragmentedDirectory);

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

	// Startup evaluates the normal compaction threshold even when no new writes
	// arrive. With no live index entries, a fully dead pack is truncated safely.
	const std::string startupCompactDirectory = directory + "-startup-compact";
	removeDirectory(startupCompactDirectory);
	makeDirectory(startupCompactDirectory);
	const std::string deadPackPath = startupCompactDirectory + "/previews-v1.pack";
	if (!createSizedFile(deadPackPath, 65ull * 1024ull * 1024ull)) {
		std::cerr << "[FAIL] could not prepare startup compaction test\n";
		return 1;
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(startupCompactDirectory, {});
		const bool compactedAtStartup = waitUntil([&]() {
			return worker.state() == deepcache::DatabaseState::EMPTY && fileSize(deadPackPath) == 0;
		});
		worker.shutdown();
		if (!compactedAtStartup) {
			std::cerr << "[FAIL] dead archive was not compacted during startup\n";
			return 1;
		}
	}
	removeDirectory(startupCompactDirectory);

	// A user-requested rebuild resets the archive on its owning worker thread,
	// clears progress, and leaves the same worker ready to persist fresh data.
	const std::string resetDirectory = directory + "-reset";
	removeDirectory(resetDirectory);
	makeDirectory(resetDirectory);
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(resetDirectory, {{"reset-model", "fp-reset", "plugin-reset"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] reset archive did not finish initial load\n";
			return 1;
		}
		deepcache::PreviewWrite original;
		original.cacheKey = "reset-model";
		original.fingerprint = "fp-reset";
		original.width = 13;
		original.height = 9;
		original.rgba = std::make_shared<const std::vector<std::uint8_t>>(firstPixels);
		worker.enqueue(std::move(original));
		if (!waitUntil([&]() { return worker.readyCount() == 1; }) || !worker.requestReset()) {
			std::cerr << "[FAIL] archive reset request was not accepted\n";
			return 1;
		}
		if (!waitUntil([&]() {
			return worker.state() == deepcache::DatabaseState::EMPTY &&
			       worker.readyCount() == 0 && worker.readyPluginCount() == 0 && worker.packBytes() == 0;
		})) {
			std::cerr << "[FAIL] archive reset did not clear persistent state\n";
			return 1;
		}
		deepcache::PreviewWrite replacement;
		replacement.cacheKey = "reset-model";
		replacement.fingerprint = "fp-reset";
		replacement.width = 13;
		replacement.height = 9;
		replacement.rgba = std::make_shared<const std::vector<std::uint8_t>>(updatedPixels);
		worker.enqueue(std::move(replacement));
		if (!waitUntil([&]() { return worker.readyCount() == 1; })) {
			std::cerr << "[FAIL] archive did not accept a fresh write after reset\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(resetDirectory, {{"reset-model", "fp-reset", "plugin-reset"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] rebuilt archive did not reload\n";
			return 1;
		}
		deepcache::DecodedPreview preview;
		const bool rebuilt = worker.tryPopDecoded(preview) && preview.rgba == updatedPixels;
		worker.shutdown();
		if (!rebuilt) {
			std::cerr << "[FAIL] rebuilt archive did not persist only the replacement preview\n";
			return 1;
		}
	}
	removeDirectory(resetDirectory);

#ifndef _WIN32
	// A fatal archive I/O error keeps the owning worker available for an
	// explicit reset once the underlying filesystem problem is corrected.
	const std::string errorResetDirectory = directory + "-error-reset";
	removeDirectory(errorResetDirectory);
	makeDirectory(errorResetDirectory);
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(errorResetDirectory, {{"error-model", "fp-error", "plugin-error"}});
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] error-reset archive did not finish initial load\n";
			return 1;
		}
		if (chmod(errorResetDirectory.c_str(), 0500) != 0) {
			std::cerr << "[FAIL] could not make archive temporarily unwritable\n";
			return 1;
		}
		deepcache::PreviewWrite blocked;
		blocked.cacheKey = "error-model";
		blocked.fingerprint = "fp-error";
		blocked.width = 13;
		blocked.height = 9;
		blocked.rgba = std::make_shared<const std::vector<std::uint8_t>>(firstPixels);
		worker.enqueue(std::move(blocked));
		const bool enteredError = waitUntil([&]() {
			return worker.state() == deepcache::DatabaseState::ERROR;
		});
		chmod(errorResetDirectory.c_str(), 0700);
		if (!enteredError || !worker.requestReset() || !waitUntil([&]() {
			return worker.state() == deepcache::DatabaseState::EMPTY && worker.errorCode() == 0;
		})) {
			std::cerr << "[FAIL] fatal archive worker did not recover through reset\n";
			return 1;
		}
		worker.shutdown();
	}
	removeDirectory(errorResetDirectory);
#endif

	// Reset must interrupt a startup decode that is already filling the bounded
	// handoff queue. No pre-reset pixels may appear after EMPTY is published.
	const std::string loadingResetDirectory = directory + "-loading-reset";
	removeDirectory(loadingResetDirectory);
	makeDirectory(loadingResetDirectory);
	std::vector<deepcache::ArchiveWantedEntry> loadingWanted;
	for (int i = 0; i < 20; ++i) {
		const std::string key = "loading-" + std::to_string(i);
		loadingWanted.push_back({key, "fp-" + std::to_string(i), "plugin-loading"});
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(loadingResetDirectory, loadingWanted);
		if (!waitUntil([&]() { return worker.state() != deepcache::DatabaseState::LOADING; })) {
			std::cerr << "[FAIL] loading-reset archive did not initialize\n";
			return 1;
		}
		for (int i = 0; i < 20; ++i) {
			deepcache::PreviewWrite write;
			write.cacheKey = "loading-" + std::to_string(i);
			write.fingerprint = "fp-" + std::to_string(i);
			write.width = 13;
			write.height = 9;
			write.rgba = std::make_shared<const std::vector<std::uint8_t>>(pixels(13, 9, 10 + i));
			if (!waitUntil([&]() { return worker.enqueue(write); })) {
				std::cerr << "[FAIL] loading-reset archive write queue stalled\n";
				return 1;
			}
		}
		if (!waitUntil([&]() { return worker.readyCount() == 20; })) {
			std::cerr << "[FAIL] loading-reset archive was not populated\n";
			return 1;
		}
		worker.shutdown();
	}
	{
		deepcache::DeepcacheArchiveWorker worker;
		worker.start(loadingResetDirectory, loadingWanted);
		if (!waitUntil([&]() { return worker.hasPendingDecoded(); }) || !worker.requestReset()) {
			std::cerr << "[FAIL] reset was not accepted during startup decode\n";
			return 1;
		}
		if (!waitUntil([&]() {
			return worker.state() == deepcache::DatabaseState::EMPTY &&
			       worker.readyCount() == 0 && worker.packBytes() == 0;
		})) {
			std::cerr << "[FAIL] startup decode reset did not reach an empty archive\n";
			return 1;
		}
		deepcache::DecodedPreview stale;
		if (worker.tryPopDecoded(stale)) {
			std::cerr << "[FAIL] pre-reset startup pixels escaped after reset completion\n";
			return 1;
		}
		worker.shutdown();
	}
	removeDirectory(loadingResetDirectory);

	removeDirectory(directory);
	const bool pass = decodedLatest && staleRejected && compactedSize < sizeAfterUpdate;
	std::cout << (pass ? "[PASS]" : "[FAIL]")
	          << " append-only QOI archive reloads, invalidates, compacts, cancels, and repairs corruption"
	          << " :: append=" << sizeAfterUpdate << " compact=" << compactedSize << "\n";
	return pass ? 0 : 1;
}
