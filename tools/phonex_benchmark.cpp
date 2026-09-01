#include "../src/PhonexEngine.hpp"
#include "../src/PhonexFixtures.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>

int main() {
	constexpr int kSamples = 2000000;
	constexpr int kRepeats = 3;
	const phonex::LpcSequence sequence = phonex::makeVoicedFixture(64);
	std::cout << "host_rate\tfilter_order\tns_per_sample\n" << std::setprecision(9);
	volatile float sink = 0.f;
	for (float hostRate : {48000.f, 96000.f, 192000.f}) {
		for (int order : {2, 4, 6}) {
			std::array<double, kRepeats> timings{};
			for (int repeat = 0; repeat < kRepeats; ++repeat) {
				phonex::Engine engine;
				engine.setSequence(&sequence);
				engine.setReconstructionMode(phonex::ReconstructionMode::Filtered);
				engine.setReconstructionOrder(order == 2 ? phonex::ReconstructionOrder::TwoPole
					: order == 4 ? phonex::ReconstructionOrder::FourPole
					: phonex::ReconstructionOrder::SixPole);
				phonex::EngineControls controls;
				controls.hostSampleRate = hostRate;
				controls.speed = 0.f;
				for (int sample = 0; sample < 10000; ++sample)
					sink += engine.process(controls).audio;
				const auto begin = std::chrono::steady_clock::now();
				for (int sample = 0; sample < kSamples; ++sample)
					sink += engine.process(controls).audio;
				const auto end = std::chrono::steady_clock::now();
				timings[repeat] = std::chrono::duration<double, std::nano>(end - begin).count()
					/ static_cast<double>(kSamples);
			}
			std::sort(timings.begin(), timings.end());
			std::cout << static_cast<int>(hostRate) << '\t' << order << '\t'
				<< timings[kRepeats / 2] << '\n';
		}
	}
	return sink == 1234567.f ? 1 : 0;
}
