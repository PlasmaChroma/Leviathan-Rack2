#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace phonex {

class StringView {
public:
	static const std::size_t npos = static_cast<std::size_t>(-1);

	constexpr StringView() : data_(nullptr), size_(0) {}
	template <std::size_t N>
	constexpr StringView(const char (&text)[N]) : data_(text), size_(N - 1) {}
	StringView(const char* text) : data_(text), size_(text ? std::strlen(text) : 0) {}
	constexpr StringView(const char* text, std::size_t size) : data_(text), size_(size) {}
	StringView(const std::string& text) : data_(text.data()), size_(text.size()) {}

	constexpr const char* data() const { return data_; }
	constexpr std::size_t size() const { return size_; }
	constexpr bool empty() const { return size_ == 0; }
	constexpr char operator[](std::size_t index) const { return data_[index]; }
	constexpr char back() const { return data_[size_ - 1]; }
	const char* begin() const { return data_; }
	const char* end() const { return data_ + size_; }
	void remove_suffix(std::size_t count) { size_ -= count; }
	StringView substr(std::size_t position, std::size_t count = npos) const {
		if (position > size_) position = size_;
		const std::size_t available = size_ - position;
		return StringView(data_ + position, count < available ? count : available);
	}
	std::size_t find(char needle, std::size_t position = 0) const {
		for (std::size_t i = position; i < size_; ++i)
			if (data_[i] == needle) return i;
		return npos;
	}
	std::size_t find(StringView needle, std::size_t position = 0) const {
		if (needle.size_ == 0) return position <= size_ ? position : npos;
		for (std::size_t i = position; i + needle.size_ <= size_; ++i)
			if (std::memcmp(data_ + i, needle.data_, needle.size_) == 0) return i;
		return npos;
	}
	friend bool operator==(StringView a, StringView b) {
		return a.size_ == b.size_
			&& (a.size_ == 0 || std::memcmp(a.data_, b.data_, a.size_) == 0);
	}
	friend bool operator!=(StringView a, StringView b) { return !(a == b); }

private:
	const char* data_;
	std::size_t size_;
};

constexpr int kLpcOrder = 10;
constexpr int kMaxFrames = 2048;
constexpr float kSourceFrameSeconds = 0.020f;
constexpr std::uint16_t kNoPhrase = 0xffffu;

enum class Phone : std::uint8_t {
	AA = 0, AE, AH, AO, AW, AY,
	B, CH, D, DH,
	EH, ER, EY,
	F, G, HH,
	IH, IY,
	JH, K, L, M, N, NG,
	OW, OY,
	P, R, S, SH, T, TH,
	UH, UW,
	V, W, Y, Z, ZH,
	SIL,
	Count,
};

constexpr std::size_t kPhoneCount = static_cast<std::size_t>(Phone::Count);

enum class Excitation : std::uint8_t {
	Silence = 0,
	Unvoiced,
	Voiced,
};

struct LpcFrame {
	float energy = 0.f;
	float pitchPeriod10k = 0.f;
	std::array<float, kLpcOrder> reflection{};
	Excitation excitation = Excitation::Silence;

	LpcFrame() = default;
	LpcFrame(float energy, float pitchPeriod10k,
		const std::array<float, kLpcOrder>& reflection, Excitation excitation)
		: energy(energy), pitchPeriod10k(pitchPeriod10k),
		  reflection(reflection), excitation(excitation) {}
};

struct LpcSequence {
	std::array<LpcFrame, kMaxFrames> frames{};
	std::uint16_t frameCount = 0;
	std::uint16_t phraseId = kNoPhrase;
	std::uint32_t generation = 0;

	bool valid() const {
		return frameCount <= frames.size();
	}

	const LpcFrame* frame(std::uint16_t index) const {
		return valid() && index < frameCount ? &frames[index] : nullptr;
	}
};

} // namespace phonex
