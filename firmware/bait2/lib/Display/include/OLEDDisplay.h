//
// Created by David Kadish on 29/05/2023.
//

#ifndef BAIT2_OLEDDISPLAY_H
#define BAIT2_OLEDDISPLAY_H


#include <SparkFun_Qwiic_OLED.h>  //http://librarymanager/All#SparkFun_Qwiic_Graphic_OLED

class OLEDDisplay {

public:
    void setup();

    void loop();

//private:

    QwiicMicroOLED myOLED;
};


#endif //BAIT2_OLEDDISPLAY_H
