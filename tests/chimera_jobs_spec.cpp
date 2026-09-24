#include "ChimeraJobs.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}

int main() {
    using namespace chimera;
    {
        CoreOwnership owner;
        need(owner.tryAudio() && !owner.tryMaintenance(300000000ull),
             "audio owns core exclusively");
        owner.releaseAudio(300000000ull);
        need(!owner.tryMaintenance(549999999ull) && owner.tryMaintenance(550000000ull),
             "maintenance waits 250 ms of silence");
        need(!owner.tryAudio() && owner.audioMisses() == 1,
             "host restart cannot enter maintenance-owned core");
        Reel stopped(1, 1);
        need(stopped.write(0, StereoFrame{1,2}, 0) && stopped.beginSnapshot(0),
             "stopped host establishes cut under maintenance owner");
        need(stopped.maintenanceTick() == 1 && stopped.readyForWorker(),
             "stopped host finishes capture without callback progress");
        need(stopped.beginRelease() && stopped.maintenanceTick() == 1,
             "stopped host can reclaim for a second cut");
        need(stopped.beginSnapshot(1) && stopped.maintenanceTick() == 1,
             "second stopped-host save progresses");
        owner.releaseMaintenance(550000000ull);
        need(owner.tryMaintenance(550000001ull),
             "stopped host can immediately continue maintenance after release");
        owner.releaseMaintenance(550000001ull);
        need(owner.tryAudio(), "audio resumes after maintenance releases token");
        owner.releaseAudio(550000001ull);
    }
    {
        SnapshotReaders readers;
        need(readers.begin() && readers.tryAcquire() && readers.tryAcquire() &&
             readers.count() == 3, "multiple consumers share one immutable cut");
        need(!readers.finish() && !readers.finish() && readers.count() == 1,
             "first two readers cannot release Reel lease");
        need(readers.finish() && !readers.tryAcquire() && !readers.finish(),
             "only final reader triggers core-side release");
    }
    {
        ServiceToAudio commands;
        AudioCommand command = {7, 1, 2, 3, 4, nullptr};
        for (int i = 0; i < 64; ++i) need(commands.tryPush(command), "64 command capacity");
        need(!commands.tryPush(command), "busy command queue rejects overflow");
        AudioCommand out = {};
        for (int i = 0; i < 64; ++i)
            need(commands.tryPop(out) && out.moduleGeneration == 7, "ordered command drain");
        AudioToService completions;
        AudioCompletion completion = {7, 1, 0, 0, 0};
        for (int i = 0; i < 112; ++i)
            need(completions.tryPush(completion), "normal completions leave reserve");
        need(!completions.tryPush(completion), "normal result cannot consume retirement reserve");
        for (int i = 0; i < 16; ++i)
            need(completions.tryPushCritical(completion), "retirement/completion reserve available");
        need(!completions.tryPushCritical(completion), "critical queue has hard bound");
    }
    {
        SpscRing<AudioCommand, 64> queue;
        std::atomic<bool> ordered(true);
        std::thread producer([&] {
            for (std::uint64_t i = 1; i <= 100000; ++i) {
                AudioCommand item = {7, i, 0, 0, 0, nullptr};
                while (!queue.tryPush(item)) std::this_thread::yield();
            }
        });
        std::thread consumer([&] {
            for (std::uint64_t i = 1; i <= 100000; ++i) {
                AudioCommand item = {};
                while (!queue.tryPop(item)) std::this_thread::yield();
                if (item.requestId != i) ordered.store(false);
            }
        });
        producer.join(); consumer.join();
        need(ordered.load() && queue.size() == 0,
             "concurrent SPSC handoff preserves 100,000 commands in order");
    }
    {
        StoreBudget budget;
        const std::uint64_t full = 133632000ull;
        need(budget.admit(1, full, StoreBudget::Active) &&
             budget.admit(2, full, StoreBudget::Prepared), "active plus prepared fit 256 MiB");
        need(budget.transition(1, StoreBudget::Retired) &&
             budget.transition(2, StoreBudget::Active), "old leased store stays charged");
        need(!budget.admit(3, full, StoreBudget::Prepared),
             "retired lease blocks third full store");
        need(budget.destroyOffAudio(1) && budget.admit(3, full, StoreBudget::Prepared),
             "off-audio retirement releases payload credit");
    }
    {
        IoService service(2);
        std::shared_ptr<JobGeneration> token(new JobGeneration);
        for (int i = 0; i < 16; ++i)
            need(service.prepare(token, i+1, 1, 1) == IoService::Accepted,
                 "bounded per-module preparation accepted");
        need(service.prepare(token, 17, 1, 1) == IoService::Busy,
             "17th unresolved module job rejected busy");
        token->invalidate(); // Module removed; workers never hold its pointer.
        service.shutdown();
        need(service.outstanding() == 16, "shutdown drains jobs into completion queue");
        for (int i = 0; i < 16; ++i) {
            IoService::Result result;
            need(service.poll(result) && result.status == IoService::Stale &&
                 !result.prepared, "removed generation discards prepared payload");
        }
        need(service.outstanding() == 0 && token->outstanding.load() == 0,
             "all stale job credits returned");
    }
    {
        IoService service(1);
        std::shared_ptr<JobGeneration> token(new JobGeneration);
        need(service.prepare(token, 21, 2, 2) == IoService::Accepted,
             "prepare store off audio");
        IoService::Result result;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (!service.poll(result) && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        need(result.status == IoService::Ready && result.prepared &&
             result.prepared->capacityFrames() == 512,
             "worker returns prepared store without a Module pointer");
        StoreRegistry registry;
        need(registry.accept(101, result.prepared, StoreBudget::Prepared) &&
             !result.prepared && registry.lookup(101) &&
             registry.chargedBytes() == 4ull * 2048ull,
             "prepared store adopted by service-owned handle registry");
        need(registry.transition(101, StoreBudget::Active) &&
             !registry.releaseOffAudio(101), "active borrowed audio store cannot be destroyed");
        need(registry.transition(101, StoreBudget::Retired),
             "audio acknowledgment marks prior store retired");
        std::unique_ptr<Reel> retiring(new Reel(2, 2));
        need(service.retire(token, 22, retiring) == IoService::Accepted && !retiring,
             "retirement sent to worker");
        IoService::Result retired;
        while (!service.poll(retired) && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        need(retired.status == IoService::Retired && !retired.prepared,
             "worker destroys retired store off audio");
        need(registry.releaseOffAudio(101) && registry.chargedBytes() == 0,
             "retired handle releases payload credit only after off-audio destruction");
        service.shutdown();
    }
    {
        IoService service(1);
        std::shared_ptr<JobGeneration> first(new JobGeneration);
        std::shared_ptr<JobGeneration> second(new JobGeneration);
        need(service.prepare(first, 31, 2, 2) == IoService::Accepted &&
             service.prepare(second, 32, 2, 2) == IoService::Accepted,
             "two modules can submit to one worker");
        IoService::Result result;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (!service.pollFor(second, result) && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        need(result.status == IoService::Ready && result.requestId == 32 && result.prepared,
             "module-specific poll never steals first module's result");
        need(service.pollFor(first, result) && result.requestId == 31 && result.prepared,
             "first module still receives its result");
        for (int i = 0; i < 8; ++i)
            need(service.prepare(first, 40+i, 2, 2) == IoService::Accepted,
                 "queued module jobs accepted before removal");
        service.cancel(first);
        need(service.prepare(first, 99, 2, 2) == IoService::Closed,
             "removed module token cannot resubmit");
        service.shutdown();
        need(first->outstanding.load() == 0 && second->outstanding.load() == 0 &&
             service.outstanding() == 0 && !service.pollFor(first, result),
             "module removal retires queued, running, and completed work");
    }
    std::puts("PASS: Chimera core ownership, bounded queues/budget, stale jobs, worker preparation/retirement");
}
