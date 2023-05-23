//
// Created by David Kadish on 14/11/2018.
//

#ifndef BAIT_SENSOR_H
#define BAIT_SENSOR_H

#include <TimeLib.h>
#include <Metro.h>

/** Base class for a sensor operating in the BAIT system.
 *
 */
class Sensor {
public:
    /**
     *
     * @param interval Interval in seconds
     */
    Sensor(int interval);

    virtual void setup() {
        metro.interval(_interval * 1000L);
        metro.reset();
    };

    virtual void loop() { if (metro.check()) process(); };

    virtual void start() { metro.reset(); };

    virtual void reset() { metro.reset(); };

    virtual void process() {};

    virtual void record() {};

    int getInterval() { return _interval; }

    virtual int firstMeasurement() {
        return Sensor::timeToFirstMeasurement(_interval);
    }

    static int timeToFirstMeasurement(int m_interval) {
        return m_interval - (now() % m_interval);
    }

    void enablePrint() { _print = true; };

    void disablePrint() { _print = false; };

    bool getPrint() { return _print; };

//    bool ready(){ return metro.check(); }

private:
    int _interval; /**< Time between measurements in seconds. */
    bool _print = true;

    Metro metro;

};


#endif //BAIT_SENSOR_H
