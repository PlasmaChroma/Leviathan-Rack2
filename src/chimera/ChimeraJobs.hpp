#pragma once

#include "ChimeraOwnership.hpp"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace chimera {

struct JobGeneration {
    std::atomic<std::uint64_t> value;
    std::atomic<std::uint32_t> outstanding;
    std::atomic<bool> closed;
    JobGeneration() : value(1), outstanding(0), closed(false) {}
    void invalidate() { value.fetch_add(1, std::memory_order_acq_rel); }
    void close() {
        closed.store(true, std::memory_order_release);
        invalidate();
    }
};

class IoService {
public:
    enum Status { Accepted, Busy, Closed, Ready, Stale, Failed, Retired };
    struct Result {
        Status status;
        std::uint64_t requestId, generation;
        std::unique_ptr<Reel> prepared;
        Result() : status(Failed), requestId(0), generation(0) {}
        Result(Result&&) = default;
        Result& operator=(Result&&) = default;
        Result(const Result&) = delete;
        Result& operator=(const Result&) = delete;
    };

    // Construct one plugin-scoped instance off audio. At most two workers.
    explicit IoService(std::uint32_t workers = 2) : closing_(false), outstanding_(0) {
        if (workers > 2) workers = 2;
        for (std::uint32_t i = 0; i < workers; ++i)
            workers_[i] = std::thread(&IoService::run, this);
        workerCount_ = workers;
    }
    IoService(const IoService&) = delete;
    IoService& operator=(const IoService&) = delete;
    ~IoService() { shutdown(); }

    Status prepare(const std::shared_ptr<JobGeneration>& token,
                   std::uint64_t requestId, std::uint32_t pages,
                   std::uint32_t reservePages) {
        if (!token || !pages || pages > kMaxPages || reservePages > pages) return Failed;
        Job job;
        job.kind = Prepare;
        job.token = token;
        job.generation = token->value.load(std::memory_order_acquire);
        job.requestId = requestId;
        job.pages = pages;
        job.reserve = reservePages;
        return submit(std::move(job));
    }
    Status retire(const std::shared_ptr<JobGeneration>& token,
                  std::uint64_t requestId, std::unique_ptr<Reel>& payload) {
        if (!token || !payload) return Failed;
        Job job;
        job.kind = Retire;
        job.token = token;
        job.generation = token->value.load(std::memory_order_acquire);
        job.requestId = requestId;
        job.payload = std::move(payload);
        const Status status = submit(std::move(job));
        if (status != Accepted) payload = std::move(job.payload);
        return status;
    }
    // Bounded off-audio operation with caller-owned completion state. A task
    // captures no Module pointer; it publishes completion before its owner
    // releases any Reel snapshot lease.
    Status execute(const std::shared_ptr<JobGeneration>& token,
                   std::uint64_t requestId, std::function<void()> operation) {
        if (!token || !operation) return Failed;
        Job job;
        job.kind = External;
        job.token = token;
        job.generation = token->value.load(std::memory_order_acquire);
        job.requestId = requestId;
        job.operation = std::move(operation);
        return submit(std::move(job));
    }
    bool poll(Result& out) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (results_.empty()) return false;
        takeResult(results_.begin(), out);
        return true;
    }
    // A module's control dispatcher must not consume another module's result.
    bool pollFor(const std::shared_ptr<JobGeneration>& token, Result& out) {
        if (!token) return false;
        std::lock_guard<std::mutex> lock(mutex_);
        for (std::deque<Completion>::iterator it = results_.begin(); it != results_.end(); ++it)
            if (it->token == token) { takeResult(it, out); return true; }
        return false;
    }
    // Called off audio on module removal. Queued payloads and completed stores
    // are destroyed here; an in-flight worker sees closed and self-retires.
    void cancel(const std::shared_ptr<JobGeneration>& token) {
        if (!token) return;
        token->close();
        std::lock_guard<std::mutex> lock(mutex_);
        for (std::deque<Job>::iterator it = jobs_.begin(); it != jobs_.end();) {
            if (it->token == token) {
                it = jobs_.erase(it);
                token->outstanding.fetch_sub(1, std::memory_order_relaxed);
                --outstanding_;
            }
            else ++it;
        }
        for (std::deque<Completion>::iterator it = results_.begin(); it != results_.end();) {
            if (it->token == token) {
                it = results_.erase(it);
                token->outstanding.fetch_sub(1, std::memory_order_relaxed);
                --outstanding_;
            }
            else ++it;
        }
    }
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closing_ = true;
        }
        cv_.notify_all();
        for (std::uint32_t i = 0; i < workerCount_; ++i)
            if (workers_[i].joinable()) workers_[i].join();
        workerCount_ = 0;
    }
    std::uint32_t outstanding() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return outstanding_;
    }

