/*
 *    Created by David Kadish on 2021-03-23.
 *
 *
 *    Compute ACI for 50 wav files stored on an SD card numbered 0-49
 */

/*
 * Code to:
 * - Save a spectrogram to a csv with timing and freq bins (test_save_spectro.cpp)
 * - Compute ACI and export FFT at variable precision with frames per clump
 * -
 */

#ifdef ACI_TESTING

#warning ("Warning: Compiling aci_test.")

/// Should measurements begin immediately on boot (0) or on regularized time intervals (1)
#define START_ON_THE_HOUR 0

#include <Arduino.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include "sdcard.h"
#include <SerialFlash.h>

#include "FFTReader.h"
#include "ACI_TemporalWindow.h"
#include "AcousticComplexityIndex.h"
#include "parameters.h"
#include "print_utils.h"

void printTimestamp();
void printDirectory(File dir, int numTabs);
void printSpaces(int num);

long aciSubwindowInterval = 5L;
long soundSensingInterval = 15L * 60L;

/* AUDIO SYSTEM */
AudioPlaySdWav playSdWav;
AudioAnalyzeFFT256 aa_fft;
AudioOutputAnalog audioOutput; // Needs to be defined otherwise sound will not play?
AudioConnection pcFFT(playSdWav, aa_fft);
//AudioConnection pcOut(playSdWav, 1, audioOutput, 0);
const int n_fft = 256; // Number of FFT bins
const int bin_width = 172; // 43; Bin width in Hz

FFTReader fft = FFTReader(aa_fft, 1);

ACI_TemporalWindow aci_tw = ACI_TemporalWindow(aciSubwindowInterval, fft, true, true);

AcousticComplexityIndex aci = AcousticComplexityIndex(soundSensingInterval, aci_tw, true);

File soundfiles;

void setup() {
    delay(2000);

    Wire.begin();
    delay(2000);

    Serial.begin(9600);
    delay(2000);

    sd_setup();

    /* AUDIO Setup */
    //audio_setup();
    AudioMemory(200);

    /* END */

    fft.setup();

    aci_tw.setup();
    aci_tw.disablePrint(); // Don't print ACI_j values

    aci.setup();

    printSetupMessage();
    printTimestamp();

    Serial.println("Prepping.");
    for(int k=0; k <= 49; k++){
        String filename (String(k) + String(".WAV"));
        playSdWav.play(filename.c_str());

        Serial.print(playSdWav.isPlaying());
        Serial.print(" Playing ");
        Serial.println(filename);

        // A brief delay for the library read WAV info
        while( !playSdWav.isPlaying() ){delayMicroseconds(1);}

        // Simply wait for the file to finish playing.
        while (playSdWav.isPlaying()) {
            fft.loop();
            if (fft.available()){// && (fft.getCount() < fft.getMaxCount())) {
                aci_tw.loop();
                aci.loop();
            }
        }
        long count = fft.getCount();
        aci.process();

        Serial.print("Found ");
        Serial.print(count);
        Serial.println(" FFT values.");

        // Wait a split second for other things to finish.
        long start = millis();
        while( millis() - start < 250 ){}
    }

    Serial.println("Done.");
}

void loop() {}

#endif