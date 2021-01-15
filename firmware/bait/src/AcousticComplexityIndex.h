//
// Created by David Kadish on 15/11/2018.
//

#ifndef SBAT_ACOUSTICCOMPLEXITYINDEX_H
#define SBAT_ACOUSTICCOMPLEXITYINDEX_H


#include "Sensor.h"
#include "ACI_TemporalWindow.h"

class AcousticComplexityIndex : public Sensor {
public:

    AcousticComplexityIndex(int interval, ACI_TemporalWindow &aci_tw);

    void reset() override;

    void record() override;

    void process() override;

private:

    ACI_TemporalWindow & _aci;
};


#endif //SBAT_ACOUSTICCOMPLEXITYINDEX_H
