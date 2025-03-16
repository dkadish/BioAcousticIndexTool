//
// Created by David Kadish on 15/11/2018.
//

#include <SD.h>
#include "AcousticComplexityIndex.h"
#include "parameters.h"
#include "logging.h"
#include "SensorDefinitions.h"

AcousticComplexityIndex::AcousticComplexityIndex(ACI_TemporalWindow &aci_tw, const char *filepath, LoRaWANTTN *lorattn, int interval, int debugInterval = 30) : Sensor(interval, debugInterval), _aci(aci_tw), m_lwTTN(lorattn)
{
}

void AcousticComplexityIndex::reset()
{
    Sensor::reset();

    _aci.zero();
}

void AcousticComplexityIndex::record()
{
    float aci = _aci.getValue();
    reset();

    // ACI
    DEBUG("ACI: %f, Windows count: %d\n", aci, _aci.getCount());

    // Record LoRaWAN data
    m_lwTTN->getLPP().addAnalogInput(ACOUSTIC_COMPLEXITY_INDEX, aci);

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

    int charsWritten = f.printf("%f", aci);
    f.println();
    f.close();

    digitalWrite(LED_BUILTIN, LOW);
}

void AcousticComplexityIndex::process() {}

void AcousticComplexityIndex::debug()
{
    Sensor::debug();

    DEBUG("ACI: %f, Windows count: %d", _aci.getValue(), _aci.getCount());
}