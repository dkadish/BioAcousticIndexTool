#include "AudioPower.h"

#include "logging.h"

#include <Arduino.h>
#include <Audio.h>

// ---------------------------------------------------------------------------
// KNOWN ISSUE: this used to also power the SGTL5000 codec chip itself up/down
// over I2C (writing CHIP_ANA_POWER / CHIP_DIG_POWER). Those I2C writes were
// found to fail 100% of the time - even with audio confirmed working before
// and after - while the codec's own boot-time init (AudioControlSGTL5000::
// enable(), same register values, same bus) succeeds. Root cause not yet
// found. Until it is, this only toggles the Teensy-side I2S/clock path,
// which was verified reliable (RMS sampling cleanly stops/resumes across
// off()/on()). The codec chip itself likely stays fully powered throughout,
// so this does NOT capture the codec's own power draw - only whatever the
// Teensy-side SAI1 clock tree costs.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Power off: stop the I2S (SAI1) receiver and gate its clock.
// ---------------------------------------------------------------------------
bool AudioPower::off()
{
    DEBUG("AudioPower: disabling audio chain")

    // 1. Stop the SAI1 receiver.  Clearing RCSR stops it from generating
    //    DMA requests, which idles the existing DMA channel (its TCD is
    //    untouched and will resume later).
    I2S1_RCSR = 0;

    // 2. Gate the SAI1 peripheral clock to stop the PLL divider / MCLK
    //    output.  This saves the clock tree power draw.
    CCM_CCGR5 &= ~(((uint32_t)3) << 18);

    DEBUG("AudioPower: audio chain disabled")
    return true;
}

// ---------------------------------------------------------------------------
// Power on: ungate the SAI1 clock, then restart the SAI1 receiver.  The DMA
// TCD is still set up from AudioInputI2S::begin() and resumes automatically.
// ---------------------------------------------------------------------------
bool AudioPower::on()
{
    DEBUG("AudioPower: enabling audio chain")

    // 1. Ungate the SAI1 peripheral clock.
    CCM_CCGR5 |= (((uint32_t)3) << 18);

    // 2. Restart the SAI1 receiver.
    I2S1_RCSR = I2S_RCSR_RE | I2S_RCSR_BCE | I2S_RCSR_FRDE;

    DEBUG("AudioPower: audio chain enabled")
    return true;
}
