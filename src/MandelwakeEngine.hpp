#pragma once

#include "MandelwakeFixedPoint.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace mandelwake {

constexpr int kMaxChannels = 16;
constexpr int kHistoryCapacity = 256;

enum class Map : std::uint8_t {
	Mandelbrot = 0,
	Julia = 1,
	BurningShip = 2
};

struct StepInputs {
	OrbitQ28 cXQ28 = 0;
	OrbitQ28 cYQ28 = 0;
	OrbitQ28 mutationDepthQ28 = 0;
	std::uint32_t densityQ16 = 32768;
	std::uint8_t iterations = 4;
};

struct StepOutputs {
	OrbitQ28 xQ28 = 0;
	OrbitQ28 yQ28 = 0;
	std::uint32_t radiusQ28 = 0;
	std::int32_t phaseQ30 = 0;
	std::uint64_t stepIndex = 0;
	bool gate = false;
	bool escaped = false;
};

struct HistoryPoint {
	OrbitQ28 xQ28;
	OrbitQ28 yQ28;

	HistoryPoint(OrbitQ28 x = 0, OrbitQ28 y = 0)
		: xQ28(x), yQ28(y) {
	}
};

struct ChannelState {
	OrbitQ28 xQ28 = 0;
	OrbitQ28 yQ28 = 0;
	std::uint64_t stepIndex = 0;
	std::array<HistoryPoint, kHistoryCapacity> history {};
	std::uint16_t historyWriteIndex = 0;
	std::uint16_t historyCount = 0;
	std::uint32_t resetSerial = 0;
	std::uint32_t escapeSerial = 0;
	HistoryPoint lastPreEscape {};
	HistoryPoint lastReentry {};
};

class Engine {
public:
	explicit Engine(std::uint64_t seed = UINT64_C(0x4D414E44454C574B));

	std::uint64_t baseSeed() const { return baseSeed_; }
	Map map() const { return map_; }
	std::uint32_t mapSerial() const { return mapSerial_; }
	std::uint32_t reseedSerial() const { return reseedSerial_; }

	void setBaseSeed(std::uint64_t seed) { baseSeed_ = seed; }
	bool setMap(Map map);
	std::uint64_t reseed();
	std::uint64_t reseed(Map map);
	void resetAll();
	void resetChannel(int channel);
	StepOutputs step(int channel, const StepInputs& inputs);

	const ChannelState& channel(int index) const;
	HistoryPoint historyPointOldestFirst(int channel, int index) const;

private:
	std::array<ChannelState, kMaxChannels> channels_ {};
	std::uint64_t baseSeed_ = 0;
	Map map_ = Map::Mandelbrot;
	std::uint32_t mapSerial_ = 0;
	std::uint32_t reseedSerial_ = 0;

	static int clampChannel(int channel);
	void appendHistory(ChannelState& state, OrbitQ28 xQ28, OrbitQ28 yQ28);
};

} // namespace mandelwake
