#pragma once

#include "ChimeraReel.hpp"
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace chimera {

class CoreOwnership {
public:
    enum Owner { Idle = 0, Audio = 1, Maintenance = 2 };
    CoreOwnership() : owner_(Idle), heartbeatNs_(0), misses_(0) {}
    bool tryAudio() {
        int expected = Idle;
        if (owner_.compare_exchange_strong(expected, Audio, std::memory_order_acquire)) return true;
        misses_.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    void releaseAudio(std::uint64_t nowNs) {
        heartbeatNs_.store(nowNs, std::memory_order_relaxed);
        owner_.store(Idle, std::memory_order_release);
    }
    bool tryMaintenance(std::uint64_t nowNs) {
        const std::uint64_t last = heartbeatNs_.load(std::memory_order_acquire);
        if (nowNs < last || nowNs - last < 250000000ull) return false;
        int expected = Idle;
        return owner_.compare_exchange_strong(expected, Maintenance, std::memory_order_acquire);
    }
    void releaseMaintenance(std::uint64_t) {
        // Only audio progress resets the silence interval. A stopped host may
        // need successive short maintenance claims to release and recapture.
        owner_.store(Idle, std::memory_order_release);
    }
    Owner owner() const { return static_cast<Owner>(owner_.load(std::memory_order_acquire)); }
    std::uint64_t audioMisses() const { return misses_.load(std::memory_order_relaxed); }
private:
    std::atomic<int> owner_;
    std::atomic<std::uint64_t> heartbeatNs_;
    std::atomic<std::uint64_t> misses_;
};

// The service starts with one reference, lends read references to encoder and
// display consumers, and enqueues core-side release only when finish() returns
// true. No worker mutates the Reel's page table.
class SnapshotReaders {
public:
    SnapshotReaders() : references_(0) {}
    bool begin() {
        std::uint32_t empty = 0;
        return references_.compare_exchange_strong(empty, 1, std::memory_order_acq_rel);
    }
    bool tryAcquire() {
        std::uint32_t refs = references_.load(std::memory_order_acquire);
        while (refs) {
            if (references_.compare_exchange_weak(refs, refs + 1,
                    std::memory_order_acq_rel)) return true;
        }
        return false;
    }
    bool finish() {
        std::uint32_t refs = references_.load(std::memory_order_acquire);
        while (refs) {
            if (references_.compare_exchange_weak(refs, refs - 1,
                    std::memory_order_acq_rel)) return refs == 1;
        }
        return false;
    }
    std::uint32_t count() const { return references_.load(std::memory_order_acquire); }
private:
    std::atomic<std::uint32_t> references_;
};

template <class T, std::uint32_t Capacity, std::uint32_t Reserved = 0>
class SpscRing {
    static_assert(std::is_trivially_copyable<T>::value, "RT queue payload must be POD");
    static_assert(Capacity > Reserved && Capacity > 1, "queue capacity invalid");
    static_assert((Capacity & (Capacity - 1)) == 0, "queue capacity must be a power of two");
public:
    SpscRing() : head_(0), tail_(0) {}
    bool tryPush(const T& value) { return push(value, Capacity - Reserved); }
    bool tryPushCritical(const T& value) { return push(value, Capacity); }
    bool tryPop(T& value) {
        const std::uint64_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) return false;
        value = data_[tail % Capacity];
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }
    std::uint64_t size() const {
        const std::uint64_t tail = tail_.load(std::memory_order_acquire);
        return head_.load(std::memory_order_acquire) - tail;
    }
private:
    bool push(const T& value, std::uint32_t limit) {
        const std::uint64_t head = head_.load(std::memory_order_relaxed);
        if (head - tail_.load(std::memory_order_acquire) >= limit) return false;
        data_[head % Capacity] = value;
        head_.store(head + 1, std::memory_order_release);
        return true;
    }
    T data_[Capacity];
    std::atomic<std::uint64_t> head_, tail_;
};

struct AudioCommand {
    std::uint64_t moduleGeneration, requestId, expectedDocumentRevision;
    std::uint32_t kind, handle;
    Reel* prepared; // Registry-owned; producer retains lifetime through audio ack.
    std::uint64_t expectedAudioRevision; // Only checked for fenced heavy edit adoption.
};
struct AudioCompletion {
    std::uint64_t moduleGeneration, requestId;
    std::uint32_t kind, handle, status;
};
typedef SpscRing<AudioCommand, 64> ServiceToAudio;
typedef SpscRing<AudioCompletion, 128, 16> AudioToService;

// Service-owned ledger. A retired store remains charged until off-audio
// destruction; a second prepared store is rejected while that charge lives.
class StoreBudget {
public:
    enum Role { Empty, Active, Prepared, Retired };
    // Two full active/reserve stores plus their core-owned playback moments.
    static const std::uint64_t kPayloadLimit = 304ull * 1024ull * 1024ull;
    StoreBudget() : used_(0) { for (int i = 0; i < 3; ++i) entries_[i] = Entry{0, 0, Empty}; }
    bool admit(std::uint32_t handle, std::uint64_t bytes, Role role) {
        if (!handle || role == Empty || bytes > kPayloadLimit - used_) return false;
        int freeSlot = -1;
        for (int i = 0; i < 3; ++i) {
            if (entries_[i].handle == handle) return false;
            if (entries_[i].role == Empty) freeSlot = i;
        }
        if (freeSlot < 0) return false;
        entries_[freeSlot] = Entry{handle, bytes, role};
        used_ += bytes;
        return true;
    }
    bool transition(std::uint32_t handle, Role role) {
        if (role == Empty) return false;
        for (int i = 0; i < 3; ++i)
            if (entries_[i].handle == handle && entries_[i].role != Empty) {
                entries_[i].role = role; return true;
            }
        return false;
    }
    bool destroyOffAudio(std::uint32_t handle) {
        for (int i = 0; i < 3; ++i)
            if (entries_[i].handle == handle &&
                    (entries_[i].role == Prepared || entries_[i].role == Retired)) {
                used_ -= entries_[i].bytes;
                entries_[i] = Entry{0, 0, Empty};
                return true;
            }
        return false;
    }
    std::uint64_t usedBytes() const { return used_; }
    Role roleOf(std::uint32_t handle) const {
        for (int i = 0; i < 3; ++i) if (entries_[i].handle == handle) return entries_[i].role;
        return Empty;
    }
private:
    struct Entry { std::uint32_t handle; std::uint64_t bytes; Role role; };
    Entry entries_[3];
    std::uint64_t used_;
};

// Service/control side owns each allocation. The audio callback sees only a
// borrowed pointer plus handle; it never destroys a store or drops a final
// shared_ptr reference. Retirement follows an audio completion/acknowledgment.
class StoreRegistry {
public:
    StoreRegistry() {
        for (int i = 0; i < 3; ++i) {
            entries_[i].handle = 0;
            entries_[i].role = StoreBudget::Empty;
        }
    }
    bool accept(std::uint32_t handle, std::unique_ptr<Reel>& payload, StoreBudget::Role role) {
        if (!payload || !budget_.admit(handle, payload->payloadBytes(), role)) return false;
        for (int i = 0; i < 3; ++i) if (!entries_[i].handle) {
            entries_[i].handle = handle;
            entries_[i].role = role;
            entries_[i].payload = std::move(payload);
            return true;
        }
        return false;
    }
    Reel* lookup(std::uint32_t handle) const {
        for (int i = 0; i < 3; ++i)
            if (entries_[i].handle == handle) return entries_[i].payload.get();
        return 0;
    }
    bool transition(std::uint32_t handle, StoreBudget::Role role) {
        if (!budget_.transition(handle, role)) return false;
        for (int i = 0; i < 3; ++i) if (entries_[i].handle == handle) {
            entries_[i].role = role;
            return true;
        }
        return false;
    }
    // Move a retired allocation to the worker while retaining its budget
    // charge. releaseOffAudio(handle) is called only after worker completion.
    std::unique_ptr<Reel> detachRetired(std::uint32_t handle) {
        for (int i = 0; i < 3; ++i)
            if (entries_[i].handle == handle && entries_[i].role == StoreBudget::Retired)
                return std::move(entries_[i].payload);
        return std::unique_ptr<Reel>();
    }
    bool releaseOffAudio(std::uint32_t handle) {
        if (!budget_.destroyOffAudio(handle)) return false;
        for (int i = 0; i < 3; ++i) if (entries_[i].handle == handle) {
            entries_[i].payload.reset();
            entries_[i].handle = 0;
            entries_[i].role = StoreBudget::Empty;
            return true;
        }
        return false;
    }
    std::uint64_t chargedBytes() const { return budget_.usedBytes(); }
private:
    struct Entry {
        std::uint32_t handle;
        StoreBudget::Role role;
        std::unique_ptr<Reel> payload;
    };
    Entry entries_[3];
    StoreBudget budget_;
};

} // namespace chimera
