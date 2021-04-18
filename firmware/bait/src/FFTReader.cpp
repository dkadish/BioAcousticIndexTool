//
// Created by David Kadish on 14/11/2018.
//

#include "FFTReader.h"

FFTReader::FFTReader(AudioAnalyzeFFT256 &fft) : Sensor(0), _fft(fft), _available(false) {}

FFTReader::FFTReader(AudioAnalyzeFFT256 &fft, int averageTogether) : Sensor(0), _fft(fft), _available(false),
                                                                     _avg_together(averageTogether) {}

void FFTReader::loop() {
    Sensor::loop();
//    if(_fft.available()){
//        Serial.println("AA_FFT is available here too!");
//    }

    if (_fft.available()) {
//        Serial.println("FFT Available");
        for (int i = 0; i < length; i++) {
            fft[i] = _fft.read(i);

#ifdef SAVE_SPECTROGRAM
            if (((int) getCount()) < _max_count) {
                _spectrogram[i][(int) getCount()] = fft[i];
            }
#endif
//            Serial.printf("FFT %i is ", i);
//            Serial.println(fft[i]);
        }
        _count += 1;
        _available = true;
    } else {
        _available = false;
    }
}


float FFTReader::get(int i) {
    return fft[i];
}

void FFTReader::setup() {
    Sensor::setup();

    Serial.print("Starting FFT with ");
    Serial.print(_avg_together);
    Serial.println(" values averaged together.");
    _fft.averageTogether(_avg_together);
    _fft.windowFunction(AudioWindowHamming256);

#ifdef SAVE_SPECTROGRAM
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < _max_count; j++) {
            _spectrogram[i][j] = 0;
        }
    }
#endif
}

long FFTReader::getCount() const {
    return _count;
}

void FFTReader::resetCount() {
    _count = 0;
}