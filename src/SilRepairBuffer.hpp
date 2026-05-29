#pragma once

#include "SilRepairKernel.hpp"

#include <algorithm>
#include <vector>

namespace sil {
namespace repair {

struct StereoWindows {
	Window5 left;
	Window5 right;
};

class RepairBuffer {
public:
	void setLookaheadSamples(int lookaheadSamples) {
		lookaheadSamples_ = std::max(0, std::min(lookaheadSamples, maxLookaheadSamples_));
	}

	void configure(int lookaheadSamples, int minHistorySamples) {
		maxLookaheadSamples_ = std::max(0, lookaheadSamples);
		lookaheadSamples_ = maxLookaheadSamples_;
		const int historySamples = std::max(maxLookaheadSamples_, std::max(0, minHistorySamples));
		const int requiredLength = maxLookaheadSamples_ + historySamples + 1;
		if (int(left_.size()) == requiredLength && int(right_.size()) == requiredLength) {
			return;
		}
		left_.assign(size_t(requiredLength), 0.f);
		right_.assign(size_t(requiredLength), 0.f);
		writeIndex_ = 0;
	}

	void clear() {
		std::fill(left_.begin(), left_.end(), 0.f);
		std::fill(right_.begin(), right_.end(), 0.f);
		writeIndex_ = 0;
	}

	bool empty() const {
		return left_.empty() || right_.empty();
	}

	int size() const {
		return std::min(int(left_.size()), int(right_.size()));
	}

	void push(float left, float right) {
		if (empty()) {
			return;
		}
		left_[size_t(writeIndex_)] = left;
		right_[size_t(writeIndex_)] = right;
		writeIndex_ = (writeIndex_ + 1) % size();
	}

	StereoWindows readCurrentWindows() const {
		StereoWindows out;
		if (empty()) {
			return out;
		}
		const int center = writeIndex_ - 1 - lookaheadSamples_;
		out.left = readWindow(left_, center);
		out.right = readWindow(right_, center);
		return out;
	}

private:
	Window5 readWindow(const std::vector<float>& buf, int center) const {
		Window5 w;
		w.prev2 = readWrapped(buf, center - 2);
		w.prev1 = readWrapped(buf, center - 1);
		w.center = readWrapped(buf, center);
		w.next1 = readWrapped(buf, center + 1);
		w.next2 = readWrapped(buf, center + 2);
		return w;
	}

	float readWrapped(const std::vector<float>& buf, int idx) const {
		if (buf.empty()) {
			return 0.f;
		}
		const int len = int(buf.size());
		const int wrapped = (idx % len + len) % len;
		return buf[size_t(wrapped)];
	}

	std::vector<float> left_;
	std::vector<float> right_;
	int writeIndex_ = 0;
	int lookaheadSamples_ = 0;
	int maxLookaheadSamples_ = 0;
};

} // namespace repair
} // namespace sil
