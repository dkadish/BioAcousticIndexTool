# Developing

This guide covers everything you need to know to develop firmware for the BioAcoustic Index Tool (BAIT), work with the analysis scripts, and contribute to the project.

## Table of Contents

- [Development Environment Setup](#development-environment-setup)
- [Project Structure](#project-structure)
- [Firmware Development](#firmware-development)
- [Running Tests](#running-tests)
- [Data Analysis](#data-analysis)
- [Hardware Development](#hardware-development)
- [Contributing](#contributing)

---

## Development Environment Setup

### Prerequisites

Before you begin, make sure you have the following installed:

- **Git** — for version control
- **PlatformIO Core** — the build system for embedded firmware
- **Python 3** — required by PlatformIO and for analysis notebooks
- An IDE — either **CLion** (recommended) or **VS Code** with PlatformIO extension

### Installing PlatformIO

PlatformIO can be installed as a standalone command-line tool:

#### macOS
```bash
brew install platformio
```

#### Linux
```bash
pip install platformio
```

#### Windows
```bash
pip install platformio
```

For detailed installation instructions, see the [PlatformIO Installation Guide](https://docs.platformio.org/en/latest/core/installation/index.html).

### IDE Setup

#### Option 1: CLion (Recommended)

[CLion](https://www.jetbrains.com/clion/) provides excellent C++ support with advanced code completion, refactoring, and debugging.

1. Download and install CLion from [JetBrains](https://www.jetbrains.com/clion/)
2. Install the PlatformIO plugin:
   - Open CLion
   - Go to **Settings/Preferences → Plugins**
   - Search for "PlatformIO" and install it
3. Restart CLion
4. Open a firmware directory (e.g., `firmware/bait2/`) as a project

For complete setup instructions, see the [PlatformIO + CLion Integration Guide](https://docs.platformio.org/en/latest/integration/ide/clion.html).

#### Option 2: VS Code

[Visual Studio Code](https://code.visualstudio.com/) is a free, lightweight alternative.

1. Download and install VS Code
2. Install the **PlatformIO IDE** extension from the Extensions marketplace
3. Restart VS Code
4. Open a firmware directory using **PlatformIO: Open Project**

### Verified Tool Versions

The following versions have been tested and confirmed to work:

| Tool | Version |
|------|---------|
| **PlatformIO** | 6.1.7 |
| **CLion** | 2023.1.3 |
| **Python** | 3.8+ |

Newer versions should also work, but if you encounter issues, consider using these verified versions.

---

## Project Structure

The BioAcousticIndexTool repository is organized as follows:

```
BioAcousticIndexTool/
├── firmware/
│   ├── bait/          # BAIT v1 firmware (Teensy 3.5/3.6/MicroMod)
│   └── bait2/         # BAIT v2 firmware (Teensy MicroMod — active development)
├── electronics/
│   ├── BAIT.fzz       # v1 Fritzing schematic
│   ├── BAIT_bb.png    # v1 breadboard wiring diagram
│   └── BAIT2_MicroMod/ # v2 Eagle PCB & schematic
├── enclosure/
│   ├── BAIT.dwg       # AutoCAD enclosure drawing
│   └── BAIT.f3z       # Fusion 360 enclosure design
├── analysis/          # Jupyter notebooks and R scripts for data analysis
├── data/              # Sample raw and processed data
│   ├── raw/           # Raw acoustic and sensor data
│   └── processed/     # Processed data files
└── docs/              # Developer documentation
    ├── developing.md  # This file
    └── v2_ideas.md    # Hardware design notes for v2
```

### Firmware Structure

#### BAIT v1 (`firmware/bait/`)

The original firmware for Teensy 3.5, 3.6, and MicroMod processors.

```
firmware/bait/
├── src/                              # Source files
│   ├── main.cpp                      # Main program entry point
│   ├── AcousticComplexityIndex.*     # ACI implementation
│   ├── AcousticDiversityIndex.*      # ADI implementation
│   ├── AcousticDifferenceIndex.*     # ADE implementation
│   ├── FFTReader.*                   # FFT capture from Teensy Audio Library
│   ├── AtmosphericSensors.*          # Si7021 temperature/humidity driver
│   ├── BatteryMeasurement.*          # Battery voltage monitoring
│   ├── sdcard.h                      # SD card helpers
│   ├── definitions.h                 # Pin definitions and constants
│   └── parameters.h                  # Measurement parameters
├── platformio.ini                    # PlatformIO build configuration
└── README.md
```

**Build Environments:**
- `teensy35` — Teensy 3.5 board
- `teensy36` — Teensy 3.6 board (120 MHz)
- `teensymm` — Teensy MicroMod

#### BAIT v2 (`firmware/bait2/`)

The actively developed v2 firmware with modular library structure.

```
firmware/bait2/
├── src/                              # Main application code
│   └── main.cpp                      # Entry point
├── lib/                              # Internal libraries
│   ├── Ecoacoustics/                 # Acoustic index computation (ACI, ADI, ADE)
│   ├── Environment/                  # Environmental sensors (BME680, TSL2591, BQ27441)
│   ├── Communication/                # LoRaWAN via Seeed WIO-E5
│   ├── Logging/                      # SD card logging
│   ├── Display/                      # Display driver
│   └── Sensor/                       # Base sensor abstraction
├── test/                             # Unit tests
├── platformio.ini                    # PlatformIO build configuration
└── README.md
```

**Build Environments:**
- `teensymm` — Development build with USB serial, waits for serial monitor on startup
- `teensymm-release` — Release build with USB audio only, no serial wait
- `desktop` — Native (x86/ARM) build for unit testing without hardware

### Library Overview (BAIT v2)

The BAIT v2 firmware is organized into modular libraries:

| Library | Purpose | Key Components |
|---------|---------|----------------|
| **Ecoacoustics** | Acoustic index computation | ACI, ADI, ADE algorithms; FFT processing |
| **Environment** | Environmental monitoring | BME680 (temp, humidity, pressure, gas), TSL2591 (light), BQ27441 (battery) |
| **Communication** | Wireless data transmission | LoRaWAN via Seeed WIO-E5 module, CayenneLPP encoding |
| **Logging** | Data persistence | SD card file management, CSV logging |
| **Display** | User interface | Optional display output |
| **Sensor** | Abstraction layer | Base classes for sensor drivers |

---

## Firmware Development

### Cloning the Repository

Make sure to clone with submodules, as some libraries are linked as Git submodules:

```bash
git clone --recurse-submodules https://github.com/dkadish/BioAcousticIndexTool.git
cd BioAcousticIndexTool
```

If you already cloned without submodules, initialize them:

```bash
git submodule update --init --recursive
```

### Building the Firmware

#### BAIT v2 (Active Development)

Navigate to the BAIT v2 firmware directory:

```bash
cd firmware/bait2
```

**Development Build (with serial debugging):**
```bash
pio run -e teensymm
```

**Upload to connected Teensy MicroMod:**
```bash
pio run -e teensymm -t upload
```

**Release Build (for deployment):**
```bash
pio run -e teensymm-release -t upload
```

**Native Desktop Build (for testing without hardware):**
```bash
pio run -e desktop
```

#### BAIT v1 (Legacy)

Navigate to the BAIT v1 firmware directory:

```bash
cd firmware/bait
```

**Build for Teensy 3.5:**
```bash
pio run -e teensy35
```

**Upload to Teensy 3.5:**
```bash
pio run -e teensy35 -t upload
```

**Build for Teensy 3.6:**
```bash
pio run -e teensy36 -t upload
```

**Build for Teensy MicroMod:**
```bash
pio run -e teensymm -t upload
```

### Monitoring Serial Output

To view serial debug output from a connected device:

```bash
pio device monitor
```

Or specify a baud rate:

```bash
pio device monitor -b 115200
```

### Cleaning Build Files

If you encounter build issues, clean the build cache:

```bash
pio run -t clean
```

---

## Running Tests

BAIT v2 includes unit tests that can run both on the desktop (without hardware) and on embedded devices.

### Desktop Tests (No Hardware Required)

Desktop tests run natively on your development machine using the `desktop` environment:

```bash
cd firmware/bait2
pio test -e desktop
```

This is the fastest way to verify core functionality without needing a physical Teensy board.

### Embedded Tests (Requires Hardware)

To run tests on a connected Teensy MicroMod:

```bash
cd firmware/bait2
pio test -e teensymm
```

Embedded tests validate hardware-specific functionality like sensor communication and audio processing.

### Writing Tests

Tests are located in `firmware/bait2/test/`. To add a new test, create a file in the `test/` directory following the [PlatformIO testing conventions](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html).

Example test structure:

```cpp
#include <unity.h>

void test_example_function(void) {
    TEST_ASSERT_EQUAL(42, example_function());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_example_function);
    UNITY_END();
}

void loop() {}
```

---

## Data Analysis

The `analysis/` directory contains Jupyter notebooks (Python) and R Markdown files for verifying acoustic index calculations and visualizing field data.

### Python / Jupyter Setup

1. Install Python dependencies:
   ```bash
   pip install jupyter numpy scipy matplotlib pandas
   ```

2. Launch Jupyter:
   ```bash
   cd analysis
   jupyter notebook
   ```

3. Open a notebook (e.g., `prototype_data_analysis.ipynb`)

### R / RStudio Setup

1. Install R from [r-project.org](https://www.r-project.org/)

2. Install required R packages:
   ```r
   install.packages(c("soundecology", "tuneR", "seewave", "ggplot2", "rmarkdown"))
   ```

3. Open an R Markdown file (e.g., `aci_verification/compare_bait_soundecology_aci.Rmd`) in RStudio and click **Knit**

   Or render from the command line:
   ```bash
   Rscript -e "rmarkdown::render('aci_verification/compare_bait_soundecology_aci.Rmd')"
   ```

### Analysis Files

| File / Directory | Description |
|------------------|-------------|
| `prototype_data_analysis.ipynb` | Jupyter notebook — exploratory analysis of field data |
| `aci_verification/FFT_Comparison.ipynb` | Python — compares BAIT FFT output to reference implementations |
| `aci_verification/compare_bait_soundecology_aci.Rmd` | R Markdown — compares BAIT ACI against `soundecology` package |
| `aci_verification/compare_bait_soundecology_chunks.Rmd` | R Markdown — chunk-level ACI comparison |

Sample data files for testing are located in the `data/` directory.

---

## Hardware Development

### Electronics

Hardware design files are in the `electronics/` directory:

- **BAIT v1:** Fritzing schematic (`BAIT.fzz`) and breadboard diagram (`BAIT_bb.png`)
- **BAIT v2:** Eagle schematic and PCB layout in `BAIT2_MicroMod/`

**Required Tools:**
- [Fritzing](https://fritzing.org/) — for v1 designs
- [Autodesk Eagle](https://www.autodesk.com/products/eagle/overview) — for v2 designs

See [`../electronics/README.md`](../electronics/README.md) for details.

### Enclosure

Mechanical design files are in the `enclosure/` directory:

- **AutoCAD:** `BAIT.dwg`
- **Fusion 360:** `BAIT.f3z`

See [`../enclosure/README.md`](../enclosure/README.md) for details.

### Hardware Design Notes

For ideas and notes on v2 hardware improvements, see [`v2_ideas.md`](v2_ideas.md).

---

## Contributing

Contributions to BAIT are welcome! Whether you're fixing bugs, adding features, improving documentation, or sharing field deployment experiences, your input is valuable.

### General Guidelines

- **Code Style:** Follow the existing code style in the firmware (consistent indentation, meaningful variable names)
- **Testing:** Add tests for new features when possible
- **Documentation:** Update README files and comments when changing functionality
- **Commit Messages:** Write clear, descriptive commit messages

### Submitting Changes

1. Fork the repository on GitHub
2. Create a feature branch (`git checkout -b feature/my-new-feature`)
3. Make your changes and commit them (`git commit -am 'Add new feature'`)
4. Push to your branch (`git push origin feature/my-new-feature`)
5. Open a Pull Request on GitHub

### Reporting Issues

If you encounter bugs or have feature requests, please [open an issue](https://github.com/dkadish/BioAcousticIndexTool/issues) on GitHub with:

- A clear description of the problem or request
- Steps to reproduce (for bugs)
- Expected vs. actual behavior
- Hardware and software versions

### Questions?

For questions about the project, feel free to open a GitHub Discussion or reach out to the maintainers.

---

## Additional Resources

- **Firmware READMEs:**
  - [BAIT v1 Firmware](../firmware/bait/README.md)
  - [BAIT v2 Firmware](../firmware/bait2/README.md)
- **Hardware:**
  - [Electronics Documentation](../electronics/README.md)
  - [Enclosure Documentation](../enclosure/README.md)
- **Data Analysis:**
  - [Analysis Scripts](../analysis/README.md)
- **PlatformIO Documentation:**
  - [PlatformIO Core](https://docs.platformio.org/en/latest/core/index.html)
  - [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)

---

**Happy developing! 🎤🔧**