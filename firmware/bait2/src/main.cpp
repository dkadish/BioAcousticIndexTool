#include <Arduino.h>
#include "config.h"
#include "sdcard.h"
#include "logging.h"
#include "PowerSensor.h"

PowerSensor powerSensor = PowerSensor(10, "power.csv", 4400);

void setup() {
#ifdef USE_STATUS_LED
    // Indicator LED
    pinMode(LED_BUILTIN, OUTPUT);
    LEDHIGH
#endif
    Serial.begin(115200);

#ifdef WAIT_FOR_SERIAL
    while(!Serial){}
#endif
    delay(1000);

#ifdef  USE_STATUS_LED
    for(int i=0; i<5; i++){
        LEDHIGH
        delay(500);
        LEDLOW
        delay(500);
    }
    LEDLOW
#endif

    DEBUG("Started.")

    sd_setup();

    DEBUG("SD Initialised.")

    powerSensor.setup();

    DEBUG("Setup Complete.")
}

void loop() {
    powerSensor.loop();
}