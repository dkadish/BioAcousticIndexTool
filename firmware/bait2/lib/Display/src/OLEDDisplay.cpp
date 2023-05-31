//
// Created by David Kadish on 29/05/2023.
//

#include "OLEDDisplay.h"
#include "icons_16x16.h"
#include "logging.h"
#include <res/qw_fnt_5x7.h>

void OLEDDisplay::setup() {
    DEBUG("Starting OLED Display Setup")
    Wire.begin();

    // Initalize the OLED device and related graphics system
    if (!myOLED.begin()) {
        WARNING("OLED Failed to Start")
        while (true);
    }
    DEBUG("OLED Display Setup Complete")

     myOLED.setFont(QW_FONT_5X7);
}

void OLEDDisplay::loop() {

    myOLED.erase();

    // if the character is a carriage return, send a blank - otherwise the
    // write routine will perform a CR and lead to a confusing display.
//    myOLED.text(0,0, String(soc) + "%");
//    myOLED.text(0,8, String(volts) + "mV");
//    myOLED.text(0,16, String(current) + "mA");
//    myOLED.text(0,24, String(capacity) + "mAh");
//    myOLED.text(0,32, String(power) + "mW");
//    myOLED.text(0,40, String(health) + "%");
    myOLED.text(0,0, "Hello World!");

    myOLED.display();  // show the added char
}
