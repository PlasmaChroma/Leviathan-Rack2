#pragma once

#include <atomic>
#include <memory>

namespace octavia {

template <typename T>
inline bool beginQueuedJob(const std::shared_ptr<T>& job) {
	if (!job) return false;
	if (!job->cancelled.load(std::memory_order_acquire)) return true;
	job->done.store(true, std::memory_order_release);
	return false;
}

template <typename T>
inline void cancelTimedOutJob(const std::shared_ptr<T>& job) {
	if (job && !job->done.load(std::memory_order_acquire))
		job->cancelled.store(true, std::memory_order_release);
}

} // namespace octavia
