#include "Chromatide.hpp"
#include "../common/UiExpanderUtils.hpp"
#include <algorithm>
#include <cctype>
#include <stb_image_write.h>
#include <stb_image.h>

Chromatide::Chromatide() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
    configParam(BRUSH_SIZE_PARAM, 1.0f, 128.0f, 24.0f, "Brush Size", "px");
    configParam<ChromatideSlotQuantity>(BRUSH_OPACITY_PARAM, 0.0f, 1.0f, 1.0f, "Brush Opacity", "%", 0.0f, 100.0f)->slot = 1;
    configParam(TOOL_PARAM, 0.0f, 3.0f, 0.0f, "Tool");
    configParam(CLEAR_PARAM, 0.0f, 1.0f, 0.0f, "Clear");
    configParam(UNDO_PARAM, 0.0f, 1.0f, 0.0f, "Undo");
    configParam(REDO_PARAM, 0.0f, 1.0f, 0.0f, "Redo");
    configParam<ChromatideSlotQuantity>(BRUSH_FLOW_PARAM, 0.01f, 1.0f, 0.35f, "Brush Flow", "%", 0.0f, 100.0f)->slot = 0;
    configParam<ChromatideSlotQuantity>(BRUSH_HARDNESS_PARAM, 0.0f, 1.0f, 0.6f, "Brush Hardness", "%", 0.0f, 100.0f)->slot = 2;
    configParam<ChromatideSlotQuantity>(BRUSH_JITTER_PARAM, 0.0f, 1.0f, 0.0f, "Brush Jitter", "%", 0.0f, 100.0f)->slot = 3;
    for (int p = 0; p < int(ChromatideKnobProfile::Count); ++p) {
        chromatideKnobProfileDefaults(ChromatideKnobProfile(p), knobProfiles[size_t(p)].data());
    }

    brushState.foreground = palette[selectedPaletteIndex];
    publishToIris();
}

void Chromatide::process(const ProcessArgs& args) {
    (void)args;
    // Same semantics as Nautiloid: amber while linked but a canvas hand-off is
    // outstanding, green once Iris holds the newest canvas.
    Module* right = rightExpander.module;
    const bool irisConnected = right && right->model == modelIris && right->leftExpander.module == this;
    const bool ready = irisConnected && irisDelivered.load(std::memory_order_acquire);
    lights[IRIS_LINK_LIGHT].setBrightness(irisConnected && !ready ? 1.f : 0.f);
    lights[IRIS_READY_LIGHT].setBrightness(ready ? 1.f : 0.f);
    lights[IRIS_LINK_SPARE_LIGHT].setBrightness(0.f);
}

namespace {
ChromatideKnobProfile slotProfile(const ParamQuantity* q) {
    const auto* m = dynamic_cast<const Chromatide*>(q->module);
    return m ? m->activeKnobProfile : ChromatideKnobProfile::Round;
}
}

std::string ChromatideSlotQuantity::getLabel() {
    const char* label = chromatideKnobSlotLabel(slotProfile(this), slot);
    if (!label) return "Unused";
    std::string text(label);
    for (size_t i = 1; i < text.size(); ++i) text[i] = char(std::tolower(static_cast<unsigned char>(text[i])));
    return text;
}

std::string ChromatideSlotQuantity::getDisplayValueString() {
    const ChromatideKnobProfile profile = slotProfile(this);
    if (!chromatideKnobSlotLabel(profile, slot)) return "-";
    const float v = getValue();
    if (profile == ChromatideKnobProfile::Chisel && slot == 2) return string::f("%.0f", v * 180.0f);
    if (profile == ChromatideKnobProfile::Chisel && slot == 3) return string::f("%.0f", (0.08f + 0.92f * v) * 100.0f);
    if (profile == ChromatideKnobProfile::Pick && slot == 0) {
        const int r = static_cast<int>(std::lround((v - 0.01f) / 0.99f * 8.0f));
        return string::f("%d", 2 * r + 1);
    }
    return ParamQuantity::getDisplayValueString();
}

