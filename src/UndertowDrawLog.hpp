#pragma once
#include "plugin.hpp"
#include "WavePreviewTracer.hpp"
#include <fstream>
#include <iomanip>
#include <ctime>

struct UndertowPreviewDrawMetrics {
  WavePreviewTracerDrawStats history;
  double drawUs = 0, samplesUs = 0, pointsUs = 0, historyUs = 0;
  double simplifySubmitUs = 0, strokeSubmitUs = 0, labelUs = 0;
  uint32_t bridgeStrokes = 0, bridgeFallbacks = 0;
  uint32_t sampleRebuilds = 0, pointRebuilds = 0, attempts = 0, captures = 0, simplifiedPoints = 0;
};

// Each module widget owns its file and preview counters. No shared step counters.
struct UndertowDrawLog {
  std::ofstream file;
  uint64_t row = 0;
  bool attempted = false;

  void sync(bool enabled, uint32_t instance) {
    if (!enabled) {
      if (file.is_open()) file.close();
      attempted = false;
      row = 0;
      return;
    }
    if (attempted) return;
    attempted = true;
    const std::string root = system::join(leviathanPluginUserRootPath(), "Undertow");
    system::createDirectories(root);
    const std::time_t now = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    char stamp[32];
    std::strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", &tm);
    static unsigned sequence = 0;
    const std::string path = system::join(root, "undertow_draw_" + std::to_string(instance)
      + "_" + stamp + "_" + std::to_string(sequence++) + ".csv");
    file.open(path.c_str(), std::ios::out | std::ios::trunc);
    if (!file) { WARN("Undertow failed to open draw log: %s", path.c_str()); return; }
    file << std::fixed << std::setprecision(3);
    file << "row,module_id,instance_id,time_sec,step_us,draw_us,module_widget_draw_us,preview_draw_us,"
      "sample_rebuild_us,point_rebuild_us,history_draw_us,simplify_path_submit_us,stroke_submit_us,label_us,"
      "sample_rebuilds,point_rebuilds,capture_attempts,accepted_captures,simplified_points,"
      "tracer_enabled,tracer_mode,shape,edge_hardness,asym_enabled,asym_right,frequency_hz,scale_x,scale_y,pixel_ratio,history_trails,history_source_points,history_rasterizations,bridge_strokes,bridge_fallbacks\n";
  }
};
