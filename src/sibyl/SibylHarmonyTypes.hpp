#pragma once
#include "SibylAutomation.hpp"
#include "SibylTuning.hpp"
#include <string>
#include <vector>

namespace sibyl {
struct HarmonyBinding {
  bool present = false, disabled = false, loop = true;
  std::string progression;
  AutomationClock clock = AutomationClock::ARRANGEMENT;
  int compiledProgression = -1;
};
struct NativeChordTone {
  std::string id; std::vector<std::string> roles; NativePitch pitch;
};
struct HarmonicRoutePitch {
  int step, chord; float volts;
  bool operator==(const HarmonicRoutePitch& b) const { return step==b.step && chord==b.chord && volts==b.volts; }
};
struct HarmonyChord {
  std::string authored, context; double periodV=1.;
  std::vector<NativeChordTone> tones;
  std::string id, root;
  double beat = 0.;
  int rootSemitone = 0, definition = -1;
  std::vector<int> intervals;
};
struct HarmonyProgression {
  std::string id, pitchContext;
  double length = 0.;
  std::vector<HarmonyChord> chords;
};
struct HarmonicExpression {
  std::string roleName, toneId;
  bool extended=false, hasPeriods=false; int32_t periods=0;
  double referenceV=0., minimumV=-10., maximumV=10.;
  std::string authored; // Canonical JSON, control-side only.
  bool nearest = false, higher = false;
  int index = 0, octave = 0, role = 0, minimum = -120, maximum = 120;
  double reference = 0.; // Semitones relative to C4.
  struct Pitch {
    int chord;
    float volts;
  };
  std::vector<Pitch> pitches; // Sorted by interned chord definition, reachable contexts only.
};
} // namespace sibyl
