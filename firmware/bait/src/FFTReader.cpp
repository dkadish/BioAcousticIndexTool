//
// Created by David Kadish on 14/11/2018.
//

#include "FFTReader.h"

FFTReader::FFTReader(AudioAnalyzeFFT256 &fft) : Sensor(0), _fft(fft), _available(false) {}

void FFTReader::loop() {
    Sensor::loop();
//    if(_fft.available()){
//        Serial.println("AA_FFT is available here too!");
//    }

    if(_fft.available()){
//        Serial.println("FFT Available");
        for (int i = 0; i < length; i++) {
            fft[i] = _fft.read(i);
//            Serial.printf("FFT %i is ", i);
//            Serial.println(fft[i]);
        }
        _available = true;
    } else {
        _available = false;
    }
}


float FFTReader::get(int i) {
   return fft[i];
}