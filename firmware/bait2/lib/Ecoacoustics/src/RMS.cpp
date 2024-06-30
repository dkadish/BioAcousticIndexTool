//
// Created by David Kadish on 25/07/2024.
//

#include "logging.h"
#include "RMS.h"
#include "SensorDefinitions.h"

RootMeanSquare::RootMeanSquare(AudioAnalyzeRMS &rms, const char *filepath, LoRaWANTTN *lorattn, int interval, int measureInterval, int debugInterval) : OversamplingSensor(interval, measureInterval, debugInterval), m_rms(rms), m_lwTTN(lorattn)
{
}

void RootMeanSquare::setup()
{
    Sensor::setup();

    Serial.print("Starting RMS...");
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

    float avg_rms = currentRMS() * 1000.0;
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

    int charsWritten = f.printf("%f", avg_rms);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void RootMeanSquare::debug()
{
    DEBUG("RMS: Current %f, Accumulated %f, Count %ld", currentRMS(), m_rms_accumulator, m_count);
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