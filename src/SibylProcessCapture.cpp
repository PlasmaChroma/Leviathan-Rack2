#include "SibylProcessCapture.hpp"

#include "plugin.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <limits>
#include <sstream>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/thread_info.h>
#else
#include <sched.h>
#include <time.h>
#endif

namespace sibyl_debug {
namespace {

struct ProcessRecord {
  uint64_t frame = 0;
  uint64_t processNs = 0;
  uint64_t wallNs = 0;
  uint64_t threadCpuNs = 0;
  uint64_t threadId = 0;
  int32_t processorStart = -1;
  int32_t processorEnd = -1;
  uint32_t workFlags = 0;
  uint16_t activeTracks = 0;
  uint16_t evaluatedEvents = 0;
  uint16_t firedEvents = 0;
};

uint64_t currentThreadCpuNs() noexcept {
#if defined(_WIN32)
  FILETIME createTime, exitTime, kernelTime, userTime;
  if (!GetThreadTimes(GetCurrentThread(), &createTime, &exitTime, &kernelTime, &userTime)) return 0;
  ULARGE_INTEGER kernel, user;
  kernel.LowPart = kernelTime.dwLowDateTime;
  kernel.HighPart = kernelTime.dwHighDateTime;
  user.LowPart = userTime.dwLowDateTime;
  user.HighPart = userTime.dwHighDateTime;
  return (kernel.QuadPart + user.QuadPart) * 100u;
#elif defined(__APPLE__)
  thread_basic_info_data_t info {};
  mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;
  if (thread_info(mach_thread_self(), THREAD_BASIC_INFO,
                  reinterpret_cast<thread_info_t>(&info), &count) != KERN_SUCCESS) return 0;
  const uint64_t userNs = uint64_t(info.user_time.seconds) * 1000000000ull
    + uint64_t(info.user_time.microseconds) * 1000ull;
  const uint64_t systemNs = uint64_t(info.system_time.seconds) * 1000000000ull
    + uint64_t(info.system_time.microseconds) * 1000ull;
  return userNs + systemNs;
#else
  timespec ts {};
  if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts) != 0) return 0;
  return uint64_t(ts.tv_sec) * 1000000000ull + uint64_t(ts.tv_nsec);
#endif
}

uint64_t steadyNowNs() noexcept {
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::steady_clock::now().time_since_epoch()).count());
}

int32_t currentProcessorId() noexcept {
#if defined(_WIN32)
  return static_cast<int32_t>(GetCurrentProcessorNumber());
#elif defined(__linux__)
  return static_cast<int32_t>(sched_getcpu());
#else
  return -1;
#endif
}

uint64_t currentThreadId() noexcept {
  static thread_local const uint64_t id =
    static_cast<uint64_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
  return id;
}

std::string jsonEscape(const std::string& value) {
  std::string out;
  out.reserve(value.size() + 8u);
  for (char c : value) {
    if (c == '\\') out += "\\\\";
    else if (c == '"') out += "\\\"";
    else if (c == '\n') out += "\\n";
    else if (c == '\r') out += "\\r";
    else if (c == '\t') out += "\\t";
    else out += c;
  }
  return out;
}

uint64_t percentile(const std::vector<uint64_t>& sorted, double fraction) {
  if (sorted.empty()) return 0;
  const double position = fraction * double(sorted.size() - 1u);
  return sorted[static_cast<size_t>(std::llround(position))];
}

} // namespace

struct ProcessCapture::Session {
  std::unique_ptr<ProcessRecord[]> records;
  size_t capacity = 0;
  std::atomic<size_t> count {0};
  std::atomic<bool> complete {false};
  float sampleRate = 0.f;
  int rackThreadCount = 0;
  int64_t moduleId = -1;
  uint32_t instanceId = 0;
  int revision = 0;
  std::string path;
};

struct ProcessCapture::Summary {
  size_t samples = 0;
  size_t processOverBudget = 0;
  size_t captureOverBudget = 0;
  double meanProcessNs = 0.0;
  double meanWallNs = 0.0;
  double meanCpuNs = 0.0;
  uint64_t p50ProcessNs = 0;
  uint64_t p95ProcessNs = 0;
  uint64_t p99ProcessNs = 0;
  uint64_t p999ProcessNs = 0;
  uint64_t maxProcessNs = 0;
  uint64_t p50WallNs = 0;
  uint64_t p95WallNs = 0;
  uint64_t p99WallNs = 0;
  uint64_t p999WallNs = 0;
  uint64_t maxWallNs = 0;
  uint64_t maxCpuNs = 0;
  uint64_t maxWaitNs = 0;
};

