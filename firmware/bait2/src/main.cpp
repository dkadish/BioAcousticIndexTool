#include <Arduino.h>
#include "baitconfig.h"
#include "sdcard.h"
#include "logging.h"
#include "PowerSensor.h"
// #include "OLEDDisplay.h"
// #include "FFTReader.h"

PowerSensor powerSensor = PowerSensor(10, "power.csv", 4400);

// OLEDDisplay display = OLEDDisplay();

void setup()
{
#ifdef USE_STATUS_LED
    // Indicator LED
    pinMode(LED_BUILTIN, OUTPUT);
    LEDHIGH
#endif
    Serial.begin(115200);

#ifdef WAIT_FOR_SERIAL
    for (int i = 0; i < 10 && !Serial; i++)
    {
        Serial.begin(115200);
        delay(100);
        DEBUG("Waiting for Serial...")
    }
#endif
    delay(1000);

#ifdef USE_STATUS_LED
    for (int i = 0; i < 5; i++)
    {
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
    // display.setup();

    DEBUG("Setup Complete.")
}

int v = 0;
// ̆̆
void loop()
{
    powerSensor.loop();

    // oledLoop();
}