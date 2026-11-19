#include "DebugTerminalTransport.hpp"
#include "../plugin.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
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
#include <cerrno>
#include <fcntl.h>
#include <poll.h>
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
  int64_t rackModuleId = -1;
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
  Transport() {
    const char *host = std::getenv("LEVIATHAN_DEBUG_HOST");
    const char *port = std::getenv("LEVIATHAN_DEBUG_PORT");
    host_ = host && *host ? host : "127.0.0.1";
    port_ = port && *port ? std::atoi(port) : kDefaultPort;
  }

  void stop() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      stop_.store(true);
    }
    wake_.notify_all();
    if (worker_.joinable()) worker_.join();
  }

  void submit(const char *module,
              uint32_t instanceId,
              const char *stream,
              const char *kind,
              const std::string &dataJson,
              double ts,
              int64_t rackModuleId = -1) {
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
    snap.rackModuleId = rackModuleId;
    snap.ts = ts;

    std::lock_guard<std::mutex> lock(mutex_);
    if (stop_.load()) return;
    if (!worker_.joinable()) worker_ = std::thread(&Transport::workerLoop, this);
    snapshots_[makeKey(snap.module, snap.instance, snap.stream, snap.kind)] = snap;
  }

  std::string latestMetricsJson(int64_t rackModuleId) {
    const double nowSec = system::getTime();
    std::string out = "{\"debugEnabled\":";
    out += isDragonKingDebugEnabled() ? "true" : "false";
    out += ",\"moduleId\":" + std::to_string(rackModuleId) + ",\"metrics\":[";

    bool first = true;
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &entry : snapshots_) {
      const Snapshot &snap = entry.second;
      if (snap.kind != "metric" || snap.rackModuleId != rackModuleId
          || (nowSec - snap.ts) > kSnapshotStaleSec) {
        continue;
      }
      if (!first) {
        out += ',';
      }
      first = false;
      out += "{\"module\":\"" + jsonEscape(snap.module)
          + "\",\"instance\":\"" + jsonEscape(snap.instance)
          + "\",\"stream\":\"" + jsonEscape(snap.stream)
          + "\",\"ts\":" + formatFloat(snap.ts)
          + ",\"ageMs\":" + formatFloat(std::max(0.0, (nowSec - snap.ts) * 1000.0))
          + ",\"data\":" + snap.dataJson + '}';
    }
    out += "]}";
    return out;
  }

private:
  std::unordered_map<std::string, Snapshot> snapshots_;
  std::mutex mutex_;
  std::condition_variable wake_;
  std::thread worker_;
  std::atomic<bool> stop_ {false};
  std::string host_;
  int port_ = 0;
#ifdef _WIN32
  SOCKET sock_ = INVALID_SOCKET;
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

  bool isEnabled() const {
    return isDragonKingDebugEnabled() && !host_.empty() && port_ > 0 && port_ <= 65535;
  }

  void workerLoop() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;
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
    closeSocket();
#ifdef _WIN32
    WSACleanup();
#endif
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

    sock_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!socketValid()) return false;
