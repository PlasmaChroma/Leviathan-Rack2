#pragma once

#include "BulkheadGeometry.hpp"
#include "plugin.hpp"
#include "SpscLatestSnapshot.hpp"
#include <array>
#include <vector>

struct Bulkhead : Module {
	ModuleTeardownTimer teardownTimer {"Bulkhead"};
	enum ParamId {
		DECAY_PARAM,
		DIFFUSE_PARAM,
		MIX_PARAM,
		ABSORB_PARAM,
		MOTION_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		LST_X_INPUT,
		LST_Y_INPUT,
		WALL_LEFT_INPUT,
		WALL_RIGHT_INPUT,
		WALL_FRONT_INPUT,
		WALL_BACK_INPUT,
		IN_L_INPUT,
		IN_R_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHTS_LEN
	};

	// Authored geometry is UI-owned. Audio derives CV-modulated geometry from
	// a coherent fixed-size publication and never writes these controls back.
	bulkhead::geometry::RoomBounds room;
	bulkhead::geometry::Vec2 listener;
	bulkhead::geometry::Vec2 speakerLeft;
	bulkhead::geometry::Vec2 speakerRight;
	float listenerYawRadians = 0.f;
	float speakerLeftYawRadians = 0.f;
	float speakerRightYawRadians = 0.f;
	bool directGeoDryEnabled = true;
	struct GeometryState {
	bulkhead::geometry::RoomBounds room;
	bulkhead::geometry::Vec2 listener;
	bulkhead::geometry::Vec2 speakerLeft;
	bulkhead::geometry::Vec2 speakerRight;
	float listenerYawRadians = 0.f;
	float speakerLeftYawRadians = 0.f;
	float speakerRightYawRadians = 0.f;
	bool directGeoDryEnabled = true;
		bool valid = false;
	};
	snapshot_transport::SpscLatestSnapshot<GeometryState> authoredGeometry;
	snapshot_transport::SpscLatestSnapshot<GeometryState> effectiveGeometry;
	GeometryState displayGeometry;
	float geometryDisplayElapsed = 0.f;
	void publishGeometry();
	void serviceGeometryUi();
	float sampleRate = 44100.f;

	struct DelayLine {
		std::vector<float> buffer;
		int writeIndex = 0;

		void resize(int size);
		float readDelay(int delaySamples) const;
		void writeSample(float v);
	};

	struct CombFilter {
		DelayLine line;
		float feedback = 0.75f;
		float damp = 0.25f;
		float filterStore = 0.f;

		float process(float in, int delaySamples);
	};

	struct AllpassFilter {
		DelayLine line;
		float feedback = 0.5f;

		float process(float in, int delaySamples);
	};

	static constexpr int COMB_COUNT = 4;
	std::array<CombFilter, COMB_COUNT> combL {};
	std::array<CombFilter, COMB_COUNT> combR {};
	std::array<AllpassFilter, 2> allpassL {};
	std::array<AllpassFilter, 2> allpassR {};
	float wetPostLpL = 0.f;
	float wetPostLpR = 0.f;

	void initDsp();
	void resetSceneDefaults();
	void onSampleRateChange() override;
	void onReset() override;

	Bulkhead();
	~Bulkhead() override;
	void process(const ProcessArgs& args) override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* rootJ) override;
};

struct BulkheadWidget : ModuleWidget {
	explicit BulkheadWidget(Bulkhead* module);
	void appendContextMenu(Menu* menu) override;
	void step() override;
};

extern Model* modelBulkhead;
