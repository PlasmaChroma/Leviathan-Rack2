#include "Vessel.hpp"
#include "vessel/SeedProfiles.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace {
std::atomic<std::uint32_t> vesselInstances {0};
double safeValue(double value, double fallback = 0.0) { return std::isfinite(value) ? value : fallback; }
double bound(double value, double lo, double hi) { return std::max(lo, std::min(hi, safeValue(value))); }
int choice(float value, int maximum) { return int(std::round(bound(value, 0, maximum))); }
bool edge(bool& high, double voltage) {
    voltage = safeValue(voltage);
    if (!high && voltage >= 1.0) { high = true; return true; }
    if (high && voltage <= .1) high = false;
    return false;
}
double mix(double a, double b, double t) { return a+(b-a)*t; }
struct ProcessTimer {
    debug_terminal::BaselineModuleMetrics& metrics;
    bool enabled = isDragonKingDebugEnabled();
    std::chrono::steady_clock::time_point start = debug_terminal::debugTimerStart(enabled);
    explicit ProcessTimer(debug_terminal::BaselineModuleMetrics& m) : metrics(m) {}
    ~ProcessTimer() { if (enabled) metrics.recordProcess(debug_terminal::elapsedNsSince(start)); }
};
}

Vessel::Vessel() {
    debugMetrics.assignInstanceId(vesselInstances);
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
    rightExpander.producerMessage = &tuneMessages[0];
    rightExpander.consumerMessage = &tuneMessages[1];
    configParam(PITCH_PARAM, std::log2(20.f/261.625565f), std::log2(2000.f/261.625565f), 0.f,
        "Basic bowl frequency", " Hz", 2.f, 261.625565f);
    configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tuning", " cents");
    configParam(VELOCITY_PARAM, 0.f, 1.f, .5f, "Strike velocity", "%", 0.f, 100.f);
    configButton(STRIKE_PARAM, "Strike bowl");
    getParamQuantity(STRIKE_PARAM)->randomizeEnabled = false;
    configButton(ROTATE_PARAM, "Rub bowl");
    getParamQuantity(ROTATE_PARAM)->randomizeEnabled = false;
    configParam(SPEED_PARAM, -2.f, 2.f, .4f, "Rubbing speed", " rev/s");
    configParam(PRESSURE_PARAM, 0.f, 15.f, 2.5f, "Contact pressure", " N");
    configSwitch(BOWL_PARAM, 0.f, 1.f, 0.f, "Bowl material", {"Metal", "Crystal prototype"});
    configSwitch(MALLET_PARAM, 0.f, 3.f, 1.f, "Mallet", {"Wood", "Suede", "Silicone", "Felt"});
    configParam(DECAY_PARAM, -2.f, 2.f, 0.f, "Sustain", "x", 2.f);
    configParam(IMPERFECTION_PARAM, 0.f, 2.f, 1.f, "Mode-pair imperfection");
    configParam(WIDTH_PARAM, 0.f, 1.f, .7f, "Stereo width", "%", 0.f, 100.f);
    configParam(LEVEL_PARAM, 0.f, 2.f, 1.f, "Output level", "%", 0.f, 100.f);
    configParam(BINAURAL_PARAM, 0.f, 33.f, 0.f, "Binaural bowl frequency difference", " Hz");
    const char* names[] = {"Pitch (1 V/oct)", "Strike gate", "Velocity (0-10 V, replaces knob)",
        "Rub gate", "Rubbing speed (0-10 V multiplier)", "Pressure (0-10 V multiplier)"};
    for (int i = 0; i < INPUTS_LEN; ++i) configInput(i, names[i]);
    configOutput(LEFT_OUTPUT, "Left"); configOutput(RIGHT_OUTPUT, "Right");
    resetRuntime();
}

