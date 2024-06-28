//
// Created by David Kadish on 28/06/2024.
//

#ifndef BAIT2_LORA_H
#define BAIT2_LORA_H

#include <CayenneLPP.h>

class LoRaWANTTN
{

public:
  LoRaWANTTN();

  void setup();

  void loop();

  // // Register the sensors and check to make sure the IDs don't overlap
  // bool registerSensor();

  bool send();

  CayenneLPP &getLPP() { return m_lpp; }
  void setDirty();

private:
  CayenneLPP m_lpp;
  bool m_dirty = false;
};

#endif // BAIT2_LORA_H
