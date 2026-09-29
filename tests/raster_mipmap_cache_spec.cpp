// Real cache and PNG preflight; only the NanoVG resource boundary is mocked.
#include "../src/visual/RasterImageAssets.cpp"
#include <cstdio>
#include <cstdlib>
#include <map>

Plugin* pluginInstance = nullptr;
struct NVGcontext { int id; };
namespace rack { namespace window { Image::~Image() {} } }
static int creates = 0, deletes = 0;
static bool failCreate = false, failSize = false;
static std::map<int, NVGcontext*> textures;
int nvgCreateImage(NVGcontext* vg, const char*, int) {
    if (failCreate) return 0;
    textures[++creates] = vg;
    return creates;
}
int nvgCreateImageRGBA(NVGcontext* vg, int, int, int, const unsigned char*) {
    return nvgCreateImage(vg, "", 0);
}
void nvgImageSize(NVGcontext* vg, int image, int* w, int* h) {
    const auto it = textures.find(image);
    *w = *h = !failSize && it != textures.end() && it->second == vg ? 256 : 0;
}
void nvgDeleteImage(NVGcontext* vg, int image) {
    if (!textures.count(image) || textures[image] != vg) std::abort();
    ++deletes;
    textures.erase(image);
}
void nvgUpdateImage(NVGcontext*, int, const unsigned char*) {}
static void need(bool condition, const char* message) {
    if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
int main() {
    NVGcontext main{1}, framebuffer{2}, otherWindow{3};
    auto image = std::make_shared<rack::window::Image>();
    image->vg = &main;
    image->handle = 9000;
    const char* path = "res/icon/LeviathanVU-256.png";
    const auto load = [&](NVGcontext* vg) {
        return visual_assets::loadRasterMipmapHandle(vg, image, path);
    };
    const int first = load(&main), second = load(&framebuffer);
    need(first > 0 && second > 0 && first != second, "each live context owns its texture");
    for (unsigned i = 0; i < 120; ++i)
        need(load(&main) == first && load(&framebuffer) == second,
             "alternating live contexts retain both images");
    need(creates == 2 && !deletes, "steady rendering never recreates images");
    auto other = std::make_shared<rack::window::Image>();
    other->vg = &otherWindow;
    other->handle = 9001;
    const int third = visual_assets::loadRasterMipmapHandle(&otherWindow, other, path);
    visual_assets::onRasterContextDestroy(&main);
    need(visual_assets::rasterMipmapCache().contexts.size() == 1,
         "main destruction forgets its framebuffer context too");
    need(visual_assets::loadRasterMipmapHandle(&otherWindow, other, path) == third,
         "another live window retains its cache");
    textures.erase(first); textures.erase(second); // Simulated host destruction.
    visual_assets::onRasterContextCreate(&main);
    const int recreated = load(&main);
    need(recreated != first && load(&framebuffer) != second,
         "reused context addresses rebuild lazily");
    visual_assets::onRasterContextCreate(&main); // Missed old destroy event.
    need(load(&main) != recreated, "create event invalidates even identical pointers");
    need(!deletes, "never delete possibly reassigned old handles");
    failCreate = true;
    need(visual_assets::loadRasterMipmapHandle(&main, image, "missing.png") == -1,
         "zero NanoVG creation result is failure");
    failCreate = false;
    failSize = true;
    need(visual_assets::loadRasterMipmapHandle(&main, image, "invalid-size.png") == -1 && deletes == 1,
         "a freshly created invalid-size image is reclaimed in its owning context");
    std::puts("PASS: raster cache retains concurrent contexts, isolates windows, rebuilds on lifecycle events, and handles failures");
}
