#include "../src/ChromatideCanvas.hpp"
#include "../src/third_party/qoi.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>

static int decodes = 0;
void* qoi_encode(const void*, const qoi_desc*, int*) { return nullptr; }
void* qoi_decode(const void*, int, qoi_desc* desc, int) {
    ++decodes;
    desc->width = ChromatideCanvas::WIDTH;
    desc->height = ChromatideCanvas::HEIGHT;
    desc->channels = ChromatideCanvas::CHANNELS;
    return std::calloc(1, ChromatideCanvas::BUFFER_SIZE);
}
int main() {
    ChromatideCanvas canvas;
    canvas.clear(17, 41, 93, nullptr);
    const auto revision = canvas.revision;
    std::vector<uint8_t> bytes = {'q','o','i','f',0,0,4,0,0,0,1,0,3,0};
    size_t left = size_t(ChromatideCanvas::WIDTH) * ChromatideCanvas::HEIGHT;
    while (left) { size_t n = std::min(left, size_t(62)); bytes.push_back(uint8_t(0xc0 | (n-1))); left -= n; }
    bytes.insert(bytes.end(), 7, 0); bytes.push_back(1);
    auto reject = [&](const std::vector<uint8_t>& data) {
        assert(!canvas.deserializeQoiBase64(ChromatideCanvas::base64Encode(data.data(), data.size())));
        assert(decodes == 0 && canvas.revision == revision && canvas.pixels[0] == 17);
    };
    auto bad = bytes; bad[4] = 0x7f; reject(bad); // Huge declared allocation never reaches decoder.
    bad = bytes; bad[12] = 4; reject(bad);
    bad = bytes; bad[13] = 2; reject(bad);
    bad = bytes; bad[0] = 'x'; reject(bad);
    bad = bytes; bad.pop_back(); reject(bad);
    bad = bytes; bad.erase(bad.begin() + 14); reject(bad);
    bad = bytes; bad[bad.size()-9] = 0xfe; reject(bad); // truncated RGB opcode
    bad = bytes; bad.insert(bad.end()-8, 0); reject(bad); // extra payload
    reject({});
    assert(!canvas.deserializeQoiBase64(std::string(2 * 1024 * 1024, 'A')));
    assert(decodes == 0 && canvas.revision == revision);
    assert(canvas.deserializeQoiBase64(ChromatideCanvas::base64Encode(bytes.data(), bytes.size())));
    assert(decodes == 1 && canvas.revision == revision + 1);
    std::puts("Chromatide QOI preflight: invalid streams never reach decoder; valid stream accepted");
}
