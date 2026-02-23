# Environment Library

Drivers for the three environmental sensors used in BAIT2: the Bosch BME680
(temperature / humidity / pressure / gas), the Adafruit TSL2591 (light), and
the SparkFun BQ27441 LiPo fuel gauge (battery power).

**Source**: `lib/Environment/`

---

## Classes

### `EnvironmentalSensor`

**Header**: `include/BME680.h`  
**Source**: `src/BME680.cpp`

Measures temperature, relative humidity, barometric pressure, and gas resistance
using the Bosch BME680 via the Adafruit BME680 library. Readings are accumulated
across multiple `sample()` calls and averaged when `record()` is triggered.

#### Constructor

```cpp
EnvironmentalSensor(const char *filepath, LoRaWANTTN *lorattn,
                    int interval = 60, int measureInterval = 15,
                    int debugInterval = 30);
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `filepath` | `const char *` | — | SD card path for CSV output (e.g. `"/env.csv"`) |
| `lorattn` | `LoRaWANTTN *` | — | LoRaWAN uplink handle |
| `interval` | `int` | `60` s | Seconds between `record()` calls |
| `measureInterval` | `int` | `15` s | Seconds between `sample()` calls |
| `debugInterval` | `int` | `30` s | Seconds between `debug()` log output |

#### Methods

```cpp
void setup() override;
```

Initialises the BME680 at I²C address `0x76`. Configures 1× oversampling for all
channels, IIR filter size 3, and gas heater at 320 °C for 150 ms. Logs a `DEBUG`
message if the sensor is not found.

---

```cpp
bool sample() override;
```

Non-blocking state machine:

1. If a reading has not been started, calls `beginReading()` and returns `false`.
2. If the reading is in progress (`remainingReadingMillis() > 0`), returns `false`.
3. When complete, calls `endReading()` and accumulates temperature, humidity, pressure,
   and gas resistance into running totals. Returns `true`.

---

```cpp
void record() override;
```

Computes averages (temperature in °C, humidity in %, pressure in hPa, gas resistance in
kΩ) and:
- Adds them to the CayenneLPP buffer via `lora.getLPP()`.
- Marks the LoRaWAN instance dirty.

> **Note**: The current implementation does **not** write to the SD card in `record()`.
> CSV logging may be added in a future release.

---

```cpp
void debug() override;
```

Prints current averaged values and sample count to `Serial` at `DEBUG` verbosity.

---

```cpp
void reset();
```

Zeroes the four accumulators and resets the sample count.

---

#### CSV Columns

*(SD logging not yet implemented; values are transmitted over LoRaWAN only)*

| Column | Unit | CayenneLPP channel |
|--------|------|--------------------|
| Temperature | °C | `TEMPERATURE` (9) |
| Humidity | % RH | `HUMIDITY` (10) |
| Pressure | hPa | `PRESSURE` (11) |
| Gas resistance | kΩ | `GAS` (12) |

---

### `LightSensor`

**Header**: `include/LightSensor.h`  
**Source**: `src/LightSensor.cpp`

Measures ambient light using the Adafruit TSL2591 sensor. The Adafruit library is
blocking by default; this driver implements a non-blocking state machine using a
`Chrono` timer to track the sensor's integration time.

#### Compile-time flag

| Macro | Behaviour |
|-------|-----------|
| `USE_RAW_LIGHT` (defined by default) | Records raw IR and visible counts in addition to the computed lux value |

#### Constructor

```cpp
LightSensor(const char *filepath, LoRaWANTTN *lorattn,
            int interval = 60, int measureInterval = 10,
            int debugInterval = 30);
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `filepath` | `const char *` | — | SD card path (e.g. `"/light.csv"`) |
| `lorattn` | `LoRaWANTTN *` | — | LoRaWAN uplink handle |
| `interval` | `int` | `60` s | Seconds between `record()` calls |
| `measureInterval` | `int` | `10` s | Seconds between `sample()` calls |
| `debugInterval` | `int` | `30` s | Seconds between `debug()` log output |

#### Methods

```cpp
void setup() override;
```

