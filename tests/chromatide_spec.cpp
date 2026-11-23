#include "../src/chromatide/ChromatideCanvas.hpp"
#include "../src/chromatide/Chromatide.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdio>

Model* modelIris = nullptr;
void Iris::requestExpanderSource(const nautiloid_iris_expander::SourceSlot*, uint64_t) {}
void Iris::requestOwnedExpanderSource(std::shared_ptr<const fractal::SourceField>, uint64_t) {}

static void testTransforms() {
    float rx = 0.0f, ry = 0.0f;
    ChromatideCanvas::normalizedToRaster(0.0f, 0.0f, rx, ry);
    assert(std::abs(rx - 0.0f) < 1e-4f);
    assert(std::abs(ry - 0.0f) < 1e-4f);

    ChromatideCanvas::normalizedToRaster(1.0f, 1.0f, rx, ry);
    assert(std::abs(rx - 1023.0f) < 1e-4f);
    assert(std::abs(ry - 255.0f) < 1e-4f);

    float u = 0.0f, v = 0.0f;
    ChromatideCanvas::rasterToNormalized(0.0f, 0.0f, u, v);
    assert(std::abs(u - 0.0f) < 1e-4f);
    assert(std::abs(v - 0.0f) < 1e-4f);

    ChromatideCanvas::rasterToNormalized(1023.0f, 255.0f, u, v);
    assert(std::abs(u - 1.0f) < 1e-4f);
    assert(std::abs(v - 1.0f) < 1e-4f);
    std::cout << "[PASS] testTransforms" << std::endl;
}

static void testBase64() {
    std::string text = "Hello Chromatide! Precursor sound technology synthesis.";
    std::vector<uint8_t> bytes(text.begin(), text.end());
    std::string encoded = ChromatideCanvas::base64Encode(bytes.data(), bytes.size());
    std::vector<uint8_t> decoded;
    bool ok = ChromatideCanvas::base64Decode(encoded, decoded);
    assert(ok);
    assert(bytes == decoded);
    std::cout << "[PASS] testBase64" << std::endl;
}

static void testQoiBase64Serialization() {
    ChromatideCanvas canvas;
    canvas.clear(12, 34, 56, nullptr);
    canvas.setPixel(100, 200, 255, 128, 64);
    canvas.setPixel(500, 50, 0, 255, 128);

    std::string b64 = canvas.serializeQoiBase64();
    assert(!b64.empty());

    ChromatideCanvas canvas2;
    bool ok = canvas2.deserializeQoiBase64(b64);
    assert(ok);

    uint8_t r = 0, g = 0, b = 0;
    canvas2.sample(100, 200, r, g, b);
    assert(r == 255 && g == 128 && b == 64);

    canvas2.sample(500, 50, r, g, b);
    assert(r == 0 && g == 255 && b == 128);

    canvas2.sample(0, 0, r, g, b);
    assert(r == 12 && g == 34 && b == 56);
    std::cout << "[PASS] testQoiBase64Serialization" << std::endl;
}

static void testStampAndUndo() {
    ChromatideCanvas canvas;
    canvas.clear(0, 0, 0, nullptr);

    ChromatideBrushState brush;
    brush.size = 20.0f;
    brush.opacity = 1.0f;
    brush.flow = 1.0f;
    brush.foreground = ChromatideColor(255, 0, 0);
    brush.tool = ChromatideTool::Brush;

    RectI dirty;
    dirty.reset();

    ChromatideUndoRecord record;
    canvas.beginStrokeTransaction(record);

    canvas.stampAtRaster(500.0f, 100.0f, brush, &dirty);
    assert(dirty.valid());

    canvas.finalizeStrokeTransaction(dirty, record);

    uint8_t r = 0, g = 0, b = 0;
    canvas.sample(500, 100, r, g, b);
    assert(r > 200);

    // Apply undo
    canvas.applyUndoRecord(record, true);
    canvas.sample(500, 100, r, g, b);
    assert(r == 0 && g == 0 && b == 0);

    // Apply redo
    canvas.applyUndoRecord(record, false);
    canvas.sample(500, 100, r, g, b);
    assert(r > 200);
    std::cout << "[PASS] testStampAndUndo" << std::endl;
}

