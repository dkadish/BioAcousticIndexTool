//
// Created by David Kadish on 14/11/2018.
//

#ifndef BAIT_SENSOR_H
#define BAIT_SENSOR_H

#include <TimeLib.h>
#include <Chrono.h>
#include <SD.h>

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

    Sensor(int interval, const char *filepath);

    virtual void setup() {
        chrono.restart();
    };

    //TODO Return status code for main loop error status
    virtual void loop();

    virtual void start() { chrono.restart(); };

    virtual void reset() { chrono.restart(); };

    virtual void process() {};

    virtual void record() {};

    int getInterval() { return _interval; }

    virtual int firstMeasurement() {
        return Sensor::timeToFirstMeasurement(_interval);
    }

    static int timeToFirstMeasurement(int m_interval) {
        return m_interval - (now() % m_interval);
    }

    const char * getFilePath(){ return m_filepath; };

    void writeTimestamp(stream_t* f);

    int getStatus(){ return m_status; };

protected:
    int m_status=0;

private:
    int _interval; /**< Time between measurements in seconds. */

    const char * m_filepath;

    Chrono chrono;

};


#endif //BAIT_SENSOR_H
