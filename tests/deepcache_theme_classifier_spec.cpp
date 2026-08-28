#include "../src/DeepcacheThemeClassifier.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

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
	const char* files[] = {"theme-classifier-v1.bin", "theme-classifier-v1.bin.tmp",
	                       "theme-classifier-v1.bin.bak"};
	for (const char* file : files)
		std::remove((path + "/" + file).c_str());
#ifdef _WIN32
	_rmdir(path.c_str());
#else
	rmdir(path.c_str());
#endif
}

}  // namespace

int main() {
	using deepcache::ThemeClassification;
	const std::string directory =
		"/tmp/leviathan-deepcache-theme-classifier-" + std::to_string(processId());
	removeDirectory(directory);
	makeDirectory(directory);

	deepcache::ThemeClassifier classifier;
	const bool missingIsSafe = classifier.load(directory) && classifier.size() == 0 &&
		classifier.get("model", "build-a") == ThemeClassification::UNKNOWN;
	const bool learned = classifier.set("model", "build-a", ThemeClassification::INVARIANT) &&
		classifier.set("sensitive", "build-s", ThemeClassification::SENSITIVE) &&
		classifier.save(directory);

	deepcache::ThemeClassifier restored;
	const bool roundTrip = restored.load(directory) && restored.size() == 2 &&
		restored.get("model", "build-a") == ThemeClassification::INVARIANT &&
		restored.get("sensitive", "build-s") == ThemeClassification::SENSITIVE;
	const bool buildChangeInvalidates =
		restored.get("model", "build-b") == ThemeClassification::UNKNOWN;
	const bool updatePersists = restored.set("model", "build-a", ThemeClassification::SENSITIVE) &&
		restored.save(directory);

	deepcache::ThemeClassifier updated;
	const bool updatedRoundTrip = updated.load(directory) &&
		updated.get("model", "build-a") == ThemeClassification::SENSITIVE;

	{
		std::ofstream corrupt((directory + "/theme-classifier-v1.bin").c_str(),
		                      std::ios::binary | std::ios::trunc);
		corrupt << "not-a-classifier";
	}
	deepcache::ThemeClassifier rejected;
	const bool corruptionDegradesToUnknown = !rejected.load(directory) && rejected.size() == 0 &&
		rejected.get("model", "build-a") == ThemeClassification::UNKNOWN;

	removeDirectory(directory);
	const bool pass = missingIsSafe && learned && roundTrip && buildChangeInvalidates &&
	                  updatePersists && updatedRoundTrip && corruptionDegradesToUnknown;
	std::cout << (pass ? "[PASS]" : "[FAIL]")
	          << " Deepcache theme classifications round-trip and invalidate safely by build"
	          << " missing=" << missingIsSafe << " learned=" << learned
	          << " roundTrip=" << roundTrip << " invalidated=" << buildChangeInvalidates
	          << " update=" << updatePersists << " updated=" << updatedRoundTrip
	          << " corruption=" << corruptionDegradesToUnknown << "\n";
	return pass ? 0 : 1;
}
