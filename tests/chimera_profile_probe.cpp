// Line protocol used by tools/chimera_phase1_vectors.py; no JSON dependency
// enters the DSP layer. Each line is one independent profile evaluation.
#include "ChimeraProfile.hpp"
#include <iomanip>
#include <iostream>
#include <string>

int main() {
    using namespace chimera;
    using namespace chimera::profile1;
    std::cout << std::setprecision(17);
    std::string op;
    while (std::cin >> op) {
        if (op == "classic") {
            double k, a, v; std::cin >> k >> a >> v;
            std::cout << classicRate(k, a, v) << '\n';
        }
        else if (op == "pitch") {
            int mode; double k, a, v; std::cin >> mode >> k >> a >> v;
            std::cout << pitchRate(mode, k, a, v) << '\n';
        }
        else if (op == "sos") {
            double k, v; int patched; std::cin >> k >> patched >> v;
            std::cout << sos(k, patched != 0, v) << '\n';
        }
        else if (op == "gene") {
            std::uint32_t length; double u; std::cin >> length >> u;
            std::cout << finiteGeneFrames(length, u) << '\n';
        }
        else if (op == "density") {
            double m; std::cin >> m;
            const double d = morphDensity(m);
            std::cout << d << ' ' << 480.0 / d << '\n';
        }
        else if (op == "cubic") {
            double a,b,c,d,t; std::cin >> a >> b >> c >> d >> t;
            std::cout << cubic(a,b,c,d,t) << '\n';
        }
        else if (op == "window") {
            std::uint32_t n, age; int smooth; std::cin >> n >> smooth >> age;
            std::cout << windowEdge(n, smooth != 0) << ' '
                      << window(n, age, smooth != 0) << '\n';
        }
        else if (op == "unity") {
            double density, base; int smooth; std::cin >> density >> base >> smooth;
            std::cout << unityBlend(density, smooth != 0) << ' '
                      << effectiveWindow(base, density, smooth != 0) << '\n';
        }
        else if (op == "prng") {
            std::uint32_t seed; std::cin >> seed;
            Xorshift32 random(seed);
            const std::uint32_t next = random.next();
            std::cout << next << ' ' << static_cast<double>(next >> 8) / 16777216.0 << '\n';
        }
        else if (op == "pan") {
            double p, l, r; std::cin >> p; stereoBalance(p, l, r);
            std::cout << l << ' ' << r << '\n';
        }
        else if (op == "pole") {
            double tau; std::cin >> tau; OnePole pole; pole.setTau(tau);
            std::cout << pole.alpha << '\n';
        }
        else return 2;
        if (!std::cin || !std::cout) return 3;
    }
}
