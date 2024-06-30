#ifndef BME680_H
#define BME680_H

#include "OversamplingSensor.h"
#include "lora.h"

#include <Adafruit_BME680.h>

class EnvironmentalSensor : public OversamplingSensor
{
private:
  Adafruit_BME680 m_bme; // BME680 sensor for temperature, pressure, humidity, and gas
  LoRaWANTTN *m_lwTTN;

  float m_temperatureAccumulator;
  float m_humidityAccumulator;
  float m_pressureAccumulator;
  float m_gasAccumulator;
  long m_count = 0;

  uint32_t m_readingCompleteTime = 0;

  float currentTemperature() { return m_count > 0 ? m_temperatureAccumulator / (float)m_count : 0.0; };
  float currentHumidity() { return m_count > 0 ? m_humidityAccumulator / (float)m_count : 0.0; };
  float currentPressure() { return m_count > 0 ? m_pressureAccumulator / (float)m_count / 100.0 : 0.0; }; // Convert to hectoPascals
  float currentGas() { return m_count > 0 ? m_gasAccumulator / (float)m_count / 1000.0 : 0.0; };          // Convert to kOhms

public:
  EnvironmentalSensor(const char *filepath, LoRaWANTTN *lorattn, int interval = 60, int measureInterval = 15, int debugInterval = 30);
  void setup() override;

  bool sample() override;

  void record() override;

  void debug() override;

  void reset();
};

#endif // BME680_H