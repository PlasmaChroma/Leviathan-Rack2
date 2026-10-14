#include "OctaviaAnalysis.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <limits>

namespace octavia {
namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kFloorDb = -140.f;
constexpr float kClipVolts = 10.f;

float toDb(float value) {
	return value > 1e-7f ? 20.f * std::log10(value / 5.f) : kFloorDb;
}

float ratioDb(float numerator, float denominator) {
	return 20.f * std::log10(std::max(numerator, 1e-7f) / std::max(denominator, 1e-7f));
}

struct BiquadCoefficients {
	double b0 = 1.0, b1 = 0.0, b2 = 0.0, a1 = 0.0, a2 = 0.0;
};

BiquadCoefficients makeHighPass(double sampleRate, double hz, double q) {
	const double k = std::tan(double(kPi) * hz / sampleRate);
	const double a0 = 1.0 + k / q + k * k;
	BiquadCoefficients result;
	result.b0 = 1.0 / a0;
	result.b1 = -2.0 / a0;
	result.b2 = result.b0;
	result.a1 = 2.0 * (k * k - 1.0) / a0;
	result.a2 = (1.0 - k / q + k * k) / a0;
	return result;
}

BiquadCoefficients makeHighShelf(double sampleRate, double hz, double gainDb) {
	constexpr double q = 0.7071752369554196;
	const double k = std::tan(double(kPi) * hz / sampleRate);
	const double vh = std::pow(10.0, gainDb / 20.0);
	const double vb = std::pow(vh, 0.4996667741545416);
	const double a0 = 1.0 + k / q + k * k;
	BiquadCoefficients result;
	result.b0 = (vh + vb * k / q + k * k) / a0;
	result.b1 = 2.0 * (k * k - vh) / a0;
	result.b2 = (vh - vb * k / q + k * k) / a0;
	result.a1 = 2.0 * (k * k - 1.0) / a0;
	result.a2 = (1.0 - k / q + k * k) / a0;
	return result;
}

void loudnessAnalysis(const std::vector<float>& samples, float sampleRate,
		ChannelAnalysis* result) {
	if (!result || samples.empty() || sampleRate <= 0.f) return;
	const BiquadCoefficients shelf = makeHighShelf(sampleRate, 1681.974450955533, 3.999843853973347);
	const BiquadCoefficients highPass = makeHighPass(sampleRate, 38.13547087602444, 0.5003270373238773);
	double shelfZ1 = 0.0, shelfZ2 = 0.0, highPassZ1 = 0.0, highPassZ2 = 0.0;
	const size_t blockFrames = std::max<size_t>(1, size_t(std::llround(sampleRate * 0.1)));
	std::vector<double> blocks;
	blocks.reserve((samples.size() + blockFrames - 1) / blockFrames);
	double blockSum = 0.0, totalSum = 0.0;
	size_t blockCount = 0;
	for (float volts : samples) {
		const double input = double(volts) * 0.2;
		const double y1 = shelf.b0 * input + shelfZ1;
		shelfZ1 = shelf.b1 * input - shelf.a1 * y1 + shelfZ2;
		shelfZ2 = shelf.b2 * input - shelf.a2 * y1;
		const double y2 = highPass.b0 * y1 + highPassZ1;
		highPassZ1 = highPass.b1 * y1 - highPass.a1 * y2 + highPassZ2;
		highPassZ2 = highPass.b2 * y1 - highPass.a2 * y2;
		blockSum += y2 * y2;
		totalSum += y2 * y2;
		if (++blockCount == blockFrames) {
			blocks.push_back(blockSum / blockCount);
			blockSum = 0.0;
			blockCount = 0;
		}
	}
	if (blockCount) blocks.push_back(blockSum / blockCount);
	auto lufs = [](double power) {
		return power > 1e-14 ? float(-0.691 + 10.0 * std::log10(power)) : kFloorDb;
	};
	auto recent = [&](size_t count) {
		if (blocks.size() < count) return kFloorDb;
		double sum = 0.0;
		for (size_t i = blocks.size() - count; i < blocks.size(); ++i) sum += blocks[i];
		return lufs(sum / count);
	};
	std::vector<double> absoluteGated;
	for (double power : blocks) if (lufs(power) > -70.f) absoluteGated.push_back(power);
	double absoluteMean = 0.0;
	for (double power : absoluteGated) absoluteMean += power;
	if (!absoluteGated.empty()) absoluteMean /= absoluteGated.size();
	const float relativeGate = lufs(absoluteMean) - 10.f;
	double gatedSum = 0.0;
	uint32_t gatedCount = 0;
	for (double power : absoluteGated) {
		if (lufs(power) > relativeGate) { gatedSum += power; ++gatedCount; }
	}
	result->loudnessAvailable = true;
	result->integratedLufs = gatedCount ? lufs(gatedSum / gatedCount) : kFloorDb;
	result->momentaryLufs = recent(4);
	result->shortTermLufs = recent(30);
	result->kWeightedDbfsEstimate = lufs(totalSum / samples.size());
	result->loudnessBlocks = static_cast<uint32_t>(blocks.size());
}

size_t fftSizeFor(size_t available) {
	size_t size = 1;
	while (size <= available / 2 && size < 4096) size <<= 1;
	return size >= 64 ? size : 0;
}

void fft(std::vector<std::complex<float> >& values) {
	const size_t n = values.size();
	for (size_t i = 1, j = 0; i < n; ++i) {
		size_t bit = n >> 1;
		for (; j & bit; bit >>= 1) j ^= bit;
		j ^= bit;
		if (i < j) std::swap(values[i], values[j]);
	}
	for (size_t length = 2; length <= n; length <<= 1) {
		const std::complex<float> step = std::polar(1.f, -2.f * kPi / float(length));
		for (size_t base = 0; base < n; base += length) {
			std::complex<float> phase(1.f, 0.f);
			for (size_t offset = 0; offset < length / 2; ++offset) {
				const std::complex<float> even = values[base + offset];
				const std::complex<float> odd = values[base + offset + length / 2] * phase;
				values[base + offset] = even + odd;
				values[base + offset + length / 2] = even - odd;
				phase *= step;
			}
		}
	}
}

std::vector<SpectrumBin> fftSpectrum(const std::vector<float>& samples, size_t start,
		size_t n, float sampleRate) {
	std::vector<SpectrumBin> spectrum;
	if (!n || sampleRate <= 0.f) return spectrum;
	double mean = 0.0;
	for (size_t i = start; i < start + n; ++i) mean += samples[i];
	mean /= double(n);
	std::vector<std::complex<float> > bins(n);
	for (size_t i = 0; i < n; ++i) {
		const float window = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(n - 1));
		bins[i] = std::complex<float>((samples[start + i] - float(mean)) * window, 0.f);
	}
	fft(bins);
	for (size_t i = 1; i < n / 2; ++i) {
		const float hz = float(i) * sampleRate / float(n);
		if (hz < 20.f || hz > std::min(20000.f, sampleRate * 0.5f)) continue;
		SpectrumBin bin;
		bin.hz = hz;
		bin.db = toDb(4.f * std::abs(bins[i]) / float(n));
		spectrum.push_back(bin);
	}
	return spectrum;
}

