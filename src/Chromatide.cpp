#include "Chromatide.hpp"
#include <algorithm>

Chromatide::Chromatide() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
    configParam(BRUSH_SIZE_PARAM, 1.0f, 128.0f, 24.0f, "Brush Size", "px");
    configParam(BRUSH_OPACITY_PARAM, 0.0f, 1.0f, 1.0f, "Brush Opacity", "%", 0.0f, 100.0f);
    configParam(TOOL_PARAM, 0.0f, 2.0f, 0.0f, "Tool");
    configParam(CLEAR_PARAM, 0.0f, 1.0f, 0.0f, "Clear");
    configParam(UNDO_PARAM, 0.0f, 1.0f, 0.0f, "Undo");
    configParam(REDO_PARAM, 0.0f, 1.0f, 0.0f, "Redo");

    brushState.foreground = palette[selectedPaletteIndex];
}

void Chromatide::process(const ProcessArgs& args) {
    (void)args;
    brushState.size = params[BRUSH_SIZE_PARAM].getValue();
    brushState.opacity = params[BRUSH_OPACITY_PARAM].getValue();
    int toolIdx = static_cast<int>(params[TOOL_PARAM].getValue());
    brushState.tool = static_cast<ChromatideTool>(clampVal(toolIdx, 0, 2));

    uint64_t currentRev = canvas.revision;
    uint64_t lastPub = lastPublishedCanvasRevision.load(std::memory_order_relaxed);
    if (currentRev != lastPub || forceIrisSync) {
        forceIrisSync = false;
        publishToIris();
    }
}

void Chromatide::onExpanderChange(const ExpanderChangeEvent& e) {
    Module::onExpanderChange(e);
    forceIrisSync = true;
}


void Chromatide::beginStroke(float u, float v) {
    if (brushState.tool == ChromatideTool::Eyedropper) {
        float rx = 0.0f, ry = 0.0f;
        canvas.normalizedToRaster(u, v, rx, ry);
        int x = clampVal(static_cast<int>(std::round(rx)), 0, ChromatideCanvas::WIDTH - 1);
        int y = clampVal(static_cast<int>(std::round(ry)), 0, ChromatideCanvas::HEIGHT - 1);

        canvas.sample(x, y, brushState.foreground.r, brushState.foreground.g, brushState.foreground.b);

        // Update selected palette index if matching
        for (size_t i = 0; i < palette.size(); ++i) {
            if (palette[i] == brushState.foreground) {
                selectedPaletteIndex = static_cast<int>(i);
                break;
            }
        }

        brushState.tool = ChromatideTool::Brush;
        params[TOOL_PARAM].setValue(0.0f);
        return;
    }

    strokeActive = true;
    prevStrokeU = u;
    prevStrokeV = v;
    activeStrokeDirty.reset();

    float rx = 0.0f, ry = 0.0f;
    canvas.normalizedToRaster(u, v, rx, ry);

    float size = clampVal(brushState.size, 1.0f, 128.0f);
    float Ry = size * 0.5f;
    float Rx = (size * 0.5f) * (4.0f / ChromatideCanvas::VIEWPORT_ASPECT_RATIO);

    RectI initialBounds;
    initialBounds.minX = clampVal(static_cast<int>(std::floor(rx - Rx - 2.0f)), 0, ChromatideCanvas::WIDTH - 1);
    initialBounds.maxX = clampVal(static_cast<int>(std::ceil(rx + Rx + 2.0f)), 0, ChromatideCanvas::WIDTH - 1);
    initialBounds.minY = clampVal(static_cast<int>(std::floor(ry - Ry - 2.0f)), 0, ChromatideCanvas::HEIGHT - 1);
    initialBounds.maxY = clampVal(static_cast<int>(std::ceil(ry + Ry + 2.0f)), 0, ChromatideCanvas::HEIGHT - 1);


    canvas.beginStrokeTransaction(initialBounds, activeStrokeUndoRecord);
    canvas.stampAtRaster(rx, ry, brushState, &activeStrokeDirty);
}

void Chromatide::updateStroke(float u, float v) {
    if (!strokeActive) return;
    canvas.strokeToNormalized(prevStrokeU, prevStrokeV, u, v, brushState, &activeStrokeDirty);
    prevStrokeU = u;
    prevStrokeV = v;
}

void Chromatide::endStroke() {
    if (!strokeActive) return;
    strokeActive = false;
    canvas.finalizeStrokeTransaction(activeStrokeDirty, activeStrokeUndoRecord);
    pushUndoRecord(activeStrokeUndoRecord);
    canvas.revision++;
    publishToIris();
}

void Chromatide::cancelStroke() {
    if (!strokeActive) return;
    strokeActive = false;
}

