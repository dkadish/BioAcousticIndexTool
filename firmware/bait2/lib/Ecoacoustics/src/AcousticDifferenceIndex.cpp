//
// Created by David Kadish on 15/11/2018.
//

#include "AcousticDifferenceIndex.h"
#include "parameters.h"

AcousticDifferenceIndex::AcousticDifferenceIndex(int interval, FFTReader &fft) : Sensor(interval), _fft(fft) {

}

void AcousticDifferenceIndex::setup() {
    Sensor::setup();

    reset();
}

void AcousticDifferenceIndex::loop() {
    Sensor::loop();
    if (_first) {
        for (int i = 0; i < FFTReader::length; i++) {
            float f = _fft.get(i);
            diff_freq[i] = f;
        }
        _first = false;
    } else {
        for (int i = 0; i < FFTReader::length; i++) {
            float f = _fft.get(i);
            diff_freq[i] += f;
        }
    }
}

void AcousticDifferenceIndex::reset() {
    Sensor::reset();

    for (int i = 0; i < FFTReader::length; i++) {
        diff_freq[i] = 0.0;
        diff_cumfreq[i] = 0.0;
    }
    _first = true;
}

void AcousticDifferenceIndex::process() {
    Sensor::process();

    for (int i = 0; i < FFTReader::length; i++) {
        diff_cumfreq[i] = diff_freq[i];
        if (i > 0) {
            diff_cumfreq[i] += diff_cumfreq[i - 1];
        }
    }

    record();
    reset();
}

void AcousticDifferenceIndex::record() {
    Sensor::record();

    Serial.print("D: ");
    for (int i = 0; i < 10; i++) {
        Serial.print(diff_freq[i], PRECISION);
        if (i < FFTReader::length - 1)
            Serial.print(",");
    }
    Serial.println(diff_freq[10], PRECISION);

    // Spectral
    File dataFile = SD.open("diffspec.csv", FILE_WRITE);
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

        // Frequency Bins
        for (int i = 0; i < FFTReader::length; i++) {
            dataFile.print(diff_freq[i], PRECISION);
            if (i < FFTReader::length - 1)
                dataFile.print(",");
        }

        dataFile.println();
        dataFile.close();
    } else {
        Serial.println("D: Can't open file.");
    }

    File cumDataFile = SD.open("diffcum.csv", FILE_WRITE);
    if (cumDataFile) {
        // Time
        cumDataFile.print(year());
        cumDataFile.print("-");
        cumDataFile.print(month());
        cumDataFile.print("-");
        cumDataFile.print(day());
        cumDataFile.print(" ");
        cumDataFile.print(hour());
        cumDataFile.print(":");
        if (minute() < 10)
            cumDataFile.print('0');
        cumDataFile.print(minute());
        cumDataFile.print(":");
        if (second() < 10)
            cumDataFile.print('0');
        cumDataFile.print(second());
        cumDataFile.print(", ");

        // Frequency Bins
        for (int i = 0; i < FFTReader::length; i++) {
            cumDataFile.print(diff_cumfreq[i], PRECISION);
            if (i < FFTReader::length - 1)
                cumDataFile.print(",");
        }

        cumDataFile.println();
        cumDataFile.close();
    } else {
        Serial.println("Dc: Can't open file.");
    }
}
