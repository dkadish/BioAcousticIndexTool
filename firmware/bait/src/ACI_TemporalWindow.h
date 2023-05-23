//
// Created by David Kadish on 15/11/2018.
//

#ifndef BAIT_ACI_TEMPORALWINDOW_H
#define BAIT_ACI_TEMPORALWINDOW_H


#include "Sensor.h"
#include "FFTReader.h"
#include <float.h>

class ACI_TemporalWindow : public Sensor {
public:
    ACI_TemporalWindow(int interval, FFTReader &fft);
    ACI_TemporalWindow(int interval, FFTReader &fft, bool doFilter, bool doDiscardAdjacentZeros);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void zero(){ aci = 0.0; _count = 0; _fft.resetCount(); }

    float getValue(){ return aci; }

    int getCount(){ return _count; };

    int getFFTCount(){ return _fft.getCount(); };

    void setFilter(bool doFilter){ _filter_fft = doFilter; };

    void setFilterLimit(float filterLimit){ _filter_below = filterLimit; };

    void setDiscardAdjacentZeros(bool doDiscardAdjacentZeros){ _discard_adjacent_zeros = doDiscardAdjacentZeros; };

private:

    float D[FFTReader::length]; // The sum of binwise differences between timesteps, for each bin
    float I[FFTReader::length]; // The sum of intesities for each bin
    float I_k[FFTReader::length]; // The previous intesity for each bin
    bool _first = true;

    bool _filter_fft = false; // Should FFT values be set to zero when below a threshold? See Farina et. al. (2016). The Application of ...
    float _filter_below = FLT_MIN + FLT_MIN; // Set the default filter value to be twice the Minimum normalized positive floating-point number

    bool _discard_adjacent_zeros = false; // Ignore frame differences when one of the two frames is zero. See  Farina et. al. (2016).

    int _count = 0;

    float aci;

    FFTReader &_fft;

};


#endif //BAIT_ACI_TEMPORALWINDOW_H