std::vector<SpectrumBin> averagedSpectrum(const std::vector<float>& samples,
		size_t n, float sampleRate) {
	if (!n || samples.size() < n) return {};
	const size_t availableStarts = samples.size() - n;
	const size_t possibleWindows = availableStarts ? 1 + availableStarts / std::max<size_t>(1, n / 2) : 1;
	const size_t windowCount = std::min<size_t>(32, possibleWindows);
	std::vector<SpectrumBin> result;
	std::vector<double> powers;
	for (size_t window = 0; window < windowCount; ++window) {
		const size_t start = windowCount == 1 ? availableStarts
			: availableStarts * window / (windowCount - 1);
		const std::vector<SpectrumBin> spectrum = fftSpectrum(samples, start, n, sampleRate);
		if (result.empty()) {
			result = spectrum;
			powers.assign(spectrum.size(), 0.0);
		}
		const size_t count = std::min(result.size(), spectrum.size());
		for (size_t bin = 0; bin < count; ++bin)
			powers[bin] += std::pow(10.0, spectrum[bin].db / 10.0);
	}
	for (size_t bin = 0; bin < result.size(); ++bin)
		result[bin].db = powers[bin] > 1e-14
			? float(10.0 * std::log10(powers[bin] / windowCount)) : kFloorDb;
	return result;
}

