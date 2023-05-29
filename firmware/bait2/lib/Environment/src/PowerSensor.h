//
// Created by David Kadish on 24/05/2023.
//

#ifndef BAIT2_POWERSENSOR_H
#define BAIT2_POWERSENSOR_H


#include "Sensor.h"

class PowerSensor : public Sensor {
public:
    PowerSensor(int interval, const char *filepath, int initialCapacity);

    void setup() override;

    void process() override;

    void record() override;

private:
    int initialCapacity;
    float soc=0.0, // Read state-of-charge (in %)
          volts=0.0, // Read voltage (in mV)
          current=0.0, // Read average current (in mA)
          capacity=0.0, // Read remaining capacity (in mAh)
          power=0.0, // Read power consumption (in mW)
          health=0.0; // Read state-of-health (in %)
};


#endif //BAIT2_POWERSENSOR_H
