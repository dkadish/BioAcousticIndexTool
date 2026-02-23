# BAIT v1 Firmware

Firmware for the original BioAcoustic Index Tool (BAIT), targeting **Teensy 3.5**, **Teensy 3.6**, and **Teensy MicroMod** processors.

## Overview

BAIT v1 computes ecoacoustic indices — ACI, ADI, and ADE — directly on the microcontroller and logs the results alongside environmental sensor readings to an SD card.

### Source Files

Key source files in `src/`:

| File | Description |
|------|-------------|
| `main.cpp` | Main entry point — setup and loop |
| `AcousticComplexityIndex.*` | ACI computation |
| `AcousticDiversityIndex.*` | ADI computation |
| `AcousticDifferenceIndex.*` | ADE computation |
| `ACI_TemporalWindow.*` | Sliding time-window management for ACI |
| `FFTReader.*` | Audio FFT data capture |
| `AtmosphericSensors.*` | Si7021 temperature/humidity sensor driver |
| `BatteryMeasurement.*` | Battery voltage measurement |
| `sdcard.h` | SD card logging helpers |
| `definitions.h` | Shared constants and pin definitions |
| `parameters.h` | Configurable measurement parameters |

## Development Setup

### Prerequisites

- [PlatformIO Core](https://platformio.org/install/cli) — `brew install platformio` on macOS
- [CLion](https://www.jetbrains.com/clion/) with the PlatformIO plugin (recommended), or VS Code with the PlatformIO extension

### Library Dependencies

PlatformIO will automatically download the following libraries:

- `Audio` (Teensy Audio Library)
- `Adafruit Si7021 Library`
- `Adafruit Unified Sensor`
- `Adafruit TSL2561`
- `SdFat`
- `Snooze` (>= 6.3.4) — low-power sleep

## Build Environments

Three PlatformIO environments are defined in `platformio.ini`:

| Environment | Board |
|-------------|-------|
| `teensy35` | Teensy 3.5 |
| `teensy36` | Teensy 3.6 (120 MHz) |
| `teensymm` | Teensy MicroMod |

## Building and Uploading

```bash
# Build for Teensy 3.5
pio run -e teensy35

# Build and upload to connected board
pio run -e teensy35 -t upload

# Build for Teensy 3.6
pio run -e teensy36 -t upload

# Build for Teensy MicroMod
pio run -e teensymm -t upload
```

## Hardware

The BAIT v1 carrier board schematic (Fritzing) is located at [`../../electronics/BAIT.fzz`](../../electronics/BAIT.fzz).
A breadboard wiring diagram is available at [`../../electronics/BAIT_bb.png`](../../electronics/BAIT_bb.png).