void Chromatide::clearCanvas() {
    ChromatideUndoRecord record;
    RectI fullCanvas(0, 0, ChromatideCanvas::WIDTH - 1, ChromatideCanvas::HEIGHT - 1);
    canvas.beginStrokeTransaction(fullCanvas, record);
    RectI dirty;
    dirty.reset();
    canvas.clear(brushState.background.r, brushState.background.g, brushState.background.b, &dirty);
    canvas.finalizeStrokeTransaction(dirty, record);
    pushUndoRecord(record);
    publishToIris();
}


bool Chromatide::undo() {
    if (undoStack.empty()) return false;
    ChromatideUndoRecord record = undoStack.back();
    undoStack.pop_back();
    canvas.applyUndoRecord(record, true);
    redoStack.push_back(record);
    publishToIris();
    return true;
}

bool Chromatide::redo() {
    if (redoStack.empty()) return false;
    ChromatideUndoRecord record = redoStack.back();
    redoStack.pop_back();
    canvas.applyUndoRecord(record, false);
    undoStack.push_back(record);
    publishToIris();
    return true;
}

void Chromatide::pushUndoRecord(ChromatideUndoRecord record) {
    if (!record.bounds.valid()) return;
    undoStack.push_back(std::move(record));
    redoStack.clear();

    size_t totalMem = 0;
    for (const auto& rec : undoStack) {
        totalMem += rec.memoryUsage();
    }

    while (totalMem > MAX_UNDO_MEMORY && !undoStack.empty()) {
        totalMem -= undoStack.front().memoryUsage();
        undoStack.erase(undoStack.begin());
    }
}

void Chromatide::selectPaletteColor(int index) {
    if (index < 0 || index >= static_cast<int>(palette.size())) return;
    selectedPaletteIndex = index;
    brushState.foreground = palette[index];
}

void Chromatide::publishToIris() {
    uint64_t nextGen = ++irisPreviewGeneration;

    int slotIdx = (irisExpanderWriteSlot.load(std::memory_order_relaxed) + 1) % nautiloid_iris_expander::kSourceSlotCount;
    nautiloid_iris_expander::SourceSlot* slot = &irisExpanderSlots[slotIdx];

    if (!nautiloid_iris_expander::claimSourceSlotForWrite(slot)) return;

    slot->source.width = iris::kCanonicalSourceWidth;
    slot->source.height = iris::kCanonicalSourceHeight;
    slot->source.channels = iris::kCanonicalSourceChannels;
    slot->source.bitDepth = iris::kCanonicalSourceBitDepth;
    slot->source.rgb8.assign(canvas.pixels.begin(), canvas.pixels.end());
    slot->source.sourceName = "Chromatide Canvas";
    slot->source.sourcePath = "";
    slot->source.originalWidth = iris::kCanonicalSourceWidth;
    slot->source.originalHeight = iris::kCanonicalSourceHeight;
    slot->source.originalChannels = iris::kCanonicalSourceChannels;
    slot->source.generatorKind = iris::SOURCE_GENERATOR_NONE;

    slot->generation.store(nextGen, std::memory_order_release);
    nautiloid_iris_expander::releaseSourceSlotWrite(slot);
    irisExpanderPublishedSlot.store(slotIdx, std::memory_order_release);
    irisExpanderWriteSlot.store(slotIdx, std::memory_order_release);

    Module* right = rightExpander.module;
    if (right && right->model && (right->model == modelIris || right->model->slug == "Iris")) {
        if (right->leftExpander.module == this) {
            Iris* irisModule = static_cast<Iris*>(right);
            irisModule->requestExpanderSource(slot, nextGen);
        }
    }
    lastPublishedCanvasRevision.store(canvas.revision, std::memory_order_release);
}