std::string ChromatideSlotQuantity::getUnit() {
    const ChromatideKnobProfile profile = slotProfile(this);
    if (!chromatideKnobSlotLabel(profile, slot)) return "";
    if (profile == ChromatideKnobProfile::Chisel && slot == 2) return "\u00b0";
    if (profile == ChromatideKnobProfile::Pick && slot == 0) return " px square";
    return ParamQuantity::getUnit();
}

bool Chromatide::savePng(const std::string& path) const {
    return stbi_write_png(path.c_str(), ChromatideCanvas::WIDTH, ChromatideCanvas::HEIGHT,
        ChromatideCanvas::CHANNELS, canvas.pixels.data(),
        ChromatideCanvas::WIDTH * ChromatideCanvas::CHANNELS) != 0;
}

bool Chromatide::loadImage(const std::string& path) {
    int w = 0, h = 0, n = 0;
    uint8_t* src = stbi_load(path.c_str(), &w, &h, &n, 3);
    if (!src || w <= 0 || h <= 0) {
        if (src) stbi_image_free(src);
        return false;
    }
    cancelStroke();
    ChromatideUndoRecord record;
    canvas.beginStrokeTransaction(record);
    // One-time path: plain bilinear stretch with pixel-center alignment.
    const float sx = float(w) / float(ChromatideCanvas::WIDTH);
    const float sy = float(h) / float(ChromatideCanvas::HEIGHT);
    for (int y = 0; y < ChromatideCanvas::HEIGHT; ++y) {
        const float fy = clampVal((float(y) + 0.5f) * sy - 0.5f, 0.0f, float(h - 1));
        const int y0 = int(fy), y1 = std::min(y0 + 1, h - 1);
        const float ty = fy - float(y0);
        for (int x = 0; x < ChromatideCanvas::WIDTH; ++x) {
            const float fx = clampVal((float(x) + 0.5f) * sx - 0.5f, 0.0f, float(w - 1));
            const int x0 = int(fx), x1 = std::min(x0 + 1, w - 1);
            const float tx = fx - float(x0);
            uint8_t rgb[3];
            for (int c = 0; c < 3; ++c) {
                const float a = src[(size_t(y0) * w + x0) * 3 + c], b = src[(size_t(y0) * w + x1) * 3 + c];
                const float d = src[(size_t(y1) * w + x0) * 3 + c], e = src[(size_t(y1) * w + x1) * 3 + c];
                const float top = a + tx * (b - a), bottom = d + tx * (e - d);
                rgb[c] = uint8_t(clampVal(top + ty * (bottom - top) + 0.5f, 0.0f, 255.0f));
            }
            canvas.setPixel(x, y, rgb[0], rgb[1], rgb[2]);
        }
    }
    stbi_image_free(src);
    canvas.finalizeStrokeTransaction(
        RectI(0, 0, ChromatideCanvas::WIDTH - 1, ChromatideCanvas::HEIGHT - 1), record);
    pushUndoRecord(std::move(record));
    canvas.revision++;
    publishToIris();
    return true;
}

void Chromatide::serviceIrisSource() {
    if (strokeActive) {
        Module* right = ui_expander::neighbor(this, true);
        if (right && right->model == modelIris) publishLiveStrokeIfDirty();
    }
    const uint64_t generation = irisPreviewGeneration.load(std::memory_order_acquire);
    const bool force = forceIrisSync.exchange(false, std::memory_order_acq_rel);
    if (generation == 0u || (!force && generation == lastExpanderGenerationSent)) {
        irisDelivered.store(generation != 0u && generation == lastExpanderGenerationSent, std::memory_order_release);
        return;
    }

    Module* right = ui_expander::neighbor(this, true);
    if (right && right->model == modelIris) {
        // Iris drops sources until its left expander points back at us, so a
        // hand-off before the link settles would be silently lost.
        if (right->leftExpander.module != this) {
            irisDelivered.store(false, std::memory_order_release);
            forceIrisSync.store(true, std::memory_order_release);
            return;
        }
        std::shared_ptr<const fractal::SourceField> source =
            std::atomic_load_explicit(&irisPublishedSource, std::memory_order_acquire);
        if (!source) return;
        Iris* irisModule = static_cast<Iris*>(right);
        irisModule->requestOwnedExpanderSource(std::move(source), generation);
        lastExpanderGenerationSent = generation;
        irisDelivered.store(true, std::memory_order_release);
    }
}

