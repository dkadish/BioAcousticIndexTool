#include <Arduino.h>
#include "baitconfig.h"
#include "sdcard.h"
#include "logging.h"
#include "lora.h"
#include "PowerSensor.h"
// #include "OLEDDisplay.h"
#include "FFTReader.h"
#include "BME680.h"
#include "RMS.h"

#include <Audio.h>
#include <SPI.h>
/****************** AUDIO */
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// NOTE: MK1 seems to be much more sensitiv and stable than mk2. That is the one the is LEFT in the code
//  GUItool: begin automatically generated code
AudioInputI2S i2s;           // xy=139,174
AudioAmplifier amp_l;        // xy=292,156
AudioAnalyzeFFT256 fft256_l; // xy=568,129
AudioAnalyzePeak peak_l;     // xy=569,55
AudioAnalyzeRMS rms_l;       // xy=568,92
// AudioAmplifier amp_r;        // xy=294,202
// AudioAnalyzeFFT256 fft256_r; // xy=557,271
// AudioAnalyzePeak peak_r;     // xy=560,193
// AudioAnalyzeRMS rms_r;       // xy=558,231
AudioOutputUSB usb; // xy=561,385
AudioConnection patchCord1(i2s, 0, amp_l, 0);
AudioConnection patchCord3(amp_l, rms_l);
AudioConnection patchCord4(amp_l, peak_l);
AudioConnection patchCord5(amp_l, fft256_l);
AudioConnection patchCord6(amp_l, 0, usb, 0);
AudioConnection patchCord11(amp_l, 0, usb, 1);
// AudioConnection patchCord2(i2s, 1, amp_r, 0);
// AudioConnection patchCord7(amp_r, peak_r);
// AudioConnection patchCord8(amp_r, rms_r);
// AudioConnection patchCord9(amp_r, fft256_r);
// AudioConnection patchCord10(amp_r, 0, usb, 1);
AudioControlSGTL5000 sgtl5000; // xy=700,563
// GUItool: end automatically generated code

float gain_l = 32.0; // Seems to be a good default value
// float gain_r = 1.0;

LoRaWANTTN lora = LoRaWANTTN();

PowerSensor powerSensor = PowerSensor(5L * 60L, "/power.csv", &lora, 4400);
FFTReader fftReader = FFTReader(fft256_l, "/fft.csv", false, 2, -1);
EnvironmentalSensor envSensor = EnvironmentalSensor("/env.csv", &lora, 5 * 60);
RootMeanSquare rms = RootMeanSquare(rms_l, "/rms.csv", &lora, 5L * 60L, 0, -1);

// OLEDDisplay display = OLEDDisplay();0

void audioSetup()
{
    AudioMemory(20);
    sgtl5000.enable();
    sgtl5000.volume(0.5);

    amp_l.gain(gain_l);
    // amp_r.gain(gain_r);
}

void setup()
{
#ifdef USE_STATUS_LED
    // Indicator LED
    pinMode(LED_BUILTIN, OUTPUT);
    LEDHIGH
#endif

    audioSetup();

    Serial.begin(9600);

#ifdef WAIT_FOR_SERIAL
    for (int i = 0; i < 10 && !Serial; i++)
    {
        Serial.begin(9600);
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

    lora.setup();
    powerSensor.setup();
    // display.setup();
    fftReader.setup();
    envSensor.setup();
    rms.setup();

    DEBUG("Setup Complete.")
}

int v = 0;
// ̆̆
void loop()
{
    powerSensor.loop();
    fftReader.loop();
    envSensor.loop();
    rms.loop();

    // oledLoop();

    lora.loop();
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
        else if (inChar == 'G')
        {
            gain_l *= 2.0;
            amp_l.gain(gain_l);
            Serial.printf("Gain L: %f\n", gain_l);
        }
        else if (inChar == 'g')
        {
            gain_l /= 2.0;
            amp_l.gain(gain_l);
            Serial.printf("Gain L: %f\n", gain_l);
        }
        // else if (inChar == 'H')
        // {
        //     gain_r *= 2.0;
        //     amp_r.gain(gain_r);
        //     Serial.printf("Gain R: %f\n", gain_r);
        // }
        // else if (inChar == 'h')
        // {
        //     gain_r /= 2.0;
        //     amp_r.gain(gain_r);
        //     Serial.printf("Gain R: %f\n", gain_r);
        // }
    }
}