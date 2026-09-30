#!/usr/bin/env python3
"""Independent algebra checks for the Vessel draft, not a synthesizer or benchmark.

Requires Python 3.10+ and NumPy. Run: python vessel_numerical_checks.py
Checks double-precision modal pole placement, collocated energy balance,
friction root residual/passivity, and isolated/coupled nonlinear striker energy balance.
It does NOT validate sound quality, material calibration, aliasing, or Rack CPU use.
"""
from __future__ import annotations
import json
import math
from pathlib import Path
import numpy as np


def coefficients(f: np.ndarray, t60: np.ndarray, h: float):
    """Inverse-bilinear pole mapping. f is damped digital frequency in Hz."""
    r = np.exp(-math.log(1000.0) * h / t60)
    theta = 2.0 * math.pi * f * h
    denominator = 1.0 + 2.0 * r * np.cos(theta) + r * r
    # expm1 avoids cancellation in 1-r^2 for long decays.
    sigma = (2.0 / h) * (-np.expm1(-2.0 * math.log(1000.0) * h / t60)) / denominator
    nu = (4.0 / h) * r * np.sin(theta) / denominator
    omega = np.hypot(sigma, nu)
    a = 0.5 * h * omega
    D = 1.0 + h * sigma + a * a
    return r, theta, sigma, omega, a, D


def friction(s: float, normal: float, mu_s: float, mu_k: float,
             v_s: float, epsilon: float) -> float:
    return normal * (mu_k + (mu_s - mu_k) * math.exp(-(s / v_s)**2)) * math.tanh(s / epsilon)


def friction_root(delta: float, Y: float, normal: float,
                  mu_s=0.65, mu_k=0.30, v_s=0.20, epsilon=1e-4):
    """Slow high-accuracy oracle; production should use safeguarded Newton."""
    if normal == 0.0:
        return 0.0
    lo, hi = -normal * mu_s, normal * mu_s
    for _ in range(64):
        F = 0.5 * (lo + hi)
        residual = F - friction(delta - Y * F, normal, mu_s, mu_k, v_s, epsilon)
        if residual > 0:
            hi = F
        else:
            lo = F
    return 0.5 * (lo + hi)


def potential(delta: float, k: float, p: float) -> float:
    return k * max(delta, 0.0)**(p + 1.0) / (p + 1.0)


def discrete_gradient(d0: float, d1: float, k: float, p: float) -> float:
    if abs(d1 - d0) <= 1e-12 * max(1.0, abs(d0), abs(d1)):
        return k * max(0.5 * (d0 + d1), 0.0)**p
    return (potential(d1, k, p) - potential(d0, k, p)) / (d1 - d0)


