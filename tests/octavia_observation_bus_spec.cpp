#include "OctaviaObservationBus.hpp"

#include <iostream>
#include <thread>
#include <vector>

namespace {
int failures = 0;
void check(bool pass, const char* name) {
	std::cout << (pass ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!pass) failures++;
}
}

int main() {
	octavia::ObservationBus bus;
	octavia::ObservationTrigger sent;
	sent.octaviaModuleId = 42;
	sent.triggerFrame = 123456;
	sent.preFrames = 128;
	sent.postFrames = 256;
	sent.monitorMask = 0x0c;
	sent.setLabel("scene-hit");
	const uint64_t id = bus.publish(sent);
	check(id == 1, "publication assigns a stable request sequence");

	uint64_t firstCursor = 0, secondCursor = 0, dropped = 0;
	octavia::ObservationTrigger first, second;
	check(bus.poll(&firstCursor, &first, &dropped)
		&& first.requestId == id && first.octaviaModuleId == 42
		&& first.triggerFrame == 123456 && first.preFrames == 128
		&& first.postFrames == 256 && first.monitorMask == 0x0c
		&& first.labelString() == "scene-hit",
		"consumer receives a coherent exact-frame POD trigger");
	check(bus.poll(&secondCursor, &second, &dropped) && second.requestId == id,
		"independent Octavia cursors observe the same broadcast trigger");

	octavia::ObservationTrigger other = sent;
	other.octaviaModuleId = 99;
	other.triggerFrame++;
	bus.publish(other);
	check(bus.poll(&firstCursor, &first, &dropped) && first.octaviaModuleId == 99,
		"target module identity survives publication for consumer-side filtering");

	for (size_t i = 0; i < octavia::OBSERVATION_TRIGGER_CAPACITY + 8; ++i) {
		octavia::ObservationTrigger item = sent;
		item.triggerFrame = 200000 + i;
		bus.publish(item);
	}
	uint64_t staleCursor = 0, overwritten = 0;
	octavia::ObservationTrigger newest;
	bool received = false;
	for (size_t i = 0; i < octavia::OBSERVATION_TRIGGER_CAPACITY + 2; ++i)
		received |= bus.poll(&staleCursor, &newest, &overwritten);
	check(received && overwritten > 0,
		"bounded ring reports overwritten requests instead of blocking a producer");

	octavia::ObservationBus concurrent;
	std::vector<std::thread> producers;
	for (int producer = 0; producer < 4; ++producer) {
		producers.emplace_back([&concurrent, producer] {
			for (int i = 0; i < 8; ++i) {
				octavia::ObservationTrigger item;
				item.octaviaModuleId = producer;
				item.triggerFrame = uint64_t(producer * 100 + i);
				item.monitorMask = uint8_t(1u << producer);
				concurrent.publish(item);
			}
		});
	}
	for (auto& producer : producers) producer.join();
	uint64_t cursor = 0, concurrentDropped = 0;
	int coherent = 0;
	while (cursor < concurrent.latestSequence()) {
		octavia::ObservationTrigger item;
		if (concurrent.poll(&cursor, &item, &concurrentDropped)) {
			const int producer = int(item.octaviaModuleId);
			if (producer >= 0 && producer < 4
					&& item.monitorMask == uint8_t(1u << producer)
					&& item.triggerFrame / 100 == uint64_t(producer)) coherent++;
		}
	}
	check(coherent == 32 && concurrentDropped == 0,
		"concurrent publishers never expose torn cross-trigger fields");

	std::cout << "[TEST SUMMARY] failures=" << failures << "\n";
	return failures ? 1 : 0;
}
