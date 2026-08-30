#include "../src/PanelSvgUtils.hpp"
#include "../src/PanelAnchorAtlas.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <unistd.h>

namespace {

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

std::string makeTempSvgPath(const char* stem) {
  static uint32_t counter = 0u;
  counter++;
  return std::string("/tmp/") + stem + "_" + std::to_string(unsigned(getpid())) + "_" + std::to_string(counter) + ".svg";
}

bool writeTextFile(const std::string& path, const std::string& text) {
  std::ofstream out(path.c_str(), std::ios::out | std::ios::trunc);
  if (!out.good()) {
    return false;
  }
  out << text;
  return out.good();
}

bool readTextFile(const std::string& path, std::string* out) {
  if (!out) {
    return false;
  }
  std::ifstream in(path.c_str(), std::ios::in);
  if (!in.good()) {
    return false;
  }
  out->assign(
    std::istreambuf_iterator<char>(in),
    std::istreambuf_iterator<char>());
  return in.good() || in.eof();
}

bool nearlyEqual(float a, float b, float eps = 1e-6f) {
  return std::fabs(a - b) <= eps;
}

TestResult testRectParsesInMillimeters() {
  const std::string path = makeTempSvgPath("panel_svg_rect");
  const std::string svg = R"SVG(<svg xmlns="http://www.w3.org/2000/svg"><rect id="BOARD_AREA" x="150" y="275" width="840" height="930"/></svg>)SVG";
  if (!writeTextFile(path, svg)) {
    return {"Rect parsing", false, "failed to write temp SVG"};
  }

  math::Rect rect;
  bool ok = panel_svg::loadRectFromSvgMm(path, "BOARD_AREA", &rect);
  bool pass = ok
    && nearlyEqual(rect.pos.x, 1.5f)
    && nearlyEqual(rect.pos.y, 2.75f)
    && nearlyEqual(rect.size.x, 8.4f)
    && nearlyEqual(rect.size.y, 9.3f);
  return {"Rect parses and converts 1/100mm units", pass,
          "ok=" + std::to_string(ok ? 1 : 0) + " x=" + std::to_string(rect.pos.x) +
            " y=" + std::to_string(rect.pos.y)};
}

TestResult testPointParsesCircleCenter() {
  const std::string path = makeTempSvgPath("panel_svg_circle");
  const std::string svg = R"SVG(<svg xmlns="http://www.w3.org/2000/svg"><circle id="CLOCK_INPUT" cx="123" cy="456" r="10"/></svg>)SVG";
  if (!writeTextFile(path, svg)) {
    return {"Circle point parsing", false, "failed to write temp SVG"};
  }

  Vec point;
  bool ok = panel_svg::loadPointFromSvgMm(path, "CLOCK_INPUT", &point);
  bool pass = ok && nearlyEqual(point.x, 1.23f) && nearlyEqual(point.y, 4.56f);
  return {"Point parser reads circle center", pass,
          "ok=" + std::to_string(ok ? 1 : 0) + " x=" + std::to_string(point.x) +
            " y=" + std::to_string(point.y)};
}

TestResult testPointParsesRectCenter() {
  const std::string path = makeTempSvgPath("panel_svg_rect_center");
  const std::string svg = R"SVG(<svg xmlns="http://www.w3.org/2000/svg"><rect id="PITCH_OUTPUT" x="400" y="600" width="200" height="100"/></svg>)SVG";
  if (!writeTextFile(path, svg)) {
    return {"Rect-center point parsing", false, "failed to write temp SVG"};
  }

  Vec point;
  bool ok = panel_svg::loadPointFromSvgMm(path, "PITCH_OUTPUT", &point);
  bool pass = ok && nearlyEqual(point.x, 5.0f) && nearlyEqual(point.y, 6.5f);
  return {"Point parser falls back to rect center", pass,
          "ok=" + std::to_string(ok ? 1 : 0) + " x=" + std::to_string(point.x) +
            " y=" + std::to_string(point.y)};
}

TestResult testFindsRectsByIdSubstring() {
  const std::string path = makeTempSvgPath("panel_svg_rect_substring");
  const std::string svg = R"SVG(<svg xmlns="http://www.w3.org/2000/svg">
    <defs>
      <linearGradient id="baseGradient">
        <stop offset="0" style="stop-color:#5740bf;stop-opacity:1"/>
        <stop offset="1" style="stop-color:#5740bf;stop-opacity:0"/>
      </linearGradient>
      <linearGradient id="derivedGradient" xlink:href="#baseGradient"/>
    </defs>
    <rect id="FRAME_LEFT" x="10" y="20" width="30" height="40"/>
    <g transform="matrix(1,0,0,1,-50,-75)">
      <rect
        x="150"
        y="275"
        width="840"
        height="930"
        id="frame_left_ENHANCE"
        style="fill:url(#derivedGradient)" />
    </g>
  </svg>)SVG";
  if (!writeTextFile(path, svg)) {
    return {"Rect substring finder", false, "failed to write temp SVG"};
  }

