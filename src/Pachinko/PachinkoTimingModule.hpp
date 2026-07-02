#pragma once
#include "../plugin.hpp"
#include <vector>
#include <array>

struct PachinkoTimingModule : Module {
    enum ParamId {
        BALL_RATE_PARAM,
        DAMPING_PARAM,
        BALL_TO_BALL_PARAM,
        NUM_PARAMS
    };

    enum InputId {
        CLOCK_INPUT,
        RESET_INPUT,
        LATTICE_CV_INPUT,
        NUM_INPUTS
    };

    enum OutputId {
        GATE_1_OUTPUT,
        GATE_2_OUTPUT,
        GATE_3_OUTPUT,
        GATE_4_OUTPUT,
        GATE_5_OUTPUT,
        GATE_6_OUTPUT,
        GATE_7_OUTPUT,
        GATE_8_OUTPUT,
        NUM_OUTPUTS = 8
    };

    enum LightId {
        GATE_1_LIGHT,
        GATE_2_LIGHT,
        GATE_3_LIGHT,
        GATE_4_LIGHT,
        GATE_5_LIGHT,
        GATE_6_LIGHT,
        GATE_7_LIGHT,
        GATE_8_LIGHT,
        NUM_LIGHTS = 8
    };

    struct Ball {
        math::Vec position;
        math::Vec velocity;
        float lifetime = 0.0f;
        bool active = false;
    };

    struct Peg {
        math::Vec position;
        float radius = 3.0f;
    };

    enum BoardType {
        BOARD_SYMMETRIC,
        BOARD_ZIGZAG,
        BOARD_HONEYCOMB,
        BOARD_NUM_TYPES
    };

    // Configuration
    int numBuckets = 8;
    int maxBalls = 32;
    bool ballToBallCollisions = true;

    // Current state
    BoardType boardType = BOARD_SYMMETRIC;
    std::vector<Peg> pegs;
    std::vector<Ball> balls;
    std::array<float, NUM_OUTPUTS> bucketOutputs;
    std::array<float, NUM_OUTPUTS> outputDecay;
    
    // Cached output values for UI thread access (must be accessed with mutex)
    std::array<float, NUM_OUTPUTS> lastOutputValues;

    // Timing
    float ballRate = 1.0f; // balls per second
    float lastBallTime = 0.0f;
    dsp::SchmittTrigger clockTrigger;
    dsp::SchmittTrigger resetTrigger;

    PachinkoTimingModule();
    void process(const ProcessArgs& args) override;
    void onReset(const ResetEvent& e) override;
    void onRandomize(const RandomizeEvent& e) override;

    // Peg layout generation
    void generatePegs();
    void generateSymmetricPegs();
    void generateZigzagPegs();
    void generateHoneycombPegs();

    // Physics
    void updateBalls(float dt);
    void checkPegCollisions(Ball& ball);
    void checkBallCollisions();
    void resolveBallCollision(Ball& a, Ball& b);

    // Output
    void updateOutputs();

    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
};
