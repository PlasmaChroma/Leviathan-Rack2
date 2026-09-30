#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
inline std::int64_t signed32(std::uint64_t bits) noexcept {
    const std::uint64_t u = bits & 0xffffffffULL;
    return (u < 0x80000000ULL)
        ? static_cast<std::int64_t>(u)
        : static_cast<std::int64_t>(u) - 0x100000000LL;
}

inline std::int64_t wrapSigned32(std::int64_t value) noexcept {
    return signed32(static_cast<std::uint64_t>(value));
}

inline std::uint32_t ratioHalfPeriod(int r, std::uint32_t masterH) noexcept {
    std::int64_t h = wrapSigned32(masterH);
    if (r > 0)
        h = wrapSigned32(h * 4) / (r + 4);
    else if (r < 0)
        h = wrapSigned32(h * (4 - r)) / 4;
    if (h < 200) return 200;
    if (h > 0x00ffffff) return 0x00ffffff;
    return static_cast<std::uint32_t>(h);
}

inline std::int64_t phaseOffset(int r, std::uint8_t p,
                                std::uint32_t masterH) noexcept {
    const std::int64_t base = (r < 0)
        ? wrapSigned32(masterH)
        : static_cast<std::int64_t>(ratioHalfPeriod(r, masterH));
    return wrapSigned32(base * static_cast<std::int64_t>(p) * 2) / 4;
}

inline std::uint8_t nextRandom(std::uint64_t& state) noexcept {
    state = state * 0x5851F42D4C957F2DULL + 1ULL;
    return static_cast<std::uint8_t>((state >> 49) & 0xffULL);
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> cells;
    std::stringstream stream(line); std::string cell;
    while (std::getline(stream, cell, ',')) cells.push_back(cell);
    return cells;
}
int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::runtime_error("Expected fixture directory");
        const std::string root=argv[1];
        std::size_t ratioCases=0, phaseCases=0, randomCases=0;
        for (const std::string name : {"ratio_test_vectors.csv", "phase_test_vectors.csv", "random_test_vectors.csv"}) {
            std::ifstream file(root+"/"+name);
            if (!file) throw std::runtime_error("Cannot open " + name);
            std::string line; std::getline(file,line);
            while (std::getline(file,line)) {
                const auto c=split(line);
                if (name=="ratio_test_vectors.csv") {
                    const auto result=ratioHalfPeriod(std::stoi(c.at(0)), static_cast<std::uint32_t>(std::stoull(c.at(1))));
                    if (result!=std::stoull(c.at(2))) throw std::runtime_error("Ratio mismatch");
                    ++ratioCases;
                } else if (name=="phase_test_vectors.csv") {
                    const auto result=phaseOffset(std::stoi(c.at(0)), static_cast<std::uint8_t>(std::stoul(c.at(1))), static_cast<std::uint32_t>(std::stoull(c.at(2))));
                    if (result!=std::stoll(c.at(3))) throw std::runtime_error("Phase mismatch");
                    ++phaseCases;
                } else {
                    std::uint64_t state=std::stoull(c.at(0),nullptr,0);
                    const auto result=nextRandom(state);
                    if (state!=std::stoull(c.at(1),nullptr,0) || result!=std::stoul(c.at(2))) throw std::runtime_error("PRNG mismatch");
                    ++randomCases;
                }
            }
        }
        std::cout<<"PASS: extracted specification C++ helpers, "<<ratioCases<<" ratio rows, "<<phaseCases<<" phase rows, "<<randomCases<<" PRNG rows; UBSan enabled.\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr<<ex.what()<<"\n"; return 1;
    }
}