def main() -> dict:
    rng = np.random.default_rng(671093)
    results: dict = {"scope": "algebra checks only; not audio validation or profiling", "seed": 671093}
    max_pole_error = 0.0
    for fs in (44100.0, 48000.0, 96000.0, 192000.0):
        R = 1
        while R * fs < 176400.0:
            R *= 2
        h = 1.0 / (R * fs)
        for f in (20.0, 55.0, 261.625565, 1000.0, 8000.0, 18000.0):
            for t in (0.05, 0.5, 5.0, 60.0):
                r, theta, sigma, omega, a, D = coefficients(np.array([f]), np.array([t]), h)
                aa, DD = float(a[0]), float(D[0])
                A = np.array([[1 - 2 * aa * aa / DD, 2 * aa / DD], [-2 * aa / DD, 2 / DD - 1]])
                eig = np.linalg.eigvals(A)
                expected = complex(r[0] * np.cos(theta[0]), r[0] * np.sin(theta[0]))
                max_pole_error = max(max_pole_error, min(abs(eig - expected)))
    assert max_pole_error < 1e-12, max_pole_error
    results["max_complex_pole_error_192_cases"] = max_pole_error

    # Random 14-coordinate, 3-port one-step systems.
    n = 10000
    h = 1.0 / 192000.0
    f = rng.uniform(30.0, 18000.0, (n, 14))
    t60 = rng.uniform(0.2, 60.0, (n, 14))
    _, _, sigma, omega, a, D = coefficients(f, t60, h)
    x = rng.normal(size=(n, 14))
    y = rng.normal(size=(n, 14))
    b = rng.normal(size=(n, 14, 3))
    F = rng.normal(size=(n, 3))
    modal_force = np.einsum('nij,nj->ni', b, F)
    ybar = (y - a * x + 0.5 * h * modal_force) / D
    x1, y1 = x + h * omega * ybar, 2.0 * ybar - y
    E0 = 0.5 * np.sum(x*x + y*y, axis=1)
    E1 = 0.5 * np.sum(x1*x1 + y1*y1, axis=1)
    work = h * np.sum(ybar * modal_force, axis=1)
    loss = 2.0 * h * np.sum(sigma * ybar*ybar, axis=1)
    balance_error = float(np.max(np.abs(E1 - E0 - work + loss)))
    assert balance_error < 1e-12, balance_error
    results["max_modal_energy_balance_error_10000_cases"] = balance_error

    ybar0 = (y - a*x)/D
    xx = x + h*omega*ybar0
    yy = 2*ybar0-y
    passive_delta = 0.5*np.sum(xx*xx+yy*yy, axis=1)-E0
    assert float(np.max(passive_delta)) <= 1e-12
    results["largest_unforced_energy_change_10000_cases"] = float(np.max(passive_delta))

    max_friction_residual = 0.0
    min_dissipation = 0.0
    max_uniqueness_number = 0.0
    for _ in range(3000):
        delta = rng.uniform(-2.0, 2.0)
        Y = rng.uniform(1e-7, 1e-3)
        N = rng.uniform(0.0, 15.0)
        Ff = friction_root(delta, Y, N)
        s = delta - Y * Ff
        max_friction_residual = max(max_friction_residual, abs(Ff - friction(s, N, 0.65, 0.30, 0.20, 1e-4)))
        min_dissipation = min(min_dissipation, Ff * s)
        max_uniqueness_number = max(max_uniqueness_number, Y*N*math.sqrt(2/math.e)*(0.65-0.30)/0.20)
    assert max_friction_residual < 1e-9, max_friction_residual
    assert min_dissipation >= -1e-12, min_dissipation
    assert max_uniqueness_number < 0.9
    results["max_friction_force_residual_3000_cases_N"] = max_friction_residual
    results["minimum_friction_dissipation_F_times_slip_W"] = min_dissipation
    results["maximum_tested_uniqueness_number"] = max_uniqueness_number

    # An isolated impact coupled to a modal bank. No rubbing in this test.
    max_collision_balance_error = 0.0
    for _ in range(200):
        f = np.array([310.2,312.1,828.1,828.8,1503.4,1506.7])
        _,_,sig,om,aa,DD = coefficients(f, np.full(6,15.0), h)
        xx0=rng.normal(0,0.01,6); yy0=rng.normal(0,0.01,6)
        bb=rng.normal(0,2.0,6)
        mm=0.045; vm=float(rng.uniform(-0.5,1.0)); d0=float(rng.uniform(0,0.0002))
        k=1e7; pp=1.5; cc=1.0
        yfree=(yy0-aa*xx0)/DD
        vfree=float(bb@yfree)
        Yss=float(np.sum(bb*bb*h/(2*DD)))
        Ytot=Yss+h/(2*mm)
        def trial(Fs: float):
            d1=d0+h*(vm-vfree-Ytot*Fs)
            vdelta=(d1-d0)/h
            visc=cc*max(vdelta,0.0) if max(d0,d1)>0 else 0.0
            return Fs-discrete_gradient(d0,d1,k,pp)-visc,d1,visc,vdelta
        lo=0.0; hi=1.0
        for _ in range(30):
            if trial(hi)[0]>=0: break
            hi*=2
        else: raise AssertionError("collision bracket failure")
        for _ in range(70):
            mid=0.5*(lo+hi)
            if trial(mid)[0]>0: hi=mid
            else: lo=mid
        Fs=0.5*(lo+hi)
        _,d1,visc,vdelta=trial(Fs)
        ymid=yfree+(h/(2*DD))*bb*Fs
        xx1=xx0+h*om*ymid; yy1=2*ymid-yy0
        vm1=vm-h*Fs/mm
        total0=0.5*np.sum(xx0*xx0+yy0*yy0)+0.5*mm*vm*vm+potential(d0,k,pp)
        total1=0.5*np.sum(xx1*xx1+yy1*yy1)+0.5*mm*vm1*vm1+potential(d1,k,pp)
        expected_loss=2*h*float(np.sum(sig*ymid*ymid))+h*visc*vdelta
        max_collision_balance_error=max(max_collision_balance_error,abs(float(total1-total0+expected_loss)))
    assert max_collision_balance_error < 1e-10, max_collision_balance_error
    results["max_isolated_impact_energy_balance_error_200_cases_J"] = max_collision_balance_error
    # Simultaneous normal strike and prescribed-load rubbing, one-step oracle.
    max_coupled_balance_error = 0.0
    max_coupled_force_residual = 0.0
    for _ in range(200):
        ff=np.array([310.2,312.1,828.1,828.8,1503.4,1506.7])
        _,_,sig,om,aa,DD=coefficients(ff,np.full(6,15.0),h)
        xx0=rng.normal(0,0.01,6); yy0=rng.normal(0,0.01,6)
        bs=rng.normal(0,2.0,6); bt=rng.normal(0,1.0,6)
        mm=.045; vm=float(rng.uniform(-.5,1)); d0=float(rng.uniform(0,.0002))
        k=1e7; pp=1.5; cc=1.0
        N=float(rng.uniform(0,15)); U=float(rng.uniform(-.8,.8))
        yfree=(yy0-aa*xx0)/DD
        vsfree=float(bs@yfree); vtfree=float(bt@yfree)
        Yss=float(np.sum(bs*bs*h/(2*DD)))
        Ytt=float(np.sum(bt*bt*h/(2*DD)))
        Yst=float(np.sum(bs*bt*h/(2*DD)))
        Ytot=Yss+h/(2*mm)
        assert Ytt*N*math.sqrt(2/math.e)*(.65-.30)/.20 < .9
        def coupled_trial(Fs: float):
            Ft=friction_root(U-vtfree-Yst*Fs,Ytt,N)
            d1=d0+h*(vm-vsfree-Ytot*Fs-Yst*Ft)
            vd=(d1-d0)/h
            visc=cc*max(vd,0.0) if max(d0,d1)>0 else 0.0
            return Fs-discrete_gradient(d0,d1,k,pp)-visc,d1,visc,vd,Ft
        lo=0.0; hi=1.0
        for _ in range(24):
            if coupled_trial(hi)[0]>=0: break
            hi*=2
        else: raise AssertionError("coupled collision bracket failure")
        for _ in range(70):
            mid=.5*(lo+hi)
            if coupled_trial(mid)[0]>0: hi=mid
            else: lo=mid
        Fs=.5*(lo+hi)
        residual,d1,visc,vd,Ft=coupled_trial(Fs)
        max_coupled_force_residual=max(max_coupled_force_residual,abs(residual))
        ymid=yfree+(h/(2*DD))*(bs*Fs+bt*Ft)
        xx1=xx0+h*om*ymid; yy1=2*ymid-yy0
        vm1=vm-h*Fs/mm
        total0=.5*np.sum(xx0*xx0+yy0*yy0)+.5*mm*vm*vm+potential(d0,k,pp)
        total1=.5*np.sum(xx1*xx1+yy1*yy1)+.5*mm*vm1*vm1+potential(d1,k,pp)
        slip=U-float(bt@ymid)
        work=h*Ft*U
        losses=h*Ft*slip+2*h*float(np.sum(sig*ymid*ymid))+h*visc*vd
        max_coupled_balance_error=max(max_coupled_balance_error,abs(float(total1-total0-work+losses)))
    assert max_coupled_balance_error < 1e-10, max_coupled_balance_error
    assert max_coupled_force_residual < 1e-7, max_coupled_force_residual
    results["max_coupled_contact_energy_balance_error_200_cases_J"] = max_coupled_balance_error
    results["max_coupled_strike_force_residual_200_cases_N"] = max_coupled_force_residual

    # Optional conservative check on the supplied prototype descriptor set.
    profile_path=Path(__file__).with_name("vessel_seed_profiles.json")
    if profile_path.exists():
        profiles=json.loads(profile_path.read_text(encoding="utf-8"))
        max_bound=0.0
        hmax=1.0/176400.0
        for bowl in profiles["bowls"]:
            for mallet in profiles["mallets"]:
                w=mallet["fullAngularPatchWidthRadians"]
                Ybound=0.0
                for pair in bowl["pairs"]:
                    n=pair["azimuthalOrder"]; arg=n*w/2
                    patch=math.sin(arg)/arg if arg else 1.0
                    Ybound+=(hmax/2)*patch**2/(n*n*min(pair["massAKg"],pair["massBKg"]))
                L=15*math.sqrt(2/math.e)*(mallet["muS"]-mallet["muK"])/mallet["weakeningVelocityMetersPerSecond"]
                max_bound=max(max_bound,Ybound*L)
        assert max_bound<.9, max_bound
        results["max_conservative_seed_profile_uniqueness_bound"] = max_bound
    results["status"] = "PASS"
    return results

if __name__ == '__main__':
    result = main()
    text = json.dumps(result, indent=2)
    print(text)
    Path(__file__).with_name('numerical_check_results.json').write_text(text+'\n', encoding='utf-8')
