//
// Created by David Kadish on 14/11/2018.
//

#include "Sensor.h"

Sensor::Sensor(int interval, const char *filepath) : _interval(interval), m_filepath(filepath) {
}

void Sensor::writeTimestamp(File f) {
    if (f) {
        f.printf("%d-%d-%d %d:%02d:%02d, ", year(), month(), day(), hour(), minute(), second());
    }
//        // Time
//        f.print(year());
//        f.print("-");
//        f.print(month());
//        f.print("-");
//        f.print(day());
//        f.print(" ");
//        f.print(hour());
//        f.print(":");
//        if (minute() < 10)
//            dataFile.print('0');
//        dataFile.print(minute());
//        dataFile.print(":");
//        if (second() < 10)
//            dataFile.print('0');
//        dataFile.print(second());
//        dataFile.print(", ");
}

void Sensor::loop() {
    if (chrono.hasPassed(_interval * 1000L, true)) {
        process();
        record();
    }
}
