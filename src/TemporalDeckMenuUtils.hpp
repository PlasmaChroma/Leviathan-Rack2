#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace temporaldeck_menu {

struct ArtBatch {
  int begin = 0;
  int endExclusive = 0;
  std::string label;
};

inline std::vector<ArtBatch> buildArtBatches(int totalEntries, int chunkSize) {
  std::vector<ArtBatch> batches;
  int total = std::max(0, totalEntries);
  int chunk = std::max(1, chunkSize);
  if (total <= chunk) {
    return batches;
  }

  for (int start = 0; start < total; start += chunk) {
    int end = std::min(start + chunk, total);
    bool isFinalPartial = (end == total) && ((end - start) < chunk);
    std::string label =
      isFinalPartial ? ("Art " + std::to_string(start + 1) + "+") : ("Art " + std::to_string(start + 1) + "-" + std::to_string(end));
    ArtBatch batch;
    batch.begin = start;
    batch.endExclusive = end;
    batch.label = label;
    batches.push_back(batch);
  }
  return batches;
}

struct SubmenuItem {
  int menuId = -1;
  std::string submenu;
  int index = -1;
};

struct SubmenuGroup {
  std::string label;
  std::vector<int> indices;
  int firstMenuId = -1;
};

struct SubmenuLayout {
  std::vector<int> rootIndices;
  std::vector<SubmenuGroup> groups;
};

inline SubmenuLayout buildSubmenuLayout(const std::vector<SubmenuItem> &items) {
  SubmenuLayout layout;
  std::vector<SubmenuItem> sorted = items;
  std::sort(sorted.begin(), sorted.end(), [](const SubmenuItem &a, const SubmenuItem &b) {
    if (a.menuId != b.menuId) {
      return a.menuId < b.menuId;
    }
    return a.index < b.index;
  });

  std::map<std::string, size_t> groupIndexByLabel;
  for (const SubmenuItem &item : sorted) {
    if (item.index < 0) {
      continue;
    }
    if (item.submenu.empty()) {
      layout.rootIndices.push_back(item.index);
      continue;
    }
    auto it = groupIndexByLabel.find(item.submenu);
    if (it == groupIndexByLabel.end()) {
      SubmenuGroup group;
      group.label = item.submenu;
      group.firstMenuId = item.menuId;
      layout.groups.push_back(group);
      groupIndexByLabel[item.submenu] = layout.groups.size() - 1;
      it = groupIndexByLabel.find(item.submenu);
    }
    SubmenuGroup &group = layout.groups[it->second];
    if (group.firstMenuId < 0 || (item.menuId >= 0 && item.menuId < group.firstMenuId)) {
      group.firstMenuId = item.menuId;
    }
    group.indices.push_back(item.index);
  }

  std::sort(layout.groups.begin(), layout.groups.end(), [](const SubmenuGroup &a, const SubmenuGroup &b) {
    if (a.firstMenuId != b.firstMenuId) {
      return a.firstMenuId < b.firstMenuId;
    }
    return a.label < b.label;
  });
  return layout;
}

} // namespace temporaldeck_menu
