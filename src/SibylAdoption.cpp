#include "SibylAdoption.hpp"

#include <cmath>
#include <set>
#include <unordered_map>

namespace sibyl {

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
	return a.step == b.step && a.pitchType == b.pitchType && a.pitchV == b.pitchV &&
		a.degree == b.degree && a.note == b.note && a.octave == b.octave &&
		a.hasGate == b.hasGate && a.gate == b.gate &&
		a.hasVelocity == b.hasVelocity && a.velocity == b.velocity &&
		a.hasMod == b.hasMod && a.mod == b.mod &&
		a.hasProbability == b.hasProbability && a.probability == b.probability &&
		a.tie == b.tie && a.glideMs == b.glideMs && a.microshift == b.microshift &&
		a.ratchets == b.ratchets && a.compiledPitchV == b.compiledPitchV;
}

static bool samePattern(const Pattern& a, const Pattern& b) {
	if (a.length != b.length || a.resolutionStr != b.resolutionStr || a.steps.size() != b.steps.size()) return false;
	for (size_t i = 0; i < a.steps.size(); ++i) if (!sameEvent(a.steps[i], b.steps[i])) return false;
	return true;
}

static bool sameTrack(const TrackDef& a, const TrackDef& b) {
	return a.id == b.id && a.channel == b.channel && a.defaultGate == b.defaultGate &&
		a.defaultVelocity == b.defaultVelocity && a.modRange == b.modRange;
}

static void collectAssignments(const Composition& composition, const std::string& trackId,
		std::unordered_map<std::string, std::string>& assignments, std::set<std::string>& patterns) {
	for (const Scene& scene : composition.arrangement) {
		auto found = scene.tracks.find(trackId);
		std::string value;
		if (found != scene.tracks.end()) {
			value = found->second.patternId;
			if (found->second.hasPhaseModeOverride)
				value += "#" + std::to_string(static_cast<int>(found->second.phaseModeOverride));
			if (!found->second.patternId.empty()) patterns.insert(found->second.patternId);
		}
		assignments[scene.id] = value;
	}
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
		bool changed = false;
		auto oldTrack = previousTracks.find(entry.first);
		if (oldTrack == previousTracks.end() || !sameTrack(*oldTrack->second, nextTrack)) changed = true;

		std::unordered_map<std::string, std::string> oldAssignments;
		std::unordered_map<std::string, std::string> newAssignments;
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