#ifdef _WIN32
    u_long nonblocking = 1;
    if (ioctlsocket(sock_, FIONBIO, &nonblocking) != 0) {
#else
    if (fcntl(sock_, F_SETFL, O_NONBLOCK) != 0) {
#endif
      closeSocket();
      return false;
    }
#ifdef SO_NOSIGPIPE
    int noSigPipe = 1;
    setsockopt(sock_, SOL_SOCKET, SO_NOSIGPIPE, &noSigPipe, sizeof(noSigPipe));
#endif
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(uint16_t(port_));
    if (::inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1) {
      closeSocket();
      return false;
    }
    if (::connect(sock_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) {
      if (!wouldBlock() || !waitWritable()) {
        closeSocket();
        return false;
      }
      int error = 0;
#ifdef _WIN32
      int size = sizeof(error);
#else
      socklen_t size = sizeof(error);
#endif
      if (getsockopt(sock_, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &size) != 0 || error) {
        closeSocket();
        return false;
      }
    }
    return true;
  }

  static bool wouldBlock() {
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EINPROGRESS || errno == EWOULDBLOCK || errno == EAGAIN;
#endif
  }

  bool waitWritable() {
    // A slow peer cannot keep the plugin alive during unload.
    for (int i = 0; i < 20 && !stop_.load(); ++i) {
#ifdef _WIN32
      WSAPOLLFD fd = {sock_, POLLWRNORM, 0};
      const int ready = WSAPoll(&fd, 1, 50);
#else
      pollfd fd = {sock_, POLLOUT, 0};
      const int ready = poll(&fd, 1, 50);
#endif
      if (ready != 0) return ready > 0 && !(fd.revents & (POLLERR | POLLHUP | POLLNVAL));
    }
    return false;
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
      if (stop_.load()) return false;
#ifdef _WIN32
      int sent = ::send(sock_, data, int(size), 0);
#elif defined(MSG_NOSIGNAL)
      ssize_t sent = ::send(sock_, data, size, MSG_NOSIGNAL);
#else
      ssize_t sent = ::send(sock_, data, size, 0);
#endif
      if (sent < 0 && wouldBlock() && waitWritable()) continue;
      if (sent <= 0) {
        return false;
      }
      data += sent;
      size -= size_t(sent);
    }
    return true;
  }

  void sleepMs(int ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    wake_.wait_for(lock, std::chrono::milliseconds(ms), [this] { return stop_.load(); });
  }

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

};

static Transport &transport() {
  static Transport instance;
  return instance;
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
  char dataBuf[2048];
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
  const size_t rangeEnd = std::strlen(buf);
  if (std::isfinite(range.average))
    std::snprintf(buf + rangeEnd, size - rangeEnd, ",\"%s_avg\":%.3f",
                  key ? key : "", std::max(0.f, range.average));
  else
    std::snprintf(buf + rangeEnd, size - rangeEnd, ",\"%s_avg\":null", key ? key : "");
}

} // namespace

void shutdownTransport() { transport().stop(); }

void submitTDScopeUiMetrics(uint32_t instanceId,
                            TimingRangeUs processUs,
                            TimingRangeUs stepUs,
                            TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t publishSeq,
                            uint64_t drawSeq,
                            uint64_t drawCalls) {
  submitUiMetricSchema("TDScope",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"rows\",\"label\":\"Rows\"},{\"key\":\"density_pct\",\"label\":\"Density%\"},{\"key\":\"zoom\",\"label\":\"Zoom\"},{\"key\":\"thickness\",\"label\":\"Thickness\"},{\"key\":\"publish_seq\",\"label\":\"Publish\"},{\"key\":\"draw_seq\",\"label\":\"Draw Seq\"},{\"key\":\"draw_calls\",\"label\":\"Calls\"}]");
  char dataBuf[2048];
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
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
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
                                 TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid) {
  submitUiMetricSchema("TemporalDeck",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"scope_preview_us\",\"label\":\"Scope (us)\"},{\"key\":\"scope_stride\",\"label\":\"Stride\"},{\"key\":\"scope_metric_valid\",\"label\":\"Scope OK\"}]");
  char dataBuf[2048];
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
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"scope_preview_us\":%.4f,\"scope_stride\":%d,\"scope_metric_valid\":%d}",
                std::max(0.f, scopePreviewUs),
                std::max(0, scopeStride),
                scopeMetricValid ? 1 : 0);
  double ts = system::getTime();
  transport().submit("TemporalDeck", instanceId, "ui", "metric", dataBuf, ts);
}

