# BAIT2 Firmware

**BioAcoustic Index Tool — Generation 2**

BAIT2 is embedded firmware for the Teensy MicroMod that continuously captures audio and
environmental data in the field, computes real-time ecoacoustic indices, and transmits
summaries to The Things Network (TTN) via LoRaWAN.

---

## Table of Contents

1. [Hardware Requirements](#hardware-requirements)
2. [Project Layout](#project-layout)
3. [Quick Start](#quick-start)
4. [Build Environments](#build-environments)
5. [Configuration & Macros](#configuration--macros)
6. [Architecture & Data Flow](#architecture--data-flow)
7. [Modules](#modules)
8. [SD Card Data Files](#sd-card-data-files)
9. [Serial Commands](#serial-commands)
10. [LoRaWAN Sensor Channel Definitions](#lorawan-sensor-channel-definitions)
11. [Library API Reference](#library-api-reference)
12. [Troubleshooting](#troubleshooting)

---

## Hardware Requirements

| Component | Part | Interface |
|-----------|------|-----------|
| Microcontroller | Teensy MicroMod | — |
| Audio codec / microphone | SGTL5000 | I2S |
| Environmental sensor | Bosch BME680 | I²C (`0x76`) |
| Light sensor | Adafruit TSL2591 | I²C |
| Fuel gauge / battery monitor | SparkFun BQ27441 | I²C |
| LoRaWAN radio | Seeed WIO-E5 | UART (`Serial1`, 9 600 baud) |
| Storage | SDIO SD card | SDIO (FIFO mode) |

---

## Project Layout

```
firmware/bait2/
├── platformio.ini          # PlatformIO build configuration
├── src/
│   └── main.cpp            # Entry point: setup() / loop()
├── include/
│   ├── baitconfig.h        # Global compile-time settings (verbosity, LED)
│   └── sdcard.h            # SD card initialisation (SdFs / SDIO)
├── lib/
│   ├── Ecoacoustics/       # FFT, ACI, RMS computation (see lib/Ecoacoustics/README.md)
│   ├── Communication/      # LoRaWAN uplink via WIO-E5 (see lib/Communication/README.md)
│   ├── Environment/        # BME680, TSL2591, BQ27441 drivers (see lib/Environment/README.md)
│   ├── Sensor/             # Base sensor lifecycle classes (see lib/Sensor/README.md)
│   ├── Logging/            # Compile-time debug macros (see lib/Logging/README.md)
│   └── Display/            # OLED display driver (currently unused)
└── test/                   # PlatformIO unit tests
```

---

## Quick Start

### Prerequisites

- [PlatformIO Core](https://platformio.org/install/cli) or the PlatformIO IDE extension
- Teensy MicroMod with attached peripherals (see [Hardware Requirements](#hardware-requirements))

### Build and upload (development)

```bash
cd firmware/bait2
pio run -e teensymm -t upload
```

### Open the serial monitor

```bash
pio device monitor -b 9600
```

### Docker build (CI / headless)

A `Dockerfile` is provided to run a PlatformIO build without a local installation:

```bash
docker build -t bait2 .
docker run --rm bait2
```

---

## Build Environments

Three PlatformIO environments are defined in `platformio.ini`:

| Environment | Target | Purpose |
|-------------|--------|---------|
| `teensymm` | Teensy MicroMod | Development — USB MIDI + Audio + Serial, debug output enabled |
| `teensymm-release` | Teensy MicroMod | Production — USB Audio only, no serial wait |
| `desktop` | Native (x86) | Unit testing with the `test/test_desktop` suite |

### `teensymm` (development)

```ini
[env:teensymm]
platform = teensy
board = teensymm
board_build.f_cpu = 150000000L   ; 150 MHz (max 600 MHz)
framework = arduino
lib_deps = Wire, CayenneLPP
build_flags = -D USB_MIDI_AUDIO_SERIAL -D WAIT_FOR_SERIAL
test_ignore = test_desktop
```

- **`USB_MIDI_AUDIO_SERIAL`** — Activates the Teensy USB MIDI + Audio + Serial composite device.
- **`WAIT_FOR_SERIAL`** — Blocks `setup()` until a serial monitor connects (up to 10 × 100 ms
  retries). Also enables detailed SD card error output via `sd.initErrorHalt()`.

### `teensymm-release` (production)

```ini
[env:teensymm-release]
platform = teensy
board = teensymm
board_build.f_cpu = 150000000L
framework = arduino
lib_deps = Wire, CayenneLPP
build_flags = -D USB_AUDIO
test_ignore = test_desktop
```

- **`USB_AUDIO`** — USB Audio only; no serial enumeration delay in the field.

### `desktop` (native testing)

```ini
[env:desktop]
platform = native
lib_extra_dirs = lib/ include/
test_ignore = test_embedded
build_flags = -D DESKTOP_NATIVE
```

- **`DESKTOP_NATIVE`** — Guards hardware-specific code so the desktop test runner can compile
  pure-logic modules without Arduino headers.

---

## Configuration & Macros

### `include/baitconfig.h`

Global compile-time settings applied to every translation unit.

| Macro | Default | Description |
|-------|---------|-------------|
| `VERBOSITY` | `4` | Log verbosity level (overrides the default `0` in `logging.h`). See [Logging](#logging) |
| `USE_STATUS_LED` | defined | When defined, `LEDHIGH` / `LEDLOW` macros toggle `LED_BUILTIN` |
| `LEDHIGH` | `digitalWrite(LED_BUILTIN, HIGH);` | Turn on the status LED |
| `LEDLOW` | `digitalWrite(LED_BUILTIN, LOW);` | Turn off the status LED |

### `include/sdcard.h`

Thin initialisation wrapper for the SdFs (SdFat v2) SD card driver.

```cpp
void sd_setup();   // Calls sd.begin(SdioConfig(FIFO_SDIO)), logs on failure
```

Global objects exposed:

| Object | Type | Description |
|--------|------|-------------|
| `sd` | `SdFs` | Filesystem handle; use for opening files |
| `file` | `FsFile` | Convenience handle (unused in current release) |

### `src/main.cpp` tunables

| Symbol | Default | Description |
|--------|---------|-------------|
| `INTERVAL_SECONDS` | `5 * 60` (300 s) | Recording and reporting interval, overridable via `-D INTERVAL_SECONDS=<n>` |
| `gain_l` | `36.0` | Initial amplifier gain for the left audio channel |
| `batteryCapacity` | `10400` mAh | Nominal battery capacity reported to BQ27441 |

---

## Architecture & Data Flow

### Audio pipeline

```
I2S microphone (SGTL5000)
    └─> AudioInputI2S (i2s)
            └─> AudioAmplifier (amp_l, gain = 36)
                    ├─> AudioAnalyzeRMS  (rms_l)   ──> RootMeanSquare
                    ├─> AudioAnalyzePeak (peak_l)   ──> (unused)
                    ├─> AudioAnalyzeFFT256 (fft256_l) ──> FFTReader ──> ACI_TemporalWindow ──> AcousticComplexityIndex
                    └─> AudioOutputUSB  (usb)        ──> USB audio monitor (dev builds)
```

The Teensy Audio library manages DMA-driven I2S capture. All audio objects are statically
allocated and wired via `AudioConnection` patch cords. `AudioMemory(200)` allocates 200
audio blocks (~2.9 s of buffering at 44.1 kHz).

### Sensor loop

Every sensor follows the same three-phase lifecycle driven by the `Sensor` base class:

1. **`loop()`** — Called every Arduino loop iteration. Checks a `Chrono` timer; when the
   interval has elapsed, calls `process()` then `record()`.
2. **`process()`** — Reads / aggregates raw hardware values.
3. **`record()`** — Writes averaged values to the SD card (CSV) and queues a CayenneLPP
   payload for LoRaWAN transmission.

`OversamplingSensor` adds a second, faster `Chrono` so `sample()` is called at a higher
rate than `record()`, enabling accumulation / averaging over the reporting interval.

### LoRaWAN uplink

`LoRaWANTTN` wraps a Seeed WIO-E5 module connected on `Serial1` at 9 600 baud.

- On `setup()`, it sends `AT+JOIN` to the module.
- Each sensor calls `lora.getLPP().add<Type>(channel, value)` then `lora.setDirty()`.
- `setDirty()` starts a 2-second debounce timer.
- After the debounce expires, `send()` transmits the CayenneLPP buffer as a hex string
  via `AT+CMSGHEX="..."`.

---

## Modules

| Module | Location | Key Classes |
|--------|----------|-------------|
| Ecoacoustics | `lib/Ecoacoustics/` | `FFTReader`, `ACI_TemporalWindow`, `AcousticComplexityIndex`, `RootMeanSquare`, `AcousticDifferenceIndex`, `AcousticDiversityIndex` |
| Environment | `lib/Environment/` | `EnvironmentalSensor`, `LightSensor`, `PowerSensor` |
| Communication | `lib/Communication/` | `LoRaWANTTN` |
| Sensor | `lib/Sensor/` | `Sensor`, `OversamplingSensor` |
| Logging | `lib/Logging/` | `DEBUG`, `INFO`, `WARNING`, `ERROR`, `TRACE` macros |
| Display | `lib/Display/` | `OLEDDisplay` (currently disabled) |

See each library's `README.md` for detailed API documentation.

---

## SD Card Data Files

All files are opened in append mode (`O_WRITE | O_CREAT | O_AT_END`). Each row begins with
an ISO-style timestamp written by `Sensor::writeTimestamp()`.

| File | Columns | Source class |
|------|---------|--------------|
| `/power.csv` | timestamp, SOC (%), voltage (mV), current (mA), remaining capacity (mAh), power (mW), health (%), initial capacity (mAh) | `PowerSensor` |
| `/env.csv` | timestamp, temperature (°C), humidity (%), pressure (hPa), gas (kΩ) | `EnvironmentalSensor` |
| `/light.csv` | timestamp, lux, IR (raw), visible (raw) | `LightSensor` |
| `/rms.csv` | timestamp, RMS × 100 (integer) | `RootMeanSquare` |
| `/aci.csv` | timestamp, ACI value | `AcousticComplexityIndex` |
| `/fft.csv` | timestamp, 128 comma-separated FFT bin values | `FFTReader` (when `recordSpectrogram = true`) |

---

## Serial Commands

When a USB serial monitor is connected, single-character commands are handled in
`serialEvent()`:

| Key | Action |
|-----|--------|
| `F` | Print the SD card directory listing to serial |
| `G` | Double the left-channel amplifier gain (`gain_l *= 2`) |
| `g` | Halve the left-channel amplifier gain (`gain_l /= 2`) |

---

## LoRaWAN Sensor Channel Definitions

CayenneLPP channel IDs are defined in `lib/Communication/include/SensorDefinitions.h`:

| Channel | ID | CayenneLPP type | Unit |
|---------|----|-----------------|------|
| `STATE_OF_CHARGE` | 1 | Percentage | % |
| `VOLTAGE` | 2 | Voltage | V |
| `CURRENT` | 3 | Current | A |
| `REMAINING_CAPACITY` | 4 | Generic sensor | mAh |
| `POWER_CONSUMPTION` | 5 | Generic sensor | mW |
| `BATTERY_HEALTH` | 6 | Percentage | % |
| `INITIAL_CAPACITY` | 7 | Generic sensor | mAh |
| `ROOT_MEAN_SQUARE` | 8 | Analog input | — |
| `TEMPERATURE` | 9 | Temperature | °C |
| `HUMIDITY` | 10 | Relative humidity | % |
| `PRESSURE` | 11 | Barometric pressure | hPa |
| `GAS` | 12 | Analog input | kΩ |
| `LUMINOSITY` | 13 | Luminosity | lux |
| `INFRARED` | 14 | Analog input | raw *(deprecated)* |
| `VISIBLE` | 15 | Analog input | raw *(deprecated)* |
| `ACOUSTIC_COMPLEXITY_INDEX` | 16 | Analog input | — |

---

## Library API Reference

- **[Sensor base classes](lib/Sensor/README.md)** — `Sensor`, `OversamplingSensor`
- **[Ecoacoustics](lib/Ecoacoustics/README.md)** — `FFTReader`, `ACI_TemporalWindow`, `AcousticComplexityIndex`, `RootMeanSquare`
- **[Environment sensors](lib/Environment/README.md)** — `EnvironmentalSensor`, `LightSensor`, `PowerSensor`
- **[Communication](lib/Communication/README.md)** — `LoRaWANTTN`
- **[Logging](lib/Logging/README.md)** — Debug macros

---

## Generating HTML API Documentation

A `Doxyfile` is included for generating browsable HTML documentation into `docs/`:

```bash
cd firmware/bait2
doxygen Doxyfile
# Open docs/index.html in a browser
```

[Doxygen](https://www.doxygen.nl/index.html) must be installed. On macOS:
```bash
brew install doxygen
```
On Debian/Ubuntu:
```bash
sudo apt install doxygen
```

The `docs/` output directory is git-ignored (it is a build artefact).

---

## Troubleshooting

### SD card fails to initialise

- Check that the SD card is inserted and formatted as FAT32 or exFAT.
- In the `teensymm` environment, `WAIT_FOR_SERIAL` must be defined for `sd.initErrorHalt()`
  to print a detailed error to the serial monitor.
- Verify the SDIO wiring; the firmware uses FIFO SDIO mode (`SdioConfig(FIFO_SDIO)`).

### LoRaWAN join fails

- Ensure the WIO-E5 module is connected on `Serial1` and powered.
- The firmware sends `AT+JOIN` on `setup()`; check that the module has valid OTAA credentials
  flashed (AppEUI, DevEUI, AppKey).
- The join request is fire-and-forget in this release. Failed joins are logged at the
  `ERROR` verbosity level but the firmware continues running.

### No serial output

- Set `VERBOSITY` ≥ 1 in `include/baitconfig.h` (default is `4` in development builds).
- Ensure you are using the `teensymm` environment (which defines `WAIT_FOR_SERIAL` and
  `USB_MIDI_AUDIO_SERIAL`).
- The firmware waits up to 1 second after `Serial.begin(9600)` before continuing; open the
  monitor promptly.

### BME680 / TSL2591 not found

- Both sensors use I²C. Confirm correct I²C address (`0x76` for BME680) and check wiring.
- Sensors log a `DEBUG` message on failure but do not halt execution.

### Audio gain too low / high

- Default `gain_l = 36.0`. Adjust at runtime with the `G` (double) and `g` (halve) serial
  commands.
- Persist a new default by changing the `gain_l` initialiser in `src/main.cpp` and
  rebuilding.

### ACI value is always 0

- Ensure `FFTReader` is calling `_fft.averageTogether()` and the Hamming window is applied
  (done in `FFTReader::setup()`).
- `ACI_TemporalWindow::process()` accumulates across multiple windows before
  `AcousticComplexityIndex::record()` reports. Wait at least one full interval before
  expecting output.
