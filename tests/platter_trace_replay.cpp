#include "platter_trace_replay.hpp"

#include "../src/TemporalDeckTest.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace spec {

namespace {

bool parseDouble(const std::string &s, double *out) {
  try {
    size_t idx = 0;
    double v = std::stod(s, &idx);
    if (idx != s.size()) {
      return false;
    }
    *out = v;
    return true;
  } catch (...) {
    return false;
  }
}

std::string trimToken(const std::string &s) {
  size_t start = 0;
  size_t end = s.size();
  while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) {
    ++start;
  }
  while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
    --end;
  }
  if (end >= start + 3 && static_cast<unsigned char>(s[start]) == 0xEF &&
      static_cast<unsigned char>(s[start + 1]) == 0xBB && static_cast<unsigned char>(s[start + 2]) == 0xBF) {
    start += 3;
  }
  return s.substr(start, end - start);
}

std::vector<std::string> splitCsvLine(const std::string &line) {
  std::vector<std::string> out;
  std::stringstream ss(line);
  std::string token;
  while (std::getline(ss, token, ',')) {
    out.push_back(token);
  }
  return out;
}

} // namespace

bool replayTraceFile(const std::string &path, std::ostream &out) {
  std::ifstream f(path);
  if (!f.good()) {
    out << "Replay error: unable to open trace file: " << path << "\n";
    return false;
  }

  int totalRows = 0;
  int scratchApplyRows = 0;
  int wheelRows = 0;
  double lastTimeSec = 0.0;
  double lastRecordedLag = 0.0;
  bool hasRecordedLag = false;

  bool hasReplayLag = false;
  double replayLag = 0.0;
  double maxAbsErr = 0.0;
  double errSqSum = 0.0;
  int errCount = 0;

  std::string line;
  std::map<std::string, int> headerIndex;
  int idxTime = 1;
  int idxEvent = 2;
  int idxFreeze = 3;
  int idxLagDelta = 12;
  int idxLiveLag = 13;
  int idxLocalLag = 14;

  auto assignIndex = [&](const std::string &name, int *dst, int fallback) {
    auto it = headerIndex.find(name);
    *dst = (it != headerIndex.end()) ? it->second : fallback;
  };

  while (std::getline(f, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::vector<std::string> cols = splitCsvLine(line);
    if (cols.size() < 18) {
      continue;
    }
    if (trimToken(cols[0]) == "seq") {
      headerIndex.clear();
      for (int i = 0; i < int(cols.size()); ++i) {
        headerIndex[trimToken(cols[i])] = i;
      }
      assignIndex("t_sec", &idxTime, idxTime);
      assignIndex("event", &idxEvent, idxEvent);
      assignIndex("freeze", &idxFreeze, idxFreeze);
      assignIndex("lag_delta", &idxLagDelta, idxLagDelta);
      assignIndex("live_lag", &idxLiveLag, idxLiveLag);
      assignIndex("local_lag", &idxLocalLag, idxLocalLag);
      continue;
    }

    int requiredIdx = std::max({idxTime, idxEvent, idxFreeze, idxLagDelta, idxLiveLag, idxLocalLag});
    if (requiredIdx >= int(cols.size())) {
      continue;
    }

    double tSec = 0.0;
    double freezeFlag = 0.0;
    double lagDelta = 0.0;
    double liveLag = 0.0;
    double localLag = 0.0;
    if (!parseDouble(cols[idxTime], &tSec) || !parseDouble(cols[idxFreeze], &freezeFlag) ||
        !parseDouble(cols[idxLagDelta], &lagDelta) || !parseDouble(cols[idxLiveLag], &liveLag) ||
        !parseDouble(cols[idxLocalLag], &localLag)) {
      continue;
    }

    totalRows++;
    lastTimeSec = std::max(lastTimeSec, tSec);
    lastRecordedLag = localLag;
    hasRecordedLag = true;

    const std::string eventName = trimToken(cols[idxEvent]);
    if (eventName == "WHEEL") {
      wheelRows++;
      continue;
    }
    if (eventName != "SCRATCH_APPLY") {
      continue;
    }

    scratchApplyRows++;
    if (!hasReplayLag) {
      replayLag = localLag;
      hasReplayLag = true;
      continue;
    }

    if (freezeFlag < 0.5) {
      replayLag = platter_interaction::rebaseLagTarget(float(replayLag), float(liveLag), float(lagDelta));
    }
    replayLag = std::max(0.0, replayLag - lagDelta);

    double absErr = std::fabs(replayLag - localLag);
    maxAbsErr = std::max(maxAbsErr, absErr);
    errSqSum += absErr * absErr;
    errCount++;
  }

  out << "TemporalDeck Trace Replay\n";
  out << "-------------------------\n";
  out << "File: " << path << "\n";
  out << "Rows: " << totalRows << " (scratch_apply=" << scratchApplyRows << ", wheel=" << wheelRows << ")\n";
  out << "Duration: " << lastTimeSec << " s\n";
  if (hasRecordedLag) {
    out << "Final recorded local_lag: " << lastRecordedLag << " samples\n";
  }
  if (errCount > 0) {
    double rmsErr = std::sqrt(errSqSum / double(errCount));
    out << "Replay lag error on scratch_apply: max=" << maxAbsErr << " samples, rms=" << rmsErr << " samples\n";
  } else {
    out << "Replay lag error on scratch_apply: n/a (insufficient scratch_apply rows)\n";
  }
  out << "-------------------------\n";
  return totalRows > 0;
}

} // namespace spec