void Vessel::resetRuntime() {
    audio.reset();
    currentBowl = startBowl = vessel::seedBowls[choice(params[BOWL_PARAM].getValue(), 1)];
    currentMallet = startMallet = vessel::seedMallets[choice(params[MALLET_PARAM].getValue(), 3)];
    selectedBowl = choice(params[BOWL_PARAM].getValue(), 1);
    selectedMallet = choice(params[MALLET_PARAM].getValue(), 3);
    bowlBlend = malletBlend = 1.0;
    pitchOctaves = bound(params[PITCH_PARAM].getValue()+safeValue(inputs[VOCT_INPUT].getVoltage())
        +safeValue(params[FINE_PARAM].getValue())/1200, std::log2(20.0/261.625565), std::log2(2000.0/261.625565));
    applied = vessel::EngineSettings{};
    separationHz = bound(params[BINAURAL_PARAM].getValue(), 0, 33);
    controlElapsed = .001; visualElapsed = idleElapsed = 0;
    meter = strikeFlash = 0;
    strikeAnimationRemaining = 0.f;
    visualStrikeAftermath.store(0.f, std::memory_order_relaxed);
    strikeHigh = manualHigh = rotateHigh = false;
    manualStrikeVelocity.store(1.f, std::memory_order_relaxed);
    manualRotateSpeedScale.store(1.f, std::memory_order_relaxed);
    sleeping = true; configured = false; needsConfigure = true;
    visualEnergy.store(0); rawEnergy.store(0); visualFault.store(false); visualSleeping.store(true);
    visualRotationAngle.store(0.f, std::memory_order_relaxed);
    visualRubbing.store(false, std::memory_order_relaxed);
    leftEnergy.store(0); rightEnergy.store(0); visualSeparation.store(float(separationHz));
}

vessel_expander::TuneMessage Vessel::tuneControls() {
    vessel_expander::TuneMessage tune;
    tune.velocity = float(bound(params[VELOCITY_PARAM].getValue(), 0, 1));
    tune.speed = float(bound(params[SPEED_PARAM].getValue(), -2, 2));
    tune.pressure = float(bound(params[PRESSURE_PARAM].getValue(), 0, 15));
    tune.sustain = float(bound(params[DECAY_PARAM].getValue(), -2, 2));
    tune.imperfection = float(bound(params[IMPERFECTION_PARAM].getValue(), 0, 2));
    tune.width = float(bound(params[WIDTH_PARAM].getValue(), 0, 1));
    tune.level = float(bound(params[LEVEL_PARAM].getValue(), 0, 2));

    const Module* right = rightExpander.module;
    const bool linked = right && right->model == modelVTune && right->leftExpander.module == this;
    const auto* message = linked && rightExpander.consumerMessage
        ? reinterpret_cast<const vessel_expander::TuneMessage*>(rightExpander.consumerMessage)
        : nullptr;
    const bool ready = message && vessel_expander::isValid(*message);
    lights[VTUNE_LINK_LIGHT].setBrightness(linked && !ready ? 1.f : 0.f);
    lights[VTUNE_READY_LIGHT].setBrightness(ready ? 1.f : 0.f);
    if (!ready) {
        return tune;
    }
    tune.velocity = float(bound(message->velocity, 0, 1));
    tune.speed = float(bound(message->speed, -2, 2));
    tune.pressure = float(bound(message->pressure, 0, 15));
    tune.sustain = float(bound(message->sustain, -2, 2));
    tune.imperfection = float(bound(message->imperfection, 0, 2));
    tune.width = float(bound(message->width, 0, 1));
    tune.level = float(bound(message->level, 0, 2));
    return tune;
}

