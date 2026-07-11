#include "ChronoDoom.hpp"
#include <cstdio>
#include <fstream>
#include <thread>

extern "C" {
	void D_DoomMain(void);
	void I_SetTargetRGBA(uint8_t *buffer);
	extern int doom_exit_requested;
	extern volatile int doom_engine_status;
	extern char doom_engine_error[256];
	extern volatile int doom_dirty_frame;
	void W_Shutdown(void);
	extern int myargc;
	extern char** myargv;
#include "doom/i_sound.h"
#include "doom/midifile.h"
	extern volatile float g_cv_xmove;
	extern volatile float g_cv_ymove;
	extern volatile int g_cv_fire;
	extern volatile int g_cv_weapon;
	extern volatile int g_game_health;
	extern volatile int g_game_frag_trigger;
}

// Single-instance engine owner
static ChronoDoomModule* gDoomModuleOwner = nullptr;
static std::thread gDoomThread;

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
	configOutput(MIDI_PITCH_OUTPUT, "MIDI Pitch");
	configOutput(MIDI_GATE_OUTPUT, "MIDI Gate");

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

	if (gDoomModuleOwner == nullptr) {
		gDoomModuleOwner = this;
	}

	loadGlobalSettings();
}

ChronoDoomModule::~ChronoDoomModule() {
	if (gDoomModuleOwner == this) {
		doom_exit_requested = 1;
		if (gDoomThread.joinable()) {
			gDoomThread.join();
		}
		W_Shutdown();
		gDoomModuleOwner = nullptr;
	}
}

bool ChronoDoomModule::isEngineOwner() const {
	return gDoomModuleOwner == this;
}

