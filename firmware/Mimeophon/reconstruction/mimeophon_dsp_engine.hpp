#pragma once
// MP86 Closed-Loop DSP Engine Reference Implementation
// Unifies:
// - Delay read & non-linear slew (delay_read_components.hpp)
// - Color filter cascade & DC blocker (color_feedback_components.hpp)
// - Halo 8x8 Hadamard diffusion network (audited_dsp_components.hpp)
// - Modulation scheduler & expiry triggers (modulation_components.hpp)
// - Hold transport & retargeting window wrap (hold_control_components.hpp)
// - Clock synchronization & 13-ratio quantization (clock_sync_components.hpp)

#include "audited_dsp_components.hpp"
#include "clock_sync_components.hpp"
#include "color_feedback_components.hpp"
#include "delay_read_components.hpp"
#include "event_components.hpp"
#include "exact_tables.hpp"
#include "hold_control_components.hpp"
#include "modulation_components.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace mp86_audit {

struct ControlInputs {
    float mix = 0.5f;          // 0..1
    float zone = 0.0f;         // 0..7 (continuous or discrete)
    float repeats = 0.5f;      // 0..1
    float color = 0.5f;        // 0..1
    float halo = 0.0f;         // 0..1
    float rate = 0.5f;         // 0..1
    float microrate = 0.0f;    // -1..1 or bipolar fine

    bool hold = false;
    bool flip = false;
    bool skew = false;
    bool ping_pong = false;
    bool clock_pulse = false;
    bool stereo_in = true;
};

class MimeophonDspEngine {
public:
    static constexpr uint32_t MAIN_RING_SIZE = 2097152; // 2^21 floats (8 MiB each)
    static constexpr uint32_t MAIN_RING_MASK = 0x1fffff;
    static constexpr uint32_t AUX_RING_SIZE  = 32768;   // 2^15 floats
    static constexpr uint32_t AUX_RING_MASK  = 32767;

    MimeophonDspEngine() {
        main_ring_L.resize(MAIN_RING_SIZE, 0.0f);
        main_ring_R.resize(MAIN_RING_SIZE, 0.0f);
        reset();
    }

    void reset() {
        std::fill(main_ring_L.begin(), main_ring_L.end(), 0.0f);
        std::fill(main_ring_R.begin(), main_ring_R.end(), 0.0f);
        aux_ring.fill(0.0f);
        allpass_969.fill(0.0f);
        allpass_803.fill(0.0f);
        allpass_1236.fill(0.0f);
        allpass_1511.fill(0.0f);

        cursor_main = 0;
        cursor_aux = 0;
        cursor_969 = 0;
        cursor_803 = 0;
        cursor_1236 = 0;
        cursor_1511 = 0;

        // Initialize 4 delay heads (0/2 on L, 1/3 on R)
        heads[0] = {1, 100.0f, 0.0f, 0.0f, 0, 0, 1, 0.0f, 1.0f, 0, 0};
        heads[1] = {1, 100.0f, 0.0f, 0.0f, 0, 0, 1, 0.0f, 1.0f, 0, 0};
        heads[2] = {0, 100.0f, 0.0f, 0.0f, 0, 0, 1, 0.0f, 0.0f, 0, 0};
        heads[3] = {0, 100.0f, 0.0f, 0.0f, 0, 0, 1, 0.0f, 0.0f, 0, 0};

        color_channel_L = {};
        color_channel_R = {};
        halo_mod = {};
        hold_transport = {};
        clock_sync = {};
        expiry_events = {};
        halo_injection_branches = {};

        dc_blocker_L = {0.0f, 0.0f};
        dc_blocker_R = {0.0f, 0.0f};

        squelch_envelope = 0.0f;
        smoothed_mix = 0.5f;
        smoothed_repeats_gain = 0.5f;
        smoothed_halo_gain = 0.0f;
        smoothed_halo_amount = 0.0f;
        mode_blend_B = 0.0f;

        hadamard_prev.fill(0.0f);
        recording_L = 0.0f;
        recording_R = 0.0f;

        block_counter = 0;
    }