  std::vector<panel_svg::SvgRectMatch> matches;
  bool ok = panel_svg::findRectsWithIdSubstringMm(path, "ENHANCE", &matches);
  bool pass = ok
    && matches.size() == 1u
    && matches[0].id == "frame_left_ENHANCE"
    && nearlyEqual(matches[0].rect.pos.x, 1.0f)
    && nearlyEqual(matches[0].rect.pos.y, 2.0f)
    && nearlyEqual(matches[0].rect.size.x, 8.4f)
    && nearlyEqual(matches[0].rect.size.y, 9.3f)
    && matches[0].hasFillColor
    && nearlyEqual(matches[0].fillColor.r, 0x57 / 255.f, 1e-5f)
    && nearlyEqual(matches[0].fillColor.g, 0x40 / 255.f, 1e-5f)
    && nearlyEqual(matches[0].fillColor.b, 0xbf / 255.f, 1e-5f);
  return {"Rect substring finder returns matching rect geometry", pass,
          "ok=" + std::to_string(ok ? 1 : 0) +
            " count=" + std::to_string(matches.size()) +
            " color=" + (matches.empty() ? std::string("n/a") :
              (std::to_string(matches[0].hasFillColor ? 1 : 0) + "," +
               std::to_string(matches[0].fillColor.r) + "," +
               std::to_string(matches[0].fillColor.g) + "," +
               std::to_string(matches[0].fillColor.b)))};
}

TestResult testCircleParsesWithExplicitScale() {
  const std::string path = makeTempSvgPath("panel_svg_circle_scaled");
  const std::string svg =
    R"SVG(<svg xmlns="http://www.w3.org/2000/svg"><circle id="PLATTER_AREA" cx="50.8" cy="53.971752" r="37.031235"/></svg>)SVG";
  if (!writeTextFile(path, svg)) {
    return {"Circle parser with scale", false, "failed to write temp SVG"};
  }

  Vec center;
  float radius = 0.f;
  bool ok = panel_svg::loadCircleFromSvg(path, "PLATTER_AREA", &center, &radius, 1.f);
  bool pass = ok
    && nearlyEqual(center.x, 50.8f)
    && nearlyEqual(center.y, 53.971752f)
    && nearlyEqual(radius, 37.031235f);
  return {"Circle parser supports explicit unit scale", pass,
          "ok=" + std::to_string(ok ? 1 : 0) + " cx=" + std::to_string(center.x) +
            " cy=" + std::to_string(center.y) + " r=" + std::to_string(radius)};
}

TestResult testMissingElementFailsGracefully() {
  const std::string path = makeTempSvgPath("panel_svg_missing");
  const std::string svg = R"SVG(<svg xmlns="http://www.w3.org/2000/svg"><rect id="SOME_OTHER_ID" x="1" y="1" width="1" height="1"/></svg>)SVG";
  if (!writeTextFile(path, svg)) {
    return {"Missing element handling", false, "failed to write temp SVG"};
  }

  math::Rect rect;
  Vec point;
  bool rectOk = panel_svg::loadRectFromSvgMm(path, "BOARD_AREA", &rect);
  bool pointOk = panel_svg::loadPointFromSvgMm(path, "CLOCK_INPUT", &point);
  bool pass = !rectOk && !pointOk;
  return {"Missing element returns false", pass,
          "rectOk=" + std::to_string(rectOk ? 1 : 0) +
            " pointOk=" + std::to_string(pointOk ? 1 : 0)};
}