void ChronoDoomModule::process(const ProcessArgs& args) {
	float outL = 0.f;
	float outR = 0.f;

	if (gDoomModuleOwner == this && hasWad && !isBypassed()) {
		// 1. Process CV Inputs
		g_cv_xmove = inputs[X_MOVE_INPUT].getNormalVoltage(0.f);
		g_cv_ymove = inputs[Y_MOVE_INPUT].getNormalVoltage(0.f);
		g_cv_fire = (inputs[FIRE_GATE_INPUT].getNormalVoltage(0.f) >= 1.f) ? 1 : 0;

		if (inputs[WEAPON_CV_INPUT].isConnected()) {
			float v = inputs[WEAPON_CV_INPUT].getVoltage();
			int w = (int)(v + 0.5f);
			if (w < 0) w = 0;
			if (w > 6) w = 6;
			g_cv_weapon = w;
		} else {
			g_cv_weapon = -1;
		}

		// 2. Process CV Outputs
		float healthVolt = (float)g_game_health / 100.f * 10.f;
		outputs[HEALTH_OUTPUT].setVoltage(clamp(healthVolt, 0.f, 10.f));

		if (g_game_frag_trigger) {
			g_game_frag_trigger = 0;
			fragTrigTime = (int)(0.010f * args.sampleRate); // 10ms pulse
		}

		if (fragTrigTime > 0) {
			outputs[FRAG_TRIG_OUTPUT].setVoltage(10.f);
			fragTrigTime--;
		} else {
			outputs[FRAG_TRIG_OUTPUT].setVoltage(0.f);
		}

		// 3. Process Audio Resampling & Mixing
		for (int c = 0; c < MIXER_CHANNELS; ++c) {
			mixer_channel_t& chan = g_mixer_channels[c];
			if (chan.active && chan.data) {
				float pos = chan.pos;
				uint32_t idx = (uint32_t)pos;

				if (idx < chan.length) {
					// Read samples for linear interpolation
					float sample0 = (float)chan.data[idx] - 128.f;
					float sample1 = 0.f;
					if (idx + 1 < chan.length) {
						sample1 = (float)chan.data[idx + 1] - 128.f;
					}

					// Linear interpolation
					float frac = pos - (float)idx;
					float interp = sample0 + frac * (sample1 - sample0);

					// Scale to [-1.f, 1.f]
					interp /= 128.f;

					// Apply channel volume (vol is 0-15) and master scale
					float volume = (float)chan.vol / 15.f;
					float sampleVal = interp * volume * 0.5f;

					// Apply panning (sep is 0-255)
					float pan = (float)chan.sep / 255.f;
					float panL = 1.f - pan;
					float panR = pan;

					outL += sampleVal * panL * 2.f;
					outR += sampleVal * panR * 2.f;

					// Step position
					float step = ((float)chan.src_rate / args.sampleRate) * chan.pitch_factor;
					chan.pos += step;
				} else {
					chan.active = 0;
				}
			}
		}

		// Clamp mixed audio
		if (outL < -1.f) outL = -1.f;
		if (outL > 1.f) outL = 1.f;
		if (outR < -1.f) outR = -1.f;
		if (outR > 1.f) outR = 1.f;

		// 4. Process MIDI Music Sequencer
		if (g_music_playing && g_active_midi_iter) {
			double deltaSecs = (double)args.sampleTime;
			double deltaTicks = deltaSecs * g_music_ticks_per_sec;
			g_music_ticks += deltaTicks;

			while (g_music_ticks >= g_next_event_tick) {
				midi_event_t* event = nullptr;
				if (MIDI_GetNextEvent((midi_track_iter_t*)g_active_midi_iter, &event)) {
					if (event->event_type == MIDI_EVENT_NOTE_ON) {
						int channel = event->data.channel.channel;
						int note = event->data.channel.param1;
						int velocity = event->data.channel.param2;

						if (velocity == 0) {
							// Note Off
							for (int i = 0; i < 16; ++i) {
								if (voices[i].note == note && voices[i].channel == channel) {
									voices[i].gate = 0.f;
									voices[i].note = -1;
								}
							}
						} else {
							// Note On
							int targetVoice = -1;
							for (int i = 0; i < 16; ++i) {
								if (voices[i].note == note && voices[i].channel == channel) {
									targetVoice = i;
									break;
								}
							}
							if (targetVoice < 0) {
								for (int i = 0; i < 16; ++i) {
									if (voices[i].note == -1) {
										targetVoice = i;
										break;
									}
								}
							}
							if (targetVoice < 0) {
								targetVoice = voiceTriggerCounter % 16;
								voiceTriggerCounter++;
							}

							voices[targetVoice].note = note;
							voices[targetVoice].channel = channel;
							voices[targetVoice].pitch = (float)(note - 60) / 12.f;
							voices[targetVoice].gate = 10.f;
						}
					} else if (event->event_type == MIDI_EVENT_NOTE_OFF) {
						int channel = event->data.channel.channel;
						int note = event->data.channel.param1;
						for (int i = 0; i < 16; ++i) {
							if (voices[i].note == note && voices[i].channel == channel) {
								voices[i].gate = 0.f;
								voices[i].note = -1;
							}
						}
					} else if (event->event_type == MIDI_EVENT_META && event->data.meta.type == MIDI_META_SET_TEMPO) {
						unsigned int tempo = (event->data.meta.data[0] << 16) | (event->data.meta.data[1] << 8) | event->data.meta.data[2];
						if (tempo > 0) {
							g_tempo = tempo;
							g_music_ticks_per_sec = (1000000.0 / g_tempo) * g_time_division;
						}
					}

					unsigned int delta = MIDI_GetDeltaTime((midi_track_iter_t*)g_active_midi_iter);
					g_next_event_tick += delta;
				} else {
					if (g_music_looping) {
						// A MUS/MIDI loop can end with active notes. Clear them before
						// restarting so a missing note-off cannot leave a gate latched.
						for (int i = 0; i < 16; ++i) {
							voices[i].gate = 0.f;
							voices[i].note = -1;
						}
						MIDI_RestartIterator((midi_track_iter_t*)g_active_midi_iter);
						g_music_ticks = 0.0;
						g_next_event_tick = MIDI_GetDeltaTime((midi_track_iter_t*)g_active_midi_iter);
					} else {
						g_music_playing = 0;
						for (int i = 0; i < 16; ++i) {
							voices[i].gate = 0.f;
							voices[i].note = -1;
						}
						break;
					}
				}
			}
		} else {
			for (int i = 0; i < 16; ++i) {
				voices[i].gate = 0.f;
				voices[i].note = -1;
			}
		}

		// 5. Output Polyphonic MIDI CV
		// All channels are written below, so avoid the SDK's redundant
		// higher-channel clearing pass (which also trips a GCC false positive).
		outputs[MIDI_PITCH_OUTPUT].channels = 16;
		outputs[MIDI_GATE_OUTPUT].channels = 16;
		for (int i = 0; i < 16; ++i) {
			outputs[MIDI_PITCH_OUTPUT].setVoltage(voices[i].pitch, i);
			outputs[MIDI_GATE_OUTPUT].setVoltage(voices[i].gate, i);
		}

		// Signal graphical frame updates
		if (doom_dirty_frame) {
			doom_dirty_frame = 0;
			dirtyFrame.store(true);
		}
	} else {
		// Output 0V when bypassed or uninitialized
		outputs[HEALTH_OUTPUT].setVoltage(0.f);
		outputs[FRAG_TRIG_OUTPUT].setVoltage(0.f);
		for (int i = 0; i < 16; ++i) {
			voices[i].gate = 0.f;
			voices[i].note = -1;
		}
		outputs[MIDI_PITCH_OUTPUT].channels = 0;
		outputs[MIDI_GATE_OUTPUT].channels = 0;
	}

	outputs[AUDIO_L_OUTPUT].setVoltage(outL * 5.f);
	outputs[AUDIO_R_OUTPUT].setVoltage(outR * 5.f);
}

bool ChronoDoomModule::loadWad(const std::string& path) {
	if (path.empty()) {
		return false;
	}

	if (gDoomModuleOwner != this) {
		return false;
	}

	// Phase 1 validation: file must exist and be readable
	std::ifstream file(path, std::ios::binary);
	if (!file.good()) {
		return false;
	}

	// Basic WAD header check: First 4 bytes must be "IWAD" or "PWAD".
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

	// Shut down existing thread if any
	doom_exit_requested = 1;
	if (gDoomThread.joinable()) {
		gDoomThread.join();
	}
	W_Shutdown();

	// Start a new thread
	doom_exit_requested = 0;
	doom_dirty_frame = 0;
	doom_engine_error[0] = '\0';
	doom_engine_status = 1;
	gDoomThread = std::thread([this]() {
		I_SetTargetRGBA(dummyFramebuffer);

		// Set up argc / argv
		int argc = 3;
		char** argv = (char**)malloc((argc + 1) * sizeof(char*));
		argv[0] = strdup("chronodoom");
		argv[1] = strdup("-iwad");
		argv[2] = strdup(wadPath.c_str());
		argv[3] = NULL;
		myargc = argc;
		myargv = argv;

		D_DoomMain();

		for (int i = 0; i < argc; ++i) {
			free(argv[i]);
		}
		free(argv);
	});

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