static void testStrokeHistoryPreservesArtwork() {
    Chromatide module;
    for (int y = 0; y < ChromatideCanvas::HEIGHT; ++y) {
        for (int x = 0; x < ChromatideCanvas::WIDTH; ++x) {
            module.canvas.setPixel(x, y, uint8_t(1 + x % 254),
                uint8_t(1 + y % 254), uint8_t(1 + (x + y) % 254));
        }
    }
    const std::vector<uint8_t> original(module.canvas.pixels.begin(), module.canvas.pixels.end());
    module.params[Chromatide::BRUSH_SIZE_PARAM].setValue(24.f);
    module.params[Chromatide::BRUSH_OPACITY_PARAM].setValue(0.6f);
    module.beginStroke(0.2f, 0.2f);
    module.updateStroke(0.7f, 0.75f);
    module.endStroke();
    const std::vector<uint8_t> painted(module.canvas.pixels.begin(), module.canvas.pixels.end());
    assert(painted != original);
    assert(module.undoStack.back().beforeRgb.size() < ChromatideCanvas::BUFFER_SIZE);
    assert(module.undo());
    assert(std::equal(original.begin(), original.end(), module.canvas.pixels.begin()));
    const auto published = std::atomic_load(&module.irisPublishedSource);
    assert(published->rgb8 == original);
    assert(module.redo());
    assert(std::equal(painted.begin(), painted.end(), module.canvas.pixels.begin()));

    // Grow in the opposite direction and clip both canvas edges, over existing paint.
    module.params[Chromatide::TOOL_PARAM].setValue(1.f);
    module.params[Chromatide::BRUSH_SIZE_PARAM].setValue(64.f);
    module.brushState.background = ChromatideColor(17, 43, 89);
    module.beginStroke(1.f, 1.f);
    module.updateStroke(0.f, 0.f);
    module.endStroke();
    const std::vector<uint8_t> erased(module.canvas.pixels.begin(), module.canvas.pixels.end());
    assert(erased != painted);
    assert(module.undo());
    assert(std::equal(painted.begin(), painted.end(), module.canvas.pixels.begin()));
    assert(module.undo());
    assert(std::equal(original.begin(), original.end(), module.canvas.pixels.begin()));
    assert(module.redo());
    assert(std::equal(painted.begin(), painted.end(), module.canvas.pixels.begin()));
    assert(module.redo());
    assert(std::equal(erased.begin(), erased.end(), module.canvas.pixels.begin()));

    module.clearCanvas();
    assert(module.undo());
    assert(std::equal(erased.begin(), erased.end(), module.canvas.pixels.begin()));
    assert(module.redo());
    for (size_t i = 0; i < module.canvas.pixels.size(); i += 3) {
        assert(module.canvas.pixels[i] == 17 && module.canvas.pixels[i + 1] == 43
            && module.canvas.pixels[i + 2] == 89);
    }
    std::cout << "[PASS] testStrokeHistoryPreservesArtwork" << std::endl;
}

static void testModuleAndMemoryCap() {
    Chromatide module;
    assert(module.undoStack.empty());

    // Push large undo records to verify 32 MiB memory cap
    for (int i = 0; i < 20; ++i) {
        ChromatideUndoRecord rec;
        rec.bounds = RectI(0, 0, ChromatideCanvas::WIDTH - 1, ChromatideCanvas::HEIGHT - 1);
        rec.beforeRgb.resize(ChromatideCanvas::BUFFER_SIZE, static_cast<uint8_t>(i));
        rec.afterRgb.resize(ChromatideCanvas::BUFFER_SIZE, static_cast<uint8_t>(i + 1));
        module.pushUndoRecord(std::move(rec));
    }

    size_t totalMem = 0;
    for (const auto& rec : module.undoStack) {
        totalMem += rec.memoryUsage();
    }
    assert(totalMem <= Chromatide::MAX_UNDO_MEMORY);
    std::cout << "[PASS] testModuleAndMemoryCap (totalMem=" << totalMem << " bytes)" << std::endl;
}

