//
// Created by David Kadish on 15/11/2018.
//

#ifndef BAIT_ACOUSTICDIVERSITYINDEX_H
#define BAIT_ACOUSTICDIVERSITYINDEX_H


#include "Sensor.h"
#include "FFTReader.h"

class AcousticDiversityIndex : public Sensor {
public:
    AcousticDiversityIndex(int interval, FFTReader &fft);

    void loop() override;

    void setup() override;

    void reset() override;

    void process() override;

    void record() override;

private:

    FFTReader & _fft;
    const int bins_per_band = 12; // Number of bins per 1000 Hz band, given 256 bins for 20kHz
    static const int n_bands = 10;
    float bands[n_bands];
    long sample_count = 0L;
    float adi = 0.0;
};


#endif //BAIT_ACOUSTICDIVERSITYINDEX_H
