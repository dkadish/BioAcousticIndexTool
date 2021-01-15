//
// Created by David Kadish on 11/12/2018.
//

#ifndef SBAT_LOWPOWERBATTERYWAKE_H
#define SBAT_LOWPOWERBATTERYWAKE_H


#include <Snooze.h>
#include "BatteryMeasurement.h"

#define VREF 3.3

typedef void (*LowPowerSetup) ();
typedef void (*LowPowerLoop) ();

class LowPowerBatteryWake : public BatteryMeasurement {
public:
    LowPowerBatteryWake(int pin, LowPowerSetup setup, LowPowerLoop loop);

    /**
     * Sets up low battery wake.
     */
    void setup();
    void loop();

    int getIndicatorLED(){ return _led; };
    bool hasIndicatorLED(){ return _led >= 0; };

    /**
     * Put the microcontroller to sleep
     */
    void sleep();

    /**
     * Wake up from sleep mode.
     */
    void wakeUp();

    /**
     * Sets sleep for N cycles, where a cycle is the length of Sleep Time.
     * For example, setSleepTime(0,5,0) and then sleepFor(3) could sleep for 5*3=15 minutes.
     *
     * @param n The number of cycles to sleep for.
     */
    void sleepFor(int n);

    // Settings
    void setThresholds(float lower, float upper){
        if (lower > upper){
            Serial.println("ERROR: Lower threshold cannot be higher than Upper threshold. Not set.");
            return;
        }

        _threshold_L = lower;
        _threshold_H = upper;
    }

    void setIndicatorLED(int led){
        _led = led;

    }
    void setSleepTime(int hours, int minutes, int seconds){
        _hours = (long)hours;
        _minutes = (long)minutes;
        _seconds = (long)seconds;
    }

    void writeBoot();

    void inhibitSleep(){ inhibit_sleep = true; };
    void allowSleep(){ inhibit_sleep = false; };
    bool isInbihited(){ return inhibit_sleep; }

private:
    void _startSleep(); /// Called when sleep begins

    void _writeSleep(); /// Called from _startSleep. Write
    void _writeWakeup();

    float _threshold_L=3.7, _threshold_H=3.8; // Sleep at 3.7V re-wake at 3.8V
    int _led=-1;
    int _pin;

    long _hours=0, _minutes=5, _seconds=0;

    LowPowerSetup _setup;
    LowPowerLoop _loop;

    bool isSleeping;
    bool inhibit_sleep;

    SnoozeAlarm  alarm;
    SnoozeUSBSerial usb;
    SnoozeSPI spi;
#ifdef USB_AUDIO
    SnoozeAudio audio;
#endif
    SnoozeBlock config;//(usb, audio, alarm);
};


#endif //SBAT_LOWPOWERBATTERYWAKE_H