Initialises the TSL2591 with low gain (`TSL2591_GAIN_LOW`) and 100 ms integration time
(`TSL2591_INTEGRATIONTIME_100MS`). Resets the sample timer in a stopped state so the
first `sample()` call starts a fresh reading.

---

```cpp
bool sample() override;
```

Non-blocking state machine:

1. If the sample timer is not running, calls `startSampling()` (enables the sensor,
   starts the timer) and returns `false`.
2. If the integration period (`≥ 120 ms` for 100 ms mode) has not yet elapsed, returns
   `false`.
3. When complete, calls `readSample()` which reads the raw 32-bit luminosity register,
   splits it into IR and full-spectrum channels, computes lux, and accumulates values.
   Returns `true`.

---

```cpp
void record() override;
```

Computes averages and:
- Adds lux to the CayenneLPP buffer under `LUMINOSITY` (13).
- With `USE_RAW_LIGHT`, also adds raw IR (`INFRARED`, 14) and visible (`VISIBLE`, 15).
- Marks the LoRaWAN instance dirty.

> **Note**: The `INFRARED` and `VISIBLE` channels are marked *deprecated* in
> `SensorDefinitions.h`. They may be removed in a future release.

---

```cpp
void debug() override;
```

Prints current averaged lux (and optionally IR / visible) plus sample count to `Serial`.

---

```cpp
void reset();
```

Zeroes all accumulators and the sample count.

---

#### CSV Columns

*(SD logging not yet implemented; values are transmitted over LoRaWAN only)*

| Column | Unit | CayenneLPP channel |
|--------|------|--------------------|
| Lux | lux | `LUMINOSITY` (13) |
| IR (raw) | counts | `INFRARED` (14) |
| Visible (raw) | counts | `VISIBLE` (15) |

---

### `PowerSensor`

**Header**: `include/PowerSensor.h`  
**Source**: `src/PowerSensor.cpp`

Reads battery state from the SparkFun BQ27441 LiPo fuel gauge. Unlike the other
sensors, `PowerSensor` does not oversample — it reads all registers once per
`interval` in `process()`.

#### Constructor

```cpp
PowerSensor(const char *filepath, LoRaWANTTN *lorattn,
            int interval, int initialCapacity);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `filepath` | `const char *` | SD card path for CSV output (e.g. `"/power.csv"`) |
| `lorattn` | `LoRaWANTTN *` | LoRaWAN uplink handle |
| `interval` | `int` | Seconds between readings |
| `initialCapacity` | `int` | Nominal battery capacity in mAh, used to configure the BQ27441 |

#### Public Data Members

These are populated by `process()` and can be read directly:

| Member | Type | Unit | Description |
|--------|------|------|-------------|
| `soc` | `float` | % | State of charge |
| `volts` | `float` | mV | Battery voltage |
| `current` | `float` | mA | Average current draw |
| `capacity` | `float` | mAh | Remaining capacity |
| `power` | `float` | mW | Instantaneous power consumption |
| `health` | `float` | % | Battery health (state of health) |
| `initialCapacity` | `int` | mAh | Configured nominal capacity |

#### Methods

```cpp
void setup() override;
```

Attempts to connect to the BQ27441 up to 5 times (with 200 ms delays), then calls
`lipo.setCapacity(initialCapacity)` to configure the fuel gauge.

---

```cpp
void process() override;
```

Reads all six BQ27441 registers (SOC, voltage, average current, remaining capacity,
power, state of health) and stores them in the public data members.

---

```cpp
void record() override;
```

- Writes a CSV row to the SD card file (`filepath`) containing:
  `timestamp, SOC, voltage, current, capacity, power, health, initialCapacity`
- Adds all seven values to the CayenneLPP buffer.
- Marks the LoRaWAN instance dirty.

---

#### CSV Format

```
2024-07-25 14:30:00, 85, 4150, -45, 8840, 186, 100, 10400
```

Columns: timestamp, SOC (%), voltage (mV), current (mA), remaining capacity (mAh),
power (mW), health (%), initial capacity (mAh).
