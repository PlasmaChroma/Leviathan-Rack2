#pragma once

#include "plugin.hpp"
#include "ChromatideCanvas.hpp"
#include "Iris.hpp"

#include <vector>
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>

struct Chromatide : Module {
    enum ParamId {
        BRUSH_SIZE_PARAM,
        BRUSH_OPACITY_PARAM,
        TOOL_PARAM,
        CLEAR_PARAM,
        UNDO_PARAM,
        REDO_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        INPUTS_LEN
    };
    enum OutputId {
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    ChromatideCanvas canvas;
    ChromatideBrushState brushState;

    std::array<ChromatideColor, 8> palette {
        ChromatideColor{0, 0, 0},       // 0: Black
        ChromatideColor{255, 255, 255}, // 1: White
        ChromatideColor{255, 59, 48},   // 2: Red
        ChromatideColor{255, 149, 0},   // 3: Orange
        ChromatideColor{255, 204, 0},   // 4: Yellow
        ChromatideColor{52, 199, 89},   // 5: Green
        ChromatideColor{90, 200, 250},  // 6: Cyan
        ChromatideColor{175, 82, 222}   // 7: Purple
    };
    int selectedPaletteIndex = 1;

    std::vector<ChromatideUndoRecord> undoStack;
    std::vector<ChromatideUndoRecord> redoStack;
    static constexpr size_t MAX_UNDO_MEMORY = 32 * 1024 * 1024; // 32 MiB

    bool strokeActive = false;
    RectI activeStrokeDirty;
    ChromatideUndoRecord activeStrokeUndoRecord;
    float prevStrokeU = 0.0f;
    float prevStrokeV = 0.0f;

    // Published canvas snapshots are immutable and shared with Iris's worker.
    // Use the C++17 atomic shared_ptr free functions to exchange this between
    // the UI/state thread and Rack's engine thread.
    std::shared_ptr<const iris::SourceField> irisPublishedSource;
    std::atomic<uint64_t> irisPreviewGeneration {0u};
    std::atomic<bool> forceIrisSync {true};
    uint64_t lastExpanderGenerationSent = 0u;

    Chromatide();

    void process(const ProcessArgs& args) override;
    void onExpanderChange(const ExpanderChangeEvent& e) override;


    void beginStroke(float u, float v);
    void updateStroke(float u, float v);
    void endStroke();
    void cancelStroke();

    void clearCanvas();
    bool undo();
    bool redo();
    void pushUndoRecord(ChromatideUndoRecord record);

    void selectPaletteColor(int index);

    void syncBrushFromParams();
    void publishToIris();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
};
