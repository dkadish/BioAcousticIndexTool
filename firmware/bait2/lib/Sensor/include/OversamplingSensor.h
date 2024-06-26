//
// Created by David Kadish on 24/05/2023.
//

#ifndef BAIT2_OVERSAMPLINGSENSOR_H
#define BAIT2_OVERSAMPLINGSENSOR_H

#include "Sensor.h"

class OversamplingSensor : public Sensor
{
public:
    OversamplingSensor(int interval, int measureInterval, int debugInterval = 0);

    virtual void loop();
    virtual void sample() {};

private:
    Chrono m_measure;
    int m_measureInterval;
};

#endif // BAIT2_OVERSAMPLINGSENSOR_H
