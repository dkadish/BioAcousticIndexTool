# Infrastructure

Off-device services that support the BAIT firmware in `../bait2/` — currently
just the telemetry monitoring stack that pulls uplinks off The Things Network
and stores them for inspection.

```
Teensy (bait2 firmware) --LoRaWAN--> TTN --MQTT(TLS 8883)--> ttn-bridge --> InfluxDB
```

## Components

- **[`ttn-influxdb-bridge/`](ttn-influxdb-bridge/README.md)** — Docker Compose
  stack: an InfluxDB 2.7 instance plus a small Python bridge (`bridge.py`)
  that subscribes to the TTN MQTT integration, flattens each decoded uplink
  into one InfluxDB point (Telegraf-style, one row per uplink), and writes it
  to the `bait2` bucket. Used to eyeball/graph what the device is actually
  sending over LoRaWAN — CayenneLPP fields like `temperature_9`, `voltage_2`,
  `percentage_6` (`<type>_<channel>`, channel map in the firmware's
  `SensorDefinitions.h`) plus radio metadata (RSSI/SNR, spreading factor,
  frame counter).

  Config (TTN app ID/API key, InfluxDB credentials) lives in a git-ignored
  `.env` — copy `.env.example` and fill it in. See that directory's README
  for setup, running, and how to export a wide CSV of uplinks.

