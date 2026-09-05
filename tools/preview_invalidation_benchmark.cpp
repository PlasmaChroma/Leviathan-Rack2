// Native Rack-linked CPU microbenchmark; does not measure rendering or FPS.
#include "WavePreviewGeometryKey.hpp"
#include "WavePreviewTracer.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>

static void require(bool condition, const char* message) {
	if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

static volatile unsigned checksum = 0;

__attribute__((noinline)) static void oldClear(WavePreviewBufferedTracer<128>& tracer) {
	std::fill(tracer.pixels.begin(), tracer.pixels.end(), 0u);
	tracer.pixelsDirty = true;
	tracer.hasVisiblePixels = false;
	tracer.lastCaptureSec = tracer.lastFadeSec = -1.0;
}

__attribute__((noinline)) static void newClear(WavePreviewBufferedTracer<128>& tracer) {
	tracer.clear();
}

template <typename F>
static void bench(const char* label, F fn) {
	constexpr int iterations = 2000;
	std::vector<double> samples;
	for (int batch = 0; batch < 55; ++batch) {
		const auto start = std::chrono::steady_clock::now();
		for (int i = 0; i < iterations; ++i) {
			fn(i);
			asm volatile("" ::: "memory");
		}
		const double ns = std::chrono::duration<double, std::nano>(
			std::chrono::steady_clock::now() - start).count() / iterations;
		if (batch >= 5) samples.push_back(ns);
	}
	std::sort(samples.begin(), samples.end());
	std::printf("%s,median_ns=%.2f,p95_ns=%.2f\n", label, samples[25], samples[47]);
}

int main() {
	wave_preview::GeometryKey key;
	require(key.accept(1.f, 3.f, 0.f, 106.f, 48.f), "initial geometry accepted");
	for (int i = 1; i <= 12000; ++i) {
		const float scale = 0.001f + i * 0.001f;
		require(!key.accept(scale, 3.f * scale, 0.f, 106.f, 48.f), "common-rate geometry stable");
	}
	bool accumulated = false;
	for (int i = 1; i <= 32; ++i)
		accumulated |= key.accept(1.f + i * 1e-6f, 3.f, 0.f, 106.f, 48.f);
	require(accumulated, "slow changes accumulate against accepted key");
	require(key.accept(2.f, 3.f, 0.f, 106.f, 48.f), "clamped/asymmetric timing changes geometry");
	require(key.accept(2.f, 3.f, 0.1f, 106.f, 48.f), "shape changes accepted");
	require(key.accept(2.f, 3.f, 0.1f, 212.f, 96.f), "resize accepted");

	WavePreviewBufferedTracer<128> tracer;
	tracer.ensureSize(212, 96);
	tracer.clear();
	require(tracer.pixelsDirty, "initial upload stays pending");
	tracer.pixelsDirty = false; // Simulate completed upload.
	tracer.clear();
	require(!tracer.pixelsDirty, "already-clear buffer stays clean");
	std::array<Vec, 128> points{};
	for (size_t i = 0; i < points.size(); ++i) points[i] = Vec(float(i) * 0.8f, 24.f);
	tracer.capture(points, 1.0, Vec(106.f, 48.f), WavePreviewBufferedTracerStyle{});
	require(tracer.hasVisiblePixels, "real capture creates visible pixels");
	tracer.clear();
	require(!tracer.hasVisiblePixels && tracer.pixelsDirty, "visible buffer cleared for upload");
	require(std::all_of(tracer.pixels.begin(), tracer.pixels.end(), [](uint32_t p) { return p == 0; }), "pixels transparent");
	tracer.clear();
	require(tracer.pixelsDirty, "repeat clear preserves pending upload");
	std::puts("PASS: geometry and buffer-clear contracts");
	std::puts("rate-only schedule: old version rule=12000 rebuild/capture attempts; geometry key=0 after initial acceptance");
	for (int scale : {1, 2, 4}) {
		tracer.ensureSize(106 * scale, 48 * scale);
		char label[100];
		std::snprintf(label, sizeof(label), "old repeated clear %dx", scale);
		bench(label, [&](int) { oldClear(tracer); });
		std::snprintf(label, sizeof(label), "idempotent clear %dx", scale);
		bench(label, [&](int) { newClear(tracer); });
	}
	key = {};
	key.accept(1.f, 3.f, 0.f, 106.f, 48.f);
	bench("geometry key common-rate", [&](int i) {
		const float scale = 0.001f + i * 0.001f;
		checksum += key.accept(scale, scale * 3.f, 0.f, 106.f, 48.f);
	});
	bench("geometry key changing ratio", [&](int i) {
		checksum += key.accept(1.f + i * 0.001f, 3.f, 0.f, 106.f, 48.f);
	});
	return 0;
}
