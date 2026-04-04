#pragma once

#include <algorithm>
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

} // namespace temporaldeck_menu

