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
#include "OctaviaConsoleMailbox.hpp"
#include "OctaviaCableValidation.hpp"
#include "OctaviaActionValidation.hpp"
#include "OctaviaJobControl.hpp"
#include "OctaviaObservation.hpp"
#include "OctaviaMeasurement.hpp"
#include "OctaviaAnalysis.hpp"
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
#include <limits>
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

// ── Config from environment ───────────────────────────────────────────────────
static int octaviaPort() {
    const char* e = getenv("OCTAVIA_PORT");
    if (e) { int p = atoi(e); if (p > 0 && p < 65536) return p; }
    return 34570;
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

// Legacy analysis window size. Samples now come from the shared six-channel,
// frame-addressed observation history rather than independent ring heads.
static const int AUDIO_BUF = 4096;  // power of 2, ~93ms @ 44100 Hz
static const int MASTER_METER_BLOCKS = 32; // 3.2 s of live 100 ms meter blocks

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
    std::atomic<bool> done{false}, cancelled{false}; bool success = false; std::string error;
};
struct CableJob {
    enum Type { ADD, REMOVE, REMOVE_OUTPUT };
    Type type;
    int64_t outModId=-1, outPortId=-1, inModId=-1, inPortId=-1;
    std::string color;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct AddModuleJob {
    std::string pluginSlug, modelSlug;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; int64_t newId=-1; std::string error;
};
struct DeleteModuleJob {
    int64_t moduleId;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct MoveModuleJob {
    int64_t moduleId; float hp; int row = 0; bool hasRow = false;
    float resolvedHp = 0.f; int resolvedRow = 0;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct BulkMoveJob {
    struct Change { int64_t moduleId; float hp; int row; };
    struct Result { int64_t moduleId; float hp; int row; };
    std::vector<Change> changes;
    std::vector<Result> results;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct BypassJob {
    int64_t moduleId; bool bypassed;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct ModuleStateJob {
    enum Type { GET, SET };
    Type type; int64_t moduleId;
    std::string stateJson; // output for GET, input for SET
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct SibylJob {
    SibylControl::Operation operation;
    int64_t moduleId = -1;
    std::string requestJson = "{}";
    std::string responseJson;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct PatchSaveJob {
    std::string savedPath;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct TemporalDeckJob {
    enum Type { LOAD, PLAY, STOP_REWIND, SEEK, SET_LOOP, STATUS } type;
    int64_t moduleId = -1; std::string path; float position = 0.f; bool enabled = false;
    bool loaded = false, playing = false, loop = false;
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct BulkParamJob {
    struct Change { int64_t moduleId; int paramId; float value; };
    std::vector<Change> changes;
    std::vector<int> failed;              // indices that could not be applied
    std::atomic<bool> done{false}, cancelled{false}; bool success=false; std::string error;
};
struct UndoJob {
    std::atomic<bool> done{false}, cancelled{false}; bool success=false;
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
    enum InputId  {
        MASTER_L_INPUT = 0,
        MASTER_R_INPUT = 1,
        MONITOR_A_INPUT,
        MONITOR_B_INPUT,
        MONITOR_C_INPUT,
        MONITOR_D_INPUT,
        INPUTS_LEN
    };
    enum OutputId { OUTPUTS_LEN };
    enum LightId  {
        STATUS_R_LIGHT, STATUS_G_LIGHT, STATUS_B_LIGHT,
        READ_ACTIVITY_LIGHT, WRITE_ACTIVITY_LIGHT,
        MONITOR_A_LIGHT, MONITOR_B_LIGHT, MONITOR_C_LIGHT, MONITOR_D_LIGHT,
        LIGHTS_LEN
    };

    // Rack patch cables and light state persist numeric IDs. Preserve the original
    // Master and activity IDs; append future observation ports and lights only.
    static_assert(MASTER_L_INPUT == 0 && MASTER_R_INPUT == 1,
        "Octavia Master input IDs must remain patch-compatible");
    static_assert(MONITOR_A_INPUT == 2 && MONITOR_B_INPUT == 3 &&
        MONITOR_C_INPUT == 4 && MONITOR_D_INPUT == 5 && INPUTS_LEN == 6,
        "Octavia monitor input IDs are append-only");
    static_assert(STATUS_R_LIGHT == 0 && STATUS_G_LIGHT == 1 && STATUS_B_LIGHT == 2 &&
        READ_ACTIVITY_LIGHT == 3 && WRITE_ACTIVITY_LIGHT == 4,
        "Octavia legacy light IDs must remain patch-compatible");
    static_assert(MONITOR_A_LIGHT == 5 && MONITOR_B_LIGHT == 6 &&
        MONITOR_C_LIGHT == 7 && MONITOR_D_LIGHT == 8 && LIGHTS_LEN == 9,
        "Octavia monitor light IDs are append-only");

    std::atomic<bool> serverRunning{false};
    std::atomic<uint64_t> readActivityGeneration{0};
    std::atomic<uint64_t> writeActivityGeneration{0};
    uint64_t seenReadActivityGeneration = 0;
    uint64_t seenWriteActivityGeneration = 0;
    float readActivityEnvelope = 0.f;
    float writeActivityEnvelope = 0.f;
    httplib::Server   svr;
    std::thread       serverThread;

    // Audio observation — one coherent Rack-frame timeline for Master L/R and A-D.
    octavia::ObservationHistory observationHistory;
    octavia::ObservationSnapshotPool snapshotPool;
    octavia::AnalysisEngine analysisEngine;
    std::atomic<float> sampleRate{44100.f};  // set in process(), read by HTTP
    std::atomic<bool> audioInputConnected[2];
    std::array<uint64_t, 4> seenMonitorSnapshotGeneration{{0, 0, 0, 0}};
    std::array<float, 4> monitorActivityEnvelope{{0.f, 0.f, 0.f, 0.f}};
    std::array<std::atomic<uint32_t>, 4> monitorAnalysisCount;

    // Feedback needs a trend across independent requests. Keeping only the
    // strongest candidate per channel gives useful confirmation without
    // retaining spectra or flooding MCP responses.
    struct FeedbackTrack {
        float hz = 0.f, db = -140.f;
        int risingObservations = 0;
        std::chrono::steady_clock::time_point seen;
    } feedbackTrack[2];
    std::mutex feedbackMtx;

    // ── Minimal always-on Master meter ────────────────────────────────────────
    // Only live K-weighted blocks and recent peaks run continuously. Integrated
    // loudness, raw RMS, clipping, and stereo metrics live in masterMeasurement
    // and incur per-sample work only while explicitly armed.
    // K-weighting: ITU BS.1770 biquads, recalculated for Rack's sample rate
    // from the standard's analog prototypes.
    struct LoudnessMeter {
        struct Coeffs { double b0=1., b1=0., b2=0., a1=0., a2=0.; } shelf, highPass;
        // audio-thread working state
        double z[2][4] = {};                 // biquad states per ch: hs1,hs2,hp1,hp2
        double meterBlockPeak[2] = {};
        double blockKSum[2] = {};
        int blockFrames = 0, blockTarget = 4800;
        float filterSampleRate = 0.f;
        std::array<std::array<std::atomic<float>, MASTER_METER_BLOCKS>, 2> blocks;
        std::atomic<uint64_t> blockTotal{0};
        std::atomic<float> meterPeak[2];
        LoudnessMeter() {
            for (int j = 0; j < 2; ++j) {
                meterPeak[j].store(0.f, std::memory_order_relaxed);
                for (int i = 0; i < MASTER_METER_BLOCKS; ++i)
                    blocks[j][i].store(0.f, std::memory_order_relaxed);
            }
        }
    } lm;
    octavia::MasterMeasurement masterMeasurement;

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

    Octavia() : snapshotPool(&observationHistory) {
        for (auto& count : monitorAnalysisCount)
            count.store(0, std::memory_order_relaxed);
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configButton(START_PARAM, "Start Octavia Server");
        configInput(MASTER_L_INPUT, "Master L");
        configInput(MASTER_R_INPUT, "Master R");
        configInput(MONITOR_A_INPUT, "Monitor A");
        configInput(MONITOR_B_INPUT, "Monitor B");
        configInput(MONITOR_C_INPUT, "Monitor C");
        configInput(MONITOR_D_INPUT, "Monitor D");
        configLight(READ_ACTIVITY_LIGHT, "HTTP read activity");
        configLight(WRITE_ACTIVITY_LIGHT, "HTTP write activity");
        configLight(MONITOR_A_LIGHT, "Monitor A attention");
        configLight(MONITOR_B_LIGHT, "Monitor B attention");
        configLight(MONITOR_C_LIGHT, "Monitor C attention");
        configLight(MONITOR_D_LIGHT, "Monitor D attention");
        setupRoutes();
    }
    ~Octavia() { stopServer(); }

    // ── Audio thread: sample inputs into ring buffers ─────────────────────────
    void process(const ProcessArgs& args) override {
        if (startTrig.process(params[START_PARAM].getValue() > 0.f)) startServer();

        sampleRate.store(args.sampleRate, std::memory_order_relaxed);

        // Publish all physical observation points as one coherent Rack engine frame.
        // Channel 0 is intentionally the v1 observation boundary for polyphonic ports.
        float ch[2];
        std::array<float, octavia::OBSERVATION_CHANNELS> observationVolts{{}};
        std::array<uint8_t, octavia::OBSERVATION_CHANNELS> observationChannels{{}};
        uint8_t connectedMask = 0;
        for (size_t channel = 0; channel < octavia::OBSERVATION_CHANNELS; ++channel) {
            const bool connected = inputs[channel].isConnected();
            const int channels = connected ? inputs[channel].getChannels() : 0;
            observationChannels[channel] = static_cast<uint8_t>(
                std::max(0, std::min(channels, 255)));
            if (connected) {
                connectedMask |= static_cast<uint8_t>(1u << channel);
                observationVolts[channel] = inputs[channel].getVoltage(0);
            }
            if (channel < 2) {
                audioInputConnected[channel].store(connected, std::memory_order_relaxed);
                ch[channel] = observationVolts[channel];
            }
        }
        observationHistory.publish(static_cast<uint64_t>(args.frame), args.sampleRate,
            observationVolts, connectedMask, observationChannels);

        // Minimal live Master meter (normalized: 5V = 0 dBFS).
        if (lm.filterSampleRate != args.sampleRate) {
            lm.shelf = makeHighShelf(args.sampleRate, 1681.974450955533f, 3.999843853973347f);
            lm.highPass = makeHighPass(args.sampleRate, 38.13547087602444f, .5003270373238773f);
            lm.filterSampleRate = args.sampleRate;
            lm.blockTarget = std::max(1, (int)roundf(args.sampleRate * .1f));
            lm.blockFrames = 0;
            lm.blockTotal.store(0, std::memory_order_relaxed);
            memset(lm.z, 0, sizeof(lm.z));
            for (int j = 0; j < 2; ++j) {
                lm.blockKSum[j] = 0.0;
                lm.meterBlockPeak[j] = 0.0;
                lm.meterPeak[j].store(0.f, std::memory_order_relaxed);
            }
        }
        std::array<double, 2> normalized{{}};
        std::array<double, 2> kPower{{}};
        for (int j = 0; j < 2; j++) {
            const double in = ch[j] * 0.2;
            normalized[j] = in;
            lm.meterBlockPeak[j] = std::max(lm.meterBlockPeak[j], std::fabs(in));
            const double y1 = lm.shelf.b0*in + lm.z[j][0];
            lm.z[j][0] = lm.shelf.b1*in - lm.shelf.a1*y1 + lm.z[j][1];
            lm.z[j][1] = lm.shelf.b2*in - lm.shelf.a2*y1;
            const double y2 = lm.highPass.b0*y1 + lm.z[j][2];
            lm.z[j][2] = lm.highPass.b1*y1 - lm.highPass.a1*y2 + lm.z[j][3];
            lm.z[j][3] = lm.highPass.b2*y1 - lm.highPass.a2*y2;
            kPower[j] = y2 * y2;
            lm.blockKSum[j] += kPower[j];
        }
        if (++lm.blockFrames >= lm.blockTarget) {
            const uint64_t block = lm.blockTotal.load(std::memory_order_relaxed);
            for (int j = 0; j < 2; ++j) {
                lm.blocks[j][block % MASTER_METER_BLOCKS].store(
                    (float)(lm.blockKSum[j] / lm.blockFrames), std::memory_order_relaxed);
                lm.meterPeak[j].store((float)lm.meterBlockPeak[j], std::memory_order_relaxed);
                lm.blockKSum[j] = 0.0;
                lm.meterBlockPeak[j] = 0.0;
            }
            lm.blockTotal.store(block + 1, std::memory_order_release);
            lm.blockFrames = 0;
        }
        masterMeasurement.process(static_cast<uint64_t>(args.frame), args.sampleRate,
            normalized, kPower);

        bool on = serverRunning;
        lights[STATUS_R_LIGHT].setBrightness(on ? 0.f  : 0.8f);
        lights[STATUS_G_LIGHT].setBrightness(on ? 0.8f : 0.f);
        lights[STATUS_B_LIGHT].setBrightness(0.f);

        const uint64_t readGeneration = readActivityGeneration.load(std::memory_order_relaxed);
        const uint64_t writeGeneration = writeActivityGeneration.load(std::memory_order_relaxed);
        if (readGeneration != seenReadActivityGeneration) {
            seenReadActivityGeneration = readGeneration;
            readActivityEnvelope = 1.f;
        }
        if (writeGeneration != seenWriteActivityGeneration) {
            seenWriteActivityGeneration = writeGeneration;
            writeActivityEnvelope = 1.f;
        }
        constexpr float activityDecaySeconds = 0.18f;
        const float activityDecay = args.sampleTime / activityDecaySeconds;
        readActivityEnvelope = std::max(0.f, readActivityEnvelope - activityDecay);
        writeActivityEnvelope = std::max(0.f, writeActivityEnvelope - activityDecay);
        lights[READ_ACTIVITY_LIGHT].setBrightness(readActivityEnvelope);
        lights[WRITE_ACTIVITY_LIGHT].setBrightness(writeActivityEnvelope);
        constexpr float monitorActivityDecaySeconds = 0.22f;
        const float monitorActivityDecay = args.sampleTime / monitorActivityDecaySeconds;
        for (int monitor = 0; monitor < 4; ++monitor) {
            const octavia::ObserveChannel channel = static_cast<octavia::ObserveChannel>(monitor + 2);
            const uint64_t generation = observationHistory.snapshotGeneration(channel);
            if (generation != seenMonitorSnapshotGeneration[monitor]) {
                seenMonitorSnapshotGeneration[monitor] = generation;
                monitorActivityEnvelope[monitor] = 1.f;
            }
            monitorActivityEnvelope[monitor] = std::max(
                0.f, monitorActivityEnvelope[monitor] - monitorActivityDecay);
            const bool connected = inputs[MONITOR_A_INPUT + monitor].isConnected();
            const float idle = connected ? 0.12f : 0.f;
            const bool analyzing = monitorAnalysisCount[monitor].load(std::memory_order_relaxed) > 0;
            const float active = analyzing
                ? 0.82f + 0.18f * std::sin(2.f * float(M_PI) * float(args.frame) * args.sampleTime * 2.f)
                : 0.f;
            lights[MONITOR_A_LIGHT + monitor].setBrightness(
                std::max(active, std::max(idle, monitorActivityEnvelope[monitor])));
        }
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

    template <typename T>
    static bool beginJob(const std::shared_ptr<T>& job) {
        return octavia::beginQueuedJob(job);
    }

    void processSetQueue() {
        std::shared_ptr<SetParamJob> job;
        { std::unique_lock<std::mutex> lk(setQueueMtx); if (!setQueue.empty()) { job=setQueue.front(); setQueue.pop(); } }
        if (!beginJob(job)) return;
        engine::Module* m = APP->engine->getModule(job->moduleId);
        if (!m) job->error="module not found: "+std::to_string(job->moduleId);
        else if (job->paramId<0 || job->paramId>=(int)m->params.size()) {
            job->error="parameter "+std::to_string(job->paramId)+" out of range for "+moduleIdentity(m)
                +" module "+std::to_string(job->moduleId)+"; valid IDs are "
                +(m->params.empty()?std::string("none"):std::string("0-")+std::to_string(m->params.size()-1));
        } else if (!(job->error = octavia::validateParameterValue(job->value)).empty()) {
            job->error += " on module "+std::to_string(job->moduleId)+" parameter "+std::to_string(job->paramId);
        } else {
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

    static std::string moduleIdentity(engine::Module* module) {
        if (!module || !module->model) return {};
        const std::string pluginSlug = module->model->plugin ? module->model->plugin->slug : "unknown";
        return pluginSlug + ":" + module->model->slug;
    }

    PortWidget* validateLiveCableEndpoint(const char* direction, int64_t moduleId,
                                           int64_t portId, std::string* error) {
        engine::Module* module = APP->engine->getModule(moduleId);
        ModuleWidget* widget = APP->scene->rack->getModule(moduleId);
        const bool output = std::string(direction) == "output";
        const std::size_t portCount = module ? (output ? module->outputs.size() : module->inputs.size()) : 0;
        octavia::CableEndpointValidation initial{
            direction, moduleId, moduleIdentity(module), portId, portCount,
            module != nullptr, widget != nullptr, true};
        std::string validationError = octavia::validateCableEndpoint(initial);
        if (!validationError.empty()) {
            if (error) *error = std::move(validationError);
            return nullptr;
        }
        PortWidget* port = output ? widget->getOutput((int)portId) : widget->getInput((int)portId);
        if (!port) {
            initial.portWidgetExists = false;
            if (error) *error = octavia::validateCableEndpoint(initial);
        }
        return port;
    }

    bool addCableValidated(int64_t outModuleId, int64_t outPortId,
                           int64_t inModuleId, int64_t inPortId,
                           NVGcolor color, std::string* error) {
        PortWidget* outPort = validateLiveCableEndpoint("output", outModuleId, outPortId, error);
        if (!outPort) return false;
        PortWidget* inPort = validateLiveCableEndpoint("input", inModuleId, inPortId, error);
        if (!inPort) return false;
        engine::Module* src = APP->engine->getModule(outModuleId);
        engine::Module* dst = APP->engine->getModule(inModuleId);
        engine::Cable* cable = new engine::Cable;
        cable->outputModule = src;
        cable->outputId = (int)outPortId;
        cable->inputModule = dst;
        cable->inputId = (int)inPortId;
        CableWidget* cableWidget = new CableWidget;
        cableWidget->color = color;
        cableWidget->setCable(cable);
        if (!cableWidget->isComplete()) {
            engine::Cable* released = cableWidget->releaseCable();
            delete cableWidget;
            delete released;
            if (error) *error = "Rack rejected a cable whose validated widgets became incomplete";
            return false;
        }
        APP->engine->addCable(cable);
        APP->scene->rack->addCable(cableWidget);
        return true;
    }

    void processCableQueue() {
        std::shared_ptr<CableJob> job;
        { std::unique_lock<std::mutex> lk(cableQueueMtx); if (!cableQueue.empty()) { job=cableQueue.front(); cableQueue.pop(); } }
        if (!beginJob(job)) return;
        if (job->type==CableJob::ADD) {
            job->success = addCableValidated(job->outModId, job->outPortId,
                job->inModId, job->inPortId, parseColorString(job->color), &job->error);
            if (job->success) {
                UndoAction a; a.type=UndoAction::CABLE_CLEAR;
                a.inModId=job->inModId; a.inPortId=(int)job->inPortId;
                a.label="connect cable -> module "+std::to_string(job->inModId)+" port "+std::to_string(job->inPortId);
                pushUndo(std::move(a));
            }
        } else if (job->type==CableJob::REMOVE) {
            PortWidget* inPort = validateLiveCableEndpoint("input", job->inModId, job->inPortId, &job->error);
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
        } else { // REMOVE_OUTPUT — disconnect all cables from one output port
            PortWidget* outPort = validateLiveCableEndpoint("output", job->outModId, job->outPortId, &job->error);
            if (!outPort) { job->done=true; return; }
            (void)outPort;
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
        if (!beginJob(job)) return;
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
        if (!beginJob(job)) return;
        plugin::Model* model=plugin::getModel(job->pluginSlug,job->modelSlug);
        if (!model) { job->error="model not found: "+job->pluginSlug+"/"+job->modelSlug; }
        else {
            engine::Module* module = nullptr;
            ModuleWidget* widget = nullptr;
            bool engineAdded = false;
            bool widgetAdded = false;
            try {
                module=model->createModule();
                if (!module) throw std::runtime_error("model returned no module instance");
                APP->engine->addModule(module); engineAdded = true;
                widget=model->createModuleWidget(module);
                if (!widget) throw std::runtime_error("model returned no module widget");
                APP->scene->rack->addModule(widget); widgetAdded = true;
                float sumX=0.f,sumY=0.f; int cnt=0;
                for (int64_t oid : APP->engine->getModuleIds()) {
                    if (oid==module->id) continue;
                    ModuleWidget* omw=APP->scene->rack->getModule(oid);
                    if (omw) { sumX+=omw->box.pos.x+omw->box.size.x*0.5f; sumY+=omw->box.pos.y+omw->box.size.y*0.5f; cnt++; }
                }
                math::Vec target=cnt>0?math::Vec(sumX/cnt,sumY/cnt):math::Vec(0.f,0.f);
                APP->scene->rack->setModulePosNearest(widget,target);
                job->newId=module->id; job->success=true;
                UndoAction a; a.type=UndoAction::DELETE_ADDED; a.moduleId=module->id;
                a.label="add module "+job->pluginSlug+"/"+job->modelSlug;
                pushUndo(std::move(a));
            } catch (const std::exception& e) {
                job->error = std::string("could not add ") + job->pluginSlug + "/" + job->modelSlug + ": " + e.what();
                if (widgetAdded) APP->scene->rack->removeModule(widget);
                if (engineAdded) APP->engine->removeModule(module);
                if (widget) delete widget;
                if (module) delete module;
            } catch (...) {
                job->error = "could not add " + job->pluginSlug + "/" + job->modelSlug + ": unknown exception";
                if (widgetAdded) APP->scene->rack->removeModule(widget);
                if (engineAdded) APP->engine->removeModule(module);
                if (widget) delete widget;
                if (module) delete module;
            }
        }
        job->done=true;
    }

    void processDeleteQueue() {
        std::shared_ptr<DeleteModuleJob> job;
        { std::unique_lock<std::mutex> lk(deleteQueueMtx); if (!deleteQueue.empty()) { job=deleteQueue.front(); deleteQueue.pop(); } }
        if (!beginJob(job)) return;
        if (job->moduleId == id) {
            job->error="refusing to delete the Octavia module serving this request";
            job->done=true;
            return;
        }
        ModuleWidget* mw = APP->scene->rack->getModule(job->moduleId);
        if (!mw) { job->error="module not found"; job->done=true; return; }
        engine::Module* m = mw->module;
        Octavia* targetOctavia = dynamic_cast<Octavia*>(m);
        if (targetOctavia && targetOctavia->serverRunning.load(std::memory_order_acquire)) {
            job->error="refusing to delete an Octavia module while its server is running";
            job->done=true; return;
        }
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
        if (!beginJob(job)) return;
        UndoAction a; a.type=UndoAction::PARAMS;
        if (job->changes.empty()) {
            job->error="changes must contain at least one parameter update";
            job->done=true;
            return;
        }
        if (job->changes.size() > octavia::kMaxBulkChanges) {
            job->error="parameter batch exceeds 1024 changes";
            job->done=true;
            return;
        }
        for (size_t i = 0; i < job->changes.size(); ++i) {
            auto& ch = job->changes[i];
            engine::Module* m = APP->engine->getModule(ch.moduleId);
            if (!m) job->error="change "+std::to_string(i)+": module not found: "+std::to_string(ch.moduleId);
            else if (ch.paramId < 0 || ch.paramId >= (int)m->params.size())
                job->error="change "+std::to_string(i)+": parameter "+std::to_string(ch.paramId)
                    +" out of range for "+moduleIdentity(m)+" module "+std::to_string(ch.moduleId);
            else if (!octavia::validateParameterValue(ch.value).empty())
                job->error="change "+std::to_string(i)+": parameter value must be finite";
            if (!job->error.empty()) { job->done=true; return; }
        }
        for (size_t i = 0; i < job->changes.size(); i++) {
            auto& ch = job->changes[i];
            engine::Module* m = APP->engine->getModule(ch.moduleId);
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
        if (!beginJob(job)) return;
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
                    std::string cableError;
                    if (!addCableValidated(cb.outModId, cb.outPortId, cb.inModId, cb.inPortId,
                                            cb.color, &cableError)) {
                        job->success=false;
                        job->error = "could not restore cable: " + cableError;
                        break;
                    }
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
                    Octavia* targetOctavia = dynamic_cast<Octavia*>(m);
                    if (targetOctavia && targetOctavia->serverRunning.load(std::memory_order_acquire)) {
                        job->error="refusing to undo-add an Octavia module while its server is running";
                        break;
                    }
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
        if (!beginJob(job)) return;
        ModuleWidget* mw = APP->scene->rack->getModule(job->moduleId);
        if (!mw) { job->error="module not found"; job->done=true; return; }
        const int targetRow = job->hasRow ? job->row : (int)std::lround(mw->box.pos.y / RACK_GRID_HEIGHT);
        job->error = octavia::validateRackPosition(job->hp, targetRow);
        if (!job->error.empty()) { job->done=true; return; }
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
        if (!beginJob(job)) return;

        std::unordered_set<int64_t> moving;
        std::vector<std::pair<ModuleWidget*, math::Vec>> targets;
        for (auto& ch : job->changes) {
            job->error = octavia::validateRackPosition(ch.hp, ch.row);
            if (!job->error.empty()) { job->done=true; return; }
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
        if (!beginJob(job)) return;
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
        if (!beginJob(job)) return;
        engine::Module* m = APP->engine->getModule(job->moduleId);
        if (!m) { job->error="module not found"; job->done=true; return; }
        try {
          if (job->type == ModuleStateJob::GET) {
            json_t* rootJ = m->toJson();
            if (!rootJ) { job->error="toJson() returned null"; job->done=true; return; }
            char* str = json_dumps(rootJ, JSON_COMPACT);
            job->stateJson = str ? std::string(str) : "{}";
            if (str) free(str);
            json_decref(rootJ);
          } else {
            if (job->stateJson.size() > octavia::kMaxModuleStateBytes) {
                job->error="module state exceeds the 1 MiB safety limit"; job->done=true; return;
            }
            json_error_t jerr;
            json_t* rootJ = json_loads(job->stateJson.c_str(), 0, &jerr);
            if (!rootJ) { job->error = std::string("JSON parse: ") + jerr.text; job->done=true; return; }
            if (!json_is_object(rootJ)) {
                json_decref(rootJ); job->error="module state root must be a JSON object"; job->done=true; return;
            }
            json_t* pluginJ = json_object_get(rootJ, "plugin");
            json_t* modelJ = json_object_get(rootJ, "model");
            if (!json_is_string(pluginJ) || !json_is_string(modelJ)) {
                json_decref(rootJ);
                job->error="module state must preserve the Rack plugin and model identity fields";
                job->done=true; return;
            }
            const std::string expectedPlugin = m->model && m->model->plugin ? m->model->plugin->slug : "";
            const std::string expectedModel = m->model ? m->model->slug : "";
            if (expectedPlugin != json_string_value(pluginJ) || expectedModel != json_string_value(modelJ)) {
                job->error="module state identity mismatch: expected "+expectedPlugin+":"+expectedModel
                    +", received "+json_string_value(pluginJ)+":"+json_string_value(modelJ);
                json_decref(rootJ); job->done=true; return;
            }
            UndoAction a; a.type=UndoAction::STATE; a.moduleId=job->moduleId;
            { json_t* oldJ = m->toJson();
              if (oldJ) { char* str=json_dumps(oldJ,JSON_COMPACT); if (str){ a.oldState=str; free(str);} json_decref(oldJ); } }
            a.label="restore state on module "+std::to_string(job->moduleId);
            m->fromJson(rootJ);
            if (!a.oldState.empty()) pushUndo(std::move(a));
            json_decref(rootJ);
          }
        } catch (const std::exception& e) {
            job->error=std::string("module state operation failed: ")+e.what(); job->done=true; return;
        } catch (...) {
            job->error="module state operation failed with an unknown exception"; job->done=true; return;
        }
        job->success=true; job->done=true;
    }

    void processSibylQueue() {
        std::shared_ptr<SibylJob> job;
        { std::unique_lock<std::mutex> lk(sibylQueueMtx); if (!sibylQueue.empty()) { job=sibylQueue.front(); sibylQueue.pop(); } }
        if (!beginJob(job)) return;
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
        if (!beginJob(job)) return;
        if (APP->patch->path.empty()) {
            job->error = "patch has no file path (File > Save As in VCV Rack first)";
        } else {
            try {
                job->savedPath = APP->patch->path;
                APP->patch->save(APP->patch->path);
                job->success = true;
            } catch (const std::exception& e) {
                job->error = std::string("patch save failed: ") + e.what();
            } catch (...) {
                job->error = "patch save failed with an unknown exception";
            }
        }
        job->done = true;
    }

    template<typename T>
    static bool waitDone(std::shared_ptr<T>& job, int ms=500) {
        auto dl=std::chrono::steady_clock::now()+std::chrono::milliseconds(ms);
        while (!job->done.load() && std::chrono::steady_clock::now()<dl)
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        if (!job->done.load()) octavia::cancelTimedOutJob(job);
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
        if (requestJson.size() > octavia::kMaxSemanticRequestBytes) {
            res.set_content("{\"error\":\"Sibyl request exceeds the 1 MiB safety limit\"}","application/json");
            return;
        }
        auto job=std::make_shared<SibylJob>();
        job->moduleId=moduleId; job->operation=operation; job->requestJson=requestJson;
        { std::unique_lock<std::mutex> lk(sibylQueueMtx); sibylQueue.push(job); }
        if (!waitDone(job, 5000)) res.set_content("{\"error\":\"timeout\"}","application/json");
        else if (!job->success && !job->responseJson.empty()) res.set_content(job->responseJson,"application/json");
        else if (!job->success) res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        else res.set_content(job->responseJson,"application/json");
    }

    // ── HTTP routes ───────────────────────────────────────────────────────────
    static std::string snapshotJson(const octavia::ObservationSnapshot& snapshot) {
        const octavia::FrozenObservation& frozen = snapshot.observation;
        std::string body = "{";
        body += jStr("snapshotId") + ":" + std::to_string(frozen.id) + ",";
        body += jStr("state") + ":" + jStr(octavia::snapshotStateName(snapshot.state)) + ",";
        body += jStr("triggerFrame") + ":" + std::to_string(frozen.triggerFrame) + ",";
        body += jStr("startFrame") + ":" + std::to_string(frozen.startFrame) + ",";
        body += jStr("endFrame") + ":" + std::to_string(frozen.endFrame) + ",";
        body += jStr("preFrames") + ":" + std::to_string(frozen.preFrames) + ",";
        body += jStr("postFrames") + ":" + std::to_string(frozen.postFrames) + ",";
        body += jStr("sampleRate") + ":" + jNum(frozen.sampleRate) + ",";
        const double frames = frozen.endFrame >= frozen.startFrame
            ? static_cast<double>(frozen.endFrame - frozen.startFrame + 1) : 0.0;
        body += jStr("durationMs") + ":" + jNum(frozen.sampleRate > 0.f
            ? static_cast<float>(1000.0 * frames / frozen.sampleRate) : 0.f) + ",";
        body += jStr("label") + ":" + jStr(frozen.label) + ",";
        body += jStr("requestedMonitors") + ":[";
        bool first = true;
        for (size_t channel = 0; channel < octavia::OBSERVATION_CHANNELS; ++channel) {
            if (!(frozen.requestedMask & (1u << channel))) continue;
            if (!first) body += ",";
            first = false;
            body += jStr(octavia::observeChannelName(
                static_cast<octavia::ObserveChannel>(channel)));
        }
        body += "],";
        body += jStr("allConnectedMask") + ":" + std::to_string(frozen.allConnectedMask) + ",";
        body += jStr("anyConnectedMask") + ":" + std::to_string(frozen.anyConnectedMask);
        if (!snapshot.error.empty()) body += "," + jStr("error") + ":" + jStr(snapshot.error);
        body += "}";
        return body;
    }

    static bool parseAnalysisGroup(json_t* value, octavia::AnalysisGroup* group,
            std::string* error) {
        if (!json_is_object(value) || !group) {
            if (error) *error = "analysis group must be an object";
            return false;
        }
        json_t* channels = json_object_get(value, "channels");
        json_t* stereo = json_object_get(value, "stereo");
        if (channels && stereo) {
            if (error) *error = "analysis group cannot contain both channels and stereo";
            return false;
        }
        if (channels) {
            if (!json_is_array(channels) || json_array_size(channels) != 1
                    || !json_is_string(json_array_get(channels, 0))
                    || !octavia::parseObserveChannel(
                        json_string_value(json_array_get(channels, 0)), &group->first)) {
                if (error) *error = "channels must contain exactly one named monitor";
                return false;
            }
            group->stereo = false;
            return true;
        }
        if (!json_is_object(stereo)) {
            if (error) *error = "analysis group requires channels or stereo";
            return false;
        }
        json_t* left = json_object_get(stereo, "left");
        json_t* right = json_object_get(stereo, "right");
        if (!json_is_string(left) || !json_is_string(right)
                || !octavia::parseObserveChannel(json_string_value(left), &group->first)
                || !octavia::parseObserveChannel(json_string_value(right), &group->second)
                || group->first == group->second) {
            if (error) *error = "stereo requires distinct named left and right monitors";
            return false;
        }
        group->stereo = true;
        return true;
    }

    static std::string channelAnalysisJson(const octavia::ChannelAnalysis& analysis,
            bool includeSpectrum) {
        std::string body = "{";
        body += jStr("channel") + ":" + jStr(octavia::observeChannelName(analysis.channel)) + ",";
        body += jStr("connected") + ":" + (analysis.connected ? "true" : "false") + ",";
        body += jStr("frames") + ":" + std::to_string(analysis.frames) + ",";
        body += jStr("rms") + ":" + jNum(analysis.rms) + ",";
        body += jStr("rmsDb") + ":" + jNum(analysis.rmsDb) + ",";
        body += jStr("peak") + ":" + jNum(analysis.peak) + ",";
        body += jStr("peakDb") + ":" + jNum(analysis.peakDb) + ",";
        body += jStr("crestDb") + ":" + jNum(analysis.crestDb) + ",";
        body += jStr("dcOffset") + ":" + jNum(analysis.dcOffset) + ",";
        body += jStr("clippedSamples") + ":" + std::to_string(analysis.clippedSamples) + ",";
        body += jStr("noiseFloorDb") + ":" + jNum(analysis.noiseFloorDb) + ",";
        body += jStr("temporalSeparationFrames") + ":" +
            std::to_string(analysis.temporalSeparationFrames) + ",";
        body += jStr("issues") + ":[";
        for (size_t i = 0; i < analysis.issues.size(); ++i) {
            if (i) body += ",";
            body += jStr(analysis.issues[i]);
        }
        body += "]," + jStr("hum") + ":{";
        body += jStr("detected50") + ":" + (analysis.hum.detected50 ? "true" : "false") + ",";
        body += jStr("detected60") + ":" + (analysis.hum.detected60 ? "true" : "false") + ",";
        body += jStr("series50Db") + ":[";
        for (size_t i = 0; i < 4; ++i) { if (i) body += ","; body += jNum(analysis.hum.series50Db[i]); }
        body += "]," + jStr("series60Db") + ":[";
        for (size_t i = 0; i < 4; ++i) { if (i) body += ","; body += jNum(analysis.hum.series60Db[i]); }
        body += "]}," + jStr("feedback") + ":";
        if (analysis.feedbackSuspect) {
            body += "{" + jStr("hz") + ":" + jNum(analysis.feedbackHz) + ","
                + jStr("riseDb") + ":" + jNum(analysis.feedbackRiseDb) + "},";
        } else body += "null,";
        body += jStr("resonances") + ":[";
        for (size_t i = 0; i < analysis.resonances.size(); ++i) {
            if (i) body += ",";
            const auto& resonance = analysis.resonances[i];
            body += "{" + jStr("hz") + ":" + jNum(resonance.hz) + ","
                + jStr("db") + ":" + jNum(resonance.db) + ","
                + jStr("prominenceDb") + ":" + jNum(resonance.prominenceDb) + ","
                + jStr("stable") + ":" + (resonance.stable ? "true" : "false") + "}";
        }
        body += "],";
        body += jStr("bandsDb") + ":{";
        const auto& names = octavia::analysisBandNames();
        for (size_t i = 0; i < names.size(); ++i) {
            if (i) body += ",";
            body += jStr(names[i]) + ":" + jNum(analysis.bandsDb[i]);
        }
        body += "}";
        if (includeSpectrum) {
            body += "," + jStr("spectrum") + ":[";
            for (size_t i = 0; i < analysis.spectrum.size(); ++i) {
                if (i) body += ",";
                body += "{" + jStr("hz") + ":" + jNum(analysis.spectrum[i].hz)
                    + "," + jStr("db") + ":" + jNum(analysis.spectrum[i].db) + "}";
            }
            body += "]";
        }
        return body + "}";
    }

    static std::string groupAnalysisJson(const octavia::GroupAnalysis& analysis,
            bool includeSpectrum) {
        if (!analysis.group.stereo)
            return "{" + jStr("type") + ":" + jStr("mono") + "," +
                jStr("analysis") + ":" + channelAnalysisJson(analysis.mono, includeSpectrum) + "}";
        const octavia::StereoAnalysis& stereo = analysis.stereo;
        return "{" + jStr("type") + ":" + jStr("stereo") + ","
            + jStr("left") + ":" + channelAnalysisJson(stereo.leftAnalysis, includeSpectrum) + ","
            + jStr("right") + ":" + channelAnalysisJson(stereo.rightAnalysis, includeSpectrum) + ","
            + jStr("balanceDb") + ":" + jNum(stereo.balanceDb) + ","
            + jStr("correlation") + ":" + jNum(stereo.correlation) + ","
            + jStr("midRms") + ":" + jNum(stereo.midRms) + ","
            + jStr("sideRms") + ":" + jNum(stereo.sideRms) + ","
            + jStr("sideToMidDb") + ":" + jNum(stereo.sideToMidDb) + "}";
    }

    static std::string comparisonJson(const octavia::ComparisonAnalysis& comparison,
            bool includeSpectrum) {
        std::string body = "{";
        body += jStr("reference") + ":" + groupAnalysisJson(comparison.reference, includeSpectrum) + ",";
        body += jStr("target") + ":" + groupAnalysisJson(comparison.target, includeSpectrum) + ",";
        body += jStr("delta") + ":{";
        body += jStr("rmsDb") + ":" + jNum(comparison.rmsDeltaDb) + ",";
        body += jStr("peakDb") + ":" + jNum(comparison.peakDeltaDb) + ",";
        body += jStr("crestDb") + ":" + jNum(comparison.crestDeltaDb) + ",";
        body += jStr("dcOffset") + ":" + jNum(comparison.dcOffsetDelta) + ",";
        body += jStr("balanceDb") + ":" + jNum(comparison.balanceDeltaDb) + ",";
        body += jStr("correlation") + ":" + jNum(comparison.correlationDelta) + ",";
        body += jStr("sideToMidDb") + ":" + jNum(comparison.widthDeltaDb) + ",";
        body += jStr("bandsDb") + ":{";
        const auto& names = octavia::analysisBandNames();
        for (size_t i = 0; i < names.size(); ++i) {
            if (i) body += ",";
            body += jStr(names[i]) + ":" + jNum(comparison.spectralDeltaDb[i]);
        }
        body += "}," + jStr("levelNormalizedBandsDb") + ":{";
        for (size_t i = 0; i < names.size(); ++i) {
            if (i) body += ",";
            body += jStr(names[i]) + ":" + jNum(comparison.normalizedSpectralDeltaDb[i]);
        }
        return body + "}}}";
    }

    bool analyzeLatestMaster(int port, uint32_t frames, bool includeSpectrum,
            octavia::ObservationSnapshot* snapshot, octavia::ChannelAnalysis* analysis,
            std::string* error) {
        if (port < 0 || port > 1 || frames < 64) {
            if (error) *error = "invalid_legacy_analysis_request";
            return false;
        }
        const octavia::ObserveChannel channel = static_cast<octavia::ObserveChannel>(port);
        if (!snapshotPool.create(frames - 1, 0, octavia::observeChannelBit(channel),
                "legacy-master-analysis", snapshot, error)) return false;
        if (snapshot->state != octavia::SnapshotState::Complete) {
            if (error) *error = snapshot->error.empty()
                ? octavia::snapshotStateName(snapshot->state) : snapshot->error;
            return false;
        }
        octavia::AnalysisGroup group;
        group.first = channel;
        octavia::GroupAnalysis result;
        if (!analysisEngine.tryAnalyze(snapshot->observation, group, true,
                includeSpectrum, &result, error)) return false;
        *analysis = result.mono;
        return true;
    }

    static std::string legacyTimingJson(const octavia::ObservationSnapshot& snapshot) {
        const auto& frozen = snapshot.observation;
        return jStr("snapshotId") + ":" + std::to_string(frozen.id) + ","
            + jStr("triggerFrame") + ":" + std::to_string(frozen.triggerFrame) + ","
            + jStr("startFrame") + ":" + std::to_string(frozen.startFrame) + ","
            + jStr("endFrame") + ":" + std::to_string(frozen.endFrame) + ","
            + jStr("sampleRate") + ":" + jNum(frozen.sampleRate);
    }

    static std::string legacyBasicAnalysisJson(int port,
            const octavia::ObservationSnapshot& snapshot,
            const octavia::ChannelAnalysis& analysis) {
        float dominantHz = 0.f;
        float dominantDb = -140.f;
        for (const auto& bin : analysis.spectrum) {
            if (bin.hz > 2000.f) break;
            if (bin.db > dominantDb) { dominantDb = bin.db; dominantHz = bin.hz; }
        }
        std::string body = "{";
        body += jStr("port") + ":" + std::to_string(port) + ",";
        body += jStr("rms") + ":" + jNum(analysis.rms) + ",";
        body += jStr("peak") + ":" + jNum(analysis.peak) + ",";
        body += jStr("dominantHz") + ":" + jNum(dominantHz) + ",";
        body += jStr("dominantNote") + ":" + jStr(freqToNote(dominantHz)) + ",";
        body += legacyTimingJson(snapshot) + "," + jStr("spectrum") + ":[";
        bool first = true;
        for (const auto& bin : analysis.spectrum) {
            if (bin.hz > 2000.f) break;
            if (!first) body += ",";
            first = false;
            const float magnitude = 5.f * std::pow(10.f, bin.db / 20.f);
            body += "{" + jStr("hz") + ":" + jNum(bin.hz) + ","
                + jStr("mag") + ":" + jNum(magnitude) + "}";
        }
        return body + "]}";
    }

    static std::string legacyDetailedAnalysisJson(int port,
            const octavia::ObservationSnapshot& snapshot,
            const octavia::ChannelAnalysis& analysis, bool includeSpectrum) {
        std::string full = channelAnalysisJson(analysis, includeSpectrum);
        // Preserve legacy top-level fields while reusing the canonical analyzer
        // serialization. Removing the outer braces makes the result additive.
        if (full.size() >= 2) full = full.substr(1, full.size() - 2);
        return "{" + jStr("port") + ":" + std::to_string(port) + ","
            + jStr("inputConnected") + ":" + (analysis.connected ? "true" : "false") + ","
            + legacyTimingJson(snapshot) + "," + full + ","
            + jStr("inharmonicHighPeaks") + ":[]}";
    }

    static std::string measurementJson(const octavia::MeasurementResult& measured,
            bool leftConnected, bool rightConnected) {
        const double n = static_cast<double>(measured.measuredFrames);
        auto db = [](double power) { return 10.0 * log10(power + 1e-12); };
        auto lufs = [&](double power) { return -0.691 + db(power); };
        auto windowLufs = [&](size_t blocks) -> double {
            if (measured.blockPowers.size() < blocks) return -std::numeric_limits<double>::infinity();
            double sum = 0.0;
            for (size_t i = measured.blockPowers.size() - blocks;
                    i < measured.blockPowers.size(); ++i) sum += measured.blockPowers[i];
            return lufs(sum / blocks);
        };
        std::vector<double> absoluteGated;
        for (size_t i = 0; i < measured.blockPowers.size(); ++i) {
            const double power = measured.blockPowers[i];
            if (lufs(power) > -70.0) absoluteGated.push_back(power);
        }
        double absoluteMean = 0.0;
        for (size_t i = 0; i < absoluteGated.size(); ++i) absoluteMean += absoluteGated[i];
        absoluteMean /= std::max<size_t>(1, absoluteGated.size());
        const double relativeGate = lufs(absoluteMean) - 10.0;
        double gatedSum = 0.0; int gatedCount = 0;
        for (size_t i = 0; i < absoluteGated.size(); ++i) {
            if (lufs(absoluteGated[i]) > relativeGate) {
                gatedSum += absoluteGated[i]; ++gatedCount;
            }
        }
        const double integratedLufs = gatedCount ? lufs(gatedSum / gatedCount)
            : -std::numeric_limits<double>::infinity();
        const double rmsL = db(measured.rawSum[0] / std::max(1.0, n));
        const double rmsR = db(measured.rawSum[1] / std::max(1.0, n));
        const double peakL = 20.0 * log10(measured.peak[0] + 1e-12);
        const double peakR = 20.0 * log10(measured.peak[1] + 1e-12);
        const double correlation = measured.sumLR /
            (sqrt(measured.rawSum[0] * measured.rawSum[1]) + 1e-12);
        const double balance = 0.5 * (db(measured.rawSum[0]) - db(measured.rawSum[1]));
        const double mid = (measured.rawSum[0] + 2.0 * measured.sumLR
            + measured.rawSum[1]) / (4.0 * std::max(1.0, n));
        const double side = (measured.rawSum[0] - 2.0 * measured.sumLR
            + measured.rawSum[1]) / (4.0 * std::max(1.0, n));
        std::string body = "{";
        body += jStr("measurementId") + ":" + std::to_string(measured.id) + ",";
        body += jStr("state") + ":" + jStr(octavia::measurementStateName(measured.state)) + ",";
        body += jStr("startFrame") + ":" + std::to_string(measured.startFrame) + ",";
        body += jStr("endFrame") + ":" + std::to_string(measured.endFrame) + ",";
        body += jStr("framesMeasured") + ":" + std::to_string(measured.measuredFrames) + ",";
        body += jStr("secondsMeasured") + ":" + jNum(measured.sampleRate > 0.f
            ? static_cast<float>(n / measured.sampleRate) : 0.f) + ",";
        body += jStr("inputs") + ":{" + jStr("leftConnected") + ":" +
            (leftConnected ? "true" : "false") + "," + jStr("rightConnected") + ":" +
            (rightConnected ? "true" : "false") + "},";
        body += jStr("integratedLufs") + ":" + jNum((float)integratedLufs) + ",";
        body += jStr("momentaryLufs") + ":" + jNum((float)windowLufs(4)) + ",";
        body += jStr("shortTermLufs") + ":" + jNum((float)windowLufs(30)) + ",";
        body += jStr("gatedBlocks") + ":" + std::to_string(gatedCount) + ",";
        body += jStr("blockHistoryTruncated") + ":" +
            (measured.blockHistoryTruncated ? "true" : "false") + ",";
        body += jStr("kWeightedDbfsEstimate") + ":" + jNum((float)lufs(
            (measured.kSum[0] + measured.kSum[1]) / std::max(1.0, n))) + ",";
        body += jStr("left") + ":{" + jStr("kWeightedDbfsEstimate") + ":" +
            jNum((float)lufs(measured.kSum[0] / std::max(1.0, n))) + "," +
            jStr("rmsDb") + ":" + jNum((float)rmsL) + "," + jStr("peakDb") + ":" +
            jNum((float)peakL) + "," + jStr("crestDb") + ":" +
            jNum((float)(peakL - rmsL)) + "," + jStr("clippedSamples") + ":" +
            std::to_string(measured.clipped[0]) + "},";
        body += jStr("right") + ":{" + jStr("kWeightedDbfsEstimate") + ":" +
            jNum((float)lufs(measured.kSum[1] / std::max(1.0, n))) + "," +
            jStr("rmsDb") + ":" + jNum((float)rmsR) + "," + jStr("peakDb") + ":" +
            jNum((float)peakR) + "," + jStr("crestDb") + ":" +
            jNum((float)(peakR - rmsR)) + "," + jStr("clippedSamples") + ":" +
            std::to_string(measured.clipped[1]) + "},";
        body += jStr("stereo") + ":{" + jStr("available") + ":" +
            (leftConnected && rightConnected ? "true" : "false");
        if (leftConnected && rightConnected) {
            body += "," + jStr("correlation") + ":" + jNum((float)correlation) + "," +
                jStr("balanceDb") + ":" + jNum((float)balance) + "," +
                jStr("sideMidDb") + ":" + jNum((float)(db(side) - db(mid)));
        }
        body += "}}";
        return body;
    }

    void setupRoutes() {
        // Optional shared-secret auth: set OCTAVIA_TOKEN in the environment
        // (both for VCV Rack and the MCP server) to require it on every request.
        static const std::string token = octaviaToken();
        svr.set_pre_routing_handler([this](const httplib::Request& req, httplib::Response& res){
            if (!token.empty() && req.get_header_value("X-Octavia-Token") != token) {
                res.status = 401;
                res.set_content("{\"error\":\"invalid or missing X-Octavia-Token\"}", "application/json");
                return httplib::Server::HandlerResponse::Handled;
            }
            if (req.method == "GET" || req.method == "HEAD")
                readActivityGeneration.fetch_add(1, std::memory_order_relaxed);
            else
                writeActivityGeneration.fetch_add(1, std::memory_order_relaxed);
            return httplib::Server::HandlerResponse::Unhandled;
        });
        svr.Get("/status", [this](const httplib::Request&, httplib::Response& res) {
            std::string b = "{"+jStr("running")+": "+(serverRunning?"true":"false")+", "+jStr("port")+": "+std::to_string(octaviaPort())+", "+jStr("version")+": "+jStr("2.10.0")+"}";
            res.set_content(b,"application/json");
        });

        // ── Octavia Console mailbox ─────────────────────────────────────────
        // Text exchange is control/UI work only. The audio thread never reads
        // or writes these mailboxes.
        svr.Get("/console", [](const httplib::Request&, httplib::Response& res) {
            const std::vector<int64_t> ids = octavia_console::listMailboxIds();
            std::string body = "{" + jStr("moduleIds") + ":[";
            for (std::size_t i = 0; i < ids.size(); ++i) {
                if (i) body += ",";
                body += std::to_string(ids[i]);
            }
            body += "]}";
            res.set_content(body, "application/json");
        });
        svr.Get(R"(/console/(\d+)/status)", [](const httplib::Request& r, httplib::Response& res) {
            int64_t moduleId = std::stoll(r.matches[1].str());
            auto mailbox = octavia_console::findMailbox(moduleId);
            if (!mailbox) {
                res.status = 404;
                res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json");
                return;
            }
            auto snapshot = mailbox->snapshot();
            std::string body = "{" + jStr("moduleId") + ":" + std::to_string(moduleId)
                + "," + jStr("state") + ":" + jStr(octavia_console::agentStateName(snapshot.state))
                + "," + jStr("latestPromptId") + ":" + std::to_string(snapshot.latestPromptId)
                + "," + jStr("latestResponsePromptId") + ":" + std::to_string(snapshot.latestResponsePromptId)
                + "," + jStr("pendingCount") + ":" + std::to_string(snapshot.pendingCount)
                + "," + jStr("queuedCount") + ":" + std::to_string(snapshot.queuedCount)
                + "," + jStr("claimedCount") + ":" + std::to_string(snapshot.claimedCount)
                + "," + jStr("liveWorkerCount") + ":" + std::to_string(snapshot.liveWorkerCount)
                + "," + jStr("backgroundWorkerEnabled") + ":" + (snapshot.backgroundWorkerEnabled ? "true" : "false") + "}";
            res.set_content(body, "application/json");
        });
        svr.Get(R"(/console/(\d+)/prompt)", [](const httplib::Request& r, httplib::Response& res) {
            int64_t moduleId = std::stoll(r.matches[1].str());
            auto mailbox = octavia_console::findMailbox(moduleId);
            if (!mailbox) {
                res.status = 404;
                res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json");
                return;
            }
            uint64_t afterId = 0;
            int waitMs = 0;
            try {
                if (r.has_param("after")) afterId = std::stoull(r.get_param_value("after"));
                if (r.has_param("waitMs")) waitMs = std::stoi(r.get_param_value("waitMs"));
            } catch (...) {
                res.status = 400;
                res.set_content("{\"error\":\"after and waitMs must be integers\"}", "application/json");
                return;
            }
            octavia_console::Prompt prompt;
            if (!mailbox->waitForPrompt(afterId, std::max(0, std::min(waitMs, 25000)), &prompt)) {
                res.set_content("{\"prompt\":null}", "application/json");
                return;
            }
            res.set_content("{" + jStr("prompt") + ":{" + jStr("id") + ":" +
                std::to_string(prompt.id) + "," + jStr("text") + ":" + jStr(prompt.text) + "}}",
                "application/json");
        });
        svr.Post(R"(/console/(\d+)/response)", [](const httplib::Request& r, httplib::Response& res) {
            int64_t moduleId = std::stoll(r.matches[1].str());
            auto mailbox = octavia_console::findMailbox(moduleId);
            if (!mailbox) {
                res.status = 404;
                res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json");
                return;
            }
            json_error_t jsonError;
            json_t* root = json_loads(r.body.c_str(), 0, &jsonError);
            if (!root || !json_is_object(root)) {
                if (root) json_decref(root);
                res.status = 400;
                res.set_content("{\"error\":\"response body must be a JSON object\"}", "application/json");
                return;
            }
            json_t* promptIdJ = json_object_get(root, "promptId");
            json_t* textJ = json_object_get(root, "text");
            json_t* errorJ = json_object_get(root, "error");
            int64_t promptId = json_is_integer(promptIdJ) ? json_integer_value(promptIdJ) : -1;
            std::string text = json_is_string(textJ) ? json_string_value(textJ) : "";
            bool isError = json_is_true(errorJ);
            json_decref(root);
            std::string error;
            if (promptId < 0 || !mailbox->postResponse((uint64_t)promptId, std::move(text), isError, &error)) {
                res.status = 400;
                res.set_content("{" + jStr("error") + ":" + jStr(error.empty() ? "invalid promptId" : error) + "}",
                    "application/json");
                return;
            }
            res.set_content("{\"ok\":true}", "application/json");
        });

        // Background-worker API. These routes are additive; the MCP long-poll
        // path above remains a supported synthetic legacy claimant.
        svr.Post(R"(/console/(\d+)/workers)", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str()));
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            std::string name = "worker";
            json_error_t je; json_t* root = json_loads(r.body.c_str(), 0, &je);
            if (root && json_is_object(root)) { json_t* value = json_object_get(root, "name"); if (json_is_string(value)) name = json_string_value(value); }
            if (root) json_decref(root);
            std::string workerId, error;
            if (!mailbox->registerWorker(std::move(name), &workerId, &error)) { res.status = 403; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            res.status = 201; res.set_content("{" + jStr("workerId") + ":" + jStr(workerId) + "}", "application/json");
        });
        svr.Post(R"(/console/(\d+)/workers/([^/]+)/heartbeat)", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str())); std::string error;
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            if (!mailbox->heartbeatWorker(r.matches[2].str(), &error)) { res.status = 410; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            res.set_content("{\"ok\":true}", "application/json");
        });
        svr.Post(R"(/console/(\d+)/workers/([^/]+)/unregister)", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str())); std::string error;
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            if (!mailbox->unregisterWorker(r.matches[2].str(), &error)) { res.status = 410; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            res.set_content("{\"ok\":true}", "application/json");
        });
        svr.Get(R"(/console/(\d+)/events)", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str()));
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            const std::string workerId = r.has_param("workerId") ? r.get_param_value("workerId") : "";
            std::string error;
            if (!mailbox->heartbeatWorker(workerId, &error)) { res.status = 410; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            uint64_t afterId = 0;
            try {
                if (r.has_param("after")) afterId = std::stoull(r.get_param_value("after"));
                const std::string lastEventId = r.get_header_value("Last-Event-ID");
                if (!lastEventId.empty()) afterId = std::max(afterId, static_cast<uint64_t>(std::stoull(lastEventId)));
            } catch (...) { res.status = 400; res.set_content("{\"error\":\"after and Last-Event-ID must be integers\"}", "application/json"); return; }
            res.set_header("Cache-Control", "no-cache");
            res.set_header("X-Accel-Buffering", "no");
            res.set_chunked_content_provider("text/event-stream",
                [mailbox, workerId, afterId](size_t, httplib::DataSink& sink) mutable {
                    std::string heartbeatError;
                    if (!mailbox->heartbeatWorker(workerId, &heartbeatError)) {
                        const std::string revoked = "event: worker.revoked\ndata: {}\n\n";
                        sink.write(revoked.data(), revoked.size());
                        sink.done();
                        return false;
                    }
                    octavia_console::Event event;
                    if (!mailbox->waitForEvent(afterId, 10000, &event)) {
                        const std::string keepalive = ": keepalive\n\n";
                        return sink.write(keepalive.data(), keepalive.size());
                    }
                    afterId = event.id;
                    const std::string data = "id: " + std::to_string(event.id) + "\nevent: " + event.type
                        + "\ndata: {" + jStr("promptId") + ":" + std::to_string(event.promptId) + "}\n\n";
                    return sink.write(data.data(), data.size());
                });
        });
        svr.Post(R"(/console/(\d+)/prompts/(\d+)/claim)", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str()));
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            json_error_t je; json_t* root = json_loads(r.body.c_str(), 0, &je); std::string workerId;
            if (root && json_is_object(root)) { json_t* value = json_object_get(root, "workerId"); if (json_is_string(value)) workerId = json_string_value(value); }
            if (root) json_decref(root);
            octavia_console::Prompt prompt; std::string claimToken, error;
            if (!mailbox->claimPrompt(workerId, std::stoull(r.matches[2].str()), &prompt, &claimToken, &error)) { res.status = 409; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            res.set_content("{" + jStr("prompt") + ":{" + jStr("id") + ":" + std::to_string(prompt.id) + "," + jStr("text") + ":" + jStr(prompt.text) + "}," + jStr("claimToken") + ":" + jStr(claimToken) + "}", "application/json");
        });
        svr.Post(R"(/console/(\d+)/prompts/claim-next)", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str()));
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            json_error_t je; json_t* root = json_loads(r.body.c_str(), 0, &je); std::string workerId;
            if (root && json_is_object(root)) { json_t* value = json_object_get(root, "workerId"); if (json_is_string(value)) workerId = json_string_value(value); }
            if (root) json_decref(root);
            octavia_console::Prompt prompt; std::string claimToken, error;
            if (!mailbox->claimNextPrompt(workerId, &prompt, &claimToken, &error)) { res.status = 409; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            res.set_content("{" + jStr("prompt") + ":{" + jStr("id") + ":" + std::to_string(prompt.id) + "," + jStr("text") + ":" + jStr(prompt.text) + "}," + jStr("claimToken") + ":" + jStr(claimToken) + "}", "application/json");
        });
        svr.Post(R"(/console/(\d+)/prompts/(\d+)/(renew|release|complete))", [](const httplib::Request& r, httplib::Response& res) {
            auto mailbox = octavia_console::findMailbox(std::stoll(r.matches[1].str()));
            if (!mailbox) { res.status = 404; res.set_content("{\"error\":\"Octavia Console not found\"}", "application/json"); return; }
            json_error_t je; json_t* root = json_loads(r.body.c_str(), 0, &je);
            std::string token, text, operationId; bool isError = false;
            if (root && json_is_object(root)) {
                json_t* v = json_object_get(root, "claimToken"); if (json_is_string(v)) token = json_string_value(v);
                v = json_object_get(root, "text"); if (json_is_string(v)) text = json_string_value(v);
                v = json_object_get(root, "operationId"); if (json_is_string(v)) operationId = json_string_value(v);
                isError = json_is_true(json_object_get(root, "error"));
            }
            if (root) json_decref(root);
            const uint64_t promptId = std::stoull(r.matches[2].str()); const std::string action = r.matches[3].str(); std::string error; bool ok = false;
            if (action == "renew") ok = mailbox->renewClaim(promptId, token, &error);
            else if (action == "release") ok = mailbox->releaseClaim(promptId, token, &error);
            else ok = mailbox->completeClaim(promptId, token, std::move(text), isError, operationId, &error);
            if (!ok) { res.status = 409; res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return; }
            res.set_content("{\"ok\":true}", "application/json");
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

        // ── Shared observation history and snapshot API ─────────────────────
        svr.Get("/audio/monitors", [this](const httplib::Request&, httplib::Response& res){
            const bool published = observationHistory.hasPublishedFrame();
            std::string body = "{";
            body += jStr("sampleRate") + ":" + jNum(observationHistory.currentSampleRate()) + ",";
            body += jStr("publishedFrame") + ":";
            body += published ? std::to_string(observationHistory.publishedFrame()) : "null";
            body += "," + jStr("historyFrames") + ":" +
                std::to_string(octavia::OBSERVATION_HISTORY_FRAMES) + ",";
            body += jStr("monitors") + ":[";
            const uint8_t connectedMask = observationHistory.currentConnectedMask();
            for (size_t channel = 0; channel < octavia::OBSERVATION_CHANNELS; ++channel) {
                if (channel) body += ",";
                const octavia::ObserveChannel observed =
                    static_cast<octavia::ObserveChannel>(channel);
                body += "{" + jStr("id") + ":" +
                    jStr(octavia::observeChannelName(observed)) + ",";
                body += jStr("connected") + ":" +
                    ((connectedMask & (1u << channel)) ? "true" : "false") + ",";
                body += jStr("channels") + ":" +
                    std::to_string(observationHistory.currentChannelCount(observed)) + ",";
                body += jStr("liveMeter") + ":" + (channel < 2 ? "true" : "false") + ",";
                body += jStr("snapshotGeneration") + ":" +
                    std::to_string(observationHistory.snapshotGeneration(observed)) + ",";
                const uint32_t activeUsers = channel >= 2
                    ? monitorAnalysisCount[channel - 2].load(std::memory_order_relaxed) : 0;
                body += jStr("activeAnalysisUsers") + ":" + std::to_string(activeUsers) + ",";
                body += jStr("activity") + ":" + jStr(activeUsers ? "analyzing"
                    : ((connectedMask & (1u << channel)) ? "rolling" : "disconnected")) + "}";
            }
            body += "]}";
            res.set_content(body, "application/json");
        });

        svr.Post("/audio/snapshot", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jsonError;
            json_t* root = json_loads(r.body.c_str(), 0, &jsonError);
            if (!root || !json_is_object(root)) {
                if (root) json_decref(root);
                res.status = 400;
                res.set_content("{\"error\":\"body must be a JSON object\"}", "application/json");
                return;
            }

            uint8_t requestedMask = 0;
            json_t* monitors = json_object_get(root, "monitors");
            if (!monitors) {
                requestedMask = octavia::observeChannelBit(octavia::ObserveChannel::MasterL)
                    | octavia::observeChannelBit(octavia::ObserveChannel::MasterR);
            } else if (!json_is_array(monitors) || json_array_size(monitors) == 0) {
                json_decref(root);
                res.status = 400;
                res.set_content("{\"error\":\"monitors must be a non-empty string array\"}", "application/json");
                return;
            } else {
                size_t index; json_t* value;
                json_array_foreach(monitors, index, value) {
                    octavia::ObserveChannel channel;
                    if (!json_is_string(value)
                            || !octavia::parseObserveChannel(json_string_value(value), &channel)) {
                        json_decref(root);
                        res.status = 400;
                        res.set_content("{\"error\":\"unknown monitor name\"}", "application/json");
                        return;
                    }
                    requestedMask |= octavia::observeChannelBit(channel);
                }
            }

            const float sr = observationHistory.currentSampleRate();
            double preMs = sr > 0.f ? 1000.0 * (AUDIO_BUF - 1) / sr : 0.0;
            double postMs = 0.0;
            json_t* pre = json_object_get(root, "preMs");
            json_t* post = json_object_get(root, "postMs");
            if (pre) preMs = json_is_number(pre) ? json_number_value(pre) : -1.0;
            if (post) postMs = json_is_number(post) ? json_number_value(post) : -1.0;
            json_t* labelValue = json_object_get(root, "label");
            std::string label = json_is_string(labelValue) ? json_string_value(labelValue) : "";
            if (!std::isfinite(preMs) || !std::isfinite(postMs) || preMs < 0.0 || postMs < 0.0
                    || label.size() > 256 || sr <= 0.f) {
                json_decref(root);
                res.status = 400;
                res.set_content("{\"error\":\"invalid preMs, postMs, label, or sample rate\"}",
                    "application/json");
                return;
            }
            const double maxFrames = static_cast<double>(octavia::OBSERVATION_HISTORY_FRAMES - 1);
            const double preFrameValue = std::round(preMs * sr / 1000.0);
            const double postFrameValue = std::round(postMs * sr / 1000.0);
            if (preFrameValue + postFrameValue > maxFrames) {
                json_decref(root);
                res.status = 400;
                res.set_content("{\"error\":\"requested window exceeds observation history\"}",
                    "application/json");
                return;
            }
            const uint32_t preFrames = static_cast<uint32_t>(preFrameValue);
            const uint32_t postFrames = static_cast<uint32_t>(postFrameValue);
            json_decref(root);

            octavia::ObservationSnapshot snapshot;
            std::string error;
            if (!snapshotPool.create(preFrames, postFrames, requestedMask, label,
                    &snapshot, &error)) {
                res.status = error == "snapshot_pool_busy" ? 429 : 409;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}",
                    "application/json");
                return;
            }
            res.status = snapshot.state == octavia::SnapshotState::Complete ? 201 : 202;
            res.set_content(snapshotJson(snapshot), "application/json");
        });

        svr.Get(R"(/audio/snapshot/(\d+))", [this](const httplib::Request& r,
                httplib::Response& res){
            uint64_t id = 0;
            try { id = std::stoull(r.matches[1].str()); }
            catch (...) {
                res.status = 400;
                res.set_content("{\"error\":\"invalid snapshot ID\"}", "application/json");
                return;
            }
            octavia::ObservationSnapshot snapshot;
            if (!snapshotPool.get(id, &snapshot)) {
                res.status = 404;
                res.set_content("{\"error\":\"snapshot_expired\"}", "application/json");
                return;
            }
            if (snapshot.state == octavia::SnapshotState::Pending) res.status = 202;
            else if (snapshot.state == octavia::SnapshotState::Failed) res.status = 409;
            res.set_content(snapshotJson(snapshot), "application/json");
        });

        // Explicit snapshot analysis. Frozen samples make multi-monitor results
        // repeatable and remove the legacy analyzer's sleep-based recapture.
        svr.Post("/audio/analyze", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jsonError;
            json_t* root = json_loads(r.body.c_str(), 0, &jsonError);
            json_t* idValue = root ? json_object_get(root, "snapshotId") : NULL;
            if (!json_is_object(root) || !json_is_integer(idValue)
                    || json_integer_value(idValue) <= 0) {
                if (root) json_decref(root);
                res.status = 400;
                res.set_content("{\"error\":\"analysis requires a positive snapshotId\"}", "application/json");
                return;
            }
            octavia::AnalysisGroup group;
            std::string error;
            if (!parseAnalysisGroup(root, &group, &error)) {
                json_decref(root); res.status = 400;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json");
                return;
            }
            json_t* detailValue = json_object_get(root, "detail");
            const std::string detail = json_is_string(detailValue) ? json_string_value(detailValue) : "basic";
            json_t* spectrumValue = json_object_get(root, "includeSpectrum");
            const bool includeSpectrum = json_is_true(spectrumValue);
            const uint64_t snapshotId = static_cast<uint64_t>(json_integer_value(idValue));
            json_decref(root);
            if (detail != "basic" && detail != "detailed") {
                res.status = 400;
                res.set_content("{\"error\":\"detail must be basic or detailed\"}", "application/json");
                return;
            }
            octavia::ObservationSnapshot snapshot;
            if (!snapshotPool.get(snapshotId, &snapshot)) {
                res.status = 404; res.set_content("{\"error\":\"snapshot_expired\"}", "application/json"); return;
            }
            if (snapshot.state != octavia::SnapshotState::Complete) {
                res.status = snapshot.state == octavia::SnapshotState::Pending ? 409 : 422;
                res.set_content("{" + jStr("error") + ":" + jStr(octavia::snapshotStateName(snapshot.state)) + "}", "application/json");
                return;
            }
            const uint8_t selectedMask = octavia::observeChannelBit(group.first)
                | (group.stereo ? octavia::observeChannelBit(group.second) : 0);
            if ((snapshot.observation.requestedMask & selectedMask) != selectedMask) {
                res.status = 422;
                res.set_content("{\"error\":\"selected monitor is not present in snapshot\"}", "application/json"); return;
            }
            octavia::GroupAnalysis analysis;
            for (int monitor = 0; monitor < 4; ++monitor)
                if (selectedMask & (1u << (monitor + 2)))
                    monitorAnalysisCount[monitor].fetch_add(1, std::memory_order_relaxed);
            if (!analysisEngine.tryAnalyze(snapshot.observation, group, detail == "detailed",
                    includeSpectrum, &analysis, &error)) {
                for (int monitor = 0; monitor < 4; ++monitor)
                    if (selectedMask & (1u << (monitor + 2)))
                        monitorAnalysisCount[monitor].fetch_sub(1, std::memory_order_relaxed);
                res.status = error == "analysis_busy" ? 429 : 422;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return;
            }
            for (int monitor = 0; monitor < 4; ++monitor)
                if (selectedMask & (1u << (monitor + 2)))
                    monitorAnalysisCount[monitor].fetch_sub(1, std::memory_order_relaxed);
            res.set_content("{" + jStr("snapshotId") + ":" + std::to_string(snapshotId) + ","
                + jStr("triggerFrame") + ":" + std::to_string(snapshot.observation.triggerFrame) + ","
                + jStr("startFrame") + ":" + std::to_string(snapshot.observation.startFrame) + ","
                + jStr("endFrame") + ":" + std::to_string(snapshot.observation.endFrame) + ","
                + jStr("sampleRate") + ":" + jNum(snapshot.observation.sampleRate) + ","
                + jStr("detail") + ":" + jStr(detail) + ","
                + jStr("result") + ":" + groupAnalysisJson(analysis, includeSpectrum) + "}", "application/json");
        });

        svr.Post("/audio/compare", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jsonError;
            json_t* root = json_loads(r.body.c_str(), 0, &jsonError);
            json_t* idValue = root ? json_object_get(root, "snapshotId") : NULL;
            if (!json_is_object(root) || !json_is_integer(idValue) || json_integer_value(idValue) <= 0) {
                if (root) json_decref(root);
                res.status = 400; res.set_content("{\"error\":\"comparison requires a positive snapshotId\"}", "application/json"); return;
            }
            octavia::AnalysisGroup reference, target;
            std::string error;
            if (!parseAnalysisGroup(json_object_get(root, "reference"), &reference, &error)
                    || !parseAnalysisGroup(json_object_get(root, "target"), &target, &error)) {
                json_decref(root); res.status = 400;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return;
            }
            json_t* detailValue = json_object_get(root, "detail");
            const std::string detail = json_is_string(detailValue) ? json_string_value(detailValue) : "detailed";
            const bool includeSpectrum = json_is_true(json_object_get(root, "includeSpectrum"));
            const uint64_t snapshotId = static_cast<uint64_t>(json_integer_value(idValue));
            json_decref(root);
            if (detail != "basic" && detail != "detailed") {
                res.status = 400; res.set_content("{\"error\":\"detail must be basic or detailed\"}", "application/json"); return;
            }
            octavia::ObservationSnapshot snapshot;
            if (!snapshotPool.get(snapshotId, &snapshot)) {
                res.status = 404; res.set_content("{\"error\":\"snapshot_expired\"}", "application/json"); return;
            }
            if (snapshot.state != octavia::SnapshotState::Complete) {
                res.status = snapshot.state == octavia::SnapshotState::Pending ? 409 : 422;
                res.set_content("{" + jStr("error") + ":" + jStr(octavia::snapshotStateName(snapshot.state)) + "}", "application/json"); return;
            }
            uint8_t selectedMask = octavia::observeChannelBit(reference.first)
                | octavia::observeChannelBit(target.first);
            if (reference.stereo) selectedMask |= octavia::observeChannelBit(reference.second);
            if (target.stereo) selectedMask |= octavia::observeChannelBit(target.second);
            if ((snapshot.observation.requestedMask & selectedMask) != selectedMask) {
                res.status = 422; res.set_content("{\"error\":\"selected monitor is not present in snapshot\"}", "application/json"); return;
            }
            octavia::ComparisonAnalysis comparison;
            for (int monitor = 0; monitor < 4; ++monitor)
                if (selectedMask & (1u << (monitor + 2)))
                    monitorAnalysisCount[monitor].fetch_add(1, std::memory_order_relaxed);
            if (!analysisEngine.tryCompare(snapshot.observation, reference, target,
                    detail == "detailed", includeSpectrum, &comparison, &error)) {
                for (int monitor = 0; monitor < 4; ++monitor)
                    if (selectedMask & (1u << (monitor + 2)))
                        monitorAnalysisCount[monitor].fetch_sub(1, std::memory_order_relaxed);
                res.status = error == "analysis_busy" ? 429 : 422;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json"); return;
            }
            for (int monitor = 0; monitor < 4; ++monitor)
                if (selectedMask & (1u << (monitor + 2)))
                    monitorAnalysisCount[monitor].fetch_sub(1, std::memory_order_relaxed);
            res.set_content("{" + jStr("snapshotId") + ":" + std::to_string(snapshotId) + ","
                + jStr("triggerFrame") + ":" + std::to_string(snapshot.observation.triggerFrame) + ","
                + jStr("startFrame") + ":" + std::to_string(snapshot.observation.startFrame) + ","
                + jStr("endFrame") + ":" + std::to_string(snapshot.observation.endFrame) + ","
                + jStr("sampleRate") + ":" + jNum(snapshot.observation.sampleRate) + ","
                + jStr("comparison") + ":" + comparisonJson(comparison, includeSpectrum) + "}", "application/json");
        });

