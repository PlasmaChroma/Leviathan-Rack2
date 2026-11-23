#pragma once

#include "../plugin.hpp"
#include "ChromatideCanvas.hpp"
#include "../iris/Iris.hpp"

#include <vector>
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>

// Knob-slot quantity: its name and display units follow the active tool profile.
struct ChromatideSlotQuantity : ParamQuantity {
    int slot = 0;
    std::string getLabel() override;
    std::string getDisplayValueString() override;
    std::string getUnit() override;
};

struct Chromatide : Module {
    enum ParamId {
        BRUSH_SIZE_PARAM,
        BRUSH_OPACITY_PARAM,
        TOOL_PARAM,
        CLEAR_PARAM,
        UNDO_PARAM,
        REDO_PARAM,
        BRUSH_FLOW_PARAM,
        BRUSH_HARDNESS_PARAM,
        BRUSH_JITTER_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        INPUTS_LEN
    };
    enum OutputId {
        OUTPUTS_LEN
    };
    enum LightId {
        // Consecutive amber/green/violet channels for the shared aperture light.
        IRIS_LINK_LIGHT,
        IRIS_READY_LIGHT,
        IRIS_LINK_SPARE_LIGHT,
        LIGHTS_LEN
    };

    ChromatideCanvas canvas;
    ChromatideBrushState brushState;

    static constexpr size_t PALETTE_SIZE = 9;
    std::array<ChromatideColor, PALETTE_SIZE> palette {
        ChromatideColor{0, 0, 0},       // 0: Black
        ChromatideColor{255, 255, 255}, // 1: White
        ChromatideColor{255, 59, 48},   // 2: Red
        ChromatideColor{255, 149, 0},   // 3: Orange
        ChromatideColor{255, 204, 0},   // 4: Yellow
        ChromatideColor{52, 199, 89},   // 5: Green
        ChromatideColor{0, 122, 255},   // 6: Blue
        ChromatideColor{88, 86, 214},   // 7: Indigo
        ChromatideColor{175, 82, 222}   // 8: Violet
    };
    int selectedPaletteIndex = 1; // -1: brush color came from the eyedropper, no swatch matches

    // Per-tool knob memory (UI thread). The params always hold the active
    // profile's values; switching tool or head stores them and loads the next.
    static int slotParam(int slot) {
        static const int ids[kChromatideKnobSlots] = {
            BRUSH_FLOW_PARAM, BRUSH_OPACITY_PARAM, BRUSH_HARDNESS_PARAM, BRUSH_JITTER_PARAM};
        return ids[slot];
    }
    std::array<std::array<float, kChromatideKnobSlots>, size_t(ChromatideKnobProfile::Count)> knobProfiles {};
    ChromatideKnobProfile activeKnobProfile = ChromatideKnobProfile::Round;
    void updateKnobProfile();

    std::vector<ChromatideUndoRecord> undoStack;
    std::vector<ChromatideUndoRecord> redoStack;
    static constexpr size_t MAX_UNDO_MEMORY = 32 * 1024 * 1024; // 32 MiB

    bool strokeActive = false;
    RectI activeStrokeDirty;
    ChromatideUndoRecord activeStrokeUndoRecord;
    ChromatideStrokeBuffer activeStrokeBuffer;
    float prevStrokeU = 0.0f;
    float prevStrokeV = 0.0f;

    // Best-effort mid-stroke Iris streaming, as Nautiloid does: hand off the
    // newest canvas at most once per UI frame; Iris's worker is latest-wins so
    // superseded snapshots are simply dropped. endStroke always publishes.
    uint64_t lastLiveIrisRevision = 0;
    bool publishLiveStrokeIfDirty(); // UI thread only

    // Published canvas snapshots are immutable and shared with Iris's worker.
    // Use the C++17 atomic shared_ptr free functions to exchange this between
    // the state loader and UI delivery service; audio never acquires ownership.
    std::shared_ptr<const fractal::SourceField> irisPublishedSource;
    std::atomic<uint64_t> irisPreviewGeneration {0u};
    std::atomic<bool> forceIrisSync {true};
    uint64_t lastExpanderGenerationSent = 0u; // UI thread
    std::atomic<bool> irisDelivered {false};    // UI -> audio: Iris holds the newest canvas

    Chromatide();

    void process(const ProcessArgs& args) override;
    void serviceIrisSource(); // UI thread only
    // Re-send the current canvas to a right-hand Iris on the next UI service.
    void requestIrisSourceSync() { forceIrisSync.store(true, std::memory_order_release); }
    void onExpanderChange(const ExpanderChangeEvent& e) override;


    void beginStroke(float u, float v);
    void updateStroke(float u, float v);
    // Airbrush-style build-up while the cursor rests: one gentle dab per UI
    // frame, its flow scaled to elapsed time. UI thread.
    void dwellStroke(double now);
    double lastStrokeMotionTime = 0.0;
    double lastDwellTime = 0.0;
    void endStroke();
    void cancelStroke();

    void clearCanvas();
    bool undo();
    bool redo();
    void pushUndoRecord(ChromatideUndoRecord record);

    void selectPaletteColor(int index);
    void setPaletteColor(int index, ChromatideColor color);

    void syncBrushFromParams();
    // Writes the canvas at native resolution (1024x256 RGB). UI thread.
    bool savePng(const std::string& path) const;
    // Replaces the canvas with an image file (PNG/JPEG/BMP/TGA...), stretched
    // to 1024x256 with bilinear filtering. Undoable. UI thread.
    bool loadImage(const std::string& path);
    void publishToIris();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
};
