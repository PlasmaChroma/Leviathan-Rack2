#include "../src/OctaviaServerLifecycle.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>

template <typename Predicate>
void awaitState(Predicate ready) {
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!ready()) {
        if (std::chrono::steady_clock::now() >= deadline)
            throw std::runtime_error("listener transition timed out");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

int main() {
    try {
        httplib::Server a, b;
        std::atomic<bool> aLit{false}, bLit{false};
        octavia::ServerLifecycle first(a, aLit), second(b, bLit);
        a.Get("/owner", [](const httplib::Request&, httplib::Response& r) {
            r.set_content("first", "text/plain");
        });
        b.Get("/owner", [](const httplib::Request&, httplib::Response& r) {
            r.set_content("second", "text/plain");
        });
        const int port = first.start(0);
        require(port > 0, "could not allocate test port");
        awaitState([&]() { return aLit.load(); });
        for (int attempt = 0; attempt < 3; ++attempt) {
            require(second.start(port) == -1, "competing listener claimed occupied port");
            require(!bLit && !second.isActive(), "failed claimant lit up");
        }
        httplib::Client client("127.0.0.1", port);
        client.set_connection_timeout(2);
        client.set_read_timeout(2);
        auto response = client.Get("/owner");
        require(response && response->body == "first", "request reached wrong owner");
        first.stop();
        awaitState([&]() { return !first.isActive(); });
        require(!aLit, "stopped owner remained lit");
        require(second.start(port) == port, "failed claimant could not retry after release");
        awaitState([&]() { return bLit.load(); });
        response = client.Get("/owner");
        require(response && response->body == "second", "handoff reached wrong owner");
        second.shutdown();
        for (int attempt = 0; attempt < 20; ++attempt) {
            require(first.start(port) == port, "restart failed");
            first.stop(); // Also covers cancellation before listener startup.
            awaitState([&]() { return !first.isActive(); });
            require(!aLit, "cancelled startup remained lit");
        }
        {
            httplib::Server transientServer;
            std::atomic<bool> lit{false};
            octavia::ServerLifecycle transient(transientServer, lit);
            require(transient.start(port) == port, "destruction test could not start");
        }
        require(second.start(port) == port, "destruction did not release port");
        second.shutdown();
        std::cout << "PASS: exclusive ownership, failed-claim darkness, HTTP routing, "
                     "handoff, startup cancellation, restart, and destruction\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
