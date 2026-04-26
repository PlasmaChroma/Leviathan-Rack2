#include "../src/TemporalDeckMenuUtils.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

using temporaldeck_menu::SubmenuGroup;
using temporaldeck_menu::SubmenuItem;

struct TestResult {
  std::string name;
  bool pass = false;
  std::string detail;
};

std::string joinLabels(const std::vector<SubmenuGroup> &groups) {
  std::string out;
  for (size_t i = 0; i < groups.size(); ++i) {
    if (i > 0) {
      out += ", ";
    }
    out += groups[i].label;
  }
  return out;
}

TestResult testItemsWithoutSubmenuStayAtRoot() {
  std::vector<SubmenuItem> items;
  SubmenuItem a;
  a.menuId = 20;
  a.submenu = "";
  a.index = 0;
  items.push_back(a);
  SubmenuItem b;
  b.menuId = 10;
  b.submenu = "";
  b.index = 1;
  items.push_back(b);

  temporaldeck_menu::SubmenuLayout layout = temporaldeck_menu::buildSubmenuLayout(items);
  bool pass = layout.groups.empty() && layout.rootIndices.size() == 2 && layout.rootIndices[0] == 1 &&
              layout.rootIndices[1] == 0;
  return {"Items with no submenu stay in root list", pass,
          "rootCount=" + std::to_string(layout.rootIndices.size()) + " groupCount=" + std::to_string(layout.groups.size())};
}

TestResult testSubmenuGroupingByLabelAndMenuIdOrder() {
  std::vector<SubmenuItem> items;
  SubmenuItem i0;
  i0.menuId = 30;
  i0.submenu = "Drums";
  i0.index = 0;
  items.push_back(i0);
  SubmenuItem i1;
  i1.menuId = 10;
  i1.submenu = "Bass";
  i1.index = 1;
  items.push_back(i1);
  SubmenuItem i2;
  i2.menuId = 20;
  i2.submenu = "Drums";
  i2.index = 2;
  items.push_back(i2);
  SubmenuItem i3;
  i3.menuId = 40;
  i3.submenu = "";
  i3.index = 3;
  items.push_back(i3);

  temporaldeck_menu::SubmenuLayout layout = temporaldeck_menu::buildSubmenuLayout(items);
  bool pass = layout.rootIndices.size() == 1 && layout.rootIndices[0] == 3 && layout.groups.size() == 2 &&
              layout.groups[0].label == "Bass" && layout.groups[1].label == "Drums" &&
              layout.groups[0].indices.size() == 1 && layout.groups[0].indices[0] == 1 &&
              layout.groups[1].indices.size() == 2 && layout.groups[1].indices[0] == 2 &&
              layout.groups[1].indices[1] == 0;
  return {"Submenu groups sorted by first menuId; items sorted by menuId", pass, "labels=[" + joinLabels(layout.groups) + "]"};
}

TestResult testSubmenuLabelWhitespaceIsHandledByCaller() {
  std::vector<SubmenuItem> items;
  SubmenuItem i0;
  i0.menuId = 5;
  i0.submenu = "FX";
  i0.index = 0;
  items.push_back(i0);
  SubmenuItem i1;
  i1.menuId = 6;
  i1.submenu = "";
  i1.index = 1;
  items.push_back(i1);

  temporaldeck_menu::SubmenuLayout layout = temporaldeck_menu::buildSubmenuLayout(items);
  bool pass = layout.groups.size() == 1 && layout.groups[0].label == "FX" && layout.rootIndices.size() == 1 &&
              layout.rootIndices[0] == 1;
  return {"Submenu layout supports mixed grouped/ungrouped entries", pass, "labels=[" + joinLabels(layout.groups) + "]"};
}

TestResult testExplicitSubmenuOrderOverridesMenuIdOrder() {
  std::vector<SubmenuItem> items;
  SubmenuItem i0;
  i0.menuId = 10;
  i0.submenu = "Core";
  i0.submenuOrder = 20;
  i0.index = 0;
  items.push_back(i0);
  SubmenuItem i1;
  i1.menuId = 50;
  i1.submenu = "Expansion";
  i1.submenuOrder = 5;
  i1.index = 1;
  items.push_back(i1);
  SubmenuItem i2;
  i2.menuId = 15;
  i2.submenu = "Core";
  i2.submenuOrder = 20;
  i2.index = 2;
  items.push_back(i2);

  temporaldeck_menu::SubmenuLayout layout = temporaldeck_menu::buildSubmenuLayout(items);
  bool pass = layout.groups.size() == 2 && layout.groups[0].label == "Expansion" && layout.groups[1].label == "Core";
  return {"Explicit submenuOrder controls submenu ordering", pass, "labels=[" + joinLabels(layout.groups) + "]"};
}

} // namespace

int main() {
  std::vector<TestResult> tests;
  tests.push_back(testItemsWithoutSubmenuStayAtRoot());
  tests.push_back(testSubmenuGroupingByLabelAndMenuIdOrder());
  tests.push_back(testSubmenuLabelWhitespaceIsHandledByCaller());
  tests.push_back(testExplicitSubmenuOrderOverridesMenuIdOrder());

  int failed = 0;
  std::cout << "TemporalDeck Menu Utils Spec\n";
  std::cout << "----------------------------\n";
  for (const auto &t : tests) {
    std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
    if (!t.pass) {
      failed++;
    }
  }
  std::cout << "----------------------------\n";
  std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}