void Chromatide::onExpanderChange(const ExpanderChangeEvent& e) {
    Module::onExpanderChange(e);
    forceIrisSync.store(true, std::memory_order_release);
}


void Chromatide::beginStroke(float u, float v) {
    syncBrushFromParams();
    if (brushState.tool == ChromatideTool::Eyedropper) {
        float rx = 0.0f, ry = 0.0f;
        canvas.normalizedToRaster(u, v, rx, ry);
        int x = clampVal(static_cast<int>(std::round(rx)), 0, ChromatideCanvas::WIDTH - 1);
        int y = clampVal(static_cast<int>(std::round(ry)), 0, ChromatideCanvas::HEIGHT - 1);

        canvas.sampleAverage(x, y, brushState.sampleRadius,
            brushState.foreground.r, brushState.foreground.g, brushState.foreground.b);

        // Highlight the matching swatch, or none if the color isn't in the palette.
        selectedPaletteIndex = -1;
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

    canvas.beginStrokeTransaction(activeStrokeUndoRecord);
    canvas.beginStrokeBuffer(activeStrokeBuffer);
    canvas.stampAtRaster(rx, ry, brushState, &activeStrokeDirty, &activeStrokeBuffer);
    lastStrokeMotionTime = lastDwellTime = system::getTime();
}

void Chromatide::dwellStroke(double now) {
    if (!strokeActive) return;
    // Smudge only moves colour that is already there; resting would just
    // blur its own dab, so it builds nothing.
    if (brushState.tool != ChromatideTool::Brush && brushState.tool != ChromatideTool::Eraser) return;
    const double dt = std::min(now - lastDwellTime, 0.1);
    lastDwellTime = now;
    // Short grace period so a pause mid-stroke does not blob the dab.
    if (dt <= 0.0 || now - lastStrokeMotionTime < 0.25) return;
    // Resting builds at kDwellRate equivalent dabs per second: at 35% flow
    // about half coverage after 1 s, ~85% after 3 s.
    constexpr float kDwellRate = 1.6f;
    ChromatideBrushState dwell = brushState;
    dwell.jitter = 0.0f;
    const float keep = std::pow(1.0f - clampVal(brushState.flow, 0.01f, 1.0f), kDwellRate * static_cast<float>(dt));
    dwell.flow = 1.0f - keep;
    if (dwell.flow < 0.0006f) return;
    float rx = 0.0f, ry = 0.0f;
    canvas.normalizedToRaster(prevStrokeU, prevStrokeV, rx, ry);
    canvas.stampAtRaster(rx, ry, dwell, &activeStrokeDirty, &activeStrokeBuffer);
}

void Chromatide::updateStroke(float u, float v) {
    if (!strokeActive) return;
    if (u != prevStrokeU || v != prevStrokeV) lastStrokeMotionTime = system::getTime();
    canvas.strokeToNormalized(prevStrokeU, prevStrokeV, u, v, brushState, &activeStrokeDirty, &activeStrokeBuffer);
    prevStrokeU = u;
    prevStrokeV = v;
}

bool Chromatide::publishLiveStrokeIfDirty() {
    if (!strokeActive || canvas.revision == lastLiveIrisRevision) return false;
    lastLiveIrisRevision = canvas.revision;
    publishToIris();
    return true;
}

void Chromatide::endStroke() {
    if (!strokeActive) return;
    strokeActive = false;
    activeStrokeBuffer.active = false;
    canvas.finalizeStrokeTransaction(activeStrokeDirty, activeStrokeUndoRecord);
    pushUndoRecord(std::move(activeStrokeUndoRecord));
    canvas.revision++;
    publishToIris();
}

void Chromatide::cancelStroke() {
    if (!strokeActive) return;
    strokeActive = false;
    activeStrokeBuffer.active = false;
}

void Chromatide::clearCanvas() {
    ChromatideUndoRecord record;
    canvas.beginStrokeTransaction(record);
    RectI dirty;
    dirty.reset();
    canvas.clear(brushState.background.r, brushState.background.g, brushState.background.b, &dirty);
    canvas.finalizeStrokeTransaction(dirty, record);
    pushUndoRecord(std::move(record));
    publishToIris();
}


bool Chromatide::undo() {
    if (undoStack.empty()) return false;
    ChromatideUndoRecord record = std::move(undoStack.back());
    undoStack.pop_back();
    canvas.applyUndoRecord(record, true);
    redoStack.push_back(std::move(record));
    publishToIris();
    return true;
}

bool Chromatide::redo() {
    if (redoStack.empty()) return false;
    ChromatideUndoRecord record = std::move(redoStack.back());
    redoStack.pop_back();
    canvas.applyUndoRecord(record, false);
    undoStack.push_back(std::move(record));
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

void Chromatide::setPaletteColor(int index, ChromatideColor color) {
    if (index < 0 || index >= static_cast<int>(palette.size())) return;
    palette[static_cast<size_t>(index)] = color;
    if (index == selectedPaletteIndex) brushState.foreground = color;
}

void Chromatide::updateKnobProfile() {
    const ChromatideKnobProfile next = chromatideKnobProfileFor(brushState.tool, brushState.head);
    if (next == activeKnobProfile) return;
    auto& from = knobProfiles[size_t(activeKnobProfile)];
    for (int i = 0; i < kChromatideKnobSlots; ++i) from[size_t(i)] = params[slotParam(i)].getValue();
    const auto& to = knobProfiles[size_t(next)];
    for (int i = 0; i < kChromatideKnobSlots; ++i) params[slotParam(i)].setValue(to[size_t(i)]);
    activeKnobProfile = next;
}

// Interpret the shared knobs for the active profile.
void Chromatide::syncBrushFromParams() {
    const int toolIdx = static_cast<int>(params[TOOL_PARAM].getValue());
    brushState.tool = static_cast<ChromatideTool>(clampVal(toolIdx, 0, 3));
    updateKnobProfile();

    const float a = params[BRUSH_FLOW_PARAM].getValue();
    const float b = params[BRUSH_OPACITY_PARAM].getValue();
    const float c = params[BRUSH_HARDNESS_PARAM].getValue();
    brushState.size = params[BRUSH_SIZE_PARAM].getValue();
    brushState.jitter = params[BRUSH_JITTER_PARAM].getValue();
    brushState.flow = a;
    brushState.opacity = b;
    brushState.hardness = c;
    switch (activeKnobProfile) {
        case ChromatideKnobProfile::Chisel:
            brushState.angle = c * 3.14159265f;
            brushState.hardness = 0.85f;
            // Slot 3 is thickness here, not jitter: 8%..100% of the nib length.
            brushState.thickness = 0.08f + 0.92f * brushState.jitter;
            brushState.jitter = 0.0f;
            break;
        case ChromatideKnobProfile::Spray:
            // At 10% spacing about 8 dabs overlap each pixel near the stroke
            // centre, so coverage = 1 - (1 - d)^8. Invert that so the knob
            // maps roughly linearly to stroke coverage across its whole travel.
            brushState.density = 1.0f - std::pow(1.0f - clampVal(a, 0.0f, 1.0f), 1.0f / 8.0f);
            brushState.flow = 0.6f;
            brushState.hardness = 1.0f - c;
            break;
        case ChromatideKnobProfile::Eraser:
            brushState.hardness = 1.0f - c;
            break;
        case ChromatideKnobProfile::Smudge:
            // Strength, Length, Softness, Jitter.
            brushState.flow = a;
            brushState.smudgeLength = b;
            brushState.hardness = 1.0f - c;
            break;
        case ChromatideKnobProfile::Pick:
            brushState.sampleRadius = static_cast<int>(std::lround((a - 0.01f) / 0.99f * 8.0f));
            brushState.jitter = 0.0f;
            break;
        default:
            break;
    }
}

void Chromatide::publishToIris() {
    auto source = std::make_shared<fractal::SourceField>();
    source->width = fractal::kCanonicalSourceWidth;
    source->height = fractal::kCanonicalSourceHeight;
    source->channels = fractal::kCanonicalSourceChannels;
    source->bitDepth = fractal::kCanonicalSourceBitDepth;
    source->rgb8.assign(canvas.pixels.begin(), canvas.pixels.end());
    source->sourceName = "Chromatide Canvas";
    source->sourcePath.clear();
    source->originalWidth = fractal::kCanonicalSourceWidth;
    source->originalHeight = fractal::kCanonicalSourceHeight;
    source->originalChannels = fractal::kCanonicalSourceChannels;
    source->generatorKind = fractal::SOURCE_GENERATOR_NONE;

    std::shared_ptr<const fractal::SourceField> immutableSource = std::move(source);
    std::atomic_store_explicit(
        &irisPublishedSource, std::move(immutableSource), std::memory_order_release);
    irisPreviewGeneration.fetch_add(1u, std::memory_order_release);
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
    json_object_set_new(brushJ, "flow", json_real(brushState.flow));
    json_object_set_new(brushJ, "hardness", json_real(brushState.hardness));
    json_object_set_new(brushJ, "tool", json_integer(static_cast<int>(brushState.tool)));
    json_object_set_new(brushJ, "head", json_integer(static_cast<int>(brushState.head)));
    {
        // Params already hold the active profile; refresh its stored copy first.
        auto profiles = knobProfiles;
        for (int i = 0; i < kChromatideKnobSlots; ++i)
            profiles[size_t(activeKnobProfile)][size_t(i)] = params[slotParam(i)].getValue();
        json_t* profilesJ = json_array();
        for (const auto& profile : profiles) {
            json_t* slotsJ = json_array();
            for (float v : profile) json_array_append_new(slotsJ, json_real(v));
            json_array_append_new(profilesJ, slotsJ);
        }
        json_object_set_new(brushJ, "knobProfiles", profilesJ);
    }

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
    // Undo records are pixel diffs against the old canvas; they are invalid
    // against loaded artwork.
    cancelStroke();
    undoStack.clear();
    redoStack.clear();

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
        json_t* flowJ = json_object_get(brushJ, "flow");
        if (flowJ && json_is_number(flowJ)) brushState.flow = static_cast<float>(json_number_value(flowJ));
        json_t* hardnessJ = json_object_get(brushJ, "hardness");
        if (hardnessJ && json_is_number(hardnessJ)) brushState.hardness = static_cast<float>(json_number_value(hardnessJ));

        json_t* toolJ = json_object_get(brushJ, "tool");
        if (toolJ && json_is_integer(toolJ)) {
            int t = static_cast<int>(json_integer_value(toolJ));
            brushState.tool = static_cast<ChromatideTool>(clampVal(t, 0, 3));
            params[TOOL_PARAM].setValue(static_cast<float>(clampVal(t, 0, 3)));
        }

        json_t* headJ = json_object_get(brushJ, "head");
        brushState.head = ChromatideBrushHead::Round;
        if (headJ && json_is_integer(headJ)) {
            const int h = static_cast<int>(json_integer_value(headJ));
            if (h >= 0 && h < static_cast<int>(ChromatideBrushHead::Count))
                brushState.head = static_cast<ChromatideBrushHead>(h);
        }

        json_t* profilesJ = json_object_get(brushJ, "knobProfiles");
        for (int p = 0; p < int(ChromatideKnobProfile::Count); ++p) {
            auto& profile = knobProfiles[size_t(p)];
            chromatideKnobProfileDefaults(ChromatideKnobProfile(p), profile.data());
            json_t* slotsJ = profilesJ && json_is_array(profilesJ) ? json_array_get(profilesJ, size_t(p)) : nullptr;
            if (!slotsJ || !json_is_array(slotsJ)) continue;
            for (int i = 0; i < kChromatideKnobSlots; ++i) {
                json_t* vJ = json_array_get(slotsJ, size_t(i));
                if (vJ && json_is_number(vJ)) profile[size_t(i)] = static_cast<float>(json_number_value(vJ));
            }
        }
        // Rack restores params before dataFromJson, so they already hold the
        // active profile; adopt it without overwriting them.
        activeKnobProfile = chromatideKnobProfileFor(brushState.tool, brushState.head);

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
        const int saved = static_cast<int>(json_integer_value(selPalJ));
        if (saved < 0) {
            selectedPaletteIndex = -1; // eyedropper color; foreground restored from "brush"
        } else {
            selectedPaletteIndex = clampVal(saved, 0, static_cast<int>(palette.size() - 1));
            brushState.foreground = palette[selectedPaletteIndex];
        }
    }
    publishToIris();
}
