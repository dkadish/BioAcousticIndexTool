//
// Created by David Kadish on 15/11/2018.
//

#include "AcousticDiversityIndex.h"
#include "parameters.h"

AcousticDiversityIndex::AcousticDiversityIndex(int interval, FFTReader &fft) :
    Sensor(interval), _fft(fft) {

}

void AcousticDiversityIndex::loop() {
    Sensor::loop();

    //TODO Can I just use fft.read(start, finish) here?
    for (int i = 0; i < n_bands; i++) {
        float band = 0.0;
        int start_bin = i * bins_per_band;
        for (int j = start_bin; j < start_bin + bins_per_band; j++) {
            float dbfs = 20 * log10(_fft.get(j)); // Should be divided by MAX but MAX=1.0 (https://www.pjrc.com/teensy/gui/index.html?info=AudioAnalyzeFFT1024)

            // Using the default threshold from https://cran.r-project.org/web/packages/soundecology/vignettes/intro.html
            if (dbfs > -50.0) {
                band += 1.0;
            }
        }

        bands[i] += band / ((float)bins_per_band);
    }
    sample_count++;
}

void AcousticDiversityIndex::reset() {
    Sensor::reset();
    for (int i = 0; i < n_bands; i++)
        bands[i] = 0.0;

    adi = 0.0;
    sample_count = 0L;
}

void AcousticDiversityIndex::process() {
    Sensor::process();
    adi = 0.0;
    for ( int i = 0; i < n_bands; i++) {
        float b = bands[i] / ((float)sample_count);
    //TODO This is incorrect in the prototype. Should be -=.
        adi += b * log(b + 0.0000001); // Add to avoid log(0.0) https://cran.r-project.org/web/packages/soundecology/vignettes/changeADI.html
    }

    record();
    reset();
}

void AcousticDiversityIndex::record() {
    Sensor::record();

    Serial.print("ADI: ");
    Serial.println(adi, PRECISION);

    File dataFile = SD.open("adi.csv", FILE_WRITE);
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
        dataFile.print(adi, PRECISION);
        dataFile.println();

        dataFile.close();
    } else {
        Serial.println("ADI: Can't open file.");
    }
}

void AcousticDiversityIndex::setup() {
    Sensor::setup();

    reset();
}
