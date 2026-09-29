// Runs the firmware index maths (lib/EcoacousticMath) over FFT frames read from stdin.
//
// Input: float32 little-endian magnitudes, 128 per frame, until EOF.
// Output: one CSV line on stdout: bi,hf,ht,h,events,background_db,frames
//
// Build (from this directory):
//   g++ -O2 -I../../../firmware/bait2/lib/EcoacousticMath/include \
//       replay_indices.cpp ../../../firmware/bait2/lib/EcoacousticMath/src/EcoacousticMath.cpp \
//       -o replay_indices
//
// Usage: replay_indices <sample_rate> < frames.f32

#include <cstdio>
#include <cstdlib>

#include "EcoacousticMath.h"

using namespace ecoacoustics;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::fprintf(stderr, "usage: %s <sample_rate> < frames.f32\n", argv[0]);
        return 1;
    }
    float sampleRate = std::strtof(argv[1], nullptr);

    // Same defaults as the firmware sensors in main.cpp
    BioacousticIndexAccumulator bi(sampleRate);
    TotalEntropyAccumulator h;
    AcousticEventAccumulator events(sampleRate);

    float frame[kSpectrumLength];
    while (std::fread(frame, sizeof(float), kSpectrumLength, stdin) == (size_t)kSpectrumLength)
    {
        bi.addFrame(frame);
        h.addFrame(frame);
        events.addFrame(frame);
    }

    std::printf("%.8f,%.8f,%.8f,%.8f,%ld,%.2f,%ld\n",
                bi.value(), h.spectralEntropy(), h.temporalEntropy(), h.value(),
                events.value(), events.backgroundDb(), bi.getFrameCount());
    return 0;
}