TestResult testGeneratedAtlasFindsRealPanelAnchor() {
  panel_svg::PanelAnchorLookupResult anchor;
  bool ok = panel_svg::lookupPanelAnchor("res/crownstep.svg", "ROOT_INPUT", &anchor);
  bool pass = ok && anchor.found && anchor.hasCenter &&
    nearlyEqual(anchor.cx, 3878.885f, 1e-3f) &&
    nearlyEqual(anchor.cy, 11465.241f, 1e-3f);
  return {"Generated atlas finds Crownstep ROOT_INPUT", pass,
          "ok=" + std::to_string(ok ? 1 : 0) +
            " found=" + std::to_string(anchor.found ? 1 : 0) +
            " cx=" + std::to_string(anchor.cx) +
            " cy=" + std::to_string(anchor.cy)};
}

TestResult testDeepcachePanelAnchorsUseRepositoryUnits() {
  Vec ready;
  math::Rect progress;
  math::Rect framebufferProgress;
  math::Rect databaseStatus;
  math::Rect soloWave;
  math::Rect sourceSoloWave;
  math::Rect excludedWave;
  const bool readyOk = panel_svg::loadPointFromSvgMm("res/Deepcache.panel.svg", "ready_light", &ready);
  const bool progressOk = panel_svg::loadRectFromSvgMm("res/Deepcache.panel.svg", "progress", &progress);
  const bool framebufferProgressOk = panel_svg::loadRectFromSvgMm(
    "res/Deepcache.panel.svg", "framebuffer_progress", &framebufferProgress);
  const bool databaseStatusOk = panel_svg::loadRectFromSvgMm(
    "res/Deepcache.panel.svg", "database_status", &databaseStatus);
  const bool soloWaveOk = panel_svg::loadRectFromSvgMm(
    "res/Deepcache.panel.svg", "BRANDING_WAVE_SOLO_RASTER", &soloWave);
  const bool sourceSoloWaveOk = panel_svg::loadRectFromSvgMm(
    "res/Deepcache.svg", "BRANDING_WAVE_SOLO_RASTER", &sourceSoloWave);
  const bool pairedWavesExcluded =
    !panel_svg::loadRectFromSvgMm(
      "res/Deepcache.panel.svg", "BRANDING_WAVE_LEFT_RASTER", &excludedWave)
    && !panel_svg::loadRectFromSvgMm(
      "res/Deepcache.panel.svg", "BRANDING_WAVE_RIGHT_RASTER", &excludedWave);
  std::string labelsText;
  const bool labelsOk = readTextFile("res/Deepcache.labels.svg", &labelsText);
  const size_t logoPos = labelsText.find("id=\"reworked_LEVIATHAN_LOGO\"");
  const size_t logoEnd = logoPos == std::string::npos
    ? std::string::npos : labelsText.find("/>", logoPos);
  const size_t logoHiddenPos = logoPos == std::string::npos
    ? std::string::npos : labelsText.find("display:none", logoPos);
  const bool logoHidden = labelsOk
    && logoPos != std::string::npos
    && logoEnd != std::string::npos
    && logoHiddenPos != std::string::npos
    && logoHiddenPos < logoEnd;
  const bool pass = readyOk && progressOk && framebufferProgressOk && databaseStatusOk
    && soloWaveOk && sourceSoloWaveOk && pairedWavesExcluded && logoHidden
    && nearlyEqual(ready.x, 4.5f) && nearlyEqual(ready.y, 107.59958f, 1e-4f)
    && nearlyEqual(progress.pos.x, 3.f) && nearlyEqual(progress.pos.y, 27.099751f, 1e-4f)
    && nearlyEqual(progress.size.x, 14.32f) && nearlyEqual(progress.size.y, 7.f, 1e-4f)
    && nearlyEqual(framebufferProgress.pos.x, 3.f) && nearlyEqual(framebufferProgress.pos.y, 49.199771f, 1e-4f)
    && nearlyEqual(framebufferProgress.size.x, 14.32f) && nearlyEqual(framebufferProgress.size.y, 7.f, 1e-4f)
    && nearlyEqual(databaseStatus.pos.x, 3.f) && nearlyEqual(databaseStatus.pos.y, 72.600005f, 1e-4f)
    && nearlyEqual(databaseStatus.size.x, 14.32f) && nearlyEqual(databaseStatus.size.y, 12.186809f, 1e-4f)
    && nearlyEqual(soloWave.pos.x, 3.62626f, 1e-4f)
    && nearlyEqual(soloWave.pos.x + soloWave.size.x * 0.5f, 10.16f, 1e-4f)
    && nearlyEqual(soloWave.pos.y + soloWave.size.y, 128.5f, 1e-4f)
    && nearlyEqual(soloWave.size.x, 13.06748f, 1e-4f)
    && nearlyEqual(soloWave.size.y, 5.11810f, 1e-4f)
    && nearlyEqual(sourceSoloWave.pos.x, soloWave.pos.x, 1e-4f)
    && nearlyEqual(sourceSoloWave.pos.y, soloWave.pos.y, 1e-4f)
    && nearlyEqual(sourceSoloWave.size.x, soloWave.size.x, 1e-4f)
    && nearlyEqual(sourceSoloWave.size.y, soloWave.size.y, 1e-4f);
  return {"Deepcache anchors and solo branding use repository units", pass,
          "ready=" + std::to_string(ready.x) + "," + std::to_string(ready.y) +
            " progress=" + std::to_string(progress.pos.x) + "," + std::to_string(progress.pos.y) +
            " progress_sz=" + std::to_string(progress.size.x) + "," + std::to_string(progress.size.y) +
            " fb_progress=" + std::to_string(framebufferProgress.pos.x) + "," + std::to_string(framebufferProgress.pos.y) +
            " fb_progress_sz=" + std::to_string(framebufferProgress.size.x) + "," + std::to_string(framebufferProgress.size.y) +
            " db_status=" + std::to_string(databaseStatus.pos.x) + "," + std::to_string(databaseStatus.pos.y) +
            " db_status_sz=" + std::to_string(databaseStatus.size.x) + "," + std::to_string(databaseStatus.size.y) +
            " solo=" + std::to_string(soloWave.pos.x) + "," + std::to_string(soloWave.pos.y) +
            " solo_sz=" + std::to_string(soloWave.size.x) + "," + std::to_string(soloWave.size.y) +
            " logoHidden=" + std::to_string(logoHidden ? 1 : 0)};
}

