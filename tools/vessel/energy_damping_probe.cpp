// Offline A/B probe of the optional high-energy loss; not physical calibration.
// Build with the same strict-math flags and vessel sources as vessel_engine_spec.
#include "vessel/DualBowlAdapter.hpp"
#include "vessel/SeedProfiles.hpp"
#include "vessel/RubIntensity.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
using namespace vessel;

int main(int argc, char**) {
    try {
        std::cout << std::setprecision(8) << "case,bowl,mallet,governor,peak_mJ,end_drive_mJ,end_tail_mJ,clipped_samples\n";
        for (const auto& bowl : seedBowls) for (std::size_t m = 0; m < seedMalletCount; ++m)
            for (int scenario = argc > 1 ? 5 : 0; scenario < 6; ++scenario) for (bool governed : {false, true}) {
                DualBowlAdapter adapter;
                EngineSettings settings; settings.decayMultiplier = 4;
                if (!adapter.configure(bowl, seedMallets[m], settings, 0, 48000, ProcessingQuality::Balanced))
                    throw std::runtime_error("setup");
                HostControls c;
                c.velocity = 1; c.strikeVelocityScale = m == 3 ? 2 : 1;
                c.speed = scenario == 2 ? 2 : scenario == 3 ? .6 : .4;
                c.pressure = scenario >= 2 ? 15 : 2.5;
                const int drive = scenario == 0 ? 2400 : scenario == 5 ? 20*48000 : 12*48000;
                RubIntensityPlayer player;
                const double maximumSpeed = rubIntensityMaximumSpeed(bowl, seedMallets[m], m == 0 ? 2 : 1.2);
                const double startSpeed = rubIntensityMaximumSpeed(bowl, seedMallets[m], .8);
                double peak = 0, endDrive = 0, endTail = 0;
                unsigned clipped = 0;
                for (int i = 0; i < drive+5*48000; ++i) {
                    if (governed && i%48 == 0) adapter.updateHighEnergyDamping();
                    c.strikeEvent = (scenario == 0 && i == 0) || (scenario == 4 && i < drive && i%4800 == 0);
                    c.rotate = scenario > 0 && scenario != 4 && i < drive;
                    if (scenario == 5) {
                        const auto gesture = player.process(1, c.rotate, endTail, maximumSpeed, startSpeed, 1./48000);
                        c.speed = gesture.speed; c.pressure = gesture.pressure;
                    }
                    const auto f = adapter.process(c);
                    if (f.fault) throw std::runtime_error("trajectory fault");
                    peak = std::max(peak, f.bowlEnergy);
                    if (i == drive-1) endDrive = f.bowlEnergy;
                    endTail = f.bowlEnergy;
                    if (std::max(std::abs(f.audio.left), std::abs(f.audio.right))*16 >= 20) ++clipped;
                }
                const char* names[] = {"strike", "rub_default", "rub_max", "rub_strong", "strike_10Hz", "intensity_full"};
                std::cout << names[scenario] << ','
                    << bowl.stableId << ',' << seedMallets[m].stableId << ',' << governed << ','
                    << peak*1000 << ',' << endDrive*1000 << ',' << endTail*1000 << ',' << clipped << '\n';
            }
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