        // Legacy numeric contracts now freeze Master history and use the same
        // analyzer as named snapshot requests.
        svr.Get(R"(/audio/(\d+))", [this](const httplib::Request& r, httplib::Response& res){
            const int port = std::stoi(r.matches[1].str());
            if (port < 0 || port > 1) {
                res.status = 400;
                res.set_content("{\"error\":\"port must be 0 (L) or 1 (R)\"}", "application/json");
                return;
            }
            octavia::ObservationSnapshot snapshot;
            octavia::ChannelAnalysis analysis;
            std::string error;
            if (!analyzeLatestMaster(port, AUDIO_BUF, true, &snapshot, &analysis, &error)) {
                res.status = error == "analysis_busy" ? 429 : 409;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json");
                return;
            }
            res.set_content(legacyBasicAnalysisJson(port, snapshot, analysis), "application/json");
        });

        svr.Get(R"(/audio/(\d+)/analyze)", [this](const httplib::Request& r,
                httplib::Response& res){
            const int port = std::stoi(r.matches[1].str());
            if (port < 0 || port > 1) {
                res.status = 400;
                res.set_content("{\"error\":\"port must be 0 (L) or 1 (R)\"}", "application/json");
                return;
            }
            const bool includeSpectrum = r.has_param("spectrum")
                && r.get_param_value("spectrum") != "0";
            octavia::ObservationSnapshot snapshot;
            octavia::ChannelAnalysis analysis;
            std::string error;
            // Enough frozen context for two 4096-frame windows separated by
            // the analyzer's deterministic 120 ms stability interval.
            const float sr = std::max(1.f, observationHistory.currentSampleRate());
            const uint32_t frames = static_cast<uint32_t>(std::min<double>(
                octavia::OBSERVATION_HISTORY_FRAMES, std::max<double>(16384.0, sr * 0.35)));
            if (!analyzeLatestMaster(port, frames, includeSpectrum,
                    &snapshot, &analysis, &error)) {
                res.status = error == "analysis_busy" ? 429 : 409;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json");
                return;
            }
            res.set_content(legacyDetailedAnalysisJson(port, snapshot, analysis,
                includeSpectrum), "application/json");
        });

