//
// Total acoustic entropy (Htf) sensor. The maths lives in EcoacousticMath.
//

#ifndef BAIT2_TOTALENTROPY_H
#define BAIT2_TOTALENTROPY_H

#include "Sensor.h"
#include "FFTReader.h"
#include "EcoacousticMath.h"
#include "lora.h"

class TotalEntropy : public Sensor
{
public:
    TotalEntropy(FFTReader &fft, const char *filepath, LoRaWANTTN *lorattn, int interval, int debugInterval = 30);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void record() override;

    void debug() override;

    float getValue() { return m_value; }

private:
    FFTReader &_fft;
    ecoacoustics::TotalEntropyAccumulator m_h;
    float m_value = 0.0;  // H = Ht * Hf
    float m_ht = 0.0;
    float m_hf = 0.0;
    long m_frames = 0;

    LoRaWANTTN *m_lwTTN;
};

#endif // BAIT2_TOTALENTROPY_H
