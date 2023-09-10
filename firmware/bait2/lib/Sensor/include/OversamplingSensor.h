//
// Created by David Kadish on 24/05/2023.
//

#ifndef BAIT2_OVERSAMPLINGSENSOR_H
#define BAIT2_OVERSAMPLINGSENSOR_H


#include "Sensor.h"

class OversamplingSensor : public Sensor {
public:
    OversamplingSensor(int interval, int measureInterval);

private:
    Chrono measure;
    int measureInterval;
};


#endif //BAIT2_OVERSAMPLINGSENSOR_H
