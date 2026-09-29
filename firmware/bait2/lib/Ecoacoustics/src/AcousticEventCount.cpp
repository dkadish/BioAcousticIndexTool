//
// Acoustic event count sensor. See EcoacousticMath.h for the definition.
//

#include "AcousticEventCount.h"
#include "logging.h"
#include "SensorDefinitions.h"

AcousticEventCount::AcousticEventCount(FFTReader &fft, const char *filepath, LoRaWANTTN *lorattn, int interval,
                                       float minFreq, float maxFreq, float thresholdDb, int debugInterval)
    : Sensor(interval, filepath, debugInterval), _fft(fft),
      m_events(AUDIO_SAMPLE_RATE_EXACT, minFreq, maxFreq, thresholdDb), m_lwTTN(lorattn)
{
}

void AcousticEventCount::setup()
{
    Sensor::setup();

    reset();
}

void AcousticEventCount::loop()
{
    Sensor::loop();

    if (_fft.available())
    {
        m_events.addFrame(_fft.getSpectrum());
    }
}

void AcousticEventCount::reset()
{
    Sensor::reset();

    m_events.reset();
}

/** Closes the interval: counts the events and clears the accumulator for the next one. */
void AcousticEventCount::process()
{
    Sensor::process();

    m_value = m_events.value();
    m_background = m_events.backgroundDb();
    m_frames = m_events.getFrameCount();
    m_events.reset();
}

void AcousticEventCount::record()
{
    DEBUG("Events: %ld, Background: %f dB, Frames: %ld", m_value, m_background, m_frames);

    // CayenneLPP analog inputs saturate at 327.67, so send events per second
    // rather than the raw count (which also depends on the interval length).
    float perSecond = getInterval() > 0 ? (float)m_value / (float)getInterval() : 0.0f;
    m_lwTTN->getLPP().addAnalogInput(ACOUSTIC_EVENT_RATE, perSecond);

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

    f.printf("%ld, %f, %ld", m_value, m_background, m_frames);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void AcousticEventCount::debug()
{
    Sensor::debug();

    DEBUG("Events (so far): %ld, Frames: %ld", m_events.value(), m_events.getFrameCount());
}