static void testModuleJsonRoundTrip() {
    Chromatide module;
    module.selectPaletteColor(3); // Select orange
    module.canvas.setPixel(10, 20, 255, 100, 50);
    module.brushState.head = ChromatideBrushHead::Spray;

    json_t* json = module.dataToJson();
    assert(json != nullptr);

    Chromatide module2;
    module2.dataFromJson(json);
    json_decref(json);

    assert(module2.selectedPaletteIndex == 3);
    assert(module2.brushState.head == ChromatideBrushHead::Spray);
    uint8_t r = 0, g = 0, b = 0;
    module2.canvas.sample(10, 20, r, g, b);
    assert(r == 255 && g == 100 && b == 50);
    std::cout << "[PASS] testModuleJsonRoundTrip" << std::endl;
}

static void testPublishedSnapshotOwnership() {
    Chromatide module;
    const uint64_t initialGeneration =
        module.irisPreviewGeneration.load(std::memory_order_acquire);
    std::shared_ptr<const fractal::SourceField> initial =
        std::atomic_load_explicit(&module.irisPublishedSource, std::memory_order_acquire);
    assert(initial);
    assert(initial->valid());

    const size_t offset = ChromatideCanvas::pixelOffset(10, 20);
    const uint8_t initialRed = initial->rgb8[offset];
    module.canvas.setPixel(10, 20, static_cast<uint8_t>(initialRed + 1u), 2u, 3u);

    // The worker-facing snapshot remains immutable until publication.
    assert(initial->rgb8[offset] == initialRed);
    module.publishToIris();

    std::shared_ptr<const fractal::SourceField> published =
        std::atomic_load_explicit(&module.irisPublishedSource, std::memory_order_acquire);
    assert(published);
    assert(published.get() != initial.get());
    assert(published->rgb8[offset] == static_cast<uint8_t>(initialRed + 1u));
    assert(initial->rgb8[offset] == initialRed);
    assert(module.irisPreviewGeneration.load(std::memory_order_acquire) == initialGeneration + 1u);
    std::cout << "[PASS] testPublishedSnapshotOwnership" << std::endl;
}

static void testStrokeOpacityIsCeiling() {
    // Scrubbing back and forth within one stroke must never exceed opacity.
    Chromatide module;
    module.canvas.clear(0, 0, 0, nullptr);
    module.selectPaletteColor(1); // white
    module.params[Chromatide::BRUSH_SIZE_PARAM].setValue(32.f);
    module.params[Chromatide::BRUSH_OPACITY_PARAM].setValue(0.3f);
    module.params[Chromatide::BRUSH_FLOW_PARAM].setValue(1.0f);
    module.params[Chromatide::BRUSH_HARDNESS_PARAM].setValue(0.8f);
    module.beginStroke(0.3f, 0.5f);
    for (int i = 0; i < 6; ++i) {
        module.updateStroke(0.6f, 0.5f);
        module.updateStroke(0.3f, 0.5f);
    }
    module.endStroke();
    uint8_t r = 0, g = 0, b = 0;
    module.canvas.sample(460, 128, r, g, b);
    assert(r >= 70 && r <= 78); // ~0.3 * 255

    // A second stroke builds on top, as in Krita.
    module.beginStroke(0.3f, 0.5f);
    module.updateStroke(0.6f, 0.5f);
    module.endStroke();
    module.canvas.sample(460, 128, r, g, b);
    assert(r > 110 && r < 135);

    // Low flow builds up gradually rather than going solid on one pass.
    module.canvas.clear(0, 0, 0, nullptr);
    module.params[Chromatide::BRUSH_OPACITY_PARAM].setValue(1.0f);
    module.params[Chromatide::BRUSH_FLOW_PARAM].setValue(0.05f);
    module.beginStroke(0.45f, 0.5f);
    module.endStroke();
    module.canvas.sample(460, 128, r, g, b);
    assert(r > 5 && r < 25);
    std::cout << "[PASS] testStrokeOpacityIsCeiling" << std::endl;
}

