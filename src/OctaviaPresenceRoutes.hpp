#pragma once

#include "OctaviaPresence.hpp"
#include "third_party/httplib.h"
#include <jansson.h>
#include <cstdlib>
#include <memory>

namespace octavia {

inline void registerPresenceRoutes(httplib::Server& server, OctaviaPresence& presence,
        const std::atomic<bool>& running, const std::atomic<bool>& startupFailed) {
    auto reply = [&presence, &running, &startupFailed](httplib::Response& response) {
        const auto status = presence.status(running.load(std::memory_order_relaxed),
            startupFailed.load(std::memory_order_relaxed));
        json_t* root = json_object();
        json_object_set_new(root, "ok", json_true());
        json_object_set_new(root, "targetState", json_string(octaviaPresenceName(status.target)));
        json_object_set_new(root, "automaticState", json_string(octaviaPresenceName(status.automatic)));
        json_object_set_new(root, "overrideState", status.remainingMs > 0
            ? json_string(octaviaPresenceName(status.overrideState)) : json_null());
        json_object_set_new(root, "remainingMs", json_integer(status.remainingMs));
        json_object_set_new(root, "automaticHoldMs", json_integer(OctaviaPresence::HoldMs));
        json_object_set_new(root, "crossfadeMs", json_integer(OctaviaPresence::FadeMs));
        char* body = json_dumps(root, JSON_COMPACT);
        response.set_content(body, "application/json");
        std::free(body);
        json_decref(root);
    };
    server.Get("/presence", [reply](const httplib::Request&, httplib::Response& res) {
        reply(res);
    });
    server.Post("/presence", [&presence, reply](const httplib::Request& req, httplib::Response& res) {
        auto reject = [&res](const char* message, int code = 400) {
            res.status = code;
            res.set_content(std::string("{\"ok\":false,\"error\":\"") + message + "\"}",
                "application/json");
        };
        if (req.body.size() > 1024) {
            reject("presence body exceeds 1024 bytes", 413);
            return;
        }
        std::unique_ptr<json_t, decltype(&json_decref)> root(
            json_loads(req.body.c_str(), JSON_REJECT_DUPLICATES, nullptr), &json_decref);
        if (!json_is_object(root.get())) { reject("body must be a JSON object"); return; }
        const char* key; json_t* value;
        json_object_foreach(root.get(), key, value) {
            if (std::string(key) != "state" && std::string(key) != "leaseMs") {
                reject("unknown presence field"); return;
            }
        }
        json_t* stateJson = json_object_get(root.get(), "state");
        if (!json_is_string(stateJson)) { reject("state must be a string"); return; }
        const std::string name(json_string_value(stateJson), json_string_length(stateJson));
        json_t* leaseJson = json_object_get(root.get(), "leaseMs");
        int64_t lease = OctaviaPresence::DefaultLeaseMs;
        if (leaseJson) {
            if (!json_is_integer(leaseJson)) { reject("leaseMs must be an integer"); return; }
            lease = json_integer_value(leaseJson);
        }
        if (lease < 1000 || lease > OctaviaPresence::MaxLeaseMs) {
            reject("leaseMs must be between 1000 and 300000"); return;
        }
        if (name == "auto") presence.releaseOverride();
        else {
            int index = 0;
            for (; index < 6; ++index)
                if (name == octaviaPresenceName(static_cast<OctaviaPresenceState>(index))) break;
            if (index == 6) { reject("unknown presence state"); return; }
            presence.setOverride(static_cast<OctaviaPresenceState>(index), lease);
        }
        reply(res);
    });
}

} // namespace octavia
