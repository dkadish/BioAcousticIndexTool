//
// Created by David Kadish on 28/06/2024.
//

#ifndef BAIT2_LORA_H
#define BAIT2_LORA_H

#include <CayenneLPP.h>
#include <Chrono.h>

class LoRaWANTTN
/**
 * @brief Constructor for the LoRaWANTTN class.
 *
 * This constructor initializes an instance of the LoRaWANTTN class.
 */

{

public:
  LoRaWANTTN();

  void setup();

  void loop();

  // // Register the sensors and check to make sure the IDs don't overlap
  // bool registerSensor();

  CayenneLPP &getLPP() { return m_lpp; }
  void setDirty();

private:
  /**
   * @brief Sends the data using LoRaWAN protocol.
   *
   * This method is responsible for transmitting the data that has been
   * collected and prepared. It uses the LoRaWAN protocol to send the data
   * to the designated receiver. The method ensures that the data is sent
   * only when it is marked as dirty, indicating that there is new data to
   * be sent.
   *
   * Send should only be called internally when the "dirty" flag has expired.
   *
   * @return true if the data was successfully sent, false otherwise.
   */
  bool send();

  CayenneLPP m_lpp;
  bool m_dirty = false;
  Chrono m_sendDelay;

  Chrono::chrono_t m_sendDelayMillis = 2000L;
};

#endif // BAIT2_LORA_H