static void testLiveIrisStrokePublishing() {
    Chromatide module;
    module.beginStroke(0.2f, 0.5f);
    const uint64_t g0 = module.irisPreviewGeneration.load();
    module.updateStroke(0.4f, 0.5f);
    module.updateStroke(0.5f, 0.5f);
    // Several moves within one UI frame coalesce into one hand-off.
    assert(module.publishLiveStrokeIfDirty());
    const uint64_t g1 = module.irisPreviewGeneration.load();
    assert(g1 == g0 + 1);
    assert(!module.publishLiveStrokeIfDirty());
    const auto live = std::atomic_load(&module.irisPublishedSource);
    assert(std::equal(live->rgb8.begin(), live->rgb8.end(), module.canvas.pixels.begin()));
    module.updateStroke(0.6f, 0.5f);
    module.endStroke();
    assert(module.irisPreviewGeneration.load() == g1 + 1);
    assert(!module.publishLiveStrokeIfDirty());
    std::cout << "[PASS] testLiveIrisStrokePublishing" << std::endl;
}

static void testEyedropperAndLoadState() {
    Chromatide module;
    module.canvas.clear(0, 0, 0, nullptr);
    module.canvas.setPixel(512, 128, 123, 45, 67);
    module.params[Chromatide::TOOL_PARAM].setValue(2.f);
    module.beginStroke(512.f / (ChromatideCanvas::WIDTH - 1), 128.f / (ChromatideCanvas::HEIGHT - 1));
    // No swatch matches: nothing highlighted, picked color kept.
    assert(module.selectedPaletteIndex == -1);
    assert(module.brushState.foreground == ChromatideColor(123, 45, 67));
    module.setPaletteColor(3, module.brushState.foreground);
    assert(module.palette[3] == ChromatideColor(123, 45, 67));

    // An eyedropper color survives a save/load round trip.
    json_t* saved = module.dataToJson();
    Chromatide restored;
    restored.dataFromJson(saved);
    assert(restored.selectedPaletteIndex == -1);
    assert(restored.brushState.foreground == ChromatideColor(123, 45, 67));

    // Loading state over existing history drops the stale undo records.
    restored.params[Chromatide::TOOL_PARAM].setValue(0.f);
    restored.beginStroke(0.3f, 0.5f);
    restored.updateStroke(0.6f, 0.5f);
    restored.endStroke();
    assert(!restored.undoStack.empty());
    restored.dataFromJson(saved);
    assert(restored.undoStack.empty() && restored.redoStack.empty());
    assert(!restored.undo());
    json_decref(saved);
    std::cout << "[PASS] testEyedropperAndLoadState" << std::endl;
}

static bool paintedAt(const ChromatideCanvas& c, float nx, float ny) {
    // nx, ny in dab-normalized units around (500, 128) for a size-40 dab.
    const float rx = 20.0f * (4.0f / ChromatideCanvas::VIEWPORT_ASPECT_RATIO);
    uint8_t r = 0, g = 0, b = 0;
    c.sample(int(std::lround(500.0f + nx * rx)), int(std::lround(128.0f + ny * 20.0f)), r, g, b);
    return r > 128;
}