void submitCrownstepAiMetrics(uint32_t instanceId, int aiThinkMs) {
  if (shouldSubmitSchema("Crownstep", "ai")) {
    transport().submit(
      "Crownstep",
      0u,
      "ai",
      "schema",
      "{\"schema\":1,\"target_kind\":\"metric\",\"columns\":[{\"key\":\"ai_ms\",\"label\":\"AI (ms)\"}]}",
      system::getTime());
  }

  char dataBuf[64];
  std::snprintf(dataBuf, sizeof(dataBuf), "{\"ai_ms\":%d}", std::max(0, aiThinkMs));
  transport().submit("Crownstep", instanceId, "ai", "metric", dataBuf, system::getTime());
}

void submitBifurxUiMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                           bool renderOpengl,
                           float curvePrepUs,
                           float overlayPrepUs,
                           float surfaceRenderUs,
                           float workerSubmitUs,
                           float conduitDrawUs) {
  submitUiMetricSchema("Bifurx",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"opengl\",\"label\":\"GL\"},{\"key\":\"curve_prep_us\",\"label\":\"Curve (us)\"},{\"key\":\"overlay_prep_us\",\"label\":\"Overlay (us)\"},{\"key\":\"surface_render_us\",\"label\":\"Surface (us)\"},{\"key\":\"worker_submit_us\",\"label\":\"Worker (us)\"},{\"key\":\"conduit_draw_us\",\"label\":\"Conduit (us)\"}]");
  char dataBuf[2048];
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
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"opengl\":%d,\"curve_prep_us\":%.3f,\"overlay_prep_us\":%.3f,\"surface_render_us\":%.3f,\"worker_submit_us\":%.3f,\"conduit_draw_us\":%.3f}",
                renderOpengl ? 1 : 0,
                std::max(0.f, curvePrepUs),
                std::max(0.f, overlayPrepUs),
                std::max(0.f, surfaceRenderUs),
                std::max(0.f, workerSubmitUs),
                std::max(0.f, conduitDrawUs));
  double ts = system::getTime();
  transport().submit("Bifurx", instanceId, "ui", "metric", dataBuf, ts);
}

void submitWyrmMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                       TimingRangeUs editorStepUs,
                       TimingRangeUs cachedEditorUs,
                       TimingRangeUs overlayUs,
                       float editorDrawUs,
                       int channels,
                       int bodySamples,
                       uint64_t bodySampleCacheHits,
                       uint64_t bodySampleCacheMisses,
                       bool fixedSurface,
                       int fixedSurfaceWidth,
                       int fixedSurfaceHeight,
                       uint64_t fixedSurfaceGeneration) {
  submitUiMetricSchema("Wyrm",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"editor_step_us\",\"label\":\"Ed Step (us)\"},{\"key\":\"cached_editor_us\",\"label\":\"Cache (us)\"},{\"key\":\"overlay_us\",\"label\":\"Overlay (us)\"},{\"key\":\"ed_us\",\"label\":\"CL.us\"},{\"key\":\"ch\",\"label\":\"Ch\"},{\"key\":\"body\",\"label\":\"Body\"},{\"key\":\"body_cache_hit\",\"label\":\"BHit\"},{\"key\":\"body_cache_miss\",\"label\":\"BMiss\"},{\"key\":\"fixed_surface\",\"label\":\"Fixed\"},{\"key\":\"surface_w\",\"label\":\"Surf W\"},{\"key\":\"surface_h\",\"label\":\"Surf H\"},{\"key\":\"surface_gen\",\"label\":\"Gen\"}]");
  char dataBuf[2048];
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
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "editor_step_us", editorStepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "cached_editor_us", cachedEditorUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "overlay_us", overlayUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"ed_us\":%.3f,\"ch\":%d,\"body\":%d,\"body_cache_hit\":%llu,\"body_cache_miss\":%llu,\"fixed_surface\":%d,\"surface_w\":%d,\"surface_h\":%d,\"surface_gen\":%llu}",
                std::max(0.f, editorDrawUs),
                std::max(0, channels),
                std::max(0, bodySamples),
                static_cast<unsigned long long>(bodySampleCacheHits),
                static_cast<unsigned long long>(bodySampleCacheMisses),
                fixedSurface ? 1 : 0,
                std::max(0, fixedSurfaceWidth),
                std::max(0, fixedSurfaceHeight),
                static_cast<unsigned long long>(fixedSurfaceGeneration));
  double ts = system::getTime();
  transport().submit("Wyrm", instanceId, "ui", "metric", dataBuf, ts);
}

