//
// Created by David Kadish on 14/11/2018.
//

#ifndef SBAT_FFTREADER_H
#define SBAT_FFTREADER_H

#include <Audio.h>
#include <Sensor.h>

class FFTReader : public Sensor
{
public:
    FFTReader(AudioAnalyzeFFT256 &fft, const char *filepath, bool recordSpectrogram = false, int averageTogether = 2, int debugInterval = 1);
    // FFTReader(AudioAnalyzeFFT256 &fft, int averageTogether=2);

    virtual void setup() override;
    virtual void loop() override;
    virtual float get(int i);
    virtual void debug();

    virtual void recordFFT();

    bool available()
    {
        return _available;
    }

    static const int length = 128; // The length of the FFT array is half of the FFT length (128 = 256/2)

    long getCount() const;
    void resetCount();

private:
    AudioAnalyzeFFT256 &_fft;
    bool _available = false;
    float fft[length];
    long _count;
    bool _record_spectrogram; // Wheether the spectrogram is recorded to SD card
    int _avg_together;

#ifdef SAVE_SPECTROGRAM
    static const int _max_count = 250; // 8610
    float _spectrogram[length][_max_count];

public:
    float getSpectro(int f, int t) { return _spectrogram[f][t]; };
    int getMaxCount() { return _max_count; };
#endif
};

#endif // SBAT_FFTREADER_H
