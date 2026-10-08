#include "../reconstruction/clock_sync_components.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace mp86_audit;

void test_tempo_ratios_properties() {
    std::cout << "Testing 13 tempo delay ratios..." << std::endl;
    assert(tempo_delay_ratios.size() == 13);
    assert(tempo_delay_ratios[6] == 1.0f); // Center token must be exact unity

    // Check monotonic ordering
    for (size_t i = 1; i < tempo_delay_ratios.size(); ++i) {
        assert(tempo_delay_ratios[i] > tempo_delay_ratios[i - 1]);
    }

    // Check specific harmonic relationships
    assert(tempo_delay_ratios[0] == 0.5f);
    assert(std::abs(tempo_delay_ratios[1] - (4.0f / 7.0f)) < 1e-6f);
    assert(std::abs(tempo_delay_ratios[2] - 0.6f) < 1e-6f);
    assert(std::abs(tempo_delay_ratios[3] - (2.0f / 3.0f)) < 1e-6f);
    assert(tempo_delay_ratios[4] == 0.75f);
    assert(std::abs(tempo_delay_ratios[5] - 0.8f) < 1e-6f);
    assert(tempo_delay_ratios[7] == 1.25f);
    assert(std::abs(tempo_delay_ratios[8] - (4.0f / 3.0f)) < 1e-6f);
    assert(tempo_delay_ratios[9] == 1.5f);
    assert(std::abs(tempo_delay_ratios[10] - (5.0f / 3.0f)) < 1e-6f);
    assert(tempo_delay_ratios[11] == 1.75f);
    assert(tempo_delay_ratios[12] == 2.0f);

    // Check complementary symmetry around center (token 6)
    // token 0 (1/2) and token 12 (2/1)
    assert(std::abs(tempo_delay_ratios[0] * tempo_delay_ratios[12] - 1.0f) < 1e-6f);
    // token 3 (2/3) and token 9 (3/2)
    assert(std::abs(tempo_delay_ratios[3] * tempo_delay_ratios[9] - 1.0f) < 1e-6f);
    // token 4 (3/4) and token 8 (4/3)
    assert(std::abs(tempo_delay_ratios[4] * tempo_delay_ratios[8] - 1.0f) < 1e-6f);
}

void test_rate_token_quantization() {
    std::cout << "Testing Rate ADC to token quantization..." << std::endl;
    // Bounds
    assert(quantize_rate_token(0) == 0);
    assert(quantize_rate_token(4095) == 12);

    // Center value (2048) -> 2048 * 13 / 4096 = 6.5 -> truncates to 6 (Unity)
    assert(quantize_rate_token(2048) == 6);

    // Check monotonic progression across 0..4095
    uint32_t last_token = 0;
    for (uint32_t adc = 0; adc <= 4095; ++adc) {
        uint32_t token = quantize_rate_token(adc);
        assert(token >= last_token);
        assert(token <= 12);
        last_token = token;
    }
}

void test_clock_base_interval() {
    std::cout << "Testing clock base interval calculation..." << std::endl;
    // 6000 blocks at 12kHz = 0.5s = 24000 audio samples
    // s14 = 2.0 + 4.0 * 6000 = 24002.0
    // Candidate checks:
    // candidate with rate_aux[1] (0.0010208214) = 24002.0 * 0.0010208214 = 24.50... (> 8.0)
    // candidate with rate_aux[2] (0.00025520535) = 24002.0 * 0.00025520535 = 6.125... (in [2.0, 8.0]!)
    float interval = calculate_clock_base_interval(6000);
    assert(interval >= 2.0f && interval <= 8.0f);
    assert(std::abs(interval - 6.125438f) < 0.01f);
}

