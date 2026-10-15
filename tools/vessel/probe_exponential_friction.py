"""Make an offline exponential-law variant of the current contact implementation.

Usage: python tools/vessel/probe_exponential_friction.py OUTPUT.cpp
Compile the result INSTEAD OF FrictionContact.cpp, adding -Isrc/vessel.
Retains Vessel's tanh regularization; only the weakening envelope and its
derivative/certificate change. This is not a reproduction of the paper's full
normal-contact model or near-zero regularization.
"""
from pathlib import Path
import sys

source = (Path(__file__).resolve().parents[2]/"src/vessel/FrictionContact.cpp").read_text()
changes = {
    "const double weakening = std::abs(z) < 27.0 ? std::exp(-z*z) : 0.0;":
    "const double weakening = std::exp(-std::abs(z));",
    "-2.0*z/m.weakeningVelocity*(m.muS-m.muK)*weakening":
    "(z < 0.0 ? 1.0 : -1.0)/m.weakeningVelocity*(m.muS-m.muK)*weakening",
    "load*0.8577638849607068*(m.muS-m.muK)/m.weakeningVelocity":
    "load*(m.muS-m.muK)/m.weakeningVelocity",
}
for before, after in changes.items():
    if source.count(before) != 1:
        raise RuntimeError("Contact source changed; review experiment transformation")
    source = source.replace(before, after)
Path(sys.argv[1]).write_text("// GENERATED OFFLINE EXPERIMENT; NOT PRODUCTION\n" + source)
