//
// Created by David Kadish on 14/11/2018.
//

#ifndef BAIT_SDCARD_H
#define BAIT_SDCARD_H

// SD Card
#define USE_SDIO 1
#include <SD.h>
#include <SPI.h>
Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = BUILTIN_SDCARD;

void sd_setup(){
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");
}

#endif //BAIT_SDCARD_H
