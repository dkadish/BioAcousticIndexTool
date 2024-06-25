//
// Created by David Kadish on 14/11/2018.
//

#include "logging.h"
#include "FFTReader.h"

// FFTReader::FFTReader(AudioAnalyzeFFT256 &fft) : Sensor(0), _fft(fft), _available(false) {}

FFTReader::FFTReader(AudioAnalyzeFFT256 &fft, const char *filepath, bool recordSpectrogram, int averageTogether, int debugInterval) : Sensor(0, filepath, debugInterval),
                                                                                                                                      _fft(fft), _available(false), _record_spectrogram(recordSpectrogram), _avg_together(averageTogether) {}

void FFTReader::loop()
{
    Sensor::loop();
    //    if(_fft.available()){
    //        Serial.println("AA_FFT is available here too!");
    //    }

    if (_fft.available())
    {
        //        Serial.println("FFT Available");
        for (int i = 0; i < length; i++)
        {
            fft[i] = _fft.read(i);

#ifdef SAVE_SPECTROGRAM
            if (((int)getCount()) < _max_count)
            {
                _spectrogram[i][(int)getCount()] = fft[i];
            }
#endif
            //            Serial.printf("FFT %i is ", i);
            //            Serial.println(fft[i]);
        }
        _count += 1;
        _available = true;

        if (_record_spectrogram)
        {
            recordFFT();
        }
    }
    else
    {
        _available = false;
    }
}

float FFTReader::get(int i)
{
    return fft[i];
}

void FFTReader::setup()
{
    Sensor::setup();

    Serial.print("Starting FFT with ");
    Serial.print(_avg_together);
    Serial.println(" values averaged together.");
    _fft.averageTogether(_avg_together);
    _fft.windowFunction(AudioWindowHamming256);

#ifdef SAVE_SPECTROGRAM
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < _max_count; j++)
        {
            _spectrogram[i][j] = 0;
        }
    }
#endif
}

void FFTReader::recordFFT()
{

    digitalWrite(LED_BUILTIN, HIGH);

    //    File f = SD.open(getFilePath(), FILE_WRITE);
    FsFile f;
    if (!f.open(getFilePath(), O_WRITE | O_CREAT | O_AT_END)) // O_CREAT | O_APPEND))
    {
        DEBUG("Error opening file")
        digitalWrite(LED_BUILTIN, LOW);
        return;
    }
    Sensor::writeTimestamp(&f);

    for (int i = 0; i < FFTReader::length; i++)
    {
        f.printf(", %f", fft[i]);
    }
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void FFTReader::debug()
{
    DEBUG("%f, %f, %f, %f, %f, %f, %f", fft[1], fft[3], fft[7], fft[15], fft[31], fft[63], fft[127])
}

long FFTReader::getCount() const
{
    return _count;
}

void FFTReader::resetCount()
{
    _count = 0;
}