    // Process a 4-stereo-frame block (the native MP86 quantum)
    void process_block_4(const float inL[4], const float inR[4],
                         float outL[4], float outR[4],
                         const ControlInputs& controls) {
        ++block_counter;

        // 1. Block-rate Control Smoothing (Mix, Repeats, Halo matrix gain)
        const uint32_t repeats_idx = std::clamp(static_cast<uint32_t>(controls.repeats * 127.0f), 0u, 127u);
        const float target_repeats_gain = mp86_tables::repeats_gain[repeats_idx];
        smoothed_repeats_gain += 0.001f * (target_repeats_gain - smoothed_repeats_gain);

        const uint32_t halo_idx = std::clamp(static_cast<uint32_t>(controls.halo * 127.0f), 0u, 127u);
        const float target_halo_gain = mp86_tables::halo_matrix_gain[halo_idx];
        smoothed_halo_gain += 0.001f * (target_halo_gain - smoothed_halo_gain);
        smoothed_halo_amount += 0.001f * (controls.halo - smoothed_halo_amount);

        const float target_mix = std::clamp(controls.mix, 0.0f, 1.0f);
        smoothed_mix += 0.001f * (target_mix - smoothed_mix);

        const float target_mode_blend = controls.hold ? 1.0f : 0.0f;
        mode_blend_B += 0.001f * (target_mode_blend - mode_blend_B);

        // 2. Zone Selection & Base Delay Scaling
        const int active_zone = std::clamp(static_cast<int>(controls.zone), 0, 7);
        const float zone_scale = mp86_tables::zone_transition_scale[active_zone];
        const float zone_base_delay = mp86_tables::zone_minimum_samples[active_zone];
        const float zone_slew = mp86_tables::zone_slew_coefficient[active_zone];

        // 3. Clock Synchronization & Token Evaluation
        const uint32_t raw_adc_rate = std::clamp(static_cast<uint32_t>(controls.rate * 4095.0f), 0u, 4095u);
        const uint32_t current_token = quantize_rate_token(raw_adc_rate);

        if (controls.clock_pulse) {
            uint32_t token_A = expiry_events.a.seen_token;
            uint32_t token_B = expiry_events.b.seen_token;
            bool resync = clock_sync.on_clock_pulse(block_counter, current_token,
                                                    token_A, token_B,
                                                    expiry_events.a.status, expiry_events.b.status,
                                                    controls.hold ? 1 : 0, controls.flip ? 1 : 0);
            if (resync) {
                expiry_events.counters.a = 0;
                expiry_events.counters.b = 0;
                expiry_events.a.status = 1;
                expiry_events.b.status = 1;
            }
            expiry_events.a.seen_token = token_A;
            expiry_events.b.seen_token = token_B;
        }

        // Delay target calculation
        float target_delay_A = 0.0f;
        float target_delay_B = 0.0f;

        if (clock_sync.clock_mode == 1) {
            compute_clocked_delay_targets(clock_sync.base_interval,
                                          expiry_events.a.seen_token,
                                          expiry_events.b.seen_token,
                                          controls.ping_pong || controls.skew,
                                          target_delay_A, target_delay_B);
        } else {
            // Unclocked 2-octave exponential mapping using exp2_fraction table
            const int32_t rate_int = std::clamp(static_cast<int32_t>((raw_adc_rate - 128) * 1.06666672f), 0, 4095);
            const uint32_t exp_idx = static_cast<uint32_t>(rate_int) & 0x7ff;
            float rate_mult = mp86_tables::exp2_fraction[exp_idx];
            if (rate_int >= 2048) {
                rate_mult *= 2.0f;
            }
            target_delay_A = zone_base_delay * rate_mult;
            target_delay_B = target_delay_A;

            if (controls.skew) {
                // Skew offsets rates in opposite directions by 16/15
                target_delay_A *= 1.06666672f;
                target_delay_B *= (1.0f / 1.06666672f);
            }
        }

        // 4. Sample-by-Sample Loop (4 stereo frames per block)
        const float hold_drift = controls.hold ? (controls.flip ? 1.0f : -1.0f) : 0.0f;

        for (int frame = 0; frame < 4; ++frame) {
            float s_inL = inL[frame];
            float s_inR = inR[frame];

            // A. Stereo Low-Level Squelch
            const float mag_sum = std::abs(s_inL) + std::abs(s_inR);
            squelch_envelope += 0.01f * (mag_sum - squelch_envelope);
            if (squelch_envelope < 0.0005f) {
                const float squelch_gain = squelch_envelope * squelch_envelope * 4000000.0f;
                s_inL *= squelch_gain;
                s_inR *= squelch_gain;
            }
            const float dryL = s_inL;
            const float dryR = s_inR;

            // B. Delay Counter Expiry & Modulation Updates
            expiry_events.counters.decrement(1.0f);
            if (static_cast<int32_t>(expiry_events.counters.a) < 0) {
                expiry_events.counters.a = static_cast<int32_t>(target_delay_A);
                halo_mod.perturb(false);
            }
            if (static_cast<int32_t>(expiry_events.counters.b) < 0) {
                expiry_events.counters.b = static_cast<int32_t>(target_delay_B);
                halo_mod.perturb(true);
            }
            halo_mod.advance();

            // C. Hold Transport & Retargeting Window
            hold_transport.step(controls.hold ? 1 : 0, hold_drift, MAIN_RING_MASK,
                                expiry_events.a, expiry_events.b);
            cursor_main = hold_transport.cursor;

            if (controls.hold) {
                retarget_hold(expiry_events.a, expiry_events.b, target_delay_A, target_delay_B,
                              zone_base_delay, 0.5f, zone_scale, current_token);
            }

            // D. Delay Head Tracking & Crossfading
            heads[0].delay = tracked_delay_step(heads[0].delay, target_delay_A);
            heads[2].delay = tracked_delay_step(heads[2].delay, target_delay_A);
            heads[1].delay = tracked_delay_step(heads[1].delay, target_delay_B);
            heads[3].delay = tracked_delay_step(heads[3].delay, target_delay_B);

            // Slew crossfade gains
            advance_head_gains(heads[0], heads[2], zone_slew);
            advance_head_gains(heads[1], heads[3], zone_slew);

            // E. Main Delay Reads (4-Tap Quadratic Interpolation)
            float readL = 0.0f;
            float readR = 0.0f;

            if (heads[0].gain > 0.0f) {
                readL += read_delay_head(heads[0], main_ring_L.data(), MAIN_RING_MASK, cursor_main, 0.0f);
            }
            if (heads[2].gain > 0.0f) {
                readL += read_delay_head(heads[2], main_ring_L.data(), MAIN_RING_MASK, cursor_main, 0.0f);
            }

            if (heads[1].gain > 0.0f) {
                readR += read_delay_head(heads[1], main_ring_R.data(), MAIN_RING_MASK, cursor_main, 0.0f);
            }
            if (heads[3].gain > 0.0f) {
                readR += read_delay_head(heads[3], main_ring_R.data(), MAIN_RING_MASK, cursor_main, 0.0f);
            }

            // F. Feedback / Ping-Pong Routing Matrix
            float cond_inL = 0.0f;
            float cond_inR = 0.0f;

            if (!controls.ping_pong) {
                cond_inL = dryL + smoothed_repeats_gain * readL;
                cond_inR = dryR + smoothed_repeats_gain * readR;
            } else {
                if (controls.stereo_in) {
                    cond_inL = dryL + smoothed_repeats_gain * readR;
                    cond_inR = dryR + smoothed_repeats_gain * readL;
                } else {
                    cond_inL = 0.5f * (dryL + dryR) + smoothed_repeats_gain * readR;
                    cond_inR = smoothed_repeats_gain * readL;
                }
            }

            // Blend towards Hadamard previous output if Halo amount > 0.22
            if (smoothed_halo_amount > 0.22f) {
                cond_inL = cond_inL * (1.22f - smoothed_halo_amount) + hadamard_prev[0] * (smoothed_halo_amount - 0.22f);
                cond_inR = cond_inR * (1.22f - smoothed_halo_amount) + hadamard_prev[1] * (smoothed_halo_amount - 0.22f);
            }

            // Mode blend toward read
            cond_inL += mode_blend_B * (readL - cond_inL);
            cond_inR += mode_blend_B * (readR - cond_inR);

            // G. DC Blocker
            cond_inL = step_dc_blocker(dc_blocker_L, cond_inL);
            cond_inR = step_dc_blocker(dc_blocker_R, cond_inR);

            // H. Color Filter Cascade & Envelope
            const int32_t color_target = std::clamp(static_cast<int32_t>(controls.color * 254.0f), 0, 254);
            color_channel_L.state.target = color_target;
            color_channel_R.state.target = color_target;

            const float color_scale = color_envelope_scale(smoothed_repeats_gain, active_zone);
            const std::array<float, 4> poly_coeffs = {
                mp86_tables::zone_polynomial_c0[std::min(active_zone, 3)],
                mp86_tables::zone_polynomial_c1[std::min(active_zone, 3)],
                mp86_tables::zone_polynomial_c2[std::min(active_zone, 3)],
                mp86_tables::zone_polynomial_c3[std::min(active_zone, 3)]
            };

            const float color_outL = color_channel_L.process(cond_inL, color_scale, poly_coeffs);
            const float color_outR = color_channel_R.process(cond_inR, color_scale, poly_coeffs);

            // I. Recording Stores & Halo Injection
            if (!controls.hold) {
                recording_L = color_outL + mode_blend_B * (recording_L - color_outL);
                recording_R = color_outR + mode_blend_B * (recording_R - color_outR);
            }
            main_ring_L[cursor_main] = recording_L;
            main_ring_R[cursor_main] = recording_R;

            const float injectionL = 0.65f * (readL + mode_blend_B * (color_outL - readL));
            const float injectionR = 0.65f * (readR + mode_blend_B * (color_outR - readR));

            // J. Halo Auxiliary Ring & 8x8 Hadamard Diffusion
            cursor_aux = (cursor_aux - 1u) & AUX_RING_MASK;
            static constexpr std::array<uint32_t, 8> halo_read_offsets = {
                1838, 2241, 2662, 3128, 3688, 4251, 4861, 5539
            };
            std::array<float, 8> h_in;
            for (size_t k = 0; k < 8; ++k) {
                h_in[k] = aux_ring[(cursor_aux + halo_read_offsets[k]) & AUX_RING_MASK];
            }
            const auto h_out = hadamard8(h_in);
            hadamard_prev = h_out;

            // Delayed allpass reads
            const float del_969 = read_linear_interpolated(allpass_969.data(), 969, cursor_969, halo_mod.a.position);
            const float del_803 = read_linear_interpolated(allpass_803.data(), 803, cursor_803, halo_mod.b.position);
            const float del_1236 = allpass_1236[cursor_1236];
            const float del_1511 = allpass_1511[cursor_1511];

            const std::array<float, 4> delayed = {del_969, del_803, del_1236, del_1511};
            const std::array<float, 2> injection = {injectionL, injectionR};

            const auto hw = halo_write_values(h_out, injection, smoothed_halo_gain, 0.5f,
                                             delayed, halo_injection_branches);

            // Store back allpass state
            allpass_969[cursor_969] = hw.allpass[0];
            cursor_969 = (cursor_969 + 1u) % 969;

            allpass_803[cursor_803] = hw.allpass[1];
            cursor_803 = (cursor_803 + 1u) % 803;

            allpass_1236[cursor_1236] = hw.allpass[2];
            cursor_1236 = (cursor_1236 + 1u) % 1236;

            allpass_1511[cursor_1511] = hw.allpass[3];
            cursor_1511 = (cursor_1511 + 1u) % 1511;

            // Store back to auxiliary ring
            aux_ring[(cursor_aux + 0) & AUX_RING_MASK]    = hw.ring[0];
            aux_ring[(cursor_aux + 1839) & AUX_RING_MASK] = hw.ring[1];
            aux_ring[(cursor_aux + 2242) & AUX_RING_MASK] = hw.ring[2];
            aux_ring[(cursor_aux + 2663) & AUX_RING_MASK] = hw.ring[3];
            aux_ring[(cursor_aux + 3129) & AUX_RING_MASK] = hw.ring[4];
            aux_ring[(cursor_aux + 3689) & AUX_RING_MASK] = hw.ring[5];
            aux_ring[(cursor_aux + 4252) & AUX_RING_MASK] = hw.ring[6];
            aux_ring[(cursor_aux + 4862) & AUX_RING_MASK] = hw.ring[7];

            // K. Wet Output Sums & Mix
            const float sumL = ((h_out[0] + h_out[2]) + h_out[4]) + h_out[6];
            const float sumR = ((h_out[1] + h_out[3]) + h_out[5]) + h_out[7];

            const float wetL = injectionL + smoothed_halo_amount * (sumL - injectionL);
            const float wetR = injectionR + smoothed_halo_amount * (sumR - injectionR);

            outL[frame] = final_mix(dryL, wetL, smoothed_mix);
            outR[frame] = final_mix(dryR, wetR, smoothed_mix);
        }
    }

private:
    static float step_dc_blocker(std::array<float, 2>& state, float in) noexcept {
        const float ap = state[0] + (-0.9999f) * (in - state[1]);
        state[0] = in;
        state[1] = ap;
        return (in - ap) * 0.5f;
    }

