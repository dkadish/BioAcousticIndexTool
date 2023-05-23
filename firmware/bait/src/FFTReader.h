//
// Created by David Kadish on 14/11/2018.
//

#ifndef BAIT_FFTREADER_H
#define BAIT_FFTREADER_H

#include <Audio.h>
#include "Sensor.h"

class FFTReader: public Sensor {
public:
    FFTReader(AudioAnalyzeFFT256 &fft);
    FFTReader(AudioAnalyzeFFT256 &fft, int averageTogether);

    virtual void setup() override;
    virtual void loop() override;
    virtual float get(int i);

    bool available(){
        return _available;
    }

    static const int length = 128; // The length of the FFT array is half of the FFT length (128 = 256/2)

    long getCount() const;
    void resetCount();

private:

    AudioAnalyzeFFT256 &_fft;
    bool _available=false;
    float fft[length];
    long _count;
    int _avg_together=2;

#ifdef SAVE_SPECTROGRAM
    static const int _max_count = 250; //8610
    float _spectrogram[length][_max_count];
public:
    float getSpectro(int f, int t){ return _spectrogram[f][t]; };
    int getMaxCount(){ return _max_count; };
#endif
};


#endif //BAIT_FFTREADER_H
