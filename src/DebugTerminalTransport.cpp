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
    snapshots_[makeKey(snap.module, snap.instance, snap.stream)] = snap;
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

  static std::string makeKey(const std::string &module, const std::string &instance, const std::string &stream) {
    return module + "|" + instance + "|" + stream;
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

      std::vector<Snapshot> snapshots = copySnapshots();
      for (const Snapshot &snap : snapshots) {
        if (!sendSnapshot(snap)) {
          closeSocket();
          break;
        }
      }
      sleepMs(kPublishIntervalMs);
    }
  }

  std::vector<Snapshot> copySnapshots() {
    std::vector<Snapshot> out;
    std::lock_guard<std::mutex> lock(mutex_);
    out.reserve(snapshots_.size());
    for (const auto &entry : snapshots_) {
      out.push_back(entry.second);
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

} // namespace

void submitTDScopeUiMetrics(uint32_t instanceId,
                            float uiMs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t misses) {
  char dataBuf[256];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{\"ui_ms\":%.4f,\"rows\":%d,\"density_pct\":%.2f,\"zoom\":%.4f,\"thickness\":%.4f,\"misses\":%llu}",
                std::max(0.f, uiMs),
                std::max(0, rows),
                std::max(0.f, densityPct),
                std::max(0.f, zoom),
                std::max(0.f, thickness),
                (unsigned long long) misses);
  double ts = system::getTime();
  transport().submit("TDScope", instanceId, "ui", "metric", dataBuf, ts);
}

void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 float uiMs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid) {
  char dataBuf[256];
  std::snprintf(dataBuf,
                sizeof(dataBuf),
                "{\"ui_ms\":%.4f,\"scope_preview_us\":%.4f,\"scope_stride\":%d,\"scope_metric_valid\":%d}",
                std::max(0.f, uiMs),
                std::max(0.f, scopePreviewUs),
                std::max(0, scopeStride),
                scopeMetricValid ? 1 : 0);
  double ts = system::getTime();
  transport().submit("TemporalDeck", instanceId, "ui", "metric", dataBuf, ts);
}

} // namespace debug_terminal
