#include "platter_trace_replay.hpp"

#include "../src/TemporalDeckTest.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace spec {

namespace {

std::string trim(std::string s) {
  size_t b = 0;
  while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) {
    b++;
  }
  size_t e = s.size();
  while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) {
    e--;
  }
  return s.substr(b, e - b);
}

bool parseDouble(const std::string &s, double *out) {
  try {
    std::string t = trim(s);
    if (t.empty()) {
      return false;
    }
    size_t idx = 0;
    double v = std::stod(t, &idx);
    if (idx != t.size()) {
      return false;
    }
    *out = v;
    return true;
  } catch (...) {
    return false;
  }
}

std::vector<std::string> splitCsvLine(const std::string &line) {
  std::vector<std::string> out;
  std::stringstream ss(line);
  std::string token;
  while (std::getline(ss, token, ',')) {
    out.push_back(trim(token));
  }
  return out;
}

typedef std::map<std::string, size_t> ColMap;

ColMap buildColMap(const std::vector<std::string> &cols) {
  ColMap map;
  for (size_t i = 0; i < cols.size(); ++i) {
    if (!cols[i].empty()) {
      map[cols[i]] = i;
    }
  }
  return map;
}

int colIndex(const std::vector<std::string> &cols, const ColMap *colMap, const std::string &name, int fallbackIndex) {
  if (colMap) {
    ColMap::const_iterator it = colMap->find(name);
    if (it != colMap->end()) {
      return int(it->second);
    }
  }
  if (fallbackIndex >= 0 && fallbackIndex < int(cols.size())) {
    return fallbackIndex;
  }
  return -1;
}

bool readStringField(const std::vector<std::string> &cols, const ColMap *colMap, const std::string &name, int fallbackIndex,
                     std::string *out) {
  int idx = colIndex(cols, colMap, name, fallbackIndex);
  if (idx < 0) {
    return false;
  }
  *out = cols[size_t(idx)];
  return true;
}

bool readDoubleField(const std::vector<std::string> &cols, const ColMap *colMap, const std::string &name, int fallbackIndex,
                     double *out) {
  int idx = colIndex(cols, colMap, name, fallbackIndex);
  if (idx < 0) {
    return false;
  }
  return parseDouble(cols[size_t(idx)], out);
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
  bool parsedHeader = false;
  ColMap headerCols;

  bool hasReplayLag = false;
  double replayLag = 0.0;
  double maxAbsErr = 0.0;
  double errSqSum = 0.0;
  int errCount = 0;

  const double kAngleEps = 0.005;
  const double kLagEps = 1e-3;
  int forwardApplyRows = 0;
  int reverseApplyRows = 0;
  int forwardNoMoveRows = 0;
  int reverseNoMoveRows = 0;
  int forwardNoMoveNowClampRows = 0;
  int reverseNoMoveBackClampRows = 0;
  int nowClampRows = 0;
  int backClampRows = 0;
  int currentNowRun = 0;
  int longestNowRun = 0;
  int currentBackRun = 0;
  int longestBackRun = 0;
  bool hasPrevApplyLag = false;
  double prevApplyLag = 0.0;

  std::string line;
  while (std::getline(f, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::vector<std::string> cols = splitCsvLine(line);
    if (cols.empty()) {
      continue;
    }

    if (cols[0] == "seq") {
      headerCols = buildColMap(cols);
      parsedHeader = true;
      continue;
    }

    if (cols.size() < 3) {
      continue;
    }

    const ColMap *colMap = parsedHeader ? &headerCols : nullptr;

    std::string eventName;
    double tSec = 0.0;
    double freezeFlag = 0.0;
    double lagDelta = 0.0;
    double liveLag = 0.0;
    double localLag = 0.0;
    double accessibleLag = std::numeric_limits<double>::quiet_NaN();
    double deltaAngle = 0.0;
    if (!readStringField(cols, colMap, "event", 2, &eventName) || !readDoubleField(cols, colMap, "t_sec", 1, &tSec) ||
        !readDoubleField(cols, colMap, "freeze", 3, &freezeFlag) ||
        !readDoubleField(cols, colMap, "lag_delta", 12, &lagDelta) ||
        !readDoubleField(cols, colMap, "live_lag", 13, &liveLag) ||
        !readDoubleField(cols, colMap, "local_lag", 14, &localLag)) {
      continue;
    }
    readDoubleField(cols, colMap, "accessible_lag", 21, &accessibleLag);
    readDoubleField(cols, colMap, "delta_angle", 17, &deltaAngle);

    totalRows++;
    lastTimeSec = std::max(lastTimeSec, tSec);
    lastRecordedLag = localLag;
    hasRecordedLag = true;

    if (eventName == "WHEEL") {
      wheelRows++;
      continue;
    }
    if (eventName != "SCRATCH_APPLY") {
      continue;
    }

    scratchApplyRows++;
    bool atNowClamp = localLag <= kLagEps;
    bool hasAccessibleLag = std::isfinite(accessibleLag);
    bool atBackClamp = hasAccessibleLag && (accessibleLag - localLag) <= kLagEps;
    if (atNowClamp) {
      nowClampRows++;
      currentNowRun++;
      longestNowRun = std::max(longestNowRun, currentNowRun);
    } else {
      currentNowRun = 0;
    }
    if (atBackClamp) {
      backClampRows++;
      currentBackRun++;
      longestBackRun = std::max(longestBackRun, currentBackRun);
    } else {
      currentBackRun = 0;
    }

    if (hasPrevApplyLag) {
      double localLagDelta = prevApplyLag - localLag; // positive == forward motion toward NOW
      if (deltaAngle > kAngleEps) {
        forwardApplyRows++;
        if (localLagDelta <= kLagEps) {
          forwardNoMoveRows++;
          if (atNowClamp) {
            forwardNoMoveNowClampRows++;
          }
        }
      } else if (deltaAngle < -kAngleEps) {
        reverseApplyRows++;
        if (localLagDelta >= -kLagEps) {
          reverseNoMoveRows++;
          if (atBackClamp) {
            reverseNoMoveBackClampRows++;
          }
        }
      }
    }
    prevApplyLag = localLag;
    hasPrevApplyLag = true;

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
  out << "Header parsing: " << (parsedHeader ? "by-name (v2)" : "fallback positional") << "\n";
  if (hasRecordedLag) {
    out << "Final recorded local_lag: " << lastRecordedLag << " samples\n";
  }
  if (errCount > 0) {
    double rmsErr = std::sqrt(errSqSum / double(errCount));
    out << "Replay lag error on scratch_apply: max=" << maxAbsErr << " samples, rms=" << rmsErr << " samples\n";
  } else {
    out << "Replay lag error on scratch_apply: n/a (insufficient scratch_apply rows)\n";
  }
  out << "Directional diagnostics:\n";
  out << "  forward apply rows: " << forwardApplyRows << ", no-move: " << forwardNoMoveRows
      << " (now-clamp subset: " << forwardNoMoveNowClampRows << ")\n";
  out << "  reverse apply rows: " << reverseApplyRows << ", no-move: " << reverseNoMoveRows
      << " (back-clamp subset: " << reverseNoMoveBackClampRows << ")\n";
  out << "Clamp diagnostics:\n";
  out << "  now clamp rows: " << nowClampRows << ", longest run: " << longestNowRun << " scratch_apply events\n";
  out << "  back clamp rows: " << backClampRows << ", longest run: " << longestBackRun << " scratch_apply events\n";
  out << "-------------------------\n";
  return totalRows > 0;
}

} // namespace spec