ProcessCapture::ProcessCapture() = default;
ProcessCapture::~ProcessCapture() { shutdown(); }

bool ProcessCapture::start(double durationSec,
                           float sampleRate,
                           int rackThreadCount,
                           int64_t moduleId,
                           uint32_t instanceId,
                           int revision,
                           std::string& error) {
  poll();
  if (active_.load(std::memory_order_acquire) || owner_ || writerRunning_.load(std::memory_order_acquire)) {
    error = "a Sibyl process capture is already active";
    return false;
  }
  if (!(sampleRate > 0.f) || !std::isfinite(sampleRate)) {
    error = "sample rate is unavailable";
    return false;
  }
  durationSec = std::max(1.0, std::min(durationSec, 10.0));
  const size_t capacity = static_cast<size_t>(std::ceil(durationSec * double(sampleRate)));
  std::unique_ptr<Session> session(new Session());
  try {
    session->records.reset(new ProcessRecord[capacity]);
  } catch (...) {
    error = "could not allocate the bounded process capture buffer";
    return false;
  }
  session->capacity = capacity;
  session->sampleRate = sampleRate;
  session->rackThreadCount = std::max(1, rackThreadCount);
  session->moduleId = moduleId;
  session->instanceId = instanceId;
  session->revision = revision;

  const std::string directory = system::join(asset::user("Sibyl"), "Profiles");
  system::createDirectories(directory);
  const long long stampMs = static_cast<long long>(std::llround(system::getUnixTime() * 1000.0));
  session->path = system::join(directory,
    "sibyl_process_" + std::to_string(instanceId) + "_" + std::to_string(stampMs) + ".csv");

  {
    std::lock_guard<std::mutex> lock(statusMutex_);
    state_ = "capturing";
    path_ = session->path;
    error_.clear();
    summary_.reset();
  }
  owner_ = std::move(session);
  active_.store(owner_.get(), std::memory_order_release);
  return true;
}

ProcessCaptureToken ProcessCapture::begin() noexcept {
  ProcessCaptureToken token;
  token.session = active_.load(std::memory_order_acquire);
  if (token.session) {
    // Wall time deliberately brackets thread CPU time so wall - CPU remains a
    // meaningful approximation of time lost to pre-emption/scheduling.
    token.wallStartNs = steadyNowNs();
    token.processorStart = currentProcessorId();
    token.threadCpuStartNs = currentThreadCpuNs();
  }
  return token;
}

void ProcessCapture::finish(ProcessCaptureToken token,
                            uint64_t frame,
                            uint64_t processNs,
                            uint32_t workFlags,
                            uint16_t activeTracks,
                            uint16_t evaluatedEvents,
                            uint16_t firedEvents) noexcept {
  Session* session = static_cast<Session*>(token.session);
  if (!session || active_.load(std::memory_order_acquire) != session) return;
  const size_t index = session->count.load(std::memory_order_relaxed);
  if (index >= session->capacity) return;
  const uint64_t cpuEndNs = currentThreadCpuNs();
  const int32_t processorEnd = currentProcessorId();
  const uint64_t wallEndNs = steadyNowNs();
  ProcessRecord& record = session->records[index];
  record.frame = frame;
  record.processNs = processNs;
  record.wallNs = wallEndNs >= token.wallStartNs ? wallEndNs - token.wallStartNs : 0;
  record.threadCpuNs = cpuEndNs >= token.threadCpuStartNs ? cpuEndNs - token.threadCpuStartNs : 0;
  record.threadId = currentThreadId();
  record.processorStart = token.processorStart;
  record.processorEnd = processorEnd;
  record.workFlags = workFlags;
  record.activeTracks = activeTracks;
  record.evaluatedEvents = evaluatedEvents;
  record.firedEvents = firedEvents;
  session->count.store(index + 1u, std::memory_order_release);
  if (index + 1u >= session->capacity) {
    Session* expected = session;
    active_.compare_exchange_strong(expected, nullptr, std::memory_order_acq_rel);
    session->complete.store(true, std::memory_order_release);
  }
}

