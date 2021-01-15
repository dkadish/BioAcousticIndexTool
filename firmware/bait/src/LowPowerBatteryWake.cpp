//
// Created by David Kadish on 11/12/2018.
//

#include <SD.h>
#include "Parameters.h"
#include "LowPowerBatteryWake.h"
#include "BatteryVoltageSensor.h"

LowPowerBatteryWake::LowPowerBatteryWake(int pin, LowPowerSetup setup, LowPowerLoop loop):
#ifdef USB_AUDIO
#warning ("Warning: Compiling with USB_AUDIO. Snooze is disabled.")
        config(usb, spi, audio, alarm),
#else
#warning ("Warning: Compiling without USB_AUDIO. Listening via computer is disabled.")
        config(usb, spi, alarm),
#endif
        isSleeping(false), _pin(pin), _setup(setup), _loop(loop) {

}

void LowPowerBatteryWake::setup() {
    // Set sensor pin
    pinMode(_pin, INPUT);

    if( hasIndicatorLED() ){
        pinMode(_led, OUTPUT);
        digitalWrite(_led, HIGH);

        delay(1000);

        digitalWrite(_led, LOW);
    }

#ifdef USB_AUDIO
    Serial.println("WARNING: Compiled with USB Audio support. Sleep is disabled. Delay will be used instead. To use sleep, compile without USB Audio support.");
#else
    Serial.println("Compiled without USB Audio support. Sleep is enabled.");
#endif

    int v = analogRead(_pin);
    _voltage = calculateVoltage(v);

}

//FIXME The looping back and forth between LPBW:loop() and LPBW:Wake() will cause a recursion error eventually. Break out a sleep_loop so that this doesn't happen.
void LowPowerBatteryWake::loop() {
    // Checks whether the microcontroller should go to sleep
    int v = analogRead(_pin);
    _voltage = calculateVoltage(v);

    if ( !inhibit_sleep && (_voltage < _threshold_L)) {
        sleep();
    }

}

void LowPowerBatteryWake::_startSleep() {
//    SIM_SCGC6 &= ~SIM_SCGC6_I2S; // Turn off the I2S Clock

    _writeSleep();
}

void LowPowerBatteryWake::sleep() {
    // Indicates sleeping, runs sleep loop, checks whether microcontroller should wake up.
#ifndef USB_AUDIO
    int who;
#endif

    // Start sleeping process.
    _startSleep();

    // While sleep conditions remain
    while(_voltage < _threshold_H){
        // Indicate Sleeping
        if(hasIndicatorLED()){
            digitalWrite(_led, HIGH);
            delay(10);
            digitalWrite(_led, LOW);
        }

        if(hasIndicatorLED()){digitalWrite(_led, HIGH);}
#ifndef USB_AUDIO
        alarm.setRtcTimer(_hours, _minutes, _seconds); // Hour, minute, second
        who = Snooze.sleep( config );

        // Sync the clock so that the time is correctly logged.
        setTime(Teensy3Clock.get());
#else
        Serial.println("WARNING: Sleeping via delay.");
        delay(_hours*60L*60L*1000L + _minutes*60L*1000L + _seconds*1000L);
#endif

        // Run the low power loop passed to this object
        _loop();

        // Measure Voltage
        int v = analogRead(_pin);
        _voltage = calculateVoltage(v);
    }

    if(hasIndicatorLED()){digitalWrite(_led, LOW);}

#ifndef USB_AUDIO
    delay(1000);
    Serial.printf("Woken up by %i\n", who);
#endif
    wakeUp();
}

void LowPowerBatteryWake::wakeUp() {

//    SIM_SCGC6 |= SIM_SCGC6_I2S; // Turn on the I2S Clock

    // Force re-sync the clock to the RTC
    setTime(Teensy3Clock.get());

    isSleeping = false;

    // TODO Do I need to do something with the Serial and Audio things here?
    _writeWakeup();

    _setup();
}

void LowPowerBatteryWake::writeBoot() {
    Serial.print(year());
    Serial.print("-");
    Serial.print(month());
    Serial.print("-");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(hour());
    Serial.print(":");
    if (minute() < 10)
        Serial.print('0');
    Serial.print(minute());
    Serial.print(":");
    if (second() < 10)
        Serial.print('0');
    Serial.print(second());
    Serial.print(", boot, ");
    Serial.print(_voltage, PRECISION);
    Serial.println();

    File dataFile = SD.open("sleep.csv", FILE_WRITE);
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
        dataFile.print(", boot, ");

        // Frequency Bins
        dataFile.print(_voltage, PRECISION);

        dataFile.println();
        dataFile.close();
    } else {
    }
}

void LowPowerBatteryWake::_writeSleep() {
    Serial.print(year());
    Serial.print("-");
    Serial.print(month());
    Serial.print("-");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(hour());
    Serial.print(":");
    if (minute() < 10)
        Serial.print('0');
    Serial.print(minute());
    Serial.print(":");
    if (second() < 10)
        Serial.print('0');
    Serial.print(second());
    Serial.print(", sleep, ");
    Serial.print(_voltage, PRECISION);
    Serial.println();

    File dataFile = SD.open("sleep.csv", FILE_WRITE);
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
        dataFile.print(", sleep, ");

        // Frequency Bins
        dataFile.print(_voltage, PRECISION);

        dataFile.println();
        dataFile.close();
    } else {
    }
}

void LowPowerBatteryWake::_writeWakeup() {
    Serial.print(year());
    Serial.print("-");
    Serial.print(month());
    Serial.print("-");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(hour());
    Serial.print(":");
    if (minute() < 10)
        Serial.print('0');
    Serial.print(minute());
    Serial.print(":");
    if (second() < 10)
        Serial.print('0');
    Serial.print(second());
    Serial.print(", wake, ");
    Serial.print(_voltage, PRECISION);
    Serial.println();

    File dataFile = SD.open("sleep.csv", FILE_WRITE);
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
        dataFile.print(", wake, ");

        // Frequency Bins
        dataFile.print(_voltage);

        dataFile.println();
        dataFile.close();
    } else {
    }
}

void LowPowerBatteryWake::sleepFor(int n) {
#ifndef USB_AUDIO
    int who;
#endif

    // Start sleeping process.
    _startSleep();

    // While sleep conditions remain
    for(int i=0; i < n; i++){
        // Indicate Sleeping
        if(hasIndicatorLED()){
            digitalWrite(_led, HIGH);
            delay(10);
            digitalWrite(_led, LOW);
        }

        if(hasIndicatorLED()){digitalWrite(_led, HIGH);}
#ifndef USB_AUDIO
        alarm.setRtcTimer(_hours, _minutes, _seconds); // Hour, minute, second
        who = Snooze.sleep( config );

        // Sync the clock so that the time is correctly logged.
        setTime(Teensy3Clock.get());
#else
        Serial.println("WARNING: Sleeping via delay.");
        delay(_hours*60L*60L*1000L + _minutes*60L*1000L + _seconds*1000L);
#endif

        // Run the low power loop passed to this object
        _loop();
    }

    if(hasIndicatorLED()){digitalWrite(_led, LOW);}

#ifndef USB_AUDIO
    delay(1000);
    Serial.printf("Woken up by %i\n", who);
#endif
    wakeUp();
}
