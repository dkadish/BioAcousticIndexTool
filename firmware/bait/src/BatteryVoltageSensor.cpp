//
// Created by David Kadish on 14/11/2018.
//

#include "BatteryVoltageSensor.h"
#include "parameters.h"
#include <SD.h>


#ifdef USE_ANALOG_16
#warning Voltage will not be read because the Audio system is using the ADC.
#endif

BatteryVoltageSensor::BatteryVoltageSensor(int interval, int batteryVoltagePin) :
    Sensor(interval), _pin(batteryVoltagePin), _counter(0) {}

void BatteryVoltageSensor::setup() {
    Sensor::setup();
    pinMode(_pin, INPUT);
}

void BatteryVoltageSensor::process() {
    Sensor::process();

    _voltage /= (float) _counter;

    record();
    reset();
}

void BatteryVoltageSensor::record() {
    Sensor::record();

    Serial.print("Battery: ");
    Serial.println(_voltage, PRECISION);

    File dataFile = SD.open("batt.csv", FILE_WRITE);
    if (dataFile) {
        // Time
        dataFile.print(year());
        dataFile.print("-");
        dataFile.print(month());
        dataFile.print("-");
        dataFile.print(day());
        dataFile.print(" ");
        dataFile.print(hour());
        dataFile.print(":");
        if (minute() < 10)
            dataFile.print('0');
        dataFile.print(minute());
        dataFile.print(":");
        if (second() < 10)
            dataFile.print('0');
        dataFile.print(second());
        dataFile.print(", ");

        // Battery Voltage
        dataFile.print(_voltage, PRECISION);

        dataFile.println();
        dataFile.close();
    } else {
        Serial.println("BATTERY: Can't open file.");
    }
}

void BatteryVoltageSensor::loop() {
    Sensor::loop();

    if(measureTimer.check()){
#ifndef USE_ANALOG_16
        _value = analogRead(_pin);
#endif
        _voltage += calculateVoltage((float)_value); //map((float)_value, 0.0, 1023.0, 0.0, _vref * 2.0);
        _counter++;

        measureTimer.reset();
    }
}

void BatteryVoltageSensor::reset() {
    Sensor::reset();

    _voltage = 0.0;
    _counter = 0;
}


