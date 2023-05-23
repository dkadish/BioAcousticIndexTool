/** Class that handles battery measurements through a voltage divider
 * @author David Kadish
 * @date 13/12/2018
 *
 * Handles measurements of battery voltage across a voltage divider.
 *
 * V_batt -------\/\/\/--------o---------- V
 *                 R1          |
 *                             |
 *                             /
 *                             \ R2
 *                             /
 *                             |
 *                             |
 *                            ---
 *                             -
 */

//
// Created by David Kadish on 13/12/2018.
//

#ifndef BAIT_BATTERYMEASUREMENT_H
#define BAIT_BATTERYMEASUREMENT_H

#include <Arduino.h>


class BatteryMeasurement {
protected:
    float _r1 = 22e3;
    float _r2 = 22e3;
    float _voltage=0.0;

    static constexpr float _vref = 3.3;

public:
    void setVoltageDividers(float R1, float R2){
        _r1 = R1;
        _r2 = R2;
    }

    float calculateVoltage(int measurement){
        float v_measured = map((float)measurement, 0.0, 1023.0, 0.0, _vref);
        float v = v_measured * (_r1 + _r2) / _r2 ;

        return v;
    }
};


#endif //BAIT_BATTERYMEASUREMENT_H
