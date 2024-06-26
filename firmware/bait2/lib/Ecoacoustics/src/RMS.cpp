//
// Created by David Kadish on 25/07/2024.
//

#include "logging.h"
#include "RMS.h"

RootMeanSquare::RootMeanSquare(AudioAnalyzeRMS &rms, const char *filepath, int interval, int measureInterval, int debugInterval) : OversamplingSensor(interval, measureInterval, debugInterval), m_rms(rms)
{
}

void RootMeanSquare::setup()
{
    Sensor::setup();

    Serial.print("Starting RMS...");
}

void RootMeanSquare::sample()
{
    // Don't worry about subsampling rate here. Just sample when available.
    if (m_rms.available())
    {
        m_rms_accumulator += m_rms.read();
        m_count++;
    }
}

void RootMeanSquare::record()
{

    float avg_rms = currentRMS();

    // Timestamp, RMS
    DEBUG("%d", avg_rms)

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

    resetCount();
}

void RootMeanSquare::debug()
{
    DEBUG("RMS: %f", currentRMS());
}

long RootMeanSquare::getCount() const
{
    return m_count;
}

void RootMeanSquare::resetCount()
{
    m_count = 0;
}