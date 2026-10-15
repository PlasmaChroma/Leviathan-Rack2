#include "vessel/ModalBank.hpp"
#include "vessel/SeedProfiles.hpp"
#include "vessel/FrictionContact.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace vessel;
void require(bool okay, const char* message) { if (!okay) throw std::runtime_error(message); }
void orbitGeometry() {
    ContactOrbit cached;
    for (unsigned i=0; i<1000; ++i) {
        auto bowl=seedBowls[(i/20)%2];
        if (i%7==0) --bowl.pairCount;
        for (std::size_t n=0; n<bowl.pairCount; ++n) {
            bowl.pairs[n].order += (i/30)%2;
            bowl.pairs[n].massA *= 1.+.01*(i%3);
            bowl.pairs[n].orientation += .01*(i%5);
        }
        const double width=(i/10)%3*.04, angle=i*.003;
        ModalBank bank;
        require(bank.configure(bowl,200.+i%100,1.,1.,192000.), "orbit bank setup");
        for (std::size_t n=0; n<bowl.pairCount; ++n) {
            const double half=.5*bowl.pairs[n].order*width;
            const double patch=std::abs(half)<1e-8 ? 1.-half*half/6. : std::sin(half)/half;
            require(cached.patchFactor(n,bowl.pairs[n].order,width)==patch, "footprint key mismatch");
        }
        ContactOrbit fresh;
        cached.configure(bowl,bank,width,angle); fresh.configure(bowl,bank,width,angle);
        for (unsigned tick=0; tick<9; ++tick) {
            ModalVector a,b,c,d;
            const double increment=tick%2 ? .00003 : -.005;
            cached.midpoint(increment,a,b,true); fresh.midpoint(increment,c,d,true);
            require(a==c && b==d, "cached footprint changes resynchronized orbit");
            cached.finish(); fresh.finish();
        }
    }
    std::cout << "[PASS] 1000 exact footprint/orbit comparisons across width, order, mass, angle and topology changes\n";
}
int main() { try { orbitGeometry(); }
catch (const std::exception& e) { std::cerr << e.what() << "\n"; return 1; } }
