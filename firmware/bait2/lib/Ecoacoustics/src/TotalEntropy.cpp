//
// Total acoustic entropy (Htf) sensor. See EcoacousticMath.h for the definition.
//

#include "TotalEntropy.h"
#include "logging.h"
#include "SensorDefinitions.h"

TotalEntropy::TotalEntropy(FFTReader &fft, const char *filepath, LoRaWANTTN *lorattn, int interval, int debugInterval)
    : Sensor(interval, filepath, debugInterval), _fft(fft), m_lwTTN(lorattn)
{
}

void TotalEntropy::setup()
{
    Sensor::setup();

    reset();
}

void TotalEntropy::loop()
{
    Sensor::loop();

    if (_fft.available())
    {
        m_h.addFrame(_fft.getSpectrum());
    }
}

void TotalEntropy::reset()
{
    Sensor::reset();

    m_h.reset();
}

/** Closes the interval: computes the entropies and clears the accumulator for the next one. */
void TotalEntropy::process()
{
    Sensor::process();

    m_ht = m_h.temporalEntropy();
    m_hf = m_h.spectralEntropy();
    m_value = m_ht * m_hf;
    m_frames = m_h.getFrameCount();
    m_h.reset();
}

void TotalEntropy::record()
{
    DEBUG("H: %f (Ht %f, Hf %f), Frames: %ld", m_value, m_ht, m_hf, m_frames);

    // Record LoRaWAN data
    m_lwTTN->getLPP().addAnalogInput(TOTAL_ENTROPY, m_value);

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

    f.printf("%f, %f, %f, %ld", m_value, m_ht, m_hf, m_frames);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void TotalEntropy::debug()
{
    Sensor::debug();

    DEBUG("H (so far): %f, Frames: %ld", m_h.value(), m_h.getFrameCount());
}
