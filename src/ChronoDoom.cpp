#include "ChronoDoom.hpp"
#include <cstdio>
#include <fstream>

ChronoDoomModule::ChronoDoomModule() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

	// Config inputs
	configInput(X_MOVE_INPUT, "X-Move (Strafe)");
	configInput(Y_MOVE_INPUT, "Y-Move (Forward/Backward)");
	configInput(FIRE_GATE_INPUT, "Fire Gate");
	configInput(WEAPON_CV_INPUT, "Weapon CV");

	// Config outputs
	configOutput(HEALTH_OUTPUT, "Health (0-10V)");
	configOutput(FRAG_TRIG_OUTPUT, "Frag Trigger");
	configOutput(AUDIO_L_OUTPUT, "Audio L");
	configOutput(AUDIO_R_OUTPUT, "Audio R");

	// Initialize dummy framebuffer to static noise / test grid
	for (int y = 0; y < 200; ++y) {
		for (int x = 0; x < 320; ++x) {
			int idx = (y * 320 + x) * 4;
			dummyFramebuffer[idx + 0] = 0;   // R
			dummyFramebuffer[idx + 1] = 0;   // G
			dummyFramebuffer[idx + 2] = 0;   // B
			dummyFramebuffer[idx + 3] = 255; // A
		}
	}

	loadGlobalSettings();
}

ChronoDoomModule::~ChronoDoomModule() {
}

void ChronoDoomModule::process(const ProcessArgs& args) {
	// Outputs default to 0V when inactive
	outputs[HEALTH_OUTPUT].setVoltage(0.f);
	outputs[FRAG_TRIG_OUTPUT].setVoltage(0.f);
	outputs[AUDIO_L_OUTPUT].setVoltage(0.f);
	outputs[AUDIO_R_OUTPUT].setVoltage(0.f);

	if (hasWad) {
		// Render a simple animated pattern in dummy framebuffer for verification
		static float accumTime = 0.f;
		accumTime += args.sampleTime;
		if (accumTime >= 1.f / 35.f) {
			accumTime = 0.f;
			static int frameOffset = 0;
			frameOffset = (frameOffset + 1) % 320;

			for (int y = 0; y < 200; ++y) {
				for (int x = 0; x < 320; ++x) {
					int idx = (y * 320 + x) * 4;
					// Draw a moving gradient pattern
					dummyFramebuffer[idx + 0] = (x + frameOffset) % 256;         // R
					dummyFramebuffer[idx + 1] = y % 256;                        // G
					dummyFramebuffer[idx + 2] = ((x + y) / 2 + frameOffset) % 256; // B
					dummyFramebuffer[idx + 3] = 255;                            // A
				}
			}
			dirtyFrame.store(true);
		}
	}
}

bool ChronoDoomModule::loadWad(const std::string& path) {
	if (path.empty()) {
		return false;
	}

	// Phase 1 validation: file must exist and be readable
	std::ifstream file(path, std::ios::binary);
	if (!file.good()) {
		return false;
	}

	// Basic WAD header check: First 4 bytes must be "IWAD" or "PWAD"
	char header[4];
	file.read(header, 4);
	if (file.gcount() < 4) {
		return false;
	}

	std::string magic(header, 4);
	if (magic != "IWAD" && magic != "PWAD") {
		return false;
	}

	wadPath = path;
	hasWad = true;
	dirtyFrame.store(true);

	saveGlobalSettings();
	return true;
}

void ChronoDoomModule::saveGlobalSettings() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "wadPath", json_string(wadPath.c_str()));

	const std::string dir = system::join(asset::user(), "Leviathan");
	system::createDirectories(dir);
	const std::string path = system::join(dir, "chronodoom.json");
	FILE* file = std::fopen(path.c_str(), "w");
	if (file) {
		json_dumpf(rootJ, file, JSON_INDENT(2));
		std::fclose(file);
	}
	json_decref(rootJ);
}

void ChronoDoomModule::loadGlobalSettings() {
	const std::string dir = system::join(asset::user(), "Leviathan");
	const std::string path = system::join(dir, "chronodoom.json");
	FILE* file = std::fopen(path.c_str(), "r");
	if (!file) {
		return;
	}
	json_error_t error;
	json_t* rootJ = json_loadf(file, 0, &error);
	std::fclose(file);
	if (!rootJ) {
		return;
	}

	json_t* pathJ = json_object_get(rootJ, "wadPath");
	if (pathJ && json_is_string(pathJ)) {
		std::string savedPath = json_string_value(pathJ);
		if (!loadWad(savedPath)) {
			WARN("ChronoDoom: Saved WAD path '%s' failed validation", savedPath.c_str());
		}
	}
	json_decref(rootJ);
}
