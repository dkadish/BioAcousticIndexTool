# Electronics

Hardware design files for the BioAcoustic Index Tool (BAIT) custom carrier boards.

## Contents

| File / Directory | Description |
|-----------------|-------------|
| `BAIT.fzz` | BAIT v1 schematic and PCB layout (Fritzing) |
| `BAIT_bb.png` | BAIT v1 breadboard wiring diagram |
| `BAIT2_MicroMod/` | BAIT v2 custom carrier board for Teensy MicroMod (Eagle) |

## BAIT v1

The v1 board is a through-hole prototype carrier designed in [Fritzing](https://fritzing.org/).

**File:** `BAIT.fzz`

Open with the Fritzing application to view the schematic and breadboard layout.

## BAIT v2 — MicroMod Carrier Board (`BAIT2_MicroMod/`)

The v2 carrier board is designed in [Eagle](https://www.autodesk.com/products/eagle/overview) and mounts a **Teensy MicroMod** processor module.

| File | Description |
|------|-------------|
| `BAIT MicroMod Carrier Board.sch` | Eagle schematic |
| `BAIT MicroMod Carrier Board.brd` | Eagle PCB layout |
| `eagle.epf` | Eagle project file |
| `gerbers/` | Gerber files for PCB fabrication |
| `Assembly/` | Assembly documentation and BOM |

### Key Features

- **Processor socket:** SparkFun MicroMod M.2 connector (accepts Teensy MicroMod)
- **Sensors:** BME680 (temperature, humidity, pressure, gas), TSL2591 (light), BQ27441 (LiPo fuel gauge)
- **Communication:** Seeed WIO-E5 LoRaWAN module connector
- **Storage:** MicroSD card slot
- **Power:** LiPo battery connector with charging circuit

### Ordering PCBs

Use the Gerber files in `BAIT2_MicroMod/gerbers/` to order boards from a PCB fabrication service such as OSH Park, JLCPCB, or PCBWay.

## Tools Required

- [Fritzing](https://fritzing.org/) — for opening BAIT v1 design files
- [Autodesk Eagle](https://www.autodesk.com/products/eagle/overview) — for opening BAIT v2 design files
