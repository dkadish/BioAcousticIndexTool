#include <Arduino.h>
#include "baitconfig.h"
#include "sdcard.h"
#include "logging.h"
#include "PowerSensor.h"
#include "OLEDDisplay.h"

PowerSensor powerSensor = PowerSensor(10, "power.csv", 6600);

OLEDDisplay  display = OLEDDisplay();

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
    display.setup();

    DEBUG("Setup Complete.")
}

int v=0;
void oledLoop(){
    int line = 0;

    display.myOLED.erase();

    //Time
    char str[48];
    sprintf(str, "%d%c%d %02d:%02d", year() % 100, (char) ('A' - 1 + month()), day(), hour(), minute());
    display.myOLED.text(0, line, str);
//    line += 8;
//    sprintf(str, "%d:%02d:%02d", hour(), minute(), second());
//    display.myOLED.text(0, line, str);

    line += 8;
    display.myOLED.line(0,line,display.myOLED.getWidth(), line);
    line += 2;

    //Battery Info
    display.myOLED.text(0,line, String(powerSensor.soc, 0) + "%");
//    display.myOLED.text(0,8, String(powerSensor.volts, 0) + "mV");
    display.myOLED.text(28,line, "0000mA");
//    display.myOLED.text(0,16, String(powerSensor.current, 0) + "mA");
//    display.myOLED.text(0,24, String(powerSensor.capacity, 0) + "mAh");
    line += 8;
    display.myOLED.text(0,line, "0000mW");
//    display.myOLED.text(0,32, String(powerSensor.power, 0) + "mW");
//    display.myOLED.text(0,40, String(powerSensor.health, 0) + "%");

    line += 8;
    display.myOLED.line(0,line,display.myOLED.getWidth(), line);
    line += 2;

    // SD Card
    if(powerSensor.getStatus() == 0){
        display.myOLED.text(0,line, "SD OK");
    } else {
        display.myOLED.text(0,line, "SD FAIL");
    }

    // Working animation
    display.myOLED.pixel(v, display.myOLED.getHeight()-1);
    v += 1;
    v %= display.myOLED.getWidth();

    display.myOLED.display();  // show the added char
}

void loop() {
    powerSensor.loop();

    oledLoop();
}