#include "../src/OctaviaPresenceRoutes.hpp"
#include "../src/OctaviaServerLifecycle.hpp"
#include <iostream>
#include <stdexcept>

void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
int main() {
    try {
        OctaviaPresence presence;
        std::atomic<bool> running{false}, failed{false};
        httplib::Server server;
        octavia::registerPresenceRoutes(server, presence, running, failed);
        octavia::ServerLifecycle lifecycle(server, running);
        const int port = lifecycle.start(0);
        require(port > 0, "test could not allocate localhost port");
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
        while (!running && std::chrono::steady_clock::now() < deadline)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        require(running, "test listener did not start");
        httplib::Client client("127.0.0.1", port);
        client.set_read_timeout(2);
        client.set_connection_timeout(2);
        auto get = client.Get("/presence");
        require(get && get->status == 200 && get->body.find("\"targetState\":\"idle\"") != std::string::npos,
            "initial GET state");
        for (auto state : {"idle", "inspecting", "thinking", "working", "error", "sleeping"}) {
            auto response = client.Post("/presence", std::string("{\"state\":\"") + state + "\",\"leaseMs\":1000}", "application/json");
            require(response && response->status == 200, "valid state rejected");
            require(response->body.find(std::string("\"overrideState\":\"") + state + "\"") != std::string::npos,
                "override not reflected in response");
        }
        for (const auto& body : {"{}", "[]", "{bad", "{\"state\":\"bogus\"}",
                "{\"state\":\"thinking\",\"leaseMs\":999}",
                "{\"state\":\"thinking\",\"leaseMs\":300001}",
                "{\"state\":\"thinking\",\"leaseMs\":1000.5}",
                "{\"state\":\"thinking\",\"leaseMs\":true}",
                "{\"state\":\"thinking\",\"extra\":0}",
                "{\"state\":\"thinking\",\"state\":\"working\"}"}) {
            auto response = client.Post("/presence", body, "application/json");
            require(response && response->status == 400, "invalid request accepted");
        }
        get = client.Get("/presence");
        require(get && get->body.find("\"overrideState\":\"sleeping\"") != std::string::npos,
            "invalid request changed existing lease");
        auto large = client.Post("/presence", std::string(1025, ' '), "application/json");
        require(large && large->status == 413, "oversized body accepted");
        auto release = client.Post("/presence", "{\"state\":\"auto\"}", "application/json");
        require(release && release->status == 200 && release->body.find("\"overrideState\":null") != std::string::npos,
            "auto failed to release");
        lifecycle.shutdown();
        std::cout << "PASS: presence HTTP states, release, strict validation and failed-write preservation\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
