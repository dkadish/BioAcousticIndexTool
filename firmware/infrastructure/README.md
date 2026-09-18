# Infrastructure

Off-device services that support the BAIT firmware in `../bait2/` — currently
just the telemetry monitoring stack that pulls uplinks off The Things Network
and stores them for inspection.

```
Teensy (bait2 firmware) --LoRaWAN--> TTN --MQTT(TLS 8883)--> ttn-bridge --> InfluxDB
```


