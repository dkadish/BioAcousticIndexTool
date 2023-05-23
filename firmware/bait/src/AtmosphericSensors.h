//
// Created by David Kadish on 16/11/2018.
//

#ifndef BAIT_ATMOSPHERICSENSORS_H
#define BAIT_ATMOSPHERICSENSORS_H


#include <Adafruit_Si7021.h>
#include <Adafruit_TSL2561_U.h>
#include <Metro.h>
#include "Sensor.h"

class AtmosphericSensors : public Sensor {
public:
    AtmosphericSensors(int interval);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void record() override;

private:
    // Temperature/Humidity
    Adafruit_Si7021 si7021 = Adafruit_Si7021();

    // Luminosity
    Adafruit_TSL2561_Unified tsl2561 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

    float temperature, humidity, luminosity;
    int counter;

    Metro measureTimer = Metro(20);
};


#endif //BAIT_ATMOSPHERICSENSORS_H
