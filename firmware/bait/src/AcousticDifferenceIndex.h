//
// Created by David Kadish on 15/11/2018.
//

#ifndef SBAT_ACOUSTICDIFFERENCEINDEX_H
#define SBAT_ACOUSTICDIFFERENCEINDEX_H


#include "Sensor.h"
#include "FFTReader.h"

class AcousticDifferenceIndex : public Sensor {
public:
    AcousticDifferenceIndex(int interval, FFTReader &fft);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void record() override;

private:
    FFTReader & _fft;

    float diff_freq[FFTReader::length];
    float diff_cumfreq[FFTReader::length];
    bool _first = true;
};


#endif //SBAT_ACOUSTICDIFFERENCEINDEX_H
