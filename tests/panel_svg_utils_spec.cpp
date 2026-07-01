#include "../src/PanelSvgUtils.hpp"
#include "../src/PanelAnchorAtlas.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
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

TestResult testBifurxGlassPathParses() {
  std::vector<panel_svg::SvgPathMatch> matches;
  bool ok = panel_svg::findPathsInGroupsWithIdSubstringMm("res/bifurx.panel.svg", "glass", &matches);
  const auto input = std::find_if(matches.begin(), matches.end(), [](const panel_svg::SvgPathMatch& match) {
    return match.id == "inputs";
  });
  const bool hasRoundedCurve = input != matches.end() && std::any_of(
    input->commands.begin(), input->commands.end(), [](const panel_svg::SvgPathCommand& command) {
      return command.type == panel_svg::SvgPathCommand::QuadTo;
    });
  const bool pass = ok && input != matches.end() && !input->commands.empty() && hasRoundedCurve;
  return {"Bifurx inputs path is available to the glass renderer", pass,
          "ok=" + std::to_string(ok ? 1 : 0) +
            " count=" + std::to_string(matches.size()) +
            " inputs=" + std::to_string(input != matches.end() ? 1 : 0) +
            " rounded=" + std::to_string(hasRoundedCurve ? 1 : 0)};
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
  tests.push_back(testBifurxGlassPathParses());

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