void submitIntegralFluxMetrics(uint32_t instanceId,
                               TimingRangeUs processUs,
                               TimingRangeUs stepUs,
                               TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                               TimingRangeUs apertureUs,
                               float gearUs,
                               float eclipseUs) {
  submitUiMetricSchema("IntegralFlux",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"aperture_us\",\"label\":\"Aperture (us)\"},{\"key\":\"gear_us\",\"label\":\"Halo2 (us)\"},{\"key\":\"eclipse_us\",\"label\":\"E2 (us)\"}]");
  char dataBuf[2048];
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
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",");
  appendRange(dataBuf, sizeof(dataBuf), "aperture_us", apertureUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"gear_us\":%.3f,\"eclipse_us\":%.3f}",
                std::max(0.f, gearUs),
                std::max(0.f, eclipseUs));
  double ts = system::getTime();
  transport().submit("IntegralFlux", instanceId, "ui", "metric", dataBuf, ts);
}

void submitBaselineMetrics(const char* moduleName,
                           uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs, TimingRangeUs drawLayerUs) {
  const char* safeModuleName = moduleName ? moduleName : "";
  submitUiMetricSchema(safeModuleName,
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"}]");
  char dataBuf[2048];
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
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                "}");
  double ts = system::getTime();
  transport().submit(safeModuleName, instanceId, "ui", "metric", dataBuf, ts);
}

void submitChimeraUiMetrics(uint32_t instanceId,
                            TimingRangeUs processUs, TimingRangeUs stepUs, TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                            TimingRangeUs cacheUs, TimingRangeUs liveUs, TimingRangeUs lightUs,
                            TimingRangeUs glStepUs, unsigned cacheRenders) {
  submitUiMetricSchema("Chimera",
      "[{\"key\":\"process_us\",\"label\":\"Process (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},"
      "{\"key\":\"cache_us\",\"label\":\"Display cache (us)\"},{\"key\":\"live_us\",\"label\":\"Live UI (us)\"},{\"key\":\"light_us\",\"label\":\"Light pass (us)\"},"
      "{\"key\":\"gl_step_us\",\"label\":\"GL in step (us)\"},{\"key\":\"cache_renders\",\"label\":\"Cache renders\"}]");
  char dataBuf[4096] = "{";
  const char* keys[] = {"process_us", "step_us", "draw_us", "draw_layer_us", "cache_us", "live_us", "light_us", "gl_step_us"};
  const TimingRangeUs ranges[] = {processUs, stepUs, drawUs, drawLayerUs, cacheUs, liveUs, lightUs, glStepUs};
  for (unsigned i = 0; i < sizeof(ranges) / sizeof(ranges[0]); ++i) {
    if (i) std::strcat(dataBuf, ",");
    appendRange(dataBuf, sizeof(dataBuf), keys[i], ranges[i]);
  }
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf),
      ",\"cache_renders\":%u}", cacheRenders);
  transport().submit("Chimera", instanceId, "ui", "metric", dataBuf, system::getTime());
}

void submitSibylMetrics(uint32_t instanceId,
                        int64_t rackModuleId,
                        TimingRangeUs processUs,
                        float processMeanUs,
                        uint64_t processSamples,
                        TimingRangeUs stepUs,
                        TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                        TimingRangeUs snapshotUs,
                        TimingRangeUs oracleUs,
                        int nvgPathOps) {
  submitUiMetricSchema("Sibyl",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"snapshot_us\",\"label\":\"Snap (us)\"},{\"key\":\"oracle_us\",\"label\":\"Oracle (us)\"},{\"key\":\"nvg_path_ops\",\"label\":\"NVG Paths\"}]");
  char dataBuf[2048];
  std::snprintf(dataBuf, sizeof(dataBuf), "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "snapshot_us", snapshotUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "oracle_us", oracleUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"nvg_path_ops\":%d,\"process_mean_us\":%.4f,\"process_samples\":%llu}",
                std::max(0, nvgPathOps),
                std::max(0.f, processMeanUs),
                static_cast<unsigned long long>(processSamples));
  transport().submit("Sibyl", instanceId, "ui", "metric", dataBuf, system::getTime(), rackModuleId);
}

