#include "SibylAdoption.hpp"
#include "SibylHarmony.hpp"

#include <cmath>
#include <set>
#include <unordered_map>

namespace sibyl {

void prepareAutomationAdoption(const Composition* previous, const Composition& next, AdoptionRequest& request) {
    size_t count=std::max(size_t(1),std::max(previous?previous->arrangement.size():size_t(0),next.arrangement.size()));
    request.automationChangeMasks.assign(count,0);request.rawModResetMasks.assign(count,0);
    auto curve=[](const Composition* comp,size_t scene,int lane)->const AutomationCurve* {
        if(!comp || scene>=comp->automationRoutes.size())return nullptr;
        int index=comp->automationRoutes[scene][lane];return index<0?nullptr:&comp->automation[index];
    };
    auto pattern=[](const Composition* comp,size_t scene,int channel)->std::string {
        if(!comp || scene>=comp->arrangement.size())return {};
        for(const auto& track:comp->tracks)if(track.channel==channel){
            auto assignment=comp->arrangement[scene].tracks.find(track.id);
            if(assignment!=comp->arrangement[scene].tracks.end())return assignment->second.patternId;
        }
        return {};
    };
    for(size_t scene=0;scene<count;++scene) {
        for(int channel=0;channel<16;++channel)
            if(pattern(previous,scene,channel)!=pattern(&next,scene,channel))request.rawModResetMasks[scene]|=uint16_t(1u<<channel);
        for(int lane=0;lane<48;++lane) {
            const auto* a=curve(previous,scene,lane);const auto* b=curve(&next,scene,lane);
            bool changed=(a==nullptr)!=(b==nullptr) || (a && b && !a->audibleEquals(*b));
            if(a && b && previous && scene<previous->arrangement.size() && scene<next.arrangement.size()) {
                changed |= sceneTimelineLength(previous->arrangement[scene])!=sceneTimelineLength(next.arrangement[scene]) ||
                    previous->arrangement[scene].repeats!=next.arrangement[scene].repeats;
                if(b->clock==AutomationClock::ARRANGEMENT)
                    changed |= previous->sceneBeatPrefixes[scene]!=next.sceneBeatPrefixes[scene];
            }
            if(changed)request.automationChangeMasks[scene]|=uint64_t(1)<<lane;
        }
    }
}

bool parseApplyAtName(const std::string& name, ApplyAt& value) {
	if (name == "immediate") value = ApplyAt::IMMEDIATE;
	else if (name == "nextStep") value = ApplyAt::NEXT_STEP;
	else if (name == "nextBeat") value = ApplyAt::NEXT_BEAT;
	else if (name == "nextScene") value = ApplyAt::NEXT_SCENE;
	else return false;
	return true;
}

const char* applyAtName(ApplyAt value) {
	switch (value) {
		case ApplyAt::IMMEDIATE: return "immediate";
		case ApplyAt::NEXT_STEP: return "nextStep";
		case ApplyAt::NEXT_SCENE: return "nextScene";
		case ApplyAt::NEXT_BEAT: return "nextBeat";
	}
	return "nextBeat";
}

bool parsePhasePolicyName(const std::string& name, PhasePolicy& value) {
	if (name == "preserve") value = PhasePolicy::PRESERVE;
	else if (name == "restartChanged") value = PhasePolicy::RESTART_CHANGED;
	else if (name == "restartAll") value = PhasePolicy::RESTART_ALL;
	else return false;
	return true;
}

const char* phasePolicyName(PhasePolicy value) {
	switch (value) {
		case PhasePolicy::PRESERVE: return "preserve";
		case PhasePolicy::RESTART_CHANGED: return "restartChanged";
		case PhasePolicy::RESTART_ALL: return "restartAll";
	}
	return "preserve";
}

ChannelAdoptionAction channelAdoptionAction(PhasePolicy policy, bool channelChanged) {
	ChannelAdoptionAction action;
	action.restartPhase = policy == PhasePolicy::RESTART_ALL ||
		(policy == PhasePolicy::RESTART_CHANGED && channelChanged);
	action.closeGate = channelChanged || action.restartPhase;
	action.cancelGlide = channelChanged || action.restartPhase;
	return action;
}

double preservedPatternPhase(double elapsedBeats, double replacementDurationBeats) {
	if (!std::isfinite(elapsedBeats) || elapsedBeats < 0.0) return 0.0;
	if (!std::isfinite(replacementDurationBeats) || replacementDurationBeats <= 0.0) return elapsedBeats;
	double phase = std::fmod(elapsedBeats, replacementDurationBeats);
	return phase < 0.0 ? phase + replacementDurationBeats : phase;
}

static bool sameEvent(const StepEvent& a, const StepEvent& b) {
	return a.pitchOffsets == b.pitchOffsets && a.condition == b.condition && a.evolve == b.evolve && a.step == b.step && a.pitchType == b.pitchType && a.pitchV == b.pitchV &&
		a.harmonic == b.harmonic && a.degree == b.degree && a.note == b.note && a.octave == b.octave &&
		a.hasGate == b.hasGate && a.gate == b.gate &&
		a.hasVelocity == b.hasVelocity && a.velocity == b.velocity &&
		a.hasMod == b.hasMod && a.mod == b.mod &&
		a.hasMod2 == b.hasMod2 && a.mod2 == b.mod2 &&
		a.hasMod3 == b.hasMod3 && a.mod3 == b.mod3 &&
		a.transposeSemitones == b.transposeSemitones && a.hasProbability == b.hasProbability && a.probability == b.probability &&
		a.tie == b.tie && a.glideMs == b.glideMs && a.microshift == b.microshift &&
		a.ratchets == b.ratchets && a.compiledPitchV == b.compiledPitchV;
}

static bool samePattern(const Pattern& a, const Pattern& b) {
	if (!(a.evolution == b.evolution) || a.length != b.length || a.resolutionStr != b.resolutionStr || a.steps.size() != b.steps.size()) return false;
	for (size_t i = 0; i < a.steps.size(); ++i) if (!sameEvent(a.steps[i], b.steps[i])) return false;
	return true;
}

static bool sameTrack(const TrackDef& a, const TrackDef& b) {
	return a.id == b.id && a.channel == b.channel && a.defaultGate == b.defaultGate &&
		a.defaultVelocity == b.defaultVelocity;
}

static void collectAssignments(const Composition& composition, const std::string& trackId,
		std::unordered_map<std::string, TrackAssignment>& assignments, std::set<std::string>& patterns) {
	for (const Scene& scene : composition.arrangement) {
		auto found = scene.tracks.find(trackId);
		TrackAssignment value;
		if (found != scene.tracks.end()) {
			value = found->second;
			if (!found->second.patternId.empty()) patterns.insert(found->second.patternId);
		}
		assignments[scene.id] = value;
	}
}

template <typename T>
static void appendHarmonyScalar(std::string& key, const T& value) {
    key.append(reinterpret_cast<const char*>(&value), sizeof(value));
}

static std::vector<std::string> harmonyDependencies(const Composition& comp,const std::string& trackId) {
    std::vector<std::string> result;
    for(size_t s=0;s<comp.arrangement.size();++s){const auto& scene=comp.arrangement[s];
        auto a=scene.tracks.find(trackId);if(a==scene.tracks.end())continue;
        auto p=comp.patterns.find(a->second.patternId);if(p==comp.patterns.end())continue;
        bool harmonic=false;for(const auto& event:p->second.steps)harmonic|=event.pitchType==PitchType::HARMONIC;
        if(!harmonic)continue;
        const auto* b=effectiveHarmony(comp,s);
        if(!b)continue;
        const auto& progression=comp.progressions[b->compiledProgression];
        // Exact binary doubles avoid rounding small musical-coordinate edits.
        std::string key=scene.id;
        appendHarmonyScalar(key,b->clock);appendHarmonyScalar(key,b->loop);appendHarmonyScalar(key,progression.length);double length=sceneTimelineLength(scene);appendHarmonyScalar(key,length);appendHarmonyScalar(key,scene.repeats);
        if(b->clock==AutomationClock::ARRANGEMENT)appendHarmonyScalar(key,comp.sceneBeatPrefixes[s]);
        for(const auto& c:progression.chords){appendHarmonyScalar(key,c.periodV); for(const auto& tone:c.tones) { key+=tone.id;key.push_back(0);appendHarmonyScalar(key,tone.pitch.baseV); for(const auto& role:tone.roles) { key+=role;key.push_back(0); } } appendHarmonyScalar(key,c.beat);appendHarmonyScalar(key,c.rootSemitone);size_t n=c.intervals.size();appendHarmonyScalar(key,n);for(int interval:c.intervals)appendHarmonyScalar(key,interval);}
        for(const auto& e:p->second.steps)if(e.pitchType==PitchType::HARMONIC){const auto& expression=comp.harmonicExpressions[e.harmonicExpression];appendHarmonyScalar(key,expression.reference);}
        result.push_back(std::move(key));
    }
    return result;
}

uint16_t changedTrackChannelMask(const Composition& previous, const Composition& next) {
	uint16_t mask = 0;
	std::unordered_map<std::string, const TrackDef*> previousTracks;
	std::unordered_map<std::string, const TrackDef*> nextTracks;
	for (const TrackDef& track : previous.tracks) previousTracks[track.id] = &track;
	for (const TrackDef& track : next.tracks) nextTracks[track.id] = &track;

	for (const auto& entry : previousTracks) {
		if (!nextTracks.count(entry.first) && entry.second->channel >= 0 && entry.second->channel < 16)
			mask |= uint16_t(1u << entry.second->channel);
	}
	for (const auto& entry : nextTracks) {
		const TrackDef& nextTrack = *entry.second;
		bool changed = harmonyDependencies(previous,entry.first)!=harmonyDependencies(next,entry.first);
		auto oldTrack = previousTracks.find(entry.first);
		if (oldTrack == previousTracks.end() || !sameTrack(*oldTrack->second, nextTrack)) changed = true;

		std::unordered_map<std::string, TrackAssignment> oldAssignments;
		std::unordered_map<std::string, TrackAssignment> newAssignments;
		std::set<std::string> oldPatterns;
		std::set<std::string> newPatterns;
		collectAssignments(previous, entry.first, oldAssignments, oldPatterns);
		collectAssignments(next, entry.first, newAssignments, newPatterns);
		if (oldAssignments != newAssignments) changed = true;

		std::set<std::string> referencedPatterns = oldPatterns;
		referencedPatterns.insert(newPatterns.begin(), newPatterns.end());
		for (const std::string& patternId : referencedPatterns) {
			auto oldPattern = previous.patterns.find(patternId);
			auto newPattern = next.patterns.find(patternId);
			if (oldPattern == previous.patterns.end() || newPattern == next.patterns.end() ||
				!samePattern(oldPattern->second, newPattern->second)) {
				changed = true;
				break;
			}
		}

		if (changed && nextTrack.channel >= 0 && nextTrack.channel < 16)
			mask |= uint16_t(1u << nextTrack.channel);
	}
	return mask;
}

} // namespace sibyl