TestResult testDoorstopSoloBrandingFitsThreeHp() {
  math::Rect panelWave;
  math::Rect sourceWave;
  math::Rect excludedWave;
  const bool panelOk = panel_svg::loadRectFromSvgMm(
    "res/doorstop.panel.svg", "BRANDING_WAVE_SOLO_RASTER", &panelWave);
  const bool sourceOk = panel_svg::loadRectFromSvgMm(
    "res/doorstop.svg", "BRANDING_WAVE_SOLO_RASTER", &sourceWave);
  const bool pairedWavesExcluded =
    !panel_svg::loadRectFromSvgMm(
      "res/doorstop.panel.svg", "BRANDING_WAVE_LEFT_RASTER", &excludedWave)
    && !panel_svg::loadRectFromSvgMm(
      "res/doorstop.panel.svg", "BRANDING_WAVE_RIGHT_RASTER", &excludedWave);
  const bool pass = panelOk && sourceOk && pairedWavesExcluded
    && nearlyEqual(panelWave.pos.x, 1.08626f, 1e-4f)
    && nearlyEqual(panelWave.pos.x + panelWave.size.x * 0.5f, 7.62f, 1e-4f)
    && nearlyEqual(panelWave.pos.y + panelWave.size.y, 128.5f, 1e-4f)
    && nearlyEqual(panelWave.size.x, 13.06748f, 1e-4f)
    && nearlyEqual(panelWave.size.y, 5.11810f, 1e-4f)
    && nearlyEqual(sourceWave.pos.x, panelWave.pos.x, 1e-4f)
    && nearlyEqual(sourceWave.pos.y, panelWave.pos.y, 1e-4f)
    && nearlyEqual(sourceWave.size.x, panelWave.size.x, 1e-4f)
    && nearlyEqual(sourceWave.size.y, panelWave.size.y, 1e-4f);
  return {"Doorstop solo branding fits centered within three HP", pass,
          "solo=" + std::to_string(panelWave.pos.x) + "," +
            std::to_string(panelWave.pos.y) +
            " solo_sz=" + std::to_string(panelWave.size.x) + "," +
            std::to_string(panelWave.size.y)};
}

