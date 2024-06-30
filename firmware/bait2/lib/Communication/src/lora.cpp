#include "lora.h"

#include "logging.h"

LoRaWANTTN::LoRaWANTTN() : m_lpp(51)
{
  // Constructor initialization code here
  m_lpp.reset();
}

void LoRaWANTTN::setup()
{
  Serial1.begin(9600);

  // Initialization code here
  for (int i = 0; i < 10 && !Serial1; i++)
  {
    Serial1.begin(9600);
    delay(100);
    DEBUG("Waiting for Serial1...")
  }

  if (!Serial1)
  {
    ERROR("Serial1 not available")
  }

  Serial1.println("AT+JOIN");
}

void LoRaWANTTN::loop()
{
  // If there's new data and we have delayed for 1 second, send it.
  if (m_dirty && m_sendDelay.hasPassed(1000))
  {
    send();
  }

  if (Serial1.available())
  {                               // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial1.read()); // read it and send it out Serial (USB)
  }
}

// bool LoRaWANTTN::registerSensor()
// {
//   // Implementation for registering a sensor
//   // Return true if successful, false otherwise
//   return true;
// }

bool LoRaWANTTN::send()
{
  // TODO: Use the +CMSGHEX function and wait for ACK to ensure data has been sent.

  Serial.print("AT+CMSGHEX=\"");
  for (int i = 0; i < m_lpp.getSize(); i++)
  {
    Serial.printf("%02X", m_lpp.getBuffer()[i]);
  }
  Serial.println("\"");

  DEBUG("Sending data to TTN");

  // Code to send data
  // Return true if data is sent successfully, false otherwise
  Serial1.print("AT+CMSGHEX=\"");
  for (int i = 0; i < m_lpp.getSize(); i++)
  {
    Serial1.printf("%02X", m_lpp.getBuffer()[i]);
  }
  Serial1.println("\"");
  // Serial1.println("AT");

  DEBUG("Data sent to TTN");

  // Reset the lpp buffer
  m_lpp.reset();
  m_dirty = false;
  // Reset and then pause the send delay timer so that it is ready for next time.
  m_sendDelay.restart();
  m_sendDelay.stop();

  DEBUG("LPP state reset");

  return true;
}

void LoRaWANTTN::setDirty()
{
  m_dirty = true;

  // Resume the send delay timer if it has stopped.
  m_sendDelay.resume();
}