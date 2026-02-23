//
// Created by David Kadish on 15/11/2018.
//

#ifndef SBAT_ACOUSTICCOMPLEXITYINDEX_H
#define SBAT_ACOUSTICCOMPLEXITYINDEX_H

#include "Sensor.h"
#include "ACI_TemporalWindow.h"
#include "lora.h"

class AcousticComplexityIndex : public Sensor
{
public:
    AcousticComplexityIndex(ACI_TemporalWindow &aci_tw, const char *filepath, LoRaWANTTN *lorattn, int interval, int debugInterval = 30);

    void reset() override;

    void record() override;

    void process() override;

    void debug() override;

private:
    ACI_TemporalWindow &_aci;

    LoRaWANTTN *m_lwTTN;
};

#endif // SBAT_ACOUSTICCOMPLEXITYINDEX_H
