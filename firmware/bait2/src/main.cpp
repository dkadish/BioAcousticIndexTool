#include <Arduino.h>
#include "baitconfig.h"
#include "sdcard.h"
#include "logging.h"
#include "PowerSensor.h"
// #include "OLEDDisplay.h"
#include "FFTReader.h"
#include "RMS.h"

#include <Audio.h>
#include <SPI.h>
/****************** AUDIO */
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S i2s;           // xy=242,151
AudioOutputUSB usb;          // xy=602,297
AudioAnalyzeFFT256 fft256_l; // xy=624,76
AudioAnalyzeFFT256 fft256_r; // xy=627,132
AudioAnalyzeRMS rms_l;       // xy=630,190
AudioAnalyzeRMS rms_r;       // xy=621,244
AudioConnection patchCord1(i2s, 0, fft256_l, 0);
AudioConnection patchCord2(i2s, 0, usb, 0);
AudioConnection patchCord3(i2s, 0, rms_l, 0);
AudioConnection patchCord4(i2s, 1, fft256_r, 0);
AudioConnection patchCord5(i2s, 1, usb, 1);
AudioConnection patchCord6(i2s, 1, rms_r, 0);
AudioControlSGTL5000 sgtl5000; // xy=412,358
// GUItool: end automatically generated code

PowerSensor powerSensor = PowerSensor(10, "/power.csv", 4400);
FFTReader fftReader = FFTReader(fft256_l, "/fft.csv", false, 2, -1);
RootMeanSquare rms = RootMeanSquare(rms_l, "/rms.csv");

// OLEDDisplay display = OLEDDisplay();0

void audioSetup()
{
    AudioMemory(20);
    sgtl5000.enable();
    sgtl5000.volume(0.5);
}

void setup()
{
#ifdef USE_STATUS_LED
    // Indicator LED
    pinMode(LED_BUILTIN, OUTPUT);
    LEDHIGH
#endif

    audioSetup();

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
    fftReader.setup();
    rms.setup();

    DEBUG("Setup Complete.")
}

int v = 0;
// ̆̆
void loop()
{
    powerSensor.loop();
    fftReader.loop();
    rms.loop();

    // oledLoop();
}

// **************************************************************************************************
// *  Helper functions for printing the directory structure

void printSpaces(int num)
{
    for (int i = 0; i < num; i++)
    {
        Serial.print(" ");
    }
}

void printTime(const DateTimeFields tm)
{
    const char *months[12] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"};
    if (tm.hour < 10)
        Serial.print('0');
    Serial.print(tm.hour);
    Serial.print(':');
    if (tm.min < 10)
        Serial.print('0');
    Serial.print(tm.min);
    Serial.print("  ");
    Serial.print(tm.mon < 12 ? months[tm.mon] : "???");
    Serial.print(" ");
    Serial.print(tm.mday);
    Serial.print(", ");
    Serial.print(tm.year + 1900);
}

void printDirectory(File dir, int numSpaces)
{
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            // Serial.println("** no more files **");
            break;
        }
        printSpaces(numSpaces);
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numSpaces + 2);
        }
        else
        {
            // files have sizes, directories do not
            int n = log10f(entry.size());
            if (n < 0)
                n = 10;
            if (n > 10)
                n = 10;
            printSpaces(50 - numSpaces - strlen(entry.name()) - n);
            Serial.print("  ");
            Serial.print(entry.size(), DEC);
            DateTimeFields datetime;
            if (entry.getModifyTime(datetime))
            {
                printSpaces(4);
                printTime(datetime);
            }
            Serial.println();
        }
        entry.close();
    }
}

void serialEvent()
{
    while (Serial.available())
    {
        // get the new byte:
        char inChar = (char)Serial.read();

        if (inChar == 'F')
        {
            File root = SD.open("/");
            Serial.println("Listing directory: /");
            printDirectory(root, 0);
        }
    }
}