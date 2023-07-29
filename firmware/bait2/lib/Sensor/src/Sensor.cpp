//
// Created by David Kadish on 14/11/2018.
//

#include "Sensor.h"

Sensor::Sensor(int interval, const char *filepath) : _interval(interval), m_filepath(filepath) {
}

void Sensor::writeTimestamp(stream_t * f) {
    if(f) {
        f->printf("%d-%d-%d %d:%02d:%02d, ", year(), month(), day(), hour(), minute(), second());
    }
}

void Sensor::loop() {
    if (chrono.hasPassed(_interval * 1000L, true)) {
        process();
        record();
    }
}
