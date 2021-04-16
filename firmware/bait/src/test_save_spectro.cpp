/*
 *    Created by David Kadish on 2021-03-22.
 *
 *
 *    Compute and save the first fft.getMaxCount() frames of an FFT for comparison with other FFT functions.
 */

#ifdef TEST_SAVE_SPECTRO

#warning ("Compiling test_save_spectro. If you are looking for the BAIT firmware, please compile main.cpp.")


#include <Arduino.h>

#include <Audio.h>
#include <Wire.h>
#include "sdcard.h"

#include "FFTReader.h"
#include "ACI_TemporalWindow.h"
#include "AcousticComplexityIndex.h"
#include "parameters.h"

void printTimestamp();
void printSpaces(int num);

const int SPECTRO_PRECISION = 8;
long aciSubwindowInterval = 5L;
long soundSensingInterval = 15L * 60L;

/* AUDIO SYSTEM */
AudioPlaySdWav playSdWav;
AudioAnalyzeFFT256 aa_fft;
AudioOutputAnalog audioOutput;
AudioConnection pcFFT(playSdWav, aa_fft);
//AudioConnection pcOut(playSdWav, 1, audioOutput, 0);
const int n_fft = 256; // Number of FFT bins
const int bin_width = 172; // 43; Bin width in Hz

FFTReader fft = FFTReader(aa_fft);

//ACI_TemporalWindow aci_tw = ACI_TemporalWindow(aciSubwindowInterval, fft);
//
//AcousticComplexityIndex aci = AcousticComplexityIndex(soundSensingInterval, aci_tw);

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

//    aci_tw.setup();
////    aci_tw.disablePrint();
//
//    aci.setup();

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

    Serial.println("Prepping.");
    String filename ("6.WAV");
    playSdWav.play(filename.c_str());

//    // A brief delay for the library read WAV info
//    delay(25);

    Serial.print("Playing ");
    Serial.println(filename);

    // Wait for the file to start playing
    while( !playSdWav.isPlaying() ){delayMicroseconds(1);}

    int start_time=0;
    int time_log[fft.getMaxCount()];
    int t=0;

    while (playSdWav.isPlaying()) {
        fft.loop();
        if (fft.available() && (fft.getCount() < fft.getMaxCount())) {
//            aci_tw.loop();
//            aci.loop();
            if( t==0 ){
                start_time = millis();
            }
            time_log[t] = millis() - start_time;
            t++;
        } else if (fft.getCount() >= fft.getMaxCount()){ // Only play until the fft buffer is full.
            playSdWav.stop();
        }
    }
//    aci.process();

    Serial.print("Found ");
    Serial.print(fft.getCount());
    Serial.println(" FFT values.");

    Serial.println("Saving Spectrogram.");
    File dataFile = SD.open("spectro.csv", FILE_WRITE); //O_APPEND
    if (dataFile) {

        // Print timing
        for( int i=0; i<fft.getMaxCount(); i++){
            dataFile.print(time_log[i]);
            dataFile.print(",");
        }
        dataFile.println();

        // Print ffts
        for( int i=0; i<fft.length; i++){
            dataFile.print(fft.getSpectro(i,0), SPECTRO_PRECISION);
            dataFile.print(",");
            for(int j=1; j<fft.getCount() && j<fft.getMaxCount(); j++){
                dataFile.print(fft.getSpectro(i,j), SPECTRO_PRECISION);
                dataFile.print(",");
            }
            dataFile.println();
            Serial.print(".");
        }
        dataFile.close();
    } else {
        Serial.println("SPECTRO: Can't open file.");
    }

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

#endif