/*
 * This file contains the implementation of the RootMeanSquare class, which is responsible for
 * calculating the root mean square (RMS) value of audio signals. The class interfaces with
 * an AudioAnalyzeRMS object to read RMS values and records the data to an SD card and a LoRaWAN
 * network. The class also provides debugging information and handles the setup and reset of the
 * RMS calculations.
 *
 * Created by David Kadish on 25/07/2024.
 */

#include "logging.h"
#include "RMS.h"
#include "SensorDefinitions.h"

RootMeanSquare::RootMeanSquare(AudioAnalyzeRMS &rms, const char *filepath, LoRaWANTTN *lorattn, int interval, int measureInterval, int debugInterval)
    : OversamplingSensor(interval, measureInterval, debugInterval), m_rms(rms), m_lwTTN(lorattn)
{
}

void RootMeanSquare::setup()
{
    Sensor::setup();

    DEBUG("Starting RMS...")
}

bool RootMeanSquare::sample()
{
    // Don't worry about subsampling rate here. Just sample when available.
    if (m_rms.available())
    {
        m_rms_accumulator += m_rms.read();
        m_count++;

        return true;
    }

    return false;
}

void RootMeanSquare::record()
{

    float avg_rms = currentRMS();
    reset();

    // Timestamp, RMS
    DEBUG("RMS (for period): %f", avg_rms)

    // Record LoRaWAN data
    m_lwTTN->getLPP().addAnalogInput(ROOT_MEAN_SQUARE, avg_rms);

    // Mark that there is data to send
    m_lwTTN->setDirty();

    // Record to SD card

    digitalWrite(LED_BUILTIN, HIGH);

    FsFile f;
    if (!f.open(getFilePath(), O_WRITE | O_CREAT | O_AT_END)) // O_CREAT | O_APPEND))
    {
        DEBUG("Error opening file")
        digitalWrite(LED_BUILTIN, LOW);
        return;
    }
    Sensor::writeTimestamp(&f);

    int charsWritten = f.printf("%d", (int)avg_rms);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void RootMeanSquare::debug()
{
    float avg_rms = currentRMS();
    DEBUG("RMS: Current %f, Accumulated %f, Count %ld", avg_rms, m_rms_accumulator, m_count);
}

long RootMeanSquare::getCount() const
{
    return m_count;
}

void RootMeanSquare::reset()
{
    m_count = 0;
    m_rms_accumulator = 0.0;
}