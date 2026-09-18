#pragma once
#include "SibylHarmonyTypes.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <limits>
#include <numeric>

namespace sibyl {
struct VoicingVoice {
  std::string track, pattern;
  int minimum = 0, maximum = 0;
};
struct VoicingBudget {
  size_t partials = 0, transitions = 0;
  size_t maxPartials = 200000, maxCandidates = 2048, maxTransitions = 2000000;
  bool exhausted = false;
  bool visit() {
    if (partials >= maxPartials) {
      exhausted = true;
      return false;
    }
    ++partials;
    return true;
  }
  bool transition() {
    if (transitions >= maxTransitions) {
      exhausted = true;
      return false;
    }
    ++transitions;
    return true;
  }
};
struct VoicingCandidate {
  std::array<int, 8> pitches{};
  int missing = 0;
};
struct VoicingSolution {
  bool valid = false;
  std::string code, message;
  std::vector<std::array<int, 8>> pitches;
  int missing = 0, maximumLeap = 0;
  int64_t movement = 0, squaredMovement = 0, initialDisplacementTwice = 0;
};
inline int voicingPitchClass(int pitch) { return (pitch % 12 + 12) % 12; }
inline int voicingPopcount(int bits) {
  int count = 0;
  for (; bits; bits &= bits - 1)
    ++count;
  return count;
}
inline int voicingLeap(const VoicingCandidate &a, const VoicingCandidate &b, size_t voices) {
  int leap = 0;
  for (size_t i = 0; i < voices; ++i)
    leap = std::max(leap, std::abs(a.pitches[i] - b.pitches[i]));
  return leap;
}

// Pure control-side solver. Candidate layers are in authored chord order;
// arrays use the declared low-to-high voice order, never unordered iteration.
inline VoicingSolution solveVoicing(std::vector<std::vector<VoicingCandidate>> layers,
                                    const std::vector<VoicingVoice> &voices, VoicingBudget &budget) {
  VoicingSolution result;
  auto fail = [&](const char *code, const char *message) {
    result.code = code;
    result.message = message;
    return result;
  };
  if (layers.empty() || voices.empty())
    return fail("voicing_unsatisfiable", "No chords or voices");
  // At unbounded leap, every pair of complete candidates can connect. Thus the
  // exact minimum coverage is the sum of each layer's minimum, with no DP edges
  // to evaluate. Discarding higher-coverage-cost candidates cannot remove any
  // solution to objective 1, including when finding the leap ceiling next.
  for (auto &layer : layers) {
    if (layer.empty())
      return fail("voicing_unsatisfiable", "No complete candidate meets register and coverage constraints");
    int missing = layer[0].missing;
    for (const auto &c : layer)
      missing = std::min(missing, c.missing);
    result.missing += missing;
    layer.erase(
        std::remove_if(layer.begin(), layer.end(), [&](const VoicingCandidate &c) { return c.missing != missing; }),
        layer.end());
    std::sort(layer.begin(), layer.end(),
              [](const VoicingCandidate &a, const VoicingCandidate &b) { return a.pitches < b.pitches; });
  }
  auto feasible = [&](int ceiling) {
    std::vector<uint8_t> reachable(layers[0].size(), 1);
    for (size_t chord = 1; chord < layers.size(); ++chord) {
      std::vector<uint8_t> next(layers[chord].size(), 0);
      bool any = false;
      for (size_t j = 0; j < next.size(); ++j)
        for (size_t i = 0; i < reachable.size(); ++i)
          if (reachable[i]) {
            if (!budget.transition())
              return false;
            if (voicingLeap(layers[chord - 1][i], layers[chord][j], voices.size()) <= ceiling) {
              next[j] = 1;
              any = true;
              break;
            }
          }
      if (!any)
        return false;
      reachable = std::move(next);
    }
    return true;
  };
  int low = -1, high = 240; // Validated pitch domain is -120..120 semitones.
  if (layers.size() == 1)
    high = 0;
  while (high - low > 1) {
    int middle = (low + high) / 2;
    bool possible = feasible(middle);
    if (budget.exhausted)
      return fail("capacity_exceeded", "DP transition budget exhausted while finding leap ceiling");
    if (possible)
      high = middle;
    else
      low = middle;
  }
  result.maximumLeap = high;
  struct Cost {
    bool valid = false;
    std::array<int64_t, 3> value{};
    int rank = 0;
  };
  std::vector<Cost> previous(layers[0].size());
  for (size_t i = 0; i < previous.size(); ++i) {
    previous[i].valid = true;
    previous[i].rank = int(i);
    for (size_t v = 0; v < voices.size(); ++v)
      previous[i].value[2] += std::abs(2 * layers[0][i].pitches[v] - voices[v].minimum - voices[v].maximum);
  }
  std::vector<std::vector<int>> parents(layers.size());
  for (size_t chord = 1; chord < layers.size(); ++chord) {
    std::vector<Cost> next(layers[chord].size());
    parents[chord].assign(next.size(), -1);
    for (size_t j = 0; j < next.size(); ++j)
      for (size_t i = 0; i < previous.size(); ++i)
        if (previous[i].valid) {
          if (!budget.transition())
            return fail("capacity_exceeded", "DP transition budget exhausted while optimizing movement");
          const auto &from = layers[chord - 1][i];
          const auto &to = layers[chord][j];
          if (voicingLeap(from, to, voices.size()) > high)
            continue;
          auto cost = previous[i].value;
          for (size_t v = 0; v < voices.size(); ++v) {
            int delta = std::abs(to.pitches[v] - from.pitches[v]);
            cost[0] += delta;
            cost[1] += delta * delta;
          }
          int parent = parents[chord][j];
          if (!next[j].valid || cost < next[j].value ||
              (cost == next[j].value && previous[i].rank < previous[parent].rank)) {
            next[j].valid = true;
            next[j].value = cost;
            parents[chord][j] = int(i);
          }
        }
    // Rank whole selected prefixes, not just the current chord. Rank ties are
    // exact lexicographic pitch-sequence ties for the final objective.
    std::vector<int> order;
    for (size_t j = 0; j < next.size(); ++j)
      if (next[j].valid)
        order.push_back(int(j));
    std::sort(order.begin(), order.end(), [&](int a, int b) {
      int ra = previous[parents[chord][a]].rank, rb = previous[parents[chord][b]].rank;
      return ra != rb ? ra < rb : layers[chord][a].pitches < layers[chord][b].pitches;
    });
    for (size_t rank = 0; rank < order.size(); ++rank)
      next[order[rank]].rank = int(rank);
    previous = std::move(next);
  }
  int best = -1;
  for (size_t i = 0; i < previous.size(); ++i)
    if (previous[i].valid && (best < 0 || previous[i].value < previous[best].value ||
                              (previous[i].value == previous[best].value && previous[i].rank < previous[best].rank)))
      best = int(i);
  if (best < 0)
    return fail("voicing_unsatisfiable", "No path meets minimum leap ceiling");
  result.movement = previous[best].value[0];
  result.squaredMovement = previous[best].value[1];
  result.initialDisplacementTwice = previous[best].value[2];
  result.pitches.resize(layers.size());
  for (size_t chord = layers.size(); chord-- > 0;) {
    result.pitches[chord] = layers[chord][best].pitches;
    if (chord)
      best = parents[chord][best];
  }
  result.valid = true;
  return result;
}

inline VoicingSolution voiceProgression(const HarmonyProgression &progression, const std::vector<VoicingVoice> &voices,
                                        VoicingBudget &budget) {
  std::vector<std::vector<VoicingCandidate>> layers;
  for (const auto &chord : progression.chords) {
    std::vector<std::vector<int>> candidates(voices.size());
    int available = 0, thirdMask = 0, thirds = 0;
    for (int interval : chord.intervals) {
      available |= 1 << voicingPitchClass(chord.rootSemitone + interval);
      if (interval % 12 == 3 || interval % 12 == 4) {
        thirdMask = 1 << voicingPitchClass(chord.rootSemitone + interval);
        ++thirds;
      }
    }
    int required = 1 << voicingPitchClass(chord.rootSemitone);
    if (voices.size() >= 2 && thirds == 1)
      required |= thirdMask;
    for (size_t v = 0; v < voices.size(); ++v)
      for (int pitch = voices[v].minimum; pitch <= voices[v].maximum; ++pitch)
        if (available & (1 << voicingPitchClass(pitch)))
          candidates[v].push_back(pitch);
    std::vector<VoicingCandidate> layer;
    VoicingCandidate current;
    std::function<void(size_t, int)> enumerate = [&](size_t voice, int coverage) {
      if (budget.exhausted || !budget.visit())
        return;
      if (voice == voices.size()) {
        if ((coverage & required) != required)
          return;
        if (layer.size() >= budget.maxCandidates) {
          budget.exhausted = true;
          return;
        }
        current.missing = voicingPopcount(available & ~coverage);
        layer.push_back(current);
        return;
      }
      for (int pitch : candidates[voice]) {
        if (voice && pitch < current.pitches[voice - 1])
          continue;
        current.pitches[voice] = pitch;
        enumerate(voice + 1, coverage | (1 << voicingPitchClass(pitch)));
        if (budget.exhausted)
          return;
      }
    };
    enumerate(0, 0);
    if (budget.exhausted) {
      VoicingSolution result;
      result.code = "capacity_exceeded";
      result.message = "Candidate budget exhausted at chord " + chord.id;
      return result;
    }
    if (layer.empty()) {
      VoicingSolution result;
      result.code = "voicing_unsatisfiable";
      result.message = "Register/root/third coverage impossible at chord " + chord.id;
      return result;
    }
    layers.push_back(std::move(layer));
  }
  return solveVoicing(std::move(layers), voices, budget);
}
} // namespace sibyl
