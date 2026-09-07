#include "../src/NvgGraphicsLifecycle.hpp"
#include <nanovg.h>

#include <cstdio>
#include <cstdlib>

// Fake only the four NanoVG boundary calls; exercise the real helper without
// requiring a live GL context or deliberately exhausting graphics memory.
struct NVGcontext { int id; };
namespace {
int createResult = 0;
int creates = 0;
int updates = 0;
int deletes = 0;
int queries = 0;
int imageWidth = 0;
int imageHeight = 0;
NVGcontext* imageOwner = nullptr;
unsigned checks = 0;

void require(bool ok, const char* message) {
  ++checks;
  if (!ok) {
    std::fprintf(stderr, "FAIL: %s\n", message);
    std::abort();
  }
}
}

int nvgCreateImageRGBA(NVGcontext* vg, int w, int h, int, const unsigned char*) {
  ++creates;
  imageOwner = vg;
  imageWidth = w;
  imageHeight = h;
  return createResult;
}
void nvgUpdateImage(NVGcontext* vg, int image, const unsigned char*) {
  require(image > 0 && vg == imageOwner, "update uses a valid context-owned image");
  ++updates;
}
void nvgImageSize(NVGcontext* vg, int image, int* w, int* h) {
  require(image > 0 && vg == imageOwner, "query uses a valid context-owned image");
  ++queries;
  *w = imageWidth;
  *h = imageHeight;
}
void nvgDeleteImage(NVGcontext* vg, int image) {
  require(image > 0 && vg == imageOwner, "delete uses a valid context-owned image");
  ++deletes;
}

int main() {
  using namespace nvg_gfx_lifecycle;
  NVGcontext first{1}, second{2};
  NVGcontext* owner = nullptr;
  int handle = -1, width = 0, height = 0;
  unsigned char pixels[16] = {};
  auto upload = [&](NVGcontext* vg, int w = 2, int h = 2) {
    return updateOwnedNvgImageRgba(owner, handle, width, height, vg, w, h, 0, pixels);
  };

  require(!upload(&first), "zero creation handle reports failure");
  require(handle == -1 && width == 0 && height == 0, "failed upload leaves no valid image metadata");
  require(creates == 1 && updates == 0 && deletes == 0, "failed create is not updated or deleted");
  createResult = -3;
  require(!upload(&first) && handle == -1, "negative failure also uses shared invalid sentinel");
  createResult = 7;
  require(upload(&first), "creation recovers on a later retry");
  require(handle == 7 && owner == &first && width == 2 && height == 2, "successful upload publishes dimensions");
  require(upload(&first) && creates == 3 && updates == 1, "stable image updates without recreation");

  createResult = 0;
  require(!upload(&first, 1, 1), "resize creation failure is reported");
  require(deletes == 1 && handle == -1 && width == 0 && height == 0, "resize retires old image and invalidates metadata");
  createResult = 8;
  require(upload(&first), "resize failure can recover");
  const int beforeSwitchDeletes = deletes;
  createResult = 9;
  require(upload(&second), "context switch recreates the image");
  require(owner == &second && handle == 9 && deletes == beforeSwitchDeletes,
      "context switch never deletes an old-context handle");
  resetOwnedNvgImage(owner, handle, width, height, &second, true);
  require(deletes == beforeSwitchDeletes + 1 && handle == -1 && !owner, "valid same-context reset still deletes");

  const int beforeQueries = queries;
  require(!ownedNvgImageSizeMatches(&second, 0, 0, 0), "zero image cannot match even empty dimensions");
  require(queries == beforeQueries, "invalid handle is not passed to NanoVG size query");
  owner = &second;
  handle = 0;
  const int beforeInvalidDelete = deletes;
  resetOwnedNvgImage(owner, handle, width, height, &second, true);
  require(deletes == beforeInvalidDelete && handle == -1, "legacy zero handle is normalized without deletion");
  std::printf("NanoVG image lifecycle: %u checks passed\n", checks);
}
