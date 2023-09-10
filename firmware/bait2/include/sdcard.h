//
// Created by David Kadish on 14/11/2018.
//

#ifndef BAIT_SDCARD_H
#define BAIT_SDCARD_H

// SD Card
#define USE_SDIO 1

#include "SdFat.h"
#include "logging.h"

// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)

SdFs sd;
FsFile file;

void sd_setup() {
    if (!sd.begin(SD_CONFIG)) {
        WARNING("SD Card Failed to Initilise.")
#ifdef WAIT_FOR_SERIAL
        sd.initErrorHalt(&Serial);
#endif
    }
}

#endif //BAIT_SDCARD_H
