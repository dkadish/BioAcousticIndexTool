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
    AcousticComplexityIndex(int interval, ACI_TemporalWindow &aci_tw, const char *filepath, LoRaWANTTN *lorattn, boolean printWindowCount = false);

    void reset() override;

    void record() override;

    void process() override;

    void debug() override;

private:
    ACI_TemporalWindow &_aci;

    boolean _printWindowCount;

    LoRaWANTTN *m_lwTTN;
};

#endif // SBAT_ACOUSTICCOMPLEXITYINDEX_H
