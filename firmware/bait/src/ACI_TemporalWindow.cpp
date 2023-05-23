//
// Created by David Kadish on 15/11/2018.
//

#include <Arduino.h>
#include "ACI_TemporalWindow.h"
#include "parameters.h"

ACI_TemporalWindow::ACI_TemporalWindow(int interval, FFTReader &fft) : Sensor(interval), _fft(fft) {}

ACI_TemporalWindow::ACI_TemporalWindow(int interval, FFTReader &fft, bool doFilter, bool doDiscardAdjacentZeros) :
        Sensor(interval), _fft(fft), _filter_fft(doFilter), _discard_adjacent_zeros(doDiscardAdjacentZeros) {}

void ACI_TemporalWindow::loop() {
    Sensor::loop();
    float n;

    // ACI
    if (_first) {
        for (int i = 0; i < FFTReader::length; i++) {
            n = _fft.get(i);
            if (_filter_fft && n < _filter_below) {
                n = 0.0;
            }
            I_k[i] = n;
            D[i] = 0.0;
            I[i] = 0.0;
        }
        _first = false;
    } else {
        for (int i = 0; i < FFTReader::length; i++) {
            n = _fft.get(i);
            if (_filter_fft && (n < _filter_below)) {
                n = 0.0;
            }
            float d_k = fabs(n - I_k[i]);
            if (_discard_adjacent_zeros && ((n == 0.0) || (I_k[i] == 0.0))) {
                d_k = 0.0;
            }
            D[i] += d_k;
            I_k[i] = n; // Set the next last value
            I[i] += n;
        }
    }
}

void ACI_TemporalWindow::reset() {
    Sensor::reset();

    _first = true;
}

/** Processes a temporal step of the ACI
 *
 *  Does not actually record a full time step.
 */
void ACI_TemporalWindow::process() {
    Sensor::process();

    int zeros = 0;

    for (int i = 0; i < FFTReader::length; i++) {
        if (I[i] != 0.0) {
            aci += D[i] / I[i];
        } else {
            zeros++;
        }
    }

    if (Sensor::getPrint()) {
        Serial.print("ACI TW: ");
        Serial.print(aci, PRECISION);
        Serial.print(" after ");
        Serial.print(_fft.getCount());
        Serial.println(" frames");
    }

#ifdef ACI_TW_LOG_VALUES
    Serial.println("Saving ACI_TW Value.");
    AudioNoInterrupts();
    File dataFile = SD.open("aci_tw.csv", FILE_WRITE | O_APPEND);
    if (dataFile) {
        dataFile.print(_fft.getCount());
        dataFile.print(",");
        dataFile.println(aci, PRECISION);
        dataFile.close();
    } else {
        Serial.println("ACI_TW: Can't open file.");
    }
    AudioInterrupts();
#endif

    reset();

    _count++;
}

void ACI_TemporalWindow::setup() {
    Sensor::setup();

    reset();
}
