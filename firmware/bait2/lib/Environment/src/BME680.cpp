#include "BME680.h"
#include "logging.h"
#include "SensorDefinitions.h"

EnvironmentalSensor::EnvironmentalSensor(const char *filepath, LoRaWANTTN *lorattn, int interval, int measureInterval, int debugInterval)
    : OversamplingSensor(interval, measureInterval, debugInterval), m_lwTTN(lorattn)
{
  // Constructor implementation
}

void EnvironmentalSensor::setup()
{
  // Initialize BME680
  if (!m_bme.begin(0x76)) // TODO - check the address
  {
    DEBUG("Could not find a valid BME680 sensor");
  }

  // TODO: Is this necessary?
  //  Set up oversampling and filter initialization
  m_bme.setTemperatureOversampling(BME680_OS_1X);
  m_bme.setHumidityOversampling(BME680_OS_1X);
  m_bme.setPressureOversampling(BME680_OS_1X);
  m_bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  m_bme.setGasHeater(320, 150); // 320*C for 150 ms
}

bool EnvironmentalSensor::sample()
{
  if (m_bme.remainingReadingMillis() == m_bme.reading_not_started)
  {
    m_bme.beginReading();
    return false;
  }
  else if (m_bme.remainingReadingMillis() == m_bme.reading_complete)
  {
    m_bme.endReading();

    // Accumulate sensor values
    m_temperatureAccumulator += m_bme.temperature;
    m_humidityAccumulator += m_bme.humidity;
    m_pressureAccumulator += m_bme.pressure;
    m_gasAccumulator += m_bme.gas_resistance;
    m_count++;

    return true;
  }
  else // Reading in progress
  {
    return false;
  }
}

void EnvironmentalSensor::record()
{
  // Record the sampled data to a file or and send over LoRaWAN
  if (m_count > 0)
  {
    float temperature = currentTemperature(); // C
    float humidity = currentHumidity();       // %
    float pressure = currentPressure();       // hPa
    float gas = currentGas();                 // Ohms

    // Send data over LoRaWAN
    m_lwTTN->getLPP().addTemperature(TEMPERATURE, temperature);
    m_lwTTN->getLPP().addRelativeHumidity(HUMIDITY, humidity);
    m_lwTTN->getLPP().addBarometricPressure(PRESSURE, pressure);
    m_lwTTN->getLPP().addAnalogInput(GAS, gas);
  }
  else
  {
    DEBUG("No data to record");
  }
}

void EnvironmentalSensor::debug()
{
  // Output debug information to the console or debug interface
  DEBUG("BME: Temperature %f C, Humidity %f \%, Pressure %f hPa, and Gas %f kohms. Count %ld", currentTemperature(), currentHumidity(), currentPressure(), currentGas(), m_count);
}

void EnvironmentalSensor::reset()
{
  // Reset the accumulators and count
  m_temperatureAccumulator = 0.0;
  m_humidityAccumulator = 0.0;
  m_pressureAccumulator = 0.0;
  m_gasAccumulator = 0.0;
  m_count = 0;
}