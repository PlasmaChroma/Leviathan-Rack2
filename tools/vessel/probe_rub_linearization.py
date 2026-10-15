"""Frozen-angle, continuous-time onset screen; NOT a playable contact solver.

Compare the existing prescribed-load Jacobian with a hypothetical compliant
normal follower. No opening/recontact or moving-geometry work is integrated here.
Positive growth is only local evidence, not a prediction of sustained singing.
Requires NumPy. Run from any directory; CSV is written to stdout.
"""
import csv
import json
import math
from pathlib import Path
import sys

import numpy as np

seeds = json.loads((Path(__file__).resolve().parents[2] /
                    "doc/Vessel/vessel_seed_profiles.json").read_text())
bowl, mallet = seeds["bowls"][0], seeds["mallets"][1]


def jacobian(speed, angle, stiffness=0., damping=0., slope_enabled=True):
    omega, sigma, normal, tangent = [], [], [], []
    for pair in bowl["pairs"]:
        n = pair["azimuthalOrder"]
        beta = n * (angle - pair["orientationRadians"])
        half = .5*n*mallet["fullAngularPatchWidthRadians"]
        patch = math.sin(half)/half
        for side, sign in (("A", -1), ("B", 1)):
            mass = pair[f"mass{side}Kg"]
            omega.append(2*math.pi*261.625565*pair["centerRatio"] *
                         2**(sign*pair["splitCents"]/2400))
            sigma.append(math.log(1000)/pair[f"t60{side}Seconds"])
            normal.append(-patch*(math.cos(beta) if side == "A" else math.sin(beta))/math.sqrt(mass))
            tangent.append(patch*(-math.sin(beta) if side == "A" else math.cos(beta))/(n*math.sqrt(mass)))
    w, s, c, t = map(np.array, (omega, sigma, normal, tangent))
    U = 2*math.pi*bowl["effectiveRimRadiusMeters"]*speed
    vc = mallet["weakeningVelocityMetersPerSecond"]
    weakening = math.exp(-(U/vc)**2)
    mu = mallet["muK"]+(mallet["muS"]-mallet["muK"])*weakening
    # All sampled slips are far beyond the tanh regularization region.
    slope = -2*2.5*(mallet["muS"]-mallet["muK"])*U/vc**2*weakening if slope_enabled else 0.
    count = len(w)
    dim = count + bool(stiffness)
    K = np.zeros((dim, dim))
    D = np.zeros_like(K)
    K[:count, :count] = -np.diag(w*w)
    D[:count, :count] = -np.diag(2*s) - slope*np.outer(t, t)
    if stiffness:
        # delta = z - c.q; Fn = Kc*delta + Cc*delta_dot.
        # Bowl force = (c + mu*t)*Fn - Phi'(U)*t*(t.v).
        # Follower: m*zdd = -Fn - Kh*z - Ch*zdot (perturbations).
        gap = np.r_[-c, 1.]
        response = np.r_[c+mu*t, -1/mallet["massKg"]]
        K += stiffness*np.outer(response, gap)
        D += damping*np.outer(response, gap)
        K[-1, -1] -= 1000/mallet["massKg"]
        D[-1, -1] -= 2/mallet["massKg"]
    return np.block([[np.zeros_like(K), np.eye(dim)], [K, D]])


writer = csv.writer(sys.stdout, lineterminator="\n")
writer.writerow(["speed_rps", "normal_K_Npm", "normal_C_Nspm", "friction_slope_enabled",
                 "min_local_growth_per_s", "max_local_growth_per_s", "angles_positive", "angle_count"])
for speed in (.4, .8, 2.):
    for stiffness, damping in ((0.,0.), (1e4,1.5), (1e5,1.5), (1e6,50.)):
        for slope_enabled in (True, False):
            growth = [max(np.linalg.eigvals(jacobian(speed, angle, stiffness, damping, slope_enabled)).real)
                      for angle in np.linspace(0, math.pi, 24, endpoint=False)]
            writer.writerow([speed, stiffness, damping, int(slope_enabled), min(growth), max(growth),
                             sum(g > 0 for g in growth), len(growth)])
