#include "../src/ChromatideCanvas.hpp"
#include "../src/Chromatide.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

Model* modelIris = nullptr;
void Iris::requestExpanderSource(const nautiloid_iris_expander::SourceSlot*, uint64_t) {}

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
    brush.foreground = ChromatideColor(255, 0, 0);
    brush.tool = ChromatideTool::Brush;

    RectI dirty;
    dirty.reset();

    ChromatideUndoRecord record;
    canvas.beginStrokeTransaction(dirty, record);

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

    json_t* json = module.dataToJson();
    assert(json != nullptr);

    Chromatide module2;
    module2.dataFromJson(json);
    json_decref(json);

    assert(module2.selectedPaletteIndex == 3);
    uint8_t r = 0, g = 0, b = 0;
    module2.canvas.sample(10, 20, r, g, b);
    assert(r == 255 && g == 100 && b == 50);
    std::cout << "[PASS] testModuleJsonRoundTrip" << std::endl;
}

int main() {
    std::cout << "Running Chromatide Spec tests..." << std::endl;
    testTransforms();
    testBase64();
    testQoiBase64Serialization();
    testStampAndUndo();
    testModuleAndMemoryCap();
    testModuleJsonRoundTrip();
    std::cout << "All Chromatide Spec tests passed successfully!" << std::endl;
    return 0;
}

