#define SIBYL_MODULE_TEST
#include "../src/Sibyl.cpp"
#include <iostream>
#include <cstring>
Plugin* pluginInstance = nullptr;
int main() {
    bool ok = true;
    const uint64_t expected[2][3] = {{2018724515009493274ULL,10000105836002149975ULL,10338453823389365619ULL},{782236348153378706ULL,16270975918290080511ULL,8983016345456801156ULL}};
    int mode = 0;
    for (bool evolve : {false, true}) {
        int rateIndex = 0;
        for (float rate : {44100.f, 48000.f, 96000.f}) {
            std::string body = R"({"meta":{"seed":303,"bpm":120,"swing":0.2},"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":4,"resolution":"1/16","evolution":{"velocity":0.2,"gate":0.1,"mod":0.3,"probability":DEPTH},"steps":[{"step":0,"note":"C3","gate":1.2,"mod":2},{"step":1,"degree":2,"tie":true,"glideMs":15},{"step":2,"pitchV":-0.5,"gate":0.3,"ratchets":3,"probability":0.65,"microshift":-0.2},{"step":3,"note":"G3","probability":0.5,"evolve":false}]}},"arrangement":[{"id":"s","lengthBeats":2,"repeats":2,"tracks":{"v":"p"}}]})";
            body.replace(body.find("DEPTH"), 5, evolve ? "0.2" : "0");
            auto parsed = sibyl::parseCompositionJson(body, 1);
            if (!parsed.valid) return 2;
            SibylModule module;
            module.acceptComposition(parsed.composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
            uint64_t hash = 1469598103934665603ULL;
            rack::engine::Module::ProcessArgs args;
            args.sampleRate = rate; args.sampleTime = 1.f / rate;
            for (int frame = 0; frame < int(rate * 5); ++frame) {
                args.frame = frame; module.process(args);
                for (int output = 0; output < SibylModule::NUM_OUTPUTS; ++output) {
                    float voltage = module.outputs[output].getVoltage();
                    uint32_t bits; std::memcpy(&bits, &voltage, sizeof(bits));
                    hash = (hash ^ bits) * 1099511628211ULL;
                }
            }
            std::cout << mode << " " << int(rate) << " " << hash << "\n";
            if (expected[mode][rateIndex] && hash != expected[mode][rateIndex]) ok = false;
            ++rateIndex;
        }
        ++mode;
    }
    return ok ? 0 : 1;
}
