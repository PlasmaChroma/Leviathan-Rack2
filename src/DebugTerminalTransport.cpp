#include "DebugTerminalTransport.hpp"
#include "plugin.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace debug_terminal {
namespace {

static constexpr int kDefaultPort = 8765;
static constexpr int kReconnectIntervalMs = 1000;
static constexpr int kPublishIntervalMs = 125;
static constexpr double kSnapshotStaleSec = 2.0;
static constexpr double kSchemaSubmitIntervalSec = 5.0;

struct Snapshot {
  std::string module;
  std::string instance;
  std::string stream;
  std::string kind;
  std::string dataJson;
  double ts = 0.0;
};

static std::string jsonEscape(const std::string &input) {
  std::string out;
  out.reserve(input.size() + 8u);
  for (char c : input) {
    switch (c) {
      case '\\':
        out += "\\\\";
        break;
      case '"':
        out += "\\\"";
        break;
      case '\n':
        out += "\\n";
        break;
      case '\r':
        out += "\\r";
        break;
      case '\t':
        out += "\\t";
        break;
      default:
        out += c;
        break;
    }
  }
  return out;
}

class Transport {
public:
  Transport() { startWorker(); }
  ~Transport() {
    stop_.store(true, std::memory_order_relaxed);
    closeSocket();
    if (worker_.joinable()) {
      worker_.join();
    }
#ifdef _WIN32
    if (wsaStarted_) {
      WSACleanup();
    }
#endif
  }

  void submit(const char *module,
              uint32_t instanceId,
              const char *stream,
              const char *kind,
              const std::string &dataJson,
              double ts) {
    if (!isEnabled()) {
      return;
    }

    Snapshot snap;
    snap.module = module ? module : "";
    char instanceBuf[32];
    std::snprintf(instanceBuf, sizeof(instanceBuf), "%u", instanceId);
    snap.instance = instanceBuf;
    snap.stream = stream ? stream : "";
    snap.kind = kind ? kind : "";
    snap.dataJson = dataJson;
    snap.ts = ts;

    std::lock_guard<std::mutex> lock(mutex_);
    snapshots_[makeKey(snap.module, snap.instance, snap.stream, snap.kind)] = snap;
  }

private:
  std::unordered_map<std::string, Snapshot> snapshots_;
  std::mutex mutex_;
  std::thread worker_;
  std::atomic<bool> stop_ {false};
  std::string host_;
  int port_ = 0;
  bool configLoaded_ = false;
#ifdef _WIN32
  SOCKET sock_ = INVALID_SOCKET;
  bool wsaStarted_ = false;
#else
  int sock_ = -1;
#endif
  std::chrono::steady_clock::time_point lastConnectAttempt_;

  static std::string makeKey(const std::string &module,
                             const std::string &instance,
                             const std::string &stream,
                             const std::string &kind) {
    return module + "|" + instance + "|" + stream + "|" + kind;
  }

  void startWorker() { worker_ = std::thread([this]() { workerLoop(); }); }

  bool isEnabled() {
    refreshConfig();
    return isDragonKingDebugEnabled() && !host_.empty() && port_ > 0;
  }

  void refreshConfig() {
    if (configLoaded_) {
      return;
    }
    const char *hostEnv = std::getenv("LEVIATHAN_DEBUG_HOST");
    const char *portEnv = std::getenv("LEVIATHAN_DEBUG_PORT");
    host_ = (hostEnv && hostEnv[0]) ? hostEnv : "127.0.0.1";
    port_ = (portEnv && portEnv[0]) ? std::atoi(portEnv) : kDefaultPort;
    if (port_ <= 0) {
      host_.clear();
      port_ = 0;
    }
    configLoaded_ = true;
  }

  void workerLoop() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
      wsaStarted_ = true;
    }
#endif
    lastConnectAttempt_ = std::chrono::steady_clock::now() - std::chrono::milliseconds(kReconnectIntervalMs);

