//
// Created by David Kadish on 14/11/2018.
//

#ifndef SBAT_FFTREADER_H
#define SBAT_FFTREADER_H

#include <Audio.h>
#include "Sensor.h"

class FFTReader: public Sensor {
public:
    FFTReader(AudioAnalyzeFFT256 &fft);

    virtual void loop() override;
    virtual float get(int i);

    bool available(){
        return _available;
    }

    static const int length = 128; // The length of the FFT array is half of the FFT length (128 = 256/2)

private:

    AudioAnalyzeFFT256 &_fft;
    bool _available=false;
    float fft[length];
};


#endif //SBAT_FFTREADER_H