float goertzelDb(const std::vector<float>& samples, size_t start, size_t count,
		float sampleRate, float hz) {
	if (!count || sampleRate <= 0.f || hz >= sampleRate * 0.5f) return kFloorDb;
	double mean = 0.0;
	for (size_t i = 0; i < count; ++i) mean += samples[start + i];
	mean /= count;
	const float coefficient = 2.f * std::cos(2.f * kPi * hz / sampleRate);
	float s1 = 0.f, s2 = 0.f;
	for (size_t i = 0; i < count; ++i) {
		const float window = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(count - 1));
		const float s0 = (samples[start + i] - float(mean)) * window + coefficient * s1 - s2;
		s2 = s1; s1 = s0;
	}
	const float power = std::max(0.f, s1 * s1 + s2 * s2 - coefficient * s1 * s2);
	return toDb(4.f * std::sqrt(power) / float(count));
}

float nearestDb(const std::vector<SpectrumBin>& spectrum, float hz) {
	if (spectrum.empty()) return kFloorDb;
	auto found = std::min_element(spectrum.begin(), spectrum.end(), [hz](const SpectrumBin& a,
			const SpectrumBin& b) { return std::fabs(a.hz - hz) < std::fabs(b.hz - hz); });
	return found->db;
}

void spectralAnalysis(const std::vector<float>& samples, float sampleRate,
		bool includeSpectrum, ChannelAnalysis* result) {
	const size_t n = fftSizeFor(samples.size());
	if (!n || sampleRate <= 0.f) return;
	const std::vector<SpectrumBin> aggregate = averagedSpectrum(samples, n, sampleRate);
	if (aggregate.empty()) return;
	const size_t temporalN = fftSizeFor(samples.size() / 2);
	const size_t temporalLatestStart = samples.size() - temporalN;
	const size_t desiredSeparation = static_cast<size_t>(std::round(sampleRate * 0.120f));
	const size_t earlyStart = temporalLatestStart > desiredSeparation
		? temporalLatestStart - desiredSeparation : 0;
	const std::vector<SpectrumBin> early = fftSpectrum(samples, earlyStart, temporalN, sampleRate);
	const std::vector<SpectrumBin> temporalLatest = fftSpectrum(samples, temporalLatestStart,
		temporalN, sampleRate);
	result->temporalSeparationFrames = temporalLatestStart - earlyStart;
	static const float edges[8] = {20.f, 45.f, 250.f, 800.f, 2500.f, 5000.f, 8000.f, 16000.f};
	std::array<double, 7> powers{};
	std::array<uint32_t, 7> counts{};
	std::vector<float> floorBins;
	for (const SpectrumBin& bin : aggregate) {
		const float amplitude = 5.f * std::pow(10.f, bin.db / 20.f);
		floorBins.push_back(bin.db);
		for (size_t band = 0; band < 7; ++band) {
			if (bin.hz >= edges[band] && bin.hz < edges[band + 1]) {
				powers[band] += double(amplitude) * amplitude;
				counts[band]++;
				break;
			}
		}
	}
	if (!floorBins.empty()) {
		const size_t middle = floorBins.size() / 2;
		std::nth_element(floorBins.begin(), floorBins.begin() + middle, floorBins.end());
		result->noiseFloorDb = floorBins[middle];
	}
	for (size_t band = 0; band < 7; ++band) {
		if (counts[band]) result->bandsDb[band] =
			toDb(std::sqrt(float(powers[band] / counts[band])));
	}

	// FFT-local maxima provide broad candidates; stability is evaluated from
	// two disjoint windows in this same frozen capture, never by sleeping.
	for (size_t i = 2; i + 2 < aggregate.size(); ++i) {
		if (aggregate[i].db <= aggregate[i - 1].db || aggregate[i].db < aggregate[i + 1].db) continue;
		float neighborhood = 0.f;
		int count = 0;
		for (int offset = -6; offset <= 6; ++offset) {
			if (offset >= -1 && offset <= 1) continue;
			const int index = int(i) + offset;
			if (index < 0 || index >= int(aggregate.size())) continue;
			neighborhood += aggregate[index].db; count++;
		}
		const float prominence = aggregate[i].db - neighborhood / std::max(count, 1);
		if (prominence < 8.f || aggregate[i].db < result->noiseFloorDb + 6.f
				|| aggregate[i].db < -80.f) continue;
		ResonanceCandidate candidate;
		candidate.hz = aggregate[i].hz;
		candidate.db = aggregate[i].db;
		candidate.prominenceDb = prominence;
		candidate.stable = std::fabs(nearestDb(early, candidate.hz)
			- nearestDb(temporalLatest, candidate.hz)) <= 3.f;
		result->resonances.push_back(candidate);
	}
	std::sort(result->resonances.begin(), result->resonances.end(),
		[](const ResonanceCandidate& a, const ResonanceCandidate& b) {
			return a.prominenceDb > b.prominenceDb;
		});
	if (result->resonances.size() > 10) result->resonances.resize(10);
	for (const ResonanceCandidate& candidate : result->resonances) {
		const float rise = nearestDb(temporalLatest, candidate.hz) - nearestDb(early, candidate.hz);
		if (candidate.prominenceDb >= 18.f && candidate.db >= -20.f && rise >= 3.f) {
			result->feedbackSuspect = true;
			result->feedbackHz = candidate.hz;
			result->feedbackRiseDb = rise;
			result->issues.push_back("feedback_suspect");
			break;
		}
	}
	float fundamental = 0.f;
	float fundamentalDb = -999.f;
	for (const ResonanceCandidate& candidate : result->resonances) {
		if (candidate.hz < 5000.f && candidate.db > fundamentalDb) {
			fundamental = candidate.hz;
			fundamentalDb = candidate.db;
		}
	}
	for (const ResonanceCandidate& candidate : result->resonances) {
		if (candidate.hz < 8000.f || fundamental <= 0.f) continue;
		const float ratio = candidate.hz / fundamental;
		if (std::fabs(ratio - std::round(ratio)) > 0.07f
				&& candidate.db > result->noiseFloorDb + 12.f) {
			result->issues.push_back("aliasing_suspect");
			break;
		}
	}

	auto detectHum = [&](float fundamental, std::array<float, 4>* series) {
		const size_t humCount = std::min<size_t>(8192, samples.size() / 2);
		const size_t humLatestStart = samples.size() - humCount;
		const size_t humEarlyStart = humLatestStart > desiredSeparation
			? humLatestStart - desiredSeparation : 0;
		int hits = 0;
		for (int harmonic = 1; harmonic <= 4; ++harmonic) {
			const float hz = fundamental * harmonic;
			const float latestDb = goertzelDb(samples, humLatestStart, humCount, sampleRate, hz);
			(*series)[harmonic - 1] = latestDb;
			const float neighbor = std::max(
				goertzelDb(samples, humLatestStart, humCount, sampleRate, hz * 0.85f),
				goertzelDb(samples, humLatestStart, humCount, sampleRate, hz * 1.15f));
			const float earlyDb = goertzelDb(samples, humEarlyStart, humCount, sampleRate, hz);
			if (latestDb > result->noiseFloorDb + 12.f && latestDb - neighbor >= 6.f
					&& std::fabs(latestDb - earlyDb) < 4.f) hits++;
		}
		return hits >= 2;
	};
	result->hum.detected50 = detectHum(50.f, &result->hum.series50Db);
	result->hum.detected60 = detectHum(60.f, &result->hum.series60Db);
	if (result->hum.detected50) result->issues.push_back("hum_50");
	if (result->hum.detected60) result->issues.push_back("hum_60");
	if (result->rms < 1e-4f) result->issues.push_back("silence");
	if (result->clippedSamples) result->issues.push_back("clipping");
	if (std::fabs(result->dcOffset) > 0.25f) result->issues.push_back("dc_offset");
	if (result->bandsDb[0] > result->bandsDb[1] + 6.f && result->bandsDb[0] > -60.f)
		result->issues.push_back("rumble");
	if (result->bandsDb[5] > result->bandsDb[3] + 6.f && result->bandsDb[5] > -60.f)
		result->issues.push_back("sibilance");
	if (includeSpectrum) result->spectrum = aggregate;
}

