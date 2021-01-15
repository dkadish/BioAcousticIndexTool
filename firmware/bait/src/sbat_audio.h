/***
 * @author David Kadish
 * @date 14/11/2018
 *
 * [1] Should only be used if we are in USB_AUDIO mode because it breaks Snooze support
 * (https://github.itu.dk/davk/ecosystem-soundscape/wiki/Standalone-Biodiversity-Assessment-Tool#sleep-mode)
 */

#ifndef SBAT_AUDIO_H
#define SBAT_AUDIO_H

#define USE_I2S

#ifdef USE_ANALOG_16
#define ANALOG_PIN 16
#endif

#include <Audio.h>

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
#ifdef USE_I2S
AudioInputI2S i2s;           //xy=148,229
#endif
#ifdef USE_ANALOG_16
AudioInputAnalog adc(ANALOG_PIN);
#endif

// See [1]
#ifdef USB_AUDIO
AudioOutputUSB usb; // must set Tools > USB Type to Audio
AudioOutputAnalog        dac; // THIS SEEMS TO BE REQUIRED
#endif

AudioFilterBiquad highpass;
AudioAnalyzeFFT256 aa_fft;
const int n_fft = 256; // Number of FFT bins
const int bin_width = 172; // 43; Bin width in Hz
AudioAmplifier amp;

// Connect either the live input or synthesized sine wave
#ifdef USE_I2S
AudioConnection     pcFilter(i2s, highpass);
#endif
#ifdef USE_ANALOG_16
AudioConnection     pcFilter(adc, highpass);
#endif
AudioConnection     pcAmp(highpass, amp);
AudioConnection     pcFFT(amp, aa_fft);
#ifdef USB_AUDIO // See [1]
AudioConnection     pcUSB_L(amp, 0, usb, 0);
AudioConnection     pcUSB_R(amp, 0, usb, 1);
#endif

void audio_setup(){
    // Audio connections require memory to work.  For more
    // detailed information, see the MemoryAndCpuUsage example
    AudioMemory(200);

    highpass.setHighpass(0, 50, 0.707);

#ifdef USE_I2S
    amp.gain(20.0);
#endif

#ifdef USE_ANALOG_16
    float gain = 1.0;
    Serial.println(gain);
    amp.gain(gain); // This fails at 100.0
    pinMode(ANALOG_PIN, INPUT);
#endif

    // Configure the window algorithm to use. DONE AUTOMATICALLY.
//    aa_fft.windowFunction(AudioWindowHanning1024);
}

#endif //SBAT_AUDIO_H