        // Retained temporarily as non-compiled reference code while compatibility
        // output is validated; Phase 5 traffic uses the routes above.
        // Legacy reset/read workflow backed by an explicitly armed measurement session.
        svr.Get("/audio/loudness", [this](const httplib::Request&, httplib::Response& res){
            const octavia::MeasurementResult measured = masterMeasurement.read();
            if (measured.state == octavia::MeasurementState::Idle || measured.id == 0) {
                res.status = 409;
                res.set_content("{\"state\":\"idle\",\"error\":\"no measurement armed; POST /audio/loudness/reset first\"}",
                    "application/json");
                return;
            }
            res.set_content(measurementJson(measured,
                audioInputConnected[0].load(std::memory_order_relaxed),
                audioInputConnected[1].load(std::memory_order_relaxed)), "application/json");
        });

        svr.Post("/audio/loudness/reset", [this](const httplib::Request&, httplib::Response& res){
            uint64_t id = 0; std::string error;
            masterMeasurement.arm(0, true, &id, &error);
            res.status = 202;
            res.set_content("{\"ok\":true,\"state\":\"pending\",\"measurementId\":" +
                std::to_string(id) + "}", "application/json");
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
            json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* pluginJ=root?json_object_get(root,"plugin"):NULL;
            json_t* modelJ=root?json_object_get(root,"model"):NULL;
            if (!json_is_object(root) || !json_is_string(pluginJ) || !json_is_string(modelJ)
                || !*json_string_value(pluginJ) || !*json_string_value(modelJ)) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"module addition requires non-empty string plugin and model slugs\"}","application/json"); return;
            }
            auto job=std::make_shared<AddModuleJob>();
            job->pluginSlug=json_string_value(pluginJ); job->modelSlug=json_string_value(modelJ);
            json_decref(root);
            { std::unique_lock<std::mutex> lk(addQueueMtx); addQueue.push(job); }
            if (!waitDone(job,2000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true,\"id\":"+std::to_string(job->newId)+"}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post(R"(/modules/(\d+)/params/(\d+))",
            [this](const httplib::Request& r, httplib::Response& res){
                json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
                json_t* valueJ=root?json_object_get(root,"value"):NULL;
                if (!json_is_object(root) || !json_is_number(valueJ)) {
                    if (root) json_decref(root);
                    res.set_content("{\"error\":\"parameter update requires numeric value\"}","application/json"); return;
                }
                auto job=std::make_shared<SetParamJob>();
                job->moduleId=std::stoll(r.matches[1].str()); job->paramId=std::stoi(r.matches[2].str()); job->value=(float)json_number_value(valueJ);
                json_decref(root);
                { std::unique_lock<std::mutex> lk(setQueueMtx); setQueue.push(job); }
                if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
                else if (job->success) res.set_content("{\"ok\":true,\"value\":"+std::to_string(job->value)+"}","application/json");
                else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
            });

        svr.Post("/cables", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* om=root?json_object_get(root,"outputModuleId"):NULL; json_t* op=root?json_object_get(root,"outputPortId"):NULL;
            json_t* im=root?json_object_get(root,"inputModuleId"):NULL; json_t* ip=root?json_object_get(root,"inputPortId"):NULL;
            json_t* color=root?json_object_get(root,"color"):NULL;
            if (!json_is_object(root) || !json_is_integer(om) || !json_is_integer(op)
                || !json_is_integer(im) || !json_is_integer(ip) || (color && !json_is_string(color))) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"cable requires integer module/port IDs and optional string color\"}","application/json"); return;
            }
            auto job=std::make_shared<CableJob>(); job->type=CableJob::ADD;
            job->outModId=json_integer_value(om); job->outPortId=json_integer_value(op);
            job->inModId=json_integer_value(im); job->inPortId=json_integer_value(ip);
            if (color) job->color=json_string_value(color);
            json_decref(root);
            { std::unique_lock<std::mutex> lk(cableQueueMtx); cableQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post("/cables/disconnect", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* im=root?json_object_get(root,"inputModuleId"):NULL; json_t* ip=root?json_object_get(root,"inputPortId"):NULL;
            if (!json_is_object(root) || !json_is_integer(im) || !json_is_integer(ip)) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"disconnect requires integer inputModuleId and inputPortId\"}","application/json"); return;
            }
            auto job=std::make_shared<CableJob>(); job->type=CableJob::REMOVE;
            job->inModId=json_integer_value(im); job->inPortId=json_integer_value(ip); json_decref(root);
            { std::unique_lock<std::mutex> lk(cableQueueMtx); cableQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post("/cables/disconnect-output", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* om=root?json_object_get(root,"outputModuleId"):NULL; json_t* op=root?json_object_get(root,"outputPortId"):NULL;
            if (!json_is_object(root) || !json_is_integer(om) || !json_is_integer(op)) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"disconnect requires integer outputModuleId and outputPortId\"}","application/json"); return;
            }
            auto job=std::make_shared<CableJob>(); job->type=CableJob::REMOVE_OUTPUT;
            job->outModId=json_integer_value(om); job->outPortId=json_integer_value(op); json_decref(root);
            { std::unique_lock<std::mutex> lk(cableQueueMtx); cableQueue.push(job); }
            if (!waitDone(job)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
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
            json_error_t jerr;
            json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* hp=root ? json_object_get(root,"hp") : NULL;
            json_t* row=root ? json_object_get(root,"row") : NULL;
            if (!json_is_object(root) || !json_is_number(hp) || (row && !json_is_integer(row))) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"position requires numeric hp and optional integer row\"}","application/json"); return;
            }
            const json_int_t rowValue = row ? json_integer_value(row) : 0;
            if (row && (rowValue < -octavia::kMaxAbsRackRow || rowValue > octavia::kMaxAbsRackRow)) {
                json_decref(root); res.set_content("{\"error\":\"row exceeds the safe coordinate limit of +/-100000\"}","application/json"); return;
            }
            job->hp=(float)json_number_value(hp);
            if (row) { job->row=(int)rowValue; job->hasRow=true; }
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
            if (!arr || !json_is_array(arr) || json_array_size(arr)==0 || json_array_size(arr)>octavia::kMaxBulkChanges) {
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
                const json_int_t rowValue=json_integer_value(jr);
                if (json_integer_value(jm)<0 || rowValue < -octavia::kMaxAbsRackRow || rowValue > octavia::kMaxAbsRackRow) {
                    json_decref(root); res.set_content("{\"error\":\"layout module IDs must be non-negative and rows within +/-100000\"}","application/json"); return;
                }
                job->changes.push_back({(int64_t)json_integer_value(jm),(float)json_number_value(jh),(int)rowValue});
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
            if (!arr || !json_is_array(arr) || json_array_size(arr)==0 || json_array_size(arr)>octavia::kMaxBulkChanges) {
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
                if (!json_is_integer(jm) || !json_is_integer(jp) || !json_is_number(jv)
                    || json_integer_value(jm)<0 || json_integer_value(jp)<0) {
                    json_decref(root);
                    res.set_content("{\"error\":"+jStr("change "+std::to_string(idx)+" requires non-negative integer moduleId/paramId and numeric value")+"}","application/json");
                    return;
                }
                job->changes.push_back({(int64_t)json_integer_value(jm),
                                        (int)json_integer_value(jp),
                                        (float)json_number_value(jv)});
            }
            json_decref(root);
            { std::unique_lock<std::mutex> lk(bulkParamQueueMtx); bulkParamQueue.push(job); }
            if (!waitDone(job, 2000)) { res.set_content("{\"error\":\"timeout\"}","application/json"); return; }
            if (!job->success) { res.set_content("{\"error\":"+jStr(job->error)+"}","application/json"); return; }
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

        // Convenience route backed by exact engine-frame session boundaries.
        svr.Get("/audio/measure", [this](const httplib::Request& r, httplib::Response& res){
            int seconds = 15;
            try { if (r.has_param("seconds")) seconds = std::stoi(r.get_param_value("seconds")); }
            catch (...) {
                res.status = 400;
                res.set_content("{\"error\":\"seconds must be an integer\"}", "application/json");
                return;
            }
            seconds = std::max(1, std::min(60, seconds));
            const float sr = sampleRate.load(std::memory_order_relaxed);
            uint64_t id = 0; std::string error;
            if (!masterMeasurement.arm(static_cast<uint64_t>(std::llround(seconds * sr)),
                    false, &id, &error)) {
                res.status = 409;
                res.set_content("{" + jStr("error") + ":" + jStr(error) + "}", "application/json");
                return;
            }
            const auto deadline = std::chrono::steady_clock::now()
                + std::chrono::milliseconds(seconds * 1000 + 1500);
            octavia::MeasurementResult measured;
            do {
                measured = masterMeasurement.read();
                if (measured.id == id && (measured.state == octavia::MeasurementState::Complete
                        || measured.state == octavia::MeasurementState::Cancelled)) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            } while (std::chrono::steady_clock::now() < deadline);
            if (measured.id != id || measured.state != octavia::MeasurementState::Complete) {
                res.status = measured.state == octavia::MeasurementState::Cancelled ? 409 : 503;
                res.set_content("{\"error\":\"measurement did not complete on engine frames\"}",
                    "application/json");
                return;
            }
            res.set_content(measurementJson(measured,
                audioInputConnected[0].load(std::memory_order_relaxed),
                audioInputConnected[1].load(std::memory_order_relaxed)), "application/json");
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
            json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* bypassedJ=root?json_object_get(root,"bypassed"):NULL;
            if (!json_is_object(root) || !json_is_boolean(bypassedJ)) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"bypass requires boolean bypassed\"}","application/json"); return;
            }
            auto job=std::make_shared<BypassJob>();
            job->moduleId=std::stoll(r.matches[1].str());
            job->bypassed=json_is_true(bypassedJ); json_decref(root);
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
            if (r.body.size() > octavia::kMaxModuleStateBytes) {
                res.set_content("{\"error\":\"module state exceeds the 1 MiB safety limit\"}","application/json"); return;
            }
            auto job=std::make_shared<ModuleStateJob>(); job->type=ModuleStateJob::SET;
            job->moduleId=std::stoll(r.matches[1].str());
            job->stateJson=r.body;
            { std::unique_lock<std::mutex> lk(stateQueueMtx); stateQueue.push(job); }
            if (!waitDone(job,2000)) res.set_content("{\"error\":\"timeout\"}","application/json");
            else if (job->success) res.set_content("{\"ok\":true}","application/json");
            else res.set_content("{\"error\":"+jStr(job->error)+"}","application/json");
        });

        svr.Post(R"(/temporal-deck/(\d+)/transport)", [this](const httplib::Request& r, httplib::Response& res){
            json_error_t jerr; json_t* root=json_loads(r.body.c_str(),0,&jerr);
            json_t* actionJ=root?json_object_get(root,"action"):NULL;
            if (!json_is_object(root) || !json_is_string(actionJ)) {
                if (root) json_decref(root);
                res.set_content("{\"error\":\"transport requires string action\"}","application/json"); return;
            }
            auto job=std::make_shared<TemporalDeckJob>(); job->moduleId=std::stoll(r.matches[1].str());
            std::string action=json_string_value(actionJ);
            if (action=="load") {
                json_t* pathJ=json_object_get(root,"path");
                if (!json_is_string(pathJ) || !*json_string_value(pathJ)) {
                    json_decref(root); res.set_content("{\"error\":\"load requires non-empty string path\"}","application/json"); return;
                }
                job->type=TemporalDeckJob::LOAD; job->path=json_string_value(pathJ);
            }
            else if (action=="play") job->type=TemporalDeckJob::PLAY;
            else if (action=="stop_rewind") job->type=TemporalDeckJob::STOP_REWIND;
            else if (action=="seek") {
                json_t* positionJ=json_object_get(root,"position");
                if (!json_is_number(positionJ)) { json_decref(root); res.set_content("{\"error\":\"seek requires numeric position\"}","application/json"); return; }
                const float position=(float)json_number_value(positionJ);
                if (!std::isfinite(position)) { json_decref(root); res.set_content("{\"error\":\"seek position must be finite\"}","application/json"); return; }
                job->type=TemporalDeckJob::SEEK; job->position=rack::math::clamp(position,0.f,1.f);
            }
            else if (action=="set_loop") {
                json_t* enabledJ=json_object_get(root,"enabled");
                if (!json_is_boolean(enabledJ)) { json_decref(root); res.set_content("{\"error\":\"set_loop requires boolean enabled\"}","application/json"); return; }
                job->type=TemporalDeckJob::SET_LOOP; job->enabled=json_is_true(enabledJ);
            }
            else if (action=="status") job->type=TemporalDeckJob::STATUS;
            else { json_decref(root); res.set_content("{\"error\":\"action must be load, play, stop_rewind, seek, set_loop, or status\"}","application/json"); return; }
            json_decref(root);
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
                    power += module->lm.blocks[j][i % MASTER_METER_BLOCKS].load(std::memory_order_relaxed);
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
        nvgBeginPath(vg);
        nvgRect(vg, bounds.pos.x, bounds.pos.y, bounds.size.x, bounds.size.y);
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
            nvgRect(vg, channelX, fillPos.y, channelWidth, fillSize.y);
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
    widget::FramebufferWidget* statusFramebuffer = nullptr;
    bool statusFramebufferStateInitialized = false;
    bool lastStatusFramebufferServerRunning = false;
    Vec titleLabelMm{19.05f, 7.5f};
    Vec portLabelMm{4.5f, 55.5f};
    Vec portValueLabelMm{26.f, 55.5f};
    Vec readActivityLabelMm{11.f, 56.f};
    Vec writeActivityLabelMm{19.f, 56.f};
    Vec startLabelMm{4.5f, 66.f};
    Vec masterLabelLMm{11.f, 120.5f};
    Vec masterLabelRMm{28.f, 120.5f};
    std::array<Vec, 4> monitorLabelMm{{
        Vec(44.f, 20.f), Vec(44.f, 42.f), Vec(44.f, 64.f), Vec(44.f, 86.f)
    }};

    void step() override {
        ModuleWidget::step();
        if (!module) return;
        Octavia* m = static_cast<Octavia*>(module);
        const bool serverRunning = m->serverRunning.load(std::memory_order_relaxed);
        if (!statusFramebufferStateInitialized
                || serverRunning != lastStatusFramebufferServerRunning) {
            statusFramebufferStateInitialized = true;
            lastStatusFramebufferServerRunning = serverRunning;
            if (statusFramebuffer) statusFramebuffer->setDirty();
        }
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
        addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
        addChild(createWidget<CyanOrbScrew>(Vec(
            box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        auto anchorPoint = [&](const char* id, const Vec& fallbackMm) {
            Vec result;
            if (!panel_svg::loadPointFromSvgMm(panelPath, id, &result)) result = fallbackMm;
            return result;
        };
        auto anchorPointWithLegacy = [&](const char* id, const char* legacyId,
                                         const Vec& fallbackMm) {
            Vec result;
            if (!panel_svg::loadPointFromSvgMm(panelPath, id, &result)
                    && !panel_svg::loadPointFromSvgMm(panelPath, legacyId, &result))
                result = fallbackMm;
            return result;
        };
        titleLabelMm = anchorPoint("TITLE_LABEL", titleLabelMm);
        portLabelMm = anchorPoint("PORT_LABEL", portLabelMm);
        portValueLabelMm = anchorPoint("PORT_VALUE_LABEL", portValueLabelMm);
        readActivityLabelMm = anchorPoint("READ_ACTIVITY_LABEL", readActivityLabelMm);
        writeActivityLabelMm = anchorPoint("WRITE_ACTIVITY_LABEL", writeActivityLabelMm);
        startLabelMm = anchorPoint("START_LABEL", startLabelMm);
        masterLabelLMm = anchorPointWithLegacy("MASTER_L_LABEL", "AUDIO_LABEL_L", masterLabelLMm);
        masterLabelRMm = anchorPointWithLegacy("MASTER_R_LABEL", "AUDIO_LABEL_R", masterLabelRMm);
        static const char* monitorLabelAnchors[] = {
            "MONITOR_A_LABEL", "MONITOR_B_LABEL", "MONITOR_C_LABEL", "MONITOR_D_LABEL"
        };
        for (int monitor = 0; monitor < 4; ++monitor)
            monitorLabelMm[monitor] = anchorPoint(monitorLabelAnchors[monitor], monitorLabelMm[monitor]);

        math::Rect statusRectMm(Vec(0.74f, 13.5f), Vec(29.f, 36.f));
        panel_svg::loadRectFromSvgMm(panelPath, "OCTOPUS_STATUS", &statusRectMm);
        statusFramebuffer = new widget::FramebufferWidget;
        statusFramebuffer->box.pos = mm2px(statusRectMm.pos);
        statusFramebuffer->box.size = mm2px(statusRectMm.size);
        OctaviaStatusWidget* status = new OctaviaStatusWidget(module);
        status->box.size = statusFramebuffer->box.size;
        statusFramebuffer->addChild(status);
        addChild(statusFramebuffer);

        addChild(createLightCentered<SmallAperture<GreenApertureLight>>(
            mm2px(anchorPoint("READ_ACTIVITY_LIGHT", Vec(11.f, 51.f))), module, Octavia::READ_ACTIVITY_LIGHT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(
            mm2px(anchorPoint("WRITE_ACTIVITY_LIGHT", Vec(19.f, 51.f))), module, Octavia::WRITE_ACTIVITY_LIGHT));

        OctaviaMeterWidget* meter = new OctaviaMeterWidget(module);
        math::Rect meterRectMm(Vec(3.f, 72.f), Vec(24.48f, 29.f));
        panel_svg::loadRectFromSvgMm(panelPath, "LOUDNESS_METERS", &meterRectMm);
        meter->box.pos = mm2px(meterRectMm.pos);
        meter->box.size = mm2px(meterRectMm.size);
        addChild(meter);

        addParam(createParamCentered<SmallGoldButton>(
            mm2px(anchorPoint("START_PARAM", Vec(23.f, 66.f))), module, Octavia::START_PARAM));

        // Persistent Master listening inputs. The legacy SVG anchors remain valid while
        // custom/intermediate panels migrate to the semantic Master names.
        addInput(createInputCentered<Magitek2InputJack>(
            mm2px(anchorPointWithLegacy("MASTER_L_INPUT", "AUDIO_IN_L", Vec(11.f, 110.f))),
            module, Octavia::MASTER_L_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(
            mm2px(anchorPointWithLegacy("MASTER_R_INPUT", "AUDIO_IN_R", Vec(28.f, 110.f))),
            module, Octavia::MASTER_R_INPUT));

        static const char* monitorInputAnchors[] = {
            "MONITOR_A_INPUT", "MONITOR_B_INPUT", "MONITOR_C_INPUT", "MONITOR_D_INPUT"
        };
        static const char* monitorLightAnchors[] = {
            "MONITOR_A_LIGHT", "MONITOR_B_LIGHT", "MONITOR_C_LIGHT", "MONITOR_D_LIGHT"
        };
        for (int monitor = 0; monitor < 4; ++monitor) {
            const float y = 27.f + 22.f * monitor;
            addInput(createInputCentered<Magitek2InputJack>(
                mm2px(anchorPoint(monitorInputAnchors[monitor], Vec(44.f, y))), module,
                Octavia::MONITOR_A_INPUT + monitor));
            addChild(createLightCentered<SmallAperture<TealApertureLight>>(
                mm2px(anchorPoint(monitorLightAnchors[monitor], Vec(37.5f, y))), module,
                Octavia::MONITOR_A_LIGHT + monitor));
        }
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
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg,WHITE);
        nvgText(args.vg, mm2px(portLabelMm).x, mm2px(portLabelMm).y, "Port", NULL);
        const std::string portText = std::to_string(octaviaPort());
        nvgText(args.vg, mm2px(portValueLabelMm).x, mm2px(portValueLabelMm).y, portText.c_str(), NULL);

        nvgFontSize(args.vg,7.f);
        nvgText(args.vg, mm2px(readActivityLabelMm).x, mm2px(readActivityLabelMm).y, "READ", NULL);
        nvgText(args.vg, mm2px(writeActivityLabelMm).x, mm2px(writeActivityLabelMm).y, "WRITE", NULL);

        nvgTextAlign(args.vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        nvgFillColor(args.vg,WHITE);
        nvgText(args.vg, mm2px(startLabelMm).x, mm2px(startLabelMm).y, "Start", NULL);

        // ── Zone 3: Master and independent monitor labels ────────────────────
        nvgFontSize(args.vg,7.f); nvgFillColor(args.vg,WHITE);
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        nvgText(args.vg, mm2px(masterLabelLMm).x, mm2px(masterLabelLMm).y, "MASTER L", NULL);
        nvgText(args.vg, mm2px(masterLabelRMm).x, mm2px(masterLabelRMm).y, "MASTER R", NULL);
        static const char* monitorLabels[] = {"A", "B", "C", "D"};
        for (int monitor = 0; monitor < 4; ++monitor)
            nvgText(args.vg, mm2px(monitorLabelMm[monitor]).x,
                mm2px(monitorLabelMm[monitor]).y, monitorLabels[monitor], NULL);
    }
};

Model* modelOctavia = createModel<Octavia, OctaviaWidget>("Octavia");
