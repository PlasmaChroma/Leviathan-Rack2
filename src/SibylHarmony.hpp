#pragma once
#include "SibylTypes.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace sibyl {
inline const HarmonyBinding *effectiveHarmony(const Composition &comp, size_t scene) {
  if (scene >= comp.arrangement.size())
    return nullptr;
  const auto &local = comp.arrangement[scene].harmony;
  const auto &binding = local.present ? local : comp.defaultHarmony;
  return binding.present && !binding.disabled && binding.compiledProgression >= 0 ? &binding : nullptr;
}
inline double harmonyCoordinate(const Composition &comp, size_t scene, int repeat, double beat,
                                const HarmonyBinding &binding) {
  const double length = sceneTimelineLength(comp.arrangement[scene]);
  const double visit = std::max(0., double(repeat) * length + beat);
  if (binding.clock == AutomationClock::ARRANGEMENT)
    return comp.sceneBeatPrefixes[scene] + visit;
  if (binding.clock == AutomationClock::SCENE_VISIT)
    return visit;
  // An anticipated onset may belong to the preceding repeat.
  if (beat < 0.)
    return beat + length;
  return beat;
}
inline size_t harmonyChordIndex(const HarmonyProgression &progression, double coordinate, bool loop) {
  double t = std::max(0., coordinate);
  if (loop)
    t -= std::floor(t / progression.length) * progression.length;
  size_t lo = 0, hi = progression.chords.size();
  while (lo < hi) {
    size_t mid = (lo + hi) / 2;
    if (progression.chords[mid].beat <= t)
      lo = mid + 1;
    else
      hi = mid;
  }
  return lo ? lo - 1 : 0;
}
inline float harmonicSelectedPitch(const Composition &comp, const StepEvent &event, size_t scene, int repeat,
                                   double beat) {
  const auto *binding = effectiveHarmony(comp, scene);
  if (!binding || event.harmonicExpression < 0)
    return std::numeric_limits<float>::quiet_NaN();
  const auto &progression = comp.progressions[binding->compiledProgression];
  int chord =
    progression
      .chords[harmonyChordIndex(progression, harmonyCoordinate(comp, scene, repeat, beat, *binding), binding->loop)]
      .definition;
  const auto &entries = comp.harmonicExpressions[event.harmonicExpression].pitches;
  auto found = std::lower_bound(entries.begin(), entries.end(), chord,
                                [](const HarmonicExpression::Pitch &p, int key) { return p.chord < key; });
  return found != entries.end() && found->chord == chord ? found->volts : std::numeric_limits<float>::quiet_NaN();
}
inline float contextualPitch(const Composition &comp, const StepEvent &event, size_t scene, int repeat, double beat) {
  return event.pitchType == PitchType::HARMONIC
           ? (event.pitchOffsets.fields ? float(double(harmonicSelectedPitch(comp,event,scene,repeat,beat))+double(event.transposeSemitones)/12.+event.pitchOffsets.periods+event.pitchOffsets.cents/1200.) : harmonicSelectedPitch(comp, event, scene, repeat, beat) + event.transposeSemitones / 12.f)
           : event.compiledPitchV;
}
// Assignment-specific native results include every event and scene transform.
inline float sceneEventPitch(const Composition& comp,const StepEvent& event,const TrackAssignment& assignment,size_t scene,int repeat,double beat) {
  if(event.pitchType==PitchType::HARMONIC && assignment.harmonicPitches) {
    const auto* binding=effectiveHarmony(comp,scene);
    if(!binding) return std::numeric_limits<float>::quiet_NaN();
    const auto& p=comp.progressions[binding->compiledProgression];
    int chord=p.chords[harmonyChordIndex(p,harmonyCoordinate(comp,scene,repeat,beat,*binding),binding->loop)].definition;
    const auto& table=*assignment.harmonicPitches;
    HarmonicRoutePitch key{event.step,chord,0.f};
    auto it=std::lower_bound(table.begin(),table.end(),key,[](const HarmonicRoutePitch& a,const HarmonicRoutePitch& b){return a.step==b.step?a.chord<b.chord:a.step<b.step;});
    if(it!=table.end()&&it->step==event.step&&it->chord==chord) return it->volts;
  }
  return assignedPitch(event,assignment,contextualPitch(comp,event,scene,repeat,beat));
}
} // namespace sibyl
