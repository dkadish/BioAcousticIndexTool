# BAIT v2 Firmware

Firmware for the BioAcoustic Index Tool version 2, targeting the **Teensy MicroMod** processor on a custom carrier board.

## Overview

BAIT v2 computes ecoacoustic indices — ACI, ADI, and ADE — directly on the microcontroller, logs environmental sensor data to SD card, and optionally transmits results over LoRaWAN to The Things Network.

### Libraries

The firmware is structured around several internal libraries located in `lib/`:

| Library | Description |
|---------|-------------|
| `Ecoacoustics` | Core acoustic index computation (ACI, ADI, ADE, FFT) |
| `Environment` | BME680, TSL2591, and BQ27441 sensor drivers |
| `Communication` | LoRaWAN transmission via Seeed WIO-E5 (CayenneLPP encoding) |
| `Logging` | SD card data logging |
| `Display` | Optional display output |
| `Sensor` | Base sensor abstraction |

## Development Setup

### Prerequisites

- [PlatformIO Core](https://platformio.org/install/cli) — `brew install platformio` on macOS
- [CLion](https://www.jetbrains.com/clion/) with the PlatformIO plugin (recommended), or VS Code with the PlatformIO extension

See the [PlatformIO + CLion setup guide](https://docs.platformio.org/en/latest/integration/ide/clion.html) for detailed instructions.

### Verified Versions

| Tool | Version |
|------|---------|
| PlatformIO | 6.1.7 |
| CLion | 2023.1.3 |

## Build Environments

Three PlatformIO environments are defined in `platformio.ini`:

| Environment | Description |
|-------------|-------------|
| `teensymm` | Development build — USB MIDI/Audio/Serial, waits for serial monitor on startup |
| `teensymm-release` | Release build — USB Audio only, no serial wait |
| `desktop` | Native desktop build for unit testing (no hardware required) |

## Building and Uploading

```bash
# Development build (with serial output)
pio run -e teensymm

# Upload to connected Teensy MicroMod
pio run -e teensymm -t upload

# Release build
pio run -e teensymm-release -t upload
```

## Running Tests

Unit tests can be run on the desktop (without hardware) using the native environment:

```bash
# Run desktop tests (no hardware required)
pio test -e desktop
```

Embedded tests require a connected Teensy MicroMod:

```bash
pio test -e teensymm
```

## Hardware

The BAIT v2 carrier board design files are located in [`../../electronics/BAIT2_MicroMod/`](../../electronics/BAIT2_MicroMod/).
