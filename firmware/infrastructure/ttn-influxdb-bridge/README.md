# BAIT infra — TTN → InfluxDB

## One-time TTN setup

1. In the [TTN console](https://console.cloud.thethings.network/), open your
   application. Note its **Application ID** (lowercase, e.g. `bait-sensors`).
2. Make sure a **payload formatter** is set (Application → Payload formatters →
   Uplink). Without it, uplinks arrive with no `decoded_payload` and the bridge
   skips them. For CayenneLPP pick the built-in "CayenneLPP" formatter.
3. Application → **API keys** → *Add API key*, grant **Read application traffic
   (uplink)**. Copy the key paste both things in the .env file


## Run

# edit .env: TTN_APP_ID, TTN_API_KEY, INFLUX_PASSWORD, INFLUX_TOKEN
# optional: TTN_DEVICE_ID=<one device id>  to ingest only that device

docker compose up -d --build
docker compose logs -f ttn-bridge

## Schema

One InfluxDB point per uplink. The whole `uplink_message` object is flattened
onto it (Telegraf-style: nested keys joined with `_`, list items indexed):

| | |
|---|---|
| measurement | `sensor_data` |
| tags | `device`, `source=ttn` |
| fields | `topic`, and every scalar under `uplink_message`, e.g. `uplink_message_decoded_payload_temperature_9`, `uplink_message_f_cnt`, `uplink_message_f_port`, `uplink_message_frm_payload`, `uplink_message_rx_metadata_0_rssi`, `uplink_message_rx_metadata_0_snr`, `uplink_message_settings_data_rate_lora_spreading_factor`, … |
| time | TTN `received_at` |

InfluxDB UI: <http://localhost:8086> (user/pass from `.env`).