std::string latestMetricsJson(int64_t rackModuleId) {
  return transport().latestMetricsJson(rackModuleId);
}

void submitProcMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs, TimingRangeUs drawLayerUs) {
  submitBaselineMetrics("Proc", instanceId, processUs, stepUs, drawUs, drawLayerUs);
}

void submitUndertowMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs, TimingRangeUs drawLayerUs) {
  submitBaselineMetrics("Undertow", instanceId, processUs, stepUs, drawUs, drawLayerUs);
}

void submitIrisMetrics(uint32_t instanceId,
                       TimingRangeUs processUs,
                       TimingRangeUs stepUs,
                       TimingRangeUs drawUs, TimingRangeUs drawLayerUs) {
  submitBaselineMetrics("Iris", instanceId, processUs, stepUs, drawUs, drawLayerUs);
}

void submitDoorstopMetrics(uint32_t instanceId,
                           TimingRangeUs processUs,
                           TimingRangeUs stepUs,
                           TimingRangeUs drawUs, TimingRangeUs drawLayerUs,
                           TimingRangeUs geometryIdleUs,
                           TimingRangeUs geometryTrailUs,
                           TimingRangeUs panelIdleUs,
                           TimingRangeUs panelTrailUs,
                           TimingRangeUs overflowIdleUs,
                           TimingRangeUs overflowTrailUs,
                           bool trailsActive) {
  submitUiMetricSchema("Doorstop",
                       "[{\"key\":\"process_us\",\"label\":\"Pro (us)\"},{\"key\":\"step_us\",\"label\":\"Step (us)\"},{\"key\":\"draw_us\",\"label\":\"Draw (us)\"},{\"key\":\"draw_layer_us\",\"label\":\"DL (us)\"},{\"key\":\"geometry_idle_us\",\"label\":\"Geo I (us)\"},{\"key\":\"geometry_trail_us\",\"label\":\"Geo T (us)\"},{\"key\":\"panel_idle_us\",\"label\":\"Panel I (us)\"},{\"key\":\"panel_trail_us\",\"label\":\"Panel T (us)\"},{\"key\":\"overflow_idle_us\",\"label\":\"Over I (us)\"},{\"key\":\"overflow_trail_us\",\"label\":\"Over T (us)\"},{\"key\":\"trails_active\",\"label\":\"Trails\"}]");
  char dataBuf[2048];
  std::snprintf(dataBuf, sizeof(dataBuf), "{");
  appendRange(dataBuf, sizeof(dataBuf), "process_us", processUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "step_us", stepUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_us", drawUs);
  std::strcat(dataBuf, ",");
  appendRange(dataBuf, sizeof(dataBuf), "draw_layer_us", drawLayerUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "geometry_idle_us", geometryIdleUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "geometry_trail_us", geometryTrailUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "panel_idle_us", panelIdleUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "panel_trail_us", panelTrailUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "overflow_idle_us", overflowIdleUs);
  std::snprintf(dataBuf + std::strlen(dataBuf), sizeof(dataBuf) - std::strlen(dataBuf), ",");
  appendRange(dataBuf, sizeof(dataBuf), "overflow_trail_us", overflowTrailUs);
  std::snprintf(dataBuf + std::strlen(dataBuf),
                sizeof(dataBuf) - std::strlen(dataBuf),
                ",\"trails_active\":%d}",
                trailsActive ? 1 : 0);
  transport().submit("Doorstop", instanceId, "ui", "metric", dataBuf, system::getTime());
}

} // namespace debug_terminal