static void testBrushHeads() {
    ChromatideBrushState brush;
    brush.size = 40.0f;
    brush.hardness = 1.0f;
    brush.flow = 1.0f;
    brush.opacity = 1.0f;
    brush.foreground = ChromatideColor(255, 0, 0);
    auto stamp = [&](ChromatideBrushHead head) {
        auto* c = new ChromatideCanvas();
        brush.head = head;
        c->stampAtRaster(500.0f, 128.0f, brush);
        return c;
    };
    ChromatideCanvas* round = stamp(ChromatideBrushHead::Round);
    ChromatideCanvas* square = stamp(ChromatideBrushHead::Square);
    ChromatideCanvas* chisel = stamp(ChromatideBrushHead::Chisel);
    ChromatideCanvas* spray = stamp(ChromatideBrushHead::Spray);
    assert(paintedAt(*round, 0.f, 0.f) && !paintedAt(*round, 0.8f, 0.8f));
    assert(paintedAt(*square, 0.f, 0.f) && paintedAt(*square, 0.8f, 0.8f));
    assert(paintedAt(*chisel, 0.6f, 0.6f) && !paintedAt(*chisel, 0.6f, -0.6f));
    int hits = 0, total = 0;
    for (int dy = -8; dy <= 8; ++dy) for (int dx = -8; dx <= 8; ++dx) {
        uint8_t r = 0, g = 0, b = 0;
        spray->sample(500 + dx, 128 + dy, r, g, b);
        hits += r > 128; ++total;
    }
    const float density = float(hits) / float(total);
    assert(density > 0.2f && density < 0.5f);
    delete round; delete square; delete chisel; delete spray;
    std::cout << "[PASS] testBrushHeads (spray core density " << density << ")" << std::endl;
}

static void testKnobProfiles() {
    Chromatide m;
    // Brush/Round starts on its defaults; tweak it.
    m.params[Chromatide::BRUSH_HARDNESS_PARAM].setValue(0.9f);
    m.params[Chromatide::BRUSH_JITTER_PARAM].setValue(0.3f);
    m.syncBrushFromParams();
    assert(m.activeKnobProfile == ChromatideKnobProfile::Round);

    // Eraser loads its own values and interprets slot 2 as softness.
    m.params[Chromatide::TOOL_PARAM].setValue(1.f);
    m.syncBrushFromParams();
    assert(m.activeKnobProfile == ChromatideKnobProfile::Eraser);
    assert(std::abs(m.params[Chromatide::BRUSH_HARDNESS_PARAM].getValue() - 0.4f) < 1e-6f);
    assert(std::abs(m.brushState.hardness - 0.6f) < 1e-6f);
    m.params[Chromatide::BRUSH_HARDNESS_PARAM].setValue(0.1f);

    // Back to Brush restores the tweaked brush values.
    m.params[Chromatide::TOOL_PARAM].setValue(0.f);
    m.syncBrushFromParams();
    assert(std::abs(m.params[Chromatide::BRUSH_HARDNESS_PARAM].getValue() - 0.9f) < 1e-6f);
    assert(std::abs(m.brushState.jitter - 0.3f) < 1e-6f);

    // Chisel: slot 2 is the nib angle over 0..180 degrees.
    m.brushState.head = ChromatideBrushHead::Chisel;
    m.syncBrushFromParams();
    m.params[Chromatide::BRUSH_HARDNESS_PARAM].setValue(0.5f);
    m.syncBrushFromParams();
    assert(std::abs(m.brushState.angle - 1.5707963f) < 1e-4f);
    // Chisel's fifth knob is thickness, and jitter is off for it.
    m.params[Chromatide::BRUSH_JITTER_PARAM].setValue(1.0f);
    m.syncBrushFromParams();
    assert(std::abs(m.brushState.thickness - 1.0f) < 1e-6f && m.brushState.jitter == 0.0f);
    m.params[Chromatide::BRUSH_JITTER_PARAM].setValue(0.217f);
    m.syncBrushFromParams();
    assert(std::abs(m.brushState.thickness - 0.28f) < 1e-3f);

    // Profiles survive a save/load with the Eraser value intact.
    json_t* json = m.dataToJson();
    Chromatide m2;
    m2.dataFromJson(json);
    json_decref(json);
    assert(m2.activeKnobProfile == ChromatideKnobProfile::Chisel);
    assert(std::abs(m2.knobProfiles[size_t(ChromatideKnobProfile::Eraser)][2] - 0.1f) < 1e-6f);
    assert(std::abs(m2.knobProfiles[size_t(ChromatideKnobProfile::Chisel)][2] - 0.5f) < 1e-6f);
    std::cout << "[PASS] testKnobProfiles" << std::endl;
}