json_t* Chromatide::dataToJson() {
    json_t* root = json_object();
    json_object_set_new(root, "chromatideVersion", json_integer(1));

    std::string b64 = canvas.serializeQoiBase64();
    if (!b64.empty()) {
        json_t* canvasJ = json_object();
        json_object_set_new(canvasJ, "encoding", json_string("qoi-base64"));
        json_object_set_new(canvasJ, "width", json_integer(ChromatideCanvas::WIDTH));
        json_object_set_new(canvasJ, "height", json_integer(ChromatideCanvas::HEIGHT));
        json_object_set_new(canvasJ, "channels", json_integer(ChromatideCanvas::CHANNELS));
        json_object_set_new(canvasJ, "data", json_string(b64.c_str()));
        json_object_set_new(root, "canvas", canvasJ);
    }

    json_t* brushJ = json_object();
    json_object_set_new(brushJ, "size", json_real(brushState.size));
    json_object_set_new(brushJ, "opacity", json_real(brushState.opacity));
    json_object_set_new(brushJ, "tool", json_integer(static_cast<int>(brushState.tool)));

    json_t* fgJ = json_array();
    json_array_append_new(fgJ, json_integer(brushState.foreground.r));
    json_array_append_new(fgJ, json_integer(brushState.foreground.g));
    json_array_append_new(fgJ, json_integer(brushState.foreground.b));
    json_object_set_new(brushJ, "foreground", fgJ);

    json_t* bgJ = json_array();
    json_array_append_new(bgJ, json_integer(brushState.background.r));
    json_array_append_new(bgJ, json_integer(brushState.background.g));
    json_array_append_new(bgJ, json_integer(brushState.background.b));
    json_object_set_new(brushJ, "background", bgJ);

    json_object_set_new(root, "brush", brushJ);

    json_t* paletteJ = json_array();
    for (const auto& color : palette) {
        json_t* cJ = json_array();
        json_array_append_new(cJ, json_integer(color.r));
        json_array_append_new(cJ, json_integer(color.g));
        json_array_append_new(cJ, json_integer(color.b));
        json_array_append_new(paletteJ, cJ);
    }
    json_object_set_new(root, "palette", paletteJ);
    json_object_set_new(root, "selectedPaletteIndex", json_integer(selectedPaletteIndex));

    return root;
}

void Chromatide::dataFromJson(json_t* root) {
    if (!root) return;

    json_t* canvasJ = json_object_get(root, "canvas");
    if (canvasJ) {
        json_t* dataJ = json_object_get(canvasJ, "data");
        if (dataJ && json_is_string(dataJ)) {
            std::string b64 = json_string_value(dataJ);
            if (!canvas.deserializeQoiBase64(b64)) {
                WARN("Chromatide: patch canvas decode failed");
            }
        }
    }

    json_t* brushJ = json_object_get(root, "brush");
    if (brushJ) {
        json_t* sizeJ = json_object_get(brushJ, "size");
        if (sizeJ && json_is_number(sizeJ)) brushState.size = static_cast<float>(json_number_value(sizeJ));

        json_t* opacityJ = json_object_get(brushJ, "opacity");
        if (opacityJ && json_is_number(opacityJ)) brushState.opacity = static_cast<float>(json_number_value(opacityJ));

        json_t* toolJ = json_object_get(brushJ, "tool");
        if (toolJ && json_is_integer(toolJ)) {
            int t = static_cast<int>(json_integer_value(toolJ));
            brushState.tool = static_cast<ChromatideTool>(clampVal(t, 0, 2));
            params[TOOL_PARAM].setValue(static_cast<float>(t));
        }

        json_t* fgJ = json_object_get(brushJ, "foreground");
        if (fgJ && json_is_array(fgJ) && json_array_size(fgJ) >= 3) {
            brushState.foreground.r = static_cast<uint8_t>(json_integer_value(json_array_get(fgJ, 0)));
            brushState.foreground.g = static_cast<uint8_t>(json_integer_value(json_array_get(fgJ, 1)));
            brushState.foreground.b = static_cast<uint8_t>(json_integer_value(json_array_get(fgJ, 2)));
        }

        json_t* bgJ = json_object_get(brushJ, "background");
        if (bgJ && json_is_array(bgJ) && json_array_size(bgJ) >= 3) {
            brushState.background.r = static_cast<uint8_t>(json_integer_value(json_array_get(bgJ, 0)));
            brushState.background.g = static_cast<uint8_t>(json_integer_value(json_array_get(bgJ, 1)));
            brushState.background.b = static_cast<uint8_t>(json_integer_value(json_array_get(bgJ, 2)));
        }
    }

    json_t* paletteJ = json_object_get(root, "palette");
    if (paletteJ && json_is_array(paletteJ)) {
        size_t count = std::min(json_array_size(paletteJ), palette.size());
        for (size_t i = 0; i < count; ++i) {
            json_t* cJ = json_array_get(paletteJ, i);
            if (cJ && json_is_array(cJ) && json_array_size(cJ) >= 3) {
                palette[i].r = static_cast<uint8_t>(json_integer_value(json_array_get(cJ, 0)));
                palette[i].g = static_cast<uint8_t>(json_integer_value(json_array_get(cJ, 1)));
                palette[i].b = static_cast<uint8_t>(json_integer_value(json_array_get(cJ, 2)));
            }
        }
    }

    json_t* selPalJ = json_object_get(root, "selectedPaletteIndex");
    if (selPalJ && json_is_integer(selPalJ)) {
        selectedPaletteIndex = clampVal(static_cast<int>(json_integer_value(selPalJ)), 0, static_cast<int>(palette.size() - 1));
        brushState.foreground = palette[selectedPaletteIndex];
    }
}

