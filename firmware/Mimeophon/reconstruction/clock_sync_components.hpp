#pragma once
// MP86 Clock Synchronization and Tempo Ratio reference components.
// See analysis/CLOCK_AND_TEMPO_RATIOS.md for assembly evidence and derivation.

#include "exact_tables.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace mp86_audit {

// Reconstructed 13 tempo delay ratios from initialization.asm (0x08023428..0x0802348e)
inline constexpr std::array<float, 13> tempo_delay_ratios = {{
    0x1.000000p-1f, // [0] 0.5f     (1/2)
    0x1.24924ap-1f, // [1] 0.571429f (4/7)
    0x1.333334p-1f, // [2] 0.6f     (3/5)
    0x1.555556p-1f, // [3] 0.666667f (2/3)
    0x1.800000p-1f, // [4] 0.75f    (3/4)
    0x1.99999ap-1f, // [5] 0.8f     (4/5)
    0x1.000000p+0f, // [6] 1.0f     (1/1, Unity)
    0x1.400000p+0f, // [7] 1.25f    (5/4)
    0x1.555556p+0f, // [8] 1.333333f (4/3)
    0x1.800000p+0f, // [9] 1.5f     (3/2)
    0x1.aaaaaap+0f, // [10] 1.666667f (5/3)
    0x1.c00000p+0f, // [11] 1.75f    (7/4)
    0x1.000000p+1f  // [12] 2.0f     (2/1)
}};

// Quantize 12-bit Rate ADC (0..4095) into 13 tempo tokens (0x080261a2..0x080261c6)
inline uint32_t quantize_rate_token(uint32_t adc_rate) noexcept {
    // 0x3b500000 = 13.0f / 4096.0f = 0.003173828125f
    const float token_scale = 0.003173828125f;
    const float scaled = static_cast<float>(adc_rate) * token_scale;
    const int32_t token = static_cast<int32_t>(scaled);
    return static_cast<uint32_t>(std::clamp(token, 0, 12));
}

// Convert clock period (in 12 kHz blocks) into base delay interval (samples)
// Implements 0x08026562..0x08026614 using rate_aux_coefficients
inline float calculate_clock_base_interval(uint32_t period_blocks, bool prescale_div4 = false) noexcept {
    const float s14 = 2.0f + 4.0f * static_cast<float>(period_blocks);
    float selected_time = 0.0f;

    for (size_t i = 0; i < mp86_tables::rate_aux_coefficients.size(); ++i) {
        const float candidate = s14 * mp86_tables::rate_aux_coefficients[i];
        if (candidate <= 8.0f && candidate >= 2.0f) {
            selected_time = candidate;
            break;
        }
        if (i == mp86_tables::rate_aux_coefficients.size() - 1) {
            selected_time = candidate;
        }
    }

    if (prescale_div4) {
        selected_time *= 0.25f;
    }
    return selected_time;
}

struct ClockSyncState {
    uint32_t last_accepted_timestamp = 0;
    uint32_t stored_period = 0;
    uint32_t clock_mode = 0; // 0 = free, 1 = clocked
    uint32_t overdue = 0;
    float base_interval = 0.0f;

    // Checks tolerance and updates state; returns true if phase re-alignment is triggered
    bool on_clock_pulse(uint32_t current_block_timestamp, uint32_t current_token,
                        uint32_t& token_A, uint32_t& token_B,
                        uint32_t status_A, uint32_t status_B,
                        uint32_t hold_flag, uint32_t flip_flag) noexcept {
        const uint32_t elapsed = current_block_timestamp - last_accepted_timestamp;
        last_accepted_timestamp = current_block_timestamp;

        bool trigger_resync = false;

        if (stored_period > 0) {
            const int32_t error = static_cast<int32_t>(elapsed) - static_cast<int32_t>(stored_period);
            const int32_t tolerance = static_cast<int32_t>(stored_period >> 7); // +/- ~0.78%

            if (std::abs(error) > tolerance) {
                // Period changed beyond tolerance: update period and sync tokens
                stored_period = elapsed;
                token_A = current_token;
                token_B = current_token;

                // If neither pair is in mid-crossfade, trigger immediate transition
                if (status_A != 2 && status_B != 2 && hold_flag == 0 && flip_flag == 0) {
                    trigger_resync = true;
                }
            }
        } else {
            stored_period = elapsed;
        }

        base_interval = calculate_clock_base_interval(stored_period);
        clock_mode = 1;
        overdue = 0;

        return trigger_resync;
    }

    // Check clock loss timeout (48,000 blocks = 4.0 seconds at 48kHz)
    void check_timeout(uint32_t current_block_timestamp, bool manual_rate_changed) noexcept {
        if (current_block_timestamp - last_accepted_timestamp > 48000) {
            overdue = 1;
            if (manual_rate_changed) {
                clock_mode = 0; // Disengage clocked mode on manual rate change after timeout
            }
        }
    }
};

// Evaluates pair delay targets from tokens and base interval
// Implements 0x08025d94..0x08025e6c
inline void compute_clocked_delay_targets(float base_interval,
                                         uint32_t token_A, uint32_t token_B,
                                         bool ping_pong_or_skew,
                                         float& delay_target_A, float& delay_target_B) noexcept {
    const uint32_t clamped_token_A = std::clamp(token_A, 0u, 12u);
    delay_target_A = base_interval * tempo_delay_ratios[clamped_token_A];

    uint32_t effective_token_B = token_B;
    if (ping_pong_or_skew) {
        // Mirrored around token 6 (Unity): 2 * 6 - token_B = 12 - token_B
        effective_token_B = (token_B <= 12) ? (12 - token_B) : 0;
    }
    const uint32_t clamped_token_B = std::clamp(effective_token_B, 0u, 12u);
    delay_target_B = base_interval * tempo_delay_ratios[clamped_token_B];
}

} // namespace mp86_audit
