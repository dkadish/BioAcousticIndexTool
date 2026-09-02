# Enclosure

Mechanical design files for the BioAcoustic Index Tool (BAIT) weatherproof enclosure.

## Contents

| File | Format | Description |
|------|--------|-------------|
| `BAIT.dwg` | AutoCAD DWG | 2D drawing of the enclosure |
| `BAIT.f3z` | Fusion 360 Archive | 3D parametric enclosure design |

## Design Goals

- **Weatherproof** — protects electronics from rain and moisture in outdoor deployments
- **Acoustic transparency** — allows sound to reach the microphone with minimal attenuation
- **Field serviceable** — accessible battery and SD card without disassembling the enclosure

## Opening the Files

### Fusion 360 (`.f3z`)

1. Open [Autodesk Fusion 360](https://www.autodesk.com/products/fusion-360/overview)
2. Go to **File → Open**
3. Select `BAIT.f3z`

### AutoCAD (`.dwg`)

Open `BAIT.dwg` with [AutoCAD](https://www.autodesk.com/products/autocad/overview) or any DWG-compatible viewer (e.g., [DWG TrueView](https://www.autodesk.com/viewers)).

## Acoustic Venting

To allow sound into the enclosure while maintaining weatherproofing, acoustic vents are recommended. Options used in similar devices:

- **Porelle Acoustic Vents** — IP68-rated (used in AudioMoth)
- **Gore Acoustic Vents** — e.g., GAW334

See [`../docs/v2_ideas.md`](../docs/v2_ideas.md) for additional notes on waterproofing materials.
