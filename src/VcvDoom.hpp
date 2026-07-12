#pragma once

#include "plugin.hpp"
#include <atomic>
#include <string>

struct VcvDoomModule : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		X_MOVE_INPUT,
		Y_MOVE_INPUT,
		FIRE_GATE_INPUT,
		WEAPON_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		HEALTH_OUTPUT,
		FRAG_TRIG_OUTPUT,
		AUDIO_L_OUTPUT,
		AUDIO_R_OUTPUT,
		MIDI_PITCH_OUTPUT,
		MIDI_GATE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	// WAD Configuration & State
	std::string wadPath = "";
	bool hasWad = false;
	std::string savedGameHex = "";

	// Dummy framebuffer for Phase 1 skeleton
	uint8_t dummyFramebuffer[320 * 200 * 4];
	std::atomic<bool> dirtyFrame{false};
	int fragTrigTime = 0;

	struct Voice {
		int note = -1;
		int channel = -1;
		float pitch = 0.f;
		float gate = 0.f;
	};
	Voice voices[16];
	int voiceTriggerCounter = 0;
	unsigned int musicGeneration = 0;

	// Focus state
	std::atomic<bool> isFocused{false};

	VcvDoomModule();
	~VcvDoomModule() override;

	void process(const ProcessArgs& args) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

	// WAD loading, validation, and settings persistence
	bool isEngineOwner() const;
	bool loadWad(const std::string& path, int startSlot = -1);
	void saveGlobalSettings();
	void loadGlobalSettings();

	void triggerExplicitSave();
	void triggerExplicitLoad();
};
