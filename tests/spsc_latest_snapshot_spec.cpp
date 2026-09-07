#include "../src/SpscLatestSnapshot.hpp"
#include "../src/TemporalDeckExpanderProtocol.hpp"
#include "../src/SilSpectrumSnapshot.hpp"

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

void fillSpectrumRings(float* mid, float* side, uint32_t sequence) {
  for (int i = 0; i < sil::SpectrumSnapshot::kFftSize; ++i) {
    mid[i] = float(sequence) + float(i) / 4096.f;
    side[i] = -mid[i];
  }
}

bool spectrumMatches(const sil::SpectrumSnapshot& snapshot, uint32_t sequence, int writePosition) {
  if (snapshot.sequence != sequence) return false;
  for (int i = 0; i < sil::SpectrumSnapshot::kFftSize; ++i) {
    const int index = (writePosition + i) % sil::SpectrumSnapshot::kFftSize;
    const float expected = float(sequence) + float(index) / 4096.f;
    if (snapshot.mid[i] != expected || snapshot.side[i] != -expected) return false;
  }
  return true;
}

void testSilChronology() {
  sil::SpectrumSnapshot snapshot;
  float mid[sil::SpectrumSnapshot::kFftSize], side[sil::SpectrumSnapshot::kFftSize];
  fillSpectrumRings(mid, side, 17);
  for (int position = 0; position < sil::SpectrumSnapshot::kFftSize; ++position) {
    snapshot.capture(mid, side, position, 17);
    require(spectrumMatches(snapshot, 17, position), "Sil preserves both rings at every wrap position");
  }
}

void testSilConcurrentCapture() {
  using Spectrum = sil::SpectrumSnapshot;
  snapshot_transport::SpscLatestSnapshot<Spectrum> mailbox;
  float mid[Spectrum::kFftSize], side[Spectrum::kFftSize];
  fillSpectrumRings(mid, side, 1);
  mailbox.publishWith([&](Spectrum& snapshot) { snapshot.capture(mid, side, 1, 1); });
  const Spectrum& held = mailbox.readLatest();
  std::atomic<uint32_t> published{1};
  std::thread producer([&] {
    float audioMid[Spectrum::kFftSize], audioSide[Spectrum::kFftSize];
    trackHeap = true;
    for (uint32_t seq = 2; seq <= 10000; ++seq) {
      fillSpectrumRings(audioMid, audioSide, seq);
      mailbox.publishWith([&](Spectrum& snapshot) {
        snapshot.capture(audioMid, audioSide, int(seq % Spectrum::kFftSize), seq);
      });
      published.store(seq, std::memory_order_release);
    }
    trackHeap = false;
    require(allocations == 0 && releases == 0, "Sil direct capture does not allocate or free");
  });
  do {
    require(spectrumMatches(held, 1, 1), "Sil reader retains its old spectrum while audio advances");
    std::this_thread::yield();
  } while (published.load(std::memory_order_acquire) < 1000);
  uint32_t last = 1;
  do {
    const Spectrum& snapshot = mailbox.readLatest();
    require(snapshot.sequence >= last, "Sil spectrum sequence does not regress");
    require(spectrumMatches(snapshot, snapshot.sequence, int(snapshot.sequence % Spectrum::kFftSize)),
        "Sil samples and sequence stay coherent throughout a read");
    last = snapshot.sequence;
  } while (published.load(std::memory_order_acquire) != 10000);
  producer.join();
  require(spectrumMatches(mailbox.readLatest(), 10000, 10000 % Spectrum::kFftSize),
      "Sil consumer receives final capture");
}
}

int main() {
  testInitialAndLatest();
  testPausedReader();
  testConcurrentCopies();
  testHeapFreeLifecycle();
  testSilChronology();
  testSilConcurrentCapture();
  std::puts("SPSC latest snapshot: 6/6 passed (TD.Scope and Sil payloads; zero tracked heap operations)");
}