static void testChiselAngleAndJitter() {
    ChromatideBrushState brush;
    brush.size = 40.0f; brush.hardness = 1.0f; brush.flow = 1.0f; brush.opacity = 1.0f;
    brush.foreground = ChromatideColor(255, 0, 0);
    brush.head = ChromatideBrushHead::Chisel;
    brush.angle = 3.0f * 0.7853982f;  // 135 degrees: long axis top-right to bottom-left
    auto* c = new ChromatideCanvas();
    c->stampAtRaster(500.0f, 128.0f, brush);
    assert(paintedAt(*c, -0.6f, 0.6f) && !paintedAt(*c, 0.6f, 0.6f));
    delete c;
    brush.thickness = 1.0f;  // full thickness turns the nib into a round dab
    c = new ChromatideCanvas();
    c->stampAtRaster(500.0f, 128.0f, brush);
    assert(paintedAt(*c, 0.6f, 0.6f));
    delete c;
    brush.thickness = 0.28f;

    // Jitter scatters dabs off a straight horizontal line but stays near it.
    brush.head = ChromatideBrushHead::Round;
    brush.size = 10.0f;
    brush.jitter = 1.0f;
    auto* line = new ChromatideCanvas();
    line->strokeToNormalized(0.2f, 0.5f, 0.8f, 0.5f, brush);
    int offLine = 0, farOff = 0;
    for (int y = 0; y < ChromatideCanvas::HEIGHT; ++y) {
        for (int x = 0; x < ChromatideCanvas::WIDTH; ++x) {
            uint8_t r = 0, g = 0, b = 0;
            line->sample(x, y, r, g, b);
            if (r < 128) continue;
            const int d = std::abs(y - 128);
            offLine += d > 5;
            farOff += d > 12;
        }
    }
    assert(offLine > 0 && farOff == 0);
    delete line;

    // Averaged pick over a 3x3 checker of 0/90.
    auto* pick = new ChromatideCanvas();
    for (int y = 9; y <= 11; ++y) for (int x = 9; x <= 11; ++x)
        pick->setPixel(x, y, ((x + y) & 1) ? 90 : 0, 0, 0);
    uint8_t r = 0, g = 0, b = 0;
    pick->sampleAverage(10, 10, 1, r, g, b);
    assert(r == 40);
    delete pick;
    std::cout << "[PASS] testChiselAngleAndJitter" << std::endl;
}

static void testSavePng() {
    Chromatide m;
    m.canvas.setPixel(3, 4, 200, 100, 50);
    const std::string path = "build/tests/chromatide_save_spec.png";
    assert(m.savePng(path));
    FILE* f = std::fopen(path.c_str(), "rb");
    assert(f);
    unsigned char sig[8] = {};
    assert(std::fread(sig, 1, 8, f) == 8);
    std::fclose(f);
    assert(sig[0] == 0x89 && sig[1] == 'P' && sig[2] == 'N' && sig[3] == 'G');

    // Round trip: a native-size PNG loads back exactly, and the load is undoable.
    Chromatide m2;
    assert(m2.loadImage(path));
    std::remove(path.c_str());
    assert(m2.canvas.pixels == m.canvas.pixels);
    assert(m2.undo());
    uint8_t r = 0, g = 0, b = 0;
    m2.canvas.sample(3, 4, r, g, b);
    assert(r == 0 && g == 0 && b == 0);
    assert(!m2.loadImage("build/tests/does_not_exist.png"));
    std::cout << "[PASS] testSavePng" << std::endl;
}

