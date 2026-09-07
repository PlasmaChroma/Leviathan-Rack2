#include "../src/SpscLatestSnapshot.hpp"
#include "../src/TemporalDeckExpanderProtocol.hpp"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <thread>

namespace {
thread_local bool trackHeap = false;
thread_local unsigned allocations = 0;
thread_local unsigned releases = 0;
}

void* operator new(std::size_t bytes) {
  if (trackHeap) ++allocations;
  if (void* p = std::malloc(bytes ? bytes : 1)) return p;
  throw std::bad_alloc();
}
void* operator new[](std::size_t bytes) { return ::operator new(bytes); }
void operator delete(void* p) noexcept {
  if (trackHeap && p) ++releases;
  std::free(p);
}
void operator delete[](void* p) noexcept { ::operator delete(p); }

namespace {
using Message = temporaldeck_expander::HostToDisplay;
using Mailbox = snapshot_transport::SpscLatestSnapshot<Message>;

void require(bool condition, const char* name) {
  if (!condition) {
    std::fprintf(stderr, "FAIL: %s\n", name);
    std::abort();
  }
}

void fill(Message& msg, uint64_t sequence) {
  msg.publishSeq = sequence;
  msg.bufferGeneration = sequence ^ 0xabcdefu;
  msg.flags = unsigned(sequence & 0xffu);
  msg.sampleRate = 48000.f;
  msg.lagSamples = float(sequence);
  msg.scopeBinCount = temporaldeck_expander::SCOPE_BIN_COUNT;
  for (unsigned i = 0; i < msg.scopeBinCount; ++i) {
    const int16_t value = int16_t((sequence + i) % 16000u);
    msg.scope[i].min = -value;
    msg.scope[i].max = value;
    msg.scopeRight[i].min = -value - 1;
    msg.scopeRight[i].max = value + 1;
  }
}

bool matches(const Message& msg, uint64_t sequence) {
  if (msg.magic != temporaldeck_expander::MAGIC || msg.version != temporaldeck_expander::VERSION
      || msg.size != sizeof(Message) || msg.publishSeq != sequence
      || msg.bufferGeneration != (sequence ^ 0xabcdefu) || msg.flags != (sequence & 0xffu)
      || msg.sampleRate != 48000.f || msg.lagSamples != float(sequence)
      || msg.scopeBinCount != temporaldeck_expander::SCOPE_BIN_COUNT) return false;
  for (unsigned i = 0; i < msg.scopeBinCount; ++i) {
    const int16_t value = int16_t((sequence + i) % 16000u);
    if (msg.scope[i].min != -value || msg.scope[i].max != value
        || msg.scopeRight[i].min != -value - 1 || msg.scopeRight[i].max != value + 1) return false;
  }
  return true;
}

void testInitialAndLatest() {
  Mailbox mailbox;
  const auto& initial = mailbox.readLatest();
  require(initial.magic == temporaldeck_expander::MAGIC && initial.version == temporaldeck_expander::VERSION
      && initial.size == sizeof(Message) && initial.publishSeq == 0 && initial.scopeBinCount == 0,
      "initial TD.Scope snapshot retains its valid empty protocol payload");
  Message msg;
  for (uint64_t seq = 1; seq <= 100; ++seq) {
    fill(msg, seq);
    mailbox.publish(msg);
  }
  require(matches(mailbox.readLatest(), 100), "slow UI receives latest publication");
  for (int i = 0; i < 100; ++i)
    require(matches(mailbox.readLatest(), 100), "repeated reads retain the last snapshot");
}

void testPausedReader() {
  Mailbox mailbox;
  Message msg;
  fill(msg, 1);
  mailbox.publish(msg);
  const Message& held = mailbox.readLatest();
  // Simulate a UI preempted midway through copying: keep reading its slot
  // while the audio producer laps the remaining slots many thousands of times.
  const uint64_t copiedSequence = held.publishSeq;
  std::atomic<bool> done{false};
  std::thread producer([&] {
    Message next;
    trackHeap = true;
    for (uint64_t seq = 2; seq <= 25000; ++seq) {
      fill(next, seq);
      mailbox.publish(next);
    }
    trackHeap = false;
    require(allocations == 0 && releases == 0, "audio publication performs no heap operations");
    done.store(true, std::memory_order_release);
  });
  do {
    require(matches(held, copiedSequence), "held reader slot cannot be overwritten");
    std::this_thread::yield();
  } while (!done.load(std::memory_order_acquire));
  producer.join();
  require(matches(held, 1), "producer finished while the original reader slot remained held");
  require(matches(mailbox.readLatest(), 25000), "resumed UI receives latest state without a backlog");
}

void testConcurrentCopies() {
  Mailbox mailbox;
  std::atomic<bool> start{false};
  std::atomic<bool> done{false};
  std::thread producer([&] {
    while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
    Message msg;
    for (uint64_t seq = 1; seq <= 30000; ++seq) {
      fill(msg, seq);
      mailbox.publish(msg);
      if (seq % 13 == 0) std::this_thread::yield();
    }
    done.store(true, std::memory_order_release);
  });
  uint64_t previous = 0;
  unsigned copies = 0;
  start.store(true, std::memory_order_release);
  do {
    const Message copy = mailbox.readLatest();
    require(copy.publishSeq >= previous, "publication sequence never moves backwards");
    if (copy.publishSeq) require(matches(copy, copy.publishSeq), "entire stereo payload is coherent");
    previous = copy.publishSeq;
    ++copies;
    if (copies % 17 == 0) std::this_thread::yield();
  } while (!done.load(std::memory_order_acquire));
  producer.join();
  require(matches(mailbox.readLatest(), 30000), "final publication remains available");
  std::printf("Concurrent stereo payload copies: %u\n", copies);
}

void testHeapFreeLifecycle() {
  trackHeap = true;
  {
    Mailbox mailbox;
    Message msg;
    for (uint64_t seq = 1; seq <= 1000; ++seq) {
      fill(msg, seq);
      mailbox.publish(msg);
      require(matches(mailbox.readLatest(), seq), "alternating publish/read");
    }
  }
  trackHeap = false;
  require(allocations == 0 && releases == 0, "construction, exchange, reads and destruction are heap-free");
}
}

int main() {
  testInitialAndLatest();
  testPausedReader();
  testConcurrentCopies();
  testHeapFreeLifecycle();
  std::puts("SPSC latest snapshot: 4/4 passed (real TD.Scope payload; zero tracked heap operations)");
}
