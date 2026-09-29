#include "DebugTerminalTransport.hpp"
#include <jansson.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <thread>

// Enable submissions on the test thread; keep the network worker disabled.
static const auto testThread = std::this_thread::get_id();
bool isDragonKingDebugEnabled() { return std::this_thread::get_id() == testThread; }

static void checkRows(const std::string& packet, size_t expected) {
    json_error_t error;
    json_t* root = json_loads(packet.c_str(), 0, &error);
    assert(root);
    json_t* rows = json_object_get(root, "metrics");
    assert(json_array_size(rows) == expected);
    for (size_t i = 0; i < expected; ++i) {
        json_t* data = json_object_get(json_array_get(rows, i), "data");
        assert(std::strcmp(json_string_value(json_object_get(data, "draw_us")), "11.00-19.00") == 0);
        assert(json_number_value(json_object_get(data, "draw_us_avg")) == 14.0);
        assert(std::strcmp(json_string_value(json_object_get(data, "draw_layer_us")), "4.00-10.00") == 0);
        assert(json_number_value(json_object_get(data, "draw_layer_us_avg")) == 7.0);
    }
    json_decref(root);
}

int main() {
    using namespace debug_terminal;
    TimingRangeUs process(1, 2), step(3, 4), draw(11, 19), layer(4, 10), component;
    process.average = 1.5f; step.average = 3.5f; draw.average = 14.f; layer.average = 7.f;
    submitBaselineMetrics("TestBaseline", 1, process, step, draw, layer);
    submitTDScopeUiMetrics(1, process, step, draw, layer, 0, 0, 1, 1, 0, 0, 0);
    submitTemporalDeckUiMetrics(1, process, step, draw, layer, 0, 1, false);
    submitBifurxUiMetrics(1, process, step, draw, layer, false, 0, 0, 0, 0, 0);
    submitWyrmMetrics(1, process, step, draw, layer, component, component, component,
                      0, 1, 0, 0, 0, false, 0, 0, 0);
    submitIntegralFluxMetrics(1, process, step, draw, layer, component, 0, 0);
    submitProcMetrics(1, process, step, draw, layer);
    submitUndertowMetrics(1, process, step, draw, layer);
    submitIrisMetrics(1, process, step, draw, layer);
    submitDoorstopMetrics(1, process, step, draw, layer,
                          component, component, component, component, component, component, false);
    submitChimeraUiMetrics(1, process, step, draw, layer, component, component, component, component, 0);
    submitSibylMetrics(1, 42, process, 1.5f, 1, step, draw, layer, component, component, 0);
    checkRows(latestMetricsJson(-1), 11);
    checkRows(latestMetricsJson(42), 1);
    std::puts("PASS: separate Draw and DrawLayer ranges/means survive every telemetry serializer");
}
