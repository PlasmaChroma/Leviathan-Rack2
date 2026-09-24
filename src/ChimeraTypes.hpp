#pragma once

#include <cstdint>

namespace chimera {

static const std::uint32_t kCoreRate = 48000;
static const std::uint32_t kMaxReelFrames = 8352000;
static const std::uint16_t kMaxSplices = 300;
static const std::uint8_t kReelSlots = 32;
static const std::uint8_t kMusicalVoices = 4;
static const std::uint16_t kPageFrames = 256;
static const std::uint32_t kMaxPages = 32625;
static const std::uint32_t kDspProfile = 1;

enum ParamId {
    SOS_PARAM, GENE_SIZE_PARAM, VARISPEED_PARAM, MORPH_PARAM,
    SLIDE_PARAM, ORGANIZE_PARAM, GENE_ATT_PARAM, VARISPEED_ATT_PARAM,
    SLIDE_ATT_PARAM, REC_PARAM, SPLICE_PARAM, SHIFT_PARAM, NUM_PARAMS
};
enum InputId {
    AUDIO_L_INPUT, AUDIO_R_INPUT, SOS_CV_INPUT, GENE_SIZE_CV_INPUT,
    VARISPEED_CV_INPUT, MORPH_CV_INPUT, SLIDE_CV_INPUT, ORGANIZE_CV_INPUT,
    CLOCK_INPUT, PLAY_INPUT, REC_INPUT, SPLICE_INPUT, SHIFT_INPUT, NUM_INPUTS
};
enum OutputId {
    AUDIO_L_OUTPUT, AUDIO_R_OUTPUT, CV_OUTPUT, EOSG_OUTPUT, NUM_OUTPUTS
};
enum LightId {
    REC_LIGHT, REC_ARMED_LIGHT, PLAY_LIGHT, PENDING_LIGHT, CLOCK_LIGHT,
    PM_LIGHT, IO_BUSY_LIGHT, CLIP_LIGHT, ERROR_LIGHT, NUM_LIGHTS
};

struct StereoFrame { float l; float r; };
struct Region { std::uint32_t begin; std::uint32_t end; }; // [begin,end)

struct ControlFrame {
    float sos, gene, rate, morph, slide, organize;
    float geneAtt, rateAtt, slideAtt;
    float sosCv, geneCv, rateCv, morphCv, slideCv, organizeCv;
    bool sosPatched;
};

enum FrameEventBits {
    kOnsetEvent = 1u << 0,
    kClockEvent = 1u << 1,
    kPlayEvent = 1u << 2,
    kRecordEvent = 1u << 3,
    kSpliceEvent = 1u << 4,
    kShiftEvent = 1u << 5
};

struct CoreInput {
    StereoFrame live;
    ControlFrame controls;
    std::uint32_t events;
    float pmRightVolts;
    bool pmRightConnected;
};

struct CoreOutput {
    StereoFrame live; // Sanitized monitoring input; musical output arrives in Phase 3.
    float sos, gene, rate, morph, slide, organize;
    std::uint64_t frame;
    std::uint64_t onsetCount;
    std::uint32_t randomState;
};

static_assert(NUM_PARAMS == 12 && NUM_INPUTS == 13 && NUM_OUTPUTS == 4 && NUM_LIGHTS == 9,
              "Chimera v1 Rack schema changed");
static_assert(kMaxReelFrames / kPageFrames == kMaxPages, "Chimera page capacity changed");
static_assert(sizeof(StereoFrame) == 8, "Chimera stereo frame must be float32 x2");

} // namespace chimera