TestResult testTemporalDeckBrandingRasterAnchors() {
  math::Rect left;
  math::Rect right;
  math::Rect logo;
  const bool leftOk = panel_svg::loadRectFromSvgMm(
    "res/deck.panel.svg", "BRANDING_WAVE_LEFT_RASTER", &left);
  const bool rightOk = panel_svg::loadRectFromSvgMm(
    "res/deck.panel.svg", "BRANDING_WAVE_RIGHT_RASTER", &right);
  const bool logoOk = panel_svg::loadRectFromSvgMm(
    "res/deck.panel.svg", "BRANDING_LEVIATHAN_LOGO_RASTER", &logo);
  const bool pass = leftOk && rightOk && logoOk
    && nearlyEqual(left.pos.x, 21.89610f, 1e-4f)
    && nearlyEqual(right.pos.x, 66.63674f, 1e-4f)
    && nearlyEqual(left.pos.y, right.pos.y, 1e-4f)
    && nearlyEqual(left.size.x, right.size.x, 1e-4f)
    && nearlyEqual(left.size.y, right.size.y, 1e-4f)
    && nearlyEqual(left.pos.y + left.size.y, 128.5f, 1e-4f)
    && nearlyEqual(right.pos.y + right.size.y, 128.5f, 1e-4f)
    && nearlyEqual(logo.pos.x, 34.44015f, 1e-4f)
    && nearlyEqual(logo.pos.y, 119.43102f, 1e-4f)
    && nearlyEqual(logo.size.x, 32.71933f, 1e-4f)
    && nearlyEqual(logo.size.y, 12.24054f, 1e-4f)
    && nearlyEqual(logo.pos.x + logo.size.x * 0.5f, 50.799815f, 1e-4f);
  return {"Temporal Deck raster branding anchors match runtime layout", pass,
          "left=" + std::to_string(left.pos.x) + "," + std::to_string(left.pos.y) +
            " right=" + std::to_string(right.pos.x) + "," + std::to_string(right.pos.y) +
            " size=" + std::to_string(left.size.x) + "," + std::to_string(left.size.y) +
            " logo=" + std::to_string(logo.pos.x) + "," + std::to_string(logo.pos.y) +
            " logo_sz=" + std::to_string(logo.size.x) + "," + std::to_string(logo.size.y)};
}

TestResult testCompactLeviathanLogoBaseline() {
  const char* paths[] = {
    "res/Puffy.svg",
    "res/Puffy.panel.svg",
    "res/bifurx.svg",
    "res/bifurx.panel.svg",
    "res/deck.svg",
    "res/deck.panel.svg",
    "res/iris.svg",
    "res/iris.panel.svg",
    "res/mandelwake.svg",
    "res/mandelwake.panel.svg",
    "res/nautiloid.svg",
    "res/nautiloid.panel.svg",
    "res/wyrm.svg",
    "res/wyrm.panel.svg",
  };
  bool pass = true;
  std::string firstFailure;
  for (const char* path : paths) {
    math::Rect logo;
    const bool pathPass = panel_svg::loadRectFromSvgMm(
      path, "BRANDING_LEVIATHAN_LOGO_RASTER", &logo)
      && nearlyEqual(logo.pos.y, 119.43102f, 1e-4f)
      && nearlyEqual(logo.size.x, 32.71933f, 1e-4f)
      && nearlyEqual(logo.size.y, 12.24054f, 1e-4f);
    if (!pathPass && firstFailure.empty()) {
      firstFailure = path;
    }
    pass = pass && pathPass;
  }
  return {"Compact Leviathan logos share the Bifurx baseline", pass,
          "paths=" + std::to_string(sizeof(paths) / sizeof(paths[0]))
            + (firstFailure.empty() ? std::string() :
              " firstFailure=" + firstFailure)};
}

