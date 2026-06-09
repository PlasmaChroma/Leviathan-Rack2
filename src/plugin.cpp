#include "plugin.hpp"
#include "BifurxWorker.hpp"

#include <atomic>
#include <fstream>

Plugin* pluginInstance;
static std::atomic<bool> gDragonKingDebugEnabled{false};
static std::atomic<bool> gClockworkDragDebugLoggingEnabled{false};

void refreshDragonKingDebugEnabled() {
	gDragonKingDebugEnabled.store(false, std::memory_order_relaxed);
	gClockworkDragDebugLoggingEnabled.store(false, std::memory_order_relaxed);
	if (!pluginInstance) {
		return;
	}
	const std::string flagPath = asset::plugin(pluginInstance, "res/dragonking.txt");
	std::ifstream flagFile(flagPath);
	if (!flagFile.good()) {
		return;
	}
	json_error_t error;
	json_t* root = json_load_file(flagPath.c_str(), 0, &error);
	if (!root) {
		gDragonKingDebugEnabled.store(true, std::memory_order_relaxed);
		return;
	}
	if (json_is_object(root)) {
		json_t* debugJ = json_object_get(root, "debug");
		json_t* clockworkDragLoggingJ = json_object_get(root, "clockworkDragLogging");
		gDragonKingDebugEnabled.store(!debugJ || json_boolean_value(debugJ), std::memory_order_relaxed);
		gClockworkDragDebugLoggingEnabled.store(json_boolean_value(clockworkDragLoggingJ), std::memory_order_relaxed);
	}
	json_decref(root);
}

bool isDragonKingDebugEnabled() {
	return gDragonKingDebugEnabled.load(std::memory_order_relaxed);
}

bool isClockworkDragDebugLoggingEnabled() {
	return gClockworkDragDebugLoggingEnabled.load(std::memory_order_relaxed);
}


void init(Plugin* p) {
	pluginInstance = p;
	refreshDragonKingDebugEnabled();

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelIntegralFlux);
	p->addModel(modelProc);
	p->addModel(modelTemporalDeck);
	p->addModel(modelTDScope);
	p->addModel(modelUndertow);
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}

void destroy() {
	// Explicit plugin-lifecycle shutdown avoids static-destruction order hazards across TUs.
	bifurx::shutdownBifurxRenderService();
}
