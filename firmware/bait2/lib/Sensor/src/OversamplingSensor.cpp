//
// Created by David Kadish on 24/05/2023.
//

#include "OversamplingSensor.h"

OversamplingSensor::OversamplingSensor(int interval, int measureInterval, int debugInterval) : Sensor(interval, debugInterval),
                                                                                               m_measureInterval(measureInterval) {}

void OversamplingSensor::loop()
{
  // Check the main interval and the debug message interval
  Sensor::loop();

  if (m_measure.hasPassed(m_measureInterval * 1000L, true))
  {
    sample();
  }
}
