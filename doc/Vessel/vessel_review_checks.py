#!/usr/bin/env python3
"""Review diagnostics: independent derivations plus a short passive trajectory.

Requires NumPy and the adjacent algebra oracle. No listening, calibrated physics,
moving-contact onset certification, or production performance is claimed.
Run from any directory: python doc/Vessel/vessel_review_checks.py
"""
import json
import math
from pathlib import Path

import numpy as np

from vessel_numerical_checks import coefficients, friction_root, potential


def slope(s, normal, mallet):
    ms, mk = mallet['muS'], mallet['muK']
    vc = mallet['weakeningVelocityMetersPerSecond']
    eps = mallet['regularizationVelocityMetersPerSecond']
    e = math.exp(-(s / vc)**2)
    t = math.tanh(s / eps)
    return normal * (-2*s/(vc*vc)*(ms-mk)*e*t
                     + (mk+(ms-mk)*e)*(1-t*t)/eps)


def bank(bowl, mallet, f0, theta, h):
    f, decay, bt = [], [], []
    for pair in bowl['pairs']:
        n = pair['azimuthalOrder']
        beta = n*(theta-pair['orientationRadians'])
        patch = np.sinc(n*mallet['fullAngularPatchWidthRadians']/(2*math.pi))
        for side, sign, shape in [('A', -1, -math.sin(beta)/n),
                                  ('B', 1, math.cos(beta)/n)]:
            f.append(f0*pair['centerRatio']*2**(sign*pair['splitCents']/2400))
            decay.append(pair[f't60{side}Seconds'])
            bt.append(patch*shape/math.sqrt(pair[f'mass{side}Kg']))
    co = coefficients(np.array(f), np.array(decay), h)
    return co, np.array(bt)


def main():
    base = Path(__file__).parent
    profiles = json.loads((base/'vessel_seed_profiles.json').read_text())
    result = {'scope': 'review diagnostics; no sustained moving-contact audio validation',
              'seed': 9302026}
    rng = np.random.default_rng(result['seed'])
    h = 1/192000
    # Digital pole matching changes static compliance of a mass-normalized mode.
    warp = []
    for fraction in [0.05, 0.1, 0.2, 0.35, 0.4]:
        f = fraction/h
        _, _, _, om, _, _ = coefficients(np.array([f]), np.array([120.]), h)
        ratio = (2*math.pi*f/om[0])**2
        predicted = (math.pi*fraction/math.tan(math.pi*fraction))**2
        assert abs(ratio-predicted) < 1e-10
        warp.append({'f_over_internal_rate': fraction,
                     'static_compliance_over_continuous_target': float(ratio)})
    result['prewarping_compliance'] = warp
    # Prove the nested compression response stays decreasing for either slope sign.
    smallest = math.inf
    for _ in range(10000):
        b = rng.normal(size=(14, 2))
        Y = b.T @ (rng.uniform(1e-7, 1e-5, 14)[:, None]*b)
        a = rng.uniform(-0.9, 100.)/Y[1, 1]
        effective = Y[0, 0]+h/(2*.045)-Y[0, 1]**2*a/(1+Y[1, 1]*a)
        smallest = min(smallest, effective)
        assert effective > 0
    result['minimum_outer_effective_admittance_10000_cases'] = smallest
    # Continuous-time local linearization at steady sliding, frozen angle.
    # This is NOT the monodromy/Floquet analysis of an orbiting contact.
    onset = []
    for bowl in profiles['bowls']:
        for mallet in profiles['mallets']:
            U = 2*math.pi*bowl['effectiveRimRadiusMeters']*.4
            a = slope(U, 2.5, mallet)
            max_rates = []
            for theta in np.linspace(0, 2*math.pi, 32, endpoint=False):
                co, bt = bank(bowl, mallet, 261.625565, theta, h)
                _, _, sig, om, _, D = co
                W = np.diag(om)
                A = np.block([[np.zeros_like(W), W],
                              [-W, -2*np.diag(sig)-a*np.outer(bt, bt)]])
                max_rates.append(float(np.max(np.linalg.eigvals(A).real)))
            co, bt = bank(bowl, mallet, 261.625565, 0., h)
            D = co[-1]
            Ytt = float(np.sum(bt*bt*h/(2*D)))
            onset.append({'bowl': bowl['stableId'], 'mallet': mallet['stableId'],
                          'speed_m_per_s': U, 'sliding_force_slope_Ns_per_m': a,
                          'frozen_angle_max_growth_rate_range_per_s': [min(max_rates), max(max_rates)],
                          'Ytt_times_zero_slip_slope_at_15N': Ytt*slope(0., 15., mallet)})
    result['local_sliding_diagnostics_32_angles_each'] = onset
    # Rotational covariance in a truly degenerate pair; changing orientation
    # rotates BOTH the state coordinates and port vectors.
    err = 0.
    for _ in range(1000):
        angle = rng.uniform(-math.pi, math.pi)
        R = np.array([[math.cos(angle), -math.sin(angle)],
                      [math.sin(angle), math.cos(angle)]])
        y, b = rng.normal(size=(2, 2))
        err = max(err, abs(float(b@y-(R@b)@(R@y))))
    assert err < 1e-12
    result['max_pair_rotation_port_error_1000_cases'] = err
    # Finite passive trajectory with U=0, not just independent one-step states.
    bowl, mallet = profiles['bowls'][0], profiles['mallets'][1]
    co, bt = bank(bowl, mallet, 261.625565, .3, h)
    _, _, sig, om, aa, D = co
    x, y = rng.normal(0., .01, size=(2, 14))
    initial = .5*float(x@x+y@y)
    max_delta, ledger_error = -math.inf, 0.
    Y = float(np.sum(bt*bt*h/(2*D)))
    for _ in range(8192):
        e0 = .5*float(x@x+y@y)
        free = (y-aa*x)/D
        F = friction_root(-float(bt@free), Y, 2.5)
        mid = free+h/(2*D)*bt*F
        x, y = x+h*om*mid, 2*mid-y
        e1 = .5*float(x@x+y@y)
        loss = 2*h*float(sig@(mid*mid))-h*F*float(bt@mid)
        max_delta = max(max_delta, e1-e0)
        ledger_error = max(ledger_error, abs(e1-e0+loss))
    assert max_delta <= 1e-14 and ledger_error < 1e-12
    result['stationary_contact_8192_steps'] = {
        'duration_s': 8192*h, 'initial_energy_J': initial, 'final_energy_J': e1,
        'largest_energy_increment_J': max_delta, 'max_ledger_error_J': ledger_error}
    # Show why material changes cannot silently mutate a compressed spring.
    d, p = .0002, 1.5
    result['unaccounted_spring_energy_if_suede_changes_to_wood_J'] = (
        potential(d, 2e8, p)-potential(d, 1e7, p))
    result['status'] = 'PASS'
    return result


if __name__ == '__main__':
    results = main()
    text = json.dumps(results, indent=2, allow_nan=False)+'\n'
    Path(__file__).with_name('review_check_results.json').write_text(text, encoding='utf-8')
    print(text, end='')