void test_clock_sync_jitter_and_resync() {
    std::cout << "Testing clock jitter tolerance and phase re-sync..." << std::endl;
    ClockSyncState sync;
    uint32_t token_A = 6, token_B = 6;
    uint32_t current_token = 6;

    sync.last_accepted_timestamp = 1000;
    // Initial pulse establishes first period (7000 - 1000 = 6000)
    bool resync = sync.on_clock_pulse(7000, current_token, token_A, token_B, 0, 0, 0, 0);
    assert(sync.stored_period == 6000);
    assert(sync.clock_mode == 1);

    // Second pulse with small jitter within 6000 >> 7 = 46 counts (e.g. period = 6030)
    resync = sync.on_clock_pulse(7000 + 6030, current_token, token_A, token_B, 0, 0, 0, 0);
    assert(!resync); // Should NOT trigger resync
    assert(sync.stored_period == 6000); // Stored period maintained

    // Fourth pulse with major tempo change (e.g. period = 3000, delta = 3030 > 46)
    current_token = 8;
    resync = sync.on_clock_pulse(7000 + 6030 + 3000, current_token, token_A, token_B, 0, 0, 0, 0);
    assert(resync); // MUST trigger phase re-sync
    assert(sync.stored_period == 3000); // Updated to new period
    assert(token_A == 8); // Tokens updated to current_token
    assert(token_B == 8);

    // If head is in mid-crossfade (status == 2), resync should be suppressed
    resync = sync.on_clock_pulse(7000 + 6030 + 3000 + 1500, current_token, token_A, token_B, 2, 0, 0, 0);
    assert(!resync); // Suppressed because status_A == 2
}

void test_clocked_delay_targets() {
    std::cout << "Testing delay target evaluation (normal vs ping-pong/skew)..." << std::endl;
    const float base = 1000.0f;
    float target_A = 0.0f, target_B = 0.0f;

    // Normal mode with token 4 (3/4 = 0.75): both channels should match
    compute_clocked_delay_targets(base, 4, 4, false, target_A, target_B);
    assert(target_A == 750.0f);
    assert(target_B == 750.0f);

    // Ping-pong / Skew mode with token 4:
    // Pair A uses token 4 (0.75)
    // Pair B mirrors around 6: 12 - 4 = 8 (4/3 = 1.3333334)
    compute_clocked_delay_targets(base, 4, 4, true, target_A, target_B);
    assert(target_A == 750.0f);
    assert(std::abs(target_B - (1000.0f * (4.0f / 3.0f))) < 0.01f);

    // Center token 6: both channels remain 1000.0f
    compute_clocked_delay_targets(base, 6, 6, true, target_A, target_B);
    assert(target_A == 1000.0f);
    assert(target_B == 1000.0f);
}

void test_clock_timeout() {
    std::cout << "Testing clock timeout logic..." << std::endl;
    ClockSyncState sync;
    uint32_t token_A = 6, token_B = 6;
    sync.on_clock_pulse(1000, 6, token_A, token_B, 0, 0, 0, 0);
    assert(sync.clock_mode == 1);

    // Within 48,000 blocks: still active
    sync.check_timeout(1000 + 40000, false);
    assert(sync.overdue == 0);
    assert(sync.clock_mode == 1);

    // Overdue (> 48,000 blocks): sets overdue flag
    sync.check_timeout(1000 + 49000, false);
    assert(sync.overdue == 1);
    assert(sync.clock_mode == 1); // Not cleared until manual rate change

    // Manual rate change while overdue drops clock mode
    sync.check_timeout(1000 + 50000, true);
    assert(sync.clock_mode == 0);
}

int main() {
    std::cout << "Running MP86 Clock Synchronization and Tempo Ratio spec tests..." << std::endl;
    test_tempo_ratios_properties();
    test_rate_token_quantization();
    test_clock_base_interval();
    test_clock_sync_jitter_and_resync();
    test_clocked_delay_targets();
    test_clock_timeout();
    std::cout << "ALL CLOCK SYNC SPEC TESTS PASSED!" << std::endl;
    return 0;
}