    while (!stop_.load(std::memory_order_relaxed)) {
      if (!isEnabled()) {
        closeSocket();
        sleepMs(kReconnectIntervalMs);
        continue;
      }
      if (!ensureConnected()) {
        sleepMs(100);
        continue;
      }

      std::vector<Snapshot> snapshots = collectFreshSnapshots(system::getTime());
      for (const Snapshot &snap : snapshots) {
        if (!sendSnapshot(snap)) {
          closeSocket();
          break;
        }
      }
      sleepMs(kPublishIntervalMs);
    }
  }

  std::vector<Snapshot> collectFreshSnapshots(double nowSec) {
    std::vector<Snapshot> out;
    std::lock_guard<std::mutex> lock(mutex_);
    if (snapshots_.empty()) {
      return out;
    }
    out.reserve(snapshots_.size());
    for (auto it = snapshots_.begin(); it != snapshots_.end();) {
      const Snapshot &snap = it->second;
      const bool stale = (nowSec - snap.ts) > kSnapshotStaleSec;
      if (stale) {
        it = snapshots_.erase(it);
        continue;
      }
      out.push_back(snap);
      ++it;
    }
    return out;
  }

  bool ensureConnected() {
    if (socketValid()) {
      return true;
    }
    auto now = std::chrono::steady_clock::now();
    if (now - lastConnectAttempt_ < std::chrono::milliseconds(kReconnectIntervalMs)) {
      return false;
    }
    lastConnectAttempt_ = now;

    closeSocket();
    if (host_.empty() || port_ <= 0) {
      return false;
    }

    int sock = int(::socket(AF_INET, SOCK_STREAM, 0));
#ifdef _WIN32
    if (sock == int(INVALID_SOCKET)) {
      return false;
    }
#else
    if (sock < 0) {
      return false;
    }
#endif

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(uint16_t(port_));
    if (::inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1) {
      closeNativeSocket(sock);
      return false;
    }
    if (::connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) {
      closeNativeSocket(sock);
      return false;
    }
    assignSocket(sock);
    return true;
  }

  bool sendSnapshot(const Snapshot &snap) {
    if (!socketValid()) {
      return false;
    }
    std::string line =
      std::string("{\"plugin\":\"Leviathan\",\"module\":\"") + jsonEscape(snap.module) +
      "\",\"instance\":\"" + jsonEscape(snap.instance) +
      "\",\"stream\":\"" + jsonEscape(snap.stream) +
      "\",\"kind\":\"" + jsonEscape(snap.kind) +
      "\",\"ts\":" + formatFloat(snap.ts) +
      ",\"data\":" + snap.dataJson + "}\n";
    return sendAll(line.c_str(), line.size());
  }

  static std::string formatFloat(double value) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.6f", value);
    return buf;
  }

  bool sendAll(const char *data, size_t size) {
    while (size > 0u) {
#ifdef _WIN32
      int sent = ::send(sock_, data, int(size), 0);
#else
      ssize_t sent = ::send(sock_, data, size, 0);
#endif
      if (sent <= 0) {
        return false;
      }
      data += sent;
      size -= size_t(sent);
    }
    return true;
  }

  void sleepMs(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

  bool socketValid() const {
#ifdef _WIN32
    return sock_ != INVALID_SOCKET;
#else
    return sock_ >= 0;
#endif
  }

  void closeSocket() {
    if (!socketValid()) {
      return;
    }
#ifdef _WIN32
    ::shutdown(sock_, SD_BOTH);
    ::closesocket(sock_);
    sock_ = INVALID_SOCKET;
#else
    ::shutdown(sock_, SHUT_RDWR);
    ::close(sock_);
    sock_ = -1;
#endif
  }

  void closeNativeSocket(int sock) {
#ifdef _WIN32
    ::closesocket(SOCKET(sock));
#else
    ::close(sock);
#endif
  }

  void assignSocket(int sock) {
#ifdef _WIN32
    sock_ = SOCKET(sock);
#else
    sock_ = sock;
#endif
  }
};

static Transport &transport() {
  // Debug-only transport: avoid process-shutdown stalls by intentionally
  // keeping the singleton alive until OS teardown instead of running a static
  // destructor that may block on socket/thread cleanup during Rack exit.
  static Transport *gTransport = new Transport();
  return *gTransport;
}

static bool shouldSubmitSchema(const char *module, const char *stream) {
  static std::mutex schemaMutex;
  static std::unordered_map<std::string, double> lastSubmitSec;
  const double nowSec = system::getTime();
  const std::string key = std::string(module ? module : "") + "|" + (stream ? stream : "");

  std::lock_guard<std::mutex> lock(schemaMutex);
  double &last = lastSubmitSec[key];
  if (last > 0.0 && (nowSec - last) < kSchemaSubmitIntervalSec) {
    return false;
  }
  last = nowSec;
  return true;
}

