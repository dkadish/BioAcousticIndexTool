//
// Created by David Kadish on 14/11/2018.
//

#ifndef SBAT_SENSOR_H
#define SBAT_SENSOR_H

#include <TimeLib.h>
#include <Metro.h>

/** Base class for a sensor operating in the SBAT system.
 *
 */
class Sensor {
public:
    /**
     *
     * @param interval Interval in seconds
     */
    Sensor(int interval);

    virtual void setup(){ metro.interval(_interval*1000L); metro.reset(); };
    virtual void loop(){ if(metro.check()) process(); };

    virtual void start(){ metro.reset(); };
    virtual void reset(){ metro.reset(); };

    virtual void process(){};
    virtual void record(){};

    int getInterval(){ return _interval; }

    virtual int firstMeasurement(){
        return Sensor::timeToFirstMeasurement(_interval);
    }

    static int timeToFirstMeasurement(int m_interval) {
        return m_interval - (now() % m_interval);
    }

//    bool ready(){ return metro.check(); }

private:
    int _interval; /**< Time between measurements in seconds. */

    Metro metro;

};


#endif //SBAT_SENSOR_H
