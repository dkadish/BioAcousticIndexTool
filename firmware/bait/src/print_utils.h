//
// Created by David Kadish on 2021-03-23.
//

#ifndef SRC_PRINT_UTILS_H
#define SRC_PRINT_UTILS_H

void printTimestamp();

void printSetupMessage();

void printTimestamp() {
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
    Serial.println(second());
}

void printSetupMessage() {
    Serial.print("Setup complete. Using Firmware at ");
    Serial.println(STRINGIFY(PIO_SRC_REV));
}

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include "sdcard.h"
#include "FFTReader.h"
#include "ACI_TemporalWindow.h"
#include "AcousticComplexityIndex.h"
#include "parameters.h"

#endif //SRC_PRINT_UTILS_H
