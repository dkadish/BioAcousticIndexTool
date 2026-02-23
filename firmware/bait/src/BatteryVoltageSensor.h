//
// Created by David Kadish on 14/11/2018.
//

#ifndef BAIT_BATTERYVOLTAGESENSOR_H
#define BAIT_BATTERYVOLTAGESENSOR_H

#include <Sensor.h>

#include <Arduino.h>
#include <Metro.h>
#include "BatteryMeasurement.h"

class BatteryVoltageSensor : public Sensor, public BatteryMeasurement {
public:
    BatteryVoltageSensor(int interval, int batteryVoltagePin);

    void setup() override;

    void loop() override;

    void reset() override;

    void process() override;

    void record() override;

private:
    int _pin;
    int _value;
    float _voltage;

    int _counter;

    Metro measureTimer = Metro(100);

};


#endif //BAIT_BATTERYVOLTAGESENSOR_H
