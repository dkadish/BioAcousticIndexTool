//
// Created by David Kadish on 14/11/2018.
//

#include "Sensor.h"

Sensor::Sensor(int interval) : metro(interval * 1000L) {
    _interval = interval;
}