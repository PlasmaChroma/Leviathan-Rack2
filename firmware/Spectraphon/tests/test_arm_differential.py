"""Compare authored equations against original firmware instruction slices.

Requires unicorn==2.1.4. Run directly; missing emulator is an error, not a skip.
"""
import json
import math
import random
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'reference'))
from sp67_arm import SliceMachine
from sp67_ui import ArrayButtonState, array_button_action
from sp67_ui import SharedButtonState, shared_button_action
from sp67_ui import button_actions
from sp67_ui import ButtonEdgeState, sample_button_edges
from sp67_ui import button_inputs
from sp67_ui import ClockTimerState, clock_timer_tick
from sp67_ui import AdmittedClockState, admitted_clock_action
from sp67_ui import ArraySelectionState, array_selection_action
from sp67_ui import calibration_release_action, calibration_save_return
from sp67_ui import ShiftSideState, ShiftStageState, ShiftStageResult, normal_shift_stage, resume_shift_after_reset
from sp67_ui import PassiveShiftState, shift_press_action, shift_idle_action, shift_release_finish
from sp67_ui import long_hold_step, shift_release_action, consumed_shift_route, ConsumedShiftRoute, shift_dispatch
from sp67_file_fixture import MemoryReadFileFixture, MemoryWriteFileFixture
from sp67_extended import chaos_step, noise_step, clip_a, clip_b, normal_phase_step, auxiliary_value
from sp67_extended import even_increment, b_pitch_increment
from sp67_extended import phase_step, interaction_ratios, interaction_gate, mul, add, fma
from sp67_extended import planar_coordinates, planar_deltas, planar_slot_deltas, planar_storage_deltas, linear_deltas, slide_clock_tracking
from sp67_extended import linear_coordinates, linear_storage_deltas, array_reader_addresses
from sp67_extended import default_calibration, pitch_coordinate
from sp67_extended import smoothed_control, partials_control
from sp67_extended import analysis_references, sam_detector_step, analysis_parameters_exact
from sp67_extended import SETTINGS_FIELDS, pack_settings, unpack_settings
from sp67_extended import array_save_frame, array_decode_sample_buffer
from sp67_extended import tuning_beacon_action, wav_format_classification, array_load_count, pending_operation_step
from sp67_extended import indicator_register_values
from sp67_extended import calibration_ring_step, calibration_endpoints
from sp67_extended import calibration_pitch_tables
from sp67_reference import f32, bits, polynomial_synthesis, active_terms, analysis_parameters, table, from_bits

COVERAGE = set()
COUNTS = {'normal_shift_stage_cases': 0, 'normal_shift_reset_resumptions': 0, 'normal_shift_button_tail_cases': 0, 'normal_shift_stage_reset_boundaries': 0, 'shift_release_finish_cases': 0, 'shift_passive_action_cases': 0, 'shift_dispatch_cases': 0, 'consumed_shift_route_cases': 0, 'shift_release_decision_cases': 0, 'chaos_samples': 0, 'noise_samples': 0, 'clip_pairs': 0, 'output_frames': 0, 'phase_samples': 0,
          'standard_samples': 0, 'capture_cases': 0, 'clock_callbacks': 0, 'auxiliary_cases': 0, 'mute_cases': 0,
          'even_rate_cases': 0, 'b_pitch_cases': 0, 'joined_samples': 0,
          'slow_control_cases': 0, 'cold_noise_cases': 0, 'planar_cases': 0,
          'planar_outside_logical_array_cases': 0, 'capture_ui_cases': 0,
          'interaction_button_transitions': 0, 'clock_ui_cases': 0,
          'clock_table_entries': 0, 'clock_restore_cases': 0,
          'calibration_default_cases': 0, 'pitch_calibration_cases': 0,
          'calibration_load_cases': 0, 'fast_control_cases': 0,
          'full_callbacks': 0, 'full_callback_samples': 0, 'sam_bank_samples': 0,
          'full_noise_chaos_callbacks': 0, 'full_noise_chaos_samples': 0,
          'cold_noise_callbacks': 0, 'cold_noise_samples': 0, 'cold_noise_unmuted_nan_frames': 0,
          'clock_sequence_callbacks': 0, 'clock_planar_reads': 0, 'clock_planar_outside_cases': 0,
          'mode_button_cases': 0, 'aux_button_cases': 0, 'concurrent_button_cases': 0,
          'concurrent_button_held_callbacks': 0,
          'capture_audio_callbacks': 0, 'captured_frames_checked': 0, 'captured_playback_readers': 0,
          'clock_admission_cases': 0, 'clock_timeout_cases': 0,
          'settings_pack_cases': 0, 'settings_unpack_cases': 0, 'settings_scan_cases': 0,
          'settings_default_cases': 0, 'settings_save_tail_cases': 0,
          'factory_reset_cases': 0, 'long_hold_cases': 0, 'factory_reset_gestures': 0,
          'array_selection_cases': 0, 'array_save_dispatch_cases': 0,
          'array_save_frames': 0, 'array_pcm16_readback_frames': 0,
          'mode_save_dispatch_cases': 0, 'array_sample_decode_cases': 0,
          'array_header_format_cases': 0, 'array_load_dimension_cases': 0,
          'array_load_admission_cases': 0, 'array_load_copy_cases': 0,
          'interaction_led_cases': 0, 'tuning_beacon_cases': 0, 'linear_reader_cases': 0,
          'full_sao_callbacks': 0, 'full_sao_samples': 0, 'full_sao_array_switches': 0,
          'slide_clock_offset_reset_cases': 0,
          'wav_parser_signature_cases': 0, 'wav_parser_format_cases': 0,
          'wav_parser_chunk_cases': 0, 'wav_parser_termination_cases': 0,
          'array_load_count_cases': 0, 'array_read_window_cases': 0,
          'array_stereo_load_sequences': 0, 'array_initial_descriptors': 0,
          'planar_retained_slot_cases': 0, 'planar_retained_tail_changes': 0,
          'capture_shrink_offset_sequences': 0, 'capture_shrink_cross_slot_reads': 0,
          'capture_shrink_mode_entry_sequences': 0, 'capture_shrink_manual_takeovers': 0,
          'pending_operation_prefix_cases': 0, 'full_sao_recovery_callbacks': 0,
          'linear_physical_storage_cases': 0, 'linear_preceding_frame_cases': 0,
          'initializer_enable_sequences': 0, 'disabled_audio_callbacks': 0,
          'disabled_audio_stale_captures': 0, 'startup_noise_callbacks': 0,
          'startup_noise_samples': 0, 'startup_noise_unmuted_nan_frames': 0,
          'complete_wav_parser_files': 0, 'complete_wav_parser_io_errors': 0,
          'complete_wav_parser_edge_files': 0, 'parsed_array_load_sequences': 0,
          'complete_array_save_files': 0, 'saved_array_reload_sequences': 0,
          'array_save_failed_payload_writes': 0, 'array_save_short_payload_writes': 0,
          'multi_slot_array_save_batches': 0, 'indicator_priority_cases': 0,
          'indicator_mode_aux_cases': 0, 'calibration_boot_dispatch_cases': 0,
          'calibration_release_cases': 0, 'calibration_save_dispatch_cases': 0,
          'calibration_post_save_cases': 0, 'calibration_stable_callbacks': 0,
          'calibration_slow_ring_updates': 0, 'calibration_fast_ring_updates': 0,
          'calibration_endpoint_cases': 0, 'calibration_pitch_cases': 0,
          'calibration_pitch_sequence_steps': 0, 'calibration_pitch_ui_advances': 0,
          'generated_pitch_coordinate_cases': 0, 'concurrent_clock_capture_cases': 0,
          'concurrent_clock_capture_tail_calls': 0, 'clock_selected_array_cases': 0,
          'clock_capture_unsigned_stop_lengths': 0, 'persistent_clock_capture_stop_sequences': 0,
          'raw_array_reader_cases': 0, 'large_descriptor_reader_cases': 0,
          'raw_reader_outside_bank_cases': 0, 'capture_underflow_reader_prefixes': 0,
          'capture_underflow_save_dispatches': 0, 'mode_cycle_full_callbacks': 0,
          'mode_cycle_button_edges': 0, 'receive_callback_registration_cases': 0,
          'dma_to_audio_entry_cases': 0, 'receive_start_success_cases': 0,
          'started_receive_audio_dispatches': 0, 'simultaneous_shift_release_cases': 0,
          'persistent_shift_release_capture_sequences': 0, 'shift_release_shared_edge_cases': 0,
          'shift_release_shared_held_cases': 0, 'consumed_gesture_rearm_sequences': 0,
          'consumed_gesture_clock_sequences': 0, 'rearmed_clock_admissions': 0,
          'array_button_action_model_cases': 0, 'shared_button_action_model_cases': 0,
          'composed_button_action_cases': 0, 'button_edge_model_cases': 0,
          'button_input_tail_cases': 0, 'clock_timer_model_cases': 0,
          'admitted_clock_model_cases': 0, 'array_selection_model_cases': 0,
          'long_hold_model_cases': 0}
METRICS = {'mode_cycle_standard_max_absolute_error': 0., 'standard_max_absolute_error': 0., 'full_sam_audio_max_absolute_error': 0.,
           'cold_noise_first_finite_sample_a': 0, 'cold_noise_first_finite_sample_b': 0,
           'startup_noise_first_finite_sample_a': 0, 'startup_noise_first_finite_sample_b': 0,
           'full_sao_audio_max_absolute_error': 0.,
           'full_sao_min_active_terms': 60, 'full_sao_max_active_terms': 0}


