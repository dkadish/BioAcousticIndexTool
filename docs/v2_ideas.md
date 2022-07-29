# Ideas for Version 2.0

## Sensors

### Wind Speed
* https://posifatech.com/air-velocity-sensors/pav3000/
* https://posifatech.com/wp-content/uploads/2020/11/Datasheet_PAV3000_AirVelocity_RevC_C6.pdf

## Processor

### Micromod Teensy
* https://www.sparkfun.com/micromod#processor_boards
* Mounting HW: https://www.sparkfun.com/products/16549
* Hookup Guide: https://learn.sparkfun.com/tutorials/micromod-teensy-processor-hookup-guide/hardware-overview
* Enviro board: https://www.sparkfun.com/products/18632

### Teensy Power
* https://forum.pjrc.com/threads/54711-Teensy-4-0-First-Beta-Test?p=194245&viewfull=1#post194245

## Charger

### BQ24074
* Adafruit: https://learadwawefsefsefsn.adafruit.com/adafruit-bq24074-universal-usb-dc-solar-charger-breakout/pinouts
* https://www.ti.com/product/BQ24074
* Datasheet: https://www.ti.com/lit/ds/symlink/bq24074.pdf?ts=1647598429451&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FBQ24074
* BQ24075: https://www.ti.com/product/BQ24075

### Power MUX
* https://www.ti.com/power-management/power-switches/power-muxes/products.html#p1028=2&p1498=Catalog&sort=p2849;asc
* TPS2113A: https://www.ti.com/product/TPS2113A
* Datasheet: https://www.ti.com/lit/ds/symlink/tps2113a.pdf?ts=1632059993216&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FTPS2113A
* TPS2116: https://www.ti.com/product/TPS2116

### Wireless Modules

#### LoRaWAN

* LoRa-E5 (https://www.seeedstudio.com/LoRa-E5-Wireless-Module-p-4745.html) also has the latest Semtech chips and comes with 2 free boards from Seeed, so start with this!
* HopeRF RFM6501W (https://www.hoperf.com/modules/LoRaWAN/RFM6501W.html)
* Onethinx LoRaWan and BT5 (https://www.onethinx.com/module.html)
* Embit LRWL55 (http://www.embit.eu/products/wireless-modules/emb-lrwl55/)
* Heltec CubeCell (https://heltec.org/project/htcc-am01/)
* Move-X Mamwle (https://www.move-x.it/mamwle-module/)

#### NB-IoT
* nrf9160 SOM: https://docs.actinius.com/icarus-som/introduction#icarus-som-nrf9160

# Waterproofing
* AudioMoth uses Porelle Acoustic Vents (https://selectronix.co.uk/products/black-acoustic-vent-11x5mm-ip68-3m-30min)
* Gore also makes them https://www.gore.com/news-events/news/venting-portable-news-new-acoustic-vent-gaw334-us
* Porex sample request: https://www.porex.com/about-porex/contact-us/sample-request/

# Modifications to MicroMod DataLogger
* Is the RTC Backup Battery big enough?
* I2S connector
* Solar power
  * Upgrade the charging circuit
  * Solar charging circuit w/ PPM
  * Add battery babysitter
  * Add current measurement
  * Move battery connector
  * Add low-power co-processor?
  * Do I need a better LDO Regulator?
* Piezo connector (for self-testing)
* Wireless Connectivity
  * Add connection board/LoRaWAN

## Schematic To Do

  - [x] Wireless Block
  - [x] Piezo Speaker
  - [x] Connect I2S Wires
  - [x] Connect SDIO to SD card
  - [ ] Hook up all connections
  - [ ] Provide solar panel connection
 
 
# Design Notes
## LDO Regulator
Current design uses AP7361C (Dropout Voltage 360 mV (3.66V @ 3.3V regulation).
Preferred parts include TI TPS7A8033 (170 mV Dropout) or R1172H331B (180 mV dropout @ 1.0A), but they are rejected due to production/supply issues.
LD39150 (ST, 200 mV dropout), is available, but does not have an enable.

S-1172B33-E6T1U (ABLIC, 0.1V DO) seems to be in stock and works well. 
Another option is TPS7A2133PYWDJ (TI)

By default, the EN isn't used in the MM Datalogger Carrier. It can be if you close a jumper on the board, and my understanding is that then you can power the board with USB and control the 3V3 bus. But we don't need that.

###Requirements
* Vin: ~3.3-4.4 (From regulated output from bq24074, solar charger)
* Iout: > 500 mA
* 
