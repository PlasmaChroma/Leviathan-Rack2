#pragma once

#include "SibylTypes.hpp"

#include <cstdint>
#include <string>

namespace sibyl {

enum class PhasePolicy { PRESERVE, RESTART_CHANGED, RESTART_ALL };

struct AdoptionRequest {
	const Composition* composition = nullptr;
	ApplyAt applyAt = ApplyAt::NEXT_BEAT;
	PhasePolicy phasePolicy = PhasePolicy::PRESERVE;
	uint16_t restartChannelMask = 0;
};

bool parseApplyAtName(const std::string& name, ApplyAt& value);
const char* applyAtName(ApplyAt value);
bool parsePhasePolicyName(const std::string& name, PhasePolicy& value);
const char* phasePolicyName(PhasePolicy value);

// Computed off the audio thread when a revision is accepted. The mask is
// deliberately conservative: a channel restarts if its declaration, any scene
// assignment, or any pattern it may play changed.
uint16_t changedTrackChannelMask(const Composition& previous, const Composition& next);

struct BoundaryState {
	bool step = false;
	bool beat = false;
	bool scene = false;
};

inline bool adoptionBoundaryReached(ApplyAt applyAt, const BoundaryState& boundary) {
	switch (applyAt) {
		case ApplyAt::IMMEDIATE: return true;
		case ApplyAt::NEXT_STEP: return boundary.step;
		case ApplyAt::NEXT_SCENE: return boundary.scene;
		case ApplyAt::NEXT_BEAT: return boundary.beat;
	}
	return false;
}

} // namespace sibyl