private:
    enum Kind { Prepare, Retire, External };
    struct Job {
        Kind kind;
        std::shared_ptr<JobGeneration> token;
        std::uint64_t generation, requestId;
        std::uint32_t pages, reserve;
        std::unique_ptr<Reel> payload;
        std::function<void()> operation;
        Job() : kind(Prepare), generation(0), requestId(0), pages(0), reserve(0) {}
        Job(Job&&) = default;
        Job& operator=(Job&&) = default;
        Job(const Job&) = delete;
        Job& operator=(const Job&) = delete;
    };
    struct Completion {
        std::shared_ptr<JobGeneration> token;
        Result result;
        Completion() {}
        Completion(Completion&&) = default;
        Completion& operator=(Completion&&) = default;
        Completion(const Completion&) = delete;
        Completion& operator=(const Completion&) = delete;
    };
    void takeResult(std::deque<Completion>::iterator it, Result& out) {
        out = std::move(it->result);
        if (out.generation != it->token->value.load(std::memory_order_acquire)) {
            out.prepared.reset(); // Poll is service/control side, never audio.
            out.status = Stale;
        }
        it->token->outstanding.fetch_sub(1, std::memory_order_relaxed);
        results_.erase(it);
        --outstanding_;
    }
    Status submit(Job&& job) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closing_) return Closed;
        if (job.token->closed.load(std::memory_order_acquire)) return Closed;
        if (outstanding_ >= 64 || job.token->outstanding.load(std::memory_order_relaxed) >= 16)
            return Busy;
        job.token->outstanding.fetch_add(1, std::memory_order_relaxed);
        ++outstanding_;
        jobs_.push_back(std::move(job));
        cv_.notify_one();
        return Accepted;
    }
    void run() {
        for (;;) {
            Job job;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return closing_ || !jobs_.empty(); });
                if (jobs_.empty()) return;
                job = std::move(jobs_.front());
                jobs_.pop_front();
            }
            if (job.kind == External) {
                if (job.generation == job.token->value.load(std::memory_order_acquire) &&
                    !job.token->closed.load(std::memory_order_acquire)) {
                    try { job.operation(); }
                    catch (...) { /* The task publishes its own error state. */ }
                }
                std::lock_guard<std::mutex> lock(mutex_);
                job.token->outstanding.fetch_sub(1, std::memory_order_relaxed);
                --outstanding_;
                continue;
            }
            Completion completion;
            completion.token = job.token;
            completion.result.requestId = job.requestId;
            completion.result.generation = job.generation;
            if (job.generation != job.token->value.load(std::memory_order_acquire))
                completion.result.status = Stale;
            else if (job.kind == Retire) {
                job.payload.reset(); // Off audio.
                completion.result.status = Retired;
            }
            else {
                try {
                    completion.result.prepared.reset(new Reel(job.pages, job.reserve));
                    completion.result.status = Ready;
                }
                catch (...) { completion.result.status = Failed; }
                if (job.generation != job.token->value.load(std::memory_order_acquire)) {
                    completion.result.prepared.reset(); // Off audio.
                    completion.result.status = Stale;
                }
            }
            std::lock_guard<std::mutex> lock(mutex_);
            if (job.token->closed.load(std::memory_order_acquire)) {
                job.token->outstanding.fetch_sub(1, std::memory_order_relaxed);
                --outstanding_;
            }
            else results_.push_back(std::move(completion));
        }
    }

    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<Job> jobs_;
    std::deque<Completion> results_;
    std::thread workers_[2];
    std::uint32_t workerCount_ = 0;
    bool closing_;
    std::uint32_t outstanding_;
};

} // namespace chimera
