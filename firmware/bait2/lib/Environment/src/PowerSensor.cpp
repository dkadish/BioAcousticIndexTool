//
// Created by David Kadish on 24/05/2023.
//

#include "logging.h"

#include "PowerSensor.h"
#include <SparkFunBQ27441.h>

PowerSensor::PowerSensor(int interval, const char *filepath, int initialCapacity) : Sensor(interval, filepath),
                                                                                   initialCapacity(initialCapacity) {}

void PowerSensor::setup() {
    DEBUG("Starting PowerSensor Setup")
    Sensor::setup();

    for(int i =0; i<5; i++)
    {
        if(lipo.begin()) {
            DEBUG("Connected to BQ27441!")
            break;
        } else {
            DEBUG("Error: Unable to communicate with BQ27441.");
            DEBUG("  Check wiring and try again.");
            DEBUG("  (Battery must be plugged into Battery Babysitter!)");
        }
        delay(200);
    }

    lipo.setCapacity(initialCapacity);
}

void PowerSensor::process() {
    TRACE("Starting PowerSensor process")
    soc = (float) lipo.soc(); // Read state-of-charge (in %)
    volts = (float) lipo.voltage(); // Read voltage (in mV)
    current = (float) lipo.current(AVG); // Read average current (in mA)
    capacity = (float) lipo.capacity(REMAIN); // Read remaining capacity (in mAh)
    power = (float) lipo.power(); // Read power consumption (in mW)
    health = (float) lipo.soh(); // Read state-of-health (in %)
}

void PowerSensor::record() {
    // Timestamp, Charge (%), Voltage (mV), Average Current (mA), Remaining Capacity (mAh), Power Consumption (mW), Battery Health (%), Total Capacity (mAh)

    DEBUG("%d, %d, %d, %d, %d, %d, %d", (int)soc, (int)volts, (int)current, (int)capacity, (int)power, (int)health, initialCapacity)

    digitalWrite(LED_BUILTIN, HIGH);

//    File f = SD.open(getFilePath(), FILE_WRITE);
    FsFile f;
    f.open(getFilePath(), O_CREAT | O_APPEND);
    Sensor::writeTimestamp(&f);

    int charsWritten = f.printf("%d, %d, %d, %d, %d, %d, %d", (int)soc, (int)volts, (int)current, (int)capacity, (int)power, (int)health, initialCapacity);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}