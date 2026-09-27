#include "../reconstruction/audited_dsp_components.hpp"
#include "../reconstruction/exact_tables.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
int main() {
    using namespace mp86_audit;
    const std::array<float,8> x{{1,2,3,4,5,6,7,8}};
    const auto twice=hadamard8(hadamard8(x));
    for (std::size_t i=0;i<8;++i) assert(twice[i]==8*x[i]);
    AllpassAverage f;
    assert(f.process(1.0f,0.0f)==0.5f);
    assert(f.process(0.0f,0.0f)==0.5f);
    assert(f.process(0.0f,0.0f)==0.0f);
    assert(std::abs(clipped_cubic(0.3f)+clipped_cubic(-0.3f))<1e-7f);
    assert(mp86_tables::exp2_fraction.size()==2048);
    assert(mp86_tables::zone_minimum_samples[0]>61.22f);
    assert(mp86_tables::repeats_gain[127]>1.26f);
    std::cout << "PASS: Hadamard identity, component sanity, exact-table header.\n";
}
