// Rack-linked CPU test of recipe ownership/traversal. No graphics context needed.
#include "render/LegacyCurvePreview.hpp"
#include <cstdio>
#include <cstdlib>

namespace rack {
// As in gl_surface_lifecycle_spec: this fixture owns stack services and does
// not initialize Rack's private host teardown/logging machinery.
Context::~Context() {}
}

static void require(bool ok, const char* message) {
	if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

struct Counts { int steps = 0, creates = 0, destroys = 0, draws = 0, deleted = 0; };
struct Probe : rack::widget::Widget {
	Counts* counts = nullptr;
	~Probe() override { if (counts) ++counts->deleted; }
	void step() override { ++counts->steps; Widget::step(); }
	void draw(const DrawArgs&) override { ++counts->draws; }
	void onContextCreate(const ContextCreateEvent& e) override {
		++counts->creates; Widget::onContextCreate(e);
	}
	void onContextDestroy(const ContextDestroyEvent& e) override {
		++counts->destroys; Widget::onContextDestroy(e);
	}
};
struct ContourProbe : Probe {
	visual_assets::ContourSettlement settlement;
	int updates = 0;
	void drawContour(const DrawArgs&, const std::array<float, 14>& key, double now) {
		updates += settlement.observe(key, now);
	}
};
struct HistoryProbe : Probe {
	const WavePreviewTracer<8, 2>* observed = nullptr;
	double observedTime = 0;
	void drawHistory(const DrawArgs&, const WavePreviewTracer<8, 2>& tracer, double now,
	                 const WavePreviewTracerStyle&, WavePreviewTracerDrawStats* stats) {
		observed = &tracer; observedTime = now;
		if (stats) ++stats->trails;
	}
};

int main() {
	rack::Context context;
	rack::widget::EventState events;
	context.event = &events;
	rack::contextSet(&context);
	Counts contourCounts, historyCounts, contentCounts;
	{
		auto* contour = new ContourProbe;
		contour->counts = &contourCounts;
		auto* content = new Probe;
		content->counts = &contentCounts;
		leviathan::render::LegacyCurvePreview<8, 2, ContourProbe, HistoryProbe> recipe(contour, content);
		recipe.history->counts = &historyCounts;
		recipe.setExtent(rack::math::Vec(106, 48));
		require(contour->parent == &recipe && content->parent == contour
			&& recipe.history->parent == &recipe, "single Rack ownership tree");
		for (auto* widget : {static_cast<rack::widget::Widget*>(contour),
		                    static_cast<rack::widget::Widget*>(content),
		                    static_cast<rack::widget::Widget*>(recipe.history)})
			require(widget->box.size == recipe.box.size, "layout propagated to all layers");
		recipe.step();
		rack::widget::Widget::ContextCreateEvent create{};
		rack::widget::Widget::ContextDestroyEvent destroy{};
		recipe.onContextCreate(create);
		recipe.onContextDestroy(destroy);
		for (const Counts* counts : {&contourCounts, &historyCounts, &contentCounts})
			require(counts->steps == 1 && counts->creates == 1 && counts->destroys == 1,
				"step and lifecycle events reach each layer exactly once");
		rack::widget::Widget::DrawArgs args{};
		recipe.draw(args);
		require(contourCounts.draws == 0 && historyCounts.draws == 0 && contentCounts.draws == 0,
			"automatic traversal does not duplicate explicit composition");
		std::array<float, 14> key{};
		recipe.drawContour(args, key, 1.0);
		WavePreviewTracer<8, 2> tracer;
		WavePreviewTracerDrawStats stats;
		recipe.drawHistory(args, tracer, 1.05, WavePreviewTracerStyle{}, &stats);
		require(recipe.history->observed == &tracer && recipe.history->observedTime == 1.05
			&& stats.trails == 1, "history data/time/statistics forwarded without copying or retiming");
		recipe.drawContour(args, key, 1.2);
		require(contour->updates == 1 && contour->settlement.settled(1.2),
			"history presentation does not invalidate or delay settled contour");
		key[0] = .25f;
		recipe.drawContour(args, key, 1.3);
		require(contour->updates == 2 && !contour->settlement.settled(1.3),
			"changed contour immediately returns to live policy");
	}
	for (const Counts* counts : {&contourCounts, &historyCounts, &contentCounts})
		require(counts->deleted == 1, "subtree destruction owns each layer exactly once");
	context.event = nullptr; // Stack-owned event fixture, not Context-owned storage.
	rack::contextSet(nullptr);
	std::puts("PASS: legacy curve recipe ownership, traversal, independent presentation, and teardown");
}
