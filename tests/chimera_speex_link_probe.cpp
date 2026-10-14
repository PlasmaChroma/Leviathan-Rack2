// Phase 0: prove the installed native Rack SDK resolves the Speex API.
#include <speex/speex_resampler.h>
#include <cstdio>
#include <initializer_list>

int main() {
    for (int hostRate : {8000, 44100, 96000, 192000, 768000}) {
        int error = 0;
        SpeexResamplerState* state = speex_resampler_init(2, hostRate, 48000, 5, &error);
        if (!state || error != RESAMPLER_ERR_SUCCESS)
            return 1;
        const int inputLatency = speex_resampler_get_input_latency(state);
        const int outputLatency = speex_resampler_get_output_latency(state);
        std::printf("%d -> 48000: input=%d host frames, output=%d core frames\n",
                    hostRate, inputLatency, outputLatency);
        speex_resampler_destroy(state);
        if (inputLatency < 0 || outputLatency < 0)
            return 2;
    }
    return 0;
}
