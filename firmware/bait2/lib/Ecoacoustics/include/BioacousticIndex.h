//
// Bioacoustic Index sensor. The maths lives in EcoacousticMath.
//

#ifndef BAIT2_BIOACOUSTICINDEX_H
#define BAIT2_BIOACOUSTICINDEX_H

#include "Sensor.h"
#include "FFTReader.h"
#include "EcoacousticMath.h"
#include "lora.h"

class BioacousticIndex : public Sensor
{
public:
    BioacousticIndex(FFTReader &fft, const char *filepath, LoRaWANTTN *lorattn, int interval,
                     float minFreq = 2000.0f, float maxFreq = 8000.0f, int debugInterval = 30);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void record() override;

    void debug() override;

    float getValue() { return m_value; }

private:
    FFTReader &_fft;
    ecoacoustics::BioacousticIndexAccumulator m_bi;
    float m_value = 0.0;
    long m_frames = 0;

    LoRaWANTTN *m_lwTTN;
};

#endif // BAIT2_BIOACOUSTICINDEX_H