void Vessel::updateControls(double dt, const vessel_expander::TuneMessage& tune) {
    const int bowl = choice(params[BOWL_PARAM].getValue(), 1), mallet = choice(params[MALLET_PARAM].getValue(), 3);
    if (bowl != selectedBowl) { startBowl = currentBowl; selectedBowl = bowl; bowlBlend = 0; }
    if (mallet != selectedMallet) { startMallet = currentMallet; selectedMallet = mallet; malletBlend = 0; }
    const bool morph = bowlBlend < 1 || malletBlend < 1;
    bowlBlend = std::min(1.0, bowlBlend+dt/.1); malletBlend = std::min(1.0, malletBlend+dt/.1);
    const auto& b = vessel::seedBowls[selectedBowl]; const auto& m = vessel::seedMallets[selectedMallet];
    if (morph) {
        currentBowl = b;
        currentBowl.rimRadius = mix(startBowl.rimRadius, b.rimRadius, bowlBlend);
        for (std::size_t i = 0; i < b.pairCount; ++i) {
            auto& p = currentBowl.pairs[i]; const auto& a = startBowl.pairs[i]; const auto& target = b.pairs[i];
            p.ratio = mix(a.ratio, target.ratio, bowlBlend); p.splitCents = mix(a.splitCents, target.splitCents, bowlBlend);
            p.massA = mix(a.massA, target.massA, bowlBlend); p.massB = mix(a.massB, target.massB, bowlBlend);
            p.t60A = mix(a.t60A, target.t60A, bowlBlend); p.t60B = mix(a.t60B, target.t60B, bowlBlend);
            p.radiationA = mix(a.radiationA, target.radiationA, bowlBlend); p.radiationB = mix(a.radiationB, target.radiationB, bowlBlend);
        }
        currentMallet = m;
        currentMallet.mass = mix(startMallet.mass, m.mass, malletBlend);
        currentMallet.stiffness = mix(startMallet.stiffness, m.stiffness, malletBlend);
        currentMallet.loadingDamping = mix(startMallet.loadingDamping, m.loadingDamping, malletBlend);
        currentMallet.patchWidth = mix(startMallet.patchWidth, m.patchWidth, malletBlend);
        currentMallet.muS = mix(startMallet.muS, m.muS, malletBlend); currentMallet.muK = mix(startMallet.muK, m.muK, malletBlend);
        currentMallet.weakeningVelocity = mix(startMallet.weakeningVelocity, m.weakeningVelocity, malletBlend);
        currentMallet.regularizationVelocity = mix(startMallet.regularizationVelocity, m.regularizationVelocity, malletBlend);
        needsConfigure = true;
    }
    const double target = bound(safeValue(params[PITCH_PARAM].getValue())+safeValue(inputs[VOCT_INPUT].getVoltage())
        +safeValue(params[FINE_PARAM].getValue())/1200, std::log2(20.0/261.625565), std::log2(2000.0/261.625565));
    pitchOctaves += -std::expm1(-dt/.0015)*(target-pitchOctaves);
    if (std::abs(target-pitchOctaves) < 1e-7) pitchOctaves = target;
    vessel::EngineSettings next = applied;
    next.frequency = std::max(20.0, std::min(2000.0, 261.625565*std::exp2(pitchOctaves)));
    next.decayMultiplier = std::exp2(tune.sustain);
    next.imperfection = tune.imperfection;
    next.observerSeparation = vessel::pi/6*tune.width;
    if (next.frequency != applied.frequency || next.decayMultiplier != applied.decayMultiplier
        || next.imperfection != applied.imperfection || next.observerSeparation != applied.observerSeparation) needsConfigure = true;
    applied = next;
    const double separationTarget = bound(params[BINAURAL_PARAM].getValue(), 0, 33);
    const double previousSeparation = separationHz;
    separationHz += -std::expm1(-dt/.01)*(separationTarget-separationHz);
    if (std::abs(separationTarget-separationHz) < 1e-6) separationHz = separationTarget;
    if (previousSeparation != separationHz) needsConfigure = true;
}

bool Vessel::configureAudio(double rate) {
    const bool okay = audio.configure(currentBowl, currentMallet, applied, separationHz, rate, activeQuality);
    if (okay) {
        hostRate = rate; configured = true; needsConfigure = false;
        visualInternalRate.store(float(audio.internalRate()), std::memory_order_relaxed);
        visualRateFallback.store(audio.internalRate() > rate*vessel::HostRateAdapter::factorForRate(rate, activeQuality),
            std::memory_order_relaxed);
    }
    return okay;
}

