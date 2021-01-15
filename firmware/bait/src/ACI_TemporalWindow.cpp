//
// Created by David Kadish on 15/11/2018.
//

#include <Arduino.h>
#include "ACI_TemporalWindow.h"
#include "parameters.h"

ACI_TemporalWindow::ACI_TemporalWindow(int interval, FFTReader &fft) : Sensor(interval), _fft(fft) {}

void ACI_TemporalWindow::loop() {
    Sensor::loop();
    float n;

    // ACI
    if (_first) {
        for (int i = 0; i < FFTReader::length; i++) {
            n = _fft.get(i);
            I_k[i] = n;
            D[i] = 0.0;
            I[i] = 0.0;
        }
        _first = false;
    } else {
        for (int i = 0; i < FFTReader::length; i++) {
            n = _fft.get(i);
            float d_k = fabs(n - I_k[i]);
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

    Serial.print("ACI TW: ");
    Serial.println(aci, PRECISION);

    reset();

    _count++;
}

void ACI_TemporalWindow::setup() {
    Sensor::setup();

    reset();
}
