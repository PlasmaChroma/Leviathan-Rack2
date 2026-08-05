#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace longplayer {

class Stream {
public:
	static constexpr std::uint32_t kBlockFrames = 32768u;
	static constexpr int kBlockCount = 12;

	Stream();
	~Stream();

	void requestLoad(const std::string& path);
	void clear();
	void setDesiredFrame(std::uint64_t frame, bool loop);
	bool readFrame(std::uint64_t frame, float* left, float* right) const;

	bool ready() const;
	bool loading() const;
	std::uint64_t totalFrames() const;
	std::uint32_t sampleRate() const;
	std::uint32_t channels() const;
	std::uint64_t generation() const;
	std::string path() const;
	std::string displayName() const;
	std::string error() const;

private:
	struct Block {
		std::vector<float> stereo;
		std::atomic<std::uint64_t> sequence {0u};
		mutable std::atomic<std::uint32_t> readers {0u};
		std::uint64_t startFrame = 0u;
		std::uint32_t validFrames = 0u;

		Block();
	};

	void workerLoop();
	void invalidateBlocks();

	std::array<Block, kBlockCount> blocks;
	std::thread worker;
	mutable std::mutex requestMutex;
	std::condition_variable requestCv;
	bool stopRequested = false;
	std::string requestedPath;
	std::uint64_t requestedSerial = 0u;
	std::uint64_t appliedSerial = 0u;

	mutable std::mutex metadataMutex;
	std::string loadedPath;
	std::string loadedDisplayName;
	std::string loadError;
	std::atomic<bool> streamReady {false};
	std::atomic<bool> loadInProgress {false};
	std::atomic<std::uint64_t> publishedFrames {0u};
	std::atomic<std::uint32_t> publishedSampleRate {0u};
	std::atomic<std::uint32_t> publishedChannels {0u};
	std::atomic<std::uint64_t> publishedGeneration {0u};
	std::atomic<std::uint64_t> desiredFrame {0u};
	std::atomic<bool> desiredLoop {false};
};

} // namespace longplayer
