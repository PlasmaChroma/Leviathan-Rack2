#pragma once

#include "third_party/httplib.h"
#include <atomic>
#include <thread>

namespace octavia {

// Start/stop are called by the UI owner. HTTP callbacks only publish state.
// Keep the thread joinable so an old listener cannot outlive its module.
class ServerLifecycle {
    httplib::Server& server;
    std::atomic<bool>& running;
    std::atomic<bool> active{false};
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> stopIssued{false};
    std::thread listener;

    void stopListening() {
        if (server.is_running() && !stopIssued.exchange(true, std::memory_order_acq_rel))
            server.stop();
    }

public:
    ServerLifecycle(httplib::Server& server, std::atomic<bool>& running)
        : server(server), running(running) {
        server.set_socket_options([](socket_t socket) {
#ifdef _WIN32
            // Do not inherit httplib's SO_REUSEADDR policy on Windows.
            httplib::set_socket_opt(socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, 1);
#else
            // POSIX SO_REUSEADDR permits restart after TIME_WAIT, but does
            // not share a live listening endpoint. Never enable SO_REUSEPORT.
            httplib::set_socket_opt(socket, SOL_SOCKET, SO_REUSEADDR, 1);
#endif
        });
        server.set_start_handler([this]() {
            this->running.store(true, std::memory_order_release);
            if (stopRequested.load(std::memory_order_acquire)) {
                this->running.store(false, std::memory_order_release);
                stopListening();
            }
        });
    }

    ~ServerLifecycle() { shutdown(); }

    bool isActive() const { return active.load(std::memory_order_acquire); }

    // Returns the claimed port, or -1 if occupied. Port zero is useful for
    // isolated tests: the OS selects an unused ephemeral port.
    int start(int port) {
        if (isActive()) return -1;
        if (listener.joinable()) listener.join();
        running.store(false, std::memory_order_release);
        server.stop(); // Clear httplib's failed-bind/decommissioned state.
        const int boundPort = port == 0
            ? server.bind_to_any_port("127.0.0.1")
            : (server.bind_to_port("127.0.0.1", port) ? port : -1);
        if (boundPort < 0) return -1;
        stopRequested.store(false, std::memory_order_release);
        stopIssued.store(false, std::memory_order_release);
        active.store(true, std::memory_order_release);
        listener = std::thread([this]() {
            server.listen_after_bind();
            running.store(false, std::memory_order_release);
            active.store(false, std::memory_order_release);
        });
        return boundPort;
    }

    void stop() {
        stopRequested.store(true, std::memory_order_release);
        running.store(false, std::memory_order_release);
        // If startup has not reached listen_internal yet, its start handler
        // observes stopRequested and closes the socket there instead.
        stopListening();
    }

    void shutdown() {
        stop();
        if (listener.joinable()) listener.join();
    }
};

} // namespace octavia
