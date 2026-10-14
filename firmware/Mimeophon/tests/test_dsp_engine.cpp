#include "../reconstruction/mimeophon_dsp_engine.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace mp86_audit;

void test_engine_silence() {
    std::cout << "Testing DSP Engine: Silence input..." << std::endl;
    MimeophonDspEngine engine;
    ControlInputs controls;
    controls.mix = 0.5f;
    controls.repeats = 0.5f;
    controls.zone = 0.0f;

    float inL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float inR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float outL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float outR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    // Run for 100 blocks
    for (int b = 0; b < 100; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
        for (int i = 0; i < 4; ++i) {
            assert(std::abs(outL[i]) < 1e-6f);
            assert(std::abs(outR[i]) < 1e-6f);
        }
    }
}

void test_engine_impulse_and_repeats() {
    std::cout << "Testing DSP Engine: Impulse and feedback repeats..." << std::endl;
    MimeophonDspEngine engine;
    ControlInputs controls;
    controls.mix = 1.0f; // 100% wet
    controls.repeats = 0.7f;
    controls.zone = 0.0f;
    controls.rate = 0.5f;

    // Settle controls for 200 blocks
    float inL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float inR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float outL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float outR[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (int b = 0; b < 200; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
    }

    // Send impulse
    inL[0] = 1.0f;
    inR[0] = 1.0f;
    engine.process_block_4(inL, inR, outL, outR, controls);
    inL[0] = 0.0f;
    inR[0] = 0.0f;

    float max_echo_energy = 0.0f;
    int echo_blocks = 0;

    // Monitor for 1000 blocks
    for (int b = 0; b < 1000; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
        for (int i = 0; i < 4; ++i) {
            float energy = std::abs(outL[i]) + std::abs(outR[i]);
            if (energy > 0.01f) {
                ++echo_blocks;
                if (energy > max_echo_energy) {
                    max_echo_energy = energy;
                }
            }
        }
    }

    assert(max_echo_energy > 0.05f); // Must produce audible echo
    assert(echo_blocks > 5);         // Must produce repeating tail
    std::cout << "  Echo detected: peak = " << max_echo_energy << ", active samples = " << echo_blocks << std::endl;
}

void test_engine_dry_mix_passthrough() {
    std::cout << "Testing DSP Engine: Dry mix passthrough..." << std::endl;
    MimeophonDspEngine engine;
    ControlInputs controls;
    controls.mix = 0.0f; // 100% dry
    controls.repeats = 0.0f;

    // Settle mix smoother (0.001 per block) and squelch envelope for 8000 blocks
    float inL[4] = {0.5f, -0.3f, 0.2f, -0.4f};
    float inR[4] = {-0.5f, 0.3f, -0.2f, 0.4f};
    float outL[4], outR[4];

    for (int b = 0; b < 8000; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
    }

    // With mix = 0.0, output should match dry input (curved mix law: dry*(1 - m^2) = dry)
    for (int i = 0; i < 4; ++i) {
        assert(std::abs(outL[i] - inL[i]) < 0.01f);
        assert(std::abs(outR[i] - inR[i]) < 0.01f);
    }
}

void test_engine_ping_pong_cross_feedback() {
    std::cout << "Testing DSP Engine: Ping Pong cross feedback..." << std::endl;
    MimeophonDspEngine engine;
    ControlInputs controls;
    controls.mix = 1.0f;
    controls.repeats = 0.6f;
    controls.zone = 0.0f;
    controls.ping_pong = true;
    controls.stereo_in = false; // Mono input (Left only)

    float inL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float inR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float outL[4], outR[4];

    // Settle for 200 blocks
    for (int b = 0; b < 200; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
    }

    // Send impulse on Left input only
    inL[0] = 1.0f;
    engine.process_block_4(inL, inR, outL, outR, controls);
    inL[0] = 0.0f;

    float total_energy_L = 0.0f;
    float total_energy_R = 0.0f;

    for (int b = 0; b < 800; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
        for (int i = 0; i < 4; ++i) {
            total_energy_L += std::abs(outL[i]);
            total_energy_R += std::abs(outR[i]);
        }
    }

    // Both channels must have received significant energy due to cross-feedback bouncing
    assert(total_energy_L > 0.1f);
    assert(total_energy_R > 0.1f);
    std::cout << "  Ping pong energy: L = " << total_energy_L << ", R = " << total_energy_R << std::endl;
}

void test_engine_hold_loop() {
    std::cout << "Testing DSP Engine: Hold loop freeze..." << std::endl;
    MimeophonDspEngine engine;
    ControlInputs controls;
    controls.mix = 1.0f;
    controls.repeats = 0.5f;
    controls.zone = 0.0f;

    float inL[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float inR[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float outL[4], outR[4];

    // Settle
    for (int b = 0; b < 100; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
    }

    // Record a 10-block sine wave burst
    for (int b = 0; b < 10; ++b) {
        for (int i = 0; i < 4; ++i) {
            inL[i] = std::sin(static_cast<float>(b * 4 + i) * 0.2f);
            inR[i] = std::cos(static_cast<float>(b * 4 + i) * 0.2f);
        }
        engine.process_block_4(inL, inR, outL, outR, controls);
    }

    // Disconnect input and engage HOLD
    inL[0] = inL[1] = inL[2] = inL[3] = 0.0f;
    inR[0] = inR[1] = inR[2] = inR[3] = 0.0f;
    controls.hold = true;

    float energy_early = 0.0f;
    for (int b = 0; b < 100; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
        for (int i = 0; i < 4; ++i) {
            energy_early += std::abs(outL[i]) + std::abs(outR[i]);
        }
    }

    // Continue running under HOLD for another 500 blocks
    float energy_late = 0.0f;
    for (int b = 0; b < 500; ++b) {
        engine.process_block_4(inL, inR, outL, outR, controls);
        for (int i = 0; i < 4; ++i) {
            energy_late += std::abs(outL[i]) + std::abs(outR[i]);
        }
    }

    // Held audio should still be cycling and not decayed away
    assert(energy_late > 0.1f);
    std::cout << "  Hold loop energy: early = " << energy_early << ", late = " << energy_late << std::endl;
}

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << "Running MP86 Closed-Loop DSP Engine Verification Tests..." << std::endl;
    std::cout << "==========================================================" << std::endl;
    test_engine_silence();
    test_engine_impulse_and_repeats();
    test_engine_dry_mix_passthrough();
    test_engine_ping_pong_cross_feedback();
    test_engine_hold_loop();
    std::cout << "==========================================================" << std::endl;
    std::cout << "ALL CLOSED-LOOP DSP ENGINE TESTS PASSED!" << std::endl;
    std::cout << "==========================================================" << std::endl;
    return 0;
}
