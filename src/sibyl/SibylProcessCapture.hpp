#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace sibyl_debug {

enum ProcessWorkFlags : uint32_t {
  PROCESS_EVENT_EVALUATED = 1u << 0,
  PROCESS_EVENT_FIRED = 1u << 1,
  PROCESS_SCENE_BOUNDARY = 1u << 2,
  PROCESS_COMPOSITION_ADOPTION = 1u << 3,
  PROCESS_EXTERNAL_CLOCK_TICK = 1u << 4,
};

struct ProcessCaptureToken {
  void* session = nullptr;
  uint64_t wallStartNs = 0;
  uint64_t threadCpuStartNs = 0;
  int32_t processorStart = -1;
};

class ProcessCapture {
public:
  ProcessCapture();
  ~ProcessCapture();

  bool start(double durationSec,
             float sampleRate,
             int rackThreadCount,
             int64_t moduleId,
             uint32_t instanceId,
             int revision,
             std::string& error);
  ProcessCaptureToken begin() noexcept;
  void finish(ProcessCaptureToken token,
              uint64_t frame,
              uint64_t processNs,
              uint32_t workFlags,
              uint16_t activeTracks,
              uint16_t evaluatedEvents,
              uint16_t firedEvents) noexcept;
  std::string statusJson();
  void poll();
  void shutdown();

private:
  struct Session;
  struct Summary;

  std::atomic<Session*> active_ {nullptr};
  std::unique_ptr<Session> owner_;
  std::thread writer_;
  std::atomic<bool> writerRunning_ {false};
  std::mutex statusMutex_;
  std::string state_ {"idle"};
  std::string path_;
  std::string error_;
  std::unique_ptr<Summary> summary_;

  void writeSession(std::unique_ptr<Session> session);
};

} // namespace sibyl_debug
