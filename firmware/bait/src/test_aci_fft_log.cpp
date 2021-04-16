/*
 *    Created by David Kadish on 2021-03-23.
 *
 *
 *    Compute ACI and export FFT to CSV with a count of the number of frames per clump
 */

#ifdef TEST_ACI_FFT_LOG

#define ACI_TW_LOG_VALUES

#warning ("Compiling test_aci_fft_log. If you are looking for the BAIT firmware, please compile main.cpp.")


#include <Arduino.h>

#include <Audio.h>
#include <Wire.h>
#include "sdcard.h"

#include "FFTReader.h"
#include "ACI_TemporalWindow.h"
#include "AcousticComplexityIndex.h"
#include "parameters.h"
#include "print_utils.h"

void printSpaces(int num);
void printACILogHeader();

const int SPECTRO_PRECISION = 6;
long aciSubwindowInterval = 5L;
long soundSensingInterval = 15L * 60L;

/* AUDIO SYSTEM */
AudioPlaySdWav playSdWav;
AudioAnalyzeFFT256 aa_fft;
AudioOutputAnalog audioOutput;
AudioConnection pcFFT(playSdWav, aa_fft);
//AudioConnection pcOut(playSdWav, 1, audioOutput, 0);
const int n_fft = 256; // Number of FFT bins
const int avgTogether = 8;

FFTReader fft = FFTReader(aa_fft, avgTogether);

ACI_TemporalWindow aci_tw = ACI_TemporalWindow(aciSubwindowInterval, fft);

AcousticComplexityIndex aci = AcousticComplexityIndex(soundSensingInterval, aci_tw);

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
//    aci_tw.disablePrint();

    aci.setup();

    printSetupMessage();
    printTimestamp();

    // Printing ACI log header
    printACILogHeader();

    Serial.println("Prepping.");
    String filename ("6.WAV");
    playSdWav.play(filename.c_str());

    Serial.print("Playing ");
    Serial.println(filename);

    // A brief delay for the library read WAV info
    while( !playSdWav.isPlaying() ){delayMicroseconds(1);}

    int start_time=0;
    int t=0;

    while (playSdWav.isPlaying()) {
        fft.loop();
        if (fft.available()){// && (fft.getCount() < fft.getMaxCount())) {
            aci_tw.loop();
            aci.loop();

            // Write the latest value to the SD card
            AudioNoInterrupts();
            File dataFile = SD.open("fft.csv", FILE_WRITE | O_APPEND);
            if (dataFile) {
                for( int i=0; i<fft.length; i++){
                    dataFile.print(fft.get(i), SPECTRO_PRECISION);
                    dataFile.print(",");
                }
                dataFile.println();
                dataFile.close();
            }
            AudioInterrupts();
        }
    }
    aci.process();

    Serial.print("Found ");
    Serial.print(fft.getCount());
    Serial.println(" FFT values.");

    Serial.println("Done.");
}

void loop() {
    // Do nothing here. Just run once and then stop.
}


void printSpaces(int num) {
    for (int i=0; i < num; i++) {
        Serial.print(" ");
    }
}

void printACILogHeader(){
    File dataFile = SD.open("aci_tw.csv", FILE_WRITE | O_APPEND);
    if (dataFile) {
        dataFile.println("frame, ACI_j");
        dataFile.close();
    } else {
        Serial.println("ACI_TW: Can't open file.");
    }
}

#endif