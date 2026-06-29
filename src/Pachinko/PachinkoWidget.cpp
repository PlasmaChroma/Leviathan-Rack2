#include "PachinkoWidget.hpp"
#include <cmath>
#include <sstream>

PachinkoWidget::PachinkoWidget(PachinkoTimingModule* module) : rack::app::ModuleWidget() {
    setModule(module);
    
    // Set panel size
    box.size = math::Vec(350, 500);
    
    // Add visual widget for rendering
    PachinkoVisualWidget* visualWidget = new PachinkoVisualWidget();
    visualWidget->box.size = box.size;
    visualWidget->setModule(module);
    addChild(visualWidget);
    
    // Add panel SVG
    std::shared_ptr<rack::window::Svg> panel = rack::window::Svg::load(rack::asset::plugin(pluginInstance, "res/Pachinko.svg"));
    if (panel) {
        setPanel(panel);
    }
    
    // Add params
    rack::app::SvgKnob* ballRateKnob = new rack::app::SvgKnob();
    ballRateKnob->box.pos = math::Vec(20, 40);
    ballRateKnob->box.size = math::Vec(30, 30);
    ballRateKnob->module = module;
    ballRateKnob->paramId = PachinkoTimingModule::BALL_RATE_PARAM;
    addParam(ballRateKnob);
    
    rack::app::SvgKnob* dampingKnob = new rack::app::SvgKnob();
    dampingKnob->box.pos = math::Vec(20, 100);
    dampingKnob->box.size = math::Vec(30, 30);
    dampingKnob->module = module;
    dampingKnob->paramId = PachinkoTimingModule::DAMPING_PARAM;
    addParam(dampingKnob);
    
    // Add inputs
    rack::app::SvgPort* clockPort = new rack::app::SvgPort();
    clockPort->box.pos = math::Vec(20, 160);
    clockPort->box.size = math::Vec(16, 16);
    clockPort->module = module;
    clockPort->type = rack::engine::Port::INPUT;
    clockPort->portId = PachinkoTimingModule::CLOCK_INPUT;
    addInput(clockPort);
    
    rack::app::SvgPort* resetPort = new rack::app::SvgPort();
    resetPort->box.pos = math::Vec(20, 200);
    resetPort->box.size = math::Vec(16, 16);
    resetPort->module = module;
    resetPort->type = rack::engine::Port::INPUT;
    resetPort->portId = PachinkoTimingModule::RESET_INPUT;
    addInput(resetPort);
    
    // Add outputs (8 gates in a column)
    for (int i = 0; i < 8; i++) {
        rack::app::SvgPort* port = new rack::app::SvgPort();
        port->box.pos = math::Vec(280, 40 + i * 40);
        port->box.size = math::Vec(16, 16);
        port->module = module;
        port->type = rack::engine::Port::OUTPUT;
        port->portId = PachinkoTimingModule::GATE_1_OUTPUT + i;
        addOutput(port);
    }
    
    // Add lights (using ModuleLightWidget)
    for (int i = 0; i < 8; i++) {
        rack::app::ModuleLightWidget* light = new rack::app::ModuleLightWidget();
        light->box.pos = math::Vec(305, 52 + i * 40);
        light->box.size = math::Vec(12, 12);
        light->module = module;
        light->firstLightId = PachinkoTimingModule::GATE_1_LIGHT + i;
        addChild(light);
    }
}

void PachinkoWidget::PachinkoVisualWidget::step() {
    // This is called on the UI thread - update any UI state here
}

void PachinkoWidget::PachinkoVisualWidget::draw(const DrawArgs& args) {
    if (!module) return;
    
    // Get NanoVG context
    NVGcontext* vg = args.vg;
    if (!vg) return;
    
    // Clear with dark background (nvgBeginFrame already clears)
    nvgBeginFrame(vg, box.size.x, box.size.y, 1.0f);
    
    // Draw pegs
    if (module->pegs.size() > 0) {
        nvgBeginPath(vg);
        for (const auto& peg : module->pegs) {
            nvgCircle(vg, peg.position.x + 175.0f, peg.position.y + 250.0f, peg.radius);
        }
        nvgFillColor(vg, nvgRGB(200, 200, 200));
        nvgFill(vg);
    }
    
    // Draw buckets at bottom
    float bucketWidth = 180.0f / module->numBuckets;
    for (int i = 0; i < module->numBuckets; i++) {
        float x = -90.0f + i * bucketWidth;
        nvgBeginPath(vg);
        nvgRect(vg, x + 175.0f, 240.0f, bucketWidth - 2, 20);
        nvgFillColor(vg, nvgRGB(100, 100, 100));
        nvgFill(vg);
        
        // Highlight active bucket
        if (module->outputs[PachinkoTimingModule::GATE_1_OUTPUT + i].value > 1.0f) {
            nvgBeginPath(vg);
            nvgRect(vg, x + 175.0f, 240.0f, bucketWidth - 2, 20);
            nvgFillColor(vg, nvgRGB(255, 100, 0));
            nvgFill(vg);
        }
    }
    
    // Draw balls
    nvgBeginPath(vg);
    for (const auto& ball : module->balls) {
        if (ball.active) {
            nvgCircle(vg, ball.position.x + 175.0f, ball.position.y + 250.0f, 4.0f);
        }
    }
    nvgFillColor(vg, nvgRGB(255, 200, 50));
    nvgFill(vg);
    
    nvgEndFrame(vg);
}
