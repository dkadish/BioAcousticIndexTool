# Communication Library

LoRaWAN uplink to The Things Network (TTN) via a Seeed WIO-E5 module using CayenneLPP
payload encoding.

**Source**: `lib/Communication/`

---

## Classes

### `LoRaWANTTN`

**Header**: `include/lora.h`  
**Source**: `src/lora.cpp`

Manages the serial AT-command interface to the WIO-E5 LoRaWAN module and exposes a
`CayenneLPP` buffer that sensors populate before each uplink.

#### Constructor

```cpp
LoRaWANTTN();
```

Initialises an internal `CayenneLPP` buffer (51-byte payload limit) and resets it.

#### Public Methods

```cpp
void setup();
```

Opens `Serial1` at 9 600 baud and sends `AT+JOIN` to initiate OTAA network join.
Waits up to 10 × 100 ms for `Serial1` to become available.

---

```cpp
void loop();
```

Must be called from the Arduino `loop()` function. Checks whether new data has been
queued (the "dirty" flag) and, after a 2-second debounce, transmits the payload.

---

```cpp
CayenneLPP &getLPP();
```

Returns a mutable reference to the internal `CayenneLPP` buffer. Sensors call
`addXxx()` methods on this object to append their readings before marking the
instance dirty.

**Example**:
```cpp
lora.getLPP().addTemperature(TEMPERATURE, 22.5f);
lora.getLPP().addRelativeHumidity(HUMIDITY, 60.0f);
lora.setDirty();
```

---

```cpp
void setDirty();
```

Signals that new data has been added to the LPP buffer. Resets and resumes the
2-second debounce timer. After the delay expires, the next `loop()` call transmits
the payload via `AT+CMSGHEX="..."` on `Serial1`.

The buffer is automatically reset and the dirty flag cleared after each successful
transmission.

---

#### Private Method

```cpp
bool send();
```

Formats the binary CayenneLPP buffer as a hexadecimal string and writes
`AT+CMSGHEX="<hex>"` to `Serial1`. Resets the LPP buffer and clears the dirty flag.
Returns `true` on success.

> **Note**: The current implementation does not wait for an ACK from the module.
> Future work should use `AT+CMSGHEX` with response parsing to confirm delivery.

---

## CayenneLPP Channel IDs

Channel constants are defined in `include/SensorDefinitions.h`. Each sensor writes to
a fixed channel so that TTN decoders can reliably parse the payload.

| Constant | Channel | Type | Unit |
|----------|---------|------|------|
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

## Dependencies

| Library | Version | Notes |
|---------|---------|-------|
| `electroniccats/CayenneLPP` | `^1.4.0` | Low-power payload encoder for LoRaWAN |
| `Chrono` | (bundled) | Non-blocking timer used for the send-debounce delay |

---

## Hardware Wiring

The WIO-E5 module is connected on the Teensy's `Serial1` UART:

| Signal | Teensy pin | WIO-E5 pin |
|--------|-----------|-----------|
| TX | TX1 | RX |
| RX | RX1 | TX |
| GND | GND | GND |
| 3.3 V | 3.3 V | VCC |

The module must have valid OTAA credentials (AppEUI / DevEUI / AppKey) stored in its
non-volatile memory before deployment. Consult the Seeed WIO-E5 AT Command reference for
provisioning instructions.