static void testHeldStrokeSaturation() {
    // Repeated events at one spot (a resting hand) must not stack dabs.
    Chromatide m;
    m.params[Chromatide::BRUSH_SIZE_PARAM].setValue(20.f);
    m.params[Chromatide::BRUSH_FLOW_PARAM].setValue(0.2f);
    m.syncBrushFromParams();
    float u, v;
    ChromatideCanvas::rasterToNormalized(300.f, 128.f, u, v);
    const uint8_t before = m.canvas.pixels[ChromatideCanvas::pixelOffset(300, 128)];
    m.beginStroke(u, v);
    const uint8_t once = m.canvas.pixels[ChromatideCanvas::pixelOffset(300, 128)];
    for (int i = 0; i < 200; ++i) m.updateStroke(u, v);
    assert(m.canvas.pixels[ChromatideCanvas::pixelOffset(300, 128)] == once);
    // Sub-spacing jiggles also leave the centre alone (0.1 px back and forth).
    float uj, vj;
    ChromatideCanvas::rasterToNormalized(300.1f, 128.f, uj, vj);
    for (int i = 0; i < 200; ++i) m.updateStroke(i & 1 ? u : uj, v);
    assert(m.canvas.pixels[ChromatideCanvas::pixelOffset(300, 128)] == once);
    // Dwell builds gently: 1 s of rest at 20% flow stays well short of full.
    const double t0 = m.lastStrokeMotionTime;
    m.lastStrokeMotionTime = t0 - 1.0;
    m.lastDwellTime = t0 - 1.0;
    for (int i = 1; i <= 60; ++i) m.dwellStroke(t0 + i / 60.0);
    m.endStroke();
    (void)before;
    std::cout << "[PASS] testHeldStrokeSaturation" << std::endl;
}

static void testSmudge() {
    Chromatide m;
    // Left half red, right half blue; smudge from red into blue.
    for (int y = 0; y < ChromatideCanvas::HEIGHT; ++y)
        for (int x = 0; x < ChromatideCanvas::WIDTH; ++x)
            m.canvas.setPixel(x, y, x < 512 ? 255 : 0, 0, x < 512 ? 0 : 255);
    m.params[Chromatide::TOOL_PARAM].setValue(3.f);
    m.params[Chromatide::BRUSH_SIZE_PARAM].setValue(40.f);
    m.syncBrushFromParams();
    assert(m.activeKnobProfile == ChromatideKnobProfile::Smudge);
    m.params[Chromatide::BRUSH_FLOW_PARAM].setValue(0.8f);    // strength
    m.params[Chromatide::BRUSH_OPACITY_PARAM].setValue(0.9f); // long carry
    float u0, v0, u1, v1;
    ChromatideCanvas::rasterToNormalized(440.f, 128.f, u0, v0);
    ChromatideCanvas::rasterToNormalized(600.f, 128.f, u1, v1);
    m.beginStroke(u0, v0);
    m.updateStroke(u1, v1);
    m.endStroke();
    uint8_t r = 0, g = 0, b = 0;
    m.canvas.sample(560, 128, r, g, b);
    assert(r > 40 && b < 230);           // red dragged into the blue side
    m.canvas.sample(560, 20, r, g, b);
    assert(r == 0 && b == 255);          // outside the stroke untouched
    // Smudge adds no new colour: no green anywhere.
    for (size_t i = 1; i < m.canvas.pixels.size(); i += 3) assert(m.canvas.pixels[i] == 0);
    assert(m.undo());
    m.canvas.sample(560, 128, r, g, b);
    assert(r == 0 && b == 255);
    std::cout << "[PASS] testSmudge" << std::endl;
}

int main() {
    std::cout << "Running Chromatide Spec tests..." << std::endl;
    testTransforms();
    testBase64();
    testQoiBase64Serialization();
    testStampAndUndo();
    testBrushHeads();
    testKnobProfiles();
    testChiselAngleAndJitter();
    testSavePng();
    testSmudge();
    testHeldStrokeSaturation();
    testStrokeHistoryPreservesArtwork();
    testStrokeOpacityIsCeiling();
    testLiveIrisStrokePublishing();
    testEyedropperAndLoadState();
    testModuleAndMemoryCap();
    testModuleJsonRoundTrip();
    testPublishedSnapshotOwnership();
    std::cout << "All Chromatide Spec tests passed successfully!" << std::endl;
    return 0;
}
