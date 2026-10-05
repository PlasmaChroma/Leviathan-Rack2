"""Focused original-firmware oracle for the reusable UI action models.

Run directly, optionally with --report PATH. This does not replace the full
DSP/integration suite or claim full-suite coverage in its report.
"""
import argparse
import json
import unittest
from pathlib import Path

import test_arm_differential as differential


TESTS = (
    'test_calibration_shift_release_transitions',
    'test_calibration_post_save_caller_transitions',
    'test_normal_shift_stage_model',
    'test_shift_release_finish_model',
    'test_shift_passive_action_models',
    'test_shift_dispatch_model',
    'test_consumed_shift_route_model',
    'test_consumed_gesture_release_and_rearm',
    'test_shift_release_decision_model',
    'test_long_hold_linear_toggle_and_factory_reset_dispatch',
    'test_array_selection_action_model',
    'test_clock_interrupt_through_ui_handler',
    'test_clock_timer_prefix_model',
    'test_button_edge_sampling_model',
    'test_array_button_action_model',
    'test_shared_button_action_model',
    'test_concurrent_array_and_shared_button_edges',
    'test_simultaneous_shift_releases_with_array_edges',
)


if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report',type=Path)
    args=parser.parse_args()
    suite=unittest.TestSuite(differential.FirmwareDifferential(name) for name in TESTS)
    result=unittest.TextTestRunner(verbosity=2).run(suite)
    if args.report:
        keys=('calibration_stable_callbacks','calibration_release_cases','calibration_save_dispatch_cases','calibration_post_save_cases','normal_shift_reset_resumptions','normal_shift_stage_cases','normal_shift_stage_reset_boundaries','normal_shift_button_tail_cases','shift_release_finish_cases','shift_passive_action_cases','shift_dispatch_cases','consumed_gesture_clock_sequences','rearmed_clock_admissions','consumed_shift_route_cases','shift_release_decision_cases','long_hold_model_cases','array_selection_model_cases','admitted_clock_model_cases','clock_ui_cases',
              'clock_timer_model_cases','button_edge_model_cases','button_input_tail_cases',
              'array_button_action_model_cases','shared_button_action_model_cases',
              'composed_button_action_cases','concurrent_button_cases',
              'concurrent_button_held_callbacks','simultaneous_shift_release_cases',
              'shift_release_shared_edge_cases','shift_release_shared_held_cases')
        report={'scope':'Focused UI action-model comparisons; not the full firmware suite',
                'tests':list(TESTS),'passed':result.wasSuccessful(),
                'tests_run':result.testsRun,'failures':len(result.failures),'errors':len(result.errors),
                'counts':{k:differential.COUNTS[k] for k in keys},
                'unique_instruction_addresses':len(differential.COVERAGE)}
        args.report.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    raise SystemExit(not result.wasSuccessful())
