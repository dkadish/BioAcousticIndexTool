//
// Created by David Kadish on 15/11/2018.
//

#include <SD.h>
#include "AcousticComplexityIndex.h"
#include "parameters.h"

AcousticComplexityIndex::AcousticComplexityIndex(int interval, ACI_TemporalWindow &aci_tw) :
        Sensor(interval), _aci(aci_tw) {

}

AcousticComplexityIndex::AcousticComplexityIndex(int interval, ACI_TemporalWindow &aci_tw, boolean printWindowCount) :
        Sensor(interval), _aci(aci_tw), _printWindowCount(printWindowCount) {

}

void AcousticComplexityIndex::reset() {
    Sensor::reset();

    _aci.zero();
}

void AcousticComplexityIndex::record() {
    Sensor::record();

    Serial.print(year());
    Serial.print("-");
    Serial.print(month());
    Serial.print("-");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(hour());
    Serial.print(":");
    if (minute() < 10)
        Serial.print('0');
    Serial.print(minute());
    Serial.print(":");
    if (second() < 10)
        Serial.print('0');
    Serial.print(second());
    Serial.print(", ");

    // ACI/ADI
    Serial.print(_aci.getValue(), PRECISION);
    if (_printWindowCount) {
        Serial.print(", ");
        // Number of windows processed (j)
        Serial.print(_aci.getCount());
    }
    Serial.println();

    File dataFile = SD.open("aci.csv", FILE_WRITE);
    if (dataFile) {
        // Time
        dataFile.print(year());
        dataFile.print("-");
        dataFile.print(month());
        dataFile.print("-");
        dataFile.print(day());
        dataFile.print(" ");
        dataFile.print(hour());
        dataFile.print(":");
        if (minute() < 10)
            dataFile.print('0');
        dataFile.print(minute());
        dataFile.print(":");
        if (second() < 10)
            dataFile.print('0');
        dataFile.print(second());
        dataFile.print(", ");

        // ACI/ADI
        dataFile.print(_aci.getValue(), PRECISION);
        if (_printWindowCount) {
            dataFile.print(", ");
            // Number of windows processed (j)
            dataFile.print(_aci.getCount());
        }
        dataFile.println();

        dataFile.close();
    } else {
        Serial.println("ACI: Can't open file.");
    }
}

void AcousticComplexityIndex::process() {
    Sensor::process();

    record();
    reset();
}
