#include "plugin.hpp"

#include <atomic>
#include <fstream>

Plugin* pluginInstance;
static std::atomic<bool> gDragonKingDebugEnabled{false};

void refreshDragonKingDebugEnabled() {
	if (!pluginInstance) {
		gDragonKingDebugEnabled.store(false, std::memory_order_relaxed);
		return;
	}
	const std::string flagPath = asset::plugin(pluginInstance, "res/dragonking.txt");
	std::ifstream flagFile(flagPath);
	gDragonKingDebugEnabled.store(flagFile.good(), std::memory_order_relaxed);
}

bool isDragonKingDebugEnabled() {
	return gDragonKingDebugEnabled.load(std::memory_order_relaxed);
}


void init(Plugin* p) {
	pluginInstance = p;
	refreshDragonKingDebugEnabled();

	auto registerModel = [&](Model* model, const char* label) {
		if (!model) {
			WARN("Leviathan init: model %s is null", label);
			return;
		}
		p->addModel(model);
		INFO("Leviathan init: registered %s (slug=%s)", label, model->slug.c_str());
	};

	// Add modules here
	// p->addModel(modelMyModule);
	registerModel(modelIntegralFlux, "Integral Flux");
	registerModel(modelProc, "Proc");
	registerModel(modelTemporalDeck, "Temporal Deck");
	registerModel(modelTDScope, "TD.Scope");
	registerModel(modelCrownstep, "Crownstep");
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