class FirmwareDifferential(unittest.TestCase):
    def equal_floats(self, expected, actual, context=''):
        self.assertEqual([bits(x) for x in expected], [bits(x) for x in actual], context)

    def run_slice(self, machine, start, end):
        machine.run(start, end)
        COVERAGE.update(machine.last_trace)

    def button_action_snapshot(self,m):
        sides=[]
        for side in range(2):
            slot=m.word((0x20002430,0x20000b20)[side])
            desc=(0x20000a20,0x20000920)[side]+16*slot
            sides.append(ArrayButtonState(
                raw_sao=m.uc.mem_read(0x20002f52+side,1)[0],
                cached_sao=m.word((0x20002e94,0x20002e90)[side]),
                engine=m.word((0x20002e8c,0x20002e88)[side]),
                capture=m.word((0x20002e84,0x20002e80)[side]),
                hold=m.word(0x20002f34+4*side),
                gesture=m.uc.mem_read(0x20002f48+side,1)[0],
                lf=m.word((0x200023d0,0x20000820)[side]),
                descriptor=struct.unpack('<4I',m.uc.mem_read(desc,16)),
                cursor=m.word((0x20002e7c,0x200011a0)[side]),
                peak=m.floats((0x20002e9c,0x200011e0)[side])[0],
                policy=m.word((0x20002ed8,0x20002ed4)[side]),
                engine_mirror=m.word(0x2001349c+4*side),
                lf_mirror=m.word(0x200134ac+4*side),dirty=m.word(0x20002eb8)))
        shared=SharedButtonState(
            holds=tuple(s.hold for s in sides),gestures=tuple(s.gesture for s in sides),
            cached_sao=tuple(s.cached_sao for s in sides),
            auxiliary=tuple(m.word(a) for a in (0x20002ef8,0x20002ef4)),
            auxiliary_mirrors=tuple(m.word(a) for a in (0x20013494,0x20013498)),
            interaction=m.uc.mem_read(0x20002f54,1)[0],
            interaction_mirror=m.word(0x200134bc),dirty=m.word(0x20002eb8))
        return sides[0],sides[1],shared

    def test_chaos_both_sides(self):
        rng = random.Random(6701)
        for side in ('A', 'B'):
            m = SliceMachine()
            base = 0x200022bc + (64 if side == 'B' else 0)
            state = [0.] * 16
            for i in range(600):
                # Random states and long consecutive runs; include negative FM,
                # integer-ratio boundaries, zero controls and cycle crossings.
                if i < 100:
                    state = [f32(rng.uniform(-1., 1.)) for _ in state]
                p, focus = rng.random(), rng.random()
                slide = (i % 22) / 21 if i < 100 else .731
                if i % 51 == 0: p = 0.
                inc_o, inc_e = rng.uniform(.0001, .11), rng.uniform(.0001, .11)
                index, fm = rng.uniform(0, 60), rng.uniform(-1, 1)
                expected, output, delta = chaos_step(state, p, focus, slide, inc_o, inc_e, index, fm, side)
                m.floats(base, state)
                if side == 'A':
                    for reg, val in ((1,p),(4,focus),(6,slide),(5,inc_o),(3,inc_e)):
                        m.s(reg, val)
                    m.floats(m.sp + 28, [fm]); m.floats(0x20001140, [index])
                    self.run_slice(m, 0x0802fa04, 0x0802fc16)
                    actual = (m.s(14), m.s(13))
                    self.equal_floats([delta], [m.s(24)], f'{side} delta {i}')
                else:
                    for reg, val in ((2,p),(12,focus),(14,slide),(15,inc_o),(19,inc_e),(25,fm),(9,1.)):
                        m.s(reg, val)
                    m.floats(0x20002e50, [index])
                    self.run_slice(m, 0x0802fc96, 0x0802ff00)
                    actual = (m.s(14), m.s(6))
                self.equal_floats(expected, m.floats(base,count=16), f'{side} state {i}')
                self.equal_floats(output, actual, f'{side} output {i}')
                state = expected
                COUNTS['chaos_samples'] += 1

    def test_noise_both_sides(self):
        rng = random.Random(6702)
        for side in ('A', 'B'):
            m = SliceMachine()
            gen_base = 0x2000239c + (24 if side == 'B' else 0)
            filt_base = 0x2000233c + (24 if side == 'B' else 0)
            env_addr = 0x20002e98 if side == 'A' else 0x200011c0
            slow_addr = 0x200023d0 if side == 'A' else 0x20000820
            g, v, env, seed = [0.,0.,1.,0.,0.,0.], [0.] * 12, 1., 0
            for i in range(600):
                if i < 100:
                    g = [f32(rng.uniform(-.5,.5)) for _ in range(6)]
                    g[0] = (0., .99, 1., 10., 10.01)[i % 5]
                    g[2] = f32(rng.uniform(.5,1.5))
                    v = [f32(rng.uniform(-.5,.5)) for _ in range(12)]
                    env = f32((.01, 1.)[i % 2])
                # Stable trajectory range; random-state tests also cover top end.
                p = rng.random() if i < 100 else .4
                focus, slide = rng.random(), rng.random()
                po, pe = rng.uniform(-1.,1.), rng.uniform(-1.,1.)
                slow = bool(i % 2)
                gains = (.75, 1.25) if i % 3 == 0 else (1.,1.)
                expected = noise_step(g,v,env,seed,p,focus,slide,po,pe,slow,side,gains)
                m.floats(gen_base,g); m.floats(filt_base,v[:6]); m.floats(filt_base+48,v[6:])
                m.floats(env_addr,[env]);m.word(0x200023cc,seed);m.word(slow_addr,int(slow))
                m.word(m.sp+32,0x08043a74)
                if side == 'A':
                    m.r(3,1)
                    for reg,val in ((1,p),(4,focus),(6,slide),(18,po),(16,pe),(5,.01)):
                        m.s(reg,val)
                    self.run_slice(m,0x08031038,0x0802fc16)
                    actual_out=(m.s(14),m.s(13))
                else:
                    for reg,val in ((2,p),(12,focus),(14,slide),(9,1.)):
                        m.s(reg,val)
                    m.floats(0x20002424,[po]);m.floats(0x20002420,[pe])
                    m.floats(0x20002404,[gains[0]]);m.floats(0x20002400,[gains[1]])
                    m.word(m.sp+12,2 if i % 3 == 0 else 0)
                    self.run_slice(m,0x08031310,0x0802ff00)
                    actual_out=(m.s(14),m.s(6))
                eg,ev,ee,es,eo=expected
                self.equal_floats(eg,m.floats(gen_base,count=6),f'{side} generator {i}')
                self.equal_floats(ev,m.floats(filt_base,count=6)+m.floats(filt_base+48,count=6),f'{side} filters {i}')
                self.equal_floats([ee],m.floats(env_addr),f'{side} env {i}')
                self.assertEqual(es,m.word(0x200023cc),f'{side} seed {i}')
                self.equal_floats(eo,actual_out,f'{side} out {i}')
                g,v,env,seed=eg,ev,ee,es
                COUNTS['noise_samples'] += 1

    def test_clipping_and_output_lanes(self):
        rng=random.Random(6703)
        m=SliceMachine()
        # Verify pointer setup from actual firmware for both DMA half offsets.
        for half in (0,128):
            m.r(9,half);m.word(m.sp+144,0x20002e90)
            self.run_slice(m,0x0802ec94,0x0802ed12)
            bases={92:0x30001040,96:0x30000c40,100:0x30000840,104:0x38000000,
                   112:0x30001044,124:0x30000c44,108:0x30000844,120:0x38000004}
            for offset,base in bases.items():self.assertEqual(m.word(m.sp+offset),base+half*4)
            for i in range(100):
                ao,ae,bo,be=[f32(rng.uniform(-2.,2.)) for _ in range(4)]
                m.s(14,ao);m.s(13,ae)
                self.run_slice(m,0x0802fc16,0x0802fc66)
                self.equal_floats([clip_a(ao),clip_a(ae)],[m.s(31),m.s(30)])
                COUNTS['clip_pairs']+=1
                carrier_b,aux_a,aux_b,carrier_a=[f32(rng.uniform(-.99,.99)) for _ in range(4)]
                m.s(6,be);m.s(14,bo);m.s(25,carrier_b);m.s(7,aux_a);m.s(13,aux_b)
                m.floats(m.sp+28,[carrier_a]);m.r(9,(i%64)*2)
                self.run_slice(m,0x080302b8,0x0803038c)
                def fixed(x):return int(x*(1<<30)) & 0xffffffff
                def auxiliary(x):return int(f32(x*f32(858993472.))) & 0xffffffff
                wanted={92:fixed(carrier_b),96:fixed(clip_a(ao)),100:fixed(clip_b(be)),104:auxiliary(aux_b),
                        112:auxiliary(aux_a),124:fixed(clip_a(ae)),108:fixed(clip_b(bo)),120:fixed(carrier_a)}
                for offset,value in wanted.items():
                    self.assertEqual(m.word(m.word(m.sp+offset)+(i%64)*8),value)
                COUNTS['output_frames']+=1

    def test_normal_phase_and_cross_fm(self):
        rng=random.Random(6704)
        m=SliceMachine()
        locations={'a_odd':0x2000241c,'a_even':0x20000900,'b_odd':0x20002424,'b_even':0x20002420,
                   'a_sine':0x2000240c,'b_sine':0x200008c0,'a_sub':0x20002418,'b_sub':0x20002414,
                   'a_analysis':0x20002410,'b_analysis':0x200008e0,'a_random':0x200023d8,
                   'b_random':0x20000840,'a_previous':0x200023dc,'b_previous':0x200023d4}
        for i in range(400):
            st={k:f32(rng.uniform(-.99,.99)) for k in locations}
            st.update(seed=rng.getrandbits(32),a_pulse=0,b_pulse=0)
            inc=[f32(rng.uniform(-.2,.2)) for _ in range(6)]
            if i%2==0:inc[1]=inc[0];inc[3]=inc[2]
            modes=(i%6,(i//6)%6)
            rates=(f32(.007),f32(.013))
            indices=(f32(rng.uniform(0,60)),f32(rng.uniform(0,60)))
            expected,sources,delta=normal_phase_step(st,inc,indices,modes,rates)
            for key,addr in locations.items():m.floats(addr,[st[key]])
            m.word(0x20002eb0,st['seed']);m.word(0x2000242c,0);m.word(0x20002428,0)
            # Independently execute the clean-sine source calculation.
            self.run_slice(m,0x0802f1da,0x0802f27c)
            self.equal_floats(sources,[m.floats(m.sp+28)[0],m.s(25)])
            ao,ae,bo,be,ai,bi=inc
            for reg,val in ((5,ao),(3,ae),(15,bo),(19,be),(11,ai),(10,bi),
                            (18,st['a_odd']),(16,st['a_even']),(28,st['b_odd']),
                            (21,st['b_even']),(24,delta),(25,sources[1]),(8,indices[1]),(12,st['b_sub'])):
                m.s(reg,val)
            for offset,key in ((60,'b_sine'),(64,'a_sine'),(68,'a_analysis'),(72,'b_analysis'),(36,'a_sub')):
                m.floats(m.sp+offset,[st[key]])
            m.word(m.sp+16,modes[0]);m.r(2,modes[1]);m.word(m.sp+40,0)
            m.floats(0x200023f8,[rates[0]]);m.floats(0x200023f4,[rates[1]])
            self.run_slice(m,0x0802ffb6,0x080301ce)
            for key,addr in locations.items():
                self.equal_floats([expected[key]],m.floats(addr),f'{key} sample {i}')
            self.assertEqual(expected['seed'],m.word(0x20002eb0))
            self.assertEqual(expected['a_pulse'],m.word(0x2000242c))
            self.assertEqual(expected['b_pulse'],m.word(0x20002428))
            COUNTS['phase_samples']+=1

    def test_standard_recurrence_and_ramps(self):
        rng=random.Random(6705)
        for side in ('A','B'):
            m=SliceMachine(double_precision=True)
            current=0x20002a40 if side=='A' else 0x20002c40
            ramp=0x20000d40 if side=='A' else 0x20000f40
            for i in range(192):
                coefficients=[0.]*64
                if i<64:coefficients[i]=f32(.1)
                else:coefficients=[f32(rng.uniform(0,.1)) for _ in range(64)]
                delta=[f32(rng.uniform(-.0001,.0001)) for _ in range(64)] if i>=128 else [0.]*64
                inc=f32((.001,.007,.025,.05625,.1125,.12)[i%6]) if i>=64 else f32(.001)
                p=f32((0.,.01,.5,1.,1.2)[i%5])
                po,pe=f32(rng.random()),f32(rng.random())
                terms=active_terms(inc)
                advanced=[f32(c+d) if j<terms else c for j,(c,d) in enumerate(zip(coefficients,delta))]
                m.floats(current,coefficients);m.floats(ramp,delta)
                # Exercise both SAM and SAO selection. Only SAO working bank ramps.
                sao=i%2
                m.floats(current+256,coefficients)
                selected=advanced if sao else coefficients
                expected=polynomial_synthesis(selected,p,po,pe,terms)
                if side=='A':
                    for reg,val in ((17,p),(18,po),(16,pe),(3,inc),(5,.01)):m.s(reg,val)
                    m.word(m.sp+24,sao);m.word(m.sp+148,0x20002ea4)
                    self.run_slice(m,0x08030cd4,0x0802fc16)
                    actual=(m.s(14),m.s(13))
                else:
                    for reg,val in ((20,p),(9,1.),(15,inc)):m.s(reg,val)
                    m.floats(0x20002424,[po]);m.floats(0x20002420,[pe])
                    m.word(m.sp+12,0);m.word(m.sp+20,sao);m.word(m.sp+188,0x20002ea0)
                    self.run_slice(m,0x08030924,0x0802ff00)
                    actual=(m.s(14),m.s(6))
                self.equal_floats(advanced,m.floats(current,count=64),f'{side} ramps {i}')
                for e,a in zip(expected,actual):
                    error=abs(e-a);METRICS['standard_max_absolute_error']=max(METRICS['standard_max_absolute_error'],error)
                    self.assertLess(error,2e-4,f'{side} standard sample {i}: {e} vs {a}')
                COUNTS['standard_samples']+=1

    def test_capture_writer_and_clock_callback(self):
        m=SliceMachine()
        for side in ('A','B'):
            if side=='A':
                bank,ptr,base,flag,countdown,peak,descriptor,selection,amps,active=\
                    0x60c01000,0x20002e7c,0x20002f74,0x20002ed8,0x20002ed0,0x20002e9c,0x20000a20,0x20002430,0x20002b40,0x20002e84
                start,stop=0x080320e8,0x0802e7e8
            else:
                bank,ptr,base,flag,countdown,peak,descriptor,selection,amps,active=\
                    0x60001000,0x200011a0,0x20002f70,0x20002ed4,0x20002ecc,0x200011e0,0x20000920,0x20000b20,0x20002d40,0x20002e80
                start,stop=0x0803219a,0x0802e7fc
            m.uc.mem_map(bank,0x50000)
            frame=[f32(j/64) for j in range(64)]
            for mode,remaining in ((0,0),(0,1),(1,0),(1,1),(1,2),(2,1)):
                for at_end in (False,True):
                    offset=65536-64 if at_end else 128
                    m.word(ptr,offset);m.word(base,bank);m.word(flag,mode);m.word(countdown,remaining)
                    m.floats(peak,[.25]);m.word(selection,0);m.floats(amps,frame);m.word(active,1)
                    for j,x in enumerate((0,17,3,99)):m.word(descriptor+j*4,x)
                    m.floats(bank+offset*4,[-9.]*64)
                    m.r(7,active);m.r(12,active)
                    self.run_slice(m,start,stop)
                    write=mode==0 or (mode==1 and remaining==1)
                    self.equal_floats(frame if write else [-9.]*64,m.floats(bank+offset*4,count=64))
                    self.assertEqual(m.word(ptr),offset+(64 if write else 0))
                    self.equal_floats([max(frame) if write else .25],m.floats(peak))
                    full=write and at_end
                    self.assertEqual(m.word(active),0 if full else 1)
                    self.assertEqual(m.word(flag),0 if full else mode)
                    self.assertEqual(m.word(descriptor+4),1024 if full else 17)
                    self.assertEqual(m.word(descriptor+8),1 if full else 3)
                    self.assertEqual(m.word(descriptor+12),99)
                    COUNTS['capture_cases']+=1
        for pin in (0,64,128,192):
            for count in (0,15,0xffffffff):
                m.word(0x20002ee0,count);m.word(0x20002ee8,count)
                m.word(0x20002edc,0);m.word(0x20002ee4,0);m.r(0,pin)
                self.run_slice(m,0x08032688,0x0803269e)
                self.assertEqual(m.word(0x20002ee0),(count+(pin==128))&0xffffffff)
                self.assertEqual(m.word(0x20002ee8),(count+(pin==64))&0xffffffff)
                self.assertEqual(m.word(0x20002edc),int(pin==128))
                self.assertEqual(m.word(0x20002ee4),int(pin==64))
                COUNTS['clock_callbacks']+=1

    def test_auxiliary_shapes_and_startup_mute(self):
        rng=random.Random(6706);m=SliceMachine()
        for side in ('A','B'):
            for mode in range(6):
                for sao in (0,1):
                    for i in range(20):
                        phase=f32(rng.random());current=f32(rng.uniform(-1,1));previous=f32(rng.uniform(-1,1))
                        env=f32(rng.random());pitch=rng.randrange(0,65536)
                        expected=auxiliary_value(side,mode,phase,current,previous,sao,env,pitch)
                        if side=='A':
                            m.word(0x20002ef8,mode);m.word(m.sp+24,sao)
                            for addr,val in ((0x20002418,phase),(0x200023d8,current),(0x200023dc,previous),(0x20002eac,env)):
                                m.floats(addr,[val])
                            self.run_slice(m,0x0802f1da,0x0802f29c);actual=m.s(7)
                        else:
                            m.word(0x20002ef4,mode);m.word(m.sp+20,sao);m.word(0x20002028,pitch)
                            for addr,val in ((0x20002414,phase),(0x20000840,current),(0x200023d4,previous),(0x20002ea8,env)):
                                m.floats(addr,[val])
                            self.run_slice(m,0x0802ff84,0x0802ffa6);actual=m.s(13)
                        self.equal_floats([expected],[actual],f'{side} auxiliary mode={mode} sao={sao}')
                        COUNTS['auxiliary_cases']+=1
        m.r(9,0);m.word(m.sp+144,0x20002e90)
        self.run_slice(m,0x0802ec94,0x0802ed12)
        for counter in (8191,8192,8193):
            m.word(0x20002ec8,1);m.r(2,counter);m.r(9,0)
            self.run_slice(m,0x080302ae,0x0803038c)
            for offset in (92,96,100,104,112,124,108,120):self.assertEqual(m.word(m.word(m.sp+offset)),0)
            self.assertEqual(m.word(0x20002ec8),int(counter<=8192))
            COUNTS['mute_cases']+=1

    def test_even_rates_and_linked_pitch(self):
        m=SliceMachine();rng=random.Random(6707)
        for side in ('A','B'):
            for raw in (-1000,0,2999,3000,3001,5000,10000,11000):
                for odd in (.00001,.001,.01,.1):
                    normalized=f32(f32(raw)*f32(.0001))
                    index=6 if side=='A' else 7
                    m.word(0x20001380+4*index,0);m.floats(0x20001200+4*index,[.0001])
                    if side=='A':
                        m.r(12,raw&0xffffffff);m.s(5,odd)
                        self.run_slice(m,0x080306da,0x0802ef10)
                        actual=m.floats(0x20000860)[0]
                    else:
                        m.r(7,raw&0xffffffff);m.s(15,odd)
                        self.run_slice(m,0x08030722,0x0802f074)
                        actual=m.s(19)
                    self.equal_floats([even_increment(odd,normalized)],[actual])
                    COUNTS['even_rate_cases']+=1
        for mode in (0,1,2):
            for slow in (False,True):
                for q in (0,2047,2048,4096,8191,12288,16383):
                    a_hz=f32(rng.uniform(16,2000))
                    m.r(3,q);m.s(15,a_hz);m.word(m.sp+32,0x08043a74)
                    m.word(m.sp+12,mode);m.word(0x20000820,int(slow))
                    self.run_slice(m,0x0802ef3e,0x0802f068)
                    self.equal_floats([b_pitch_increment(q,a_hz,bool(mode),slow)],[m.s(15)])
                    COUNTS['b_pitch_cases']+=1

    def test_joined_synthesis_interaction_and_outputs(self):
        """Post-analysis entry through phase tail/output stores, without stubs.

        Warm finite DSP state; input acquisition/analyzer/UI are outside entry.
        Both sides transition among engines and interaction modes in one run.
        """
        m=SliceMachine(double_precision=True);m.uc.mem_map(0x40000000,0x10000)
        m.r(9,0);m.word(m.sp+144,0x20002e90)
        self.run_slice(m,0x0802ec94,0x0802ed12)
        locations={'a_odd':0x2000241c,'a_even':0x20000900,'b_odd':0x20002424,'b_even':0x20002420,
                   'a_sine':0x2000240c,'b_sine':0x200008c0,'a_sub':0x20002418,'b_sub':0x20002414,
                   'a_analysis':0x20002410,'b_analysis':0x200008e0,'a_random':0x200023d8,
                   'b_random':0x20000840,'a_previous':0x200023dc,'b_previous':0x200023d4}
        st={key:0. for key in locations};st.update(seed=0,a_pulse=0,b_pulse=0)
        st.update(a_odd=f32(.98),a_even=f32(.02),b_odd=f32(.97),b_even=f32(.01),a_sub=f32(.995),b_sub=f32(.998))
        for key,addr in locations.items():m.floats(addr,[st[key]])
        chaos=[[0.]*16 for _ in range(2)]
        generators=[[0.,0.,1.,f32(.2),f32(.7),0.] for _ in range(2)]
        filters=[[0.]*12 for _ in range(2)];envelopes=[1.,1.];noise_seed=0
        coefficients=[f32(.02/(1+i)) for i in range(64)]
        for addr in (0x20002a40,0x20002c40):m.floats(addr,coefficients)
        for side in range(2):
            m.floats(0x2000239c+side*24,generators[side])
            m.floats(0x20002e98 if side==0 else 0x200011c0,[1.])
        m.word(m.sp+32,0x08043a74);m.word(m.sp+132,0x20002e5c);m.word(m.sp+136,0x20002e64)
        m.word(m.sp+148,0x20002ea4);m.word(m.sp+188,0x20002ea0)
        for i in range(900):
            interaction=(i//100)%3
            engines=((i//30)%3,(i//10)%3)
            inc=tuple(map(f32,(.009,.00451,.021,.014,.002,.003)))
            if i%19==0:inc=(inc[0],inc[0],inc[2],inc[2],inc[4],inc[5])
            indices=tuple(map(f32,(.31,.23)));aux_modes=((i//40)%6,(i//60)%6)
            aux_rates=tuple(map(f32,(.004,.007)))
            controls=((f32(.4),f32(.5),f32(.65)),(f32(.43),f32(.6),f32(.7)))
            m.word(m.sp+12,interaction);m.floats(0x20000860,[inc[1]])
            m.s(5,inc[0]);m.s(15,inc[2]);m.s(19,inc[3])
            self.run_slice(m,0x0802f07a,0x0802f09c)
            ratios=interaction_ratios(inc,interaction)
            self.equal_floats(ratios,m.floats(0x20002404)+m.floats(0x20002400))
            m.word(0x20002ef8,aux_modes[0]);m.word(0x20002ef4,aux_modes[1])
            self.run_slice(m,0x0802f1da,0x0802f27c)
            next_st,sources,delta=phase_step(st,inc,indices,aux_modes,aux_rates,interaction)
            self.equal_floats(sources,m.floats(m.sp+28)+[m.s(25)])
            m.word(m.sp+16,aux_modes[0]);m.floats(m.sp+36,[st['a_sub']])
            m.floats(m.sp+68,[st['a_analysis']]);m.floats(m.sp+72,[st['b_analysis']])
            m.floats(0x200023f8,[aux_rates[0]]);m.floats(0x200023f4,[aux_rates[1]])
            raw_b_pitch=32768;m.word(0x20002028,raw_b_pitch)
            aux_a=auxiliary_value('A',aux_modes[0],st['a_sub'],st['a_random'],st['a_previous'],1,0.)
            aux_b=auxiliary_value('B',aux_modes[1],st['b_sub'],st['b_random'],st['b_previous'],1,0.,raw_b_pitch)
            for reg,val in ((5,inc[0]),(15,inc[2]),(19,inc[3]),(11,inc[4]),(10,inc[5]),
                            (17,controls[0][0]),(20,controls[1][0]),(7,aux_a)):
                m.s(reg,val)
            for offset in (20,24):m.word(m.sp+offset,1)
            m.word(m.sp+40,i);m.r(9,0)
            outputs=[]
            for side,name in enumerate(('a','b')):
                p,x,y=controls[side];engine=engines[side]
                m.word(0x20002e8c if side==0 else 0x20002e88,engine)
                for addr,val in ((0x20000880 if side==0 else 0x200023fc,p),
                                 (0x20002e5c if side==0 else 0x20002e64,x),
                                 (0x20002e58 if side==0 else 0x20002e54,y),
                                 (0x20001140 if side==0 else 0x20002e50,indices[side])):
                    m.floats(addr,[val])
                gains=ratios if side==1 and interaction==2 else (1.,1.)
                if engine==0:
                    o=polynomial_synthesis(coefficients,p,mul(st[name+'_odd'],gains[0]),mul(st[name+'_even'],gains[1]),active_terms(inc[1 if side==0 else 2]))
                elif engine==1:
                    generators[side],filters[side],envelopes[side],noise_seed,o=noise_step(
                        generators[side],filters[side],envelopes[side],noise_seed,p,x,y,
                        st[name+'_odd'],st[name+'_even'],False,name.upper(),gains)
                else:
                    chaos[side],o,_=chaos_step(chaos[side],p,x,y,inc[side*2],inc[side*2+1],indices[side],sources[side],name.upper())
                if side==1 and interaction==2:o=(mul(o[0],interaction_gate(st['a_odd'])),mul(o[1],interaction_gate(st['a_even'])))
                outputs.append(tuple((clip_a if side==0 else clip_b)(v) for v in o))
            self.run_slice(m,0x0802f9bc,0x0803038c)
            for key,addr in locations.items():self.equal_floats([next_st[key]],m.floats(addr),f'joined {i} {key}')
            self.assertEqual(next_st['seed'],m.word(0x20002eb0));self.assertEqual(noise_seed,m.word(0x200023cc))
            for side in range(2):
                self.equal_floats(chaos[side],m.floats(0x200022bc+64*side,count=16))
                self.equal_floats(generators[side],m.floats(0x2000239c+24*side,count=6))
                base=0x2000233c+24*side
                self.equal_floats(filters[side],m.floats(base,count=6)+m.floats(base+48,count=6))
                self.equal_floats([envelopes[side]],m.floats(0x20002e98 if side==0 else 0x200011c0))
                addresses=(0x30000c40,0x30000c44) if side==0 else (0x30000844,0x30000840)
                for addr,value in zip(addresses,outputs[side]):
                    actual=m.word(addr)
                    if engines[side]:self.assertEqual(int(value*(1<<30))&0xffffffff,actual,f'joined {i} {side}')
                    else:
                        signed=actual if actual<0x80000000 else actual-0x100000000
                        self.assertLess(abs(value-signed/(1<<30)),2e-4)
            self.assertEqual(int(sources[1]*(1<<30))&0xffffffff,m.word(0x30001040))
            self.assertEqual(int(sources[0]*(1<<30))&0xffffffff,m.word(0x38000004))
            for mode,value,addr in ((aux_modes[0],aux_a,0x30001044),(aux_modes[1],aux_b,0x38000000)):
                if mode not in (0,5):value=add(value,1.)
                self.assertEqual(int(mul(value,f32(858993472.)))&0xffffffff,m.word(addr))
            st=next_st;COUNTS['joined_samples']+=1

    def test_slow_control_slot_mapping(self):
        m=SliceMachine(double_precision=True);rng=random.Random(6708)
        for i in range(100):
            raw=[rng.randrange(65536) for _ in range(6)]
            offsets=[rng.randrange(1000,5000) for _ in raw]
            gains=[f32(1/rng.randrange(40000,60000)) for _ in raw]
            for j in range(6):m.word(0x20001380+j*4,offsets[j])
            m.floats(0x20001200,gains);m.uc.mem_write(0x30000020,struct.pack('<6H',*raw))
            m.word(0x20002e90,0);m.word(0x20002e94,0)
            self.run_slice(m,0x0802e86c,0x0802eb5c)
            normalized=[mul(f32(r-o),g) for r,o,g in zip(raw,offsets,gains)]
            clamped=[min(1.,max(0.,x)) for x in normalized[:4]]
            for addr,x in zip((0x20002e5c,0x20002e60,0x20002e68,0x20002e64),clamped):
                self.equal_floats([x],m.floats(addr))
            for addr,x in zip((0x20001140,0x20002e50),normalized[4:]):
                square=mul(x,x);expected=mul(mul(square,square),60.)
                self.equal_floats([expected],m.floats(addr))
            for slide,focus,addr,pole_reg in ((clamped[0],clamped[1],0x200023ec,22),
                                              (clamped[3],clamped[2],0x200023f0,23)):
                rate,pole=analysis_parameters_exact(slide,focus)
                self.equal_floats([rate,pole],m.floats(addr)+[m.s(pole_reg)])
            COUNTS['slow_control_cases']+=1

    def test_slide_movement_cancels_clock_offset(self):
        threshold=bits(f32(.005))
        anchors=(0.,.5,1.,from_bits(threshold-1),from_bits(threshold),from_bits(threshold+1))
        for side in range(2):
            for gain in (1/65536,1/32768):
                for offset in (0,1,63):
                    for raw_slide in (0,32768,65535):
                        for anchor in anchors:
                            m=SliceMachine(double_precision=True)
                            m.floats(0x20001200,[gain]*6)
                            raw=[16384]*6;raw[3 if side else 0]=raw_slide
                            m.uc.mem_write(0x30000020,struct.pack('<6H',*raw))
                            offset_addr=0x20002434 if side else 0x20002438
                            slide_addr=0x20002e64 if side else 0x20002e5c
                            anchor_addr=0x20002e40 if side else 0x20002e44
                            movement_addr=0x20002e48 if side else 0x20002e4c
                            m.word(offset_addr,offset);m.floats(slide_addr,[.25]);m.floats(anchor_addr,[anchor])
                            self.run_slice(m,0x0802e86c,0x0802eb5c)
                            raw_normalized=mul(raw_slide,gain)
                            clamped,expected_anchor,movement,expected_offset=slide_clock_tracking(
                                'B' if side else 'A',raw_normalized,.25,anchor,offset)
                            self.equal_floats([expected_anchor],m.floats(anchor_addr))
                            self.equal_floats([movement],m.floats(movement_addr),(side,gain,offset,raw_slide,anchor))
                            self.equal_floats([clamped],m.floats(slide_addr))
                            self.assertEqual(m.word(offset_addr),expected_offset,
                                             (side,gain,offset,raw_slide,anchor))
                            COUNTS['slide_clock_offset_reset_cases']+=1

    def test_linear_reader_addresses_and_focus_deltas(self):
        from unicorn import UC_HOOK_MEM_READ
        rng=random.Random(6719)
        backing=[[f32(rng.uniform(-.02,.2)) for _ in range(64)] for _ in range(1024)]
        current=[f32(rng.uniform(-.1,.2)) for _ in range(64)]
        one_third=bits(f32(1/3))
        for side in range(2):
            m=SliceMachine();m.uc.mem_map(0x60000000,0x100000)
            bank=0x60001000;offset=128;data=bank+offset*4
            m.floats(data,[v for row in backing for v in row])
            desc=(0x20000920 if side else 0x20000a20)+3*16
            m.word(0x20000b20 if side else 0x20002430,3)
            m.word(0x20002f70 if side else 0x20002f74,bank);m.word(desc,offset);m.word(desc+8,1)
            m.floats(0x20002c40 if side else 0x20002a40,current)
            reads=[]
            def record(uc,access,address,size,value,user):
                if data<=address<data+1024*256:reads.append((address-data)//4)
            handle=m.uc.hook_add(UC_HOOK_MEM_READ,record)
            try:
                for n in (1,2,7,64,65,1024):
                    m.word(desc+4,n)
                    for slide in (0.,f32(.37),1.):
                        for focus in (0.,from_bits(one_third-1),from_bits(one_third),from_bits(one_third+1),.5,1.):
                            for clock_offset in (0,n-1):
                                m.word(0x20002434 if side else 0x20002438,clock_offset)
                                m.s(0,focus);m.s(1,slide);m.r(14,0x08020001);reads.clear()
                                self.run_slice(m,0x0802ce5c if side else 0x0802cd0c,0x08020000)
                                pos=fma(f32(n-1),slide,f32(clock_offset));index=math.trunc(pos)
                                indices=[i-n if i>=n else i for i in (index,index+1)]
                                self.assertEqual(reads,[i*64+j for j in range(64) for i in indices])
                                self.equal_floats(linear_deltas(backing[:n],current,slide,focus,clock_offset),
                                                  m.floats(0x20000f40 if side else 0x20000d40,count=64),
                                                  (side,n,slide,focus,clock_offset))
                                COUNTS['linear_reader_cases']+=1
            finally:m.uc.hook_del(handle)

    def test_linear_empty_and_retained_physical_storage(self):
        from unicorn import UC_HOOK_MEM_READ
        rng=random.Random(6731)
        # Index 0 is the actual preceding frame, not Python's negative index.
        backing=[[f32(rng.uniform(-.02,.2)) for _ in range(64)] for _ in range(1026)]
        current=[f32(rng.uniform(-.1,.2)) for _ in range(64)]
        for side in range(2):
            for slot in (0,15):
                m=SliceMachine();bank=0x60001000 if side else 0x60c01000
                offset=slot*65536+side*1048576;data=bank+4*offset
                m.uc.mem_map(data-0x1000,0x42000)
                m.floats(data-256,[v for row in backing for v in row])
                desc=(0x20000920 if side else 0x20000a20)+slot*16
                m.word(0x20000b20 if side else 0x20002430,slot)
                m.word(0x20002f70 if side else 0x20002f74,bank)
                m.word(desc,offset);m.word(desc+8,1)
                m.floats(0x20002c40 if side else 0x20002a40,current)
                reads=[]
                def record(uc,access,address,size,value,user):
                    if data-256<=address<data+1025*256:reads.append((address-data)//4)
                handle=m.uc.hook_add(UC_HOOK_MEM_READ,record)
                try:
                    for n in (0,1,2,65,1024):
                        m.word(desc+4,n)
                        for slide in (0.,f32(.37),from_bits(bits(1.)-1),1.):
                            for focus in (0.,f32(1/3),.5,1.):
                                for scan in (0,1,1023):
                                    m.word(0x20002434 if side else 0x20002438,scan)
                                    m.s(0,focus);m.s(1,slide);m.r(14,0x08020001);reads.clear()
                                    self.run_slice(m,0x0802ce5c if side else 0x0802cd0c,0x08020000)
                                    indices,fraction=linear_coordinates(n,slide,scan)
                                    self.assertEqual(reads,[i*64+j for j in range(64) for i in indices])
                                    if n==0 and scan==0:
                                        self.assertEqual(indices,[-1,0] if slide==1 else [0,1])
                                        self.assertEqual(fraction,0. if slide==1 else -slide)
                                    self.equal_floats(linear_storage_deltas(backing,1,n,current,slide,focus,scan),
                                                      m.floats(0x20000f40 if side else 0x20000d40,count=64),
                                                      (side,slot,n,slide,focus,scan))
                                    COUNTS['linear_physical_storage_cases']+=1
                                    COUNTS['linear_preceding_frame_cases']+=indices[0]<0
                finally:m.uc.hook_del(handle)
        with self.assertRaises(ValueError):linear_storage_deltas(backing,0,0,current,1.,0.)
        with self.assertRaises(ValueError):linear_storage_deltas(backing[:1025],1,0,current,0.,0.,1023)
        with self.assertRaises(ValueError):linear_deltas([],current,1.,0.)

    def test_planar_reader_addresses_and_ramps(self):
        from unicorn import UC_HOOK_MEM_READ
        rng = random.Random(6709)
        backing = [[f32(rng.uniform(-.2, 1.)) for _ in range(64)] for _ in range(2048)]
        current = [f32(rng.uniform(-.1, .8)) for _ in range(64)]
        for side in ('A', 'B'):
            m = SliceMachine(); m.uc.mem_map(0x60000000, 0x100000)
            slot_offset = 128; bank = 0x60001000
            data_start = bank + slot_offset*4
            m.floats(data_start, [v for row in backing for v in row])
            is_b = side == 'B'
            descriptor = 0x20000920 if is_b else 0x20000a20
            m.word(0x20000b20 if is_b else 0x20002430, 3)
            m.word(0x20002f70 if is_b else 0x20002f74, bank)
            m.word(descriptor+3*16, slot_offset)
            working = 0x20002c40 if is_b else 0x20002a40
            destination = 0x20000f40 if is_b else 0x20000d40
            m.floats(working, current)
            reads = []
            def record_read(uc, access, address, size, value, user):
                if data_start <= address < data_start+len(backing)*256:
                    reads.append((address-data_start)//4)
            handle = m.uc.hook_add(UC_HOOK_MEM_READ, record_read)
            try:
                for n in (0,1,2,3,5,6,8,9,10,16,17,63,64,65,1023,1024):
                    m.word(descriptor+3*16+4,n);m.word(descriptor+3*16+8,1)
                    grid = math.isqrt(n-1)+1 if n else 2
                    cases = [(0.,0.,0),(1.,1.,0),(.5,.5,0),
                             (1.,1.,grid-1),(0.,1.,max(0,n-1))]
                    cases += [(f32(rng.random()),f32(rng.random()),rng.randrange(grid)) for _ in range(3)]
                    for slide,focus,offset in cases:
                        indices,tx,ty,g = planar_coordinates(n,slide,focus,offset)
                        m.s(0,focus);m.s(1,slide)
                        m.word(0x20002434 if is_b else 0x20002438,offset)
                        m.r(14,0x08020001);reads.clear()
                        self.run_slice(m,0x0802d0f8 if is_b else 0x0802cfac,0x08020000)
                        expected_reads = [indices[j]*64+k for k in range(64) for j in (0,2,1,3)]
                        self.assertEqual(expected_reads,reads,(side,n,slide,focus,offset))
                        self.assertEqual(g,m.word(0x2000227c+(56 if is_b else 52)))
                        if any(i<0 or i>=n for i in indices):
                            with self.assertRaises(ValueError):planar_deltas(backing[:n],current,slide,focus,offset)
                            COUNTS['planar_outside_logical_array_cases']+=1
                        else:
                            expected=planar_deltas(backing[:n],current,slide,focus,offset)
                            self.equal_floats(expected,m.floats(destination,count=64),(side,n,slide,focus,offset))
                        COUNTS['planar_cases']+=1
            finally:
                m.uc.hook_del(handle)

    def test_raw_reader_addresses_after_capture_underflow(self):
        from unicorn import UC_HOOK_MEM_READ
        from sp67_extended import _linear_source_deltas, _planar_source_deltas
        # The last two dimensions come from the checked 17/54-frame captures
        # stopped after selection of initialized slot 1 (second dimension 8).
        dimensions=((0,1),(8,8),(1024,1),(67107857,8),(67107894,8))
        controls=((0.,0.),(0.,1.),(1.,0.),(1.,1.),(f32(.37),f32(.61)))
        current=[f32((j-32)/128) for j in range(64)]
        for side in range(2):
            for linear in (False,True):
                m=SliceMachine();bank=0x60001000 if side else 0x60c01000
                origin=1048576 if side else 0;offset=origin+65536
                desc=(0x20000920 if side else 0x20000a20)+16
                m.word(0x20000b20 if side else 0x20002430,1)
                m.word(0x20002f70 if side else 0x20002f74,bank)
                m.word(desc,offset)
                working=0x20002c40 if side else 0x20002a40
                destination=0x20000f40 if side else 0x20000d40
                m.floats(working,current)
                entry=(0x0802ce5c if side else 0x0802cd0c) if linear else (0x0802d0f8 if side else 0x0802cfac)
                boundary=(0x0802cf22 if side else 0x0802cdd2) if linear else (0x0802d1da if side else 0x0802d08e)
                mapped=set()
                for d1,d2 in dimensions:
                    m.word(desc+4,d1);m.word(desc+8,d2)
                    for slide,focus in controls:
                        for scan in (0,1023):
                            case=(side,linear,d1,d2,slide,focus,scan)
                            expected,fractions,grid,n=array_reader_addresses(d1,d2,offset,bank,slide,focus,scan,linear)
                            m.word(0x20002434 if side else 0x20002438,scan)
                            m.s(0,focus);m.s(1,slide);m.r(14,0x08020001)
                            m.run(entry,boundary,limit=150000);COVERAGE.update(m.last_trace)
                            actual=[m.r(r) for r in ((2,1) if linear else (0,1,2,12))]
                            self.assertEqual(actual,expected,case)
                            self.equal_floats(fractions,[m.s(r) for r in ((12,) if linear else (10,9))],case)
                            if not linear:self.assertEqual(m.word(0x2000227c+(56 if side else 52)),grid,case)
                            # Sparse supplied memory tests the complete arithmetic tail;
                            # it does not claim these addresses contain RAM on hardware.
                            rows={}
                            for address in expected:
                                for page in (address&~4095,(address+255)&~4095):
                                    if page not in mapped:
                                        m.uc.mem_map(page,4096);mapped.add(page)
                                rows[address]=[f32(((address//256)%17+j-32)/128) for j in range(64)]
                                m.floats(address,rows[address])
                            reads=[]
                            def record(uc,access,address,size,value,user):
                                if any(base<=address<base+256 for base in rows):reads.append(address)
                            handle=m.uc.hook_add(UC_HOOK_MEM_READ,record)
                            try:self.run_slice(m,boundary,0x08020000)
                            finally:m.uc.hook_del(handle)
                            order=(0,1) if linear else (0,2,1,3)
                            self.assertEqual(reads,[(expected[i]+j*4)&0xffffffff for j in range(64) for i in order],case)
                            sources=[rows[a] for a in expected]
                            deltas=(_linear_source_deltas(sources,current,fractions[0],focus) if linear else
                                    _planar_source_deltas(sources,current,*fractions))
                            self.equal_floats(deltas,m.floats(destination,count=64),case)
                            COUNTS['raw_array_reader_cases']+=1
                            COUNTS['large_descriptor_reader_cases']+=int(d1>1024)
                            start=bank+origin*4
                            COUNTS['raw_reader_outside_bank_cases']+=int(any(not start<=a<start+16*1024*256 for a in expected))

    def test_array_initial_descriptor_layout(self):
        m=SliceMachine();m.r(11,0x20000920)
        m.uc.mem_write(0x20000920,b'\xa5'*512)
        self.run_slice(m,0x0803418c,0x08034426)
        for side,base in enumerate((0x20000a20,0x20000920)):
            for slot in range(16):
                self.assertEqual(struct.unpack('<4I',m.uc.mem_read(base+slot*16,16)),
                                 (slot*65536+side*1048576,8,8,1))
                COUNTS['array_initial_descriptors']+=1

    def test_planar_retained_physical_slot(self):
        # Same logical prefix, two distinct retained tails. Original readers
        # must follow physical storage rather than clamp/zero at logical end.
        current=[f32((j-31)/100) for j in range(64)]
        for side in range(2):
            m=SliceMachine();bank=0x60001000 if side else 0x60c01000
            m.uc.mem_map(bank,0x40000)
            desc=0x20000920 if side else 0x20000a20
            m.word(0x20002f70 if side else 0x20002f74,bank)
            for length in (0,1,2,3,5,63,64,65,255,257,1023,1024):
                m.word(desc+4,length);m.word(desc+8,1)
                for slide,focus,offset in ((0.,0.,0),(.99,.99,max(0,length-1)),
                                           (1.,1.,max(0,length-1)),(.51,.73,0)):
                    outputs=[]
                    for tail in (0.,.875):
                        backing=[[f32(((i+3)*(j+7)%89)/100) if i<length else tail
                                  for j in range(64)] for i in range(1024)]
                        m.floats(bank,[x for row in backing for x in row])
                        m.floats(0x20002c40 if side else 0x20002a40,current)
                        m.word(0x20002434 if side else 0x20002438,offset)
                        m.s(0,focus);m.s(1,slide);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d0f8 if side else 0x0802cfac,0x08020000)
                        expected=planar_slot_deltas(backing,length,current,slide,focus,offset)
                        actual=m.floats(0x20000f40 if side else 0x20000d40,count=64)
                        self.equal_floats(expected,actual,(side,length,slide,focus,offset,tail))
                        outputs.append([bits(v) for v in actual])
                        COUNTS['planar_retained_slot_cases']+=1
                    if outputs[0]!=outputs[1]:COUNTS['planar_retained_tail_changes']+=1
        self.assertGreater(COUNTS['planar_retained_tail_changes'],0)
        # The portable adapter rejects stale/corrupt offsets outside its actual
        # allocation instead of issuing the firmware's unchecked host read.
        with self.assertRaises(ValueError):
            planar_slot_deltas(backing,65,current,1.,1.,65536)

    def test_capture_and_interaction_button_handler(self):
        # Run the complete handler, including its original GPIO read helper.
        # Input registers are snapshots, not emulated electrical peripherals.
        for side in ('A','B'):
            is_b = side == 'B'
            for recorded in (None,0,1,17,1023):
                m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                descriptor=0x20000920 if is_b else 0x20000a20
                active=0x20002e80 if is_b else 0x20002e84
                position=0x200011a0 if is_b else 0x20002e7c
                peak=0x200011e0 if is_b else 0x20002e9c
                policy=0x20002ed4 if is_b else 0x20002ed8
                offset=65536;initial_active=int(recorded is not None)
                m.word(descriptor,offset);m.word(descriptor+4,1024 if initial_active else 8)
                m.word(descriptor+8,1 if initial_active else 8);m.word(descriptor+12,7)
                m.word(active,initial_active);m.word(position,offset+(recorded or 0)*64)
                m.word(policy,1);m.floats(peak,[.75])
                # Shift already held; counter is below the long-hold branch.
                m.word(0x20002f34+(4 if is_b else 0),51)
                m.word(0x20002f58,0x100 if is_b else 1)
                m.word(0x58021810,128 if is_b else 64)
                m.word(0x58020410 if is_b else 0x58020c10,64 if is_b else 128)
                m.r(14,0x08020001)
                self.run_slice(m,0x0802d900,0x08020000)
                if recorded is None:
                    self.assertEqual(m.word(active),1)
                    self.assertEqual(m.word(position),offset)
                    self.assertEqual(m.word(descriptor+4),1024)
                    self.assertEqual(m.word(descriptor+12),0)
                    self.equal_floats([0.],m.floats(peak))
                    self.assertEqual(m.word(policy),1)
                else:
                    self.assertEqual(m.word(active),0)
                    self.assertEqual(m.word(descriptor+4),recorded)
                    self.assertEqual(m.word(policy),1 if is_b else 0)
                    self.assertEqual(m.word(descriptor+12),7)
                    self.equal_floats([.75],m.floats(peak))
                self.assertEqual(m.word(descriptor+8),1)
                COUNTS['capture_ui_cases']+=1
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        for expected in (1,2,0,1,2,0):
            m.word(0x58020c10,0);m.r(14,0x08020001)
            self.run_slice(m,0x0802d900,0x08020000)
            m.word(0x20002eb8,0)
            m.word(0x58020c10,8);m.r(14,0x08020001)
            self.run_slice(m,0x0802d900,0x08020000)
            self.assertEqual(m.uc.mem_read(0x20002f54,1)[0],expected)
            self.assertEqual(m.word(0x2001348c+48),expected)
            self.assertEqual(m.word(0x20002eb8),1)
            COUNTS['interaction_button_transitions']+=1

    def test_capture_shrink_preserves_scan_offset(self):
        from unicorn import UC_HOOK_MEM_READ
        for side in range(2):
            for slot in (0,15):
                for scan in (0,63,1023):
                    for recorded in (0,1,2,65):
                        m=SliceMachine(double_precision=True);m.uc.mem_map(0x58020000,0x10000)
                        bank=0x60001000 if side else 0x60c01000
                        word_offset=slot*65536+side*1048576
                        data_start=bank+word_offset*4;m.uc.mem_map(data_start,0x42000)
                        desc=(0x20000920 if side else 0x20000a20)+slot*16
                        selection=0x20000b20 if side else 0x20002430
                        active=0x20002e80 if side else 0x20002e84
                        position=0x200011a0 if side else 0x20002e7c
                        offset_addr=0x20002434 if side else 0x20002438
                        button=0x58020410 if side else 0x58020c10
                        mask=64 if side else 128
                        m.word(selection,slot);m.word(desc,word_offset)
                        for index in range(16):m.word((0x20000920 if side else 0x20000a20)+index*16+12,1)
                        m.word(desc+4,1024);m.word(desc+8,1)
                        m.word(0x20002f70 if side else 0x20002f74,bank)
                        backing=[[f32((i%31+j%7)/40) for j in range(64)] for i in range(1056)]
                        m.floats(data_start,[x for row in backing for x in row])
                        m.word(offset_addr,scan)
                        m.word(0x20002f34+side*4,51)
                        m.word(0x20002f58,0x100 if side else 1)
                        m.word(0x58021810,128 if side else 64)
                        m.word(button,mask);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d900,0x08020000)
                        self.assertEqual((m.word(active),m.word(position),m.word(offset_addr)),
                                         (1,word_offset,scan))
                        for frame in range(recorded):
                            values=[f32((frame+j+1)/128) for j in range(64)]
                            m.floats(0x20002d40 if side else 0x20002b40,values)
                            m.r(7,active);m.r(12,active)
                            self.run_slice(m,0x0803219a if side else 0x080320e8,
                                           0x0802e7fc if side else 0x0802e7e8)
                            backing[frame]=values
                        m.word(button,0);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d900,0x08020000)
                        m.word(button,mask);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d900,0x08020000)
                        self.assertEqual((m.word(active),m.word(desc+4),m.word(desc+8),m.word(offset_addr)),
                                         (0,recorded,1,scan),(side,slot,scan,recorded))
                        # Read the resulting descriptor directly. Mode switching,
                        # media saves and intervening slow controls are excluded.
                        indices,_,_,_=planar_coordinates(recorded,1.,1.,scan)
                        reads=[]
                        def record_read(uc,access,address,size,value,user):
                            if data_start<=address<data_start+0x42000:reads.append(address)
                        handle=m.uc.hook_add(UC_HOOK_MEM_READ,record_read)
                        try:
                            m.s(0,1.);m.s(1,1.);m.r(14,0x08020001)
                            self.run_slice(m,0x0802d0f8 if side else 0x0802cfac,0x08020000)
                        finally:m.uc.hook_del(handle)
                        self.assertEqual(reads,[data_start+4*(indices[j]*64+k)
                                                for k in range(64) for j in (0,2,1,3)])
                        escaped=any(i>=1024 for i in indices)
                        COUNTS['capture_shrink_cross_slot_reads']+=escaped
                        if escaped:
                            with self.assertRaises(ValueError):
                                planar_slot_deltas(backing[:1024],recorded,[0.]*64,1.,1.,scan)
                        # Offset view exercises a nonzero physical origin while
                        # retaining the actual adjacent memory supplied above.
                        expected=planar_storage_deltas([[-.25]*64]*2+backing,2,recorded,
                                                      [0.]*64,1.,1.,scan)
                        self.equal_floats(expected,m.floats(0x20000f40 if side else 0x20000d40,count=64))
                        COUNTS['capture_shrink_offset_sequences']+=1
                        # Unchanged Slide first, while still in SAM. All slow
                        # controls and gain/anchor fields are explicit fixtures.
                        m.floats(0x20001200,[1/32768]*6)
                        m.uc.mem_write(0x30000020,struct.pack('<6H',*([32768]*6)))
                        m.floats(0x20002e40,[1.,1.])
                        m.floats(0x20002e5c,[1.]);m.floats(0x20002e64,[1.])
                        self.run_slice(m,0x0802e86c,0x0802eb5c)
                        self.assertEqual(m.word(offset_addr),scan)
                        # Release Shift/Array, then press Array unshifted.
                        m.word(button,0);m.word(0x58021810,0);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d900,0x08020000)
                        m.word(button,mask);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d900,0x08020000)
                        self.assertEqual(m.uc.mem_read(0x20002f52+side,1)[0],1)
                        self.assertEqual(m.word(offset_addr),scan)
                        # Real failure paths: no mounted filesystem, busy HAL
                        # transport/program handles, plain flash-register RAM.
                        # No media/peripheral call is intercepted or replaced.
                        m.uc.mem_map(0x52002000,0x1000)
                        m.uc.mem_write(0x20014c38,b'\x01')
                        m.uc.mem_write(0x20002064,b'\x01')
                        m.word(0x20002ebc,0x081e0000)
                        m.word(0x20002eec,129);m.uc.mem_write(0x20002f66,b'\x01')
                        m.r(4,0x20002f65)
                        m.run(0x0802e77e,0x0802eb5c,1000000)
                        COVERAGE.update(m.last_trace)
                        self.assertIn(0x08033af8,m.last_trace)
                        self.assertIn(0x0802a00c,m.last_trace) # no registered filesystem
                        self.assertIn(0x08022df2,m.last_trace) # actual busy program return
                        self.assertEqual(m.word(offset_addr),scan)
                        self.assertEqual(m.word(0x20002e90 if side else 0x20002e94),1)
                        peak=m.floats(0x200011e0 if side else 0x20002e9c)[0]
                        for frame in range(recorded):backing[frame]=array_save_frame(backing[frame],peak)[0]
                        self.equal_floats([v for row in backing[:recorded] for v in row],
                                          m.floats(data_start,count=recorded*64))
                        expected=planar_storage_deltas(backing,0,recorded,[0.]*64,1.,1.,scan)
                        self.equal_floats(expected,m.floats(0x20000f40 if side else 0x20000d40,count=64))
                        self.assertEqual(m.word(0x20002ebc),0x081e0020)
                        COUNTS['capture_shrink_mode_entry_sequences']+=1
                        raw=[32768]*6;raw[3 if side else 0]=32400
                        m.uc.mem_write(0x30000020,struct.pack('<6H',*raw))
                        self.run_slice(m,0x0802e86c,0x0802eb5c)
                        self.assertEqual(m.word(offset_addr),0)
                        expected=planar_storage_deltas(backing,0,recorded,[0.]*64,32400/32768,1.,0)
                        self.equal_floats(expected,m.floats(0x20000f40 if side else 0x20000d40,count=64))
                        COUNTS['capture_shrink_manual_takeovers']+=1

    def test_array_mode_cycle_and_lf_gestures(self):
        for side in range(2):
            for sao,engine in ((0,0),(1,0),(1,1),(1,2)):
                for hold in (0,49,50,51):
                    for lf in (0,1):
                        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                        mode_addr=0x20002f52+side;engine_addr=0x20002e88 if side else 0x20002e8c
                        lf_addr=0x20000820 if side else 0x200023d0
                        flag_addr=0x20002e90 if side else 0x20002e94
                        active_addr=0x20002e80 if side else 0x20002e84
                        m.uc.mem_write(mode_addr,bytes([sao]));m.word(flag_addr,sao)
                        m.word(engine_addr,engine);m.word(lf_addr,lf)
                        m.word(0x2001349c+side*4,engine);m.word(0x200134ac+side*4,lf)
                        if hold:
                            m.word(0x58021810,128 if side else 64)
                            m.word(0x20002f58,0x100 if side else 1)
                            m.word(0x20002f34+side*4,hold)
                        m.word(0x58020410 if side else 0x58020c10,64 if side else 128)
                        m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                        shifted=hold>=50  # sampled counter increments before dispatch
                        if shifted and not sao:
                            expected=(0,0,lf,1)
                        elif shifted:
                            expected=(1,engine,1-lf,0)
                        elif not sao:
                            expected=(1,0,lf,0)
                        elif engine==2:
                            expected=(0,0,lf,0)
                        else:
                            expected=(1,engine+1,lf,0)
                        actual=(m.uc.mem_read(mode_addr,1)[0],m.word(engine_addr),m.word(lf_addr),m.word(active_addr))
                        self.assertEqual(actual,expected,(side,sao,engine,hold,lf))
                        self.assertEqual(m.word(0x2001349c+side*4),expected[1])
                        self.assertEqual(m.word(0x200134ac+side*4),expected[2])
                        COUNTS['mode_button_cases']+=1

    def test_array_selection_action_model(self):
        import itertools
        for side,slot,mirror_kind,scan,dirty in itertools.product(
                range(2),range(16),range(3),(0,7),range(2)):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            selection=(0x20002430,0x20000b20)[side]
            scan_addr=(0x20002438,0x20002434)[side]
            mirror=(slot,(slot+1)&15,37)[mirror_kind]
            m.word(selection,slot);m.word(scan_addr,scan)
            m.word(0x200134b4+4*side,mirror);m.word(0x20002eb8,dirty)
            m.word(0x20002f04,1);m.word(0x20002f08,1)
            m.word(0x58021810,64 if side==0 else 128)
            m.word(0x20002f58,257)
            m.word(0x20002f34,100);m.word(0x20002f38,100)
            def snapshot():
                return ArraySelectionState(m.word(selection),m.word(scan_addr),
                    m.word(0x200134b4+4*side),m.word(0x20002f34+4*side),
                    m.uc.mem_read(0x20002f48+side,1)[0],
                    (m.word(0x20002f08),m.word(0x20002f04)),m.word(0x20002eb8))
            start,end=(0x0802e4ec,0x0802da50) if side==0 else (0x0802dc50,0x0802dc96)
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,start)
            expected=array_selection_action('AB'[side],snapshot())
            self.run_slice(m,start,end)
            self.assertEqual(snapshot(),expected,(side,slot,mirror,scan,dirty))
            COUNTS['array_selection_model_cases']+=1

    def test_clock_timer_prefix_model(self):
        import itertools
        elapsed_values=(0,5999,6000,6001,0xfffffffe,0xffffffff)
        countdown_values=(0,1,2,0x7fffffff,0x80000000,0x80000001,0xffffffff)
        for ei,ci,active,policy in itertools.product(range(6),range(7),range(4),
                                                    ((0,1),(1,0),(7,7))):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            state=ClockTimerState((elapsed_values[ei],elapsed_values[(ei+3)%6]),
                                 (countdown_values[ci],countdown_values[6-ci]),policy)
            capture=tuple((active>>i)&1 for i in range(2))
            for side in range(2):
                m.word(0x20002f0c+4*side,state.elapsed[side])
                m.word((0x20002ed0,0x20002ecc)[side],state.countdown[side])
                m.word((0x20002ed8,0x20002ed4)[side],state.policy[side])
                m.word((0x20002e84,0x20002e80)[side],capture[side])
            expected=clock_timer_tick(state,capture)
            self.run_slice(m,0x0802d900,0x0802d99e)
            actual=ClockTimerState(tuple(m.word(0x20002f0c+4*i) for i in range(2)),
                                  tuple(m.word(a) for a in (0x20002ed0,0x20002ecc)),
                                  tuple(m.word(a) for a in (0x20002ed8,0x20002ed4)))
            self.assertEqual(actual,expected,(state,capture))
            self.assertEqual(tuple(m.word(a) for a in (0x20002e84,0x20002e80)),capture)
            COUNTS['clock_timer_model_cases']+=1

    def test_button_edge_sampling_model(self):
        import itertools
        for previous,current,pending in itertools.product(range(8),repeat=3):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            old=tuple((previous>>i)&1 for i in range(3))
            now=tuple((current>>i)&1 for i in range(3))
            events=tuple((pending>>i)&1 for i in range(3))
            m.uc.mem_write(0x20002f5a,bytes(old))
            m.word(0x58020c10,(128 if now[0] else 0)|(8 if now[2] else 0))
            m.word(0x58020410,64 if now[1] else 0)
            for i in range(3):
                m.word(0x20002f28+4*i,events[i]);m.word(0x20002f3c+4*i,77+i)
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x0802dce4)
            def snapshot():
                return ButtonEdgeState(tuple(m.uc.mem_read(0x20002f5a,3)),
                                       tuple(m.word(0x20002f28+4*i) for i in range(3)),
                                       tuple(m.word(0x20002f3c+4*i) for i in range(3)))
            before=snapshot();self.assertEqual(before,ButtonEdgeState(old,events,(77,78,79)))
            expected=sample_button_edges(before,now)
            tail=button_inputs(*self.button_action_snapshot(m),before,now)
            self.run_slice(m,0x0802dce4,0x0802dd24)
            self.assertEqual(snapshot(),expected,(previous,current,pending))
            COUNTS['button_edge_model_cases']+=1
            self.run_slice(m,0x0802dd24,0x08020000)
            self.assertEqual((*self.button_action_snapshot(m),snapshot()),tail,
                             (previous,current,pending))
            COUNTS['button_input_tail_cases']+=1

    def test_array_button_action_model(self):
        import itertools
        for side,raw,cached,engine,active,hold,lf,mirror,cursor in itertools.product(
                range(2),range(2),range(2),range(3),range(2),(0,49,50),
                range(2),range(2),(0,65536+17*64)):
            case=(side,raw,cached,engine,active,hold,lf,mirror,cursor)
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            words={'cached_sao':(0x20002e94,0x20002e90)[side],
                   'engine':(0x20002e8c,0x20002e88)[side],
                   'capture':(0x20002e84,0x20002e80)[side],
                   'hold':0x20002f34+4*side,'lf':(0x200023d0,0x20000820)[side],
                   'cursor':(0x20002e7c,0x200011a0)[side],
                   'policy':(0x20002ed8,0x20002ed4)[side],
                   'engine_mirror':0x2001349c+4*side,'lf_mirror':0x200134ac+4*side,
                   'dirty':0x20002eb8}
            desc=(0x20000a20,0x20000920)[side];peak=(0x20002e9c,0x200011e0)[side]
            seed=ArrayButtonState(raw_sao=raw,cached_sao=cached,engine=engine,
                                  capture=active,hold=hold,lf=lf,cursor=cursor,
                                  descriptor=(65536,65,8,7),peak=.75,policy=1,
                                  engine_mirror=engine if mirror else 7,lf_mirror=lf if mirror else 7)
            for key,addr in words.items():m.word(addr,getattr(seed,key))
            m.uc.mem_write(0x20002f52+side,bytes([raw]))
            m.uc.mem_write(desc,struct.pack('<4I',*seed.descriptor));m.floats(peak,[seed.peak])
            m.word(0x58021810,64 if side==0 else 128)
            m.word(0x20002f58,1 if side==0 else 256)
            m.word(0x58020c10 if side==0 else 0x58020410,128 if side==0 else 64)
            def snapshot():
                values={key:m.word(addr) for key,addr in words.items()}
                values.update(raw_sao=m.uc.mem_read(0x20002f52+side,1)[0],
                              gesture=m.uc.mem_read(0x20002f48+side,1)[0],
                              descriptor=struct.unpack('<4I',m.uc.mem_read(desc,16)),
                              peak=m.floats(peak)[0])
                return ArrayButtonState(**values)
            start=0x0802dd60 if side==0 else 0x0802ddce
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,start)
            before=snapshot();expected=array_button_action('AB'[side],before)
            self.run_slice(m,start,0x08020000)
            self.assertEqual(snapshot(),expected,case)
            COUNTS['array_button_action_model_cases']+=1

    def test_shared_button_action_model(self):
        import itertools
        for ha,hb,ca,cb,aux,interaction,mirrors in itertools.product(
                (0,49,50,51),(0,49,50,51),range(2),range(2),
                ((0,5),(1,4),(2,3),(3,2),(4,1),(5,0),(5,5)),range(3),range(3)):
            case=(ha,hb,ca,cb,aux,interaction,mirrors)
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            m.word(0x58021810,192);m.word(0x20002f58,257);m.word(0x58020c10,8)
            m.word(0x20002f34,ha);m.word(0x20002f38,hb)
            m.word(0x20002e94,ca);m.word(0x20002e90,cb)
            m.word(0x20002ef8,aux[0]);m.word(0x20002ef4,aux[1])
            m.word(0x20013494,(7,0,1)[mirrors])
            m.word(0x20013498,(7,0,1)[mirrors])
            m.uc.mem_write(0x20002f54,bytes([interaction]))
            m.word(0x200134bc,(7,0,1)[mirrors]);m.word(0x20002eb8,mirrors&1)
            def snapshot():
                return SharedButtonState(
                    holds=tuple(m.word(a) for a in (0x20002f34,0x20002f38)),
                    gestures=tuple(m.uc.mem_read(0x20002f48,2)),
                    cached_sao=tuple(m.word(a) for a in (0x20002e94,0x20002e90)),
                    auxiliary=tuple(m.word(a) for a in (0x20002ef8,0x20002ef4)),
                    auxiliary_mirrors=tuple(m.word(a) for a in (0x20013494,0x20013498)),
                    interaction=m.uc.mem_read(0x20002f54,1)[0],
                    interaction_mirror=m.word(0x200134bc),dirty=m.word(0x20002eb8))
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x0802de3a)
            expected=shared_button_action(snapshot())
            self.run_slice(m,0x0802de3a,0x08020000)
            self.assertEqual(snapshot(),expected,case)
            COUNTS['shared_button_action_model_cases']+=1

    def test_concurrent_array_and_shared_button_edges(self):
        """Stable Shift levels, idle capture, no clocks or long-hold actions."""
        import itertools
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        modes=((0,0),(1,0),(1,1),(1,2))
        for a,b,ha,hb,edges,aux,interaction in itertools.product(
                modes,modes,(0,49,50),(0,49,50),(3,5,6,7),(0,5),range(3)):
            case=(a,b,ha,hb,edges,aux,interaction)
            m.uc.mem_write(0x20000800,bytes(0x2800))
            m.uc.mem_write(0x2001348c,bytes(64))
            m.word(0x58021810,(64 if ha else 0)|(128 if hb else 0))
            m.word(0x58020c10,(128 if edges&1 else 0)|(8 if edges&4 else 0))
            m.word(0x58020410,64 if edges&2 else 0)
            m.uc.mem_write(0x20002f58,bytes((int(bool(ha)),int(bool(hb)),0,0,0)))
            m.uc.mem_write(0x20002f52,bytes((a[0],b[0],interaction)))
            expected=[]
            holds=[ha+1 if ha else 0,hb+1 if hb else 0]
            for side,((sao,engine),hold) in enumerate(zip((a,b),(ha,hb))):
                m.word(0x20002f34+4*side,hold)
                m.word((0x20002e94,0x20002e90)[side],sao)
                m.word((0x20002e8c,0x20002e88)[side],engine)
                m.word(0x2001349c+4*side,engine)
                m.word((0x20002ef8,0x20002ef4)[side],aux)
                m.word(0x20013494+4*side,aux)
                lf=capture=0
                if edges&(1<<side):
                    if holds[side]>50:
                        holds[side]=1601
                        if sao:lf=1
                        else:capture=1
                    elif not sao:sao,engine=1,0
                    elif engine==2:sao,engine=0,0
                    else:engine+=1
                expected.append((sao,engine,lf,capture))
            expected_aux=[aux,aux]
            if edges&4:
                target=0 if holds[0]>50 else 1 if holds[1]>50 else None
                if target is None:interaction=(interaction+1)%3
                else:
                    holds[target]=1601
                    value=(aux+1)%6
                    expected_aux[target]=1 if (a,b)[target][0] and value==0 else value
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x0802dce4)
            composed=button_actions(*self.button_action_snapshot(m),tuple(bool(edges&(1<<i)) for i in range(3)))
            self.run_slice(m,0x0802dce4,0x08020000)
            self.assertEqual(self.button_action_snapshot(m),composed,case)
            COUNTS['composed_button_action_cases']+=1
            dispatch=[pc for pc in m.last_trace if pc in (0x0802dd60,0x0802ddce,0x0802de3a)]
            self.assertEqual(dispatch,[pc for bit,pc in enumerate((0x0802dd60,0x0802ddce,0x0802de3a))
                                       if edges&(1<<bit)],case)
            for side in range(2):
                actual=(m.uc.mem_read(0x20002f52+side,1)[0],
                        m.word((0x20002e8c,0x20002e88)[side]),
                        m.word((0x200023d0,0x20000820)[side]),
                        m.word((0x20002e84,0x20002e80)[side]))
                self.assertEqual(actual,expected[side],case)
                self.assertEqual(m.word((0x20002ef8,0x20002ef4)[side]),expected_aux[side],case)
                self.assertEqual(m.word(0x20013494+4*side),expected_aux[side],case)
                self.assertEqual(m.word(0x2001349c+4*side),expected[side][1],case)
                self.assertEqual(m.word(0x200134ac+4*side),expected[side][2],case)
                self.assertEqual(m.word(0x20002f34+4*side),holds[side],case)
                self.assertEqual(m.word((0x20002e94,0x20002e90)[side]),(a,b)[side][0],case)
            self.assertEqual(m.uc.mem_read(0x20002f54,1)[0],interaction,case)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f28,12)),bytes(12),case)
            COUNTS['concurrent_button_cases']+=1
            # Keep every input high: no new edge should redispatch an action.
            retained=((0x20002f52,3),(0x20002e80,24),(0x20002ef4,8),
                      (0x200023d0,4),(0x20000820,4),(0x20013494,32))
            before=[bytes(m.uc.mem_read(addr,size)) for addr,size in retained]
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
            self.assertFalse(any(pc in (0x0802dd60,0x0802ddce,0x0802de3a) for pc in m.last_trace),case)
            self.assertEqual([bytes(m.uc.mem_read(addr,size)) for addr,size in retained],before,case)
            COUNTS['concurrent_button_held_callbacks']+=1

    def test_simultaneous_shift_releases_with_array_edges(self):
        """Normal UI, prior held Shifts, no clock edges; release precedes Arrays."""
        import itertools
        for ha,hb,edges,active in itertools.product(
                (0,9,10,49,50,51,499,500,1499,1500),
                (0,9,10,49,50,51,499,500,1499,1500),range(8),range(4)):
            case=(ha,hb,edges,active)
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            m.word(0x20002f58,0x101);m.uc.mem_write(0x20002f48,b'\x01\x01')
            m.word(0x20002f34,ha);m.word(0x20002f38,hb)
            m.word(0x58020c10,(128 if edges&1 else 0)|(8 if edges&4 else 0))
            m.word(0x58020410,64 if edges&2 else 0)
            m.uc.mem_write(0x20002f54,b'\x02');m.word(0x200134bc,2)
            for side in range(2):
                m.word((0x20002ef8,0x20002ef4)[side],5)
                m.word(0x20013494+4*side,5)
                m.word((0x20002e84,0x20002e80)[side],(active>>side)&1)
                m.word((0x20002e7c,0x200011a0)[side],1088)
                m.word((0x20002438,0x20002434)[side],7)
                for slot in range(2):
                    desc=(0x20000a20,0x20000920)[side]+16*slot
                    for j,value in enumerate((slot*65536,65,1,1)):m.word(desc+4*j,value)
            # Ordered model derived from the A release branch, then B branch.
            selected=[0,0];holds=[ha,hb];admit=[False,False]
            if hb>49:
                if 51<=ha<=1499:selected[1]=1;holds[1]=1601
            elif ha<500:admit[0]=True
            if selected[1]:
                holds[1]=1600 if edges else 0
            elif ha>49:
                if 51<=hb<=1499:selected[0]=1;holds[0]=1601
            elif hb<500:admit[1]=True
            # Reusable decisions must reproduce the independent ordered oracle above.
            model_selected=[0,0];model_admit=[False,False]
            action=shift_release_action(ha,hb)
            model_selected[1]=int(action=='select_other')
            model_admit[0]=action=='clock_self'
            if not model_selected[1]:
                action=shift_release_action(hb,ha)
                model_selected[0]=int(action=='select_other')
                model_admit[1]=action=='clock_self'
            self.assertEqual((model_selected,model_admit),(selected,admit),case)
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x0802dce4)
            self.assertEqual([m.word(a) for a in (0x20002430,0x20000b20)],selected,case)
            # Both-low entry clears displays; a subsequent selection restores its side.
            displays=(selected[0],selected[1])
            _,displays=shift_release_finish(0,(1,1),displays,4 if selected[1] else 2)
            if not selected[1]:
                _,displays=shift_release_finish(1,(0,1),displays,4 if selected[0] else 2)
            self.assertEqual(tuple(m.word(a) for a in (0x20002f08,0x20002f04)),displays,case)
            self.assertEqual([m.word(a) for a in (0x20002f34,0x20002f38)],holds,case)
            for side in range(2):
                count=(1 if active&(1<<side) else 2) if admit[side] else 0
                self.assertEqual(m.word((0x20002ed0,0x20002ecc)[side]),count,case)
                self.assertEqual(m.word((0x20002ed8,0x20002ed4)[side]),int(admit[side]),case)
                self.assertEqual(m.word((0x20002438,0x20002434)[side]),
                                 0 if selected[side] else 7+int(admit[side]),case)
            composed=button_actions(*self.button_action_snapshot(m),tuple(bool(edges&(1<<i)) for i in range(3)))
            self.run_slice(m,0x0802dce4,0x08020000)
            self.assertEqual(self.button_action_snapshot(m),composed,case)
            COUNTS['composed_button_action_cases']+=1
            for side in range(2):
                was_active=(active>>side)&1;now_active=was_active;mode=0
                desc=(0x20000a20,0x20000920)[side]+selected[side]*16
                expected=[selected[side]*65536,65,1,1]
                if edges&(1<<side):
                    if holds[side]>50:
                        now_active=1-was_active
                        if was_active:expected[1]=((1088-expected[0])&0xffffffff)>>6
                        else:expected[1:]=[1024,1,0]
                    elif not was_active:mode=1
                self.assertEqual(m.word((0x20002e84,0x20002e80)[side]),now_active,case)
                self.assertEqual(m.uc.mem_read(0x20002f52+side,1)[0],mode,case)
                self.assertEqual(list(struct.unpack('<4I',m.uc.mem_read(desc,16))),expected,case)
            aux=[5,5];interaction=2
            if edges&4:
                target=0 if holds[0]>50 else 1 if holds[1]>50 else None
                if target is None:interaction=0
                else:aux[target]=0
            self.assertEqual([m.word(a) for a in (0x20002ef8,0x20002ef4)],aux,case)
            self.assertEqual([m.word(0x20013494+4*side) for side in range(2)],aux,case)
            self.assertEqual(m.uc.mem_read(0x20002f54,1)[0],interaction,case)
            self.assertEqual(m.word(0x200134bc),interaction,case)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f28,12)),bytes(12),case)
            COUNTS['shift_release_shared_edge_cases' if edges&4 else 'simultaneous_shift_release_cases']+=1
            if edges&4:
                retained=((0x20002ef4,8),(0x20002f52,3),(0x20002e80,8),
                          (0x20000920,32),(0x20000a20,32),(0x20013494,48))
                before=[bytes(m.uc.mem_read(a,n)) for a,n in retained]
                m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                self.assertFalse(any(pc in (0x0802dd60,0x0802ddce,0x0802de3a)
                                     for pc in m.last_trace),case)
                self.assertEqual([bytes(m.uc.mem_read(a,n)) for a,n in retained],before,case)
                COUNTS['shift_release_shared_held_cases']+=1

    def test_persistent_simultaneous_shift_release_capture(self):
        for initial_capture in (False,True):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            for side in range(2):
                for slot in range(2):
                    desc=(0x20000a20,0x20000920)[side]+16*slot
                    for j,value in enumerate((side*1048576+slot*65536,65,1,1)):
                        m.word(desc+4*j,value)
            def ui(shifts,arrays):
                m.word(0x58021810,shifts)
                m.word(0x58020c10,128 if arrays&1 else 0)
                m.word(0x58020410,64 if arrays&2 else 0)
                m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
            if initial_capture:
                for _ in range(61):ui(192,0)
                ui(192,3)
                self.assertEqual([m.word(a) for a in (0x20002e84,0x20002e80)],[1,1])
                ui(192,0);ui(0,0);ui(0,0)
            for _ in range(61):ui(192,0)
            # Both Shifts fall while both Array buttons rise in this sample.
            ui(0,3)
            self.assertEqual([m.word(a) for a in (0x20002430,0x20000b20)],[0,1])
            self.assertEqual([m.word(a) for a in (0x20002e84,0x20002e80)],
                             [int(not initial_capture)]*2)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f52,2)),b'\x00\x00')
            self.assertEqual(m.word(0x20000a24),0 if initial_capture else 1024)
            self.assertEqual(m.word(0x20000934),67107840 if initial_capture else 1024)
            # No spectra/capture writer run between start and stop: the cursor
            # remains at slot 0's origin, while B stop subtracts slot 1's origin.
            retained=[bytes(m.uc.mem_read(a,n)) for a,n in
                      ((0x20000a20,32),(0x20000920,32),(0x20002e80,8),(0x20002f52,2))]
            ui(0,3)
            self.assertEqual([bytes(m.uc.mem_read(a,n)) for a,n in
                              ((0x20000a20,32),(0x20000920,32),(0x20002e80,8),(0x20002f52,2))],retained)
            COUNTS['persistent_shift_release_capture_sequences']+=1

    def test_consumed_gesture_release_and_rearm(self):
        """Reach both consumed gestures through real capture presses, then release."""
        import itertools
        for remaining,clocks in itertools.product(range(32),range(4)):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            for side in range(2):
                desc=(0x20000a20,0x20000920)[side]
                for j,value in enumerate((side*1048576,65,1,1)):m.word(desc+4*j,value)
                m.word((0x20002ef8,0x20002ef4)[side],5)
                m.word(0x20013494+4*side,5)
            def ui(mask):
                m.word(0x58021810,(64 if mask&1 else 0)|(128 if mask&2 else 0))
                m.word(0x58020c10,(128 if mask&4 else 0)|(8 if mask&16 else 0))
                m.word(0x58020410,64 if mask&8 else 0)
                m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
            for _ in range(61):ui(3)
            ui(15)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f48,2)),b'\x04\x04')
            self.assertEqual([m.word(a) for a in (0x20002e84,0x20002e80)],[1,1])
            def clock_edges(mask):
                for side in range(2):
                    if mask&(1<<side):
                        m.r(0,64 if side==0 else 128);m.r(14,0x08020001)
                        self.run_slice(m,0x08032688,0x08020000)
            clock_edges(clocks)
            routes=tuple(consumed_shift_route(side,tuple((remaining>>i)&1 for i in range(5)),
                                             int(bool(clocks&(1<<side)))) for side in range(2))
            ui(remaining)
            self.assertEqual(tuple(m.uc.mem_read(0x20002f48,2)),tuple(x.gesture for x in routes))
            self.assertEqual(tuple(m.word(a) for a in (0x20002ee4,0x20002edc)),
                             tuple(x.pending_clock for x in routes))
            self.assertEqual(bytes(m.uc.mem_read(0x20002f48,2)),
                             b'\x04\x04' if remaining else bytes((2 if clocks&1 else 0,0)),
                             (remaining,clocks))
            self.assertEqual([m.word(a) for a in (0x20002ee4,0x20002edc)],[0,0])
            self.assertEqual([m.word(a) for a in (0x20002ee8,0x20002ee0)],
                             [int(bool(clocks&1)),int(bool(clocks&2))])
            self.assertEqual([m.word(a) for a in (0x20002ed8,0x20002ed4,
                                                 0x20002ed0,0x20002ecc,
                                                 0x20002438,0x20002434)],[0]*6,
                             (remaining,clocks))
            expected_aux=[0 if remaining&16 else 5,5]
            self.assertEqual([m.word(a) for a in (0x20002ef8,0x20002ef4)],expected_aux,remaining)
            ui(remaining)
            self.assertEqual([m.word(a) for a in (0x20002f34,0x20002f38)],
                             [1600,1600] if remaining else [0,0],remaining)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f48,2)),
                             b'\x04\x04' if remaining else b'\x00\x00',remaining)
            self.assertFalse(any(pc in (0x0802dd60,0x0802ddce,0x0802de3a) for pc in m.last_trace))
            ui(0)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f48,2)),b'\x00\x00',remaining)
            self.assertEqual([m.word(a) for a in (0x20002f34,0x20002f38)],[0,0],remaining)
            ui(16)
            self.assertEqual(m.uc.mem_read(0x20002f54,1)[0],1,remaining)
            self.assertEqual(m.word(0x200134bc),1,remaining)
            self.assertEqual([m.word(a) for a in (0x20002ef8,0x20002ef4)],expected_aux,remaining)
            ui(16)
            self.assertEqual(m.uc.mem_read(0x20002f54,1)[0],1,remaining)
            self.assertNotIn(0x0802de3a,m.last_trace)
            self.assertEqual([m.word(a) for a in (0x20002e84,0x20002e80)],[1,1],remaining)
            self.assertEqual([m.word(a) for a in (0x20002ed8,0x20002ed4,
                                                 0x20002ed0,0x20002ecc,
                                                 0x20002438,0x20002434)],[0]*6,
                             (remaining,clocks))
            # Re-arming restores the ordinary asymmetric admission rules:
            # stable-low A admits, stable-low B consumes without admission.
            clock_edges(3);ui(0)
            self.assertEqual([m.word(a) for a in (0x20002ee4,0x20002edc)],[0,0])
            self.assertEqual([m.word(a) for a in (0x20002ed8,0x20002ed4,
                                                 0x20002ed0,0x20002ecc,
                                                 0x20002438,0x20002434)],[1,0,1,0,1,0],
                             (remaining,clocks))
            ui(2);clock_edges(2);ui(2)
            self.assertEqual([m.word(a) for a in (0x20002ed8,0x20002ed4,
                                                 0x20002ed0,0x20002ecc,
                                                 0x20002438,0x20002434)],[1,1,0,1,1,1],
                             (remaining,clocks))
            self.assertEqual([m.word(a) for a in (0x20002ee8,0x20002ee0)],
                             [int(bool(clocks&1))+1,int(bool(clocks&2))+2])
            COUNTS['rearmed_clock_admissions']+=2
            COUNTS['consumed_gesture_clock_sequences' if clocks else 'consumed_gesture_rearm_sequences']+=1

    def test_calibration_boot_button_dispatch(self):
        """Boot decision only: stop before ADC startup or defaults/reset path."""
        from unicorn import arm_const as arm
        import itertools
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        for ca,cb,pins,shared in itertools.product((-1,0,1,2,0x7fffffff),
                                                   (-1,0,1,2,0x7fffffff),(0,64,128,192),(0,1)):
            m.word(0x20002ee8,ca);m.word(0x20002ee0,cb)
            m.word(0x200144d4,0);m.word(0x200144d0,0)
            m.word(0x58021810,pins);m.word(0x58020c10,8*shared)
            m.r(7,0x200144d4)
            self.run_slice(m,0x08034582,(0x0803458a,0x080347ec))
            bypass=ca>1 or cb>1
            reset=not bypass and pins==192
            expected=0 if bypass or reset else 3 if pins&64 else int(bool(pins&128 or shared))
            shortcut=int(not bypass and not reset and not pins&64 and bool(shared))
            self.assertEqual(m.uc.reg_read(arm.UC_ARM_REG_PC),0x080347ec if reset else 0x0803458a)
            self.assertEqual(m.word(0x200144d4),expected,(ca,cb,pins,shared))
            self.assertEqual(m.word(0x200144d0),shortcut,(ca,cb,pins,shared))
            COUNTS['calibration_boot_dispatch_cases']+=1

    def test_calibration_shift_release_transitions(self):
        """Complete UI handler or exact pending calibration-save boundary."""
        from unicorn import arm_const as arm
        import itertools
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        for mode,pins,hold in itertools.product(range(1,12),(0,64,128,192),(0,49,50,100)):
            m.uc.mem_write(0x20000800,bytes(0x2800));m.word(0x200144d4,mode)
            m.word(0x58021810,pins);m.word(0x58020c10,0);m.word(0x58020410,0)
            m.uc.mem_write(0x20002f58,bytes((int(bool(pins&64)),int(bool(pins&128)),0,0,0)))
            m.word(0x20002f34,hold if pins&64 else 0);m.word(0x20002f38,hold if pins&128 else 0)
            for callback in range(4):
                m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                self.assertEqual(m.word(0x200144d4),mode,(mode,pins,hold,callback))
                COUNTS['calibration_stable_callbacks']+=1
        for mode,side,hold in itertools.product(range(1,12),range(2),(0,1,8,9,10,49,50,100)):
            m.uc.mem_write(0x20000800,bytes(0x2800))
            m.uc.mem_write(0x2001348c,bytes(64))
            m.word(0x200144d4,mode);m.word(0x200144d0,0)
            for addr in (0x58021810,0x58020c10,0x58020410):m.word(addr,0)
            m.uc.mem_write(0x20002f50,bytes((1,1,1,1,2)))
            m.uc.mem_write(0x20002f58,bytes((int(side==0),int(side==1),0,0,0)))
            m.word(0x20002f34+side*4,hold)
            m.r(13,m.sp);m.r(14,0x08020001)
            self.run_slice(m,0x0802d900,(0x08020000,0x0802d330))
            expected=mode+1 if side==1 and hold>9 else 0
            saving=expected in (3,12)
            modeled=calibration_release_action(mode,side,hold)
            self.assertEqual((modeled.stage,modeled.clear_ui,modeled.save_pending),
                             (expected,expected==0,saving))
            self.assertEqual(m.word(0x200144d4),expected,(mode,side,hold))
            self.assertEqual(m.uc.reg_read(arm.UC_ARM_REG_PC),0x0802d330 if saving else 0x08020000,
                             (mode,side,hold))
            if expected==0:
                for base in (0x20002f50,0x20002f58,0x20002f60):
                    self.assertEqual(bytes(m.uc.mem_read(base,5)),bytes(5),(mode,side,hold,base))
            else:self.assertEqual(bytes(m.uc.mem_read(0x20002f50,5)),bytes((1,1,1,1,2)))
            COUNTS['calibration_release_cases']+=1
            if saving:COUNTS['calibration_save_dispatch_cases']+=1

    def test_calibration_post_save_caller_transitions(self):
        """Separate caller continuation with supplied save return; no flash I/O."""
        from unicorn import UC_HOOK_MEM_WRITE
        import itertools
        for mode,shortcut,status in itertools.product((2,11),(0,1,2),(0,2)):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            m.word(0x200144d4,mode);m.word(0x200144d0,shortcut)
            m.uc.mem_write(0x20002f50,bytes((1,1,1,1,2)))
            m.word(0x20002f58,256);m.word(0x20002f38,10)
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x0802d330)
            resume=m.r(14)&~1
            self.assertEqual(resume,0x0802e44c if mode==2 else 0x0802e5b0)
            writes=[]
            def record(uc,access,address,size,value,user):
                if address in (0x58020818,0x58020c18,0x58020418):writes.append((address,value))
            hook=m.uc.hook_add(UC_HOOK_MEM_WRITE,record)
            # Only the documented caller-saved inputs are supplied here. The
            # calibration-save function is not executed or counted as covered.
            m.r(0,status)
            for reg in (1,2,3,12):m.r(reg,0xa5000000+reg)
            try:self.run_slice(m,resume,0x08020000)
            finally:m.uc.hook_del(hook)
            cleared=mode==11 or shortcut==1
            modeled=calibration_save_return(mode+1,shortcut)
            self.assertEqual((modeled.stage,modeled.clear_ui,modeled.save_pending),
                             (0 if cleared else 3,cleared,False))
            self.assertEqual(m.word(0x200144d4),modeled.stage,(mode,shortcut,status))
            self.assertEqual(m.word(0x200144d0),shortcut)
            self.assertEqual(bytes(m.uc.mem_read(0x20002f50,5)),
                             bytes(5) if cleared else bytes((1,1,1,1,2)))
            if cleared:
                for base in (0x20002f58,0x20002f60):self.assertEqual(bytes(m.uc.mem_read(base,5)),bytes(5))
            expected=[(0x58020818,64),(0x58020818,128),(0x58020c18,4096),
                      (0x58020c18,8192),(0x58020418,32768)]*5 if cleared else []
            self.assertEqual(writes,expected,(mode,shortcut,status))
            COUNTS['calibration_post_save_cases']+=1

    def test_calibration_shared_slow_and_fast_ring(self):
        rng=random.Random(6723)
        for initial_index in (0,1,63):
            m=SliceMachine();index=initial_index
            history=[[rng.randrange(65536) for _ in range(64)] for _ in range(12)]
            sums=[sum(row) for row in history]
            m.uc.mem_write(0x20001400,struct.pack('<768i',*(v for row in history for v in row)))
            m.uc.mem_write(0x200013c0,struct.pack('<12i',*sums));m.word(0x20002f00,index)
            for block in range(3):
                for fast in [False]+[True]*64:
                    start=6 if fast else 0
                    samples=[rng.randrange(65536) for _ in range(6)]
                    if fast:
                        m.r(12,samples[0]);m.r(7,samples[1])
                        m.word(m.sp+28,-samples[0]);m.word(m.sp+36,-samples[1])
                        for reg,value in zip((5,14,6,8),samples[2:]):m.r(reg,value)
                        m.word(m.sp+16,1)
                        self.run_slice(m,0x0802f0a4,0x0802f154)
                    else:
                        m.uc.mem_write(0x30000020,struct.pack('<6H',*samples));m.r(6,0x30000020)
                        m.word(m.sp+16,1)
                        self.run_slice(m,0x0802eb68,0x0802ec88)
                    sums[start:start+6],history[start:start+6],index=calibration_ring_step(
                        samples,sums[start:start+6],history[start:start+6],index)
                    self.assertEqual(m.word(0x20002f00),index)
                    self.assertEqual(bytes(m.uc.mem_read(0x200013c0,48)),struct.pack('<12i',*sums))
                    self.assertEqual(bytes(m.uc.mem_read(0x20001400,3072)),
                                     struct.pack('<768i',*(v for row in history for v in row)))
                    COUNTS['calibration_fast_ring_updates' if fast else 'calibration_slow_ring_updates']+=1

    def test_calibration_endpoint_thresholds_and_gains(self):
        from unicorn import UC_HOOK_MEM_WRITE, arm_const as arm
        import itertools
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        ordinary=itertools.product((1,2),(False,True),range(6),
                                   (-1,0,7999,8000,8001,59999,60000,60001,65535),(1801,61003))
        cases=[(*case,None) for case in ordinary]
        cases.extend((2,fast,channel,1000,1801,span) for fast,channel,span in
                     itertools.product((False,True),range(6),(-16777217,-1,0,1,16777217)))
        for stage,fast,channel,value,mean,span in cases:
            case=(stage,fast,channel,value,mean,span)
            current=[61000 if stage==1 else 1000]*6;current[channel]=value
            sums=[(mean+i*3)*64+(i*11%64) for i in range(6)]
            high=[62000+i*29 for i in range(12)];offsets=[2000+i*7 for i in range(12)]
            gains=[f32(.001+i*.0001) for i in range(12)];start=6 if fast else 0
            if span is not None:high[start+channel]=(sums[channel]>>6)+span
            m.uc.mem_write(0x20001340,struct.pack('<12i',*high))
            m.uc.mem_write(0x20001380,struct.pack('<12i',*offsets));m.floats(0x20001200,gains)
            m.word(0x200144d4,stage);m.word(m.sp+16,stage)
            writes=[]
            def record(uc,access,address,size,data,user):
                if address==0x58020818:writes.append(data)
            hook=m.uc.hook_add(UC_HOOK_MEM_WRITE,record)
            try:
                if fast:
                    # Supply the complete fast ring-update inputs; ring words
                    # are zero and sums chosen to produce the requested totals.
                    m.uc.mem_write(0x20001a00,bytes(1536));m.word(0x20002f00,0)
                    m.uc.mem_write(0x200013d8,struct.pack('<6i',*(s-v for s,v in zip(sums,current))))
                    m.r(12,current[0]);m.r(7,current[1])
                    m.word(m.sp+28,-current[0]);m.word(m.sp+36,-current[1])
                    for reg,v in zip((5,14,6,8),current[2:]):m.r(reg,v)
                    self.run_slice(m,0x0802f0a4,(0x0803051a,0x08030552))
                else:
                    m.uc.mem_write(0x200013c0,struct.pack('<6i',*sums))
                    m.uc.mem_write(0x20002000,struct.pack('<6i',*current));m.r(4,0x200013c0)
                    self.run_slice(m,0x080323ca if stage==1 else 0x080324d6,0x0802ec94)
            finally:m.uc.hook_del(hook)
            new_high,new_offsets,new_gains,ready=calibration_endpoints(
                stage,current,sums,high[start:start+6],offsets[start:start+6],gains[start:start+6])
            high[start:start+6]=new_high;offsets[start:start+6]=new_offsets;gains[start:start+6]=new_gains
            self.assertEqual(bytes(m.uc.mem_read(0x20001340,48)),struct.pack('<12i',*high),case)
            self.assertEqual(bytes(m.uc.mem_read(0x20001380,48)),struct.pack('<12i',*offsets),case)
            self.equal_floats(gains,m.floats(0x20001200,count=12),case)
            if fast:
                self.assertEqual(m.uc.reg_read(arm.UC_ARM_REG_PC),0x08030552 if ready else 0x0803051a,case)
                self.assertEqual(writes,[],case)
            else:self.assertEqual(writes,[64<<16,128<<16]+([] if ready else [64,128]),case)
            COUNTS['calibration_endpoint_cases']+=1

    def test_calibration_pitch_measurements_and_tail_fill(self):
        from unicorn import UC_HOOK_MEM_WRITE
        import itertools
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        cases=[(3,a,b,None) for a,b in itertools.product((-1,0,25,26,27,1998,1999,2000,2001,65535),repeat=2)]
        cases.extend((stage,8100*(stage-3)+a,8100*(stage-3)+b,None) for stage,a,b in
                     itertools.product(range(4,12),(-1501,-1500,-1499,0,1499,1500,1501),
                                       (-1501,-1500,-1499,0,1499,1500,1501)))
        cases.extend((stage,8100*(stage-3),8100*(stage-3),span) for stage,span in
                     itertools.product(range(4,12),(-16777217,-1,0,1,16777217)))
        for stage,a,b,span in cases:
            case=(stage,a,b,span);j=stage-3
            points=[[40+i*8100+side*17+i*i for i in range(16)] for side in range(2)]
            rates=[[f32(.25+side*.01+i*.002) for i in range(16)] for side in range(2)]
            if span is not None:
                for side,value in enumerate((a,b)):points[side][j-1]=value-span
            offsets=(2000,3101);sums=[(value+offset)*64+remainder for value,offset,remainder in
                                       zip((a,b),offsets,(0,63))]
            for addr,rows,kind in ((0x20001300,points[0],'i'),(0x200012c0,points[1],'i'),
                                   (0x20001280,rates[0],'f'),(0x20001240,rates[1],'f')):
                m.uc.mem_write(addr,struct.pack('<16'+kind,*rows))
            for side,index in enumerate((8,10)):
                m.word(0x200013c0+4*index,sums[side]);m.word(0x20001380+4*index,offsets[side])
            m.word(0x2000123c,0xa55a1234);m.word(0x20001340,0x1234a55a)
            m.word(0x200144d4,stage);m.r(3,stage);m.r(4,0x200013c0)
            writes=[]
            def record(uc,access,address,size,value,user):
                if address==0x58020818:writes.append(value)
            hook=m.uc.hook_add(UC_HOOK_MEM_WRITE,record)
            try:self.run_slice(m,0x0802f15e,0x0802f1aa)
            finally:m.uc.hook_del(hook)
            expected_points,expected_rates,accepted=calibration_pitch_tables(stage,sums,offsets,points,rates)
            for side,(point_addr,rate_addr) in enumerate(((0x20001300,0x20001280),(0x200012c0,0x20001240))):
                self.assertEqual(bytes(m.uc.mem_read(point_addr,64)),struct.pack('<16i',*expected_points[side]),case)
                self.equal_floats(expected_rates[side],m.floats(rate_addr,count=16),case)
            self.assertEqual(writes,[64,128]+[pin<<16 for pin,ok in zip((64,128),accepted) if ok],case)
            self.assertEqual(m.word(0x2000123c),0xa55a1234,case)
            self.assertEqual(m.word(0x20001340),0x1234a55a,case)
            COUNTS['calibration_pitch_cases']+=1

    def test_calibration_pitch_sequence_with_shift_advances(self):
        from unicorn import arm_const as arm
        for sequence in range(2):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            defaults=default_calibration()
            points=[list(defaults['pitch_breakpoints'])+[-123]*8 for _ in range(2)]
            rates=[[from_bits(0x3e808312)]*8+[f32(.75+sequence*.125)]*8 for _ in range(2)]
            offsets=(2000,2100)
            for side,(point_addr,rate_addr) in enumerate(((0x20001300,0x20001280),(0x200012c0,0x20001240))):
                m.uc.mem_write(point_addr,struct.pack('<16i',*points[side]));m.floats(rate_addr,rates[side])
                m.word(0x20001380+4*(8+2*side),offsets[side])
            m.word(0x200144d4,3)
            for stage in range(3,12):
                self.assertEqual(m.word(0x200144d4),stage)
                valid=[40+sequence*10,57+sequence*10] if stage==3 else [
                    8100*(stage-3)+sequence*40-17,8100*(stage-3)+sequence*30+29]
                if stage==11:valid=[63350+sequence*20,63380+sequence*20]
                for rejected in (True,False):
                    values=list(valid)
                    if rejected:values[sequence]=25 if stage==3 else 8100*(stage-3)-1500
                    sums=[(value+offset)*64+31 for value,offset in zip(values,offsets)]
                    for side,index in enumerate((8,10)):m.word(0x200013c0+4*index,sums[side])
                    m.r(3,stage);m.r(4,0x200013c0);self.run_slice(m,0x0802f15e,0x0802f1aa)
                    points,rates,_=calibration_pitch_tables(stage,sums,offsets,points,rates)
                    for side,(point_addr,rate_addr) in enumerate(((0x20001300,0x20001280),(0x200012c0,0x20001240))):
                        self.assertEqual(bytes(m.uc.mem_read(point_addr,64)),struct.pack('<16i',*points[side]))
                        self.equal_floats(rates[side],m.floats(rate_addr,count=16))
                    COUNTS['calibration_pitch_sequence_steps']+=1
                # Consume these exact generated RAM tables through the normal
                # pitch-coordinate paths, including every region boundary.
                for side in range(2):
                    codes=[0,offsets[side],65535]+[p+offsets[side]+d for p in points[side][:8] for d in (-1,0,1)]
                    for raw in codes:
                        if not 0<=raw<=65535:continue
                        if side==0:
                            m.r(5,raw);m.r(3,offsets[side]);m.r(0,0x20002000)
                            self.run_slice(m,0x0802ee80,0x0802eeb0);actual=m.r(2)
                        else:
                            m.r(6,raw);self.run_slice(m,0x0802ef10,0x0802ef42);actual=m.r(3)
                        self.assertEqual(actual,pitch_coordinate(raw,offsets[side],points[side][:8],rates[side][:8]),
                                         (sequence,stage,side,raw))
                        COUNTS['generated_pitch_coordinate_cases']+=1
                # Genuine GPIO press, ten held callbacks and a falling edge.
                m.word(0x58021810,128)
                for _ in range(11):
                    m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                    self.assertEqual(m.word(0x200144d4),stage)
                self.assertEqual(m.word(0x20002f38),10)
                m.word(0x58021810,0);m.r(14,0x08020001)
                self.run_slice(m,0x0802d900,(0x08020000,0x0802d330))
                self.assertEqual(m.word(0x200144d4),stage+1)
                self.assertEqual(m.uc.reg_read(arm.UC_ARM_REG_PC),0x0802d330 if stage==11 else 0x08020000)
                COUNTS['calibration_pitch_ui_advances']+=1

    def test_interaction_led_patterns(self):
        from unicorn import UC_HOOK_MEM_WRITE
        for interaction in range(3):
            for counter in (0,8191,8192,16383,16384,32767,32768,0xffffffff):
                for display,select_a,select_b in ((0,0,0),(1,0,0),(0,1,0),(0,0,1)):
                    m=SliceMachine();m.uc.mem_map(0x58020000,0x10000);m.uc.mem_map(0x40000000,0x10000)
                    m.uc.mem_write(0x20002f54,bytes([interaction]));m.word(0x20002eec,counter)
                    m.word(0x200144d4,display);m.word(0x20002f08,select_a);m.word(0x20002f04,select_b)
                    m.floats(0x200023e0,[1.]);m.floats(0x200023e8,[1.])
                    writes=[]
                    def record(uc,access,address,size,value,user):
                        if address==0x58020418:writes.append(value)
                    handle=m.uc.hook_add(UC_HOOK_MEM_WRITE,record)
                    try:
                        m.r(14,0x08020001);self.run_slice(m,0x0802d464,0x08020000)
                    finally:m.uc.hook_del(handle)
                    low=interaction==1 or (interaction==2 and counter&8192)
                    self.assertEqual(writes,[0x80000000 if low else 0x8000],
                                     (interaction,counter,display,select_a,select_b))
                    COUNTS['interaction_led_cases']+=1

    def check_indicator_register_cases(self,cases,count_key):
        from unicorn import UC_HOOK_MEM_WRITE
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000);m.uc.mem_map(0x40000000,0x10000)
        m.floats(0x200023e0,[1.]);m.floats(0x200023e8,[1.])
        pwm_addresses=(0x40001834,0x4000083c,0x40000040,0x4000003c)
        gpio={0x58020c18:[],0x58020818:[]}
        def record(uc,access,address,size,value,user):
            if address in gpio:gpio[address].append(value)
        hook=m.uc.hook_add(UC_HOOK_MEM_WRITE,record)
        try:
            for supplied in cases:
                state=dict(sample_counter=0,display_mode=0,selection_flags=(0,0),slots=(0,0),engines=(0,0),
                           sao=(0,0),capture=(0,0),stored_linear=(0,0),live_linear=(0,0),
                           shift_states=(0,0),lf=(0,0),countdown=(0,0))
                state.update(supplied)
                m.word(0x20002eec,state['sample_counter']);m.word(0x200144d4,state['display_mode'])
                pairs={'selection_flags':(0x20002f08,0x20002f04),'slots':(0x20002430,0x20000b20),
                       'engines':(0x20002e8c,0x20002e88),'capture':(0x20002e84,0x20002e80),
                       'live_linear':(0x20002ec4,0x20002ec0),'lf':(0x200023d0,0x20000820),
                       'countdown':(0x2000242c,0x20002428)}
                for key,addresses in pairs.items():
                    for addr,value in zip(addresses,state[key]):m.word(addr,value)
                m.uc.mem_write(0x20002f52,bytes(state['sao']))
                m.uc.mem_write(0x20002f60,bytes(state['shift_states']))
                m.uc.mem_write(0x20013490,struct.pack('<2h',*state['stored_linear']))
                m.word(0x20002ef0,99)
                for addr in pwm_addresses:m.word(addr,0xa5a5a5a5)
                for values in gpio.values():values.clear()
                m.r(14,0x08020001);self.run_slice(m,0x0802d464,0x08020000)
                pwm,override,mode_high,aux_high=indicator_register_values(**state)
                self.assertEqual(tuple(m.word(addr) for addr in pwm_addresses),pwm,state)
                self.assertEqual(m.word(0x20002ef0),override,state)
                self.assertEqual(gpio[0x58020c18],[pin if high else pin<<16
                                                  for pin,high in zip((4096,8192),mode_high)],state)
                self.assertEqual(gpio[0x58020818],[pin if high else pin<<16
                                                  for pin,high in zip((64,128),aux_high) if high is not None],state)
                COUNTS[count_key]+=1
        finally:m.uc.hook_del(hook)

    def test_indicator_display_priority_and_engine_bits(self):
        def cases():
            for display in range(1,16):
                for counter in (0,32767,32768,65535,65536):
                    for timers in ((0,0),(1,0),(0,1),(1,1)):
                        yield dict(display_mode=display,sample_counter=counter,selection_flags=timers,
                                   slots=(5,10),engines=(1,2))
            for slot in range(16):
                for timers in ((-1,0),(0,0),(1,0),(0,1),(1,1)):
                    for a in range(3):
                        for b in range(3):
                            yield dict(selection_flags=timers,slots=(slot,15-slot),engines=(a,b))
        self.check_indicator_register_cases(cases(),'indicator_priority_cases')

    def test_indicator_mode_capture_and_aux_truth_table(self):
        import itertools
        def cases():
            for side,mismatch,raw,lf,remaining,sao,capture,counter in itertools.product(
                    range(2),range(2),range(3),range(3),(-1,0,1),range(2),range(3),(0,4096,8192,12288)):
                state=dict(sample_counter=counter,selection_flags=(1,1),slots=(3,12),live_linear=(1,1))
                for key,value in (('stored_linear',1-mismatch),('shift_states',raw),('lf',lf),
                                  ('countdown',remaining),('sao',sao),('capture',capture)):
                    pair=[1,1] if key=='stored_linear' else [0,0];pair[side]=value;state[key]=tuple(pair)
                yield state
        self.check_indicator_register_cases(cases(),'indicator_mode_aux_cases')

    def test_tuning_beacon_windows_and_pin_retention(self):
        from unicorn import UC_HOOK_MEM_WRITE
        ratios=[0.,.25,.5,1.,2.,4.,8.,1.2,1.25,4/3,1.5,1.6,5/3,6/5,5/6]
        for boundary in (.99,1.01,1.24,1.26,1.323,1.343,1.49,1.51,1.59,1.61,1.656,1.676):
            for delta in (-1,0,1):
                value=from_bits(bits(f32(boundary))+delta)
                ratios.extend(mul(value,scale) for scale in (.25,1.,4.))
        rng=random.Random(6718);ratios.extend(2**rng.uniform(-8,8) for _ in range(64))
        for ratio in ratios:
            for initial in (0,0x100,0x400,0x500):
                m=SliceMachine();m.uc.mem_map(0x58020000,0x10000);m.uc.mem_map(0x40000000,0x10000)
                m.floats(0x200023e0,[1.]);m.floats(0x200023e8,[f32(ratio)])
                state=[initial];writes=[]
                def record(uc,access,address,size,value,user):
                    if address==0x58020018:
                        writes.append(value)
                        state[0]=(state[0]|(value&0xffff))&~(value>>16)
                handle=m.uc.hook_add(UC_HOOK_MEM_WRITE,record)
                try:
                    m.r(14,0x08020001);self.run_slice(m,0x0802d464,0x08020000)
                finally:m.uc.hook_del(handle)
                action=tuning_beacon_action(ratio,1.)
                expected_writes={'green':[0x01000000],'red':[0x04000000],'off':[0x400,0x100]}[action]
                self.assertEqual(writes,expected_writes,(ratio,initial,action))
                expected=initial&~0x100 if action=='green' else initial&~0x400 if action=='red' else 0x500
                self.assertEqual(state[0],expected)
                COUNTS['tuning_beacon_cases']+=1

    def test_shifted_auxiliary_mode_cycles(self):
        for side in range(2):
            for sao in (0,1):
                for mode in range(6):
                    m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                    addr=0x20002ef4 if side else 0x20002ef8
                    m.word(addr,mode);m.word(0x20013494+side*4,mode)
                    m.word(0x20002e90 if side else 0x20002e94,sao)
                    m.word(0x20002f34+side*4,51)
                    m.word(0x58021810,128 if side else 64)
                    m.word(0x20002f58,0x100 if side else 1)
                    m.word(0x58020c10,8)  # shared interaction/CV button
                    m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                    expected=(mode+1)%6
                    if sao and expected==0:expected=1
                    self.assertEqual(m.word(addr),expected,(side,sao,mode))
                    self.assertEqual(m.word(0x20013494+side*4),expected)
                    self.assertEqual(m.uc.mem_read(0x20002f54,1)[0],0)
                    self.assertEqual(m.word(0x20002eb8),1)
                    COUNTS['aux_button_cases']+=1

    def test_clock_interrupt_through_ui_handler(self):
        for side in ('A','B'):
            is_b=side=='B'
            for mode in range(6):
                for period in (0,1,1023,1024,5999,6000,6001,10000,
                               0x5fffffff,0x60000000,0x60000100,0xffffffff):
                    for capturing in (0,1):
                        for dimension,second in ((0,1),(8,1),(65,1),(8,8)):
                            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                            m.uc.mem_map(0x60000000,0x1000)
                            short_rates=[f32(.001+j*.000001) for j in range(1024)]
                            m.floats(0x60000000,short_rates)
                            descriptor=0x20000920 if is_b else 0x20000a20
                            m.word(descriptor+4,dimension);m.word(descriptor+8,second)
                            m.word(0x20002e80 if is_b else 0x20002e84,capturing)
                            m.word(0x20002ef4 if is_b else 0x20002ef8,mode)
                            rate_addr=0x200023f4 if is_b else 0x200023f8
                            m.floats(rate_addr,[.125])
                            m.word(0x20002f10 if is_b else 0x20002f0c,period-1)
                            offset_addr=0x20002434 if is_b else 0x20002438
                            m.word(offset_addr,max(0,dimension-1))
                            # Stable GPIO levels avoid a concurrent button-edge gesture.
                            m.word(0x58021810,192);m.word(0x20002f58,0x101)
                            m.r(0,128 if is_b else 64);m.r(14,0x08020001)
                            self.run_slice(m,0x08032688,0x08020000)
                            m.r(14,0x08020001)
                            start=0x0802e37e if is_b else 0x0802e2ca
                            self.run_slice(m,0x0802d900,start)
                            def snapshot():
                                return AdmittedClockState(dimension,second,m.word(offset_addr),capturing,mode,
                                    m.word(0x20002f10 if is_b else 0x20002f0c),m.floats(rate_addr)[0],
                                    m.word(0x2001348c+(28 if is_b else 24)),m.word(0x20002eb8),
                                    m.word(0x20002ecc if is_b else 0x20002ed0),
                                    m.word(0x20002ed4 if is_b else 0x20002ed8))
                            expected=admitted_clock_action(snapshot(),short_rates)
                            self.run_slice(m,start,0x08020000)
                            self.assertEqual(snapshot(),expected,(side,mode,period,capturing,dimension,second))
                            COUNTS['admitted_clock_model_cases']+=1
                            self.assertEqual(m.word(0x20002edc if is_b else 0x20002ee4),0)
                            self.assertEqual(m.word(0x20002ed4 if is_b else 0x20002ed8),1)
                            expected_count=1 if capturing else (dimension+63)//64
                            self.assertEqual(m.word(0x20002ecc if is_b else 0x20002ed0),expected_count)
                            if mode in (0,5):
                                advanced=max(0,dimension-1)+1
                                self.assertEqual(m.word(offset_addr),advanced if advanced<dimension*second else 0)
                                expected_rate=.125
                            else:
                                self.assertEqual(m.word(offset_addr),max(0,dimension-1))
                                expected_rate=(short_rates[period] if 1<=period<=1023 else
                                               f32((1/64)/period) if 1024<=period<=5999 else .125)
                                display_period=min(0x7fffffff,int(mul(f32(period),f32(1.333333254))))
                                self.assertEqual(m.word(0x2001348c+(28 if is_b else 24)),display_period)
                            self.equal_floats([expected_rate],m.floats(rate_addr))
                            self.assertEqual(m.word(0x20002f10 if is_b else 0x20002f0c),0)
                            COUNTS['clock_ui_cases']+=1

    def test_normal_shift_stage_model(self):
        import random
        from unicorn.arm_const import UC_ARM_REG_PC
        rng=random.Random(67031)
        words={'pending_clock':(0x20002ee4,0x20002edc),'hold':(0x20002f34,0x20002f38),
            'linear_runtime':(0x20002ec4,0x20002ec0),'slot':(0x20002430,0x20000b20),
            'slot_mirror':(0x200134b4,0x200134b8),'scan':(0x20002438,0x20002434),
            'capture':(0x20002e84,0x20002e80),'auxiliary':(0x20002ef8,0x20002ef4),
            'elapsed':(0x20002f0c,0x20002f10),'period_mirror':(0x200134a4,0x200134a8),
            'countdown':(0x20002ed0,0x20002ecc),'policy':(0x20002ed8,0x20002ed4)}
        floats={'phase':(0x20002418,0x20002414),'rate':(0x200023f8,0x200023f4)}
        shorts=(0x20013490,0x20013492);descs=(0x20000a20,0x20000920)
        rates=[0.]+[f32((1/64)/j) for j in range(1,1024)]
        for case in range(1536):
            mask=case%32;previous=(case//32)%4;clocks=(case//128)%4
            current=tuple((mask>>i)&1 for i in range(5))
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000);m.uc.mem_map(0x60000000,0x1000)
            m.floats(0x60000000,rates)
            m.word(0x58021810,(64 if mask&1 else 0)|(128 if mask&2 else 0))
            m.word(0x58020c10,(128 if mask&4 else 0)|(8 if mask&16 else 0))
            m.word(0x58020410,64 if mask&8 else 0)
            for i in range(2):
                state=ShiftSideState(previous=(previous>>i)&1,gesture=rng.choice((0,1,2,3,4)),
                    pending_clock=(clocks>>i)&1,hold=rng.choice((0,9,10,49,50,51,499,500,1499,1500,1501,1600,1601)),
                    linear_setting=rng.choice((0,1,0x8000,0xffff)),linear_runtime=7,
                    slot=rng.randrange(16),slot_mirror=rng.randrange(16),scan=rng.choice((0,7,63,0xffffffff)),
                    capture=rng.randrange(2),auxiliary=rng.randrange(6),phase=-0.25,
                    elapsed=rng.choice((0,1,1022,1023,5998,5999,6000,0xffffffff)),rate=f32(.123),
                    period_mirror=rng.choice((0,1,8000)),countdown=rng.choice((0,1,2,8,0x80000000)),policy=rng.randrange(2),
                    dimensions=tuple(rng.choice(((0,1),(8,8),(65,1),(0x80000000,1))) for _ in range(16)))
                for name,addresses in words.items():m.word(addresses[i],getattr(state,name))
                for name,addresses in floats.items():m.floats(addresses[i],[getattr(state,name)])
                m.uc.mem_write(shorts[i],struct.pack('<H',state.linear_setting))
                m.uc.mem_write(0x20002f58+i,bytes((state.previous,)))
                m.uc.mem_write(0x20002f48+i,bytes((state.gesture,)))
                for slot,dimensions in enumerate(state.dimensions):
                    m.word(descs[i]+16*slot+4,dimensions[0]);m.word(descs[i]+16*slot+8,dimensions[1])
            m.word(0x20002f08,7);m.word(0x20002f04,19);m.word(0x20002eb8,case&1)
            def snapshot():
                sides=[]
                for i in range(2):
                    values={name:m.word(addresses[i]) for name,addresses in words.items()}
                    values.update({name:m.floats(addresses[i])[0] for name,addresses in floats.items()})
                    values.update(previous=m.uc.mem_read(0x20002f58+i,1)[0],gesture=m.uc.mem_read(0x20002f48+i,1)[0],
                        linear_setting=struct.unpack('<H',m.uc.mem_read(shorts[i],2))[0],
                        dimensions=tuple((m.word(descs[i]+16*j+4),m.word(descs[i]+16*j+8)) for j in range(16)))
                    sides.append(ShiftSideState(**values))
                return ShiftStageState(tuple(sides),tuple(m.word(a) for a in (0x20002f08,0x20002f04)),m.word(0x20002eb8))
            initial=snapshot();expected=normal_shift_stage(initial,current,rates)
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,(0x0802dce4,0x0802e698,0x0802e678))
            pc=m.uc.reg_read(UC_ARM_REG_PC)
            reset={0x0802dce4:None,0x0802e698:0,0x0802e678:1}[pc]
            self.assertEqual(ShiftStageResult(snapshot(),reset),expected,(case,current,initial))
            COUNTS['normal_shift_stage_cases']+=1
            COUNTS['normal_shift_stage_reset_boundaries']+=int(reset is not None)
            if reset is not None:
                # Execute the original factory copy, then supply a save return.
                # File/flash work is intentionally outside this continuation fixture.
                bank=(0x60c01000,0x60001000)[reset]
                m.uc.mem_map(bank,0x4000)
                m.run(pc,(0x0802e6a6,0x0802e686)[reset],limit=100000)
                COVERAGE.update(m.last_trace)
                self.assertEqual(bytes(m.uc.mem_read(bank,16384)),bytes(m.uc.mem_read(0x08045a98,16384)))
                desc=descs[reset]+16*initial.sides[reset].slot
                self.assertEqual((m.word(desc+4),m.word(desc+8),m.word(desc+12)),(8,8,0))
                self.assertEqual(m.r(0),reset)
                # Explicitly supplied post-save dirty state and varied return words.
                m.word(0x20002eb8,(case//2)&1);m.r(0,(0,1,0xffffffff)[case%3])
                resumed=resume_shift_after_reset(snapshot(),reset,current,rates)
                self.run_slice(m,(0x0802e6aa,0x0802e68a)[reset],
                               (0x0802dce4,0x0802e698,0x0802e678))
                pc=m.uc.reg_read(UC_ARM_REG_PC)
                reset={0x0802dce4:None,0x0802e698:0,0x0802e678:1}[pc]
                self.assertEqual(ShiftStageResult(snapshot(),reset),resumed,(case,'resumed'))
                self.assertIsNone(reset)  # A is now 1601, preventing B from winning a second reset.
                COUNTS['normal_shift_reset_resumptions']+=1
            if reset is None:
                tail=button_inputs(*self.button_action_snapshot(m),ButtonEdgeState(),current[2:])
                self.run_slice(m,0x0802dce4,0x08020000)
                edge=ButtonEdgeState(tuple(m.uc.mem_read(0x20002f5a,3)),
                    tuple(m.word(0x20002f28+4*j) for j in range(3)),
                    tuple(m.word(0x20002f3c+4*j) for j in range(3)))
                self.assertEqual((*self.button_action_snapshot(m),edge),tail,(case,current))
                COUNTS['normal_shift_button_tail_cases']+=1
        self.assertGreater(COUNTS['normal_shift_stage_reset_boundaries'],0)

    def test_shift_release_finish_model(self):
        from unicorn.arm_const import UC_ARM_REG_SP
        import itertools
        for side,gesture,elapsed,display in itertools.product(range(2),(0,1,2,3,4,5,255),
                ((0,0),(1,9),(6000,6001),(0xffffffff,0x80000000)),((0,0),(1,1),(7,19))):
            m=SliceMachine()
            for i in range(2):
                m.word(0x20002f0c+4*i,elapsed[i]);m.word((0x20002f08,0x20002f04)[i],display[i])
                m.word(0x20002f34+4*i,77+i)
            m.uc.mem_write(0x20002f48+1-side,bytes((gesture,)))
            m.r(6,0x20002f0c);m.r(5,0x20002f48);m.r(2,0x20002f58)
            m.word(m.uc.reg_read(UC_ARM_REG_SP)+4,gesture)
            self.run_slice(m,(0x0802db88,0x0802da44)[side],
                           (0x0802dc9a,0x0802db9a) if side==0 else 0x0802dce4)
            actual=(tuple(m.word(0x20002f0c+4*i) for i in range(2)),
                    tuple(m.word(a) for a in (0x20002f08,0x20002f04)))
            self.assertEqual(actual,shift_release_finish(side,elapsed,display,gesture),
                             (side,gesture,elapsed,display))
            self.assertEqual(tuple(m.word(0x20002f34+4*i) for i in range(2)),(77,78))
            COUNTS['shift_release_finish_cases']+=1

    def test_shift_passive_action_models(self):
        import itertools
        for side,press,aux,count,linear,scan,dims in itertools.product(
                range(2),range(2),range(6),(0,1,2,0x80000000,0xffffffff),
                (0,1,0x7fff,0x8000,0xffff),(0,63,0xffffffff),((8,8),(0,1),(0x80000000,1))):
            # Press ignores scan/countdown/linear; sample those dimensions once.
            if press and (count!=0 or scan!=0 or dims!=(8,8)):continue
            state=PassiveShiftState(77,aux,-0.25,linear,123,scan,*dims,count)
            m=SliceMachine()
            hold=0x20002f34+4*side;mode=(0x20002ef8,0x20002ef4)[side]
            phase=(0x20002418,0x20002414)[side];runtime=(0x20002ec4,0x20002ec0)[side]
            scanaddr=(0x20002438,0x20002434)[side];desc=(0x20000a20,0x20000920)[side]
            m.word(hold,77);m.word(mode,aux);m.floats(phase,[-0.25]);m.word(runtime,123)
            m.uc.mem_write(0x20013490+2*side,struct.pack('<H',linear))
            m.word(scanaddr,scan);m.word(desc+4,dims[0]);m.word(desc+8,dims[1])
            m.word((0x20002ed0,0x20002ecc)[side],count)
            m.r(1,0x20002f34);m.r(4,0x20002f60);m.r(3,count);m.r(11,0x20002ecc)
            start=((0x0802e07c,0x0802d9d4),(0x0802dcbe,0x0802dd4a))[side][press]
            self.run_slice(m,start,(0x0802d9ee,0x0802dce4)[side])
            actual=PassiveShiftState(m.word(hold),m.word(mode),m.floats(phase)[0],linear,
                m.word(runtime),m.word(scanaddr),m.word(desc+4),m.word(desc+8),
                m.word((0x20002ed0,0x20002ecc)[side]))
            expected=shift_press_action(state) if press else shift_idle_action(side,state)
            self.assertEqual(actual,expected,(side,press,state))
            COUNTS['shift_passive_action_cases']+=1

    def test_shift_dispatch_model(self):
        from unicorn.arm_const import UC_ARM_REG_PC
        import itertools
        for side in range(2):
            boundaries=((0x0802e0cc,0x0802da90,0x0802e07c,0x0802d9d4),
                        (0x0802dfb8,0x0802da18,0x0802dfe6,0x0802dcbe,0x0802dd4a))[side]
            routes=(('hold','release','idle','press'),
                    ('hold','release','release','idle','press'))[side]
            for mask,previous,gesture,clock in itertools.product(range(32),range(2),(0,1,2,3,4,5,255),(0,1,2,0xffffffff)):
                current=tuple((mask>>i)&1 for i in range(5))
                m=SliceMachine()
                m.uc.mem_write(0x20002f60,bytes(current))
                m.uc.mem_write(0x20002f48+side,bytes((gesture,)))
                m.uc.mem_write(0x20002f58+side,bytes((previous,)))
                event=(0x20002ee4,0x20002edc)[side];m.word(event,clock)
                m.r(4,0x20002f60);m.r(5,0x20002f48)
                m.r(2,0x20002f58);m.r(0,current[1])
                m.r(1,0x20002f34 if side else current[4])
                self.run_slice(m,(0x0802d99e,0x0802d9ee)[side],boundaries)
                route=routes[boundaries.index(m.uc.reg_read(UC_ARM_REG_PC))]
                actual=ConsumedShiftRoute(m.uc.mem_read(0x20002f58+side,1)[0],
                    m.uc.mem_read(0x20002f48+side,1)[0],m.word(event),route)
                self.assertEqual(actual,shift_dispatch(side,current,previous,gesture,clock),
                                 (side,mask,previous,gesture,clock))
                COUNTS['shift_dispatch_cases']+=1

    def test_consumed_shift_route_model(self):
        from unicorn.arm_const import UC_ARM_REG_PC
        for side in range(2):
            boundaries=((0x0802e0cc,0x0802da90,0x0802e07c),
                        (0x0802dfb8,0x0802dcbe))[side]
            routes=(('hold','release','idle'),('hold','idle'))[side]
            for mask in range(32):
                current=tuple((mask>>i)&1 for i in range(5))
                for clock in (0,1,2,0xffffffff):
                    m=SliceMachine()
                    m.uc.mem_write(0x20002f60,bytes(current))
                    m.uc.mem_write(0x20002f48,b'\x04\x04')
                    m.uc.mem_write(0x20002f58,b'\x01\x01')
                    event=(0x20002ee4,0x20002edc)[side];m.word(event,clock)
                    m.r(4,0x20002f60);m.r(5,0x20002f48)
                    m.r(2,0x20002f58);m.r(0,current[1])
                    m.r(1,0x20002f34 if side else current[4])
                    self.run_slice(m,(0x0802d99e,0x0802dc9c)[side],boundaries)
                    route=routes[boundaries.index(m.uc.reg_read(UC_ARM_REG_PC))]
                    actual=ConsumedShiftRoute(m.uc.mem_read(0x20002f58+side,1)[0],
                        m.uc.mem_read(0x20002f48+side,1)[0],m.word(event),route)
                    self.assertEqual(actual,consumed_shift_route(side,current,clock),(side,mask,clock))
                    COUNTS['consumed_shift_route_cases']+=1

    def test_shift_release_decision_model(self):
        from unicorn.arm_const import UC_ARM_REG_PC
        values=(0,9,10,49,50,51,499,500,1498,1499,1500,1600,1601,0x80000000,0xffffffff)
        for side in range(2):
            boundaries=((0x0802dc50,0x0802e2ca,0x0802db88),
                        (0x0802e4ec,0x0802e37e,0x0802da44))[side]
            for hold in values:
                for other in values:
                    m=SliceMachine()
                    m.r(1,0x20002f34);m.r(5,0x20002f48)
                    m.word(0x20002f34,other if side else hold)
                    m.word(0x20002f38,hold if side else other)
                    self.run_slice(m,(0x0802e422,0x0802e36c)[side],boundaries)
                    actual=('select_other','clock_self','none')[boundaries.index(m.uc.reg_read(UC_ARM_REG_PC))]
                    self.assertEqual(actual,shift_release_action(hold,other),(side,hold,other))
                    COUNTS['shift_release_decision_cases']+=1

    def test_stable_gpio_clock_admission(self):
        for pins in (0,64,128,192):
            for hold_a in (0,49,50,51):
                for hold_b in (0,49,50,51):
                    for edges in range(4):
                        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                        for desc in (0x20000a20,0x20000920):
                            m.word(desc+4,65);m.word(desc+8,1)
                        m.word(0x58021810,pins)
                        m.word(0x20002f58,(1 if pins&64 else 0)+(256 if pins&128 else 0))
                        m.word(0x20002f34,hold_a);m.word(0x20002f38,hold_b)
                        for side in range(2):
                            if edges&(1<<side):
                                m.r(0,128 if side else 64);m.r(14,0x08020001)
                                self.run_slice(m,0x08032688,0x08020000)
                        # Stable previous button levels, idle gesture state,
                        # no Array/CV button and counters restricted to 0..51.
                        admit_a=bool(edges&1) and hold_b<=49
                        a_hold_at_b=hold_a if edges&1 else hold_a+1 if pins&64 else 0
                        admit_b=bool(edges&2) and bool(pins&128) and a_hold_at_b<=49
                        m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                        context=(pins,hold_a,hold_b,edges)
                        for side,admitted in enumerate((admit_a,admit_b)):
                            self.assertEqual(m.word(0x20002ed4 if side else 0x20002ed8),int(admitted),context)
                            self.assertEqual(m.word(0x20002ecc if side else 0x20002ed0),2 if admitted else 0,context)
                            self.assertEqual(m.word(0x20002edc if side else 0x20002ee4),0,context)
                        COUNTS['clock_admission_cases']+=1

    def test_concurrent_clocks_array_buttons_and_capture_tail(self):
        """Stable held Shifts, SAM, bounded counters; no media or DSP substitution."""
        import itertools
        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
        banks=(0x60c01000,0x60001000)
        origins=(0,1048576)
        for side,bank in enumerate(banks):m.uc.mem_map(bank+origins[side]*4,0x50000)
        sentinels=struct.pack('<64f',*([-9.]*64))
        frames=[[f32((j+1)/(64+side*16)) for j in range(64)] for side in range(2)]
        addresses={
            'active':(0x20002e84,0x20002e80),'cursor':(0x20002e7c,0x200011a0),
            'peak':(0x20002e9c,0x200011e0),'policy':(0x20002ed8,0x20002ed4),
            'count':(0x20002ed0,0x20002ecc),'scan':(0x20002438,0x20002434),
            'slot':(0x20002430,0x20000b20),'desc':(0x20000a20,0x20000920),
            'clock':(0x20002ee4,0x20002edc),'bank':(0x20002f74,0x20002f70),
            'amps':(0x20002b40,0x20002d40)}
        for ha,hb,edges,buttons,active_mask,policy_mask,length in itertools.product(
                (0,49,50,51),(0,49,50,51),range(4),(1,2,3),range(4),range(4),(8,65)):
            case=(ha,hb,edges,buttons,active_mask,policy_mask,length)
            m.uc.mem_write(0x20000800,bytes(0x2800));m.uc.mem_write(0x2001348c,bytes(64))
            m.word(0x58021810,192);m.word(0x20002f58,257)
            m.word(0x20002f48,257)
            m.word(0x58020c10,128 if buttons&1 else 0);m.word(0x58020410,64 if buttons&2 else 0)
            m.word(0x20002f34,ha);m.word(0x20002f38,hb)
            states=[]
            for side in range(2):
                active=int(bool(active_mask&(1<<side)));policy=int(bool(policy_mask&(1<<side)))
                descriptors=[[origins[side],1024 if active else length,1,0 if active else 1],
                             [origins[side]+65536,8,8,1]]
                state=dict(active=active,policy=policy,count=2,cursor=origins[side]+17*64,peak=.25,
                           scan=descriptors[0][1]-1,slot=0,mode=0,descriptors=descriptors)
                states.append(state)
                for key in ('active','policy','cursor','scan'):m.word(addresses[key][side],state[key])
                m.word(addresses['count'][side],3);m.floats(addresses['peak'][side],[.25])
                m.word(addresses['bank'][side],banks[side]);m.floats(addresses['amps'][side],frames[side])
                for slot,desc in enumerate(descriptors):
                    m.uc.mem_write(addresses['desc'][side]+slot*16,struct.pack('<4I',*desc))
                for cursor in (0,17*64,65536):m.uc.mem_write(banks[side]+(origins[side]+cursor)*4,sentinels)
                if edges&(1<<side):
                    m.r(0,128 if side else 64);m.r(14,0x08020001)
                    self.run_slice(m,0x08032688,0x08020000)
            # Clock processing precedes Array edges. A is processed first;
            # a clock can enter the opposite-Shift release/selection branch.
            holds=[ha,hb];admitted=[False,False]
            if edges&1:
                if hb<=49:admitted[0]=True
                elif ha>=51:states[1]['slot']=1;holds[1]=1601
            else:holds[0]+=1
            if states[1]['slot']:
                holds[1]=1600  # selection's gesture=4 suppresses the B clock
            elif edges&2:
                if holds[0]<=49:admitted[1]=True
                elif hb>=51:states[0]['slot']=1;holds[0]=1601
            else:holds[1]+=1
            for side,state in enumerate(states):
                if admitted[side]:
                    state['policy']=1
                    state['count']=1 if state['active'] else (state['descriptors'][0][1]+63)//64
                    state['scan']=0
                if state['slot']:state['scan']=0
                if buttons&(1<<side):
                    if holds[side]>50:
                        holds[side]=1601;desc=state['descriptors'][state['slot']]
                        if state['active']:
                            state['active']=0;desc[1]=((state['cursor']-desc[0])&0xffffffff)>>6
                            if side==0:state['policy']=0
                        else:
                            state['active']=1;state['cursor']=desc[0];state['peak']=0.
                            desc[1:]=[1024,1,0]
                    elif not state['active']:state['mode']=1
            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
            for side,state in enumerate(states):
                for key in ('active','policy','count','cursor','scan','slot'):
                    self.assertEqual(m.word(addresses[key][side]),state[key],(case,side,key))
                self.equal_floats([state['peak']],m.floats(addresses['peak'][side]),case)
                self.assertEqual(m.word(0x20002f34+side*4),holds[side],case)
                self.assertEqual(m.uc.mem_read(0x20002f52+side,1)[0],state['mode'],case)
                self.assertEqual(m.word(0x200134b4+side*4),state['slot'],case)
                self.assertEqual(m.word(addresses['clock'][side]),0,case)
                for slot,desc in enumerate(state['descriptors']):
                    self.assertEqual(bytes(m.uc.mem_read(addresses['desc'][side]+slot*16,16)),
                                     struct.pack('<4I',*desc),(case,side,slot))
                    COUNTS['clock_capture_unsigned_stop_lengths']+=int(desc[1]>1024)
            COUNTS['concurrent_clock_capture_cases']+=1
            COUNTS['clock_selected_array_cases']+=int(any(state['slot'] for state in states))
            # Join the original active checks and both capture writers. The
            # analyzer spectra are explicit inputs; the DSP body is not run.
            m.word(m.sp+148,0x20002ea4)
            self.run_slice(m,0x0802e7de,0x0802e7fc)
            for side,state in enumerate(states):
                write=state['active']==1 and (state['policy']==0 or state['count']==1)
                for relative_cursor in (0,17*64,65536):
                    cursor=origins[side]+relative_cursor
                    expected=struct.pack('<64f',*frames[side]) if write and cursor==state['cursor'] else sentinels
                    self.assertEqual(bytes(m.uc.mem_read(banks[side]+cursor*4,256)),expected,(case,side,cursor))
                self.assertEqual(m.word(addresses['cursor'][side]),state['cursor']+(64 if write else 0),case)
                self.equal_floats([max(state['peak'],max(frames[side])) if write else state['peak']],
                                  m.floats(addresses['peak'][side]),case)
                self.assertEqual(m.word(addresses['active'][side]),state['active'],case)
            COUNTS['concurrent_clock_capture_tail_calls']+=1

    def test_persistent_capture_clock_selection_stop(self):
        """Real press/hold histories reach the unsigned stop length on each side."""
        for side in range(2):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            bank=0x60001000 if side else 0x60c01000
            origin=1048576 if side else 0
            m.uc.mem_map(bank+origin*4,0x50000)
            desc=0x20000920 if side else 0x20000a20
            active=0x20002e80 if side else 0x20002e84
            cursor=0x200011a0 if side else 0x20002e7c
            slot=0x20000b20 if side else 0x20002430
            m.word(0x20002f70 if side else 0x20002f74,bank)
            m.uc.mem_write(desc,struct.pack('<8I',origin,8,8,1,origin+65536,8,8,1))
            spectrum=[f32((j+1)/64) for j in range(64)]
            m.floats(0x20002d40 if side else 0x20002b40,spectrum)
            writes=0
            def step(shifts,button=False,opposite_clock=False):
                nonlocal writes
                m.word(0x58021810,shifts)
                m.word(0x58020c10,128 if button and side==0 else 0)
                m.word(0x58020410,64 if button and side==1 else 0)
                if opposite_clock:
                    m.r(0,64 if side else 128);m.r(14,0x08020001)
                    self.run_slice(m,0x08032688,0x08020000)
                m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                before=m.word(cursor);recording=m.word(active)==1
                m.word(m.sp+148,0x20002ea4)
                self.run_slice(m,0x0802e7de,0x0802e7fc)
                self.assertEqual(m.word(cursor),before+(64 if recording else 0))
                if recording:
                    self.equal_floats(spectrum,list(struct.unpack('<64f',m.uc.mem_read(bank+before*4,256))))
                    writes+=1
            # Establish the capture entirely through original GPIO handling.
            for _ in range(53):step(128 if side else 64)
            step(128 if side else 64,True)
            self.assertEqual(m.word(active),1)
            self.assertEqual(m.word(cursor),origin+64)
            step(0)  # release the latched capture gesture; capture continues
            for _ in range(52):step(192)
            self.assertEqual(m.word(active),1)
            self.assertEqual(m.word(slot),0)
            old_cursor=m.word(cursor)
            self.assertEqual(old_cursor,origin+writes*64)
            # The other side's clock selects slot 1 before this Array edge.
            step(192,True,True)
            self.assertEqual(m.word(slot),1)
            self.assertEqual(m.word(active),0)
            self.assertEqual(m.word(cursor),old_cursor)
            expected=((old_cursor-(origin+65536))&0xffffffff)>>6
            self.assertGreater(expected,1024)
            self.assertEqual(tuple(struct.unpack('<8I',m.uc.mem_read(desc,32))),
                             (origin,1024,1,0,origin+65536,expected,8,1))
            COUNTS['persistent_clock_capture_stop_sequences']+=1
            # Consume the actual resulting descriptor; stop before any source
            # read, without inventing contents for addresses outside the bank.
            stopped_context=m.uc.context_save()
            for linear in (False,True):
                for slide,focus in ((0.,0.),(1.,1.),(f32(.37),f32(.61))):
                    m.uc.context_restore(stopped_context)
                    m.s(0,focus);m.s(1,slide);m.r(14,0x08020001)
                    entry=(0x0802ce5c if side else 0x0802cd0c) if linear else (0x0802d0f8 if side else 0x0802cfac)
                    boundary=(0x0802cf22 if side else 0x0802cdd2) if linear else (0x0802d1da if side else 0x0802d08e)
                    m.run(entry,boundary,limit=150000);COVERAGE.update(m.last_trace)
                    pointers,fractions,grid,n=array_reader_addresses(expected,8,origin+65536,bank,slide,focus,0,linear)
                    self.assertEqual([m.r(r) for r in ((2,1) if linear else (0,1,2,12))],pointers)
                    COUNTS['capture_underflow_reader_prefixes']+=1
            # The old recording slot remains dirty and is the next save target;
            # the newly selected oversized descriptor retains its saved marker.
            m.uc.context_restore(stopped_context);m.r(0,side)
            self.run_slice(m,0x08033af8,0x08033ce4 if side else 0x08033b76)
            self.assertEqual(bytes(m.uc.mem_read(m.r(1),13)),f"spec{'b' if side else 'a'}000.wav".encode()+b'\0')
            self.assertEqual(m.word(desc+12),1)
            self.assertEqual(m.word(desc+16+12),1)
            self.assertEqual(m.word(desc+16+4),expected)
            COUNTS['capture_underflow_save_dispatches']+=1

    def test_clock_policy_timeout_boundaries(self):
        for side in range(2):
            for elapsed in (5998,5999,6000,6001):
                for capturing in (0,1):
                    for edge in (False,True):
                        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                        policy=0x20002ed4 if side else 0x20002ed8
                        timer=0x20002f10 if side else 0x20002f0c
                        m.word(policy,1);m.word(timer,elapsed)
                        m.word(0x20002e80 if side else 0x20002e84,capturing)
                        desc=0x20000920 if side else 0x20000a20
                        m.word(desc+4,65);m.word(desc+8,1)
                        m.word(0x58021810,192);m.word(0x20002f58,0x101)
                        if edge:
                            m.r(0,128 if side else 64);m.r(14,0x08020001)
                            self.run_slice(m,0x08032688,0x08020000)
                        m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                        self.assertEqual(m.word(policy),int(edge or capturing or elapsed+1<=6000))
                        self.assertEqual(m.word(timer),0 if edge else elapsed+1)
                        COUNTS['clock_timeout_cases']+=1

    def test_clock_countdown_sequences(self):
        # Bounded trajectories stay below the 50-callback gesture threshold.
        # A short release itself reloads the countdown, independently of ISR.
        for side in range(2):
            for mode in (0,1,5):
                for dimension in (65,129,1024):
                    for capturing,release in ((0,False),(1,False),(0,True),(1,True)):
                        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                        m.uc.mem_map(0x60000000,0x1000)
                        m.floats(0x60000000,[0.]+[f32((1/64)/j) for j in range(1,1024)])
                        desc=0x20000920 if side else 0x20000a20
                        m.word(desc+4,dimension);m.word(desc+8,1)
                        m.word(0x20002e80 if side else 0x20002e84,capturing)
                        m.word(0x20002ef4 if side else 0x20002ef8,mode)
                        offset_addr=0x20002434 if side else 0x20002438
                        count_addr=0x20002ecc if side else 0x20002ed0
                        elapsed_addr=0x20002f10 if side else 0x20002f0c
                        policy_addr=0x20002ed4 if side else 0x20002ed8
                        rate_addr=0x200023f4 if side else 0x200023f8
                        offset=dimension-1;count=0;elapsed=0;rate=f32(.125)
                        m.word(offset_addr,offset);m.floats(rate_addr,[rate])
                        m.word(0x58021810,192);m.word(0x20002f58,0x101)
                        for callback in range(24):
                            edge=callback==0 if release else callback in (0,4,19)
                            held=not release or callback==0
                            m.word(0x58021810,192 if held else 0)
                            if edge:
                                m.r(0,128 if side else 64);m.r(14,0x08020001)
                                self.run_slice(m,0x08032688,0x08020000)
                            admitted=edge or (release and callback==1)
                            elapsed+=1;count=max(0,count-1)
                            if admitted:
                                count=1 if capturing else (dimension+63)//64
                                if mode not in (0,5):rate=f32((1/64)/elapsed)
                                elapsed=0
                            if (admitted or (count>0 and not held)) and mode in (0,5):offset=(offset+1)%dimension
                            m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                            context=(side,mode,dimension,capturing,release,callback)
                            self.assertEqual(m.word(offset_addr),offset,context)
                            self.assertEqual(m.word(count_addr),count,context)
                            self.assertEqual(m.word(elapsed_addr),elapsed,context)
                            self.assertEqual(m.word(policy_addr),1,context)
                            self.equal_floats([rate],m.floats(rate_addr),context)
                            COUNTS['clock_sequence_callbacks']+=1

    def test_clock_to_planar_logical_bounds(self):
        from unicorn import UC_HOOK_MEM_READ
        for side in range(2):
            for length in (64,65):
                m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                bank=0x60001000 if side else 0x60c01000
                m.uc.mem_map(bank,0x10000)
                desc=0x20000920 if side else 0x20000a20
                m.word(desc+4,length);m.word(desc+8,1)
                m.word(0x20002f70 if side else 0x20002f74,bank)
                offset_addr=0x20002434 if side else 0x20002438
                m.word(0x58021810,128 if side else 64)
                m.word(0x20002f58,0x100 if side else 1)
                outside=0
                for step in range(length):
                    # Each admitted clock resets the short-hold counter; no
                    # RAM rewriting of offsets or hold state between steps.
                    m.r(0,128 if side else 64);m.r(14,0x08020001)
                    self.run_slice(m,0x08032688,0x08020000)
                    m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                    offset=(step+1)%length
                    self.assertEqual(m.word(offset_addr),offset)
                    indices,_,_,_=planar_coordinates(length,1.,1.,offset)
                    reads=[]
                    def record(uc,access,address,size,value,user):
                        if bank<=address<bank+0x10000:reads.append(address)
                    handle=m.uc.hook_add(UC_HOOK_MEM_READ,record)
                    try:
                        m.s(0,1.);m.s(1,1.);m.r(14,0x08020001)
                        self.run_slice(m,0x0802d0f8 if side else 0x0802cfac,0x08020000)
                    finally:m.uc.hook_del(handle)
                    # Actual loop order: top-left, bottom-left, top-right,
                    # bottom-right for each of the 64 spectral coefficients.
                    expected=[bank+4*(indices[j]*64+k) for k in range(64) for j in (0,2,1,3)]
                    self.assertEqual(reads,expected,(side,length,offset))
                    escaped=any(i>=length for i in indices)
                    outside+=escaped
                    COUNTS['clock_planar_reads']+=1;COUNTS['clock_planar_outside_cases']+=escaped
                self.assertEqual(outside,0 if length==64 else 16)

    def test_clock_table_and_saved_period_restore(self):
        m=SliceMachine();m.uc.mem_map(0x60000000,0x1000)
        m.floats(0x60000000,[.125]);m.r(4,1);m.s(13,1/64)
        self.run_slice(m,0x0802c536,0x0802c556)
        rates=[f32((1/64)/j) for j in range(1,1024)]
        self.equal_floats([.125]+rates,m.floats(0x60000000,count=1024))
        COUNTS['clock_table_entries']+=1023
        for stored in (0,1,2,1364,1365,1366,7999,8000):
            for addr in (0x200134a4,0x200134a8):m.word(addr,stored)
            m.s(13,1/64)
            self.run_slice(m,0x0802c556,0x0802c5a8)
            period=(3*stored)//4
            expected=f32((1/64)/period) if period else .125
            for addr in (0x200023f8,0x200023f4):self.equal_floats([expected],m.floats(addr))
            COUNTS['clock_restore_cases']+=1

    def test_opposite_shift_array_selection(self):
        for side in range(2):
            for held in (49,50,51,300):
                for released in (0,50,51,1499,1500):
                    for slot in range(16):
                        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                        selection=0x20000b20 if side else 0x20002430
                        offset=0x20002434 if side else 0x20002438
                        m.word(selection,slot);m.word(offset,7)
                        m.word(0x200134b4+side*4,slot)
                        m.word(0x58021810,128 if side else 64)
                        m.word(0x20002f58,0x101)  # opposite Shift falls on this call
                        m.word(0x20002f34,released if side else held)
                        m.word(0x20002f38,held if side else released)
                        m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                        selected=held+(0 if side else 1)>49 and 51<=released<=1499
                        expected=(slot+1)%16 if selected else slot
                        context=(side,held,released,slot)
                        self.assertEqual(m.word(selection),expected,context)
                        self.assertEqual(m.word(0x200134b4+side*4),expected,context)
                        if selected:
                            self.assertEqual(m.word(offset),0,context)
                            self.assertEqual(m.word(0x20002eb8),1,context)
                        COUNTS['array_selection_cases']+=1

    def test_factory_array_reset(self):
        expected=struct.pack('<4096f',*table('factory_spectra_64x64'))
        for side in range(2):
            m=SliceMachine();bank=0x60001000 if side else 0x60c01000
            m.uc.mem_map(bank,0x800000)
            for slot in range(16):
                desc=(0x20000920 if side else 0x20000a20)+slot*16
                offset=slot*65536+side*1048576
                for j,value in enumerate((offset,37,2,9)):m.word(desc+j*4,value)
                target=bank+offset*4
                m.uc.mem_write(target,b'\xa5'*(16384+32))
                m.r(0,slot);m.r(14,0x08020001)
                m.run(0x080333fc if side else 0x080333c4,0x08020000,100000)
                COVERAGE.update(m.last_trace)
                self.assertEqual(bytes(m.uc.mem_read(target,16384)),expected)
                self.assertEqual(bytes(m.uc.mem_read(target+16384,32)),b'\xa5'*32)
                self.assertEqual(struct.unpack('<4I',m.uc.mem_read(desc,16)),(offset,8,8,0))
                COUNTS['factory_reset_cases']+=1

    def test_long_hold_linear_toggle_and_factory_reset_dispatch(self):
        expected=struct.pack('<4096f',*table('factory_spectra_64x64'))
        for side in range(2):
            for hold in (1499,1500,1501,1600):
                for other in (0,299,300,301):
                    for linear in (0,1):
                        m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
                        bank=0x60001000 if side else 0x60c01000
                        m.uc.mem_map(bank,0x800000)
                        m.word(0x58021810,192);m.word(0x20002f58,0x101)
                        m.word(0x20002f34,other if side else hold)
                        m.word(0x20002f38,hold if side else other)
                        m.uc.mem_write(0x20013490+side*2,struct.pack('<H',linear))
                        m.word(0x20000b20 if side else 0x20002430,2)
                        desc=(0x20000920 if side else 0x20000a20)+32
                        offset=2*65536+side*1048576
                        for j,value in enumerate((offset,37,2,9)):m.word(desc+j*4,value)
                        target=bank+offset*4;m.uc.mem_write(target,b'\xa5'*(16384+32))
                        sampled_other=other+side  # A increments before B dispatch
                        action=1500<hold+1<=1600 and hold+1>sampled_other
                        reset=action and sampled_other>300
                        modeled=long_hold_step(hold,sampled_other,linear)
                        self.assertEqual(modeled.action,'reset' if reset else 'toggle' if action else 'none')
                        m.r(14,0x08020001)
                        stop=(0x0802e686 if side else 0x0802e6a6) if reset else 0x08020000
                        m.run(0x0802d900,stop,100000);COVERAGE.update(m.last_trace)
                        actual_linear=struct.unpack('<H',m.uc.mem_read(0x20013490+side*2,2))[0]
                        self.assertEqual(actual_linear,1-linear if action and not reset else linear)
                        self.assertEqual(actual_linear,modeled.linear)
                        self.assertEqual(m.word(0x20002f34+side*4),modeled.hold)
                        self.assertEqual(m.word(0x20002eb8),modeled.dirty)
                        COUNTS['long_hold_model_cases']+=1
                        if reset:
                            self.assertEqual(bytes(m.uc.mem_read(target,16384)),expected)
                            self.assertEqual(struct.unpack('<4I',m.uc.mem_read(desc,16)),(offset,8,8,0))
                            self.assertEqual(m.r(0),side)  # argument to pending Array-save call
                            COUNTS['factory_reset_gestures']+=1
                        else:
                            self.assertEqual(bytes(m.uc.mem_read(target,16384)),b'\xa5'*16384)
                            if action:
                                self.assertEqual(m.word(0x20002f34+side*4),1601)
                                self.assertEqual(m.word(0x20002eb8),1)
                        self.assertEqual(bytes(m.uc.mem_read(target+16384,32)),b'\xa5'*32)
                        COUNTS['long_hold_cases']+=1

    def test_settings_record_codec(self):
        from unicorn import arm_const as arm
        rng=random.Random(6715);m=SliceMachine()
        for case in range(256):
            if case<128:
                values=[rng.randrange(2) for _ in range(4)]+[rng.randrange(6) for _ in range(2)]+\
                       [rng.randrange(3) for _ in range(2)]+[rng.randrange(8000) for _ in range(2)]+\
                       [rng.randrange(2) for _ in range(2)]+[rng.randrange(16) for _ in range(2)]+[rng.randrange(3)]
            else:values=[rng.getrandbits(16) for _ in range(4)]+[rng.getrandbits(32) for _ in range(11)]
            config=dict(zip(SETTINGS_FIELDS,values))
            m.uc.mem_write(0x2001348c,struct.pack('<4H11I',*values))
            m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
            self.run_slice(m,0x0802d280,0x0802d2fc)
            expected=pack_settings(config)
            actual=[m.word(0x2001346c+4*j) for j in range(8)]
            self.assertEqual(actual,expected)
            COUNTS['settings_pack_cases']+=1
            # Half the decoder cases use arbitrary records independently of
            # the packer, exercising signed fields and noncanonical flags.
            words=expected if case<128 else [rng.getrandbits(32) for _ in range(8)]
            m.uc.mem_write(0x2001346c,struct.pack('<8I',*words))
            self.run_slice(m,0x08033312,0x08033362)
            restored=unpack_settings(words)
            encoded=struct.pack('<4H11I',*[restored[k]&0xffff for k in SETTINGS_FIELDS[:4]],
                                *[restored[k]&0xffffffff for k in SETTINGS_FIELDS[4:]])
            self.assertEqual(bytes(m.uc.mem_read(0x2001348c,52)),encoded)
            if case<128:self.assertEqual(restored,config)
            COUNTS['settings_unpack_cases']+=1

    def test_array_save_slot_dispatch(self):
        # Execute the real scanner/name builder, stopping before file open.
        for side in range(2):
            for first_dirty in range(17):
                m=SliceMachine(double_precision=True)
                base=0x20000920 if side else 0x20000a20
                for slot in range(16):
                    m.word(base+slot*16+12,0 if slot>=first_dirty else 99)
                m.word(0x20000b20 if side else 0x20002430,15-first_dirty%16)
                m.r(0,side)
                stop=0x08033c62 if first_dirty==16 else (0x08033ce4 if side else 0x08033b76)
                self.run_slice(m,0x08033af8,stop)
                if first_dirty<16:
                    name=f"spec{'b' if side else 'a'}{first_dirty:03d}.wav".encode()+b'\0'
                    self.assertEqual(bytes(m.uc.mem_read(m.r(1),13)),name)
                    self.assertEqual(m.r(0),0x20002f8c)
                    self.assertEqual(m.r(2),11)
                for slot in range(16):
                    expected=99 if slot<first_dirty else slot+1 if slot==first_dirty else 0
                    self.assertEqual(m.word(base+slot*16+12),expected)
                COUNTS['array_save_dispatch_cases']+=1

    def test_disabled_audio_callbacks_preserve_dsp_but_continue_capture(self):
        from unicorn import arm_const as arm
        for ready in (0,128,129):
            for half in (0,128):
                for capture_mask in range(4):
                    m=SliceMachine(double_precision=True)
                    for base,size in ((0x60000000,0x100000),(0x60c00000,0x100000),
                                      (0x080e0000,0x1000),(0x58020000,0x10000),(0x40000000,0x10000)):
                        m.uc.mem_map(base,size)
                    m.r(0,0);m.run(0x0802c4e0,0x0802c9ba,100000);COVERAGE.update(m.last_trace)
                    m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
                    m.word(0x20002eec,ready)
                    m.uc.mem_write(0x20002f52,b'\x00\x00')
                    m.uc.mem_write(0x2001348c,struct.pack('<2H',0,0))
                    m.word(0x2000242c,2);m.word(0x20002428,1)
                    outputs=(0x30001040,0x30000c40,0x30000840,0x38000000)
                    for j,base in enumerate(outputs):m.uc.mem_write(base,bytes([0x61+j])*1024)
                    sides=((0x60c01000,0x20002f74,0x20000a20,0x20002e7c,0x20002e84,0x20002b40,0x20002e9c),
                           (0x60001000,0x20002f70,0x20000920,0x200011a0,0x20002e80,0x20002d40,0x200011e0))
                    frames=[[f32((j+1)*(side+1)/256) for j in range(64)] for side in range(2)]
                    for side,(bank,ptr,desc,cursor,active,amps,peak) in enumerate(sides):
                        m.word(ptr,bank);m.word(desc,0);m.word(desc+4,8);m.word(desc+8,8)
                        m.word(cursor,64);m.word(active,(capture_mask>>side)&1)
                        m.floats(amps,frames[side]);m.floats(peak,[0.])
                        m.floats(bank+256,[-9.]*128)
                    frozen=((0x200022bc,128),(0x2000239c,48),(0x2000240c,28),
                            (0x20000d40,256),(0x20000f40,256),(0x20002a40,1024))
                    snapshots=[bytes(m.uc.mem_read(base,size)) for base,size in frozen]
                    for callback in range(2):
                        m.r(0,half);m.r(14,0x08020001)
                        m.run(0x0802e750,0x08020000,100000);COVERAGE.update(m.last_trace)
                        self.assertNotIn(0x0802e86c,m.last_trace)
                        self.assertIn(0x0802d900,m.last_trace);self.assertIn(0x0802d464,m.last_trace)
                        self.assertEqual(m.word(0x20002eec),ready)
                        self.assertEqual(m.word(0x2000242c),1-callback)
                        self.assertEqual(m.word(0x20002428),0)
                        self.assertEqual([bytes(m.uc.mem_read(base,size)) for base,size in frozen],snapshots)
                        for j,base in enumerate(outputs):
                            self.assertEqual(bytes(m.uc.mem_read(base,1024)),bytes([0x61+j])*1024)
                        for side,(bank,ptr,desc,cursor,active,amps,peak) in enumerate(sides):
                            writing=bool(capture_mask&(1<<side))
                            self.assertEqual(m.word(cursor),64+(callback+1)*64*writing)
                            for frame in range(2):
                                expected=frames[side] if writing and frame<=callback else [-9.]*64
                                self.equal_floats(expected,m.floats(bank+256+frame*256,count=64))
                            self.equal_floats([max(frames[side]) if writing else 0.],m.floats(peak))
                            COUNTS['disabled_audio_stale_captures']+=writing
                        COUNTS['disabled_audio_callbacks']+=1

    def test_initializer_audio_enable_order_busy_transport(self):
        # Execute the complete initializer. Tick RAM is explicitly advanced at
        # each original tick-reader entry; delays and drivers are never stubbed.
        # Unready DMA handles and locked transport exercise real busy returns.
        from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
        commands=[(3,0),(4,64),(5,70),(6,34),(8,None),(13,40),(30,128),
                  (2,0),(28,2),(29,2)]
        expected=[]
        for register,value in commands:
            if value is not None:expected.append(('send',bytes([0x9e,register,value])))
            expected.extend([('send',bytes([0x9e,register])),('exchange',bytes([0x9f,register]))])
        for tick_start in (0,0xfffffffa):
            for tick_step in (1,10):
                m=SliceMachine(double_precision=True)
                for base,size in ((0x60000000,0x100000),(0x080e0000,0x1000),(0x58020000,0x10000)):
                    m.uc.mem_map(base,size)
                m.uc.mem_write(0x20014c38,b'\x01')
                m.uc.mem_write(0x20002f88,b'\x11\xa7')
                m.uc.mem_write(0x20000000,b'\x01') # delay adds one tick
                m.word(0x2000204c,tick_start)
                m.uc.mem_write(0x20002f66,b'\x7f')
                m.word(0x20002eec,12345)
                events=[];requests=[];delays=[];dma=[];enable=[]
                def observe(uc,address,size,user):
                    if address==0x0802039c:m.word(0x2000204c,m.word(0x2000204c)+tick_step)
                    elif address==0x080203a8:
                        delays.append(m.r(0));events.append(('delay',m.r(0)))
                        self.assertEqual(uc.mem_read(0x20002f66,1)[0],0)
                    elif address in (0x08026ea8,0x08027138):
                        kind='send' if address==0x08026ea8 else 'exchange'
                        count=m.r(2) if kind=='send' else m.r(3)
                        requests.append((kind,bytes(uc.mem_read(m.r(1),count))))
                        self.assertEqual(uc.mem_read(0x20002f66,1)[0],0)
                    elif address in (0x08025bbc,0x08025ce4):
                        dma.append((address,m.r(0),m.r(1),m.r(2)))
                        events.append(('dma',len(dma)))
                        self.assertEqual(uc.mem_read(0x20002f66,1)[0],1)
                def observe_write(uc,access,address,size,value,user):
                    if address==0x20002f66:
                        enable.append(value);events.append(('enable',value))
                hooks=[m.uc.hook_add(UC_HOOK_CODE,observe),m.uc.hook_add(UC_HOOK_MEM_WRITE,observe_write)]
                m.r(0,0x20014bb8);m.r(14,0x08020001)
                try:m.run(0x0802c4e0,0x08020000,200000);COVERAGE.update(m.last_trace)
                finally:
                    for hook in hooks:m.uc.hook_del(hook)
                self.assertEqual(requests,expected)
                self.assertEqual(delays,[2,10]);self.assertEqual(enable,[0,1])
                self.assertEqual(events,[('enable',0),('delay',2),('delay',10),('enable',1)]+
                                 [('dma',i) for i in range(1,6)])
                self.assertEqual(dma,[(0x08025bbc,0x200149b0,0x30001040,256),
                                     (0x08025bbc,0x20014880,0x30000c40,256),
                                     (0x08025bbc,0x200147e8,0x30000840,256),
                                     (0x08025bbc,0x20014750,0x38000000,256),
                                     (0x08025ce4,0x20014918,0x30000440,256)])
                self.assertEqual(bytes(m.uc.mem_read(0x20002f7c,7)),b'\xa7'*7)
                self.assertEqual(bytes(m.uc.mem_read(0x20002f66,3)),b'\x01\x02\x02')
                self.assertEqual(m.word(0x20002eec),0)
                for base in (0x30000440,0x30000840,0x30000c40,0x30001040,0x38000000):
                    self.assertEqual(bytes(m.uc.mem_read(base,1024)),bytes(1024))
                COUNTS['initializer_enable_sequences']+=1

    def test_pending_operation_callback_prefix(self):
        from unicorn import UC_HOOK_CODE
        for initial in range(256):
            m=SliceMachine();m.uc.mem_map(0x58020000,0x10000)
            m.uc.mem_write(0x20002f65,bytes([initial]))
            m.uc.mem_write(0x20014c38,b'\x01')
            requests=[]
            def observe_call(uc,address,size,user):
                if address==0x08026ea8:
                    requests.append(('send',bytes(uc.mem_read(m.r(1),m.r(2)))))
                elif address==0x08027138:
                    requests.append(('exchange',bytes(uc.mem_read(m.r(1),m.r(3)))))
                    self.assertEqual(m.r(2),0x20002f88)
            handle=m.uc.hook_add(UC_HOOK_CODE,observe_call)
            try:self.run_slice(m,0x0802e750,0x0802e77a)
            finally:m.uc.hook_del(handle)
            expected,recover=pending_operation_step(initial)
            self.assertEqual(m.uc.mem_read(0x20002f65,1)[0],expected,initial)
            self.assertEqual(requests,[('send',b'\x9e\x0e\x00'),('send',b'\x9e\x05'),
                                       ('exchange',b'\x9f\x05')] if recover else [],initial)
            self.assertEqual(0x08032224 in m.last_trace,recover)
            self.assertIn(0x0802d900,m.last_trace) # handler always precedes recovery
            COUNTS['pending_operation_prefix_cases']+=1

    def test_mode_gesture_to_save_dispatch_busy_transport(self):
        # Real gesture followed by the mode-comparison slice before slow controls.
        # Explicit busy transport takes original early returns; no call is stubbed.
        # Stop before either file open or flash unlock, never claim media success.
        for side in range(2):
            for previous in (0,1):
                for ready in (128,129):
                    m=SliceMachine(double_precision=True)
                    m.uc.mem_map(0x58020000,0x10000)
                    m.uc.mem_write(0x20002f52+side,bytes([previous]))
                    m.word(0x20002e90 if side else 0x20002e94,previous)
                    m.word(0x20002e88 if side else 0x20002e8c,2 if previous else 0)
                    m.uc.mem_write(0x2001348c+side*2,struct.pack('<H',previous))
                    desc=0x20000920 if side else 0x20000a20
                    for slot in range(16):m.word(desc+16*slot+12,0 if slot==7 else 99)
                    m.word(0x20000b20 if side else 0x20002430,7)
                    m.word(0x58020410 if side else 0x58020c10,64 if side else 128)
                    m.r(14,0x08020001);self.run_slice(m,0x0802d900,0x08020000)
                    self.assertEqual(m.uc.mem_read(0x20002f52+side,1)[0],1-previous)
                    m.word(0x20002eec,ready);m.r(4,0x20002f65)
                    m.uc.mem_write(0x20014c38,b'\x01')  # HAL handle's busy/lock byte
                    stop=0x0802e7b2 if ready==128 else 0x0802d2fc if previous else \
                         0x08033ce4 if side else 0x08033b76
                    self.run_slice(m,0x0802e77e,stop)
                    restored=struct.unpack('<H',m.uc.mem_read(0x2001348c+side*2,2))[0]
                    self.assertEqual(restored,previous if ready==128 else 1-previous)
                    if ready==129:
                        self.assertIn(0x08026ea8,m.last_trace)
                        self.assertIn(0x08027138,m.last_trace)
                        self.assertEqual(m.uc.mem_read(0x20002f65,1)[0],1)
                        if not previous:
                            name=f"spec{'b' if side else 'a'}007.wav".encode()+b'\0'
                            self.assertEqual(bytes(m.uc.mem_read(m.r(1),13)),name)
                            self.assertEqual(m.word(desc+7*16+12),8)
                        else:
                            self.assertNotIn(0x08033af8,m.last_trace)
                            self.assertEqual(m.word(desc+7*16+12),0)
                    COUNTS['mode_save_dispatch_cases']+=1

    def test_array_save_mutation_quantization_and_readback(self):
        from unicorn import arm_const as arm
        rng=random.Random(6716)
        for side in range(2):
            for peak in (-1.,0.,.125,.3,from_bits(0x3f7fffff),1.,2.):
                m=SliceMachine(double_precision=True)
                bank=0x60001000 if side else 0x60c01000
                m.uc.mem_map(bank,0x800000)
                desc=(0x20000920 if side else 0x20000a20)+3*16
                offset=3*65536+side*1048576
                for j,v in enumerate((offset,2,1,4)):m.word(desc+j*4,v)
                frames=[]
                for frame in range(2):
                    values=[-2.,-1.,-1/32767,0.,1/32767,1.,2.]+[rng.uniform(-2,2) for _ in range(57)]
                    frames.append(list(map(f32,values)))
                target=bank+offset*4;m.floats(target,frames[0]+frames[1])
                m.uc.mem_write(target+512,b'\xa5'*32)
                m.floats(0x200011e0 if side else 0x20002e9c,[peak])
                m.r(5,desc);m.r(6,0);m.r(8 if side else 7,0x20002f8c)
                m.r(11,4);m.word(m.sp,4);m.word(0x20002f98,44)
                m.uc.reg_write(arm.UC_ARM_REG_D9,struct.unpack('<Q',struct.pack('<d',32767.))[0])
                for frame in range(2):
                    if frame:
                        m.r(7 if side else 9,64)
                        start=0x08033d32 if side else 0x08033bca
                    else:start=0x08033cf0 if side else 0x08033b82
                    self.run_slice(m,start,0x08033d60 if side else 0x08033bf8)
                    scaled,pcm=array_save_frame(frames[frame],peak)
                    self.equal_floats(scaled,m.floats(target+frame*256,count=64))
                    self.equal_floats(scaled,m.floats(0x200134c8,count=64))
                    # Separate post-seek slice: the file operation is not emulated.
                    self.run_slice(m,0x08033d64 if side else 0x08033bfc,
                                   0x08033d8a if side else 0x08033c22)
                    self.assertEqual(bytes(m.uc.mem_read(0x20003468,128)),pcm)
                    self.assertEqual(m.r(2),128)
                    self.assertEqual(m.word(desc+12),4)
                    COUNTS['array_save_frames']+=1
                    # Execute only the PCM16 conversion after an assumed successful
                    # file read. Preserve all save-loop registers across this slice.
                    context=m.uc.context_save()
                    m.r(2,128);m.word(m.sp+32,0x20018000)
                    self.run_slice(m,0x08033490,0x080334ba)
                    expected=[f32(v/32768) for v in struct.unpack('<64h',pcm)]
                    self.equal_floats(expected,m.floats(0x20018000,count=64))
                    m.uc.context_restore(context)
                    COUNTS['array_pcm16_readback_frames']+=1
                self.assertEqual(bytes(m.uc.mem_read(target+512,32)),b'\xa5'*32)

    def test_complete_array_save_and_reload_file_fixture(self):
        rng=random.Random(6732)
        for side in range(2):
            for slot in (0,7,15):
                for frame_count in (0,1,2,64):
                    for peak in (0.,.25,1.,2.):
                        m=SliceMachine(double_precision=True)
                        base=0x20000920 if side else 0x20000a20;desc=base+slot*16
                        offset=slot*65536+side*1048576
                        target=(0x60001000 if side else 0x60c01000)+offset*4
                        size=((frame_count*256+256+4095)//4096)*4096
                        m.uc.mem_map(target,size)
                        for j in range(16):m.word(base+j*16+12,1)
                        for j,value in enumerate((offset,frame_count,1,0)):m.word(desc+j*4,value)
                        m.floats(0x200011e0 if side else 0x20002e9c,[peak])
                        source=[[f32(v) for v in ([-1.,0.,1.,2.,-2.]+[rng.uniform(-.5,.5) for _ in range(59)])]
                                for _ in range(frame_count)]
                        scaled=[];payload=b''
                        for frame in source:
                            values,encoded=array_save_frame(frame,peak);scaled.extend(values);payload+=encoded
                        if source:m.floats(target,[v for row in source for v in row])
                        m.uc.mem_write(target+frame_count*256,b'\xa5'*256)
                        m.uc.mem_write(m.sp-1024,b'\xa5'*1024)
                        fixture=MemoryWriteFileFixture(m,0x20002f8c)
                        m.r(0,side)
                        fixture.run(0x08033af8,0x0802d280,max_calls=512)
                        COVERAGE.update(fixture.trace)
                        filename=f'spec{"b" if side else "a"}{slot:03d}.wav'
                        self.assertEqual(set(fixture.files),{filename})
                        header=struct.pack('<4sI4s4sIHHIIHH4sI',b'RIFF',36+len(payload),b'WAVE',
                                           b'fmt ',16,1,1,48000,192000,4,16,b'data',len(payload))
                        self.assertEqual(fixture.files[filename],header+payload,(side,slot,frame_count,peak))
                        self.assertEqual(fixture.operations[0],('open',filename,11,0))
                        self.assertEqual(fixture.operations[-1][0],'close')
                        self.assertTrue(all(op[-1]==0 for op in fixture.operations))
                        for boundary in fixture.BOUNDARIES:self.assertNotIn(boundary,fixture.trace)
                        if scaled:self.equal_floats(scaled,m.floats(target,count=len(scaled)))
                        self.assertEqual(bytes(m.uc.mem_read(target+frame_count*256,256)),b'\xa5'*256)
                        self.assertEqual([m.word(desc+4*j) for j in range(4)],[offset,frame_count,1,slot+1])
                        COUNTS['complete_array_save_files']+=1
                        # Feed these exact generated bytes through the original
                        # parser/loader, starting after open and stopping before close.
                        load=SliceMachine();load.uc.mem_map(target,size)
                        load.uc.mem_write(target,b'\xa5'*size)
                        for j,value in enumerate((offset,37,2,99)):load.word(desc+j*4,value)
                        load.word(0x20000b20 if side else 0x20002430,slot)
                        load.r(11 if side else 8,base)
                        for stack_offset,value in ((28,104),(32,108),(36,112)):
                            load.word(load.sp+stack_offset,load.sp+value)
                        reader=MemoryReadFileFixture(load,0x20002f8c,fixture.files[filename])
                        reader.run(0x08034d66 if side else 0x08034b66,
                                   0x08034db4 if side else 0x08034bb2,max_calls=512)
                        COVERAGE.update(reader.trace)
                        if frame_count>1:
                            decoded,_=array_decode_sample_buffer(payload,'pcm16')
                            self.equal_floats(decoded,load.floats(target,count=len(decoded)))
                            dims=(8,8) if frame_count==64 else (frame_count,1)
                            self.assertEqual([load.word(desc+4*j) for j in range(4)],[offset,*dims,1])
                            self.assertEqual(bytes(load.uc.mem_read(target+frame_count*256,256)),b'\xa5'*256)
                        else:
                            self.assertEqual([load.word(desc+4*j) for j in range(4)],[offset,37,2,99])
                            self.assertEqual(bytes(load.uc.mem_read(target,size)),b'\xa5'*size)
                        COUNTS['saved_array_reload_sequences']+=1

    def test_array_save_multiple_dirty_slots(self):
        for side in range(2):
            m=SliceMachine(double_precision=True);base=0x20000920 if side else 0x20000a20
            bank=0x60001000 if side else 0x60c01000;slots=(0,7,15);expected={}
            for slot in range(16):
                offset=slot*65536+side*1048576
                for j,value in enumerate((offset,2,1,0 if slot in slots else 99)):
                    m.word(base+slot*16+j*4,value)
                if slot in slots:
                    destination=bank+offset*4;m.uc.mem_map(destination,0x1000)
                    values=[f32((slot*128+i+1)/16384) for i in range(128)]
                    m.floats(destination,values)
                    payload=b''.join(array_save_frame(values[j:j+64],.25)[1] for j in (0,64))
                    expected[f'spec{"b" if side else "a"}{slot:03d}.wav']=payload
            m.floats(0x200011e0 if side else 0x20002e9c,[.25])
            fixture=MemoryWriteFileFixture(m,0x20002f8c);m.r(0,side)
            fixture.run(0x08033af8,0x0802d280,max_calls=256);COVERAGE.update(fixture.trace)
            self.assertEqual(list(fixture.files),list(expected))
            for name,payload in expected.items():
                self.assertEqual(fixture.files[name][44:],payload)
                self.assertEqual(len(fixture.files[name]),300)
            for slot in range(16):
                self.assertEqual(m.word(base+slot*16+12),slot+1 if slot in slots else 99)
            COUNTS['multi_slot_array_save_batches']+=1

    def test_array_save_payload_write_failures_and_short_counts(self):
        def save(side,failures=None,short_writes=None):
            m=SliceMachine(double_precision=True);base=0x20000920 if side else 0x20000a20
            bank=0x60001000 if side else 0x60c01000
            m.uc.mem_map(bank,0x1000)
            for j in range(16):m.word(base+16*j+12,1)
            for j,value in enumerate((0,3,1,0)):m.word(base+4*j,value)
            source=[[f32((frame*64+j+1)/1024) for j in range(64)] for frame in range(3)]
            m.floats(bank,[v for row in source for v in row])
            m.floats(0x200011e0 if side else 0x20002e9c,[.5])
            fixture=MemoryWriteFileFixture(m,0x20002f8c,failures,short_writes)
            m.r(0,side);fixture.run(0x08033af8,0x0802d280)
            COVERAGE.update(fixture.trace)
            encoded=[];scaled=[]
            for frame in source:
                values,raw=array_save_frame(frame,.5);scaled.extend(values);encoded.append(raw)
            self.equal_floats(scaled,m.floats(bank,count=192))
            self.assertEqual(m.word(base+12),1) # no retry marker after a failed write
            self.assertEqual(fixture.operations[-1][0],'close')
            return next(iter(fixture.files.values())),fixture,encoded
        for side in range(2):
            _,baseline,encoded=save(side)
            payload_ops=[i for i,op in enumerate(baseline.operations)
                         if op[0]=='write' and op[2]==128]
            self.assertEqual(len(payload_ops),3)
            for frame,operation in enumerate(payload_ops):
                for status in (1,9):
                    data,fixture,_=save(side,{operation:status})
                    payload=b''.join(raw for i,raw in enumerate(encoded) if i!=frame)
                    self.assertEqual(len(data),44+len(payload))
                    self.assertEqual(struct.unpack_from('<I',data,4)[0],36+len(payload))
                    self.assertEqual(struct.unpack_from('<I',data,40)[0],len(payload))
                    self.assertEqual(data[44:],payload)
                    self.assertEqual(fixture.operations[operation][-1],status)
                    COUNTS['array_save_failed_payload_writes']+=1
                for count in (0,1,64,127):
                    data,fixture,_=save(side,short_writes={operation:count})
                    payload=b''.join(raw if i!=frame else raw[:count]+bytes(128-count)
                                     for i,raw in enumerate(encoded))
                    self.assertEqual(len(data),428)
                    self.assertEqual(struct.unpack_from('<I',data,40)[0],384)
                    self.assertEqual(data[44:],payload)
                    # The finalizer writes RIFF length before its last seek grows
                    # the file to the advertised payload end in this fixture.
                    expected_riff=292+count if frame==2 else 420
                    self.assertEqual(struct.unpack_from('<I',data,4)[0],expected_riff)
                    self.assertEqual(fixture.operations[operation][-2:],(count,0))
                    COUNTS['array_save_short_payload_writes']+=1

    def test_array_sample_formats_and_alignment(self):
        formats=(('pcm8',0x75733038,1),('pcm16',0x73693136,2),
                 ('pcm24',0x73693234,3),('pcm32',0x73693332,4),('float32',0x666c3332,4))
        rng=random.Random(6717)
        for name,code,width in formats:
            for count in (0,1,2,3,4,5,7,8,9,63,64,65):
                if name=='float32':
                    values=[-0.,-1.,1.,2.,1e-25]+[rng.uniform(-2,2) for _ in range(65)]
                    payload=struct.pack(f'<{count}f',*values[:count])
                elif name=='pcm8':
                    payload=bytes(([0,127,128,129,255]+[rng.randrange(256) for _ in range(65)])[:count])
                else:
                    low=-(1<<(width*8-1));high=-low-1
                    values=[low,low+1,-1,0,1,high-1,high]+[rng.randint(low,high) for _ in range(65)]
                    payload=b''.join(v.to_bytes(width,'little',signed=True) for v in values[:count])
                expected,reported=array_decode_sample_buffer(payload,name)
                for alignment in range(4):
                    m=SliceMachine();destination=0x20018000
                    m.uc.mem_write(0x20003468,b'\x5a'*alignment+payload+b'\xa5'*16)
                    m.uc.mem_write(destination,b'\x5a'*(len(expected)*4+16))
                    m.r(2,len(payload));m.r(6,alignment);m.r(7,code);m.word(m.sp+32,destination)
                    stop=0x0803353e if count==0 or name in ('pcm8','pcm24') else 0x080334ba
                    self.run_slice(m,0x08033466,stop)
                    self.equal_floats(expected,m.floats(destination,count=len(expected)),(name,count,alignment))
                    self.assertEqual(m.r(5),reported,(name,count,alignment,'reported count'))
                    self.assertEqual(bytes(m.uc.mem_read(destination+count*4,16)),b'\x5a'*16)
                    COUNTS['array_sample_decode_cases']+=1

    def test_wav_parser_signatures(self):
        # Entry immediately after the 12-byte read: explicitly supplied return
        # status and scratch data. No file call is intercepted or replaced.
        for status in (0,1,9):
            for riff in (b'RIFF',b'RIFX',b'RF64',b'xxxx'):
                for wave in (b'WAVE',b'xxxx'):
                    for count in (0,4,12):
                        m=SliceMachine();sp=m.sp;stop=0x08020000
                        m.word(sp+356,stop|1);m.r(0,status)
                        m.uc.mem_write(sp+28,riff+struct.pack('<I',0)+wave)
                        m.word(sp+16,count)
                        valid=status==0 and riff==b'RIFF' and wave==b'WAVE'
                        self.run_slice(m,0x080330ac,0x080330e4 if valid else stop)
                        if valid:
                            self.assertEqual((m.r(4),m.r(6),m.r(10),m.r(11)),(12,0,0,0))
                        else:
                            self.assertEqual(m.r(0),status)
                        COUNTS['wav_parser_signature_cases']+=1

    def parse_wav_file_fixture(self,data,failures=None):
        m=SliceMachine();file_object=0x20018000;out=0x20018400;stop=0x08020000
        m.uc.mem_write(m.sp-1024,b'\xa5'*1024) # explicit previous stack contents
        for i in range(7):m.word(out+4*i,0xa5a5a5a5)
        m.r(0,file_object);m.r(1,out);m.r(2,out+4);m.r(3,out+8);m.r(14,stop|1)
        for i in range(4):m.word(m.sp+4*i,out+12+4*i)
        fixture=MemoryReadFileFixture(m,file_object,data,failures)
        result=fixture.run(0x08033ab0,stop)
        COVERAGE.update(fixture.trace)
        self.assertNotIn(fixture.READ,fixture.trace);self.assertNotIn(fixture.SEEK,fixture.trace)
        return result,[m.word(out+4*i) for i in range(7)],fixture

    def test_complete_wav_parser_with_explicit_file_fixture(self):
        def chunk(kind,payload):
            return kind+struct.pack('<I',len(payload))+payload+(b'\0' if len(payload)&1 else b'')
        formats=((1,8,0x75733038),(1,16,0x73693136),(1,24,0x73693234),
                 (1,32,0x73693332),(3,32,0x666c3332))
        for tag,depth,code in formats:
            for channels in (1,2,3):
                for fmt_size in (16,18,40):
                    fmt=chunk(b'fmt ',struct.pack('<HHIIHH',tag,channels,48000,192000,4,depth)+bytes(fmt_size-16))
                    data=chunk(b'data',bytes(128*channels*depth//8))
                    for chunks in ((fmt,data),(data,fmt),(chunk(b'JUNK',b'abc'),chunk(b'clm ',b'hello'),fmt,data)):
                        payload_start=12+sum(len(c) for c in chunks[:chunks.index(data)])+8
                        # Deliberately unsupported trailing fmt must not be read
                        # after both required chunks have been found.
                        body=b''.join(chunks)+chunk(b'fmt ',struct.pack('<HHIIHH',0xffff,1,123,0,0,16))
                        for declared in (0,len(body)+4,0xffffffff):
                            file=b'RIFF'+struct.pack('<I',declared)+b'WAVE'+body
                            result,fields,fixture=self.parse_wav_file_fixture(file)
                            self.assertEqual(result,0)
                            self.assertEqual(fields,[48000,depth,channels,code,len(data)-8,payload_start,
                                                     payload_start+len(data)-8])
                            self.assertEqual(fixture.operations[0],('seek',0,0,0))
                            self.assertTrue(all(op[-1]==0 for op in fixture.operations))
                            COUNTS['complete_wav_parser_files']+=1
        # Exercise every read/seek failure in a file with ancillary chunks.
        fmt=chunk(b'fmt ',struct.pack('<HHIIHH',1,1,48000,96000,2,16))
        data=chunk(b'data',bytes(256))
        body=chunk(b'JUNK',b'abc')+chunk(b'clm ',b'hello')+fmt+data
        file=b'RIFF'+struct.pack('<I',len(body)+4)+b'WAVE'+body
        result,fields,fixture=self.parse_wav_file_fixture(file)
        for operation in range(len(fixture.operations)):
            for status in (1,9):
                result,fields,failed=self.parse_wav_file_fixture(file,{operation:status})
                self.assertEqual(result,status,(operation,status))
                self.assertEqual(len(failed.operations),operation+1)
                self.assertEqual(failed.operations[-1][-1],status)
                COUNTS['complete_wav_parser_io_errors']+=1

    def test_complete_wav_parser_duplicates_missing_and_truncated_chunks(self):
        sentinel=0xa5a5a5a5
        def chunk(kind,payload,pad=True,declared=None):
            return kind+struct.pack('<I',len(payload) if declared is None else declared)+payload+\
                   (b'\0' if pad and len(payload)&1 else b'')
        def riff(body):return b'RIFF'+struct.pack('<I',len(body)+4)+b'WAVE'+body
        fmt16=chunk(b'fmt ',struct.pack('<HHIIHH',1,1,48000,96000,2,16))
        fmt8=chunk(b'fmt ',struct.pack('<HHIIHH',1,2,22050,44100,2,8))
        a=chunk(b'data',bytes(6));b=chunk(b'data',bytes(10))
        fmt17=struct.pack('<HHIIHH',1,1,48000,96000,2,16)+b'\0'
        cases=[
            (b'', [sentinel]*7),
            (b'not a WAV at all', [sentinel]*7),
            (riff(b''), [sentinel]*7),
            (riff(fmt16), [48000,16,1,0x73693136,sentinel,sentinel,sentinel]),
            (riff(a), [sentinel]*4+[6,20,26]),
            (riff(fmt8+fmt16+a), [48000,16,1,0x73693136,6,68,74]),
            (riff(fmt16+fmt8+a), [22050,8,2,0x75733038,6,68,74]),
            (riff(a+b+fmt16), [48000,16,1,0x73693136,10,34,44]),
            (riff(fmt16+a+b), [48000,16,1,0x73693136,6,44,50]),
            (riff(a+fmt16+fmt8), [48000,16,1,0x73693136,6,20,26]),
            # Standard even padding after an odd fmt chunk is not skipped.
            (riff(chunk(b'fmt ',fmt17)+a), [48000,16,1,0x73693136,sentinel,sentinel,sentinel]),
            (riff(chunk(b'fmt ',fmt17,pad=False)+a), [48000,16,1,0x73693136,6,45,51]),
            # No sample payload is read by the parser; advertised data length wins.
            (riff(fmt16+chunk(b'data',b'abc',declared=256)), [48000,16,1,0x73693136,256,44,300]),
            # A short data header retains old stack bytes for the missing size.
            (riff(fmt16+b'data'), [48000,16,1,0x73693136,16,44,60]),
        ]
        for file,expected in cases:
            result,fields,fixture=self.parse_wav_file_fixture(file)
            self.assertEqual(result,0,file[:48])
            self.assertEqual(fields,expected,file[:48])
            COUNTS['complete_wav_parser_edge_files']+=1

    def test_wav_parser_format_classification(self):
        tags=(0,1,2,3,4,6,7,16,17,18,257,258,259,260,0xfffe,0xffff)
        for tag in tags:
            for depth in (0,8,12,16,24,32,64,0xffff):
                for channels in (1,2,0xffff):
                    m=SliceMachine();sp=m.sp;out=0x20018000;stop=0x08020000
                    old_code=0xa5a5a5a5;old_depth=0x5a5a5a5a
                    m.word(sp+356,stop|1);m.word(sp,out);m.word(sp+4,out+4)
                    m.word(sp+360,out+12);m.r(7,out+8)
                    m.word(out+4,old_depth);m.word(out+12,old_code)
                    m.uc.mem_write(sp+48,struct.pack('<HHI',tag,channels,12345))
                    expected=wav_format_classification(tag,depth,old_code)
                    boundary=0x08033244 if tag==1 else (0x080331d6 if expected else stop)
                    self.run_slice(m,0x0803317c,boundary)
                    self.assertEqual(m.word(out),12345)
                    self.assertEqual(m.word(out+8),channels if channels<0x8000 else 0xffffffff)
                    if tag==1:
                        # Separate post-read entry for the two-byte PCM depth.
                        m.uc.mem_write(sp+14,struct.pack('<H',depth))
                        self.run_slice(m,0x08033260,0x080331d6)
                    if expected:
                        self.assertEqual((m.word(out+4),m.word(out+12)),
                                         (expected[0]&0xffffffff,expected[1]),(tag,depth,channels))
                    else:
                        self.assertEqual(m.r(0),0xffffffff)
                        self.assertEqual((m.word(out+4),m.word(out+12)),(old_depth,old_code))
                    COUNTS['wav_parser_format_cases']+=1

    def test_wav_parser_chunk_progress(self):
        # Chunk dispatch and post-read advancement, excluding read/seek calls.
        for kind in (b'JUNK',b'data',b'fmt ',b'clm '):
            for position in (12,13,0xfffffff0):
                for size in (0,1,2,15,16,17,0xffffffff):
                    m=SliceMachine();sp=m.sp;out=0x20018000
                    m.r(4,position);m.r(8,0x20746d66);m.r(9,0x61746164)
                    m.r(10,0);m.r(6,0)
                    m.uc.mem_write(sp+20,kind+struct.pack('<I',size))
                    for offset,pointer in ((364,out),(368,out+4),(372,out+8)):
                        m.word(sp+offset,pointer)
                    if kind==b'fmt ':
                        self.run_slice(m,0x08033104,0x0803315c)
                        m.word(sp+44,size)
                        self.run_slice(m,0x080331d6,0x08033120)
                    elif kind==b'clm ':
                        self.run_slice(m,0x08033104,0x08033204)
                        self.assertEqual(m.r(2),size) # payload read length, not bounded here
                        self.assertEqual(m.r(1),sp+64)
                        m.r(0,0)
                        self.run_slice(m,0x08033208,0x08033120)
                    else:
                        self.run_slice(m,0x08033104,0x08033120)
                    end=(position+8+size)&0xffffffff
                    next_position=end if kind==b'fmt ' else (end+(end&1))&0xffffffff
                    self.assertEqual(m.r(4),next_position,(kind,position,size))
                    if kind==b'data':
                        self.assertEqual([m.word(out+i*4) for i in range(3)],
                                         [size,(position+8)&0xffffffff,end])
                        self.assertEqual(m.r(10),1)
                    if kind==b'fmt ':self.assertEqual(m.r(6),1)
                    COUNTS['wav_parser_chunk_cases']+=1

    def test_wav_parser_loop_termination(self):
        for position in (12,13,100):
            for file_size in (position-1,position,position+1):
                for fmt_found in (0,1):
                    for data_found in (0,1):
                        m=SliceMachine();stop=0x08020000;file_object=0x20018000
                        m.word(m.sp+356,stop|1);m.r(4,position);m.r(5,file_object)
                        m.word(file_object+12,file_size)
                        m.r(6,fmt_found);m.r(10,data_found);m.r(11,0)
                        again=file_size>position and not (fmt_found and data_found)
                        self.run_slice(m,0x08033120,0x080330e4 if again else stop)
                        if not again:self.assertEqual(m.r(0),0)
                        COUNTS['wav_parser_termination_cases']+=1

    def test_array_header_format_fields(self):
        for code,tag,depth in ((0x75733038,1,8),(0x73693136,1,16),(0x73693234,1,24),
                               (0x73693332,1,32),(0x666c3332,3,32)):
            for chunk_size in (16,18,40):
                m=SliceMachine();m.r(4,0x20002f8c);m.r(5,code);m.r(7,12);m.r(8,0x20746d66)
                m.uc.mem_write(m.sp,b'\xa5'*80);m.word(m.sp+24,chunk_size)
                self.run_slice(m,0x080338b4,0x0803390e)
                expected=struct.pack('<4sIHHIIH',b'fmt ',chunk_size,tag,1,48000,192000,4)
                self.assertEqual(bytes(m.uc.mem_read(m.sp+40,22)),expected)
                self.assertEqual(bytes(m.uc.mem_read(m.sp+10,2)),struct.pack('<H',depth))
                self.assertEqual(m.r(11),1)
                # Separate slices after an explicitly assumed successful seek.
                m.r(0,0);self.run_slice(m,0x08033912,0x08033920)
                self.assertEqual((m.r(1),m.r(2)),(m.sp+40,22))
                self.run_slice(m,0x080339e6,0x080339ec)
                self.assertEqual(m.r(1),34)
                self.run_slice(m,0x080339f0,0x080339fa)
                self.assertEqual((m.r(1),m.r(2)),(m.sp+10,2))
                COUNTS['array_header_format_cases']+=1

    def test_array_load_count_and_initial_seek(self):
        for start,end in ((0x08034b8a,0x08034ba8),(0x08034d8a,0x08034daa),
                          (0x08034e98,0x08034eba)):
            for depth in (8,16,24,32):
                for channels in (1,2,3):
                    for size in (0,127,128,129,8192,8193):
                        for offset in range(44,48):
                            m=SliceMachine();m.r(0,0)
                            for index,value in ((88,12345),(92,depth),(96,channels),
                                                (100,0x73693136),(104,size),(108,offset)):
                                m.word(m.sp+index,value)
                            self.run_slice(m,start,end)
                            self.assertEqual(m.r(5),array_load_count(size,depth,channels))
                            self.assertEqual((m.r(0),m.r(1)),(0x20002f8c,offset&~3))
                            COUNTS['array_load_count_cases']+=1

    def test_array_read_request_and_short_read_window(self):
        from unicorn import arm_const as arm
        formats=(('pcm8',8,0x75733038),('pcm16',16,0x73693136),
                 ('pcm24',24,0x73693234),('pcm32',32,0x73693332),
                 ('float32',32,0x666c3332))
        for name,depth,code in formats:
            request=64*depth//8+4
            for alignment in range(4):
                for returned,status in [(n,0) for n in (0,1,3,request-1,request)]+[(request,1),(request,9)]:
                    m=SliceMachine();dest=0x20018000;stop=0x08020000
                    m.word(m.sp,dest);m.word(m.sp+4,64)
                    m.r(0,0x20002f8c);m.r(1,44+alignment);m.r(2,code);m.r(3,depth)
                    m.r(14,stop|1)
                    self.run_slice(m,0x08033434,0x08033452)
                    self.assertEqual((m.r(1),m.r(2)),(0x20003468,request))
                    # Bytes outside the supplied read count are explicit old
                    # scratch, with finite float bit patterns at every alignment.
                    scratch=bytes(17+(i%37) for i in range(request+8))
                    m.uc.mem_write(0x20003468,scratch)
                    m.uc.mem_write(dest,b'\xa5'*320)
                    actual_sp=m.uc.reg_read(arm.UC_ARM_REG_SP)
                    m.word(actual_sp+4,returned);m.r(0,status)
                    used=returned-4 if returned==request else returned
                    expected,count=array_decode_sample_buffer(scratch[alignment:alignment+used],name)
                    if status:expected,count=[],0
                    self.run_slice(m,0x08033456,stop)
                    self.equal_floats(expected,m.floats(dest,count=len(expected)),(name,alignment,returned))
                    self.assertEqual(m.r(0),count)
                    self.assertEqual(bytes(m.uc.mem_read(dest+4*len(expected),16)),b'\xa5'*16)
                    COUNTS['array_read_window_cases']+=1

    def test_complete_parser_through_array_load_with_file_fixture(self):
        formats=(('pcm8',1,8),('pcm16',1,16),('pcm24',1,24),('pcm32',1,32),('float32',3,32))
        for name,tag,depth in formats:
            for channels in (1,2,3):
                count=128*channels
                if name=='pcm8':payload=bytes((i*7+19)&255 for i in range(count))
                elif name=='pcm16':payload=struct.pack(f'<{count}h',*[i*29-12000 for i in range(count)])
                elif name=='pcm24':payload=b''.join((i*3001-400000).to_bytes(3,'little',signed=True) for i in range(count))
                elif name=='pcm32':payload=struct.pack(f'<{count}i',*[i*90001-30000000 for i in range(count)])
                else:payload=struct.pack(f'<{count}f',*[f32((i-100)/512) for i in range(count)])
                expected,_=array_decode_sample_buffer(payload[:128*depth//8],name)
                for fmt_size in (16,18):
                    fmt=b'fmt '+struct.pack('<IHHIIHH',fmt_size,tag,channels,48000,192000,4,depth)+bytes(fmt_size-16)
                    body=fmt+b'data'+struct.pack('<I',len(payload))+payload
                    file=b'RIFF'+struct.pack('<I',len(body)+4)+b'WAVE'+body
                    for side in range(2):
                        for slot in (0,15):
                            m=SliceMachine();desc=(0x20000920 if side else 0x20000a20)+slot*16
                            offset=slot*65536+side*1048576
                            bank=0x60001000 if side else 0x60c01000;destination=bank+offset*4
                            m.uc.mem_map(destination,0x2000)
                            m.uc.mem_write(destination,b'\xa5'*768)
                            m.word(desc,offset);m.word(desc+4,37);m.word(desc+8,2);m.word(desc+12,99)
                            m.word(0x20000b20 if side else 0x20002430,slot)
                            m.r(11 if side else 8,0x20000920 if side else 0x20000a20)
                            for stack_offset,target in ((28,104),(32,108),(36,112)):
                                m.word(m.sp+stack_offset,m.sp+target)
                            fixture=MemoryReadFileFixture(m,0x20002f8c,file)
                            fixture.run(0x08034d66 if side else 0x08034b66,
                                        0x08034db4 if side else 0x08034bb2)
                            COVERAGE.update(fixture.trace)
                            self.assertIn(0x08033094,fixture.trace)
                            self.assertIn(0x08033434,fixture.trace)
                            self.assertNotIn(fixture.READ,fixture.trace);self.assertNotIn(fixture.SEEK,fixture.trace)
                            self.equal_floats(expected,m.floats(destination,count=128),(name,channels,fmt_size,side,slot))
                            self.assertEqual(bytes(m.uc.mem_read(destination+512,256)),b'\xa5'*256)
                            self.assertEqual([m.word(desc+4*j) for j in range(4)],[offset,2,1,1])
                            self.assertEqual([m.word(m.sp+88+4*j) for j in range(3)],[48000,depth,channels])
                            self.assertEqual(m.word(m.sp+104),len(payload))
                            self.assertEqual(m.word(m.sp+108),28+fmt_size)
                            COUNTS['parsed_array_load_sequences']+=1

    def test_stereo_loader_scalar_sequence(self):
        # Joined A loader arithmetic, descriptor setup, reader and copies.
        # Every seek/read is an explicit boundary, with no file-call stubs.
        from unicorn import arm_const as arm
        for channels in (1,2,3):
            m=SliceMachine();base=0x60c01000;m.uc.mem_map(base,0x2000)
            scalar=[i*29-12000 for i in range(128*channels)]
            payload=struct.pack(f'<{len(scalar)}h',*scalar)
            file_bytes=b'\0'*44+payload+b'\0'*8
            m.r(8,0x20000a20);m.word(0x20002430,0)
            for offset,value in ((92,16),(96,channels),(100,0x73693136),
                                 (104,len(payload)),(108,44)):
                m.word(m.sp+offset,value)
            self.run_slice(m,0x08034b8a,0x08034ba8)
            self.assertEqual(m.r(5),128)
            m.r(0,0);m.r(4,0)
            self.run_slice(m,0x08034bac,0x08035018)
            for frame in range(2):
                self.assertEqual(m.r(1),44+frame*128)
                m.r(0,0)
                self.run_slice(m,0x0803501c,0x08033452)
                self.assertEqual(m.r(2),132)
                data=file_bytes[44+frame*128:44+frame*128+132]
                m.uc.mem_write(0x20003468,data)
                active_sp=m.uc.reg_read(arm.UC_ARM_REG_SP)
                m.word(active_sp+4,len(data));m.r(0,0)
                self.run_slice(m,0x08033456,0x08035046)
                self.assertEqual(m.r(4),(frame+1)*64)
                if frame==0:self.run_slice(m,0x08035046,0x08035018)
            self.run_slice(m,0x08035046,0x08034bb2)
            self.equal_floats([f32(x/32768) for x in scalar[:128]],m.floats(base,count=128))
            self.assertEqual([m.word(0x20000a20+i*4) for i in range(4)], [0,2,1,1])
            COUNTS['array_stereo_load_sequences']+=1

    def test_array_load_dimension_assignment(self):
        for start,rejected,admitted in ((0x08034bac,0x08034bb2,0x08034fd6),
                                        (0x08034dae,0x08034db4,0x0803504c),
                                        (0x08034ebe,0x080348b4,0x08034ec6)):
            for count in (-1,0,1,63,64,65,65536,65537):
                m=SliceMachine();m.r(5,count&0xffffffff)
                self.run_slice(m,start,admitted if count>64 else rejected)
                COUNTS['array_load_admission_cases']+=1
        for side in range(2):
            for count in (65,127,128,4095,4096,4097,65536,65537):
                for slot in (0,7,15):
                    m=SliceMachine();base=0x20000920 if side else 0x20000a20
                    desc=base+slot*16;offset=slot*65536+side*1048576
                    for j,v in enumerate((offset,37,2,99)):m.word(desc+j*4,v)
                    m.word(0x20000b20 if side else 0x20002430,slot)
                    m.r(5,count);m.r(11 if side else 8,base)
                    self.run_slice(m,0x0803504c if side else 0x08034fd6,
                                   0x08035076 if side else 0x08035004)
                    dims=(8,8) if count==4096 else (count>>6,1)
                    self.assertEqual(struct.unpack('<4I',m.uc.mem_read(desc,16)),(offset,*dims,1))
                    pointer=m.r(6) if side else m.word(m.sp+16)
                    self.assertEqual(pointer,(0x60001000 if side else 0x60c01000)+4*offset)
                    COUNTS['array_load_dimension_cases']+=1
        # The shared load path assigns both selected descriptors together.
        for count in (65,127,128,4095,4096,4097,65536,65537):
            for slot in (0,7,15):
                m=SliceMachine();m.r(5,count);m.r(8,0x20000a20);m.r(11,0x20000920)
                desc_a=0x20000a20+slot*16;desc_b=0x20000920+(15-slot)*16
                offset_a=slot*65536;offset_b=(15-slot)*65536+1048576
                m.word(0x20002430,slot);m.word(0x20000b20,15-slot)
                m.word(desc_a,offset_a);m.word(desc_b,offset_b)
                self.run_slice(m,0x08034ec6,0x08034f06)
                dims=(8,8) if count==4096 else (count>>6,1)
                for desc,offset in ((desc_a,offset_a),(desc_b,offset_b)):
                    self.assertEqual(struct.unpack('<4I',m.uc.mem_read(desc,16)),(offset,*dims,1))
                self.assertEqual(m.word(0x200134c4),offset_a)
                self.assertEqual(m.word(0x200134c0),offset_b)
                COUNTS['array_load_dimension_cases']+=1

    def test_array_load_fixed_frame_copy(self):
        scratch=bytes((i*73+19)&255 for i in range(256))
        for path in ('A','B','both'):
            for returned in (0,1,3,4,63,64):
                m=SliceMachine()
                a=0x60c01000;b=0x60001000
                m.uc.mem_map(a,0x1000);m.uc.mem_map(b,0x1000)
                m.uc.mem_write(a,b'\xa5'*512);m.uc.mem_write(b,b'\xa5'*512)
                m.uc.mem_write(0x200134c8,scratch);m.r(0,returned);m.r(4,128)
                if path=='A':
                    m.word(m.sp+16,a)
                    self.run_slice(m,0x08035030,0x08035046)
                    self.assertEqual(m.word(m.sp+16),a+256)
                elif path=='B':
                    m.r(6,b);self.run_slice(m,0x080350a2,0x080350b4)
                    self.assertEqual(m.r(6),b+256)
                else:
                    m.word(0x200134c0,0);m.word(0x200134c4,0)
                    self.run_slice(m,0x08034f32,0x08034f70)
                    self.assertEqual(m.word(0x200134c0),64)
                    self.assertEqual(m.word(0x200134c4),64)
                self.assertEqual(m.r(4),128+returned)
                for side,base in (('A',a),('B',b)):
                    expected=scratch if path in (side,'both') else b'\xa5'*256
                    self.assertEqual(bytes(m.uc.mem_read(base,256)),expected)
                    self.assertEqual(bytes(m.uc.mem_read(base+256,256)),b'\xa5'*256)
                COUNTS['array_load_copy_cases']+=1

    def test_settings_flash_record_scan(self):
        from unicorn import UC_HOOK_MEM_WRITE
        for records in (0,1,2,17,2047,2048):
            m=SliceMachine();m.uc.mem_map(0x081e0000,0x20000)
            data=bytearray(b'\xff'*0x20000)
            for record in range(records):
                # Distinct neighboring records reveal the copy's real length.
                words=[0x10001,record,record+1,500,600,record%16,(record+1)%16,record%3]
                data[record*32:record*32+32]=struct.pack('<8I',*words)
            m.uc.mem_write(0x081e0000,bytes(data))
            sentinel=[0x10001,2,3,500,600,7,8,2]
            m.uc.mem_write(0x2001346c,struct.pack('<8I',*sentinel))
            writes=[]
            def record_write(uc,access,address,size,value,user):
                if 0x2001346c<=address<0x2001356c:writes.append((address,size))
            handle=m.uc.hook_add(UC_HOOK_MEM_WRITE,record_write)
            try:
                # Stop before decode or the full-region maintenance branch.
                m.r(14,0x08020001)
                m.run(0x080332e4,0x08033364 if records==2048 else 0x08033312,4000000)
                COVERAGE.update(m.last_trace)
            finally:m.uc.hook_del(handle)
            self.assertEqual(m.word(0x20002ebc),0x081e0000+records*32)
            if records:
                self.assertEqual(bytes(m.uc.mem_read(0x2001346c,256)),bytes(data[(records-1)*32:(records-1)*32+256]))
                self.assertEqual(sum(size for _,size in writes),records*256)
            else:
                self.assertEqual(bytes(m.uc.mem_read(0x2001346c,32)),struct.pack('<8I',*sentinel))
                self.assertEqual(writes,[])
            if records<2048:
                expected=unpack_settings(struct.unpack('<8I',m.uc.mem_read(0x2001346c,32)))
                self.run_slice(m,0x08033312,0x08020000)
                actual=struct.unpack('<4H11I',m.uc.mem_read(0x2001348c,52))
                self.assertEqual(actual,tuple(expected[k]& (0xffff if j<4 else 0xffffffff)
                                               for j,k in enumerate(SETTINGS_FIELDS)))
            COUNTS['settings_scan_cases']+=1

    def test_settings_partial_defaults_and_save_tail(self):
        from unicorn import arm_const as arm
        m=SliceMachine()
        for a_linear,b_linear in ((0,0),(1,1),(0,1),(0xffff,0x8000)):
            m.uc.mem_write(0x2001348c,b'\x5a'*52)
            m.uc.mem_write(0x20013490,struct.pack('<2H',a_linear,b_linear))
            self.run_slice(m,0x08033372,0x08033396)
            expected=(1,1,a_linear,b_linear,0,0,0,0,500,500,0,0,0,0,0)
            self.assertEqual(struct.unpack('<4H11I',m.uc.mem_read(0x2001348c,52)),expected)
            COUNTS['settings_default_cases']+=1
        # The post-program-call tail does not inspect the returned status.
        # This does not emulate programming, erase, completion or failure.
        for cursor in (0x081e0000,0x081effc0,0x081effe0):
            for status in (0,1,2,3):
                m.word(0x20002ebc,cursor);m.word(0x20002eb8,1)
                m.r(0,status);m.r(4,0x20002ebc)
                m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
                self.run_slice(m,0x0802d30a,0x0802d31a)
                self.assertEqual(m.word(0x20002ebc),cursor+32)
                self.assertEqual(m.word(0x20002eb8),0)
                COUNTS['settings_save_tail_cases']+=1

    def test_default_calibration_and_pitch_regions(self):
        m=SliceMachine(double_precision=True)
        defaults=default_calibration();m.r(6,0)
        self.run_slice(m,0x0802c684,0x0802c8c2)
        self.assertEqual([m.word(0x20001380+4*j) for j in range(12)],defaults['offsets'])
        self.equal_floats(defaults['gains'],m.floats(0x20001200,count=12))
        for addr in (0x20001300,0x200012c0):
            self.assertEqual([m.word(addr+4*j) for j in range(8)],defaults['pitch_breakpoints'])
        for addr in (0x20001280,0x20001240):self.equal_floats(defaults['pitch_slopes'],m.floats(addr,count=8))
        COUNTS['calibration_default_cases']+=1
        rng=random.Random(6710)
        for side in ('A','B'):
            for custom in (False,True):
                bp=([40,7900,16000,24600,32900,41000,49100,57500] if custom else defaults['pitch_breakpoints'])
                slopes=[f32(.17+j*.031) for j in range(8)] if custom else defaults['pitch_slopes']
                offset=1200 if custom else 2000
                bp_addr=0x20001300 if side=='A' else 0x200012c0
                slope_addr=0x20001280 if side=='A' else 0x20001240
                for j,value in enumerate(bp):m.word(bp_addr+j*4,value)
                m.floats(slope_addr,slopes)
                m.word(0x200013a0 if side=='A' else 0x200013a8,offset)
                codes=[0,offset,65535]+[x+offset+d for x in bp for d in (-1,0,1)]
                codes += [rng.randrange(65536) for _ in range(100)]
                for raw in codes:
                    if side=='A':
                        m.r(5,raw);m.r(3,offset);m.r(0,0x20002000)
                        self.run_slice(m,0x0802ee80,0x0802eeb0)
                        actual=m.r(2)
                    else:
                        m.r(6,raw)
                        self.run_slice(m,0x0802ef10,0x0802ef42)
                        actual=m.r(3)
                    self.assertEqual(actual,pitch_coordinate(raw,offset,bp,slopes),(side,custom,raw))
                    COUNTS['pitch_calibration_cases']+=1

    def test_saved_calibration_selection_and_layout(self):
        m=SliceMachine(double_precision=True);m.uc.mem_map(0x080e0000,0x1000)
        defaults=default_calibration();sentinel=0xdeadbeef
        fields=((0x20001380,32,12),(0x20001200,96,12),
                (0x20001300,160,8),(0x200012c0,192,8),
                (0x20001280,224,8),(0x20001240,256,8),
                (0x20001320,288,8),(0x200012e0,320,8),
                (0x200012a0,352,8),(0x20001260,384,8))
        for magic,mode,ready in ((0,0,0),(0,3,0),(1237,0,0),(1237,1,0),(1237,3,0),(1237,1,1)):
            flash=[0x10000000+j for j in range(104)]
            for j,value in enumerate(flash):m.word(0x080e0000+j*4,value)
            m.word(0x080e0000,magic);m.word(0x200144d4,mode);m.word(0x200144d0,ready)
            for addr,_,n in fields:
                for j in range(n):m.word(addr+4*j,sentinel)
            m.r(1,0);m.r(2,0x20002438)
            self.run_slice(m,0x0802c66c,0x0802c8c2)
            full=magic==1237 and (mode<=0 or ready!=0)
            for addr,source,n in fields:
                if full or (mode==3 and addr in (0x20001380,0x20001200)):
                    expected=flash[source//4:source//4+n]
                elif addr==0x20001380:expected=defaults['offsets']
                elif addr==0x20001200:expected=list(map(bits,defaults['gains']))
                elif addr in (0x20001300,0x200012c0):expected=defaults['pitch_breakpoints']
                elif addr in (0x20001280,0x20001240):expected=list(map(bits,defaults['pitch_slopes']))
                else:expected=[sentinel]*n
                self.assertEqual(expected,[m.word(addr+4*j) for j in range(n)],(magic,mode,ready,hex(addr)))
            COUNTS['calibration_load_cases']+=1

    def test_fast_partials_and_focus_smoothing(self):
        m=SliceMachine();rng=random.Random(6711)
        for i in range(128):
            raw=[rng.randrange(65536) for _ in range(4)]
            offsets=[rng.randrange(1000,5000) for _ in range(2)]
            gains=[f32(1/rng.randrange(20000,60000)) for _ in range(2)]
            old=[f32(rng.uniform(0,1.5)) for _ in range(4)]
            target=[f32(rng.random()),f32(rng.random())]
            m.word(m.sp+88,0);m.r(3,i)
            m.uc.mem_write(0x30000040+i*8,struct.pack('<4H',*raw))
            for j in range(2):
                m.word(0x20001380+(9+2*j)*4,offsets[j])
                m.floats(0x20001200+(9+2*j)*4,[gains[j]])
            for addr,value in zip((0x20002e58,0x20002e54,0x20000880,0x200023fc),old):m.floats(addr,[value])
            m.floats(0x20002e60,[target[0]]);m.floats(0x20002e68,[target[1]])
            self.run_slice(m,0x0802edb6,0x0802ee80)
            expected=[smoothed_control(old[0],target[0]),smoothed_control(old[1],target[1]),
                      partials_control(raw[0],offsets[0],gains[0],old[2]),
                      partials_control(raw[2],offsets[1],gains[1],old[3])]
            for addr,value in zip((0x20002e58,0x20002e54,0x20000880,0x200023fc),expected):
                self.equal_floats([value],m.floats(addr))
            self.assertEqual(m.r(5),raw[1]);self.assertEqual(m.r(6),raw[3])
            COUNTS['fast_control_cases']+=1

    def test_full_sam_callbacks_controls_phases_and_clean_outputs(self):
        self.check_full_sam_callbacks(False)

    def test_full_nonzero_sam_callbacks_all_outputs(self):
        self.check_full_sam_callbacks(True)

    def test_capture_gestures_audio_clock_and_planar_playback(self):
        self.check_full_sam_callbacks(True,capture=True)

    def check_full_sam_callbacks(self,nonzero,capture=False):
        """Complete callbacks; no substituted functions or instruction skips."""
        from unicorn import arm_const as arm
        m=SliceMachine(double_precision=True)
        for base,size in ((0x60000000,0x1000),(0x080e0000,0x1000),
                          (0x40000000,0x10000),(0x58020000,0x10000)):m.uc.mem_map(base,size)
        m.r(0,0);m.run(0x0802c4e0,0x0802c9ba,100000);COVERAGE.update(m.last_trace)
        m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)  # initializer slice ends before its epilogue
        m.word(0x20002eec,256);m.word(0x20002ec8,0)
        m.uc.mem_write(0x20002f66,bytes([1]))
        saved=[[],[]]
        if capture:
            for ptr,bank,desc in ((0x20002f74,0x60c01000,0x20000a20),(0x20002f70,0x60001000,0x20000920)):
                m.uc.mem_map(bank,0x40000);m.word(ptr,bank)
                m.word(desc+4,8);m.word(desc+8,8)
                m.floats(bank,[-9.]*(8*64))
        defaults=default_calibration();gain=defaults['gains'][0]
        slow=[31000,24000,37000,41000,18000,20000]
        m.uc.mem_write(0x30000020,struct.pack('<6H',*slow))
        normalized=[mul(x-2000,gain) for x in slow]
        fm=[mul(mul(mul(x,x),mul(x,x)),60.) for x in normalized[4:]]
        locations={'a_odd':0x2000241c,'a_even':0x20000900,'b_odd':0x20002424,'b_even':0x20002420,
                   'a_sine':0x2000240c,'b_sine':0x200008c0,'a_sub':0x20002418,'b_sub':0x20002414,
                   'a_analysis':0x20002410,'b_analysis':0x200008e0,
                   'a_random':0x200023d8,'b_random':0x20000840,
                   'a_previous':0x200023dc,'b_previous':0x200023d4}
        st={key:0. for key in locations};st.update(a_analysis=0.,b_analysis=0.,seed=0,a_pulse=0,b_pulse=0)
        focus=[0.,0.];partials=[0.,0.];rng=random.Random(6712)
        detector=[(0.,0.,[0.]*128,[0.]*128,[0.]*64) for _ in range(2)]
        envelopes=[0.,0.]
        analysis=[analysis_parameters_exact(normalized[j],normalized[k]) for j,k in ((0,1),(3,2))]
        for callback in range(8 if capture else 6):
            interaction=callback%3;half=128 if callback%2 else 0
            m.uc.mem_write(0x20002f54,bytes([interaction]));m.word(0x200134bc,interaction)
            if capture:
                if callback in (1,7):
                    # Recognized held Shift + fresh Array edges, start/stop.
                    m.word(0x58021810,192);m.word(0x20002f58,0x101)
                    m.word(0x20002f34,51);m.word(0x20002f38,51)
                    m.word(0x58020c10,128);m.word(0x58020410,64)
                else:
                    m.word(0x58021810,192 if callback>=3 else 0)
                    m.word(0x58020c10,0);m.word(0x58020410,0)
                for side in range(2):
                    if callback==4 or (callback==6 and side==0):
                        m.r(0,128 if side else 64);m.r(14,0x08020001)
                        self.run_slice(m,0x08032688,0x08020000)
            frames=[[rng.randrange(22000,64000),rng.randrange(20000,32000),
                     rng.randrange(22000,64000),rng.randrange(21000,33000)] for _ in range(64)]
            m.uc.mem_write(0x30000040+half*4,struct.pack('<256H',*[v for frame in frames for v in frame]))
            audio=[[int((1<<31)*.025*math.sin((callback*64+n+7)*f)) if nonzero else 0
                    for f in (.071,.113)] for n in range(64)]
            m.uc.mem_write(0x30000440+half*4,struct.pack('<128i',*[v for pair in audio for v in pair]))
            expected=[]
            for n,raw in enumerate(frames):
                q=[pitch_coordinate(raw[j],2000,defaults['pitch_breakpoints'],defaults['pitch_slopes']) for j in (1,3)]
                a_hz=mul(1<<((q[0]>>11)+4),mul(table('exp2_2048')[q[0]&2047],from_bits(0x3f82d013)))
                ao=mul(a_hz,from_bits(0x37aec33e));bo=b_pitch_increment(q[1],a_hz,interaction!=0)
                outputs=[]
                gains=interaction_ratios((ao,ao,bo,bo),interaction) if interaction==2 else (1.,1.)
                for side,slot in enumerate((0,2)):
                    partials[side]=partials_control(raw[slot],2000,gain,partials[side])
                    focus[side]=smoothed_control(focus[side],normalized[1 if side==0 else 2])
                    x=mul(f32(audio[n][side]),from_bits(0x2ffffff6))
                    old=envelopes[side];rising=old<abs(x)
                    envelopes[side]=fma(abs(x),f32(.1 if rising else .001),mul(old,f32(.9 if rising else .999)))
                    name='a' if side==0 else 'b'
                    detector[side]=sam_detector_step(*detector[side],x,st[name+'_analysis'],*analysis[side])
                    phase_gains=(1.,1.) if side==0 else gains
                    out=polynomial_synthesis(detector[side][4],partials[side],
                        mul(st[name+'_odd'],phase_gains[0]),mul(st[name+'_even'],phase_gains[1]),
                        active_terms(ao if side==0 else bo))
                    if side==1 and interaction==2:
                        out=tuple(mul(value,interaction_gate(st[key])) for value,key in zip(out,('a_odd','a_even')))
                    outputs.append(tuple((clip_a if side==0 else clip_b)(value) for value in out))
                st,sources,_=phase_step(st,(ao,ao,bo,bo,analysis[0][0],analysis[1][0]),fm,interaction=interaction)
                expected.append((sources,outputs,[mul(e,4.) for e in envelopes]))
            m.r(0,half);m.r(14,0x08020001)
            m.run(0x0802e750,0x08020000,1000000);COVERAGE.update(m.last_trace)
            for key,addr in locations.items():self.equal_floats([st[key]],m.floats(addr),(callback,key))
            self.assertEqual(st['seed'],m.word(0x20002eb0))
            for addr,value in zip((0x20000880,0x200023fc,0x20002e58,0x20002e54),partials+focus):
                self.equal_floats([value],m.floats(addr))
            for side in range(2):
                self.equal_floats(detector[side][2],m.floats(0x20000220+side*1024,count=128))
                self.equal_floats(detector[side][3],m.floats(0x20000020+side*1024,count=128))
                self.equal_floats(detector[side][4],m.floats(0x20002b40+side*512,count=64))
                if capture:
                    if callback in (1,2,3,4) or (callback==6 and side==0):saved[side].append(list(detector[side][4]))
                    bank=0x60001000 if side else 0x60c01000
                    for index,frame in enumerate(saved[side]):
                        self.equal_floats(frame,m.floats(bank+index*256,count=64),(callback,side,index))
                    self.equal_floats([-9.]*64,m.floats(bank+len(saved[side])*256,count=64),
                                      ('capture sentinel',callback,side,len(saved[side]),m.word(0x20002ed4 if side else 0x20002ed8)))
                    self.assertEqual(m.word(0x200011a0 if side else 0x20002e7c),len(saved[side])*64)
                    self.assertEqual(m.word(0x20002e80 if side else 0x20002e84),int(1<=callback<7))
                    peak=max((max(frame) for frame in saved[side]),default=0.)
                    self.equal_floats([peak],m.floats(0x200011e0 if side else 0x20002e9c))
            for i,(sources,outputs,aux) in enumerate(expected):
                for base,value in ((0x30001040,sources[1]),(0x38000004,sources[0])):
                    self.assertEqual(int(value*(1<<30))&0xffffffff,m.word(base+half*4+i*8),(callback,i))
                for base,value in zip((0x30001044,0x38000000),aux):
                    self.assertEqual(m.word(base+half*4+i*8),int(mul(value,f32(858993472.)))&0xffffffff)
                for base,value in ((0x30000c40,outputs[0][0]),(0x30000c44,outputs[0][1]),
                                   (0x30000844,outputs[1][0]),(0x30000840,outputs[1][1])):
                    word=m.word(base+half*4+i*8);signed=word if word<0x80000000 else word-0x100000000
                    error=abs(value-signed/(1<<30))
                    METRICS['full_sam_audio_max_absolute_error']=max(METRICS['full_sam_audio_max_absolute_error'],error)
                    self.assertLess(error,2e-4,(nonzero,callback,i,hex(base)))
            if capture:COUNTS['capture_audio_callbacks']+=1
            else:COUNTS['full_callbacks']+=1;COUNTS['full_callback_samples']+=64
        if capture:
            for side in range(2):
                desc=0x20000920 if side else 0x20000a20
                self.assertEqual(m.word(desc+4),4 if side else 5)
                self.assertEqual(m.word(desc+8),1)
                self.assertEqual(m.word(0x20002ed4 if side else 0x20002ed8),side)
                # Read the spectra just captured by the full audio callbacks.
                m.word(0x20002434 if side else 0x20002438,0)
                m.s(0,.25);m.s(1,.75);m.r(14,0x08020001)
                self.run_slice(m,0x0802d0f8 if side else 0x0802cfac,0x08020000)
                self.equal_floats(planar_deltas(saved[side],[0.]*64,.75,.25),
                                  m.floats(0x20000f40 if side else 0x20000d40,count=64))
                COUNTS['captured_frames_checked']+=len(saved[side]);COUNTS['captured_playback_readers']+=1

    def test_receive_start_success_and_audio_dispatch(self):
        from unicorn import arm_const as arm
        for stream in range(8):
            for config in range(4):
                for protocol in (0,8):
                    for enabled in (0,65536):
                        m=SliceMachine();m.uc.mem_map(0x40000000,0x30000)
                        sai=0x20014918;dma=0x20015000;peripheral=0x40015800
                        channel=0x40020010+24*stream
                        shift=(0,6,16,22)[stream%4]
                        status=0x40020000+(4 if stream>=4 else 0)
                        m.word(sai,peripheral);m.word(sai+132,dma)
                        m.word(sai+4,config);m.word(sai+68,protocol)
                        m.uc.mem_write(sai+145,b'\x01')
                        m.word(peripheral,enabled|0x80);m.word(peripheral+16,0x200)
                        m.word(dma,channel);m.word(dma+28,256);m.word(dma+56,sai)
                        m.uc.mem_write(dma+53,b'\x01')
                        m.word(dma+84,0x1234);m.word(dma+88,status);m.word(dma+92,shift)
                        m.word(dma+96,0x40020800);m.word(dma+100,0x40020810)
                        m.word(dma+104,0x40);m.word(channel,0x40100)
                        values=[f32((i+1)/64) for i in range(32)]
                        for i,value in enumerate(values):m.s(i,value)
                        m.r(0,sai);m.r(1,0x30000440);m.r(2,256);m.r(14,0x08020001)
                        self.run_slice(m,0x08025ce4,0x08020000)
                        self.assertEqual(m.r(0),0)
                        self.assertIn(0x08021cb0,m.last_trace)
                        self.assertEqual(bytes(m.uc.mem_read(sai+144,2)),b'\x00\x22')
                        self.assertEqual(bytes(m.uc.mem_read(dma+52,2)),b'\x01\x02')
                        self.assertEqual(m.word(sai+120),0x30000440)
                        self.assertEqual(m.word(sai+124),0x01000100)
                        self.assertEqual(m.word(sai+148),0);self.assertEqual(m.word(dma+84),0)
                        self.assertEqual([m.word(dma+i) for i in (60,64,76,80)],
                                         [0x08025e2d,0x08025e89,0x08025e95,0])
                        self.assertEqual([m.word(channel+i) for i in (0,4,8,12)],
                                         [0x11f,256,peripheral+28,0x30000440])
                        self.assertEqual(m.word(status+8),63<<shift)
                        self.assertEqual(m.word(0x40020814),0x40)
                        mask=97 if config in (2,3) else 5
                        if protocol==8 and config in (1,3):mask|=16
                        self.assertEqual(m.word(peripheral+16),0x200|mask)
                        self.assertEqual(m.word(peripheral),0x30080)
                        self.equal_floats(values,[m.s(i) for i in range(32)],'successful receive start')
                        COUNTS['receive_start_success_cases']+=1
                        # The firmware has configured these handles/registers itself.
                        # Supply each interrupt status explicitly; RAM register fixtures
                        # do not simulate transfers, W1C behavior, or exception entry.
                        for flag,offset in ((16,0),(32,128)):
                            m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
                            m.word(status,flag<<shift);m.r(0,dma);m.r(14,0x08020001)
                            self.run_slice(m,0x080223a8,0x0802e750)
                            self.assertEqual(m.r(0),offset)
                            self.assertEqual(m.word(status+8),flag<<shift)
                            self.equal_floats(values,[m.s(i) for i in range(32)],'started receive IRQ')
                            COUNTS['started_receive_audio_dispatches']+=1

    def test_receive_registration_dma_dispatch_preserves_fp_context(self):
        from unicorn import arm_const as arm
        for stream in range(8):
            for flag,offset,wrapper in ((16,0,0x08025e88),(32,128,0x08025e2c)):
                for variant in range(3):
                    m=SliceMachine();m.uc.mem_map(0x40000000,0x30000)
                    sai=0x20014918;dma=0x20015000;peripheral=0x40015800
                    m.word(sai,peripheral);m.word(sai+132,dma)
                    m.uc.mem_write(sai+145,b'\x01')
                    values=[f32((i+1)*(variant+1)/64) for i in range(32)]
                    for i,value in enumerate(values):m.s(i,value)
                    m.r(0,sai);m.r(1,0x30000440);m.r(2,256)
                    self.run_slice(m,0x08025ce4,0x08021cb0)
                    self.assertEqual([m.r(i) for i in range(4)],[dma,peripheral+28,0x30000440,256])
                    self.assertEqual([m.word(dma+i) for i in (60,64,76,80)],
                                     [0x08025e2d,0x08025e89,0x08025e95,0])
                    self.equal_floats(values,[m.s(i) for i in range(32)],'receive registration')
                    COUNTS['receive_callback_registration_cases']+=1
                    # Stop above before the DMA start routine. Supply a DMA1
                    # status snapshot explicitly; no hardware start/IRQ claimed.
                    m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
                    channel=0x40020010+24*stream
                    shift=(0,6,16,22)[stream%4]
                    status=0x40020000+(4 if stream>=4 else 0)
                    m.word(dma,channel);m.word(dma+28,256);m.word(dma+56,sai)
                    m.word(dma+88,status);m.word(dma+92,shift)
                    m.word(channel,0x118);m.word(status,flag<<shift)
                    m.r(0,dma);m.r(14,0x08020001)
                    self.run_slice(m,0x080223a8,0x0802e750)
                    self.assertEqual(m.r(0),offset,(stream,flag,variant))
                    self.assertIn(wrapper,m.last_trace)
                    self.assertIn(0x08032678 if offset else 0x08032680,m.last_trace)
                    self.assertEqual(m.word(status+8),flag<<shift)
                    self.equal_floats(values,[m.s(i) for i in range(32)],(stream,flag,variant))
                    COUNTS['dma_to_audio_entry_cases']+=1

    def test_full_button_mode_cycles_preserve_dsp_state(self):
        from unicorn import arm_const as arm
        sequence=(0,1,1,0,2,2,0,3,3,0,1,0,2,0,3,0)*2
        locations={'a_odd':0x2000241c,'a_even':0x20000900,'b_odd':0x20002424,'b_even':0x20002420,
                   'a_sine':0x2000240c,'b_sine':0x200008c0,'a_sub':0x20002418,'b_sub':0x20002414,
                   'a_analysis':0x20002410,'b_analysis':0x200008e0,
                   'a_random':0x200023d8,'b_random':0x20000840,
                   'a_previous':0x200023dc,'b_previous':0x200023d4}
        for interaction in range(3):
            m=SliceMachine(double_precision=True)
            for base,size in ((0x60000000,0x500000),(0x60c00000,0x100000),(0x080e0000,0x1000),
                              (0x40000000,0x10000),(0x58020000,0x10000),(0x52002000,0x1000)):
                m.uc.mem_map(base,size)
            m.r(0,0);m.run(0x0802c4e0,0x0802c9ba,100000);COVERAGE.update(m.last_trace)
            m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
            m.word(0x20002eec,256);m.word(0x20002ec8,0);m.uc.mem_write(0x20002f66,b'\x01')
            m.uc.mem_write(0x20002f54,bytes([interaction]));m.word(0x200134bc,interaction)
            # Original transport/flash routines take their real busy returns.
            # Every Array starts saved; there is no substituted file operation.
            m.uc.mem_write(0x20014c38,b'\x01');m.uc.mem_write(0x20002064,b'\x01')
            m.word(0x20002ebc,0x081e0000)
            arrays=[[[f32(.02+((side+1)*(frame+3)*(j+5)%97)/2000.) for j in range(64)]
                     for frame in range(64)] for side in range(2)]
            for side in range(2):
                base=0x20000920 if side else 0x20000a20;bank=0x60001000 if side else 0x60c01000
                origin=1048576 if side else 0
                for slot in range(16):m.uc.mem_write(base+slot*16,struct.pack('<4I',origin+slot*65536,8,8,1))
                m.word(0x20002f70 if side else 0x20002f74,bank)
                m.floats(bank+origin*4,[x for row in arrays[side] for x in row])
                m.word(0x20002ef4 if side else 0x20002ef8,5);m.word(0x20013494+side*4,5)
                # Explicit warm Noise fixture; this test targets mode continuity.
                m.floats(0x2000239c+side*24,[0.,0.,1.,.2,.7,0.])
                m.floats(0x200011c0 if side else 0x20002e98,[1.])
            defaults=default_calibration();gain=defaults['gains'][0]
            slow=(31000,24000,37000,41000,18000,20000)
            m.uc.mem_write(0x30000020,struct.pack('<6H',*slow))
            norm=[mul(x-2000,gain) for x in slow]
            fm=[mul(mul(mul(x,x),mul(x,x)),60.) for x in norm[4:]]
            analysis=[analysis_parameters_exact(norm[j],norm[k]) for j,k in ((0,1),(3,2))]
            st={key:m.floats(addr)[0] for key,addr in locations.items()};st.update(seed=0,a_pulse=0,b_pulse=0)
            detector=[(0.,0.,[0.]*128,[0.]*128,[0.]*64) for _ in range(2)]
            coefficients=[[0.]*64 for _ in range(2)];deltas=[[0.]*64 for _ in range(2)]
            chaos=[[0.]*16 for _ in range(2)]
            generators=[[0.,0.,1.,f32(.2),f32(.7),0.] for _ in range(2)]
            filters=[[0.]*12 for _ in range(2)];noise_env=[1.,1.];seed=0
            partials=[0.,0.];focus=[0.,0.];modes=[0,0];previous_buttons=0
            for callback,buttons in enumerate(sequence):
                half=128 if callback&1 else 0
                for side in range(2):
                    if buttons&(1<<side) and not previous_buttons&(1<<side):
                        modes[side]=(modes[side]+1)%4
                        COUNTS['mode_cycle_button_edges']+=1
                previous_buttons=buttons
                sao=[int(mode!=0) for mode in modes];engines=[max(0,mode-1) for mode in modes]
                slow=[18000+(callback*997+i*5003)%28000 for i in range(4)]+[18000,20000]
                m.uc.mem_write(0x30000020,struct.pack('<6H',*slow))
                norm=[mul(x-2000,gain) for x in slow]
                for side,(j,k) in enumerate(((0,1),(3,2))):
                    if not sao[side]:analysis[side]=analysis_parameters_exact(norm[j],norm[k])
                # SAO skips pole setup but still runs detectors. Vary the
                # callee-saved entry registers explicitly, not as board evidence.
                entry_poles=(f32((callback%3+1)/8),f32((callback%3+4)/8))
                m.s(22,entry_poles[0]);m.s(23,entry_poles[1])
                m.word(0x58020c10,128 if buttons&1 else 0);m.word(0x58020410,64 if buttons&2 else 0)
                frames=[[26000+(n*31)%3000,24000,29000+(n*43)%3000,27000] for n in range(64)]
                audio=[[int((1<<31)*.025*math.sin((callback*64+n+7)*f)) for f in (.071,.113)] for n in range(64)]
                m.uc.mem_write(0x30000040+half*4,struct.pack('<256H',*[x for row in frames for x in row]))
                m.uc.mem_write(0x30000440+half*4,struct.pack('<128i',*[x for row in audio for x in row]))
                preserved=((0x20000020,2048),(0x200022bc,272),(0x20002a40,1024))
                snapshot=[bytes(m.uc.mem_read(a,n)) for a,n in preserved]
                old_phases=[m.word(a) for a in locations.values()]
                m.r(0,half);m.r(14,0x08020001)
                m.run(0x0802e750,0x0802e86c,1000000);COVERAGE.update(m.last_trace)
                self.assertEqual(snapshot,[bytes(m.uc.mem_read(a,n)) for a,n in preserved],(interaction,callback,'prefix state'))
                self.assertEqual(old_phases,[m.word(a) for a in locations.values()])
                self.assertEqual(list(m.uc.mem_read(0x20002f52,2)),sao)
                self.assertEqual([m.word(a) for a in (0x20002e94,0x20002e90)],sao)
                self.assertEqual([m.word(a) for a in (0x20002e8c,0x20002e88)],engines)
                self.assertEqual(list(struct.unpack('<2H',m.uc.mem_read(0x2001348c,4))),sao)
                self.assertEqual([m.word(0x2001349c+i*4) for i in range(2)],engines)
                deltas=[planar_deltas(arrays[i],coefficients[i],norm[0 if i==0 else 3],norm[1 if i==0 else 2])
                        if sao[i] else deltas[i] for i in range(2)]
                expected=[]
                for n,raw in enumerate(frames):
                    q=[pitch_coordinate(raw[j],2000,defaults['pitch_breakpoints'],defaults['pitch_slopes']) for j in (1,3)]
                    hz=mul(1<<((q[0]>>11)+4),mul(table('exp2_2048')[q[0]&2047],from_bits(0x3f82d013)))
                    odd=(mul(hz,from_bits(0x37aec33e)),b_pitch_increment(q[1],hz,interaction!=0))
                    even=[even_increment(odd[i],mul(-(audio[n][i]>>15)-2000,gain),bool(sao[i])) for i in range(2)]
                    # SAM computes these stored rates; oscillator modes retain them.
                    inc=(odd[0],even[0],odd[1],even[1],analysis[0][0],analysis[1][0])
                    next_st,sources,_=phase_step(st,inc,fm,(5,5),interaction=interaction)
                    ratios=interaction_ratios(inc,interaction) if interaction==2 else (1.,1.)
                    aux=[auxiliary_value(name.upper(),5,st[name+'_sub'],st[name+'_random'],st[name+'_previous'],bool(sao[i]),0.,raw[3])
                         for i,name in enumerate(('a','b'))]
                    outputs=[]
                    for side,name in enumerate(('a','b')):
                        partials[side]=partials_control(raw[2*side],2000,gain,partials[side])
                        focus[side]=smoothed_control(focus[side],norm[1 if side==0 else 2])
                        slide=norm[0 if side==0 else 3]
                        x=mul(f32(audio[n][side]),from_bits(0x2ffffff6))
                        detector[side]=sam_detector_step(*detector[side],x,st[name+'_analysis'],analysis[side][0],
                                                         entry_poles[side] if sao[side] else analysis[side][1])
                        if engines[side]==0:
                            terms=active_terms(even[0] if side==0 and sao[0] else odd[side])
                            # Standard synthesis updates the working bank even
                            # in SAM, using the last SAO deltas retained in RAM.
                            coefficients[side]=[add(v,deltas[side][j]) if j<terms else v for j,v in enumerate(coefficients[side])]
                            phase_ratios=ratios if side else (1.,1.)
                            out=polynomial_synthesis(coefficients[side] if sao[side] else detector[side][4],partials[side],
                                mul(st[name+'_odd'],phase_ratios[0]),mul(st[name+'_even'],phase_ratios[1]),terms)
                        elif engines[side]==1:
                            generators[side],filters[side],noise_env[side],seed,out=noise_step(
                                generators[side],filters[side],noise_env[side],seed,partials[side],slide,focus[side],
                                st[name+'_odd'],st[name+'_even'],False,name.upper(),ratios if side else (1.,1.))
                        else:chaos[side],out,_=chaos_step(chaos[side],partials[side],slide,focus[side],odd[side],even[side],fm[side],sources[side],name.upper())
                        if side and interaction==2:out=tuple(mul(v,interaction_gate(st[k])) for v,k in zip(out,('a_odd','a_even')))
                        outputs.append(tuple((clip_b if side else clip_a)(v) for v in out))
                    expected.append((sources,aux,outputs));st=next_st
                m.run(0x0802e86c,0x08020000,1000000);COVERAGE.update(m.last_trace)
                self.assertEqual(m.word(0x20002eec),256+(callback+1)*64)
                self.equal_floats(entry_poles,[m.s(22),m.s(23)],'callee-saved pole registers')
                self.equal_floats([analysis[0][0],analysis[1][0]],m.floats(0x200023ec,count=2),'retained analyzer rates')
                for addr,value in zip((0x20000880,0x200023fc,0x20002e58,0x20002e54),partials+focus):
                    self.equal_floats([value],m.floats(addr))
                for key,addr in locations.items():self.equal_floats([st[key]],m.floats(addr),(interaction,callback,key))
                self.assertEqual(m.word(0x20002eb0),st['seed']);self.assertEqual(m.word(0x200023cc),seed)
                for side in range(2):
                    for values,addr in ((detector[side][2],0x20000220+side*1024),(detector[side][3],0x20000020+side*1024),
                                        (detector[side][4],0x20002b40+side*512),(coefficients[side],0x20002a40+side*512),
                                        (chaos[side],0x200022bc+side*64),(generators[side],0x2000239c+side*24)):
                        self.equal_floats(values,m.floats(addr,count=len(values)),(interaction,callback,side,hex(addr)))
                    base=0x2000233c+side*24
                    self.equal_floats(filters[side],m.floats(base,count=6)+m.floats(base+48,count=6))
                    self.equal_floats([noise_env[side]],m.floats(0x200011c0 if side else 0x20002e98))
                for n,(sources,aux,outputs) in enumerate(expected):
                    for base,value in ((0x30001040,sources[1]),(0x38000004,sources[0])):
                        self.assertEqual(m.word(base+half*4+n*8),int(value*(1<<30))&0xffffffff)
                    for side,base in enumerate((0x30001044,0x38000000)):
                        self.assertEqual(m.word(base+half*4+n*8),int(mul(aux[side],f32(858993472.)))&0xffffffff)
                    for side,bases in enumerate(((0x30000c40,0x30000c44),(0x30000844,0x30000840))):
                        for base,value in zip(bases,outputs[side]):
                            word=m.word(base+half*4+n*8)
                            if engines[side]:self.assertEqual(word,int(mul(value,1<<30))&0xffffffff)
                            else:
                                error=abs(value-(word if word<0x80000000 else word-0x100000000)/(1<<30))
                                METRICS['mode_cycle_standard_max_absolute_error']=max(METRICS['mode_cycle_standard_max_absolute_error'],error)
                                self.assertLess(error,2e-4,(interaction,callback,n,side))
                COUNTS['mode_cycle_full_callbacks']+=1
            self.assertEqual(modes,[0,0])

    def test_full_noise_chaos_callbacks_all_outputs(self):
        self.check_full_noise_chaos_callbacks(False)

    def test_full_sao_callbacks_linear_and_planar(self):
        self.check_full_noise_chaos_callbacks(False,sao=True)

    def test_cold_noise_callbacks_through_mute_release(self):
        self.check_full_noise_chaos_callbacks(True)

    def test_cold_noise_from_complete_initializer(self):
        self.check_full_noise_chaos_callbacks(True,startup=True)

    def check_full_noise_chaos_callbacks(self,cold,sao=False,startup=False):
        """ADC through all eight lanes, including mixed engines and LF switches.

        The mixed-engine case supplies warm finite Noise state. The cold case
        retains zeroed RAM and the initializer's Noise state and mute flag.
        Both use valid synthetic Arrays. The startup variant retains counter
        and enable from the complete initializer; other variants supply them.
        Engine selections are RAM fixtures, not simulated selection gestures.
        The SAO variant checks changing linear/planar readers and slow controls,
        persistent working coefficients and mathematical standard synthesis.
        """
        from unicorn import arm_const as arm
        m=SliceMachine(double_precision=True)
        for base,size in ((0x60000000,0x100000),(0x60c00000,0x100000),
                          (0x080e0000,0x1000),(0x40000000,0x10000),(0x58020000,0x10000)):
            m.uc.mem_map(base,size)
        if cold:
            m.uc.mem_write(0x2001348c,struct.pack('<2H',1,1))
            m.word(0x2001349c,1);m.word(0x200134a0,1)
        initial_counter=0 if startup else 256
        cold_prefix='startup_noise' if startup else 'cold_noise'
        if startup:
            from unicorn import UC_HOOK_CODE
            # The complete initializer supplies enable=1 and counter=0 itself.
            # Driver calls return busy; callback delivery below is explicit,
            # not evidence of physical DMA running after a failed start.
            m.uc.mem_write(0x20014c38,b'\x01');m.uc.mem_write(0x20000000,b'\x01')
            def advance_tick(uc,address,size,user):
                if address==0x0802039c:m.word(0x2000204c,m.word(0x2000204c)+1)
            hook=m.uc.hook_add(UC_HOOK_CODE,advance_tick)
            m.r(0,0x20014bb8);m.r(14,0x08020001)
            try:m.run(0x0802c4e0,0x08020000,200000);COVERAGE.update(m.last_trace)
            finally:m.uc.hook_del(hook)
            self.assertEqual(m.word(0x20002eec),0)
            self.assertEqual(m.uc.mem_read(0x20002f66,1)[0],1)
        else:
            m.r(0,0);m.run(0x0802c4e0,0x0802c9ba,100000);COVERAGE.update(m.last_trace)
        m.uc.reg_write(arm.UC_ARM_REG_SP,m.sp)
        if not startup:m.word(0x20002eec,initial_counter)
        if not cold:m.word(0x20002ec8,0)
        if not startup:m.uc.mem_write(0x20002f66,b'\x01')
        m.uc.mem_write(0x20002f52,b'\x01\x01')
        m.uc.mem_write(0x2001348c,struct.pack('<2H',1,1))
        for ptr,bank,desc in ((0x20002f74,0x60c01000,0x20000a20),(0x20002f70,0x60001000,0x20000920)):
            m.word(ptr,bank);m.word(desc,0);m.word(desc+4,8);m.word(desc+8,8)
        arrays=[[[[f32(.01+((side+1)*(slot+1)*(frame+3)*(j+5)%97)/2000.) for j in range(64)]
                   for frame in range(64)] for slot in range(2)] for side in range(2)] if sao else None
        selected=[0,0]
        coefficients=[[0.]*64 for _ in range(2)]
        if sao:
            # Start in the state left by mode-entry save handling. Subsequent
            # callbacks must preserve normal DSP while clearing this byte.
            m.uc.mem_write(0x20002f65,b'\x01')
            m.uc.mem_write(0x20014c38,b'\x01')
            for side,bank in enumerate((0x60c01000,0x60001000)):
                base=0x20000920 if side else 0x20000a20
                for slot in range(2):
                    m.word(base+slot*16,slot*65536);m.word(base+slot*16+4,8);m.word(base+slot*16+8,8)
                    m.floats(bank+slot*262144,[v for row in arrays[side][slot] for v in row])
        defaults=default_calibration();gain=defaults['gains'][0]
        slow=[31000,24000,37000,41000,2000 if cold else 18000,2000 if cold else 20000]
        m.uc.mem_write(0x30000020,struct.pack('<6H',*slow))
        normalized=[mul(x-2000,gain) for x in slow]
        fm=[mul(mul(mul(x,x),mul(x,x)),60.) for x in normalized[4:]]
        locations={'a_odd':0x2000241c,'a_even':0x20000900,'b_odd':0x20002424,'b_even':0x20002420,
                   'a_sine':0x2000240c,'b_sine':0x200008c0,'a_sub':0x20002418,'b_sub':0x20002414,
                   'a_analysis':0x20002410,'b_analysis':0x200008e0,
                   'a_random':0x200023d8,'b_random':0x20000840,
                   'a_previous':0x200023dc,'b_previous':0x200023d4}
        st={key:0. for key in locations};st.update(seed=0,a_pulse=0,b_pulse=0)
        chaos=[[0.]*16 for _ in range(2)]
        generators=[[0.,0.,1.,0. if cold else f32(.2),0. if cold else f32(.7),0.] for _ in range(2)]
        filters=[[0.]*12 for _ in range(2)];envelopes=[0.,0.] if cold else [1.,1.];noise_seed=0
        for side in range(2):
            if cold:
                self.equal_floats(generators[side],m.floats(0x2000239c+side*24,count=6))
                self.equal_floats([0.],m.floats(0x20002e98 if side==0 else 0x200011c0))
            else:
                m.floats(0x2000239c+side*24,generators[side])
                m.floats(0x20002e98 if side==0 else 0x200011c0,[1.])
        focus=[0.,0.];partials=[0.,0.];rng=random.Random(6714)
        muted=cold
        for callback in range(200 if cold else 12):
            engines=(0,0) if sao else (1,1) if cold else ((1,1),(2,2),(1,2),(2,1))[callback%4]
            interaction=0 if cold else callback%3;half=128 if callback%2 else 0
            lf=(False,False) if cold else (callback%4==2,callback%4==3)
            m.uc.mem_write(0x20002f54,bytes([interaction]));m.word(0x200134bc,interaction)
            for addr,value in ((0x20002e8c,engines[0]),(0x20002e88,engines[1]),
                               (0x200023d0,int(lf[0])),(0x20000820,int(lf[1]))):m.word(addr,value)
            if sao:
                # Selection occurs inside this audio callback's real UI handler.
                # Hold one Shift and release the opposite after the short-hold threshold.
                if callback in (3,7):
                    side=int(callback==7);selected[side]+=1
                    m.word(0x58021810,128 if side else 64);m.word(0x20002f58,0x101)
                    m.word(0x20002f34,51 if side else 50);m.word(0x20002f38,50 if side else 51)
                else:m.word(0x58021810,0)
                linear=(callback&1,(callback>>1)&1)
                m.uc.mem_write(0x20013490,struct.pack('<2H',*linear))
                slow=[2000+(callback*13001+i*11003)%60000 for i in range(4)]+[18000,20000]
                m.uc.mem_write(0x30000020,struct.pack('<6H',*slow))
                normalized=[mul(x-2000,gain) for x in slow]
                deltas=[(linear_deltas if linear[side] else planar_deltas)(
                    arrays[side][selected[side]],coefficients[side],normalized[0 if side==0 else 3],
                    normalized[1 if side==0 else 2]) for side in range(2)]
            frames=[[rng.randrange(22000,40000),rng.randrange(20000,32000),
                     rng.randrange(22000,40000),rng.randrange(21000,33000)] for _ in range(64)]
            if sao:
                for n,raw in enumerate(frames):
                    raw[1]=(12000,26000,45000,62000)[(callback+n//16)%4]
                    raw[3]=31000 if interaction else (12000,26000,45000,62000)[(callback+n//16+1)%4]
            if cold:frames=[[2000,26000,2000,30000] for _ in range(64)]
            m.uc.mem_write(0x30000040+half*4,struct.pack('<256H',*[v for row in frames for v in row]))
            # Signed audio words become the calibrated even-rate controls in SAO.
            audio=[[-code*32768 for code in (2000+(n*997+callback*307)%61000,
                                            2000+(n*1231+callback*541)%61000)] for n in range(64)]
            if cold:audio=[[0,0] for _ in range(64)]
            m.uc.mem_write(0x30000440+half*4,struct.pack('<128i',*[v for row in audio for v in row]))
            expected=[]
            for n,raw in enumerate(frames):
                q=[pitch_coordinate(raw[j],2000,defaults['pitch_breakpoints'],defaults['pitch_slopes']) for j in (1,3)]
                a_hz=mul(1<<((q[0]>>11)+4),mul(table('exp2_2048')[q[0]&2047],from_bits(0x3f82d013)))
                ao=mul(a_hz,from_bits(0x37aec33e))
                if lf[0]:ao=mul(ao,1/256)
                bo=b_pitch_increment(q[1],a_hz,interaction!=0,lf[1])
                even=[even_increment(odd,mul(-(audio[n][side]>>15)-2000,gain))
                      for side,odd in enumerate((ao,bo))]
                inc=(ao,even[0],bo,even[1],0.,0.)
                next_st,sources,_=phase_step(st,inc,fm,interaction=interaction)
                ratios=interaction_ratios(inc,interaction) if interaction==2 else (1.,1.)
                aux=[auxiliary_value(name.upper(),0,st[name+'_sub'],st[name+'_random'],st[name+'_previous'],
                                     True,0.,raw[3]) for name in ('a','b')]
                outputs=[]
                for side,name in enumerate(('a','b')):
                    partials[side]=partials_control(raw[side*2],2000,gain,partials[side])
                    focus[side]=smoothed_control(focus[side],normalized[1 if side==0 else 2])
                    slide=normalized[0 if side==0 else 3]
                    if engines[side]==0:
                        terms=active_terms(inc[1 if side==0 else 2])
                        METRICS['full_sao_min_active_terms']=min(METRICS['full_sao_min_active_terms'],terms)
                        METRICS['full_sao_max_active_terms']=max(METRICS['full_sao_max_active_terms'],terms)
                        coefficients[side]=[add(value,deltas[side][j]) if j<terms else value
                                            for j,value in enumerate(coefficients[side])]
                        phase_ratios=ratios if side else (1.,1.)
                        out=polynomial_synthesis(coefficients[side],partials[side],
                                                 mul(st[name+'_odd'],phase_ratios[0]),
                                                 mul(st[name+'_even'],phase_ratios[1]),terms)
                    elif engines[side]==1:
                        generators[side],filters[side],envelopes[side],noise_seed,out=noise_step(
                            generators[side],filters[side],envelopes[side],noise_seed,partials[side],slide,focus[side],
                            st[name+'_odd'],st[name+'_even'],lf[side],name.upper(),ratios if side else (1.,1.))
                    else:
                        chaos[side],out,_=chaos_step(chaos[side],partials[side],slide,focus[side],
                                                  inc[side*2],inc[side*2+1],fm[side],sources[side],name.upper())
                    if side and interaction==2:
                        out=tuple(mul(value,interaction_gate(st[key])) for value,key in zip(out,('a_odd','a_even')))
                    if cold and side==0 and not muted and any(math.isnan(value) for value in out):
                        COUNTS[cold_prefix+'_unmuted_nan_frames']+=1
                    metric=cold_prefix+'_first_finite_sample_'+name
                    if cold and not METRICS[metric] and all(math.isfinite(value) for value in out):
                        METRICS[metric]=callback*64+n+1
                    outputs.append(tuple((clip_a if side==0 else clip_b)(value) for value in out))
                if muted:
                    sources=(0.,0.);outputs=[(0.,0.),(0.,0.)];aux=[0.,0.]
                    if initial_counter+callback*64+n+1>8192:muted=False
                expected.append((sources,outputs,aux));st=next_st
            m.r(0,half);m.r(14,0x08020001);m.run(0x0802e750,0x08020000,1000000)
            COVERAGE.update(m.last_trace)
            self.assertEqual(m.word(0x20002eec),initial_counter+(callback+1)*64)
            if sao:
                self.assertEqual(m.uc.mem_read(0x20002f65,1)[0],2 if callback==0 else 0)
                self.assertEqual(0x08032224 in m.last_trace,callback==1)
                self.assertEqual(m.word(0x20002eec),256+(callback+1)*64)
                if callback==1:COUNTS['full_sao_recovery_callbacks']+=1
            self.assertEqual(m.word(0x20002ec8),int(muted))
            for key,addr in locations.items():self.equal_floats([st[key]],m.floats(addr),(callback,key))
            self.assertEqual(st['seed'],m.word(0x20002eb0));self.assertEqual(noise_seed,m.word(0x200023cc))
            for addr,value in zip((0x20000880,0x200023fc,0x20002e58,0x20002e54),partials+focus):
                self.equal_floats([value],m.floats(addr))
            for side in range(2):
                if sao:
                    self.assertEqual(m.word(0x20000b20 if side else 0x20002430),selected[side])
                    self.assertEqual(m.word(0x200134b4+side*4),selected[side])
                    self.equal_floats(coefficients[side],m.floats(0x20002c40 if side else 0x20002a40,count=64),
                                      (callback,side,'SAO working coefficients'))
                    self.equal_floats(deltas[side],m.floats(0x20000f40 if side else 0x20000d40,count=64))
                self.equal_floats(chaos[side],m.floats(0x200022bc+side*64,count=16),(callback,side,'chaos'))
                self.equal_floats(generators[side],m.floats(0x2000239c+side*24,count=6))
                base=0x2000233c+side*24
                self.equal_floats(filters[side],m.floats(base,count=6)+m.floats(base+48,count=6))
                self.equal_floats([envelopes[side]],m.floats(0x20002e98 if side==0 else 0x200011c0))
            for n,(sources,outputs,aux) in enumerate(expected):
                for base,value in ((0x30001040,sources[1]),(0x38000004,sources[0]),
                                   (0x30000c40,outputs[0][0]),(0x30000c44,outputs[0][1]),
                                   (0x30000844,outputs[1][0]),(0x30000840,outputs[1][1])):
                    actual=m.word(base+half*4+n*8)
                    if sao and base in (0x30000c40,0x30000c44,0x30000844,0x30000840):
                        signed=actual if actual<0x80000000 else actual-0x100000000
                        error=abs(value-signed/(1<<30))
                        METRICS['full_sao_audio_max_absolute_error']=max(METRICS['full_sao_audio_max_absolute_error'],error)
                        self.assertLess(error,2e-4,(callback,n,hex(base)))
                    else:self.assertEqual(int(value*(1<<30))&0xffffffff,actual,(callback,n,hex(base)))
                for base,value in zip((0x30001044,0x38000000),aux):
                    self.assertEqual(int(mul(value,f32(858993472.)))&0xffffffff,m.word(base+half*4+n*8))
            prefix='full_sao' if sao else cold_prefix if cold else 'full_noise_chaos'
            COUNTS[prefix+'_callbacks']+=1;COUNTS[prefix+'_samples']+=64
            if sao and callback in (3,7):COUNTS['full_sao_array_switches']+=1
        if cold:
            self.assertEqual(COUNTS[cold_prefix+'_unmuted_nan_frames'],3811 if startup else 4067)
            for name in ('a','b'):self.assertEqual(METRICS[cold_prefix+'_first_finite_sample_'+name],12005)
            self.assertTrue(all(e>0. and math.isfinite(e) for e in envelopes))

    def test_nonzero_sam_reference_and_detector_banks(self):
        m=SliceMachine();rng=random.Random(6713)
        m.r(9,0);m.word(m.sp+144,0x20002e90)
        self.run_slice(m,0x0802ec94,0x0802ed12)
        m.word(m.sp+80,0x200023ec);m.word(m.sp+84,0x200023f0)
        state=[]
        for side in range(2):
            cs=[f32(rng.uniform(-.1,.1)) for _ in range(128)]
            ss=[f32(rng.uniform(-.1,.1)) for _ in range(128)]
            amps=[f32(rng.random()) for _ in range(64)]
            state.append((0.,0.,cs,ss,amps))
            m.floats(0x20000220+side*1024,cs);m.floats(0x20000020+side*1024,ss)
            m.floats(0x20002b40+side*512,amps)
            m.floats(0x20000e40+side*512,[.125]*64)
        for sample in range(160):
            phases=[f32(rng.random()),f32(rng.random())]
            increments=[f32((.001,.0074,.0075,.06,.11249,.1125,.13)[(sample+j)%7]) for j in range(2)]
            poles=[f32(rng.uniform(.9,.9999)) for _ in range(2)]
            inputs=[f32(rng.uniform(-.1,.1)) for _ in range(2)]
            for side in range(2):
                m.floats(0x20002410 if side==0 else 0x200008e0,[phases[side]])
                m.floats(0x200023ec+side*4,[increments[side]])
                m.s(16+side*2,inputs[side]);m.s(22+side,poles[side])
                state[side]=sam_detector_step(*state[side],inputs[side],phases[side],increments[side],poles[side])
            self.run_slice(m,0x0802f29c,0x0802f9bc)
            for side in range(2):
                c,s=analysis_references(phases[side])
                self.equal_floats(c,m.floats(0x20002640 if side==0 else 0x20002940,count=64))
                self.equal_floats(s,m.floats(0x20002540 if side==0 else 0x20002840,count=64))
                u,conditioned,cs,ss,amps=state[side]
                self.equal_floats([u],m.floats(0x20002e74 if side==0 else 0x20002e6c))
                self.equal_floats([conditioned],m.floats(0x20002e70 if side==0 else 0x20001160))
                self.equal_floats(cs,m.floats(0x20000220+side*1024,count=128),(sample,side,'cosine'))
                self.equal_floats(ss,m.floats(0x20000020+side*1024,count=128),(sample,side,'sine'))
                self.equal_floats(amps,m.floats(0x20002b40+side*512,count=64),(sample,side,'magnitude'))
                COUNTS['sam_bank_samples']+=1

    def test_cold_noise_nan_clipping(self):
        for side in ('A','B'):
            m=SliceMachine()
            base=0x2000239c+(24 if side=='B' else 0)
            m.floats(base,[0.,0.,1.,0.,0.,0.]);m.word(m.sp+32,0x08043a74)
            if side=='A':
                m.r(3,1)
                for reg,value in ((1,0.),(4,.5),(6,.5),(18,.1),(16,.2)):m.s(reg,value)
                self.run_slice(m,0x08031038,0x0802fc16)
                self.assertTrue(math.isnan(m.s(14)) and math.isnan(m.s(13)))
                self.run_slice(m,0x0802fc16,0x0802fc66)
                self.equal_floats([clip_a(math.nan)]*2,[m.s(31),m.s(30)])
            else:
                m.word(m.sp+12,0);m.s(9,1.)
                for reg,value in ((2,0.),(12,.5),(14,.5)):m.s(reg,value)
                m.floats(0x20002424,[.1]);m.floats(0x20002420,[.2])
                self.run_slice(m,0x08031310,0x0802ff00)
                self.assertTrue(math.isnan(m.s(14)) and math.isnan(m.s(6)))
                self.run_slice(m,0x080302b8,0x08030304)
                self.equal_floats([clip_b(math.nan)]*2,[m.s(15),m.s(11)])
            COUNTS['cold_noise_cases']+=1


if __name__ == '__main__':
    report=None
    if '--report' in sys.argv:
        i=sys.argv.index('--report');report=Path(sys.argv[i+1]);del sys.argv[i:i+2]
    suite=unittest.defaultTestLoader.loadTestsFromTestCase(FirmwareDifferential)
    result=unittest.TextTestRunner(verbosity=2).run(suite)
    if report:
        import unicorn
        report.write_text(json.dumps({'passed':result.wasSuccessful(),'unicorn':unicorn.__version__,
                                     'tests_run':result.testsRun,'failures':len(result.failures),'errors':len(result.errors),
                                     'counts':COUNTS,'metrics':METRICS,'unique_instruction_addresses':len(COVERAGE),
                                     'addresses':[f'0x{x:08x}' for x in sorted(COVERAGE)]},indent=2)+'\n')
    sys.exit(not result.wasSuccessful())