void ProcessCapture::poll() {
  if (writer_.joinable() && !writerRunning_.load(std::memory_order_acquire)) writer_.join();
  if (!owner_ || !owner_->complete.load(std::memory_order_acquire)
      || writerRunning_.load(std::memory_order_acquire)) return;
  std::unique_ptr<Session> session = std::move(owner_);
  {
    std::lock_guard<std::mutex> lock(statusMutex_);
    state_ = "writing";
  }
  writerRunning_.store(true, std::memory_order_release);
  writer_ = std::thread(&ProcessCapture::writeSession, this, std::move(session));
}

void ProcessCapture::writeSession(std::unique_ptr<Session> session) {
  const size_t count = std::min(session->count.load(std::memory_order_acquire), session->capacity);
  std::ofstream file(session->path.c_str(), std::ios::out | std::ios::trunc);
  if (!file.good()) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    state_ = "error";
    error_ = "could not open Sibyl process capture CSV";
    writerRunning_.store(false, std::memory_order_release);
    return;
  }

  const uint64_t budgetNs = static_cast<uint64_t>(std::llround(1000000000.0 / session->sampleRate));
  file << "# Sibyl process capture v2\n";
  file << "# module_id=" << session->moduleId << "\n";
  file << "# debug_instance=" << session->instanceId << "\n";
  file << "# revision=" << session->revision << "\n";
  file << "# sample_rate=" << session->sampleRate << "\n";
  file << "# rack_threads=" << session->rackThreadCount << "\n";
  file << "frame,process_ns,capture_wall_ns,thread_cpu_ns,scheduler_wait_ns,"
          "capture_overhead_ns,cpu_probe_overhead_ns,budget_ns,process_over_budget,"
          "capture_over_budget,thread_id,cpu_start,cpu_end,cpu_migrated,work_flags,"
          "event_evaluated,event_fired,scene_boundary,composition_adoption,external_clock_tick,"
          "active_tracks,evaluated_events,fired_events\n";

  std::vector<uint64_t> processValues;
  processValues.reserve(count);
  std::vector<uint64_t> wallValues;
  wallValues.reserve(count);
  uint64_t processTotal = 0;
  uint64_t wallTotal = 0;
  uint64_t cpuTotal = 0;
  std::unique_ptr<Summary> summary(new Summary());
  summary->samples = count;
  for (size_t i = 0; i < count; ++i) {
    const ProcessRecord& record = session->records[i];
    const uint64_t waitNs = record.wallNs > record.threadCpuNs ? record.wallNs - record.threadCpuNs : 0;
    const uint64_t captureOverheadNs = record.wallNs > record.processNs ? record.wallNs - record.processNs : 0;
    const uint64_t cpuProbeOverheadNs = record.threadCpuNs > record.processNs
      ? record.threadCpuNs - record.processNs : 0;
    const bool processOverBudget = record.processNs > budgetNs;
    const bool captureOverBudget = record.wallNs > budgetNs;
    processValues.push_back(record.processNs);
    wallValues.push_back(record.wallNs);
    processTotal += record.processNs;
    wallTotal += record.wallNs;
    cpuTotal += record.threadCpuNs;
    summary->processOverBudget += processOverBudget ? 1u : 0u;
    summary->captureOverBudget += captureOverBudget ? 1u : 0u;
    summary->maxProcessNs = std::max(summary->maxProcessNs, record.processNs);
    summary->maxWallNs = std::max(summary->maxWallNs, record.wallNs);
    summary->maxCpuNs = std::max(summary->maxCpuNs, record.threadCpuNs);
    summary->maxWaitNs = std::max(summary->maxWaitNs, waitNs);
    file << record.frame << ',' << record.processNs << ',' << record.wallNs << ','
         << record.threadCpuNs << ',' << waitNs << ',' << captureOverheadNs << ','
         << cpuProbeOverheadNs << ',' << budgetNs << ',' << (processOverBudget ? 1 : 0) << ','
         << (captureOverBudget ? 1 : 0) << ',' << record.threadId << ','
         << record.processorStart << ',' << record.processorEnd << ','
         << ((record.processorStart >= 0 && record.processorEnd >= 0
              && record.processorStart != record.processorEnd) ? 1 : 0) << ','
         << record.workFlags << ','
         << ((record.workFlags & PROCESS_EVENT_EVALUATED) ? 1 : 0) << ','
         << ((record.workFlags & PROCESS_EVENT_FIRED) ? 1 : 0) << ','
         << ((record.workFlags & PROCESS_SCENE_BOUNDARY) ? 1 : 0) << ','
         << ((record.workFlags & PROCESS_COMPOSITION_ADOPTION) ? 1 : 0) << ','
         << ((record.workFlags & PROCESS_EXTERNAL_CLOCK_TICK) ? 1 : 0) << ','
         << record.activeTracks << ',' << record.evaluatedEvents << ',' << record.firedEvents << '\n';
  }
  file.close();

  if (count > 0) {
    summary->meanProcessNs = double(processTotal) / double(count);
    summary->meanWallNs = double(wallTotal) / double(count);
    summary->meanCpuNs = double(cpuTotal) / double(count);
    std::sort(processValues.begin(), processValues.end());
    summary->p50ProcessNs = percentile(processValues, 0.50);
    summary->p95ProcessNs = percentile(processValues, 0.95);
    summary->p99ProcessNs = percentile(processValues, 0.99);
    summary->p999ProcessNs = percentile(processValues, 0.999);
    std::sort(wallValues.begin(), wallValues.end());
    summary->p50WallNs = percentile(wallValues, 0.50);
    summary->p95WallNs = percentile(wallValues, 0.95);
    summary->p99WallNs = percentile(wallValues, 0.99);
    summary->p999WallNs = percentile(wallValues, 0.999);
  }

  {
    std::lock_guard<std::mutex> lock(statusMutex_);
    state_ = "complete";
    error_.clear();
    summary_ = std::move(summary);
  }
  writerRunning_.store(false, std::memory_order_release);
}

