#pragma once

#include "DeepcachePlanner.hpp"
#include "plugin.hpp"

#include <atomic>
#include <cstdint>

struct DeepcacheModule : rack::engine::Module {
	enum ParamIds {
		CACHE_PARAM,
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
	std::atomic<int> constructionCompletedCount {0};
	std::atomic<int> constructionTotalCount {0};
	std::atomic<int> framebufferCompletedCount {0};
	std::atomic<int> framebufferTotalCount {0};
	std::atomic<int> databaseState {0};
	std::atomic<int> databaseReadyCount {0};
	std::atomic<int> databaseTargetCount {0};
	std::atomic<std::uint64_t> databaseBytes {0};
	std::atomic<int> databaseErrorCode {0};
	std::atomic<std::uint64_t> buttonPressSerial {0};
	std::atomic<bool> autoStart {true};
	std::atomic<int> uiBudgetMicros {2000};
	std::atomic<int> cacheScope {static_cast<int>(deepcache::CacheScope::ALL)};
	std::atomic<bool> experimentalFramebufferWarm {true};

	DeepcacheModule();
	void process(const ProcessArgs& args) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;

private:
	bool cacheButtonHigh_ = false;
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
