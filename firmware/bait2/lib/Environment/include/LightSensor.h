#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "OversamplingSensor.h"
#include "lora.h"
#include <Adafruit_TSL2591.h>

class LightSensor : public OversamplingSensor
{
private:
  Adafruit_TSL2591 m_tsl; // LightSensor sensor for light
  LoRaWANTTN *m_lwTTN;

  tsl2591Gain_t m_gainMode = TSL2591_GAIN_LOW;
  tsl2591IntegrationTime_t m_integrationMode = TSL2591_INTEGRATIONTIME_200MS; // TSL2591_INTEGRATIONTIME_100MS;

  // Accumulators
  float m_luxAccumulator;
  uint32_t m_irAccumulator;
  uint32_t m_visibleAccumulator;
  long m_count = 0;

  Chrono m_sampleTimer;

  // Functions
  float currentLux() { return m_count > 0 ? m_luxAccumulator / (float)m_count : 0.0; };
  float currentIR() { return m_count > 0 ? (float)m_irAccumulator / (float)m_count : 0.0; };
  float currentVisible() { return m_count > 0 ? (float)m_visibleAccumulator / (float)m_count : 0.0; };

  // Sampling functions
  void startSampling();
  void readSample();
  uint32_t getFullLuminosity();

  long getMillisFromIntegrationTime(tsl2591IntegrationTime_t integrationTime);

public:
  LightSensor(const char *filepath, LoRaWANTTN *lorattn, int interval = 60, int measureInterval = 10, int debugInterval = 30);
  void setup() override;

  bool sample() override;

  void record() override;

  void debug() override;

  void reset();
};

#endif // LIGHTSENSOR_H