//
// Created by David Kadish on 15/11/2018.
//

#ifndef SBAT_ACI_TEMPORALWINDOW_H
#define SBAT_ACI_TEMPORALWINDOW_H


#include "Sensor.h"
#include "FFTReader.h"

class ACI_TemporalWindow : public Sensor {
public:
    ACI_TemporalWindow(int interval, FFTReader &fft);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void zero(){ aci = 0.0; _count = 0;}

    float getValue(){ return aci; }

    int getCount(){ return _count; };

private:

    long j = 30L; // The number of seconds in a temporal step of the recording
    //TODO WHY IS THIS over 2?
    float D[FFTReader::length]; // The sum of binwise differences between timesteps, for each bin
    float I[FFTReader::length]; // The sum of intesities for each bin
    float I_k[FFTReader::length]; // The previous intesity for each bin
    bool _first = true;

    int _count = 0;

    float aci;

    FFTReader &_fft;

};


#endif //SBAT_ACI_TEMPORALWINDOW_H
