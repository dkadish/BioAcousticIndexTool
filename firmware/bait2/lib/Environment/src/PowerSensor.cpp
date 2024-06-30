//
// Created by David Kadish on 24/05/2023.
//

#include "logging.h"

#include "PowerSensor.h"
#include "SensorDefinitions.h"
#include <SparkFunBQ27441.h>

PowerSensor::PowerSensor(int interval, const char *filepath, LoRaWANTTN *lorawanTTN, int initialCapacity) : Sensor(interval, filepath),
                                                                                                            initialCapacity(initialCapacity), m_lwTTN(lorawanTTN)
{
}

void PowerSensor::setup()
{
    DEBUG("Starting PowerSensor Setup")
    Sensor::setup();

    for (int i = 0; i < 5; i++)
    {
        if (lipo.begin())
        {
            DEBUG("Connected to BQ27441!")
            break;
        }
        else
        {
            DEBUG("Error: Unable to communicate with BQ27441.");
            DEBUG("  Check wiring and try again.");
            DEBUG("  (Battery must be plugged into Battery Babysitter!)");
        }
        delay(200);
    }

    lipo.setCapacity(initialCapacity);
}

void PowerSensor::process()
{
    TRACE("Starting PowerSensor process")
    soc = (float)lipo.soc();                 // Read state-of-charge (in %)
    volts = (float)lipo.voltage();           // Read voltage (in mV)
    current = (float)lipo.current(AVG);      // Read average current (in mA)
    capacity = (float)lipo.capacity(REMAIN); // Read remaining capacity (in mAh)
    power = (float)lipo.power();             // Read power consumption (in mW)
    health = (float)lipo.soh();              // Read state-of-health (in %)
}

void PowerSensor::record()
{
    // Timestamp, Charge (%), Voltage (mV), Average Current (mA), Remaining Capacity (mAh), Power Consumption (mW), Battery Health (%), Total Capacity (mAh)

    DEBUG("%d, %d, %d, %d, %d, %d, %d", (int)soc, (int)volts, (int)current, (int)capacity, (int)power, (int)health, initialCapacity)

    digitalWrite(LED_BUILTIN, HIGH);

    // Record LoRaWAN data
    m_lwTTN->getLPP().addPercentage(STATE_OF_CHARGE, soc);
    m_lwTTN->getLPP().addVoltage(VOLTAGE, volts / 1000.0);                 // V
    m_lwTTN->getLPP().addCurrent(CURRENT, current / 1000.0);               // A
    m_lwTTN->getLPP().addGenericSensor(REMAINING_CAPACITY, capacity);      // mAh, Remaining Capacity
    m_lwTTN->getLPP().addPower(POWER_CONSUMPTION, power / 1000.0);         // W
    m_lwTTN->getLPP().addPercentage(BATTERY_HEALTH, health);               // %
    m_lwTTN->getLPP().addGenericSensor(INITIAL_CAPACITY, initialCapacity); // mAh, Initial Capacity

    // Mark that there is data to send
    m_lwTTN->setDirty();

    // Record to SD card

    //    File f = SD.open(getFilePath(), FILE_WRITE);
    FsFile f;
    if (!f.open(getFilePath(), O_WRITE | O_CREAT | O_AT_END)) // O_CREAT | O_APPEND))
    {
        DEBUG("Error opening file")
        digitalWrite(LED_BUILTIN, LOW);
        return;
    }
    Sensor::writeTimestamp(&f);

    int charsWritten = f.printf("%d, %d, %d, %d, %d, %d, %d", (int)soc, (int)volts, (int)current, (int)capacity, (int)power, (int)health, initialCapacity);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}
