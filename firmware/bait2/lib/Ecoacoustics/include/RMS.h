//
// Created by David Kadish on 14/11/2018.
//

#ifndef SBAT_RMS_H
#define SBAT_RMS_H

#include <Audio.h>
#include <OversamplingSensor.h>
#include "lora.h"

class RootMeanSquare : public OversamplingSensor
{
public:
    RootMeanSquare(AudioAnalyzeRMS &rms, const char *filepath, LoRaWANTTN *lorattn, int interval = 60, int measureInterval = 0, int debugInterval = 1);

    void setup() override;

    bool sample() override;

    void record() override;

    void debug() override;

    long getCount() const;
    void reset();

private:
    float currentRMS() { return m_count > 0 ? m_rms_accumulator / (float)m_count : 0.0; };

    AudioAnalyzeRMS &m_rms;
    float m_rms_accumulator = 0.0; // Accumulate RMS values over the oversampling period
    bool m_available = false;
    long m_count = 0;

    LoRaWANTTN *m_lwTTN;
};

#endif // SBAT_RMS_H