    static float read_linear_interpolated(const float* buffer, uint32_t length, uint32_t cursor, float fraction) noexcept {
        const float pos = static_cast<float>(cursor) + fraction * static_cast<float>(length);
        const uint32_t idx0 = static_cast<uint32_t>(pos) % length;
        const uint32_t idx1 = (idx0 + 1u) % length;
        const float frac = pos - static_cast<float>(static_cast<uint32_t>(pos));
        return buffer[idx0] + frac * (buffer[idx1] - buffer[idx0]);
    }

    std::vector<float> main_ring_L;
    std::vector<float> main_ring_R;
    std::array<float, AUX_RING_SIZE> aux_ring{};
    std::array<float, 969> allpass_969{};
    std::array<float, 803> allpass_803{};
    std::array<float, 1236> allpass_1236{};
    std::array<float, 1511> allpass_1511{};

    uint32_t cursor_main = 0;
    uint32_t cursor_aux = 0;
    uint32_t cursor_969 = 0;
    uint32_t cursor_803 = 0;
    uint32_t cursor_1236 = 0;
    uint32_t cursor_1511 = 0;

    std::array<DelayHeadLayout, 4> heads{};
    ColorChannel color_channel_L{};
    ColorChannel color_channel_R{};
    HaloModulation halo_mod{};
    HoldTransport hold_transport{};
    ClockSyncState clock_sync{};
    ExpiryEvents expiry_events{};
    std::array<HaloInjectionBranch, 2> halo_injection_branches{};

    std::array<float, 2> dc_blocker_L{};
    std::array<float, 2> dc_blocker_R{};

    float squelch_envelope = 0.0f;
    float smoothed_mix = 0.5f;
    float smoothed_repeats_gain = 0.5f;
    float smoothed_halo_gain = 0.0f;
    float smoothed_halo_amount = 0.0f;
    float mode_blend_B = 0.0f;

    std::array<float, 8> hadamard_prev{};
    float recording_L = 0.0f;
    float recording_R = 0.0f;

    uint32_t block_counter = 0;
};

} // namespace mp86_audit
