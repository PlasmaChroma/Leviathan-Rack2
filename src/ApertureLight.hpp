#pragma once

#include "plugin.hpp"

enum class ApertureLightSize {
	Tiny,
	Small,
	Medium,
	Large
};

struct LeviathanApertureLight : app::ModuleLightWidget {
	NVGcolor baseColor = nvgRGB(42, 246, 255);
	float socketRadius = 4.8f;
	float lensRadius = 3.3f;
	float coreRadius = 2.1f;
	float bloomRadius = 8.0f;
	float bloomAlpha = 0.22f;

	LeviathanApertureLight();
	void applySize(ApertureLightSize size);
	void drawBackground(const DrawArgs& args) override;
	void drawLight(const DrawArgs& args) override;
	void drawHalo(const DrawArgs& args) override;

private:
	void drawSocket(NVGcontext* vg, float cx, float cy);
	void drawUnlitLens(NVGcontext* vg, float cx, float cy);
	void drawBloom(NVGcontext* vg, float cx, float cy, float glow);
	void drawCore(NVGcontext* vg, float cx, float cy, float core, float hot);
	void drawSpecular(NVGcontext* vg, float cx, float cy, float hot);
	void drawCrescent(NVGcontext* vg, float cx, float cy, float amount);
};

struct TinyApertureLight : LeviathanApertureLight {
	TinyApertureLight();
};

struct SmallApertureLight : LeviathanApertureLight {
	SmallApertureLight();
};

struct MediumApertureLight : LeviathanApertureLight {
	MediumApertureLight();
};

struct LargeApertureLight : LeviathanApertureLight {
	LargeApertureLight();
};

template <typename TBase>
struct TinyAperture : TBase {
	TinyAperture() {
		this->applySize(ApertureLightSize::Tiny);
	}
};

template <typename TBase>
struct SmallAperture : TBase {
	SmallAperture() {
		this->applySize(ApertureLightSize::Small);
	}
};

template <typename TBase>
struct MediumAperture : TBase {
	MediumAperture() {
		this->applySize(ApertureLightSize::Medium);
	}
};

template <typename TBase>
struct LargeAperture : TBase {
	LargeAperture() {
		this->applySize(ApertureLightSize::Large);
	}
};

struct TealApertureLight : LeviathanApertureLight {
	TealApertureLight();
};

struct VioletApertureLight : LeviathanApertureLight {
	VioletApertureLight();
};

struct AmberApertureLight : LeviathanApertureLight {
	AmberApertureLight();
};

struct BlueApertureLight : LeviathanApertureLight {
	BlueApertureLight();
};

struct GreenApertureLight : LeviathanApertureLight {
	GreenApertureLight();
};

struct MagentaApertureLight : LeviathanApertureLight {
	MagentaApertureLight();
};

struct WhiteApertureLight : LeviathanApertureLight {
	WhiteApertureLight();
};