TestResult testPerfectWaveBrandingDeploymentContract() {
  struct PanelContract {
    const char* path;
    float widthMm;
    bool hasLeft;
    bool hasRight;
  };
  const PanelContract panels[] = {
    {"res/deck.panel.svg", 101.6f, true, true},
    {"res/deck.svg", 101.6f, true, true},
    {"res/bifurx.panel.svg", 71.12f, true, true},
    {"res/bifurx.svg", 71.12f, true, true},
    {"res/crownstep.panel.svg", 91.44f, true, true},
    {"res/crownstep.svg", 91.44f, true, true},
    {"res/wyrm.panel.svg", 71.12f, true, true},
    {"res/wyrm.svg", 71.12f, true, true},
    {"res/proc.panel.svg", 40.64f, true, false},
    {"res/proc.svg", 40.64f, true, false},
    {"res/iris.panel.svg", 60.96f, true, true},
    {"res/iris.svg", 60.96f, true, true},
    {"res/nautiloid.panel.svg", 101.6f, true, true},
    {"res/nautiloid.svg", 101.6f, true, true},
    {"res/undertow.panel.svg", 40.64f, true, false},
    {"res/undertow.svg", 40.64f, true, false},
    {"res/bulkhead.svg", 81.28f, true, true},
    {"res/chronomaw.svg", 203.2f, true, true},
    {"res/sil.svg", 101.6f, true, true},
    {"res/tdscope.svg", 40.64f, true, false},
  };

  bool pass = true;
  int anchorCount = 0;
  std::string firstFailure;
  for (const PanelContract& panel : panels) {
    math::Rect left;
    math::Rect right;
    const bool leftOk = panel_svg::loadRectFromSvgMm(
      panel.path, "BRANDING_WAVE_LEFT_RASTER", &left);
    const bool rightOk = panel_svg::loadRectFromSvgMm(
      panel.path, "BRANDING_WAVE_RIGHT_RASTER", &right);
    bool panelPass = leftOk == panel.hasLeft && rightOk == panel.hasRight;
    auto validAnchor = [&](const math::Rect& rect) {
      return nearlyEqual(rect.size.x, 13.06748f, 1e-4f)
        && nearlyEqual(rect.size.y, 5.11810f, 1e-4f)
        && nearlyEqual(rect.pos.y + rect.size.y, 128.5f, 1e-4f)
        && rect.pos.x >= -1e-4f
        && rect.pos.x + rect.size.x <= panel.widthMm + 1e-4f;
    };
    if (leftOk) {
      panelPass = panelPass && validAnchor(left);
      anchorCount++;
    }
    if (rightOk) {
      panelPass = panelPass && validAnchor(right);
      anchorCount++;
    }
    if (leftOk && rightOk) {
      panelPass = panelPass
        && nearlyEqual(
          left.pos.x + right.pos.x + left.size.x,
          panel.widthMm,
          5e-4f);
    }
    if (!panelPass && firstFailure.empty()) {
      firstFailure = panel.path;
    }
    pass = pass && panelPass;
  }

  math::Rect excluded;
  const bool fluxPanelExcluded = !panel_svg::loadRectFromSvgMm(
    "res/flux.panel.svg", "BRANDING_WAVE_LEFT_RASTER", &excluded);
  const bool fluxSourceExcluded = !panel_svg::loadRectFromSvgMm(
    "res/flux.svg", "BRANDING_WAVE_LEFT_RASTER", &excluded);
  pass = pass && fluxPanelExcluded && fluxSourceExcluded;

  return {"Perfect Wave branding deployment contract", pass,
          "panels=" + std::to_string(sizeof(panels) / sizeof(panels[0])) +
            " anchors=" + std::to_string(anchorCount) +
            " fluxExcluded=" + std::to_string(
              fluxPanelExcluded && fluxSourceExcluded ? 1 : 0) +
            (firstFailure.empty() ? std::string() :
              " firstFailure=" + firstFailure)};
}

TestResult testBifurxGlassPathParses() {
  std::vector<panel_svg::SvgPathMatch> matches;
  std::vector<panel_svg::SvgRectMatch> rects;
  bool ok = panel_svg::findThemeGlassPathsMm("res/bifurx.panel.svg", &matches);
  bool rectOk = panel_svg::findThemeGlassRectsMm("res/bifurx.panel.svg", &rects);
  const auto input = std::find_if(matches.begin(), matches.end(), [](const panel_svg::SvgPathMatch& match) {
    return match.id == "inputs";
  });
  const auto output = std::find_if(rects.begin(), rects.end(), [](const panel_svg::SvgRectMatch& match) {
    return match.id == "outputs-3";
  });
  const bool hasRoundedCurve = input != matches.end() && std::any_of(
    input->commands.begin(), input->commands.end(), [](const panel_svg::SvgPathCommand& command) {
      return command.type == panel_svg::SvgPathCommand::QuadTo ||
             command.type == panel_svg::SvgPathCommand::BezierTo;
    });
  const bool pass = ok && rectOk
    && input != matches.end()
    && input->themeRole == leviathan::theme::ThemeRole::Input
    && !input->commands.empty() && hasRoundedCurve
    && output != rects.end()
    && output->themeRole == leviathan::theme::ThemeRole::Output;
  return {"Bifurx inputs path is available to the glass renderer", pass,
          "ok=" + std::to_string(ok ? 1 : 0) +
            " count=" + std::to_string(matches.size()) +
            " inputs=" + std::to_string(input != matches.end() ? 1 : 0) +
            " output=" + std::to_string(output != rects.end() ? 1 : 0) +
            " rounded=" + std::to_string(hasRoundedCurve ? 1 : 0)};
}