ChannelAnalysis analyzeChannel(const FrozenObservation& snapshot, ObserveChannel channel,
		bool detailed, bool includeSpectrum) {
	ChannelAnalysis result;
	result.channel = channel;
	const size_t index = static_cast<size_t>(channel);
	const std::vector<float>& samples = snapshot.samples[index];
	result.frames = samples.size();
	result.connected = (snapshot.anyConnectedMask & observeChannelBit(channel)) != 0;
	if (samples.empty()) return result;
	double sum = 0.0;
	double sumSquares = 0.0;
	for (float sample : samples) {
		sum += sample;
		sumSquares += double(sample) * sample;
		result.peak = std::max(result.peak, std::fabs(sample));
		if (std::fabs(sample) >= kClipVolts) result.clippedSamples++;
	}
	result.dcOffset = float(sum / samples.size());
	result.rms = std::sqrt(float(sumSquares / samples.size()));
	result.rmsDb = toDb(result.rms);
	result.peakDb = toDb(result.peak);
	result.crestDb = ratioDb(result.peak, result.rms);
	if (detailed) {
		loudnessAnalysis(samples, snapshot.sampleRate, &result);
		spectralAnalysis(samples, snapshot.sampleRate, includeSpectrum, &result);
	}
	return result;
}

GroupAnalysis analyzeGroup(const FrozenObservation& snapshot, const AnalysisGroup& group,
		bool detailed, bool includeSpectrum) {
	GroupAnalysis result;
	result.group = group;
	if (!group.stereo) {
		result.mono = analyzeChannel(snapshot, group.first, detailed, includeSpectrum);
		return result;
	}
	StereoAnalysis& stereo = result.stereo;
	stereo.left = group.first;
	stereo.right = group.second;
	stereo.leftAnalysis = analyzeChannel(snapshot, group.first, detailed, includeSpectrum);
	stereo.rightAnalysis = analyzeChannel(snapshot, group.second, detailed, includeSpectrum);
	stereo.balanceDb = ratioDb(stereo.rightAnalysis.rms, stereo.leftAnalysis.rms);
	const std::vector<float>& left = snapshot.samples[static_cast<size_t>(group.first)];
	const std::vector<float>& right = snapshot.samples[static_cast<size_t>(group.second)];
	const size_t count = std::min(left.size(), right.size());
	double ll = 0.0, rr = 0.0, lr = 0.0, mid = 0.0, side = 0.0;
	for (size_t i = 0; i < count; ++i) {
		ll += double(left[i]) * left[i]; rr += double(right[i]) * right[i];
		lr += double(left[i]) * right[i];
		const double m = 0.5 * (left[i] + right[i]);
		const double s = 0.5 * (left[i] - right[i]);
		mid += m * m; side += s * s;
	}
	if (count) {
		stereo.correlation = float(lr / std::sqrt(std::max(ll * rr, 1e-20)));
		stereo.midRms = std::sqrt(float(mid / count));
		stereo.sideRms = std::sqrt(float(side / count));
		stereo.sideToMidDb = ratioDb(stereo.sideRms, stereo.midRms);
	}
	return result;
}