std::string ProcessCapture::statusJson() {
  poll();
  std::lock_guard<std::mutex> lock(statusMutex_);
  std::ostringstream out;
  out << "{\"ok\":true,\"state\":\"" << jsonEscape(state_) << "\",\"path\":\""
      << jsonEscape(path_) << "\"";
  if (!error_.empty()) out << ",\"error\":\"" << jsonEscape(error_) << "\"";
  if (owner_) {
    out << ",\"capturedFrames\":" << owner_->count.load(std::memory_order_acquire)
        << ",\"requestedFrames\":" << owner_->capacity;
  }
  if (summary_) {
    out << std::fixed << std::setprecision(4)
        << ",\"summary\":{\"samples\":" << summary_->samples
        << ",\"overBudget\":" << summary_->processOverBudget
        << ",\"processOverBudget\":" << summary_->processOverBudget
        << ",\"captureOverBudget\":" << summary_->captureOverBudget
        << ",\"meanProcessUs\":" << summary_->meanProcessNs * 0.001
        << ",\"meanWallUs\":" << summary_->meanWallNs * 0.001
        << ",\"meanThreadCpuUs\":" << summary_->meanCpuNs * 0.001
        << ",\"p50ProcessUs\":" << summary_->p50ProcessNs * 0.001
        << ",\"p95ProcessUs\":" << summary_->p95ProcessNs * 0.001
        << ",\"p99ProcessUs\":" << summary_->p99ProcessNs * 0.001
        << ",\"p999ProcessUs\":" << summary_->p999ProcessNs * 0.001
        << ",\"maxProcessUs\":" << summary_->maxProcessNs * 0.001
        << ",\"p50WallUs\":" << summary_->p50WallNs * 0.001
        << ",\"p95WallUs\":" << summary_->p95WallNs * 0.001
        << ",\"p99WallUs\":" << summary_->p99WallNs * 0.001
        << ",\"p999WallUs\":" << summary_->p999WallNs * 0.001
        << ",\"maxWallUs\":" << summary_->maxWallNs * 0.001
        << ",\"maxThreadCpuUs\":" << summary_->maxCpuNs * 0.001
        << ",\"maxWaitUs\":" << summary_->maxWaitNs * 0.001 << '}';
  }
  out << '}';
  return out.str();
}

void ProcessCapture::shutdown() {
  active_.store(nullptr, std::memory_order_release);
  owner_.reset();
  if (writer_.joinable()) writer_.join();
}

} // namespace sibyl_debug
