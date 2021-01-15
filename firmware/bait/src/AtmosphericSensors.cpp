//
// Created by David Kadish on 16/11/2018.
//

#include <SD.h>
#include "AtmosphericSensors.h"
#include "parameters.h"

AtmosphericSensors::AtmosphericSensors(int interval) : Sensor(interval) {

}

void AtmosphericSensors::setup() {
    Sensor::setup();

    si7021.begin();
    tsl2561.setGain(TSL2561_GAIN_1X);
    tsl2561.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */

    measureTimer.reset();
}

void AtmosphericSensors::loop() {
    Sensor::loop();

    if(measureTimer.check()){
        float temp = si7021.readTemperature();
        float hum = si7021.readHumidity();

        sensors_event_t event;
        tsl2561.getEvent(&event);
        float light = event.light;

        temperature += temp;
        humidity += hum;
        luminosity += light;
        counter++;
        measureTimer.reset();
    }
}

void AtmosphericSensors::reset() {
    Sensor::reset();

    temperature = 0;
    humidity = 0;
    luminosity = 0;
    counter = 0;
}

void AtmosphericSensors::process() {
    Sensor::process();

    temperature /= (float) counter;
    humidity /= (float) counter;
    luminosity /= (float) counter;

    record();
    reset();
}

void AtmosphericSensors::record() {
    Sensor::record();

    Serial.print("T: ");
    Serial.print(temperature, PRECISION);
    Serial.print("C, H: ");
    Serial.print(humidity, PRECISION);
    Serial.print("%, L: ");
    Serial.println(luminosity, PRECISION);

    File dataFile = SD.open("atmos.csv", FILE_WRITE);
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

        dataFile.print(temperature, PRECISION);
        dataFile.print(", ");
        dataFile.print(humidity, PRECISION);
        dataFile.print(", ");
        dataFile.print(luminosity, PRECISION);
        dataFile.println();

        dataFile.close();
    } else {
        Serial.println("ATMOSPHERE: Can't open file.");
    }
}
