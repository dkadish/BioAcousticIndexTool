//
// Created by David Kadish on 09/11/2018.
//

#ifndef TESTING
#warning ("Warning: Compiling main.")

/// Should measurements begin immediately on boot (0) or on regularized time intervals (1)
#define START_ON_THE_HOUR 0

#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include "bait_audio.h"
#include "sdcard.h"
#include <SerialFlash.h>

#include <TimeAlarms.h>

#include "BatteryVoltageSensor.h"
#include "FFTReader.h"
#include "ACI_TemporalWindow.h"
#include "AcousticComplexityIndex.h"
#include "AcousticDiversityIndex.h"
#include "AcousticDifferenceIndex.h"
#include "AtmosphericSensors.h"
#include "LowPowerBatteryWake.h"
#include "parameters.h"

void printTimestamp();

long environmentSensingInterval = 5L * 60L; // in seconds
long aciSubwindowInterval = 30L;
long soundSensingInterval = 15L * 60L;

#ifndef ARDUINO_TEENSY_MICROMOD
BatteryVoltageSensor batt = BatteryVoltageSensor(environmentSensingInterval, A17);
#endif

const int averageTogether = 1; // Don't average FFT values
FFTReader fft = FFTReader(aa_fft, averageTogether);

ACI_TemporalWindow aci_tw = ACI_TemporalWindow(aciSubwindowInterval, fft);

AcousticComplexityIndex aci = AcousticComplexityIndex(soundSensingInterval, aci_tw);
AcousticDiversityIndex adi = AcousticDiversityIndex(soundSensingInterval, fft);
AcousticDifferenceIndex diff = AcousticDifferenceIndex(soundSensingInterval, fft);
Metro sleepTimer(soundSensingInterval*1000L+1L);

AtmosphericSensors atm = AtmosphericSensors(environmentSensingInterval);

time_t getTeensy3Time()
{
    return Teensy3Clock.get();
}
void time_loop();
unsigned long processSyncMessage();

void setup_lowpower();
void loop_lowpower();
#ifndef ARDUINO_TEENSY_MICROMOD
LowPowerBatteryWake lowPowerBatteryWake = LowPowerBatteryWake(A17, setup_lowpower, loop_lowpower);
#endif

void setup() {
    delay(2000);

    Wire.begin();
    delay(2000);
    setSyncProvider(getTeensy3Time);
    setSyncInterval(60*60); // Sync with the RTC every hour

    Serial.begin(9600);
    delay(2000);

    sd_setup();
    audio_setup();

#ifndef ARDUINO_TEENSY_MICROMOD
    batt.setVoltageDividers(22e3, 22e3);
    batt.setup();
    batt.loop();
#endif

    fft.setup();

    aci_tw.setup();

    aci.setup();
    adi.setup();
    diff.setup();

    atm.setup();

    Serial.print("Setup complete. Using Firmware at ");
    Serial.println(STRINGIFY(PIO_SRC_REV));

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
    Serial.println(second());

#ifndef ARDUINO_TEENSY_MICROMOD
    lowPowerBatteryWake.setThresholds(3.7, 3.9);
    lowPowerBatteryWake.setVoltageDividers(22e3, 22e3);
    lowPowerBatteryWake.setSleepTime(0,5,0);
//    lowPowerBatteryWake.setIndicatorLED(LED_BUILTIN);
    lowPowerBatteryWake.setup();
    lowPowerBatteryWake.writeBoot();

    // Testing
//    lowPowerBatteryWake.inhibitSleep();
#endif

    sleepTimer.reset();
}

void loop() {

#ifndef ARDUINO_TEENSY_MICROMOD
    lowPowerBatteryWake.loop();

    batt.loop();
#endif
    atm.loop();

    // FFT dependent loops
    fft.loop();
    if(fft.available()) {
        aci_tw.loop();
        aci.loop();
        adi.loop();
        diff.loop();
    }

    // Don't sleep if we are already substantially into the recording of a new audio sample
//    if( aci_tw.getCount() > 1 ){
//        lowPowerBatteryWake.inhibitSleep();
//    } else {
//        lowPowerBatteryWake.allowSleep();
//    }
    // Testing
//    lowPowerBatteryWake.inhibitSleep();

    if(sleepTimer.check()){
        Serial.println("Sleeping for 3...");
        delay(100);
#ifndef ARDUINO_TEENSY_MICROMOD
        lowPowerBatteryWake.sleepFor(3);
#endif
        sleepTimer.reset();
    }

    time_loop();
}

/**
 * Wake back up from low power sleep
 */
void setup_lowpower(){
    // Reset all of the sensors/metronomes
#ifndef ARDUINO_TEENSY_MICROMOD
    batt.reset();
#endif
    atm.reset();

    aci_tw.reset();
    aci.reset();
    adi.reset();
    diff.reset();
}

void loop_lowpower(){
    // Loop to get averaged measurements
    for(int i=0; i < 10; i++){
#ifndef ARDUINO_TEENSY_MICROMOD
        batt.loop();
#endif
        atm.loop();
        delay(10);
    }

    // Process and write values
#ifndef ARDUINO_TEENSY_MICROMOD
    batt.process();
#endif
    atm.process();
}

void time_loop(){
    if (Serial.available()) {
        time_t t = processSyncMessage();
        if (t != 0) {
            Teensy3Clock.set(t); // set the RTC
            setTime(t);
        }
    }
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TZ_ADJ 1 // CEST

unsigned long processSyncMessage() {
    unsigned long pctime = 0L;
    const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

    if(Serial.find(TIME_HEADER)) {
        pctime = Serial.parseInt();
        pctime += TZ_ADJ*60*60;
        return pctime;
        if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
            pctime = 0L; // return 0 to indicate that the time is not valid
        }
    }
    return pctime;
}

void printTimestamp(){
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
}

#endif