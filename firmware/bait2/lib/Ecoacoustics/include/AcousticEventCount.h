//
// Acoustic event count sensor. The maths lives in EcoacousticMath.
//

#ifndef BAIT2_ACOUSTICEVENTCOUNT_H
#define BAIT2_ACOUSTICEVENTCOUNT_H

#include "Sensor.h"
#include "FFTReader.h"
#include "EcoacousticMath.h"
#include "lora.h"

class AcousticEventCount : public Sensor
{
public:
    AcousticEventCount(FFTReader &fft, const char *filepath, LoRaWANTTN *lorattn, int interval,
                       float minFreq = 1000.0f, float maxFreq = 8000.0f, float thresholdDb = 3.0f,
                       int debugInterval = 30);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void record() override;

    void debug() override;

    long getValue() { return m_value; }

private:
    FFTReader &_fft;
    ecoacoustics::AcousticEventAccumulator m_events;
    long m_value = 0;
    float m_background = 0.0; // Background noise level (dB) for the last interval
    long m_frames = 0;

    LoRaWANTTN *m_lwTTN;
};

#endif // BAIT2_ACOUSTICEVENTCOUNT_H
