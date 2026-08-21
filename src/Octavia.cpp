#if defined(_WIN32)
// cpp-httplib 0.48 uses Windows 10 APIs (CreateFile2 and
// CreateFileMappingFromApp). Set the SDK target before Rack or any other
// header has a chance to include the Windows headers.
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#elif _WIN32_WINNT < 0x0A00
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER _WIN32_WINNT
#elif WINVER < 0x0A00
#undef WINVER
#define WINVER 0x0A00
#endif
#endif

#include "plugin.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "TemporalDeck.hpp"
#include "SibylControl.hpp"
#include "third_party/httplib.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <chrono>
#include <memory>
#include <algorithm>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <cctype>
#include <cstring>
#if defined(_WIN32)
  #include <windows.h>
#else
  #include <sys/resource.h>
#endif
#include <patch.hpp>

// ── JSON helpers ──────────────────────────────────────────────────────────────
static std::string jStr(const std::string& s) {
    std::string r = "\"";
    for (unsigned char c : s) {
        if      (c == '"')  r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\r') r += "\\r";
        else if (c == '\t') r += "\\t";
        else if (c < 0x20)  { char buf[8]; snprintf(buf, sizeof(buf), "\\u%04x", c); r += buf; }
        else r += (char)c;
    }
    return r + "\"";
}

static std::string jNum(float f) {
    if (std::isnan(f) || std::isinf(f)) return "0";
    return std::to_string(f);
}

static float parseFloatField(const std::string& body, const std::string& key) {
    auto p = body.find("\"" + key + "\"");
    if (p == std::string::npos) return 0.f;
    p = body.find(":", p);
    if (p == std::string::npos) return 0.f;
    try { return std::stof(body.substr(p + 1)); } catch (...) { return 0.f; }
}

static int64_t parseInt64Field(const std::string& body, const std::string& key) {
    auto p = body.find("\"" + key + "\"");
    if (p == std::string::npos) return -1;
    p = body.find(":", p);
    if (p == std::string::npos) return -1;
    try { return std::stoll(body.substr(p + 1)); } catch (...) { return -1; }
}

static std::string parseStringField(const std::string& body, const std::string& key) {
    auto p = body.find("\"" + key + "\"");
    if (p == std::string::npos) return "";
    p = body.find(":", p);
    if (p == std::string::npos) return "";
    auto q1 = body.find("\"", p);
    if (q1 == std::string::npos) return "";
    std::string out;
    for (size_t i = q1 + 1; i < body.size(); i++) {
        char c = body[i];
        if (c == '\\' && i + 1 < body.size()) { out += body[i + 1]; i++; continue; }
        if (c == '"') break;
        out += c;
    }
    return out;
}

static bool parseBoolField(const std::string& body, const std::string& key) {
    auto p = body.find("\"" + key + "\"");
    if (p == std::string::npos) return false;
    p = body.find(":", p);
    if (p == std::string::npos) return false;
    while (p < body.size() && (body[p] == ':' || body[p] == ' ')) p++;
    return p + 4 <= body.size() && body.substr(p, 4) == "true";
}

// ── Config from environment ───────────────────────────────────────────────────
static int octaviaPort() {
    const char* e = getenv("OCTAVIA_PORT");
    if (e) { int p = atoi(e); if (p > 0 && p < 65536) return p; }
    return 7777;
}
static std::string octaviaToken() {
    const char* e = getenv("OCTAVIA_TOKEN");
    return e ? std::string(e) : std::string();
}