static void submitUiMetricSchema(const char *module, const char *columnsJson) {
  if (!shouldSubmitSchema(module, "ui")) {
    return;
  }
  char dataBuf[1024];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{\"schema\":1,\"target_kind\":\"metric\",\"columns\":%s}",
                columnsJson ? columnsJson : "[]");
  transport().submit(module, 0u, "ui", "schema", dataBuf, system::getTime());
}

static void appendRange(char *buf, size_t size, const char *key, TimingRangeUs range) {
  const size_t len = std::strlen(buf);
  if (len >= size) {
    return;
  }
  std::snprintf(buf + len,
                size - len,
                "\"%s\":\"%.2f-%.2f\"",
                key ? key : "",
                std::max(0.f, range.min),
                std::max(0.f, range.max));
}

} // namespace

void submitTDScopeUiMetrics(uint32_t instanceId,
                            TimingRangeUs processUs,
                            TimingRangeUs stepUs,
                            TimingRangeUs drawUs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t publishSeq,
                            uint64_t drawSeq,
                            uint64_t drawCalls) {
  submitUiMetricSchema("TDScope",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"rows\",\"label\":\"Rows\"},{\"key\":\"density_pct\",\"label\":\"Density%\"},{\"key\":\"zoom\",\"label\":\"Zoom\"},{\"key\":\"thickness\",\"label\":\"Thickness\"},{\"key\":\"publish_seq\",\"label\":\"Publish\"},{\"key\":\"draw_seq\",\"label\":\"Draw Seq\"},{\"key\":\"draw_calls\",\"label\":\"Calls\"}]");
  char dataBuf[320];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"rows\":%d,\"density_pct\":%.2f,\"zoom\":%.4f,\"thickness\":%.4f,\"publish_seq\":%llu,\"draw_seq\":%llu,\"draw_calls\":%llu}",
                std::max(0, rows),
                std::max(0.f, densityPct),
                std::max(0.f, zoom),
                std::max(0.f, thickness),
                (unsigned long long) publishSeq,
                (unsigned long long) drawSeq,
                (unsigned long long) drawCalls);
  double ts = system::getTime();
  transport().submit("TDScope", instanceId, "ui", "metric", dataBuf, ts);
}

void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 TimingRangeUs processUs,
                                 TimingRangeUs stepUs,
                                 TimingRangeUs drawUs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid) {
  submitUiMetricSchema("TemporalDeck",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"scope_preview_us\",\"label\":\"Scope (us)\"},{\"key\":\"scope_stride\",\"label\":\"Stride\"},{\"key\":\"scope_metric_valid\",\"label\":\"Scope OK\"}]");
  char dataBuf[320];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"scope_preview_us\":%.4f,\"scope_stride\":%d,\"scope_metric_valid\":%d}",
                std::max(0.f, scopePreviewUs),
                std::max(0, scopeStride),
                scopeMetricValid ? 1 : 0);
  double ts = system::getTime();
  transport().submit("TemporalDeck", instanceId, "ui", "metric", dataBuf, ts);
}

void submitBifurxUiMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs,
                           float uiLocalPrepUs,
                           bool renderOpengl,
                           float curvePrepUs,
                           float overlayPrepUs,
                           int visualWorkerMode,
                           float visualWorkerAgeMs,
                           float visualWorkerQueueMs) {
  submitUiMetricSchema("Bifurx",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"ui_local_prep_us\",\"label\":\"Prep (us)\"},{\"key\":\"opengl\",\"label\":\"GL\"},{\"key\":\"vw_mode\",\"label\":\"VW\"},{\"key\":\"vw_age_ms\",\"label\":\"VW age (ms)\"},{\"key\":\"vw_queue_ms\",\"label\":\"VW q (ms)\"},{\"key\":\"curve_prep_us\",\"label\":\"Curve (us)\"},{\"key\":\"overlay_prep_us\",\"label\":\"Overlay (us)\"}]");
  char dataBuf[512];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"ui_local_prep_us\":%.3f,\"opengl\":%d,\"curve_prep_us\":%.3f,\"overlay_prep_us\":%.3f,\"vw_mode\":%d,\"vw_age_ms\":%.3f,\"vw_queue_ms\":%.3f}",
                std::max(0.f, uiLocalPrepUs),
                renderOpengl ? 1 : 0,
                std::max(0.f, curvePrepUs),
                std::max(0.f, overlayPrepUs),
                visualWorkerMode,
                std::max(0.f, visualWorkerAgeMs),
                std::max(0.f, visualWorkerQueueMs));
  double ts = system::getTime();
  transport().submit("Bifurx", instanceId, "ui", "metric", dataBuf, ts);
}

void submitWyrmMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs,
                       float editorDrawUs,
                       float sandUpdateUs,
                       float sandDrawUs,
                       float sandGlUs,
                       int channels,
                       int bodySamples,
                       uint64_t bodySampleCacheHits,
                       uint64_t bodySampleCacheMisses) {
  submitUiMetricSchema("Wyrm",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"ed_us\",\"label\":\"Ed (us)\"},{\"key\":\"sand_up_us\",\"label\":\"SUp (us)\"},{\"key\":\"sand_dr_us\",\"label\":\"SDr (us)\"},{\"key\":\"sand_gl_us\",\"label\":\"SGL (us)\"},{\"key\":\"ch\",\"label\":\"Ch\"},{\"key\":\"body\",\"label\":\"Body\"},{\"key\":\"body_cache_hit\",\"label\":\"BHit\"},{\"key\":\"body_cache_miss\",\"label\":\"BMiss\"}]");
  char dataBuf[512];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"ed_us\":%.3f,\"sand_up_us\":%.3f,\"sand_dr_us\":%.3f,\"sand_gl_us\":%.3f,\"ch\":%d,\"body\":%d,\"body_cache_hit\":%llu,\"body_cache_miss\":%llu}",
                std::max(0.f, editorDrawUs),
                std::max(0.f, sandUpdateUs),
                std::max(0.f, sandDrawUs),
                std::max(0.f, sandGlUs),
                std::max(0, channels),
                std::max(0, bodySamples),
                static_cast<unsigned long long>(bodySampleCacheHits),
                static_cast<unsigned long long>(bodySampleCacheMisses));
  double ts = system::getTime();
  transport().submit("Wyrm", instanceId, "ui", "metric", dataBuf, ts);
}

void submitIntegralFluxMetrics(uint32_t instanceId,
                               TimingRangeUs processUs,
                               TimingRangeUs stepUs,
                               TimingRangeUs drawUs,
                               TimingRangeUs apertureUs,
                               float gearUs,
                               float eclipseUs,
                               float ch1CurvePointsReducedAvg,
                               float ch1TracerExtraPointsReducedAvg) {
  submitUiMetricSchema("IntegralFlux",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"aperture_us\",\"label\":\"Aperture (us)\"},{\"key\":\"gear_us\",\"label\":\"Halo2 (us)\"},{\"key\":\"eclipse_us\",\"label\":\"E2 (us)\"},{\"key\":\"ch1_curve_reduced_avg\",\"label\":\"C1 Red\"},{\"key\":\"ch1_tracer_extra_reduced_avg\",\"label\":\"C1 Tr+\"}]");
  char dataBuf[640];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "aperture_us", apertureUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"gear_us\":%.3f,\"eclipse_us\":%.3f,\"ch1_curve_reduced_avg\":%.3f,\"ch1_tracer_extra_reduced_avg\":%.3f}",
                std::max(0.f, gearUs),
                std::max(0.f, eclipseUs),
                std::max(0.f, ch1CurvePointsReducedAvg),
                std::max(0.f, ch1TracerExtraPointsReducedAvg));
  double ts = system::getTime();
  transport().submit("IntegralFlux", instanceId, "ui", "metric", dataBuf, ts);
}

void submitProcMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs) {
  submitUiMetricSchema("Proc",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"}]");
  char dataBuf[160];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                "}");
  double ts = system::getTime();
  transport().submit("Proc", instanceId, "ui", "metric", dataBuf, ts);
}

void submitUndertowMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs) {
  submitUiMetricSchema("Undertow",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"}]");
  char dataBuf[160];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                "}");
  double ts = system::getTime();
  transport().submit("Undertow", instanceId, "ui", "metric", dataBuf, ts);
}

} // namespace debug_terminal
