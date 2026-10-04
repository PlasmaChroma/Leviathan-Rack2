#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../src/Vessel.hpp"
#include "../src/VTune.hpp"
#include "../src/vessel/SeedProfiles.hpp"
#include "../src/vessel/RubIntensity.hpp"
#include "../src/vessel/EnergyMeter.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <new>

static thread_local bool trapAllocations = false;
static thread_local std::size_t heapOperations = 0;
void* operator new(std::size_t size) {
    if (trapAllocations) ++heapOperations;
    if (void* p = std::malloc(size ? size : 1)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* p) noexcept { if (trapAllocations && p) ++heapOperations; std::free(p); }
void operator delete[](void* p) noexcept { ::operator delete(p); }
void operator delete(void* p, std::size_t) noexcept { ::operator delete(p); }
void operator delete[](void* p, std::size_t) noexcept { ::operator delete(p); }

bool isDragonKingDebugEnabled() { return false; }
Model* modelVessel = nullptr;
Model* modelVTune = nullptr;
// Stack-owned headless fixture; omit private application teardown.
namespace rack { Context::~Context() {} }
namespace {
void require(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
void run(Vessel& m, int samples = 1, float rate = 48000) {
    Module::ProcessArgs a; a.sampleRate = rate; a.sampleTime = 1/rate;
    for (int i = 0; i < samples; ++i) { a.frame = i; m.process(a); }
}
void cable(Vessel& m, int input, float volts, int channels = 1) { m.inputs[input].channels = channels; m.inputs[input].setVoltage(volts); }

void feltStrikeLevel() {
    for (int bowl = 0; bowl < 2; ++bowl) for (int mallet = 0; mallet < 4; ++mallet) {
        Vessel m;
        m.params[Vessel::BOWL_PARAM].setValue(float(bowl));
        m.params[Vessel::MALLET_PARAM].setValue(float(mallet));
        run(m, 6000); // Finish material morph before measuring launch work.
        cable(m, Vessel::VELOCITY_INPUT, 10);
        cable(m, Vessel::STRIKE_INPUT, 10);
        run(m);
        const double speed = mallet == 3 ? 2.0 : 1.0;
        const double expected = .5 * vessel::seedMallets[mallet].mass * speed * speed;
        require(std::abs(m.audio.engine().ledger().launchWork - expected) < 1e-10,
            "material strike boost missing or applied to a non-Felt mallet");
        run(m, 24000);
        require(m.audio.engine().ledger().solverFaults == 0, "boosted strike solver fault");
    }
    std::cout << "[PASS] Felt strike boost at full velocity, both bowls, other mallets unchanged\n";
}

void gatesAndControls() {
    Vessel m;
    m.outputs[Vessel::LEFT_OUTPUT].channels = 16; m.outputs[Vessel::RIGHT_OUTPUT].channels = 16;
    cable(m, Vessel::STRIKE_INPUT, 10, 16); m.params[Vessel::STRIKE_PARAM].setValue(1);
    run(m, 1000);
    require(m.audio.engine().ledger().strikes == 1, "initial high/manual gate must coalesce and fire once");
    require(m.outputs[Vessel::LEFT_OUTPUT].getChannels() == 1 && m.outputs[Vessel::RIGHT_OUTPUT].getChannels() == 1, "Vessel must stay monophonic");
    cable(m, Vessel::STRIKE_INPUT, 0); m.params[Vessel::STRIKE_PARAM].setValue(0); run(m);
    cable(m, Vessel::STRIKE_INPUT, 2); run(m);
    require(m.audio.engine().ledger().strikes == 2, "rising retrigger missing");
    cable(m, Vessel::STRIKE_INPUT, std::numeric_limits<float>::quiet_NaN()); run(m);
    cable(m, Vessel::VELOCITY_INPUT, 0); cable(m, Vessel::STRIKE_INPUT, 10); run(m);
    require(m.audio.engine().ledger().strikes == 2, "zero patched velocity must replace knob and skip strike");
    cable(m, Vessel::ROTATE_INPUT, 10); run(m, 1200);
    require(m.audio.engine().contactEngagement() == 1, "rotation gate does not engage contact");
    cable(m, Vessel::ROTATE_INPUT, 0); run(m, 1000);
    require(m.audio.engine().contactEngagement() == 0, "gate release does not lift contact");
    m.params[Vessel::ROTATE_PARAM].setValue(1); run(m, 1000);
    require(m.audio.engine().contactEngagement() == 1, "rotation latch not OR'd with input gate");
    require(m.audio.engine().ledger().solverFaults == 0, "normal controls fault");
    std::cout << "[PASS] Initial/held/retrigger gates, manual coalescing, velocity replacement, rotate gate/control and mono outputs\n";
}

void tuningAndMorph() {
    Vessel m; m.params[Vessel::ROTATE_PARAM].setValue(1); run(m, 2000);
    const double previous = m.audio.engine().settings().frequency;
    cable(m, Vessel::VOCT_INPUT, 1); run(m);
    require(m.audio.engine().settings().frequency < 2*previous, "pitch CV change is not smoothed");
    run(m, 2000);
    require(std::abs(m.audio.engine().settings().frequency-2*261.625565) < .001, "1 V/oct tuning");
    m.params[Vessel::FINE_PARAM].setValue(100); run(m, 2000);
    require(std::abs(m.audio.engine().settings().frequency-2*261.625565*std::exp2(100.0/1200)) < .001, "fine tuning");
    m.params[Vessel::BOWL_PARAM].setValue(1); m.params[Vessel::MALLET_PARAM].setValue(2); run(m, 48);
    const double earlyRatio = m.audio.engine().bowl().coefficients(2).frequency/m.audio.engine().settings().frequency;
    require(earlyRatio > 2.66 && earlyRatio < 2.83, "material switch is not a continuous descriptor morph");
    run(m, 6000);
    require(m.audio.engine().ledger().solverFaults == 0 && m.audio.engine().contactEngagement() == 1, "material morph clears/faults ongoing contact");
    const double energy = m.audio.engine().totalEnergy();
    run(m, 1, 44100);
    require(m.audio.hostRate() == 44100 && m.audio.engine().totalEnergy() > 0 && energy > 0, "live sample-rate change clears mechanics");
    cable(m, Vessel::VOCT_INPUT, std::numeric_limits<float>::infinity());
    cable(m, Vessel::SPEED_INPUT, std::numeric_limits<float>::quiet_NaN());
    cable(m, Vessel::PRESSURE_INPUT, std::numeric_limits<float>::infinity()); run(m, 1000, 44100);
    require(std::isfinite(m.outputs[Vessel::LEFT_OUTPUT].getVoltage()) && m.audio.engine().ledger().solverFaults == 0, "nonfinite CV safety");
    std::cout << "[PASS] Smoothed octave/fine tuning, live material morph, rate continuity and nonfinite CV\n";
}

void bodyMapTelemetry() {
    Vessel host;
    VTune tune;
    Model vesselModel;
    Model* previousModel = modelVessel;
    modelVessel = &vesselModel;
    host.model = &vesselModel;
    host.rightExpander.module = &tune;
    tune.leftExpander.module = &host;
    Module::ProcessArgs args; args.sampleRate = 48000; args.sampleTime = 1.f / 48000.f;
    host.visualFrequency.store(528.f);
    tune.process(args);
    require(tune.bodyFrequencyHz.load() == 528.f, "body map does not publish Vessel display frequency");
    host.visualFrequency.store(127.25f); // Exact heart center in the bowl's color map.
    for (int i = 0; i < 96000; ++i) tune.process(args);
    require(tune.lights[VTune::CHAKRA_HEART_LIGHT].getBrightness() > .999f,
        "chakra LED does not follow the shared bowl frequency map");
    for (int i = VTune::CHAKRA_ROOT_LIGHT; i < VTune::LIGHTS_LEN; ++i)
        if (i != VTune::CHAKRA_HEART_LIGHT)
            require(tune.lights[i].getBrightness() < .001f, "unrelated chakra LED remains lit");
    host.visualFrequency.store(std::numeric_limits<float>::quiet_NaN());
    for (int i = 0; i < 300; ++i) tune.process(args);
    require(tune.bodyFrequencyHz.load() == 0.f, "body map retained invalid frequency");
    host.visualFrequency.store(174.f);
    for (int i = 0; i < 300; ++i) tune.process(args);
    require(tune.bodyFrequencyHz.load() == 174.f, "body map failed to recover");
    tune.processBypass(args);
    require(tune.bodyFrequencyHz.load() == 0.f, "bypass retained body map telemetry");
    for (int i = VTune::CHAKRA_ROOT_LIGHT; i < VTune::LIGHTS_LEN; ++i)
        require(tune.lights[i].getBrightness() == 0.f, "bypassed chakra LED remains lit");
    tune.process(args);
    require(tune.bodyFrequencyHz.load() == 174.f, "unbypass failed to republish");
    tune.leftExpander.module = nullptr;
    Module::ExpanderChangeEvent changed; changed.side = 0;
    tune.onExpanderChange(changed);
    require(tune.bodyFrequencyHz.load() == 0.f, "detach retained body map telemetry");
    tune.bodyMapMode.store(2); tune.bodyMapOpacity.store(.85f);
    json_t* saved = tune.dataToJson();
    VTune restored; restored.dataFromJson(saved); json_decref(saved);
    require(restored.bodyMapMode.load() == 0 && restored.bodyMapOpacity.load() == .85f
        && restored.bodyFrequencyHz.load() == 0.f, "body map persistence or transient state failed");
    saved = json_object(); restored.dataFromJson(saved);
    require(restored.bodyMapMode.load() == 0 && restored.bodyMapOpacity.load() == vtune_body::kDefaultOpacity,
        "old patch body map defaults failed");
    json_object_set_new(saved, "bodyMapSchema", json_integer(99));
    restored.dataFromJson(saved); json_decref(saved);
    require(restored.bodyMapMode.load() == 3, "unknown body map schema should disable highlights");
    modelVessel = previousModel;
    std::cout << "[PASS] Body map frequency bridge, invalid values, bypass, detach and persistence\n";
}

void tuneExpander() {
    Vessel standalone;
    Vessel retiredPressure;
    cable(standalone, Vessel::ROTATE_INPUT, 10.f);
    cable(retiredPressure, Vessel::ROTATE_INPUT, 10.f);
    cable(retiredPressure, Vessel::PRESSURE_INPUT, 0.f);
    run(standalone, 96000);
    run(retiredPressure, 96000);
    require(standalone.audio.engine().rotationAngle() == retiredPressure.audio.engine().rotationAngle()
        && standalone.audio.engine().bowl().energy() == retiredPressure.audio.engine().bowl().energy(),
        "retired pressure cable still changes rubbing");
    require(std::abs(standalone.audio.engine().rotationAngle()) > .05,
        "Rub gate with no intensity or expander must rotate using defaults");
    Vessel host;
    VTune tune;
    Model vesselModel, tuneModel;
    vesselModel.slug = "Vessel";
    tuneModel.slug = "VTune";
    modelVessel = &vesselModel;
    modelVTune = &tuneModel;
    host.model = &vesselModel;
    tune.model = &tuneModel;
    host.rightExpander.module = &tune;
    tune.leftExpander.module = &host;

    tune.params[VTune::VELOCITY_PARAM].setValue(0.f);
    tune.params[VTune::SPEED_PARAM].setValue(-1.25f);
    tune.params[VTune::PRESSURE_PARAM].setValue(7.f);
    tune.params[VTune::SUSTAIN_PARAM].setValue(2.f);
    tune.params[VTune::IMPERFECTION_PARAM].setValue(.25f);
    tune.params[VTune::WIDTH_PARAM].setValue(.2f);
    tune.params[VTune::LEVEL_PARAM].setValue(0.f);
    Module::ProcessArgs args; args.sampleRate = 48000; args.sampleTime = 1.f/48000.f;
    tune.process(args);
    require(host.rightExpander.messageFlipRequested, "V.Tune did not request an expander flip");
    require(tune.lights[VTune::VESSEL_LINK_LIGHT].getBrightness() == 0.f
        && tune.lights[VTune::VESSEL_READY_LIGHT].getBrightness() == 1.f,
        "V.Tune connection LED did not show a ready Vessel link");
    host.tuneMessages[1] = host.tuneMessages[0];
    require(host.tuneMessages[1].speed == 0.f, "V.Tune speed still permits reverse rotation");
    host.rightExpander.messageFlipRequested = false;
    cable(host, Vessel::STRIKE_INPUT, 10.f);
    run(host, 100);
    require(host.lights[Vessel::VTUNE_LINK_LIGHT].getBrightness() == 0.f
        && host.lights[Vessel::VTUNE_READY_LIGHT].getBrightness() == 1.f,
        "Vessel connection LED did not show a ready V.Tune link");
    const auto& settings = host.audio.engine().settings();
    require(host.audio.engine().ledger().strikes == 0, "V.Tune velocity does not replace Vessel fallback knob");
    require(std::abs(settings.decayMultiplier - 4.0) < 1e-12, "V.Tune sustain was not applied");
    require(std::abs(settings.imperfection - .25) < 1e-12, "V.Tune imperfection was not applied");
    require(std::abs(settings.observerSeparation - vessel::pi/30.0) < 1e-7, "V.Tune width was not applied");
    require(host.params[Vessel::LEVEL_PARAM].getValue() == 1.f,
        "retired V.Tune level overrides Vessel level");
    cable(host,Vessel::ROTATE_INPUT,10.f);
    cable(host,Vessel::INTENSITY_INPUT,5.f);
    run(host,96000); // Allow the gradual contact approach to produce visible travel.
    require(std::abs(host.audio.engine().rotationAngle()) > .05,
        "intensity CV does not override V.Tune's zero speed");
    host.params[Vessel::LEVEL_PARAM].setValue(0.f);
    run(host, 100);
    require(host.outputs[Vessel::LEFT_OUTPUT].getVoltage() == 0.f
        && host.outputs[Vessel::RIGHT_OUTPUT].getVoltage() == 0.f
        && host.audio.engine().bowl().energy() > 0,
        "Vessel level must mute output without stopping the connected model");
    host.params[Vessel::LEVEL_PARAM].setValue(1.5f);
    run(host, 100);
    require(host.params[Vessel::LEVEL_PARAM].getValue() == 1.5f,
        "V.Tune overwrites Vessel level adjustment");
    host.inputs[Vessel::INTENSITY_INPUT].channels = 0;
    run(host,24000);
    const double restoredAngle = host.audio.engine().rotationAngle();
    run(host,1200);
    require(std::abs(host.audio.engine().rotationAngle()-restoredAngle) < 1e-8,
        "unplugging intensity does not restore V.Tune speed");

    tune.params[VTune::SPEED_PARAM].setValue(.6f);
    tune.params[VTune::LEVEL_PARAM].setValue(1.f);
    tune.process(args);
    host.tuneMessages[1] = host.tuneMessages[0];
    run(host, 48000);
    host.rightExpander.module = nullptr;
    tune.leftExpander.module = nullptr;
    const double detachedAngle = host.audio.engine().rotationAngle();
    run(host, 12000);
    require(std::abs(host.audio.engine().rotationAngle() - detachedAngle) > .05,
        "detaching V.Tune must retain gate-driven rotation with unpatched intensity");
    require(std::abs(host.params[Vessel::SPEED_PARAM].getValue() - .6f) < 1e-6
        && host.params[Vessel::PRESSURE_PARAM].getValue() == 7.f,
        "last expander speed/pressure were not retained");
    tune.process(args);
    require(host.lights[Vessel::VTUNE_LINK_LIGHT].getBrightness() == 0.f
        && host.lights[Vessel::VTUNE_READY_LIGHT].getBrightness() == 0.f
        && tune.lights[VTune::VESSEL_LINK_LIGHT].getBrightness() == 0.f
        && tune.lights[VTune::VESSEL_READY_LIGHT].getBrightness() == 0.f,
        "expander connection LEDs did not clear after disconnection");
    const auto& fallback = host.audio.engine().settings();
    require(std::abs(fallback.decayMultiplier - 4.0) < 1e-12
        && std::abs(fallback.imperfection - .25) < 1e-12
        && std::abs(fallback.observerSeparation - vessel::pi/30.0) < 1e-7,
        "Vessel did not retain V.Tune settings after disconnection");
    json_t* saved = host.paramsToJson();
    Vessel restored;
    restored.paramsFromJson(saved); json_decref(saved);
    cable(restored, Vessel::ROTATE_INPUT, 10.f);
    run(restored, 96000);
    require(std::abs(restored.params[Vessel::SPEED_PARAM].getValue() - .6f) < 1e-6
        && restored.params[Vessel::PRESSURE_PARAM].getValue() == 7.f
        && std::abs(restored.audio.engine().rotationAngle()) > .05,
        "retained V.Tune controls must survive saving without an expander");
    modelVessel = nullptr;
    modelVTune = nullptr;
    std::cout << "[PASS] V.Tune publishing, takeover and Vessel fallback restoration\n";
}

void manualPerformancePads() {
    Vessel softStrike, hardStrike;
    require(softStrike.getParamQuantity(Vessel::SPEED_PARAM)->minValue == 0.f,
        "Vessel speed knob is not unipolar");
    VTune tuneRange;
    require(tuneRange.getParamQuantity(VTune::SPEED_PARAM)->minValue == 0.f,
        "V.Tune speed knob is not unipolar");
    run(softStrike); run(hardStrike);
    cable(softStrike, Vessel::VELOCITY_INPUT, 0.f);
    cable(hardStrike, Vessel::VELOCITY_INPUT, 0.f);
    softStrike.manualStrikeVelocity.store(.25f);
    hardStrike.manualStrikeVelocity.store(1.f);
    softStrike.params[Vessel::STRIKE_PARAM].setValue(1.f);
    hardStrike.params[Vessel::STRIKE_PARAM].setValue(1.f);
    run(softStrike); run(hardStrike);
    require(softStrike.audio.engine().ledger().strikes == 1 && hardStrike.audio.engine().ledger().strikes == 1,
        "manual pad strikes were replaced by patched velocity CV");
    require(hardStrike.audio.engine().totalEnergy() > 4.0 * softStrike.audio.engine().totalEnergy(),
        "manual strike pad height does not scale launch velocity");

    Vessel slowRotate, fastRotate, gatedRotate;
    slowRotate.params[Vessel::SPEED_PARAM].setValue(0.f);
    fastRotate.params[Vessel::SPEED_PARAM].setValue(0.f);
    run(slowRotate); run(fastRotate); run(gatedRotate);
    slowRotate.manualRubIntensity.store(.25f);
    fastRotate.manualRubIntensity.store(1.f);
    gatedRotate.manualRubIntensity.store(0.f);
    slowRotate.params[Vessel::ROTATE_PARAM].setValue(1.f);
    fastRotate.params[Vessel::ROTATE_PARAM].setValue(1.f);
    cable(gatedRotate, Vessel::ROTATE_INPUT, 10.f);
    run(slowRotate, 48000); run(fastRotate, 48000); run(gatedRotate, 48000);
    const double slowAngle = std::abs(slowRotate.audio.engine().rotationAngle());
    const double fastAngle = std::abs(fastRotate.audio.engine().rotationAngle());
    require(fastAngle > slowAngle && slowAngle > 0,
        "manual rotate pad height does not scale speed");
    require(std::abs(gatedRotate.audio.engine().rotationAngle()) > 0,
        "manual speed scale incorrectly affects the external rotate gate");
    Vessel independentPad, fullRangeGate, stoppedPad;
    run(independentPad); run(fullRangeGate); run(stoppedPad);
    independentPad.params[Vessel::SPEED_PARAM].setValue(0.f);
    cable(independentPad, Vessel::SPEED_INPUT, 0.f);
    cable(independentPad, Vessel::PRESSURE_INPUT, 0.f);
    cable(independentPad, Vessel::INTENSITY_INPUT, 0.f);
    cable(independentPad, Vessel::ROTATE_INPUT, 10.f);
    independentPad.manualRubIntensity.store(1.f);
    independentPad.params[Vessel::ROTATE_PARAM].setValue(1.f);
    fullRangeGate.params[Vessel::SPEED_PARAM].setValue(float(
        vessel::rubIntensityMaximumSpeed(vessel::seedBowls[0], vessel::seedMallets[1])));
    fullRangeGate.params[Vessel::PRESSURE_PARAM].setValue(15.f);
    cable(fullRangeGate, Vessel::INTENSITY_INPUT, 10.f);
    cable(fullRangeGate, Vessel::ROTATE_INPUT, 10.f);
    stoppedPad.params[Vessel::SPEED_PARAM].setValue(2.f);
    cable(stoppedPad, Vessel::ROTATE_INPUT, 10.f);
    stoppedPad.manualRubIntensity.store(0.f);
    stoppedPad.params[Vessel::ROTATE_PARAM].setValue(1.f);
    run(independentPad, 1200); run(fullRangeGate, 1200); run(stoppedPad, 1200);
    require(std::abs(independentPad.audio.engine().rotationAngle()
        - fullRangeGate.audio.engine().rotationAngle()) < 1e-7,
        "rub pad does not override knobs, speed/pressure CV, intensity CV and gate");
    require(std::abs(independentPad.audio.engine().bowl().energy()
        - fullRangeGate.audio.engine().bowl().energy()) < 1e-8,
        "rub pad does not apply the coordinated contact pressure");
    // Let the engine's existing speed smoothing settle before checking zero.
    run(stoppedPad, 24000);
    const double stoppedAngle = stoppedPad.audio.engine().rotationAngle();
    run(stoppedPad, 1200);
    require(std::abs(stoppedPad.audio.engine().rotationAngle() - stoppedAngle) < 1e-8,
        "rub pad zero does not override knob and gate speed");
    independentPad.params[Vessel::ROTATE_PARAM].setValue(0.f);
    run(independentPad, 24000);
    const double settledAngle = independentPad.audio.engine().rotationAngle();
    run(independentPad, 1200);
    require(std::abs(independentPad.audio.engine().rotationAngle() - settledAngle) < 1e-8,
        "rub pad release does not restore gated knob/CV speed");
    const double visualPhaseError = std::abs(std::remainder(
        double(fastRotate.visualRotationAngle.load(std::memory_order_relaxed))
            - fastRotate.audio.engine().rotationAngle(), 2.0 * vessel::pi));
    require(fastRotate.visualRubbing.load(std::memory_order_relaxed)
            && visualPhaseError < .08,
        "mallet visual phase does not follow the active bowl rotation");

    // The removed Speed CV slot remains inert, even in older patch data.
    Vessel retiredSpeed, knobOnly;
    for (Vessel* v : {&retiredSpeed, &knobOnly}) {
        v->params[Vessel::SPEED_PARAM].setValue(.3f);
        cable(*v,Vessel::ROTATE_INPUT,10.f);
    }
    cable(retiredSpeed,Vessel::SPEED_INPUT,10.f);
    run(retiredSpeed,4800); run(knobOnly,4800);
    require(retiredSpeed.audio.engine().rotationAngle() == knobOnly.audio.engine().rotationAngle()
        && retiredSpeed.audio.engine().bowl().energy() == knobOnly.audio.engine().bowl().energy(),
        "retired speed input still affects direct controls");
    slowRotate.params[Vessel::ROTATE_PARAM].setValue(0.f);
    fastRotate.params[Vessel::ROTATE_PARAM].setValue(0.f);
    run(slowRotate, 2000); run(fastRotate, 2000);
    require(slowRotate.audio.engine().contactEngagement() == 0 && fastRotate.audio.engine().contactEngagement() == 0,
        "manual rotate pad remains latched after release");
    require(!fastRotate.visualRubbing.load(std::memory_order_relaxed),
        "mallet visual remains active after rub release");
    std::cout << "[PASS] Manual pad strike velocity, held rotation speed, retired speed input, gate independence and release\n";
}

void intensityOverride() {
    static_assert(Vessel::INTENSITY_INPUT == 6, "existing input IDs moved");
    require(vessel::rubIntensityEffort(0.) == 0. && vessel::rubIntensityEffort(1.) == 1.,
        "intensity effort curve changes zero/full endpoints");
    double lastEffort = 0;
    for (int i = 1; i <= 1000; ++i) {
        const double amount = i/1000.;
        const double effort = vessel::rubIntensityEffort(amount);
        require(effort > lastEffort && effort >= amount && effort <= 1.,
            "intensity effort curve loses low-end boost, monotonicity, or bounds");
        lastEffort = effort;
    }
    Vessel lowIntensity;
    lowIntensity.params[Vessel::MALLET_PARAM].setValue(0.f);
    cable(lowIntensity,Vessel::ROTATE_INPUT,10.f);
    cable(lowIntensity,Vessel::INTENSITY_INPUT,1.f);
    run(lowIntensity,20*48000);
    require(lowIntensity.rawEnergy.load() > .00001f && !lowIntensity.visualFault.load(),
        "1 V intensity cannot build wood-bowl resonance");
    Vessel easedContact, directContact;
    for (Vessel* v : {&easedContact, &directContact}) {
        v->params[Vessel::SPEED_PARAM].setValue(0.f);
        v->params[Vessel::PRESSURE_PARAM].setValue(0.f);
        run(*v);
        cable(*v,Vessel::ROTATE_INPUT,10.f);
    }
    cable(easedContact,Vessel::INTENSITY_INPUT,10.f);
    directContact.params[Vessel::SPEED_PARAM].setValue(float(
        vessel::rubIntensityMaximumSpeed(vessel::seedBowls[0],vessel::seedMallets[1])));
    directContact.params[Vessel::PRESSURE_PARAM].setValue(15.f);
    run(easedContact,4800); run(directContact,4800);
    require(easedContact.audio.engine().rotationAngle() > 0
        && easedContact.audio.engine().rotationAngle() < .9*directContact.audio.engine().rotationAngle(),
        "quiet-bowl intensity does not choose a starting speed");
    require(easedContact.audio.engine().bowl().energy()
        < .1*directContact.audio.engine().bowl().energy(),
        "intensity approach does not soften initial contact");
    for (double rate : {44100.,48000.,96000.}) {
        vessel::RubIntensityPlayer quiet, ringing;
        vessel::RubGesture cold {}, warm {};
        for (int i = 0; i < int(rate*.1); ++i) {
            cold = quiet.process(1.,true,0.,.5,.2,1/rate);
            warm = ringing.process(1.,true,.02,.5,.2,1/rate);
        }
        require(cold.speed == .2 && cold.pressure == 3., "quiet bowl waits for a timer or has no starting effort");
        require(warm.speed > 2*cold.speed && warm.pressure > 4*cold.pressure,
            "already-ringing bowl does not inform the player's gesture");
        const double steadyPressure = warm.pressure;
        for (int i = 0; i < int(rate*.2); ++i)
            warm = ringing.process(1.,true,.02+.2*i/rate,.5,.2,1/rate);
        require(warm.pressure < .95*steadyPressure, "rapid energy growth does not relax grip");
        const auto released = ringing.process(1.,false,.06,.5,.2,1/rate);
        require(released.speed == 0 && released.pressure == 0, "release retains a contact gesture");
        const auto zero = ringing.process(0.,true,.06,.5,.2,1/rate);
        require(zero.speed == 0 && zero.pressure == 0, "zero intensity retains contact effort");
    }
    Vessel cv, pad, manual, unplugged, silent;
    for (Vessel* v : {&cv, &pad, &manual, &unplugged, &silent}) run(*v);
    cable(cv, Vessel::INTENSITY_INPUT, 5.f);
    cable(cv, Vessel::SPEED_INPUT, 0.f);
    cable(cv, Vessel::PRESSURE_INPUT, 0.f);
    cv.params[Vessel::SPEED_PARAM].setValue(0.f);
    cv.params[Vessel::PRESSURE_PARAM].setValue(0.f);
    cable(cv, Vessel::ROTATE_INPUT, 10.f);
    pad.manualRubIntensity.store(.5f);
    pad.params[Vessel::ROTATE_PARAM].setValue(1.f);
    run(cv, 4800); run(pad, 4800);
    require(cv.audio.engine().rotationAngle() == pad.audio.engine().rotationAngle()
        && cv.audio.engine().bowl().energy() == pad.audio.engine().bowl().energy(),
        "intensity CV and pad do not produce identical gestures");
    cable(silent, Vessel::INTENSITY_INPUT, 10.f);
    run(silent, 1000);
    require(silent.audio.engine().contactEngagement() == 0 && silent.rawEnergy.load() == 0,
        "intensity CV incorrectly replaces the rub gate");
    // Once disconnected, saved knob/CV behavior exactly matches an unused input.
    cable(unplugged, Vessel::INTENSITY_INPUT, 10.f); run(unplugged, 100);
    unplugged.inputs[Vessel::INTENSITY_INPUT].channels = 0;
    for (Vessel* v : {&manual,&unplugged}) {
        v->params[Vessel::SPEED_PARAM].setValue(.3f);
        v->params[Vessel::PRESSURE_PARAM].setValue(5.f);
        cable(*v, Vessel::PRESSURE_INPUT, 5.f);
        cable(*v, Vessel::ROTATE_INPUT, 10.f);
        run(*v, 4800);
    }
    require(manual.audio.engine().rotationAngle() == unplugged.audio.engine().rotationAngle()
        && manual.audio.engine().bowl().energy() == unplugged.audio.engine().bowl().energy(),
        "unpatched intensity changes independent controls");
    for (float volts : {-5.f, 0.f, std::numeric_limits<float>::quiet_NaN(), 20.f}) {
        Vessel bounded, reference;
        cable(bounded,Vessel::INTENSITY_INPUT,volts);
        cable(reference,Vessel::INTENSITY_INPUT,volts > 10.f ? 10.f : 0.f);
        cable(bounded,Vessel::ROTATE_INPUT,10.f); cable(reference,Vessel::ROTATE_INPUT,10.f);
        run(bounded,4800); run(reference,4800);
        require(bounded.audio.engine().rotationAngle() == reference.audio.engine().rotationAngle()
            && bounded.audio.engine().bowl().energy() == reference.audio.engine().bowl().energy(),
            "intensity CV sanitation/clamping failed");
    }
    // Exercise hot changes and restoration with an already moving bowl.
    cv.inputs[Vessel::INTENSITY_INPUT].channels = 0;
    run(cv,24000);
    const double angle = cv.audio.engine().rotationAngle();
    run(cv,1200);
    require(std::abs(cv.audio.engine().rotationAngle()-angle) < 1e-8 && !cv.visualFault.load(),
        "live intensity unplug does not restore zero-speed CV smoothly");
    std::cout << "[PASS] Intensity/pad priority, shared gesture, clamping, gate independence and fallback\n";
}

void strikeAftermathVisual() {
    Vessel m;
    run(m, 100);
    m.params[Vessel::STRIKE_PARAM].setValue(1.f);
    run(m, 1);
    require(m.visualStrikeAftermath.load() > .99f, "manual strike does not start aftermath");
    run(m, 27000);
    require(m.visualStrikeAftermath.load() == 0.f, "held strike repeats or aftermath fails to expire");
    m.params[Vessel::STRIKE_PARAM].setValue(0.f);
    run(m, 1);
    cable(m, Vessel::STRIKE_INPUT, 10.f);
    run(m, 1);
    require(m.visualStrikeAftermath.load() > .99f, "CV strike does not start aftermath");
    m.params[Vessel::ROTATE_PARAM].setValue(1.f);
    run(m, 300);
    require(m.visualStrikeAftermath.load() == 0.f, "rub does not cancel strike aftermath");
    cable(m, Vessel::STRIKE_INPUT, 0.f);
    run(m, 1);
    cable(m, Vessel::STRIKE_INPUT, 10.f);
    run(m, 1);
    require(m.visualStrikeAftermath.load() == 0.f, "strike while rubbing starts a second mallet");
    m.params[Vessel::ROTATE_PARAM].setValue(0.f);
    run(m, 300);
    require(m.visualStrikeAftermath.load() == 0.f, "rub release revives stale strike aftermath");
    std::cout << "[PASS] Strike aftermath manual/CV trigger, expiry and rub priority\n";
}

void independentOutputAndEnergy() {
    require(vessel::energyMeterPosition(0) == 0
        && std::abs(vessel::energyMeterPosition(.02)-1.) < 1e-12
        && vessel::energyMeterPosition(.04) == 1., "energy meter endpoints changed");
    double previous = 0;
    for (double energy = 1e-14; energy < .02; energy *= 1.05) {
        const double position = vessel::energyMeterPosition(energy);
        require(position > previous, "energy meter has a dead zone or nonmonotonic toe");
        previous = position;
    }
    require(std::abs(vessel::smoothEnergyMeter(0,1,.15)
        +vessel::smoothEnergyMeter(1,0,.15)-1.) < 1e-12,
        "energy meter favors rising peaks over falling energy");
    const double oneStep = vessel::smoothEnergyMeter(0,1,.15);
    double manySteps = 0;
    for (int i = 0; i < 30; ++i) manySteps = vessel::smoothEnergyMeter(manySteps,1,.005);
    require(std::abs(oneStep-manySteps) < 1e-12, "energy meter depends on display update frequency");
    Vessel audible, silent;
    audible.params[Vessel::ROTATE_PARAM].setValue(1); silent.params[Vessel::ROTATE_PARAM].setValue(1);
    silent.params[Vessel::LEVEL_PARAM].setValue(0); silent.params[Vessel::WIDTH_PARAM].setValue(0);
    for (int i = 0; i < 96000; ++i) {
        run(audible); run(silent);
        require(audible.audio.engine().bowl().energy() == silent.audio.engine().bowl().energy(), "level/width changes mechanics");
    }
    require(silent.rawEnergy.load() > 0 && silent.visualEnergy.load() > 0 && silent.outputs[Vessel::LEFT_OUTPUT].getVoltage() == 0,
        "energy bar follows output level instead of mechanics");
    silent.params[Vessel::LEVEL_PARAM].setValue(1); run(silent, 1000);
    require(silent.outputs[Vessel::LEFT_OUTPUT].getVoltage() == silent.outputs[Vessel::RIGHT_OUTPUT].getVoltage(), "zero-width outputs do not null");
    Module::ProcessArgs a; a.sampleRate = 48000; a.sampleTime = 1.f/48000;
    const double before = silent.audio.engine().bowl().energy(); silent.processBypass(a);
    require(silent.outputs[Vessel::LEFT_OUTPUT].getVoltage() == 0 && silent.outputs[Vessel::RIGHT_OUTPUT].getVoltage() == 0
        && silent.audio.engine().bowl().energy() > 0 && before > 0, "bypass must mute while retaining active mechanics");
    std::cout << "[PASS] Mechanical energy is independent of width/level/cables; zero-width null and bypass mute\n";
}

void patchAndReset() {
    Vessel source; source.params[Vessel::BOWL_PARAM].setValue(1); source.params[Vessel::MALLET_PARAM].setValue(2);
    source.params[Vessel::PITCH_PARAM].setValue(-1); source.params[Vessel::ROTATE_PARAM].setValue(1);
    source.params[Vessel::BINAURAL_PARAM].setValue(33);
    source.params[Vessel::STRIKE_PARAM].setValue(1); run(source, 100);
    json_t* params = source.paramsToJson(); json_t* data = source.dataToJson();
    Vessel loaded; loaded.paramsFromJson(params); loaded.dataFromJson(data); run(loaded);
    require(loaded.params[Vessel::STRIKE_PARAM].getValue() == 0 && loaded.audio.engine().ledger().strikes == 0, "held manual strike restored from patch");
    require(loaded.params[Vessel::BOWL_PARAM].getValue() == 1 && loaded.params[Vessel::MALLET_PARAM].getValue() == 2
        && loaded.params[Vessel::ROTATE_PARAM].getValue() == 0, "stable profiles or momentary rotation state not restored safely");
    require(std::abs(loaded.audio.centerFrequency()-130.8127825) < .001
        && loaded.audio.separationHz()==33 && loaded.params[Vessel::BINAURAL_PARAM].getValue()==33, "saved center/separation not restored");
    require(loaded.audio.engine().bowl().energy() < source.audio.engine().bowl().energy(), "patch load resumes old mechanical state");
    json_decref(params); json_decref(data);
    json_t* future = json_pack("{s:i,s:s}", "schema", 999, "bowlId", vessel::seedBowls[0].stableId);
    loaded.dataFromJson(future); json_decref(future);
    require(loaded.params[Vessel::BOWL_PARAM].getValue() == 1, "future schema partially applied");
    Module::ResetEvent e; loaded.onReset(e); run(loaded, 6000);
    require(loaded.rawEnergy.load() == 0 && loaded.visualSleeping.load() && loaded.params[Vessel::ROTATE_PARAM].getValue() == 0
        && loaded.outputs[Vessel::LEFT_OUTPUT].getVoltage() == 0, "reset does not return to quiet defaults");
    cable(loaded, Vessel::STRIKE_INPUT, 5); run(loaded);
    require(!loaded.visualSleeping.load() && loaded.audio.engine().ledger().strikes == 1, "strike does not immediately wake module");
    std::cout << "[PASS] Stable profile/tuning serialization, no held manual controls or mechanical resume, schema safety, reset and sleep wake\n";
}
void audioHeapSafety() {
    Vessel m;
    trapAllocations = true;
    run(m, 100);
    m.params[Vessel::ROTATE_PARAM].setValue(1); run(m, 2000);
    m.params[Vessel::ROTATE_PARAM].setValue(0);
    cable(m,Vessel::ROTATE_INPUT,10.f);
    cable(m,Vessel::INTENSITY_INPUT,5.f); run(m,1000);
    cable(m,Vessel::INTENSITY_INPUT,10.f); run(m,1000);
    cable(m, Vessel::STRIKE_INPUT, 10); run(m, 1000);
    m.params[Vessel::BOWL_PARAM].setValue(1); m.params[Vessel::MALLET_PARAM].setValue(2);
    m.params[Vessel::BINAURAL_PARAM].setValue(33);
    cable(m, Vessel::VOCT_INPUT, 1); run(m, 6000);
    run(m, 1000, 44100);
    m.params[Vessel::BINAURAL_PARAM].setValue(0); run(m, 12000, 44100);
    m.params[Vessel::BINAURAL_PARAM].setValue(33); run(m, 1000, 44100);
    m.requestedQuality.store(0); run(m, 1000, 44100);
    m.requestedQuality.store(1); run(m, 1000, 44100);
    m.pendingReset.store(true); run(m, 100);
    trapAllocations = false;
    require(heapOperations == 0, "audio callback allocates or frees heap storage");
    require(m.audio.engine().ledger().solverFaults == 0, "heap-safety trajectory faults");
    std::cout << "[PASS] No C++ heap operations in initial, rubbing, strike, morph, pitch, rate-change and reset callbacks\n";
}
void dualControls() {
    Vessel m; m.params[Vessel::ROTATE_PARAM].setValue(1); run(m, 6000);
    m.params[Vessel::BINAURAL_PARAM].setValue(33); run(m, 48);
    require(m.audio.separationHz()>0 && m.audio.separationHz()<33, "separation not smoothed");
    run(m, 12000);
    require(m.audio.separationHz()==33 && m.audio.rightEngine().settings().frequency-m.audio.engine().settings().frequency==33,
        "33 Hz separation not reached");
    m.params[Vessel::LEVEL_PARAM].setValue(0); m.params[Vessel::WIDTH_PARAM].setValue(0); run(m, 12000);
    require(m.rawEnergy.load()>0 && m.visualEnergy.load()>0 && m.leftEnergy.load()>0 && m.rightEnergy.load()>0
        && std::abs(m.rawEnergy.load()-.5f*(m.leftEnergy.load()+m.rightEnergy.load()))<1e-8,
        "dual telemetry is not mean mechanical energy");
    require(m.outputs[Vessel::LEFT_OUTPUT].getVoltage()==0 && m.outputs[Vessel::RIGHT_OUTPUT].getVoltage()==0, "dual output mute");
    const double le=m.audio.engine().totalEnergy(), re=m.audio.rightEngine().totalEnergy();
    m.params[Vessel::BINAURAL_PARAM].setValue(0); run(m, 12000);
    require(m.audio.separationHz()==0 && le>0 && re>0 && m.audio.engine().totalEnergy()>0 && m.audio.rightEngine().totalEnergy()>0,
        "zero crossing clears dual state");
    require(!m.audio.secondBowlActive() && m.audio.dualMix()==0, "settled Rack zero does not stop second bowl");
    require(m.audio.engine().ledger().solverFaults==0 && m.audio.rightEngine().ledger().solverFaults==0, "dual controls fault");
    std::cout << "[PASS] Rack separation smoothing/range, two-bowl energy telemetry, muted mechanics and live return to zero\n";
}
void qualityMenuState() {
    Vessel m;
    run(m);
    require(m.audio.internalRate() == 96000 && m.requestedQuality.load() == 1, "Balanced default");
    m.params[Vessel::ROTATE_PARAM].setValue(1);
    m.params[Vessel::BINAURAL_PARAM].setValue(33);
    cable(m, Vessel::STRIKE_INPUT, 10); run(m, 12000);
    for (int q : {0,1,2,0}) {
        m.requestedQuality.store(q);
        run(m, 12000);
        const double expected = 48000*(1 << q);
        require(m.audio.internalRate() == expected && m.visualInternalRate.load() == expected
            && m.audio.rightEngine().bowl().timeStep() == m.audio.engine().bowl().timeStep(), "quality rate/paired telemetry");
        require(m.audio.engine().ledger().strikes == 0, "quality switch retriggers held strike");
        require(m.audio.engine().contactEngagement() == 1 && !m.visualFault.load()
            && m.audio.engine().bowl().energy() > 0, "rotation does not recover after switch");
    }
    auto* data = m.dataToJson(); Vessel restored; restored.dataFromJson(data); json_decref(data); run(restored);
    require(restored.requestedQuality.load() == 0 && restored.audio.internalRate() == 48000, "quality persistence");
    auto* legacy = json_object(); restored.dataFromJson(legacy); json_decref(legacy); run(restored);
    require(restored.requestedQuality.load() == 1 && restored.audio.internalRate() == 96000, "legacy patch Balanced default");
    auto* invalid = json_pack("{s:i}", "processingQuality", 9000); restored.dataFromJson(invalid); json_decref(invalid); run(restored);
    require(restored.requestedQuality.load() == 1, "invalid quality default");
    restored.requestedQuality.store(2);
    Module::ResetEvent resetEvent;
    restored.onReset(resetEvent);
    run(restored);
    require(restored.requestedQuality.load() == 1 && restored.audio.internalRate() == 96000,
        "reset Balanced default");
    m.params[Vessel::BOWL_PARAM].setValue(1);
    m.params[Vessel::PITCH_PARAM].setValue(float(std::log2(2000./261.625565)));
    run(m, 20000);
    require(m.visualRateFallback.load() && m.audio.internalRate() == 192000 && !m.visualFault.load(), "high-pitch fallback recovery");
    m.params[Vessel::PITCH_PARAM].setValue(0); m.params[Vessel::BOWL_PARAM].setValue(0); run(m, 20000, 44100);
    require(m.audio.internalRate() == 44100 && !m.visualRateFallback.load(), "fallback release/44.1 kHz family");
    m.params[Vessel::ROTATE_PARAM].setValue(0); m.requestedQuality.store(1); run(m, 1000, 44100);
    require(m.audio.engine().bowl().energy() == 0 && m.outputs[Vessel::LEFT_OUTPUT].getVoltage() == 0, "switch must clear isolated tail");
    std::cout << "[PASS] Quality switching, held gates, rotation recovery, patch persistence/defaults and shared fallback\n";
}

}
int main() {
    rack::Context context; rack::contextSet(&context);
    int result = 0;
    {
        rack::engine::Engine engine; context.engine = &engine;
        try { bodyMapTelemetry(); feltStrikeLevel(); gatesAndControls(); tuningAndMorph(); tuneExpander(); manualPerformancePads(); intensityOverride(); strikeAftermathVisual(); independentOutputAndEnergy(); patchAndReset(); audioHeapSafety(); dualControls(); qualityMenuState();
            std::cout << "Vessel Rack adapter: 11 groups PASS\n";
        } catch (const std::exception& error) { std::cerr << "[FAIL] " << error.what() << '\n'; result = 1; }
        context.engine = nullptr;
    }
    rack::contextSet(nullptr); return result;
}