TestResult testPlasmaConduitAnchorConvention() {
  struct PanelContract {
    const char* path;
    size_t expectedPaths;
  };
  const PanelContract panels[] = {
    {"res/bifurx.panel.svg", 5u},
    {"res/flux.panel.svg", 8u},
    {"res/undertow.panel.svg", 4u},
    {"res/deck.panel.svg", 5u},
    {"res/proc.panel.svg", 2u},
    {"res/wyrm.panel.svg", 6u},
    {"res/iris.panel.svg", 4u},
  };

  bool pass = true;
  size_t totalPaths = 0u;
  std::string firstFailure;
  for (const PanelContract& panel : panels) {
    std::vector<panel_svg::SvgPathMatch> paths;
    std::string svgText;
    const bool pathsOk = panel_svg::findPathsInGroupsWithIdSubstringMm(
      panel.path, "plasma_conduit_anchors", &paths);
    const bool textOk = readTextFile(panel.path, &svgText);
    const size_t idPos = svgText.find("id=\"plasma_conduit_anchors\"");
    const size_t groupStart = idPos == std::string::npos
      ? std::string::npos : svgText.rfind("<g", idPos);
    const size_t groupEnd = idPos == std::string::npos
      ? std::string::npos : svgText.find('>', idPos);
    const size_t hiddenPos = idPos == std::string::npos
      ? std::string::npos : svgText.find("display:none", idPos);
    const bool runtimeHidden = textOk
      && groupStart != std::string::npos
      && groupEnd != std::string::npos
      && hiddenPos != std::string::npos
      && hiddenPos < groupEnd;
    const bool straightPaths = std::all_of(
      paths.begin(), paths.end(), [](const panel_svg::SvgPathMatch& path) {
        return path.commands.size() == 2u
          && path.commands[0].type == panel_svg::SvgPathCommand::MoveTo
          && path.commands[1].type == panel_svg::SvgPathCommand::LineTo;
      });
    const bool panelPass = pathsOk
      && paths.size() == panel.expectedPaths
      && straightPaths
      && runtimeHidden;
    if (!panelPass && firstFailure.empty()) {
      firstFailure = panel.path;
    }
    totalPaths += paths.size();
    pass = pass && panelPass;
  }

  return {"Plasma conduit anchor groups provide hidden straight centerlines", pass,
          "panels=" + std::to_string(sizeof(panels) / sizeof(panels[0])) +
            " paths=" + std::to_string(totalPaths) +
            (firstFailure.empty() ? std::string() :
              " firstFailure=" + firstFailure)};
}

TestResult testThemeGlassDeploymentContract() {
  using leviathan::theme::ThemeRole;
  struct PanelContract {
    const char* path;
    size_t expectedInputs;
    size_t expectedOutputs;
  };
  const PanelContract panels[] = {
    {"res/bifurx.panel.svg", 1u, 1u},
    {"res/Sibyl.panel.svg", 1u, 1u},
    {"res/Puffy.panel.svg", 1u, 1u},
    {"res/crownstep.panel.svg", 1u, 1u},
    {"res/deck.panel.svg", 1u, 1u},
    {"res/doorstop.panel.svg", 1u, 1u},
    {"res/flux.panel.svg", 4u, 1u},
    {"res/iris.panel.svg", 1u, 1u},
    {"res/nautiloid.panel.svg", 1u, 1u},
    {"res/proc.panel.svg", 1u, 1u},
    {"res/undertow.panel.svg", 1u, 1u},
    {"res/wyrm.panel.svg", 1u, 1u},
  };

  bool pass = true;
  size_t totalInputs = 0u;
  size_t totalOutputs = 0u;
  std::string firstFailure;
  for (const PanelContract& panel : panels) {
    std::vector<panel_svg::SvgRectMatch> rects;
    std::vector<panel_svg::SvgPathMatch> paths;
    panel_svg::findThemeGlassRectsMm(panel.path, &rects);
    panel_svg::findThemeGlassPathsMm(panel.path, &paths);
    const auto countRole = [&](ThemeRole role) {
      return static_cast<size_t>(std::count_if(
               rects.begin(), rects.end(), [role](const panel_svg::SvgRectMatch& match) {
                 return match.themeRole == role;
               }))
        + static_cast<size_t>(std::count_if(
               paths.begin(), paths.end(), [role](const panel_svg::SvgPathMatch& match) {
                 return match.themeRole == role;
               }));
    };
    const size_t inputs = countRole(ThemeRole::Input);
    const size_t outputs = countRole(ThemeRole::Output);
    const size_t accents = countRole(ThemeRole::Accent);
    const size_t generic = countRole(ThemeRole::None);
    const bool panelPass = inputs == panel.expectedInputs
      && outputs == panel.expectedOutputs
      && accents == 0u
      && generic == 0u;
    if (!panelPass && firstFailure.empty()) {
      firstFailure = panel.path;
    }
    totalInputs += inputs;
    totalOutputs += outputs;
    pass = pass && panelPass;
  }

  return {"Split panels expose semantic THEME input/output glass roles", pass,
          "panels=" + std::to_string(sizeof(panels) / sizeof(panels[0]))
            + " inputs=" + std::to_string(totalInputs)
            + " outputs=" + std::to_string(totalOutputs)
            + (firstFailure.empty() ? std::string() :
              " firstFailure=" + firstFailure)};
}

