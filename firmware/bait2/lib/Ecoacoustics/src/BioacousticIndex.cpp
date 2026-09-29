//
// Bioacoustic Index sensor. See EcoacousticMath.h for the definition.
//

#include "BioacousticIndex.h"
#include "logging.h"
#include "SensorDefinitions.h"

BioacousticIndex::BioacousticIndex(FFTReader &fft, const char *filepath, LoRaWANTTN *lorattn, int interval,
                                   float minFreq, float maxFreq, int debugInterval)
    : Sensor(interval, filepath, debugInterval), _fft(fft), m_bi(AUDIO_SAMPLE_RATE_EXACT, minFreq, maxFreq), m_lwTTN(lorattn)
{
}

void BioacousticIndex::setup()
{
    Sensor::setup();

    reset();
}

void BioacousticIndex::loop()
{
    Sensor::loop();

    if (_fft.available())
    {
        m_bi.addFrame(_fft.getSpectrum());
    }
}

void BioacousticIndex::reset()
{
    Sensor::reset();

    m_bi.reset();
}

/** Closes the interval: computes the index and clears the accumulator for the next one. */
void BioacousticIndex::process()
{
    Sensor::process();

    m_value = m_bi.value();
    m_frames = m_bi.getFrameCount();
    m_bi.reset();
}

void BioacousticIndex::record()
{
    DEBUG("BI: %f, Frames: %ld", m_value, m_frames);

    // Record LoRaWAN data
    m_lwTTN->getLPP().addAnalogInput(BIOACOUSTIC_INDEX, m_value);

    // Mark that there is data to send
    m_lwTTN->setDirty();

    // Record to SD card
    digitalWrite(LED_BUILTIN, HIGH);

    FsFile f;
    if (!f.open(getFilePath(), O_WRITE | O_CREAT | O_AT_END))
    {
        DEBUG("Error opening file")
        digitalWrite(LED_BUILTIN, LOW);
        return;
    }
    Sensor::writeTimestamp(&f);

    f.printf("%f, %ld", m_value, m_frames);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void BioacousticIndex::debug()
{
    Sensor::debug();

    DEBUG("BI (so far): %f, Frames: %ld", m_bi.value(), m_bi.getFrameCount());
}
