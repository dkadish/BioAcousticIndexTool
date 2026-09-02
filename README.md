# BioAcoustic Index Tool (BAIT)

A smart, field-deployable sensor that automatically calculates acoustic indices and captures environmental data in real time.

BAIT records soundscape data and computes standard ecoacoustic metrics — including the **Acoustic Complexity Index (ACI)**, **Acoustic Diversity Index (ADI)**, and **Acoustic Difference Index (ADE)** — directly on-device. Environmental readings (temperature, humidity, light, and battery level) are logged alongside the acoustic data to SD card, with optional LoRaWAN transmission.

## Table of Contents

- [Project Overview](#project-overview)
- [Repository Structure](#repository-structure)
- [Hardware Versions](#hardware-versions)
- [Getting Started](#getting-started)
- [Contributing](#contributing)
- [License](#license)

## Project Overview

Traditional soundscape monitoring requires transporting recording equipment to the field, recording raw audio files, and then processing them offline. BAIT eliminates the offline processing step by computing acoustic indices on the microcontroller itself. This reduces storage requirements and allows near-real-time data delivery over low-power wide-area networks (LoRaWAN).

### Acoustic Indices Computed

| Index | Description |
|-------|-------------|
| **ACI** | Acoustic Complexity Index — captures the variability in sound intensity across frequency bins |
| **ADI** | Acoustic Diversity Index — measures the diversity of sound across frequency bands (Shannon entropy) |
| **ADE** | Acoustic Difference Index — measures the difference between consecutive time windows |

## Repository Structure

```
BioAcousticIndexTool/
├── firmware/
│   ├── bait/          # v1 firmware (Teensy 3.5 / 3.6 / MicroMod)
│   └── bait2/         # v2 firmware (Teensy MicroMod — active development)
├── electronics/
│   ├── BAIT.fzz       # v1 Fritzing schematic
│   └── BAIT2_MicroMod/ # v2 Eagle PCB & schematic
├── enclosure/
│   ├── BAIT.dwg       # AutoCAD drawing
│   └── BAIT.f3z       # Fusion 360 design
├── analysis/          # R & Python analysis notebooks
├── data/              # Sample raw and processed data
└── docs/              # Developer documentation
```

## Hardware Versions

### BAIT v1
- **Processor:** Teensy 3.5 or 3.6
- **PCB:** Custom carrier board designed in Fritzing
- **Sensors:** Si7021 (temperature/humidity), TSL2561 (light), battery voltage divider
- **Storage:** SD card via SdFat
- **Power:** Low-power sleep via Snooze library

### BAIT v2 (Active)
- **Processor:** Teensy MicroMod on SparkFun MicroMod carrier
- **PCB:** Custom Eagle carrier board ([`electronics/BAIT2_MicroMod`](electronics/BAIT2_MicroMod))
- **Sensors:** BME680 (temperature, humidity, pressure, gas), TSL2591 (light), BQ27441 LiPo fuel gauge
- **Communication:** LoRaWAN via Seeed WIO-E5 (The Things Network)
- **Storage:** SD card

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) — used for building and uploading firmware
- [CLion](https://www.jetbrains.com/clion/) (recommended IDE) or VS Code with PlatformIO extension
- A Teensy MicroMod board (for BAIT v2) or Teensy 3.5/3.6 (for BAIT v1)

See [`docs/developing.md`](docs/developing.md) for detailed setup instructions.

### Building the Firmware

1. Clone this repository (including submodules):
   ```bash
   git clone --recurse-submodules https://github.com/dkadish/BioAcousticIndexTool.git
   ```

2. Open the relevant firmware directory in your IDE:
   - BAIT v2: `firmware/bait2/`
   - BAIT v1: `firmware/bait/`

3. Build and upload using PlatformIO:
   ```bash
   # For BAIT v2
   cd firmware/bait2
   pio run -e teensymm -t upload
   ```

   See each firmware directory's README for full build instructions.

### Data Analysis

The [`analysis/`](analysis/) directory contains Jupyter notebooks and R Markdown files for verifying acoustic index calculations and visualising results. See [`analysis/README.md`](analysis/README.md) for details.

## Contributing

Contributions are welcome! Please read [`docs/developing.md`](docs/developing.md) for information on setting up your development environment.

## License

This project is licensed under the terms in the [LICENSE](LICENSE) file.