void Vessel::process(const ProcessArgs& args) {
    ProcessTimer timer(debugMetrics);
    if (pendingReset.exchange(false, std::memory_order_acq_rel)) resetRuntime();
    const int requested = requestedQuality.load(std::memory_order_relaxed);
    const auto quality = requested >= 0 && requested <= 2 ? vessel::ProcessingQuality(requested)
        : vessel::ProcessingQuality::Balanced;
    if (quality != activeQuality) {
        // Test-only quality switching deliberately discards the current tail.
        // Keep gate edge state, so a held strike does not become a new event.
        audio.reset();
        activeQuality = quality;
        sleeping = true; configured = false; needsConfigure = true;
        idleElapsed = 0; meter = 0;
    }
    const double dt = args.sampleTime;
    const vessel_expander::TuneMessage tune = tuneControls();
    const bool gateEvent = edge(strikeHigh, inputs[STRIKE_INPUT].getVoltage());
    const bool manualEvent = edge(manualHigh, params[STRIKE_PARAM].getValue());
    edge(rotateHigh, inputs[ROTATE_INPUT].getVoltage());
    const bool manualRotate = safeValue(params[ROTATE_PARAM].getValue()) >= .5;
    vessel::HostControls controls;
    controls.strikeEvent = gateEvent || manualEvent;
    controls.rotate = rotateHigh || manualRotate;
    controls.velocity = manualEvent ? bound(manualStrikeVelocity.load(std::memory_order_relaxed), 0, 1)
        : inputs[VELOCITY_INPUT].isConnected() ? bound(inputs[VELOCITY_INPUT].getVoltage()/10, 0, 1) : tune.velocity;
    const double manualSpeedScale = manualRotate && !rotateHigh
        ? bound(manualRotateSpeedScale.load(std::memory_order_relaxed), 0, 1) : 1;
    controls.speed = tune.speed * manualSpeedScale
        * (inputs[SPEED_INPUT].isConnected() ? bound(inputs[SPEED_INPUT].getVoltage()/10, 0, 1) : 1);
    controls.pressure = tune.pressure
        * (inputs[PRESSURE_INPUT].isConnected() ? bound(inputs[PRESSURE_INPUT].getVoltage()/10, 0, 1) : 1);
    controlElapsed += dt;
    if (controlElapsed >= .001 || controls.strikeEvent) { updateControls(controlElapsed, tune); controlElapsed = 0; }
    const bool wake = controls.rotate || (controls.strikeEvent && controls.velocity > 0);
    if (wake) { sleeping = false; idleElapsed = 0; }
    bool fault = false;
    if ((!sleeping || !configured) && (needsConfigure || !configured || hostRate != args.sampleRate))
        fault = !configureAudio(args.sampleRate);
    vessel::DualBowlFrame frame;
    if (!sleeping && !fault) frame = audio.process(controls);
    fault = fault || frame.fault;
    if (!sleeping && !controls.rotate && !audio.engine().strikerActive()
        && !audio.rightEngine().strikerActive() && audio.engine().contactEngagement() == 0
        && audio.rightEngine().contactEngagement() == 0 && frame.leftEnergy < 2e-14 && frame.rightEnergy < 2e-14) {
        idleElapsed += dt;
        if (idleElapsed >= .1) sleeping = true;
    } else if (!sleeping) idleElapsed = 0;
    const double gain = 8*tune.level;
    for (int i = 0; i < OUTPUTS_LEN; ++i) {
        const double velocity = i == LEFT_OUTPUT ? frame.audio.left : frame.audio.right;
        // Emergency audio protection observes the model; it cannot drive it.
        const float voltage = fault || sleeping ? 0.f : float(std::max(-20.0, std::min(20.0, gain*velocity)));
        outputs[i].setChannels(1); outputs[i].setVoltage(voltage);
    }
    if (controls.strikeEvent && controls.velocity > 0) strikeFlash = 1;
    // Publish a short post-contact animation. Rub cancels it, including strikes
    // received while rubbing, so releasing Rub cannot reveal a stale mallet.
    if (controls.rotate) strikeAnimationRemaining = 0.f;
    else if (controls.strikeEvent && controls.velocity > 0)
        strikeAnimationRemaining = strikeApproachSeconds + strikeReboundSeconds;
    strikeAnimationRemaining = std::max(0.f, strikeAnimationRemaining - float(dt));
    strikeFlash = std::max(0.f, strikeFlash-float(dt/.08));
    lights[STRIKE_LIGHT].setBrightness(strikeFlash);
    lights[ROTATE_LIGHT].setBrightness(controls.rotate ? 1.f : 0.f);
    lights[FAULT_LIGHT].setBrightness(fault ? 1.f : 0.f);
    visualElapsed += dt;
    if (visualElapsed >= .005 || controls.strikeEvent || fault) {
        const double energy = sleeping ? audio.meanEnergy() : frame.bowlEnergy;
        const double db = 10*std::log10((std::max(0.0, energy)+2e-20)/.02);
        const float target = float(std::max(0.0, std::min(1.0, (db+60)/60)));
        meter += float(-std::expm1(-visualElapsed/(target > meter ? .005 : .15)))*(target-meter);
        visualEnergy.store(meter, std::memory_order_relaxed); rawEnergy.store(float(energy), std::memory_order_relaxed);
        leftEnergy.store(float(audio.engine().bowl().energy()), std::memory_order_relaxed);
        rightEnergy.store(float(audio.rightEngine().bowl().energy()), std::memory_order_relaxed);
        const double center = std::max(20+.5*separationHz, std::min(2000-.5*separationHz, applied.frequency));
        visualFrequency.store(float(center), std::memory_order_relaxed);
        visualRotationAngle.store(float(audio.engine().rotationAngle()), std::memory_order_relaxed);
        visualRubbing.store(controls.rotate, std::memory_order_relaxed);
        visualStrikeAftermath.store(
            strikeAnimationRemaining / (strikeApproachSeconds + strikeReboundSeconds),
            std::memory_order_relaxed);
        visualSeparation.store(float(separationHz), std::memory_order_relaxed);
        visualFault.store(fault, std::memory_order_relaxed); visualSleeping.store(sleeping, std::memory_order_relaxed);
        visualElapsed = 0;
    }
}

