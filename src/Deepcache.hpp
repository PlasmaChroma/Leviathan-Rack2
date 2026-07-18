#pragma once

#include "DeepcachePlanner.hpp"
#include "plugin.hpp"

#include <atomic>
#include <cstdint>

struct DeepcacheModule : rack::engine::Module {
	enum ParamIds {
		NUM_PARAMS
	};

	enum InputIds {
		NUM_INPUTS
	};

	enum OutputIds {
		NUM_OUTPUTS
	};

	enum LightIds {
		PLANNING_LIGHT,
		WARMING_LIGHT,
		READY_LIGHT,
		ERROR_LIGHT,
		NUM_LIGHTS
	};

	std::atomic<int> cacheState {static_cast<int>(deepcache::CacheState::DISABLED)};
	std::atomic<int> completedCount {0};
	std::atomic<int> totalCount {0};
	std::atomic<int> failedCount {0};
	std::atomic<int> constructionPluginCompletedCount {0};
	std::atomic<int> constructionPluginTotalCount {0};
	std::atomic<int> framebufferPluginCompletedCount {0};
	std::atomic<int> framebufferPluginTotalCount {0};
	std::atomic<int> databaseState {0};
	std::atomic<int> databaseReadyPluginCount {0};
	std::atomic<int> databaseTargetPluginCount {0};
	std::atomic<std::uint64_t> databaseBytes {0};
	std::atomic<int> databaseErrorCode {0};
	std::atomic<bool> browserStandby {false};
	std::atomic<bool> autoStart {true};
	std::atomic<int> uiBudgetMicros {2000};
	std::atomic<int> cacheScope {static_cast<int>(deepcache::CacheScope::ALL)};

	DeepcacheModule();
	void process(const ProcessArgs& args) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;

};

struct DeepcacheWidget : rack::app::ModuleWidget {
	explicit DeepcacheWidget(DeepcacheModule* module);
	~DeepcacheWidget() override;
	void step() override;
	void appendContextMenu(rack::ui::Menu* menu) override;

private:
	struct Internal;
	Internal* internal_ = nullptr;
};

extern Model* modelDeepcache;