float groupRmsDb(const GroupAnalysis& group) {
	if (!group.group.stereo) return group.mono.rmsDb;
	const float l = group.stereo.leftAnalysis.rms;
	const float r = group.stereo.rightAnalysis.rms;
	return toDb(std::sqrt(0.5f * (l * l + r * r)));
}

float groupPeakDb(const GroupAnalysis& group) {
	if (!group.group.stereo) return group.mono.peakDb;
	return std::max(group.stereo.leftAnalysis.peakDb, group.stereo.rightAnalysis.peakDb);
}

float groupCrestDb(const GroupAnalysis& group) {
	return groupPeakDb(group) - groupRmsDb(group);
}

float groupBandDb(const GroupAnalysis& group, size_t band) {
	if (!group.group.stereo) return group.mono.bandsDb[band];
	const float left = group.stereo.leftAnalysis.bandsDb[band];
	const float right = group.stereo.rightAnalysis.bandsDb[band];
	const double leftPower = std::pow(10.0, left / 10.0);
	const double rightPower = std::pow(10.0, right / 10.0);
	return float(10.0 * std::log10(0.5 * (leftPower + rightPower) + 1e-14));
}

float groupDcOffset(const GroupAnalysis& group) {
	return group.group.stereo
		? 0.5f * (group.stereo.leftAnalysis.dcOffset + group.stereo.rightAnalysis.dcOffset)
		: group.mono.dcOffset;
}

} // namespace

