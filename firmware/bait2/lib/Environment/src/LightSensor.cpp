#include "LightSensor.h"
#include "logging.h"
#include "SensorDefinitions.h"

LightSensor::LightSensor(const char *filepath, LoRaWANTTN *lorattn, int interval, int measureInterval, int debugInterval)
    : OversamplingSensor(interval, measureInterval, debugInterval), m_lwTTN(lorattn), m_tsl(2591)
{
  // Constructor implementation
}

void LightSensor::setup()
{
  // Initialize TSL2591
  if (!m_tsl.begin()) // TODO - check the address
  {
    DEBUG("Could not find a valid TSL2591 sensor");
  }

  m_tsl.setGain(m_gainMode);
  m_tsl.setTiming(m_integrationMode);

  m_sampleTimer.restart();
  m_sampleTimer.stop();
}

// TODO Should this use microseconds?
long LightSensor::getMillisFromIntegrationTime(tsl2591IntegrationTime_t integrationTime)
{
  switch (integrationTime)
  {
  case TSL2591_INTEGRATIONTIME_100MS:
    return 120L;
  case TSL2591_INTEGRATIONTIME_200MS:
    return 240L;
  case TSL2591_INTEGRATIONTIME_300MS:
    return 360L;
  case TSL2591_INTEGRATIONTIME_400MS:
    return 480L;
  case TSL2591_INTEGRATIONTIME_500MS:
    return 600L;
  case TSL2591_INTEGRATIONTIME_600MS:
    return 720L;
  default:
    return 120L;
  }
}

void LightSensor::startSampling()
{
  TRACE("Starting sampling on TSL2591 (%ld)", millis());
  // Start sampling the sensor
  m_tsl.enable();
  m_sampleTimer.restart();
}

void LightSensor::readSample()
{
  // Sample data from the sensors
  uint16_t ir, full;

  // TODO: is this still necessary?
  /* Early silicon seems to have issues when there is a sudden jump in */
  /* light levels. :( To work around this for now sample the sensor 2x */
  uint32_t lum = m_tsl.getFullLuminosityManual();
  // lum = getFullLuminosity();

  m_tsl.disable();

  ir = lum >> 16;
  full = lum & 0xFFFF;

  TRACE("%ld | IR: %d, Full: %d, Dual: %ld", millis(), ir, full, lum);

  /* Calculate the actual lux value */
  /* 0 = sensor overflow (too much light) */
  float light = m_tsl.calculateLux(full, ir);

  // if (light > 0.0)
  // { // Only record if there is light
  m_luxAccumulator += light;
  m_irAccumulator += (uint32_t)ir;
  m_visibleAccumulator += (uint32_t)full;
  m_count++;
  //   return true;
  // }
  // else
  // {
  //   // TODO: Autorange
  //   return false;
  // }
}

/* The Adafruit library is blocking. In order to sample the sensor without blocking the main loop,
 *  we need to track state, enable the sensor, and then read and disable it after a delay.
 */
bool LightSensor::sample()
{
  if (!m_sampleTimer.isRunning()) // TODO: Check if the sensor is ready to sample
  {
    startSampling();
    return false;
  }
  else if (m_sampleTimer.hasPassed(getMillisFromIntegrationTime(m_integrationMode))) // TODO: Check if the sensor is done sampling
  {
    readSample();
    m_sampleTimer.stop();
    return true;
  }

  // Sampling in progress
  return false;
}

void LightSensor::record()
{
  // Record the sampled data to a file or and send over LoRaWAN
  if (m_count > 0)
  {
    float lux = currentLux();         // Lux
    float ir = currentIR();           // Raw
    float visible = currentVisible(); // Raw

    // Send data over LoRaWAN
    m_lwTTN->getLPP().addLuminosity(LUMINOSITY, lux);
    m_lwTTN->getLPP().addAnalogInput(INFRARED, ir);
    m_lwTTN->getLPP().addAnalogInput(VISIBLE, visible);
  }
  else
  {
    DEBUG("No data to record");
  }
}

void LightSensor::debug()
{
  // Output debug information to the console or debug interface
  DEBUG("LUX: %f lm, IR %f, Visible: %f. Count: %ld", currentLux(), currentIR(), currentVisible(), m_count);
}

void LightSensor::reset()
{
  // Reset the accumulators and count
  m_luxAccumulator = 0.0;
  m_irAccumulator = 0L;
  m_visibleAccumulator = 0L;
  m_count = 0;
}