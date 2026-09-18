# BAIT infra — TTN → InfluxDB

Pulls decoded LoRaWAN uplinks from The Things Network and stores them in InfluxDB.

```
Teensy  --LoRaWAN-->  TTN  --MQTT (TLS 8883)-->  ttn-bridge  -->  InfluxDB (bucket "bait2")
```

## One-time TTN setup

1. In the [TTN console](https://console.cloud.thethings.network/), open your
   application. Note its **Application ID** (lowercase, e.g. `bait-sensors`).
2. Make sure a **payload formatter** is set (Application → Payload formatters →
   Uplink). Without it, uplinks arrive with no `decoded_payload` and the bridge
   skips them. For CayenneLPP pick the built-in "CayenneLPP" formatter.
3. Application → **API keys** → *Add API key*, grant **Read application traffic
   (uplink)**. Copy the key (`NNSXS.…`) — shown only once.

The AppEUI / DevEUI / AppKey configured on the LoRa module (`AT+KEY=APPKEY,…`)
are unrelated to this — they authenticate the *device to the network*, not this
bridge to TTN.

## Run

```bash
cp .env.example .env
# edit .env: TTN_APP_ID, TTN_API_KEY, INFLUX_PASSWORD, INFLUX_TOKEN
# optional: TTN_DEVICE_ID=<one device id>  to ingest only that device

docker compose up -d --build
docker compose logs -f ttn-bridge
```

Set `TTN_DEVICE_ID` in `.env` to a single device id (as shown in the TTN
console). The bridge subscribes to every device but only **writes** that one;
other devices are logged and ignored. Leave it blank to ingest all of them.

Expected log on a healthy uplink:

```
Connected to TTN MQTT, subscribing to v3/bait-sensors@ttn/devices/+/up (qos=1)
[2026-09-07T…] bait-ioio-1: wrote 41 fields
```

## Schema

One InfluxDB point per uplink. The whole `uplink_message` object is flattened
onto it (Telegraf-style: nested keys joined with `_`, list items indexed):

| | |
|---|---|
| measurement | `sensor_data` |
| tags | `device`, `source=ttn` |
| fields | `topic`, and every scalar under `uplink_message`, e.g. `uplink_message_decoded_payload_temperature_9`, `uplink_message_f_cnt`, `uplink_message_f_port`, `uplink_message_frm_payload`, `uplink_message_rx_metadata_0_rssi`, `uplink_message_rx_metadata_0_snr`, `uplink_message_settings_data_rate_lora_spreading_factor`, … |
| time | TTN `received_at` |

`uplink_message_decoded_payload_*` fields only appear if a TTN payload formatter
is configured. Field names like `temperature_9` / `voltage_2` are CayenneLPP
`<type>_<channel>`; the channel map is the firmware's `SensorDefinitions.h`.

## Verify data

InfluxDB UI: <http://localhost:8086> (user/pass from `.env`).

```bash
docker compose exec influxdb \
  influx query 'from(bucket:"bait2") |> range(start:-24h) |> limit(n:10)' \
  --org bait --token "$INFLUX_TOKEN"
```

## Export: one row per uplink (wide CSV)

InfluxDB stores one series per field, so a plain query returns one row per
*field*. To get one row per *uplink* (every field as a column, like a
spreadsheet), pivot on `_field`:

```flux
from(bucket: "bait2")
  |> range(start: -7d)
  |> filter(fn: (r) => r._measurement == "sensor_data" and r.device == "bait-ioio-1")
  |> pivot(rowKey: ["_time"], columnKey: ["_field"], valueColumn: "_value")
```

- **UI:** Data Explorer → *Script Editor* → paste → *Submit* → *CSV* (download button).
- **CLI:**
  ```bash
  docker compose exec -T influxdb influx query --org bait --token bait-local-testing-token-2026 --raw '
  from(bucket:"bait2") |> range(start:-7d)
    |> filter(fn:(r)=>r._measurement=="sensor_data" and r.device=="bait-ioio-1")
    |> pivot(rowKey:["_time"], columnKey:["_field"], valueColumn:"_value")' > uplinks.csv
  ```

## Notes

- Port `8086` is published. If another InfluxDB is already running there
  (e.g. a serial-testing stack), stop it first or change the published port.
- The bridge auto-reconnects to TTN. `restart: unless-stopped` covers crashes.
- `.env` holds secrets and is git-ignored — keep it that way.