const std::array<const char*, 7>& analysisBandNames() {
	static const std::array<const char*, 7> names{{
		"rumble_20_45", "bass_45_250", "lowmid_250_800", "mid_800_2500",
		"highmid_2500_5000", "sibilance_5000_8000", "air_8000_16000"
	}};
	return names;
}

bool AnalysisEngine::tryAnalyze(const FrozenObservation& snapshot, const AnalysisGroup& group,
		bool detailed, bool includeSpectrum, GroupAnalysis* result, std::string* error) {
	std::unique_lock<std::mutex> lock(executionMutex_, std::try_to_lock);
	if (!lock.owns_lock()) {
		if (error) *error = "analysis_busy";
		return false;
	}
	if (!result || snapshot.sampleRate <= 0.f) {
		if (error) *error = "invalid_snapshot";
		return false;
	}
	*result = analyzeGroup(snapshot, group, detailed, includeSpectrum);
	return true;
}

bool AnalysisEngine::tryCompare(const FrozenObservation& snapshot,
		const AnalysisGroup& referenceGroup, const AnalysisGroup& targetGroup,
		bool detailed, bool includeSpectrum, ComparisonAnalysis* result, std::string* error) {
	std::unique_lock<std::mutex> lock(executionMutex_, std::try_to_lock);
	if (!lock.owns_lock()) {
		if (error) *error = "analysis_busy";
		return false;
	}
	if (!result || snapshot.sampleRate <= 0.f) {
		if (error) *error = "invalid_snapshot";
		return false;
	}
	result->reference = analyzeGroup(snapshot, referenceGroup, detailed, includeSpectrum);
	result->target = analyzeGroup(snapshot, targetGroup, detailed, includeSpectrum);
	result->rmsDeltaDb = groupRmsDb(result->target) - groupRmsDb(result->reference);
	result->peakDeltaDb = groupPeakDb(result->target) - groupPeakDb(result->reference);
	result->crestDeltaDb = groupCrestDb(result->target) - groupCrestDb(result->reference);
	result->dcOffsetDelta = groupDcOffset(result->target) - groupDcOffset(result->reference);
	for (size_t band = 0; band < 7; ++band) {
		result->spectralDeltaDb[band] = groupBandDb(result->target, band)
			- groupBandDb(result->reference, band);
		result->normalizedSpectralDeltaDb[band] = result->spectralDeltaDb[band] - result->rmsDeltaDb;
	}
	if (referenceGroup.stereo && targetGroup.stereo) {
		result->balanceDeltaDb = result->target.stereo.balanceDb - result->reference.stereo.balanceDb;
		result->correlationDelta = result->target.stereo.correlation - result->reference.stereo.correlation;
		result->widthDeltaDb = result->target.stereo.sideToMidDb - result->reference.stereo.sideToMidDb;
	}
	return true;
}

} // namespace octavia
