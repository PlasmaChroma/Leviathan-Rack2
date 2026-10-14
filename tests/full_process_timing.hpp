// Shared regression for Process timing: every invocation while debug is enabled.
template <typename Module>
bool verifyFullProcessTiming(Module& module, debug_terminal::AtomicTimingAverage& average,
                             bool& debugEnabled) {
    typename Module::ProcessArgs args;
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f / args.sampleRate;
    average.consume();
    debugEnabled = true;
    for (int i = 0; i < 257; ++i) { args.frame = i; module.process(args); }
    uint64_t enabledSamples = 0;
    average.consume(&enabledSamples);
    debugEnabled = false;
    for (int i = 0; i < 31; ++i) { args.frame = 257 + i; module.process(args); }
    uint64_t disabledSamples = 0;
    average.consume(&disabledSamples);
    const bool pass = enabledSamples == 257 && disabledSamples == 0;
    std::cout << (pass ? "[PASS] " : "[FAIL] ") << "Process timing: enabled="
              << enabledSamples << "/257, disabled=" << disabledSamples << "/0\n";
    return pass;
}
