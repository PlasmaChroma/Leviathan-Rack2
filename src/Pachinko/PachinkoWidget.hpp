#pragma once
#include "PachinkoTimingModule.hpp"
#include <rack.hpp>

struct PachinkoWidget : rack::app::ModuleWidget {
    struct PachinkoVisualWidget : rack::widget::Widget {
        PachinkoTimingModule* module = nullptr;
        
        void setModule(PachinkoTimingModule* mod) { module = mod; }
        
        void step() override;
        void draw(const DrawArgs& args) override;
    };
    
    PachinkoWidget(PachinkoTimingModule* module);
};