TestResult testExactThemeGlassRoles() {
  using leviathan::theme::ThemeRole;
  std::vector<panel_svg::SvgRectMatch> rects;
  std::vector<panel_svg::SvgPathMatch> paths;
  const bool rectOk = panel_svg::findThemeGlassRectsMm(
    "tests/fixtures/theme_glass_roles.svg", &rects);
  const bool pathOk = panel_svg::findThemeGlassPathsMm(
    "tests/fixtures/theme_glass_roles.svg", &paths);
  auto rectRole = [&](const char* id, ThemeRole role) {
    const auto found = std::find_if(rects.begin(), rects.end(), [&](const panel_svg::SvgRectMatch& match) {
      return match.id == id;
    });
    return found != rects.end() && found->themeRole == role;
  };
  const auto nearMiss = std::find_if(rects.begin(), rects.end(), [](const panel_svg::SvgRectMatch& match) {
    return match.id == "near_miss";
  });
  const auto inputPath = std::find_if(paths.begin(), paths.end(), [](const panel_svg::SvgPathMatch& match) {
    return match.id == "input";
  });
  const bool pass = rectOk && pathOk
    && rectRole("generic", ThemeRole::None)
    && rectRole("input_nested", ThemeRole::Input)
    && rectRole("output", ThemeRole::Output)
    && rectRole("nearest_wins", ThemeRole::Accent)
    && rectRole("label_role", ThemeRole::Output)
    && nearMiss == rects.end()
    && inputPath != paths.end()
    && inputPath->themeRole == ThemeRole::Input;
  return {"Exact theme glass roles inherit from the nearest semantic ancestor", pass,
          "rects=" + std::to_string(rects.size()) +
            " paths=" + std::to_string(paths.size()) +
            " nearMiss=" + std::to_string(nearMiss != rects.end() ? 1 : 0)};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testRectParsesInMillimeters());
  tests.push_back(testPointParsesCircleCenter());
  tests.push_back(testPointParsesRectCenter());
  tests.push_back(testFindsRectsByIdSubstring());
  tests.push_back(testCircleParsesWithExplicitScale());
  tests.push_back(testMissingElementFailsGracefully());
  tests.push_back(testGeneratedAtlasFindsRealPanelAnchor());
  tests.push_back(testDeepcachePanelAnchorsUseRepositoryUnits());
  tests.push_back(testDoorstopSoloBrandingFitsThreeHp());
  tests.push_back(testTemporalDeckBrandingRasterAnchors());
  tests.push_back(testCompactLeviathanLogoBaseline());
  tests.push_back(testPerfectWaveBrandingDeploymentContract());
  tests.push_back(testBifurxGlassPathParses());
  tests.push_back(testPlasmaConduitAnchorConvention());
  tests.push_back(testThemeGlassDeploymentContract());
  tests.push_back(testExactThemeGlassRoles());

  int failed = 0;
  std::cout << "Panel SVG Utils Spec\n";
  std::cout << "--------------------\n";
  for (const auto& t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "--------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