// ── Note name from frequency ──────────────────────────────────────────────────
static std::string freqToNote(float freq) {
    if (freq <= 0.f) return "?";
    static const char* names[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    float midi = 12.0f * log2f(freq / 440.0f) + 69.0f;
    int m = (int)roundf(midi);
    int oct = m / 12 - 1;
    int n   = ((m % 12) + 12) % 12;
    return std::string(names[n]) + std::to_string(oct);
}

// ── Audio ring buffer (written by audio thread, snapshotted by HTTP thread) ───
static const int AUDIO_BUF = 4096;  // power of 2, ~93ms @ 44100 Hz
static const int LOUDNESS_BLOCKS = 36000; // one hour of 100 ms EBU-R128 blocks

struct AudioRingBuf {
    // Individual atomic samples keep the audio thread lock-free while making a
    // best-effort HTTP snapshot defined by the C++ memory model. A snapshot can
    // span a write boundary, but never reads a concurrently-written float.
    std::atomic<float> data[AUDIO_BUF];
    std::atomic<int> head{0};         // total samples written; pos = head & (AUDIO_BUF-1)

    AudioRingBuf() {
        for (int i = 0; i < AUDIO_BUF; i++) data[i].store(0.f, std::memory_order_relaxed);
    }
};

// ── Oscilloscope voltage tracking ─────────────────────────────────────────────
static const int HISTORY_LEN       = 16;
static const int VOLT_SAMPLE_EVERY = 6;

struct PortVolts {
    float peak    = 0.f;
    float history[HISTORY_LEN] = {};
    int   histIdx = 0;
};
struct ModuleVolts {
    std::vector<PortVolts> inputs;
    std::vector<PortVolts> outputs;
};

static NVGcolor parseColorString(const std::string& str) {
    if (str.empty()) {
        return nvgRGB(255, 255, 255); // Default white
    }
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    if (s == "white") return nvgRGB(255, 255, 255);
    if (s == "black") return nvgRGB(30, 30, 30);
    if (s == "red") return nvgRGB(255, 50, 50);
    if (s == "green") return nvgRGB(50, 230, 80);
    if (s == "blue") return nvgRGB(50, 140, 255);
    if (s == "yellow") return nvgRGB(255, 230, 50);
    if (s == "orange") return nvgRGB(255, 140, 30);
    if (s == "purple" || s == "violet") return nvgRGB(180, 60, 255);
    if (s == "cyan" || s == "teal") return nvgRGB(40, 220, 240);
    if (s == "magenta" || s == "pink") return nvgRGB(255, 80, 200);
    if (s == "gray" || s == "grey") return nvgRGB(160, 160, 160);

    // Hex parsing #RRGGBB or RRGGBB or #RGB
    if (s[0] == '#') s = s.substr(1);
    if (s.size() == 6) {
        try {
            int r = std::stoi(s.substr(0, 2), nullptr, 16);
            int g = std::stoi(s.substr(2, 2), nullptr, 16);
            int b = std::stoi(s.substr(4, 2), nullptr, 16);
            return nvgRGB(r, g, b);
        } catch (...) {}
    } else if (s.size() == 3) {
        try {
            int r = std::stoi(std::string(2, s[0]), nullptr, 16);
            int g = std::stoi(std::string(2, s[1]), nullptr, 16);
            int b = std::stoi(std::string(2, s[2]), nullptr, 16);
            return nvgRGB(r, g, b);
        } catch (...) {}
    }
    return nvgRGB(255, 255, 255);
}

// ── Job types ─────────────────────────────────────────────────────────────────
struct SetParamJob {
    int64_t moduleId; int paramId; float value;
    std::atomic<bool> done{false}; bool success = false;
};
struct CableJob {
    enum Type { ADD, REMOVE, REMOVE_OUTPUT };
    Type type;
    int64_t outModId=-1, outPortId=-1, inModId=-1, inPortId=-1;
    std::string color;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct AddModuleJob {
    std::string pluginSlug, modelSlug;
    std::atomic<bool> done{false}; bool success=false; int64_t newId=-1; std::string error;
};
struct DeleteModuleJob {
    int64_t moduleId;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct MoveModuleJob {
    int64_t moduleId; float hp; int row = 0; bool hasRow = false;
    float resolvedHp = 0.f; int resolvedRow = 0;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct BulkMoveJob {
    struct Change { int64_t moduleId; float hp; int row; };
    struct Result { int64_t moduleId; float hp; int row; };
    std::vector<Change> changes;
    std::vector<Result> results;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct BypassJob {
    int64_t moduleId; bool bypassed;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct ModuleStateJob {
    enum Type { GET, SET };
    Type type; int64_t moduleId;
    std::string stateJson; // output for GET, input for SET
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct SibylJob {
    SibylControl::Operation operation;
    int64_t moduleId = -1;
    std::string requestJson = "{}";
    std::string responseJson;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct PatchSaveJob {
    std::string savedPath;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct TemporalDeckJob {
    enum Type { LOAD, PLAY, STOP_REWIND, SEEK, SET_LOOP, STATUS } type;
    int64_t moduleId = -1; std::string path; float position = 0.f; bool enabled = false;
    bool loaded = false, playing = false, loop = false;
    std::atomic<bool> done{false}; bool success=false; std::string error;
};
struct BulkParamJob {
    struct Change { int64_t moduleId; int paramId; float value; };
    std::vector<Change> changes;
    std::vector<int> failed;              // indices that could not be applied
    std::atomic<bool> done{false}; bool success=false;
};
struct UndoJob {
    std::atomic<bool> done{false}; bool success=false;
    std::string label; std::string error;
};

// One reversible action. All fields UI-thread-owned; stack guarded by undoMtx
// only for push/pop/size (HTTP thread reads labels).
struct UndoAction {
    enum Type { PARAM, PARAMS, CABLE_RECONNECT, CABLE_CLEAR, BYPASS, STATE, MOVE, MOVES, DELETE_ADDED } type;
    int64_t moduleId=-1; int paramId=-1; float oldValue=0.f;
    std::vector<std::array<float,3>> paramOlds;           // PARAMS: not used (see paramList)
    struct POld { int64_t m; int p; float v; };
    std::vector<POld> paramList;                          // PARAMS
    int64_t inModId=-1; int inPortId=-1;                  // CABLE_CLEAR target
    struct CableRestore {
        int64_t outModId, outPortId, inModId, inPortId;
        NVGcolor color;
        CableRestore(int64_t om, int64_t op, int64_t im, int64_t ip, NVGcolor c)
            : outModId(om), outPortId(op), inModId(im), inPortId(ip), color(c) {}
    };
    std::vector<CableRestore> cables; // CABLE_RECONNECT: endpoints and original colour
    bool oldBypassed=false;
    std::string oldState;
    float oldX=0.f, oldY=0.f;
    struct MoveOld { int64_t moduleId; float x; float y; };
    std::vector<MoveOld> moveOlds;
    std::string label;
};
struct ParamEntry {
    float value=0.f, minV=0.f, maxV=1.f, defaultV=0.f;
    std::string name, unit;
};
struct ModuleEntry {
    int64_t id; std::string plugin, model;
    std::vector<ParamEntry> params;
    float posX=0.f, posY=0.f; int row=0;
};

// ── Octavia Module ─────────────────────────────────────────────
struct Octavia : Module {
    enum ParamId  { START_PARAM, PARAMS_LEN };
    enum InputId  { AUDIO_IN_L, AUDIO_IN_R, INPUTS_LEN };
    enum OutputId { OUTPUTS_LEN };
    enum LightId  { STATUS_R_LIGHT, STATUS_G_LIGHT, STATUS_B_LIGHT, LIGHTS_LEN };

    std::atomic<bool> serverRunning{false};
    httplib::Server   svr;
    std::thread       serverThread;

    // Audio analysis — written in audio thread, read by HTTP thread
    AudioRingBuf audioRing[2];    // 0 = L input, 1 = R input
    std::atomic<float> sampleRate{44100.f};  // set in process(), read by HTTP
    std::atomic<bool> audioInputConnected[2];

    // Feedback needs a trend across independent requests. Keeping only the
    // strongest candidate per channel gives useful confirmation without
    // retaining spectra or flooding MCP responses.
    struct FeedbackTrack {
        float hz = 0.f, db = -140.f;
        int risingObservations = 0;
        std::chrono::steady_clock::time_point seen;
    } feedbackTrack[2];
    std::mutex feedbackMtx;

    // ── Loudness / stereo meter ────────────────────────────────────────────────
    // Audio thread accumulates into working sums (thread-local to process()),
    // merges into published sums every 2048 samples via try_lock (never blocks).
    // HTTP thread reads published sums under the mutex. Samples normalized to
    // 5V = 0 dBFS, matching Rack's conventional audio signal range.
    // K-weighting: ITU BS.1770 biquads, recalculated for Rack's sample rate
    // from the standard's analog prototypes.
    struct LoudnessMeter {
        struct Coeffs { double b0=1., b1=0., b2=0., a1=0., a2=0.; } shelf, highPass;
        // audio-thread working state
        double z[2][4] = {};                 // biquad states per ch: hs1,hs2,hp1,hp2
        double kSum[2] = {}, rawSum[2] = {}, sumLR = 0.0, peak[2] = {};
        double meterBlockPeak[2] = {};
        uint64_t clipped[2] = {};
        uint64_t n = 0;
        int flushTimer = 0;
        double blockKSum[2] = {};
        int blockFrames = 0, blockTarget = 4800;
        float filterSampleRate = 0.f;
        std::array<std::array<std::atomic<float>, LOUDNESS_BLOCKS>, 2> blocks;
        std::atomic<uint64_t> blockTotal{0};
        std::atomic<float> meterPeak[2];
        // published (mtx-protected)
        double pKSum[2] = {}, pRawSum[2] = {}, pSumLR = 0.0, pPeak[2] = {};
        uint64_t pClipped[2] = {};
        uint64_t pN = 0;
        std::mutex mtx;
        std::atomic<bool> resetFlag{false};

        LoudnessMeter() {
            for (int j = 0; j < 2; ++j) {
                meterPeak[j].store(0.f, std::memory_order_relaxed);
                for (int i = 0; i < LOUDNESS_BLOCKS; ++i)
                    blocks[j][i].store(0.f, std::memory_order_relaxed);
            }
        }
    } lm;

    static LoudnessMeter::Coeffs makeHighPass(float fs, float hz, float q) {
        const double k = tan(M_PI * hz / fs);
        const double a0 = 1.0 + k / q + k * k;
        LoudnessMeter::Coeffs f;
        f.b0 = 1.0 / a0; f.b1 = -2.0 / a0; f.b2 = f.b0;
        f.a1 = 2.0 * (k * k - 1.0) / a0;
        f.a2 = (1.0 - k / q + k * k) / a0;
        return f;
    }

    static LoudnessMeter::Coeffs makeHighShelf(float fs, float hz, float gainDb) {
        constexpr double kShelfQ = 0.7071752369554196;
        constexpr double kVbExponent = 0.4996667741545416;
        const double k = tan(M_PI * hz / fs);
        const double vh = pow(10.0, gainDb / 20.0);
        const double vb = pow(vh, kVbExponent);
        const double a0 = 1.0 + k / kShelfQ + k * k;
        LoudnessMeter::Coeffs f;
        f.b0 = (vh + vb * k / kShelfQ + k * k) / a0;
        f.b1 = 2.0 * (k * k - vh) / a0;
        f.b2 = (vh - vb * k / kShelfQ + k * k) / a0;
        f.a1 = 2.0 * (k * k - 1.0) / a0;
        f.a2 = (1.0 - k / kShelfQ + k * k) / a0;
        return f;
    }

    // Patch cache — UI thread writes, HTTP thread reads
    std::vector<ModuleEntry> cache;
    std::string              moduleListJson    = "[]";
    std::string              moduleSummaryJson = "[]";
    std::string              cableListJson  = "[]";
    std::string              graphJson      = "[]";
    std::string              scopeJson      = "{}";
    std::string              patchInfoJson  = "{\"path\":\"\",\"hasSavePath\":false}";
    std::string              voltagesJson   = "[]";
    std::mutex               cacheMtx;

    // Job queues
    std::queue<std::shared_ptr<SetParamJob>>  setQueue;   std::mutex setQueueMtx;
    std::queue<std::shared_ptr<CableJob>>     cableQueue; std::mutex cableQueueMtx;
    std::queue<std::shared_ptr<AddModuleJob>> addQueue;   std::mutex addQueueMtx;

    // Voltage scope — UI thread only
    std::unordered_map<int64_t, ModuleVolts> voltTrack;
    int voltSampleTimer = 0;

    // Cached perf data — written by updateCache() (UI thread), read via cacheMtx
    double cachedBlockDurationMs = 0.0;
    int    cachedBlockFrames = 0;
    int    cachedModuleCount = 0;
    int    cachedCableCount  = 0;

    // Undo (stack owned by UI thread; mutex only guards container ops)
    std::vector<UndoAction> undoStack; std::mutex undoMtx;
    static const size_t UNDO_MAX = 256;
    bool applyingUndo = false;   // suppress recording while undoing
    std::queue<std::shared_ptr<UndoJob>>      undoQueue;      std::mutex undoQueueMtx;
    std::queue<std::shared_ptr<BulkParamJob>> bulkParamQueue; std::mutex bulkParamQueueMtx;

    // Delete / Move job queues
    std::queue<std::shared_ptr<DeleteModuleJob>> deleteQueue; std::mutex deleteQueueMtx;
    std::queue<std::shared_ptr<MoveModuleJob>>   moveQueue;   std::mutex moveQueueMtx;
    std::queue<std::shared_ptr<BulkMoveJob>> bulkMoveQueue; std::mutex bulkMoveQueueMtx;
    std::queue<std::shared_ptr<BypassJob>>      bypassQueue;    std::mutex bypassQueueMtx;
    std::queue<std::shared_ptr<ModuleStateJob>> stateQueue;     std::mutex stateQueueMtx;
    std::queue<std::shared_ptr<PatchSaveJob>>   patchSaveQueue; std::mutex patchSaveQueueMtx;
    std::queue<std::shared_ptr<TemporalDeckJob>> temporalDeckQueue; std::mutex temporalDeckQueueMtx;
    std::queue<std::shared_ptr<SibylJob>> sibylQueue; std::mutex sibylQueueMtx;

    dsp::BooleanTrigger startTrig;

    Octavia() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configButton(START_PARAM, "Start Octavia Server");
        configInput(AUDIO_IN_L, "Audio Analyze L");
        configInput(AUDIO_IN_R, "Audio Analyze R");
        setupRoutes();
    }
    ~Octavia() { stopServer(); }

    // ── Audio thread: sample inputs into ring buffers ─────────────────────────
    void process(const ProcessArgs& args) override {
        if (startTrig.process(params[START_PARAM].getValue() > 0.f)) startServer();

        sampleRate.store(args.sampleRate, std::memory_order_relaxed);

        // Sample audio inputs into ring buffers. Atomic writes are deliberately
        // used here because HTTP handlers snapshot this data concurrently.
        float ch[2];
        for (int j = 0; j < 2; j++) {
            audioInputConnected[j].store(inputs[j].isConnected(), std::memory_order_relaxed);
            float v = inputs[j].getVoltage();
            ch[j] = v;
            int h = audioRing[j].head.load(std::memory_order_relaxed);
            audioRing[j].data[h & (AUDIO_BUF - 1)].store(v, std::memory_order_relaxed);
            audioRing[j].head.store(h + 1, std::memory_order_release);
        }

        // Loudness/stereo meter accumulation (normalized: 5V = 0 dBFS)
        if (lm.resetFlag.load(std::memory_order_relaxed)) {
            if (lm.mtx.try_lock()) {
                for (int j = 0; j < 2; j++) {
                    lm.kSum[j] = lm.rawSum[j] = lm.peak[j] = 0.0; lm.clipped[j] = 0;
                    lm.blockKSum[j] = lm.meterBlockPeak[j] = 0.0;
                    lm.z[j][0] = lm.z[j][1] = lm.z[j][2] = lm.z[j][3] = 0.0;
                    lm.pKSum[j] = lm.pRawSum[j] = lm.pPeak[j] = 0.0; lm.pClipped[j] = 0;
                    lm.meterPeak[j].store(0.f, std::memory_order_relaxed);
                }
                lm.sumLR = lm.pSumLR = 0.0; lm.n = lm.pN = 0;
                lm.blockFrames = lm.flushTimer = 0; lm.blockTotal.store(0, std::memory_order_relaxed);
                lm.resetFlag.store(false, std::memory_order_relaxed);
                lm.mtx.unlock();
            }
        } else {
            // BS.1770 K-weighting stages, redesigned whenever Rack's sample
            // rate changes. The 100 ms block history below powers R128 gating.
            if (lm.filterSampleRate != args.sampleRate) {
                lm.shelf = makeHighShelf(args.sampleRate, 1681.974450955533f, 3.999843853973347f);
                lm.highPass = makeHighPass(args.sampleRate, 38.13547087602444f, .5003270373238773f);
                lm.filterSampleRate = args.sampleRate;
                lm.blockTarget = std::max(1, (int)roundf(args.sampleRate * .1f));
                memset(lm.z, 0, sizeof(lm.z));
            }
            double x[2];
            for (int j = 0; j < 2; j++) {
                double in = ch[j] * 0.2;                     // 5V → 1.0 full scale
                double a  = std::fabs(in);
                if (a > lm.peak[j]) lm.peak[j] = a;
                if (a > lm.meterBlockPeak[j]) lm.meterBlockPeak[j] = a;
                if (a >= 1.0) lm.clipped[j]++;
                lm.rawSum[j] += in * in;
                // shelving stage (transposed direct form II)
                double y1 = lm.shelf.b0*in + lm.z[j][0];
                lm.z[j][0] = lm.shelf.b1*in - lm.shelf.a1*y1 + lm.z[j][1];
                lm.z[j][1] = lm.shelf.b2*in - lm.shelf.a2*y1;
                // high-pass stage
                double y2 = lm.highPass.b0*y1 + lm.z[j][2];
                lm.z[j][2] = lm.highPass.b1*y1 - lm.highPass.a1*y2 + lm.z[j][3];
                lm.z[j][3] = lm.highPass.b2*y1 - lm.highPass.a2*y2;
                lm.kSum[j] += y2 * y2;
                lm.blockKSum[j] += y2 * y2;
                x[j] = in;
            }
            lm.sumLR += x[0] * x[1];
            lm.n++;
            if (++lm.blockFrames >= lm.blockTarget) {
                uint64_t block = lm.blockTotal.load(std::memory_order_relaxed);
                for (int j = 0; j < 2; ++j) {
                    lm.blocks[j][block % LOUDNESS_BLOCKS].store(
                        (float)(lm.blockKSum[j] / lm.blockFrames), std::memory_order_relaxed);
                    lm.meterPeak[j].store((float)lm.meterBlockPeak[j], std::memory_order_relaxed);
                    lm.blockKSum[j] = 0.0;
                    lm.meterBlockPeak[j] = 0.0;
                }
                lm.blockTotal.store(block + 1, std::memory_order_release);
                lm.blockFrames = 0;
            }
            if (++lm.flushTimer >= 2048 && lm.mtx.try_lock()) {
                for (int j = 0; j < 2; j++) {
                    lm.pKSum[j] += lm.kSum[j]; lm.kSum[j] = 0.0;
                    lm.pRawSum[j] += lm.rawSum[j]; lm.rawSum[j] = 0.0;
                    lm.pClipped[j] += lm.clipped[j]; lm.clipped[j] = 0;
                    if (lm.peak[j] > lm.pPeak[j]) lm.pPeak[j] = lm.peak[j];
                    lm.peak[j] = 0.0;
                }
                lm.pSumLR += lm.sumLR; lm.sumLR = 0.0;
                lm.pN += lm.n; lm.n = 0;
                lm.flushTimer = 0;
                lm.mtx.unlock();
            }
        }

        bool on = serverRunning;
        lights[STATUS_R_LIGHT].setBrightness(on ? 0.f  : 0.8f);
        lights[STATUS_G_LIGHT].setBrightness(on ? 0.8f : 0.f);
        lights[STATUS_B_LIGHT].setBrightness(0.f);
    }

    // ── UI-thread: sample voltages ─────────────────────────────────────────────
    void updateVoltages() {
        bool doHistory = (++voltSampleTimer >= VOLT_SAMPLE_EVERY);
        if (doHistory) voltSampleTimer = 0;
        for (int64_t id : APP->engine->getModuleIds()) {
            engine::Module* m = APP->engine->getModule(id);
            if (!m) continue;
            ModuleVolts& mv = voltTrack[id];
            if (mv.inputs.size()  != m->inputs.size())  mv.inputs.resize(m->inputs.size());
            if (mv.outputs.size() != m->outputs.size()) mv.outputs.resize(m->outputs.size());
            for (size_t j = 0; j < m->inputs.size(); j++) {
                float v = m->inputs[j].getVoltage(), av = std::fabs(v);
                if (av > mv.inputs[j].peak) mv.inputs[j].peak = av;
                if (doHistory) { mv.inputs[j].history[mv.inputs[j].histIdx % HISTORY_LEN] = v; mv.inputs[j].histIdx++; }
            }
            for (size_t j = 0; j < m->outputs.size(); j++) {
                float v = m->outputs[j].getVoltage(), av = std::fabs(v);
                if (av > mv.outputs[j].peak) mv.outputs[j].peak = av;
                if (doHistory) { mv.outputs[j].history[mv.outputs[j].histIdx % HISTORY_LEN] = v; mv.outputs[j].histIdx++; }
            }
        }
    }

    // ── UI-thread: build cache JSON ────────────────────────────────────────────
    void updateCache() {
        std::vector<ModuleEntry> newCache;
        std::string json = "[\n";
        std::string summary = "[\n";
        std::string vjson = "[\n";
        std::vector<int64_t> ids = APP->engine->getModuleIds();
        cachedModuleCount = (int)ids.size();
        cachedBlockFrames = APP->engine->getBlockFrames();
        cachedBlockDurationMs = APP->engine->getBlockDuration() * 1000.0;

        for (size_t i = 0; i < ids.size(); i++) {
            engine::Module* m = APP->engine->getModule(ids[i]);
            if (!m) continue;
            ModuleEntry e;
            e.id = ids[i];
            e.plugin = (m->model && m->model->plugin) ? m->model->plugin->slug : "unknown";
            e.model  = m->model ? m->model->slug : "unknown";
            ModuleWidget* mw_pos = APP->scene->rack->getModule(ids[i]);
            if (mw_pos) {
                e.posX = mw_pos->box.pos.x / RACK_GRID_WIDTH;
                e.posY = mw_pos->box.pos.y / RACK_GRID_WIDTH;
                e.row = (int)std::lround(mw_pos->box.pos.y / RACK_GRID_HEIGHT);
            }
            for (size_t pi = 0; pi < m->params.size(); pi++) {
                ParamEntry pe;
                pe.value = m->params[pi].getValue();
                if (pi < m->paramQuantities.size() && m->paramQuantities[pi]) {
                    ParamQuantity* pq = m->paramQuantities[pi];
                    pe.name = pq->name; pe.unit = pq->unit;
                    pe.minV = pq->getMinValue(); pe.maxV = pq->getMaxValue();
                    pe.defaultV = pq->getDefaultValue();
                }
                e.params.push_back(pe);
            }
            newCache.push_back(e);

            ModuleVolts* mv = nullptr;
            auto it = voltTrack.find(ids[i]);
            if (it != voltTrack.end()) mv = &it->second;

            json += "  {";
            json += jStr("id")     + ": " + std::to_string(ids[i]) + ", ";
            json += jStr("plugin") + ": " + jStr(e.plugin) + ", ";
            json += jStr("model")  + ": " + jStr(e.model) + ", ";
            json += jStr("bypassed") + ": " + (m->isBypassed() ? "true" : "false") + ", ";
            json += jStr("posX") + ": " + std::to_string(e.posX) + ", ";
            json += jStr("posY") + ": " + std::to_string(e.posY) + ", ";
            json += jStr("row") + ": " + std::to_string(e.row) + ", ";

            json += jStr("params") + ": [";
            for (size_t j = 0; j < m->params.size(); j++) {
                std::string nm, unit; float minV=0.f, maxV=1.f, defV=0.f;
                if (j < m->paramQuantities.size() && m->paramQuantities[j]) {
                    ParamQuantity* pq = m->paramQuantities[j];
                    nm=pq->name; unit=pq->unit;
                    minV=pq->getMinValue(); maxV=pq->getMaxValue(); defV=pq->getDefaultValue();
                }
                json += "{" + jStr("id") + ": " + std::to_string(j) + ", "
                            + jStr("name") + ": " + jStr(nm) + ", "
                            + jStr("value") + ": " + jNum(m->params[j].getValue()) + ", "
                            + jStr("min") + ": " + jNum(minV) + ", "
                            + jStr("max") + ": " + jNum(maxV) + ", "
                            + jStr("default") + ": " + jNum(defV) + ", "
                            + jStr("unit") + ": " + jStr(unit) + "}";
                if (j + 1 < m->params.size()) json += ", ";
            }
            json += "], ";

            json += jStr("inputs") + ": [";
            for (size_t j = 0; j < m->inputs.size(); j++) {
                std::string nm;
                if (j < m->inputInfos.size() && m->inputInfos[j]) nm = m->inputInfos[j]->name;
                float peak = (mv && j < mv->inputs.size()) ? mv->inputs[j].peak : 0.f;
                int ichs = m->inputs[j].getChannels();
                std::string ivArr = "[";
                for (int ch = 0; ch < ichs; ch++) { if (ch>0) ivArr+=","; ivArr+=jNum(m->inputs[j].getVoltage(ch)); }
                ivArr += "]";
                json += "{" + jStr("id") + ": " + std::to_string(j) + ", "
                            + jStr("name") + ": " + jStr(nm) + ", "
                            + jStr("channels") + ": " + std::to_string(ichs) + ", "
                            + jStr("voltage") + ": " + jNum(m->inputs[j].getVoltage()) + ", "
                            + jStr("voltages") + ": " + ivArr + ", "
                            + jStr("peak") + ": " + jNum(peak) + ", "
                            + jStr("connected") + ": " + (m->inputs[j].isConnected() ? "true" : "false") + "}";
                if (j + 1 < m->inputs.size()) json += ", ";
            }
            json += "], ";

            json += jStr("outputs") + ": [";
            for (size_t j = 0; j < m->outputs.size(); j++) {
                std::string nm;
                if (j < m->outputInfos.size() && m->outputInfos[j]) nm = m->outputInfos[j]->name;
                float peak = (mv && j < mv->outputs.size()) ? mv->outputs[j].peak : 0.f;
                int ochs = m->outputs[j].getChannels();
                std::string ovArr = "[";
                for (int ch = 0; ch < ochs; ch++) { if (ch>0) ovArr+=","; ovArr+=jNum(m->outputs[j].getVoltage(ch)); }
                ovArr += "]";
                json += "{" + jStr("id") + ": " + std::to_string(j) + ", "
                            + jStr("name") + ": " + jStr(nm) + ", "
                            + jStr("channels") + ": " + std::to_string(ochs) + ", "
                            + jStr("voltage") + ": " + jNum(m->outputs[j].getVoltage()) + ", "
                            + jStr("voltages") + ": " + ovArr + ", "
                            + jStr("peak") + ": " + jNum(peak) + ", "
                            + jStr("connected") + ": " + (m->outputs[j].isConnected() ? "true" : "false") + "}";
                if (j + 1 < m->outputs.size()) json += ", ";
            }
            json += "]";
            json += "}";
            if (i + 1 < ids.size()) json += ",";
            json += "\n";

            // max output polyphony for summary
            int maxOutCh = 0;
            for (size_t j = 0; j < m->outputs.size(); j++) {
                int ch = m->outputs[j].getChannels();
                if (ch > maxOutCh) maxOutCh = ch;
            }
            summary += "  {" + jStr("id") + ": " + std::to_string(ids[i]) + ", "
                     + jStr("plugin") + ": " + jStr(e.plugin) + ", "
                     + jStr("model") + ": " + jStr(e.model) + ", "
                     + jStr("bypassed") + ": " + (m->isBypassed() ? "true" : "false") + ", "
                     + jStr("posX") + ": " + std::to_string(e.posX) + ", "
                     + jStr("posY") + ": " + std::to_string(e.posY) + ", "
                     + jStr("row") + ": " + std::to_string(e.row) + ", "
                     + jStr("polyOut") + ": " + std::to_string(maxOutCh) + ", "
                     + jStr("paramCount") + ": " + std::to_string(m->params.size()) + ", "
                     + jStr("inputCount") + ": " + std::to_string(m->inputs.size()) + ", "
                     + jStr("outputCount") + ": " + std::to_string(m->outputs.size()) + "}";
            if (i + 1 < ids.size()) summary += ",";
            summary += "\n";

            // compact voltage snapshot
            vjson += "  {" + jStr("id") + ": " + std::to_string(ids[i]) + ", "
                   + jStr("model") + ": " + jStr(e.model) + ", "
                   + jStr("outputs") + ": [";
            for (size_t j = 0; j < m->outputs.size(); j++) {
                std::string nm;
                if (j < m->outputInfos.size() && m->outputInfos[j]) nm = m->outputInfos[j]->name;
                float peak = (mv && j < mv->outputs.size()) ? mv->outputs[j].peak : 0.f;
                int ochs = m->outputs[j].getChannels();
                vjson += "{" + jStr("id") + ": " + std::to_string(j) + ", "
                       + jStr("name") + ": " + jStr(nm) + ", "
                       + jStr("ch") + ": " + std::to_string(ochs) + ", "
                       + jStr("v") + ": " + jNum(m->outputs[j].getVoltage()) + ", "
                       + jStr("peak") + ": " + jNum(peak) + ", "
                       + jStr("connected") + ": " + (m->outputs[j].isConnected() ? "true" : "false") + "}";
                if (j + 1 < m->outputs.size()) vjson += ", ";
            }
            vjson += "]}";
            if (i + 1 < ids.size()) vjson += ",";
            vjson += "\n";
        }
        json += "]";
        summary += "]";
        vjson += "]";

        // Scope JSON
        std::string sjson = "{";
        bool sfirst = true;
        for (int64_t id : ids) {
            engine::Module* m = APP->engine->getModule(id);
            if (!m) continue;
            auto it = voltTrack.find(id);
            if (it == voltTrack.end()) continue;
            ModuleVolts& mv = it->second;
            if (!sfirst) sjson += ", ";
            sfirst = false;
            sjson += jStr(std::to_string(id)) + ": {";
            sjson += jStr("model") + ": " + jStr(m->model ? m->model->slug : "?") + ", ";

            sjson += jStr("inputs") + ": [";
            for (size_t j = 0; j < mv.inputs.size() && j < m->inputs.size(); j++) {
                if (j > 0) sjson += ", ";
                std::string nm = (j < m->inputInfos.size() && m->inputInfos[j]) ? m->inputInfos[j]->name : "";
                sjson += "{" + jStr("id") + ": " + std::to_string(j) + ", " + jStr("name") + ": " + jStr(nm) + ", ";
                sjson += jStr("peak") + ": " + jNum(mv.inputs[j].peak) + ", ";
                sjson += jStr("history") + ": [";
                int cnt = std::min(HISTORY_LEN, mv.inputs[j].histIdx);
                for (int k = 0; k < cnt; k++) { if (k>0) sjson+=", "; sjson += jNum(mv.inputs[j].history[(mv.inputs[j].histIdx-cnt+k)%HISTORY_LEN]); }
                sjson += "]}";
            }
            sjson += "], ";

            sjson += jStr("outputs") + ": [";
            for (size_t j = 0; j < mv.outputs.size() && j < m->outputs.size(); j++) {
                if (j > 0) sjson += ", ";
                std::string nm = (j < m->outputInfos.size() && m->outputInfos[j]) ? m->outputInfos[j]->name : "";
                sjson += "{" + jStr("id") + ": " + std::to_string(j) + ", " + jStr("name") + ": " + jStr(nm) + ", ";
                sjson += jStr("peak") + ": " + jNum(mv.outputs[j].peak) + ", ";
                sjson += jStr("history") + ": [";
                int cnt = std::min(HISTORY_LEN, mv.outputs[j].histIdx);
                for (int k = 0; k < cnt; k++) { if (k>0) sjson+=", "; sjson += jNum(mv.outputs[j].history[(mv.outputs[j].histIdx-cnt+k)%HISTORY_LEN]); }
                sjson += "]}";
            }
            sjson += "]}";
        }
        sjson += "}";

        // Reset peaks
        for (auto& kv : voltTrack) {
            for (auto& pv : kv.second.inputs)  pv.peak = 0.f;
            for (auto& pv : kv.second.outputs) pv.peak = 0.f;
        }

        // Cable + graph JSON
        auto modSlug    = [&](int64_t mid) { engine::Module* mm=APP->engine->getModule(mid); return (mm&&mm->model)?mm->model->slug:std::string("?"); };
        auto outPortNm  = [&](int64_t mid, int pid) -> std::string { engine::Module* mm=APP->engine->getModule(mid); if(!mm||pid<0||pid>=(int)mm->outputInfos.size()) return ""; return mm->outputInfos[pid]?mm->outputInfos[pid]->name:""; };
        auto inPortNm   = [&](int64_t mid, int pid) -> std::string { engine::Module* mm=APP->engine->getModule(mid); if(!mm||pid<0||pid>=(int)mm->inputInfos.size()) return ""; return mm->inputInfos[pid]?mm->inputInfos[pid]->name:""; };

        std::string cjson="[", gjson="[";
        bool cfirst=true, gfirst=true;
        auto allCableIds = APP->engine->getCableIds();
        cachedCableCount = (int)allCableIds.size();
        for (int64_t cid : allCableIds) {
            engine::Cable* c = APP->engine->getCable(cid);
            if (!c) continue;
            int64_t omid = c->outputModule ? c->outputModule->id : -1;
            int64_t imid = c->inputModule  ? c->inputModule->id  : -1;
            std::string fromMod=modSlug(omid), fromPort=outPortNm(omid,c->outputId);
            std::string toMod=modSlug(imid),   toPort=inPortNm(imid,c->inputId);
            std::string label = fromMod+":"+fromPort+" → "+toMod+":"+toPort;
            if (!cfirst) cjson += ", ";
            cfirst = false;
            cjson += "{" + jStr("id")+": "+std::to_string(cid)+", "
                         + jStr("outputModuleId")+": "+std::to_string(omid)+", "
                         + jStr("outputModule")+": "+jStr(fromMod)+", "
                         + jStr("outputPortId")+": "+std::to_string(c->outputId)+", "
                         + jStr("outputPortName")+": "+jStr(fromPort)+", "
                         + jStr("inputModuleId")+": "+std::to_string(imid)+", "
                         + jStr("inputModule")+": "+jStr(toMod)+", "
                         + jStr("inputPortId")+": "+std::to_string(c->inputId)+", "
                         + jStr("inputPortName")+": "+jStr(toPort)+", "
                         + jStr("label")+": "+jStr(label)+"}";
            if (!gfirst) gjson += ", ";
            gfirst = false;
            gjson += jStr(label);
        }
        cjson+="]"; gjson+="]";

        std::unique_lock<std::mutex> lock(cacheMtx);
        cache=std::move(newCache); moduleListJson=std::move(json); moduleSummaryJson=std::move(summary);
        cableListJson=std::move(cjson); graphJson=std::move(gjson); scopeJson=std::move(sjson);
        voltagesJson=std::move(vjson);
        // update patch info cache (safe: called from UI thread)
        std::string ppath = APP->patch->path;
        bool hsp = !ppath.empty();
        patchInfoJson = "{" + jStr("path") + ": " + jStr(ppath) + ", "
                      + jStr("hasSavePath") + ": " + (hsp ? "true" : "false") + "}";
    }

    // ── UI-thread: job processors ─────────────────────────────────────────────
    void pushUndo(UndoAction a) {
        if (applyingUndo) return;
        std::unique_lock<std::mutex> lk(undoMtx);
        undoStack.push_back(std::move(a));
        if (undoStack.size() > UNDO_MAX) undoStack.erase(undoStack.begin());
    }

    void processSetQueue() {
        std::shared_ptr<SetParamJob> job;
        { std::unique_lock<std::mutex> lk(setQueueMtx); if (!setQueue.empty()) { job=setQueue.front(); setQueue.pop(); } }
        if (!job) return;
        engine::Module* m = APP->engine->getModule(job->moduleId);
        if (m && job->paramId>=0 && job->paramId<(int)m->params.size()) {
            float v=job->value;
            ParamQuantity* pq=m->getParamQuantity(job->paramId);
            if (pq) v=rack::math::clamp(v,pq->getMinValue(),pq->getMaxValue());
            UndoAction a; a.type=UndoAction::PARAM; a.moduleId=job->moduleId; a.paramId=job->paramId;
            a.oldValue=APP->engine->getParamValue(m,job->paramId);
            a.label="set param "+std::to_string(job->paramId)+" on module "+std::to_string(job->moduleId);
            APP->engine->setParamValue(m,job->paramId,v);
            pushUndo(std::move(a));
            job->success=true;
        }
        job->done=true;
    }

    void processCableQueue() {
        std::shared_ptr<CableJob> job;
        { std::unique_lock<std::mutex> lk(cableQueueMtx); if (!cableQueue.empty()) { job=cableQueue.front(); cableQueue.pop(); } }
        if (!job) return;
        if (job->type==CableJob::ADD) {
            ModuleWidget* outW=APP->scene->rack->getModule(job->outModId);
            ModuleWidget* inW =APP->scene->rack->getModule(job->inModId);
            engine::Module* src=APP->engine->getModule(job->outModId);
            engine::Module* dst=APP->engine->getModule(job->inModId);
            if (outW&&inW&&src&&dst) {
                engine::Cable* c=new engine::Cable;
                c->outputModule=src; c->outputId=(int)job->outPortId;
                c->inputModule=dst;  c->inputId=(int)job->inPortId;
                APP->engine->addCable(c);
                CableWidget* cw=new CableWidget;
                cw->color = parseColorString(job->color);
                cw->setCable(c);
                if (!cw->isComplete()) {
                    APP->engine->removeCable(cw->releaseCable()); delete cw;
                    job->error="cable incomplete: port not found (outPort="+std::to_string(job->outPortId)+" inPort="+std::to_string(job->inPortId)+")";
                } else {
                    APP->scene->rack->addCable(cw); job->success=true;
                    UndoAction a; a.type=UndoAction::CABLE_CLEAR;
                    a.inModId=job->inModId; a.inPortId=(int)job->inPortId;
                    a.label="connect cable -> module "+std::to_string(job->inModId)+" port "+std::to_string(job->inPortId);
                    pushUndo(std::move(a));
                }
            } else { job->error="module not found"; }
        } else if (job->type==CableJob::REMOVE) {
            ModuleWidget* inW=APP->scene->rack->getModule(job->inModId);
            if (inW) {
                PortWidget* inPort=inW->getInput((int)job->inPortId);
                if (inPort) {
                    UndoAction a; a.type=UndoAction::CABLE_RECONNECT;
                    for (int64_t cid : APP->engine->getCableIds()) {
                        engine::Cable* c = APP->engine->getCable(cid);
                        if (c && c->inputModule && c->inputModule->id==job->inModId
                            && c->inputId==(int)job->inPortId && c->outputModule) {
                            CableWidget* cw = APP->scene->rack->getCable(cid);
                            a.cables.emplace_back(c->outputModule->id, (int64_t)c->outputId,
                                                  c->inputModule->id, (int64_t)c->inputId,
                                                  cw ? cw->color : parseColorString(""));
                        }
                    }
                    a.label="disconnect input port "+std::to_string(job->inPortId)+" on module "+std::to_string(job->inModId);
                    APP->scene->rack->clearCablesOnPort(inPort);
                    if (!a.cables.empty()) pushUndo(std::move(a));
                    job->success=true;
                }
                else { job->error="port not found"; }
            } else { job->error="module not found"; }
        } else { // REMOVE_OUTPUT — disconnect all cables from one output port
            // Collect input locations first, then remove (avoid modifying list while iterating)
            std::vector<UndoAction::CableRestore> targets;
            for (int64_t cid : APP->engine->getCableIds()) {
                engine::Cable* c = APP->engine->getCable(cid);
                if (c && c->outputModule && c->outputModule->id==job->outModId
                    && c->outputId==(int)job->outPortId && c->inputModule) {
                    CableWidget* cw = APP->scene->rack->getCable(cid);
                    targets.emplace_back(job->outModId, job->outPortId, c->inputModule->id, c->inputId,
                                         cw ? cw->color : parseColorString(""));
                }
            }
            UndoAction a; a.type=UndoAction::CABLE_RECONNECT;
            for (auto& t : targets)
                a.cables.push_back(t);
            for (auto& t : targets) {
                ModuleWidget* inW = APP->scene->rack->getModule(t.inModId);
                if (!inW) continue;
                PortWidget* inPort = inW->getInput((int)t.inPortId);
                if (inPort) APP->scene->rack->clearCablesOnPort(inPort);
            }
            if (!a.cables.empty()) {
                a.label="disconnect all cables from output "+std::to_string(job->outPortId)+" on module "+std::to_string(job->outModId);
                pushUndo(std::move(a));
            }
            job->success=true;
        }
        job->done=true;
    }

    void processTemporalDeckQueue() {
        std::shared_ptr<TemporalDeckJob> job;
        { std::unique_lock<std::mutex> lk(temporalDeckQueueMtx); if (!temporalDeckQueue.empty()) { job=temporalDeckQueue.front(); temporalDeckQueue.pop(); } }
        if (!job) return;
        TemporalDeck* deck = dynamic_cast<TemporalDeck*>(APP->engine->getModule(job->moduleId));
        if (!deck) job->error = "Temporal Deck module not found";
        else {
            switch (job->type) {
            case TemporalDeckJob::LOAD: {
                if (!system::exists(job->path)) job->error = "sample file not found";
                else {
                    std::string error;
                    job->success = deck->loadSampleFromPath(job->path, &error);
                    deck->setSampleModeEnabled(job->success);
                    deck->setSampleLoopEnabled(false);
                    if (!job->success) job->error = error.empty() ? "sample load failed" : error;
                }
                break;
            }
            case TemporalDeckJob::PLAY: deck->setSampleTransportPlaying(true); job->success=true; break;
            case TemporalDeckJob::STOP_REWIND: deck->stopSampleTransport(); job->success=true; break;
            case TemporalDeckJob::SEEK: deck->seekSampleByNormalizedPosition(job->position); job->success=true; break;
            case TemporalDeckJob::SET_LOOP: deck->setSampleLoopEnabled(job->enabled); job->success=true; break;
            case TemporalDeckJob::STATUS: job->success=true; break;
            }
            job->loaded=deck->hasLoadedSample(); job->playing=deck->isSampleTransportPlaying(); job->loop=deck->isSampleLoopEnabled();
        }
        job->done=true;
    }

    void processAddQueue() {
        std::shared_ptr<AddModuleJob> job;
        { std::unique_lock<std::mutex> lk(addQueueMtx); if (!addQueue.empty()) { job=addQueue.front(); addQueue.pop(); } }
        if (!job) return;
        plugin::Model* model=plugin::getModel(job->pluginSlug,job->modelSlug);
        if (!model) { job->error="model not found: "+job->pluginSlug+"/"+job->modelSlug; }
        else {
            try {
                engine::Module* m=model->createModule(); APP->engine->addModule(m);
                ModuleWidget* mw=model->createModuleWidget(m); APP->scene->rack->addModule(mw);
                float sumX=0.f,sumY=0.f; int cnt=0;
                for (int64_t oid : APP->engine->getModuleIds()) {
                    if (oid==m->id) continue;
                    ModuleWidget* omw=APP->scene->rack->getModule(oid);
                    if (omw) { sumX+=omw->box.pos.x+omw->box.size.x*0.5f; sumY+=omw->box.pos.y+omw->box.size.y*0.5f; cnt++; }
                }
                math::Vec target=cnt>0?math::Vec(sumX/cnt,sumY/cnt):math::Vec(0.f,0.f);
                APP->scene->rack->setModulePosNearest(mw,target);
                job->newId=m->id; job->success=true;
                UndoAction a; a.type=UndoAction::DELETE_ADDED; a.moduleId=m->id;
                a.label="add module "+job->pluginSlug+"/"+job->modelSlug;
                pushUndo(std::move(a));
            } catch (std::exception& e) { job->error=e.what(); }
        }
        job->done=true;
    }

    void processDeleteQueue() {
        std::shared_ptr<DeleteModuleJob> job;
        { std::unique_lock<std::mutex> lk(deleteQueueMtx); if (!deleteQueue.empty()) { job=deleteQueue.front(); deleteQueue.pop(); } }
        if (!job) return;
        ModuleWidget* mw = APP->scene->rack->getModule(job->moduleId);
        if (!mw) { job->error="module not found"; job->done=true; return; }
        engine::Module* m = mw->module;
        // removeModule: disconnects cables + removes mw from scene (no engine removal)
        APP->scene->rack->removeModule(mw);
        // Remove module from engine
        APP->engine->removeModule(m);
        // Memory intentionally NOT freed here (leak test — to isolate crash source)
        job->success=true; job->done=true;
    }

    void processBulkParamQueue() {
        std::shared_ptr<BulkParamJob> job;
        { std::unique_lock<std::mutex> lk(bulkParamQueueMtx); if (!bulkParamQueue.empty()) { job=bulkParamQueue.front(); bulkParamQueue.pop(); } }
        if (!job) return;
        UndoAction a; a.type=UndoAction::PARAMS;
        for (size_t i = 0; i < job->changes.size(); i++) {
            auto& ch = job->changes[i];
            engine::Module* m = APP->engine->getModule(ch.moduleId);
            if (!m || ch.paramId < 0 || ch.paramId >= (int)m->params.size()) {
                job->failed.push_back((int)i); continue;
            }
            float v = ch.value;
            ParamQuantity* pq = m->getParamQuantity(ch.paramId);
            if (pq) v = rack::math::clamp(v, pq->getMinValue(), pq->getMaxValue());
            a.paramList.push_back({ch.moduleId, ch.paramId, APP->engine->getParamValue(m, ch.paramId)});
            APP->engine->setParamValue(m, ch.paramId, v);
        }
        if (!a.paramList.empty()) {
            a.label="bulk set "+std::to_string(a.paramList.size())+" parameters";
            pushUndo(std::move(a));
        }
        job->success=true; job->done=true;
    }

    void processUndoQueue() {
        std::shared_ptr<UndoJob> job;
        { std::unique_lock<std::mutex> lk(undoQueueMtx); if (!undoQueue.empty()) { job=undoQueue.front(); undoQueue.pop(); } }
        if (!job) return;
        UndoAction a;
        { std::unique_lock<std::mutex> lk(undoMtx);
          if (undoStack.empty()) { job->error="nothing to undo"; job->done=true; return; }
          a = std::move(undoStack.back()); undoStack.pop_back(); }
        applyingUndo = true;
        switch (a.type) {
            case UndoAction::PARAM: {
                engine::Module* m = APP->engine->getModule(a.moduleId);
                if (m) { APP->engine->setParamValue(m, a.paramId, a.oldValue); job->success=true; }
                else job->error="module gone";
                break; }
            case UndoAction::PARAMS: {
                job->success=true;
                for (auto& po : a.paramList) {
                    engine::Module* m = APP->engine->getModule(po.m);
                    if (m) APP->engine->setParamValue(m, po.p, po.v);
                }
                break; }
            case UndoAction::CABLE_CLEAR: {
                ModuleWidget* inW = APP->scene->rack->getModule(a.inModId);
                PortWidget* inPort = inW ? inW->getInput(a.inPortId) : NULL;
                if (inPort) { APP->scene->rack->clearCablesOnPort(inPort); job->success=true; }
                else job->error="port gone";
                break; }
            case UndoAction::CABLE_RECONNECT: {
                job->success=true;
                for (auto& cb : a.cables) {
                    engine::Module* src=APP->engine->getModule(cb.outModId);
                    engine::Module* dst=APP->engine->getModule(cb.inModId);
                    ModuleWidget* outW=APP->scene->rack->getModule(cb.outModId);
                    ModuleWidget* inW =APP->scene->rack->getModule(cb.inModId);
                    if (!src||!dst||!outW||!inW) continue;
                    engine::Cable* c=new engine::Cable;
                    c->outputModule=src; c->outputId=(int)cb.outPortId;
                    c->inputModule=dst;  c->inputId=(int)cb.inPortId;
                    APP->engine->addCable(c);
                    CableWidget* cw=new CableWidget;
                    cw->color = cb.color;
                    cw->setCable(c);
                    if (!cw->isComplete()) { APP->engine->removeCable(cw->releaseCable()); delete cw; }
                    else APP->scene->rack->addCable(cw);
                }
                break; }
            case UndoAction::BYPASS: {
                engine::Module* m = APP->engine->getModule(a.moduleId);
                if (m) { APP->engine->bypassModule(m, a.oldBypassed); job->success=true; }
                else job->error="module gone";
                break; }
            case UndoAction::STATE: {
                engine::Module* m = APP->engine->getModule(a.moduleId);
                if (!m) { job->error="module gone"; break; }
                json_error_t jerr;
                json_t* rootJ = json_loads(a.oldState.c_str(), 0, &jerr);
                if (rootJ) { m->fromJson(rootJ); json_decref(rootJ); job->success=true; }
                else job->error="stored state unparsable";
                break; }
            case UndoAction::MOVE: {
                ModuleWidget* mw = APP->scene->rack->getModule(a.moduleId);
                if (mw) { APP->scene->rack->setModulePosForce(mw, math::Vec(a.oldX, a.oldY)); job->success=true; }
                else job->error="module gone";
                break; }
            case UndoAction::MOVES: {
                job->success=true;
                for (auto& old : a.moveOlds) {
                    ModuleWidget* mw = APP->scene->rack->getModule(old.moduleId);
                    if (mw) APP->scene->rack->setModulePosForce(mw, math::Vec(old.x, old.y));
                    else { job->success=false; job->error="one or more modules are gone"; }
                }
                break; }
            case UndoAction::DELETE_ADDED: {
                ModuleWidget* mw = APP->scene->rack->getModule(a.moduleId);
                if (mw) {
                    engine::Module* m = mw->module;
                    APP->scene->rack->removeModule(mw);
                    APP->engine->removeModule(m);
                    job->success=true;
                } else job->error="module gone";
                break; }
        }
        applyingUndo = false;
        job->label = a.label;
        job->done = true;
    }

    void processMoveQueue() {
        std::shared_ptr<MoveModuleJob> job;
        { std::unique_lock<std::mutex> lk(moveQueueMtx); if (!moveQueue.empty()) { job=moveQueue.front(); moveQueue.pop(); } }
        if (!job) return;
        ModuleWidget* mw = APP->scene->rack->getModule(job->moduleId);
        if (!mw) { job->error="module not found"; job->done=true; return; }
        UndoAction a; a.type=UndoAction::MOVE; a.moduleId=job->moduleId;
        a.oldX=mw->box.pos.x; a.oldY=mw->box.pos.y;
        a.label="move module "+std::to_string(job->moduleId);
        const float y = job->hasRow ? job->row * RACK_GRID_HEIGHT : mw->box.pos.y;
        APP->scene->rack->setModulePosNearest(mw, math::Vec(job->hp * RACK_GRID_WIDTH, y));
        job->resolvedHp = mw->box.pos.x / RACK_GRID_WIDTH;
        job->resolvedRow = (int)std::lround(mw->box.pos.y / RACK_GRID_HEIGHT);
        pushUndo(std::move(a));
        job->success=true; job->done=true;
    }

    void processBulkMoveQueue() {
        std::shared_ptr<BulkMoveJob> job;
        { std::unique_lock<std::mutex> lk(bulkMoveQueueMtx); if (!bulkMoveQueue.empty()) { job=bulkMoveQueue.front(); bulkMoveQueue.pop(); } }
        if (!job) return;

        std::unordered_set<int64_t> moving;
        std::vector<std::pair<ModuleWidget*, math::Vec>> targets;
        for (auto& ch : job->changes) {
            if (!moving.insert(ch.moduleId).second) { job->error="duplicate module id"; job->done=true; return; }
            ModuleWidget* mw = APP->scene->rack->getModule(ch.moduleId);
            if (!mw) { job->error="module not found: "+std::to_string(ch.moduleId); job->done=true; return; }
            targets.push_back({mw, math::Vec(ch.hp * RACK_GRID_WIDTH, ch.row * RACK_GRID_HEIGHT)});
        }
        auto overlaps = [](math::Vec ap, math::Vec as, math::Vec bp, math::Vec bs) {
            return ap.x < bp.x + bs.x && bp.x < ap.x + as.x && ap.y < bp.y + bs.y && bp.y < ap.y + as.y;
        };
        for (size_t i=0; i<targets.size(); i++) {
            for (size_t j=i+1; j<targets.size(); j++) {
                if (overlaps(targets[i].second, targets[i].first->box.size, targets[j].second, targets[j].first->box.size)) {
                    job->error="target modules overlap"; job->done=true; return;
                }
            }
            for (Widget* child : APP->scene->rack->getModuleContainer()->children) {
                ModuleWidget* other = dynamic_cast<ModuleWidget*>(child);
                if (!other || !other->module || moving.count(other->module->id)) continue;
                if (overlaps(targets[i].second, targets[i].first->box.size, other->box.pos, other->box.size)) {
                    job->error="target overlaps unmoved module "+std::to_string(other->module->id); job->done=true; return;
                }
            }
        }
        UndoAction a; a.type=UndoAction::MOVES;
        for (auto& target : targets)
            a.moveOlds.push_back({target.first->module->id, target.first->box.pos.x, target.first->box.pos.y});
        for (size_t i=0; i<targets.size(); i++) {
            APP->scene->rack->setModulePosForce(targets[i].first, targets[i].second);
            job->results.push_back({job->changes[i].moduleId,
                targets[i].first->box.pos.x / RACK_GRID_WIDTH,
                (int)std::lround(targets[i].first->box.pos.y / RACK_GRID_HEIGHT)});
        }
        a.label="layout "+std::to_string(a.moveOlds.size())+" modules";
        pushUndo(std::move(a));
        job->success=true; job->done=true;
    }

    void processBypassQueue() {
        std::shared_ptr<BypassJob> job;
        { std::unique_lock<std::mutex> lk(bypassQueueMtx); if (!bypassQueue.empty()) { job=bypassQueue.front(); bypassQueue.pop(); } }
        if (!job) return;
        engine::Module* m = APP->engine->getModule(job->moduleId);
        if (!m) { job->error="module not found"; job->done=true; return; }
        UndoAction a; a.type=UndoAction::BYPASS; a.moduleId=job->moduleId;
        a.oldBypassed=m->isBypassed();
        a.label=std::string(job->bypassed?"bypass":"un-bypass")+" module "+std::to_string(job->moduleId);
        APP->engine->bypassModule(m, job->bypassed);
        pushUndo(std::move(a));
        job->success=true; job->done=true;
    }

    void processStateQueue() {
        std::shared_ptr<ModuleStateJob> job;
        { std::unique_lock<std::mutex> lk(stateQueueMtx); if (!stateQueue.empty()) { job=stateQueue.front(); stateQueue.pop(); } }
        if (!job) return;
        engine::Module* m = APP->engine->getModule(job->moduleId);
        if (!m) { job->error="module not found"; job->done=true; return; }
        if (job->type == ModuleStateJob::GET) {
            json_t* rootJ = m->toJson();
            if (!rootJ) { job->error="toJson() returned null"; job->done=true; return; }
            char* str = json_dumps(rootJ, JSON_COMPACT);
            job->stateJson = str ? std::string(str) : "{}";
            if (str) free(str);
            json_decref(rootJ);
        } else {
            json_error_t jerr;
            json_t* rootJ = json_loads(job->stateJson.c_str(), 0, &jerr);
            if (!rootJ) { job->error = std::string("JSON parse: ") + jerr.text; job->done=true; return; }
            UndoAction a; a.type=UndoAction::STATE; a.moduleId=job->moduleId;
            { json_t* oldJ = m->toJson();
              if (oldJ) { char* str=json_dumps(oldJ,JSON_COMPACT); if (str){ a.oldState=str; free(str);} json_decref(oldJ); } }
            a.label="restore state on module "+std::to_string(job->moduleId);
            m->fromJson(rootJ);
            if (!a.oldState.empty()) pushUndo(std::move(a));
            json_decref(rootJ);
        }
        job->success=true; job->done=true;
    }

    void processSibylQueue() {
        std::shared_ptr<SibylJob> job;
        { std::unique_lock<std::mutex> lk(sibylQueueMtx); if (!sibylQueue.empty()) { job=sibylQueue.front(); sibylQueue.pop(); } }
        if (!job) return;
        engine::Module* module = APP->engine->getModule(job->moduleId);
        if (!module) { job->error="module not found"; job->done=true; return; }
        SibylControl* sibyl = dynamic_cast<SibylControl*>(module);
        if (!sibyl) { job->error="module does not provide the Sibyl semantic-control capability"; job->done=true; return; }

        std::string oldState;
        if (job->operation == SibylControl::Operation::EDIT) {
            json_t* oldJ = module->toJson();
            if (oldJ) {
                char* str = json_dumps(oldJ, JSON_COMPACT);
                if (str) { oldState = str; free(str); }
                json_decref(oldJ);
            }
        }
        job->success = sibyl->handleSibylRequest(job->operation, job->requestJson,
                                                 job->responseJson, job->error);
        if (!job->responseJson.empty()) {
            json_error_t jerr;
            json_t* responseJ = json_loads(job->responseJson.c_str(), 0, &jerr);
            if (!responseJ || !json_is_object(responseJ)) {
                if (responseJ) json_decref(responseJ);
                job->success=false;
                job->responseJson.clear();
                job->error="Sibyl returned an invalid JSON response object";
            } else json_decref(responseJ);
        } else if (job->success) {
            job->success=false;
            job->error="Sibyl returned an empty response";
        }
        if (job->success && job->operation == SibylControl::Operation::EDIT && !oldState.empty()) {
            UndoAction undo; undo.type=UndoAction::STATE; undo.moduleId=job->moduleId;
            undo.oldState=std::move(oldState);
            undo.label="edit Sibyl composition on module "+std::to_string(job->moduleId);
            pushUndo(std::move(undo));
        }
        job->done=true;
    }

    void processPatchSaveQueue() {
        std::shared_ptr<PatchSaveJob> job;
        { std::unique_lock<std::mutex> lk(patchSaveQueueMtx); if (!patchSaveQueue.empty()) { job=patchSaveQueue.front(); patchSaveQueue.pop(); } }
        if (!job) return;
        if (APP->patch->path.empty()) {
            job->error = "patch has no file path (File > Save As in VCV Rack first)";
        } else {
            job->savedPath = APP->patch->path;
            APP->patch->save(APP->patch->path);
            job->success = true;
        }
        job->done = true;
    }

    template<typename T>
    static bool waitDone(std::shared_ptr<T>& job, int ms=500) {
        auto dl=std::chrono::steady_clock::now()+std::chrono::milliseconds(ms);
        while (!job->done.load() && std::chrono::steady_clock::now()<dl)
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return job->done.load();
    }

    // Strip per-port voltage/voltages/peak fields from a module JSON blob.
    // Relies on the fixed field order written by updateCache():
    // ..."channels": N, "voltage": X, "voltages": [...], "peak": P, "connected": ...
    static std::string slimModuleJson(std::string s) {
        const std::string from = ", " + jStr("voltage") + ": ";
        const std::string upto = ", " + jStr("connected") + ": ";
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::string::npos) {
            size_t end = s.find(upto, pos);
            if (end == std::string::npos) break;
            s.erase(pos, end - pos);
            pos += upto.size();
        }
        return s;
    }

    void dispatchSibyl(httplib::Response& res, int64_t moduleId,
                       SibylControl::Operation operation, const std::string& requestJson) {
        auto job=std::make_shared<SibylJob>();
        job->moduleId=moduleId; job->operation=operation; job->requestJson=requestJson;
        { std::unique_lock<std::mutex> lk(sibylQueueMtx); sibylQueue.push(job); }
        if (!waitDone(job, 5000)) res.set_content("{\"error\":\"timeout\"}","application/json");
        else if (!job->success && !job->responseJson.empty()) res.set_content(job->responseJson,"application/json");
        else if (!job->success) res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        else res.set_content(job->responseJson,"application/json");
    }

    // ── HTTP routes ───────────────────────────────────────────────────────────
    void setupRoutes() {
        // Optional shared-secret auth: set OCTAVIA_TOKEN in the environment
        // (both for VCV Rack and the MCP server) to require it on every request.
        static const std::string token = octaviaToken();
        if (!token.empty()) {
            svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res){
                if (req.get_header_value("X-Octavia-Token") != token) {
                    res.status = 401;
                    res.set_content("{\"error\":\"invalid or missing X-Octavia-Token\"}", "application/json");
                    return httplib::Server::HandlerResponse::Handled;
                }
                return httplib::Server::HandlerResponse::Unhandled;
            });
        }
        svr.Get("/status", [this](const httplib::Request&, httplib::Response& res) {
            std::string b = "{"+jStr("running")+": "+(serverRunning?"true":"false")+", "+jStr("port")+": "+std::to_string(octaviaPort())+", "+jStr("version")+": "+jStr("2.10.0")+"}";
            res.set_content(b,"application/json");
        });

        svr.Get("/modules",  [this](const httplib::Request&, httplib::Response& res){ std::unique_lock<std::mutex> lk(cacheMtx); res.set_content(moduleListJson,"application/json"); });
        svr.Get("/modules/summary", [this](const httplib::Request&, httplib::Response& res){ std::unique_lock<std::mutex> lk(cacheMtx); res.set_content(moduleSummaryJson,"application/json"); });
        svr.Get(R"(/modules/(\d+)$)", [this](const httplib::Request& r, httplib::Response& res){
            int64_t modId = std::stoll(r.matches[1].str());
            std::unique_lock<std::mutex> lk(cacheMtx);
            std::string needle = jStr("id") + ": " + std::to_string(modId) + ",";
            auto pos = moduleListJson.find(needle);
            if (pos == std::string::npos) { res.set_content("{\"error\":\"module not found\"}","application/json"); return; }
            auto start = moduleListJson.rfind('{', pos);
            if (start == std::string::npos) { res.set_content("{\"error\":\"parse error\"}","application/json"); return; }
            int depth = 0; size_t end = start;
            for (; end < moduleListJson.size(); end++) {
                if (moduleListJson[end] == '{') depth++;
                else if (moduleListJson[end] == '}') { depth--; if (!depth) break; }
            }
            std::string body = moduleListJson.substr(start, end - start + 1);
            if (r.has_param("slim") && r.get_param_value("slim") != "0")
                body = slimModuleJson(body);
            res.set_content(body, "application/json");
        });
        svr.Get("/cables",   [this](const httplib::Request&, httplib::Response& res){ std::unique_lock<std::mutex> lk(cacheMtx); res.set_content(cableListJson,"application/json"); });
        svr.Get("/graph",    [this](const httplib::Request&, httplib::Response& res){ std::unique_lock<std::mutex> lk(cacheMtx); res.set_content(graphJson,"application/json"); });
        svr.Get("/scope",    [this](const httplib::Request&, httplib::Response& res){ std::unique_lock<std::mutex> lk(cacheMtx); res.set_content(scopeJson,"application/json"); });

        svr.Get(R"(/scope/(\d+))", [this](const httplib::Request& r, httplib::Response& res){
            int64_t modId=std::stoll(r.matches[1].str());
            std::unique_lock<std::mutex> lk(cacheMtx);
            std::string key="\""+std::to_string(modId)+"\"";
            auto pos=scopeJson.find(key);
            if (pos==std::string::npos){ res.set_content("{\"error\":\"module not found\"}","application/json"); return; }
            auto start=scopeJson.find("{",pos+key.size());
            if (start==std::string::npos){ res.set_content("{\"error\":\"parse error\"}","application/json"); return; }
            int depth=0; size_t end=start;
            for (;end<scopeJson.size();end++){ if(scopeJson[end]=='{') depth++; else if(scopeJson[end]=='}'){ depth--; if(!depth) break; } }
            res.set_content(scopeJson.substr(start,end-start+1),"application/json");
        });

        // ── GET /audio/{port} — audio-rate analysis ───────────────────────────
        // port 0 = AUDIO_IN_L, port 1 = AUDIO_IN_R
        // Connect a cable from any module's output into the Bridge's ANALYZE input
        // to get real-time frequency analysis.
        svr.Get(R"(/audio/(\d+))", [this](const httplib::Request& r, httplib::Response& res){
            int port = std::stoi(r.matches[1].str());
            if (port < 0 || port >= 2) {
                res.set_content("{\"error\": \"port must be 0 (L) or 1 (R)\"}", "application/json");
                return;
            }

            // Snapshot ring buffer
            float buf[AUDIO_BUF];
            {
                int h = audioRing[port].head.load(std::memory_order_relaxed);
                for (int i = 0; i < AUDIO_BUF; i++)
                    buf[i] = audioRing[port].data[(h - AUDIO_BUF + i + AUDIO_BUF) & (AUDIO_BUF - 1)].load(std::memory_order_relaxed);
            }
            float sr = sampleRate.load(std::memory_order_relaxed);

            // RMS + peak
            float sumSq = 0.f, pk = 0.f;
            for (int i = 0; i < AUDIO_BUF; i++) {
                sumSq += buf[i] * buf[i];
                float av = std::fabs(buf[i]);
                if (av > pk) pk = av;
            }
            float rms = sqrtf(sumSq / AUDIO_BUF);

            // Spectrum: DFT at 20–2000 Hz in 20 Hz steps, stride-8 sampling
            // Stride-8 → effective sr = 44100/8 = 5512 Hz (Nyquist = 2756 Hz, covers our range)
            const int STRIDE = 8;
            const int N      = AUDIO_BUF / STRIDE;  // 512 samples per DFT
            float strSr      = sr / STRIDE;

            float bestMag  = 0.f;
            float bestFreq = 0.f;
            std::string spec = "[";
            bool sfirst = true;

            for (int fi = 1; fi <= 100; fi++) {           // 20, 40, 60 … 2000 Hz
                float freq = fi * 20.f;
                if (freq > strSr * 0.5f) break;            // above Nyquist → skip
                float re = 0.f, im = 0.f;
                float k  = 2.0f * float(M_PI) * freq / strSr;
                for (int i = 0; i < N; i++) {
                    float v = buf[i * STRIDE];
                    re += v * cosf(k * i);
                    im += v * sinf(k * i);
                }
                float mag = sqrtf(re*re + im*im) / N;
                if (mag > bestMag) { bestMag = mag; bestFreq = freq; }
                if (!sfirst) spec += ", ";
                sfirst = false;
                spec += "{" + jStr("hz") + ": " + std::to_string((int)freq)
                           + ", " + jStr("mag") + ": " + std::to_string(mag) + "}";
            }
            spec += "]";

            std::string note = freqToNote(bestFreq);

            std::string body = "{";
            body += jStr("port")         + ": " + std::to_string(port) + ", ";
            body += jStr("rms")          + ": " + std::to_string(rms) + ", ";
            body += jStr("peak")         + ": " + std::to_string(pk) + ", ";
            body += jStr("dominantHz")   + ": " + std::to_string(bestFreq) + ", ";
            body += jStr("dominantNote") + ": " + jStr(note) + ", ";
            body += jStr("spectrum")     + ": " + spec;
            body += "}";
            res.set_content(body, "application/json");
        });

        // ── GET /audio/{port}/analyze — full-band problem-frequency analysis ──
        // Two spectral snapshots ~120 ms apart (temporal stability separates
        // constant defects like hum/feedback from musical content).
        // Goertzel spectrum 20 Hz – 20 kHz on a 1/12-octave log grid.
        // Reports: issues[] summary, resonances (with stable flag), hum
        // (narrowband + stability checked), feedback suspect, inharmonic
        // high peaks (aliasing/artifacts), band levels, noise floor, DC offset.
        // ?spectrum=1 appends the raw bins of the latest snapshot.
        svr.Get(R"(/audio/(\d+)/analyze)", [this](const httplib::Request& r, httplib::Response& res){
            int port = std::stoi(r.matches[1].str());
            if (port < 0 || port >= 2) {
                res.set_content("{\"error\": \"port must be 0 (L) or 1 (R)\"}", "application/json");
                return;
            }
            float sr = sampleRate.load(std::memory_order_relaxed);

            // 1/12-octave grid 20 Hz → min(20 kHz, 0.45*sr)
            float fMax = std::min(20000.f, sr * 0.45f);
            std::vector<float> freqs;
            for (float f = 20.f; f <= fMax; f *= 1.0594631f)  // 2^(1/12)
                freqs.push_back(f);
            const int nb = (int)freqs.size();

            // Function-local initialization is performed exactly once by C++.
            static const std::array<float, AUDIO_BUF> win = [] {
                std::array<float, AUDIO_BUF> values{};
                for (int i = 0; i < AUDIO_BUF; i++)
                    values[i] = 0.5f * (1.f - cosf(2.f * float(M_PI) * i / (AUDIO_BUF - 1)));
                return values;
            }();

            struct Snap {
                std::vector<float> wbuf;   // Hann-windowed, DC-removed
                std::vector<float> dbs;    // per grid bin
                float rms = 0.f, pk = 0.f;
                int clippedSamples = 0;
                double mean = 0.0;
            };

            // Goertzel magnitude in dB (0 dB = 5V sine amplitude)
            auto magDb = [&](const Snap& sn, float f) -> float {
                float w = 2.f * float(M_PI) * f / sr;
                float coeff = 2.f * cosf(w);
                float s1 = 0.f, s2 = 0.f;
                for (int i = 0; i < AUDIO_BUF; i++) {
                    float s0 = sn.wbuf[i] + coeff * s1 - s2;
                    s2 = s1; s1 = s0;
                }
                float power = s1*s1 + s2*s2 - coeff*s1*s2;
                if (power < 0.f) power = 0.f;
                float amp = sqrtf(power) * (4.f / AUDIO_BUF) / 5.f;
                return 20.f * log10f(amp + 1e-7f);
            };

            auto capture = [&]() -> Snap {
                Snap sn;
                float buf[AUDIO_BUF];
                {
                    int h = audioRing[port].head.load(std::memory_order_acquire);
                    for (int i = 0; i < AUDIO_BUF; i++)
                        buf[i] = audioRing[port].data[(h - AUDIO_BUF + i + AUDIO_BUF) & (AUDIO_BUF - 1)].load(std::memory_order_relaxed);
                }
                for (int i = 0; i < AUDIO_BUF; i++) sn.mean += buf[i];
                sn.mean /= AUDIO_BUF;
                float sumSq = 0.f;
                sn.wbuf.resize(AUDIO_BUF);
                for (int i = 0; i < AUDIO_BUF; i++) {
                    float v = buf[i] - (float)sn.mean;
                    sumSq += v * v;
                    float a = std::fabs(v);
                    if (a > sn.pk) sn.pk = a;
                    if (a >= 10.f) sn.clippedSamples++;
                    sn.wbuf[i] = v * win[i];
                }
                sn.rms = sqrtf(sumSq / AUDIO_BUF);
                sn.dbs.resize(nb);
                for (int i = 0; i < nb; i++) sn.dbs[i] = magDb(sn, freqs[i]);
                return sn;
            };

            Snap A = capture();
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
            Snap B = capture();

            // Noise floor = median bin level of latest snapshot
            std::vector<float> sorted = B.dbs;
            std::sort(sorted.begin(), sorted.end());
            float floorDb = sorted[nb / 2];

            std::vector<std::string> issues;

            // Resonance peaks in B: local maxima, >=8 dB prominence.
            // stable = same level (±3 dB) in snapshot A → standing resonance,
            // prime EQ-notch candidate; unstable = transient musical content.
            struct Peak { float hz, db, prom; bool stable; };
            std::vector<Peak> peaks;
            for (int i = 2; i < nb - 2; i++) {
                if (B.dbs[i] <= B.dbs[i-1] || B.dbs[i] < B.dbs[i+1]) continue;
                float loc = 0.f; int cnt = 0;
                for (int k = -6; k <= 6; k++) {
                    if (k >= -1 && k <= 1) continue;
                    int j = i + k;
                    if (j < 0 || j >= nb) continue;
                    loc += B.dbs[j]; cnt++;
                }
                loc /= std::max(cnt, 1);
                float prom = B.dbs[i] - loc;
                if (prom >= 8.f && B.dbs[i] > floorDb + 6.f && B.dbs[i] > -80.f) {
                    bool stable = std::fabs(A.dbs[i] - B.dbs[i]) <= 3.f;
                    peaks.push_back({freqs[i], B.dbs[i], prom, stable});
                }
            }
            std::sort(peaks.begin(), peaks.end(),
                      [](const Peak& a, const Peak& b){ return a.prom > b.prom; });
            if (peaks.size() > 10) peaks.resize(10);

            // Mains hum: per harmonic require level > floor+12 dB AND
            // narrowband (>=6 dB above ±15% off-grid neighbors — rejects broad
            // musical energy near the 50/60 Hz grid) AND stable across snapshots.
            auto humSeries = [&](float f0, std::string& arr) -> bool {
                int hits = 0;
                arr = "[";
                for (int k = 1; k <= 4; k++) {
                    float f = f0 * k;
                    float d  = magDb(B, f);
                    float nbr = std::max(magDb(B, f * 0.85f), magDb(B, f * 1.15f));
                    float narrow = d - nbr;
                    bool stable = std::fabs(magDb(A, f) - d) < 4.f;
                    bool hit = d > floorDb + 12.f && narrow >= 6.f && stable;
                    if (hit) hits++;
                    if (k > 1) arr += ",";
                    arr += "{" + jStr("hz") + ": " + std::to_string((int)f)
                         + ", " + jStr("db") + ": " + jNum(d)
                         + ", " + jStr("narrowDb") + ": " + jNum(narrow)
                         + ", " + jStr("stable") + ": " + (stable ? "true" : "false") + "}";
                }
                arr += "]";
                return hits >= 2;  // fundamental-ish + at least one harmonic
            };
            std::string hum50, hum60;
            bool has50 = humSeries(50.f, hum50);
            bool has60 = humSeries(60.f, hum60);
            if (has50) issues.push_back("hum_50");
            if (has60) issues.push_back("hum_60");

            // Feedback candidates must rise across independent requests. This
            // avoids calling a stable musical oscillator "feedback" after one
            // short two-snapshot observation.
            std::string feedback = "null";
            for (auto& p : peaks) {
                if (p.prom < 18.f || p.db < -20.f) continue;
                int bi = (int)roundf(12.f * log2f(p.hz / 20.f));
                if (bi < 0 || bi >= nb) continue;
                float rise = B.dbs[bi] - A.dbs[bi];
                int observations = 0;
                {
                    std::unique_lock<std::mutex> lk(feedbackMtx);
                    FeedbackTrack& track = feedbackTrack[port];
                    double age = track.seen.time_since_epoch().count() == 0 ? 99.0
                        : std::chrono::duration<double>(std::chrono::steady_clock::now() - track.seen).count();
                    bool samePeak = track.hz > 0.f && std::fabs(log2f(p.hz / track.hz)) < (1.f / 12.f);
                    if (samePeak && age < 2.5 && p.db >= track.db + 0.75f)
                        track.risingObservations++;
                    else
                        track.risingObservations = rise >= 1.f ? 1 : 0;
                    track.hz = p.hz; track.db = p.db; track.seen = std::chrono::steady_clock::now();
                    observations = track.risingObservations;
                }
                if (observations >= 2) {
                    feedback = "{" + jStr("hz") + ": " + jNum(p.hz)
                             + ", " + jStr("note") + ": " + jStr(freqToNote(p.hz))
                             + ", " + jStr("db") + ": " + jNum(p.db)
                             + ", " + jStr("riseDb") + ": " + jNum(rise)
                             + ", " + jStr("risingObservations") + ": " + std::to_string(observations) + "}";
                    issues.push_back("feedback_suspect");
                    break;
                }
            }

            // Aliasing/artifact suspect: peaks >8 kHz not harmonically related
            // to the strongest peak below 5 kHz (ratio far from an integer)
            std::string inharm = "[";
            {
                float f0 = 0.f, best = -999.f;
                for (auto& p : peaks)
                    if (p.hz < 5000.f && p.db > best) { best = p.db; f0 = p.hz; }
                bool first = true;
                for (auto& p : peaks) {
                    if (p.hz < 8000.f || f0 <= 0.f) continue;
                    float ratio = p.hz / f0;
                    float offInt = std::fabs(ratio - roundf(ratio));
                    if (offInt > 0.07f && p.db > floorDb + 12.f) {
                        if (!first) inharm += ", ";
                        first = false;
                        inharm += "{" + jStr("hz") + ": " + jNum(p.hz)
                                + ", " + jStr("db") + ": " + jNum(p.db) + "}";
                    }
                }
                if (!first) issues.push_back("aliasing_suspect");
            }
            inharm += "]";

            // Average linear power, then express it in dB. Averaging dB bins
            // would bias broad-band material downward.
            auto bandDb = [&](float lo, float hi) -> float {
                double sumPower = 0.0; int cnt = 0;
                for (int i = 0; i < nb; i++)
                    if (freqs[i] >= lo && freqs[i] < hi) { sumPower += pow(10.0, B.dbs[i] / 10.0); cnt++; }
                return cnt ? (float)(10.0 * log10(sumPower / cnt + 1e-14)) : -140.f;
            };
            float rumbleDb = bandDb(20.f, 45.f),   bassDb = bandDb(45.f, 250.f);
            float lowmidDb = bandDb(250.f, 800.f), midDb = bandDb(800.f, 2500.f);
            float highmidDb = bandDb(2500.f, 5000.f);
            float sibDb = bandDb(5000.f, 8000.f),  airDb = bandDb(8000.f, 16000.f);

            if (std::fabs((float)B.mean) > 0.25f) issues.push_back("dc_offset");
            if (rumbleDb > bassDb + 6.f && rumbleDb > -60.f) issues.push_back("rumble");
            if (sibDb > midDb + 6.f && sibDb > -60.f) issues.push_back("sibilance");
            if (B.rms < 1e-4f) issues.push_back("silence");
            if (B.clippedSamples > 0) issues.push_back("clipping");

            std::string issuesArr = "[";
            for (size_t i = 0; i < issues.size(); i++) {
                if (i) issuesArr += ", ";
                issuesArr += jStr(issues[i]);
            }
            issuesArr += "]";

            std::string body = "{";
            body += jStr("port") + ": " + std::to_string(port) + ", ";
            body += jStr("inputConnected") + ": " + (audioInputConnected[port].load(std::memory_order_relaxed) ? "true" : "false") + ", ";
            body += jStr("issues") + ": " + issuesArr + ", ";
            body += jStr("rms") + ": " + jNum(B.rms) + ", ";
            body += jStr("peak") + ": " + jNum(B.pk) + ", ";
            body += jStr("clippedSamples") + ": " + std::to_string(B.clippedSamples) + ", ";
            body += jStr("dcOffset") + ": " + jNum((float)B.mean) + ", ";
            body += jStr("noiseFloorDb") + ": " + jNum(floorDb) + ", ";
            body += jStr("bandsDb") + ": {"
                  + jStr("rumble_20_45") + ": " + jNum(rumbleDb) + ", "
                  + jStr("bass_45_250") + ": " + jNum(bassDb) + ", "
                  + jStr("lowmid_250_800") + ": " + jNum(lowmidDb) + ", "
                  + jStr("mid_800_2500") + ": " + jNum(midDb) + ", "
                  + jStr("highmid_2500_5000") + ": " + jNum(highmidDb) + ", "
                  + jStr("sibilance_5000_8000") + ": " + jNum(sibDb) + ", "
                  + jStr("air_8000_16000") + ": " + jNum(airDb) + "}, ";
            body += jStr("hum") + ": {"
                  + jStr("detected50") + ": " + (has50 ? "true" : "false") + ", "
                  + jStr("detected60") + ": " + (has60 ? "true" : "false") + ", "
                  + jStr("series50") + ": " + hum50 + ", "
                  + jStr("series60") + ": " + hum60 + "}, ";
            body += jStr("feedback") + ": " + feedback + ", ";
            body += jStr("inharmonicHighPeaks") + ": " + inharm + ", ";
            body += jStr("resonances") + ": [";
            for (size_t i = 0; i < peaks.size(); i++) {
                if (i) body += ", ";
                body += "{" + jStr("hz") + ": " + jNum(peaks[i].hz)
                      + ", " + jStr("note") + ": " + jStr(freqToNote(peaks[i].hz))
                      + ", " + jStr("db") + ": " + jNum(peaks[i].db)
                      + ", " + jStr("prominenceDb") + ": " + jNum(peaks[i].prom)
                      + ", " + jStr("stable") + ": " + (peaks[i].stable ? "true" : "false") + "}";
            }
            body += "]";
            if (r.has_param("spectrum") && r.get_param_value("spectrum") != "0") {
                body += ", " + jStr("spectrum") + ": [";
                for (int i = 0; i < nb; i++) {
                    if (i) body += ",";
                    body += "{" + jStr("hz") + ": " + std::to_string((int)roundf(freqs[i]))
                          + "," + jStr("db") + ": " + jNum(B.dbs[i]) + "}";
                }
                body += "]";
            }
            body += "}";
            res.set_content(body, "application/json");
        });

        // ── GET /audio/loudness — integrated loudness + stereo image since reset ──
        // Continuous meter fed by the Analyze L/R inputs. POST /audio/loudness/reset
        // to start a fresh measurement window, let the patch play, then read.
        svr.Get("/audio/loudness", [this](const httplib::Request&, httplib::Response& res){
            double kSum[2], rawSum[2], peak[2], sumLR; uint64_t clipped[2], n;
            {
                std::unique_lock<std::mutex> lk(lm.mtx);
                for (int j = 0; j < 2; j++) { kSum[j]=lm.pKSum[j]; rawSum[j]=lm.pRawSum[j]; peak[j]=lm.pPeak[j]; clipped[j]=lm.pClipped[j]; }
                sumLR = lm.pSumLR; n = lm.pN;
            }
            if (n < sampleRate.load(std::memory_order_relaxed) * 3.f) {
                res.set_content("{\"error\":\"not enough data — let the patch play for at least three seconds, then read again\"}",
                                "application/json");
                return;
            }
            auto db  = [](double x){ return 10.0 * log10(x + 1e-12); };
            auto lufsFromPower = [&](double p){ return -0.691 + db(p); };
            uint64_t totalBlocks = lm.blockTotal.load(std::memory_order_acquire);
            int availableBlocks = (int)std::min<uint64_t>(totalBlocks, LOUDNESS_BLOCKS);
            std::vector<float> blockPowers;
            blockPowers.reserve(availableBlocks);
            for (uint64_t i = totalBlocks - availableBlocks; i < totalBlocks; i++)
                blockPowers.push_back(
                    lm.blocks[0][i % LOUDNESS_BLOCKS].load(std::memory_order_relaxed)
                    + lm.blocks[1][i % LOUDNESS_BLOCKS].load(std::memory_order_relaxed));
            auto windowLufs = [&](int blocks) -> double {
                if ((int)blockPowers.size() < blocks) return -INFINITY;
                double sum = 0.;
                for (int i = (int)blockPowers.size() - blocks; i < (int)blockPowers.size(); i++) sum += blockPowers[i];
                return lufsFromPower(sum / blocks);
            };
            // EBU R128: 400 ms blocks, 75% overlap, absolute -70 LUFS gate,
            // then a relative gate 10 LU below the absolute-gated programme.
            std::vector<double> absoluteGated;
            for (float p : blockPowers) if (lufsFromPower(p) > -70.0) absoluteGated.push_back(p);
            double absoluteMean = 0.; for (double p : absoluteGated) absoluteMean += p;
            absoluteMean /= std::max<size_t>(1, absoluteGated.size());
            double relativeGate = lufsFromPower(absoluteMean) - 10.0, gatedSum = 0.; int gatedCount = 0;
            for (double p : absoluteGated) if (lufsFromPower(p) > relativeGate) { gatedSum += p; gatedCount++; }
            double integratedLufs = gatedCount ? lufsFromPower(gatedSum / gatedCount) : -INFINITY;
            double momentaryLufs = windowLufs(4), shortTermLufs = windowLufs(30);
            double seconds = n / (double)sampleRate.load(std::memory_order_relaxed);
            double kWeightedDbfsL  = lufsFromPower(kSum[0] / n);
            double kWeightedDbfsR  = lufsFromPower(kSum[1] / n);
            double rmsL   = db(rawSum[0] / n), rmsR = db(rawSum[1] / n);
            double peakL  = 20.0 * log10(peak[0] + 1e-12), peakR = 20.0 * log10(peak[1] + 1e-12);
            double corr   = sumLR / (sqrt(rawSum[0] * rawSum[1]) + 1e-12);
            double balDb  = 0.5 * (db(rawSum[0]) - db(rawSum[1]));  // >0 = left louder
            double midMS  = (rawSum[0] + 2.0*sumLR + rawSum[1]) / (4.0 * n);
            double sideMS = (rawSum[0] - 2.0*sumLR + rawSum[1]) / (4.0 * n);
            double sideMidDb = db(sideMS) - db(midMS);
            bool leftConnected = audioInputConnected[0].load(std::memory_order_relaxed);
            bool rightConnected = audioInputConnected[1].load(std::memory_order_relaxed);

            std::string b = "{";
            b += jStr("secondsMeasured") + ": " + jNum((float)seconds) + ", ";
            b += jStr("inputs") + ": {" + jStr("leftConnected") + ": " + (leftConnected ? "true" : "false")
               + ", " + jStr("rightConnected") + ": " + (rightConnected ? "true" : "false") + "}, ";
            b += jStr("integratedLufs") + ": " + jNum((float)integratedLufs) + ", ";
            b += jStr("momentaryLufs") + ": " + jNum((float)momentaryLufs) + ", ";
            b += jStr("shortTermLufs") + ": " + jNum((float)shortTermLufs) + ", ";
            b += jStr("gatedBlocks") + ": " + std::to_string(gatedCount) + ", ";
            b += jStr("left")  + ": {" + jStr("kWeightedDbfsEstimate") + ": " + jNum((float)kWeightedDbfsL) + ", "
               + jStr("rmsDb") + ": " + jNum((float)rmsL) + ", "
               + jStr("peakDb") + ": " + jNum((float)peakL) + ", "
               + jStr("crestDb") + ": " + jNum((float)(peakL - rmsL)) + ", "
               + jStr("clippedSamples") + ": " + std::to_string(clipped[0]) + "}, ";
            b += jStr("right") + ": {" + jStr("kWeightedDbfsEstimate") + ": " + jNum((float)kWeightedDbfsR) + ", "
               + jStr("rmsDb") + ": " + jNum((float)rmsR) + ", "
               + jStr("peakDb") + ": " + jNum((float)peakR) + ", "
               + jStr("crestDb") + ": " + jNum((float)(peakR - rmsR)) + ", "
               + jStr("clippedSamples") + ": " + std::to_string(clipped[1]) + "}, ";
            b += jStr("stereo") + ": {" + jStr("available") + ": " + (leftConnected && rightConnected ? "true" : "false");
            if (leftConnected && rightConnected) {
                b += ", " + jStr("correlation") + ": " + jNum((float)corr) + ", "
                   + jStr("balanceDb") + ": " + jNum((float)balDb) + ", "
                   + jStr("sideMidDb") + ": " + jNum((float)sideMidDb);
            }
            b += "}";
            b += "}";
            res.set_content(b, "application/json");
        });

        svr.Post("/audio/loudness/reset", [this](const httplib::Request&, httplib::Response& res){
            lm.resetFlag.store(true, std::memory_order_relaxed);
            res.set_content("{\"ok\":true}", "application/json");
        });

        svr.Get(R"(/modules/(\d+)/params/(\d+))",
            [this](const httplib::Request& r, httplib::Response& res){
                int64_t modId=std::stoll(r.matches[1].str()); int pid=std::stoi(r.matches[2].str());
                std::unique_lock<std::mutex> lk(cacheMtx);
                for (auto& e:cache) {
                    if (e.id==modId && pid<(int)e.params.size()) {
                        const ParamEntry& pe = e.params[pid];
                        std::string b = "{";
                        b += jStr("module_id")+": "+std::to_string(modId)+", ";
                        b += jStr("param_id")+": "+std::to_string(pid)+", ";
                        b += jStr("name")+": "+jStr(pe.name)+", ";
                        b += jStr("value")+": "+jNum(pe.value)+", ";
                        b += jStr("min")+": "+jNum(pe.minV)+", ";
                        b += jStr("max")+": "+jNum(pe.maxV)+", ";
                        b += jStr("default")+": "+jNum(pe.defaultV)+", ";
                        b += jStr("unit")+": "+jStr(pe.unit);
                        b += "}";
                        res.set_content(b,"application/json"); return;
                    }
                }
                res.set_content("{\"error\":\"not found\"}","application/json");
            });

        svr.Post("/modules", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<AddModuleJob>();
            job->pluginSlug=parseStringField(r.body,"plugin"); job->modelSlug=parseStringField(r.body,"model");
            { std::unique_lock<std::mutex> lk(addQueueMtx); addQueue.push(job); }
            if (!waitDone(job,2000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true,\"id\":"+std::to_string(job->newId)+"}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post(R"(/modules/(\d+)/params/(\d+))",
            [this](const httplib::Request& r, httplib::Response& res){
                auto job=std::make_shared<SetParamJob>();
                job->moduleId=std::stoll(r.matches[1].str()); job->paramId=std::stoi(r.matches[2].str()); job->value=parseFloatField(r.body,"value");
                { std::unique_lock<std::mutex> lk(setQueueMtx); setQueue.push(job); }
                if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
                else if (job->success) res.set_content("{\"ok\":true,\"value\":"+std::to_string(job->value)+"}","application/json");
                else res.set_content("{\"error\":\"param not found\"}","application/json");
            });

        svr.Post("/cables", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<CableJob>(); job->type=CableJob::ADD;
            job->outModId=parseInt64Field(r.body,"outputModuleId"); job->outPortId=parseInt64Field(r.body,"outputPortId");
            job->inModId=parseInt64Field(r.body,"inputModuleId");   job->inPortId=parseInt64Field(r.body,"inputPortId");
            job->color=parseStringField(r.body,"color");
            { std::unique_lock<std::mutex> lk(cableQueueMtx); cableQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":\""+job->error+"\"}","application/json");
        });

        svr.Post("/cables/disconnect", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<CableJob>(); job->type=CableJob::REMOVE;
            job->inModId=parseInt64Field(r.body,"inputModuleId"); job->inPortId=parseInt64Field(r.body,"inputPortId");
            { std::unique_lock<std::mutex> lk(cableQueueMtx); cableQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":\""+job->error+"\"}","application/json");
        });

        svr.Post("/cables/disconnect-output", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<CableJob>(); job->type=CableJob::REMOVE_OUTPUT;
            job->outModId=parseInt64Field(r.body,"outputModuleId"); job->outPortId=parseInt64Field(r.body,"outputPortId");
            { std::unique_lock<std::mutex> lk(cableQueueMtx); cableQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":\""+job->error+"\"}","application/json");
        });

        svr.Delete(R"(/modules/(\d+))", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<DeleteModuleJob>();
            job->moduleId=std::stoll(r.matches[1].str());
            { std::unique_lock<std::mutex> lk(deleteQueueMtx); deleteQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post(R"(/modules/(\d+)/position)", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<MoveModuleJob>();
            job->moduleId=std::stoll(r.matches[1].str());
            job->hp=parseFloatField(r.body,"hp");
            json_error_t jerr;
            json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* row=root ? json_object_get(root,"row") : NULL;
            if (row && json_is_integer(row)) { job->row=(int)json_integer_value(row); job->hasRow=true; }
            if (root) json_decref(root);
            { std::unique_lock<std::mutex> lk(moveQueueMtx); moveQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true,\"hp\":"+jNum(job->resolvedHp)+",\"row\":"+std::to_string(job->resolvedRow)+"}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        // Atomically position multiple modules. All targets are validated before
        // any module moves, and the complete prior layout occupies one undo slot.
        svr.Post("/modules/layout", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jerr;
            json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* arr=root ? json_object_get(root,"changes") : NULL;
            if (!arr || !json_is_array(arr) || json_array_size(arr)==0) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"body must be {changes:[{moduleId,hp,row}]}\"}","application/json");
                return;
            }
            auto job=std::make_shared<BulkMoveJob>();
            size_t idx; json_t* el;
            json_array_foreach(arr,idx,el) {
                json_t* jm=json_object_get(el,"moduleId");
                json_t* jh=json_object_get(el,"hp");
                json_t* jr=json_object_get(el,"row");
                if (!json_is_integer(jm) || !json_is_number(jh) || !json_is_integer(jr)) {
                    json_decref(root);
                    res.set_content("{\"error\":\"each layout change requires integer moduleId, numeric hp, and integer row\"}","application/json");
                    return;
                }
                job->changes.push_back({(int64_t)json_integer_value(jm),(float)json_number_value(jh),(int)json_integer_value(jr)});
            }
            json_decref(root);
            { std::unique_lock<std::mutex> lk(bulkMoveQueueMtx); bulkMoveQueue.push(job); }
            if (!waitDone(job,2000)) { res.set_content("{\"error\":\"timeout\"}","application/json"); return; }
            if (!job->success) { res.set_content("{\"error\":"+jStr(job->error)+"}","application/json"); return; }
            std::string b="{\"ok\":true,\"applied\":"+std::to_string(job->results.size())+",\"positions\":[";
            for (size_t i=0;i<job->results.size();i++) {
                if (i) b+=",";
                auto& p=job->results[i];
                b+="{\"moduleId\":"+std::to_string(p.moduleId)+",\"hp\":"+jNum(p.hp)+",\"row\":"+std::to_string(p.row)+"}";
            }
            b+="]}";
            res.set_content(b,"application/json");
        });

        // ── POST /params/bulk — set many parameters in one call ──────────────
        // Body: {"changes":[{"moduleId":..,"paramId":..,"value":..}, ...]}
        svr.Post("/params/bulk", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jerr;
            json_t* root = json_loads(r.body.c_str(), 0, &jerr);
            json_t* arr = root ? json_object_get(root, "changes") : NULL;
            if (!arr || !json_is_array(arr)) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"body must be {changes:[{moduleId,paramId,value}]}\"}","application/json");
                return;
            }
            auto job = std::make_shared<BulkParamJob>();
            size_t idx; json_t* el;
            json_array_foreach(arr, idx, el) {
                json_t* jm = json_object_get(el, "moduleId");
                json_t* jp = json_object_get(el, "paramId");
                json_t* jv = json_object_get(el, "value");
                if (!jm || !jp || !jv) { job->failed.push_back((int)idx); continue; }
                job->changes.push_back({(int64_t)json_integer_value(jm),
                                        (int)json_integer_value(jp),
                                        (float)json_number_value(jv)});
            }
            json_decref(root);
            { std::unique_lock<std::mutex> lk(bulkParamQueueMtx); bulkParamQueue.push(job); }
            if (!waitDone(job, 2000)) { res.set_content("{\"error\":\"timeout\"}","application/json"); return; }
            std::string b = "{" + jStr("ok") + ": true, "
                          + jStr("applied") + ": " + std::to_string(job->changes.size() - job->failed.size())
                          + ", " + jStr("failedIndices") + ": [";
            for (size_t i = 0; i < job->failed.size(); i++) {
                if (i) b += ",";
                b += std::to_string(job->failed[i]);
            }
            b += "]}";
            res.set_content(b, "application/json");
        });

        // ── POST /undo — revert the most recent write operation ──────────────
        svr.Post("/undo", [this](const httplib::Request&, httplib::Response& res){
            auto job = std::make_shared<UndoJob>();
            { std::unique_lock<std::mutex> lk(undoQueueMtx); undoQueue.push(job); }
            if (!waitDone(job, 2000)) { res.set_content("{\"error\":\"timeout\"}","application/json"); return; }
            if (!job->success) { res.set_content("{"+jStr("error")+": "+jStr(job->error)+"}","application/json"); return; }
            res.set_content("{"+jStr("ok")+": true, "+jStr("undone")+": "+jStr(job->label)+"}","application/json");
        });

        // ── GET /undo/status — pending undo stack (newest first) ──────────────
        svr.Get("/undo/status", [this](const httplib::Request&, httplib::Response& res){
            std::string b = "{" + jStr("actions") + ": [";
            { std::unique_lock<std::mutex> lk(undoMtx);
              for (size_t i = 0; i < undoStack.size(); i++) {
                  if (i) b += ", ";
                  b += jStr(undoStack[undoStack.size()-1-i].label);
              } }
            b += "]}";
            res.set_content(b, "application/json");
        });

        // ── GET /audio/measure?seconds=N — reset meter, measure, return ──────
        // Server-side version of reset -> wait -> read: one call, correct timing.
        svr.Get("/audio/measure", [this](const httplib::Request& r, httplib::Response& res){
            int seconds = 15;
            if (r.has_param("seconds")) seconds = std::stoi(r.get_param_value("seconds"));
            seconds = std::max(1, std::min(60, seconds));
            lm.resetFlag.store(true, std::memory_order_relaxed);
            // wait for audio thread to consume the reset (or engine paused)
            for (int i = 0; i < 100 && lm.resetFlag.load(std::memory_order_relaxed); i++)
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (lm.resetFlag.load(std::memory_order_relaxed)) {
                res.set_content("{\"error\":\"audio engine not running — is VCV paused?\"}","application/json");
                return;
            }
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
            double kSum[2], rawSum[2], peak[2], sumLR; uint64_t n;
            { std::unique_lock<std::mutex> lk(lm.mtx);
              for (int j = 0; j < 2; j++) { kSum[j]=lm.pKSum[j]; rawSum[j]=lm.pRawSum[j]; peak[j]=lm.pPeak[j]; }
              sumLR = lm.pSumLR; n = lm.pN; }
            if (n < sampleRate.load(std::memory_order_relaxed) * 1.f) { res.set_content("{\"error\":\"no audio data during measurement window\"}","application/json"); return; }
            auto db = [](double x){ return 10.0 * log10(x + 1e-12); };
            double kWeightedDbfs = -0.691 + db((kSum[0] + kSum[1]) / n);
            double rmsL  = db(rawSum[0] / n), rmsR = db(rawSum[1] / n);
            double peakL = 20.0*log10(peak[0]+1e-12), peakR = 20.0*log10(peak[1]+1e-12);
            double corr  = sumLR / (sqrt(rawSum[0]*rawSum[1]) + 1e-12);
            double balDb = 0.5 * (db(rawSum[0]) - db(rawSum[1]));
            double midMS  = (rawSum[0] + 2.0*sumLR + rawSum[1]) / (4.0*n);
            double sideMS = (rawSum[0] - 2.0*sumLR + rawSum[1]) / (4.0*n);
            std::string b = "{";
            b += jStr("secondsMeasured") + ": " + jNum((float)(n/(double)sampleRate.load(std::memory_order_relaxed))) + ", ";
            b += jStr("kWeightedDbfsEstimate") + ": " + jNum((float)kWeightedDbfs) + ", ";
            b += jStr("left")  + ": {" + jStr("rmsDb") + ": " + jNum((float)rmsL) + ", " + jStr("peakDb") + ": " + jNum((float)peakL) + ", " + jStr("crestDb") + ": " + jNum((float)(peakL-rmsL)) + "}, ";
            b += jStr("right") + ": {" + jStr("rmsDb") + ": " + jNum((float)rmsR) + ", " + jStr("peakDb") + ": " + jNum((float)peakR) + ", " + jStr("crestDb") + ": " + jNum((float)(peakR-rmsR)) + "}, ";
            b += jStr("stereo") + ": {" + jStr("correlation") + ": " + jNum((float)corr) + ", " + jStr("balanceDb") + ": " + jNum((float)balDb) + ", " + jStr("sideMidDb") + ": " + jNum((float)(db(sideMS)-db(midMS))) + "}";
            b += "}";
            res.set_content(b, "application/json");
        });

        svr.Get("/perf", [this](const httplib::Request&, httplib::Response& res){
            double blockDur; int blockFr, modCnt, cableCnt;
            { std::unique_lock<std::mutex> lk(cacheMtx);
              blockDur=cachedBlockDurationMs; blockFr=cachedBlockFrames;
              modCnt=cachedModuleCount; cableCnt=cachedCableCount; }
#if defined(_WIN32)
            double userSec = 0.0, sysSec = 0.0;
            FILETIME ct, et, kt, ut;
            if (GetProcessTimes(GetCurrentProcess(), &ct, &et, &kt, &ut)) {
                auto ft2sec = [](const FILETIME& ft) {
                    ULARGE_INTEGER u; u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime;
                    return (double)u.QuadPart / 1e7;  // 100ns units
                };
                userSec = ft2sec(ut); sysSec = ft2sec(kt);
            }
#else
            struct rusage ru; getrusage(RUSAGE_SELF, &ru);
            double userSec = ru.ru_utime.tv_sec + ru.ru_utime.tv_usec/1e6;
            double sysSec  = ru.ru_stime.tv_sec + ru.ru_stime.tv_usec/1e6;
#endif
            std::string b = "{";
            b += jStr("sampleRate")+": "+std::to_string(sampleRate.load(std::memory_order_relaxed))+", ";
            b += jStr("blockFrames")+": "+std::to_string(blockFr)+", ";
            b += jStr("blockDurationMs")+": "+std::to_string(blockDur)+", ";
            b += jStr("moduleCount")+": "+std::to_string(modCnt)+", ";
            b += jStr("cableCount")+": "+std::to_string(cableCnt)+", ";
            b += jStr("processCpuUserSec")+": "+std::to_string(userSec)+", ";
            b += jStr("processCpuSysSec")+": "+std::to_string(sysSec);
            b += "}";
            res.set_content(b,"application/json");
        });
        svr.Post(R"(/modules/(\d+)/bypass)", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<BypassJob>();
            job->moduleId=std::stoll(r.matches[1].str());
            job->bypassed=parseBoolField(r.body,"bypassed");
            { std::unique_lock<std::mutex> lk(bypassQueueMtx); bypassQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true,\"bypassed\":"+std::string(job->bypassed?"true":"false")+"}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Get(R"(/modules/(\d+)/state)", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<ModuleStateJob>(); job->type=ModuleStateJob::GET;
            job->moduleId=std::stoll(r.matches[1].str());
            { std::unique_lock<std::mutex> lk(stateQueueMtx); stateQueue.push(job); }
            if (!waitDone(job,2000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content(job->stateJson,"application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        // Sibyl semantic API. Payloads are passed opaquely to the target module,
        // which is the sole authority for the composition schema and validation.
        svr.Get(R"(/sibyl/(\d+)/capabilities)", [this](const httplib::Request& r, httplib::Response& res){
            dispatchSibyl(res, std::stoll(r.matches[1].str()), SibylControl::Operation::CAPABILITIES, "{}");
        });
        svr.Get(R"(/sibyl/(\d+)/composition)", [this](const httplib::Request& r, httplib::Response& res){
            std::string view=r.has_param("view") ? r.get_param_value("view") : "summary";
            std::string request="{\"view\":"+jStr(view);
            if (r.has_param("id")) request+=",\"id\":"+jStr(r.get_param_value("id"));
            request+="}";
            dispatchSibyl(res, std::stoll(r.matches[1].str()), SibylControl::Operation::GET_COMPOSITION, request);
        });
        svr.Post(R"(/sibyl/(\d+)/validate)", [this](const httplib::Request& r, httplib::Response& res){
            dispatchSibyl(res, std::stoll(r.matches[1].str()), SibylControl::Operation::VALIDATE, r.body);
        });
        svr.Post(R"(/sibyl/(\d+)/edit)", [this](const httplib::Request& r, httplib::Response& res){
            dispatchSibyl(res, std::stoll(r.matches[1].str()), SibylControl::Operation::EDIT, r.body);
        });
        svr.Get(R"(/sibyl/(\d+)/status)", [this](const httplib::Request& r, httplib::Response& res){
            dispatchSibyl(res, std::stoll(r.matches[1].str()), SibylControl::Operation::GET_STATUS, "{}");
        });
        svr.Post(R"(/sibyl/(\d+)/transport)", [this](const httplib::Request& r, httplib::Response& res){
            dispatchSibyl(res, std::stoll(r.matches[1].str()), SibylControl::Operation::TRANSPORT, r.body);
        });

        svr.Post(R"(/modules/(\d+)/state)", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<ModuleStateJob>(); job->type=ModuleStateJob::SET;
            job->moduleId=std::stoll(r.matches[1].str());
            job->stateJson=r.body;
            { std::unique_lock<std::mutex> lk(stateQueueMtx); stateQueue.push(job); }
            if (!waitDone(job,2000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post(R"(/temporal-deck/(\d+)/transport)", [this](const httplib::Request& r, httplib::Response& res){
            auto job=std::make_shared<TemporalDeckJob>(); job->moduleId=std::stoll(r.matches[1].str());
            std::string action=parseStringField(r.body,"action");
            if (action=="load") { job->type=TemporalDeckJob::LOAD; job->path=parseStringField(r.body,"path"); }
            else if (action=="play") job->type=TemporalDeckJob::PLAY;
            else if (action=="stop_rewind") job->type=TemporalDeckJob::STOP_REWIND;
            else if (action=="seek") { job->type=TemporalDeckJob::SEEK; job->position=rack::math::clamp(parseFloatField(r.body,"position"),0.f,1.f); }
            else if (action=="set_loop") { job->type=TemporalDeckJob::SET_LOOP; job->enabled=parseBoolField(r.body,"enabled"); }
            else if (action=="status") job->type=TemporalDeckJob::STATUS;
            else { res.set_content("{\"error\":\"action must be load, play, stop_rewind, seek, set_loop, or status\"}","application/json"); return; }
            { std::unique_lock<std::mutex> lk(temporalDeckQueueMtx); temporalDeckQueue.push(job); }
            if (!waitDone(job, 5000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (!job->success) res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
            else res.set_content("{\"ok\":true,\"loaded\":"+std::string(job->loaded?"true":"false")+",\"playing\":"+std::string(job->playing?"true":"false")+",\"loop\":"+std::string(job->loop?"true":"false")+"}","application/json");
        });

        svr.Post("/patch/save", [this](const httplib::Request&, httplib::Response& res){
            auto job=std::make_shared<PatchSaveJob>();
            { std::unique_lock<std::mutex> lk(patchSaveQueueMtx); patchSaveQueue.push(job); }
            if (!waitDone(job,5000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true,\"path\":"+jStr(job->savedPath)+"}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Get("/patch", [this](const httplib::Request&, httplib::Response& res){
            std::unique_lock<std::mutex> lk(cacheMtx);
            res.set_content(patchInfoJson, "application/json");
        });

        svr.Get("/modules/voltages", [this](const httplib::Request&, httplib::Response& res){
            std::unique_lock<std::mutex> lk(cacheMtx);
            res.set_content(voltagesJson, "application/json");
        });

        svr.Get("/library", [](const httplib::Request& req, httplib::Response& res){
            std::string qFilter = req.has_param("q") ? req.get_param_value("q") : "";
            std::string pFilter = req.has_param("plugin") ? req.get_param_value("plugin") : "";
            // lowercase helper
            auto toLower = [](std::string s){ for(auto& c:s) c=tolower(c); return s; };
            std::string qLow = toLower(qFilter);

            std::string json = "[";
            bool pfirst = true;
            for (plugin::Plugin* plug : plugin::plugins) {
                if (!pFilter.empty() && plug->slug != pFilter) continue;
                // collect matching models
                std::string mJson = "[";
                bool mfirst = true;
                for (plugin::Model* mod : plug->models) {
                    if (!qLow.empty()) {
                        std::string slugL=toLower(mod->slug), nameL=toLower(mod->name);
                        if (slugL.find(qLow)==std::string::npos && nameL.find(qLow)==std::string::npos) continue;
                    }
                    if (!mfirst) mJson += ",";
                    mfirst = false;
                    mJson += "{" + jStr("slug") + ": " + jStr(mod->slug) + ", "
                               + jStr("name") + ": " + jStr(mod->name) + "}";
                }
                mJson += "]";
                if (mfirst && !qLow.empty()) continue; // no models matched
                if (!pfirst) json += ",";
                pfirst = false;
                json += "{" + jStr("slug") + ": " + jStr(plug->slug) + ", "
                           + jStr("name") + ": " + jStr(plug->name) + ", "
                           + jStr("models") + ": " + mJson + "}";
            }
            json += "]";
            res.set_content(json, "application/json");
        });
    }

    void startServer() {
        if (serverRunning) return;
        serverRunning=true;
        serverThread=std::thread([this](){ svr.listen("127.0.0.1", octaviaPort()); serverRunning=false; });
        serverThread.detach();
    }
    void stopServer() { if (!serverRunning) return; svr.stop(); serverRunning=false; }
};

// ── Colors ────────────────────────────────────────────────────────────────────
static const NVGcolor WHITE = nvgRGB(255,255,255);
static const NVGcolor DIM   = nvgRGB(80,80,80);

struct OctaviaStatusWidget : TransparentWidget {
    Octavia* module = nullptr;
    std::shared_ptr<window::Svg> octopusSvg;

    explicit OctaviaStatusWidget(Octavia* module)
        : module(module) {
        if (APP && APP->window) {
            octopusSvg = APP->window->loadSvg(
                asset::plugin(pluginInstance, "res/icon/Octopus-V.svg"));
        }
    }

    void draw(const DrawArgs& args) override {
        const bool serverRunning = module
            && module->serverRunning.load(std::memory_order_relaxed);
        if (!octopusSvg || !octopusSvg->handle) {
            return;
        }

        const Vec svgSize = octopusSvg->getSize();
        if (svgSize.x <= 0.f || svgSize.y <= 0.f) {
            return;
        }

        const float scale = std::min(box.size.x / svgSize.x, box.size.y / svgSize.y);
        const Vec fittedSize = svgSize.mult(scale);
        const Vec offset = box.size.minus(fittedSize).div(2.f);
        nvgSave(args.vg);
        nvgGlobalAlpha(args.vg, serverRunning ? 1.f : 0.28f);
        nvgTranslate(args.vg, offset.x, offset.y);
        nvgScale(args.vg, scale, scale);
        octopusSvg->draw(args.vg);
        nvgRestore(args.vg);
    }
};

struct OctaviaMeterWidget : TransparentWidget {
    Octavia* module = nullptr;
    float displayedLufs[2] = {};
    float displayedDbfs[2] = {};

    explicit OctaviaMeterWidget(Octavia* module)
        : module(module) {}

    static float normalizedDb(float db) {
        return std::max(0.f, std::min(1.f, (db + 60.f) / 60.f));
    }

    static float follow(float current, float target) {
        const float amount = target > current ? 0.42f : 0.075f;
        return current + (target - current) * amount;
    }

    void step() override {
        TransparentWidget::step();
        const uint64_t total = module
            ? module->lm.blockTotal.load(std::memory_order_acquire) : 0;
        const int count = (int)std::min<uint64_t>(total, 4);
        for (int j = 0; j < 2; ++j) {
            float lufsTarget = 0.f;
            float dbfsTarget = 0.f;
            const bool connected = module
                && module->audioInputConnected[j].load(std::memory_order_relaxed);
            if (connected && count > 0) {
                double power = 0.0;
                for (uint64_t i = total - count; i < total; ++i)
                    power += module->lm.blocks[j][i % LOUDNESS_BLOCKS].load(std::memory_order_relaxed);
                const float momentaryLufs = -0.691f
                    + 10.f * std::log10((float)(power / count) + 1e-12f);
                lufsTarget = normalizedDb(momentaryLufs);
                const float peak = module->lm.meterPeak[j].load(std::memory_order_relaxed);
                dbfsTarget = normalizedDb(20.f * std::log10(peak + 1e-12f));
            }
            displayedLufs[j] = follow(displayedLufs[j], lufsTarget);
            displayedDbfs[j] = follow(displayedDbfs[j], dbfsTarget);
        }
    }

    static void drawBar(NVGcontext* vg, const math::Rect& bounds, const float levels[2]) {
        const float radius = std::min(0.5f * bounds.size.x, 2.5f);
        nvgBeginPath(vg);
        nvgRoundedRect(vg, bounds.pos.x, bounds.pos.y, bounds.size.x, bounds.size.y, radius);
        nvgFillColor(vg, nvgRGB(7, 10, 15));
        nvgFill(vg);
        nvgStrokeWidth(vg, 1.f);
        nvgStrokeColor(vg, nvgRGBA(174, 132, 255, 96));
        nvgStroke(vg);

        const float inset = 1.25f;
        const Vec fillPos = bounds.pos.plus(Vec(inset, inset));
        const Vec fillSize = bounds.size.minus(Vec(2.f * inset, 2.f * inset));
        const float gap = 1.f;
        const float channelWidth = std::max(0.f, 0.5f * (fillSize.x - gap));
        for (int j = 0; j < 2; ++j) {
            const float level = std::max(0.f, std::min(1.f, levels[j]));
            const float fillHeight = std::max(0.f, fillSize.y * level);
            if (fillHeight <= 0.f || channelWidth <= 0.f) continue;
            const float channelX = fillPos.x + j * (channelWidth + gap);
            nvgSave(vg);
            nvgIntersectScissor(vg, channelX, fillPos.y + fillSize.y - fillHeight,
                channelWidth, fillHeight);
            nvgBeginPath(vg);
            nvgRoundedRect(vg, channelX, fillPos.y, channelWidth, fillSize.y,
                std::max(0.f, 0.5f * channelWidth));
            const NVGpaint fill = nvgLinearGradient(vg,
                channelX, fillPos.y + fillSize.y, channelX, fillPos.y,
                nvgRGB(122, 92, 255), nvgRGB(28, 204, 217));
            nvgFillPaint(vg, fill);
            nvgFill(vg);
            nvgRestore(vg);
        }

        const float dividerX = fillPos.x + 0.5f * fillSize.x;
        nvgBeginPath(vg);
        nvgMoveTo(vg, dividerX, fillPos.y);
        nvgLineTo(vg, dividerX, fillPos.y + fillSize.y);
        nvgStrokeWidth(vg, 1.f);
        nvgStrokeColor(vg, nvgRGBA(174, 132, 255, 72));
        nvgStroke(vg);
    }

    void draw(const DrawArgs& args) override {
        const float barWidth = mm2px(8.f);
        const float barHeight = box.size.y - mm2px(4.5f);
        const float leftX = mm2px(0.8f);
        const float rightX = box.size.x - leftX - barWidth;
        drawBar(args.vg, math::Rect(Vec(leftX, 0.f), Vec(barWidth, barHeight)), displayedLufs);
        drawBar(args.vg, math::Rect(Vec(rightX, 0.f), Vec(barWidth, barHeight)), displayedDbfs);

        if (!APP || !APP->window || !APP->window->uiFont) return;
        nvgFontFaceId(args.vg, APP->window->uiFont->handle);
        nvgFontSize(args.vg, 8.f);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg, WHITE);
        const float labelY = box.size.y - mm2px(1.7f);
        nvgText(args.vg, leftX + 0.5f * barWidth, labelY, "LUFS", NULL);
        nvgText(args.vg, rightX + 0.5f * barWidth, labelY, "dBFS", NULL);
    }
};

// ── Widget ────────────────────────────────────────────────────────────────────
struct OctaviaWidget : ModuleWidget {
    int uiTimer = 0;
    Vec titleLabelMm{15.24f, 7.5f};
    Vec portLabelMm{4.5f, 55.5f};
    Vec portValueLabelMm{26.f, 55.5f};
    Vec startLabelMm{4.5f, 66.f};
    Vec audioLabelLMm{8.f, 120.5f};
    Vec audioLabelRMm{22.f, 120.5f};

    void step() override {
        ModuleWidget::step();
        if (!module) return;
        Octavia* m = static_cast<Octavia*>(module);
        m->updateVoltages();
        if (++uiTimer >= 60) { uiTimer=0; m->updateCache(); }
        m->processSetQueue();
        m->processCableQueue();
        m->processTemporalDeckQueue();
        m->processAddQueue();
        m->processDeleteQueue();
        m->processMoveQueue();
        m->processBulkMoveQueue();
        m->processBypassQueue();
        m->processStateQueue();
        m->processSibylQueue();
        m->processPatchSaveQueue();
        m->processBulkParamQueue();
        m->processUndoQueue();
    }

    OctaviaWidget(Octavia* module) {
        setModule(module);
        const std::string panelPath = asset::plugin(pluginInstance,"res/Octavia.svg");
        setPanel(createPanel(panelPath));
        addChild(createWidget<CyanOrbScrew>(Vec(0, 0)));
        addChild(createWidget<CyanOrbScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        auto anchorPoint = [&](const char* id, const Vec& fallbackMm) {
            Vec result;
            if (!panel_svg::loadPointFromSvgMm(panelPath, id, &result)) result = fallbackMm;
            return result;
        };
        titleLabelMm = anchorPoint("TITLE_LABEL", titleLabelMm);
        portLabelMm = anchorPoint("PORT_LABEL", portLabelMm);
        portValueLabelMm = anchorPoint("PORT_VALUE_LABEL", portValueLabelMm);
        startLabelMm = anchorPoint("START_LABEL", startLabelMm);
        audioLabelLMm = anchorPoint("AUDIO_LABEL_L", audioLabelLMm);
        audioLabelRMm = anchorPoint("AUDIO_LABEL_R", audioLabelRMm);

        OctaviaStatusWidget* status = new OctaviaStatusWidget(module);
        math::Rect statusRectMm(Vec(0.74f, 13.5f), Vec(29.f, 36.f));
        panel_svg::loadRectFromSvgMm(panelPath, "OCTOPUS_STATUS", &statusRectMm);
        status->box.pos = mm2px(statusRectMm.pos);
        status->box.size = mm2px(statusRectMm.size);
        addChild(status);

        OctaviaMeterWidget* meter = new OctaviaMeterWidget(module);
        math::Rect meterRectMm(Vec(3.f, 72.f), Vec(24.48f, 29.f));
        panel_svg::loadRectFromSvgMm(panelPath, "LOUDNESS_METERS", &meterRectMm);
        meter->box.pos = mm2px(meterRectMm.pos);
        meter->box.size = mm2px(meterRectMm.size);
        addChild(meter);

        addParam(createParamCentered<SmallGoldButton>(
            mm2px(anchorPoint("START_PARAM", Vec(23.f, 66.f))), module, Octavia::START_PARAM));

        // Audio analysis inputs
        addInput(createInputCentered<Magitek2InputJack>(
            mm2px(anchorPoint("AUDIO_IN_L", Vec(8.f, 110.f))), module, Octavia::AUDIO_IN_L));
        addInput(createInputCentered<Magitek2InputJack>(
            mm2px(anchorPoint("AUDIO_IN_R", Vec(22.f, 110.f))), module, Octavia::AUDIO_IN_R));
    }

    void draw(const DrawArgs& args) override {
        ModuleWidget::draw(args);

        const float cx = mm2px(titleLabelMm).x;

        // ── Zone 1: Identity ──────────────────────────────────────────────────
        if (!APP || !APP->window || !APP->window->uiFont) return;
        nvgFontFaceId(args.vg, APP->window->uiFont->handle);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);

        nvgFontSize(args.vg,18.f); nvgFillColor(args.vg,WHITE);
        nvgText(args.vg, cx, mm2px(titleLabelMm).y, "Octavia", NULL);

        // ── Zone 2: Server controls ───────────────────────────────────────────
        nvgFontSize(args.vg,8.f);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg,DIM);
        nvgText(args.vg, mm2px(portLabelMm).x, mm2px(portLabelMm).y, "Port", NULL);
        nvgTextAlign(args.vg, NVG_ALIGN_RIGHT|NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg,WHITE);
        nvgText(args.vg, mm2px(portValueLabelMm).x, mm2px(portValueLabelMm).y, "7777", NULL);

        nvgTextAlign(args.vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg,WHITE);
        nvgText(args.vg, mm2px(startLabelMm).x, mm2px(startLabelMm).y, "Start", NULL);

        // ── Zone 3: Audio input labels ────────────────────────────────────────
        nvgFontSize(args.vg,8.f); nvgFillColor(args.vg,WHITE);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        nvgText(args.vg, mm2px(audioLabelLMm).x, mm2px(audioLabelLMm).y, "L", NULL);
        nvgText(args.vg, mm2px(audioLabelRMm).x, mm2px(audioLabelRMm).y, "R", NULL);
    }
};

Model* modelOctavia = createModel<Octavia, OctaviaWidget>("Octavia");