void Vessel::onReset(const ResetEvent& event) {
    Module::onReset(event);
    requestedQuality.store(int(vessel::ProcessingQuality::Balanced), std::memory_order_relaxed);
    pendingReset.store(true, std::memory_order_release);
}
void Vessel::processBypass(const ProcessArgs& args) {
    // Retain the ongoing contacts/tail when bypass is lifted; bypass is output mute.
    process(args);
    outputs[LEFT_OUTPUT].setVoltage(0); outputs[RIGHT_OUTPUT].setVoltage(0);
}
json_t* Vessel::dataToJson() {
    json_t* root = json_object(); json_object_set_new(root, "schema", json_integer(1));
    json_object_set_new(root, "processingQuality", json_integer(requestedQuality.load(std::memory_order_relaxed)));
    const int b = choice(params[BOWL_PARAM].getValue(), 1), m = choice(params[MALLET_PARAM].getValue(), 3);
    json_object_set_new(root, "bowlId", json_string(vessel::seedBowls[b].stableId));
    json_object_set_new(root, "bowlVersion", json_integer(vessel::seedBowls[b].version));
    json_object_set_new(root, "malletId", json_string(vessel::seedMallets[m].stableId));
    return root;
}
void Vessel::dataFromJson(json_t* root) {
    if (!json_is_object(root)) return;
    const auto* schema = json_object_get(root, "schema");
    if (schema && (!json_is_integer(schema) || json_integer_value(schema) != 1)) return;
    const auto* quality = json_object_get(root, "processingQuality");
    const int defaultQuality = int(vessel::ProcessingQuality::Balanced);
    const auto qualityValue = json_is_integer(quality) ? json_integer_value(quality) : defaultQuality;
    requestedQuality.store(qualityValue >= 0 && qualityValue <= 2 ? int(qualityValue) : defaultQuality, std::memory_order_relaxed);
    const char* bowl = json_string_value(json_object_get(root, "bowlId"));
    const char* mallet = json_string_value(json_object_get(root, "malletId"));
    for (std::size_t i = 0; bowl && i < vessel::seedBowlCount; ++i)
        if (!std::strcmp(bowl, vessel::seedBowls[i].stableId)) params[BOWL_PARAM].setValue(float(i));
    for (std::size_t i = 0; mallet && i < vessel::seedMalletCount; ++i)
        if (!std::strcmp(mallet, vessel::seedMallets[i].stableId)) params[MALLET_PARAM].setValue(float(i));
    params[STRIKE_PARAM].setValue(0); params[ROTATE_PARAM].setValue(0);
    pendingReset.store(true, std::memory_order_release);
}
json_t* Vessel::paramsToJson() {
    json_t* root = Module::paramsToJson();
    size_t index; json_t* item;
    json_array_foreach(root, index, item) {
        const int id = int(json_integer_value(json_object_get(item, "id")));
        if (id == STRIKE_PARAM || id == ROTATE_PARAM)
            json_object_set_new(item, "value", json_real(0));
    }
    return root;
}
void Vessel::paramsFromJson(json_t* root) {
    Module::paramsFromJson(root); params[STRIKE_PARAM].setValue(0); params[ROTATE_PARAM].setValue(0);
    pendingReset.store(true, std::memory_order_release);
}
